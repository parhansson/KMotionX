// Kinematics.h: interface for the CKinematics class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KINEMATICS_H__F0E3BA96_734F_4D32_85DD_8B2FA813C991__INCLUDED_)
#define AFX_KINEMATICS_H__F0E3BA96_734F_4D32_85DD_8B2FA813C991__INCLUDED_

#ifndef _KMOTIONX
#define GCODEINTERPRETER_DLL
#else
#include <mutex>
#endif


// Now we use the generic helper definitions above to define GCODEINTERPRETER_API and GCODEINTERPRETER_LOCAL.
// GCODEINTERPRETER_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// GCODEINTERPRETER_LOCAL is used for non-api symbols.

#ifdef GCODEINTERPRETER_DLL // defined if KMOTIONDLL is compiled as a DLL
  #ifdef GCODEINTERPRETER_EXPORTS // defined if we are building the KMOTIONDLL DLL (instead of using it)
    #define GCODEINTERPRETER_API DLL_HELPER_API_EXPORT
  #else
    #define GCODEINTERPRETER_API DLL_HELPER_API_IMPORT
  #endif // GCODEINTERPRETER_DLL_EXPORTS
  #define GCODEINTERPRETER_LOCAL DLL_HELPER_API_LOCAL
#else // GCODEINTERPRETER_DLL is not defined: this means KMOTIONDLL is a static lib.
  #define GCODEINTERPRETER_API
  #define GCODEINTERPRETER_LOCAL
#endif // GCODEINTERPRETER_DLL


#include "PT2D.h"
#include "PT3D.h"



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_ACTUATORS 8




class GCODEINTERPRETER_API CKinematics  
{
public:
	int Solve(double *A, int N);
	int GetParameter(const wchar_t *, double * v);
	void removeChar(wchar_t * s, int c);
	int MaxAccelInDirection(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *accel);
	int MaxRateInDirection(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *rate);
	int MaxRateInDirection(double dx, double dy, double dz, double da, double db, double dc, double *rate);

	// Point-aware variants: max CAD path rate/accel for motion in direction
	// (dx..dv) evaluated AT the CAD point (px..pv).  With
	// MOTION_PARAMS.ActuatorLimits off these reduce exactly to the legacy
	// CAD-proportion methods above; with it on they probe
	// TransformCADtoActuators around the point (position dependent for
	// nonlinear Kinematics) so every actuator stays within its
	// GetEffectiveActuatorLimits() Vel/Accel while moving along the path.
	int MaxRateInDirectionAtPoint(double px, double py, double pz, double pa, double pb, double pc, double pu, double pv,
		double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *rate);
	int MaxAccelInDirectionAtPoint(double px, double py, double pz, double pa, double pb, double pc, double pu, double pv,
		double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *accel);
	int ActuatorRatiosInDirection(double px, double py, double pz, double pa, double pb, double pc, double pu, double pv,
		double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *ratio);
	int MaxRapidRateInDirection(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *rate);
	int MaxRapidJerkInDirection(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *jerk);
	int MaxRapidAccelInDirection(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *accel);
	virtual int TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double u, double v, double *Acts, bool NoGeo = false);
	virtual int TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts, bool NoGeo = false);
	virtual int TransformActuatorstoCAD(double *Acts, double *x, double *y, double *z, double *a, double *b, double *c, bool NoGeo = false);
	virtual int TransformActuatorstoCAD(double *Acts, double *x, double *y, double *z, double *a, double *b, double *c, double *u, double *v, bool NoGeo = false);
	virtual int ComputeAnglesOption(int is);
	virtual int Initialize();
	int InvertTransformCADtoActuators(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo = false);
	int InvertTransformCADtoActuatorsSlow(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo = false);
	int InvertWorker(double *Acts, bool NoGeo, bool adaptive, bool useWarm, int maxIter, double *vout);
	virtual int RemapForNonStandardAxes(double *x, double *y, double *z, double *a, double *b, double *c);

	int IntersectionTwoCircles(CPT2D c0, double r0, CPT2D c1, double r1, CPT2D *q);

	virtual int ReadGeoTable(const wchar_t *name);
	int ReadLinearTable(int i, const wchar_t* name, bool* valid);
	virtual int GeoCorrect(double x, double y, double z, double *cx, double *cy, double *cz);
	virtual int LinearCorrect(double x, double* cx, int ia);
	virtual int GetSoftLimits(double *xm, double *xp, double *ym, double *yp, double *zm, double *zp,
		double *am, double *ap, double *bm, double *bp, double *cm, double *cp, double *um, double *up, double *vm, double *vp) {
		return 0;
	}
	
	double MaxCorrPerIteration; // INITIAL trust radius of the iterative
	                            // invert transform (adapts from there;
	                            // settable in Kinematics.txt)

	// iterative-inverse state: cold guess (set by derived constructors
	// for machines whose origin is unreachable or singular) and the
	// warm-start cache of the last converged solution
	double m_InvGuess[6];
	bool m_InvWarmValid;
	double m_InvWarm[6];

	// 0 (normal): full solve - fast pass then the careful fallback walk.
	// >0 (probe): the fast pass gets this iteration budget and the
	// fallback (and any derived-class wrong-branch retry) is skipped.
	// Set temporarily by callers that must stay cheap on unsolvable
	// poses (G-view drawing of an out-of-reach program)
	int m_InvBudgetIters;

	// serializes the iterative-inverse state (m_InvWarm/m_InvWarmValid/
	// m_InvBudgetIters) between threads: DROs invert current positions on
	// the GUI thread while G-view drawing inverts planned knots on the
	// execution thread.  The critical section is recursive, so callers
	// composing multi-step sequences (seed the warm start + set a probe
	// budget + solve) hold InvLock() around the sequence and the locks
	// inside the solve nest harmlessly.
#ifdef _KMOTIONX
	std::recursive_mutex m_InvCS;  // like a CRITICAL_SECTION it can be locked again by the owning thread
	void InvLock()   { m_InvCS.lock(); }
	void InvUnlock() { m_InvCS.unlock(); }
#else
	CRITICAL_SECTION m_InvCS;
	void InvLock()   { EnterCriticalSection(&m_InvCS); }
	void InvUnlock() { LeaveCriticalSection(&m_InvCS); }
#endif

	CKinematics();
	virtual ~CKinematics();

	MOTION_PARAMS m_MotionParams;

	#define NGCODE_AXES 8


	bool GeoTableValid, LinearTableValid[NGCODE_AXES], AnyLinearTableValid;
	CPT3D *GeoTable;
	double *LinearTables[NGCODE_AXES];
	CString *Table2;
	int NRows, NCols;
	double GeoSpacingX, GeoSpacingY;
	double GeoOffsetX, GeoOffsetY;  // Machine coordinates of grid point row=0 col=0
	
	// Linear Tables
	int NLinear[NGCODE_AXES];
	double LinearSpacings[NGCODE_AXES];
	double LinearOffset[NGCODE_AXES];  // Machine coordinates of table point i=0

	const wchar_t *MainPath;
};

#endif // !defined(AFX_KINEMATICS_H__F0E3BA96_734F_4D32_85DD_8B2FA813C991__INCLUDED_)
