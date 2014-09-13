// TrajectoryPlanner.h  Tom Kerekes 09/11/02
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/

#if !defined(TrajectoryPlanner__INCLUDED_)
#define TrajectoryPlanner__INCLUDED_

enum {SEG_UNDEFINED,SEG_LINEAR,SEG_ARC,SEG_RAPID,SEG_DWELL};

#define MAX_TP_SEGMENTS 65536 

#define DIR_CCW true
#define DIR_CW false

typedef struct
{
	double	BreakAngle;
	double	CollinearTol;
	double	CornerTol;
	double	FacetAngle;
	double	TPLookahead;
	double	RadiusA;
	double	RadiusB;
	double	RadiusC;
	double	MaxAccelX;
	double	MaxAccelY;
	double	MaxAccelZ;
	double	MaxAccelA;
	double	MaxAccelB;
	double	MaxAccelC;
	double	MaxVelX;
	double	MaxVelY;
	double	MaxVelZ;
	double	MaxVelA;
	double	MaxVelB;
	double	MaxVelC;

	double	MaxRapidJerkX;  // used for 3rd order Rapids
	double	MaxRapidJerkY;
	double	MaxRapidJerkZ;
	double	MaxRapidJerkA;
	double	MaxRapidJerkB;
	double	MaxRapidJerkC;
	double	MaxRapidAccelX;
	double	MaxRapidAccelY;
	double	MaxRapidAccelZ;
	double	MaxRapidAccelA;
	double	MaxRapidAccelB;
	double	MaxRapidAccelC;
	double	MaxRapidVelX;
	double	MaxRapidVelY;
	double	MaxRapidVelZ;
	double	MaxRapidVelA;
	double	MaxRapidVelB;
	double	MaxRapidVelC;

	double	SoftLimitNegX;  // Soft Limits (inches)
	double	SoftLimitNegY;
	double	SoftLimitNegZ;
	double	SoftLimitNegA;
	double	SoftLimitNegB;
	double	SoftLimitNegC;
	double	SoftLimitPosX;
	double	SoftLimitPosY;
	double	SoftLimitPosZ;
	double	SoftLimitPosA;
	double	SoftLimitPosB;
	double	SoftLimitPosC;

	double	CountsPerInchX;
	double	CountsPerInchY;
	double	CountsPerInchZ;
	double	CountsPerInchA;
	double	CountsPerInchB;
	double	CountsPerInchC;
	double	MaxLinearLength;

	bool UseOnlyLinearSegments;
	bool ArcsToSegs;
	bool DegreesA;
	bool DegreesB;
	bool DegreesC;
} MOTION_PARAMS; 



typedef struct  // 2nd order polynomial for a single trip state
{
	double t;   // time duration (in sec) of trip state
	double a;   // t^3 coefficient  (jerk)
	double b;   // t^2 coefficient  (acceleration)
	double c;   // t^1 coefficient  (initial velocity)
	double d;   // t^0 constant coefficient (starting position)
} TP_COEFF;


typedef struct
{
	double dx;			// path length of segment
	double MaxVel,OrigVel;
	double MaxAccel,MaxDecel,OrigAccel;
	double MaxJerk;
	double vel;          // beginning velocity of segment
	BOOL Done;			// segment's ending vel can't ever be increased,
						// (it is either: at the MaxVel, or the entire segment 
						// is at MaxAccel and the prev segment is Done)

	TP_COEFF C[7];      // polynomials for accel, const vel, decel, plus 4 Jerks for Rapids
	int nTrips;

	int type;           // SEG_LINEAR or SEG_ARC
	CANON_PLANE plane;  // used for arc CANON_PLANE_XY,CANON_PLANE_XZ,CANON_PLANE_YZ
	int sequence_number;  // GCode sequence_number associated with this segment
	int ID;             // GCode mechanism that generated this
	double x0,y0,z0,a0,b0,c0; // starting point
	double x1,y1,z1,a1,b1,c1; // ending point
	double xc,yc;		// center (if arc)
	double dwell_time;  // dwell time if dwell segment
	int special_cmds_first;   // Special commands start index to be inserted after the segment (-1=none)
	int special_cmds_last;    // Special commands ending index at the very beginning of path

	BOOL DirIsCCW;		
	BOOL StopRequired;	// Change in Direction requires a stop at the beg of this seg
	double ChangeInDirection;  // angle from previous seg to this one
} SEGMENT;

extern int nsegs,prev_nsegs,SegBufToggle;
extern int SegsDone[2];
extern double SegsDoneTime[2];
extern SEGMENT *segments, *segments_executing;
extern SEGMENT segments0[MAX_TP_SEGMENTS];
extern SEGMENT segments1[MAX_TP_SEGMENTS];

#define MAX_SPECIAL_CMDS 1000

// special commands such as SetBit that should occur along the path

typedef struct
{
	char cmd[MAX_LINE];
} SPECIAL_CMD;


extern int nspecial_cmds;
extern int special_cmds_initial_first;   // Special commands start index at the very beginning of path
extern int special_cmds_initial_last;    // Special commands ending index at the very beginning of path
extern SPECIAL_CMD special_cmds[MAX_SPECIAL_CMDS];
extern int ispecial_cmd_downloaded;


// initialize for new list of segments
// define endpoint to have zero vel

void tp_init();

// compute total distance tool will move by considering both linear and angular movements  
double FeedRateDistance(double dx, double dy, double dz, double da, double db, double dc, MOTION_PARAMS *MP,BOOL *PureAngle);

void SetTrajectoryPlannerParams(MOTION_PARAMS *m);

// insert new segment at the end.  Re-evaluate backwards
// through list to see if velocities could be increased

int tp_insert_linear_seg(double x0, double y0, double z0, double a0, double b0, double c0, 
						 double x1, double y1, double z1, double a1, double b1, double c1, 
						 double MaxVel, double MaxAccel, double MaxCombineLength, int sequence_number, int ID);

int tp_insert_linear_seg_3rdOrder(double x0, double y0, double z0, double a0, double b0, double c0, 
								  double x1, double y1, double z1, double a1, double b1, double c1, 
						          int sequence_number,int ID);

int tp_insert_dwell(double t, double x0, double y0, double z0, double a0, double b0, double c0, int sequence_number,int ID);


int tp_insert_arc_seg(CANON_PLANE plane, 
					  double x0, double y0, double z0, double a0, double b0, double c0,
					  double x1, double y1, double z1, double a1, double b1, double c1, 
					  double xc, double yc, BOOL DirIsCCW,
					  double MaxVel, double MaxAccel, double MaxDecel, double MaxLength, int sequence_number, int ID);

double CalcLengthAlongHelix(double x0, double y0, double z0, 
					  double x1, double y1, double z1, 
					  double xc, double yc, BOOL DirIsCCW, 
					  double *radius, double *theta0, double *dtheta,
					  double da, double db, double dc, MOTION_PARAMS *MP, double *dcircle=NULL);


// calculate the trip states (three 2nd order polynomials)
// or 7 3rd order polynomials
// for a segment given the initial and ending velocities

int tp_calc_seg_trip_states(int i);

// calculate the trip states for 3rd order rapid case
// 7 3rd order polynomials
// initial and ending velocities are zero
int tp_calc_seg_trip_states_rapid(int i);

int tp_calc_seg_trip_states_dwell(int i);


// Maximize what we have so far assuming a stop at the end
void MaximizeSegments();

void AdjustSegVelocity(int i);
void AdjustSegVelocityCircle(int i, double A);

void RoundCorner(int i);

void SetSegmentVelAccels(int i, double Vel, double Accel, double Decel);
void SetSegmentVelAccelJerk(int i, double Vel, double Accel, double Jerk);
void GetSegmentDirection(int i, double *dx, double *dy, double *dz, double *da, double *db, double *dc);

void CalcFinalDirectionOfSegment(SEGMENT *p,double &dx, double &dy, double &dz, double &da, double &db, double &dc);
void CalcBegDirectionOfSegment(SEGMENT *p,double &dx, double &dy, double &dz, double &da, double &db, double &dc);

SEGMENT *GetSegPtr(int i);
int TPMOD(int i);
#endif
