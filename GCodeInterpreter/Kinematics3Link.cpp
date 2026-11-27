// Kinematics3Link.cpp: implementation of the CKinematics3Link class.
//
// 3 Link parallel arm pen robot driven by 3 rotary servos (Left, Right, Z).
//
// Coordinate System (all fixed geometry in mm):
//
//   The LAYOUT origin (in which all the geometry constants below are
//   expressed) is midway between the Left and Right servo shafts, at the
//   paper surface level.
//
//   The CAD origin (what the DROs and G Code use) is the center of
//   travel: CAD3L_ORIGIN_Y_MM (300mm) from the layout origin in +Y,
//   centered between the motors in X, at the paper surface level.
//
//   +X       - from the Left servo shaft toward the Right servo shaft
//   +Y       - horizontal, from the servos toward the paper (drawing area)
//   +Z       - up
//
//   CAD x,y,z is the pen tip position.  z=0 is the pen tip at the paper,
//   x=0 y=0 is the pen at the center of travel.
//
// Mechanism:
//
//   Left/Right servos rotate 100mm arms in the horizontal plane (38mm above
//   the paper).  The Left distal link (320mm elbow clevis to pen tip) is
//   rigidly connected to the payload so the pen tip is its end point.  The
//   Right distal link (340mm clevis to clevis) connects to the payload
//   ~10mm above the pen tip.  Together they form a five-bar linkage
//   positioning the pen in X,Y.  (The upper links of the double link pairs
//   only stabilize pen tilt and are ignored here.)
//
//   The Z servo shaft is horizontal (along X) so its 100mm arm swings in a
//   vertical Y-Z plane.  A 310mm ball jointed link connects the arm end to
//   the payload ~10mm above the pen tip, raising/lowering the pen.
//
//   Because the arms are above the paper the distal links slope downward;
//   the horizontal projection of their lengths (which shortens slightly as
//   the pen lifts) is accounted for.
//
// Actuator outputs are servo angles in DEGREES scaled by the Counts/Inch
// settings, so set Counts/Inch to counts per degree
// (e.g. Dynamixel XL430: 4096 counts/rev / 360 = 11.37778, negative to
// reverse direction):
//
//   Acts[0] - Right servo angle (Servo ID 0), CCW viewed from above,
//             0 = arm along +X
//   Acts[1] - Left  servo angle (Servo ID 1), CCW viewed from above,
//             0 = arm along +X
//   Acts[2] - Z     servo angle (Servo ID 2), CCW viewed from +X (right
//             side), 0 = arm horizontal toward the paper (+Y), 90 = straight up
//
// The OFFSET_x_DEG constants align the servo count zeros with the geometric
// angle conventions above.  To calibrate: with the machine at any pose read
// the servo counts C and estimate the geometric arm angle Theta, then
// OFFSET = C/CountsPerDegree - Theta.
//
// A,B,C pass through as simple linear axes.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Kinematics3Link.h"

#define sqr(x) ((x)*(x))

// CAD units to mm.  CAD space in inches by default, set to 1.0 for mm.
#define CAD3L_MM_PER_UNIT 25.4

// CAD origin: this far in +Y (layout mm) from the midpoint between the
// Left/Right servo shafts, i.e. the center of travel of the drawing area
#define CAD3L_ORIGIN_Y_MM 300.0

#define ARM_LEN       100.0		// all three servo arms, hole center to hole center
#define SHAFT_SEP     100.0		// Left/Right servo shaft separation in X
#define ARM_HEIGHT_LR  38.0		// Left/Right arm height above the paper

#define DIST_LEN_L    320.0		// Left distal link, elbow clevis to pen tip
#define DIST_LEN_R    340.0		// Right distal link, clevis to clevis
#define DIST_LEN_Z    310.0		// Z distal link, ball joint to ball joint

#define PAYLOAD_R_UP   10.0		// Right link payload connection above pen tip
#define PAYLOAD_Z_UP   10.0		// Z link payload connection above pen tip

// Z servo shaft position (33mm from the Left shaft in X, 8mm forward of the
// Left/Right shafts in Y, 77mm above the paper)
#define ZSHAFT_X  (-SHAFT_SEP/2.0 + 33.0)
#define ZSHAFT_Y    8.0
#define ZSHAFT_Z   77.0

// Elbow solution branch for each linkage as assembled (+1 or -1).
// Defaults: Left/Right elbows outboard, Z arm up.  Flip if mirrored.
#define ELBOW_L  (+1.0)
#define ELBOW_R  (-1.0)
#define ELBOW_Z  (+1.0)

// Servo angle offsets in degrees added to the geometric angles above,
// for aligning actuator zero with however the servos are zeroed
#define OFFSET_L_DEG  0.0
#define OFFSET_R_DEG  0.0
#define OFFSET_Z_DEG  0.0

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKinematics3Link::CKinematics3Link()
{
	m_MotionParams.MaxLinearLength = 2.0/CAD3L_MM_PER_UNIT;  // limit the segment lengths for nonlinear systems
	m_MotionParams.MaxAngularChange = 0.5;  // limit the segment angle change for nonlinear systems
	m_MotionParams.MaxRapidFRO = 1.0;       // limit the increase in Rapid HW FRO
	m_MotionParams.UseOnlyLinearSegments=true;
	m_MotionParams.DoRapidsAsFeeds=true;

	// cold guess for the base iterative inverse: near the center of
	// travel (the CAD origin), pen slightly above the paper
	m_InvGuess[0] = 0.0;
	m_InvGuess[1] = (280.0 - CAD3L_ORIGIN_Y_MM)/CAD3L_MM_PER_UNIT;
	m_InvGuess[2] = 5.0/CAD3L_MM_PER_UNIT;
}

CKinematics3Link::~CKinematics3Link()
{

}

// keep acos argument valid, clamping at full extension/fold when out of reach

double CKinematics3Link::ClampCos(double x)
{
	if (x > 1.0) return 1.0;
	if (x < -1.0) return -1.0;
	return x;
}

// Standard planar 2 link inverse kinematics.  Base joint at the origin,
// target at (dx,dy), links L1 and L2, elbow = +1/-1 selects the solution.
// Returns the L1 (servo arm) angle in radians CCW from +X.

double CKinematics3Link::PlanarTwoLink(double dx, double dy, double L1, double L2, double elbow)
{
	double D2 = sqr(dx) + sqr(dy);
	double D = sqrt(D2);

	if (D == 0.0) return 0.0;

	return atan2(dy,dx) + elbow * acos(ClampCos((sqr(L1) + D2 - sqr(L2)) / (2.0*L1*D)));
}

int CKinematics3Link::TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts, bool NoGeo)
{
	if (!NoGeo) GeoCorrect(x,y,z,&x,&y,&z);

	double xm = x*CAD3L_MM_PER_UNIT;
	double ym = y*CAD3L_MM_PER_UNIT + CAD3L_ORIGIN_Y_MM;   // CAD origin -> layout frame
	double zm = z*CAD3L_MM_PER_UNIT;

	// Left linkage: distal ends at the pen tip.  Use the horizontal
	// projection of the sloping distal link as the planar link length

	double dzL = ARM_HEIGHT_LR - zm;
	double rL2 = sqr(DIST_LEN_L) - sqr(dzL);
	if (rL2 < 100.0) rL2 = 100.0;   // keep finite far out of range
	double ThetaL = PlanarTwoLink(xm + SHAFT_SEP/2.0, ym, ARM_LEN, sqrt(rL2), ELBOW_L);

	// Right linkage: distal connects to the payload PAYLOAD_R_UP above the
	// pen tip (directly above it in the horizontal plane)

	double dzR = ARM_HEIGHT_LR - (zm + PAYLOAD_R_UP);
	double rR2 = sqr(DIST_LEN_R) - sqr(dzR);
	if (rR2 < 100.0) rR2 = 100.0;
	double ThetaR = PlanarTwoLink(xm - SHAFT_SEP/2.0, ym, ARM_LEN, sqrt(rR2), ELBOW_R);

	// Z linkage: arm swings in the Y-Z plane, ball link to the payload
	// connection PAYLOAD_Z_UP above the pen tip.  Solve
	// |arm end - connection| = DIST_LEN_Z for the arm angle

	double dx = xm - ZSHAFT_X;
	double dy = ym - ZSHAFT_Y;
	double dz = (zm + PAYLOAD_Z_UP) - ZSHAFT_Z;

	double C = (sqr(ARM_LEN) + sqr(dx) + sqr(dy) + sqr(dz) - sqr(DIST_LEN_Z)) / (2.0*ARM_LEN);
	double Dyz = sqrt(sqr(dy) + sqr(dz));
	double PhiZ;

	if (Dyz == 0.0)
		PhiZ = 0.0;
	else
		PhiZ = atan2(dz,dy) + ELBOW_Z * acos(ClampCos(C/Dyz));

	// Servo ID 0 = Right, 1 = Left, 2 = Z

	Acts[0] = (ThetaR*180.0/PI + OFFSET_R_DEG) * m_MotionParams.CountsPerInchX;
	Acts[1] = (ThetaL*180.0/PI + OFFSET_L_DEG) * m_MotionParams.CountsPerInchY;
	Acts[2] = (PhiZ  *180.0/PI + OFFSET_Z_DEG) * m_MotionParams.CountsPerInchZ;

	Acts[3] = a * m_MotionParams.CountsPerInchA;
	Acts[4] = b * m_MotionParams.CountsPerInchB;
	Acts[5] = c * m_MotionParams.CountsPerInchC;

	return 0;
}


// The forward transform CLAMPS its acos arguments and the projected link
// lengths so the numeric inverter can iterate through out-of-reach poses
// without blowing up.  A consequence: the clamped forward has PHANTOM
// solutions far outside the real workspace that produce the same servo
// angles as an in-reach pose, and the inverter can legitimately converge
// onto one (seen while drawing knots as the pose passes near the
// five-bar fold - lines shooting out to huge XY values).  Verify a
// solution is in the linkage's REAL domain: every clamp comfortably
// disengaged.  Margins: projected distal length > ~10.5mm (clamp is
// 10mm), triangle-closure cosines within +/-0.999 (within ~2.6 degrees
// of full extension/fold is rejected), planar distances > 1mm.

bool CKinematics3Link::PoseInReach(double x, double y, double z, bool NoGeo)
{
	// evaluate reach at the pose the machine PHYSICALLY goes to
	if (!NoGeo) GeoCorrect(x,y,z,&x,&y,&z);

	double xm = x*CAD3L_MM_PER_UNIT;
	double ym = y*CAD3L_MM_PER_UNIT + CAD3L_ORIGIN_Y_MM;
	double zm = z*CAD3L_MM_PER_UNIT;

	double dzL = ARM_HEIGHT_LR - zm;
	double rL2 = sqr(DIST_LEN_L) - sqr(dzL);
	double DL2 = sqr(xm + SHAFT_SEP/2.0) + sqr(ym);
	if (rL2 < 110.0 || DL2 <= 1.0) return false;
	if (fabs((sqr(ARM_LEN) + DL2 - rL2) / (2.0*ARM_LEN*sqrt(DL2))) > 0.999) return false;

	double dzR = ARM_HEIGHT_LR - (zm + PAYLOAD_R_UP);
	double rR2 = sqr(DIST_LEN_R) - sqr(dzR);
	double DR2 = sqr(xm - SHAFT_SEP/2.0) + sqr(ym);
	if (rR2 < 110.0 || DR2 <= 1.0) return false;
	if (fabs((sqr(ARM_LEN) + DR2 - rR2) / (2.0*ARM_LEN*sqrt(DR2))) > 0.999) return false;

	double dx = xm - ZSHAFT_X;
	double dy = ym - ZSHAFT_Y;
	double dz = (zm + PAYLOAD_Z_UP) - ZSHAFT_Z;
	double C = (sqr(ARM_LEN) + sqr(dx) + sqr(dy) + sqr(dz) - sqr(DIST_LEN_Z)) / (2.0*ARM_LEN);
	double Dyz = sqrt(sqr(dy) + sqr(dz));
	if (Dyz <= 1.0 || fabs(C/Dyz) > 0.999) return false;

	// physical pen height range.  The Z linkage is double valued: the same
	// servo angle also closes with the link folded UP over the arm - a real
	// solution of the model that the physical pen never reaches.  With the
	// 77mm Z shaft that folded root sits ~325mm high at the center of
	// travel, ranging roughly 108..334mm over the workspace, so this test
	// is a solid backstop near the center but NOT out at the far corners
	// where the two roots approach each other.  Reject it so inversions
	// stay on the low branch.
	if (zm < -25.0 || zm > 150.0) return false;

	return true;
}

// perform Inversion to go the other way

int CKinematics3Link::TransformActuatorstoCAD(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo)
{
	// hold the inversion lock across solve + verify + retry so another
	// thread's solve cannot perturb the warm state mid-sequence (the
	// critical section is recursive - the inner locks nest)
	InvLock();

	int r = InvertTransformCADtoActuators(Acts, xr, yr, zr, ar, br, cr, NoGeo);

	if (!r && !PoseInReach(*xr, *yr, *zr, NoGeo))
	{
		// the fast pass converged onto the wrong (folded-up) solution
		// branch: redo carefully from the cold guess - the small-step
		// walk stays in the basin of the intended nearby solution.
		// (skipped in probe mode - the caller wants cheap failure)
		m_InvWarmValid = false;
		if (m_InvBudgetIters > 0)
			r = 1;
		else
		{
			r = InvertTransformCADtoActuatorsSlow(Acts, xr, yr, zr, ar, br, cr, NoGeo);
			if (r || !PoseInReach(*xr, *yr, *zr, NoGeo))
			{
				m_InvWarmValid = false;
				r = 1;
			}
		}
	}

	InvUnlock();
	return r;
}
