// TrajectoryPlanner3.cpp
//
// 3rd-order (jerk-limited) multi-axis FIR trajectory planner —
// implementation. Extracted unchanged from the TP3 development project
// (trajectory_planner_fir.cpp rev 6c); see TrajectoryPlanner3.h for the
// interface documentation and TrajectoryPlanner3_USAGE.md for usage.

#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS
#include "TrajectoryPlanner3.h"

#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <stdexcept>
#include <thread>
#include <chrono>

namespace TP3 {

const char AXNAME[NAX + 1] = "XYZABCUV";

// ----------------------------------------------------------------------
// tip-referenced deviation norm: per-axis deviation scaled by the
// tip-error weights (so tolerances measure true tool-tip error)
static double wnorm(const VecN& d, const VecN& w)
{
    double s = 0;
    for (int i = 0; i < NAX; ++i) { double e = d[i] * w[i]; s += e * e; }
    return std::sqrt(s);
}

// elementwise max of two weight vectors (conservative at a junction)
static VecN wmax(const VecN& a, const VecN& b)
{
    VecN r;
    for (int i = 0; i < NAX; ++i) r[i] = std::max(a[i], b[i]);
    return r;
}

// ----------------------------------------------------------------------
// effective acceleration (rationale in the header): the largest
// acceleration a rest-to-Vmax step can exercise through the FIR windows
double effectiveAmax(double vmax, double amax, double jmax)
{
    if (vmax <= 0 || amax <= 0 || jmax <= 0) return amax;   // inactive slot
    double e = std::sqrt(0.5 * vmax * jmax);
    if (!(e > 0)) return amax;      // underflow / NaN: never turn an active
                                    // slot into a zero-accel one
    return std::min(amax, e);
}

double filterRatio(const AxisLimits& in)
{
    double Rr = 0;
    for (int a = 0; a < NAX; ++a) {
        if (in.vmax[a] <= 0 || in.amax[a] <= 0 || in.jmax[a] <= 0) continue;
        Rr = std::max(Rr, effectiveAmax(in.vmax[a], in.amax[a], in.jmax[a])
                          / in.jmax[a]);
    }
    return Rr;
}

// Raw accel cap per axis, consistent with the MACHINE window R (the widest
// per-axis Aeff/J): the only physical requirement on an axis's raw
// acceleration is the reversal jerk 2a/T1 = a/R <= J, i.e. a <= J*R.  On
// the axis that sets R this is exactly its Aeff; on every other axis J*R
// >= Aeff so the cap stays at the configured Amax (as before the change) -
// tightening it there would cost time and buy no accel or jerk margin.
AxisLimits effectiveLimits(const AxisLimits& in)
{
    AxisLimits e = in;
    double R = filterRatio(in);
    for (int a = 0; a < NAX; ++a) {
        if (in.vmax[a] <= 0 || in.amax[a] <= 0 || in.jmax[a] <= 0) continue;
        e.amax[a] = std::min(in.amax[a], in.jmax[a] * R);
    }
    return e;
}

// Per-segment caps use the EFFECTIVE limits so the raw speed profile ramps
// at Aeff, matching the filter windows (idempotent: either form may be
// passed in).
void Polyline::build(const std::vector<Waypoint>& in, const AxisLimits& axIn)
{
    const AxisLimits ax = effectiveLimits(axIn);
    pt.clear(); segF.clear(); segTol.clear(); segWgt.clear();
    segSeq.clear(); segId.clear(); segRapid.clear(); segNocb.clear(); segCad.clear();
    segCmd.clear();
    for (const Waypoint& w : in) {
        if (!pt.empty() && (w.p - pt.back()).norm() <= 1e-12) continue;
        if (!pt.empty()) {
            segF.push_back(w.F); segTol.push_back(w.tol);
            segWgt.push_back(w.wgt);
            segSeq.push_back(w.seq); segId.push_back(w.id);
            segRapid.push_back(w.rapid ? 1 : 0);
            segNocb.push_back(w.nocb ? 1 : 0);
            segCad.push_back(w.cad);
            segCmd.push_back(w.cmd);
        }
        pt.push_back(w.p);
    }
    size_t n = pt.size();
    cum.assign(n, 0.0);
    segVcap.assign(n > 1 ? n - 1 : 0, 0.0);
    segAcap.assign(segVcap.size(), 0.0);
    for (size_t i = 1; i < n; ++i) {
        VecN d = pt[i] - pt[i-1];
        cum[i] = cum[i-1] + d.norm();
        VecN u = d.normalized();
        double vcap = 1e300, acap = 1e300;
        for (int a = 0; a < NAX; ++a) {
            double ua = std::fabs(u[a]);
            if (ua < 1e-12) continue;
            vcap = std::min(vcap, ax.vmax[a] / ua);
            acap = std::min(acap, ax.amax[a] / ua);
        }
        double uxyz = u.xyzNorm();
        if (uxyz > 1e-12 && std::isfinite(segF[i-1]))
            vcap = std::min(vcap, segF[i-1] / uxyz);
        segVcap[i-1] = vcap;
        segAcap[i-1] = acap;
    }
    S = cum.empty() ? 0 : cum.back();
}

int Polyline::segIndex(double s) const
{
    s = std::min(std::max(s, 0.0), S);
    size_t lo = 0, hi = pt.size() - 1;
    while (lo + 1 < hi) {
        size_t mid = (lo + hi) / 2;
        if (cum[mid] <= s) lo = mid; else hi = mid;
    }
    return (int)lo;
}

VecN Polyline::pos(double s) const
{
    s = std::min(std::max(s, 0.0), S);
    int lo = segIndex(s);
    double seg = cum[lo+1] - cum[lo];
    double f = (seg > 0) ? (s - cum[lo]) / seg : 0.0;
    return pt[lo] + (pt[lo+1] - pt[lo]) * f;
}

// Moving average, window Nf, edge-hold padding.
// Output: x.size()+Nf-1 samples; starts at x.front(), ends at x.back().
static std::vector<double> movingAvgHold(const std::vector<double>& x, int Nf)
{
    int n = (int)x.size();
    if (Nf <= 1 || n == 0) return x;
    std::vector<long double> pref(n + 1, 0.0L);
    for (int i = 0; i < n; ++i) pref[i+1] = pref[i] + x[i];
    std::vector<double> y(n + Nf - 1);
    for (int k = 0; k < (int)y.size(); ++k) {
        long lo = (long)k - Nf + 1, hi = (long)k + 1;
        long double sum = 0.0L;
        if (lo < 0) { sum += (long double)(-lo) * x.front(); lo = 0; }
        if (hi > n) { sum += (long double)(hi - n) * x.back(); hi = n; }
        sum += pref[hi] - pref[lo];
        y[k] = (double)(sum / Nf);
    }
    return y;
}

// ----------------------------------------------------------------------
// Tolerance-aware corner fairing (G64 style). Each interior vertex is
// replaced by a quadratic Bezier blend whose apex offset spends at most
// fairFrac of the LOCAL tolerance; the blend half-length is also clamped
// to 45% of each adjacent segment so neighboring blends never overlap.
// Straight runs are untouched (zero displacement). Blend points inherit
// F/tol/seq/id from the segment they lie in (switching at the blend
// midpoint, matching the output seq semantics), and their tol is reduced
// by the apex offset the blend ACTUALLY spent - at most fairFrac of the
// tolerance, less when the half-length is clamped by short neighbors - so
// the planner's own filter/comp residual fits in the remaining budget:
// total deviation from the ORIGINAL path stays within the original
// tolerance.  Endpoints and vertices left unblended spend nothing and
// keep their full tolerance.
//
// For a symmetric blend of half-length L at a vertex with unit tangents
// d1 -> d2, the curve's farthest point from the corner is the apex,
// offset by |L*(d2-d1)|/4, so L = 4*budget/|d2-d1| is exact.
std::vector<Waypoint> fairPath(const std::vector<Waypoint>& in,
                               double fairFrac, double sampleStep)
{
    if (fairFrac <= 0.0 || in.size() < 3) return in;
    std::vector<Waypoint> w;                 // dedupe (mirrors Polyline::build)
    for (const Waypoint& q : in) {
        if (!w.empty() && (q.p - w.back().p).norm() <= 1e-12) continue;
        w.push_back(q);
    }
    size_t n = w.size();
    if (n < 3) return w;

    std::vector<double> Lb(n, 0.0);          // blend half-length per vertex
    std::vector<double> apex(n, 0.0);        // tip-referenced apex offset spent
    for (size_t i = 1; i + 1 < n; ++i) {
        VecN d1 = (w[i].p   - w[i-1].p).normalized();
        VecN d2 = (w[i+1].p - w[i].p  ).normalized();
        VecN dD = d2 - d1;
        // apex offset per axis is dD[a]*L/4; size L so the TIP-referenced
        // apex deviation (weighted norm) spends the budget exactly
        double dDw = wnorm(dD, wmax(w[i].wgt, w[i+1].wgt));
        if (dDw < 1e-9) continue;
        double budget = fairFrac * std::min(w[i].tol, w[i+1].tol);
        double L = 4.0 * budget / dDw;
        L = std::min(L, 0.45 * (w[i].p   - w[i-1].p).norm());
        L = std::min(L, 0.45 * (w[i+1].p - w[i].p  ).norm());
        Lb[i] = L;
        apex[i] = L * dDw / 4.0;             // <= budget
    }

    std::vector<Waypoint> out;
    out.reserve(n * 4);
    out.push_back(w[0]);                     // endpoint: nothing spent
    for (size_t i = 1; i + 1 < n; ++i) {
        if (Lb[i] <= 1e-12) {                // collinear: keep vertex as-is
            out.push_back(w[i]);
            continue;
        }
        VecN P0 = w[i].p + (w[i-1].p - w[i].p).normalized() * Lb[i];
        VecN P2 = w[i].p + (w[i+1].p - w[i].p).normalized() * Lb[i];
        int m = std::max(6, (int)std::ceil(2.0 * Lb[i]
                                           / std::max(sampleStep, 1e-9)));
        m = std::min(m, 64);
        for (int q = 0; q <= m; ++q) {
            double tb = (double)q / m;
            double b0 = (1-tb)*(1-tb), b1 = 2*tb*(1-tb), b2 = tb*tb;
            Waypoint nw = (tb < 0.5) ? w[i] : w[i+1];
            nw.p = P0*b0 + w[i].p*b1 + P2*b2;
            nw.tol -= apex[i];               // >= (1-fairFrac)*tol
            out.push_back(nw);
        }
    }
    out.push_back(w[n-1]);                   // endpoint: nothing spent
    return out;
}

// ======================================================================
// PathPlan
// ======================================================================
void PathPlan::compute(const Polyline& path, const Limits& lim, double s0abs)
{
    ok = false;
    pp = &path; lm = lim;
    lm.ax = effectiveLimits(lim.ax);   // Aeff everywhere (see the header)
    const AxisLimits& ax = lm.ax;
    Sloc = path.S;
    if (Sloc <= 0) return;
    // Non-positive limits mark an actuator slot that is NOT PART of the
    // coordinate system.  Such a slot holds a constant coordinate and never
    // moves, so every PER-AXIS cap below already ignores it for free (zero
    // direction content, zero angle content).  These whole-machine
    // REDUCTIONS have no such self-skip, so they must exclude it here:
    // otherwise a slot that contributes no motion still sets the FIR span
    // Tw = 3*max(amax/jmax) for every real axis, and inflates the
    // Vbig-derived corner-speed floor and Jmin-derived comp fade.
    double Rr = 0, Jmin = 1e300;
    int nActive = 0;
    Vbig = 0;
    for (int a = 0; a < NAX; ++a) {
        if (ax.vmax[a] <= 0 || ax.amax[a] <= 0 || ax.jmax[a] <= 0) continue;
        Rr   = std::max(Rr, ax.amax[a] / ax.jmax[a]);
        Vbig = std::max(Vbig, ax.vmax[a]);
        Jmin = std::min(Jmin, ax.jmax[a]);
        ++nActive;
    }
    if (nActive == 0) return;      // no usable axis: nothing can move
    T1 = 2.0 * Rr; T2 = Rr; Tw = T1 + T2;
    Cf = (T1*T1 + T2*T2) / 24.0;

    // Degenerate path: nothing to plan.  A "zero length" move is rarely
    // EXACTLY zero once it has been through the kinematics (a G0 to the
    // position already occupied round-trips to ~1e-12 actuator units), and
    // the tiny-path grid below sets ds = Sloc/8, so a picometer path
    // yields a picometer grid that everything scaled by the filter window
    // then dwarfs.  Bail like the Sloc <= 0 case above (emitCommon returns
    // no samples for !ok).
    //
    // ABSOLUTE threshold, in actuator units (inches or degrees) - the same
    // basis as Polyline::build's 1e-12 duplicate-point floor, and a billion
    // times below one count on any real drive.  Deliberately NOT scaled by
    // Vbig or Tw: callers assign PLACEHOLDER limits (vmax 1e6) to actuator
    // slots that are not present, so a Vbig-scaled threshold would grow to
    // ~1e-3 units on a machine with any unused axis and silently discard
    // legitimate short runs - whose displacement would then reappear as a
    // position step when the next run re-seeds from the interpreter.
    if (Sloc < 1e-9) return;

    double dt = lim.dt;
    N1 = std::max(2, (int)std::ceil(T1 / dt));
    N2 = std::max(2, (int)std::ceil(T2 / dt));
    dly = (N1 - 1 + N2 - 1) / 2;
    const double DD_DISCRETE = 2.0 * std::sin(15.0 * M_PI / 360.0); // ~15 deg
    const double NORMAL_ACCEL_CHARGE = 1.5;  // curvature accel charged against
                                             // the tangential budget (see the
                                             // backward/forward passes)

    // --- vertex direction changes, per-axis kink caps ---------------------
    verts.clear();
    const auto& P = path.pt;
    for (size_t i = 1; i + 1 < P.size(); ++i) {
        VecN d1 = (P[i]   - P[i-1]).normalized();
        VecN d2 = (P[i+1] - P[i]  ).normalized();
        VecN dD = d2 - d1;
        double dDn = dD.norm();
        if (dDn < 1e-12) continue;
        Vert v;
        v.s = path.cum[i]; v.dD = dD; v.dDn = dDn;
        // tip-referenced kink magnitude: per-axis deviation weighted by
        // tool-tip error per axis unit (weights default 1)
        v.dDw = wnorm(dD, wmax(path.segWgt[i-1], path.segWgt[i]));
        v.tol = std::min(path.segTol[i-1], path.segTol[i]);
        v.vc  = std::min(path.segVcap[i-1], path.segVcap[i]);
        v.halfSpan = 0.0;
        for (int a = 0; a < NAX; ++a) {
            double da = std::fabs(dD[a]);
            if (da < 1e-12) continue;
            v.vc = std::min(v.vc, ax.amax[a] * T1 / da);      // NATIVE units:
            v.vc = std::min(v.vc, ax.jmax[a] * T1 * T2 / da); // drive physics
        }
        if (v.dDw > 1e-12)                         // apex-miss TIP tolerance
            v.vc = std::min(v.vc, 8.0 * v.tol / (v.dDw * Tw));
        v.vc = std::max(v.vc, 1e-6 * Vbig);
        if (dDn > DD_DISCRETE) v.halfSpan = v.vc * Tw;
        verts.push_back(v);
    }

    // --- arc grid, anchored to absolute multiples of ds -------------------
    ds = (lim.dsFixed > 0) ? lim.dsFixed : Sloc / 4000.0;
    if (lim.dsFixed <= 0) {
        for (const Vert& v : verts)
            if (v.halfSpan > 0) ds = std::min(ds, v.halfSpan / 2.0);
        ds = std::max(ds, Sloc / 500000.0);
    }
    off = 0.0;
    if (s0abs > 0) {
        double r = std::fmod(s0abs, ds);
        if (r > 1e-9 * ds && ds - r > 1e-9 * ds) off = ds - r;
    }
    if ((Sloc - off) / ds < 8.0) { off = 0.0; ds = Sloc / 8.0; } // tiny path
    int K = (int)std::floor((Sloc - off) / ds * (1.0 + 1e-12));
    double wEnd = Sloc - (off + K * ds);
    N = K + 1 + (wEnd > 1e-9 * ds ? 1 : 0);
    if (N < 3) return;

    // per grid point: owning segment -> local caps
    std::vector<double> gVcap(N), gAcap(N), gTol(N);
    std::vector<int> gSeg(N, 0);
    {
        int j = 0, nseg = (int)path.segF.size();
        for (int i = 0; i < N; ++i) {
            double s = gs(i);
            while (j < nseg - 1 && path.cum[j + 1] < s) ++j;
            gVcap[i] = path.segVcap[j];
            gAcap[i] = path.segAcap[j];
            gTol[i]  = path.segTol[j];
            gSeg[i]  = j;
        }
    }

    // --- per-axis angle density -> curvature caps (fixed point) -----------
    std::vector<std::vector<double>> apax(NAX, std::vector<double>(N + 1, 0.0));
    std::vector<double> apxyz(N + 1, 0.0);
    {
        std::vector<std::vector<double>> A(NAX, std::vector<double>(N, 0.0));
        std::vector<double> Axyz(N, 0.0);
        for (const Vert& v : verts) {
            if (v.dDn > DD_DISCRETE) continue;  // discrete corners handled
                                                // by their kink caps
            // TENT deposition: split each kink between its two nearest
            // cells by fractional position instead of dumping it all into
            // the rounded one.  With point deposition a uniformly
            // tessellated curve whose facet spacing is a non-integer
            // number of cells lays its impulses down in a BEAT pattern
            // against the grid, which the windowed density then reads as
            // curvature ripple on a constant-curvature path.  (Clamps are
            // BEFORE the int cast: out-of-range or NaN double->int is UB;
            // the !(>) forms send NaN to cell 0.)
            double x = (v.s - off) / ds - 0.5;  // cell-center coordinates
            double xf = std::floor(x);
            int i0 = !(xf > 0.0) ? 0 : (xf >= (double)(N - 1)) ? N - 1 : (int)xf;
            int i1 = (i0 + 1 < N) ? i0 + 1 : N - 1;
            double w1 = x - (double)i0;
            if (!(w1 > 0.0)) w1 = 0.0;
            if (w1 > 1.0) w1 = 1.0;
            for (int a = 0; a < NAX; ++a) {
                double da = std::fabs(v.dD[a]);
                A[a][i0] += da * (1.0 - w1);
                A[a][i1] += da * w1;
            }
            Axyz[i0] += v.dDw * (1.0 - w1);     // tip-weighted density for
            Axyz[i1] += v.dDw * w1;             // tolerance caps
        }
        for (int a = 0; a < NAX; ++a)
            for (int i = 0; i < N; ++i) apax[a][i+1] = apax[a][i] + A[a][i];
        for (int i = 0; i < N; ++i) apxyz[i+1] = apxyz[i] + Axyz[i];
    }
    vlim.assign(N, 0.0);
    for (int i = 0; i < N; ++i) vlim[i] = gVcap[i];
    std::vector<double> kbxyz(N, 0.0);

    // Continuous windowed density: evaluate the cumulative-angle prefix at
    // REAL window-edge positions (linear interpolation between cells)
    // instead of whole cells.  A whole-cell window STEPS every time an
    // impulse enters or leaves as it slides; combined with the tent
    // deposition above this makes the density a continuous function of
    // position, so a uniformly tessellated curve reads as CONSTANT
    // curvature instead of cap noise that the time-optimal profile then
    // chases as an acceleration ripple (each cap step becomes one
    // filter-window-wide accel excursion).  u is in prefix coordinates
    // (cell k spans [k, k+1]); clamped edges also handle NaN (both
    // comparisons false -> P[0]).
    auto cumAt = [this](const std::vector<double>& P, double u) {
        if (!(u > 0.0)) return P[0];
        if (u >= (double)N) return P[N];
        int k = (int)u;
        double f = u - (double)k;
        return P[k] + f * (P[k + 1] - P[k]);
    };

    // per-axis windowed angle density at each grid point (last pass):
    // the curvature term the backward/forward passes charge against the
    // tangential accel budget
    std::vector<std::vector<double>> kbAxis(NAX, std::vector<double>(N, 0.0));
    // The caps and the window that measures them depend on each other (a
    // lower speed narrows the window, which reads a concentrated turn as
    // denser), so this is a fixed-point iteration: each pass takes the
    // speed to sqrt(c*v) of the previous one, i.e. the remaining error
    // ratio is (v0/v*)^(1/2^passes).  Three passes left an isolated 14 deg
    // kink planned 27% above its converged cap on a 200 deg/s joint
    // (200/30 -> 1.27 ratio), and its filtered accel pulse at 1.16x Amax;
    // twelve bring the ratio to 1.0005 and the pulse within Amax.  Paths
    // whose caps already agree with their windows (arcs, faired blends)
    // converge on the first pass and are unchanged.
    const int CURVATURE_PASSES = 12;
    std::vector<double> vPrev;
    for (int pass = 0; pass < CURVATURE_PASSES; ++pass) {
        vPrev = vlim;                         // for the convergence exit below
        for (int i = 0; i < N; ++i) {
            double L = std::max(vlim[i] * Tw, 2.0 * ds);
            // window half-width in cells; a window wider than the path
            // just means "use the whole path" (also catches NaN)
            double hw = 0.5 * L / ds;
            if (!(hw < (double)N)) hw = (double)N;
            double el = (double)i + 0.5 - hw;   // real edges in prefix
            double eh = (double)i + 0.5 + hw;   // coords (cell center i+0.5)
            if (el < 0.0) el = 0.0;
            if (eh > (double)N) eh = (double)N;
            double wlen = (eh - el) * ds;
            // Coefficient 0.45 ~ 1/2.2: the filtered per-axis accel in a
            // curvature region is centripetal v^2*kb amplified up to
            // Tw/T1 = 1.5x when the turning is concentrated in a burst
            // shorter than the filter window, plus the pre-compensation's
            // own acceleration (~0.2*v^2*kb, comp varies on the T2 scale),
            // for an effective multiplier of ~2.2 on v^2*kb. (0.7 was
            // enough only when vmax capped speed well below these caps.)
            for (int a = 0; a < NAX; ++a) {
                double kba = (cumAt(apax[a], eh) - cumAt(apax[a], el)) / wlen;
                kbAxis[a][i] = (kba > 0) ? kba : 0.0;
                if (kba <= 0) continue;
                double v = std::sqrt(0.45 * ax.amax[a] / kba);
                v = std::min(v, std::cbrt(0.45 * ax.jmax[a] / (kba * kba)));
                vlim[i] = std::min(vlim[i], v);
            }
            double kbx = (cumAt(apxyz, eh) - cumAt(apxyz, el)) / wlen;
            kbxyz[i] = kbx;
            if (kbx > 0) {
                double c3 = 0.8 * gTol[i] / (Cf * Cf * Cf);
                vlim[i] = std::min(vlim[i],
                          std::pow(c3 / std::pow(kbx, 5), 1.0/6.0));
            }
        }
        // a pass that changed no speed leaves the next one identical windows
        // and caps, so it would change nothing either: converged.  Exiting
        // here is bit-identical to running every pass; arcs and faired
        // blends leave after one or two, only concentrated kinks use them all
        if (vPrev == vlim) break;
    }

    // Comp velocity headroom: the lateral pre-compensation offset
    // (|c| ~ Cf*v^2*kb) rotates with the tangent (rate v*kb) and its
    // magnitude tracks the local curvature/window content (~1/Tw ramp
    // rate), so the comp'd raw stream carries extra per-axis velocity of
    // roughly c*(v*kb + 1/Tw) on top of the tangential speed. Reserve
    // that headroom against the segment caps so a vmax-saturated pass
    // through curvature stays within per-axis limits after comp.
    for (int i = 0; i < N; ++i) {
        if (kbxyz[i] <= 0) continue;
        for (int it = 0; it < 2; ++it) {
            double c     = Cf * vlim[i] * vlim[i] * kbxyz[i];
            double crate = c * (vlim[i] * kbxyz[i] + 1.0 / Tw);
            double vAllow = gVcap[i] - 3.0 * crate;
            vlim[i] = std::min(vlim[i], std::max(vAllow, 0.25 * gVcap[i]));
        }
    }

    // Compensation fade zones at the plan ends (comp must vanish at the
    // endpoints so the filter edge-hold keeps them exact), sized so the
    // comp ramp kink costs <= 15% of the smallest axis jerk limit, with a
    // matching strict speed cap inside the zone.
    Lf = 2.0 * ds;
    for (int i = 0; i < N; ++i)
        if (kbxyz[i] > 0)
            Lf = std::max(Lf, Cf * kbxyz[i] * std::pow(vlim[i], 3)
                              / (0.15 * Jmin * T1 * T2));
    Lf = std::min(Lf, 0.25 * Sloc);
    for (int i = 0; i < N; ++i) {
        double dStart = gs(i) - off, dEnd = Sloc - gs(i);
        double w = std::min(1.0, std::min(dStart, dEnd) / Lf);
        if (w < 1.0 && kbxyz[i] > 0)
            vlim[i] = std::min(vlim[i],
                std::sqrt(0.8 * gTol[i] / ((1.0 - w) * Cf * kbxyz[i])));
    }

    // --- discrete corner caps + plateaus -----------------------------------
    // Clamp the plateau span to the grid BEFORE converting to int: halfSpan
    // (= vc*Tw) is unrelated to ds, so the quotient is unbounded, and an
    // out-of-range or non-finite double->int cast is UB - on x86 it yields
    // INT_MIN, which here would make hi < lo and SILENTLY DROP the corner's
    // kink cap, running a sharp corner at the uncapped planned speed.
    for (const Vert& v : verts) {
        double flo = std::floor((v.s - v.halfSpan - off) / ds);
        double fhi = std::ceil ((v.s + v.halfSpan - off) / ds);
        // clamp in DOUBLE to the representable span, then cast.  -1 is the
        // "entirely before the grid" sentinel (loop body skips), preserving
        // the original behavior for genuinely out-of-window corners; NaN
        // falls to the widest span, which caps speed rather than releasing it
        if (!(flo > 0.0))               flo = 0.0;
        else if (flo > (double)(N - 1)) flo = (double)(N - 1);
        if (!(fhi < (double)(N - 1)))   fhi = (double)(N - 1);
        else if (fhi < -1.0)            fhi = -1.0;
        int lo = (int)flo, hi = (int)fhi;
        for (int i = lo; i <= hi; ++i) vlim[i] = std::min(vlim[i], v.vc);
    }

    // --- accel caps + time-optimal backward/forward passes ------------------
    std::vector<double> acap(N);
    for (int i = 0; i < N; ++i) {
        acap[i] = gAcap[i];
        if (kbxyz[i] > 0)
            acap[i] = std::min(acap[i], 8.0 * gTol[i] /
                      (Cf * kbxyz[i] * std::max(vlim[i], 1e-9) * Tw));
    }
    vlim[0] = vlim[N - 1] = 0.0;

    // Tangential accel NET of the curvature (normal) accel.  The curvature
    // caps above let the normal term alone use nearly all of an axis's Amax
    // (0.45 x the ~2.2 amplification), and acap lets the tangential term
    // alone use all of it, so a speed change THROUGH curvature - a curve
    // entry or exit ramping to or from a straight - added the two and
    // overshot Amax (measured +30% on a 3Link program at J/A = 150/s).  The
    // normal term is charged against the tangential budget at the speed
    // actually planned:
    //     a_t*|u_a| + NORMAL_ACCEL_CHARGE * v^2 * kb_a  <=  Amax_a
    // with kb_a the axis's windowed angle density from the curvature caps.
    // The budget falls with v, so each pass step takes the largest end
    // speed v1 with v1^2 <= v0^2 + 2*a(v1)*dx - a evaluated at the cell's
    // HIGHER end speed, which is conservative - found by bisection (a(v) is
    // non-increasing, so the feasible end speeds are one interval starting
    // at v0).  The density charged is the LARGER of the cell's two ends: on
    // the coarse streaming grid a small arc's windowed density can fall off
    // within one cell, and charging only the updated end (the one AWAY from
    // the curve on an exit or entry ramp) left 1.23x Amax on a R0.1 in 45
    // deg arc exit at A = 20 in/s^2 on the 0.02 grid (1.00x with the max).
    // At the curvature cap speed the charge is 1.5*0.45 = 0.675 of Amax, and
    // v0 is within both ends' caps, so some tangential budget always
    // remains at v0 and no ramp can stall.  Cells with no curvature at
    // either end take exactly the old step.
    std::vector<VecN> segU(path.segF.size());      // |unit tangent| per seg
    for (size_t j = 0; j < segU.size(); ++j) {
        VecN u = (path.pt[j + 1] - path.pt[j]).normalized();
        for (int a = 0; a < NAX; ++a) u[a] = std::fabs(u[a]);
        segU[j] = u;
    }
    std::vector<char> curved(N, 0);
    for (int i = 0; i < N; ++i)
        for (int a = 0; a < NAX; ++a)
            if (kbAxis[a][i] > 0) { curved[i] = 1; break; }
    auto accelAt = [&](int i, int nb, double v) {
        double at = acap[i];
        const VecN& u = segU[gSeg[i]];
        for (int a = 0; a < NAX; ++a) {
            if (u[a] < 1e-12 || ax.amax[a] <= 0) continue;
            double kb = std::max(kbAxis[a][i], kbAxis[a][nb]);
            double rem = ax.amax[a] - NORMAL_ACCEL_CHARGE * v * v * kb;
            at = std::min(at, std::max(rem, 0.0) / u[a]);
        }
        return at;
    };
    auto passStep = [&](int i, int nb, double v0, double dx, double vcap) {
        double hi = std::min(vcap, std::sqrt(v0*v0 + 2.0*acap[i]*dx));
        if (!(curved[i] || curved[nb]) || hi <= v0) return hi;
        if (std::sqrt(v0*v0 + 2.0*accelAt(i, nb, hi)*dx) >= hi) return hi;
        double lo = v0;                    // feasible: the budget is >= 0
        for (int it = 0; it < 12; ++it) {
            double m = 0.5 * (lo + hi);
            if (std::sqrt(v0*v0 + 2.0*accelAt(i, nb, m)*dx) >= m) lo = m; else hi = m;
        }
        return lo;
    };
    for (int i = N - 2; i >= 0; --i)
        vlim[i] = std::min(vlim[i], passStep(i, i + 1, vlim[i+1], gs(i + 1) - gs(i), vlim[i]));
    for (int i = 1; i < N; ++i)
        vlim[i] = std::min(vlim[i], passStep(i, i - 1, vlim[i-1], gs(i) - gs(i - 1), vlim[i]));
    Vpk = 0;
    for (int i = 0; i < N; ++i) Vpk = std::max(Vpk, vlim[i]);

    // --- v(s) -> t(s) --------------------------------------------------------
    ts.assign(N, 0.0);
    for (int i = 1; i < N; ++i) {
        double dx = gs(i) - gs(i - 1);
        ts[i] = ts[i-1] + 2.0 * dx / std::max(vlim[i-1] + vlim[i], 1e-9);
    }
    T0 = ts[N - 1];
    ok = true;
}

double PathPlan::sOfT(double t, int& j) const
{
    if (t <= 0) return gs(0);
    if (t >= T0) return Sloc;
    if (j < 0) j = 0;
    if (j > N - 2) j = N - 2;
    while (j > 0 && ts[j] > t) --j;
    while (j < N - 2 && ts[j + 1] < t) ++j;
    double span = std::max(ts[j+1] - ts[j], 1e-12);
    double f = (t - ts[j]) / span;
    f = std::min(1.0, std::max(0.0, f));
    double v0 = vlim[j], v1 = vlim[j+1];
    double dtau = t - ts[j];
    double s = gs(j) + 0.5 * (v0 + (v0 + f * (v1 - v0))) * dtau;
    return std::min(s, Sloc);
}

double PathPlan::sAtTime(double t) const
{
    int j = (int)(std::upper_bound(ts.begin(), ts.end(), t) - ts.begin()) - 1;
    j = std::min(N - 2, std::max(0, j));
    return sOfT(t, j);
}

double PathPlan::timeAtArc(double sLocal) const
{
    sLocal = std::min(std::max(sLocal, gs(0)), Sloc);
    int idx = (int)std::floor((sLocal - off) / ds);
    idx = std::min(N - 2, std::max(0, idx));
    while (idx < N - 2 && gs(idx + 1) < sLocal) ++idx;
    while (idx > 0 && gs(idx) > sLocal) --idx;
    double dsg  = sLocal - gs(idx);
    double span = std::max(ts[idx+1] - ts[idx], 1e-12);
    double v0 = vlim[idx], v1 = vlim[idx+1];
    double a2 = 0.5 * (v1 - v0) / span;        // s(x) = v0*x + a2*x^2
    double x;
    if (std::fabs(a2) < 1e-12) {
        x = dsg / std::max(v0, 1e-12);
    } else {
        double disc = std::max(v0 * v0 + 4.0 * a2 * dsg, 0.0);
        double den  = v0 + std::sqrt(disc);
        // den == 0 only at a rest start (v0 == 0, dsg == 0): the answer is
        // the cell START, not its end.  Returning span here made a streaming
        // replan anchored at tick 0 re-sample one grid cell ahead (seam
        // step of ds, jerk blip, plan clock one cell early); a genuine
        // zero-speed cell with dsg > 0 still maps to its end.
        x = (den > 1e-12) ? 2.0 * dsg / den : (dsg > 0.0 ? span : 0.0);
    }
    x = std::min(span, std::max(0.0, x));
    return ts[idx] + x;
}

PathPlan::Raw PathPlan::sampleCompRaw(double tFirst, int count) const
{
    Raw r;
    if (!ok || count <= 0) return r;
    const AxisLimits& ax = lm.ax;
    double dt = lm.dt;
    r.ch.assign(NAX, std::vector<double>(count));
    r.s.assign(count, 0.0);
    r.seg.assign(count, 0);

    int j;
    {
        double t0c = std::min(std::max(tFirst, 0.0), T0);
        j = (int)(std::upper_bound(ts.begin(), ts.end(), t0c) - ts.begin()) - 1;
        j = std::min(N - 2, std::max(0, j));
    }
    for (int m = 0; m < count; ++m) {
        double t = tFirst + m * dt;
        double s = (t >= T0) ? Sloc : sOfT(t, j);
        VecN p = pp->pos(s);
        for (int a = 0; a < NAX; ++a) r.ch[a][m] = p[a];
        r.s[m]   = s;
        r.seg[m] = pp->segIndex(s);
    }

    // --- exact-error undercut pre-compensation on this substream ----------
    // Lateral component of the filter's path error, measured and subtracted
    // twice; faded to zero at the plan endpoints and inside discrete-corner
    // spans; per-axis clamped (scale preserves direction).
    std::vector<const Vert*> discrete;
    for (const Vert& v : verts) if (v.halfSpan > 0) discrete.push_back(&v);
    std::vector<double> wgt(count);
    for (int m = 0; m < count; ++m) {
        double s = r.s[m];
        double dStart = std::max(s - off, 0.0), dEnd = std::max(Sloc - s, 0.0);
        double w = std::min(1.0, std::min(dStart, dEnd) / Lf);
        for (const Vert* v : discrete) {
            double d = std::fabs(s - v->s);
            if (d < v->halfSpan)            w = 0.0;
            else if (d < 2.0 * v->halfSpan) w = std::min(w, d / v->halfSpan - 1.0);
        }
        wgt[m] = std::max(w, 0.0);
    }
    int mb = std::max(1, (int)std::llround(0.5 * T2 / dt)); // tangent baseline
    std::vector<std::vector<double>> orig = r.ch;
    for (int iter = 0; iter < 2; ++iter) {
        std::vector<std::vector<double>> fc(NAX);
        for (int a = 0; a < NAX; ++a)
            fc[a] = movingAvgHold(movingAvgHold(r.ch[a], N1), N2);
        for (int k = 0; k < count; ++k) {
            double e[NAX], tv[NAX];
            for (int a = 0; a < NAX; ++a)
                e[a] = fc[a][k + dly] - orig[a][k];
            int kp = std::min(k + mb, count - 1), km = std::max(k - mb, 0);
            double tn2 = 0;
            for (int a = 0; a < NAX; ++a) {
                tv[a] = orig[a][kp] - orig[a][km];
                tn2 += tv[a] * tv[a];
            }
            if (tn2 > 1e-24) {
                double d = 0;
                for (int a = 0; a < NAX; ++a) d += e[a] * tv[a];
                d /= tn2;
                for (int a = 0; a < NAX; ++a) e[a] -= d * tv[a];
            }
            double g = 1.0;
            for (int a = 0; a < NAX; ++a) {
                double ea = std::fabs(e[a]);
                if (ea > 1e-30)
                    g = std::min(g, 4.0 * Cf * ax.amax[a] / ea);
            }
            for (int a = 0; a < NAX; ++a)
                r.ch[a][k] -= wgt[k] * g * e[a];
        }
    }
    return r;
}

// ----------------------------------------------------------------------
// One-shot planning: full path, rest-to-rest, edge-hold filtered.
// ----------------------------------------------------------------------
std::vector<TrajPoint> planTrajectory(const Polyline& path, const Limits& lim)
{
    std::vector<TrajPoint> out;
    PathPlan pl;
    pl.compute(path, lim, 0.0);
    if (!pl.ok) return out;
    double dt = lim.dt;
    int M0 = (int)std::ceil(pl.T0 / dt) + 1;
    PathPlan::Raw r = pl.sampleCompRaw(0.0, M0);

    std::vector<std::vector<double>> ch(NAX);
    for (int a = 0; a < NAX; ++a)
        ch[a] = movingAvgHold(movingAvgHold(r.ch[a], pl.N1), pl.N2);

    size_t M = ch[0].size();
    out.reserve(M);
    for (size_t k = 0; k < M; ++k) {
        TrajPoint q;
        q.t = k * dt;
        for (int a = 0; a < NAX; ++a) q.p[a] = ch[a][k];
        long long src = (long long)k - pl.dly;                // group delay
        src = std::min((long long)M0 - 1, std::max(0LL, src));
        int sg = r.seg[(size_t)src];
        q.seq = path.segSeq[sg];
        q.id  = path.segId[sg];
        q.F   = path.segF[sg];
        q.rapid = path.segRapid[sg] != 0;
        q.nocb = path.segNocb[sg] != 0;
        q.cad = path.segCad[sg];
        q.cmd = path.segCmd[sg];
        out.push_back(q);
    }
    return out;
}

// ======================================================================
// StreamingTrajectoryPlanner
// ======================================================================
StreamingTrajectoryPlanner::StreamingTrajectoryPlanner(
        const Limits& lim, double targetBufferTime,
        std::function<double()> pollFn)
    : lim_(lim), targetBuffer_(targetBufferTime), poll_(pollFn)
{
    if (targetBufferTime <= 0)
        throw std::invalid_argument("targetBufferTime must be > 0");
    if (!pollFn)
        throw std::invalid_argument("pollController callback required");
    if (lim.dsFixed <= 0)
        throw std::invalid_argument(
            "streaming requires Limits::dsFixed > 0 (grid stability)");
    // Effective accelerations (see the header) - the same reduction
    // PathPlan::compute applies, so the windows N1_/N2_ used to splice the
    // persisted raw stream are exactly the windows each replan plans with.
    lim_.ax = effectiveLimits(lim.ax);
    const AxisLimits& ax = lim_.ax;
    // Skip actuator slots that are not part of the coordinate system (see
    // PathPlan::compute).  Besides keeping a non-participating slot out of
    // the machine-wide constants, this avoids 0/0 in these ratios.
    double Rr = 0, vb = 0, Jmin = 1e300;
    decelMax_ = 0;
    int nActive = 0;
    for (int a = 0; a < NAX; ++a) {
        if (ax.vmax[a] <= 0 || ax.amax[a] <= 0 || ax.jmax[a] <= 0) continue;
        Rr        = std::max(Rr, ax.amax[a] / ax.jmax[a]);
        decelMax_ = std::max(decelMax_, ax.vmax[a] / ax.amax[a]);
        vb        = std::max(vb, ax.vmax[a]);
        Jmin      = std::min(Jmin, ax.jmax[a]);
        ++nActive;
    }
    if (nActive == 0)
        throw std::invalid_argument(
            "no axis has positive vmax/amax/jmax (all slots inactive)");
    T1_ = 2.0 * Rr; T2_ = Rr; Tw_ = 3.0 * Rr;
    // A raw position step e at a splice becomes a filtered jerk blip of
    // ~ e/(T1*T2*dt); warn when that would exceed 10% of the tightest
    // axis jerk limit.
    seamWarn_ = 0.10 * Jmin * T1_ * T2_ * lim.dt;
    N1_ = std::max(2, (int)std::ceil(T1_ / lim.dt));
    N2_ = std::max(2, (int)std::ceil(T2_ / lim.dt));
    dly_ = (N1_ - 1 + N2_ - 1) / 2;
    // comp/filter edge-effect span: the two comp iterations reach
    // ~(N1+N2) ticks inward from a window edge (each reads the cascade
    // dly=(N1+N2-2)/2 ahead and behind, and the second iteration doubles
    // the reach), so 2x(N1+N2) leaves ~2x safety.  Was 6x - pure
    // overhead in the per-update context collar, the emission margin,
    // and therefore also the streaming (jog) start latency.
    histTicks_ = 2 * (N1_ + N2_);
    ctxTime_   = 4.0 * decelMax_ + 12.0 * Tw_ + histTicks_ * lim.dt;
    keepRawTicks_ = (long long)(N1_ + N2_) + histTicks_ + 64;
    R_.assign(NAX, {});
}

void StreamingTrajectoryPlanner::addWaypoint(const Waypoint& w)
{
    if (flushed_) throw std::runtime_error("planner already flushed");
    while (!canAddWaypoint())
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    kept_.push_back(w);
    ++totalWaypoints_;
}

void StreamingTrajectoryPlanner::addWaypointNoWait(const Waypoint& w)
{
    if (flushed_) throw std::runtime_error("planner already flushed");
    kept_.push_back(w);
    ++totalWaypoints_;
}

std::vector<TrajPoint> StreamingTrajectoryPlanner::update()
{
    if (flushed_) return {};
    return emitCommon(false);
}

std::vector<TrajPoint> StreamingTrajectoryPlanner::flush()
{
    if (flushed_) return {};
    std::vector<TrajPoint> out = emitCommon(true);
    flushed_ = true;
    return out;
}

std::vector<TrajPoint> StreamingTrajectoryPlanner::emitCommon(bool toEnd)
{
    std::vector<TrajPoint> out;
    if (kept_.size() < 2) return out;
    double dt = lim_.dt;
    Polyline path;
    path.build(kept_, lim_.ax);
    PathPlan pl;
    pl.compute(path, lim_, sRetired_);
    if (!pl.ok) return out;

    // Splice anchor: plan time of the last written tick. Program tick
    // (Ka + m) is sampled at plan time (tA + m*dt); the phase offset
    // between plan clock and program clock is absorbed here.
    double tA;
    long long Ka;
    int mHist;
    if (rawCount_ == 0) { tA = -dt; Ka = -1; mHist = 0; }
    else {
        Ka = rawCount_ - 1;
        double sLoc = Rs_[(size_t)(Ka - rawBase_)] - sRetired_;
        tA = pl.timeAtArc(sLoc);
        mHist = histTicks_;
    }

    // Finalization frontier: hold back everything a future waypoint (or
    // this plan's own temporary deceleration/end-fade) could still change.
    double histTime = histTicks_ * dt;
    long long newN;
    if (toEnd) {
        newN = (long long)std::ceil((pl.T0 - tA) / dt - 1e-9);
        if (newN < 1) newN = 1;
    } else {
        double sLfStart = std::max(pl.off, pl.Sloc - pl.Lf);
        double tInLf = pl.T0 - pl.timeAtArc(sLfStart);
        double margin = std::max(decelMax_ + 3.0 * Tw_, tInLf + 3.0 * Tw_)
                      + histTime;
        lastMargin_ = margin;   // exposed: pacing callers (jog) must
                                // buffer MORE than this or nothing emits.
                                // tInLf is TIME through the end fade - at
                                // slow speeds (kinematic crawl, slow jog)
                                // it dwarfs any arc-length-based estimate
        double tFront = pl.T0 - margin;
        newN = (long long)std::floor((tFront - tA) / dt + 1e-9);
        if (newN <= 0) return out;
    }

    // Sample: history context + new ticks (+ tail context so comp edge
    // effects stay clear of appended ticks; flush needs none, the true
    // end-hold IS the tail).
    double tFirst = tA + (1 - mHist) * dt;
    long long count = mHist + newN + (toEnd ? 0 : histTicks_);
    PathPlan::Raw r = pl.sampleCompRaw(tFirst, (int)count);

    if (mHist > 0) {                    // seam check at the anchor tick
        double d = 0;
        for (int a = 0; a < NAX; ++a)
            d = std::max(d, std::fabs(r.ch[a][mHist - 1] - Rval(Ka, a)));
        seamWorst_ = std::max(seamWorst_, d);
        if (d > seamWarn_)
            std::fprintf(stderr,
                "WARNING: splice seam mismatch %.3e at tick %lld "
                "(> %.1e, jerk impact > 10%% of tightest limit)\n",
                d, Ka, seamWarn_);
    }

    for (long long q = mHist; q < mHist + newN; ++q) {
        for (int a = 0; a < NAX; ++a) R_[a].push_back(r.ch[a][(size_t)q]);
        Rs_.push_back(sRetired_ + r.s[(size_t)q]);
        int sg = r.seg[(size_t)q];
        Rseq_.push_back(path.segSeq[sg]);
        Rid_.push_back(path.segId[sg]);
        Rf_.push_back(path.segF[sg]);
        Rrapid_.push_back(path.segRapid[sg]);
        Rnocb_.push_back(path.segNocb[sg]);
        Rcad_.push_back(path.segCad[sg]);
        Rcmd_.push_back(path.segCmd[sg]);
    }
    long long k0 = rawCount_;
    rawCount_ += newN;

    if (toEnd) {                        // filter edge-hold tail
        int holds = (N1_ - 1) + (N2_ - 1);
        for (int h = 0; h < holds; ++h) {
            for (int a = 0; a < NAX; ++a) R_[a].push_back(R_[a].back());
            Rs_.push_back(Rs_.back());
            Rseq_.push_back(Rseq_.back());
            Rid_.push_back(Rid_.back());
            Rf_.push_back(Rf_.back());
            Rrapid_.push_back(Rrapid_.back());
            Rnocb_.push_back(Rnocb_.back());
            Rcad_.push_back(Rcad_.back());
            Rcmd_.push_back(Rcmd_.back());
        }
        rawCount_ += holds;
    }

    out = filterEmit(k0, rawCount_ - 1);

    if (!toEnd) {
        // Retire path behind (frontier - context): the artificial rest
        // start of the next replan, its curvature windows, comp fade and
        // filter/comp history must all stay behind the next splice.
        double tLastApp = tA + newN * dt;
        double tKeep = tLastApp - ctxTime_;
        if (tKeep > 0) {
            double sKeepAbs = sRetired_ + pl.sAtTime(tKeep)
                            - (pl.Lf + 6.0 * pl.Vpk * Tw_);
            while (kept_.size() > 2) {
                double segLen = (kept_[1].p - kept_[0].p).norm();
                if (sRetired_ + segLen >= sKeepAbs) break;
                sRetired_ += segLen;
                kept_.erase(kept_.begin());
            }
        }
        long long keepFrom = rawCount_ - keepRawTicks_;
        if (keepFrom > rawBase_) {
            size_t n = (size_t)(keepFrom - rawBase_);
            for (int a = 0; a < NAX; ++a)
                R_[a].erase(R_[a].begin(), R_[a].begin() + n);
            Rs_.erase(Rs_.begin(), Rs_.begin() + n);
            Rseq_.erase(Rseq_.begin(), Rseq_.begin() + n);
            Rid_.erase(Rid_.begin(), Rid_.begin() + n);
            Rf_.erase(Rf_.begin(), Rf_.begin() + n);
            Rrapid_.erase(Rrapid_.begin(), Rrapid_.begin() + n);
            Rnocb_.erase(Rnocb_.begin(), Rnocb_.begin() + n);
            Rcad_.erase(Rcad_.begin(), Rcad_.begin() + n);
            Rcmd_.erase(Rcmd_.begin(), Rcmd_.begin() + n);
            rawBase_ = keepFrom;
        }
    }
    return out;
}

std::vector<TrajPoint> StreamingTrajectoryPlanner::filterEmit(long long k0,
                                                              long long k1)
{
    std::vector<TrajPoint> out((size_t)(k1 - k0 + 1));
    double dt = lim_.dt;
    long long zlo = k0 - (N2_ - 1);
    long long xlo = zlo - (N1_ - 1);
    long long xn = k1 - xlo + 1, zn = k1 - zlo + 1;
    std::vector<long double> P1((size_t)xn + 1, 0.0L), P2((size_t)zn + 1, 0.0L);
    for (int a = 0; a < NAX; ++a) {
        for (long long i = 0; i < xn; ++i)
            P1[(size_t)i + 1] = P1[(size_t)i] + (long double)Rval(xlo + i, a);
        for (long long i = 0; i < zn; ++i) {
            long long j = zlo + i;
            long double z = (P1[(size_t)(j - xlo + 1)]
                           - P1[(size_t)(j - N1_ + 1 - xlo)]) / N1_;
            P2[(size_t)i + 1] = P2[(size_t)i] + z;
        }
        for (long long k = k0; k <= k1; ++k) {
            long double y = (P2[(size_t)(k - zlo + 1)]
                           - P2[(size_t)(k - N2_ + 1 - zlo)]) / N2_;
            out[(size_t)(k - k0)].p[a] = (double)y;
        }
    }
    for (long long k = k0; k <= k1; ++k) {
        TrajPoint& q = out[(size_t)(k - k0)];
        q.t = k * dt;
        long long src = k - dly_;               // group-delay attribution
        src = std::min(k1, std::max(rawBase_, src));
        q.seq = Rseq_[(size_t)(src - rawBase_)];
        q.id  = Rid_[(size_t)(src - rawBase_)];
        q.F   = Rf_[(size_t)(src - rawBase_)];
        q.rapid = Rrapid_[(size_t)(src - rawBase_)] != 0;
        q.nocb = Rnocb_[(size_t)(src - rawBase_)] != 0;
        q.cad = Rcad_[(size_t)(src - rawBase_)];
        q.cmd = Rcmd_[(size_t)(src - rawBase_)];
    }
    return out;
}

} // namespace TP3
