// CoordMotion.h: interface for the CCoordMotion class.
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#if !defined(AFX_COORDMOTION_H_INCLUDED_)
#define AFX_COORDMOTION_H_INCLUDED_

#include "KMotionDLL.h"
#include "canon.h"
#include "TrajectoryPlanner.h"
#include "Kinematics.h"	// Added by ClassView


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



typedef void STRAIGHT_TRAVERSE_CALLBACK(double x, double y, double z, int sequence_number);

typedef void STRAIGHT_TRAVERSE_SIX_AXIS_CALLBACK(double x, double y, double z, double a, double b, double c, int sequence_number);

typedef void STRAIGHT_FEED_CALLBACK(double DesiredFeedRate_in_per_sec,
							   double x, double y, double z, int sequence_number, int ID);

typedef void STRAIGHT_FEED_CALLBACK_SIX_AXIS(double DesiredFeedRate_in_per_sec,
							   double x, double y, double z, double a, double b, double c, int sequence_number, int ID);

typedef void ARC_FEED_CALLBACK(bool ZeroLenAsFullCircles, double DesiredFeedRate_in_per_sec, 
			    CANON_PLANE plane,
				double first_end, double second_end, 
		        double first_axis, double second_axis, int rotation,
				double axis_end_point,
				double first_start, double second_start, double axis_start_point, int sequence_number, int ID);
typedef void ARC_FEED_SIX_AXIS_CALLBACK(bool ZeroLenAsFullCircles, double DesiredFeedRate_in_per_sec, 
			    CANON_PLANE plane,
				double first_end, double second_end, 
		        double first_axis, double second_axis, int rotation,
				double axis_end_point,double a, double b, double c,
				double first_start, double second_start, double axis_start_point, int sequence_number, int ID);

enum {STOPPED_NONE,STOPPED_INDEP,STOPPED_COORD,STOPPED_COORD_FINISHED};

class GCODEINTERPRETER_API CCoordMotion
{
public:
	CKinematics *Kinematics;
	class CTP3FeedRun *m_TP3Feed = NULL;  // pending 3rd Order feed run (lazily allocated)
	bool m_TP3Flushing = false;           // TP3FlushRun reentry guard

	// G61/G61.1/G64 path control (interpreter modal state, set via the
	// SET_MOTION_CONTROL_MODE canon call; separate from the Tool Setup
	// CornerTol which stays the machine default)
	CANON_MOTION_MODE m_PathMode = CANON_CONTINUOUS;
	double m_PathTol = -1.0;              // G64 P inches; < 0 = UI CornerTol

	// Per-axis TOOL-TIP error weights from the kinematics Jacobian:
	// inches of tip motion per unit of normalized actuator-i motion
	// (rotary: local pivot radius per degree).  Probed per waypoint,
	// cached on the rotary pose.
	int ComputeTipWeights(double x, double y, double z, double a, double b,
		double c, double u, double v, double *wgt);
	bool m_TipWgtValid = false;
	double m_TipWgtA = 0, m_TipWgtB = 0, m_TipWgtC = 0;
	double m_TipWgt[8];
	void SetPathMode(CANON_MOTION_MODE mode, double tolInches);
	double EffectiveCornerTol();          // 0 in exact path/stop modes
	bool PathExact() { return m_PathMode != CANON_CONTINUOUS || m_PathTol == 0.0; }

	// CAD-space corner fairing for DRAWING the TP3 path: the planner
	// rounds corners in actuator space where the G-view cannot see it,
	// so these mirror the blend geometry in CAD space for the drawing
	// callbacks (one waypoint of lag, endpoints exact).  Also used in
	// Simulate mode where the planner never runs.
	class CTP3DrawWpt {
	public:
		double p[8];
		double F;         // drawing feedrate (in/sec)
		BOOL rapid;
		BOOL nocb;        // suppress callbacks for this segment
		int seq, seqT, id;// seq for feed cbs, _setup seq for traverse cbs
		double tol;       // CAD blend tolerance (inches)
	};
	CTP3DrawWpt m_TP3DrawW0, m_TP3DrawW1;  // previous two raw waypoints
	int m_TP3DrawHave = 0;                 // 0 = idle, 2 = seed+target held
	double m_TP3DrawLast[8];               // last drawn point
	void TP3DrawKnotCAD(const double *pNorm, const double *cadAnchor, double F, bool rapid, int seq, int ID);
	int m_TP3DrawFailStreak;    // consecutive undrawable knots: >=3 switches
	                            // the draw inversions to cheap probe mode
	void TP3DrawEmitPiece(const CTP3DrawWpt &attr, const double *pt);
	void TP3DrawVertex(const CTP3DrawWpt &wm1, const CTP3DrawWpt &w0, const CTP3DrawWpt &wp1);
	void TP3DrawFlush();

	// Trajectory Planner segment log (MP->LogSegments): every segment
	// downloaded to the controller is appended as one CSV row (geometry
	// + trip polynomials at float resolution) - fresh file per program.
	// MotionLogPlotter re-creates the 90us servo interpolation from it.
	FILE *m_SegLog = NULL;
	bool m_SegLogFresh = true;    // next open truncates (new interp run)
	void SegLogSegment(SEGMENT *p);
	void SegLogClose();
	void SegLogNewRun();          // called at interpreter launch only
	void CloseLogs();             // segment log + TP3 timeline (abort path)

	// real (non-simulate) TP3 runs draw the ACTUAL downloaded knot path
	// instead: each knot endpoint (normalized actuator space) is mapped
	// back through the inverse kinematics to CAD and drawn - this is the
	// post-fairing, post-compensation, post-filter trajectory exactly as
	// sent to the controller
	void DownloadInit();

	// SetStarveTime support: worst-case stop time of one segment's
	// content and the value last declared to the controller.  Increases
	// are sent BEFORE downloading the faster content; as execution
	// consumes the buffer the declaration is LOWERED to cover only what
	// remains (see UpdateStarveTime) using a ring of per-segment records.
	double SegWorstStopTime(SEGMENT *p);
	void UpdateStarveTime();
	double TP3EmissionMarginEstimate();
	double TP3ActualEmissionMargin();   // live margin from the streaming
	                                    // planner (falls back to the
	                                    // estimate before the first replan)
	double TP3PlannedStopTime();        // worst-case time to ramp planned
	                                    // content to rest at the PLANNER'S
	                                    // per-actuator limits (v/a + a/j)
	bool TP3SlotInUse(int i);

	// set while a streaming client (coordinated jog) has temporarily
	// installed its own short TPLookahead: suppresses the "Controller
	// takes longer to stop than 75% of Lookahead" warning, which refers
	// to the user's configured Lookahead
	bool m_LookaheadOverride;

	// set by the jog RELEASE handler (GUI thread) just before it stops
	// the controller: the streaming worker may be inside a
	// controller-paced wait (download pacing polls ExecTime, which a
	// frozen controller never advances) - the wait honors this flag and
	// bails so the worker can run its teardown.  Cleared by the worker
	// at jog start and teardown.
	volatile bool m_JogReleaseRequested;
	double m_StarveTimeSent;
	enum { STARVE_LIST_SIZE = 8192 };
	double m_StarveEndT[STARVE_LIST_SIZE];   // buffer time at segment end
	double m_StarveStopT[STARVE_LIST_SIZE];  // that segment's stop time
	int m_StarveHead, m_StarveTail;          // ring [tail..head)
	bool m_StarveOverflow;                   // full: degrade to ratchet-only

	// Reset the host planner/download state after the firmware buffer
	// was abandoned (StopImmediate2) - the same reset FlushSegments
	// finishes with, callable WITHOUT raising the global abort flag
	// (which would invite a concurrent ClearAbort from another thread
	// into the same teardown)
	void AbandonCoordBuffer();

	// Launch execution of the downloaded coordinated buffer NOW if it has
	// not already started.  OutputSegment normally auto-launches only
	// once PLANNED downloaded time exceeds TPLookahead - a streaming jog
	// that paces itself well below the lookahead (especially a SLOW jog,
	// whose planned time barely exceeds real time) would otherwise never
	// start.  Safe to call repeatedly.
	int LaunchIfNotStarted();
	// Re-arm LaunchIfNotStarted after the controller finished (ran dry on)
	// the downloaded buffer: the next call sends ExecBuf again and the
	// controller resumes from where it stopped.  Streaming clients (jog)
	// use this when ExecTime goes negative with no stop state active.
	void RearmCoordLaunch() { m_SegmentsStartedExecuting = false; }
	bool CoordLaunched() { return m_SegmentsStartedExecuting; }
	int CheckMotionHalt(bool Coord);
	int ExecutionStop();
	double GetFeedRateOverride();
	double GetFeedRateRapidOverride();
	double GetSpindleRateOverride();
	void SetFeedRateOverride(double v);
	void SetFeedRateRapidOverride(double v);
	void SetHardwareFRORange(double v);
	double GetHardwareFRORange();
	void SetSpindleRateOverride(double v);
	int GetDestination(int axis, double *d);
	int GetPosition(int axis, double *d);
	int GetAxisDone(int axis, int *r);

	CCoordMotion(CKMotionDLL *KMotionDLL = new CKMotionDLL(0));
	virtual ~CCoordMotion();

	void SetAbort();
	void ClearAbort();
	bool GetAbort();

	void SetHalt();
	void ClearHalt();
	bool GetHalt();

	int FlushSegments();
	int WaitForSegmentsFinished(BOOL NoErrorOnDisable = FALSE);
	int WaitForMoveXYZABCFinished();
	int DoKMotionCmd(const char *s, BOOL FlushBeforeUnbufferedOperation);
	int DoKMotionBufCmd(const char *s,int sequence_number=-1);
	MOTION_PARAMS *GetMotionParams();

	int MeasurePointAppendToFile(const wchar_t *name);
	int StraightTraverse(double x, double y, double z, double a, double b, double c, bool NoCallback=false, int sequence_number=-1, int ID=0);
	int StraightTraverse(double x, double y, double z, double a, double b, double c, double u, double v, bool NoCallback=false, int sequence_number=-1, int ID=0);
	
	int ArcFeed(double DesiredFeedRate_in_per_sec, CANON_PLANE plane,
				double first_end, double second_end, 
		        double first_axis, double second_axis, int rotation,
				double axis_end_point, double a, double b, double c, int sequence_number, int ID);
	
	int ArcFeed(double DesiredFeedRate_in_per_sec, CANON_PLANE plane,
				double first_end, double second_end, 
		        double first_axis, double second_axis, int rotation,
				double axis_end_point, double a, double b, double c, double u, double v, int sequence_number, int ID);

	int ArcFeedAccel(double DesiredFeedRate_in_per_sec, double DesiredAccel, CANON_PLANE plane,
				double first_end, double second_end, 
		        double first_axis, double second_axis, int rotation,
				double axis_end_point, double a, double b, double c, int sequence_number, int ID);
	
	int ArcFeedAccel(double DesiredFeedRate_in_per_sec, double DesiredAccel, CANON_PLANE plane,
				double first_end, double second_end, 
		        double first_axis, double second_axis, int rotation,
				double axis_end_point, double a, double b, double c, double u, double v, int sequence_number, int ID);

	int StraightFeed(double DesiredFeedRate_in_per_sec,
				     double x, double y, double z, double a, double b, double c, int sequence_number, int ID);
	
	int StraightFeed(double DesiredFeedRate_in_per_sec,
				     double x, double y, double z, double a, double b, double c, double u, double v, int sequence_number, int ID);

	int StraightFeedAccel(double DesiredFeedRate_in_per_sec, double DesiredAccel,
				     double x, double y, double z, double a, double b, double c, int sequence_number, int ID);
	
	int StraightFeedAccel(double DesiredFeedRate_in_per_sec, double DesiredAccel,
				     double x, double y, double z, double a, double b, double c, double u, double v, int sequence_number, int ID);

	int StraightFeedAccelRapid(double DesiredFeedRate_in_per_sec, double DesiredAccel, bool RapidMode, bool NoCallback,
							   double x, double y, double z, double a, double b, double c, int sequence_number, int ID);

	int StraightFeedAccelRapid(double DesiredFeedRate_in_per_sec, double DesiredAccel, bool RapidMode, bool NoCallback,
							   double x, double y, double z, double a, double b, double c, double u, double v, int sequence_number, int ID);


	int Dwell(double seconds, int sequence_number=0);

	int ReadCurAbsPosition(double *x, double *y, double *z, double *a, double *b, double *c, bool snap=false, bool NoGeo = false);
	int ReadCurAbsPosition(double *x, double *y, double *z, double *a, double *b, double *c, double *u, double *v, bool snap=false, bool NoGeo = false);

	void SetStraightTraverseCallback(STRAIGHT_TRAVERSE_CALLBACK *p);
	void SetStraightTraverseCallback(STRAIGHT_TRAVERSE_SIX_AXIS_CALLBACK *p);
	void SetStraightFeedCallback(STRAIGHT_FEED_CALLBACK *p);
	void SetStraightFeedCallback(STRAIGHT_FEED_CALLBACK_SIX_AXIS *p);
	void SetArcFeedCallback(ARC_FEED_CALLBACK *p);
	void SetArcFeedCallback(ARC_FEED_SIX_AXIS_CALLBACK *p);
	int DownloadDoneSegments();
	int OutputSegment(int iseg);
	int DoSpecialCommand(int iseg);
	int DoSpecialInitialCommands();
	void DoSegmentCallbacks(int i0, int n);
	void DoSegmentCallbacksRapid(int i0, int i1);
	int DoRateAdjustments(int i0, int i1);
	int DoRateAdjustmentsArc(int i, double radius, double theta0, double dtheta, double dcircle);

	// 3rd Order (TP3) feed path: feeds are collected as actuator-space
	// waypoints and planned jerk-limited at each motion boundary
	int TP3AddFeedWaypoint(double x, double y, double z, double a, double b, double c,
		double u, double v, double FeedRateToUse, int sequence_number, int ID, BOOL NoCallback = FALSE);
	int TP3FlushRun(const char *why = NULL);
	void TP3ClearRun();
	void TP3DeleteFeed();   // destructor helper: frees m_TP3Feed (defined
	                        // after the class so the real dtor runs)

	// 3rd Order collinear combining: the legacy CombineSegments rules applied
	// to the interpreter's block endpoints ahead of subdivision and the
	// planner (one chord held open; TP3FlushStager commits it)
	int TP3StageMove(double DesiredFeedRate_in_per_sec, double DesiredAccel, bool RapidMode,
		double x, double y, double z, double a, double b, double c, double u, double v,
		int sequence_number, int ID);
	int TP3FlushStager();

	// buffered controller commands under the 3rd Order planner: held while a
	// streaming run is open and attached to the knot that comes from the
	// waypoint they preceded (see DoKMotionBufCmd)
	int TP3AttachBufCmdNow(const char *s, int sequence_number);   // legacy attach
	int TP3AttachPendingCmds(int upTo);

	int CheckLimit(int axis, double Act, double SoftLimitPos, double SoftLimitNeg, wchar_t Name, CString &errmsg);
	int CheckSoftLimits(double x, double y, double z, double a, double b, double c, double u, double v, CString &errmsg);
	int CheckSoftLimitsArc(int plane, double XC, double YC, double Z0, double Z1,
						   double a, double b, double c, double u, double v, BOOL DirIsCCW, 
						   double radius, double theta0, double dtheta, CString &errmsg);
	
	CKMotionDLL *KMotionDLL;

	double m_TotalDownloadedTime;
	double m_TotalDoTime;  // total accumulated trajectory planner times for simulation
	double m_TotalFeedTime;  // total accumulated Feed Times while running.  Used for Tool Wear
	double m_TotalFeedDist;  // total accumulated Feed Dist while running.  Used for Tool Wear
	int m_nsegs_downloaded;
	double m_TimeAlreadyExecuted;

	int m_realtime_Sequence_number;  // latest sequence number where KFLOP is currently executing
	bool m_realtime_Sequence_number_valid;  // latest sequence number where KFLOP is currently executing is valid


	wchar_t MainPath[MAX_PATH],MainPathRoot[MAX_PATH];

	int m_board_type;

	bool m_Simulate;
	bool m_DoTime;   // do Trajectory Planning Timing during Simulation

	bool m_ThreadingMode;            // Launches coordinated motion in spindle sync mode
	double m_ThreadingBaseSpeedRPS;  // Base Rev/sec speed where trajectory should run an real-time

	bool m_DisableSoftLimits;

	bool m_AxisDisabled;

	bool m_TCP_affects_actuators;

	bool feed_override;	// whether feed override is enabled
	bool speed_override; // whether spindle override is enabled

	int m_Stopping;
	int m_PreviouslyStopped,m_PreviouslyStoppedType,m_PreviouslyStoppedID,m_PreviouslyStoppedSeqNo;

	double m_Stoppedx, m_Stoppedy, m_Stoppedz, m_Stoppeda, m_Stoppedb, m_Stoppedc, m_Stoppedu, m_Stoppedv;
	double m_StoppedMidx, m_StoppedMidy, m_StoppedMidz, m_StoppedMida, m_StoppedMidb, m_StoppedMidc, m_StoppedMidu, m_StoppedMidv;
	double m_StoppedMachinex, m_StoppedMachiney, m_StoppedMachinez, m_StoppedMachinea, m_StoppedMachineb, m_StoppedMachinec, m_StoppedMachineu, m_StoppedMachinev;

	int SetAxisDefinitions(int x, int y, int z, int a, int b, int c);
	int SetAxisDefinitions(int x, int y, int z, int a, int b, int c, int u, int v);
	int GetAxisDefinitions(int *x, int *y, int *z, int *a, int *b, int *c);
	int GetAxisDefinitions(int *x, int *y, int *z, int *a, int *b, int *c, int *u, int *v);
	bool m_DefineCS_valid;   // cached axis numbers need no re-read
	bool m_DefineCS_known;   // axis numbers have been read/defined at
	                         // least once (they are meaningful even when
	                         // the cache is marked for refresh): lets the
	                         // 3rd Order planner exclude CAD axes that are
	                         // not part of the Coordinate System
	int x_axis,y_axis,z_axis,a_axis,b_axis,c_axis,u_axis,v_axis;  // map board channel number to interperter axis 

	double current_x, current_y, current_z, current_a, current_b, current_c, current_u, current_v;

	STRAIGHT_TRAVERSE_CALLBACK *m_StraightTraverseCallback;
	STRAIGHT_TRAVERSE_SIX_AXIS_CALLBACK *m_StraightTraverseSixAxisCallback;
	STRAIGHT_FEED_CALLBACK *m_StraightFeedCallback;
	STRAIGHT_FEED_CALLBACK_SIX_AXIS *m_StraightFeedSixAxisCallback;

	void SetTPParams();

	int GetRapidSettings();
	float MaxDecelTimeForAxis(int axis, double Vel, double Accel, double Jerk);
	float GetNominalFROChangeTime(char* Axis);
	int GetRapidSettingsAxis(int axis,double *Vel,double *Accel,double *Jerk, double *SoftLimitPos, double *SoftLimitNeg, double CountsPerInch, char* Axis);
	bool RapidParamsDirty;

	void SetPreviouslyStoppedAtSeg(SEGMENT *segs_to_check,int i);
	
	double FeedRateDistance(double dx, double dy, double dz, double da, double db, double dc, BOOL *bPureAngle);
	double FeedRateDistance(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, BOOL *bPureAngle);
	int ConfigSpindle(int type, int axis, double UpdateTime, double Tau, double CountsPerRev);
	int GetSpindleRPS(float &speed);

	bool m_TapCycleInProgress;

	bool CheckCollinear(SEGMENT *s0, SEGMENT *s1, SEGMENT *s2, double tol);


private:
	bool m_Abort;
	bool m_Halt;
	double m_FeedRateOverride;
	double m_FeedRateRapidOverride;
	double m_HardwareFRORange;
	double m_SpindleRateOverride;
	ARC_FEED_CALLBACK *m_ArcFeedCallback;
	ARC_FEED_SIX_AXIS_CALLBACK *m_ArcFeedSixAxisCallback;
	bool m_SegmentsStartedExecuting;
	int m_NumLinearNotDrawn;
	CStringA WriteLineBuffer;
	double WriteLineBufferTime;
	int PutWriteLineBuffer(CStringA s, double Time);
	int FlushWriteLineBuffer();
	int ClearWriteLineBuffer();
	int CommitPendingSegments(bool RapidMode);
	int LaunchCoordMotion();
	int UpdateRealTimeState(double T);
	void DetermineSoftwareHardwareFRO(double &HW, double &SW);
};

#endif // !defined(AFX_COORDMOTION_H__36110031_9633_4D82_9C05_E1FDEC3AC8EA__INCLUDED_)
