// TrajectoryPlanner.c  Tom Kerekes 09/11/02

#include "stdafx.h"
#include "HiResTimer.h"

#ifdef DEBUG_DOWNLOAD
extern CString ds;
extern FILE *f;
extern CHiResTimer DTimer;

void PutString(CString s);
void CloseDiag();


#endif


#define SIGMA (1e-9)
#define SPEED_TOL 0.01
#define ACCEL_TOL 0.04



#ifndef TWO_PI
#define TWO_PI (2.0*3.141592653589793238)
#endif

#ifndef HALF_PI
#define HALF_PI (0.5*3.141592653589793238)
#endif


int nsegs=0,prev_nsegs=0,SegBufToggle=0;
int SegsDone[2];
double SegsDoneTime[2];
SEGMENT *segments,*segments_executing;
SEGMENT segments0[MAX_TP_SEGMENTS];
SEGMENT segments1[MAX_TP_SEGMENTS];


int nspecial_cmds;
int special_cmds_initial_first;   // Special commands start index at the very beginning of path
int special_cmds_initial_last;    // Special commands ending index at the very beginning of path
SPECIAL_CMD special_cmds[MAX_SPECIAL_CMDS];
int ispecial_cmd_downloaded;


double CalcChangeInDirection(int i);
double CalcChangeInDirectionXYZ(int i);
double SegmentXYZLength(SEGMENT *p);

double CalcLengthAlongCircle(double x0, double y0, 
					  double x1, double y1, 
					  double xc, double yc, BOOL DirIsCCW, 
					  double *radius, double *theta0, double *dtheta);

void CalcFinalDirectionOfSegment(SEGMENT *p,double &dx, double &dy, double &dz, double &da, double &db, double &dc);
void CalcBegDirectionOfSegment(SEGMENT *p,double &dx, double &dy, double &dz, double &da, double &db, double &dc);
void AdjustSegVelocity(int i);
int CombineSegments(double MaxLength);
double MaximizeSegmentForward(int i);
double MaximizeSegmentBackward(int i);


// used to combine colinear segments

#define MAX_COMBINE 100
SEGMENT CombinedList[MAX_COMBINE];
int nCombined; 


MOTION_PARAMS MP;

double FacetAngleRadians;
double BreakAngleRadians;


double Pos(TP_COEFF *p);
double Vel(TP_COEFF *p);
double CubeRoot(double v);
void Quadradic(double a, double b, double c, double *r1, double *r2);


int TPMOD(int i)
{
	return i&(MAX_TP_SEGMENTS-1);
}
SEGMENT *GetSegPtr(int i)
{
	return &segments[TPMOD(i)];
}


void SetTrajectoryPlannerParams(MOTION_PARAMS *m)
{
	MP = *m;  // copy locally
	FacetAngleRadians=MP.FacetAngle*PI/180.0;
	BreakAngleRadians=MP.BreakAngle*PI/180.0;
}


// initialize for new list of segments
// define endpoint to have zero vel

void tp_init()
{
	// use ping pong buffers so we can work ahead
	// and still have a list of what was last downloaded
	
	prev_nsegs = nsegs;

	SegBufToggle = 1-SegBufToggle;

	if (SegBufToggle)
		segments=segments1;
	else
		segments=segments0;

	// keep track of all the "Done" segments
	// times in the buffer so we don't need to scan
	// from the beginning on a halt
	SegsDoneTime[SegBufToggle]=0.0;
	SegsDone[SegBufToggle]=-1;

	ispecial_cmd_downloaded=nsegs=nspecial_cmds=nCombined=0;
	special_cmds_initial_first=special_cmds_initial_last=-1;
}


// insert new segment at the end.  Re-evaluate backwards
// through list to see if velocities could be increased

int tp_insert_linear_seg(double x0, double y0, double z0, double a0, double b0, double c0, 
						 double x1, double y1, double z1, double a1, double b1, double c1, 
						 double MaxVel, double MaxAccel, double MaxCombineLength, int sequence_number,int ID)
{
	double dx=x1-x0;
	double dy=y1-y0;
	double dz=z1-z0;
	double da=a1-a0;
	double db=b1-b0;
	double dc=c1-c0;

	// add into the list
	
	SEGMENT *p=GetSegPtr(nsegs);

	p->type = SEG_LINEAR;
	p->sequence_number = sequence_number;
	p->ID = ID;
	
	p->x0 = x0;
	p->y0 = y0;
	p->z0 = z0;
	p->a0 = a0;
	p->b0 = b0;
	p->c0 = c0;
	
	p->x1 = x1;
	p->y1 = y1;
	p->z1 = z1;
	p->a1 = a1;
	p->b1 = b1;
	p->c1 = c1;

	BOOL pure_angle;
	p->dx=FeedRateDistance(dx, dy, dz, da, db, dc, &MP, &pure_angle);
	p->OrigVel = p->MaxVel = MaxVel;
	p->OrigAccel = MaxAccel;
	p->vel=0.0f;
	p->ChangeInDirection = CalcChangeInDirection(nsegs);
	p->StopRequired=FALSE;
	p->special_cmds_first=p->special_cmds_last=-1; // init with no special commands
	p->Done=FALSE;

	if (CombineSegments(MaxCombineLength))
	{
		SEGMENT *pm1,*pm2;

		// new segment couldn't be combined

		// check if the previous combination of segments
		// resulted in a zero or near zero length segment
		// note: 
		// nsegs   currently points the new segment
		// nsegs-1 points to the previously combined
		// nsegs-2 points to the previous


		bool DiscardTinySegment=false;

		if (nsegs>1)
		{
			pm1=GetSegPtr(nsegs-1);
			pm2=GetSegPtr(nsegs-2);
			
			// size must be vey small relative to
			// Collinear Tol and MaxCombineLength and
			// max speeds must be very similar
			// both must be normal linear segments

			if (pm1->dx < MP.CollinearTol*0.5 &&
				pm1->dx < MaxCombineLength*0.25 &&
				pm1->type==SEG_LINEAR &&
				pm2->type==SEG_LINEAR &&
				fabs(pm2->OrigVel - pm1->OrigVel) <= SPEED_TOL * min(pm1->OrigVel,pm2->OrigVel) &&
				fabs(pm2->OrigAccel - pm1->OrigAccel) <= SPEED_TOL * min(pm1->OrigAccel,pm2->OrigAccel))
				DiscardTinySegment=true;
		}

		// all conditions met to discard it?		
		if (DiscardTinySegment)
		{
			// yes the result somehow resulted in a very tiny segment
			// discard it and adjust the end of the previous segment 
			// to end at the end of the microscopic combination

			pm2->x1 = p->x0;
			pm2->y1 = p->y0;
			pm2->z1 = p->z0;
			pm2->a1 = p->a0;
			pm2->b1 = p->b0;
			pm2->c1 = p->c0;

			double dx=pm2->x1 - pm2->x0;
			double dy=pm2->y1 - pm2->y0;
			double dz=pm2->z1 - pm2->z0;
			double da=pm2->a1 - pm2->a0;
			double db=pm2->b1 - pm2->b0;
			double dc=pm2->c1 - pm2->c0;

			// note keep velocity since it might already be non-zero
			// and the previous segment might be maxed out (Done)
			// and transitioning into it
			BOOL pure_angle;
			pm2->dx=FeedRateDistance(dx, dy, dz, da, db, dc, &MP, &pure_angle);
			pm2->ChangeInDirection = CalcChangeInDirection(nsegs-1);
			pm2->Done=FALSE;

			// check if there are any special commands in 
			// the segment we are going to discard
			if (p->special_cmds_first != -1)
			{
				// none already in the previous?
				if (pm2->special_cmds_first == -1)
				{
					// yes, copy start
					pm2->special_cmds_first = p->special_cmds_first;
				}

				// extend to include other segment's commands
				pm2->special_cmds_last = p->special_cmds_last;
			}
			SEGMENT *pm1=GetSegPtr(nsegs-1);

			*pm1 = *p;  // shift the new segment down over the deleted combination 
		}
		else
		{
			// normal length segment, add it in
			nsegs++;
			RoundCorner(nsegs-2);
		}

#if 0 // enable this for TP diagnostics
		if (nsegs==50)
		{
			FILE *f = fopen("c:\\temp\\segs.txt","wt");

			double time=0;

			fprintf(f,"time,sequence_number,x0,y0,dx,Done,vel,MaxVel,ChangeInDirection,C[0].t,C[1].t,C[2].t,a,b,c,a,b,c,a,b,c\n");
			for (int i=1; i<30; i++)
			{
				fprintf(f,"%f,%d,%f,%f,%f,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",time,
					segments[i].sequence_number,segments[i].x0,segments[i].y0,segments[i].dx,segments[i].Done,
					segments[i].vel,segments[i].MaxVel,segments[i].ChangeInDirection,
					segments[i].C[0].t,segments[i].C[1].t,segments[i].C[2].t,
					segments[i].C[0].a,segments[i].C[0].b,segments[i].C[0].c,
					segments[i].C[1].a,segments[i].C[1].b,segments[i].C[1].c,
					segments[i].C[2].a,segments[i].C[2].b,segments[i].C[2].c);

				time += segments[i].C[0].t + segments[i].C[1].t + segments[i].C[2].t;
			}
			fclose(f);
		}
#endif

		return 0;  // segment was added
	}
	return 2;  // segment was combined
}




// insert new Rapid 3rd order segment at the end. 

int tp_insert_linear_seg_3rdOrder(double x0, double y0, double z0, double a0, double b0, double c0, 
								  double x1, double y1, double z1, double a1, double b1, double c1, 
						          int sequence_number,int ID)
{
	double dx=x1-x0;
	double dy=y1-y0;
	double dz=z1-z0;
	double da=a1-a0;
	double db=b1-b0;
	double dc=c1-c0;

	// add into the list
	
	SEGMENT *p=GetSegPtr(nsegs);

	p->type = SEG_RAPID;
	p->sequence_number = sequence_number;
	p->ID = ID;
	
	p->x0 = x0;
	p->y0 = y0;
	p->z0 = z0;
	p->a0 = a0;
	p->b0 = b0;
	p->c0 = c0;
	
	p->x1 = x1;
	p->y1 = y1;
	p->z1 = z1;
	p->a1 = a1;
	p->b1 = b1;
	p->c1 = c1;

	BOOL pure_angle;
	p->dx=FeedRateDistance(dx, dy, dz, da, db, dc, &MP, &pure_angle);
	p->vel=0.0f;
	p->StopRequired=TRUE;
	p->special_cmds_first=p->special_cmds_last=-1; // init with no special commands
	p->Done=FALSE;

	// add it in
	nsegs++;

	return 0;  // segment was added
}



int tp_insert_dwell(double t, double x0, double y0, double z0, double a0, double b0, double c0, int sequence_number,int ID)
{
	// add into the list
	
	SEGMENT *p=GetSegPtr(nsegs);

	p->type = SEG_DWELL;
	p->sequence_number = sequence_number;
	p->ID = ID;
	
	p->x0 = x0;
	p->y0 = y0;
	p->z0 = z0;
	p->a0 = a0;
	p->b0 = b0;
	p->c0 = c0;
	
	p->x1 = x0;
	p->y1 = y0;
	p->z1 = z0;
	p->a1 = a0;
	p->b1 = b0;
	p->c1 = c0;

	p->dwell_time = t;

	p->dx=0.0;
	p->vel=0.0f;
	p->StopRequired=TRUE;
	p->special_cmds_first=p->special_cmds_last=-1; // init with no special commands
	p->Done=FALSE;

	// add it in
	nsegs++;

	return 0;  // segment was added
}





//
//
// Theta = original change in direction
// A=interior angle

typedef struct
{
	double x,y,z,a,b,c;
} P6;

void CopySegStartToP6(SEGMENT *p, P6 &point)
{
	point.x = p->x0;
	point.y = p->y0;
	point.z = p->z0;
	point.a = p->a0;
	point.b = p->b0;
	point.c = p->c0;
}

void CopySegEndToP6(SEGMENT *p, P6 &point)
{
	point.x = p->x1;
	point.y = p->y1;
	point.z = p->z1;
	point.a = p->a1;
	point.b = p->b1;
	point.c = p->c1;
}

void DiffP6(P6 &p0,P6 &p1,P6 &p2)
{
	p2.x = p0.x - p1.x;
	p2.y = p0.y - p1.y;
	p2.z = p0.z - p1.z;
	p2.a = p0.a - p1.a;
	p2.b = p0.b - p1.b;
	p2.c = p0.c - p1.c;
}

void AddP6(P6 &p0,P6 &p1,P6 &p2)
{
	p2.x = p0.x + p1.x;
	p2.y = p0.y + p1.y;
	p2.z = p0.z + p1.z;
	p2.a = p0.a + p1.a;
	p2.b = p0.b + p1.b;
	p2.c = p0.c + p1.c;
}

void ScaleP6(P6 &p0, double s)
{
	p0.x *= s;
	p0.y *= s;
	p0.z *= s;
	p0.a *= s;
	p0.b *= s;
	p0.c *= s;
}

double LengthP6(P6 &p)
{
	BOOL pure_angle;
	return FeedRateDistance(p.x, p.y, p.z, p.a, p.b, p.c, &MP, &pure_angle);
}


// adjust segment is-1 to now end at pa which is L from the end
void AdjustSegToEndAtP6(int i, double L)
{
	SEGMENT *p=GetSegPtr(i);

	double dx = p->x1 - p->x0;
	double dy = p->y1 - p->y0;
	double dz = p->z1 - p->z0;
	double da = p->a1 - p->a0;
	double db = p->b1 - p->b0;
	double dc = p->c1 - p->c0;

	double OldLength = p->dx;

	double scale;
	
	if (OldLength!=0.0)
		scale = (OldLength - L)/OldLength;
	else
		scale = 1.0;
	
	p->x1 = p->x0 + dx * scale;
	p->y1 = p->y0 + dy * scale;
	p->z1 = p->z0 + dz * scale;
	p->a1 = p->a0 + da * scale;
	p->b1 = p->b0 + db * scale;
	p->c1 = p->c0 + dc * scale;

	dx = p->x1 - p->x0;
	dy = p->y1 - p->y0;
	dz = p->z1 - p->z0;
	da = p->a1 - p->a0;
	db = p->b1 - p->b0;
	dc = p->c1 - p->c0;

	BOOL pure_angle;
	p->dx=FeedRateDistance(dx, dy, dz, da, db, dc, &MP, &pure_angle);
	p->ChangeInDirection = CalcChangeInDirection(i);
}

// adjust segment is to now begin at pb which is L from the beginning
void AdjustSegToBeginAtP6(int i, double L)
{
	SEGMENT *p=GetSegPtr(i);

	double dx = p->x1 - p->x0;
	double dy = p->y1 - p->y0;
	double dz = p->z1 - p->z0;
	double da = p->a1 - p->a0;
	double db = p->b1 - p->b0;
	double dc = p->c1 - p->c0;

	double OldLength = p->dx;

	double scale;
	
	if (OldLength!=0.0)
		scale = L/OldLength;
	else
		scale = 1.0;
	
	p->x0 = p->x0 + dx * scale;
	p->y0 = p->y0 + dy * scale;
	p->z0 = p->z0 + dz * scale;
	p->a0 = p->a0 + da * scale;
	p->b0 = p->b0 + db * scale;
	p->c0 = p->c0 + dc * scale;

	dx = p->x1 - p->x0;
	dy = p->y1 - p->y0;
	dz = p->z1 - p->z0;
	da = p->a1 - p->a0;
	db = p->b1 - p->b0;
	dc = p->c1 - p->c0;

	BOOL pure_angle;
	p->dx=FeedRateDistance(dx, dy, dz, da, db, dc, &MP, &pure_angle);
	p->ChangeInDirection = CalcChangeInDirection(i);
}


// Create the new facets, 3D goes from
// prev to px, other axis should linearly
// interpolate appropriately from pa to pb
// for example if n=3 (two facets) and i=0
// then interpolation should run from 0% to 50%
// i/(n-1) to (i+1)/(n-1)

void CreateSegFromTo(int i, SEGMENT *sa, SEGMENT *sb ,P6 &prev, P6 &px, double start, double end)
{
	SEGMENT *p=GetSegPtr(i);

	bool AisDist = (!MP.DegreesA || MP.RadiusA!=0.0);
	bool BisDist = (!MP.DegreesB || MP.RadiusB!=0.0);
	bool CisDist = (!MP.DegreesC || MP.RadiusC!=0.0);

	if (AisDist)
	{
		p->a0 = prev.a;
		p->a1 = px.a;
	}
	else
	{
		p->a0 = sa->a1 + (sb->a0 - sa->a1) * start;
		p->a1 = sa->a1 + (sb->a0 - sa->a1) * end;
	}

	if (BisDist)
	{
		p->b0 = prev.b;
		p->b1 = px.b;
	}
	else
	{
		p->b0 = sa->b1 + (sb->b0 - sa->b1) * start;
		p->b1 = sa->b1 + (sb->b0 - sa->b1) * end;
	}

	if (CisDist)
	{
		p->c0 = prev.c;
		p->c1 = px.c;
	}
	else
	{
		p->c0 = sa->c1 + (sb->c0 - sa->c1) * start;
		p->c1 = sa->c1 + (sb->c0 - sa->c1) * end;
	}
	
	p->x0 = prev.x;
	p->y0 = prev.y;
	p->z0 = prev.z;

	p->x1 = px.x;
	p->y1 = px.y;
	p->z1 = px.z;

	double dx = p->x1 - p->x0;
	double dy = p->y1 - p->y0;
	double dz = p->z1 - p->z0;
	double da = p->a1 - p->a0;
	double db = p->b1 - p->b0;
	double dc = p->c1 - p->c0;

	SEGMENT *s;

	// for first half use values from first seg, remainder use sec segment
	if (start < 0.4999999)  
		s=sa;
	else
		s=sb;

	p->type = SEG_LINEAR;
	p->sequence_number = s->sequence_number;
	p->ID = s->ID;

	BOOL pure_angle;
	p->dx=FeedRateDistance(dx, dy, dz, da, db, dc, &MP, &pure_angle);
	p->OrigVel = p->MaxVel = s->MaxVel;
	p->OrigAccel = s->OrigAccel;
	p->vel=0.0f;
	p->ChangeInDirection = CalcChangeInDirection(i);
	p->StopRequired=FALSE;
	p->special_cmds_first=p->special_cmds_last=-1; // init with no special commands
	p->Done=FALSE;
}

// round the corner between the passed segment and the previous

void RoundCorner(int is)
{
	static int prevn=-1;

	BOOL StopRequiredAtP0;

	P6 p0,p1,p2,pa,pb,pm,px,pc;
	int orig_special_cmds_first, orig_special_cmds_last;

	if (is<1) return;

	SEGMENT *seg=GetSegPtr(is);
	SEGMENT *segm=GetSegPtr(is-1);


	double Theta = seg->ChangeInDirection;
	if (fabs(Theta) > BreakAngleRadians)
	{
		// remember that we decided a stop was required and to not round the
		// corner as there are cases where the angle may change after rounding
		seg->StopRequired = TRUE;  
		return;
	}

	if (Theta == 0.0 || fabs(MP.CornerTol) < SIGMA
		|| seg->type != SEG_LINEAR|| segm->type != SEG_LINEAR) return;

	StopRequiredAtP0 = segm->StopRequired;

	// number of angle changes that will occur
	double nd = fabs(Theta)/FacetAngleRadians;
	
	int n = (int)nd+1;

	//check if the number of angle changes is borderline
	//and if adjusted slightly would be the same as the previous
	if (fabs(nd - (int)nd) < 0.01)
	{
		if (n>1 && n+1 == prevn)
			n++;
		else if (n-1 == prevn)
			n--;
	}

	if (n<2) return;

	prevn=n;

	// how much angle changes each facet
	double dtheta = Theta/(n-1);
	
	double Theta2 = Theta/2;

	double CosT2 = cos(Theta2);

	double R = MP.CornerTol * CosT2 / (1.0-CosT2);

	double TanTheta2 = tan(Theta2);

	double L = R * TanTheta2;

	double d0 = segm->dx;
	double d1 = seg->dx;

	// determine if the rounding is 
	if (d0 < L) L = d0;
	if (d1/2 < L) L = d1/2;  // only allow usage of 1/2 of the next segment

	// this is a bit complicated, but in the case that L  is  very close
	// to d0 or d1 we might end up with a microscopic segment with a sharp 
	// angle, so if it leaves less than sigma eliminate it.  sigma is the 
	// smaller of either 0.1% of the CornerTolerance or segment lengths
	// whichever is smaller

	double sigma = MP.CornerTol * 0.001;
	if (sigma > d0 * 0.001) sigma = d0 * 0.001;
	if (sigma > d1 * 0.001) sigma = d1 * 0.001;

	bool EliminateP0 = fabs(L - d0) < sigma;
	bool EliminateP2 = fabs(L - d1) < sigma;

	// recompute R in case L changed;
	R = L/TanTheta2;

	int NumberDeletedBefore = 0;
	int NumberDeletedAfter = 0;

	if (EliminateP0) NumberDeletedBefore++;
	if (EliminateP2) NumberDeletedAfter++;

	CopySegStartToP6(segm,p0);
	CopySegStartToP6(seg,p1);
	CopySegEndToP6(seg,p2);

	// compute start of curve
	//  pa = p1 + (p0-p1) L/d0
	DiffP6(p0,p1,px);
	ScaleP6(px,L/d0);
	AddP6(p1,px,pa);

	// compute end of curve
	// pb = p1 + (p2-p1) L/d1
	DiffP6(p2,p1,px);
	ScaleP6(px,L/d1);
	AddP6(p1,px,pb);

	// compute cord midpoint
	// pm = (pa+pb)/2
	AddP6(pa,pb,pm);
	ScaleP6(pm,0.5);

	// compute arc center

	// pc = pm + (pm-p1)  x/MidToCorner
	DiffP6(pm,p1,pc);
	double MidToCorner = LengthP6(pc);
	double x = R * CosT2;
	ScaleP6(pc,x/MidToCorner);
	AddP6(pm,pc,pc);

	// generate segments

	// change one angle change into n angle changes
	// or change 2 segments (is-1 and is)into n+1 segments
	// or we must insert n-1 segments
	// we must shift anything further in the list forward 
	// by n-1 places (there should only be one segment at is+1)

	if (is+1 < nsegs)
	{
		SEGMENT *segp=GetSegPtr(is+1);
		SEGMENT *segf=GetSegPtr(is+1+n-1-NumberDeletedBefore-NumberDeletedAfter);
		*segf = *segp;  // copy the whole segment
	}

	SEGMENT FromABC,ToABC;

	if (!EliminateP0)  // if it hasn't been completely eliminated
	{
		AdjustSegToEndAtP6(is-1,L);  // adjust segment is-1 to now end at pa
		FromABC.a1 = segm->a1;
		FromABC.b1 = segm->b1;
		FromABC.c1 = segm->c1;
	}
	else
	{
		FromABC.a1 = segm->a0;
		FromABC.b1 = segm->b0;
		FromABC.c1 = segm->c0;
	}

	if (!EliminateP2)  // if it hasn't been completely eliminated
	{
		AdjustSegToBeginAtP6(is,L);  // adjust segment is to now begin at pb

		// also move it down
		SEGMENT *segNew=GetSegPtr(is+n-1-NumberDeletedBefore);
		*segNew = *seg;  // copy the whole segment
		ToABC.a0 = seg->a0;
		ToABC.b0 = seg->b0;
		ToABC.c0 = seg->c0;
	}
	else
	{
		ToABC.a0 = seg->a1;
		ToABC.b0 = seg->b1;
		ToABC.c0 = seg->c1;
	}

	// use info from first segment for half of the corner segments
	FromABC.sequence_number = segm->sequence_number;
	FromABC.ID = segm->ID;
	FromABC.MaxVel = segm->MaxVel;
	FromABC.OrigAccel = segm->OrigAccel;
	// use info from second segment for second half of the corner segments
	ToABC.sequence_number = seg->sequence_number;
	ToABC.ID = seg->ID;
	ToABC.MaxVel = seg->MaxVel;
	ToABC.OrigAccel = seg->OrigAccel;

	// remember any original special commands before
	// they get over written
	orig_special_cmds_first=segm->special_cmds_first;
	orig_special_cmds_last=segm->special_cmds_last;
	// remove them from where they were
	segm->special_cmds_first=-1;
	segm->special_cmds_last=-1;




	P6 prev=pa;

	// loop through the number of facets in the corner
	for (int i=0; i<n-1; i++)
	{
		P6 pw,pv;

		double t=(i+1)*dtheta;

		DiffP6(pa,pb,px);

		double Z=LengthP6(px)/2;

		double l=sqrt(R*R-Z*Z);

		double w=l*tan(Theta2-t);

		// pw = pm + (pa-pm) w/Z
		DiffP6(pa,pm,px);
		ScaleP6(px,w/Z);
		AddP6(pm,px,pw);

		//now extend center to pw to the radius

		DiffP6(pw,pc,px);
		double g=LengthP6(px);
		// pv = pc + (pw-pc) * R/g
		DiffP6(pw,pc,px);
		ScaleP6(px,R/g);
		AddP6(pc,px,pv);


		// Create the new facets, 3D goes from
		// prev to px, other axis should linearly
		// interpolate appropriately from pa to pb
		// for example if n=3 (two facets) and i=0
		// then interpolation should run from 0% to 50%
		// i/(n-1) to (i+1)/(n-1)

		CreateSegFromTo(is+i-NumberDeletedBefore,&FromABC,&ToABC,prev,pv,(double)i/(n-1), double(i+1)/(n-1));

		if (i==0 && EliminateP0 && StopRequiredAtP0)
		{
			GetSegPtr(is+i-NumberDeletedBefore)->StopRequired=TRUE;
		}

		prev=pv;
	}

	// compute the number of segments that were added
	int nadded = n-1-NumberDeletedBefore-NumberDeletedAfter;

	// if there are special commands to be inserted at the waypoint
	// between segments, move them into a segment in the middle of the corner
	if (orig_special_cmds_first != -1)
	{
		SEGMENT *p = GetSegPtr(is + (nadded-1)/2);

		p->special_cmds_first=orig_special_cmds_first;
		p->special_cmds_last=orig_special_cmds_last;

	}

	nsegs += nadded;  // we added this many segments
}



void SetSegmentVelAccels(int i, double Vel, double Accel, double Decel)
{
	SEGMENT *p=GetSegPtr(i);
	p->MaxVel   = Vel;
	p->MaxAccel = Accel;
	p->MaxDecel = Decel;
}

void SetSegmentVelAccelJerk(int i, double Vel, double Accel, double Jerk)
{
	SEGMENT *p=GetSegPtr(i);
	p->MaxVel   = Vel;
	p->MaxAccel = Accel;
	p->MaxDecel = Accel;
	p->MaxJerk = Jerk;
}



void GetSegmentDirection(int i, double *dx, double *dy, double *dz, double *da, double *db, double *dc)
{
	SEGMENT *p=GetSegPtr(i);
	*dx = p->x1 - p->x0;
	*dy = p->y1 - p->y0;
	*dz = p->z1 - p->z0;
	*da = p->a1 - p->a0;
	*db = p->b1 - p->b0;
	*dc = p->c1 - p->c0;
}



//
// area=(1/2) x Base x Height. Where the height is an altitude drawn from the base to the opposite angle. 
// This formula makes for a relatively easy calculation of the area of a triangle but it is rather difficult 
// to naturally find a triangle that is given in terms of at least one side (the base) and a height. 
// We typically can determine or are given the sides of a triangle when a triangle is present. 
// formula does exist that can calculate the area of a triangle when all three sides are known.
// This formula is attributed to Heron of Alexandria but can be traced back to Archimedes.
//
// This formula is represented by
// Area=SQRT(s(s-a)(s-b)(s-c)),
// where s=(a+b+c)/2 or perimeter/2. 
//
// start points of the 3 segments are used as the 3 points
//

bool CheckCollinear(SEGMENT *s0, SEGMENT *s1, SEGMENT *s2, double tol)
{
	double dx = s1->x0 - s0->x0;
	double dy = s1->y0 - s0->y0;
	double dz = s1->z0 - s0->z0;
	double da = s1->a0 - s0->a0;
	double db = s1->b0 - s0->b0;
	double dc = s1->c0 - s0->c0;

	BOOL pure_angle0,pure_angle1,pure_angle2;
	double a = FeedRateDistance(dx, dy, dz, da, db, dc, &MP, &pure_angle0);

	dx = s2->x0 - s1->x0;
	dy = s2->y0 - s1->y0;
	dz = s2->z0 - s1->z0;
	da = s2->a0 - s1->a0;
	db = s2->b0 - s1->b0;
	dc = s2->c0 - s1->c0;

	double b = FeedRateDistance(dx, dy, dz, da, db, dc, &MP, &pure_angle1);

	dx = s2->x0 - s0->x0;
	dy = s2->y0 - s0->y0;
	dz = s2->z0 - s0->z0;
	da = s2->a0 - s0->a0;
	db = s2->b0 - s0->b0;
	dc = s2->c0 - s0->c0;

	double c = FeedRateDistance(dx, dy, dz, da, db, dc, &MP, &pure_angle2);

	// if all pure angles then don't combine
	if (pure_angle0 && pure_angle1 && pure_angle2) return false;

	// if dist from beg to end is tiny, then 
	//    if both other sides tiny treat as collinear

	if (c <= tol) return (a < tol && b < tol);

	double s = (a+b+c)/2;
	double v = s*(s-a)*(s-b)*(s-c);
	
	// note v should not be negative unless area is near zero and there are numerical errors
	if (v >= 0.0)
	{
		// save sqrt by squaring the tolerance instead
		double f = tol * 0.5 * c;
		if (v > f*f) return false;
	}

	// if the height is small, there is a possibility
	// that the "middle" point is actually outside
	// of the begining and end and should be treated as
	// non-collinear.  We test for that by testing if the sum of the two 
	// "short" sides are longer than the base.

	if (a+b-c > tol) return false;

	return true;
}



// attempt to combine newly added segment if within tolerance

// return 0 if it was OK to combine


int CombineSegments(double MaxLength)
{
	 SEGMENT *pn=GetSegPtr(nsegs);
	 SEGMENT *pm1=GetSegPtr(nsegs-1);

	if (nCombined >= MAX_COMBINE || nsegs < 1 || pn->type != SEG_LINEAR ||  pm1->type != SEG_LINEAR || 
		pn->Done ||  pm1->Done || pm1->special_cmds_first!=-1 || nCombined >= MAX_COMBINE)
	{
		// that's enough, clear the list and don't combine
		nCombined = 0;
		return 1;
	}

	// if already quite long then don't combine
	
	if (pm1->dx > MaxLength) 
	{
		nCombined = 0;
		return 1;
	}

	// max speeds must be very similar

	if (fabs(pn->OrigVel - pm1->OrigVel) > SPEED_TOL * min(pn->OrigVel,pm1->OrigVel)) 
	{
		nCombined = 0;
		return 1;
	}

	if (fabs(pn->OrigAccel - pm1->OrigAccel) > SPEED_TOL * min(pn->OrigAccel,pm1->OrigAccel)) 
	{
		nCombined = 0;
		return 1;
	}

/*	if (fabs(pn->MaxAccel - pm1->MaxAccel)/pm1->MaxAccel > ACCEL_TOL) 
	{
		nCombined = 0;
		return 1;
	}

	if (fabs(pn->MaxAccel - pm1->MaxAccel)/pm1->MaxAccel > ACCEL_TOL) 
	{
		nCombined = 0;
		return 1;
	}
*/
	SEGMENT end;

	end.x0 =  pn->x1;
	end.y0 =  pn->y1;
	end.z0 =  pn->z1;
	end.a0 =  pn->a1;
	end.b0 =  pn->b1;
	end.c0 =  pn->c1;

	if (!CheckCollinear(pm1,pn,&end,MP.CollinearTol))
	{
		nCombined = 0;
		return 1;
	}

	// yes it is collinear

	// check all previous points also

	for (int i=0; i<nCombined; i++)
	{
		if (!CheckCollinear(pm1,&CombinedList[i],&end,MP.CollinearTol))
		{
			// not collinear with other points
			// clear the list and return

			nCombined=0;
			return 1;
		}
	}

	// yes, ok to combine

	// save the point that was eliminated
	SEGMENT *pnc=&CombinedList[nCombined];
	pnc->x0 = pn->x0;
	pnc->y0 = pn->y0;
	pnc->z0 = pn->z0;
	pnc->a0 = pn->a0;
	pnc->b0 = pn->b0;
	pnc->c0 = pn->c0;
	nCombined++;

	// alter previous to include this one
	
	pm1->x1 = pn->x1;
	pm1->y1 = pn->y1;
	pm1->z1 = pn->z1;
	pm1->a1 = pn->a1;
	pm1->b1 = pn->b1;
	pm1->c1 = pn->c1;
	
	double dx=pm1->x1 - pm1->x0;
	double dy=pm1->y1 - pm1->y0;
	double dz=pm1->z1 - pm1->z0;
	double da=pm1->a1 - pm1->a0;
	double db=pm1->b1 - pm1->b0;
	double dc=pm1->c1 - pm1->c0;

	// note keep velocity since it might already be non-zero
	// and the previous segment might be maxed out (Done)
	// and transitioning into it
	BOOL pure_angle;
	pm1->dx=FeedRateDistance(dx, dy, dz, da, db, dc, &MP, &pure_angle);
	pm1->ChangeInDirection = CalcChangeInDirection(nsegs-1);
	pm1->Done=FALSE;

	// keep the last line number to avoid a chance of
	// backing up if we halt part way through
	pm1->sequence_number = pn->sequence_number;

	// if two different IDs are combined into a single
	// segment, then treat as if we are in the 2nd ID
	// (phase)
	pm1->ID = pn->ID;

	return 0;
}



// Reduce the allowed velocity based on the change of direction
// between this segment and the previous.  The idea is to find the
// max const speed that we could transverse a circle where the
// radial acceleration would equal our allowed acceleration
//
// This helps with the situation where the break angles are
// very small (smooth path) but the curvature is so high as
// to violate our allowed acceleration
//
// For now, we assume that our allowed radial acceleration
// is the same as our tangential acceleration
//
// Assume our const speed = s
//
// After "corner" our radial velocity will s * sin(angle)
// the time per this segment will be d/s
//
// Thus the accel would be A = (s * sin(T))/(d/s)
//                               2
//             or          A =  s  sin(T) / d
//
//     and,    s = sqrt(A * d / sin(T))
 


// from http://en.wikipedia.org/wiki/Radius

// R = dist(P1-P3)/(2sin(theta))


void AdjustSegVelocity(int i)
{
	if (i<1) return;  // first doesn't have a change in direction

	SEGMENT *p=GetSegPtr(i);

	// if stop is required between (i-1) and (i) segments don't adjust velocity
	if (fabs(p->ChangeInDirection) > BreakAngleRadians || p->StopRequired) return;


	SEGMENT *pm=GetSegPtr(i-1);

	double dx=p->x1 - pm->x0;
	double dy=p->y1 - pm->y0;
	double dz=p->z1 - pm->z0;
	double da=p->a1 - pm->a0;
	double db=p->b1 - pm->b0;
	double dc=p->c1 - pm->c0;

	BOOL pure_angle;
	double d = FeedRateDistance(dx, dy, dz, da, db, dc, &MP, &pure_angle);

	double sinT = sin(p->ChangeInDirection);  // seg contains angle (radians) from previous
	
	if (sinT == 0.0) return;

	double R = d/(2.0*sinT);

	// since, 
	// Pos = R sin(wt)
	// Vel = R w Cos(wt)
	// Accel = - R w^2 sin(wt)
	//
	// max Vel = Rw
	// max Accel = R w^2
	//
	// V = sqrt(A R)

	double A = p->MaxAccel;

	double s = sqrt(A * R);

	if (s < p->MaxVel) p->MaxVel = s;
}




// Reduce the allowed velocity based on the curvature.
//
// The idea is to find the max const speed that we could 
// transverse a circle where the radial acceleration would 
// equal our allowed acceleration
//
// radial acceleration is passed in
//
// Assume our const speed = s
//
// For a half circle the velocity will change from
// s to -s or a change of 2s
//
// The distance, D, for half a circle is PI * r
//
// the time to traverse a half circle = PI*r/s
//
// the acceleration A = 2s/(PI*r/s) = 2 s^2 / (PI*r)
//
// so, s = sqrt(A*PI*r/2)
// 
// NO Acceleration is sinusoidal not linear
//
// s = sqrt(A*r)

 
   

void AdjustSegVelocityCircle(int i, double A)
{
	SEGMENT *p=GetSegPtr(i);

	double dx = p->xc - p->x0;
	double dy = p->yc - p->y0;
	double r = sqrt(dx*dx+dy*dy);
//	double s = sqrt(A*PI*r/2);
	double s = sqrt(A*r);
	
	if (s < p->MaxVel) p->MaxVel = s;
}






void CalcFinalDirectionOfSegment(SEGMENT *p,double &dx, double &dy, double &dz,double &da, double &db, double &dc)
{
	if (p->type != SEG_ARC)
	{
		// simple linear case

		dx = p->x1 - p->x0;
		dy = p->y1 - p->y0;
		dz = p->z1 - p->z0;
		da = p->a1 - p->a0;
		db = p->b1 - p->b0;
		dc = p->c1 - p->c0;

		bool AisDist = (!MP.DegreesA || MP.RadiusA!=0.0);
		bool BisDist = (!MP.DegreesB || MP.RadiusB!=0.0);
		bool CisDist = (!MP.DegreesC || MP.RadiusC!=0.0);

		if (AisDist)
		{
			if (MP.DegreesA) da *= PI/180.0*MP.RadiusA;
		}
		if (BisDist)
		{
			if (MP.DegreesB) db *= PI/180.0*MP.RadiusB;
		}
		if (CisDist)
		{
			if (MP.DegreesC) dc *= PI/180.0*MP.RadiusC;
		}
	}
	else
	{
		// Arc case

		// calc direction (in xy plane) from
		// center of rotation to terminal point

		double dxc = p->x1 - p->xc;
		double dyc = p->y1 - p->yc;

		// then turn left 90 degrees if CCW, right if CW
		// (note dxy later will be neg if CW)

		dx = -dyc;
		dy =  dxc;

		// scale this vector to be length of total xy motion
		// right now it's length is the radius 

		double radius, theta0, dtheta;

		double dxy = CalcLengthAlongCircle(p->x0, p->y0, p->x1, p->y1, 
					  p->xc, p->yc, p->DirIsCCW, &radius, &theta0, &dtheta);

		dx *= dxy/radius;
		dy *= dxy/radius;
		dz = p->z1 - p->z0;
		da = p->a1 - p->a0;
		db = p->b1 - p->b0;
		dc = p->c1 - p->c0;

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
		}
	}
}


void CalcBegDirectionOfSegment(SEGMENT *p, double &dx, double &dy, double &dz, double &da, double &db, double &dc)
{
	if (p->type != SEG_ARC)
	{
		// simple linear case

		dx = p->x1 - p->x0;
		dy = p->y1 - p->y0;
		dz = p->z1 - p->z0;
		da = p->a1 - p->a0;
		db = p->b1 - p->b0;
		dc = p->c1 - p->c0;

		bool AisDist = (!MP.DegreesA || MP.RadiusA!=0.0);
		bool BisDist = (!MP.DegreesB || MP.RadiusB!=0.0);
		bool CisDist = (!MP.DegreesC || MP.RadiusC!=0.0);

		if (AisDist)
		{
			if (MP.DegreesA) da *= PI/180.0*MP.RadiusA;
		}
		if (BisDist)
		{
			if (MP.DegreesB) db *= PI/180.0*MP.RadiusB;
		}
		if (CisDist)
		{
			if (MP.DegreesC) dc *= PI/180.0*MP.RadiusC;
		}
	}
	else
	{
		// Arc case

		// calc direction (in xy plane) from
		// center of rotation to beginning point

		double dxc = p->x0 - p->xc;
		double dyc = p->y0 - p->yc;

		// then turn left 90 degrees if CCW, right if CW
		// (note dxy later will be neg if CW)

		dx = -dyc;
		dy =  dxc;

		// scale this vector to be length of total xy motion
		// right now it's length is the radius 

		double radius, theta0, dtheta;

		double dxy = CalcLengthAlongCircle(p->x0, p->y0, p->x1, p->y1, 
					  p->xc, p->yc, p->DirIsCCW, &radius, &theta0, &dtheta);

		dx *= dxy/radius;
		dy *= dxy/radius;
		dz = p->z1 - p->z0;
		da = p->a1 - p->a0;
		db = p->b1 - p->b0;
		dc = p->c1 - p->c0;

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
		}
	}
}




double CalcChangeInDirection(int i)
{
	if (i==0) return 0.0;  // return if first segment

	double adx,ady,adz,bdx,bdy,bdz;
	double ada,adb,adc,bda,bdb,bdc,dot;

	SEGMENT *a=GetSegPtr(i-1);
	SEGMENT *b=GetSegPtr(i);

	if (a->dx == 0.0 || b->dx == 0.0) return 0.0;

	bool AisDist = (!MP.DegreesA || MP.RadiusA!=0.0);
	bool BisDist = (!MP.DegreesB || MP.RadiusB!=0.0);
	bool CisDist = (!MP.DegreesC || MP.RadiusC!=0.0);

	CalcFinalDirectionOfSegment(a,adx,ady,adz,ada,adb,adc);
	CalcBegDirectionOfSegment(b,bdx,bdy,bdz,bda,bdb,bdc);

	bool PureAngleA = adx==0.0 && ady==0.0 && adz==0.0 && (ada==0.0 || !AisDist) && 
													      (adb==0.0 || !BisDist) && 
													      (adc==0.0 || !CisDist);

	bool PureAngleB = bdx==0.0 && bdy==0.0 && bdz==0.0 && (bda==0.0 || !AisDist) && 
													      (bdb==0.0 || !BisDist) && 
													      (bdc==0.0 || !CisDist);
	
	if (PureAngleA && PureAngleB)
	{
		dot = ada*bda + adb*bdb + adc*bdc;
	}
	else
	{
		dot = (adx) * (bdx) +
			  (ady) * (bdy) +
			  (adz) * (bdz);

		if (AisDist) dot += (ada) * (bda);
		if (BisDist) dot += (adb) * (bdb);
		if (CisDist) dot += (adc) * (bdc);
	}

	dot /= a->dx * b->dx;

	if (dot >  1.0) return 0.0;
	if (dot < -1.0) return PI;

	return acos(dot);
}


double CalcChangeInDirectionXYZ(int i)
{
	if (i==0) return 0.0;  // return if first segment

	double adx,ady,adz,bdx,bdy,bdz;
	double ada,adb,adc,bda,bdb,bdc;

	SEGMENT *a=GetSegPtr(i-1);
	SEGMENT *b=GetSegPtr(i);

	double lengtha = SegmentXYZLength(a);
	double lengthb = SegmentXYZLength(b);

	if (lengtha == 0.0 || lengthb == 0.0) return 0.0;

	CalcFinalDirectionOfSegment(a,adx,ady,adz,ada,adb,adc);
	CalcBegDirectionOfSegment(b,bdx,bdy,bdz,bda,bdb,bdc);

	double dot = (adx) * (bdx) +
		         (ady) * (bdy) +
		         (adz) * (bdz);

	dot /= lengtha * lengthb;

	if (dot >  1.0) return 0.0;
	if (dot < -1.0) return PI;

	return acos(dot);
}


double CalcLengthAlongHelix(double x0, double y0, double z0, 
					  double x1, double y1, double z1, 
					  double xc, double yc, BOOL DirIsCCW, 
					  double *radius, double *theta0, double *dtheta,
					  double da, double db, double dc, MOTION_PARAMS *MP, double *dcircle)
{
	double d,dz;

	d = CalcLengthAlongCircle(x0,y0,x1,y1,xc,yc,DirIsCCW,radius,theta0,dtheta);

	if (dcircle) *dcircle =d; // return to caller if requested

	d=d*d;

	dz = z1-z0;  // combine in helix linear motion
	d += dz*dz;

	bool AisDist = (!MP->DegreesA || MP->RadiusA!=0.0);
	bool BisDist = (!MP->DegreesB || MP->RadiusB!=0.0);
	bool CisDist = (!MP->DegreesC || MP->RadiusC!=0.0);

	if (AisDist)
	{
		if (MP->DegreesA) da *= PI/180.0*MP->RadiusA;
		d += da*da;
	}
	if (BisDist)
	{
		if (MP->DegreesB) db *= PI/180.0*MP->RadiusB;
		d += db*db;
	}
	if (CisDist)
	{
		if (MP->DegreesC) dc *= PI/180.0*MP->RadiusC;
		d += dc*dc;
	}

	return sqrt(d); // total length
}


double CalcLengthAlongCircle(double x0, double y0, 
					  double x1, double y1, 
					  double xc, double yc, BOOL DirIsCCW, 
					  double *radius, double *theta0, double *dtheta)
{	
	double t0,t1,dt,dx,dy,r;

	dx=x1-xc;
	dy=y1-yc;

	r = sqrt(dx*dx + dy*dy); 

	t0 = atan2(y0-yc,x0-xc);
	t1 = atan2(y1-yc,x1-xc);  
	                    
	dt =  t1 - t0;  
	
	if (fabs(dt) < THETA_SIGMA) dt=0;  // avoid confusing arc to same place with full circle

	if (DirIsCCW)
	{
		if (dt <= 0.0f) dt+=TWO_PI; // CCW delta should be +  
	}
	else
	{
		if (dt >= 0.0f) dt-=TWO_PI;  // CW delta should be -
	}
	
	*radius = r;  // return these to caller
	*theta0 = t0;
	*dtheta = dt;

	return  dt * r;  // path length along circle                                   
}



// insert new segment at the end.  Re-evaluate backwards
// through list to see if velocities could be increased

int tp_insert_arc_seg(CANON_PLANE plane, 
					  double x0, double y0, double z0, double a0, double b0, double c0,
					  double x1, double y1, double z1, double a1, double b1, double c1,
					  double xc, double yc, BOOL DirIsCCW,
					  double MaxVel, double MaxAccel, double MaxDecel, double MaxLength,
					  int sequence_number, int ID)
{
	double dx, radius, theta0, dtheta;
	
	dx = CalcLengthAlongHelix(x0,y0,z0,x1,y1,z1,xc,yc,DirIsCCW,&radius,&theta0,&dtheta,a1-a0,b1-b0,c1-c0,&MP, NULL);  // total length

	// add into the list
	SEGMENT *p=GetSegPtr(nsegs);

	p->type = SEG_ARC;
	p->plane = plane;
	p->sequence_number = sequence_number;
	p->ID = ID;
	
	p->x0 = x0;
	p->y0 = y0;
	p->z0 = z0;
	p->a0 = a0;
	p->b0 = b0;
	p->c0 = c0;
	
	p->x1 = x1;
	p->y1 = y1;
	p->z1 = z1;
	p->a1 = a1;
	p->b1 = b1;
	p->c1 = c1;

	p->xc = xc;
	p->yc = yc;
	
	p->DirIsCCW = DirIsCCW;

	p->dx = dx;
	p->MaxVel = p->OrigVel = MaxVel;
	p->MaxAccel=MaxAccel;
	p->MaxDecel=MaxDecel;
	p->OrigAccel=MaxAccel;
	p->vel=0.0f;

	p->ChangeInDirection = CalcChangeInDirection(nsegs);
	p->StopRequired=FALSE;
	p->special_cmds_first=p->special_cmds_last=-1; // init with no special commands
	
	p->Done=FALSE;
	nsegs++;

	return 0;
}


// Calc MaxEndVel0
// if there is no previous segment (i=0), set to zero 
// calc how far we would travel if we maintained
// MaxAccel until we achieved MaxVel

double MaximizeSegmentForward(int i)
{
	SEGMENT *p=GetSegPtr(i);

	double V0_b = p->vel;
	double VM_b = p->MaxVel;
	double A_b  = p->MaxAccel;
	double X_b  = p->dx;

	// time to achieve max vel
	double t_b = (VM_b - V0_b)/A_b;

	// dist to achieve max vel
	double d_b = (V0_b + 0.5f * A_b * t_b) * t_b;

	if (X_b > d_b)
	{
		// we need const vel phase
		return VM_b;
	}
	else
	{
		// must solve quadratic to determine time at end
		t_b = (-V0_b + sqrt(V0_b*V0_b + 2.0f*A_b*X_b))/A_b;

		return V0_b + t_b * A_b;
	}
}



// Calc MaxBegVel1
// (solve prob as if time is running backward)
// calc how far we would travel if we maintained
// MaxDeccel until we achieved MaxVel

// if there is a seg following, get its init vel
// otherwise we must stop


double MaximizeSegmentBackward(int i)
{
	double V0_e;

	if (i<nsegs-1)
		V0_e = GetSegPtr(i+1)->vel;
	else
		V0_e = 0.0f;

	SEGMENT *pi=GetSegPtr(i);
	double VM_e = pi->MaxVel;
	double A_e  = pi->MaxDecel;
	double X_e  = pi->dx;

	// time to achieve max vel
	double t_e = (VM_e - V0_e)/A_e;

	// dist to achieve max vel
	double d_e = (V0_e + 0.5f * A_e * t_e) * t_e;

	if (X_e > d_e)
	{
		// we need const vel phase
		return VM_e;
	}
	else
	{
		// must solve quadratic to determine time at end
		t_e = (-V0_e + sqrt(V0_e*V0_e + 2.0f*A_e*X_e))/A_e;

		return V0_e + t_e * A_e;
	}
}




// move backwards through the list and update velocities based on pairs (and triplets)


void MaximizeSegments()
{
	bool SomethingChanged,FirstPass=true,PassFinished;
	int i,LastDone;
	double MaxEndVel0,MaxBegVel1;

#ifdef DEBUG_DOWNLOAD
	ds.Format("MaximizeSegments %f 0 0 %d\n",DTimer.Elapsed_Seconds(),nsegs);
	PutString(ds);
#endif

	do 
	{
		SomethingChanged=false;

		PassFinished=false;
		for (i=nsegs-1; i>0 && !PassFinished; i--)
		{
			// consider two segments
			//
			//        (i-1)         (i)
			//        seg0         seg1
			//   X------------X------------X
			//  beg0         end0
			//               beg1         end1
			//
			//
			// assume velocities at beg0 and end1 are known
			//
			// calc max possible vel that could be achieved
			// by accelerating from beg0 to end0 (MaxEndVel0)
			//
			// calc max possible vel that we could have at
			// beg1 and still decel to end1 velocity (MaxBegVel1)
			//
			// we must use the lesser of the two.  once this
			// is known, calculate the motion through both
			// segments to achieve this
			//
			// Note: vel contained within the segment corresponds
			// to the velocity at the beginning of the segment


			SEGMENT *pi=GetSegPtr(i);
			SEGMENT *pm1=GetSegPtr(i-1);


			// is the segment a Rapid or dwell?
			if (pi->type==SEG_RAPID || pi->type==SEG_DWELL)
			{
				// yes simple case, stops on both ends are required and it is Done
				if (!pi->Done)
				{
					pi->Done=TRUE;
					pi->StopRequired=TRUE;
					SomethingChanged = true;
				}
			}
			else
			{
				// not a Rapid

				// check if stop is required between (i-1) and (i) segments
				if (fabs(pi->ChangeInDirection) > BreakAngleRadians || pm1->type==SEG_RAPID || pm1->type==SEG_DWELL)
				{
					if (!pi->StopRequired)
					{
						pi->StopRequired=TRUE;
						SomethingChanged = true;
					}
				}

				// Calc MaxEndVel0
				// if there is no previous segment (i=0), set to zero 
				// calc how far we would travel if we maintained
				// MaxAccel until we achieved MaxVel
				if (pi->StopRequired)
					MaxEndVel0 = 0.0f;
				else
					MaxEndVel0 = MaximizeSegmentForward(i-1);

				// Calc MaxBegVel1
				// (solve prob as if time is running backward)
				// calc how far we would travel if we maintained
				// MaxDeccel until we achieved MaxVel

				// if there is a seg following, get its init vel
				// otherwise we must stop
				MaxBegVel1 = MaximizeSegmentBackward(i);


				// check for a set of 3 to help go much faster
				// in the case of a tiny middle segment
 				if (i+1 < nsegs)
				{
					SEGMENT *pp1=GetSegPtr(i+1);

					// check if stop is required between (i) and (i+1) segments
					if (!pp1->StopRequired)
					{
						double MaxBegVel2 = MaximizeSegmentBackward(i+1);
						double min = MaxEndVel0;
						if (MaxBegVel2 < min) min = MaxBegVel2;
						if (pi->MaxVel < min) min = pi->MaxVel;
						// increase middle segment to min of all constraints if not already higher
						if (pi->vel < min)
						{
							ASSERT(!pi->Done);
							pi->vel = min;
							SomethingChanged = true;
						}
					}
				}




				// check which (or both) is limiting

				if (MaxEndVel0 <= MaxBegVel1)
				{
					// first segment is limiting velocity (MaxEndVel0 will be used).

					// if the value increased another pass might be necessary
					if (MaxEndVel0 > pi->vel) 
					{
						ASSERT(!pi->Done);
						SomethingChanged = true;
						pi->vel = MaxEndVel0;

					}

					// check if the i-1th segment can be determined to be "Done"
					// (beginning and ending velocity won't ever change)
					//
					// if it ends at MaxVel for the segment or
					// if the previous is Done and MaxAccel occurs
					// throughout the segment
					// 
					// The fact that the first seg is limiting velocity
					// is sufficient that it is maxed out.
					// 
					
					if (i<2 || GetSegPtr(i-2)->Done)
					{
						if (!pm1->Done)
						{
							pm1->Done=TRUE;
							SomethingChanged = true;
						}
					}
				}

				if (MaxEndVel0 >= MaxBegVel1)
				{
					// second segment is limiting velocity.
					
					// if the value increased another pass might be necessary
					if (MaxBegVel1 > pi->vel)
					{
						ASSERT(!pi->Done);
						pi->vel = MaxBegVel1;
						SomethingChanged = true;
					}

					// For a segment to be "Done" we must determine that BOTH ends can never be increased
					//
					// if a segment further to the right exists and is done
					// or starting at MaxVel or a Stop is required and the 
					// second segment is limiting, (unlimited on the left),
					// then the right segment is done
					if (i+1<nsegs)
					{
						SEGMENT *pp1=GetSegPtr(i+1);
						if (pp1->Done || pp1->StopRequired || pp1->vel == pi->MaxVel)
						{
							if (!pi->Done)
							{
								ASSERT(!pi->Done);
								pi->Done=TRUE;
								SomethingChanged = true;
							}
						}
					}
				}
			}

			// For a segment to be "Done" we must determine that BOTH ends can never be increased
			//
			// Any segment that has a Done segment or segment already beginning at
			// max velocity to the right, and the segment to the Left is
			// Done is Done.
			if ((pi->Done || pi->vel == pi->MaxVel || pi->StopRequired) && (i<2 || GetSegPtr(i-2)->Done))
			{
				if (!pm1->Done)
				{
					pm1->Done=TRUE;
					SomethingChanged = true;
				}
			}

			// if a Stop is required at the beginning of this segment
			// and the next segment is Done then this segment is Done.
			//
			// Any Segment between two Done segments must also be done
			// if a segment further to the right exists and is done
			// or starting at MaxVel and the left segment is Done 
			// then the right segment is done
			if (i+1<nsegs)
			{
				SEGMENT *pp1=GetSegPtr(i+1);
				if ((pm1->Done || pi->StopRequired) && (pp1->Done || pp1->vel == pi->MaxVel))
				{
					if (!pi->Done)
					{
						pi->Done=TRUE;
						SomethingChanged = true;
					}
				}
			}

		
			// How to determine if we went backward through enough
			// segments and that all the rest are done is complicated.
			//
			// Initially on the first pass all the "Done"s are contiguous on the left
			// but after the first pass there may be some scattered on the right
			//
			// So on the first pass remember where the first "Done" was encountered.
			//
			// But eventually any gaps between "Done" segments will be filled in because
			// if two ends will not ever change then everything between will be determined
			// 
			// Also advance it forward through any new "Done"s that might have been
			// set during the recent pass
			// 

			if (FirstPass)
			{
				if (i>0 && GetSegPtr(i-1)->Done)
				{
					PassFinished=true;
					LastDone = i-1;
					FirstPass=false;
				}
			}
			else
			{
				if (i-1 <= LastDone) 
					PassFinished=true;
			}

			if (PassFinished)
			{
				while (LastDone < nsegs-1 && GetSegPtr(LastDone+1)->Done) LastDone++;
			}
		}
	} while (SomethingChanged);

#if 0 // tktk
	// verify there are no gaps in the "Done"

	for (i=0; i<nsegs-1; i++)
		if (!GetSegPtr(i)->Done && GetSegPtr(i+1)->Done)
		{
			FILE *f=fopen("c:\\Temp\\data.txt","wt");

			int k=nsegs-40;
			if (k<0)k=0;
			for (;k<nsegs;k++)
			{
				fprintf (f,"segment,%d,type,%d,Done,%d,StopRequired,%d,ChangeInDirection,%.6f,vel,%.6f,MaxVel,%.6f,dx,%.6f\n",
					      k,GetSegPtr(k)->type,   GetSegPtr(k)->Done,   GetSegPtr(k)->StopRequired,   
						  GetSegPtr(k)->ChangeInDirection,     GetSegPtr(k)->vel,     GetSegPtr(k)->MaxVel,     GetSegPtr(k)->dx);
			}
			fclose(f);
			ASSERT(false);
			break;
		}
#endif

}




// calculate the trip states (three 2nd order polynomials)
// for a segment given the initial and ending velocities

int tp_calc_seg_trip_states(int i)
{
	double V0,V1,VM,X,A,D,ta,td,da,dd,tc;

	SEGMENT *p=GetSegPtr(i);

	// check for 3rd order 7 trip state case
	if (p->type == SEG_RAPID)
		return tp_calc_seg_trip_states_rapid(i);

	// check for dwell 1 trip state case
	if (p->type == SEG_DWELL)
		return tp_calc_seg_trip_states_dwell(i);

	V0 = p->vel;

	if (i<nsegs-1)
		V1 = GetSegPtr(i+1)->vel;
	else
		V1 = 0.0;

	VM = p->MaxVel;
	A  = p->MaxAccel;
	D  = p->MaxDecel;
	X  = p->dx;

	if (VM==0 || A==0 || D==0)
	{
		CString s;
		s.Format("Vel = %f Accel = %f Decel = %f",VM,A,D);
		AfxMessageBox("Trajectory Planner has Invalid Velocity or Acceleration " + s,MB_ICONSTOP|MB_OK);
		return 1;
	}


	// time to achieve max vel
	ta = (VM - V0)/A;

	// dist to achieve max vel
	da = (V0 + 0.5 * A * ta) * ta;

	// time to decel from max vel
	td = (VM - V1)/D;

	// dist to decel from max vel
	dd = (V1 + 0.5 * D * td) * td;

	if (X > da + dd)
	{
		// we need const vel phase

		tc = (X-da-dd)/VM;
	}
	else
	{
		// must solve to determine time at highest vel

		VM = sqrt((A*V1*V1+D*V0*V0+2.0f*A*D*X)/(A+D));

		ta = (VM-V0)/A;
		td = (VM-V1)/D;
		tc=0.0f;
	}

	p->C[0].a = 0;     
	p->C[0].b = 0.5*A;		// phase 0 - Accelerate
	p->C[0].c = V0;
	p->C[0].d = 0;          // parametric value relative to seg beginning
	p->C[0].t = ta;
	
	p->C[1].a = 0;
	p->C[1].b = 0;			// phase 1 - Const vel
	p->C[1].c = VM;
	p->C[1].d = 0.5*A*ta*ta + V0*ta; 
	p->C[1].t = tc;
	
	p->C[2].a = 0;
	p->C[2].b = -0.5*D;		// phase 2 - Decel
	p->C[2].c = VM;
	p->C[2].d = p->C[1].d + VM*tc; 
	p->C[2].t = td;

	p->nTrips=3;
	return 0;
}


/* calculate move profile parameters for 3rd order move to
   new Destination from previous position 
   given the maximum velocity, accel, and jerk

   basically, builds seven 3rd order polynomials
   which describe the position verses time for
   the seven states we go through during the move

   state 1 - ramp up acceleration
   state 2 - constant acceleration
   state 3 - ramp down acceleration
   state 4 - constant velocity
   state 5 - ramp up de-acceleration
   state 6 - constant de-acceleration
   state 7 - ramp down de- acceleration

   The above sequence is performed for long moves where maximum
   (and constant) velocity is achieved.  For medium length moves
   maximum acceleration is still achieved but max vel is not (the
   time for state 2 (and 6) is reduced, and the time for state 4 is
   zero). For even shorter moves, state 2, 4 and 6 are all zero
   and states 1, 3, 5, and 7 are reduced

   Note: if the Max Velocity is specified so slow that it is
   not ever possible to reach the max accel without exceeding
   the max velocity then other cases exist that are NOT handled
   correctly                                                     */




int tp_calc_seg_trip_states_rapid(int i)
{

	SEGMENT *p=GetSegPtr(i);

	double MaxV=p->MaxVel;         /* in inches and seconds units */
	double MaxA=p->MaxAccel;
   	double MaxJ=p->MaxJerk;
   	double r1,r2,qa,qb,qc,To,From;
	double NormalAccelDist,DistanceAtConstantVel;
	
	TP_COEFF *c0 = p->C;
	TP_COEFF *c1 = c0+1;
	TP_COEFF *c2 = c1+1;
	TP_COEFF *c3 = c2+1;
	TP_COEFF *c4 = c3+1;
	TP_COEFF *c5 = c4+1;
	TP_COEFF *c6 = c5+1;
	TP_COEFF *c7 = c6+1;
	


	// check what acceleration we would achieve if we ramped up and
	// back down the acceleration to give the max velocity.  If the
	// acceleration is less than the given max acceleration then 
	// use that as the max acceleration
	//                                 
	// area under triangular accel = 1/2 b h = 1/2 * 2 * T * T * J = Vmax
	// so,
	
	MaxA = sqrt(MaxV*MaxJ);
	
	if (MaxA > p->MaxAccel) MaxA = p->MaxAccel; 

	To=p->dx;
	From=0.0;
	
	
	// avoid divides by zero
	
	if (MaxV==0.0f || MaxA==0.0f || MaxJ==0.0f)	return -1;
	 
	
	/* Now fill in coeff for the new move */
	
	c0->t = MaxA/MaxJ;                                     /* state 1 */
	c0->a = MaxJ/6.0;
	c0->b = 0.0f;
	c0->c = 0.0f;
	c0->d = From;
	
	/* check if very short move, by checking if the distance
	  traveled by triangle shaped accel going up to max
	  accel would be larger than the distance to move      */
	
	if (fabs(To-From) < fabs(2.0*MaxA*c0->t*c0->t))
	{
		/* very short case, reduce state 1 time, state 2 time zero */
		
		c0->t = CubeRoot((To-From)/(MaxJ*2.0));
      
		c1->t = 0.0;        /* state 2 */
		c1->a = 0.0;
		c1->b = 0.0;
		c1->c = Vel(c0);
		c1->d = Pos(c0);

		c2->t = c0->t;     /* t same as state 1                  state 3 */
		c2->a = -MaxJ/6.0;
		c2->b = MaxJ*c0->t/2.0;
		c2->c = Vel(c1);
		c2->d = Pos(c1);

		c3->t = 0.0;                                            /* state 4 */
		c3->a = 0.0;
		c3->b = 0.0;
		c3->c = Vel(c2);
		c3->d = Pos(c2);

		c4->t = c0->t;                                          /* state 5 */
		c4->a = -MaxJ/6.0;
		c4->b = 0.0;
		c4->c = Vel(c3);
		c4->d = Pos(c3);

		c5->t = 0.0;                                            /* state 6 */
		c5->a = 0.0;
		c5->b = 0.0;
		c5->c = Vel(c4);
		c5->d = Pos(c4);

		c6->t = c0->t;                                          /* state 7 */
		c6->a = MaxJ/6;
		c6->b = -MaxJ*c0->t/2.0;
		c6->c = Vel(c5);
		c6->d = Pos(c5);
   }
   else
   {
		/* Not very short case, figure out if med or large */
      
		c1->t = (MaxV-2.0*Vel(c0))/MaxA;
		c1->a = 0.0;
		c1->b = MaxJ*c0->t/2.0;
		c1->c = Vel(c0);
		c1->d = Pos(c0);

		c2->t = c0->t;     /* t same as state 1                  state 3 */
		c2->a = -MaxJ/6.0;
		c2->b = MaxJ*c0->t/2.0;
		c2->c = Vel(c1);
		c2->d = Pos(c1);

		NormalAccelDist = Pos(c2) - From;


			/* check if medium length move, by checking if the distance
				traveled at the end of state 1 is more than 1/4 of
				the total distance required to move                 */

		if (fabs(To-From) < fabs(2.0*(Pos(c2)-From)))
		{
			/* medium length case, reduce state 2 time */

			qa = MaxJ * c0->t * 0.5;
			qb = 3.0 * MaxJ * c0->t * c0->t * 0.5;
			qc = MaxJ *c0->t * c0->t * c0->t - (To-From)*0.5;

			Quadradic(qa,qb,qc,&r1,&r2);

			if (r1 > 0)
				c1->t = r1;
			else
				c1->t = r2;

			/* now re-calculate states 2 and 3 */

			c1->a = 0.0;
			c1->b = MaxJ*c0->t/2.0;
			c1->c = Vel(c0);
			c1->d = Pos(c0);

			c2->t = c0->t;     /* t same as state 1                  state 3 */
			c2->a = -MaxJ/6.0;
			c2->b = MaxJ*c0->t/2.0;
			c2->c = Vel(c1);
			c2->d = Pos(c1);

			c3->t = ((To-From) - 2.0*(Pos(c2)-From))/MaxV;    /* state 4 */
			c3->a = 0.0;
			c3->b = 0.0;
			c3->c = Vel(c2);
			c3->d = Pos(c2);

			c4->t = c0->t;                                          /* state 5 */
			c4->a = -MaxJ/6.0;
			c4->b = 0.0;
			c4->c = Vel(c3);
			c4->d = Pos(c3);

			c5->t = c1->t;                                          /* state 6 */
			c5->a = 0.0;
			c5->b = -MaxJ*c0->t/2.0;
			c5->c = Vel(c4);
			c5->d = Pos(c4);

			c6->t = c0->t;                                          /* state 7 */
			c6->a = MaxJ/6.0;
			c6->b = -MaxJ*c0->t/2.0;
			c6->c = Vel(c5);
			c6->d = Pos(c5);
 		}
		else
		{
			/* Here we know that it is a large move (that reaches maximum velocity)
				while using jerk at he beginning and end. */
		
			c0->t = MaxA/MaxJ;                                     /* state 1 */
			c0->a = MaxJ/6.0;
			c0->b = 0.0;
			c0->c = 0.0;
			c0->d = From;

			c1->t = MaxV/MaxA-MaxA/MaxJ;         /* state 2 leftover time to get up to max Vel */
			c1->a = 0.0;
			c1->b = MaxA/2.0;                      /* at constant acceleration */    
			c1->c = Vel(c0);
			c1->d = Pos(c0);

			c2->t = MaxA/MaxJ;                   /* state 3 time to ramp down the Acceleration */
			c2->a = -MaxJ/6.0;
			c2->b = MaxA/2.0;                          
			c2->c = Vel(c1);
			c2->d = Pos(c1);

			/* now do state 4 */

			DistanceAtConstantVel = To-From;
			
			DistanceAtConstantVel -= NormalAccelDist;

			DistanceAtConstantVel -= NormalAccelDist;
			
			c3->t = DistanceAtConstantVel/MaxV;      /* state 4 */
			c3->a = 0.0;
			c3->b = 0.0;
			c3->c = Vel(c2);
			c3->d = Pos(c2);


			c4->t = MaxA/MaxJ;                                          /* state 5 */
			c4->a = -MaxJ/6.0;
			c4->b = 0.0;
			c4->c = Vel(c3);
			c4->d = Pos(c3);


			/* now states 6,7 */

			c5->t = MaxV/MaxA-MaxA/MaxJ;         
			
			c5->a = 0.0;
			c5->b = -MaxA/2.0;
			c5->c = Vel(c4);
			c5->d = Pos(c4);

			c6->t = MaxA/MaxJ;                                    /* state 7 */
			c6->a = MaxJ/6.0;
			c6->b = -MaxA/2.0;
			c6->c = Vel(c5);
			c6->d = Pos(c5);
		}
   }

	p->nTrips=7;
	return 0;
}  

/* calculates a position by evaluating a 3rd order
   polynomial given the "trip state" (that determines
   what set of polynomial coefficients to use,
   and the time (t).  Time is from the beginning
   of the trip state (not the beginning of the move)

           3    2
   Pos = at + bt + ct + d    which is equivalent to

   Pos = ((at+b)*t+c)*t+d                                 */


double Pos(TP_COEFF *p)
{                  
    register double t = p->t;
	return ((p->a * t + p->b)*t + p->c)*t + p->d;
}

/* similar to Pos above but takes the derivative
   of the Position polynomial to get the velocity

      2
   3at + 2bt + c                                      */

double Vel(TP_COEFF *p)
{
    register double t = p->t;
	return (3*p->a * t + 2*p->b)*t + p->c;
}

double CubeRoot(double v)
{
	volatile double TheLog, TheLog3, Exp;

	if (v < 1e-30) return 0.0; 
	
	TheLog = log(v);
	TheLog3 = TheLog * 0.33333333333333333333;
	Exp = exp(TheLog3);
	return Exp;
}

void Quadradic(double a, double b, double c, double *r1, double *r2)
{
   double sqrad, rad;

   rad = b*b-4.0*a*c;
   sqrad = sqrt(rad);

   *r1 = (-b + sqrad)/(2*a);
   *r2 = (-b - sqrad)/(2*a);
}



double SegmentXYZLength(SEGMENT *p)
{
	double dx = p->x1 - p->x0;
	double dy = p->y1 - p->y0;
	double dz = p->z1 - p->z0;

	return sqrt(dx*dx+dy*dy+dz*dz);
}


// Compute a single segment that just dwells a fixed time

int tp_calc_seg_trip_states_dwell(int i)
{
	SEGMENT *p=GetSegPtr(i);

	TP_COEFF *c0 = p->C;
	
	// Now fill in coeff for the dwell
	// One segment no motion for fixed time

	c0->t = p->dwell_time;
	c0->a = 0.0;
	c0->b = 0.0;
	c0->c = 0.0;
	c0->d = 0.0;
	
	p->nTrips=1;
	return 0;
}  

// compute total distance tool will move by considering both linear and angular movements  

double FeedRateDistance(double dx, double dy, double dz, double da, double db, double dc, MOTION_PARAMS *MP,BOOL *PureAngle)
{

	//  Every axis is either a pure linear distance or
	//  a pseudo linear distance by combining an angle with radius
	//  otherwise it is a pure angle. Only ABC can be pure angles
	//
	//  There are only two cases.  Either all moved axes are pure angles or not.
	// 
	//  if all are pure angles
	//      compute distance in degrees
	//
	//  if not
	//      compute as all orthogonal distances (excluding any pure angles)
	double d;

	bool AisDist = (!MP->DegreesA || MP->RadiusA!=0.0);
	bool BisDist = (!MP->DegreesB || MP->RadiusB!=0.0);
	bool CisDist = (!MP->DegreesC || MP->RadiusC!=0.0);

	*PureAngle = dx==0.0 && dy==0.0 && dz==0.0 && (da==0.0 || !AisDist) && 
	                                              (db==0.0 || !BisDist) && 
											      (dc==0.0 || !CisDist);
	if (*PureAngle)
	{
		d = da*da + db*db + dc*dc;
	}
	else
	{
		d = dx*dx + dy*dy + dz*dz;
		if (AisDist)
		{
			if (MP->DegreesA) da *= PI/180.0*MP->RadiusA;
			d += da*da;
		}
		if (BisDist)
		{
			if (MP->DegreesB) db *= PI/180.0*MP->RadiusB;
			d += db*db;
		}
		if (CisDist)
		{
			if (MP->DegreesC) dc *= PI/180.0*MP->RadiusC;
			d += dc*dc;
		}
	}
	return sqrt(d);
}
