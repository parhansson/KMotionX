// CoordMotion.cpp: implementation of the CCoordMotion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CoordMotion.h"
#include "HiResTimer.h"
#include "TrajectoryPlanner3.h"   // 3rd Order (jerk limited) feed planner
#include <vector>
#include <stdarg.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


#ifdef DEBUG_DOWNLOAD
CString ds;
FILE *f=NULL;
CHiResTimer DTimer;


void PutString(CString s)
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
	m_LookaheadOverride=false;
	m_JogReleaseRequested=false;
	m_TP3DrawFailStreak=0;
	m_DefineCS_valid=false;
	m_DefineCS_known=false;
	m_TCP_affects_actuators = true;  // assume Tool Center Point has effects except for simple cases

	m_TapCycleInProgress = false;

	feed_override = true;	// whether feed override is enabled
	speed_override = true;  // whether spindle override is enabled

	m_NumLinearNotDrawn=0;

	// Save for everybody what directory we are installed in
#ifdef _KMOTIONX
	// The machine data directory (UTF-8) is both the main path and the root
	wcscpy_s(MainPath, MAX_PATH, kmx::strtowstr(kmx::getMachineDataPath()));
	wcscpy_s(MainPathRoot, MAX_PATH, MainPath);
#else
	CString Path;

	GetModuleFileName(GetModuleHandle(L"GCodeInterpreter.dll"),Path.GetBuffer(MAX_PATH),MAX_PATH);
	Path.ReleaseBuffer();

	Path.Replace(L"\"",L"");  // remove quotes
	Path.TrimRight();
	Path.TrimLeft();

	int LastSlash=Path.ReverseFind('\\');
	Path=Path.Left(LastSlash);

	// Check if we are running from a 64bit directory directory
	// if we are, then strip it off

	if (Path.Right(2).CompareNoCase(L"64") == 0)
	{
		Path = Path.Left(Path.GetLength() - 2);
	}

	// Check if we are running from the debug directory
	// if we are, then strip it off

	if (Path.Right(6).CompareNoCase(L"\\debug") == 0)
	{
		Path = Path.Left(Path.GetLength()-6);
	}

	// Check if we are running from the release directory
	// if we are, then strip it off

	if (Path.Right(8).CompareNoCase(L"\\release") == 0)
	{
		Path = Path.Left(Path.GetLength()-8);
	}

	// Now set the root install directory

	if (Path.Right(8).CompareNoCase(L"\\KMotion") == 0)
	{
		wcsncpy(MainPathRoot,Path.Left(Path.GetLength()-8),MAX_PATH);
	}

	wcsncpy(MainPath,Path,MAX_PATH);
#endif

	m_StraightTraverseCallback=NULL;
	m_StraightTraverseSixAxisCallback=NULL;
	m_StraightFeedCallback=NULL;
	m_StraightFeedSixAxisCallback=NULL;
	m_ArcFeedCallback=NULL;
	m_ArcFeedSixAxisCallback=NULL;

	DownloadInit();

	tp_init();
	TP3ClearRun();

	m_SegmentsStartedExecuting = m_Abort = m_Halt = false;

	m_PreviouslyStopped = m_Stopping = STOPPED_NONE;
	m_PreviouslyStoppedType = SEG_UNDEFINED;
	m_PreviouslyStoppedID = -1;
	m_TCP_affects_actuators = true;  // assume Tool Center Point has effects except for simple cases
	// check for a special Kinematics File

	FILE* f;
#ifdef _KMOTIONX
	_tfopen_s(&f, (CString)MainPath + L"/data/Kinematics.txt", _T("rt,ccs=UTF-8"));
#else
	_tfopen_s(&f, (CString)MainPath + L"\\Data\\Kinematics.txt", _T("rt,ccs=UTF-8"));
#endif

	if (f)
	{
		wchar_t s[81];
		CString sw;
		fgetws(s, 80, f);

		sw = s; // make CString

		// remove any newline or carriage return characters
		sw = sw.SpanExcluding(L"\r\n");


		// one exists, check if it is calling for Geppetto otherwise assume it is the 3Rod

		if (sw == L"5AxisTableAC")
			Kinematics = new CKinematics5AxisTableAC;
		else if (sw == L"Kinematics5AxisTableAB")
			Kinematics = new CKinematics5AxisTableAB;
		else if (sw == L"5AxisTableBC")
			Kinematics = new CKinematics5AxisTableBC;
		else if (sw == L"Kinematics5AxisTableAGimbalB")
			Kinematics = new CKinematics5AxisTableAGimbalB;
		else if (sw == L"5AxisGimbalAB")
			Kinematics = new CKinematics5AxisGimbalAB;
		else if (sw == L"5AxisGimbalCB")
			Kinematics = new CKinematics5AxisGimbalCB;
		else if (sw == L"GeppettoExtruder")
			Kinematics = new CKinematicsGeppettoExtrude;
		else if (sw == L"Geppetto")
			Kinematics = new CKinematicsGeppetto;
		else if (sw == L"Scara")
			Kinematics = new CKinematicsScara;
		else if (sw == L"Kinematics2AxisRobot")
			Kinematics = new CKinematics2AxisRobot;
		else if (sw == L"3Link")
			Kinematics = new CKinematics3Link;
		else if (sw == L"Kinematics3Rod")
			Kinematics = new CKinematics3Rod;
		else
		{
			// warn - otherwise a misspelled name silently runs trivial linear kinematics
			MessageBoxW(NULL, KMotionDLL->Translate("Unrecognized Kinematics Type (using default linear) : ") + sw, L"KMotion", MB_ICONEXCLAMATION|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
			Kinematics = new CKinematics;
		}

		fclose(f);
	}
	else
	{
		m_TCP_affects_actuators = false;
		Kinematics = new CKinematics;
	}

	Kinematics->MainPath = MainPath;
	Kinematics->Initialize();

	RapidParamsDirty=true;  // Trajectory Params should be refreshed from KFLOP

	x_axis = y_axis = z_axis = a_axis = b_axis = c_axis = u_axis = v_axis = -1;  // set all as initially undefined

	#ifdef DEBUG_DOWNLOAD
	AfxMessageBox(L"Download Diag Enabled");
	#endif
}

CCoordMotion::~CCoordMotion()
{
	// release any live TP3 streaming planner, then the lazily allocated
	// feed-run state itself (TP3ClearRun deliberately keeps the object
	// for reuse between runs, so only the destructor frees it)
	TP3ClearRun();
	TP3DeleteFeed();

	if (Kinematics) delete Kinematics;
}

MOTION_PARAMS *CCoordMotion::GetMotionParams()
{
	return &Kinematics->m_MotionParams;
}

// check if arc goes outside of Limits.  Calculate as if arc is in xy plane but could be other planes
// Progress along theta from the start to each PI/2 peak position until the end.  If any of those
// are outside then flag as an error.

int CCoordMotion::CheckSoftLimitsArc(int plane, double XC, double YC, double Z0, double Z1,
	double a, double b, double c, double u, double v, BOOL DirIsCCW,
	double radius, double theta0, double dtheta, CString &errmsg)
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

		double xp = radius * cos(next_theta) + XC;
		double yp = radius * sin(next_theta) + YC;
		double den = end_theta - theta0;
		if (den == 0.0) continue;
		double zp = Z0+(next_theta-theta0)/den*(Z1-Z0);

		double x, y, z;
		if (plane == CANON_PLANE_XY)
		{
			x = xp;
			y = yp;
			z = zp;
		}
		else if (plane == CANON_PLANE_XZ) // actually ZX
		{
			z = xp;
			x = yp;
			y = zp;
		}
		else // YZX
		{
			y = xp;
			z = yp;
			x = zp;
		}


		if (CheckSoftLimits(x, y, z, a, b, c, u, v, errmsg)) return 1;

		if (DirIsCCW)
			quadrant++;
		else
			quadrant--;
	}
	while (next_theta != end_theta);
		
	return 0;
}

int CCoordMotion::CheckLimit(int axis, double Act, double SoftLimitPos, double SoftLimitNeg, wchar_t Name, CString &errmsg)
{
	if (axis >= 0)
	{
		if (Act > SoftLimitPos) { errmsg.Format(L"Actuator %8g Limit %8g %c+", Act, SoftLimitPos, Name); return 1; }
		if (Act < SoftLimitNeg) { errmsg.Format(L"Actuator %8g Limit %8g %c-", Act, SoftLimitNeg, Name); return 1; }
	}
	return 0;
}


int CCoordMotion::CheckSoftLimits(double x, double y, double z, double a, double b, double c, double u, double v, CString &errmsg)
{
	MOTION_PARAMS *MP = &Kinematics->m_MotionParams;

	double Acts[MAX_ACTUATORS];

	if (m_DisableSoftLimits) return 0;

	Kinematics->TransformCADtoActuators(x, y, z, a, b, c, u, v, Acts);

	if (CheckLimit(x_axis, Acts[0], MP->SoftLimitPosX, MP->SoftLimitNegX, 'X', errmsg)) return 1;
	if (CheckLimit(y_axis, Acts[1], MP->SoftLimitPosY, MP->SoftLimitNegY, 'Y', errmsg)) return 1;
	if (CheckLimit(z_axis, Acts[2], MP->SoftLimitPosZ, MP->SoftLimitNegZ, 'Z', errmsg)) return 1;
	if (CheckLimit(a_axis, Acts[3], MP->SoftLimitPosA, MP->SoftLimitNegA, 'A', errmsg)) return 1;
	if (CheckLimit(b_axis, Acts[4], MP->SoftLimitPosB, MP->SoftLimitNegB, 'B', errmsg)) return 1;
	if (CheckLimit(c_axis, Acts[5], MP->SoftLimitPosC, MP->SoftLimitNegC, 'C', errmsg)) return 1;
	if (CheckLimit(u_axis, Acts[6], MP->SoftLimitPosU, MP->SoftLimitNegU, 'U', errmsg)) return 1;
	if (CheckLimit(v_axis, Acts[7], MP->SoftLimitPosV, MP->SoftLimitNegV, 'V', errmsg)) return 1;
	return 0;
}

int CCoordMotion::StraightTraverse(double x, double y, double z, double a, double b, double c, bool NoCallback, int sequence_number, int ID)
{
	return StraightTraverse(x, y, z, a, b, c, current_u, current_v, NoCallback, sequence_number, ID);
}

// Direction to proportion an independent rapid's per-axis rapid V/A/J by.
// The download transforms the rapid's two CAD endpoints and the controller
// interpolates LINEARLY IN ACTUATOR SPACE between them, so actuator i moves
// dAct_i over the segment's path length dCAD.  MaxRapidVel/Accel/Jerk_i are
// the controller's own per-axis values in counts / CountsPerInch_i
// (GetRapidSettingsAxis), so proportioning them along the actuator chord
// e_i = dAct_i / CountsPerInch_i keeps every actuator within the
// controller's limits.  dir (in: the CAD delta) is replaced by e, and the
// return value converts a rate/accel/jerk computed along e back to the
// segment's path metric: dCAD / FeedRateDistance(e).  On a nonlinear
// machine the rapid stays one stop-to-stop segment - straight in actuator
// space, curved in CAD - but no actuator exceeds its rapid V/A/J.
//
// Linear kinematics return 1.0 with dir untouched, so they proportion by
// the CAD direction exactly as always - including Geo and screw correction
// tables, which the base transform applies unconditionally.  The test is
// structural: every nonlinear Kinematics class limits segment lengths
// (MaxLinearLength) because its straight CAD lines curve in actuator
// space; the base class leaves it at "infinity" (1e30) and the settings
// never write it.
//
// inUse: actuator slots with a motor in the Coordinate System (see
// TP3SlotInUse).  Any other slot never had its rapid V/A/J read from the
// controller and must not constrain the rapid, even when the transform
// moves it (e.g. an orientation-hold slot with no motor).
static double RapidActuatorDirection(CKinematics *K, const bool *inUse,
	const double *P0, const double *P1, double dCAD, double *dir)
{
	MOTION_PARAMS *MP = &K->m_MotionParams;
	if (!(MP->MaxLinearLength < 1e20) || !(dCAD > 0.0)) return 1.0;

	double cpi[8] = {MP->CountsPerInchX, MP->CountsPerInchY, MP->CountsPerInchZ,
		MP->CountsPerInchA, MP->CountsPerInchB, MP->CountsPerInchC,
		MP->CountsPerInchU, MP->CountsPerInchV};
	double A0[MAX_ACTUATORS], A1[MAX_ACTUATORS], e[8];
	bool moves = false;
	int i;

	// the same (geo corrected) transform the download applies, so e is
	// exactly the chord the controller interpolates; on a failure keep the
	// CAD direction (the download aborts there)
	if (K->TransformCADtoActuators(P0[0], P0[1], P0[2], P0[3], P0[4], P0[5], P0[6], P0[7], A0) ||
		K->TransformCADtoActuators(P1[0], P1[1], P1[2], P1[3], P1[4], P1[5], P1[6], P1[7], A1))
		return 1.0;

	for (i = 0; i < 8; i++)
	{
		e[i] = (inUse[i] && cpi[i] != 0.0) ? (A1[i] - A0[i]) / cpi[i] : 0.0;
		if (!(fabs(e[i]) < 1e300)) return 1.0;   // NaN/inf from the transform
		if (e[i] != 0.0) moves = true;
	}
	if (!moves) return 1.0;

	BOOL pure_angle;
	double de = FeedRateDistance(e[0], e[1], e[2], e[3], e[4], e[5], e[6], e[7], MP, &pure_angle);
	if (!(de > 0.0)) return 1.0;

	for (i = 0; i < 8; i++) dir[i] = e[i];
	return dCAD / de;
}

int CCoordMotion::StraightTraverse(double x, double y, double z, double a, double b, double c, double u, double v, bool NoCallback, int sequence_number, int ID)
{
	double tdx, tdy, tdz, tda, tdb, tdc, tdu, tdv, rate, AccelToUse, JerkToUse;

	if (m_Abort) return 1;

	// 3rd Order TP: rapids ALWAYS continue the waypoint stream as
	// F=INFINITY moves (DoRapidsAsFeeds is implied and ignored) - no stop,
	// no combine commit, and no Beg/EndRapidBuf brackets (rapids blend
	// with feeds, and per-move special commands would flood the special
	// command table)
	if (Kinematics->m_MotionParams.ThirdOrderTP)
		return StraightFeedAccelRapid(1e99, 1e99, true, NoCallback, x, y, z, a, b, c, u, v, sequence_number, ID);

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
	CString errmsg;
	if (CheckSoftLimits(x,y,z,a,b,c,u,v,errmsg)) 
	{
		if (m_Simulate)
		{
			KMotionDLL->DoErrMsg((CString)"Soft Limit "+errmsg+" Rapid Traverse\r\r"
				"Soft Limits disabled for remainder of Simulation");
			m_DisableSoftLimits=true;
		}
		else
		{
			SetHalt();
			CheckMotionHalt(true);
			SetAbort();
			KMotionDLL->DoErrMsg((CString)"Soft Limit "+errmsg+" Rapid Traverse Job Halted");
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

	// a legacy stop-to-stop rapid follows: finish any pending TP3 run
	if (Kinematics->m_MotionParams.ThirdOrderTP)
		if (TP3FlushRun("independent-rapid")) return 1;

	if (m_StraightTraverseCallback && !NoCallback) m_StraightTraverseCallback(x,y,z,_setup.sequence_number);
	if (m_StraightTraverseSixAxisCallback && !NoCallback) m_StraightTraverseSixAxisCallback(x,y,z,a,b,c,_setup.sequence_number);

	if (d==0.0) return 0;  // ignore zero length moves


	if (!m_Simulate || m_DoTime) // skip if we are simulating
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

		// nonlinear kinematics: proportion along the ACTUATOR chord the
		// controller will execute (linear kinematics: unchanged CAD
		// direction and scale 1.0)
		double P0[8] = {current_x, current_y, current_z, current_a, current_b, current_c, current_u, current_v};
		double P1[8] = {x, y, z, a, b, c, u, v};
		double dir[8] = {tdx, tdy, tdz, tda, tdb, tdc, tdu, tdv};
		bool inUse[8];
		for (int i = 0; i < 8; i++) inUse[i] = TP3SlotInUse(i);
		double scale = RapidActuatorDirection(Kinematics, inUse, P0, P1, GetSegPtr(nsegs-1)->dx, dir);

		// limit Vel based on proportion in that direction
		if (Kinematics->MaxRapidRateInDirection(dir[0], dir[1], dir[2], dir[3], dir[4], dir[5], dir[6], dir[7], &rate)) return 1;

		// limit accel based on proportion in that direction
		if (Kinematics->MaxRapidAccelInDirection(dir[0], dir[1], dir[2], dir[3], dir[4], dir[5], dir[6], dir[7], &AccelToUse)) return 1;

		// limit Jerk based on proportion in that direction
		if (Kinematics->MaxRapidJerkInDirection(dir[0], dir[1], dir[2], dir[3], dir[4], dir[5], dir[6], dir[7], &JerkToUse)) return 1;

		SetSegmentVelAccelJerk(nsegs-1, rate*scale, AccelToUse*scale, JerkToUse*scale);
	
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

	// 3rd Order Planner: the arc starts at the current position, so any
	// staged straight chord (collinear combining) must reach the planner
	// first (the bisection below re-enters with the stager already empty;
	// a chord being committed never comes through here)
	if (MP->ThirdOrderTP && (!m_Simulate || m_DoTime))
		if (TP3FlushStager()) return 1;

	if (FeedRateToUse <= 0.0)
	{
		SetAbort();
		KMotionDLL->DoErrMsg(L"Arc Feed with Feed Rate Zero or Negative");
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

	// 3rd Order planner: while the (sub)arc still sweeps a rotary axis by
	// more than MaxAngularChange, DON'T convert to a chord yet even if the
	// chord error is inside CollinearTol.  StraightFeed would only
	// subdivide the CHORD - a straight CAD line whose interior points
	// leave the arc - and under nonlinear kinematics the chord seams
	// become periodic curvature dips in actuator space ("scalloped arc")
	// which the planner's curvature caps then chase as a ~Tw-wide
	// acceleration ripple at the chord rate.  Keep bisecting the ARC
	// instead so every waypoint lands ON the arc and the actuator path is
	// uniformly curved.  Same waypoint count either way - the angular
	// criterion mirrors StraightFeedAccelRapid's subdivision.
	bool RotaryCoarse = MP->ThirdOrderTP && (!m_Simulate || m_DoTime) &&
		((MP->DegreesA && fabs(a - current_a) > MP->MaxAngularChange) ||
		 (MP->DegreesB && fabs(b - current_b) > MP->MaxAngularChange) ||
		 (MP->DegreesC && fabs(c - current_c) > MP->MaxAngularChange));

	if (cord < MP->CollinearTol && !RotaryCoarse)
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

	if ((!m_Simulate || m_DoTime) && (d > MaxLength || MP->UseOnlyLinearSegments || MP->ArcsToSegs || MP->ThirdOrderTP || Kinematics->GeoTableValid))
	{
		if ((MP->ArcsToSegs || MP->UseOnlyLinearSegments || MP->ThirdOrderTP || Kinematics->GeoTableValid) && MP->CollinearTol==0.0)
		{
			MessageBoxW(NULL, KMotionDLL->Translate("Error Arcs To Segs selected with Zero Collinear Tolerance"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
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

	// a legacy whole-arc segment follows: finish any pending TP3 run
	if (Kinematics->m_MotionParams.ThirdOrderTP)
		if (TP3FlushRun("legacy-arc")) return 1;

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
	CString errmsg;

	SoftLimitResult = CheckSoftLimitsArc(plane, first_axis, second_axis, cur_third, axis_end_point,
					   a, b, c, u, v, rotation, radius, theta0, dtheta, errmsg);
	
	if (SoftLimitResult) 
	{
		if (m_Simulate)
		{
			KMotionDLL->DoErrMsg((CString)"Soft Limit "+errmsg+" Arc\r\r"
				"Soft Limits disabled for remainder of Simulation");
			m_DisableSoftLimits=true;
		}
		else
		{
			SetHalt();
			CheckMotionHalt(true);
			SetAbort();
			KMotionDLL->DoErrMsg((CString)"Soft Limit "+errmsg+" Arc Job Halted");
			return 1;
		}
	}




	if (!m_Simulate || m_DoTime)  // skip if we are simulating
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

// ===================== 3rd Order (TP3) feed path ==========================
// When MOTION_PARAMS.ThirdOrderTP is set, ALL motion (feeds, rapids as
// F=INFINITY, faceted arcs - already subdivided by MaxLinearLength /
// MaxAngularChange / CollinearTol) is collected as ACTUATOR-SPACE waypoints
// into one continuous STREAMING jerk-limited plan (rev 5 planner: bounded
// memory, incremental finalized emission).  Corners are faired incrementally
// with up to 1-TP3_FIR_MIN_FRAC of the local tolerance (the rest is the
// filter's).  Finalized C2 samples are converted on the
// fly into small "cubic knot" segments (straight actuator-space chords with
// a single precomputed cubic trip state) that the controller executes
// through the existing segment download unchanged.  Only true stops (dwell,
// independent rapid, legacy whole arc, execution stop) flush/end the stream.
//
// The planner's addWaypoint blocks (on the interpreter worker thread) when
// more than TP3_STREAM_BUFFER seconds are planned ahead of the estimated
// execution point; the estimate is wall-clock from first download (exact
// feedback via executed-time readback is a planned refinement) and aborts
// unblock it immediately.

#define TP3_PLAN_DT  0.001   // internal planning sample period (sec)
#define TP3_KNOT_DT  0.010   // max downloaded cubic knot period (sec)
#define TP3_KNOT_MIN_DT 0.005       // min knot period (download bandwidth
                                    // guard: caps the burst segment rate at
                                    // 1/TP3_KNOT_MIN_DT per second)
#define TP3_KNOT_SPLIT_MIN_DT 0.002 // Cubic Knots only: shortest period a
                                    // knot is split to when its cubic would
                                    // exceed an axis's Accel (see
                                    // TP3KnotOverAccel) - a few knots only
#define TP3_KNOT_MAX_DTHETA 0.01    // max direction change between successive
                                    // knot chords (rad, ~0.57 deg): curves are
                                    // polygonized by ANGLE, not by time, so the
                                    // transverse velocity kink per junction is
                                    // ~v*0.01 (vs 0.5-1 deg legacy facets).
                                    // LINEAR knot downloads only: the Cubic
                                    // Knots option (MP->CubicKnots, requires
                                    // updated Kogna firmware) sends per-axis
                                    // cubics instead - no chords, no angle
                                    // cap, no kinks, full TP3_KNOT_DT knots
// DIAGNOSTIC 2026-09-18: was 0.03.  Lowered 10x to tell "the executed path is
// not faired" apart from "the G-view cannot resolve the fairing": a blend is
// traversed in ~10-30ms, so at 0.03 NO interior blend point is ever drawn and
// the plot collapses toward one point per G-code block.  Revert to 0.03 once
// the question is settled.
#define TP3_DRAW_DT  0.003   // min plan-time spacing of G-view knot draws:
                             // each draw maps actuators back to CAD (a full
                             // kinematics inversion on nonlinear machines)
                             // and 3ms sample spacing is far denser than the
                             // viewer resolves.  Fixed-TIME spacing is self-
                             // regulating geometrically - the planner slows
                             // on curvature, so draws get closer together
                             // exactly where the path bends
#define TP3_DSFIXED  0.02    // COARSEST streaming arc-grid step (8D actuator
                             // units); must stay FIXED within a run for grid
                             // stability across streaming replans.  Grid
                             // size sets replan cost: planning must stay
                             // comfortably faster than real time or
                             // execution starves
// NOTE on grid vs tolerance: a blend smaller than the grid step is
// invisible to the planner's curvature caps (the discretization
// sharpens the corner back up) and the corner then runs too fast,
// overshooting the tolerance (+57% measured at tol=0.02).  Resolving
// blends with a finer grid proved ~4x slower than real time on
// hardware, so instead the corner speed is enforced EXPLICITLY: the
// fairing clamps the blend waypoints' F to the kink-cap corner speed
// 8*tolF/(dDxyz*Tw) (see TP3EmitFairedVertex) - grid-independent
// tolerance at zero extra planning cost (validated -34..-49% of budget
// across tol 0.001..0.05 on the coarse grid).  That clamp and the
// half-tolerance split it was validated with are kept for sub-grid
// blends (only the facet clamp changed, 45% -> 49.9%); blends the grid
// resolves use max(that clamp, the window cap) in TP3ComputeBlend.
#define TP3_UPDATE_EVERY 8   // replan/emit once per this many waypoints
                             // (each update() replans the whole kept path)
#define TP3_FAIR_FRAC 1.0    // fraction of local tolerance the corner blend
                             // may spend at its apex (legacy RoundCorner
                             // spends all of CornerTol).  Was 0.5: half the
                             // tolerance was held back for the FIR filter's
                             // own corner cut, but measured on the 3Link
                             // (DynoMotionBig, tol 0.006, Tw 0.15, 446
                             // resolved corners) the filter actually adds
                             // only 0.02*tol median / 0.06*tol p90 beyond the
                             // geometric apex - the reserve went unused and
                             // the blends came out half legacy's length.
#define TP3_FIR_MIN_FRAC 0.25 // ...but the filter's share can never reach
                             // zero: every tolerance-denominated cap in the
                             // planner (kink caps, curvature caps, the blend
                             // crawl speed 8*tolF/(dDw*Tw)) scales with what
                             // is LEFT, so tolF=0 would stall the machine at
                             // every unclamped blend.  The apex budget is
                             // therefore min(TP3_FAIR_FRAC, 1-TP3_FIR_MIN_FRAC)
                             // of tol.  Under the old sharp-kink-only speed
                             // cap this split would have tripled the crawl
                             // through every unclamped blend (crawl =
                             // Tw*budget/tolF) and doubled the corner stops
                             // on DynoMotionBig (42 -> 88); the window cap
                             // in TP3ComputeBlend (resolved blends only) is
                             // what makes the larger apex budget affordable.
                             // Sub-grid blends keep the original 0.5 split.

// Near-reversal corner stop policy: the planner's kink-speed cap makes it
// CRAWL through a faired blend at ~8*tol/(|dDxyz|*Tw), which for a sharp
// corner with a tight tolerance is far slower (and no smoother at the
// apex) than simply stopping.  End the run with an exact stop at the
// vertex instead when the corner is nearly a reversal, or when the blend
// crawl speed would fall below a small fraction of the neighboring
// segment speeds.
#define TP3_CORNER_STOP_DD   1.98  // |d2-d1| threshold (~168 deg turn):
                                   // always stop - the Bezier blend
                                   // degenerates toward out-and-back
#define TP3_CORNER_STOP_FRAC 0.05  // stop when est. blend crawl speed
                                   // < 5% of the slower adjacent segment
#define TP3_CORNER_STOP_DDMIN 0.5  // ignore corners under ~29 deg

// Host-side timeline log: one timestamped line per streaming event
// (waypoints, throttle waits, ExecTime reads, knots, flushes, launch,
// download waits), flushed per line so it survives a hang.  Opened at the
// first streaming run when the 3rd Order planner is active AND the Tool
// Setup "Log" option (MP->LogSegments) is selected - the same switch that
// enables the segment log; all logging no-ops when the file is closed.
#ifdef _KMOTIONX
#define TP3_TIMELINE_FILE "/tmp/TP3_Timeline.log"
#else
#define TP3_TIMELINE_FILE "c:\\Temp\\TP3_Timeline.log"
#endif
#include <string>
static FILE *tp3Log = NULL;
static CHiResTimer tp3LogTimer;

// both log files live in c:\Temp which a fresh Windows install lacks -
// create it so the logs are never silently skipped
static void EnsureTempDir()
{
#ifndef _KMOTIONX
	CreateDirectoryA("c:\\Temp", NULL);   // no-op if it already exists
#endif
}

static void TP3LogOpen()
{
	if (tp3Log) fclose(tp3Log);
	EnsureTempDir();
	tp3Log = fopen(TP3_TIMELINE_FILE, "wt");
	tp3LogTimer.Start();
}

static void TP3LogClose()
{
	if (tp3Log) { fclose(tp3Log); tp3Log = NULL; }
}

static void TP3Log(const char *fmt, ...)
{
	if (!tp3Log) return;
	fprintf(tp3Log, "%10.4f ", tp3LogTimer.Elapsed_Seconds());
	va_list ap;
	va_start(ap, fmt);
	vfprintf(tp3Log, fmt, ap);
	va_end(ap);
	fputc('\n', tp3Log);
	fflush(tp3Log);
}

class CTP3FeedRun
{
public:
	TP3::StreamingTrajectoryPlanner *sp = NULL;

	// last two RAW waypoints (original tolerances) for incremental fairing
	TP3::Waypoint raw0, raw1;
	bool haveRaw0 = false, haveRaw1 = false;

	// finalized samples not yet converted to knots; buf[cur] is the next
	// knot's starting sample (buf[cur-1] kept for central differences)
	std::vector<TP3::TrajPoint> buf;
	int cur = 0;
	bool firstKnot = true;
	bool havePrevU = false;
	TP3::VecN prevU;

	CHiResTimer timer;         // wall clock since first knot download
	bool started = false;
	int sinceUpdate = 0;       // waypoints since the last update()
	double sinceUpdateT = 0;   // approx path time since the last update()
	bool warnedUnderflow = false;
	double lastExec = 0;       // last ExecTime read from the controller
	double lastExecT = -1;     // wall time of that read (throttles queries)
	bool warnedStall = false;  // reported frozen-ExecTime throttle stall
	TP3::Limits lim;           // limits of the active run (for corner policy)
	double Tw = 0;             // FIR filter span 3*max(Aeff/Jerk) of the run
	                           // (TP3::filterRatio - effective Accel)
	double execBase = 0;       // program motion time downloaded BEFORE this
	                           // run: ExecTime is program-cumulative but the
	                           // planner's written time is per-run, so all
	                           // throttle/canary math uses (exec - execBase)
	int firstSeg = -1;         // segment index of this run's first knot
	                           // (nsegs at run creation - every earlier
	                           // segment belongs to prior runs/flushes)
	bool baseFinal = false;    // execBase captured at the moment downloads
	                           // reached firstSeg: exact even when a prior
	                           // flush's tail was still inserted-but-
	                           // undownloaded at run creation (in-order
	                           // capped downloads), whose time must land on
	                           // the pre-run side of the baseline
	bool sawExec = false;      // a nonnegative ExecTime has been read
	bool inRapid = false;      // Beg/EndRapidBuf bracket state of the knot
	                           // stream (controller RFRO vs FRO selection)
	double lastDrawT = -1e9;   // plan time of the last G-view knot draw
	int lastDrawSeq = -1;      // its sequence number (force a draw per line)

	// (kept here rather than in CCoordMotion so the exported class layout
	// is unchanged - only the DLL needs rebuilding)
	bool staging = false;      // TP3FlushStager is feeding a combined chord
	                           // back through StraightFeedAccelRapid
	bool knotEmit = false;     // TP3ConsumeSamples issuing its own rapid
	                           // brackets (attach to the knot, never hold)

	// collinear stager (TP3StageMove): the open chord of interpreter block
	// endpoints not yet fed to the planner, CAD units
	bool stOpen = false;
	double stA[8], stC[8];     // chord start (= current_* while open) and end
	std::vector<double> stE;   // eliminated interior points, 8 doubles each
	int stN = 0;
	double stF = 0, stAccel = 0;
	bool stRapid = false;
	int stSeq = 0, stId = 0;
	double stLen = 0, stMaxLen = 0;

	// buffered controller commands issued while the run is open: held here
	// (cmdIssued so far) and attached to the knot that comes from the
	// waypoint they preceded (cmdEmitted attached so far)
	std::vector<std::string> cmdPending;
	std::vector<int> cmdPendingSeq;
	int cmdIssued = 0, cmdEmitted = 0;
};

// defined below the class so delete runs the REAL destructor (deleting an
// incomplete type would skip it and leak the vector's debug proxy)
void CCoordMotion::TP3DeleteFeed()
{
	delete m_TP3Feed;
	m_TP3Feed = NULL;
}

static double TP3Dist8(const TP3::VecN &a, const TP3::VecN &b)
{
	double s = 0;
	for (int i = 0; i < TP3::NAX; i++)
	{
		double d = a[i] - b[i];
		s += d * d;
	}
	return sqrt(s);
}

// Add a waypoint to the streaming planner, waiting for buffer space OUT-
// SIDE the planner's own (invisible) throttle so the wait shows in the
// timeline log, and so a controller that stops consuming (ExecTime frozen
// while we are blocked on it) is REPORTED and the throttle broken --
// downloads then continue, which directly probes whether the controller
// resumes from its data-end feedhold when data arrives, instead of both
// sides deadlocking silently.  canAddWaypoint() polls ExecTime (cached
// 100ms) as a side effect, so lastExec stays fresh while waiting.
static void TP3AddWaypointThrottled(CCoordMotion *CM, CTP3FeedRun *r,
	const TP3::Waypoint &w, const char *tag)
{
	// Never throttle BEFORE the coordinated buffer has launched: the
	// planner's write throttle (time WRITTEN vs executed) and the download
	// auto-launch (time DOWNLOADED > TPLookahead) measure slightly
	// different quantities, and a run whose written content lands just
	// over the throttle bound while its downloads quantize to exactly the
	// launch threshold DEADLOCKS - the feed pauses, downloads stop
	// growing, the launch never fires, and ExecTime stays frozen at 0
	// until the stall watchdog breaks the throttle (observed: written
	// 1.082, downloaded exactly 1.000, TPLookahead 1.0, on slow near-fold
	// 3Link content).  Pre-launch nothing is consuming, so throttling can
	// only delay the launch it is waiting for; the planner's buffers are
	// growable (canAddWaypoint is pure pacing, not a capacity guard).
	if (!CM->CoordLaunched() && !CM->m_Simulate)
	{
		r->sp->addWaypointNoWait(w);
		return;
	}

	if (!r->sp->canAddWaypoint() && !r->warnedStall)
	{
		TP3Log("THROTTLE_WAIT  %-10s written=%8.3f exec=%8.3f", tag,
			r->sp->getTotalTimeWritten(), r->lastExec);

		CHiResTimer wt;
		wt.Start();
		double lastE = r->lastExec, lastAdvance = 0;
		while (!r->sp->canAddWaypoint() && !CM->GetAbort())
		{
			// keep the download pump running while write-throttled: an
			// emission burst can insert more Done knots than one capped
			// download call moves, and with writes paused nothing else
			// drains them - the controller would consume to the download
			// frontier and data-end feedhold while the host waited on a
			// frozen ExecTime, a self-starvation only the stall watchdog
			// below could break (with a false controller-stuck report and
			// pacing latched off).  Draining here keeps the controller
			// topped up to the DLWAIT pacing bound instead, so exec keeps
			// advancing and the throttle releases naturally.  Failures
			// (and Halt) SetAbort internally - the loop condition exits.
			if (CM->DownloadDoneSegments()) break;

			// a ran-dry re-arm (negative ExecTime with no stop state)
			// cleared the launch flag: nothing is consuming until the
			// auto-relaunch, so revert to the pre-launch rule above -
			// throttling now could only delay the launch it waits for
			if (!CM->CoordLaunched()) break;

			Sleep(1);
			double el = wt.Elapsed_Seconds();
			if (r->lastExec != lastE)
			{
				lastE = r->lastExec;
				lastAdvance = el;
			}
			else if (el - lastAdvance > 2.0)
			{
				TP3Log("THROTTLE_STALL %-10s exec frozen at %8.3f for %.1fs written=%8.3f",
					tag, lastE, el - lastAdvance, r->sp->getTotalTimeWritten());
				r->warnedStall = true;
				CString s;
				s.Format(L"3rd Order Planner: controller stopped consuming (ExecTime frozen at\r\n"
					L"%.3f sec for over 2 sec) while the host was throttled with %.3f sec of\r\n"
					L"trajectory written.  If this was not a user feedhold, the controller is\r\n"
					L"stuck in a data-end feedhold.  Downloading will continue without\r\n"
					L"throttling.  With the Trajectory Planner Log option selected the event\r\n"
					L"timeline is recorded in c:\\Temp\\TP3_Timeline.log.",
					lastE, r->sp->getTotalTimeWritten());
				CM->KMotionDLL->DoErrMsg(s);
				break;
			}
		}
		TP3Log("THROTTLE_RESUME %-9s waited=%7.3f exec=%8.3f", tag,
			wt.Elapsed_Seconds(), r->lastExec);
	}
	r->sp->addWaypointNoWait(w);
}

void CCoordMotion::TP3ClearRun()
{
	if (m_TP3Feed)
	{
		// detach-then-delete: TP3ClearRun can be reached from more than
		// one thread (a streaming-jog worker's teardown vs a GUI action's
		// ClearAbort) - the atomic exchange guarantees only ONE caller
		// ever deletes the planner object
		TP3::StreamingTrajectoryPlanner *sp = (TP3::StreamingTrajectoryPlanner *)
			InterlockedExchangePointer((void **)&m_TP3Feed->sp, NULL);
		if (sp)
		{
			TP3Log("RUN_END        written=%8.3f exec=%8.3f waypoints=%d",
				sp->getTotalTimeWritten(), m_TP3Feed->lastExec,
				(int)sp->getTotalWaypointCount());
			delete sp;
		}
		m_TP3Feed->haveRaw0 = m_TP3Feed->haveRaw1 = false;
		m_TP3Feed->buf.clear();
		m_TP3Feed->cur = 0;
		m_TP3Feed->firstKnot = true;
		m_TP3Feed->havePrevU = false;
		m_TP3Feed->started = false;
		m_TP3Feed->sinceUpdate = 0;
		m_TP3Feed->sinceUpdateT = 0;
		m_TP3Feed->warnedUnderflow = false;
		m_TP3Feed->lastExec = 0;
		m_TP3Feed->lastExecT = -1;
		m_TP3Feed->warnedStall = false;
		m_TP3Feed->execBase = 0;
		m_TP3Feed->firstSeg = -1;
		m_TP3Feed->baseFinal = false;
		m_TP3Feed->sawExec = false;
		m_TP3Feed->inRapid = false;
		// a staged chord and held commands belong to the run being dropped
		// (a normal flush commits both before it gets here)
		m_TP3Feed->stOpen = false;
		m_TP3Feed->stE.clear();
		m_TP3Feed->stN = 0;
		// (staging / knotEmit are CALL-SCOPED flags owned by TP3FlushStager
		// and TP3ConsumeSamples - never reset here: a corner stop in the
		// middle of committing a chord runs this function, and clearing
		// 'staging' then sent the rest of that same chord's subdivided
		// pieces back into the stager, losing the end of the move)
		m_TP3Feed->cmdPending.clear();
		m_TP3Feed->cmdPendingSeq.clear();
		m_TP3Feed->cmdIssued = m_TP3Feed->cmdEmitted = 0;
	}
	m_TP3DrawHave = 0;   // abort/reset paths discard the drawing lag too
	m_TipWgtValid = false; // re-probe Jacobian weights (tool may change)
}

// Convert newly finalized samples into cubic knot segments and download
// them.  Non-final calls keep one sample of lookahead (for the central
// difference boundary speed) and the sample behind the frontier; a final
// call consumes everything with the run ending at rest.
// Per-axis Hermite cubic of time through the knot boundary samples k0, k1
// (TRAJECTORY_CUBIC8): position = ((K[4i]*t + K[4i+1])*t + K[4i+2])*t +
// K[4i+3] hits both boundary samples and both central-difference boundary
// velocities exactly (zero at the run's rest start, and at the rest end
// when restEnd)
static void TP3KnotHermite(const CTP3FeedRun *r, int k0, int k1, bool restEnd, double *K)
{
	double T = (k1-k0) * TP3_PLAN_DT;
	for (int i = 0; i < 8; i++)
	{
		double p0 = r->buf[k0].p[i], p1 = r->buf[k1].p[i];
		double w0 = r->firstKnot ? 0.0
			: (r->buf[k0+1].p[i] - r->buf[k0-1].p[i])/(2.0*TP3_PLAN_DT);
		double w1 = restEnd ? 0.0
			: (r->buf[k1+1].p[i] - r->buf[k1-1].p[i])/(2.0*TP3_PLAN_DT);
		K[4*i+0] = ((w0 + w1)*T - 2.0*(p1 - p0))/(T*T*T);
		K[4*i+1] = (3.0*(p1 - p0) - (2.0*w0 + w1)*T)/(T*T);
		K[4*i+2] = w0;
		K[4*i+3] = p0;
	}
}

// True when the Hermite knot k0..k1 would command more than an axis's
// Accel.  A cubic has a single jerk per knot, so when the planner's
// Accel/Jerk time is shorter than the knot the planned jerk swings inside
// one knot and the fit misses the planned acceleration at the knot ends
// (measured +8..14% of Accel with 10ms knots at Jerk/Accel = 150/s, where
// the planned samples themselves stayed within Accel).  The knot's
// acceleration is linear in time, so its two end values ARE its extremes
// and the check is exact.  0.1% slack keeps rounding on a ramp planned
// exactly at the limit from splitting.
static bool TP3KnotOverAccel(const CTP3FeedRun *r, int k0, int k1, bool restEnd)
{
	double K[32], T = (k1-k0) * TP3_PLAN_DT;
	TP3KnotHermite(r, k0, k1, restEnd, K);
	for (int i = 0; i < 8; i++)
	{
		double amax = r->lim.ax.amax[i];
		if (!(amax > 0.0)) continue;               // slot not in use
		double a0 = 2.0*K[4*i+1];
		double a1 = 6.0*K[4*i+0]*T + 2.0*K[4*i+1];
		if (fabs(a0) > 1.001*amax || fabs(a1) > 1.001*amax) return true;
	}
	return false;
}

static int TP3ConsumeSamples(CCoordMotion *CM, CTP3FeedRun *r,
	const std::vector<TP3::TrajPoint> &batch, bool final)
{
	int i;
	for (i = 0; i < (int)batch.size(); i++) r->buf.push_back(batch[i]);

	int Kmax = (int)(TP3_KNOT_DT/TP3_PLAN_DT + 0.5);
	if (Kmax < 1) Kmax = 1;
	int Kmin = (int)(TP3_KNOT_MIN_DT/TP3_PLAN_DT + 0.5);
	if (Kmin < 1) Kmin = 1;

	int N = (int)r->buf.size();
	int last = final ? N-1 : N-2;      // non-final: reserve one for lookahead
	bool inserted = false;

	// Per-axis cubic knots (TRAJECTORY_CUBIC8, requires updated Kogna
	// firmware): each axis gets its own Hermite cubic of time through the
	// knot boundary samples with central-difference boundary velocities,
	// so junctions are velocity continuous in EVERY axis and the chord
	// angle cap (the polygonization/faceting) disappears - knots run at
	// the full TP3_KNOT_DT everywhere (except the few a split shortens so
	// their cubic stays within Accel - see TP3KnotOverAccel).  Hermite fit
	// POSITION error to the interior samples is O(kappa*(v*T)^4/384),
	// orders below any real tolerance.
	bool cubic8 = CM->Kinematics->m_MotionParams.CubicKnots;

	while (r->cur < last && (final || last - r->cur >= Kmin))
	{
		int k0 = r->cur;
		int k1 = k0 + Kmax;
		if (k1 > last) k1 = last;

		// shrink the knot while its chord turns too far from the previous
		// one (linear knot downloads only: cubics have no chords to kink)
		while (!cubic8 && r->havePrevU && k1 - k0 > Kmin)
		{
			TP3::VecN uc = (r->buf[k1].p - r->buf[k0].p).normalized();
			if ((uc - r->prevU).norm() <= TP3_KNOT_MAX_DTHETA) break;
			int span = (k1 - k0 + 1) / 2;
			if (span < Kmin) span = Kmin;
			k1 = k0 + span;
			if (k1 > last) k1 = last;
		}

		// A buffered command's mark inside this span ENDS the knot at the
		// mark's sample: the command executes at the end of the knot it is
		// attached to, so it fires exactly at the executed position of the
		// point it was issued at (real time relative to the motion), never
		// up to a knot later.  A mark already present at k0 (after a
		// stationary span that inserted nothing) fires now, i.e. at the end
		// of whatever segment was inserted last.
		if (r->buf[k0].cmd > r->cmdEmitted)
		{
			TP3Log("BUFCMD_ATTACH  n=%d..%d t=%8.3f seq=%d (at rest)", r->cmdEmitted+1,
				r->buf[k0].cmd, r->buf[k0].t, r->buf[k0].seq);
			if (CM->TP3AttachPendingCmds(r->buf[k0].cmd)) {CM->SetAbort(); return 1;}
		}
		for (int k = k0 + 1; k < k1; k++)
			if (r->buf[k].cmd > r->cmdEmitted) { k1 = k; break; }

		// halve a Cubic8 knot whose cubic would exceed an axis's Accel
		// (TP3KnotOverAccel), down to TP3_KNOT_SPLIT_MIN_DT.  Only the
		// offending knots are split, so the download rate barely changes
		// (+6% knots on the 3Link program that exposed it)
		if (cubic8)
		{
			int kSplit = (int)(TP3_KNOT_SPLIT_MIN_DT/TP3_PLAN_DT + 0.5);
			if (kSplit < 1) kSplit = 1;
			while (k1 - k0 > kSplit &&
				TP3KnotOverAccel(r, k0, k1, final && k1 == N-1))
			{
				int span = (k1 - k0 + 1) / 2;
				if (span < kSplit) span = kSplit;
				k1 = k0 + span;
			}
		}

		double T = (k1-k0) * TP3_PLAN_DT;
		double L = TP3Dist8(r->buf[k1].p, r->buf[k0].p);
		double v0 = r->firstKnot ? 0.0
			: TP3Dist8(r->buf[k0+1].p, r->buf[k0-1].p)/(2.0*TP3_PLAN_DT);
		double v1 = (final && k1 == N-1) ? 0.0
			: TP3Dist8(r->buf[k1+1].p, r->buf[k1-1].p)/(2.0*TP3_PLAN_DT);

		if (L > 0.0 || v0 > 1e-9 || v1 > 1e-9)   // skip stationary spans
		{
			double P0[8], P1[8];
			for (i=0;i<8;i++) { P0[i]=r->buf[k0].p[i]; P1[i]=r->buf[k1].p[i]; }

			// bracket rapid portions of the knot stream so the
			// controller applies Rapid FRO independently of FRO
			// (F rides through the planner attributed at the filter
			// group delay; transitions are few - one pair per G0
			// region, not per facet).  NOTE: TP3 rapids BLEND, so the
			// brackets are crossed AT SPEED - requires the firmware
			// ramp fix in DSP_KOGNA SuperFast.c (Desired-only, no
			// TimeBase slam) to avoid a velocity step when RFRO != FRO.
			bool rapid = r->buf[k0].rapid;
			if (rapid != r->inRapid)
			{
				TP3Log("%s seq=%d", rapid ? "BEG_RAPID      " : "END_RAPID      ",
					r->buf[k0].seq);
				r->knotEmit = true;            // attach to the previous knot now
				int rc = CM->DoKMotionBufCmd(rapid ? "BegRapidBuf" : "EndRapidBuf",
						r->buf[k0].seq);
				r->knotEmit = false;
				if (rc) {CM->SetAbort(); return 1;}
				r->inRapid = rapid;
			}

			if (cubic8)
			{
				// per-axis Hermite: position=((a*t+b)*t+c)*t+d hits both
				// boundary samples and boundary velocities exactly
				double K[32];
				TP3KnotHermite(r, k0, k1, final && k1 == N-1, K);
				if (tp_insert_cubic8(K, T, L, (v0 > v1) ? v0 : v1,
						r->buf[k0].seq, r->buf[k0].id))
					{CM->SetAbort(); return 1;}
				inserted = true;
				TP3Log("KNOT8          t=%8.3f T=%.3f L=%8.5f v0=%8.4f v1=%8.4f seq=%d",
					r->buf[k0].t, T, L, v0, v1, r->buf[k0].seq);

				// draw at the interior samples too - a 10ms knot chord
				// would visibly re-facet the G-view on curves.  Decimated
				// by TP3_DRAW_DT (each draw is an inverse-kinematics
				// solve seeded from the sample's CAD anchor: right
				// branch, 1-2 iterations).  nocb samples (jogs) are
				// streamed but never drawn.
				for (int k = k0 + 3; !r->buf[k0].nocb && k < k1; k += 3)
				{
					if (r->buf[k].t - r->lastDrawT < TP3_DRAW_DT &&
						r->buf[k0].seq == r->lastDrawSeq) continue;
					double Pi[8];
					for (i=0;i<8;i++) Pi[i]=r->buf[k].p[i];
					CM->TP3DrawKnotCAD(Pi, r->buf[k].cad.c, r->buf[k0].F,
						r->buf[k0].rapid, r->buf[k0].seq, r->buf[k0].id);
					r->lastDrawT = r->buf[k].t;
					r->lastDrawSeq = r->buf[k0].seq;
				}
				if (!r->buf[k0].nocb &&
					((final && k1 == N-1) ||
					r->buf[k1].t - r->lastDrawT >= TP3_DRAW_DT ||
					r->buf[k0].seq != r->lastDrawSeq))
				{
					CM->TP3DrawKnotCAD(P1, r->buf[k1].cad.c, r->buf[k0].F,
						r->buf[k0].rapid, r->buf[k0].seq, r->buf[k0].id);
					r->lastDrawT = r->buf[k1].t;
					r->lastDrawSeq = r->buf[k0].seq;
				}
			}
			else
			{
				if (tp_insert_precomputed_cubic(P0, P1, L, T, v0, v1,
						r->buf[k0].seq, r->buf[k0].id))
					{CM->SetAbort(); return 1;}
				inserted = true;
				TP3Log("KNOT           t=%8.3f T=%.3f L=%8.5f v0=%8.4f v1=%8.4f seq=%d",
					r->buf[k0].t, T, L, v0, v1, r->buf[k0].seq);

				// draw the ACTUAL downloaded path (the knot chord endpoints
				// mapped back to CAD) - post-fairing, post-comp, post-filter,
				// exactly what the controller executes.  Decimated by
				// TP3_DRAW_DT (each draw is an inverse-kinematics solve
				// seeded from the sample's CAD anchor: right branch, 1-2
				// iterations).  nocb samples (jogs) are streamed but
				// never drawn.
				if (!r->buf[k0].nocb &&
					((final && k1 == N-1) ||
					r->buf[k1].t - r->lastDrawT >= TP3_DRAW_DT ||
					r->buf[k0].seq != r->lastDrawSeq))
				{
					CM->TP3DrawKnotCAD(P1, r->buf[k1].cad.c, r->buf[k0].F,
						r->buf[k0].rapid, r->buf[k0].seq, r->buf[k0].id);
					r->lastDrawT = r->buf[k1].t;
					r->lastDrawSeq = r->buf[k0].seq;
				}
			}

			// buffered commands issued before the waypoint this knot comes
			// from (the mark rides with the samples at the group delay):
			// the knot was cut to end at the mark's sample above, so
			// attaching here fires them exactly there
			if (r->buf[k1].cmd > r->cmdEmitted)
			{
				TP3Log("BUFCMD_ATTACH  n=%d..%d t=%8.3f seq=%d", r->cmdEmitted+1,
					r->buf[k1].cmd, r->buf[k1].t, r->buf[k0].seq);
				if (CM->TP3AttachPendingCmds(r->buf[k1].cmd)) {CM->SetAbort(); return 1;}
			}

			if (L > 0.0)
			{
				r->prevU = (r->buf[k1].p - r->buf[k0].p).normalized();
				r->havePrevU = true;
			}
		}
		r->cur = k1;
		r->firstKnot = false;
	}

	// trim consumed samples, keeping buf[cur-1] for the next central diff
	if (r->cur > 1)
	{
		r->buf.erase(r->buf.begin(), r->buf.begin() + (r->cur - 1));
		r->cur = 1;
	}

	if (inserted)
	{
		if (!r->started)
		{
			// (the run's exec baseline is no longer snapped here - a prior
			// flush's tail could still be inserted-but-undownloaded at this
			// point, and charging it to this run's clock over-counted
			// (exec - execBase).  OutputSegment captures execBase exactly
			// when downloads reach r->firstSeg.)
			r->timer.Start();
			r->started = true;
		}
		TP3Log("DOWNLOAD_BEGIN final=%d written=%8.3f exec=%8.3f",
			(int)final, r->sp->getTotalTimeWritten(), r->lastExec);
		if (CM->DownloadDoneSegments()) {CM->SetAbort(); return 1;}
		TP3Log("DOWNLOAD_END");
	}
	return 0;
}

// Feed one interior vertex through the tolerance-aware fairing (mirrors
// TP3::fairPath's per-vertex quadratic Bezier blend) into the streaming
// planner.  Raw interior vertices are REPLACED by their blend points; the
// planner-fed waypoints carry tol minus the apex offset the blend actually
// spent, so the total deviation from the raw path stays within the
// original tolerance.
// tip-referenced deviation of a direction change: per-axis components
// weighted by the tool-tip error per unit of that axis
static double TP3WNorm(const TP3::VecN &dD, const TP3::Waypoint &wa, const TP3::Waypoint &wb)
{
	double s = 0;
	for (int i = 0; i < TP3::NAX; i++)
	{
		double wg = (wa.wgt[i] > wb.wgt[i]) ? wa.wgt[i] : wb.wgt[i];
		double e = dD[i] * wg;
		s += e * e;
	}
	return sqrt(s);
}

// Geometry and tolerance accounting of the faired blend at vertex w0
// (wm1 -> w0 -> wp1), shared by the fairing itself and by the corner-stop
// policy so both see the same blend and the same corner speed.
//   L     blend half-length (actuator units): 4*budget/dDw with budget =
//         min(TP3_FAIR_FRAC, 1-TP3_FIR_MIN_FRAC) of the local tolerance
//         for a blend the grid resolves, half of it for a sub-grid blend,
//         clamped to 49.9% of each adjacent facet so neighboring blends
//         never overlap
//   apex  tip-referenced apex offset the blend ACTUALLY spends, L*dDw/4
//         (<= budget; well under it when the facets are short)
//   reserve  what the planner-fed waypoints give up from the local tol:
//         the apex actually spent when the blend is resolved by the
//         planning grid (2L >= dsFixed), else the fixed half-tolerance
//         budget a sub-grid blend is held to.  A
//         sub-grid blend is invisible to the planner's caps (its
//         corner speed lands ~2x the clamp) and its neighbors' filter cuts
//         superpose inside one window; the fixed reserve is what absorbs
//         that (measured: crediting there over-ran tol by up to 2x on
//         0.01-0.02 in CAM fillet facets), so the credit is only taken
//         where it is safe.
//   tolF  what is left for the FIR filter's own corner cut: tol - reserve.
//         Crediting the unspent fairing budget raises the kink-cap corner
//         speed 8*tolF/(dDw*Tw) wherever short-but-resolved facets clamp
//         the blend, with the total deviation still bounded by the
//         original tolerance.
struct TP3BlendGeom
{
	double L1, L2;             // adjacent facet lengths (0 = degenerate)
	double dDn, dDw;           // kink size: actuator norm, tip-weighted norm
	double L, apex, reserve, tolF;
	double vcap;               // blend corner-speed cap (see TP3ComputeBlend)
};

static void TP3ComputeBlend(const TP3::Waypoint &wm1, const TP3::Waypoint &w0,
	const TP3::Waypoint &wp1, double dsFixed, double Tw, TP3BlendGeom *g)
{
	TP3::VecN d1 = w0.p - wm1.p;
	TP3::VecN d2 = wp1.p - w0.p;
	g->L1 = d1.norm();  g->L2 = d2.norm();
	g->dDn = g->dDw = g->L = g->apex = g->reserve = 0.0;
	g->vcap = 1e30;
	double tolmin = (w0.tol < wp1.tol) ? w0.tol : wp1.tol;
	g->tolF = tolmin;
	if (g->L1 <= 0.0 || g->L2 <= 0.0) return;
	d1 = d1 * (1.0/g->L1);
	d2 = d2 * (1.0/g->L2);
	TP3::VecN dD = d2 - d1;
	g->dDn = dD.norm();
	g->dDw = TP3WNorm(dD, w0, wp1);           // tip-referenced kink size
	if (g->dDw >= 1e-9)
	{
		double frac = TP3_FAIR_FRAC;
		if (frac > 1.0 - TP3_FIR_MIN_FRAC) frac = 1.0 - TP3_FIR_MIN_FRAC;
		double budget = frac * tolmin;
		// 49.9% (not 50%) so the two blends sharing a facet never meet:
		// the remnant between them is 0.2% of the facet, invisible, and
		// their waypoints stay distinct for Polyline::build's dedupe
		double s1 = 0.499 * g->L1;
		double s2 = 0.499 * g->L2;
		// A blend the planning grid cannot resolve keeps the ORIGINAL
		// half-tolerance split: it is invisible to the planner's caps and
		// its neighbors' filter cuts superpose in one window, so the fixed
		// half reserve and the sharp-kink speed clamp below - the
		// combination measured on 0.01-0.02 in CAM fillet facets - stay
		// exactly as they were.  Classified on the ORIGINAL (half-budget,
		// clamped) length, so every blend the original rule treated as
		// sub-grid still is; only blends the grid already resolved at the
		// half budget receive the larger one (which only lengthens them).
		double L = 4.0 * (0.5 * tolmin) / g->dDw;
		if (L > s1) L = s1;
		if (L > s2) L = s2;
		bool subgrid = (2.0 * L < dsFixed);
		if (subgrid)
			budget = 0.5 * tolmin;
		else
		{
			L = 4.0 * budget / g->dDw;
			if (L > s1) L = s1;
			if (L > s2) L = s2;
		}
		g->L = L;
		g->apex = L * g->dDw / 4.0;           // <= budget
		g->reserve = subgrid ? budget : g->apex;
	}
	g->tolF = tolmin - g->reserve;            // >= TP3_FIR_MIN_FRAC*tol

	// Blend corner-speed cap, shared by the fairing (blend waypoint F) and
	// the corner-stop policy so both see the same corner speed.
	//   sub-grid blend (2L < ds): the sharp-kink clamp 8*tolF/(dDw*Tw),
	//        unchanged.  The grid re-sharpens the corner, the planner's
	//        caps cannot see it, and this is the regime the clamp was
	//        measured on (+57% overshoot without it, -34..-49% of budget
	//        with it, see the TP3_DSFIXED note).
	//   resolved blend (2L >= ds): the filter window may be as long as the
	//        blend, v <= vwin = 2L/Tw.  At that speed the exact two-stage
	//        boxcar (T1=2R, T2=R) convolution of a symmetric quadratic
	//        Bezier cuts f(theta)*apex further inside the corner: 0.185
	//        (=40/216) at 0 deg, 0.309 at 90, 0.600 at 150, 0.722 (=13/18)
	//        at 180.  f = 0.20 + 0.58*(1 - cos(theta/2)) bounds that exact
	//        curve from above by at least 0.015 at every angle (checked
	//        numerically against the kernel); below vwin the cut falls at
	//        least linearly with v (the cut is convex in v from 0).  So run
	//        at vwin, scaled down until that cut fits the filter's share
	//        tolF: total deviation <= apex + tolF <= tol.  theta is the
	//        LARGER of the tip-space and actuator-space turn angles (f
	//        grows with angle; the tip-space Bezier has unequal legs on a
	//        nonlinear machine, which only lowers the cut).  Never below
	//        the sharp-kink clamp for the same tolF.
	//   The sharp-kink clamp alone held a resolved blend to Tw*budget/tolF
	//        of crawl - tripling it the moment the apex budget grew - and
	//        on DynoMotionBig turned 46 rounded corners into full stops
	//        (42 -> 88); with this cap the replay gives 35 stops, none new.
	if (Tw > 0.0 && g->dDw > 1e-12)
	{
		double vkink = 8.0 * g->tolF / (g->dDw * Tw);
		double vcap = vkink;
		if (g->L > 0.0 && g->apex > 0.0 && 2.0 * g->L >= dsFixed)
		{
			double aa = 0, bb = 0, ab = 0;      // tip-space turn angle
			for (int i = 0; i < TP3::NAX; i++)
			{
				double wg = (w0.wgt[i] > wp1.wgt[i]) ? w0.wgt[i] : wp1.wgt[i];
				double x = d1[i] * wg, y = d2[i] * wg;
				aa += x*x;  bb += y*y;  ab += x*y;
			}
			double c = (aa > 0.0 && bb > 0.0) ? ab / sqrt(aa * bb) : 1.0;
			if (c > 1.0) c = 1.0;
			if (c < -1.0) c = -1.0;
			double thetaW = acos(c);
			double sinHalf = g->dDn / 2.0;       // actuator-space turn angle
			if (sinHalf > 1.0) sinHalf = 1.0;
			double thetaA = 2.0 * asin(sinHalf);
			double theta = (thetaW > thetaA) ? thetaW : thetaA;
			double f = 0.20 + 0.58 * (1.0 - cos(0.5 * theta));
			double vwin = 2.0 * g->L / Tw;
			double scale = g->tolF / (f * g->apex);
			if (scale < 1.0) vwin *= scale;
			if (vwin > vcap) vcap = vwin;
		}
		g->vcap = vcap;
	}
}

static void TP3EmitFairedVertex(CCoordMotion *CM, CTP3FeedRun *r,
	const TP3::Waypoint &wm1, const TP3::Waypoint &w0, const TP3::Waypoint &wp1)
{
	TP3BlendGeom g;
	TP3ComputeBlend(wm1, w0, wp1, r->lim.dsFixed, r->Tw, &g);
	double L = g.L;

	if (L <= 1e-12)
	{
		TP3::Waypoint w = w0;              // (nearly) collinear: keep vertex;
		                                   // nothing spent, full tol stays
		TP3AddWaypointThrottled(CM, r, w, "vertex");
		return;
	}

	TP3::VecN P0 = w0.p + (wm1.p - w0.p).normalized() * L;
	TP3::VecN P2 = w0.p + (wp1.p - w0.p).normalized() * L;

	// Enforce the blend corner speed EXPLICITLY (g.vcap, TP3ComputeBlend):
	// a blend smaller than the planning grid is invisible to the planner's
	// curvature caps (the discretization sharpens the corner back up) and
	// would be taken too fast, overshooting the tolerance (+57% measured
	// at tol=0.02 on the 0.02 grid; -34% with the sharp-kink clamp); a
	// blend the grid resolves gets the window cap instead.
	// q=0 is exempt: its arriving segment is the approach straight,
	// which must keep full F (the planner ramps down into the blend).
	double vcap = g.vcap;

	int m = (int)ceil(2.0 * L / r->lim.dsFixed);
	if (m < 6) m = 6;

	// Angular resolution floor (legacy RoundCorner's FacetAngle): emit
	// enough blend points that the direction change per point stays
	// under the machine's FacetAngle.  Bounded to two points per grid
	// step - the planner samples the polyline on the ds grid and the FIR
	// window spans the blend, so finer than that buys nothing for the
	// executed path and only adds replans.
	{
		double fa = CM->Kinematics->m_MotionParams.FacetAngle * PI / 180.0;
		if (fa < 1e-4) fa = 1e-4;
		double sinHalf = g.dDn / 2.0;  if (sinHalf > 1.0) sinHalf = 1.0;
		int mA = (int)ceil(2.0 * asin(sinHalf) / fa);
		int mG = (int)ceil(4.0 * L / r->lim.dsFixed);   // 2 per grid step
		if (mA > mG) mA = mG;
		if (mA > m) m = mA;
	}
	if (m > 128) m = 128;
	for (int q = 0; q <= m; q++)
	{
		double tb = (double)q / m;
		double b0 = (1-tb)*(1-tb), b1 = 2*tb*(1-tb), b2 = tb*tb;
		TP3::Waypoint nw = (tb < 0.5) ? w0 : wp1;
		nw.p = P0*b0 + w0.p*b1 + P2*b2;
		nw.tol -= g.reserve;               // remainder for the filter
		if (q > 0 && nw.F > vcap) nw.F = vcap;
		TP3AddWaypointThrottled(CM, r, nw, "blend");
	}
}

// ---- 3rd Order collinear combining ---------------------------------------
// The legacy planner's CombineSegments rules (TrajectoryPlanner.cpp) applied
// to the interpreter's block endpoints BEFORE subdivision and the planner:
// consecutive linear moves of the same feed / accel / mode whose eliminated
// vertices all lie within Collinear Tolerance of the chord (CheckCollinear:
// triangle height, middle-point-outside, pure-rotary guards) become ONE
// chord of up to TP3_MAX_COMBINE points (no length limit - the chord is
// subdivided afterwards; see TP3StageMove).  Without it a
// sub-tolerance jog (a 1 um side step in a straight line) is two 90 degree
// corners to the planner - a corner stop, or a crawl, for a feature the FIR
// filter would not even resolve.  One chord is held open at a time; it is
// fed back through StraightFeedAccelRapid (m_TP3Staging) when the next point
// cannot extend it, and by TP3FlushStager at every run boundary, before a
// buffered command, before an arc and at a tolerance change - where the
// legacy planner commits its pending combination.
#define TP3_MAX_COMBINE   100     // mirrors MAX_COMBINE (TrajectoryPlanner.cpp)
#define TP3_SPEED_TOL     0.01    // mirrors SPEED_TOL
#define TP3_NONZERO_ANGLE 0.001   // mirrors NON_ZERO_ANGLE_IN_DEGREES

static void TP3FillSeg(SEGMENT &s, const double *P)
{
	s.x0 = P[0]; s.y0 = P[1]; s.z0 = P[2]; s.a0 = P[3];
	s.b0 = P[4]; s.c0 = P[5]; s.u0 = P[6]; s.v0 = P[7];
}

// a significant PURE rotary change (legacy 1:1 rotary, no radius) between
// two points: the legacy combiner never merges or discards such moves
static bool TP3PureRotaryChange(MOTION_PARAMS *MP, const double *P, const double *Q)
{
	if (MP->DegreesA && MP->RadiusA == 0.0 && fabs(Q[3]-P[3]) > TP3_NONZERO_ANGLE) return true;
	if (MP->DegreesB && MP->RadiusB == 0.0 && fabs(Q[4]-P[4]) > TP3_NONZERO_ANGLE) return true;
	if (MP->DegreesC && MP->RadiusC == 0.0 && fabs(Q[5]-P[5]) > TP3_NONZERO_ANGLE) return true;
	return false;
}

int CCoordMotion::TP3StageMove(double DesiredFeedRate_in_per_sec, double DesiredAccel, bool RapidMode,
	double x, double y, double z, double a, double b, double c, double u, double v,
	int sequence_number, int ID)
{
	MOTION_PARAMS *MP=&Kinematics->m_MotionParams;
	if (!m_TP3Feed) m_TP3Feed = new CTP3FeedRun;
	CTP3FeedRun *r = m_TP3Feed;
	double P[8] = {x, y, z, a, b, c, u, v};
	BOOL pure_angle;
	int i;

	// legacy MaxCombineLength: half the lookahead at the (overridden) feed,
	// capped by the kinematics' MaxLinearLength
	double HW, SW = 1.0;
	if (!RapidMode) DetermineSoftwareHardwareFRO(HW, SW);
	double MaxLength = DesiredFeedRate_in_per_sec * SW * MP->TPLookahead/2.0;
	if (MP->MaxLinearLength < MaxLength) MaxLength = MP->MaxLinearLength;

	if (r->stOpen)
	{
		double dP = FeedRateDistance(x-r->stC[0], y-r->stC[1], z-r->stC[2], a-r->stC[3],
						b-r->stC[4], c-r->stC[5], u-r->stC[6], v-r->stC[7], &pure_angle);
		if (dP == 0.0 && !RapidMode) return 0;   // zero length move: ignore

		double fmin = (DesiredFeedRate_in_per_sec < r->stF) ? DesiredFeedRate_in_per_sec : r->stF;
		double amin = (DesiredAccel < r->stAccel) ? DesiredAccel : r->stAccel;
		bool same = (RapidMode == r->stRapid) &&
			fabs(DesiredFeedRate_in_per_sec - r->stF) <= TP3_SPEED_TOL * fmin &&
			fabs(DesiredAccel - r->stAccel) <= TP3_SPEED_TOL * amin &&
			!(m_PathMode == CANON_EXACT_STOP && sequence_number != r->stSeq);  // G61.1: never across blocks

		// legacy "already quite long, don't combine" test: a chord that has
		// reached MaxCombineLength takes no further points.  The chord is
		// subdivided to that same length afterwards, so combining beyond it
		// cannot reduce the waypoint count - it can only enlarge the kinks
		// between neighbouring chords (a faceted curve within tolerance
		// becomes a few long chords meeting at large angles), and the
		// planner's kink caps charge by kink size, not by vertex count.
		bool tooLong = r->stLen > MaxLength;
		const char *why = !same ? "attributes" : tooLong ? "length" :
			(r->stN >= TP3_MAX_COMBINE) ? "count" : "collinear";
		if (same && !tooLong && r->stN < TP3_MAX_COMBINE)
		{
			SEGMENT s0, s1, s2;
			TP3FillSeg(s0, r->stA);
			TP3FillSeg(s1, r->stC);
			TP3FillSeg(s2, P);
			bool ok = CheckCollinear(&s0, &s1, &s2, MP->CollinearTol);
			for (i = 0; ok && i < r->stN; i++)      // every eliminated point too
			{
				TP3FillSeg(s1, &r->stE[8*i]);
				ok = CheckCollinear(&s0, &s1, &s2, MP->CollinearTol);
			}
			if (ok)
			{
				r->stE.insert(r->stE.end(), r->stC, r->stC + 8);
				r->stN++;
				for (i = 0; i < 8; i++) r->stC[i] = P[i];
				// keep the last line number (a halt backs up no further than
				// it), except a radius-compensated arc's deferred vector (ID
				// 0 -> 1) which keeps the vector's line so it is re-issued
				if (!(r->stId == 0 && ID == 1)) { r->stSeq = sequence_number; r->stId = ID; }
				r->stLen = FeedRateDistance(r->stC[0]-r->stA[0], r->stC[1]-r->stA[1], r->stC[2]-r->stA[2],
								r->stC[3]-r->stA[3], r->stC[4]-r->stA[4], r->stC[5]-r->stA[5],
								r->stC[6]-r->stA[6], r->stC[7]-r->stA[7], &pure_angle);
				TP3Log("COMBINE        seq=%d pts=%d len=%.6f", sequence_number, r->stN + 2, r->stLen);
				return 0;
			}
		}

		// the chord cannot take this point.  A microscopic chord - what a
		// sub-tolerance zigzag leaves behind (legacy DiscardTinySegment) -
		// is dropped rather than emitted: the next chord starts at its
		// start, within Collinear Tolerance of the skipped point
		bool tiny = same && r->stLen < MP->CollinearTol*0.5 && r->stLen < r->stMaxLen*0.25 &&
			!TP3PureRotaryChange(MP, r->stA, r->stC);
		TP3Log("STAGE_CLOSE    seq=%d at seq=%d: %s (len=%.6f max=%.6f)%s", r->stSeq, sequence_number,
			why, r->stLen, r->stMaxLen, tiny ? " -> drop" : "");
		if (tiny)
		{
			TP3Log("COMBINE_DROP   seq=%d len=%.6f (microscopic chord)", r->stSeq, r->stLen);
			r->stOpen = false;
			r->stE.clear();
			r->stN = 0;
		}
		else if (TP3FlushStager()) return 1;     // current_* -> chord end
	}

	// open a new chord from the current position to this point
	double d0 = FeedRateDistance(x-current_x, y-current_y, z-current_z, a-current_a,
					b-current_b, c-current_c, u-current_u, v-current_v, &pure_angle);
	if (d0 == 0.0 && !RapidMode) return 0;
	r->stA[0]=current_x; r->stA[1]=current_y; r->stA[2]=current_z; r->stA[3]=current_a;
	r->stA[4]=current_b; r->stA[5]=current_c; r->stA[6]=current_u; r->stA[7]=current_v;
	for (i = 0; i < 8; i++) r->stC[i] = P[i];
	r->stE.clear();
	r->stN = 0;
	r->stF = DesiredFeedRate_in_per_sec;
	r->stAccel = DesiredAccel;
	r->stRapid = RapidMode;
	r->stSeq = sequence_number;
	r->stId = ID;
	r->stLen = d0;
	r->stMaxLen = MaxLength;
	r->stOpen = true;
	return 0;
}

// Commit the open chord: it goes through the normal StraightFeedAccelRapid
// path (subdivision, then TP3AddFeedWaypoint) with m_TP3Staging set so it is
// not staged again.  current_* advances to the chord end.
int CCoordMotion::TP3FlushStager()
{
	CTP3FeedRun *r = m_TP3Feed;
	if (!r || !r->stOpen) return 0;

	double C[8];
	for (int i = 0; i < 8; i++) C[i] = r->stC[i];
	double F = r->stF, Acc = r->stAccel;
	bool rapid = r->stRapid;
	int seq = r->stSeq, id = r->stId;
	TP3Log("CHORD          seq=%d pts=%d len=%.6f %s", seq, r->stN + 2, r->stLen, rapid ? "rapid" : "feed");
	r->stOpen = false;
	r->stE.clear();
	r->stN = 0;

	// 'staging' stays set for the WHOLE commit - the subdivision recursion,
	// and any corner-stop flush / re-seed that happens inside it
	bool wasStaging = r->staging;
	r->staging = true;
	int result = StraightFeedAccelRapid(F, Acc, rapid, false, C[0], C[1], C[2], C[3], C[4], C[5], C[6], C[7], seq, id);
	if (m_TP3Feed) m_TP3Feed->staging = wasStaging;
	return result;
}

// Attach the held buffered commands [cmdEmitted, upTo) to the segment just
// inserted - the knot that comes from the waypoint they preceded - through
// the legacy special-command ring, so they download and execute with that
// segment.  Ring order stays equal to attach order.
int CCoordMotion::TP3AttachPendingCmds(int upTo)
{
	CTP3FeedRun *r = m_TP3Feed;
	if (!r) return 0;
	if (upTo > r->cmdIssued) upTo = r->cmdIssued;
	for (; r->cmdEmitted < upTo; r->cmdEmitted++)
	{
		if (TP3AttachBufCmdNow(r->cmdPending[(size_t)r->cmdEmitted].c_str(),
				r->cmdPendingSeq[(size_t)r->cmdEmitted])) return 1;
	}
	return 0;
}

// 8x8 Gauss-Jordan inversion with partial pivoting; false if singular
static bool Invert8x8(double A[8][8], double Inv[8][8])
{
	double M[8][16];
	int i, j, k;
	for (i = 0; i < 8; i++)
		for (j = 0; j < 8; j++) { M[i][j] = A[i][j]; M[i][j+8] = (i == j) ? 1.0 : 0.0; }
	for (i = 0; i < 8; i++)
	{
		int piv = i;
		for (k = i+1; k < 8; k++) if (fabs(M[k][i]) > fabs(M[piv][i])) piv = k;
		if (fabs(M[piv][i]) < 1e-12) return false;
		if (piv != i)
			for (j = 0; j < 16; j++) { double t = M[i][j]; M[i][j] = M[piv][j]; M[piv][j] = t; }
		double dpv = M[i][i];
		for (j = 0; j < 16; j++) M[i][j] /= dpv;
		for (k = 0; k < 8; k++)
			if (k != i && M[k][i] != 0.0)
			{
				double f = M[k][i];
				for (j = 0; j < 16; j++) M[k][j] -= f * M[i][j];
			}
	}
	for (i = 0; i < 8; i++)
		for (j = 0; j < 8; j++) Inv[i][j] = M[i][j+8];
	return true;
}

// Per-axis TOOL-TIP error weights: |d(tip xyz)/d(normalized actuator i)|
// - for a rotary actuator this is its local pivot radius (inches per
// degree).  Built by probing the (analytic) forward CAD->actuator
// transform along each CAD dimension and inverting the 8x8 Jacobian.
// Inactive dimensions are regularized to identity.  Unmodeled rotaries
// (kinematics reports zero tip effect, e.g. 1:1 kinematics) fall back
// to the legacy RadiusA/B/C setting, else 1.0 (the historical 1in/deg
// equivalence).  Cached on the rotary pose: for supported kinematics
// the Jacobian doesn't depend on XYZ, so linear moves reuse the cache.
int CCoordMotion::ComputeTipWeights(double x, double y, double z, double a,
	double b, double c, double u, double v, double *wgt)
{
	int i, d;
	MOTION_PARAMS *MP=&Kinematics->m_MotionParams;

	if (m_TipWgtValid && fabs(a - m_TipWgtA) < 1e-6 &&
		fabs(b - m_TipWgtB) < 1e-6 && fabs(c - m_TipWgtC) < 1e-6)
	{
		for (i = 0; i < 8; i++) wgt[i] = m_TipWgt[i];
		return 0;
	}

	double Scale[8], Vel, Accel, Jerk;
	for (i = 0; i < 8; i++)
		GetEffectiveActuatorLimits(MP, i, &Scale[i], &Vel, &Accel, &Jerk);

	double cad[8] = {x, y, z, a, b, c, u, v};
	double Acts0[MAX_ACTUATORS], Acts1[MAX_ACTUATORS];
	if (Kinematics->TransformCADtoActuators(x, y, z, a, b, c, u, v, Acts0)) return 1;

	double J[8][8], Jinv[8][8];
	const double eps = 1e-4;
	for (d = 0; d < 8; d++)
	{
		double p[8];
		for (i = 0; i < 8; i++) p[i] = cad[i];
		p[d] += eps;
		if (Kinematics->TransformCADtoActuators(p[0], p[1], p[2], p[3],
				p[4], p[5], p[6], p[7], Acts1)) return 1;
		for (i = 0; i < 8; i++)
		{
			double n0 = (Scale[i] != 0.0) ? Acts0[i]/Scale[i] : 0.0;
			double n1 = (Scale[i] != 0.0) ? Acts1[i]/Scale[i] : 0.0;
			J[i][d] = (n1 - n0) / eps;
		}
	}

	// regularize dimensions with no coupling at all (unused slots/CAD
	// dims) so the inversion doesn't fail on them
	for (i = 0; i < 8; i++)
	{
		double rmax = 0, cmax = 0;
		for (d = 0; d < 8; d++)
		{
			if (fabs(J[i][d]) > rmax) rmax = fabs(J[i][d]);
			if (fabs(J[d][i]) > cmax) cmax = fabs(J[d][i]);
		}
		if (rmax < 1e-9 && cmax < 1e-9) J[i][i] = 1.0;
	}

	bool okInv = Invert8x8(J, Jinv);

	for (i = 0; i < 8; i++)
	{
		double w = 0.0;
		if (okInv)
			w = sqrt(Jinv[0][i]*Jinv[0][i] + Jinv[1][i]*Jinv[1][i] + Jinv[2][i]*Jinv[2][i]);
		if (w < 1e-9)
		{
			// kinematics says this axis doesn't move the tip: fall back
			// to the configured Radius (deg -> inch), else 1.0
			if      (i == 3 && MP->DegreesA && MP->RadiusA > 0.0) w = MP->RadiusA*PI/180.0;
			else if (i == 4 && MP->DegreesB && MP->RadiusB > 0.0) w = MP->RadiusB*PI/180.0;
			else if (i == 5 && MP->DegreesC && MP->RadiusC > 0.0) w = MP->RadiusC*PI/180.0;
			else w = 1.0;
		}
		if (w > 1e4) w = 1e4;      // near-singular pose: clamp
		wgt[i] = w;
	}

	m_TipWgtValid = true;
	m_TipWgtA = a; m_TipWgtB = b; m_TipWgtC = c;
	for (i = 0; i < 8; i++) m_TipWgt[i] = wgt[i];
	return 0;
}

// Max speed along unit direction d given a programmed F and the run's
// per-axis velocity limits (axis speed = v*|d_i| <= Vmax_i).
static double TP3SegSpeed(const CTP3FeedRun *r, const TP3::VecN &d, double F)
{
	double v = F;
	for (int i = 0; i < TP3::NAX; i++)
	{
		double di = fabs(d[i]);
		if (di > 1e-12 && r->lim.ax.vmax[i]/di < v) v = r->lim.ax.vmax[i]/di;
	}
	return v;
}

// Decide whether the corner raw0 -> raw1 -> w should be a full stop at
// the vertex instead of a faired blend (see TP3_CORNER_STOP_* above).
// The crawl estimate is the blend corner-speed cap TP3ComputeBlend gives
// the blend TP3EmitFairedVertex would actually emit (same geometry, same
// cap: the sharp-kink clamp for a sub-grid blend, max(that clamp, the
// window cap) for a resolved one).
// exactPath (G61 / G64 P0): stop at ANY real direction change - the FIR
// filter cannot pass a corner at speed without deviating.
static bool TP3CornerNeedsStop(const CTP3FeedRun *r, const TP3::Waypoint &w,
	bool exactPath, double *dDn_out, double *vc_out, double *vseg_out)
{
	TP3BlendGeom g;
	TP3ComputeBlend(r->raw0, r->raw1, w, r->lim.dsFixed, r->Tw, &g);
	if (g.L1 <= 0.0 || g.L2 <= 0.0 || r->Tw <= 0.0) return false;
	double dDn = g.dDn;
	*dDn_out = dDn;
	*vc_out = *vseg_out = 0.0;
	if (exactPath) return dDn > 0.01;   // ~0.57 deg direction change
	if (dDn < TP3_CORNER_STOP_DDMIN) return false;
	if (dDn >= TP3_CORNER_STOP_DD) return true;   // near-reversal: always stop

	double vc = g.vcap;                           // same cap the blend gets
	if (g.dDw < 1e-12)                            // degenerate tip weights:
		vc = 8.0 * g.tolF / (dDn * r->Tw);        // actuator-norm kink cap

	TP3::VecN d1 = (r->raw1.p - r->raw0.p) * (1.0/g.L1);
	TP3::VecN d2 = (w.p - r->raw1.p) * (1.0/g.L2);
	double vs1 = TP3SegSpeed(r, d1, r->raw1.F);   // F arrives AT a waypoint
	double vs2 = TP3SegSpeed(r, d2, w.F);
	double vseg = (vs1 < vs2) ? vs1 : vs2;
	*vc_out = vc;
	*vseg_out = vseg;

	return vc < TP3_CORNER_STOP_FRAC * vseg;
}

// Collect one (already subdivided) motion target as an actuator-space
// waypoint into the streaming plan.  The first waypoint of a run is the
// current position; interior vertices are faired incrementally (one
// waypoint of lookahead) before entering the planner.
int CCoordMotion::TP3AddFeedWaypoint(double x, double y, double z, double a, double b, double c,
	double u, double v, double FeedRateToUse, int sequence_number, int ID, BOOL NoCallback)
{
	MOTION_PARAMS *MP=&Kinematics->m_MotionParams;
	double Acts[MAX_ACTUATORS], Scale, Vel, Accel, Jerk;
	int i;

	if (!m_TP3Feed) m_TP3Feed = new CTP3FeedRun;
	CTP3FeedRun *r = m_TP3Feed;

	// start of a run: build the limits, validate them, create the
	// streaming planner, and seed it with the current position
	if (!r->sp)
	{
		TP3::Limits lim;
		lim.dt = TP3_PLAN_DT;
		lim.dsFixed = TP3_DSFIXED;
		double Rmax = 0.0, Aw = 0.0, Jw = 0.0;
		int iw = 0;
		int nAct = 0;
		for (i=0;i<TP3::NAX;i++)
		{
			GetEffectiveActuatorLimits(MP,i,&Scale,&Vel,&Accel,&Jerk);
			if (!TP3SlotInUse(i))
			{
				// Actuator slot not in use: Scale==0 (slot absent from
				// the actuator table), or - in legacy CAD 1:1 mapping -
				// the CAD axis is not part of the defined Coordinate
				// System, matching the legacy planner which never
				// constrained (or required configuration of) axes that
				// cannot move.  Hand the planner ZERO limits, which it
				// reads as "not present" and leaves out of the
				// machine-wide constants (FIR span Tw, corner-speed
				// floor, comp fade); its waypoint coordinate is constant
				// so it also contributes nothing to any per-axis cap.
				// Previously this passed PLACEHOLDER limits (1e6/1e7/1e8)
				// merely to satisfy a positivity check, which let an
				// absent slot dictate Tw (via 1e7/1e8 = 0.1s) and
				// inflate the Vbig-scaled corner floor for every real
				// axis.
				lim.ax.vmax[i]=lim.ax.amax[i]=lim.ax.jmax[i]=0.0;
				continue;
			}
			if (Vel <= 0.0 || Accel <= 0.0 || Jerk <= 0.0)
			{
				SetAbort();
				KMotionDLL->DoErrMsg(L"3rd Order Planner requires positive Vel, Accel, and Jerk for all axes in use");
				return 1;
			}
			// smoothing time constant this axis asks for: Aeff/Jerk with the
			// effective Accel Aeff = min(Accel, sqrt(Vel*Jerk/2)) - the largest
			// acceleration a rest-to-Vel step can exercise through the FIR
			// windows (TP3::effectiveAmax; the planner reduces Accel the same way)
			double Aeff = TP3::effectiveAmax(Vel, Accel, Jerk);
			double Ri = Aeff/Jerk;
			if (Ri > 1.0 && m_Simulate && !m_DefineCS_known && !MP->ActuatorLimits)
			{
				// Simulate (e.g. the G-view redraw after Tool Setup closes)
				// with the Coordinate System not yet read from the board:
				// TP3SlotInUse cannot tell whether this CAD axis is even
				// part of the machine, and an UNUSED axis left at default
				// limits (A/B/C Accel 10 / Jerk 1 = 10 s) must not raise
				// the impractical-Accel/Jerk error.  Leave it out of the
				// simulated plan; a real launch reads the CS first and the
				// check below then applies only to axes actually in use.
				lim.ax.vmax[i]=lim.ax.amax[i]=lim.ax.jmax[i]=0.0;
				continue;
			}
			lim.ax.vmax[i]=Vel; lim.ax.amax[i]=Accel; lim.ax.jmax[i]=Jerk;
			if (Ri > Rmax) { Rmax = Ri; Aw = Aeff; Jw = Jerk; iw = i; }
			nAct++;
		}
		if (nAct == 0)
		{
			SetAbort();
			KMotionDLL->DoErrMsg(L"3rd Order Planner: no actuator has a nonzero Resolution (Counts per unit).\r\n"
				L"Define at least one axis in the Trajectory Planner Axis Parameters.");
			return 1;
		}
		// One filter pair spans 3*max(Aeff/Jerk) for ALL axes; a single
		// axis with absurdly low Jerk makes every plan impractical
		if (Rmax > 1.0)
		{
			CString s;
			s.Format(L"3rd Order Planner: Axis %d has effective Accel/Jerk = %.6g/%.6g = %.1f seconds\r\n"
				L"(effective Accel = min(Accel, sqrt(Vel*Jerk/2))).\r\n"
				L"This time constant sets the smoothing filter span for ALL axes and makes\r\n"
				L"planning impractical.  Increase that axis's Jerk (or reduce its Accel) so\r\n"
				L"Accel/Jerk is well under 1 second (0.02 to 0.2 is typical).", iw, Aw, Jw, Rmax);
			SetAbort();
			KMotionDLL->DoErrMsg(s);
			return 1;
		}

		// the planner keeps at most the user's lookahead planned ahead of
		// the (estimated) execution point; the launch machinery already
		// starts execution once TPLookahead is downloaded
		double targetBuffer = MP->TPLookahead;
		if (targetBuffer < 1.0) targetBuffer = 1.0;

		r->lim = lim;                  // corner-stop policy inputs
		r->Tw = 3.0 * TP3::filterRatio(lim.ax);   // == 3*Rmax: the planner's
		                                          // own windows, one source
		// exec baseline: PROVISIONAL here - a prior flush's tail may still
		// be inserted-but-undownloaded, and its time belongs on the pre-run
		// side.  OutputSegment finalizes execBase at the exact moment
		// downloads reach this run's first segment (firstSeg); when nothing
		// is pending the provisional value is already exact.
		r->execBase = m_TotalDownloadedTime;
		r->firstSeg = nsegs;
		r->baseFinal = (m_nsegs_downloaded >= nsegs);
		r->timer.Start();              // poll cache clock (valid pre-download)

		// fresh timeline log per execution: opened at the first streaming
		// run (NOT DownloadInit - that runs during construction), closed
		// at ExecutionStop, so later runs of the same program append.
		// Only when the user asked for logging (Tool Setup "Log" option).
		// (also in Do Times simulation, which runs the real planner - the
		// same gate the segment log uses)
		if (!tp3Log && (!m_Simulate || m_DoTime) && MP->LogSegments) TP3LogOpen();
		TP3Log("RUN_START      targetBuffer=%.3f Tw=%.3f ds=%.4f tol=%.5f seq=%d",
			targetBuffer, r->Tw, lim.dsFixed, EffectiveCornerTol(), sequence_number);

		try
		{
			// executed time comes from the controller (ExecTime query,
			// cached and refreshed at most 10x/sec).  ExecTime is
			// PROGRAM-cumulative (and negative = not executing, with
			// |e| = the last buffer's total time), while the planner's
			// written time is per-RUN - so the run's execBase (motion
			// downloaded before the run) is subtracted.  Aborts and
			// time-estimating simulation unblock the throttle immediately.
			r->sp = new TP3::StreamingTrajectoryPlanner(lim, targetBuffer,
				[this]() -> double
				{
					if (m_Abort || m_Simulate) return 1e30;
					CTP3FeedRun *rr = m_TP3Feed;
					if (!rr) return 0.0;
					// (queried even before this run's first download:
					// the controller may still be executing earlier
					// runs' data - the timer starts at run creation)
					double el = rr->timer.Elapsed_Seconds();
					if (rr->lastExecT < 0 || el - rr->lastExecT > 0.1)
					{
						rr->lastExecT = el;
						CStringA resp;
						if (!KMotionDLL->WriteLineReadLine("ExecTime",
								resp.GetBufferSetLength(MAX_LINE)))
						{
							resp.ReleaseBuffer();
							double e;
							if (sscanf(resp, "%lf", &e) == 1)
							{
								TP3Log("EXEC           %8.3f", e);
								if (e >= 0.0)
								{
									rr->sawExec = true;
									rr->lastExec = e;   // program-cumulative
								}
								else if (rr->sawExec)
								{
									// negative AFTER running: the controller
									// ran the downloaded buffer to its (rest)
									// end while more was being planned - the
									// Kogna's NORMAL report for a completely
									// executed buffer, here meaning the data
									// RAN DRY.  Re-arm the launch: downloads
									// continue and the auto-launch re-issues
									// ExecBuf once TPLookahead of content is
									// ahead of the executed total again.
									// Execution resumes mid-ring, so ExecTime
									// stays program-cumulative and EXACT (the
									// firmware only zeroes it when a buffer
									// restarts from its beginning).
									// A streaming jog (m_LookaheadOverride)
									// handles its own dry buffers, and its
									// release teardown detaches on purpose.
									if (!m_LookaheadOverride && m_SegmentsStartedExecuting)
									{
										CStringA ss;
										if (!KMotionDLL->WriteLineReadLine("GetStopState",
												ss.GetBufferSetLength(MAX_LINE)))
										{
											ss.ReleaseBuffer();
											if (ss == "0")
											{
												TP3Log("RANDRY_REARM   executed=%8.3f", -e);
												m_TimeAlreadyExecuted = -e;
												RearmCoordLaunch();
											}
											else
												TP3Log("RANDRY_STOPPED state=%s executed=%8.3f",
													(const char *)ss, -e);
										}
										else
											ss.ReleaseBuffer();
									}
									rr->lastExec = -e;  // |e| = total executed
								}
								else
									rr->lastExec = 0;   // stale pre-launch value
							}
						}
						else
							resp.ReleaseBuffer();
					}
					{
						// run-relative executed time.  CLAMPED at zero: when
						// runs chain without a sync (e.g. Z pen lifts, no
						// M-code FINWAIT) the controller is still executing
						// the PREVIOUS run's content at this run's first
						// download, so lastExec < execBase - a negative here
						// would inflate the buffered estimate by the whole
						// in-flight backlog and over-throttle the feed.  If
						// the firmware's data-end feedhold then froze exec
						// first, throttle and feedhold DEADLOCKED (observed:
						// exec pinned 11.133 with only 2.045s written and
						// the planner throttled).  A run that has not begun
						// executing has consumed exactly zero of itself.
						double trun = rr->lastExec - rr->execBase;
						return trun > 0.0 ? trun : 0.0;
					}
				});

			TP3::Waypoint w0;
			if (Kinematics->TransformCADtoActuators(current_x,current_y,current_z,current_a,
					current_b,current_c,current_u,current_v,Acts)) return 1;
			for (i=0;i<MAX_ACTUATORS;i++)
			{
				GetEffectiveActuatorLimits(MP,i,&Scale,&Vel,&Accel,&Jerk);
				w0.p[i] = (Scale != 0.0) ? Acts[i]/Scale : 0.0;
			}
			w0.seq = sequence_number;
			w0.id  = ID;
			w0.cmd = r->cmdIssued;           // 0: a run starts with none held
			w0.cad[0]=current_x; w0.cad[1]=current_y; w0.cad[2]=current_z; w0.cad[3]=current_a;
			w0.cad[4]=current_b; w0.cad[5]=current_c; w0.cad[6]=current_u; w0.cad[7]=current_v;
			if (ComputeTipWeights(current_x, current_y, current_z, current_a,
					current_b, current_c, current_u, current_v, w0.wgt.c)) return 1;
			r->raw0 = w0;                    // raw copy keeps original tol
			r->haveRaw0 = true;
			r->haveRaw1 = false;
			// (a first waypoint's tol is never read: segTol comes from the
			// ARRIVING waypoint of each segment)
			r->sp->addWaypointNoWait(w0);    // planner empty: cannot block
		}
		catch (...)
		{
			SetAbort();
			TP3ClearRun();
			KMotionDLL->DoErrMsg(L"3rd Order Planner failed to start streaming plan");
			return 1;
		}
	}

	TP3::Waypoint w;
	if (Kinematics->TransformCADtoActuators(x,y,z,a,b,c,u,v,Acts)) return 1;
	for (i=0;i<MAX_ACTUATORS;i++)
	{
		GetEffectiveActuatorLimits(MP,i,&Scale,&Vel,&Accel,&Jerk);
		w.p[i] = (Scale != 0.0) ? Acts[i]/Scale : 0.0;
	}

	const TP3::Waypoint &prev = r->haveRaw1 ? r->raw1 : r->raw0;
	double L8=0, Lxyz=0;
	for (i=0;i<MAX_ACTUATORS;i++)
	{
		double dd = w.p[i]-prev.p[i];
		L8 += dd*dd;
		if (i<3) Lxyz += dd*dd;
	}
	L8 = sqrt(L8); Lxyz = sqrt(Lxyz);
	if (L8 <= 0.0) return 0;   // no actuator motion

	// programmed CAD feedrate expressed against actuator arc length: the
	// planner enforces v8 <= F/|u_xyz|, so F = F_cad * L_act_xyz / L_cad
	// makes the 8D actuator path speed equal F_cad * L_act8/L_cad -- the
	// programmed CAD feedrate seen through the kinematic map
	BOOL pure_angle;
	double Lcad = FeedRateDistance(x-current_x, y-current_y, z-current_z, a-current_a,
					b-current_b, c-current_c, u-current_u, v-current_v, &pure_angle);
	if (FeedRateToUse > 0.0 && Lcad > 0.0 && Lxyz > 1e-12)
		w.F = FeedRateToUse * Lxyz / Lcad;
	else
		w.F = INFINITY;   // rapid, or pure rotary: per-axis caps only
	// G0 attribution is a SEPARATE flag: fairing may clamp F on blend
	// waypoints (kink-cap corner speed) which must not flip the
	// rapid/feed attribution (G-view color, Beg/EndRapidBuf brackets)
	w.rapid = (FeedRateToUse < 0.0);

	// tolerance stays in TOOL-TIP inches: the per-axis Jacobian weights
	// on the waypoint denominate deviations (replacing the old scalar
	// L8/Lcad approximation).  Honors G64 P / G61 / G61.1 (exact modes
	// give 0 -> floor; exactness comes from the forced stops below, the
	// floor is numerical only)
	double tol = EffectiveCornerTol();
	if (tol < 1e-5) tol = 1e-5;
	w.tol = tol;
	w.seq = sequence_number;
	w.id  = ID;
	w.cmd = r->cmdIssued;   // buffered commands issued before this move
	w.cad[0]=x; w.cad[1]=y; w.cad[2]=z; w.cad[3]=a;
	w.cad[4]=b; w.cad[5]=c; w.cad[6]=u; w.cad[7]=v;
	w.nocb = NoCallback != FALSE;   // jogs stream but must not draw
	if (ComputeTipWeights(x, y, z, a, b, c, u, v, w.wgt.c)) return 1;

	// incremental fairing: with the new waypoint in hand, the PREVIOUS raw
	// vertex has both neighbors and can be blended into the planner; then
	// drive the streaming plan and convert any newly finalized samples
	try
	{
		TP3Log("WPT            seq=%-6d F=%-10.9g L8=%8.5f tol=%.5f "
			"p=%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g "
			"w=%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g",
			w.seq, w.F, L8, w.tol,
			w.p[0], w.p[1], w.p[2], w.p[3], w.p[4], w.p[5], w.p[6], w.p[7],
			w.wgt[0], w.wgt[1], w.wgt[2], w.wgt[3],
			w.wgt[4], w.wgt[5], w.wgt[6], w.wgt[7]);

		if (!r->haveRaw1)
		{
			r->raw1 = w;
			r->haveRaw1 = true;
		}
		else
		{
			// forced rests: G61.1 stops between G-code blocks; G61 (and
			// near-reversal / crawl corners in G64) stop exactly at the
			// vertex.  Either way: flush this run at the vertex, then
			// start a new run from it.
			bool stopHere = false;
			if (m_PathMode == CANON_EXACT_STOP && w.seq != r->raw1.seq)
			{
				stopHere = true;
				TP3Log("BLOCK_STOP     seq=%d (G61.1 block boundary)", w.seq);
			}
			else
			{
				double dDn, vcrawl, vseg;
				if (TP3CornerNeedsStop(r, w, PathExact(), &dDn, &vcrawl, &vseg))
				{
					stopHere = true;
					TP3Log("CORNER_STOP    seq=%d dDn=%.3f vcrawl=%.4g vseg=%.4g",
						w.seq, dDn, vcrawl, vseg);
				}
			}
			if (stopHere)
			{
				if (TP3FlushRun("corner-stop")) return 1;
				// re-enter: seeds a fresh run at the vertex (the current
				// position) and adds this waypoint to it
				return TP3AddFeedWaypoint(x, y, z, a, b, c, u, v,
					FeedRateToUse, sequence_number, ID, NoCallback);
			}
			TP3EmitFairedVertex(this, r, r->raw0, r->raw1, w);
			r->raw0 = r->raw1;
			r->raw1 = w;
		}

		// replan/emit on either trigger: enough waypoints (dense curves)
		// or enough path TIME (long straight facets can be many seconds
		// each - a count-only trigger starves execution on straights)
		r->sinceUpdate++;
		if (FeedRateToUse > 0.0 && Lcad > 0.0)
			r->sinceUpdateT += Lcad / FeedRateToUse;
		else
			r->sinceUpdateT += 0.05;   // rapid facet: nominal estimate

		// Simulation has no controller to keep fed, so batch updates far
		// coarser: the fixed per-update cost (full-window replan plus the
		// filter/comp context collar around the new ticks) is amortized
		// over ~20x more emitted ticks.  Results are identical - the
		// finalization frontier depends only on the plan, never on when
		// update() is called.
		int    updEvery = m_Simulate ? 20*TP3_UPDATE_EVERY : TP3_UPDATE_EVERY;
		double updTime  = m_Simulate ? 5.0 : 0.25;

		if (r->sinceUpdate >= updEvery || r->sinceUpdateT >= updTime)
		{
			r->sinceUpdate = 0;
			r->sinceUpdateT = 0;
			std::vector<TP3::TrajPoint> out = r->sp->update();
			TP3Log("UPDATE         emitted=%-5d written=%8.3f exec=%8.3f margin=%6.3f",
				(int)out.size(), r->sp->getTotalTimeWritten(), r->lastExec,
				r->sp->emissionMargin());
			if (TP3ConsumeSamples(this, r, out, false)) return 1;

			// canary: if the emitted trajectory has fallen close behind
			// the controller's executed time the buffer is about to
			// underflow - report it rather than silently stopping.
			// (exec is program-cumulative; the run's execBase converts
			// it to this run's clock.  baseFinal gate: until downloads
			// reach the run's first segment the baseline is provisional
			// and "ahead" would under-read by the pending backlog)
			if (r->started && r->baseFinal && !m_Simulate && !r->warnedUnderflow
				&& r->sawExec && r->lastExec - r->execBase > 0.1)
			{
				double ahead = r->sp->getTotalTimeWritten() - (r->lastExec - r->execBase);
				if (ahead < 0.25)
				{
					r->warnedUnderflow = true;
					CString s;
					s.Format(L"3rd Order Planner: buffer nearly underflowed - only %.2f sec of\r\n"
						L"trajectory is ahead of the controller's executed time.\r\n"
						L"(planning/download is not keeping up with execution)", ahead);
					KMotionDLL->DoErrMsg(s);
				}
			}
		}
	}
	catch (...)
	{
		SetAbort();
		TP3ClearRun();
		KMotionDLL->DoErrMsg(L"3rd Order Planner streaming update failed");
		return 1;
	}

	return 0;
}

// End the streaming run: add the held-back final raw waypoint, flush the
// planner (emits everything including the decel-to-rest tail), convert the
// remaining samples to knots, and reset for the next run.
int CCoordMotion::TP3FlushRun(const char *why)
{
	// a staged straight chord (collinear combining) enters the run first -
	// this may itself split the run at a corner stop (nested flush), which
	// is fine: the stager is already closed by then.  Nothing may remain
	// staged when a run is flushed (it would be dropped): re-check.
	for (int guard = 0; guard < 4; guard++)
	{
		if (TP3FlushStager()) return 1;
		if (!m_TP3Feed || !m_TP3Feed->stOpen) break;
	}

	CTP3FeedRun *r = m_TP3Feed;

	// finalize the drawn (faired) path BEFORE the planner check: in
	// Simulate mode the planner never runs but the drawing state does
	TP3DrawFlush();

	if (!r || !r->sp) { TP3ClearRun(); return 0; }
	if (m_TP3Flushing) return 0;   // reentry via the download machinery
	m_TP3Flushing = true;

	TP3Log("FLUSH_BEGIN    why=%s written=%8.3f exec=%8.3f",
		why ? why : "boundary", r->sp->getTotalTimeWritten(), r->lastExec);

	std::vector<TP3::TrajPoint> tail;
	try
	{
		// the last raw waypoint was held back for fairing lookahead; it is
		// the run's endpoint so it enters unblended (endpoints are exact)
		if (r->haveRaw1)
		{
			TP3::Waypoint wl = r->raw1;      // unblended endpoint: nothing
			                                 // spent on fairing, full tol
			TP3AddWaypointThrottled(this, r, wl, "flush-end");
		}
		tail = r->sp->flush();
		TP3Log("FLUSH_EMIT     tail=%-5d written=%8.3f",
			(int)tail.size(), r->sp->getTotalTimeWritten());
	}
	catch (...)
	{
		SetAbort();
		TP3ClearRun();
		m_TP3Flushing = false;
		KMotionDLL->DoErrMsg(L"3rd Order Planner failed to finish the streaming plan\r\n"
			L"(out of memory or invalid limits - check Vel/Accel/Jerk settings)");
		return 1;
	}

	if (TP3ConsumeSamples(this, r, tail, true)) { TP3ClearRun(); m_TP3Flushing = false; return 1; }

	// buffered commands still held (issued after the last waypoint that
	// produced a knot): they belong at the end of this run
	if (r->cmdIssued > r->cmdEmitted)
	{
		TP3Log("BUFCMD_ATTACH  n=%d..%d (flush end)", r->cmdEmitted+1, r->cmdIssued);
		if (TP3AttachPendingCmds(r->cmdIssued)) { TP3ClearRun(); m_TP3Flushing = false; SetAbort(); return 1; }
	}

	// close an open rapid bracket so following legacy motion (and the
	// next run, which starts in feed state) sees normal FRO
	if (r->inRapid)
	{
		TP3Log("END_RAPID       (flush)");
		if (DoKMotionBufCmd("EndRapidBuf", 0)) { TP3ClearRun(); m_TP3Flushing = false; SetAbort(); return 1; }
	}
	TP3ClearRun();

	// any following legacy motion must plan from rest
	if (nsegs > 0) GetSegPtr(nsegs-1)->StopRequiredNextSeg = TRUE;

	if (DownloadDoneSegments()) {SetAbort(); m_TP3Flushing = false; return 1;}
	TP3Log("FLUSH_END");
	m_TP3Flushing = false;
	return 0;
}


// Commit those segments pending to be combined

int CCoordMotion::CommitPendingSegments(bool RapidMode)
{
	// note: this commits LEGACY segments pending collinear combining only.
	// It must NOT flush the 3rd Order TP waypoint stream - rapids-as-feeds
	// and arc facets continue the same stream.  TP3FlushRun() is called
	// explicitly at true motion boundaries (independent rapids, legacy
	// whole arcs, dwells, execution stop) instead.

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
		KMotionDLL->DoErrMsg(L"Straight Feed with Feed Rate Zero or Negative");
		return 1;
	}

	// if exceeding limits trigger Halt

	// check if we should sync parameters with KFLOP
	if (GetRapidSettings()) return 1;

	CString errmsg;
	if (CheckSoftLimits(x,y,z,a,b,c,u,v,errmsg)) 
	{
		CString FunctionType;
		if (RapidMode) FunctionType=" Straight Traverse";
		else FunctionType=" Straight Feed";
		
		if (m_Simulate)
		{
			KMotionDLL->DoErrMsg((CString)"Soft Limit "+errmsg+FunctionType+"\r\r"
				"Soft Limits disabled for remainder of Simulation");
			m_DisableSoftLimits=true;
		}
		else
		{
			SetHalt();
			CheckMotionHalt(true);
			SetAbort();
			KMotionDLL->DoErrMsg((CString)"Soft Limit "+errmsg+FunctionType+" Job Halted");
			return 1;
		}
	}



	// 3rd Order Planner: collinear combining.  Interpreter moves are staged
	// into chords first (TP3StageMove, the legacy CombineSegments rules);
	// each finished chord comes back through here with m_TP3Staging set and
	// is then subdivided and streamed as usual.  Jogs (NoCallback) and a
	// zero Collinear Tolerance stream directly, after any staged chord.
	// (Before the zero-length test below: while a chord is open current_*
	// is the chord START, so that test is the stager's to make.)
	if (MP->ThirdOrderTP && (!m_Simulate || m_DoTime) && !(m_TP3Feed && m_TP3Feed->staging))
	{
		if (!NoCallback && MP->CollinearTol > 0.0)
			return TP3StageMove(DesiredFeedRate_in_per_sec, DesiredAccel, RapidMode,
				x, y, z, a, b, c, u, v, sequence_number, ID);
		if (TP3FlushStager()) return 1;
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

	if ((!m_Simulate || m_DoTime) && ((!pure_angle && d > MaxLength) ||
		(MP->DegreesA && fabs(da) > MP->MaxAngularChange) || 
		(MP->DegreesB && fabs(db) > MP->MaxAngularChange) || 
		(MP->DegreesC && fabs(dc) > MP->MaxAngularChange)))
	{
		int result = StraightFeedAccelRapid(DesiredFeedRate_in_per_sec, DesiredAccel, RapidMode, NoCallback,current_x+dx/2.0, current_y+dy/2.0, current_z+dz/2.0, 
														current_a+da/2.0, current_b+db/2.0, current_c+dc/2.0, current_u+du/2.0, current_v+dv/2.0, sequence_number, ID);
		if (result) return result;
		
		return StraightFeedAccelRapid(DesiredFeedRate_in_per_sec, DesiredAccel, RapidMode, NoCallback, x, y, z, a, b, c, u, v, sequence_number, ID);
	}



	if (m_Simulate && !m_DoTime)
	{
		// if simulating just draw it now.  RAW programmed path for BOTH
		// planners: plain Simulate previews the G Code as written; the
		// planned (corner-blended) path is shown by Do Times, which runs
		// the real 3rd Order planner and draws its downloaded knots
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

	
	if (MP->ThirdOrderTP)
	{
		// 3rd Order TP: collect this (already subdivided) move as an
		// actuator-space waypoint in ONE continuous stream - rapids join
		// the same stream as F=INFINITY waypoints (limited only by the
		// per-actuator Vel/Accel/Jerk) so feed<->rapid transitions blend
		// with no stop and no legacy/TP3 boundary.  Planning happens at
		// the next motion boundary (CommitPendingSegments) or when the
		// pending run exceeds the lookahead.
		if (TP3AddFeedWaypoint(x, y, z, a, b, c, u, v,
				RapidMode ? -1.0 : FeedRateToUse, sequence_number, ID, NoCallback))
			{SetAbort(); return 1;}

		// drawing: the ACTUAL downloaded path is drawn per knot in
		// TP3ConsumeSamples (TP3DrawKnotCAD) - nothing to draw here.
		// (Simulate mode draws the CAD-space faired approximation
		// instead since the planner never runs there.)

		current_x = x;
		current_y = y;
		current_z = z;
		current_a = a;
		current_b = b;
		current_c = c;
		current_u = u;
		current_v = v;

		if (DownloadDoneSegments()) {SetAbort(); return 1;}
		return 0;
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


// CAD-space coordinates of a segment's beginning point.  Arc segments
// store their coordinates in the arc-plane-local frame (x,y = plane,
// z = third axis); un-swap them the same way CalcBegDirectionOfSegment
// un-swaps the direction so the point matches the direction's frame.
static void SegBegCADPoint(SEGMENT *p, double *px, double *py, double *pz,
	double *pa, double *pb, double *pc, double *pu, double *pv)
{
	double lx = p->x0, ly = p->y0, lz = p->z0;
	*pa = p->a0; *pb = p->b0; *pc = p->c0; *pu = p->u0; *pv = p->v0;

	if (p->type == SEG_ARC && p->plane == CANON_PLANE_XZ)
	{
		*pz = lx; *px = ly; *py = lz;
	}
	else if (p->type == SEG_ARC && p->plane == CANON_PLANE_YZ)
	{
		*py = lx; *px = lz; *pz = ly;
	}
	else
	{
		*px = lx; *py = ly; *pz = lz;
	}
}

// ---- Trajectory Planner segment log --------------------------------------
// When MP->LogSegments is set, every segment downloaded to the
// controller is appended to TP_SEGLOG_FILE as ONE CSV row containing
// everything the controller receives: kind/geometry/endpoints/arc
// center and the trip-state polynomials (t,a,b,c,d per trip).  All
// values are printed at 32-bit float resolution (%.9g of the float
// cast) because that is exactly how they are downloaded.  The viewer
// (MotionLogPlotter) re-creates the motion by stepping through the
// segments at the servo tick with leftover-time carry, exactly like
// the controller's DoTrajectorys - so segment boundaries land at their
// true (off-grid) times and pre-filter errors are reproduced exactly.
// Coordinates are normalized actuator units (actuator counts / Scale)
// for everything the controller interpolates in actuator space: 3rd
// Order ActSpace knots as planned, and legacy linear/rapid/dwell
// segments after the same kinematics transform the download applies to
// their endpoints.  Legacy arcs stay CAD (see SegLogSegment).

#ifdef _KMOTIONX
#define TP_SEGLOG_FILE "/tmp/TPSegLog.csv"
#else
#define TP_SEGLOG_FILE "c:\\Temp\\TPSegLog.csv"
#endif
#define FQ(x) ((double)(float)(x))   // downloaded (float) resolution

void CCoordMotion::SegLogClose()
{
	if (m_SegLog) { fclose(m_SegLog); m_SegLog = NULL; }
}

// Close BOTH log files (segment log + TP3 timeline).  ExecutionStop does
// this at a clean end of interpretation, but an Abort (following error,
// axis disable, user stop) skips ExecutionStop entirely, leaving the
// files open in this process - and unopenable by MotionLogPlotter -
// until the next run's ClearAbort.  Both closes are no-ops when already
// closed, and the logs reopen lazily at the next write.  Must be called
// from the interpreter thread (the thread that writes them).
void CCoordMotion::CloseLogs()
{
	TP3LogClose();
	SegLogClose();
}

// a NEW INTERPRETATION RUN truncates the log; anything else that closes
// it (program end, aborts, mid-program M-code sync buffers) reopens in
// append so one run's data survives until the interpreter is relaunched
void CCoordMotion::SegLogNewRun()
{
	SegLogClose();
	m_SegLogFresh = true;
}

void CCoordMotion::SegLogSegment(SEGMENT *p)
{
	int i;

	if (!m_SegLog)
	{
		EnsureTempDir();
		m_SegLog = fopen(TP_SEGLOG_FILE, m_SegLogFresh ? "wt" : "at");
		if (!m_SegLog) return;

		// Header on any EMPTY file, not just a truncating open.  Streaming
		// clients (coordinated jog) never call SegLogNewRun, so they open
		// in append mode - if the file was deleted between runs that
		// produced a headerless log, which readers misidentify as the
		// legacy sampled format.
		fseek(m_SegLog, 0, SEEK_END);
		if (m_SegLogFresh || ftell(m_SegLog) == 0)
			fprintf(m_SegLog, "kind,plane,ccw,seq,dx,dwell,ntrips,"
			"x0,y0,z0,a0,b0,c0,u0,v0,x1,y1,z1,a1,b1,c1,u1,v1,xc,yc,"
			"tt0,ta0,tb0,tc0,td0,tt1,ta1,tb1,tc1,td1,tt2,ta2,tb2,tc2,td2,"
			"tt3,ta3,tb3,tc3,td3,tt4,ta4,tb4,tc4,td4,tt5,ta5,tb5,tc5,td5,"
			"tt6,ta6,tb6,tc6,td6\n");
		m_SegLogFresh = false;
	}

	char kind = 'L';                          // linear (incl. rapids, cubics)
	if (p->type == SEG_ARC) kind = 'A';
	else if (p->type == SEG_DWELL || p->nTrips <= 0) kind = 'D';
	if (p->Cubic8) kind = 'K';                // per-axis cubic knot: trip cols
	                                          // tt0=duration then ta0..=the 32
	                                          // axis coeffs (8 x a,b,c,d) in
	                                          // storage order

	int plane = 0;                            // 0=XY 1=XZ 2=YZ (plane-local)
	if (p->plane == CANON_PLANE_XZ) plane = 1;
	else if (p->plane == CANON_PLANE_YZ) plane = 2;

	// A legacy linear/rapid/dwell segment is planned in CAD, but the
	// download sends its two endpoints through the kinematics and the
	// controller interpolates LINEARLY IN ACTUATOR SPACE between them
	// (fraction s/dx of the trip distance).  Log those transformed
	// endpoints so the log reproduces what is executed on a nonlinear
	// machine too; dx and the trips stay as planned, which is exactly what
	// the controller uses.  Arcs stay CAD: the arc download is a circle in
	// counts about the transformed center, meaningful only for linear
	// kinematics, where the two frames coincide.
	double P0[8] = {p->x0, p->y0, p->z0, p->a0, p->b0, p->c0, p->u0, p->v0};
	double P1[8] = {p->x1, p->y1, p->z1, p->a1, p->b1, p->c1, p->u1, p->v1};
	if (!p->ActSpace && !p->Cubic8 && p->type != SEG_ARC)
	{
		MOTION_PARAMS *MP = &Kinematics->m_MotionParams;
		double A0[MAX_ACTUATORS], A1[MAX_ACTUATORS];

		// on a transform failure (the download aborts there) keep CAD
		if (!Kinematics->TransformCADtoActuators(p->x0, p->y0, p->z0, p->a0,
				p->b0, p->c0, p->u0, p->v0, A0) &&
			!Kinematics->TransformCADtoActuators(p->x1, p->y1, p->z1, p->a1,
				p->b1, p->c1, p->u1, p->v1, A1))
		{
			for (i = 0; i < MAX_ACTUATORS; i++)
			{
				double Scale, Vel, Accel, Jerk;
				GetEffectiveActuatorLimits(MP, i, &Scale, &Vel, &Accel, &Jerk);
				P0[i] = (Scale != 0.0) ? A0[i] / Scale : 0.0;
				P1[i] = (Scale != 0.0) ? A1[i] / Scale : 0.0;
			}
		}
	}

	fprintf(m_SegLog, "%c,%d,%d,%d,%.9g,%.9g,%d",
		kind, plane, p->DirIsCCW ? 1 : 0, p->sequence_number,
		FQ(p->dx), FQ(p->dwell_time), p->nTrips);
	fprintf(m_SegLog, ",%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g",
		FQ(P0[0]), FQ(P0[1]), FQ(P0[2]), FQ(P0[3]),
		FQ(P0[4]), FQ(P0[5]), FQ(P0[6]), FQ(P0[7]));
	fprintf(m_SegLog, ",%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g",
		FQ(P1[0]), FQ(P1[1]), FQ(P1[2]), FQ(P1[3]),
		FQ(P1[4]), FQ(P1[5]), FQ(P1[6]), FQ(P1[7]));
	fprintf(m_SegLog, ",%.9g,%.9g", FQ(p->xc), FQ(p->yc));
	for (i = 0; i < 7; i++)
	{
		// Cubic8 knots re-use the trip storage beyond C[0].t as the 32
		// per-axis coefficients - dump all slots so the viewer gets them
		if (i < p->nTrips || p->Cubic8)
			fprintf(m_SegLog, ",%.9g,%.9g,%.9g,%.9g,%.9g",
				FQ(p->C[i].t), FQ(p->C[i].a), FQ(p->C[i].b),
				FQ(p->C[i].c), FQ(p->C[i].d));
		else
			fprintf(m_SegLog, ",0,0,0,0,0");
	}
	fputc('\n', m_SegLog);
	fflush(m_SegLog);   // tail must be readable live (plotter reload
	                    // mid/right-after a run) and survive any missed
	                    // close - the CRT otherwise buffers ~50 rows
}

// limit speeds based on proportion in that direction
// (in Actuator Space Limits mode: based on the actuator motion the
// direction produces at the segment's location)

int CCoordMotion::DoRateAdjustments(int i0, int i1)
{
	double tdx,tdy,tdz,tda,tdb,tdc,tdu,tdv,rate,FeedRateToUse,Accel,AccelToUse;
	double bx,by,bz,ba,bb,bc,bu,bv;

	for (int i = i0; i<=i1; i++)
	{
		if (i>=0)
		{
			SEGMENT *s=GetSegPtr(i);

			if (Kinematics->ComputeAnglesOption(i)) return 1;

			CalcBegDirectionOfSegment(s, tdx, tdy, tdz, tda, tdb, tdc, tdu, tdv);
			SegBegCADPoint(s, &bx, &by, &bz, &ba, &bb, &bc, &bu, &bv);

			if (Kinematics->MaxRateInDirectionAtPoint(bx, by, bz, ba, bb, bc, bu, bv,
				tdx, tdy, tdz, tda, tdb, tdc, tdu, tdv, &rate)) return 1;

			FeedRateToUse = GetSegPtr(i)->OrigVel;
			if (rate < FeedRateToUse) FeedRateToUse = rate;

			// limit accel based on proportion in that direction
			if (Kinematics->MaxAccelInDirectionAtPoint(bx, by, bz, ba, bb, bc, bu, bv,
				tdx, tdy, tdz, tda, tdb, tdc, tdu, tdv, &Accel)) return 1;

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

		// absolute (plane-local) point on the arc at next_theta with the
		// non-circular coordinates interpolated by fraction of arc swept
		// (needed to evaluate actuator limits at the point's location)
		double frac = (dtheta != 0.0) ? (next_theta - theta0) / dtheta : 0.0;
		double cpx = p->xc + dxc;
		double cpy = p->yc + dyc;
		double cpz = p->z0 + frac * (p->z1 - p->z0);
		double cpa = p->a0 + frac * (p->a1 - p->a0);
		double cpb = p->b0 + frac * (p->b1 - p->b0);
		double cpc = p->c0 + frac * (p->c1 - p->c0);
		double cpu = p->u0 + frac * (p->u1 - p->u0);
		double cpv = p->v0 + frac * (p->v1 - p->v0);

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
			//and the point on the arc
			tempz=cpz;
			cpz=cpx;
			cpx=cpy;
			cpy=tempz;
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
			//and the point on the arc
			tempy=cpy;
			cpy=cpx;
			cpx=cpz;
			cpz=tempy;
		}

		// limit rate based on proportion in that direction (at the point)
		if (Kinematics->MaxRateInDirectionAtPoint(cpx,cpy,cpz,cpa,cpb,cpc,cpu,cpv,
			dx,dy,dz,da,db,dc,du,dv,&rate)) return 1;
		if (rate < FeedRateToUse) FeedRateToUse = rate;

		// limit accel based on proportion in that direction
		if (Kinematics->MaxAccelInDirectionAtPoint(cpx,cpy,cpz,cpa,cpb,cpc,cpu,cpv,
			dx,dy,dz,da,db,dc,du,dv,&Accel)) return 1;
		if (Accel < AccelToUse) AccelToUse = Accel;

		// also determine centripedal acceleration which is perpindicular to
		// the motion and determines the max velocity through the curve
		if (Kinematics->MaxAccelInDirectionAtPoint(cpx,cpy,cpz,cpa,cpb,cpc,cpu,cpv,
			dxc,dyc,dzc,0.0,0.0,0.0,0.0,0.0,&Accel)) return 1;
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

	// a dwell is a true stop: finish any pending TP3 run
	if (Kinematics->m_MotionParams.ThirdOrderTP)
		if (TP3FlushRun("dwell")) return 1;

	// put something in the displayed path that can be found
	if (m_StraightTraverseCallback) m_StraightTraverseCallback(current_x, current_y, current_z,_setup.sequence_number);
	if (m_StraightTraverseSixAxisCallback) m_StraightTraverseSixAxisCallback(current_x, current_y, current_z, current_a, current_b, current_c,_setup.sequence_number);

	if (!m_Simulate || m_DoTime)
	{
		int result;

		// 3rd Order TP runs plan and download in ACTUATOR units (ActSpace
		// knots).  Insert the dwell in the SAME frame so its hold position
		// is bit-identical to the surrounding knots and the segment log
		// stays single-frame - a CAD-frame dwell amid actuator-frame knots
		// plots as a phantom position jump under nonlinear kinematics
		// (the controller itself was consistent: both frames download to
		// the same counts).  Use the flushed stream's exact endpoint when
		// available, else transform the CAD position.
		if (Kinematics->m_MotionParams.ThirdOrderTP)
		{
			double N[8];
			SEGMENT *prev = (nsegs > 0) ? GetSegPtr(nsegs-1) : NULL;

			if (prev && prev->ActSpace)
			{
				N[0]=prev->x1; N[1]=prev->y1; N[2]=prev->z1; N[3]=prev->a1;
				N[4]=prev->b1; N[5]=prev->c1; N[6]=prev->u1; N[7]=prev->v1;
			}
			else
			{
				double Acts[MAX_ACTUATORS], Scale, Vel, Accel, Jerk;
				if (Kinematics->TransformCADtoActuators(current_x, current_y, current_z,
						current_a, current_b, current_c, current_u, current_v, Acts)) return 1;
				for (int j = 0; j < 8; j++)
				{
					GetEffectiveActuatorLimits(&Kinematics->m_MotionParams, j, &Scale, &Vel, &Accel, &Jerk);
					N[j] = (Scale != 0.0) ? Acts[j]/Scale : 0.0;
				}
			}
			result = tp_insert_dwell(seconds, N[0], N[1], N[2], N[3],
				N[4], N[5], N[6], N[7], sequence_number, 0);
			if (result != 1)
			{
				SEGMENT *dw = GetSegPtr(nsegs-1);
				dw->ActSpace = TRUE;   // coords are actuator units
				// mark it downloadable NOW: a dwell is fully determined at
				// insertion (rest to rest, fixed time), and in 3rd Order TP
				// mode no later 2nd-order planning pass ever runs to promote
				// it (TP3 knots insert Done, and MaximizeSegments' i>0 scan
				// never reaches a dwell that is segment 0).  Left not-Done, a
				// LEADING G4 wedges the in-order download queue: nothing
				// downloads, the buffer never launches, and the pre-launch
				// throttle bypass lets the whole program interpret instantly
				// (observed: all 9 runs planned in 1.3s, first download at
				// the end-of-program flush).
				dw->Done = TRUE;
			}
		}
		else
			result = tp_insert_dwell(seconds, current_x, current_y, current_z, current_a, current_b, current_c, current_u, current_v, sequence_number, 0);

		if (result==1) {SetAbort(); return 1;}

		MaximizeSegments();

		if (DownloadDoneSegments()) {SetAbort(); return 1;}
	}
	return 0;
}



// issue a pass through KMotion Command

int CCoordMotion::DoKMotionCmd(const char *s, BOOL FlushBeforeUnbufferedOperation)
{
	if (m_Simulate && !m_DoTime) return 0;  // exit if we are simulating

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
	if (Kinematics->m_MotionParams.ThirdOrderTP && m_TP3Feed && !m_TP3Feed->knotEmit && !m_TP3Feed->staging)
	{
		// moves issued before the command reach the planner first
		if (m_TP3Feed->stOpen && TP3FlushStager()) return 1;

		// With a streaming run open the segment list holds KNOTS that lag
		// the waypoints by the finalization margin, so attaching to the
		// last segment here would fire the command early (by up to that
		// margin).  Hold it and mark the NEXT waypoint; TP3ConsumeSamples
		// ends a knot exactly at the sample where the mark appears and
		// attaches the command to it, so it executes at the executed
		// position the command was issued at.  The converter's own rapid
		// brackets (knotEmit) and the flush-end bracket (m_TP3Flushing)
		// attach to the knot directly as before.
		if (m_TP3Feed->sp && !m_TP3Flushing)
		{
			m_TP3Feed->cmdPending.push_back(s);
			m_TP3Feed->cmdPendingSeq.push_back(sequence_number);
			m_TP3Feed->cmdIssued++;
			TP3Log("BUFCMD         n=%d seq=%d held: %s", m_TP3Feed->cmdIssued, sequence_number, s);
			return 0;
		}
	}
	return TP3AttachBufCmdNow(s, sequence_number);
}

// the legacy attach: the command executes at the end of the last segment
// inserted so far (or before the path if there is none yet)
int CCoordMotion::TP3AttachBufCmdNow(const char *s,int sequence_number)
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
	CStringA response,response2;

	if (m_Simulate || !m_SegmentsStartedExecuting) return 0;

	int count=0;

	// stall visibility: this loop waits (silently) for the controller to
	// finish the whole buffer; a stuck data-end feedhold freezes ExecTime
	// here forever with no indication
	TP3Log("FINWAIT_BEGIN  downloaded=%8.3f exec=%8.3f", m_TotalDownloadedTime, m_TimeAlreadyExecuted);
	CHiResTimer finTimer;
	finTimer.Start();
	double finLastExec = m_TimeAlreadyExecuted, finLastAdvance = 0, finLastStallLog = 0;

	do
	{
		if (count++)
		{
			if (KMotionDLL->WriteLineReadLine("ExecTime",response.GetBufferSetLength(MAX_LINE))){SetAbort(); return 1;}
			response.ReleaseBuffer();
			if (sscanf(response, "%lf",&m_TimeAlreadyExecuted)!= 1){SetAbort(); return 1;}
			UpdateRealTimeState(m_TimeAlreadyExecuted);
			Sleep(10);

			double el = finTimer.Elapsed_Seconds();
			if (m_TimeAlreadyExecuted != finLastExec)
			{
				finLastExec = m_TimeAlreadyExecuted;
				finLastAdvance = el;
			}
			else if (el - finLastAdvance > 2.0 && el - finLastStallLog > 2.0)
			{
				finLastStallLog = el;
				TP3Log("FINWAIT_STALL  exec frozen at %8.3f for %.1fs downloaded=%8.3f",
					finLastExec, el - finLastAdvance, m_TotalDownloadedTime);
			}
		}
		
		if (KMotionDLL->WriteLineReadLine("CheckDoneBuf",response.GetBufferSetLength(MAX_LINE))) {SetAbort(); return 1;}
		response.ReleaseBuffer();

		if (response == "-1")
		{
			if (KMotionDLL->WriteLineReadLine("ExecTime",response2.GetBufferSetLength(MAX_LINE))){SetAbort(); return 1;}
			response2.ReleaseBuffer();
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
	while (response!="1");

	if (KMotionDLL->WriteLineReadLine("ExecTime",response2.GetBufferSetLength(MAX_LINE))){SetAbort(); return 1;}
	response2.ReleaseBuffer();
	if (sscanf(response2, "%lf",&m_TimeAlreadyExecuted)!= 1){SetAbort(); return 1;}
	UpdateRealTimeState(m_TimeAlreadyExecuted);

	m_SegmentsStartedExecuting = false;

	TP3Log("FINWAIT_END    waited=%7.3f exec=%8.3f", finTimer.Elapsed_Seconds(), m_TimeAlreadyExecuted);

	return 0;
}

int CCoordMotion::WaitForMoveXYZABCFinished()
{
	CStringA response;

	int count=0;
	do
	{
		if (count++) Sleep(10);
		if (KMotionDLL->WriteLineReadLine("CheckDoneXYZABC",response.GetBufferSetLength(MAX_LINE))) {SetAbort(); return 1;}
		response.ReleaseBuffer();

		if (response == "-1")
		{
			m_AxisDisabled=true;
			SetAbort();
		}

		if (CheckMotionHalt(false)) return 2;

		if (m_Abort) return 1;
	}
	while (response!="1");

	return 0;
}


int CCoordMotion::CheckMotionHalt(bool Coord)
{
	CStringA response,responsebuf;
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
			if (KMotionDLL->WriteLineReadLine("GetStopState",response.GetBufferSetLength(MAX_LINE)))  {SetAbort(); return 1;}
			response.ReleaseBuffer();
			if (m_Abort) return 1;

			// There is a chance that the motion path could complete just as the StopImmediate was
			// in progress.  So see if this was the case.

			if (Coord && response == '1')
			{
				if (KMotionDLL->WriteLineReadLine("CheckDoneBuf",responsebuf.GetBufferSetLength(MAX_LINE))) {SetAbort(); return 1;}
				responsebuf.ReleaseBuffer();

				if (responsebuf == "1")
				{
					// yes we are all finished with the buffer 
					Finished = true;
				}
				else if (responsebuf == "-1")
				{
					m_AxisDisabled=true;
					SetAbort();
				}
			}
		}
		while (!Finished && (response == '1' || response == '2'));

		if (response == '0') NotStarted=true;

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
				if (KMotionDLL->WriteLineReadLine("ExecTime",response.GetBufferSetLength(MAX_LINE))) return 1;
				response.ReleaseBuffer();
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
				KMotionDLL->DoErrMsg(KMotionDLL->Translate("Invalid buffer times on Halt"));
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

	// Canon/interpreter-level sync points (threading entry/exit, TCP or
	// tool-offset changes, user M-code functions, messages, program end,
	// unbuffered commands) call FlushSegments directly to finish
	// everything in progress.  A pending 3rd Order streaming run must be
	// completed FIRST so its decel-to-rest tail is emitted and
	// downloaded - otherwise the downloaded trajectory ends abruptly at
	// speed and the pending plan is silently discarded by the
	// TP3ClearRun below (this was the mid-program halt: RUN_END with no
	// FLUSH in the timeline log).
	if (Kinematics->m_MotionParams.ThirdOrderTP)
		if (TP3FlushRun("segment-flush")) return 1;

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

	if (m_Simulate && !m_DoTime) return 0;  // exit if we are simulating

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
			

		if (!m_Simulate && FlushWriteLineBuffer()) return 1;  // flush any segments at the end

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
		if(!m_Simulate && KMotionDLL->WriteLine("FlushBuf")) {SetAbort(); return 1;}

		tp_init();
		TP3ClearRun();
		DownloadInit();
	}
	return 0;
}


int CCoordMotion::LaunchCoordMotion()
{
	CStringA s;

	if (KMotionDLL->WriteLineReadLine("CheckDoneBuf",s.GetBufferSetLength(MAX_LINE))) {SetAbort(); return 1;}
	s.ReleaseBuffer();

	if (s == "-1")
	{
		m_AxisDisabled=true;
		SetAbort();
		return 1;
	}

	if (m_ThreadingMode)  // Launch coordinated motion in spindle sync mode ?
	{
		if (fabs(m_ThreadingBaseSpeedRPS) < 1e-9)
		{
			MessageBoxW(NULL, KMotionDLL->Translate("Error Threading with Zero Speed"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
			SetAbort();
			return 1;
		}

		s.Format("TrigThread %.6f",m_ThreadingBaseSpeedRPS);
		if(KMotionDLL->WriteLine(s)){SetAbort(); return 1;}
	}
	else  // no normal coordinated motion
	{
		if(KMotionDLL->WriteLine("ExecBuf")){SetAbort(); return 1;}
	}
	return 0;
}



// intitalize everything related to downloading and look ahead times


// G61/G61.1/G64 path control.  Commits pending legacy segments first so
// they are rounded under the OLD tolerance (program-order semantics);
// the TP3 stream is NOT flushed - tolerance and stop behavior are
// per-waypoint and mode changes ride the stream without a stop.
void CCoordMotion::SetPathMode(CANON_MOTION_MODE mode, double tolInches)
{
	if (mode == m_PathMode && tolInches == m_PathTol) return;

	CommitPendingSegments(false);
	// a staged 3rd Order chord is planned under the OLD tolerance / mode
	if (Kinematics->m_MotionParams.ThirdOrderTP) TP3FlushStager();

	m_PathMode = mode;
	m_PathTol = (mode == CANON_CONTINUOUS) ? tolInches : 0.0;

	// legacy planner reads a planner-global copy (survives
	// SetTrajectoryPlannerParams refreshes of the UI values)
	tp_set_corner_tol_override(PathExact() ? 0.0 : m_PathTol);
	tp_set_exact_stop(m_PathMode == CANON_EXACT_STOP);

	TP3Log("PATHMODE       mode=%s tol=%.5f (%s)",
		mode == CANON_EXACT_PATH ? "G61" :
		mode == CANON_EXACT_STOP ? "G61.1" : "G64",
		m_PathTol,
		m_PathTol < 0.0 ? "machine default" : "programmed");
}

double CCoordMotion::EffectiveCornerTol()
{
	if (m_PathMode != CANON_CONTINUOUS) return 0.0;
	return (m_PathTol >= 0.0) ? m_PathTol : Kinematics->m_MotionParams.CornerTol;
}

// ---- CAD-space fairing for DRAWING the TP3 path --------------------------
// Mirrors TP3EmitFairedVertex's quadratic-Bezier corner blends in CAD
// space so the G-view shows the tolerance-dependent rounding (the raw
// programmed polyline draws sharp corners no matter the tolerance).
// The blends spend up to TP3_FAIR_FRAC of the local tolerance; the FIR
// filter may cut further inside by whatever the blend left unspent, so
// the true path lies between the drawn curve and the full tolerance from
// the programmed vertex.  Corner-stop / G61 vertices are drawn exact because
// TP3FlushRun finalizes the drawing at every run split.

void CCoordMotion::TP3DrawEmitPiece(const CTP3DrawWpt &attr, const double *pt)
{
	if (!attr.nocb)
	{
		if (attr.rapid)
		{
			if (m_StraightTraverseCallback) m_StraightTraverseCallback(pt[0],pt[1],pt[2],attr.seqT);
			if (m_StraightTraverseSixAxisCallback) m_StraightTraverseSixAxisCallback(pt[0],pt[1],pt[2],pt[3],pt[4],pt[5],attr.seqT);
		}
		else
		{
			if (m_StraightFeedCallback) m_StraightFeedCallback(attr.F,pt[0],pt[1],pt[2],attr.seq,attr.id);
			if (m_StraightFeedSixAxisCallback) m_StraightFeedSixAxisCallback(attr.F,pt[0],pt[1],pt[2],pt[3],pt[4],pt[5],attr.seq,attr.id);
		}
	}
	for (int i=0;i<8;i++) m_TP3DrawLast[i]=pt[i];
}

void CCoordMotion::TP3DrawVertex(const CTP3DrawWpt &wm1, const CTP3DrawWpt &w0, const CTP3DrawWpt &wp1)
{
	double d1[8], d2[8], s1=0, s2=0;
	int i;

	for (i=0;i<8;i++)
	{
		d1[i] = w0.p[i] - wm1.p[i];  s1 += d1[i]*d1[i];
		d2[i] = wp1.p[i] - w0.p[i];  s2 += d2[i]*d2[i];
	}
	s1 = sqrt(s1);  s2 = sqrt(s2);
	if (s1 <= 0.0 || s2 <= 0.0) { TP3DrawEmitPiece(w0, w0.p); return; }

	double dDn = 0;
	for (i=0;i<8;i++)
	{
		d1[i] /= s1;  d2[i] /= s2;
		double dd = d2[i] - d1[i];
		dDn += dd*dd;
	}
	dDn = sqrt(dDn);

	double frac = TP3_FAIR_FRAC;                  // mirror TP3ComputeBlend
	if (frac > 1.0 - TP3_FIR_MIN_FRAC) frac = 1.0 - TP3_FIR_MIN_FRAC;
	double budget = frac * ((w0.tol < wp1.tol) ? w0.tol : wp1.tol);
	double L = 0.0;
	if (dDn >= 1e-9)
	{
		L = 4.0 * budget / dDn;
		if (L > 0.499*s1) L = 0.499*s1;
		if (L > 0.499*s2) L = 0.499*s2;
	}

	if (L <= 1e-9)   // (nearly) collinear or zero tolerance: exact vertex
	{
		TP3DrawEmitPiece(w0, w0.p);
		return;
	}

	double P0[8], P2[8], B[8];
	for (i=0;i<8;i++)
	{
		P0[i] = w0.p[i] - d1[i]*L;
		P2[i] = w0.p[i] + d2[i]*L;
	}
	TP3DrawEmitPiece(w0, P0);            // straight run into the blend

	// facet the blend by the machine FacetAngle
	double sinHalf = dDn/2.0;  if (sinHalf > 1.0) sinHalf = 1.0;
	double theta = 2.0 * asin(sinHalf);
	double fa = Kinematics->m_MotionParams.FacetAngle * PI / 180.0;
	if (fa < 1e-4) fa = 1e-4;
	int n = (int)ceil(theta/fa);
	if (n < 2) n = 2;
	if (n > 32) n = 32;
	for (int q=1;q<=n;q++)
	{
		double tb = (double)q/n;
		double b0 = (1-tb)*(1-tb), b1 = 2*tb*(1-tb), b2 = tb*tb;
		for (i=0;i<8;i++) B[i] = P0[i]*b0 + w0.p[i]*b1 + P2[i]*b2;
		TP3DrawEmitPiece((tb < 0.5) ? w0 : wp1, B);
	}
}

// Draw the held-back final segment to the exact endpoint and reset.
// Called at every run boundary (TP3FlushRun), including corner-stop
// splits, so stopped vertices are drawn sharp.
void CCoordMotion::TP3DrawFlush()
{
	if (m_TP3DrawHave >= 2)
		TP3DrawEmitPiece(m_TP3DrawW1, m_TP3DrawW1.p);
	m_TP3DrawHave = 0;
}

// Draw one downloaded knot endpoint: normalized actuator -> actuator
// counts (Scale) -> inverse kinematics -> CAD -> drawing callbacks.
// cadAnchor is the CAD position of the sample's source waypoint,
// carried through the planner: seeding the inversion from it guarantees
// the solve starts in the correct solution branch (mechanisms like the
// 3Link Z lift have a second, folded-up pose with the same servo
// angles) and typically converges in 1-2 iterations.  A sample the
// smoothing filter pushed slightly outside the reachable workspace
// (possible right at a fold) fails to invert and is simply not drawn.
// F is the feedrate attributed to the knot; rapid selects the traverse
// callbacks (attributed independently of F - fairing clamps F on blend
// waypoints without changing the G0 attribution).
void CCoordMotion::TP3DrawKnotCAD(const double *pNorm, const double *cadAnchor, double F, bool rapid, int seq, int ID)
{
	MOTION_PARAMS *MP=&Kinematics->m_MotionParams;
	double Acts[MAX_ACTUATORS], Scale, Vel, Accel, Jerk;
	double x,y,z,a,b,c,u,v;
	int i;

	if (rapid)
	{
		if (!m_StraightTraverseCallback && !m_StraightTraverseSixAxisCallback) return;
	}
	else
	{
		if (!m_StraightFeedCallback && !m_StraightFeedSixAxisCallback) return;
	}

	for (i=0;i<MAX_ACTUATORS;i++)
	{
		GetEffectiveActuatorLimits(MP,i,&Scale,&Vel,&Accel,&Jerk);
		Acts[i] = (Scale != 0.0) ? pNorm[i]*Scale : 0.0;
	}

	// seed + budget + solve as ONE atomic sequence: a DRO inversion on
	// the GUI thread must not run in the middle (torn seed, or picking
	// up our probe budget)
	Kinematics->InvLock();

	// seed the solve from the CAD anchor
	for (i=0;i<6;i++) Kinematics->m_InvWarm[i] = cadAnchor[i];
	Kinematics->m_InvWarmValid = true;

	// short circuit: after a few consecutive undrawable poses (a program
	// out of physical reach) stop paying for the full solve + careful
	// fallback on every knot - probe with a small iteration budget
	// instead.  The anchor seed converges in 1-2 iterations the moment
	// the path returns to reach, which also resets the streak.
	if (m_TP3DrawFailStreak >= 3) Kinematics->m_InvBudgetIters = 25;
	int fail = Kinematics->TransformActuatorstoCAD(Acts,&x,&y,&z,&a,&b,&c,&u,&v);
	Kinematics->m_InvBudgetIters = 0;

	Kinematics->InvUnlock();

	if (fail)
	{
		m_TP3DrawFailStreak++;
		return;
	}
	m_TP3DrawFailStreak = 0;

	if (rapid)
	{
		if (m_StraightTraverseCallback) m_StraightTraverseCallback(x,y,z,seq);
		if (m_StraightTraverseSixAxisCallback) m_StraightTraverseSixAxisCallback(x,y,z,a,b,c,seq);
	}
	else
	{
		if (m_StraightFeedCallback) m_StraightFeedCallback(F,x,y,z,seq,ID);
		if (m_StraightFeedSixAxisCallback) m_StraightFeedSixAxisCallback(F,x,y,z,a,b,c,seq,ID);
	}
}

void CCoordMotion::DownloadInit()
{
	// NOTE: called from the CCoordMotion CONSTRUCTOR (members like
	// Kinematics are not initialized yet) - do not touch members that are
	// set up later.  The TP3 timeline log opens lazily at the first
	// streaming run instead.
	m_nsegs_downloaded=0;
	m_TotalDownloadedTime = m_TimeAlreadyExecuted = 0.0;
	m_ThreadingMode=false;
	m_StarveTimeSent=0.0;
	m_StarveHead=m_StarveTail=0;
	m_StarveOverflow=false;
}

// Reset the host planner/download state after the firmware coordinated
// buffer has been abandoned (StopImmediate2): same reset FlushSegments
// finishes with, without raising the global abort flag.
void CCoordMotion::AbandonCoordBuffer()
{
	tp_init();
	TP3ClearRun();
	DownloadInit();
	ClearWriteLineBuffer();
	SegLogClose();   // release the segment log between streaming runs so
	                 // external viewers can open the file freely
}

// Launch the downloaded coordinated buffer immediately if not already
// started (mirrors OutputSegment's auto-launch site, which fires only
// when planned downloaded time exceeds TPLookahead).
int CCoordMotion::LaunchIfNotStarted()
{
	if (m_Simulate || m_SegmentsStartedExecuting || m_nsegs_downloaded <= 0)
		return 0;

	if (FlushWriteLineBuffer()) return 1;
	if (LaunchCoordMotion()) return 1;
	m_SegmentsStartedExecuting = true;
	segments_executing = segments;  // MUST accompany m_SegmentsStartedExecuting:
	                                // UpdateRealTimeState dereferences it as soon
	                                // as execution is flagged started
	return 0;
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

	// end of program/interpretation: finish any pending TP3 run
	if (Kinematics->m_MotionParams.ThirdOrderTP)
		if (TP3FlushRun("execution-stop")) return 1;

	if (m_Abort) return 1;

	if (!m_Halt)
		if (FlushSegments()) return 1;

	int result = WaitForSegmentsFinished();

#ifdef DEBUG_DOWNLOAD
	CloseDiag();
#endif

	TP3Log("EXECUTION_STOP result=%d exec=%8.3f", result, m_TimeAlreadyExecuted);
	TP3LogClose();
	SegLogClose();

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


// Worst-case time to stop the motion contained in one segment: per-axis
// peak velocity through the segment's trip states, put through the same
// stop-time formula the firmware's DecelTimeForAxis uses
// (v/Accel + sqrt(8v/Jerk)) with the uploaded per-axis Rapid Accel/Jerk.
// The ratios are scale-invariant so CAD units work directly.  (ActSpace
// segments are in normalized actuator units, so their stop capability
// is the ACTUATOR Accel/Jerk limits - same units as the knot velocities.
// Using the CAD-axis Rapid params for those overstated the stop time by
// the Counts-per-unit/ActScale ratio - orders of magnitude on machines
// like rotary-servo kinematics - which pinned the firmware's starvation
// TimeBase gate near zero for slow streamed content such as jogs.)
double CCoordMotion::SegWorstStopTime(SEGMENT *p)
{
	int i,a;
	MOTION_PARAMS *MP=&Kinematics->m_MotionParams;

	double A[8]={MP->MaxRapidAccelX,MP->MaxRapidAccelY,MP->MaxRapidAccelZ,MP->MaxRapidAccelA,
				 MP->MaxRapidAccelB,MP->MaxRapidAccelC,MP->MaxRapidAccelU,MP->MaxRapidAccelV};
	double J[8]={MP->MaxRapidJerkX,MP->MaxRapidJerkY,MP->MaxRapidJerkZ,MP->MaxRapidJerkA,
				 MP->MaxRapidJerkB,MP->MaxRapidJerkC,MP->MaxRapidJerkU,MP->MaxRapidJerkV};
	double vax[8]={0,0,0,0,0,0,0,0};

	if (p->ActSpace || p->Cubic8)
	{
		double Scale, Vel;
		for (a=0;a<8;a++)
		{
			GetEffectiveActuatorLimits(MP, a, &Scale, &Vel, &A[a], &J[a]);
			if (!TP3SlotInUse(a)) A[a] = J[a] = 0.0;   // slot not in use
		}
	}

	if (p->Cubic8)
	{
		// per-axis cubics: axis velocity is the direct derivative; peak
		// is at an endpoint or the interior critical point
		const double *K = SEG_CUBIC8_COEFFS(p);
		double T = p->C[0].t;
		for (a=0;a<8;a++)
		{
			const double *k=K+4*a;
			double v0=fabs(k[2]);
			double v1=fabs((3.0*k[0]*T + 2.0*k[1])*T + k[2]);
			double v=(v0>v1)?v0:v1;
			if (fabs(k[0])>1e-30)
			{
				double tc=-k[1]/(3.0*k[0]);
				if (tc>0.0 && tc<T)
				{
					double vc=fabs((3.0*k[0]*tc + 2.0*k[1])*tc + k[2]);
					if (vc>v) v=vc;
				}
			}
			vax[a]=v;
		}
	}
	else
	{
		// peak PATH speed over the trip-state cubics
		double smax=0;
		for (i=0;i<p->nTrips;i++)
		{
			double T=p->C[i].t;
			if (T<=0.0) continue;
			double a3=p->C[i].a, b2=p->C[i].b, c1=p->C[i].c;
			double v0=fabs(c1);
			double v1=fabs((3.0*a3*T + 2.0*b2)*T + c1);
			double v=(v0>v1)?v0:v1;
			if (fabs(a3)>1e-30)
			{
				double tc=-b2/(3.0*a3);
				if (tc>0.0 && tc<T)
				{
					double vc=fabs((3.0*a3*tc + 2.0*b2)*tc + c1);
					if (vc>v) v=vc;
				}
			}
			if (v>smax) smax=v;
		}

		// distribute the path speed onto axes by direction content; an
		// arc's plane-local xy directions rotate, so bound both by the
		// full path speed
		double d = p->dx;
		if (d > 0.0)
		{
			double dd[8]={p->x1-p->x0,p->y1-p->y0,p->z1-p->z0,p->a1-p->a0,
						  p->b1-p->b0,p->c1-p->c0,p->u1-p->u0,p->v1-p->v0};
			for (a=0;a<8;a++) vax[a]=smax*fabs(dd[a])/d;
		}
		if (p->type==SEG_ARC) { vax[0]=smax; vax[1]=smax; }
	}

	double stopT=0;
	for (a=0;a<8;a++)
	{
		if (vax[a]<=0.0 || A[a]<=0.0) continue;
		double t=vax[a]/A[a];
		if (J[a]>0.0) t+=sqrt(8.0*vax[a]/J[a]);
		if (t>stopT) stopT=t;
	}
	return stopT;
}

// Whether actuator slot i takes part in 3rd Order planning.
//
// Scale 0 marks a slot as absent (ActuatorLimits table or legacy
// CountsPerInch).
//
// Otherwise the COORDINATE SYSTEM is authoritative in BOTH mappings -
// actuator slot i is CS coordinate i (DefineCS maps slots to channels), and
// exactly like the legacy planner an axis that is not part of the defined
// CS never moves, so it must not constrain the plan (its limits would
// widen the machine-wide smoothing filter: with the effective-Accel
// windows an idle slot's Vel/Accel/Jerk sets the window unless excluded)
// and must not be required to have Vel/Accel/Jerk configured.  CS
// definitions commonly change on the fly (custom M-code wait/sync
// actions); x_axis..v_axis are re-read from the board after every such
// sync (ReadAndSyncCurPositions) and at execution start, and this test
// runs at every run start (first waypoint after a flush).  If no CS has
// ever been defined or read (simulation without a board) all axes are
// considered in use.
bool CCoordMotion::TP3SlotInUse(int i)
{
	MOTION_PARAMS *MP=&Kinematics->m_MotionParams;
	double Scale, Vel, Accel, Jerk;

	GetEffectiveActuatorLimits(MP, i, &Scale, &Vel, &Accel, &Jerk);
	if (Scale == 0.0) return false;

	// a slot outside the defined Coordinate System cannot move and is not
	// constrained.  m_DefineCS_known (not the cache flag m_DefineCS_valid,
	// which execution start deliberately clears to force a re-read and
	// which a plain read never sets) - the axis numbers are meaningful
	// from the first read/define onward.
	if (m_DefineCS_known)
	{
		int ch;
		switch (i)
		{
		case 0:  ch = x_axis; break;
		case 1:  ch = y_axis; break;
		case 2:  ch = z_axis; break;
		case 3:  ch = a_axis; break;
		case 4:  ch = b_axis; break;
		case 5:  ch = c_axis; break;
		case 6:  ch = u_axis; break;
		default: ch = v_axis; break;
		}
		if (ch < 0) return false;
	}
	return true;
}

// Estimate of the 3rd Order streaming planner's emission (finalization)
// margin: the span of plan it withholds - decel shadow plus jerk-filter
// history, see StreamingTrajectoryPlanner's constructor and emitCommon -
// before ANY samples finalize.  A client that streams content in real
// time (the coordinated jog) must be allowed to buffer MORE than this
// much content ahead of execution or nothing will ever emit, download,
// or move.  Mirrors the planner's construction: slots with Scale==0 are
// not part of the coordinate system and are excluded.
// margin = max(decelMax+3Tw, tInLf+3Tw) + histTime with Tw=3R and
// histTime = 2*(N1+N2)*dt ~= 6R where R = max(Aeff/Jerk) and decelMax =
// max(Vel/Aeff), Aeff the planner's effective Accel (TP3::effectiveAmax):
// ~= decelMax + 9R + 6R, estimated with slack as decelMax + 16R.
double CCoordMotion::TP3EmissionMarginEstimate()
{
	MOTION_PARAMS *MP=&Kinematics->m_MotionParams;
	double Scale, Vel, Accel, Jerk, decelMax=0, Rmax=0;

	if (!MP->ThirdOrderTP) return 0.0;

	for (int i=0;i<8;i++)
	{
		GetEffectiveActuatorLimits(MP, i, &Scale, &Vel, &Accel, &Jerk);
		if (!TP3SlotInUse(i) || Vel <= 0.0 || Accel <= 0.0 || Jerk <= 0.0) continue;
		double Aeff = TP3::effectiveAmax(Vel, Accel, Jerk);
		if (Vel/Aeff > decelMax) decelMax = Vel/Aeff;
		if (Aeff/Jerk > Rmax) Rmax = Aeff/Jerk;
	}

	return decelMax + 16.0*Rmax;
}

// Worst-case time to ramp the PLANNED content to rest at the planner's
// own per-actuator limits (v/a + a/j with the planner's effective Accel,
// TP3::effectiveAmax).  The release stop of a streaming
// jog uses this so the channel Accel/Jerk parameters (bare-motor
// capability - used for independent moves and the firmware's
// conservative channel-based stop cushion) do not govern how long a jog
// takes to stop: the content was planned within these limits, so it can
// be ramped to rest within them too.
double CCoordMotion::TP3PlannedStopTime()
{
	MOTION_PARAMS *MP=&Kinematics->m_MotionParams;
	double Scale, Vel, Accel, Jerk, T=0.1;

	for (int i=0;i<8;i++)
	{
		GetEffectiveActuatorLimits(MP, i, &Scale, &Vel, &Accel, &Jerk);
		if (!TP3SlotInUse(i) || Vel <= 0.0 || Accel <= 0.0 || Jerk <= 0.0) continue;
		double Aeff = TP3::effectiveAmax(Vel, Accel, Jerk);
		double t = Vel/Aeff + Aeff/Jerk;
		if (t > T) T = t;
	}
	return T;
}

// The margin the streaming planner ACTUALLY computed at its last replan.
// The a-priori estimate above cannot capture the end-fade TIME term
// (fixed arc length divided by the path speed - unbounded as the speed
// drops near a kinematic fold or on a slow jog), so pacing callers that
// must buffer past the margin (coordinated jog) read the live value.
double CCoordMotion::TP3ActualEmissionMargin()
{
	double est = TP3EmissionMarginEstimate();
	if (m_TP3Feed && m_TP3Feed->sp)
	{
		double m = m_TP3Feed->sp->emissionMargin();
		if (m > est) return m;
	}
	return est;
}

// Lower the declared starve time as execution consumes the buffer: the
// declaration only needs to cover the content still AHEAD of execution -
// ratcheting alone would leave a days-long job pinned at the stop time
// of its fastest move ever, demanding that much buffered data forever.
// Lowering through the same FIFO is safe: by the time the command
// arrives at the controller, even less remains than when it was queued.
// m_TimeAlreadyExecuted may be stale-low between ExecTime polls, which
// only makes the result conservative (prunes less).
void CCoordMotion::UpdateStarveTime()
{
	if (m_Simulate || m_StarveOverflow || m_StarveTimeSent <= 0.0) return;

	// prune records already executed
	while (m_StarveTail != m_StarveHead &&
		   m_StarveEndT[m_StarveTail] <= m_TimeAlreadyExecuted)
		m_StarveTail = (m_StarveTail+1)%STARVE_LIST_SIZE;

	double need = 0;
	for (int i=m_StarveTail; i!=m_StarveHead; i=(i+1)%STARVE_LIST_SIZE)
		if (m_StarveStopT[i] > need) need = m_StarveStopT[i];

	double decl = need*1.1 + 0.01;
	// hysteresis: only send when meaningfully lower
	if (decl < m_StarveTimeSent - 0.05 && decl < m_StarveTimeSent*0.7)
	{
		CStringA ss;
		m_StarveTimeSent = decl;
		ss.Format("SetStarveTime %.4f", decl);
		PutWriteLineBuffer(ss, 0.0);
	}
}

int CCoordMotion::OutputSegment(int iseg)
{
	CStringA s;
	MOTION_PARAMS *MP=&Kinematics->m_MotionParams;

	static bool LastWasLinear=false;  // After an arc we need to specify all.
	bool DidThisLinear=false;  // multiple segments can use the same start/end point only with diff accel

	// output the segments (3rd Order TP knots arrive with trips precomputed)

	if (!GetSegPtr(iseg)->PrecomputedTrips) tp_calc_seg_trip_states(iseg);

	// finalize the streaming run's exec baseline the moment downloads reach
	// its first segment: downloads are strictly in-order, so everything
	// counted into m_TotalDownloadedTime before iseg==firstSeg is pre-run
	// content - including a prior flush's tail that was still undownloaded
	// when the run was created (the provisional baseline missed it, which
	// over-counted (exec - execBase) and could false-fire the underflow
	// canary on a healthy job)
	if (m_TP3Feed && !m_TP3Feed->baseFinal && m_TP3Feed->firstSeg >= 0
		&& iseg >= m_TP3Feed->firstSeg)
	{
		m_TP3Feed->execBase = m_TotalDownloadedTime;
		m_TP3Feed->baseFinal = true;
	}

	// optional segment log of everything sent to the controller.  Also
	// active in Simulate with "Do Times": segments are fully planned
	// (trip states computed) there, so the log can be produced without
	// running the job on hardware.  NOTE: the log deliberately does NOT
	// reset with each coordinated buffer (m_nsegs_downloaded==0) -
	// mid-program M-code syncs open new buffers; only a new
	// interpretation run truncates (SegLogNewRun).
	if (MP->LogSegments && (!m_Simulate || m_DoTime))
		SegLogSegment(GetSegPtr(iseg));


#if 0 
	// Output Segment buffer to a file
	FILE *f;
	
	if (iseg==0)
		_tfopen_s(&f, L"c:\\Temp\\data.txt", _T("wt,ccs=UTF-8"));
	else
		_tfopen_s(&f, L"c:\\Temp\\data.txt", _T("at,ccs=UTF-8"));

	int k = iseg;
	fwprintf(f, L"segment,%d,type,%d,Done,%d,StopRequired,%d,ChangeInDirection,%.6f,vel,%.6f,MaxVel,%.6f,dx,%.6f,x,%.6f,y,%.6f,T,%.6f\n",
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

	if (m_nsegs_downloaded==0)
	{
		LastWasLinear=false;
		m_StarveTimeSent=0.0;   // new buffer: OpenBuf reverts the firmware
		                        // to its conservative default
	}

	// determine if the segment should be included in feed statistics
	bool DoFeedStats = !m_Simulate && ((p->type == SEG_LINEAR && !PureAngle(p)) || p->type == SEG_ARC);

	// Declare the buffer's worst-case stop time to the controller BEFORE
	// downloading content that raises it (SetStarveTime): the firmware's
	// buffer-starvation feed-forward then only demands the data actually
	// needed to stop what is REALLY in the buffer, instead of assuming a
	// stop from MAX velocity on the worst axis.  Slow motion (jogs, fine
	// feeds) can then start almost immediately while fast content still
	// gets its full margin.  Ratcheted up only, per buffer: everything
	// already downloaded keeps its guarantee even if the host stalls,
	// because the increase is queued AHEAD of the faster segments in the
	// same FIFO.
	if (!m_Simulate)
	{
		// lower first if execution has consumed the fast content...
		UpdateStarveTime();

		// ...then raise BEFORE downloading anything faster
		double segStop = SegWorstStopTime(p);
		if (segStop > m_StarveTimeSent)
		{
			CStringA ss;
			m_StarveTimeSent = segStop*1.1 + 0.01;  // headroom reduces chatter
			ss.Format("SetStarveTime %.4f", m_StarveTimeSent);
			if (PutWriteLineBuffer(ss, 0.0)) return 1;
		}

		// record this segment for later lowering (buffer time at its
		// end + its stop time); on ring overflow degrade gracefully to
		// ratchet-only for the remainder of the buffer
		double segTime = 0;
		for (int k=0; k<p->nTrips; k++) segTime += p->C[k].t;
		int next = (m_StarveHead+1)%STARVE_LIST_SIZE;
		if (next == m_StarveTail)
			m_StarveOverflow = true;
		else if (!m_StarveOverflow)
		{
			m_StarveEndT[m_StarveHead] = m_TotalDownloadedTime + segTime;
			m_StarveStopT[m_StarveHead] = segStop;
			m_StarveHead = next;
		}
	}

	for (int i=0; i<p->nTrips; i++)
	{
		if (!m_Simulate && p->C[i].t > 0.0)  // discard zero time segments
		{
			if (p->Cubic8)
			{
				// per-axis cubic knot -> one CHex8 (TRAJECTORY_CUBIC8,
				// requires updated KFLOP/Kogna firmware).  Coefficients
				// are actuator units: position scales linearly so ALL
				// FOUR coefficients of an axis get that axis's Scale.
				float FC[33];
				double Scale, Vel, Accel, Jerk;
				const double *K = SEG_CUBIC8_COEFFS(p);

				for (int j = 0; j < 8; j++)
				{
					GetEffectiveActuatorLimits(MP, j, &Scale, &Vel, &Accel, &Jerk);
					FC[4*j+0] = (float)(K[4*j+0]*Scale);
					FC[4*j+1] = (float)(K[4*j+1]*Scale);
					FC[4*j+2] = (float)(K[4*j+2]*Scale);
					FC[4*j+3] = (float)(K[4*j+3]*Scale);
				}
				FC[32] = (float)(p->C[i].t);

				int *IC = (int *)FC;
				s.Format("CHex8 %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
					IC[0], IC[1], IC[2], IC[3], IC[4], IC[5], IC[6], IC[7],
					IC[8], IC[9], IC[10], IC[11], IC[12], IC[13], IC[14], IC[15],
					IC[16], IC[17], IC[18], IC[19], IC[20], IC[21], IC[22], IC[23],
					IC[24], IC[25], IC[26], IC[27], IC[28], IC[29], IC[30], IC[31],
					IC[32]);

				// CHex8 does not stage endpoints in the firmware (LinF
				// untouched) so a following linear must specify everything
				LastWasLinear = false;
			}
			else if (p->type == SEG_LINEAR || p->type == SEG_RAPID || p->type == SEG_DWELL)
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

				// 3rd Order TP knots: use the metric the cubic was built for
				if (p->PrecomputedTrips) d = p->dx;

				if (d>0.0)
					invd = 1.0/d; // inverse total length
				else    
					invd = 0.0f;

				//  send as hex binary 32 bit floats


				float FloatUVArray[4],FloatArray[17];
				double Acts[MAX_ACTUATORS];

				if (p->ActSpace)
				{
					// coordinates are already actuator units; apply scale only
					double Scale, Vel, Accel, Jerk;
					double C0[8]={p->x0,p->y0,p->z0,p->a0,p->b0,p->c0,p->u0,p->v0};
					for (int j=0;j<MAX_ACTUATORS;j++)
					{
						GetEffectiveActuatorLimits(MP,j,&Scale,&Vel,&Accel,&Jerk);
						Acts[j] = C0[j]*Scale;
					}
				}
				else if (Kinematics->TransformCADtoActuators(p->x0,p->y0,p->z0,p->a0,p->b0,p->c0,p->u0,p->v0,Acts)) return 1;

				FloatArray[0]  = (float)Acts[0];
				FloatArray[1]  = (float)Acts[1];
				FloatArray[2]  = (float)Acts[2];
				FloatArray[3]  = (float)Acts[3];
				FloatArray[4]  = (float)Acts[4];
				FloatArray[5]  = (float)Acts[5];
				FloatUVArray[0] = (float)Acts[6];
				FloatUVArray[1] = (float)Acts[7];

				if (p->ActSpace)
				{
					double Scale, Vel, Accel, Jerk;
					double C1[8]={p->x1,p->y1,p->z1,p->a1,p->b1,p->c1,p->u1,p->v1};
					for (int j=0;j<MAX_ACTUATORS;j++)
					{
						GetEffectiveActuatorLimits(MP,j,&Scale,&Vel,&Accel,&Jerk);
						Acts[j] = C1[j]*Scale;
					}
				}
				else if (Kinematics->TransformCADtoActuators(p->x1,p->y1,p->z1,p->a1,p->b1,p->c1,p->u1,p->v1,Acts)) return 1;

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
				ds.Format(L"Linear %f %d %d\n",DTimer.Elapsed_Seconds(),iseg,i);
				PutString(ds);
#endif


				if (!LastWasLinear)  // must specify all if first or there was an arc
				{
					if (u_axis >= 0 || v_axis >= 0)
						s.Format("LinearHexEx %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
							Int[0], Int[1], Int[2], Int[3], Int[4], Int[5], IntUV[0], IntUV[1], Int[6], Int[7], Int[8], Int[9], Int[10], Int[11], IntUV[2], IntUV[3], Int[12], Int[13], Int[14], Int[15], Int[16]);
					else
						s.Format("LinearHex %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
							Int[0], Int[1], Int[2], Int[3], Int[4], Int[5], Int[6], Int[7], Int[8], Int[9], Int[10], Int[11], Int[12], Int[13], Int[14], Int[15], Int[16]);

					DidThisLinear=LastWasLinear=true;
				}
				else if (!DidThisLinear) // new linear so we must specify the new endpoint
				{
					if (u_axis >= 0 || v_axis >= 0)
						s.Format("LHexEx1 %X %X %X %X %X %X %X %X %X %X %X %X %X", Int[6], Int[7], Int[8], Int[9], Int[10], Int[11], IntUV[2], IntUV[3], Int[12], Int[13], Int[14], Int[15], Int[16]);
					else
						s.Format("LHex1 %X %X %X %X %X %X %X %X %X %X %X", Int[6], Int[7], Int[8], Int[9], Int[10], Int[11], Int[12], Int[13], Int[14], Int[15], Int[16]);
							
					DidThisLinear=true;
				}
				else
				{
					s.Format("LHex2 %X %X %X %X %X",
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
						s.Format("ArcHexEx %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
							Int[0], Int[1], Int[2], Int[3], Int[4], Int[5], Int[6], Int[7], Int[8], Int[9], IntUV[0], IntUV[1], Int[10], Int[11], Int[12], Int[13], IntUV[2], IntUV[3], Int[14], Int[15], Int[16], Int[17], Int[18]);
					else
						s.Format("ArcHex %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
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
						s.Format("ArcHexZXEx %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
							Int[0], Int[1], Int[2], Int[3], Int[4], Int[5], Int[6], Int[7], Int[8], Int[9], IntUV[0], IntUV[1], Int[10], Int[11], Int[12], Int[13], IntUV[2], IntUV[3], Int[14], Int[15], Int[16], Int[17], Int[18]);
					else
						s.Format("ArcHexZX %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
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
						s.Format("ArcHexYZEx %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
							Int[0], Int[1], Int[2], Int[3], Int[4], Int[5], Int[6], Int[7], Int[8], Int[9], IntUV[0], IntUV[1], Int[10], Int[11], Int[12], Int[13], IntUV[2], IntUV[3], Int[14], Int[15], Int[16], Int[17], Int[18]);
					else
						s.Format("ArcHexYZ %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
							Int[0], Int[1], Int[2], Int[3], Int[4], Int[5], Int[6], Int[7], Int[8], Int[9], Int[10], Int[11], Int[12], Int[13], Int[14], Int[15], Int[16], Int[17], Int[18]);
				}


#ifdef DEBUG_DOWNLOAD
				ds.Format("Arc %f %d %d\n",DTimer.Elapsed_Seconds(),iseg,i);
				PutString(ds);
#endif
			}

			if (PutWriteLineBuffer(s,p->C[i].t)) return 1;
		
			m_TotalDownloadedTime += p->C[i].t;  // sum all downloaded times

#ifdef DEBUG_DOWNLOAD
				ds.Format("Done %f %d %d %f\n",DTimer.Elapsed_Seconds(),iseg,i,m_TotalDownloadedTime);
				PutString(ds);
#endif
		}
		if (m_Simulate && m_DoTime)
			m_TotalDoTime += p->C[i].t;  // sum all downloaded times for simulation

		if (DoFeedStats)
		{
			m_TotalFeedTime += p->C[i].t;        // sum all downloaded Feed Times for Tool Wear
			if (i==0) m_TotalFeedDist += p->dx;  // sum all downloaded Feed Distances for Tool Wear
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
			KMotionDLL->DoErrMsg(L"Wrong Buffer Times",MB_ICONEXCLAMATION|MB_OK);   
			return 1;
		}
	}
#endif




	m_nsegs_downloaded++;

	if (m_Simulate && !m_SegmentsStartedExecuting)
	{
		m_SegmentsStartedExecuting = true;
		segments_executing = segments;
	}
	else if (!m_SegmentsStartedExecuting && (m_TotalDownloadedTime  - m_TimeAlreadyExecuted) >= MP->TPLookahead)
	{
		if (FlushWriteLineBuffer()) return 1;  // flush any segments before starting
		if (CheckMotionHalt(true)) return 2;
		TP3Log("LAUNCH         downloaded=%8.3f exec=%8.3f", m_TotalDownloadedTime, m_TimeAlreadyExecuted);
		if(LaunchCoordMotion()) return 1;
		m_SegmentsStartedExecuting = true;
		segments_executing=segments;
	}

	if (m_SegmentsStartedExecuting)
	{
		CStringA response;
		bool wait;
		int result;

		// Check how far ahead we are

		// first check old value of m_TimeAlreadyExecuted, if we aren't too far ahead of that
		// then don't bother getting new value from DSP

		if (CheckMotionHalt(true)) return 2;

		wait = (m_TotalDownloadedTime - m_TimeAlreadyExecuted) > MP->TPLookahead;

		// timeline/stall visibility: this loop waits (silently) for the
		// controller to consume down to TPLookahead; if ExecTime freezes
		// here (user feedhold OR a stuck data-end feedhold) that is
		// otherwise indistinguishable from a hang
		bool dlWaitLogged = false;
		CHiResTimer dlWaitTimer;
		double dlLastExec = m_TimeAlreadyExecuted, dlLastAdvance = 0, dlLastStallLog = 0;

		while (!m_Simulate && wait && m_SegmentsStartedExecuting)
		{
			if (!dlWaitLogged)
			{
				dlWaitLogged = true;
				dlWaitTimer.Start();
				TP3Log("DLWAIT_BEGIN   downloaded=%8.3f exec=%8.3f", m_TotalDownloadedTime, m_TimeAlreadyExecuted);
			}
			if (FlushWriteLineBuffer()) return 1;  // flush any segments beforehand
			if (KMotionDLL->WriteLineReadLine("ExecTime",response.GetBufferSetLength(MAX_LINE))) return 1;
			response.ReleaseBuffer();
			result=sscanf(response, "%lf",&m_TimeAlreadyExecuted);
			if (result != 1)  return 1;

			if (dlWaitLogged)
			{
				double el = dlWaitTimer.Elapsed_Seconds();
				if (m_TimeAlreadyExecuted != dlLastExec)
				{
					dlLastExec = m_TimeAlreadyExecuted;
					dlLastAdvance = el;
				}
				else if (el - dlLastAdvance > 2.0 && el - dlLastStallLog > 2.0)
				{
					dlLastStallLog = el;
					TP3Log("DLWAIT_STALL   exec frozen at %8.3f for %.1fs downloaded=%8.3f",
						dlLastExec, el - dlLastAdvance, m_TotalDownloadedTime);
				}
			}

			if (m_TimeAlreadyExecuted < 0.0)
			{
				double executed = -m_TimeAlreadyExecuted;  // |e| = total executed
				TP3Log("DLWAIT_UNDERFLOW ExecTime negative downloaded=%8.3f executed=%8.3f",
					m_TotalDownloadedTime, executed);
				// Buffer ran dry.  Check if an axis was disabled
				if (KMotionDLL->WriteLineReadLine("CheckDoneXYZABC",response.GetBufferSetLength(MAX_LINE))) {SetAbort(); return 1;}
				response.ReleaseBuffer();

				if (response == "-1")
				{
					m_AxisDisabled=true;  // yes the reason was axis disabled
					SetAbort();
					return 1;
				}

				// A negative ExecTime is the Kogna's NORMAL report for a
				// completely executed buffer: with nothing having stopped
				// us (no stop state) this is a recoverable data ran-dry.
				// Re-arm silently: keep the executed total EXACT for the
				// relaunch threshold, and the auto-launch re-issues
				// ExecBuf (resuming mid-ring, ExecTime stays cumulative)
				// once TPLookahead of content is ahead again.
				if (KMotionDLL->WriteLineReadLine("GetStopState",response.GetBufferSetLength(MAX_LINE))) {SetAbort(); return 1;}
				response.ReleaseBuffer();
				if (response == "0")
				{
					TP3Log("RANDRY_REARM   executed=%8.3f", executed);
					m_TimeAlreadyExecuted = executed;
					m_SegmentsStartedExecuting = false;
				}
				else
				{
					result = MessageBoxW(NULL, KMotionDLL->Translate("Unexpected Coordinated Motion Buffer Underflow!\r(Consider increasing the Trajectory Planner Lookahead time in the Configuration Screen)\r\rPress OK to attempt to continue motion\rPress CANCEL to abort"),
						L"KMotion", MB_ICONEXCLAMATION | MB_OKCANCEL);

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
				UpdateStarveTime();   // ExecTime is fresh here: lower the
				                      // declaration as the buffer drains

				// jog released: the GUI thread has stopped (frozen) the
				// controller, so ExecTime will never advance - waiting
				// here deadlocks the streaming worker (observed: stuck
				// feedhold requiring manual clears).  Bail so the worker
				// runs its teardown, which clears the stop state.
				// m_LookaheadOverride guard: the release flag is set by
				// the GUI on any release edge while the jog worker is
				// active; a stale flag from a worker-exit race must not
				// bail a subsequent GCODE run's download wait.
				if (m_JogReleaseRequested && m_LookaheadOverride)
				{
					TP3Log("DLWAIT_RELEASE bail: jog released, controller stopped");
					return 1;
				}

				wait = (m_TotalDownloadedTime - m_TimeAlreadyExecuted) > MP->TPLookahead;
				if (!m_Simulate && wait)
				{
					Sleep(30);
				}
				if (m_Abort) return 1;
			}
		}

		if (dlWaitLogged)
			TP3Log("DLWAIT_END     waited=%7.3f exec=%8.3f", dlWaitTimer.Elapsed_Seconds(), m_TimeAlreadyExecuted);
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

		// must be one of the two ping pong buffers: if execution was ever
		// flagged started without publishing which buffer is executing
		// this would be NULL or stale.  No real time state is available
		// in that case, but it must not fault
		if (segs_to_check != segments0 && segs_to_check != segments1)
		{
			m_realtime_Sequence_number_valid = false;
			return 0;
		}

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
			KMotionDLL->DoErrMsg(KMotionDLL->Translate("Invalid buffer times in Trajectory Planner"));
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

	if (m_Simulate && !m_DoTime) return 0;  // exit if we are simulating

	if (nsegs > m_nsegs_downloaded)
	{
		// check if we have at least one more that is "done"

		if (GetSegPtr(m_nsegs_downloaded)->Done)
		{
			// yes, we have at least one to download

			if (m_nsegs_downloaded==0 && !m_Simulate)
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

			// Bound one call's blocking time: a large emission burst (the
			// 3rd Order planner can finalize seconds of content at once,
			// especially where kinematics inflate plan time near a fold)
			// otherwise downloads in ONE blocking call - observed ~1
			// second, during which a streaming caller (coordinated jog)
			// cannot react to its stop request.  Remaining Done segments
			// download on the next call - callers invoke this repeatedly.
			int iseg, ndone = 0;
			for (iseg=m_nsegs_downloaded; iseg<nsegs && GetSegPtr(iseg)->Done; iseg++)
			{
				if (OutputSegment(iseg))   {SetAbort(); return 1;}  // Output the Segment
				// see if there are any special commands that need to be inserted after segs
				if (!m_Simulate && DoSpecialCommand(iseg)) {SetAbort(); return 1;} ;
				if (++ndone >= 30) break;
			}
		}
	}
	return 0;
}


#define FLOAT_TOL 1e-6 


int CCoordMotion::SetAxisDefinitions(int x, int y, int z, int a, int b, int c)
{
	CStringA s;
	s.Format("DefineCS=%d %d %d %d %d %d", x, y, z, a, b, c);
	if (KMotionDLL->WriteLine(s)) return 1;
	x_axis=x;
	y_axis=y;
	z_axis=z;
	a_axis=a;
	b_axis=b;
	c_axis=c;

	m_DefineCS_valid=true;
	m_DefineCS_known=true;
	return 0;
}
int CCoordMotion::SetAxisDefinitions(int x, int y, int z, int a, int b, int c, int u, int v)
{
	CStringA s;
	s.Format("DefineCSEx=%d %d %d %d %d %d %d %d", x, y, z, a, b, c, u, v);
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
	m_DefineCS_known=true;
	return 0;
}

int CCoordMotion::GetAxisDefinitions(int *x, int *y, int *z, int *a, int *b, int *c)
{
	CStringA response;
	int result;

	if (!m_DefineCS_valid)
	{
		if (KMotionDLL->WriteLineReadLine("DefineCS",response.GetBufferSetLength(MAX_LINE))) return 1;
		response.ReleaseBuffer();
		result=sscanf(response, "%d%d%d%d%d%d",&x_axis,&y_axis,&z_axis,&a_axis,&b_axis,&c_axis);
		if (result != 6) return 1;
		m_DefineCS_known=true;
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
	CStringA response;
	int result;

	if (!m_DefineCS_valid)
	{
		if (KMotionDLL->WriteLineReadLine("DefineCSEx",response.GetBufferSetLength(MAX_LINE))) return 1;
		response.ReleaseBuffer();
		result = sscanf(response, "%d%d%d%d%d%d%d%d", &x_axis, &y_axis, &z_axis, &a_axis, &b_axis, &c_axis, &u_axis, &v_axis);
		if (result != 8) return 1;
		m_DefineCS_known=true;
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
									MP->CountsPerInchX,"X"))
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
									MP->CountsPerInchY,"Y"))
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
									MP->CountsPerInchZ,"Z"))
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
									MP->CountsPerInchA,"A"))
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
									MP->CountsPerInchB,"B"))
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
									MP->CountsPerInchC,"C"))
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
		MP->CountsPerInchU,"U"))
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
		MP->CountsPerInchV,"V"))
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

	// The warning is about the USER's configured Lookahead being too short
	// for the machine's stopping time.  Streaming clients (coordinated jog)
	// deliberately install a short Lookahead of their own and manage the
	// buffer themselves, so suppress it while such an override is active.
	char Axis;
	float StopTime = GetNominalFROChangeTime(&Axis);
	if (!m_LookaheadOverride && StopTime > 0.75 * MP->TPLookahead)
	{
		static bool FirstTime = true;

		if (FirstTime)
		{
			FirstTime = false; // only display warniing once
			CString Stop;
			Stop.Format(L"%c\r\rTime to Stop %.2f sec\r\rLookahead %.2f sec", Axis, StopTime, MP->TPLookahead);

			// Warning Controller configuration takes longer to stop than Trajectory Planner Look Ahead time
			KMotionDLL->DoErrMsg(Translate("Warning Controller configuration takes longer to stop than 75% of Trajectory Planner Look Ahead time. Feed Rate will be limited.\r\rLimiting Axis : ") + Stop);
		}
	}

	return 0;
}


float CCoordMotion::MaxDecelTimeForAxis(int axis, double Vel, double Accel, double Jerk)
{
	if (axis == -1) return 0.0f;

	// include jerk time as cushion because we won't be limiting jerk
	return ((float)Vel / (float)Accel) + sqrtf(8.0f * (float)Vel / (float)Jerk);
}

// computed time to change from FRO 1.0 to 0.0 for all defined 
// Coordinate System Axes and their specified Vel, Accel, jerk
float CCoordMotion::GetNominalFROChangeTime(char *Axis)
{
	MOTION_PARAMS* MP = &Kinematics->m_MotionParams;
	float t, DecelTime;

	DecelTime = MaxDecelTimeForAxis(x_axis, MP->MaxRapidVelX, MP->MaxRapidAccelX, MP->MaxRapidJerkX);
	*Axis = 'X';

	t = MaxDecelTimeForAxis(y_axis, MP->MaxRapidVelY, MP->MaxRapidAccelY, MP->MaxRapidJerkY);
	if (t > DecelTime) {DecelTime = t; *Axis = 'Y'; }

	t = MaxDecelTimeForAxis(z_axis, MP->MaxRapidVelZ, MP->MaxRapidAccelZ, MP->MaxRapidJerkZ);
	if (t > DecelTime) {DecelTime = t; *Axis = 'Z';}

	t = MaxDecelTimeForAxis(a_axis, MP->MaxRapidVelA, MP->MaxRapidAccelA, MP->MaxRapidJerkA);
	if (t > DecelTime) {DecelTime = t; *Axis = 'A';}

	t = MaxDecelTimeForAxis(b_axis, MP->MaxRapidVelB, MP->MaxRapidAccelB, MP->MaxRapidJerkB);
	if (t > DecelTime) {DecelTime = t; *Axis = 'B';}

	t = MaxDecelTimeForAxis(c_axis, MP->MaxRapidVelC, MP->MaxRapidAccelC, MP->MaxRapidJerkC);
	if (t > DecelTime) {DecelTime = t; *Axis = 'C';}

	t = MaxDecelTimeForAxis(u_axis, MP->MaxRapidVelU, MP->MaxRapidAccelU, MP->MaxRapidJerkU);
	if (t > DecelTime) {DecelTime = t; *Axis = 'U';}

	t = MaxDecelTimeForAxis(v_axis, MP->MaxRapidVelV, MP->MaxRapidAccelV, MP->MaxRapidJerkV);
	if (t > DecelTime) {DecelTime = t; *Axis = 'V';}

	return DecelTime;
}


// Get motion profile settings for a single Axis if included in the Coordinated Motion System

int CCoordMotion::GetRapidSettingsAxis(int axis,double *Vel,double *Accel,double *Jerk, double *SoftLimitPos, double *SoftLimitNeg, double CountsPerInch, char *Axis)
{
	CStringA s,response;
	int result;
	double temp;

	if (axis == -1) return 0;
	if (CountsPerInch == 0.0)
	{
		KMotionDLL->DoErrMsg(Translate("Error Counts/inch resolution has invalid value of zero for Axis:") + (CString)Axis);
		return 1;
	}

	s.Format("Vel%d;Accel%d;Jerk%d;SoftLimitPos%d;SoftLimitNeg%d",axis,axis,axis,axis,axis);
	if (KMotionDLL->WriteLine(s)) return 1;

	if (KMotionDLL->ReadLineTimeOut(response.GetBufferSetLength(MAX_LINE))) return 1;
	response.ReleaseBuffer();
	result=sscanf(response, "%lf",&temp);
	if (result != 1) return 1;
	*Vel = fabs(temp/CountsPerInch);

	if (KMotionDLL->ReadLineTimeOut(response.GetBufferSetLength(MAX_LINE))) return 1;
	response.ReleaseBuffer();
	result=sscanf(response, "%lf",&temp);
	if (result != 1) return 1;
	*Accel = fabs(temp/CountsPerInch);

	if (KMotionDLL->ReadLineTimeOut(response.GetBufferSetLength(MAX_LINE))) return 1;
	response.ReleaseBuffer();
	result=sscanf(response, "%lf",&temp);
	if (result != 1) return 1;
	*Jerk = fabs(temp/CountsPerInch);

	if (KMotionDLL->ReadLineTimeOut(response.GetBufferSetLength(MAX_LINE))) return 1;
	response.ReleaseBuffer();
	result = sscanf(response, "%lf", &temp);
	if (result != 1) return 1;
	*SoftLimitPos = temp;

	if (KMotionDLL->ReadLineTimeOut(response.GetBufferSetLength(MAX_LINE))) return 1;
	response.ReleaseBuffer();
	result = sscanf(response, "%lf", &temp);
	if (result != 1) return 1;
	*SoftLimitNeg = temp;
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
	CStringA cmd,response;

	*d=0.0;

	if (axis==-1) return 0;  // not used in coordinate system 
	
	if (axis<0 || axis>N_CHANNELS_KOGNA) {SetAbort(); return 1;} // invalid

	cmd.Format("Dest%d",axis);
	if (KMotionDLL->WriteLineReadLine(cmd,response.GetBufferSetLength(MAX_LINE))) {SetAbort(); return 1;}
	response.ReleaseBuffer();

	result=sscanf(response, "%lf",d);
	if (result != 1) {SetAbort(); return 1;}

	return 0;
}

int CCoordMotion::GetPosition(int axis, double *d)
{
	int result;
	CStringA cmd,response;

	*d=0.0;

	if (axis==-1) return 0;  // not used in coordinate system 
	
	if (axis<0 || axis>N_CHANNELS_KOGNA) {SetAbort(); return 1;} // invalid

	cmd.Format("Pos%d",axis);
	if (KMotionDLL->WriteLineReadLine(cmd,response.GetBufferSetLength(MAX_LINE))) {SetAbort(); return 1;}
	response.ReleaseBuffer();

	result=sscanf(response, "%lf",d);
	if (result != 1) {SetAbort(); return 1;}

	return 0;
}

int CCoordMotion::GetAxisDone(int axis, int *r)
{
	int result;
	CStringA cmd,response;

	*r=-1;  // assume disabled

	if (axis==-1) return 0;  // not used in coordinate system 
	
	if (axis<0 || axis>N_CHANNELS_KOGNA) {SetAbort(); return 1;} // invalid

	cmd.Format("CheckDone%d",axis);
	if (KMotionDLL->WriteLineReadLine(cmd,response.GetBufferSetLength(MAX_LINE))) {SetAbort(); return 1;}
	response.ReleaseBuffer();

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
		
		if (1.0 <= GetFeedRateOverride())  // need to increase?
		{
			// yes, go all the way with SW
			SW = GetFeedRateOverride();
		}
		else
		{
			// need to decrease

			// check if decreasing all the way to desired FRO
			// will put us below the HW Limit 

			if (GetFeedRateOverride() < m_HardwareFRORange)
			{
				// yes it will, decrease SW to limit
				SW = m_HardwareFRORange;
				// then remainder with HW
				HW = GetFeedRateOverride() / SW;
			}
			else
			{
				// no, do all with SW
				SW = GetFeedRateOverride();
			}
		}
	}
	else
	{
		// Starting FRO=1.0 is below HW limit, HW should be used
		if (1.0 <= GetFeedRateOverride())  // need to increase (above 1.0)?
		{
			// yes, check if increasing all the way to desired FRO
			// will put us above the HW Limit? 
			if (GetFeedRateOverride() > m_HardwareFRORange)
			{
				// yes it will, increase HW to limit
				HW = m_HardwareFRORange;
				// then remainder with SW
				SW = GetFeedRateOverride() /HW;
			}
			else
			{
				// no we will still be below HW limit.  Use HW entirely
				HW = GetFeedRateOverride();
			}
		}
		else
		{
			// need to decrease, do all with HW
			HW = GetFeedRateOverride();
		}
	}
}


void CCoordMotion::SetFeedRateOverride(double v)
{
	CStringA s;
	double HW,SW;

	m_FeedRateOverride = v;
	
	if (!m_Simulate)
	{
		DetermineSoftwareHardwareFRO(HW,SW);
		s.Format("SetFRO %.4f",HW);
		KMotionDLL->WriteLine(s);
	}
}

void CCoordMotion::SetFeedRateRapidOverride(double v)
{
	CStringA s;

	if (v > Kinematics->m_MotionParams.MaxRapidFRO) v = Kinematics->m_MotionParams.MaxRapidFRO;

	m_FeedRateRapidOverride=v;	

	if (!m_Simulate)
	{
		s.Format("SetRapidFRO %.4f",v);
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
	if (feed_override)
		return m_FeedRateOverride;
	else
		return 1.0;
}

double CCoordMotion::GetFeedRateRapidOverride()
{
	return m_FeedRateRapidOverride;
}

double CCoordMotion::GetSpindleRateOverride()
{
	if (speed_override)
		return m_SpindleRateOverride;
	else
		return 1.0;
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
		TP3ClearRun();
		DownloadInit();  // intialize download/look ahead variables
		ClearWriteLineBuffer();
		SegLogClose();
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


int CCoordMotion::MeasurePointAppendToFile(const wchar_t *name)
{
	double x,y,z,a,b,c;
	static int row=0, col=0;
	int NRows,NCols,rr,cc;
	double GeoSpacingX,GeoSpacingY,GeoOffsetX,GeoOffsetY,X,Y;

	// first beg of file to see how many rows and cols
	// and how much data is there, if none then assume
	// starting over

	FILE *f;
	_tfopen_s(&f, name, _T("rt,ccs=UTF-8"));

	if (!f)
	{
		KMotionDLL->DoErrMsg((CString)KMotionDLL->Translate("Unable to open Geometric Correction File : ") + name);
		return 1;
	}

	int result = fwscanf(f,L"%d,%d",&NRows,&NCols);
		
	if (result != 2 || NRows < 2 || NRows > 1000 || NCols < 2 || NCols > 1000)
	{
		fclose(f);
		KMotionDLL->DoErrMsg((CString)KMotionDLL->Translate("Invalid Geometric Correction File (NRows and NCols) : ") + name);
		return 1;
	}

	result = fwscanf(f,L"%lf,%lf",&GeoSpacingX,&GeoSpacingY);
		
	if (result != 2)
	{
		fclose(f);
		KMotionDLL->DoErrMsg((CString)KMotionDLL->Translate("Invalid Geometric Correction File (GeoSpacingX and GeoSpacingY) : ") + name);
		return 1;
	}

	result = fwscanf(f,L"%lf,%lf",&GeoOffsetX,&GeoOffsetY);
		
	if (result != 2)
	{
		fclose(f);
		KMotionDLL->DoErrMsg((CString)KMotionDLL->Translate("Invalid Geometric Correction File (GeoOffsetX and GeoOffsetY) : ") + name);
		return 1;
	}

	result = fwscanf(f,L"%d,%d,%lf,%lf",&rr,&cc,&X,&Y);

	if (result != 4) row=col=0;		// assume we are starting over

	fclose(f);

	double u, v;
	if (ReadCurAbsPosition(&x,&y,&z,&a, &b, &c, &u, &v)) return 1;
	
	_tfopen_s(&f, name, _T("at,ccs=UTF-8"));

	if (!f)
	{
		KMotionDLL->DoErrMsg((CString)KMotionDLL->Translate("Unable to open Measurement Point File : ") + name);
		return 1;
	}

	fwprintf(f,L"%d,%d,%f,%f,%f\n",row,col,x,y,z);

	col++;

	if (col == NCols)
	{
		col = 0;
		row++;
	}

	fclose(f);

	return 0;
}


int CCoordMotion::PutWriteLineBuffer(CStringA s, double Time)
{
	if (m_Abort) return 1;

	// new string won't fit, flush it first
	if (WriteLineBuffer.GetLength() + s.GetLength() > MAX_LINE-10)
	{
		if (FlushWriteLineBuffer()) return 1;
	}

	// put in the string
	if (!WriteLineBuffer.IsEmpty()) WriteLineBuffer += ';';
	WriteLineBuffer += s;
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

	int Length = WriteLineBuffer.GetLength();

	int result = KMotionDLL->WriteLine(WriteLineBuffer);
	ClearWriteLineBuffer();
	return result;
}

int CCoordMotion::ClearWriteLineBuffer()
{
	WriteLineBuffer="";
	WriteLineBufferTime=0.0;
	return 0;
}

double CCoordMotion::FeedRateDistance(double dx, double dy, double dz, double da, double db, double dc, BOOL *bPureAngle)
{
	return ::FeedRateDistance(dx, dy, dz, da, db, dc, 0.0, 0.0, &Kinematics->m_MotionParams, bPureAngle);

}

double CCoordMotion::FeedRateDistance(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, BOOL *bPureAngle)
{
	return ::FeedRateDistance(dx, dy, dz, da, db, dc, du, dv, &Kinematics->m_MotionParams, bPureAngle);
}

int CCoordMotion::ConfigSpindle(int type, int axis, double UpdateTime, double Tau, double CountsPerRev)
{
	CStringA s;
	
	s.Format("ConfigSpindle %d %d %.6f %.6f %f",type, axis, UpdateTime, Tau, CountsPerRev);
	return KMotionDLL->WriteLine(s);
}


int CCoordMotion::GetSpindleRPS(float &speed)
{
	CStringA response;

	if (KMotionDLL->WriteLineReadLine("GetSpindleRPS",response.GetBufferSetLength(MAX_LINE))) return 1;
	
	// check state
	if (sscanf(response,"%f",&speed)!=1) return 1;

	return 0;
}

bool CCoordMotion::CheckCollinear(SEGMENT *s0, SEGMENT *s1, SEGMENT *s2, double tol)
{
	return ::CheckCollinear(s0, s1, s2, tol);
}

