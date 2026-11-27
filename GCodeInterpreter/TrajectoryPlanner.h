// TrajectoryPlanner.h  Tom Kerekes 09/11/02
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/

#if !defined(TrajectoryPlanner__INCLUDED_)
#define TrajectoryPlanner__INCLUDED_

enum {SEG_UNDEFINED,SEG_LINEAR,SEG_ARC,SEG_RAPID,SEG_DWELL};

#define MAX_TP_SEGMENTS 65536

#define MAX_TP_ACTUATORS 8      // per-actuator constraint table size
                                // (independent of Kinematics MAX_ACTUATORS)

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
	double	MaxAccelU;
	double	MaxAccelV;
	double	MaxVelX;
	double	MaxVelY;
	double	MaxVelZ;
	double	MaxVelA;
	double	MaxVelB;
	double	MaxVelC;
	double	MaxVelU;
	double	MaxVelV;

	double	MaxRapidJerkX;  // used for 3rd order Rapids
	double	MaxRapidJerkY;
	double	MaxRapidJerkZ;
	double	MaxRapidJerkA;
	double	MaxRapidJerkB;
	double	MaxRapidJerkC;
	double	MaxRapidJerkU;
	double	MaxRapidJerkV;
	double	MaxRapidAccelX;
	double	MaxRapidAccelY;
	double	MaxRapidAccelZ;
	double	MaxRapidAccelA;
	double	MaxRapidAccelB;
	double	MaxRapidAccelC;
	double	MaxRapidAccelU;
	double	MaxRapidAccelV;
	double	MaxRapidVelX;
	double	MaxRapidVelY;
	double	MaxRapidVelZ;
	double	MaxRapidVelA;
	double	MaxRapidVelB;
	double	MaxRapidVelC;
	double	MaxRapidVelU;
	double	MaxRapidVelV;

	double	SoftLimitNegX;  // Soft Limits (inches)
	double	SoftLimitNegY;
	double	SoftLimitNegZ;
	double	SoftLimitNegA;
	double	SoftLimitNegB;
	double	SoftLimitNegC;
	double	SoftLimitNegU;
	double	SoftLimitNegV;
	double	SoftLimitPosX;
	double	SoftLimitPosY;
	double	SoftLimitPosZ;
	double	SoftLimitPosA;
	double	SoftLimitPosB;
	double	SoftLimitPosC;
	double	SoftLimitPosU;
	double	SoftLimitPosV;

	double	CountsPerInchX;
	double	CountsPerInchY;
	double	CountsPerInchZ;
	double	CountsPerInchA;
	double	CountsPerInchB;
	double	CountsPerInchC;
	double	CountsPerInchU;
	double	CountsPerInchV;
	double	MaxLinearLength;
	double	MaxAngularChange;

	bool ArcsToSegs;
	bool DegreesA;
	bool DegreesB;
	bool DegreesC;

	bool UseOnlyLinearSegments;
	bool DoRapidsAsFeeds;

	double MaxRapidFRO;

	bool TCP_Active;  // tool center point control active
	double TCP_X, TCP_Y, TCP_Z;  // tool center point offsets

	// ----- Constraints in Actuator Space option ---------------------------
	//
	// ActuatorLimits false (default, legacy): TP constraints are the
	//    CAD-space MaxVel/MaxAccel/etc above.  CAD dimension i corresponds
	//    1:1 with Actuator i and CountsPerInch is the only transformation,
	//    so for linear systems the mapped behavior is identical to legacy.
	//
	// ActuatorLimits true: TP constraints come from the per-actuator table
	//    below, in ACTUATOR ENGINEERING UNITS (inches or degrees of actuator
	//    motion).  For nonlinear Kinematics the CADtoActuators transform
	//    output is in actuator units and ActScale (counts per actuator
	//    unit) is applied only at the final output stage, so changing an
	//    encoder resolution never changes the planned motion.
	//
	// Feedrate F and path tolerances always remain CAD-space quantities.

	bool	ActuatorLimits;                    // enable per-actuator constraints
	double	ActScale[MAX_TP_ACTUATORS];        // counts per actuator unit
	double	MaxActVel[MAX_TP_ACTUATORS];       // actuator units/sec
	double	MaxActAccel[MAX_TP_ACTUATORS];     // actuator units/sec^2
	double	MaxActJerk[MAX_TP_ACTUATORS];      // actuator units/sec^3 (3rd order TP)
	bool	ActDegrees[MAX_TP_ACTUATORS];      // actuator unit is degrees (rotary),
	                                           // else linear inches (mm is a
	                                           // display-only conversion)

	// ----- 3rd order (jerk limited) Trajectory Planner option -------------
	//
	// ThirdOrderTP selects the new jerk-limited planner for feeds.  MaxJerk
	// below are ENTERED per-CAD-axis jerk limits used together with
	// MaxVel/MaxAccel by that planner.  They are deliberately separate from
	// MaxRapidJerk above: those are uploaded from the Controller Axes and
	// describe stop-to-stop S-curve Rapids, which is a different context
	// than continuous-path planning limits.

	bool	ThirdOrderTP;
	bool	LogSegments;    // log every segment downloaded to the controller
	                        // (sampled positions) to c:\Temp\TPSegLog.csv
	bool	CubicKnots;     // download 3rd Order TP knots as per-axis cubic
	                        // polynomials of time (CHex8/TRAJECTORY_CUBIC8,
	                        // requires updated KFLOP/Kogna firmware) instead
	                        // of chord-angle-capped linear segments - removes
	                        // the polygonization kinks entirely
	double	MaxJerkX;
	double	MaxJerkY;
	double	MaxJerkZ;
	double	MaxJerkA;
	double	MaxJerkB;
	double	MaxJerkC;
	double	MaxJerkU;
	double	MaxJerkV;

} MOTION_PARAMS;



typedef struct  // 2nd order polynomial for a single trip state
{
	double t;   // time duration (in sec) of trip state
	double a;   // t^3 coefficient  (jerk)
	double b;   // t^2 coefficient  (acceleration)
	double c;   // t^1 coefficient  (initial velocity)
	double d;   // t^0 constant coefficient (starting position)
} TP_COEFF;


typedef struct SEGMENT_STRUCT
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
	double x0, y0, z0, a0, b0, c0, u0, v0; // starting point
	double x1, y1, z1, a1, b1, c1, u1, v1; // ending point
	double xc,yc;		// center (if arc)
	double dwell_time;  // dwell time if dwell segment
	int special_cmds_first;   // Special commands start index to be inserted after the segment (-1=none)
	int special_cmds_last;    // Special commands ending index at the very beginning of path

	BOOL DirIsCCW;
	BOOL StopRequired;			// Change in Direction (or other reason) requires a stop at the beg of this seg
	BOOL StopRequiredNextSeg;	// Change in Direction (or other reason) requires a stop at the beg of next seg
	double ChangeInDirection;  // angle from previous seg to this one

	BOOL PrecomputedTrips;	// trip states C[] were precomputed (3rd Order TP
							// cubic knot); never recompute or replan them
	BOOL ActSpace;			// x0..v1 are ACTUATOR units (not CAD); download
							// scales by the effective ActScale instead of
							// calling TransformCADtoActuators
	BOOL Cubic8;			// per-axis cubic knot (TRAJECTORY_CUBIC8): the 32
							// axis coefficients (8 axes x a,b,c,d in letter
							// order) re-use the trip storage as raw doubles
							// beginning at C[0].a (see SEG_CUBIC8_COEFFS);
							// C[0].t remains the knot duration and nTrips=1
} SEGMENT;

// Cubic8 coefficient view: 32 contiguous doubles C[0].a..C[6].b holding
// a,b,c,d per axis (position = ((a*t+b)*t+c)*t+d, t from knot start).
// Mirrors the firmware's PARAMETRIC_COEFF storage re-use.
#define SEG_CUBIC8_COEFFS(p) (&(p)->C[0].a)

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
extern int special_cmds_initial_sequence_no[2];    // Special commands initial sequence number
extern SPECIAL_CMD special_cmds[MAX_SPECIAL_CMDS];
extern int ispecial_cmd_downloaded;


// initialize for new list of segments
// define endpoint to have zero vel

void tp_init();

// compute total distance tool will move by considering both linear and angular movements  
double FeedRateDistance(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, MOTION_PARAMS *MP,BOOL *PureAngle);

void SetTrajectoryPlannerParams(MOTION_PARAMS *m);

// Resolve the effective constraints for actuator i (0..MAX_TP_ACTUATORS-1):
//   ActuatorLimits true : directly from the per-actuator table
//   ActuatorLimits false: legacy 1:1 mapping from the corresponding CAD
//                         axis (X,Y,Z,A,B,C,U,V) with CountsPerInch as the
//                         scale and the entered MaxJerk as the jerk limit
//                         (NOT the uploaded stop-to-stop Rapid jerk)
// All planning code should obtain per-actuator constraints through this
// one function so both modes flow through a single path.
void GetEffectiveActuatorLimits(MOTION_PARAMS *m, int i,
                                double *Scale, double *Vel,
                                double *Accel, double *Jerk);

// insert new segment at the end.  Re-evaluate backwards
// through list to see if velocities could be increased

// G61/G61.1/G64 path-control state (interpreter modal, survives
// SetTrajectoryPlannerParams): programmed corner tolerance override in
// inches (< 0 = use MP.CornerTol) and exact-stop-between-blocks flag
void tp_set_corner_tol_override(double tol);
void tp_set_exact_stop(BOOL ExactStop);

int tp_insert_linear_seg(double x0, double y0, double z0, double a0, double b0, double c0, double u0, double v0,
						 double x1, double y1, double z1, double a1, double b1, double c1, double u1, double v1, 
						 double MaxVel, double MaxAccel, double MaxCombineLength, int sequence_number, int ID, int NumLinearNotDrawn);

int tp_insert_linear_seg_3rdOrder(double x0, double y0, double z0, double a0, double b0, double c0, double u0, double v0, 
								  double x1, double y1, double z1, double a1, double b1, double c1, double u1, double v1, 
						          int sequence_number,int ID);

int tp_insert_dwell(double t, double x0, double y0, double z0, double a0, double b0, double c0, double u0, double v0, int sequence_number,int ID);

// insert a finalized 3rd Order TP "cubic knot": straight chord in ACTUATOR
// units p0[8] -> p1[8] of metric length d traversed in time T with a single
// cubic trip state matching boundary path speeds v0,v1
int tp_insert_precomputed_cubic(const double *p0, const double *p1, double d, double T,
	double v0, double v1, int sequence_number, int ID);

// insert a finalized 3rd Order TP per-axis cubic knot (TRAJECTORY_CUBIC8):
// K[32] = 8 axes x a,b,c,d in letter order (ACTUATOR units, t in sec from
// knot start), duration T, metric path length d and status speed vmax
int tp_insert_cubic8(const double *K, double T, double d, double vmax,
	int sequence_number, int ID);


int tp_insert_arc_seg(CANON_PLANE plane, 
					  double x0, double y0, double z0, double a0, double b0, double c0, double u0, double v0,
					  double x1, double y1, double z1, double a1, double b1, double c1, double u1, double v1, 
					  double xc, double yc, BOOL DirIsCCW,
					  double MaxVel, double MaxAccel, double MaxDecel, double MaxLength, int sequence_number, int ID);

double CalcLengthAlongHelix(double x0, double y0, double z0, 
					  double x1, double y1, double z1, 
					  double xc, double yc, BOOL DirIsCCW, 
					  double *radius, double *theta0, double *dtheta,
					  double da, double db, double dc, double du, double dv, MOTION_PARAMS *MP, double *dcircle=NULL);


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
bool CheckCollinear(SEGMENT *s0, SEGMENT *s1, SEGMENT *s2, double tol);


void SetSegmentVelAccels(int i, double Vel, double Accel, double Decel);
void SetSegmentVelAccelJerk(int i, double Vel, double Accel, double Jerk);
void GetSegmentDirection(int i, double *dx, double *dy, double *dz, double *da, double *db, double *dc, double *du, double *dv);

void CalcFinalDirectionOfSegment(SEGMENT *p,double &dx, double &dy, double &dz, double &da, double &db, double &dc, double &du, double &dv);
void CalcBegDirectionOfSegment(SEGMENT *p,double &dx, double &dy, double &dz, double &da, double &db, double &dc, double &du, double &dv);
bool PureAngle(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, MOTION_PARAMS* MP);
bool PureAngle(SEGMENT* p);

SEGMENT *GetSegPtr(int i);
int TPMOD(int i);
#endif
