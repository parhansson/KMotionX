// TrajectoryPlanner3.h
//
// 3rd-order (jerk-limited) multi-axis FIR trajectory planner — library
// interface. Extracted from the TP3 development project
// (trajectory_planner_fir.cpp rev 6c); see TrajectoryPlanner3_USAGE.md
// for the streaming API contract and TrajectoryPlanner3_ANALYSIS.md for
// the integration plan into the existing 2nd-order planner pipeline.
//
// Generates trajectories through waypoints in up to 8 axes (X Y Z A B C
// U V) with velocity, acceleration and JERK bounded independently per
// axis at all times, bounded XYZ deviation from the programmed path,
// per-segment programmed feedrate (XYZ path speed; INFINITY = rapid),
// per-segment tolerance, and per-waypoint integer seq/ID passed through
// to every output sample (attributed at the filter group delay) for
// halt / rewind / resume bookkeeping.
//
// Core method: time-parameterize the raw polyline, sample it, pass every
// axis through the SAME two cascaded moving-average FIR filters
// (T1 = 2*R, T2 = R, R = max_i Aeff_i/Jmax_i with the effective
// acceleration Aeff_i = min(Amax_i, sqrt(Vmax_i*Jmax_i/2)), see
// effectiveAmax below). Discrete corners get
// per-axis kink speed caps and constant-speed plateaus; dense facets are
// handled by per-axis angle-density (curvature) caps; tolerance is
// enforced through apex-miss caps, curvature undercut caps, and a
// two-pass exact-error lateral pre-compensation.
//
// Axis semantics (G-code style): XYZ define the path geometry and the
// feedrate F (units/sec of XYZ path speed). ABCUV interpolate linearly
// with progress along each segment. Any axis may limit the achieved
// speed: on a segment with (8D) unit tangent u the axis speed is
// v*|u_i|, so v <= Vmax_i/|u_i| per axis and v <= F/|u_xyz| for finite F.
// Pure rotary moves (no XYZ motion) are parameterized by their own length.

#pragma once

#include <cmath>
#include <vector>
#include <functional>

namespace TP3 {

static const int NAX = 8;                    // X Y Z A B C U V
extern const char AXNAME[NAX + 1];

struct VecN {
    double c[NAX] = {0, 0, 0, 0, 0, 0, 0, 0};
    double&       operator[](int i)       { return c[i]; }
    const double& operator[](int i) const { return c[i]; }
    VecN operator+(const VecN& b) const { VecN r; for (int i = 0; i < NAX; ++i) r.c[i] = c[i] + b.c[i]; return r; }
    VecN operator-(const VecN& b) const { VecN r; for (int i = 0; i < NAX; ++i) r.c[i] = c[i] - b.c[i]; return r; }
    VecN operator*(double s)      const { VecN r; for (int i = 0; i < NAX; ++i) r.c[i] = c[i] * s; return r; }
    double dot(const VecN& b) const { double s = 0; for (int i = 0; i < NAX; ++i) s += c[i] * b.c[i]; return s; }
    double norm()    const { return std::sqrt(dot(*this)); }
    double xyzNorm() const { return std::sqrt(c[0]*c[0] + c[1]*c[1] + c[2]*c[2]); }
    VecN normalized() const { double n = norm(); return n > 0 ? (*this) * (1.0 / n) : VecN{}; }
};

struct AxisLimits {                          // independent per-axis limits
    double vmax[NAX], amax[NAX], jmax[NAX];
};

// ---- effective per-axis acceleration ----------------------------------
// The two boxcar filters (T1 = 2R, T2 = R, R = A/J) are sized so that a raw
// profile flipping from +A to -A filters to a jerk of exactly J.  A velocity
// step dV through them peaks at dV/T1 of acceleration and dV/(T1*T2) of
// jerk, so the accel limit is only reached by steps dV >= 2*A^2/J.  An axis
// whose Vmax is below that can never exercise its Amax: the setting merely
// widens the windows (Tw = 3A/J), slowing every ramp and corner and
// stretching the tolerance-limited corner speed 8*tol/(dD*Tw).  The planner
// therefore sizes its windows from the largest acceleration a rest-to-Vmax
// step can actually use,
//     Aeff = min(Amax, sqrt(Vmax*Jmax/2))        (2*Aeff^2/Jmax = Vmax)
// R = max over active axes of Aeff/Jmax (T1 = 2R, T2 = R, Tw = 3R), and
// caps each axis's raw acceleration at min(Amax, Jmax*R) - the value at
// which a +-a reversal filters to exactly Jmax through THAT window.  On the
// axis that sets R this is its Aeff (reducing the raw acceleration as well
// as the windows is what keeps the reversal jerk at Jmax); on every other
// axis it is the configured Amax, unchanged.  The capped limits are used
// EVERYWHERE - raw speed profile, kink / curvature caps, comp clamp.  Axes
// with Vmax >= 2A^2/J never change anything.  Inactive slots (any
// non-positive limit) pass through.
double effectiveAmax(double vmax, double amax, double jmax);   // per-axis Aeff
double filterRatio(const AxisLimits& ax);      // R; 0 when no axis is active.
                                               // Hosts take Tw = 3R from here
                                               // so it cannot drift from the
                                               // planner's own windows.
AxisLimits effectiveLimits(const AxisLimits& ax);   // amax -> min(Amax, Jmax*R)

struct Limits {
    AxisLimits ax;
    double dt;                               // output sample period
    double dsFixed = 0;                      // arc grid step; 0 = auto (S/4000).
                                             // REQUIRED (>0) for streaming.
};

// A waypoint carries the feedrate/tolerance of the segment ARRIVING at it
// (G-code convention), plus integer seq (e.g. G-code line number) and id
// (e.g. stage within the line) that are passed through to output samples.
//
// wgt[] are per-axis TIP-ERROR WEIGHTS: units of tool-tip error per unit
// of axis-i motion (for rotary actuators in degrees, this is the local
// pivot radius in length/degree - a column norm of the inverse
// kinematics Jacobian).  Tolerance-denominated caps measure deviations
// in the weighted norm |dD_i*wgt_i| so one scalar tol bounds true
// tool-tip error; velocity/accel/jerk limits stay in NATIVE axis units.
// Default 1.0 reproduces the unweighted (1 unit = 1 unit) behavior.
struct Waypoint {
    VecN   p;
    double F   = INFINITY;
    double tol = 1.0;
    int    seq = 0;
    int    id  = 0;
    bool   rapid = false;   // G0 attribution: rides with seq/id.  Kept
                            // SEPARATE from F because fairing clamps F on
                            // blend waypoints (kink-cap corner speed), which
                            // must not change the rapid/feed attribution
    VecN   cad;             // CAD position of the source waypoint: rides
                            // with seq/id so output samples carry a CAD
                            // ANCHOR - the inverse-kinematics seed that
                            // guarantees drawing inversions start in the
                            // right solution branch
    bool   nocb = false;    // no-callback attribution (jogs stream through
                            // the planner but must not appear in the G-view)
    VecN   wgt{{1,1,1,1,1,1,1,1}};
    int    cmd = 0;         // host's count of buffered controller commands
                            // issued BEFORE this waypoint: rides with seq/id
                            // (attributed at the group delay) so the host can
                            // attach each command to the output that comes
                            // from the motion the command preceded
};

// F is the programmed feedrate of the source segment (INFINITY = no feed
// constraint - per-axis caps only), rapid is the G0 attribution, cad the
// source waypoint's CAD position (inverse-kinematics seed); all
// attributed at the filter group delay like seq/id.
struct TrajPoint { double t; VecN p; int seq = 0, id = 0; double F = 0; bool rapid = false; bool nocb = false; VecN cad; int cmd = 0; };

// ----------------------------------------------------------------------
struct Polyline {
    std::vector<VecN>   pt;
    std::vector<double> cum;                 // 8D arc length at each vertex
    std::vector<double> segF, segTol;        // per segment [i] = pt[i]..pt[i+1]
    std::vector<VecN>   segWgt;              // per-axis tip-error weights
    std::vector<int>    segSeq, segId;
    std::vector<char>   segRapid;            // G0 attribution per segment
    std::vector<char>   segNocb;             // no-callback (jog) per segment
    std::vector<VecN>   segCad;              // CAD anchor per segment
    std::vector<int>    segCmd;              // buffered-command mark per segment
    std::vector<double> segVcap, segAcap;    // per-axis + feedrate caps mapped
    double S = 0;                            //   onto the tangential direction

    void build(const std::vector<Waypoint>& in, const AxisLimits& ax);
    int  segIndex(double s) const;
    VecN pos(double s) const;
};

// Tolerance-aware corner fairing (G64 style). Each interior vertex is
// replaced by a quadratic Bezier blend whose apex offset spends at most
// fairFrac of the LOCAL tolerance; blend points carry tol minus the apex
// offset ACTUALLY spent (a blend clamped by short neighbors spends less
// than its budget) so total deviation from the ORIGINAL path stays within
// the original tolerance. Streaming note: fairing vertex i needs waypoint
// i+1, so a streaming caller fairs with one waypoint of lookahead before
// addWaypoint().
std::vector<Waypoint> fairPath(const std::vector<Waypoint>& in,
                               double fairFrac, double sampleStep);

// ======================================================================
// PathPlan: speed plan over one polyline, sampleable at arbitrary time
// offsets (needed for splice-aligned streaming replans).
// ======================================================================
class PathPlan {
public:
    bool ok = false;
    double T1 = 0, T2 = 0, Tw = 0, Cf = 0, Lf = 0;
    double ds = 0, off = 0;         // grid step; grid start (absolute align)
    double T0 = 0, Sloc = 0;        // total plan time; local path length
    double Vbig = 0, Vpk = 0;       // max axis vmax; max planned speed
    int N = 0, N1 = 2, N2 = 2, dly = 0;

    // grid arc position: absolute-aligned interior, exact endpoints
    double gs(int i) const { return (i >= N - 1) ? Sloc : off + i * ds; }

    void compute(const Polyline& path, const Limits& lim, double s0abs);
    double timeAtArc(double sLocal) const;   // inverse of s(t)
    double sAtTime(double t) const;          // forward map (fresh search)

    struct Raw {
        std::vector<std::vector<double>> ch; // [NAX][count] comp'd raw
        std::vector<double> s;               // local arc per tick
        std::vector<int>    seg;             // polyline segment per tick
    };
    // Sample comp'd raw channels at plan times tFirst + m*dt (clamped to
    // [0,T0] with end-hold). Compensation is computed on this substream;
    // callers must include enough context ticks that comp edge effects
    // do not reach the ticks they intend to keep.
    Raw sampleCompRaw(double tFirst, int count) const;

private:
    const Polyline* pp = nullptr;
    Limits lm;
    std::vector<double> vlim, ts;
    struct Vert { double s; VecN dD; double dDn, dDw, vc, halfSpan, tol; };
    std::vector<Vert> verts;
    double sOfT(double t, int& j) const;     // forward map with cell hint
};

// One-shot planning: full path, rest-to-rest, edge-hold filtered.
std::vector<TrajPoint> planTrajectory(const Polyline& path, const Limits& lim);

// ======================================================================
// StreamingTrajectoryPlanner: infinite waypoint stream, bounded memory.
// ======================================================================
// The canonical object is the comp'd raw sample stream on the program
// clock. Each replan appends only new ticks (spliced at the last written
// one); the final FIR cascade is evaluated incrementally over the
// persisted stream. Path behind the finalization frontier (minus context)
// is retired; raw history behind the filter window is trimmed.
//
class StreamingTrajectoryPlanner {
public:
    StreamingTrajectoryPlanner(const Limits& lim, double targetBufferTime,
                               std::function<double()> pollFn);

    // Margin-aware pacing: the emission frontier withholds lastMargin_ of
    // planned time behind the plan's (temporary) end-of-plan deceleration,
    // so the feeder must be allowed to buffer targetBuffer_ BEYOND that
    // margin - pacing on targetBuffer_ alone caps the whole plan at less
    // than the withheld amount and the committed (downloadable) cushion
    // runs structurally near zero on any long uninterrupted stroke.
    // lastMargin_ is 0 until the first replan, which only makes startup
    // stricter, never looser, than the old rule.
    bool canAddWaypoint() { return getBufferedTime() < targetBuffer_ + lastMargin_; }

    // Blocks (sleeps) while the controller-side buffer is at/above target.
    void addWaypoint(const Waypoint& w);

    // As addWaypoint() but never blocks on the buffered-time throttle;
    // for callers that manage (and may need to override) the wait
    // themselves, e.g. to detect a controller that stopped consuming.
    void addWaypointNoWait(const Waypoint& w);

    // Replan the kept path; emit finalized output samples (immutable).
    std::vector<TrajPoint> update();

    // The finalization margin computed at the last streaming replan:
    // buffered plan time must EXCEED this before anything is emitted.
    // 0 until the first replan.  Pacing callers (coordinated jog) size
    // their stream allowance from this - an a-priori estimate cannot
    // capture the end-fade TIME term, which grows without bound as the
    // path speed drops (kinematic crawl near a fold, slow jogs).
    double emissionMargin() const { return lastMargin_; }
    // No more waypoints: emit everything, including the filter tail.
    std::vector<TrajPoint> flush();

    double getTotalTimeWritten() const {
        return rawCount_ > 0 ? (rawCount_ - 1) * lim_.dt : 0.0;
    }
    double getControllerExecutionTime() const { return poll_(); }
    double getBufferedTime() const { return getTotalTimeWritten() - poll_(); }
    size_t getKeptWaypointCount() const { return kept_.size(); }
    size_t getTotalWaypointCount() const { return totalWaypoints_; }
    double getRetiredArc() const { return sRetired_; }
    size_t getHistoryTicks() const { return Rs_.size(); }
    double getWorstSeamError() const { return seamWorst_; }

private:
    Limits lim_;
    double targetBuffer_;
    std::function<double()> poll_;
    std::vector<Waypoint> kept_;
    double sRetired_ = 0.0;                 // absolute arc of kept_[0]
    size_t totalWaypoints_ = 0;
    bool flushed_ = false;
    double T1_, T2_, Tw_, decelMax_, ctxTime_;
    int N1_, N2_, dly_, histTicks_;
    long long keepRawTicks_;
    // persisted comp'd raw stream (program ticks [rawBase_, rawCount_))
    long long rawBase_ = 0, rawCount_ = 0;
    std::vector<std::vector<double>> R_;
    std::vector<double> Rs_;                // absolute arc per tick
    std::vector<int> Rseq_, Rid_;
    std::vector<double> Rf_;                // programmed F per tick
    std::vector<char> Rrapid_;              // G0 attribution per tick
    std::vector<char> Rnocb_;               // no-callback (jog) per tick
    std::vector<VecN> Rcad_;                // CAD anchor per tick
    std::vector<int> Rcmd_;                 // buffered-command mark per tick
    double seamWorst_ = 0.0, seamWarn_ = 1e-6;
    double lastMargin_ = 0.0;               // see emissionMargin()

    double Rval(long long k, int a) const {
        if (k < rawBase_) k = rawBase_;     // program-start edge hold
        return R_[a][(size_t)(k - rawBase_)];
    }

    std::vector<TrajPoint> emitCommon(bool toEnd);
    // Final FIR cascade over the persisted raw stream for out ticks [k0,k1].
    std::vector<TrajPoint> filterEmit(long long k0, long long k1);
};

} // namespace TP3
