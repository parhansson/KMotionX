// CoordMotion.cpp: implementation of the CCoordMotion class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CoordMotion.h"
#include "HiResTimer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


#ifdef DEBUG_DOWNLOAD
char ds[MAX_LINE];
FILE *f=NULL;
CHiResTimer DTimer;


void PutString(const char *s)
{
	if (!f)
	{
		f=fopen("c:\\data.txt","wt");
		DTimer.Start();
	}
	if (f) fputs(ds,f);
}

void CloseDiag()
{
	if (f) fclose(f);
	f=NULL;
}


#endif






CCoordMotion::CCoordMotion(CKMotionDLL *KM)
{
	m_board_type = BOARD_TYPE_UNKNOWN;

	KMotionDLL=KM;

	current_x=current_y=current_z=current_a=current_b=current_c=current_u=current_v=0;

	m_FeedRateOverride=1.0;
	m_FeedRateRapidOverride=1.0;
	m_HardwareFRORange=0.0;
	m_SpindleRateOverride=1.0;
	m_DisableSoftLimits=m_Simulate=false;
	m_DefineCS_valid=false;
	m_TCP_affects_actuators = true;  // assume Tool Center Point has effects except for simple cases

	m_TapCycleInProgress = false;


	m_NumLinearNotDrawn=0;

	// Save for everybody what directory we are installed in
#ifdef _KMOTIONX

	sprintf(MainPath,"%s/KMotion",kmx::getInstallPath());
	sprintf(MainPathRoot,"%s",kmx::getInstallPath());

#else
	CString Path;

	GetModuleFileName(GetModuleHandle("GCodeInterpreter.dll"),Path.GetBuffer(MAX_PATH),MAX_PATH);
	Path.ReleaseBuffer();

	Path.Replace("\"","");  // remove quotes
	Path.TrimRight();
	Path.TrimLeft();

	int LastSlash=Path.ReverseFind('\\');
	Path=Path.Left(LastSlash);

	// Check if we are running from the debug directory
	// if we are, then strip it off

	if (Path.Right(6).CompareNoCase("\\debug") == 0)
	{
		Path = Path.Left(Path.GetLength()-6);
	}

	// Check if we are running from the release directory
	// if we are, then strip it off

	if (Path.Right(8).CompareNoCase("\\release") == 0)
	{
		Path = Path.Left(Path.GetLength()-8);
	}

	// Now set the root install directory

	if (Path.Right(8).CompareNoCase("\\KMotion") == 0)
	{
		strncpy(MainPathRoot,Path.Left(Path.GetLength()-8),MAX_PATH);
	}

	strncpy(MainPath,Path,MAX_PATH);
#endif
	m_StraightTraverseCallback=NULL;
	m_StraightTraverseSixAxisCallback=NULL;
	m_StraightFeedCallback=NULL;
	m_StraightFeedSixAxisCallback=NULL;
	m_ArcFeedCallback=NULL;
	m_ArcFeedSixAxisCallback=NULL;

	DownloadInit();

	tp_init();
	//TODO The following line is removed in 4.34. Check if initialised elsewhere 
	current_x = current_y = current_z = current_a = current_b = current_c = 0.0;

	m_SegmentsStartedExecuting = m_Abort = m_Halt = false;

	m_PreviouslyStopped = m_Stopping = STOPPED_NONE;
	m_PreviouslyStoppedType = SEG_UNDEFINED;
	m_PreviouslyStoppedID = -1;
	m_TCP_affects_actuators = true;  // assume Tool Center Point has effects except for simple cases
	// check for a special Kinematics File
	char kinFile[MAX_PATH];
	sprintf(kinFile, "%s%cData%cKinematics.txt",MainPath,PATH_SEPARATOR,PATH_SEPARATOR);

	FILE *f = fopen(kinFile,"rt");

	if (f)
	{
		char s[81];
		fgets(s, 80, f);
		// one exists, check if it is calling for Geppetto otherwise assume it is the 3Rod

		if (strstr(s, "5AxisTableAC") != NULL)
			Kinematics = new CKinematics5AxisTableAC;
		else if (strstr(s, "5AxisTableBC") != NULL)
			Kinematics = new CKinematics5AxisTableBC;
		else if (strstr(s, "5AxisGimbalAB") != NULL)
			Kinematics = new CKinematics5AxisGimbalAB;
		else if (strstr(s, "5AxisGimbalCB") != NULL)
			Kinematics = new CKinematics5AxisGimbalCB;
		else if (strstr(s, "GeppettoExtruder") != NULL)
			Kinematics = new CKinematicsGeppettoExtrude;
		else if (strstr(s, "Geppetto") != NULL)
			Kinematics = new CKinematicsGeppetto;
		else
			Kinematics = new CKinematics3Rod;
		
		fclose(f);
	}
	else
	{
		m_TCP_affects_actuators = false;
		Kinematics = new CKinematics;
	}

	RapidParamsDirty=true;  // Trsjectory Params should be refreshed from KFLOP

	x_axis=y_axis=z_axis=a_axis=b_axis=c_axis=-1;  // set all as initially undefined

	#ifdef DEBUG_DOWNLOAD
	AfxMessageBox("Download Diag Enabled");
	#endif
}

CCoordMotion::~CCoordMotion()
{
	if (Kinematics) delete Kinematics;
}

MOTION_PARAMS *CCoordMotion::GetMotionParams()
{
	return &Kinematics->m_MotionParams;
}

// check if arc goes outside of Limits.  Calculate as if arc is in xy plane but could be other planes
// Progress along theta from the start to each PI/2 peak position until the end.  If any of those
// are outside then flag as an error.

int CCoordMotion::CheckSoftLimitsArc(double XC, double YC, double z,
					   double SoftLimitPosX,double SoftLimitNegX,
					   double SoftLimitPosY,double SoftLimitNegY,
					   double SoftLimitPosZ,double SoftLimitNegZ,
					   double a, double b, double c, double u, double v, BOOL DirIsCCW, 
					   double radius, double theta0, double dtheta, 
					   int x_axis,int y_axis,int z_axis,
					   char XSTR, char YSTR, char ZSTR, char *errmsg)
{
	if (m_DisableSoftLimits) return 0;

	MOTION_PARAMS *MP=&Kinematics->m_MotionParams;

	double end_theta, next_theta, quadrant, SIGMA = radius*1e-12;

	end_theta=theta0+dtheta;

	if (DirIsCCW)
		quadrant = ceil(theta0/(PI*0.5)) + 1.0;
	else
		quadrant = floor(theta0/(PI*0.5)) - 1.0;

	do
	{
		// first advance theta (no need to check the starting point
		// as it should have already been checked) to next peak or
		// to the end.

		next_theta = quadrant * PI * 0.5;

		if (DirIsCCW)
		{	
			if (next_theta > end_theta) next_theta = end_theta;
		}
		else
		{	
			if (next_theta < end_theta) next_theta = end_theta;
		}

		double x = radius * cos(next_theta) + XC;
		double y = radius * sin(next_theta) + YC;

		if (x_axis>=0)
		{
			if (x > SoftLimitPosX+SIGMA) {errmsg[0]=XSTR;errmsg[1]='+';errmsg[2]='\0'; return 1;}
			if (x < SoftLimitNegX-SIGMA) {errmsg[0]=XSTR;errmsg[1]='-';errmsg[2]='\0'; return 1;}
		}
		if (y_axis>=0)
		{
			if (y > SoftLimitPosY+SIGMA) {errmsg[0]=YSTR;errmsg[1]='+';errmsg[2]='\0'; return 1;}
			if (y < SoftLimitNegY-SIGMA) {errmsg[0]=YSTR;errmsg[1]='-';errmsg[2]='\0'; return 1;}
		}

		if (DirIsCCW)
			quadrant++;
		else
			quadrant--;
	}
	while (next_theta != end_theta);
		
	if (z_axis>=0)
	{
		if (z > SoftLimitPosZ) {errmsg[0]=ZSTR;errmsg[1]='+';errmsg[2]='\0'; return 1;}
		if (z < SoftLimitNegZ) {errmsg[0]=ZSTR;errmsg[1]='-';errmsg[2]='\0'; return 1;}
	}
	if (a_axis>=0)
	{
		if (a > MP->SoftLimitPosA) {strcpy(errmsg,"A+"); return 1;}
		if (a < MP->SoftLimitNegA) {strcpy(errmsg,"A-"); return 1;}
	}
	if (b_axis>=0)
	{
		if (b > MP->SoftLimitPosB) {strcpy(errmsg,"B+"); return 1;}
		if (b < MP->SoftLimitNegB) {strcpy(errmsg,"B-"); return 1;}
	}
	if (c_axis>=0)
	{
		if (c > MP->SoftLimitPosC) {strcpy(errmsg,"C+"); return 1;}
		if (c < MP->SoftLimitNegC) {strcpy(errmsg,"C-"); return 1;}
	}
	if (u_axis>=0)
	{
		if (u > MP->SoftLimitPosU) {strcpy(errmsg,"U+"); return 1;}
		if (u < MP->SoftLimitNegU) {strcpy(errmsg,"U-"); return 1;}
	}
	if (v_axis>=0)
	{
		if (v > MP->SoftLimitPosV) {strcpy(errmsg,"V+"); return 1;}
		if (v < MP->SoftLimitNegV) {strcpy(errmsg,"V-"); return 1;}
	}
	return 0;
}


int CCoordMotion::CheckSoftLimits(double x, double y, double z, double a, double b, double c, double u, double v, char *errmsg)
{
	if (m_DisableSoftLimits) return 0;

	MOTION_PARAMS *MP=&Kinematics->m_MotionParams;

	if (x_axis>=0)
	{
		if (x > MP->SoftLimitPosX) {strcpy(errmsg,"X+"); return 1;}
		if (x < MP->SoftLimitNegX) {strcpy(errmsg,"X-"); return 1;}
	}
	if (y_axis>=0)
	{
		if (y > MP->SoftLimitPosY) {strcpy(errmsg,"Y+"); return 1;}
		if (y < MP->SoftLimitNegY) {strcpy(errmsg,"Y-"); return 1;}
	}
	if (z_axis>=0)
	{
		if (z > MP->SoftLimitPosZ) {strcpy(errmsg,"Z+"); return 1;}
		if (z < MP->SoftLimitNegZ) {strcpy(errmsg,"Z-"); return 1;}
	}
	if (a_axis>=0)
	{
		if (a > MP->SoftLimitPosA) {strcpy(errmsg,"A+"); return 1;}
		if (a < MP->SoftLimitNegA) {strcpy(errmsg,"A-"); return 1;}
	}
	if (b_axis>=0)
	{
		if (b > MP->SoftLimitPosB) {strcpy(errmsg,"B+"); return 1;}
		if (b < MP->SoftLimitNegB) {strcpy(errmsg,"B-"); return 1;}
	}
	if (c_axis>=0)
	{
		if (c > MP->SoftLimitPosC) {strcpy(errmsg,"C+"); return 1;}
		if (c < MP->SoftLimitNegC) {strcpy(errmsg,"C-"); return 1;}
	}
	if (u_axis >= 0)
	{
		if (u > MP->SoftLimitPosU) {strcpy(errmsg,"U+"); return 1;}
		if (u < MP->SoftLimitNegU) {strcpy(errmsg,"U-"); return 1;}
	}
	if (v_axis >= 0)
	{
		if (v > MP->SoftLimitPosV) {strcpy(errmsg,"V+"); return 1;}
		if (v < MP->SoftLimitNegV) {strcpy(errmsg,"V-"); return 1;}
	}
	return 0;
}

int CCoordMotion::StraightTraverse(double x, double y, double z, double a, double b, double c, bool NoCallback, int sequence_number, int ID)
{
	return StraightTraverse(x, y, z, a, b, c, current_u, current_v, NoCallback, sequence_number, ID);
}

int CCoordMotion::StraightTraverse(double x, double y, double z, double a, double b, double c, double u, double v, bool NoCallback, int sequence_number, int ID)
{
	double tdx, tdy, tdz, tda, tdb, tdc, tdu, tdv, rate, AccelToUse, JerkToUse;

	if (m_Abort) return 1;

	// check if we should treat rapids as 2nd order feeds as required on some non-linear systems
	if (Kinematics->m_MotionParams.DoRapidsAsFeeds)
	{
		// commit any segments waiting to potentially be combined
		if (CommitPendingSegments(false)) return 1;
		if (nsegs>0)GetSegPtr(nsegs-1)->StopRequiredNextSeg=TRUE;  // stop in case FRO changes
		if (DoKMotionBufCmd("BegRapidBuf",sequence_number)) return 1;
		int result = StraightFeedAccelRapid(1e99, 1e99, true, NoCallback, x, y, z, a, b, c, u, v, sequence_number, ID);
		// commit any segments waiting to potentially be combined
		if (CommitPendingSegments(true)) return 1;
		if (nsegs>0)GetSegPtr(nsegs-1)->StopRequiredNextSeg=TRUE;  // stop in case FRO changes
		if (DoKMotionBufCmd("EndRapidBuf",sequence_number)) return 1;
		return result;
	}

	// check if we should sync parameters with KFLOP
	if (GetRapidSettings()) return 1;
	
	// if exceeding limits trigger Halt
	char errmsg[10];
	if (CheckSoftLimits(x,y,z,a,b,c,u,v,errmsg)) 
	{
		char message[200];
		if (m_Simulate)
		{
			sprintf(message,"Soft Limit %s Rapid Traverse\r\r"
				"Soft Limits disabled for remainder of Simulation",errmsg);
			KMotionDLL->DoErrMsg(message);
			m_DisableSoftLimits=true;
		}
		else
		{
			SetHalt();
			CheckMotionHalt(true);
			SetAbort();
			sprintf(message,"Soft Limit %s Rapid Traverse Job Halted",errmsg);
			KMotionDLL->DoErrMsg(message);
			return 1;
		}
	}


	double dx = x - current_x;
	double dy = y - current_y;
	double dz = z - current_z;
	double da = a - current_a;
	double db = b - current_b;
	double dc = c - current_c;
	double du = u - current_u;
	double dv = v - current_v;

	BOOL pure_angle;

	// compute total distance tool will move by considering both linear and angular movements  

	double d = FeedRateDistance(dx, dy, dz, da, db, dc, du, dv, &pure_angle);


	// commit any segments waiting to potentially be combined
	if (CommitPendingSegments(false)) return 1;

	if (m_StraightTraverseCallback && !NoCallback) m_StraightTraverseCallback(x,y,z,_setup.sequence_number);
	if (m_StraightTraverseSixAxisCallback && !NoCallback) m_StraightTraverseSixAxisCallback(x,y,z,a,b,c,_setup.sequence_number);

	if (d==0.0) return 0;  // ignore zero length moves


	if (!m_Simulate) // skip if we are simulating
	{
		if (DoKMotionBufCmd("BegRapidBuf",sequence_number)) return 1;

		// add in the segment to the planner
		int result = tp_insert_linear_seg_3rdOrder(current_x, 
							 current_y, 
							 current_z, 
							 current_a,
							 current_b,
							 current_c,
							 current_u,
							 current_v,
							 x, y, z, a, b, c, u, v,
							 sequence_number, ID);

		if (DoKMotionBufCmd("EndRapidBuf",sequence_number)) return 1;

		if (result==1) {SetAbort(); return 1;}

		CalcBegDirectionOfSegment(GetSegPtr(nsegs-1), tdx, tdy, tdz, tda, tdb, tdc, tdu, tdv);
		
		// limit Vel based on proportion in that direction
		if (Kinematics->MaxRapidRateInDirection(tdx, tdy, tdz, tda, tdb, tdc, tdu, tdv, &rate)) return 1;
		
		// limit accel based on proportion in that direction
		if (Kinematics->MaxRapidAccelInDirection(tdx, tdy, tdz, tda, tdb, tdc, tdu, tdv, &AccelToUse)) return 1;
		
		// limit Jerk based on proportion in that direction
		if (Kinematics->MaxRapidJerkInDirection(tdx, tdy, tdz, tda, tdb, tdc, tdu, tdv, &JerkToUse)) return 1;
		
		SetSegmentVelAccelJerk(nsegs-1, rate, AccelToUse, JerkToUse);
	
		MaximizeSegments();

		if (DownloadDoneSegments()) {SetAbort(); return 1;}
	}

	current_x  = x;
	current_y  = y;
	current_z  = z;
	current_a  = a;
	current_b  = b;
	current_c  = c;
	current_u  = u;
	current_v  = v;

	return 0;
}

// 6-axes ArcFeed (using max possible Acceeration)

int CCoordMotion::ArcFeed(double DesiredFeedRate_in_per_sec, CANON_PLANE plane,
	double first_end, double second_end,
	double first_axis, double second_axis, int rotation,
	double axis_end_point, double a, double b, double c, int sequence_number, int ID)
{
	return ArcFeedAccel(DesiredFeedRate_in_per_sec, 1e99, plane, first_end, second_end,
		first_axis, second_axis, rotation, axis_end_point, a, b, c, 0.0, 0.0, sequence_number, ID);
}


// ArcFeed (using max possible Acceeration)

int CCoordMotion::ArcFeed(double DesiredFeedRate_in_per_sec, CANON_PLANE plane,
				double first_end, double second_end, 
		        double first_axis, double second_axis, int rotation,
				double axis_end_point, double a, double b, double c, double u, double v, int sequence_number, int ID)
{
	return ArcFeedAccel(DesiredFeedRate_in_per_sec, 1e99, plane, first_end, second_end, 
		        first_axis, second_axis, rotation, axis_end_point, a, b, c, u, v, sequence_number, ID);
}

// 6-axes Arc Feed with Specified Acceleration

int CCoordMotion::ArcFeedAccel(double DesiredFeedRate_in_per_sec, double DesiredAccel, CANON_PLANE plane,
	double first_end, double second_end,
	double first_axis, double second_axis, int rotation,
	double axis_end_point, double a, double b, double c, int sequence_number, int ID)
{
	return ArcFeedAccel(DesiredFeedRate_in_per_sec, DesiredAccel, plane, first_end, second_end,
		        first_axis, second_axis, rotation, axis_end_point, a, b, c, 0.0, 0.0, sequence_number, ID);
}
// Arc Feed with Specified Acceleration

int CCoordMotion::ArcFeedAccel(double DesiredFeedRate_in_per_sec, double DesiredAccel, CANON_PLANE plane,
				double first_end, double second_end, 
		        double first_axis, double second_axis, int rotation,
				double axis_end_point, double a, double b, double c, double u, double v, int sequence_number, int ID)
{
	double HW,SW,radius,theta0,dtheta,MaxLength, cur_first,cur_second,cur_third,dcircle;

	DetermineSoftwareHardwareFRO(HW,SW);

	double FeedRateToUse = DesiredFeedRate_in_per_sec * SW;

	MOTION_PARAMS *MP=&Kinematics->m_MotionParams;

	if (FeedRateToUse <= 0.0)
	{
		SetAbort();
		KMotionDLL->DoErrMsg("Arc Feed with Feed Rate Zero or Negative");
		return 1;
	}

	if (plane == CANON_PLANE_XY)
	{
		cur_first = current_x;
		cur_second= current_y;
		cur_third = current_z;
	}
	else if (plane == CANON_PLANE_XZ)
	{
		cur_first = current_z;
		cur_second= current_x;
		cur_third = current_y;
	}
	else // YZ
	{
		cur_first = current_y;
		cur_second= current_z;
		cur_third = current_x;
	}


	if (m_Abort) return 1;



	double d=CalcLengthAlongHelix(cur_first,cur_second,cur_third,
		                          first_end,second_end,axis_end_point,first_axis,second_axis,
								  rotation,&radius,&theta0,&dtheta,
								  a-current_a,b-current_b,c-current_c,u-current_u,v-current_v, &Kinematics->m_MotionParams, &dcircle);  // total length



	// for drawing on the screen, if we previously stopped part way through
	// an arc, then draw the arc all the way back to where we are
	//
	//   ID=1 : First Arc part of Arc+Straight or Arc+Arc
	//   ID=2 : Second Arc part of Arc+Arc
	//   ID=3 : Straight part of Arc+Straight

	if (m_PreviouslyStopped && m_PreviouslyStoppedID == 2)
	{
		// we stopped n the middle of the second Arc of a Arc+Arc
		// draw from middle point between the two arcs, to
		// where we are now.  Note Arc-Arc must be xy plane
		m_PreviouslyStopped = false;  // we handled it by this point

		current_x = m_StoppedMachinex;
		current_y = m_StoppedMachiney;
	}
	else if (m_PreviouslyStopped && m_PreviouslyStoppedType == SEG_ARC)
	{
		// we were stopped in the middle of an arc,
		// draw the arc all the way back from the beginning
		// of the original arc, to where we are now

		m_PreviouslyStopped = false;  // we handled it by this point


		// set the start of the actual motion to where we stopped
		current_x = m_StoppedMachinex;
		current_y = m_StoppedMachiney;
		current_z = m_StoppedMachinez;
	}


	// check if we can use a straight line segment instead

	double cord = radius * (1.0 - cos(dtheta/2.0));

	if (cord < MP->CollinearTol)
	{
		// use straight line

		if (plane == CANON_PLANE_XY)
			return StraightFeedAccel(DesiredFeedRate_in_per_sec,DesiredAccel,first_end, second_end, axis_end_point, a, b, c, u, v, sequence_number, ID);
		else if (plane == CANON_PLANE_XZ) // actually ZX
			return StraightFeedAccel(DesiredFeedRate_in_per_sec,DesiredAccel,second_end,axis_end_point, first_end,  a, b, c, u, v, sequence_number, ID);
		else // YZ
			return StraightFeedAccel(DesiredFeedRate_in_per_sec,DesiredAccel,axis_end_point, first_end, second_end, a, b, c, u, v, sequence_number, ID);
	}



	// if this move is expected to take more than 1/2th of the download time then break int0 2 parts (recursively)
	// (or if greater than the Kinematics length (line might actually be curved in actuator space)

	MaxLength = FeedRateToUse * MP->TPLookahead/2.0;
	if (MP->MaxLinearLength < MaxLength) MaxLength = MP->MaxLinearLength;

	if (!m_Simulate && (d > MaxLength || MP->UseOnlyLinearSegments || MP->ArcsToSegs || Kinematics->GeoTableValid))
	{
		if ((MP->ArcsToSegs || MP->UseOnlyLinearSegments) && MP->CollinearTol==0.0)
		{
			AfxMessageBox("Error Arcs To Segs selected with Zero Collinear Tolerance");
			SetAbort();
			return 1;
		}

		double theta = theta0 + dtheta/2.0;

		int result = ArcFeedAccel(DesiredFeedRate_in_per_sec, DesiredAccel, plane, first_axis  + radius * cos(theta),
																second_axis + radius * sin(theta),
																first_axis, second_axis, rotation, 
																(cur_third+axis_end_point)/2.0, 
																(current_a + a)/2.0, 
																(current_b + b) / 2.0,
																(current_c + c) / 2.0,
																(current_u + u) / 2.0,
																(current_v + v) / 2.0,
																sequence_number, ID);
		if (result) return result;
		
		return ArcFeedAccel(DesiredFeedRate_in_per_sec, DesiredAccel, plane, first_end, second_end, 
		        first_axis, second_axis, rotation, axis_end_point, a, b, c, u, v, sequence_number, ID);
	}

	// commit any segments waiting to potentially be combined
	if (CommitPendingSegments(false)) return 1;

	// Normal case: just draw the entire single simple arc
	// from where we are now to the end of the arc
	if (m_ArcFeedCallback) m_ArcFeedCallback(true, 
				DesiredFeedRate_in_per_sec, plane,
				first_end, second_end, 
				first_axis, second_axis, rotation,
				axis_end_point,
				cur_first,cur_second,cur_third,_setup.sequence_number, ID);
	
	if (m_ArcFeedSixAxisCallback) m_ArcFeedSixAxisCallback(true, 
				DesiredFeedRate_in_per_sec, plane,
				first_end, second_end, 
				first_axis, second_axis, rotation,
				axis_end_point,a,b,c,
				cur_first,cur_second,cur_third,_setup.sequence_number, ID);


	// if exceeding limits trigger Halt

	// check if we should sync parameters with KFLOP
	if (GetRapidSettings()) return 1;

	int SoftLimitResult;
	char errmsg[5];

	if (plane == CANON_PLANE_XY)
		SoftLimitResult = CheckSoftLimitsArc(first_axis, second_axis, axis_end_point,
					   MP->SoftLimitPosX, MP->SoftLimitNegX,
					   MP->SoftLimitPosY, MP->SoftLimitNegY,
					   MP->SoftLimitPosZ, MP->SoftLimitNegZ,
					   a, b, c, u, v, rotation, radius, theta0, dtheta, 
					   x_axis,y_axis,z_axis,
					   'X', 'Y', 'Z', errmsg);
	else if (plane == CANON_PLANE_XZ) // actually ZX
		SoftLimitResult = CheckSoftLimitsArc(first_axis, second_axis, axis_end_point,
					   MP->SoftLimitPosZ, MP->SoftLimitNegZ,
					   MP->SoftLimitPosX, MP->SoftLimitNegX,
					   MP->SoftLimitPosY, MP->SoftLimitNegY,
					   a, b, c, u, v, rotation, radius, theta0, dtheta, 
					   z_axis,x_axis,y_axis,
					   'Z', 'X', 'Y', errmsg);
	else // YZ
		SoftLimitResult = CheckSoftLimitsArc(first_axis, second_axis, axis_end_point,
					   MP->SoftLimitPosY, MP->SoftLimitNegY,
					   MP->SoftLimitPosZ, MP->SoftLimitNegZ,
					   MP->SoftLimitPosX, MP->SoftLimitNegX,
					   a, b, c, u, v, rotation, radius, theta0, dtheta, 
					   y_axis,z_axis,x_axis,
					   'Y', 'Z', 'X', errmsg);
	
	if (SoftLimitResult) 
	{
		char message[200];
		if (m_Simulate)
		{
			sprintf(message, "Soft Limit %s Arc\r\r"
					"Soft Limits disabled for remainder of Simulation",errmsg);
			KMotionDLL->DoErrMsg(message);
			m_DisableSoftLimits=true;
		}
		else
		{
			SetHalt();
			CheckMotionHalt(true);
			SetAbort();
			sprintf(message, "Soft Limit %s Arc Job Halted",errmsg);
			KMotionDLL->DoErrMsg(message);
			return 1;
		}
	}




	if (!m_Simulate)  // skip if we are simulating
	{
		tp_insert_arc_seg(plane,
						  cur_first, 
						  cur_second, 
						  cur_third, 
						  current_a, 
						  current_b,
						  current_c,
						  current_u,
						  current_v,
			first_end, second_end, axis_end_point,
						  a,
						  b,
						  c,
						  u,
						  v,
						  first_axis, second_axis, rotation,
						  FeedRateToUse, 
						  DesiredAccel, 
						  DesiredAccel,
						  MaxLength,
						  sequence_number, 
						  ID);



		// limit speeds based on proportion in that direction
		// since the segment might have been combined
		// compute the max velocities and accelerations
		// for the possibly new direction

		if (DoRateAdjustmentsArc(nsegs-1,radius,theta0,dtheta,dcircle)) return 1;

		MaximizeSegments();

		if (DownloadDoneSegments()) {SetAbort(); return 1;}
	}

	// update the current position

	if (plane == CANON_PLANE_XY)
	{
		current_x = first_end;
		current_y = second_end;
		current_z = axis_end_point;
	}
	else if (plane == CANON_PLANE_XZ)
	{
		current_z = first_end;
		current_x = second_end;
		current_y = axis_end_point;
	}
	else // YZ
	{
		current_y = first_end;
		current_z = second_end;
		current_x = axis_end_point;
	}

	current_a = a;
	current_b = b;
	current_c = c;
	current_u = u;
	current_v = v;

	return 0;
}

// Commit those segments pending to be combined

int CCoordMotion::CommitPendingSegments(bool RapidMode)
{
	if (m_NumLinearNotDrawn>0)
	{
		int PrevNsegs = nsegs;

		RoundCorner(nsegs-1);

		// only call back and draw segments that were not combined
		// also multiple segments might have been added to round the corner
		
		if (RapidMode)
			DoSegmentCallbacksRapid(PrevNsegs-m_NumLinearNotDrawn,nsegs-1);
		else
			DoSegmentCallbacks(PrevNsegs-m_NumLinearNotDrawn,nsegs-1);

		// limit speeds based on proportion in that direction
		// since the segment might have been combined
		// compute the max velocities and accelerations
		// for the possibly new direction

		if (DoRateAdjustments(PrevNsegs-m_NumLinearNotDrawn, nsegs-1)) return 1;

		MaximizeSegments();

		m_NumLinearNotDrawn = 0;
	}

	return 0;
}

// 6-axes Straight Feed (using Max possible Acceleration)

int CCoordMotion::StraightFeed(double DesiredFeedRate_in_per_sec,
	double x, double y, double z, double a, double b, double c, int sequence_number, int ID)
{
	return StraightFeedAccel(DesiredFeedRate_in_per_sec, 1e99, x, y, z, a, b, c, 0.0, 0.0, sequence_number, ID);
}

// Straight Feed (using Max possible Acceleration)

int CCoordMotion::StraightFeed(double DesiredFeedRate_in_per_sec,
	double x, double y, double z, double a, double b, double c, double u, double v, int sequence_number, int ID)
{
	return StraightFeedAccel(DesiredFeedRate_in_per_sec, 1e99, x, y, z, a, b, c, u, v, sequence_number, ID);
}
	
// 6-axes Straight Feed with specified Acceleration	

int CCoordMotion::StraightFeedAccel(double DesiredFeedRate_in_per_sec, double DesiredAccel,
	double x, double y, double z, double a, double b, double c, int sequence_number, int ID)
{
	return StraightFeedAccelRapid(DesiredFeedRate_in_per_sec, DesiredAccel, false, false, x, y, z, a, b, c, 0.0, 0.0, sequence_number, ID);
}

// Straight Feed with specified Acceleration	

int CCoordMotion::StraightFeedAccel(double DesiredFeedRate_in_per_sec, double DesiredAccel,
	double x, double y, double z, double a, double b, double c, double u, double v, int sequence_number, int ID)
{
	return StraightFeedAccelRapid(DesiredFeedRate_in_per_sec, DesiredAccel, false, false, x, y, z, a, b, c, u, v, sequence_number, ID);
}

// Straight Feed with specified Acceleration and RapidMode	

int CCoordMotion::StraightFeedAccelRapid(double DesiredFeedRate_in_per_sec, double DesiredAccel, bool RapidMode, bool NoCallback,
							   double x, double y, double z, double a, double b, double c, double u, double v, int sequence_number, int ID)
{
	MOTION_PARAMS *MP=&Kinematics->m_MotionParams;
	double HW,SW;

	if (RapidMode)
	{
		SW = 1.0;  // for rapids use all HW
		HW = m_FeedRateRapidOverride;

		if (HW > MP->MaxRapidFRO) HW = MP->MaxRapidFRO;  // limit to max allowed regardless
	}
	else
	{
		DetermineSoftwareHardwareFRO(HW,SW);
	}

	double FeedRateToUse = DesiredFeedRate_in_per_sec * SW;
	double MaxLength;

	if (m_Abort) return 1;

	if (FeedRateToUse <= 0.0)
	{
		SetAbort();
		KMotionDLL->DoErrMsg("Straight Feed with Feed Rate Zero or Negative");
		return 1;
	}

	// if exceeding limits trigger Halt

	// check if we should sync parameters with KFLOP
	if (GetRapidSettings()) return 1;

	char errmsg[5];
	if (CheckSoftLimits(x,y,z,a,b,c,u,v,errmsg)) 
	{
		char FunctionType[32];
		if (RapidMode) strcpy(FunctionType, "Straight Traverse");
		else strcpy(FunctionType, "Straight Feed");

		char message[200];
		if (m_Simulate)
		{
			sprintf(message, "Soft Limit %s %s\r\r"
					"Soft Limits disabled for remainder of Simulation",errmsg, FunctionType);
			KMotionDLL->DoErrMsg(message);
			m_DisableSoftLimits=true;
		}
		else
		{
			SetHalt();
			CheckMotionHalt(true);
			SetAbort();
			sprintf(message, "Soft Limit %s %s Job Halted",errmsg, FunctionType);
			KMotionDLL->DoErrMsg(message);
			return 1;
		}
	}



	double dx = x - current_x;
	double dy = y - current_y;
	double dz = z - current_z;
	double da = a - current_a;
	double db = b - current_b;
	double dc = c - current_c;
	double du = u - current_u;
	double dv = v - current_v;

	BOOL pure_angle;

	// compute total distance tool will move by considering both linear and angular movements  

	double d = FeedRateDistance(dx, dy, dz, da, db, dc, du, dv, &pure_angle);

	if (d==0.0 && !RapidMode) return 0;  // ignore zero length moves

	// if this move is expected to take more than 1/2th of the download time then break int0 2 parts (recursively)
	// (or if greater than the Kinematics length (line might actually be curved in actuator space)

	MaxLength = FeedRateToUse * MP->TPLookahead/2.0;
	if (MP->MaxLinearLength < MaxLength) MaxLength = MP->MaxLinearLength;

	if (!m_Simulate && ((!pure_angle && d > MaxLength) || 
		(MP->DegreesA && fabs(da) > MP->MaxAngularChange) || 
		(MP->DegreesB && fabs(db) > MP->MaxAngularChange) || 
		(MP->DegreesC && fabs(dc) > MP->MaxAngularChange)))
	{
		int result = StraightFeedAccelRapid(DesiredFeedRate_in_per_sec, DesiredAccel, RapidMode, NoCallback,current_x+dx/2.0, current_y+dy/2.0, current_z+dz/2.0, 
														current_a+da/2.0, current_b+db/2.0, current_c+dc/2.0, current_u+du/2.0, current_v+dv/2.0, sequence_number, ID);
		if (result) return result;
		
		return StraightFeedAccelRapid(DesiredFeedRate_in_per_sec, DesiredAccel, RapidMode, NoCallback, x, y, z, a, b, c, u, v, sequence_number, ID);
	}



	if (m_Simulate) 
	{
		// if simulating just draw it now
		if (RapidMode)
		{
			if (m_StraightTraverseCallback && !NoCallback) m_StraightTraverseCallback(x,y,z,_setup.sequence_number);
			if (m_StraightTraverseSixAxisCallback && !NoCallback) m_StraightTraverseSixAxisCallback(x,y,z,a,b,c,_setup.sequence_number);
		}
		else
		{
			if (m_StraightFeedCallback) m_StraightFeedCallback(DesiredFeedRate_in_per_sec,x,y,z,sequence_number, ID);
			if (m_StraightFeedSixAxisCallback) m_StraightFeedSixAxisCallback(DesiredFeedRate_in_per_sec,x,y,z,a,b,c,sequence_number, ID);
		}
		current_x  = x;
		current_y  = y;
		current_z  = z;
		current_a  = a;
		current_b  = b;
		current_c  = c;
		current_u  = u;
		current_v  = v;
		return 0;  // exit if we are simulating
	}

	
	int PrevNsegs = nsegs;
	int AlreadyDrawn = nsegs - 1 - m_NumLinearNotDrawn;
	
	// add in the segment to the planner
	int result = tp_insert_linear_seg(current_x, 
						 current_y, 
						 current_z, 
						 current_a,
						 current_b,
						 current_c, 
						 current_u,
						 current_v, 
						 x, y, z, a, b, c, u, v,
						 FeedRateToUse, 
						 DesiredAccel,
						 MaxLength, sequence_number, ID, m_NumLinearNotDrawn);

	if (result==1) {SetAbort(); return 1;}

	int NumJustAdded = nsegs - PrevNsegs;

	m_NumLinearNotDrawn += NumJustAdded;

	if (m_NumLinearNotDrawn > 2) m_NumLinearNotDrawn = 2;

	// only call back and draw segments that were not combined
	// also multiple segments might have been added to round the corner
	
	if (result==0)
	{
		if (RapidMode)
		{
			if (!NoCallback)
			{
				DoSegmentCallbacksRapid(AlreadyDrawn+1,nsegs-1-m_NumLinearNotDrawn);
			}
		}
		else
		{
			DoSegmentCallbacks(AlreadyDrawn+1,nsegs-1-m_NumLinearNotDrawn);
		}
	}

	// limit speeds based on proportion in that direction
	// since the segment might have been combined
	// compute the max velocities and accelerations
	// for the possibly new direction

	if (result==0)
		if (DoRateAdjustments(AlreadyDrawn+1, nsegs-1-m_NumLinearNotDrawn)) return 1;

	nsegs -= m_NumLinearNotDrawn;  // don't maximize the last segments they might change
	MaximizeSegments();
	nsegs += m_NumLinearNotDrawn;

	current_x  = x;
	current_y  = y;
	current_z  = z;
	current_a  = a;
	current_b  = b;
	current_c  = c;
	current_u  = u;
	current_v  = v;

	if (DownloadDoneSegments()) {SetAbort(); return 1;}

	return 0;
}


void CCoordMotion::DoSegmentCallbacks(int i0, int i1)
{
	if (nsegs >= 1 && (m_StraightFeedCallback || m_StraightFeedSixAxisCallback)) 
	{
		for (int i = i0; i<=i1; i++)
		{
			if (i>=0)
			{
				SEGMENT *p=GetSegPtr(i);
				if (p->type == SEG_LINEAR)
				{
					if (m_StraightFeedCallback)
						m_StraightFeedCallback(p->OrigVel,p->x1,p->y1,p->z1,p->sequence_number,p->ID);
					if (m_StraightFeedSixAxisCallback)
						m_StraightFeedSixAxisCallback(p->OrigVel,p->x1,p->y1,p->z1,p->a1,p->b1,p->c1,p->sequence_number,p->ID);
				}
			}
		}
	}
}

void CCoordMotion::DoSegmentCallbacksRapid(int i0, int i1)
{
	if (nsegs >= 1 && (m_StraightTraverseCallback || m_StraightTraverseSixAxisCallback)) 
	{
		for (int i = i0; i<=i1; i++)
		{
			if (i>=0)
			{
				SEGMENT *p=GetSegPtr(i);
				if (p->type == SEG_LINEAR)
				{
					if (m_StraightTraverseCallback)
						if (m_StraightTraverseCallback) m_StraightTraverseCallback(p->x1,p->y1,p->z1,_setup.sequence_number);
					if (m_StraightTraverseSixAxisCallback)
						if (m_StraightTraverseSixAxisCallback) m_StraightTraverseSixAxisCallback(p->x1,p->y1,p->z1,p->a1,p->b1,p->c1,p->sequence_number);
				}
			}
		}
	}
}


// limit speeds based on proportion in that direction

int CCoordMotion::DoRateAdjustments(int i0, int i1)
{
	double tdx,tdy,tdz,tda,tdb,tdc,tdu,tdv,rate,FeedRateToUse,Accel,AccelToUse;
	
	for (int i = i0; i<=i1; i++)
	{
		if (i>=0)
		{
			SEGMENT *s=GetSegPtr(i);

			if (Kinematics->ComputeAnglesOption(i)) return 1;

			CalcBegDirectionOfSegment(s, tdx, tdy, tdz, tda, tdb, tdc, tdu, tdv);
			
			if (Kinematics->MaxRateInDirection(tdx, tdy, tdz, tda, tdb, tdc, tdu, tdv, &rate)) return 1;
			
			FeedRateToUse = GetSegPtr(i)->OrigVel;
			if (rate < FeedRateToUse) FeedRateToUse = rate;

			// limit accel based on proportion in that direction
			if (Kinematics->MaxAccelInDirection(tdx, tdy, tdz, tda, tdb, tdc, tdu, tdv, &Accel)) return 1;

			AccelToUse = GetSegPtr(i)->OrigAccel;
			if (Accel < AccelToUse) AccelToUse = Accel;

			SetSegmentVelAccels(i, FeedRateToUse, AccelToUse, AccelToUse);
			
			AdjustSegVelocity(i);  // limit based on curvature
		}
	}
	return 0;
}


// limit accleration and velocity to the lowest values throughout the entire arc
// lowest values should occur at the beginning, end, and quadrants

int CCoordMotion::DoRateAdjustmentsArc(int i, double radius, double theta0, double dtheta, double dcircle)
{
	double dx,dy,dz,da,db,dc,du,dv,rate,FeedRateToUse,Accel,AccelToUse;
	double end_theta, next_theta, quadrant, SIGMA = radius*1e-12;
	double AccelCentripedal=1e99;
	
	SEGMENT *p=GetSegPtr(i);
	FeedRateToUse = p->OrigVel;
	AccelToUse = p->OrigAccel;

	next_theta=theta0;
	end_theta=theta0+dtheta;

	if (p->DirIsCCW)
		quadrant = ceil(theta0/(PI*0.5)) - 1.0;
	else
		quadrant = floor(theta0/(PI*0.5)) + 1.0;

	bool first=true;
	do
	{
		// if not first loop advance theta to next peak or to the end.
		if (!first)
		{

			next_theta = quadrant * PI * 0.5;

			if (p->DirIsCCW)
			{	
				if (next_theta > end_theta) next_theta = end_theta;
			}
			else
			{	
				if (next_theta < end_theta) next_theta = end_theta;
			}
		}
		else
		{
			first=false;
		}

		// calc direction (in xy plane) from
		// center of rotation to beginning point

		double dxc = radius * cos(next_theta);
		double dyc = radius * sin(next_theta);
		double dzc = 0;

		// then turn left 90 degrees if CCW, right if CW
		// (note dxy later will be neg if CW)

		dx = -dyc;
		dy =  dxc;

		// scale this vector to be length of total xy motion
		// right now it's length is the radius 

		dx *= dcircle/radius;
		dy *= dcircle/radius;
		dz = p->z1 - p->z0;
		da = p->a1 - p->a0;
		db = p->b1 - p->b0;
		dc = p->c1 - p->c0;
		du = p->u1 - p->u0;
		dv = p->v1 - p->v0;

		// we did everything as if we were in the xy plane
		// if we were in a different axis then switch them
		if (p->plane == CANON_PLANE_XZ)
		{
			// swap
			// X -> Z
			// Y -> X
			// Z -> Y
			double tempz=dz;
			dz=dx;
			dx=dy;
			dy=tempz;
			//and centripedal direction
			tempz=dzc;
			dzc=dxc;
			dxc=dyc;
			dyc=tempz;
		}
		else if (p->plane == CANON_PLANE_YZ)
		{
			//swap
			// X -> Y 
			// Z -> X
			// Y -> Z
			double tempy=dy;
			dy=dx;      
			dx=dz;		
			dz=tempy;	
			//and centripedal direction
			tempy=dyc;
			dyc=dxc;    
			dxc=dzc;	
			dzc=tempy;	
		}

		// limit accel based on proportion in that direction
		if (Kinematics->MaxRateInDirection(dx,dy,dz,da,db,dc,du,dv,&rate)) return 1;
		if (rate < FeedRateToUse) FeedRateToUse = rate;

		// limit accel based on proportion in that direction
		if (Kinematics->MaxAccelInDirection(dx,dy,dz,da,db,dc,du,dv,&Accel)) return 1;
		if (Accel < AccelToUse) AccelToUse = Accel;

		// also determine centripedal acceleration which is perpindicular to
		// the motion and determines the max velocity through the curve
		if (Kinematics->MaxAccelInDirection(dxc,dyc,dzc,0.0,0.0,0.0,0.0,0.0,&Accel)) return 1;
		if (Accel < AccelCentripedal) AccelCentripedal = Accel;

		if (p->DirIsCCW)
			quadrant++;
		else
			quadrant--;
	}
	while (next_theta != end_theta);

	SetSegmentVelAccels(i, FeedRateToUse, AccelToUse, AccelToUse);
	AdjustSegVelocityCircle(i,AccelCentripedal);

	return 0;
}


int CCoordMotion::Dwell(double seconds, int sequence_number)
{
	if (m_Abort) return 1;

	// commit any segments waiting to potentially be combined
	if (CommitPendingSegments(false)) return 1;

	// put something in the displayed path that can be found
	if (m_StraightTraverseCallback) m_StraightTraverseCallback(current_x, current_y, current_z,_setup.sequence_number);
	if (m_StraightTraverseSixAxisCallback) m_StraightTraverseSixAxisCallback(current_x, current_y, current_z, current_a, current_b, current_c,_setup.sequence_number);

	if (!m_Simulate)
	{
		// add in the segment to the planner
		int result = tp_insert_dwell(seconds, current_x, current_y, current_z, current_a, current_b, current_c, current_u, current_v, sequence_number, 0);

		if (result==1) {SetAbort(); return 1;}

		MaximizeSegments();

		if (DownloadDoneSegments()) {SetAbort(); return 1;}
	}
	return 0;
}



// issue a pass through KMotion Command

int CCoordMotion::DoKMotionCmd(const char *s, BOOL FlushBeforeUnbufferedOperation)
{
	if (m_Simulate) return 0;  // exit if we are simulating

	if (FlushBeforeUnbufferedOperation)
	{
		if (FlushSegments()) {SetAbort(); return 1;}  

		if (WaitForSegmentsFinished(TRUE)) {SetAbort(); return 1;}
	}

	if (KMotionDLL->WriteLine(s)) {SetAbort(); return 1;}

	return 0;
}


// issue a buffered KMotion Command at the end of the current segment
// if there are no segment yet, then add to the special_commands_initial list
//
// note special commands use a circular list such as lone as there are not too many
// pending in the motion buffer before they are downloaded any number in a path can
// exist.

int CCoordMotion::DoKMotionBufCmd(const char *s,int sequence_number)
{
	strncpy(special_cmds[nspecial_cmds % MAX_SPECIAL_CMDS].cmd,s,MAX_LINE);

	if (nsegs <=0)  // check if we have any segments yet
	{
		// no, put in the special list to be set at the beginning
		if (special_cmds_initial_first == -1) // none yet, put as first and last
		{
			special_cmds_initial_first = nspecial_cmds;
			special_cmds_initial_sequence_no[SegBufToggle] = sequence_number;    // save sequence number that generated these
		}
		special_cmds_initial_last = nspecial_cmds++;
	}
	else
	{
		// yes there are segments, check if the segment already has any
		SEGMENT *p = GetSegPtr(nsegs-1);

		if (p->special_cmds_first == -1) // none yet, put as first and last
			p->special_cmds_first = nspecial_cmds;

		p->special_cmds_last = nspecial_cmds++;

		// if the segment was already downloaded then send the special command immediately down
		// (Rapids can be Done and Downloaded immediately)
		if (m_nsegs_downloaded >= nsegs)
		{
			if (PutWriteLineBuffer(special_cmds[ispecial_cmd_downloaded % MAX_SPECIAL_CMDS].cmd,0.0)) return 1;
			ispecial_cmd_downloaded++;
		}
	}
	return 0;
}





int CCoordMotion::WaitForSegmentsFinished(BOOL NoErrorOnDisable)
{
	char response[MAX_LINE];
	char response2[MAX_LINE];

	if (m_Simulate || !m_SegmentsStartedExecuting) return 0;

	int count=0;

	do
	{
		if (count++)
		{
			if (KMotionDLL->WriteLineReadLine("ExecTime",response)){SetAbort(); return 1;}
			if (sscanf(response, "%lf",&m_TimeAlreadyExecuted)!= 1){SetAbort(); return 1;}
			UpdateRealTimeState(m_TimeAlreadyExecuted);
			Sleep(10);
		}
		
		if (KMotionDLL->WriteLineReadLine("CheckDoneBuf",response)) {SetAbort(); return 1;}


		if (strcmp(response,"-1")==0)
		{
			if (KMotionDLL->WriteLineReadLine("ExecTime",response2)){SetAbort(); return 1;}
			if (sscanf(response2, "%lf",&m_TimeAlreadyExecuted)!= 1){SetAbort(); return 1;}
			UpdateRealTimeState(m_TimeAlreadyExecuted);

			if (NoErrorOnDisable)
			{
				return 0;
			}
			else
			{
				m_AxisDisabled=true;
				SetAbort();
			}
		}

		if (CheckMotionHalt(true)) return 2;
		if (m_Abort) return 1;
	}
	while (strcmp(response,"1")!=0);

	if (KMotionDLL->WriteLineReadLine("ExecTime",response2)){SetAbort(); return 1;}
	if (sscanf(response2, "%lf",&m_TimeAlreadyExecuted)!= 1){SetAbort(); return 1;}
	UpdateRealTimeState(m_TimeAlreadyExecuted);

	m_SegmentsStartedExecuting = false;

	return 0;
}

int CCoordMotion::WaitForMoveXYZABCFinished()
{
	char response[MAX_LINE];

	int count=0;
	do
	{
		if (count++) Sleep(10);
		if (KMotionDLL->WriteLineReadLine("CheckDoneXYZABC",response)) {SetAbort(); return 1;}

		if (strcmp(response,"-1")==0)
		{
			m_AxisDisabled=true;
			SetAbort();
		}

		if (CheckMotionHalt(false)) return 2;

		if (m_Abort) return 1;
	}
	while (strcmp(response,"1")!=0);

	return 0;
}


int CCoordMotion::CheckMotionHalt(bool Coord)
{
	char response[MAX_LINE];
	char responsebuf[MAX_LINE];
	double BufTime=0;
	bool Finished=false, NotStarted=false;
	SEGMENT *segs_to_check;

	if (!m_Abort && m_Halt && !m_Simulate)
	{
		// stop immediately
		if (KMotionDLL->WriteLine("StopImmediate0"))  {SetAbort(); return 1;}

		// wait until stopped
		do
		{
			if (KMotionDLL->WriteLineReadLine("GetStopState",response))  {SetAbort(); return 1;}

			if (m_Abort) return 1;

			// There is a chance that the motion path could complete just as the StopImmediate was
			// in progress.  So see if this was the case.

			if (Coord && strcmp(response,"1")==0)
			{
				if (KMotionDLL->WriteLineReadLine("CheckDoneBuf",responsebuf)) {SetAbort(); return 1;}

				if (strcmp(responsebuf,"1")==0)
				{
					// yes we are all finished with the buffer 
					Finished = true;
				}
				else if (strcmp(responsebuf,"-1")==0)
				{
					m_AxisDisabled=true;
					SetAbort();
				}
			}
		}
		while (!Finished && (strcmp(response,"1")==0 || strcmp(response,"2")==0));

		if (strcmp(response,"0")==0) NotStarted=true;

		// default to current line in case somthing bad happens
		m_PreviouslyStoppedSeqNo = _setup.sequence_number;

		if (Coord)
		{
			if (Finished)
				m_PreviouslyStopped = m_Stopping = STOPPED_COORD_FINISHED;
			else
				m_PreviouslyStopped = m_Stopping = STOPPED_COORD;
		}
		else
			m_PreviouslyStopped = m_Stopping = STOPPED_INDEP;

		if (Coord)
		{
			if (Finished || NotStarted)
			{
				m_TimeAlreadyExecuted=0;
			}
			else
			{
				// determine the line number we were at
				if (KMotionDLL->WriteLineReadLine("ExecTime",response)) return 1;
				int result=sscanf(response, "%lf",&m_TimeAlreadyExecuted);
				if (result != 1)  return 1;
				if (m_TimeAlreadyExecuted < 0.0) return 1;
			}
		}


		// clear stop state - note this clears time executed in DSP
		if (KMotionDLL->WriteLine("StopImmediate2"))  {SetAbort(); return 1;}

		// save the raw machine positions
		if (ReadCurAbsPosition(&m_StoppedMachinex,&m_StoppedMachiney,&m_StoppedMachinez,&m_StoppedMachinea,&m_StoppedMachineb,&m_StoppedMachinec,&m_StoppedMachineu,&m_StoppedMachinev,true)) return 1;

		if (Coord)
		{
			int i,k,n;

			if (NotStarted)
			{
				// might be stuff in the planner
				n=nsegs;
				segs_to_check=segments;
			}
			else if (Finished)
			{
				if (segments_executing==segments)
				{
					// check if everything was downloaded
					if (m_nsegs_downloaded>0 && nsegs>0 && m_nsegs_downloaded == nsegs)
					{
						// We were all finished and nothing was done ahead so exit
						SetAbort();
						return 2;
					}
					else
					{
						// This is a somewhat invalid case where where we were buffering a very 
						// small amount ahead (less than the amount of time to stop) so we finished
						// what was in the buffer before the feedhold slowed time to a stop
						// this would result in a sudden motion stop.  But set the Stopped state to 
						// what we last downloaded.

						// set stop conditions for this segment
						SetPreviouslyStoppedAtSeg(segments,m_nsegs_downloaded-1);

						SetAbort();
						return 2;
					}
				}
				else
				{
					// yes we switched buffers so there might be newer stuff in segments
					n=nsegs;
					segs_to_check=segments;
				}
			}
			else
			{
				segs_to_check = segments_executing;

				if (segments_executing==segments)
					n=nsegs;
				else
					n=prev_nsegs;
			}

			// if we finished and there is nothing else 
			// then stay at the current line

			if (Finished && n==0)
			{
				m_PreviouslyStopped = m_Stopping = STOPPED_NONE;
				SetAbort();
				return 1;
			}


			if (Finished || NotStarted)
			{
				// we finished the path, so set that we are at the
				// at the very beginning of the next.  Note the
				// next path might not be planned yet and will have
				// invalid or zero times
				 i=0;  

				 // if nothing in the buffer exit leaving everything as it is
				 if (n==0)
				 {
					m_PreviouslyStoppedType = SEG_UNDEFINED;
					m_PreviouslyStoppedID = -1;
					SetAbort();
					return 2;
				 }
			}
			else
			{
				// search backwards for where we were in the path based on time
				int index;

				if (segs_to_check==segments0)
					index=0;
				else
					index=1;

				BufTime=SegsDoneTime[index];

				// if nothing done or downloaded yet don't search or worry if we
				// can't find current segment in the motion buffer
				if (SegsDone[index] == -1)
				{
					// if initial buffered commands are present with a valid
					// sequence number than return that
					if (special_cmds_initial_sequence_no[index] >= 0)
					{
						m_PreviouslyStoppedSeqNo = special_cmds_initial_sequence_no[index];
					}
					m_PreviouslyStoppedType = SEG_UNDEFINED;
					m_PreviouslyStoppedID = -1;
					SetAbort();
					return 2;
				}

				for (i=SegsDone[index]; i>=0; i--)
				{
					for (k=segs_to_check[TPMOD(i)].nTrips-1; k>=0; k--)
					{
						if (BufTime <= m_TimeAlreadyExecuted) break;
						BufTime -= segs_to_check[TPMOD(i)].C[k].t;  
					}
					if (BufTime <= m_TimeAlreadyExecuted || 
						(m_TimeAlreadyExecuted==0.0 && BufTime<1e-6)) break;
				}
			}

			if (i==-1)
			{
				SetAbort();
				KMotionDLL->DoErrMsg("Invalid buffer times on Halt");
				return 1;
			}

			// set stop conditions for this segment
			SetPreviouslyStoppedAtSeg(segs_to_check,i);
		}
		else
		{
			m_PreviouslyStoppedType = SEG_UNDEFINED;
			m_PreviouslyStoppedID = -1;
		}

		
		SetAbort();
		return 2;
	}
	return 0;
}


// set Gcode line number back to this line number
// also remember if we were in an arc or a linear
// to help handle resuming with tool compensation

void CCoordMotion::SetPreviouslyStoppedAtSeg(SEGMENT *segs_to_check,int i)
{
	m_PreviouslyStoppedType = segs_to_check[TPMOD(i)].type;
	m_PreviouslyStoppedID = segs_to_check[TPMOD(i)].ID;
	m_PreviouslyStoppedSeqNo = segs_to_check[TPMOD(i)].sequence_number;

	// check if we were in the middle of the second "double arc"
	// if so, find and save the beginning of the arc
	if (m_PreviouslyStoppedID == 2)
	{
		while (i>0 && segs_to_check[TPMOD(i-1)].ID == m_PreviouslyStoppedID) i--;

		// note: if the between point wasn't in the buffer, then
		// we must have halted and resumed within the second arc
		// so keep the mid point that was computed by the 
		// interpreter when we resumed
		if (i>0)
		{
			SEGMENT *p=&segs_to_check[TPMOD(i)];
			m_StoppedMidx = p->x0;
			m_StoppedMidy = p->y0;
			m_StoppedMidz = p->z0;
			m_StoppedMida = p->a0;
			m_StoppedMidb = p->b0;
			m_StoppedMidc = p->c0;
			m_StoppedMidu = p->u0;
			m_StoppedMidv = p->v0;
		}
	}
}





// execute whatever segments are in the queue

int CCoordMotion::FlushSegments()
{
	int ispecial_cmd=0;

	if (nsegs>0 && m_NumLinearNotDrawn>0)
	{
		int AlreadyDrawn = nsegs - 1 - m_NumLinearNotDrawn;

		RoundCorner(nsegs-1);

		// only call back and draw segments that were not combined
		// also multiple segments might have been added to round the corner
		
		DoSegmentCallbacks(AlreadyDrawn+1,nsegs-1);

		// limit speeds based on proportion in that direction
		// since the segment might have been combined
		// compute the max velocities and accelerations
		// for the possibly new direction

		if (DoRateAdjustments(AlreadyDrawn+1, nsegs-1)) return 1;

		MaximizeSegments();

		m_NumLinearNotDrawn = 0;
	}

	// there are no more segments in this path, so maximize them
	// since we were waiting to see in there might have been
	// another one to combine
	MaximizeSegments();

	if (m_Simulate) return 0;  // exit if we are simulating

	// now download the 3 trip states for the segment

	if (nsegs>0 || nspecial_cmds>0)
	{
		if (m_nsegs_downloaded==0)
		{
			// if the first one, open it

			if (WaitForSegmentsFinished()) {SetAbort(); return 1;}

			if(KMotionDLL->WriteLine("OpenBuf")) {SetAbort(); return 1;}
			m_TimeAlreadyExecuted=0;
			ClearWriteLineBuffer();
			
			// see if there are any special commands that need to be inserted before path starts
			if (DoSpecialInitialCommands()){SetAbort(); return 1;} ;
		}


		int iseg;
		for (iseg=m_nsegs_downloaded; iseg<nsegs; iseg++)
		{
			if (OutputSegment(iseg))   {SetAbort(); return 1;}  // Output the Segment
			// see if there are any special commands that need to be inserted after segs
			if (DoSpecialCommand(iseg)) {SetAbort(); return 1;} ;
		}
			

		if (FlushWriteLineBuffer()) return 1;  // flush any segments at the end

		// Execute segments and special commands

		if (!m_SegmentsStartedExecuting)
		{
			// we are ready to start executing the buffer
			// check if the user pushed halt, and if so
			// delete everthing and rewind the interpreter
			// back to the beginning of the sequence

			if (CheckMotionHalt(true)) return 2;

			if(LaunchCoordMotion()) return 1;
			m_SegmentsStartedExecuting = true;
			segments_executing=segments;
		}
		
		// Tell KFLOP that the buffer is complete sp that it doesn't worry about starvation
		if(KMotionDLL->WriteLine("FlushBuf")) {SetAbort(); return 1;}

		tp_init();
		DownloadInit();
	}
	return 0;
}


int CCoordMotion::LaunchCoordMotion()
{
	char s[MAX_LINE];

	if (KMotionDLL->WriteLineReadLine("CheckDoneBuf",s)) {SetAbort(); return 1;}

	if (strcmp(s, "-1") == 0)
	{
		m_AxisDisabled=true;
		SetAbort();
		return 1;
	}

	if (m_ThreadingMode)  // Launch coordinated motion in spindle sync mode ?
	{
		if (fabs(m_ThreadingBaseSpeedRPS) < 1e-9)
		{
			AfxMessageBox("Error Threading with Zero Speed");
			SetAbort();
			return 1;
		}

		sprintf(s,"TrigThread %.6f",m_ThreadingBaseSpeedRPS);
		if(KMotionDLL->WriteLine(s)){SetAbort(); return 1;}
	}
	else  // no normal coordinated motion
	{
		if(KMotionDLL->WriteLine("ExecBuf")){SetAbort(); return 1;}
	}
	return 0;
}



// intitalize everything related to downloading and look ahead times


void CCoordMotion::DownloadInit()
{
	m_nsegs_downloaded=0;
	m_TotalDownloadedTime = m_TimeAlreadyExecuted = 0.0;
	m_ThreadingMode=false;
}



// Called when the Interpreter is stopping due to either
// a single step, a halt, or program stop
//
// if some segments have been downloaded and execution
// is in progress, then we should send whatever is
// left in the TP which should bring everything to a 
// safe stop similar as if we had a break angle at
// this point
//
// 

int CCoordMotion::ExecutionStop()
{
	// commit any segments waiting to potentially be combined
	if (CommitPendingSegments(false)) return 1;

	if (m_Abort) return 1;

	if (!m_Halt)
		if (FlushSegments()) return 1;

	int result = WaitForSegmentsFinished();

#ifdef DEBUG_DOWNLOAD
	CloseDiag();
#endif

	return result;
}


// download any special commands that belong at the begining of the path

int CCoordMotion::DoSpecialInitialCommands()
{
	while (ispecial_cmd_downloaded <  nspecial_cmds &&
		   ispecial_cmd_downloaded >= special_cmds_initial_first &&
		   ispecial_cmd_downloaded <= special_cmds_initial_last)
	{
		if (PutWriteLineBuffer(special_cmds[ispecial_cmd_downloaded % MAX_SPECIAL_CMDS].cmd,0.0)) return 1;
		ispecial_cmd_downloaded++;
	}
	return 0;
}

// download any special commands that belong after this segment

int CCoordMotion::DoSpecialCommand(int iseg)
{
	// see if there are any special commands that need to be inserted after the seg

	while (ispecial_cmd_downloaded <  nspecial_cmds &&
		   ispecial_cmd_downloaded >= GetSegPtr(iseg)->special_cmds_first &&
		   ispecial_cmd_downloaded <= GetSegPtr(iseg)->special_cmds_last)
	{
		if (PutWriteLineBuffer(special_cmds[ispecial_cmd_downloaded % MAX_SPECIAL_CMDS].cmd,0.0)) return 1;
		ispecial_cmd_downloaded++;
	}
	return 0;
}


int CCoordMotion::OutputSegment(int iseg)
{
	char s[256];
	MOTION_PARAMS *MP=&Kinematics->m_MotionParams;

	static bool LastWasLinear=false;  // After an arc we need to specify all.
	bool DidThisLinear=false;  // multiple segments can use the same start/end point only with diff accel

	// output the segments

	tp_calc_seg_trip_states(iseg);


#if 0 
	// Output Segment buffer to a file
	FILE *f;
	
	if (iseg==0)
		f = fopen("c:\\Temp\\data.txt", "wt");
	else
		f = fopen("c:\\Temp\\data.txt", "at");

	int k = iseg;
	fprintf(f, "segment,%d,type,%d,Done,%d,StopRequired,%d,ChangeInDirection,%.6f,vel,%.6f,MaxVel,%.6f,dx,%.6f,x,%.6f,y,%.6f,T,%.6f\n",
			k, GetSegPtr(k)->type, GetSegPtr(k)->Done, GetSegPtr(k)->StopRequired,
			GetSegPtr(k)->ChangeInDirection, GetSegPtr(k)->vel, GetSegPtr(k)->MaxVel, GetSegPtr(k)->dx,
			GetSegPtr(k)->x1, GetSegPtr(k)->y1, GetSegPtr(k)->C[0].t + GetSegPtr(k)->C[1].t + GetSegPtr(k)->C[2].t);
	fclose(f);
#endif

	// keep track of total Done time
	// and which segment
	SegsDone[SegBufToggle] = iseg;
	SEGMENT *p=GetSegPtr(iseg);

	for (int i=0; i<p->nTrips; i++)
		SegsDoneTime[SegBufToggle] += p->C[i].t;

	if (m_nsegs_downloaded==0) LastWasLinear=false;

	for (int i=0; i<p->nTrips; i++)
	{
		if (p->C[i].t > 0.0)  // discard zero time segments
		{
			if (p->type == SEG_LINEAR || p->type == SEG_RAPID || p->type == SEG_DWELL)
			{
				double dx = p->x1-p->x0;
				double dy = p->y1-p->y0;
				double dz = p->z1-p->z0;
				double da = p->a1-p->a0;
				double db = p->b1-p->b0;
				double dc = p->c1-p->c0;
				double du = p->u1-p->u0;
				double dv = p->v1-p->v0;
				
				double d,invd;
				BOOL pure_angle;
				
				d = FeedRateDistance(dx, dy, dz, da, db, dc, du, dv, &pure_angle);
				
				if (d>0.0)
					invd = 1.0/d; // inverse total length
				else    
					invd = 0.0f;

				//  send as hex binary 32 bit floats


				float FloatUVArray[4],FloatArray[17];
				double Acts[MAX_ACTUATORS];
				
				if (Kinematics->TransformCADtoActuators(p->x0,p->y0,p->z0,p->a0,p->b0,p->c0,p->u0,p->v0,Acts)) return 1;

				FloatArray[0]  = (float)Acts[0];
				FloatArray[1]  = (float)Acts[1];
				FloatArray[2]  = (float)Acts[2];
				FloatArray[3]  = (float)Acts[3];
				FloatArray[4]  = (float)Acts[4];
				FloatArray[5]  = (float)Acts[5];
				FloatUVArray[0] = (float)Acts[6];
				FloatUVArray[1] = (float)Acts[7];

				if (Kinematics->TransformCADtoActuators(p->x1,p->y1,p->z1,p->a1,p->b1,p->c1,p->u1,p->v1,Acts)) return 1;
				
				FloatArray[6]  = (float)Acts[0];
				FloatArray[7]  = (float)Acts[1];
				FloatArray[8]  = (float)Acts[2];
				FloatArray[9]  = (float)Acts[3];
				FloatArray[10] = (float)Acts[4];
				FloatArray[11] = (float)Acts[5];
				FloatUVArray[2] = (float)Acts[6];
				FloatUVArray[3] = (float)Acts[7];

				FloatArray[12] = (float)(p->C[i].a*invd);
				FloatArray[13] = (float)(p->C[i].b*invd);
				FloatArray[14] = (float)(p->C[i].c*invd);
				FloatArray[15] = (float)(p->C[i].d*invd);
				FloatArray[16] = (float)(p->C[i].t);

				int *Int = (int *)FloatArray;
				int *IntUV = (int *)FloatUVArray;

#ifdef DEBUG_DOWNLOAD
				sprintf(ds, "Linear %f %d %d\n",DTimer.Elapsed_Seconds(),iseg,i);
				PutString(ds);
#endif


				if (!LastWasLinear)  // must specify all if first or there was an arc
				{
					if (u_axis >= 0 || v_axis >= 0)
						sprintf(s, "LinearHexEx %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
							Int[0], Int[1], Int[2], Int[3], Int[4], Int[5], IntUV[0], IntUV[1], Int[6], Int[7], Int[8], Int[9], Int[10], Int[11], IntUV[2], IntUV[3], Int[12], Int[13], Int[14], Int[15], Int[16]);
					else
						sprintf(s, "LinearHex %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
							Int[0], Int[1], Int[2], Int[3], Int[4], Int[5], Int[6], Int[7], Int[8], Int[9], Int[10], Int[11], Int[12], Int[13], Int[14], Int[15], Int[16]);

					DidThisLinear=LastWasLinear=true;
				}
				else if (!DidThisLinear) // new linear so we must specify the new endpoint
				{
					if (u_axis >= 0 || v_axis >= 0)
						sprintf(s, "LHexEx1 %X %X %X %X %X %X %X %X %X %X %X %X %X", Int[6], Int[7], Int[8], Int[9], Int[10], Int[11], IntUV[2], IntUV[3], Int[12], Int[13], Int[14], Int[15], Int[16]);
					else
						sprintf(s, "LHex1 %X %X %X %X %X %X %X %X %X %X %X", Int[6], Int[7], Int[8], Int[9], Int[10], Int[11], Int[12], Int[13], Int[14], Int[15], Int[16]);
							
					DidThisLinear=true;
				}
				else
				{
					sprintf(s, "LHex2 %X %X %X %X %X",
						Int[12],Int[13],Int[14],Int[15],Int[16]); 
				}

			}
			else
			{
				double invd, radius, theta0, theta1, d_theta;
				double dx = p->x0-p->xc;
				double dy = p->y0-p->yc;
				double dz = p->z1-p->z0;

				LastWasLinear=false;  // remember we did an arc so next linear needs to specify all parameters
				
				radius = sqrt(dx*dx + dy*dy); 

				theta0 = atan2(p->y0-p->yc,p->x0-p->xc);
				theta1 = atan2(p->y1-p->yc,p->x1-p->xc);  
									
				d_theta =  theta1 - theta0;        

				if (fabs(d_theta) < THETA_SIGMA) d_theta=0;  // force super small arcs to zero

				if (p->DirIsCCW)
				{
					if (d_theta <= 0.0f) d_theta+=TWO_PI; // CCW delta should be +  
				}
				else
				{
					if (d_theta >= 0.0f) d_theta-=TWO_PI;  // CW delta should be -
				}

				
				if (p->dx > 0.0)
					invd = 1.0/p->dx;
				else
					invd = 0.0;
			

				float FloatArray[19],FloatUVArray[4];
				int *Int = (int *)FloatArray;
				int *IntUV = (int *)FloatUVArray;
				double Acts[MAX_ACTUATORS];

				if (p->plane == CANON_PLANE_XY)
				{
					if (Kinematics->TransformCADtoActuators(p->xc, p->yc, p->z0, p->a0, p->b0, p->c0, p->u0, p->v0, Acts)) return 1;

					FloatArray[0]  = (float)Acts[0];
					FloatArray[1]  = (float)Acts[1];

					FloatArray[2]  = (float)(radius * MP->CountsPerInchX);
					FloatArray[3]  = (float)(radius * MP->CountsPerInchY);
					
					FloatArray[4]  = (float)(theta0);
					FloatArray[5]  = (float)(d_theta);
					FloatArray[6]  = (float)Acts[2];
					FloatArray[7]  = (float)Acts[3];
					FloatArray[8]  = (float)Acts[4];
					FloatArray[9]  = (float)Acts[5];
					FloatUVArray[0] = (float)Acts[6];
					FloatUVArray[1] = (float)Acts[7];

					if (Kinematics->TransformCADtoActuators(p->xc, p->yc, p->z1, p->a1, p->b1, p->c1, p->u1, p->v1, Acts)) return 1;
					
					FloatArray[10]  = (float)Acts[2];
					FloatArray[11]  = (float)Acts[3];
					FloatArray[12]  = (float)Acts[4];
					FloatArray[13]  = (float)Acts[5];
					FloatUVArray[2] = (float)Acts[6];
					FloatUVArray[3] = (float)Acts[7];

					FloatArray[14]  = (float)(p->C[i].a * invd);
					FloatArray[15]  = (float)(p->C[i].b * invd);
					FloatArray[16]  = (float)(p->C[i].c * invd);
					FloatArray[17]  = (float)(p->C[i].d * invd);
					FloatArray[18]  = (float)(p->C[i].t);

					if (u_axis >= 0 || v_axis >= 0)
						sprintf(s, "ArcHexEx %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
							Int[0], Int[1], Int[2], Int[3], Int[4], Int[5], Int[6], Int[7], Int[8], Int[9], IntUV[0], IntUV[1], Int[10], Int[11], Int[12], Int[13], IntUV[2], IntUV[3], Int[14], Int[15], Int[16], Int[17], Int[18]);
					else
						sprintf(s, "ArcHex %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
						Int[0], Int[1], Int[2], Int[3], Int[4], Int[5], Int[6], Int[7], Int[8], Int[9], Int[10], Int[11], Int[12], Int[13], Int[14], Int[15], Int[16], Int[17], Int[18]);
				}
				else if (p->plane == CANON_PLANE_XZ)
				{
					if (Kinematics->TransformCADtoActuators(p->yc,p->z0,p->xc,
						                                    p->a0,p->b0,p->c0,Acts)) return 1;

					FloatArray[0]  = (float)Acts[2];
					FloatArray[1]  = (float)Acts[0];

					FloatArray[2]  = (float)(radius * MP->CountsPerInchZ);
					FloatArray[3]  = (float)(radius * MP->CountsPerInchX);
					
					FloatArray[4]  = (float)(theta0);
					FloatArray[5]  = (float)(d_theta);
					FloatArray[6]  = (float)Acts[1];
					FloatArray[7]  = (float)Acts[3];
					FloatArray[8]  = (float)Acts[4];
					FloatArray[9]  = (float)Acts[5];
					FloatUVArray[0] = (float)Acts[6];
					FloatUVArray[1] = (float)Acts[7];

					if (Kinematics->TransformCADtoActuators(p->yc,p->z1,p->xc,
															p->a1,p->b1,p->c1,Acts)) return 1;
					
					FloatArray[10]  = (float)Acts[1];
					FloatArray[11]  = (float)Acts[3];
					FloatArray[12]  = (float)Acts[4];
					FloatArray[13]  = (float)Acts[5];
					FloatUVArray[2] = (float)Acts[6];
					FloatUVArray[3] = (float)Acts[7];

					FloatArray[14]  = (float)(p->C[i].a * invd);
					FloatArray[15]  = (float)(p->C[i].b * invd);
					FloatArray[16]  = (float)(p->C[i].c * invd);
					FloatArray[17]  = (float)(p->C[i].d * invd);
					FloatArray[18]  = (float)(p->C[i].t);

					if (u_axis >= 0 || v_axis >= 0)
						sprintf(s, "ArcHexZXEx %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
							Int[0], Int[1], Int[2], Int[3], Int[4], Int[5], Int[6], Int[7], Int[8], Int[9], IntUV[0], IntUV[1], Int[10], Int[11], Int[12], Int[13], IntUV[2], IntUV[3], Int[14], Int[15], Int[16], Int[17], Int[18]);
					else
						sprintf(s, "ArcHexZX %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
							Int[0], Int[1], Int[2], Int[3], Int[4], Int[5], Int[6], Int[7], Int[8], Int[9], Int[10], Int[11], Int[12], Int[13], Int[14], Int[15], Int[16], Int[17], Int[18]);
				}
				else // YZ
				{
					if (Kinematics->TransformCADtoActuators(p->z0,p->xc,p->yc,
  									                        p->a0,p->b0,p->c0,Acts)) return 1;

					FloatArray[0]  = (float)Acts[1];
					FloatArray[1]  = (float)Acts[2];

					FloatArray[2]  = (float)(radius * MP->CountsPerInchY);
					FloatArray[3]  = (float)(radius * MP->CountsPerInchZ);
					
					FloatArray[4]  = (float)(theta0);
					FloatArray[5]  = (float)(d_theta);
					FloatArray[6]  = (float)Acts[0];
					FloatArray[7]  = (float)Acts[3];
					FloatArray[8]  = (float)Acts[4];
					FloatArray[9]  = (float)Acts[5];
					FloatUVArray[0] = (float)Acts[6];
					FloatUVArray[1] = (float)Acts[7];

					if (Kinematics->TransformCADtoActuators(p->z1,p->xc,p->yc,
  									                        p->a0,p->b0,p->c0,Acts)) return 1;
					
					FloatArray[10]  = (float)Acts[0];
					FloatArray[11]  = (float)Acts[3];
					FloatArray[12]  = (float)Acts[4];
					FloatArray[13]  = (float)Acts[5];
					FloatUVArray[2] = (float)Acts[6];
					FloatUVArray[3] = (float)Acts[7];

					FloatArray[14]  = (float)(p->C[i].a * invd);
					FloatArray[15]  = (float)(p->C[i].b * invd);
					FloatArray[16]  = (float)(p->C[i].c * invd);
					FloatArray[17]  = (float)(p->C[i].d * invd);
					FloatArray[18]  = (float)(p->C[i].t);

					if (u_axis >= 0 || v_axis >= 0)
						sprintf(s, "ArcHexYZEx %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
							Int[0], Int[1], Int[2], Int[3], Int[4], Int[5], Int[6], Int[7], Int[8], Int[9], IntUV[0], IntUV[1], Int[10], Int[11], Int[12], Int[13], IntUV[2], IntUV[3], Int[14], Int[15], Int[16], Int[17], Int[18]);
					else
						sprintf(s, "ArcHexYZ %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
							Int[0], Int[1], Int[2], Int[3], Int[4], Int[5], Int[6], Int[7], Int[8], Int[9], Int[10], Int[11], Int[12], Int[13], Int[14], Int[15], Int[16], Int[17], Int[18]);
				}


#ifdef DEBUG_DOWNLOAD
				sprintf(ds, "Arc %f %d %d\n",DTimer.Elapsed_Seconds(),iseg,i);
				PutString(ds);
#endif
			}

			if (PutWriteLineBuffer(s,p->C[i].t)) return 1;
		
			m_TotalDownloadedTime += p->C[i].t;  // sum all downloaded times

#ifdef DEBUG_DOWNLOAD
				sprintf(ds, "Done %f %d %d %f\n",DTimer.Elapsed_Seconds(),iseg,i,m_TotalDownloadedTime);
				PutString(ds);
#endif
		}
	}



#if 0 //  sanity check.  Sum times in buffer must be equal to Downloaded times
	{
		int i;
		double BufTime=SegsDoneTime[SegBufToggle];
		for (i=SegsDone[SegBufToggle]; i>=0; i--)
		{
			for (int k=segments[TPMOD(i)].nTrips-1; k>=0; k--)
			{
				if (BufTime <= 1e-6) break;
				BufTime -= segments[TPMOD(i)].C[k].t;  
			}
			if (BufTime <= 1e-6) break;
		}
		if (i!=0)
		{
			KMotionDLL->DoErrMsg("Wrong Buffer Times",MB_ICONEXCLAMATION|MB_OK);   
			return 1;
		}
	}
#endif




	m_nsegs_downloaded++;

	if (!m_SegmentsStartedExecuting && (m_TotalDownloadedTime  - m_TimeAlreadyExecuted) > MP->TPLookahead)
	{
		if (FlushWriteLineBuffer()) return 1;  // flush any segments before starting
		if (CheckMotionHalt(true)) return 2;
		if(LaunchCoordMotion()) return 1;
		m_SegmentsStartedExecuting = true;
		segments_executing=segments;
	}

	if (m_SegmentsStartedExecuting)
	{
		char response[MAX_LINE];
		bool wait;
		int result;

		// Check how far ahead we are

		// first check old value of m_TimeAlreadyExecuted, if we aren't too far ahead of that
		// then don't bother getting new value from DSP

		if (CheckMotionHalt(true)) return 2;

		wait = (m_TotalDownloadedTime - m_TimeAlreadyExecuted) > MP->TPLookahead;
			
		while (wait && m_SegmentsStartedExecuting)
		{
			if (FlushWriteLineBuffer()) return 1;  // flush any segments beforehand
			if (KMotionDLL->WriteLineReadLine("ExecTime",response)) return 1;

			result=sscanf(response, "%lf",&m_TimeAlreadyExecuted);
			if (result != 1)  return 1;

			if (m_TimeAlreadyExecuted < 0.0)
			{
				// Buffer ran dry.  Check if an axis was disabled
				if (KMotionDLL->WriteLineReadLine("CheckDoneXYZABC",response)) {SetAbort(); return 1;}

				if (strcmp(response,"-1")==0)
				{
					m_AxisDisabled=true;  // yes the reason was axis disabled
					SetAbort();
					return 1;
				}
				else
				{
					result = AfxMessageBox("Unexpected Coordinated Motion Buffer Underflow!\r"
						"(Consider increasing the Trajectory Planner Lookahead time in the Configuration Screen)\r\r"
						"Press OK to attempt to continue motion\r"
						"Press CANCEL to abort",
						MB_ICONEXCLAMATION|MB_OKCANCEL);   

					if (result == IDCANCEL)
					{
						SetAbort();
						return 1;
					}
					else
					{
						m_SegmentsStartedExecuting = false;
					}
				}
			}
			else
			{
				if (CheckMotionHalt(true)) return 2;
				UpdateRealTimeState(m_TimeAlreadyExecuted);
				
				wait = (m_TotalDownloadedTime - m_TimeAlreadyExecuted) > MP->TPLookahead;
				if (wait) 
				{
					Sleep(30);
				}
				if (m_Abort) return 1;
			}
		}

	}

	return 0;
}

// Using the time of motion reported by KFLOP figure out
// what state the Interpreter was in at that previous state
int CCoordMotion::UpdateRealTimeState(double T)
{
	double BufTime=0;
	bool Finished=false, NotStarted=false;
	SEGMENT *segs_to_check;
	int n,i,k;

	if (!m_Simulate)
	{
		if (T < 0.0)  // Motion finish?
		{
			m_realtime_Sequence_number_valid = false;
			return 0;
		}

		segs_to_check = segments_executing;

		if (segments_executing==segments)
			n=nsegs;
		else
			n=prev_nsegs;

		// search backwards for where we were in the path based on time
		int index;

		if (segs_to_check==segments0)
			index=0;
		else
			index=1;

		BufTime=SegsDoneTime[index];

		// if nothing done or downloaded yet don't search or worry if we
		// can't find current segment in the motion buffer
		if (SegsDone[index] == -1)
		{
			// if initial buffered commands are present with a valid
			// sequence number than return that
			if (special_cmds_initial_sequence_no[index] >= 0)
			{
				m_realtime_Sequence_number = special_cmds_initial_sequence_no[index];
				m_realtime_Sequence_number_valid = true;
			}
			return 0;
		}

		// Search backwards starting at last done segment until we find
		// the one that encompasses the executed time
		for (i=SegsDone[index]; i>=0; i--)
		{
			for (k=segs_to_check[TPMOD(i)].nTrips-1; k>=0; k--)
			{
				if (BufTime <= T) break;
				BufTime -= segs_to_check[TPMOD(i)].C[k].t;  
			}
			if (BufTime <= T || (T==0.0 && BufTime<1e-6)) break;
		}

		if (i==-1)
		{
			SetAbort();
			KMotionDLL->DoErrMsg("Invalid buffer times in Trajectry Planner");
			return 1;
		}

		m_realtime_Sequence_number = segs_to_check[TPMOD(i)].sequence_number;
		m_realtime_Sequence_number_valid = true;
	}
	return 0;
}


// download any "Done" segments
//
// "Done" segments are those that are already max'ed out based on constraints
// other than the length of the vector, so adding/considering more vectors will
// not affect them.

int CCoordMotion::DownloadDoneSegments()
{
	int ispecial_cmd=0;

	if (m_Simulate) return 0;  // exit if we are simulating

	if (nsegs > m_nsegs_downloaded)
	{
		// check if we have at least one more that is "done"

		if (GetSegPtr(m_nsegs_downloaded)->Done)
		{
			// yes, we have at least one to download

			if (m_nsegs_downloaded==0)
			{
				// if the first one, wait for the buffer and open it

				if (WaitForSegmentsFinished()) {SetAbort(); return 1;}

				if(KMotionDLL->WriteLine("OpenBuf")) {SetAbort(); return 1;}
				m_TimeAlreadyExecuted=0;
				ClearWriteLineBuffer();
				m_SegmentsStartedExecuting = false;

				// see if there are any special commands that need to be inserted before path starts
				if (DoSpecialInitialCommands()){SetAbort(); return 1;} ;
			}

			int iseg;
			for (iseg=m_nsegs_downloaded; iseg<nsegs && GetSegPtr(iseg)->Done; iseg++)
			{
				if (OutputSegment(iseg))   {SetAbort(); return 1;}  // Output the Segment
				// see if there are any special commands that need to be inserted after segs
				if (DoSpecialCommand(iseg)) {SetAbort(); return 1;} ;
			}
		}
	}
	return 0;
}


#define FLOAT_TOL 1e-6 


int CCoordMotion::SetAxisDefinitions(int x, int y, int z, int a, int b, int c)
{
	char s[MAX_LINE];
	sprintf(s, "DefineCS %d %d %d %d %d %d", x, y, z, a, b, c);
	if (KMotionDLL->WriteLine(s)) return 1;
	x_axis=x;
	y_axis=y;
	z_axis=z;
	a_axis=a;
	b_axis=b;
	c_axis=c;

	m_DefineCS_valid=true;
	return 0;
}
int CCoordMotion::SetAxisDefinitions(int x, int y, int z, int a, int b, int c, int u, int v)
{
	char s[MAX_LINE];
	sprintf(s, "DefineCSEx %d %d %d %d %d %d %d %d", x, y, z, a, b, c, u, v);
	if (KMotionDLL->WriteLine(s)) return 1;
	x_axis=x;
	y_axis=y;
	z_axis=z;
	a_axis=a;
	b_axis=b;
	c_axis=c;
	u_axis=u;
	v_axis=v;

	m_DefineCS_valid=true;
	return 0;
}

int CCoordMotion::GetAxisDefinitions(int *x, int *y, int *z, int *a, int *b, int *c)
{
	char response[MAX_LINE];
	int result;

	if (!m_DefineCS_valid)
	{
		if (KMotionDLL->WriteLineReadLine("DefineCS",response)) return 1;

		result=sscanf(response, "%d%d%d%d%d%d",&x_axis,&y_axis,&z_axis,&a_axis,&b_axis,&c_axis);
		if (result != 6) return 1;
	}

	*x = x_axis;
	*y = y_axis;
	*z = z_axis;
	*a = a_axis;
	*b = b_axis;
	*c = c_axis;

	return 0;
}

int CCoordMotion::GetAxisDefinitions(int *x, int *y, int *z, int *a, int *b, int *c, int *u, int *v)
{
	char response[MAX_LINE];
	int result;

	if (!m_DefineCS_valid)
	{
		if (KMotionDLL->WriteLineReadLine("DefineCSEx",response)) return 1;
		result = sscanf(response, "%d%d%d%d%d%d%d%d", &x_axis, &y_axis, &z_axis, &a_axis, &b_axis, &c_axis, &u_axis, &v_axis);
		if (result != 8) return 1;
	}

	*x = x_axis;
	*y = y_axis;
	*z = z_axis;
	*a = a_axis;
	*b = b_axis;
	*c = c_axis;
	*u = u_axis;
	*v = v_axis;

	return 0;
}

// Get motion profile settings for all Axes in the Coordinated Motion System

int CCoordMotion::GetRapidSettings()
{
	if (!RapidParamsDirty) return 0;

	MOTION_PARAMS *MP=&Kinematics->m_MotionParams;

	// if we are simulating then check if a board is present
	// if it is then upload the settings 

	if (m_Simulate)
	{
		int result=KMotionDLL->WaitToken(false,100,"GetRapidSettings");
		if (result != KMOTION_LOCKED)
		{
			RapidParamsDirty=false;
			return 0;  // just exit
		}
		KMotionDLL->ReleaseToken();
	}


	if (GetAxisDefinitions(&x_axis,&y_axis,&z_axis,&a_axis, &b_axis, &c_axis, &u_axis, &v_axis)) {SetAbort(); return 1;}

	KMotionDLL->WaitToken("GetRapidSettings2");  // lock the Token while we get all the parameters

	if (GetRapidSettingsAxis(x_axis,&MP->MaxRapidVelX,
									&MP->MaxRapidAccelX,
									&MP->MaxRapidJerkX,
									&MP->SoftLimitPosX,
									&MP->SoftLimitNegX,
									MP->CountsPerInchX))
	{
		KMotionDLL->ReleaseToken();
		m_Abort=true;
		return 1;
	}

	if (GetRapidSettingsAxis(y_axis,&MP->MaxRapidVelY,
									&MP->MaxRapidAccelY,
									&MP->MaxRapidJerkY,
									&MP->SoftLimitPosY,
									&MP->SoftLimitNegY,
									MP->CountsPerInchY))
	{
		KMotionDLL->ReleaseToken();
		m_Abort=true;
		return 1;
	}

	if (GetRapidSettingsAxis(z_axis,&MP->MaxRapidVelZ,
									&MP->MaxRapidAccelZ,
									&MP->MaxRapidJerkZ,
									&MP->SoftLimitPosZ,
									&MP->SoftLimitNegZ,
									MP->CountsPerInchZ))
	{
		KMotionDLL->ReleaseToken();
		m_Abort=true;
		return 1;
	}

	if (GetRapidSettingsAxis(a_axis,&MP->MaxRapidVelA,
									&MP->MaxRapidAccelA,
									&MP->MaxRapidJerkA,
									&MP->SoftLimitPosA,
									&MP->SoftLimitNegA,
									MP->CountsPerInchA))
	{
		KMotionDLL->ReleaseToken();
		m_Abort=true;
		return 1;
	}

	if (GetRapidSettingsAxis(b_axis,&MP->MaxRapidVelB,
									&MP->MaxRapidAccelB,
									&MP->MaxRapidJerkB,
									&MP->SoftLimitPosB,
									&MP->SoftLimitNegB,
									MP->CountsPerInchB))
	{
		KMotionDLL->ReleaseToken();
		m_Abort=true;
		return 1;
	}

	if (GetRapidSettingsAxis(c_axis,&MP->MaxRapidVelC,
									&MP->MaxRapidAccelC,
									&MP->MaxRapidJerkC,
									&MP->SoftLimitPosC,
									&MP->SoftLimitNegC,
									MP->CountsPerInchC))
	{
		KMotionDLL->ReleaseToken();
		m_Abort=true;
		return 1;
	}

	if (GetRapidSettingsAxis(u_axis, &MP->MaxRapidVelU,
		&MP->MaxRapidAccelU,
		&MP->MaxRapidJerkU,
		&MP->SoftLimitPosU,
		&MP->SoftLimitNegU,
		MP->CountsPerInchU))
	{
		KMotionDLL->ReleaseToken();
		m_Abort = true;
		return 1;
	}

	if (GetRapidSettingsAxis(v_axis, &MP->MaxRapidVelV,
		&MP->MaxRapidAccelV,
		&MP->MaxRapidJerkV,
		&MP->SoftLimitPosV,
		&MP->SoftLimitNegV,
		MP->CountsPerInchV))
	{
		KMotionDLL->ReleaseToken();
		m_Abort = true;
		return 1;
	}


	KMotionDLL->ReleaseToken();

	if (Kinematics->GetSoftLimits(&MP->SoftLimitNegX, &MP->SoftLimitPosX, &MP->SoftLimitNegY, &MP->SoftLimitPosY, &MP->SoftLimitNegZ, &MP->SoftLimitPosZ,
		&MP->SoftLimitNegA, &MP->SoftLimitPosA, &MP->SoftLimitNegB, &MP->SoftLimitPosB, &MP->SoftLimitNegC, &MP->SoftLimitPosC,
		&MP->SoftLimitNegU, &MP->SoftLimitPosU, &MP->SoftLimitNegV, &MP->SoftLimitPosV)) return 1;

	RapidParamsDirty=false;
	return 0;
}

// Get motion profile settings for a single Axis if included in the Coordinated Motion System

int CCoordMotion::GetRapidSettingsAxis(int axis,double *Vel,double *Accel,double *Jerk, double *SoftLimitPos, double *SoftLimitNeg, double CountsPerInch)
{
	char s[64];
	char response[MAX_LINE];
	int result;
	double temp;

	if (CountsPerInch==0.0) return 1;

	if (axis==-1) return 0;
	sprintf(s, "Vel%d;Accel%d;Jerk%d;SoftLimitPos%d;SoftLimitNeg%d",axis,axis,axis,axis,axis);
	if (KMotionDLL->WriteLine(s)) return 1;

	if (KMotionDLL->ReadLineTimeOut(response)) return 1;
	result=sscanf(response, "%lf",&temp);
	if (result != 1) return 1;
	*Vel = fabs(temp/CountsPerInch);

	if (KMotionDLL->ReadLineTimeOut(response)) return 1;
	result=sscanf(response, "%lf",&temp);
	if (result != 1) return 1;
	*Accel = fabs(temp/CountsPerInch);

	if (KMotionDLL->ReadLineTimeOut(response)) return 1;
	result=sscanf(response, "%lf",&temp);
	if (result != 1) return 1;
	*Jerk = fabs(temp/CountsPerInch);

	if (KMotionDLL->ReadLineTimeOut(response)) return 1;
	result=sscanf(response, "%lf",&temp);
	if (result != 1) return 1;
	if (CountsPerInch >= 0.0)
		*SoftLimitPos = temp / CountsPerInch;
	else
		*SoftLimitNeg = temp / CountsPerInch;

	if (KMotionDLL->ReadLineTimeOut(response)) return 1;
	result=sscanf(response, "%lf",&temp);
	if (result != 1) return 1;
	if (CountsPerInch >= 0.0)
		*SoftLimitNeg = temp / CountsPerInch;
	else
		*SoftLimitPos = temp / CountsPerInch;
	return 0;
}

int CCoordMotion::ReadCurAbsPosition(double *x, double *y, double *z, double *a, double *b, double *c, bool snap, bool NoGeo)
{
	double dummyu, dummyv;
	return ReadCurAbsPosition(x, y, z, a, b, c, &dummyu, &dummyv, snap, NoGeo);
}


int CCoordMotion::ReadCurAbsPosition(double *x, double *y, double *z, double *a, double *b, double *c, double *u, double *v, bool snap,  bool NoGeo)
{
	double tx,ty,tz,ta,tb,tc,tu,tv;

	// find out which axis is which

	if (GetAxisDefinitions(&x_axis,&y_axis,&z_axis,&a_axis,&b_axis,&c_axis,&u_axis,&v_axis)) {SetAbort(); return 1;}

	// read and set all axis (if undefined return interpreter)

	double Acts[MAX_ACTUATORS];

	for (int i=0; i<MAX_ACTUATORS; i++) Acts[i]=0.0;

	if (x_axis >=0)	if (GetDestination(x_axis,&Acts[0])) {SetAbort(); return 1;}
	if (y_axis >=0)	if (GetDestination(y_axis,&Acts[1])) {SetAbort(); return 1;}
	if (z_axis >=0)	if (GetDestination(z_axis,&Acts[2])) {SetAbort(); return 1;}
	if (a_axis >=0)	if (GetDestination(a_axis,&Acts[3])) {SetAbort(); return 1;}
	if (b_axis >=0)	if (GetDestination(b_axis,&Acts[4])) {SetAbort(); return 1;}
	if (c_axis >=0)	if (GetDestination(c_axis,&Acts[5])) {SetAbort(); return 1;}
	if (u_axis >=0)	if (GetDestination(u_axis,&Acts[6])) {SetAbort(); return 1;}
	if (v_axis >=0)	if (GetDestination(v_axis,&Acts[7])) {SetAbort(); return 1;}

	Kinematics->TransformActuatorstoCAD(Acts,&tx,&ty,&tz,&ta,&tb,&tc,&tu,&tv, NoGeo);

	// if the measured positions are really close to the interpreter positions
	// then there was probably some slight roundoff so set them exactly equal
	// to the last commanded position

	double tolx = fabs(FLOAT_TOL * tx);
	double toly = fabs(FLOAT_TOL * ty);
	double tolz = fabs(FLOAT_TOL * tz);

#define TOL_MIN 1e-6 // set the tolerance to at least a ui

	if (tolx < TOL_MIN) tolx = TOL_MIN;
	if (toly < TOL_MIN) toly = TOL_MIN;
	if (tolz < TOL_MIN) tolz = TOL_MIN;

	if (x_axis < 0 || (snap && fabs(tx - current_x) < tolx)) *x = current_x; else *x = tx;
	if (y_axis < 0 || (snap && fabs(ty - current_y) < toly)) *y = current_y; else *y = ty;
	if (z_axis < 0 || (snap && fabs(tz - current_z) < tolz)) *z = current_z; else *z = tz;
	if (a_axis < 0 || (snap && fabs(ta - current_a) < fabs(FLOAT_TOL * ta))) *a = current_a; else *a = ta;
	if (b_axis < 0 || (snap && fabs(tb - current_b) < fabs(FLOAT_TOL * tb))) *b = current_b; else *b = tb;
	if (c_axis < 0 || (snap && fabs(tc - current_c) < fabs(FLOAT_TOL * tc))) *c = current_c; else *c = tc;
	if (u_axis < 0 || (snap && fabs(tu - current_u) < fabs(FLOAT_TOL * tu))) *u = current_u; else *u = tu;
	if (v_axis < 0 || (snap && fabs(tv - current_v) < fabs(FLOAT_TOL * tv))) *v = current_v; else *v = tv;
	return 0;    
}





int CCoordMotion::GetDestination(int axis, double *d)
{
	int result;
	char cmd[16];
	char response[MAX_LINE];

	*d=0.0;

	if (axis==-1) return 0;  // not used in coordinate system 
	
	if (axis<0 || axis>N_CHANNELS) {SetAbort(); return 1;} // invalid

	sprintf(cmd, "Dest%d",axis);
	if (KMotionDLL->WriteLineReadLine(cmd,response)) {SetAbort(); return 1;}

	result=sscanf(response, "%lf",d);
	if (result != 1) {SetAbort(); return 1;}

	return 0;
}

int CCoordMotion::GetPosition(int axis, double *d)
{
	int result;
	char cmd[16];
	char response[MAX_LINE];

	*d=0.0;

	if (axis==-1) return 0;  // not used in coordinate system 
	
	if (axis<0 || axis>N_CHANNELS) {SetAbort(); return 1;} // invalid

	sprintf(cmd, "Pos%d",axis);
	if (KMotionDLL->WriteLineReadLine(cmd,response)) {SetAbort(); return 1;}

	result=sscanf(response, "%lf",d);
	if (result != 1) {SetAbort(); return 1;}

	return 0;
}

int CCoordMotion::GetAxisDone(int axis, int *r)
{
	int result;
	char cmd[16];
	char response[MAX_LINE];

	*r=-1;  // assume disabled

	if (axis==-1) return 0;  // not used in coordinate system 
	
	if (axis<0 || axis>N_CHANNELS) {SetAbort(); return 1;} // invalid

	sprintf(cmd, "CheckDone%d",axis);
	if (KMotionDLL->WriteLineReadLine(cmd,response)) {SetAbort(); return 1;}

	result=sscanf(response, "%d",r);
	if (result != 1) {SetAbort(); return 1;}

	return 0;
}

// Use a combination of Hardware and Software factors to 
// achieve the desired FRO.  
//
// Start at a SW=1.0 and HW=1.0 and gradually change one or the other
// to move the current FRO toward our goal.  Whenever the total FRO
// is above the HW Limit adjust the SW factor.  Whenever below the FRO
// then adjust the HW Limit. 

void CCoordMotion::DetermineSoftwareHardwareFRO(double &HW, double &SW)
{
	HW=1.0,SW=1.0;

	// check if current FRO is above the HW limit
	if (1.0 > m_HardwareFRORange)
	{
		// yes it is above, SW should be used
		
		if (1.0 <= m_FeedRateOverride)  // need to increase?
		{
			// yes, go all the way with SW
			SW = m_FeedRateOverride;
		}
		else
		{
			// need to decrease

			// check if decreasing all the way to desired FRO
			// will put us below the HW Limit 

			if (m_FeedRateOverride < m_HardwareFRORange)
			{
				// yes it will, decrease SW to limit
				SW = m_HardwareFRORange;
				// then remainder with HW
				HW = m_FeedRateOverride/SW;
			}
			else
			{
				// no, do all with SW
				SW = m_FeedRateOverride;
			}
		}
	}
	else
	{
		// Starting FRO=1.0 is below HW limit, HW should be used
		if (1.0 <= m_FeedRateOverride)  // need to increase (above 1.0)?
		{
			// yes, check if increasing all the way to desired FRO
			// will put us above the HW Limit? 
			if (m_FeedRateOverride > m_HardwareFRORange)
			{
				// yes it will, increase HW to limit
				HW = m_HardwareFRORange;
				// then remainder with SW
				SW = m_FeedRateOverride/HW;
			}
			else
			{
				// no we will still be below HW limit.  Use HW entirely
				HW = m_FeedRateOverride;
			}
		}
		else
		{
			// need to decrease, do all with HW
			HW = m_FeedRateOverride;
		}
	}
}


void CCoordMotion::SetFeedRateOverride(double v)
{
	char s[32];
	double HW,SW;

	m_FeedRateOverride=v;	
	
	if (!m_Simulate)
	{
		DetermineSoftwareHardwareFRO(HW,SW);
		sprintf(s, "SetFRO %.4f",HW);
		KMotionDLL->WriteLine(s);
	}
}

void CCoordMotion::SetFeedRateRapidOverride(double v)
{
	char s[32];

	if (v > Kinematics->m_MotionParams.MaxRapidFRO) v = Kinematics->m_MotionParams.MaxRapidFRO;

	m_FeedRateRapidOverride=v;	

	if (!m_Simulate)
	{
		sprintf(s, "SetRapidFRO %.4f",v);
		KMotionDLL->WriteLine(s);
	}
}

void CCoordMotion::SetHardwareFRORange(double v)
{
	m_HardwareFRORange=v;
}

double CCoordMotion::GetHardwareFRORange()
{
	return m_HardwareFRORange;
}

void CCoordMotion::SetSpindleRateOverride(double v)
{
	m_SpindleRateOverride=v;
}

double CCoordMotion::GetFeedRateOverride()
{
	return m_FeedRateOverride;
}

double CCoordMotion::GetFeedRateRapidOverride()
{
	return m_FeedRateRapidOverride;
}

double CCoordMotion::GetSpindleRateOverride()
{
	return m_SpindleRateOverride;
}

void CCoordMotion::SetStraightTraverseCallback(STRAIGHT_TRAVERSE_CALLBACK *p)
{
	m_StraightTraverseCallback=p;
}

void CCoordMotion::SetStraightTraverseCallback(STRAIGHT_TRAVERSE_SIX_AXIS_CALLBACK *p)
{
	m_StraightTraverseSixAxisCallback=p;
}

void CCoordMotion::SetStraightFeedCallback(STRAIGHT_FEED_CALLBACK *p)
{
	m_StraightFeedCallback=p;
}

void CCoordMotion::SetStraightFeedCallback(STRAIGHT_FEED_CALLBACK_SIX_AXIS *p)
{
	m_StraightFeedSixAxisCallback=p;
}

void CCoordMotion::SetArcFeedCallback(ARC_FEED_CALLBACK *p)
{
	m_ArcFeedCallback=p;
}

void CCoordMotion::SetArcFeedCallback(ARC_FEED_SIX_AXIS_CALLBACK *p)
{
	m_ArcFeedSixAxisCallback=p;
}

void CCoordMotion::SetAbort()
{
	m_Abort=true;
}

void CCoordMotion::SetTPParams()
{
	SetTrajectoryPlannerParams(&Kinematics->m_MotionParams);
}


void CCoordMotion::ClearAbort()
{
	if (m_Abort)
	{
		// If we had been aborted then
		// initialize the trajectory planner
		tp_init();
		DownloadInit();  // intialize download/look ahead variables  
		ClearWriteLineBuffer();
	}
	m_Abort=false;
	m_SegmentsStartedExecuting=false;
}

bool CCoordMotion::GetAbort()
{
	return m_Abort;
}


void CCoordMotion::SetHalt()
{
	m_Halt=true;
}

void CCoordMotion::ClearHalt()
{
	m_Halt=false;
}

bool CCoordMotion::GetHalt()
{
	return m_Halt;
}


int CCoordMotion::MeasurePointAppendToFile(const char *name)
{
	double x,y,z,a,b,c;
	static int row=0, col=0;
	int NRows,NCols,rr,cc;
	double GeoSpacingX,GeoSpacingY,GeoOffsetX,GeoOffsetY,X,Y;

	// first beg of file to see how many rows and cols
	// and how much data is there, if none then assume
	// starting over

	FILE *f = fopen(name,"rt");
	char message[100+MAX_PATH];
	if (!f)
	{
		sprintf(message, "Unable to open Geometric Correction File : %s",name);
		KMotionDLL->DoErrMsg(message);
		return 1;
	}

	int result = fscanf(f,"%d,%d",&NRows,&NCols);
		
	if (result != 2 || NRows < 2 || NRows > 1000 || NCols < 2 || NCols > 1000)
	{
		fclose(f);
		sprintf(message, "Invalid Geometric Correction File (NRows and NCols) : %s",name);
		KMotionDLL->DoErrMsg(message);
		return 1;
	}

	result = fscanf(f,"%lf,%lf",&GeoSpacingX,&GeoSpacingY);
		
	if (result != 2)
	{
		fclose(f);
		sprintf(message, "Invalid Geometric Correction File (GeoSpacingX and GeoSpacingY) : %s",name);
		KMotionDLL->DoErrMsg(message);
		return 1;
	}

	result = fscanf(f,"%lf,%lf",&GeoOffsetX,&GeoOffsetY);
		
	if (result != 2)
	{
		fclose(f);
		sprintf(message, "Invalid Geometric Correction File (GeoOffsetX and GeoOffsetY) : %s",name);
		KMotionDLL->DoErrMsg(message);
		return 1;
	}

	result = fscanf(f,"%d,%d,%lf,%lf",&rr,&cc,&X,&Y);

	if (result != 4) row=col=0;		// assume we are starting over

	fclose(f);

	double u, v;
	if (ReadCurAbsPosition(&x,&y,&z,&a, &b, &c, &u, &v)) return 1;
	
	f = fopen(name,"at");

	if (!f)
	{
		sprintf(message, "Unable to open Measurement Point File : %s",name);
		KMotionDLL->DoErrMsg(message);
		return 1;
	}

	fprintf(f,"%d,%d,%f,%f,%f\n",row,col,x,y,z);

	col++;

	if (col == NCols)
	{
		col = 0;
		row++;
	}

	fclose(f);

	return 0;
}


int CCoordMotion::PutWriteLineBuffer(const char *s, double Time)
{
	if (m_Abort) return 1;

	// new string won't fit, flush it first
	if (strlen(WriteLineBuffer) + strlen(s) > MAX_LINE-10)
	{
		if (FlushWriteLineBuffer()) return 1;
	}

	// put in the string
	if (WriteLineBuffer[0] != '\0') strcat(WriteLineBuffer,";");
	strcat(WriteLineBuffer,s);
	WriteLineBufferTime += Time;

	// If we have too much motion time in the buffer send it now
	// allocate 10% of the Lookahead to be buffered here
	if (WriteLineBufferTime > Kinematics->m_MotionParams.TPLookahead * 0.1)
	{
		if (FlushWriteLineBuffer()) return 1;
	}

	return 0;
}


int CCoordMotion::FlushWriteLineBuffer()
{
	if (m_Abort) return 1;

	//int Length = strlen(WriteLineBuffer);

	int result = KMotionDLL->WriteLine(WriteLineBuffer);
	ClearWriteLineBuffer();
	return result;
}

int CCoordMotion::ClearWriteLineBuffer()
{
	WriteLineBuffer[0]='\0';
	WriteLineBufferTime=0.0;
	return 0;
}

double CCoordMotion::FeedRateDistance(double dx, double dy, double dz, double da, double db, double dc, BOOL *PureAngle)
{
	return ::FeedRateDistance(dx, dy, dz, da, db, dc, 0.0, 0.0, &Kinematics->m_MotionParams, PureAngle);

}

double CCoordMotion::FeedRateDistance(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, BOOL *PureAngle)
{
	return ::FeedRateDistance(dx, dy, dz, da, db, dc, du, dv, &Kinematics->m_MotionParams, PureAngle);
}

int CCoordMotion::ConfigSpindle(int type, int axis, double UpdateTime, double Tau, double CountsPerRev)
{
	char s[128];
	
	sprintf(s, "ConfigSpindle %d %d %.6f %.6f %f",type, axis, UpdateTime, Tau, CountsPerRev);
	return KMotionDLL->WriteLine(s);
}


int CCoordMotion::GetSpindleRPS(float &speed)
{
	char response[MAX_LINE];

	if (KMotionDLL->WriteLineReadLine("GetSpindleRPS",response)) return 1;
	
	// check state
	if (sscanf(response,"%f",&speed)!=1) return 1;

	return 0;
}
