using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace MeasureFiducials
{

    public class EDGEBUF
    {
        public int EdgesFound;
        public int[] EdgeL;
        public int[] EdgeR;
        public int[] EdgeH;
        public float[] EdgeC;

        public EDGEBUF()
        {
            EdgeL = new int[Align.EDGEBUFMAX];
            EdgeR = new int[Align.EDGEBUFMAX];
            EdgeH = new int[Align.EDGEBUFMAX];
            EdgeC = new float[Align.EDGEBUFMAX];
        }
    }


    public class TARGBUF
    {
        public bool TargetFound;
        public int TargetL;
        public int TargetR;
        public float TargetC;
        public float Adj;
        public float ImageSizeInPixels;
    }


    class Align
    {
        private const int STARTPIX = 3;
        private const int ENDPIX = 3;

        public EDGEBUF EdgeBufX;
        public EDGEBUF EdgeBufY;

        public TARGBUF TargetX;
        public TARGBUF TargetY;

        public EDGEBUF EdgeBufLeft;
        public EDGEBUF EdgeBufRight;

        public EDGEBUF EdgeBufDiagonalX;
        public EDGEBUF EdgeBufDiagonalY;

        public TARGBUF TargetDiagonalX;
        public TARGBUF TargetDiagonalY;

        public TARGBUF TargetLeft;
        public TARGBUF TargetRight;

        // EDGE BUFFER DEFINITIONS
        // The edge buffer has two sections:
        //    a) section 1, defined by MAXEDGES, is for regular edge finding/alignment
        //    b) section 2 reserves two locations for storing the obscured edges
        private const int MAXEDGES = 100;
        public const int EDGEBUFMAX = MAXEDGES + 2;



        public enum Alignment_Type
        {
            ALGN_BLK_BAR,
            ALGN_WHT_BAR
        }


        public Align()
        {
            EdgeBufX = new EDGEBUF();
            EdgeBufY = new EDGEBUF();
            TargetX = new TARGBUF();
            TargetY = new TARGBUF();

            EdgeBufDiagonalX = new EDGEBUF();
            EdgeBufDiagonalY = new EDGEBUF();
            TargetDiagonalX = new TARGBUF();
            TargetDiagonalY = new TARGBUF();

            EdgeBufLeft = new EDGEBUF();
            EdgeBufRight = new EDGEBUF();
            TargetLeft = new TARGBUF();
            TargetRight = new TARGBUF();
        }


        int EdgeExtract(int[] data, ref EDGEBUF edges, double threshold)
        {
            int dIndex, CurRunL;
            int CurRunH, DifMax, DifCur;
            bool RunEnded;

            edges.EdgesFound = 0;
            CurRunL = 1;
            CurRunH = data[STARTPIX + 1] - data[STARTPIX];  // Sign of first Difference
            DifMax = Math.Abs(CurRunH);

            threshold *= 1000.0;  // make numbers smaller for convienience

            for (dIndex = STARTPIX; dIndex < data.Length - ENDPIX; dIndex++)
            {
                DifCur = data[dIndex + 1] - data[dIndex];
                if (Math.Abs(DifCur) > DifMax) DifMax = Math.Abs(DifCur);

                RunEnded = ((DifCur ^ CurRunH) < 0) || ((4 * Math.Abs(DifCur) <= DifMax));

                if (RunEnded)
                {
                    AddEdge(ref edges, threshold, data, CurRunH, CurRunL, dIndex);
                    CurRunH = DifCur;
                    CurRunL = 1;
                    DifMax = 0;
                }
                else
                {
                    CurRunL++;
                    CurRunH += DifCur;
                }
            }
            AddEdge(ref edges, threshold, data, CurRunH, CurRunL, dIndex);  // Add last edge, maybe
            if (edges.EdgesFound > MAXEDGES) return 1;

            return 0;
        }

        private void AddEdge(ref EDGEBUF edges, double threshold, int[] data, int curRunH, int curRunL, int dIndex)
        {
            int i;

            if ((Math.Abs(curRunH) > threshold) &&
                (curRunL >= 1) &&
                ((i = edges.EdgesFound) < MAXEDGES))
            {
                edges.EdgeH[i] = curRunH / 1000;  // make the number smaller for convienince
                edges.EdgeL[i] = dIndex - curRunL;
                edges.EdgeR[i] = dIndex;
                edges.EdgeC[i] = CenterOfEdge(data, ref edges, i);
                edges.EdgesFound++;
            }
        }


        /***************************************************************/
        /*  finds the center of an edge by fitting a parabola to the   */
        /*  derivative of the data and finding the position of maximum */
        /***************************************************************/

        private float CenterOfEdge(int[] data, ref EDGEBUF edges, int eI)
        {
            const int MAXPIXS = 120;  // Maximum number of points expected in an edge

            float mfact, xbar;
            float p1, p1sq, p2;
            float g1, g2, xx1, xx2;

            float[] DerBuf;  // Derivative buffer
            int DerIndex;
            int cpn, bpn, epn; // current, beginning, end Pixel number
            int dI;          // Index to current pixel in data buffer
            float MaxD = -10000.0F;
            int MaxI = 0;
            float MaxDelta;
            int NumbPix;

            DerBuf = new float[data.Length];

            NumbPix = (edges.EdgeR[eI]) - (edges.EdgeL[eI]) + 1;
            if (NumbPix > MAXPIXS) NumbPix = MAXPIXS; /* LimitMAXPIXS pix edge */
            for (DerIndex = 1; DerIndex <= NumbPix; DerIndex++)
            {
                dI = edges.EdgeL[eI] + DerIndex - 1;  /* Current data pixel index */
                if (edges.EdgeH[eI] > 0)
                    DerBuf[DerIndex] = (float)(-(2 * data[dI - 2] + data[dI - 1] -
                                           data[dI + 1] - 2 * data[dI + 2]));
                else DerBuf[DerIndex] = (float)((2 * data[dI - 2] + data[dI - 1] -
                                    data[dI + 1] - 2 * data[dI + 2]));
                if (DerBuf[DerIndex] > MaxD)
                {
                    MaxD = DerBuf[DerIndex];
                    MaxI = DerIndex;   /* Save max value and its index */
                }
            }

            /* Trim points of shallow slope from edge */
            epn = MaxI + 2;
            bpn = MaxI - 2;
            if (epn > NumbPix) epn = NumbPix;
            if (bpn < 1) bpn = 1;

            /* Fit parabola to the derivative and find its maximum */

            cpn = 0;  /* current pixel counter */
            xbar = ((epn - bpn) / 2.0F) + 1.0F;   /* Average x value */
            mfact = ((epn - bpn + 1) * (epn - bpn + 1) - 1) / 12.0F;  /* Factor for p2 */
            g1 = g2 = xx1 = xx2 = 0.0F;
            if ((MaxI != 1) && (MaxI != NumbPix))
            {
                for (DerIndex = bpn; DerIndex <= epn; DerIndex++)
                {
                    cpn++;
                    p1 = cpn - xbar;
                    p1sq = p1 * p1;
                    xx1 = xx1 + p1sq;
                    g1 = g1 + p1 * DerBuf[DerIndex];

                    p2 = p1sq - mfact;
                    xx2 = xx2 + p2 * p2;
                    g2 = g2 + p2 * DerBuf[DerIndex];
                }
            }

            if ((g2 != 0) && (xx1 != 0))
                MaxDelta = bpn + xbar - 2.0F - (g1 * xx2) / (2.0F * g2 * xx1);
            else
                MaxDelta = bpn + xbar - 2.0F;

            if (MaxDelta >= ((edges.EdgeR[eI]) - (edges.EdgeL[eI])) || (MaxDelta < 1))
                MaxDelta = bpn + xbar - 2.0F;

            return ((edges.EdgeL[eI]) + MaxDelta);   /* Edge center from left */
        }




        public int AlignTarget(Alignment_Type AlignType, int[] XProfile, int[] YProfile, ref EDGEBUF EdgeBufX, ref EDGEBUF EdgeBufY, ref TARGBUF TargetX, ref TARGBUF TargetY,
            double DesiredTrgtBarWidthX, double DesiredTrgtBarWidthY, double TargetWidthTolX, double TargetWidthTolY, double EdgeThresholdX, double EdgeThresholdY, ref String err)
        {
            err = "";

            EdgeBufX.EdgesFound = EdgeBufY.EdgesFound = 0;
            TargetX.TargetFound = TargetY.TargetFound = false;

            if (EdgeExtract(XProfile, ref EdgeBufX, EdgeThresholdX) != 0)
            {
                return 1;
            }

            if (EdgeExtract(YProfile, ref EdgeBufY, EdgeThresholdY) != 0)
            {
                return 1;
            }

            TargetLocate(EdgeBufX, DesiredTrgtBarWidthX, TargetWidthTolX, EdgeThresholdX, XProfile.Length, ref TargetX, AlignType);
            TargetLocate(EdgeBufY, DesiredTrgtBarWidthY, TargetWidthTolY, EdgeThresholdY, YProfile.Length, ref TargetY, AlignType);

            if (!TargetX.TargetFound && !TargetY.TargetFound)
            {
                err = "Error Finding X and Y Targets";
                return 1;
            }
            else if (!TargetX.TargetFound)
            {
                err = "Error Finding X Target";
                return 1;
            }
            else if (!TargetY.TargetFound)
            {
                err = "Error Finding Y Target";
                return 1;
            }

            return 0;
        }

        /*******************************************************************************/
        /*                                                                             */
        /*    Returns 0       if successful                                            */
        /*            nonzero if unsuccessful                                          */
        /*                                                                             */
        /*  HowToLook = ONLYTARGET this parameter informs the target finding routine   */
        /*                           that there may be no reticle in view and that the */
        /*                           reticle has not been located.                     */
        /*            = INSIDERET  this informs the target finding routine to          */
        /*                           only look for the target inside the reticle       */
        /*                           edges that have been found                        */
        /*                                                                             */
        /*******************************************************************************/
        private int TargetLocate(EDGEBUF edges, double DesiredWidth, double TargetWidthTol, double Threshold, int Length, ref TARGBUF tar, Alignment_Type AlignType)
        {
            int lei, rei, BeginningOfTargetEdges = 0, EndOfTargetEdges = 0;
            double HRatio, WRatio, TargetWidth;
            double MeritValue, MaxMeritValue = -1.0F;
            int Polarity = AlignType == Alignment_Type.ALGN_BLK_BAR ? 1 : -1;

            tar.TargetFound = false;
            BeginningOfTargetEdges = 0;
            EndOfTargetEdges = edges.EdgesFound - 1;

            if (EndOfTargetEdges <= BeginningOfTargetEdges)   // we need at least 2 edges for the target
                return 1;

            lei = BeginningOfTargetEdges;
            do
            {
                rei = EndOfTargetEdges;
                do
                {
                    HRatio = (float)Math.Abs((float)(edges.EdgeH[lei]) / (float)(edges.EdgeH[rei]));
                    if (HRatio > 1.0F) HRatio = 1.0F / HRatio;


                    if ((edges.EdgeH[lei] * Polarity < -Threshold) &&
                        (edges.EdgeH[rei] * Polarity > Threshold) &&
                        (HRatio > 0.33333F))
                    {
                        TargetWidth = (edges.EdgeC[rei]) - (edges.EdgeC[lei]);
                        if ((TargetWidth >= DesiredWidth / TargetWidthTol) &&
                            (TargetWidth <= DesiredWidth * TargetWidthTol))
                        {
                            //
                            // Valid Target found, compute merit value
                            //
                            WRatio = TargetWidth / (float)DesiredWidth;
                            if (WRatio > 1.0F) WRatio = 1.0F / WRatio;
                            MeritValue = (edges.EdgeH[rei] - edges.EdgeH[lei]) * Polarity * WRatio * HRatio;
                            if (MeritValue > MaxMeritValue)  // best so far ??
                            {
                                MaxMeritValue = MeritValue;
                                tar.TargetFound = true;
                                tar.TargetL = lei;
                                tar.TargetR = rei;
                            }
                        }
                    }
                } while (--rei > lei);
            } while (++lei < EndOfTargetEdges);

            if (tar.TargetFound)
            {
                tar.TargetC = (edges.EdgeC[tar.TargetL] +
                                edges.EdgeC[tar.TargetR]) / 2;

                tar.Adj = tar.TargetC - 0.5f * Length;
                tar.ImageSizeInPixels = Length;

                return 0;
            }
            else
                return 1;
        }

        public void AlignALL(Grab Grabber, double TargetBarWidthX, double TargetBarWidthY, double TargetBarWidthTolX, double TargetBarWidthTolY,
                                                              double TargetBarMinHeightX, double TargetBarMinHeightY, ref String err)
        {
            // Align Cross
            AlignTarget(Align.Alignment_Type.ALGN_BLK_BAR, Grabber.XProfile, Grabber.YProfile, ref EdgeBufX, ref EdgeBufY,
                            ref TargetX, ref TargetY, TargetBarWidthX, TargetBarWidthY, TargetBarWidthTolX, TargetBarWidthTolY,
                                                              TargetBarMinHeightX, TargetBarMinHeightY, ref err);

            // Align Diagonal X
            AlignTarget(Align.Alignment_Type.ALGN_WHT_BAR, Grabber.XDiagonalProfile, Grabber.YDiagonalProfile, ref EdgeBufDiagonalX, ref EdgeBufDiagonalY,
                ref TargetDiagonalX, ref TargetDiagonalY, TargetBarWidthX, TargetBarWidthY, TargetBarWidthTolX, TargetBarWidthTolY,
                                                  TargetBarMinHeightX, TargetBarMinHeightY, ref err);

            // Align Rotation
            AlignTarget(Align.Alignment_Type.ALGN_BLK_BAR, Grabber.LeftProfile, Grabber.RightProfile, ref EdgeBufLeft, ref EdgeBufRight,
                ref TargetLeft, ref TargetRight, TargetBarWidthX, TargetBarWidthY, TargetBarWidthTolX, TargetBarWidthTolY,
                                                  TargetBarMinHeightX, TargetBarMinHeightY, ref err);
        }
    }
}
