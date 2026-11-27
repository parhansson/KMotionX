// MotionLogPlotter - plots KMotionCNC Trajectory Planner segment logs
// (c:\Temp\TPSegLog.csv, written when the "Log" checkbox on the
// Trajectory Planner screen is enabled).
//
// Left:  XY path plot.  Mouse wheel zooms about the cursor, left-drag
//        pans, RIGHT-DRAG a rectangle to select the data region to
//        graph, right-CLICK clears the selection, MIDDLE-CLICK or
//        F/Home fits all.
//        Double-click places the measure marker SNAPPED to the nearest
//        path sample - the same sample is marked on every time plot.
// Right: time plots of Position, Velocity, Accel, Jerk (finite
//        differences of the log) for the axis chosen in the combo box,
//        over the selected region.  Wheel zooms time about the cursor
//        (all stay in sync), left-drag pans, double-click places the
//        shared marker (the XY X mark follows), MIDDLE-CLICK or F/Home
//        fits.  Readout letters: D=Dest P=Pos V/A/J=derivatives.
//        The readouts of ALL strips show the same sample: the one under
//        the cursor while it is inside any strip, otherwise the marked
//        sample (readout drawn in red) when a marker is set, else the
//        last cursor sample.

using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Globalization;
using System.IO;
using System.Windows.Forms;

namespace MotionLogPlotter
{
    // ------------------------------------------------------------------
    // One downloaded segment: geometry + trip-state polynomials, exactly
    // as sent to the controller (float resolution).
    public class Seg
    {
        public char Kind;                 // 'L' linear/rapid/cubic, 'A' arc, 'D' dwell,
                                          // 'K' per-axis cubic knot (TRAJECTORY_CUBIC8)
        public int Plane, Ccw, Seq, NTrips;
        public double Dx, Dwell;
        public double[] P0 = new double[8], P1 = new double[8];
        public double Xc, Yc;
        public double[] Tt = new double[7], Ta = new double[7],
                        Tb = new double[7], Tc = new double[7], Td = new double[7];
        public double[] K8 = null;        // 'K' only: 8 axes x a,b,c,d of time
        public double TotalT;

        // 8-axis position at local time t: per-axis cubic for 'K' knots,
        // distance-trip + geometry for everything else
        public void Eval(double t, double[] o)
        {
            if (Kind == 'K')
            {
                if (t < 0) t = 0; if (t > TotalT) t = TotalT;
                for (int a = 0; a < 8; a++)
                {
                    int j = 4 * a;
                    o[a] = ((K8[j] * t + K8[j + 1]) * t + K8[j + 2]) * t + K8[j + 3];
                }
                return;
            }
            PointAt(DistAt(t), o);
        }

        // distance along the segment at local time t (trip cubics
        // a t^3 + b t^2 + c t + d, absolute distance per trip)
        public double DistAt(double t)
        {
            for (int i = 0; i < NTrips; i++)
            {
                if (t <= Tt[i] || i == NTrips - 1)
                {
                    if (t > Tt[i]) t = Tt[i];
                    return ((Ta[i] * t + Tb[i]) * t + Tc[i]) * t + Td[i];
                }
                t -= Tt[i];
            }
            return Dx;
        }

        // 8-axis point at path distance s; arcs are stored plane-local
        // (x,y = plane, z = third axis) and un-swapped here
        public void PointAt(double s, double[] o)
        {
            double f = Dx > 0 ? s / Dx : 0;
            if (f < 0) f = 0; if (f > 1) f = 1;

            if (Kind == 'A')
            {
                double r0 = Math.Sqrt((P0[0] - Xc) * (P0[0] - Xc) + (P0[1] - Yc) * (P0[1] - Yc));
                double r1 = Math.Sqrt((P1[0] - Xc) * (P1[0] - Xc) + (P1[1] - Yc) * (P1[1] - Yc));
                double th0 = Math.Atan2(P0[1] - Yc, P0[0] - Xc);
                double th1 = Math.Atan2(P1[1] - Yc, P1[0] - Xc);
                double dth = th1 - th0;
                if (Ccw != 0) { if (dth <= 0) dth += 2 * Math.PI; }
                else { if (dth >= 0) dth -= 2 * Math.PI; }
                double th = th0 + f * dth;
                double r = r0 + f * (r1 - r0);
                double lx = Xc + r * Math.Cos(th);
                double ly = Yc + r * Math.Sin(th);
                double lz = P0[2] + f * (P1[2] - P0[2]);
                if (Plane == 1) { o[2] = lx; o[0] = ly; o[1] = lz; }
                else if (Plane == 2) { o[1] = lx; o[0] = lz; o[2] = ly; }
                else { o[0] = lx; o[1] = ly; o[2] = lz; }
            }
            else
            {
                o[0] = P0[0] + f * (P1[0] - P0[0]);
                o[1] = P0[1] + f * (P1[1] - P0[1]);
                o[2] = P0[2] + f * (P1[2] - P0[2]);
            }
            for (int a = 3; a < 8; a++) o[a] = P0[a] + f * (P1[a] - P0[a]);
        }
    }

    // ------------------------------------------------------------------
    public static class Util
    {
        // numeric display: exponential below 1e-3 so small errors are
        // never masked by decimal truncation
        public static string Fmt(double v)
        {
            if (double.IsNaN(v)) return "-";      // no value (derivative boundary)
            if (v == 0) return "0";
            double a = Math.Abs(v);
            if (a < 1e-3 || a >= 1e7) return v.ToString("0.####e-0", CultureInfo.InvariantCulture);
            return v.ToString("0.######", CultureInfo.InvariantCulture);
        }
    }

    // ------------------------------------------------------------------
    public class MotionLog
    {
        public double[] T = new double[0];
        public double[][] P = new double[8][];   // X Y Z A B C U V positions
        public double[][] P2 = new double[8][];  // measured positions (Dest/Pos
                                                 // capture logs: P=Dest P2=Pos)
        public bool HasP2;                       // second series present
        public double Dt = 0.001;
        public static readonly string[] AxisNames = { "X", "Y", "Z", "A", "B", "C", "U", "V" };

        public List<Seg> Segs;                   // segment-format logs only
        public int[] SegIdx = null;              // segment index per sample
        public double[] BoundaryT = null;        // segment start times
        public double[] BoundaryX = null, BoundaryY = null;

        public int Count { get { return T.Length; } }

        public enum FilterKind { None, KLP, MovAvg, MovAvg2 }
        public FilterKind Filter = FilterKind.None;
        public double FilterTau = 0.01;             // KLP Tau / boxcar window (s)
        public bool CompDelay = false;              // remove group delay from FiltErr

        readonly double[][] flt = new double[8][];
        readonly double[][] vel = new double[8][];
        readonly double[][] acc = new double[8][];
        readonly double[][] jrk = new double[8][];
        readonly double[][] ferr = new double[8][];

        public void SetFilter(FilterKind kind, double tau)
        {
            SetFilter(kind, tau, CompDelay);
        }

        public void SetFilter(FilterKind kind, double tau, bool compDelay)
        {
            Filter = kind;
            FilterTau = tau;
            CompDelay = compDelay;
            for (int a = 0; a < 8; a++)
                { flt[a] = null; vel[a] = null; acc[a] = null; jrk[a] = null; ferr[a] = null; }
        }

        // nominal group delay of the active filter, in samples: the part
        // of filtered-minus-raw that is pure transport lag (v * delay)
        // rather than actual shape distortion
        int GroupDelaySamples()
        {
            if (Dt <= 0) return 0;          // degenerate tick: FilterTau/Dt
                                            // is infinite and casts to
                                            // int.MinValue
            switch (Filter)
            {
                case FilterKind.KLP:
                    { double d = Math.Round(FilterTau / Dt); return d > 0 ? (d < 1e6 ? (int)d : 1000000) : 0; }
                case FilterKind.MovAvg:
                    { int n = Math.Max(1, (int)Math.Round(FilterTau / Dt)); return (n - 1) / 2; }
                case FilterKind.MovAvg2:
                    { int n2 = Math.Max(1, (int)Math.Round(FilterTau / (2 * Dt))); return n2 - 1; }
                default:
                    return 0;
            }
        }

        // position error the filter causes: filtered - unfiltered for the
        // axis; with CompDelay the filtered series is advanced by the
        // group delay first, isolating shape distortion from pure lag
        public double[] FiltErr(int axis)
        {
            if (ferr[axis] == null)
            {
                double[] f = Filt(axis), r = P[axis];
                int n = r == null ? 0 : r.Length;
                int fn = f == null ? 0 : f.Length;
                var e = new double[n];
                int sh = CompDelay ? GroupDelaySamples() : 0;
                for (int i = 0; i < n && fn > 0; i++)
                {
                    // clamp against the FILTERED series - it is the array
                    // being indexed, and sh may be any magnitude
                    int j = i + sh;
                    if (j < 0) j = 0;
                    if (j > fn - 1) j = fn - 1;
                    e[i] = f[j] - r[i];
                }
                ferr[axis] = e;
            }
            return ferr[axis];
        }

        static double D(string s)
        {
            double v;
            double.TryParse(s, NumberStyles.Float, CultureInfo.InvariantCulture, out v);
            return v;
        }

        static bool Finite(double v)
        {
            return !double.IsNaN(v) && !double.IsInfinity(v);
        }

        // a segment row even without a header: kind letter + the full
        // segment column count (a sampled row has 9 columns)
        static bool IsSegRow(string line)
        {
            if (line == null) return false;
            string[] f = line.Split(',');
            if (f.Length < 60 || f[0].Length != 1) return false;
            return "LARDK".IndexOf(char.ToUpperInvariant(f[0][0])) >= 0;
        }

        public static MotionLog Load(string path)
        {
            var log = new MotionLog();
            // FileShare.ReadWrite: the log may still be held open for write
            // by KMotionCNC (mid-run, or after an abort) - a plain
            // StreamReader would be refused with a sharing violation
            using (var fs = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
            using (var sr = new StreamReader(fs))
            {
                string first = sr.ReadLine();
                if (first != null && first.StartsWith("kind"))
                    LoadSegments(log, sr, null);        // header consumed
                else if (IsSegRow(first))
                    LoadSegments(log, sr, first);       // headerless segment log
                else
                    LoadSampled(log, sr, first);
            }
            return log;
        }

        // segment-format log: one row per downloaded segment.  firstLine is
        // the already-read row to process before the rest (null if it was a
        // header).
        static void LoadSegments(MotionLog log, StreamReader sr, string firstLine)
        {
            log.Segs = new List<Seg>();
            for (string line = firstLine ?? sr.ReadLine(); line != null; line = sr.ReadLine())
            {
                if (line.Length == 0) continue;
                string[] f = line.Split(',');
                if (f.Length < 60) continue;
                var s = new Seg();
                s.Kind = f[0].Length > 0 ? f[0][0] : 'L';
                s.Plane = (int)D(f[1]);
                s.Ccw = (int)D(f[2]);
                s.Seq = (int)D(f[3]);
                s.Dx = D(f[4]);
                s.Dwell = D(f[5]);
                s.NTrips = (int)D(f[6]);
                for (int a = 0; a < 8; a++) s.P0[a] = D(f[7 + a]);
                for (int a = 0; a < 8; a++) s.P1[a] = D(f[15 + a]);
                s.Xc = D(f[23]); s.Yc = D(f[24]);
                for (int i = 0; i < 7; i++)
                {
                    s.Tt[i] = D(f[25 + 5 * i]);
                    s.Ta[i] = D(f[26 + 5 * i]);
                    s.Tb[i] = D(f[27 + 5 * i]);
                    s.Tc[i] = D(f[28 + 5 * i]);
                    s.Td[i] = D(f[29 + 5 * i]);
                }
                if (s.Kind == 'K')
                {
                    // per-axis cubic knot: trip cols carry tt0=duration
                    // then the 32 axis coefficients (8 x a,b,c,d).  Skip
                    // the trip sanitizer - coefficients are legitimately
                    // negative and must not be zeroed.
                    s.K8 = new double[32];
                    for (int j = 0; j < 32; j++)
                    {
                        s.K8[j] = D(f[26 + j]);
                        if (!Finite(s.K8[j])) s.K8[j] = 0;
                    }
                    if (!Finite(s.Dx)) s.Dx = 0;
                    s.NTrips = 1;
                    s.TotalT = (Finite(s.Tt[0]) && s.Tt[0] > 0) ? s.Tt[0] : 0;
                    log.Segs.Add(s);
                    continue;
                }

                // sanitize: zero-length stop segments can carry inf/NaN
                // in unused trip coefficients (zero-duration trips the
                // controller never evaluates) - neutralize them so the
                // reconstruction can't produce NaN positions
                for (int i = 0; i < 7; i++)
                {
                    if (!Finite(s.Tt[i]) || s.Tt[i] < 0) s.Tt[i] = 0;
                    if (s.Tt[i] <= 0 || !Finite(s.Ta[i]) || !Finite(s.Tb[i])
                        || !Finite(s.Tc[i]) || !Finite(s.Td[i]))
                    {
                        if (!Finite(s.Ta[i])) s.Ta[i] = 0;
                        if (!Finite(s.Tb[i])) s.Tb[i] = 0;
                        if (!Finite(s.Tc[i])) s.Tc[i] = 0;
                        if (!Finite(s.Td[i])) s.Td[i] = 0;
                    }
                }
                if (!Finite(s.Dx)) s.Dx = 0;
                if (!Finite(s.Dwell) || s.Dwell < 0) s.Dwell = 0;

                s.TotalT = 0;
                for (int i = 0; i < s.NTrips && i < 7; i++) s.TotalT += s.Tt[i];
                if (s.TotalT <= 0) s.TotalT = s.Dwell;   // dwell holds position
                log.Segs.Add(s);
            }
        }

        // sampled-format logs, three row shapes:
        //   t + 8 positions                  legacy Dest-only (9 columns)
        //   t,Pos0..2,Dest0..2               Capture...PosDestToFile (7 cols)
        //   t + 8 Dest + 8 Pos               full dual capture (17 columns)
        // Dest goes to P (the commanded series everything else derives
        // from); measured Position goes to P2 for overlay/error plots.
        static void LoadSampled(MotionLog log, StreamReader sr, string firstLine)
        {
            var t = new List<double>();
            var p = new List<double>[8];
            var p2 = new List<double>[8];
            for (int a = 0; a < 8; a++) { p[a] = new List<double>(); p2[a] = new List<double>(); }
            bool anyP2 = false;
            for (string line = firstLine; line != null; line = sr.ReadLine())
            {
                if (line.Length == 0) continue;
                string[] f = line.Split(',');
                if (f.Length < 7) continue;
                double tv;
                // skip any header/text row rather than parsing it as time 0
                if (!double.TryParse(f[0], NumberStyles.Float, CultureInfo.InvariantCulture, out tv))
                    continue;
                t.Add(tv);
                if (f.Length >= 17)
                {
                    for (int a = 0; a < 8; a++) p[a].Add(D(f[a + 1]));
                    for (int a = 0; a < 8; a++) p2[a].Add(D(f[a + 9]));
                    anyP2 = true;
                }
                else if (f.Length >= 9)
                {
                    for (int a = 0; a < 8; a++) p[a].Add(D(f[a + 1]));
                    for (int a = 0; a < 8; a++) p2[a].Add(0);
                }
                else   // 7-8 columns: t, Pos x3, Dest x3
                {
                    for (int a = 0; a < 3; a++) p2[a].Add(D(f[a + 1]));
                    for (int a = 0; a < 3; a++) p[a].Add(D(f[a + 4]));
                    for (int a = 3; a < 8; a++) { p[a].Add(0); p2[a].Add(0); }
                    anyP2 = true;
                }
            }
            log.T = t.ToArray();
            for (int a = 0; a < 8; a++) log.P[a] = p[a].ToArray();
            if (anyP2)
            {
                for (int a = 0; a < 8; a++) log.P2[a] = p2[a].ToArray();
                log.HasP2 = true;
            }
            if (log.Count > 1)
            {
                double dt = (log.T[log.Count - 1] - log.T[0]) / (log.Count - 1);
                if (dt > 0) log.Dt = dt;    // never allow a degenerate 0 tick
            }
        }

        public double BuiltTail = 0;             // settle time appended (s)

        // Re-create the controller's interpolation: step time forward by
        // the servo tick, carrying leftover time across segment
        // boundaries exactly like DoTrajectorys (CS0_t -= CS->t on
        // rollover), and evaluate the trip polynomials at each tick.
        // tailSeconds of hold-at-end samples are appended so lagging
        // filters (KLP etc.) visibly settle instead of the plot ending
        // at the last commanded sample.
        public void BuildSamples(double tick) { BuildSamples(tick, BuiltTail); }

        public void BuildSamples(double tick, double tailSeconds)
        {
            if (Segs == null) return;
            Dt = tick;
            BuiltTail = tailSeconds;
            var t = new List<double>();
            var pp = new List<double>[8];
            for (int a = 0; a < 8; a++) pp[a] = new List<double>();
            var sidx = new List<int>();
            var bt = new List<double>();
            var bx = new List<double>();
            var by = new List<double>();
            var o = new double[8];
            var oEnd = new double[8];               // seam alignment scratch
            var oStart = new double[8];
            var off = new double[8];                // accumulated seam offsets

            int si = 0;
            double ct = 0, absTime = 0;
            if (Segs.Count > 0) { bt.Add(0); bx.Add(Segs[0].P0[0]); by.Add(Segs[0].P0[1]); }

            while (si < Segs.Count && t.Count < 30000000)
            {
                Seg s = Segs[si];
                s.Eval(ct, o);
                t.Add(absTime);
                for (int a = 0; a < 8; a++) pp[a].Add(o[a] + off[a]);
                sidx.Add(si);

                absTime += tick;
                ct += tick;
                while (si < Segs.Count && ct >= Segs[si].TotalT - 1e-12)
                {
                    ct -= Segs[si].TotalT;
                    if (DeGlitch && si + 1 < Segs.Count)
                    {
                        // align the next segment to continue exactly from
                        // this segment's analytic endpoint: the float32
                        // download resolution leaves ~1e-6 steps at seams
                        // that explode into accel/jerk plot spikes
                        Segs[si].Eval(Segs[si].TotalT, oEnd);
                        Segs[si + 1].Eval(0, oStart);
                        for (int a = 0; a < 8; a++) off[a] += oEnd[a] - oStart[a];
                    }
                    si++;
                    if (si < Segs.Count)
                    {
                        bt.Add(absTime - ct);      // true boundary time
                        bx.Add(Segs[si].P0[0] + off[0]);
                        by.Add(Segs[si].P0[1] + off[1]);
                    }
                }
            }

            // settle tail: hold the final position so lagging filters
            // decay to rest on-plot; mark the motion end as a boundary
            if (t.Count > 0 && tailSeconds > 0)
            {
                bt.Add(absTime - ct);
                int lastSeg = si > 0 ? si - 1 : 0;
                if (Segs.Count > 0)
                {
                    bx.Add(Segs[Segs.Count - 1].P1[0] + off[0]);
                    by.Add(Segs[Segs.Count - 1].P1[1] + off[1]);
                }
                var hold = new double[8];
                for (int a = 0; a < 8; a++) hold[a] = pp[a][pp[a].Count - 1];
                int nTail = (int)Math.Ceiling(tailSeconds / tick);
                for (int k = 0; k < nTail; k++)
                {
                    t.Add(absTime);
                    for (int a = 0; a < 8; a++) pp[a].Add(hold[a]);
                    sidx.Add(lastSeg);
                    absTime += tick;
                }
            }

            T = t.ToArray();
            for (int a = 0; a < 8; a++) P[a] = pp[a].ToArray();
            SegIdx = sidx.ToArray();
            BoundaryT = bt.ToArray();
            BoundaryX = bx.ToArray();
            BoundaryY = by.ToArray();
            SetFilter(Filter, FilterTau);          // invalidate derived data
        }

        // central difference against the REAL timestamps.  Capture logs
        // change sample rate mid-file when user threads start or exit (the
        // thread rotation is 90us per active thread - e.g. an axis init's
        // watchdog dying on a following error drops 270us samples to
        // 180us), so dividing by one average dt mis-scales whole regions
        // and prints a phantom spike at the rate change.
        // The end samples have NO central difference and get NaN (no value)
        // rather than a copy of their neighbour: a copied boundary value is
        // a fabricated derivative, and each further Diff() amplifies the
        // fabrication by 1/dt - a capture ending mid-motion (accel a at the
        // last sample) got a jerk spike of ~0.5*a/dt at its end (observed
        // -450,000 against a real jerk level of ~8,000), which then owned
        // the strip's autoscale.  NaN propagates one sample further per
        // derivative level (Vel loses 1 sample each end, Accel 2, Jerk 3)
        // and the strips skip NaN in autoscale, lines, dots and readouts.
        double[] Diff(double[] src)
        {
            int n = Math.Min(src.Length, T != null ? T.Length : 0);
            var d = new double[src.Length];
            for (int i = 0; i < src.Length; i++) d[i] = double.NaN;
            if (n < 3) return d;
            for (int i = 1; i < n - 1; i++)
            {
                double span = T[i + 1] - T[i - 1];
                d[i] = span > 0 ? (src[i + 1] - src[i - 1]) / span : double.NaN;
            }
            return d;
        }

        // the DSP's KLP smoothing: 1-pole IIR y = k*y + (1-k)*x with
        // k = exp(-dt/Tau) (Tau = -TIMEBASE/ln(KLP) on the controller).
        // The controller updates every 90us tick regardless of how the log
        // was sampled, so k is computed per sample from the REAL time step:
        // exp(-n*90us/tau) is exactly n controller ticks with the input
        // held over the span, keeping mixed-rate captures representative.
        double[] KLPFilter(double[] x, double tau)
        {
            var y = new double[x.Length];
            double s = x.Length > 0 ? x[0] : 0;
            double kDflt = Math.Exp(-Dt / Math.Max(tau, 1e-9));
            for (int i = 0; i < x.Length; i++)
            {
                double k = (i > 0 && T != null && i < T.Length && T[i] > T[i - 1])
                    ? Math.Exp(-(T[i] - T[i - 1]) / Math.Max(tau, 1e-9)) : kDflt;
                s = k * s + (1 - k) * x[i];
                y[i] = s;
            }
            return y;
        }

        // causal running-average FIR (what a cheap DSP boxcar would do)
        static double[] BoxFilter(double[] x, int N)
        {
            if (N <= 1) return x;
            var y = new double[x.Length];
            double sum = 0;
            for (int i = 0; i < x.Length; i++)
            {
                sum += x[i];
                if (i >= N) sum -= x[i - N];
                y[i] = sum / Math.Min(i + 1, N);
            }
            return y;
        }

        double[] Filt(int axis)
        {
            if (P[axis] == null) return new double[0];   // not built yet
            if (flt[axis] == null)
            {
                switch (Filter)
                {
                    case FilterKind.KLP:
                        flt[axis] = KLPFilter(P[axis], FilterTau);
                        break;
                    case FilterKind.MovAvg:
                        flt[axis] = BoxFilter(P[axis], Math.Max(1, (int)Math.Round(FilterTau / Dt)));
                        break;
                    case FilterKind.MovAvg2:
                        int n2 = Math.Max(1, (int)Math.Round(FilterTau / (2 * Dt)));
                        flt[axis] = BoxFilter(BoxFilter(P[axis], n2), n2);
                        break;
                    default:
                        flt[axis] = P[axis];
                        break;
                }
            }
            return flt[axis];
        }

        // Knot-seam de-glitch: adjacent downloaded segments agree at their
        // seam only to FLOAT precision (coefficients are sent as float32),
        // so the reconstructed position has a ~1e-6 step at every segment
        // boundary.  Differentiation amplifies that by 1/dt per derivative
        // (90us tick: a 1e-6 seam = ~1e4 jerk spike swamping a real jerk
        // level of ~10), and a smoothing filter smears it into a wide
        // bump.  The seams' PATH contribution is nanometers - pure plot
        // noise.  When enabled, BuildSamples aligns each segment to
        // continue EXACTLY from the previous segment's analytic endpoint
        // (the float offsets accumulate to well under a micron over
        // thousands of segments), so filters and all derivatives see a
        // genuinely continuous position.  REAL inter-knot jerk steps
        // (cubics have piecewise-constant jerk) are unaffected.
        public bool DeGlitch;

        public double[] Pos(int axis) { return Filt(axis); }
        public double[] Vel(int axis) { if (vel[axis] == null) vel[axis] = Diff(Filt(axis)); return vel[axis]; }
        public double[] Acc(int axis) { if (acc[axis] == null) acc[axis] = Diff(Vel(axis)); return acc[axis]; }
        public double[] Jerk(int axis) { if (jrk[axis] == null) jrk[axis] = Diff(Acc(axis)); return jrk[axis]; }

        // measured-position series (Dest/Pos captures): always raw - the
        // smoothing-filter emulation applies to the COMMANDED series only
        readonly double[][] vel2 = new double[8][];
        readonly double[][] dperr = new double[8][];
        public double[] Meas(int axis) { return P2[axis] ?? new double[0]; }
        public double[] MeasVel(int axis)
        {
            if (vel2[axis] == null) vel2[axis] = Diff(Meas(axis));
            return vel2[axis];
        }
        // commanded minus measured: the following/tracking error
        public double[] DestPosErr(int axis)
        {
            if (dperr[axis] == null)
            {
                double[] d = P[axis], m = P2[axis];
                int n = (d == null || m == null) ? 0 : Math.Min(d.Length, m.Length);
                var e = new double[n];
                for (int i = 0; i < n; i++) e[i] = d[i] - m[i];
                dperr[axis] = e;
            }
            return dperr[axis];
        }
    }

    // ------------------------------------------------------------------
    // XY path plot with zoom/pan and rubber-band region selection
    public class XYPlotPanel : Panel
    {
        public MotionLog Log;
        public bool ShowP2 = true;                  // measured-position overlay
        public int SelStart = -1, SelEnd = -1;      // selected index range
        public event EventHandler SelectionChanged;

        double cxW, cyW, scale = 100;               // world center + pixels/unit
        bool havView;
        Point panLast; bool panning;
        Point bandStart, bandEnd; bool banding;

        bool measValid; double measX, measY;        // dbl-click reference marker
        bool haveCursor; double curWX, curWY;       // cursor world position
        double pathLen; int pathLenN = -1;          // cached XY arc length

        public XYPlotPanel()
        {
            DoubleBuffered = true;
            ResizeRedraw = true;
            BackColor = Color.White;
            SetStyle(ControlStyles.Selectable, true);   // receive F/Home keys
        }

        public void FitAll()
        {
            if (Log == null || Log.Count == 0) { havView = false; Invalidate(); return; }
            double minx = double.MaxValue, maxx = -double.MaxValue;
            double miny = double.MaxValue, maxy = -double.MaxValue;
            for (int i = 0; i < Log.Count; i++)
            {
                double x = Log.P[0][i], y = Log.P[1][i];
                if (x < minx) minx = x; if (x > maxx) maxx = x;
                if (y < miny) miny = y; if (y > maxy) maxy = y;
            }
            cxW = (minx + maxx) / 2; cyW = (miny + maxy) / 2;
            double sx = maxx - minx, sy = maxy - miny;
            if (sx <= 0) sx = 1e-6; if (sy <= 0) sy = 1e-6;
            scale = 0.9 * Math.Min(Width / sx, Height / sy);
            if (scale <= 0 || double.IsInfinity(scale)) scale = 100;
            havView = true;
            Invalidate();
        }

        PointF W2S(double x, double y)
        {
            return new PointF((float)((x - cxW) * scale + Width / 2.0),
                              (float)(Height / 2.0 - (y - cyW) * scale));
        }
        double S2Wx(int px) { return (px - Width / 2.0) / scale + cxW; }
        double S2Wy(int py) { return (Height / 2.0 - py) / scale + cyW; }

        // stride so drawn chords are ~1 screen pixel: full detail zoomed
        // in, a few thousand lines at overview, automatically
        int StrideForView()
        {
            int n = Log.Count;
            if (pathLenN != n)
            {
                pathLen = 0;
                for (int i = 1; i < n; i++)
                {
                    double dx = Log.P[0][i] - Log.P[0][i - 1];
                    double dy = Log.P[1][i] - Log.P[1][i - 1];
                    pathLen += Math.Sqrt(dx * dx + dy * dy);
                }
                pathLenN = n;
            }
            double pixLen = pathLen * scale;
            int stride = pixLen > 1 ? (int)(n / pixLen) : n / 2;
            if (stride < 1) stride = 1;
            if (stride > n / 2) stride = Math.Max(1, n / 2);
            return stride;
        }

        // draw 3px dots at samples once they are >= ~8px apart on screen
        // (cheap visibility test with an early bail on dense views)
        void DrawDots(Graphics g, double[] px, double[] py, Brush b)
        {
            int n = Log.Count;
            int visible = 0;
            for (int i = 0; i < n; i++)
            {
                PointF q = W2S(px[i], py[i]);
                if (q.X >= 0 && q.X < Width && q.Y >= 0 && q.Y < Height)
                    if (++visible > 500) return;
            }
            if (visible == 0) return;
            for (int i = 0; i < n; i++)
            {
                PointF q = W2S(px[i], py[i]);
                if (q.X >= -2 && q.X < Width + 2 && q.Y >= -2 && q.Y < Height + 2)
                    g.FillRectangle(b, q.X - 1.5f, q.Y - 1.5f, 3f, 3f);
            }
        }

        // clip segment a-b to the slightly inflated viewport (Liang-Barsky).
        // GDI+ overflows on coordinates beyond ~2^30, so at extreme zoom
        // segments must be clipped ALONG the line (not just culled) to keep
        // the path visible while every drawn coordinate stays small.
        bool ClipToView(ref PointF a, ref PointF b)
        {
            double x0 = a.X, y0 = a.Y, x1 = b.X, y1 = b.Y;
            if (double.IsNaN(x0 + y0 + x1 + y1) || double.IsInfinity(x0 + y0 + x1 + y1)) return false;
            double xmin = -50, ymin = -50, xmax = Width + 50, ymax = Height + 50;
            double dx = x1 - x0, dy = y1 - y0;
            double t0 = 0, t1 = 1;
            for (int i = 0; i < 4; i++)
            {
                double p = i == 0 ? -dx : i == 1 ? dx : i == 2 ? -dy : dy;
                double q = i == 0 ? x0 - xmin : i == 1 ? xmax - x0 : i == 2 ? y0 - ymin : ymax - y0;
                if (p == 0) { if (q < 0) return false; }
                else
                {
                    double r = q / p;
                    if (p < 0) { if (r > t1) return false; if (r > t0) t0 = r; }
                    else       { if (r < t0) return false; if (r < t1) t1 = r; }
                }
            }
            a = new PointF((float)(x0 + t0 * dx), (float)(y0 + t0 * dy));
            b = new PointF((float)(x0 + t1 * dx), (float)(y0 + t1 * dy));
            return true;
        }

        void DrawPath(Graphics g, double[] px, double[] py, int stride, Pen pen, Pen selPen)
        {
            int n = Log.Count;
            PointF prev = W2S(px[0], py[0]);
            for (int i = stride; i < n; i += stride)
            {
                PointF cur = W2S(px[i], py[i]);
                PointF a = prev, b = cur;
                if (ClipToView(ref a, ref b))
                {
                    bool inSel = selPen != null && SelStart >= 0 && i >= SelStart && i <= SelEnd;
                    g.DrawLine(inSel ? selPen : pen, a, b);
                }
                prev = cur;
            }
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            var g = e.Graphics;
            g.SmoothingMode = SmoothingMode.HighSpeed;
            if (Log == null || Log.Count < 2) { g.DrawString("File > Open a TPSegLog.csv", Font, Brushes.Gray, 10, 10); return; }
            if (!havView) FitAll();

            int stride = StrideForView();

            using (var pen = new Pen(Color.RoyalBlue, 1f))
            using (var selPen = new Pen(Color.Red, 1.6f))
            using (var fpen = new Pen(Color.DarkMagenta, 1f))
            using (var mpen = new Pen(Color.Crimson, 1f))
            {
                // unfiltered (as-sent) path, selection highlighted
                DrawPath(g, Log.P[0], Log.P[1], stride, pen, selPen);

                // filtered path overlay (only when a filter is active).
                // Note the two paths mostly coincide except where the
                // filter cuts corners: lag displaces points ALONG the
                // path and stopped corners have ~zero filter error.
                if (Log.Filter != MotionLog.FilterKind.None)
                    DrawPath(g, Log.Pos(0), Log.Pos(1), stride, fpen, null);

                // measured-position overlay (Dest/Pos capture logs) -
                // axes 0/1 in actuator units
                bool p2ok = ShowP2 && Log.HasP2
                            && Log.P2[0] != null && Log.P2[0].Length >= Log.Count
                            && Log.P2[1] != null && Log.P2[1].Length >= Log.Count;
                if (p2ok)
                    DrawPath(g, Log.P2[0], Log.P2[1], stride, mpen, null);

                // sample-point dots once they resolve: proves the drawn
                // lines are straight chords between REAL samples (the
                // viewer never curve-fits or smooths)
                DrawDots(g, Log.P[0], Log.P[1], Brushes.RoyalBlue);
                if (Log.Filter != MotionLog.FilterKind.None)
                    DrawDots(g, Log.Pos(0), Log.Pos(1), Brushes.DarkMagenta);
                if (p2ok)
                    DrawDots(g, Log.P2[0], Log.P2[1], Brushes.Crimson);
            }

            // origin cross (only when near the view: huge coords overflow GDI+)
            PointF o = W2S(0, 0);
            if (o.X > -10 && o.X < Width + 10 && o.Y > -10 && o.Y < Height + 10)
            {
                g.DrawLine(Pens.LightGray, o.X - 6, o.Y, o.X + 6, o.Y);
                g.DrawLine(Pens.LightGray, o.X, o.Y - 6, o.X, o.Y + 6);
            }

            // segment-boundary dots (segment-format logs), once resolvable
            if (Log.BoundaryX != null && Log.BoundaryX.Length > 0)
            {
                int visible = 0;
                for (int b = 0; b < Log.BoundaryX.Length && visible <= 4000; b++)
                {
                    PointF q = W2S(Log.BoundaryX[b], Log.BoundaryY[b]);
                    if (q.X >= 0 && q.X < Width && q.Y >= 0 && q.Y < Height) visible++;
                }
                if (visible > 0 && visible <= 4000)
                {
                    using (var bb = new SolidBrush(Color.DarkOrange))
                        for (int b = 0; b < Log.BoundaryX.Length; b++)
                        {
                            PointF q = W2S(Log.BoundaryX[b], Log.BoundaryY[b]);
                            if (q.X >= -2 && q.X < Width + 2 && q.Y >= -2 && q.Y < Height + 2)
                                g.FillRectangle(bb, q.X - 1.5f, q.Y - 1.5f, 3f, 3f);
                        }
                }
            }

            // measure marker: red X at the double-clicked reference
            if (measValid)
            {
                PointF m = W2S(measX, measY);
                if (m.X > -10 && m.X < Width + 10 && m.Y > -10 && m.Y < Height + 10)
                    using (var mp = new Pen(Color.Red, 2f))
                    {
                        g.DrawLine(mp, m.X - 7, m.Y - 7, m.X + 7, m.Y + 7);
                        g.DrawLine(mp, m.X - 7, m.Y + 7, m.X + 7, m.Y - 7);
                    }
            }

            if (banding)
            {
                var r = RectFromPoints(bandStart, bandEnd);
                using (var bp = new Pen(Color.DarkOrange) { DashStyle = DashStyle.Dash })
                    g.DrawRectangle(bp, r);
            }

            // readout: cursor position, measure delta, selection, hints
            string pos = haveCursor
                ? string.Format("x = {0}  y = {1}", Util.Fmt(curWX), Util.Fmt(curWY)) : "";
            if (measValid && haveCursor)
            {
                double dx = curWX - measX, dy = curWY - measY;
                pos += string.Format("   ref = {0},{1}  del = {2},{3}  len = {4}",
                    Util.Fmt(measX), Util.Fmt(measY), Util.Fmt(dx), Util.Fmt(dy),
                    Util.Fmt(Math.Sqrt(dx * dx + dy * dy)));
            }
            string info = SelStart >= 0
                ? string.Format("selected t = {0:0.###} .. {1:0.###} s  ({2} pts)", Log.T[SelStart], Log.T[SelEnd], SelEnd - SelStart + 1)
                : "right-drag select | dbl-click marker | mid-click/F fit";
            g.DrawString(pos, Font, Brushes.Black, 4, Height - 34);
            g.DrawString(info, Font, Brushes.DimGray, 4, Height - 18);
        }

        static Rectangle RectFromPoints(Point a, Point b)
        {
            return new Rectangle(Math.Min(a.X, b.X), Math.Min(a.Y, b.Y),
                                 Math.Abs(a.X - b.X), Math.Abs(a.Y - b.Y));
        }

        protected override void OnMouseWheel(MouseEventArgs e)
        {
            base.OnMouseWheel(e);
            if (!havView) return;
            double f = Math.Exp(e.Delta / 900.0);
            double wx = S2Wx(e.X), wy = S2Wy(e.Y);
            scale *= f;
            // keep the world point under the cursor fixed
            cxW = wx - (e.X - Width / 2.0) / scale;
            cyW = wy + (e.Y - Height / 2.0) / scale;
            Invalidate();
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);
            Focus();
            if (e.Button == MouseButtons.Left) { panning = true; panLast = e.Location; }
            else if (e.Button == MouseButtons.Right) { banding = true; bandStart = bandEnd = e.Location; }
            else if (e.Button == MouseButtons.Middle) FitAll();
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);
            haveCursor = true;
            curWX = S2Wx(e.X);
            curWY = S2Wy(e.Y);
            if (panning)
            {
                cxW -= (e.X - panLast.X) / scale;
                cyW += (e.Y - panLast.Y) / scale;
                panLast = e.Location;
            }
            else if (banding)
            {
                bandEnd = e.Location;
            }
            Invalidate();
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            base.OnMouseUp(e);
            if (e.Button == MouseButtons.Left) panning = false;
            else if (e.Button == MouseButtons.Right && banding)
            {
                banding = false;
                var r = RectFromPoints(bandStart, bandEnd);
                if (r.Width < 4 && r.Height < 4)
                {
                    SelStart = SelEnd = -1;          // simple right-click clears
                }
                else if (Log != null)
                {
                    double x0 = S2Wx(r.Left), x1 = S2Wx(r.Right);
                    double y0 = S2Wy(r.Bottom), y1 = S2Wy(r.Top);
                    int lo = -1, hi = -1;
                    for (int i = 0; i < Log.Count; i++)
                    {
                        double x = Log.P[0][i], y = Log.P[1][i];
                        if (x >= x0 && x <= x1 && y >= y0 && y <= y1)
                        {
                            if (lo < 0) lo = i;
                            hi = i;
                        }
                    }
                    SelStart = lo; SelEnd = hi;
                }
                Invalidate();
                if (SelectionChanged != null) SelectionChanged(this, EventArgs.Empty);
            }
        }

        public MarkState Mark;                       // shared with the strips

        // re-derive the marker's world position from the shared sample
        // index (set from a time strip, or after a reload)
        public void SyncMark()
        {
            if (Mark == null || Mark.Idx < 0 || Log == null || Mark.Idx >= Log.Count)
            {
                measValid = false;
                return;
            }
            measX = Log.P[0][Mark.Idx];
            measY = Log.P[1][Mark.Idx];
            measValid = true;
        }

        // double-click sets the measure reference, SNAPPED to the nearest
        // sample of the path - the same sample is marked on every time
        // strip; double-clicking on/near the marker clears it.
        // Fit-all is middle-click or the F / Home keys.
        protected override void OnMouseDoubleClick(MouseEventArgs e)
        {
            base.OnMouseDoubleClick(e);
            if (e.Button != MouseButtons.Left || Log == null || Log.Count == 0) return;
            double x = S2Wx(e.X), y = S2Wy(e.Y);
            double tol = 8.0 / scale;                // ~8 pixels in world units
            if (measValid && Math.Abs(x - measX) < tol && Math.Abs(y - measY) < tol)
            {
                measValid = false;
                if (Mark != null) Mark.Set(-1);
            }
            else
            {
                // closest path sample to the click (world-space)
                int best = 0;
                double bd = double.MaxValue;
                for (int i = 0; i < Log.Count; i++)
                {
                    double dx = Log.P[0][i] - x, dy = Log.P[1][i] - y;
                    double d = dx * dx + dy * dy;
                    if (d < bd) { bd = d; best = i; }
                }
                measX = Log.P[0][best]; measY = Log.P[1][best];
                measValid = true;
                if (Mark != null) Mark.Set(best);
            }
            Invalidate();
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            base.OnKeyDown(e);
            if (e.KeyCode == Keys.F || e.KeyCode == Keys.Home) FitAll();
        }
    }

    // ------------------------------------------------------------------
    // shared time-axis view so all strip charts zoom/pan together
    public class TimeView
    {
        public double T0, T1;                        // visible time range
        public event EventHandler Changed;
        public void Set(double t0, double t1)
        {
            if (t1 - t0 < 1e-6) t1 = t0 + 1e-6;
            T0 = t0; T1 = t1;
            if (Changed != null) Changed(this, EventArgs.Empty);
        }
    }

    // shared sample marker: the same log sample shown as the X mark in
    // the XY plot and a vertical line in every time strip - set from
    // EITHER side by double-click (-1 = no marker)
    public class MarkState
    {
        public int Idx = -1;
        public event EventHandler Changed;
        public void Set(int i)
        {
            Idx = i;
            if (Changed != null) Changed(this, EventArgs.Empty);
        }
    }

    // shared cursor sample: the strip under the cursor publishes the
    // sample its cursor is over and EVERY strip reads out that same
    // sample, so the P/V/A/J/E readouts always refer to one instant.
    // Active = the cursor is inside some strip right now; while it is
    // not, the readouts fall back to the marker (or, with nothing
    // marked, stay at the last cursor sample).
    public class HoverState
    {
        public int Idx = -1;
        public bool Active;
        public event EventHandler Changed;

        public void Set(int i)
        {
            if (Active && Idx == i) return;         // no change: no repaint
            Idx = i; Active = true;
            Fire();
        }

        public void Leave()
        {
            if (!Active) return;
            Active = false;
            Fire();
        }

        public void Reset()                          // sample indices are new
        {
            Idx = -1; Active = false;
            Fire();
        }

        void Fire() { if (Changed != null) Changed(this, EventArgs.Empty); }
    }

    // one strip chart: a data series vs time
    public class TimePlotPanel : Panel
    {
        public MotionLog Log;
        public TimeView View;
        public string Title = "";
        public Func<double[]> Series;                // data for current axis
        public Func<double[]> Series2;               // optional second series
                                                     // (measured Position),
                                                     // drawn in crimson
        public string ValName = "v";                 // readout letter for Series
        public string Val2Name = "v2";               // readout letter for Series2
        public MarkState Mark;                       // shared sample marker
        public HoverState Hover;                     // shared cursor sample
        public int SelStart = -1, SelEnd = -1;       // limits data drawn
        Point panLast; bool panning;

        public TimePlotPanel()
        {
            DoubleBuffered = true;
            ResizeRedraw = true;
            BackColor = Color.White;
        }

        // the sample the readout shows: the cursor's while the cursor is
        // inside any strip, else the marker's when one is set, else the
        // last cursor sample.  -1 = nothing to show.
        int ReadoutIdx()
        {
            if (Log == null || Log.Count == 0) return -1;
            int i = -1;
            if (Hover != null && Hover.Active) i = Hover.Idx;
            else if (Mark != null && Mark.Idx >= 0) i = Mark.Idx;
            else if (Hover != null) i = Hover.Idx;
            return (i >= 0 && i < Log.Count) ? i : -1;
        }

        // true when the readout is showing the marked sample (cursor
        // outside the strips): drawn in the marker's red as the cue
        bool ReadoutAtMark(int i)
        {
            return i >= 0 && Mark != null && i == Mark.Idx && !(Hover != null && Hover.Active);
        }

        string Readout(int i)
        {
            if (i < 0 || Series == null) return "";
            double[] d = Series();
            if (i >= d.Length) return "";
            string seg = (Log.SegIdx != null && i < Log.SegIdx.Length)
                ? string.Format("  seg={0}", Log.SegIdx[i]) : "";
            string r = string.Format("t={0:0.#####}  {1}={2}{3}", Log.T[i], ValName, Util.Fmt(d[i]), seg);
            if (Series2 != null)
            {
                double[] d2 = Series2();
                if (i < d2.Length)
                    r += string.Format("  {0}={1}  {2}-{0}={3}",
                        Val2Name, Util.Fmt(d2[i]), ValName, Util.Fmt(d[i] - d2[i]));
            }
            return r;
        }

        // nearest sample to time t by BINARY SEARCH of the real time array.
        // Sampled capture logs are not reliably uniform (missed capture
        // slices leave gaps), so arithmetic index-by-Dt drifts late - the
        // marker landed right of the click and zoomed views went blank on
        // the left (samples between View.T0 and the overshot index were
        // never drawn).
        int IdxOfT(double t)
        {
            if (Log == null || Log.Count == 0) return 0;
            int lo = 0, hi = Log.Count - 1;
            if (t <= Log.T[0]) return 0;
            if (t >= Log.T[hi]) return hi;
            while (hi - lo > 1)
            {
                int mid = (lo + hi) >> 1;
                if (Log.T[mid] <= t) lo = mid; else hi = mid;
            }
            return (t - Log.T[lo] <= Log.T[hi] - t) ? lo : hi;
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            var g = e.Graphics;
            g.DrawRectangle(Pens.Silver, 0, 0, Width - 1, Height - 1);
            g.DrawString(Title, Font, Brushes.Black, 4, 2);
            int ri = ReadoutIdx();
            string readout = Readout(ri);
            if (readout.Length > 0)
            {
                float rx = 4 + g.MeasureString(Title, Font).Width + 8;
                g.DrawString(readout, Font, ReadoutAtMark(ri) ? Brushes.Red : Brushes.Black, rx, 2);
            }
            if (Log == null || Log.Count < 2 || View == null || Series == null) return;

            double[] d = Series();
            double[] d2 = Series2 != null ? Series2() : null;
            if (d2 != null && d2.Length < Log.Count) d2 = null;
            // one extra sample each side so the chords entering/leaving the
            // window are drawn to the edges instead of stopping short
            int i0 = Math.Max(0, IdxOfT(View.T0) - 1);
            int i1 = Math.Min(Log.Count - 1, IdxOfT(View.T1) + 1);
            if (SelStart >= 0) { i0 = Math.Max(i0, SelStart); i1 = Math.Min(i1, SelEnd); }
            if (i1 <= i0) return;

            // y autoscale over the visible span (both series).  NaN samples
            // (derivative boundaries) fail both comparisons and so are
            // skipped; a span with no finite sample at all draws nothing.
            double mn = double.MaxValue, mx = -double.MaxValue;
            for (int i = i0; i <= i1; i++) { if (d[i] < mn) mn = d[i]; if (d[i] > mx) mx = d[i]; }
            if (d2 != null)
                for (int i = i0; i <= i1; i++) { if (d2[i] < mn) mn = d2[i]; if (d2[i] > mx) mx = d2[i]; }
            if (mn > mx) return;
            if (mx - mn < 1e-12) { mx += 1; mn -= 1; }
            double pad = (mx - mn) * 0.08;
            mn -= pad; mx += pad;

            int top = 18, bot = 4;
            double sx = Width / (View.T1 - View.T0);
            double sy = (Height - top - bot) / (mx - mn);

            // zero line
            if (mn < 0 && mx > 0)
            {
                int zy = (int)(Height - bot - (0 - mn) * sy);
                g.DrawLine(Pens.Gainsboro, 0, zy, Width, zy);
            }

            // segment-boundary markers: vertical ticks at each segment's
            // true start time, drawn only once they resolve visually
            if (Log.BoundaryT != null)
            {
                var bt = Log.BoundaryT;
                int lo = Array.BinarySearch(bt, View.T0); if (lo < 0) lo = ~lo;
                int hi = Array.BinarySearch(bt, View.T1); if (hi < 0) hi = ~hi;
                if (hi - lo > 0 && hi - lo < Width / 6)
                {
                    using (var bp = new Pen(Color.FromArgb(70, Color.DarkOrange)))
                        for (int b = lo; b < hi; b++)
                        {
                            float x = (float)((bt[b] - View.T0) * sx);
                            g.DrawLine(bp, x, top, x, Height - bot);
                        }
                }
            }

            int stride = Math.Max(1, (i1 - i0) / (Width * 4));
            using (var pen = new Pen(Color.ForestGreen, 1.2f))
            using (var pen2 = new Pen(Color.Crimson, 1.2f))
            {
                // NaN (no value) breaks the polyline instead of being drawn
                float px = 0, py = 0; bool first = true;
                for (int i = i0; i <= i1; i += stride)
                {
                    if (double.IsNaN(d[i])) { first = true; continue; }
                    float x = (float)((Log.T[i] - View.T0) * sx);
                    float y = (float)(Height - bot - (d[i] - mn) * sy);
                    if (!first) g.DrawLine(pen, px, py, x, y);
                    px = x; py = y; first = false;
                }
                if (d2 != null)
                {
                    first = true;
                    for (int i = i0; i <= i1; i += stride)
                    {
                        if (double.IsNaN(d2[i])) { first = true; continue; }
                        float x = (float)((Log.T[i] - View.T0) * sx);
                        float y = (float)(Height - bot - (d2[i] - mn) * sy);
                        if (!first) g.DrawLine(pen2, px, py, x, y);
                        px = x; py = y; first = false;
                    }
                }
            }
            // sample dots once they resolve (no smoothing anywhere: lines
            // are straight chords between real samples)
            if (i1 - i0 < Width / 8)
            {
                for (int i = i0; i <= i1; i++)
                {
                    float x = (float)((Log.T[i] - View.T0) * sx);
                    if (!double.IsNaN(d[i]))
                    {
                        float y = (float)(Height - bot - (d[i] - mn) * sy);
                        g.FillRectangle(Brushes.ForestGreen, x - 1.5f, y - 1.5f, 3f, 3f);
                    }
                    if (d2 != null && !double.IsNaN(d2[i]))
                    {
                        float y2 = (float)(Height - bot - (d2[i] - mn) * sy);
                        g.FillRectangle(Brushes.Crimson, x - 1.5f, y2 - 1.5f, 3f, 3f);
                    }
                }
            }

            // shared sample marker (double-click here or in the XY plot):
            // vertical red line + value dot at the marked sample
            if (Mark != null && Mark.Idx >= 0 && Mark.Idx < Log.Count)
            {
                double tm = Log.T[Mark.Idx];
                if (tm >= View.T0 && tm <= View.T1)
                {
                    float x = (float)((tm - View.T0) * sx);
                    using (var mp = new Pen(Color.Red, 1.5f))
                        g.DrawLine(mp, x, top, x, Height - bot);
                    if (Mark.Idx < d.Length && !double.IsNaN(d[Mark.Idx]))
                    {
                        float y = (float)(Height - bot - (d[Mark.Idx] - mn) * sy);
                        g.FillEllipse(Brushes.Red, x - 3f, y - 3f, 6f, 6f);
                    }
                }
            }

            g.DrawString(Util.Fmt(mx), Font, Brushes.Gray, Width - 90, top);
            g.DrawString(Util.Fmt(mn), Font, Brushes.Gray, Width - 90, Height - 18);
        }

        protected override void OnMouseWheel(MouseEventArgs e)
        {
            base.OnMouseWheel(e);
            if (View == null) return;
            double f = Math.Exp(-e.Delta / 900.0);
            double tCur = View.T0 + (View.T1 - View.T0) * e.X / Math.Max(1, Width);
            double t0 = tCur - (tCur - View.T0) * f;
            double t1 = tCur + (View.T1 - tCur) * f;
            View.Set(t0, t1);
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);
            Focus();
            if (e.Button == MouseButtons.Left) { panning = true; panLast = e.Location; }
            else if (e.Button == MouseButtons.Middle) FitTime();
        }

        // zoom full out (to the selection if one is active): middle-click
        // or the F / Home keys - double-click is the shared marker
        public void FitTime()
        {
            if (Log == null || Log.Count == 0 || View == null) return;
            int i0 = SelStart >= 0 ? SelStart : 0;
            int i1 = SelEnd >= 0 ? SelEnd : Log.Count - 1;
            View.Set(Log.T[i0], Log.T[i1]);
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);
            if (View == null || Log == null || Log.Count == 0) return;
            if (panning)
            {
                double dt = (panLast.X - e.X) * (View.T1 - View.T0) / Math.Max(1, Width);
                View.Set(View.T0 + dt, View.T1 + dt);
                panLast = e.Location;
            }
            // publish the cursor's sample: every strip (this one included)
            // repaints its readout at that sample via the shared state
            int i = IdxOfT(View.T0 + (View.T1 - View.T0) * e.X / Math.Max(1, Width));
            if (Hover != null) Hover.Set(i); else Invalidate();
        }

        protected override void OnMouseLeave(EventArgs e)
        {
            base.OnMouseLeave(e);
            if (Hover != null) Hover.Leave();       // readouts fall back to the marker
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            base.OnMouseUp(e);
            panning = false;
        }

        // double-click places the shared sample marker at that time (the
        // XY plot X mark follows); clicking on/near the marker clears it.
        // Fit-all is middle-click or the F / Home keys, matching the XY plot.
        protected override void OnMouseDoubleClick(MouseEventArgs e)
        {
            base.OnMouseDoubleClick(e);
            if (Log == null || Log.Count == 0 || View == null || Mark == null) return;
            if (Mark.Idx >= 0 && Mark.Idx < Log.Count)
            {
                float mxp = (float)((Log.T[Mark.Idx] - View.T0) * Width / (View.T1 - View.T0));
                if (Math.Abs(mxp - e.X) < 8) { Mark.Set(-1); return; }
            }
            Mark.Set(IdxOfT(View.T0 + (View.T1 - View.T0) * e.X / Math.Max(1, Width)));
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            base.OnKeyDown(e);
            if (e.KeyCode == Keys.F || e.KeyCode == Keys.Home) FitTime();
        }
    }

    // ------------------------------------------------------------------
    public class MainForm : Form
    {
        MotionLog log;
        readonly XYPlotPanel xy = new XYPlotPanel();
        readonly TimePlotPanel[] strips = new TimePlotPanel[5];
        // real checkboxes (hosted in the menu bar) so the on/off state is
        // always visible at a glance
        readonly CheckBox compDelayItem = new CheckBox { Text = "Comp Delay", AutoSize = true };
        readonly CheckBox showPosItem = new CheckBox { Text = "Show Pos", AutoSize = true, Checked = true };
        readonly CheckBox deGlitchItem = new CheckBox { Text = "Remove Knot Seams", AutoSize = true, Checked = true };
        SplitContainer split;
        Form helpForm;                    // modeless Help window (one instance)

        // ---- persisted settings (simple key=value file in %AppData%) ----
        static string CfgPath
        {
            get
            {
                return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData),
                    "MotionLogPlotter.cfg");
            }
        }

        static Dictionary<string, string> LoadCfg()
        {
            var d = new Dictionary<string, string>();
            try
            {
                if (File.Exists(CfgPath))
                    foreach (var line in File.ReadAllLines(CfgPath))
                    {
                        int eq = line.IndexOf('=');
                        if (eq > 0) d[line.Substring(0, eq)] = line.Substring(eq + 1);
                    }
            }
            catch { }
            return d;
        }

        static int CfgInt(Dictionary<string, string> d, string k, int def)
        {
            string s; int v;
            return d.TryGetValue(k, out s) && int.TryParse(s, out v) ? v : def;
        }

        void SaveCfg()
        {
            try
            {
                var b = WindowState == FormWindowState.Normal ? Bounds : RestoreBounds;
                var lines = new List<string>
                {
                    "WinX=" + b.X, "WinY=" + b.Y, "WinW=" + b.Width, "WinH=" + b.Height,
                    "Maximized=" + (WindowState == FormWindowState.Maximized ? 1 : 0),
                    "Split=" + split.SplitterDistance,
                    "Axis=" + axisBox.SelectedIndex,
                    "Filter=" + filterBox.SelectedIndex,
                    "Tau=" + tauBox.Text,
                    "Tick=" + tickBox.Text,
                    "CompDelay=" + (compDelayItem.Checked ? 1 : 0),
                    "ShowPos=" + (showPosItem.Checked ? 1 : 0),
                    "DeGlitch=" + (deGlitchItem.Checked ? 1 : 0)
                };
                File.WriteAllLines(CfgPath, lines.ToArray());
            }
            catch { }
        }
        readonly TimeView tview = new TimeView();
        readonly MarkState mark = new MarkState();
        readonly HoverState hover = new HoverState();
        readonly ComboBox axisBox = new ComboBox();
        readonly ComboBox filterBox = new ComboBox();
        readonly TextBox tauBox = new TextBox();
        readonly TextBox tickBox = new TextBox();
        readonly StatusStrip status = new StatusStrip();
        readonly ToolStripStatusLabel statusLabel = new ToolStripStatusLabel();

        // ---- Help: keys, commands and features (Help menu or F1) ----

        const string HelpText =
@"MOTION LOG PLOTTER

Files (File menu)
  Open   Ctrl+O    TPSegLog.csv segment log (written to c:\Temp when the
                   ""Log"" option on the Trajectory Planner screen is on)
                   or a sampled CSV such as DestPosLog.csv from
                   CaptureXYZPosDestToFile.c.  The file opens fine even
                   while KMotionCNC still holds it (mid-run or abort).
  Reload F5        re-read the same file after another run.

XY path plot (left)
  mouse wheel      zoom about the cursor
  left-drag        pan
  right-DRAG       select the region the time plots graph
  right-CLICK      clear the selection (time plots show the whole log)
  double-click     place the measure marker, snapped to the nearest path
                   sample; double-click on the marker clears it
  middle-click     zoom full out (also F or Home)
  readout          cursor x,y; with a marker set: ref, del, len

Time plots (right)
  mouse wheel      zoom time about the cursor - all strips stay in sync
  left-drag        pan
  double-click     place/clear the shared sample marker (X mark in XY)
  middle-click     zoom full out to the selection (also F or Home)
  readout          every strip shows the SAME sample: the one under the
                   cursor while it is inside any strip; with the cursor
                   outside the strips, the marked sample (readout in
                   red) if a marker is set, else the last cursor sample

Readout letters
  t    time, seconds              D    commanded Dest
  P    measured Position          D-P  following error
  V    velocity (of Dest)         Vm   measured velocity (of Pos)
  A    accel                      J    jerk
  E    bottom strip value: Dest - Pos, or Filter Err
  seg  segment index under the cursor (segment logs)

Menu bar
  Axis             which axis the time plots show
  Filter           smoothing emulation applied before the derivatives:
                   KLP IIR = the DSP's 1-pole low pass, Mov Avg (x2) =
                   candidate cheap FIR replacements
  Tau/Window s     filter time constant / window length, seconds
  Tick s           servo tick used to re-create the controller's
                   interpolation from segment logs (0.00009 = 90us)
  Comp Delay       Filter Err strip: subtract the filter's group delay
                   so the error shows shape distortion, not pure lag
  Show Pos         show/hide the crimson measured-position overlays
  Remove Knot Seams  segment logs: remove float32 knot-seam steps

Colors and marks
  green = commanded (Dest)        crimson = measured (Pos)
  orange ticks = segment starts (drawn once they resolve visually)
  red line / X = the shared sample marker - the SAME log sample in the
  XY plot and every strip.  Sample dots appear when zoomed in enough;
  lines are straight chords between real samples - nothing is smoothed
  for display.

Window size, splitter, axis, filter and checkbox settings persist in
%AppData%\MotionLogPlotter.cfg.";

        void OnHelp(object sender, EventArgs e)
        {
            if (helpForm != null && !helpForm.IsDisposed) { helpForm.Activate(); return; }

            helpForm = new Form
            {
                Text = "Motion Log Plotter Help",
                Width = 640, Height = 760,
                StartPosition = FormStartPosition.CenterParent,
                ShowIcon = false, MinimizeBox = false, MaximizeBox = false,
                KeyPreview = true
            };
            var tb = new TextBox
            {
                Multiline = true, ReadOnly = true, WordWrap = false,
                Dock = DockStyle.Fill, ScrollBars = ScrollBars.Both,
                BackColor = Color.White,
                Font = new Font(FontFamily.GenericMonospace, 9f),
                Text = HelpText.Replace("\r\n", "\n").Replace("\n", "\r\n")
            };
            helpForm.Controls.Add(tb);
            helpForm.KeyDown += delegate (object sh, KeyEventArgs ke)
                { if (ke.KeyCode == Keys.Escape) helpForm.Close(); };
            helpForm.Shown += delegate { tb.Select(0, 0); };  // no select-all
            helpForm.Show(this);          // modeless: try gestures while reading
        }

        double CurTick()
        {
            double t;
            if (!double.TryParse(tickBox.Text, NumberStyles.Float, CultureInfo.InvariantCulture, out t) || t < 1e-6)
                t = 0.00009;
            return t;
        }

        void OnTickChanged()
        {
            if (log == null || log.Segs == null) return;
            log.BuildSamples(CurTick());
            xy.SelStart = xy.SelEnd = -1;
            mark.Set(-1);           // resample invalidates sample indices
            hover.Reset();
            ApplyFilter();
            OnSelection();
            UpdateStatus();
        }

        // settle time to append after motion so the active filter's lag
        // visibly decays to rest on-plot
        double TailForFilter(double tau)
        {
            double tail;
            switch (filterBox.SelectedIndex)
            {
                case 1:  tail = 6.0 * tau; break;   // KLP: ~6 time constants
                case 2:
                case 3:  tail = 1.5 * tau; break;   // boxcars: window + margin
                default: tail = 0.02; break;        // room for the derivatives
            }
            if (tail < 0.02) tail = 0.02;
            if (tail > 5.0) tail = 5.0;
            return tail;
        }

        void ApplyFilter()
        {
            if (log == null) return;
            double tau;
            if (!double.TryParse(tauBox.Text, NumberStyles.Float, CultureInfo.InvariantCulture, out tau) || tau <= 0)
                tau = 0.01;

            // extend/shrink the settle tail to suit the filter (indices
            // of existing samples stay valid - the tail only appends)
            double tail = TailForFilter(tau);
            bool rebuilt = false;
            if (log.Segs != null && Math.Abs(tail - log.BuiltTail) > 1e-9)
            {
                log.BuildSamples(CurTick(), tail);
                rebuilt = true;
            }

            log.SetFilter((MotionLog.FilterKind)filterBox.SelectedIndex, tau, compDelayItem.Checked);
            UpdateStrips();
            if (rebuilt)
            {
                // the rebuild only appends/trims the settle tail at the
                // END - preserve the user's zoom, clamping only if the
                // view now extends past the data
                double tEnd = log.Count > 0 ? log.T[log.Count - 1] : 1e-6;
                double t0 = Math.Min(tview.T0, tEnd - 1e-6);
                double t1 = Math.Min(tview.T1, tEnd);
                if (t1 <= t0) { t0 = log.Count > 0 ? log.T[0] : 0; t1 = tEnd; }
                tview.Set(t0, t1);
            }
            xy.Invalidate();        // filtered overlay in the XY plot
        }

        public MainForm(string fileArg)
        {
            Text = "Motion Log Plotter";

            // <ApplicationIcon> in the .csproj only gives the .exe its icon in
            // Explorer.  A Form's title bar, taskbar button and Alt-Tab entry
            // come from Form.Icon, which otherwise stays the default WinForms
            // one; MotionLogPlotter.ico is embedded for exactly this.
            using (var s = typeof(MainForm).Assembly.GetManifestResourceStream("MotionLogPlotter.ico"))
            {
                if (s != null) Icon = new Icon(s);
            }

            Width = 1200; Height = 800;

            var menu = new MenuStrip();
            var mFile = new ToolStripMenuItem("&File");
            var mOpen = new ToolStripMenuItem("&Open...", null, OnOpen) { ShortcutKeys = Keys.Control | Keys.O };
            var mReload = new ToolStripMenuItem("&Reload", null, OnReload) { ShortcutKeys = Keys.F5 };
            mFile.DropDownItems.Add(mOpen);
            mFile.DropDownItems.Add(mReload);
            menu.Items.Add(mFile);

            axisBox.DropDownStyle = ComboBoxStyle.DropDownList;
            axisBox.Items.AddRange(MotionLog.AxisNames);
            axisBox.SelectedIndex = 0;
            axisBox.SelectedIndexChanged += delegate { UpdateStrips(); };
            var axisHost = new ToolStripControlHost(axisBox) { Margin = new Padding(20, 1, 2, 1) };
            menu.Items.Add(new ToolStripLabel("Axis:"));
            menu.Items.Add(axisHost);

            // controller smoothing-filter emulation applied to positions
            // before differentiating (KLP = the DSP's 1-pole IIR; the
            // moving averages are candidate cheap FIR replacements)
            filterBox.DropDownStyle = ComboBoxStyle.DropDownList;
            filterBox.Items.AddRange(new object[] { "None", "KLP IIR", "Mov Avg", "Mov Avg x2" });
            filterBox.SelectedIndex = 0;
            filterBox.SelectedIndexChanged += delegate { ApplyFilter(); };
            tauBox.Text = "0.010";
            tauBox.Width = 60;
            tauBox.KeyDown += delegate (object s2, KeyEventArgs ke)
                { if (ke.KeyCode == Keys.Enter) { ApplyFilter(); ke.SuppressKeyPress = true; } };
            tauBox.Leave += delegate { ApplyFilter(); };
            menu.Items.Add(new ToolStripLabel("   Filter:"));
            menu.Items.Add(new ToolStripControlHost(filterBox) { Margin = new Padding(2, 1, 2, 1) });
            menu.Items.Add(new ToolStripLabel("Tau/Window s:"));
            menu.Items.Add(new ToolStripControlHost(tauBox) { Margin = new Padding(2, 1, 2, 1) });

            // servo tick used to re-create the controller interpolation
            // from segment-format logs (DoTrajectorys leftover-carry)
            tickBox.Text = "0.00009";
            tickBox.Width = 70;
            tickBox.KeyDown += delegate (object s3, KeyEventArgs ke)
                { if (ke.KeyCode == Keys.Enter) { OnTickChanged(); ke.SuppressKeyPress = true; } };
            tickBox.Leave += delegate { OnTickChanged(); };
            menu.Items.Add(new ToolStripLabel("Tick s:"));
            menu.Items.Add(new ToolStripControlHost(tickBox) { Margin = new Padding(2, 1, 2, 1) });

            // Filter Err strip option: subtract the filter's group delay
            // so the error shows shape distortion instead of pure lag
            compDelayItem.CheckedChanged += delegate { ApplyFilter(); };
            menu.Items.Add(new ToolStripControlHost(compDelayItem) { Margin = new Padding(12, 2, 2, 2) });

            // show/hide the measured-position (crimson) overlays of
            // Dest/Pos capture logs; the Dest - Pos error strip stays
            showPosItem.CheckedChanged += delegate
            {
                xy.ShowP2 = showPosItem.Checked;
                UpdateStrips();
                xy.Invalidate();
            };
            menu.Items.Add(new ToolStripControlHost(showPosItem) { Margin = new Padding(12, 2, 2, 2) });

            // segment logs: remove the float32 knot-seam steps at the
            // reconstruction source (see MotionLog.DeGlitch) - needs a
            // resample, but the tick and segment set are unchanged so
            // sample indices (zoom/selection/marker) all stay valid
            deGlitchItem.CheckedChanged += delegate
            {
                if (log == null) return;
                log.DeGlitch = deGlitchItem.Checked;
                if (log.Segs != null)
                {
                    log.BuildSamples(CurTick(), log.BuiltTail);
                    ApplyFilter();
                }
                xy.Invalidate();
            };
            menu.Items.Add(new ToolStripControlHost(deGlitchItem) { Margin = new Padding(12, 2, 2, 2) });

            var mHelp = new ToolStripMenuItem("&Help", null, OnHelp) { Alignment = ToolStripItemAlignment.Right };
            menu.Items.Add(mHelp);
            KeyPreview = true;            // F1 = Help from anywhere
            KeyDown += delegate (object sh, KeyEventArgs ke)
                { if (ke.KeyCode == Keys.F1) OnHelp(null, EventArgs.Empty); };
            MainMenuStrip = menu;

            split = new SplitContainer
            {
                Dock = DockStyle.Fill,
                Orientation = Orientation.Vertical,
                SplitterDistance = 550
            };

            xy.Dock = DockStyle.Fill;
            xy.SelectionChanged += delegate { OnSelection(); };
            split.Panel1.Controls.Add(xy);

            var stripHost = new TableLayoutPanel { Dock = DockStyle.Fill, RowCount = 5, ColumnCount = 1 };
            string[] names = { "Position", "Velocity", "Accel", "Jerk", "Filter Err" };
            for (int i = 0; i < 5; i++)
            {
                stripHost.RowStyles.Add(new RowStyle(SizeType.Percent, 20f));
                strips[i] = new TimePlotPanel { Dock = DockStyle.Fill, View = tview, Title = names[i], Mark = mark, Hover = hover };
                stripHost.Controls.Add(strips[i], 0, i);
            }
            split.Panel2.Controls.Add(stripHost);

            // the shared sample marker: set from either side, shown on all
            xy.Mark = mark;
            mark.Changed += delegate
            {
                xy.SyncMark();
                xy.Invalidate();
                foreach (var s in strips) s.Invalidate();
            };
            // the shared cursor sample: one strip's mouse move repaints
            // every strip's readout at that sample
            hover.Changed += delegate { foreach (var s in strips) s.Invalidate(); };

            status.Items.Add(statusLabel);

            Controls.Add(split);
            Controls.Add(status);
            Controls.Add(menu);

            tview.Changed += delegate { foreach (var s in strips) s.Invalidate(); };

            // restore persisted window/panel/options state
            var cfg = LoadCfg();
            int w = CfgInt(cfg, "WinW", 1200), h = CfgInt(cfg, "WinH", 800);
            int wx = CfgInt(cfg, "WinX", int.MinValue), wy = CfgInt(cfg, "WinY", int.MinValue);
            Width = Math.Max(400, w); Height = Math.Max(300, h);
            if (wx != int.MinValue &&
                Screen.AllScreens.Length > 0 &&
                SystemInformation.VirtualScreen.IntersectsWith(new Rectangle(wx, wy, w, h)))
            {
                StartPosition = FormStartPosition.Manual;
                Location = new Point(wx, wy);
            }
            bool maximized = CfgInt(cfg, "Maximized", 0) != 0;
            int splitDist = CfgInt(cfg, "Split", 550);
            int axisIdx = CfgInt(cfg, "Axis", 0);
            int filtIdx = CfgInt(cfg, "Filter", 0);
            string tmp;
            if (cfg.TryGetValue("Tau", out tmp)) tauBox.Text = tmp;
            if (cfg.TryGetValue("Tick", out tmp)) tickBox.Text = tmp;
            compDelayItem.Checked = CfgInt(cfg, "CompDelay", 0) != 0;
            showPosItem.Checked = CfgInt(cfg, "ShowPos", 1) != 0;
            xy.ShowP2 = showPosItem.Checked;
            deGlitchItem.Checked = CfgInt(cfg, "DeGlitch", 1) != 0;
            if (axisIdx >= 0 && axisIdx < axisBox.Items.Count) axisBox.SelectedIndex = axisIdx;
            if (filtIdx >= 0 && filtIdx < filterBox.Items.Count) filterBox.SelectedIndex = filtIdx;

            Load += delegate
            {
                if (maximized) WindowState = FormWindowState.Maximized;
                try { split.SplitterDistance = Math.Max(100, Math.Min(split.Width - 100, splitDist)); }
                catch { }
            };
            FormClosing += delegate { SaveCfg(); };

            string path = fileArg;
            if (path == null && File.Exists(@"c:\Temp\TPSegLog.csv")) path = @"c:\Temp\TPSegLog.csv";
            if (path != null) LoadFile(path);
        }

        void OnOpen(object sender, EventArgs e)
        {
            using (var dlg = new OpenFileDialog { Filter = "CSV logs|*.csv|All files|*.*", FileName = @"c:\Temp\TPSegLog.csv" })
                if (dlg.ShowDialog(this) == DialogResult.OK) LoadFile(dlg.FileName);
        }

        string curFile;
        void OnReload(object sender, EventArgs e) { if (curFile != null) LoadFile(curFile); }

        void LoadFile(string path)
        {
            try
            {
                log = MotionLog.Load(path);
                log.DeGlitch = deGlitchItem.Checked;
                if (log.Segs != null) log.BuildSamples(CurTick(), 0.02);
                curFile = path;
            }
            catch (Exception ex)
            {
                MessageBox.Show(this, ex.Message, "Load failed");
                return;
            }
            xy.Log = log;
            xy.SelStart = xy.SelEnd = -1;
            xy.FitAll();
            foreach (var s in strips) s.Log = log;
            mark.Set(-1);           // sample indices are new
            hover.Reset();
            ApplyFilter();          // carry the current filter to the new log
            OnSelection();
            UpdateStatus();
        }

        void UpdateStatus()
        {
            if (log == null) return;
            string segInfo;
            if (log.Segs != null)
                segInfo = string.Format("{0} segments   tick = {1:0.###} us   ",
                    log.Segs.Count, log.Dt * 1e6);
            else
            {
                // ACTUAL spacing range in us: capture logs change rate when
                // user threads start/exit (90us per active thread), so one
                // average number hides e.g. a 180-270us mix
                double mn = double.MaxValue, mx = 0;
                for (int i = 1; i < log.Count; i++)
                {
                    double dt = log.T[i] - log.T[i - 1];
                    if (dt > 0 && dt < mn) mn = dt;
                    if (dt > mx) mx = dt;
                }
                if (mn > mx) { mn = mx = log.Dt; }   // <2 samples
                segInfo = (mx - mn < 1e-9)
                    ? string.Format("dt = {0:0.###} us   ", mn * 1e6)
                    : string.Format("dt = {0:0.###}-{1:0.###} us   ", mn * 1e6, mx * 1e6);
            }
            statusLabel.Text = string.Format("{0}   {1}{2} samples   {3:0.###} s total",
                curFile, segInfo, log.Count, log.Count > 0 ? log.T[log.Count - 1] - log.T[0] : 0);
        }

        void OnSelection()
        {
            foreach (var s in strips) { s.SelStart = xy.SelStart; s.SelEnd = xy.SelEnd; }
            if (log != null && log.Count > 1)
            {
                int i0 = xy.SelStart >= 0 ? xy.SelStart : 0;
                int i1 = xy.SelEnd >= 0 ? xy.SelEnd : log.Count - 1;
                tview.Set(log.T[i0], log.T[i1]);
            }
            UpdateStrips();
        }

        void UpdateStrips()
        {
            int ax = axisBox.SelectedIndex;
            if (ax < 0 || log == null) return;
            string name = MotionLog.AxisNames[ax];
            bool showPos = log.HasP2 && showPosItem.Checked;
            strips[0].Title = "Position " + name + (showPos ? " (green=Dest crimson=Pos)" : "");
            strips[0].Series = delegate { return log.Pos(ax); };
            strips[0].Series2 = showPos ? (Func<double[]>)delegate { return log.Meas(ax); } : null;
            strips[0].ValName = "D"; strips[0].Val2Name = "P";
            strips[1].Title = "Velocity " + name;   strips[1].Series = delegate { return log.Vel(ax); };
            strips[1].Series2 = showPos ? (Func<double[]>)delegate { return log.MeasVel(ax); } : null;
            strips[1].ValName = "V"; strips[1].Val2Name = "Vm";
            strips[2].Title = "Accel " + name;      strips[2].Series = delegate { return log.Acc(ax); };
            strips[2].ValName = "A";
            strips[3].Title = "Jerk " + name;       strips[3].Series = delegate { return log.Jerk(ax); };
            strips[3].ValName = "J";
            if (log.HasP2)
            {
                // the money plot for tracking diagnosis: commanded minus
                // measured (following error) over time
                strips[4].Title = "Dest - Pos " + name;
                strips[4].Series = delegate { return log.DestPosErr(ax); };
                strips[4].Series2 = null;
                strips[4].ValName = "E";
            }
            else
            {
                strips[4].Title = "Filter Err " + name + (log.CompDelay ? " (delay comp)" : "");
                strips[4].Series = delegate { return log.FiltErr(ax); };
                strips[4].Series2 = null;
                strips[4].ValName = "E";
            }
            foreach (var s in strips) s.Invalidate();
        }
    }
}
