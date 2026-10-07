/*********************************************************************/
/*         Copyright (c) 2003-2016  DynoMotion Incorporated          */
/*********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Drawing.Imaging;

namespace MeasureFiducials
{
    class Grab
    {
        byte[] ImageBytes;
        int width, height, stride;
        bool ImageValid;
        int bytes_per_pixel;
        PixelFormat format;

        Bitmap SaveBitmap;

        public int[] XProfile;
        public int[] YProfile;

        public int[] XDiagonalProfile;
        public int[] YDiagonalProfile;

        public int[] LeftProfile;
        public int[] RightProfile;

        public float Diagonaldx, Diagonaldy;

        public Grab()
        {
        }

        public void SaveImage(Bitmap bmp)
        {
            if (!ImageValid)
            {
                if (SaveBitmap == null)
                {
                    SaveBitmap = (Bitmap)bmp.Clone();
                }
                width = bmp.Width;
                height = bmp.Height;
                format = bmp.PixelFormat;

                if (format == PixelFormat.Format24bppRgb)
                    bytes_per_pixel = 3;
                else
                    throw new NotImplementedException("Only 24 bits per pixel supported");

                Rectangle rect = new Rectangle(0, 0, width, height);
                BitmapData srcData = bmp.LockBits(rect, ImageLockMode.ReadWrite, bmp.PixelFormat);
                Int64 srcScan0 = srcData.Scan0.ToInt64();
                stride = srcData.Stride;
                int rowLength = Math.Abs(srcData.Stride);
                //declare an array to hold the bytes of the bitmap
                int numBytes = rowLength * height;  // bottom up images have neg stride
                if (ImageBytes == null) ImageBytes = new byte[numBytes];

                //copy the RGB values into the array
                if (stride > 0)
                    System.Runtime.InteropServices.Marshal.Copy(new IntPtr(srcScan0), ImageBytes, 0, numBytes);
                else
                    System.Runtime.InteropServices.Marshal.Copy(new IntPtr(srcScan0 + (height - 1) * stride), ImageBytes, 0, numBytes);

                bmp.UnlockBits(srcData);
                ImageValid = true;
            }
        }

        public bool GetImage(Bitmap bmp, float Ortho)
        {
            if (ImageValid)
            {
                Rectangle rect = new Rectangle(0, 0, width, height);
                BitmapData srcData = bmp.LockBits(rect, ImageLockMode.ReadWrite, bmp.PixelFormat);
                Int64 srcScan0 = srcData.Scan0.ToInt64();
                int srcStride = srcData.Stride;
                int rowLength = Math.Abs(srcData.Stride);
                //declare an array to hold the bytes of the bitmap
                int numBytes = rowLength * height;  // bottom up images have neg stride

                //copy the RGB values into the array
                if (srcStride > 0)
                    System.Runtime.InteropServices.Marshal.Copy(ImageBytes, 0, new IntPtr(srcScan0 + (height - 1) * srcStride), numBytes);
                else
                    System.Runtime.InteropServices.Marshal.Copy(ImageBytes, 0, new IntPtr(srcScan0 + (height - 1) * srcStride), numBytes);

                bmp.UnlockBits(srcData);
                GRAB_Profiles();
                GRAB_DiagonalProfiles(Ortho);
                GRAB_LeftRightProfiles();
                ImageValid = false;
                return true;
            }
            return false;
        }

        public Bitmap MakeBitmap()
        {
            if (ImageValid)
            {
                return SaveBitmap;
            }
            else
            {
                return null;
            }
        }


        public void GRAB_Profiles()
        {
            int r, c, gray, sumrow;

            if (ImageValid)
            {
                if (XProfile == null) XProfile = new int[width];
                if (YProfile == null) YProfile = new int[height];

                for (c = 0; c < width; c++) XProfile[c] = 0;  // first clear all the sum of the columns

                for (r = 0; r < height; r++)
                {
                    sumrow = 0;

                    for (c = 0; c < width; c++)
                    {
                        gray = ImageBytes[(r * width + c) * bytes_per_pixel];       // get the rgb scale and sum the 3
                        gray += ImageBytes[(r * width + c) * bytes_per_pixel + 1];
                        gray += ImageBytes[(r * width + c) * bytes_per_pixel + 2];

                        sumrow += gray;
                        XProfile[c] += gray;
                    }

                    YProfile[r] = sumrow;
                }
            }
        }

        public void GRAB_LeftRightProfiles()
        {
            int r, c, gray, sumrow_left, sumrow_right;
            int bandwidth = width / 4;
            int margin = width / 10;
            int c0 = margin;
            int c1 = margin + bandwidth;
            int c2 = width - margin - bandwidth;
            int c3 = width - margin;

            if (ImageValid)
            {
                if (LeftProfile == null) LeftProfile = new int[height];
                if (RightProfile == null) RightProfile = new int[height];

                for (r = 0; r < height; r++)
                {
                    sumrow_left = sumrow_right = 0;

                    for (c = 0; c < width; c++)
                    {
                        gray = ImageBytes[(r * width + c) * bytes_per_pixel];       // get the rgb scale and sum the 3
                        gray += ImageBytes[(r * width + c) * bytes_per_pixel + 1];
                        gray += ImageBytes[(r * width + c) * bytes_per_pixel + 2];



                        if (c > c0 && c < c1)
                            sumrow_left += gray;
                        else if (c > c2 && c < c3)
                            sumrow_right += gray;

                    }
                    LeftProfile[r] = sumrow_left * 4;
                    RightProfile[r] = sumrow_right * 4;
                }
            }
        }




        void GRAB_DiagonalProfiles(float Ortho)
        {
            int r, rr, c, cc, gray, i, iw, iv, Length;
            float[] row;
            float[] col;
            float[] row_n;
            float[] col_n;
            //          float X_um_per_pixel = 10.55f, Y_um_per_pixel = 11.46f;
            float X_um_per_pixel = Ortho, Y_um_per_pixel = 1.0f;

            Diagonaldx = 4.0f * Y_um_per_pixel / (X_um_per_pixel + Y_um_per_pixel);
            Diagonaldy = 4.0f * X_um_per_pixel / (X_um_per_pixel + Y_um_per_pixel);



            if (ImageValid)
            {
                Length = (int)(width / Diagonaldx + height / Diagonaldy);

                XDiagonalProfile = new int[Length];
                YDiagonalProfile = new int[Length];

                row = new float[Length + 1];
                col = new float[Length + 1];
                row_n = new float[Length + 1];
                col_n = new float[Length + 1];


                float factor0, factor1, w, v;

                bool ReverseX = false, ReverseY = false;

                // Clear the sums and counts of the rows and columns
                for (c = 0; c < Length; c++) col[c] = 0;
                for (c = 0; c < Length; c++) col_n[c] = 0;
                for (r = 0; r < Length; r++) row[r] = 0;
                for (r = 0; r < Length; r++) row_n[r] = 0;


                // sum the diagonal target profiles 
                for (rr = 0; rr < height; rr++)
                {
                    if (ReverseY)
                        r = height - 1 - rr;
                    else
                        r = rr;

                    for (cc = 0; cc < width; cc++)
                    {
                        if (ReverseX)
                            c = width - 1 - cc;
                        else
                            c = cc;

                        gray = ImageBytes[(r * width + c) * bytes_per_pixel];       // get the rgb scale and sum the 3
                        gray += ImageBytes[(r * width + c) * bytes_per_pixel + 1];
                        gray += ImageBytes[(r * width + c) * bytes_per_pixel + 2];


                        // figure which rising diagonal line this pixel falls between

                        w = c / Diagonaldx + r / Diagonaldy;

                        iw = (int)w;

                        factor1 = (w - iw);
                        factor0 = 1.0f - factor1;

                        col[iw] += factor0 * gray;
                        col_n[iw] += factor0;
                        col[iw + 1] += factor1 * gray;
                        col_n[iw + 1] += factor1;

                        // figure which falling diagonal line this pixel falls between

                        v = c / Diagonaldx + (height - 1 - r) / Diagonaldy;

                        iv = (int)v;

                        factor1 = (v - iv);
                        factor0 = 1.0f - factor1;

                        row[iv] += factor0 * gray;
                        row_n[iv] += factor0;
                        row[iv + 1] += factor1 * gray;
                        row_n[iv + 1] += factor1;
                    }
                }

                // Bottom Left to Top Right target profile: reverse data and transfer to x TARGDATA buffer

                for (i = 0; i < Length; i++)
                {
                    XDiagonalProfile[i] = (int)(col[i] / col_n[i] * 512);
                }

                // Top Left to Bottom Right target profile: reverse data and transfer to x TARGDATA buffer
                for (i = 0; i < Length; i++)
                {
                    YDiagonalProfile[i] = (int)(row[i] / row_n[i] * 512);
                }
            }
        }

        public void FilterProfiles(int FilterLevel)
        {
            DoFilterProfiles(FilterLevel, ref XProfile, ref YProfile);
            DoFilterProfiles(FilterLevel, ref XDiagonalProfile, ref YDiagonalProfile);
            DoFilterProfiles(FilterLevel, ref LeftProfile, ref RightProfile);
        }


        public void DoFilterProfiles(int FilterLevel, ref int[] buf1, ref int[] buf2)
        {
            if (FilterLevel > 20) FilterLevel = 20;

            for (int i = 0; i < FilterLevel; i++)
            {
                FilterData(ref buf1);
                FilterData(ref buf2);
            }
        }


        void FilterData(ref int[] buf)
        {
            float[] temp1;
            float[] temp2;
            int i;

            temp1 = new float[buf.Length];
            temp2 = new float[buf.Length];

            for (i = 0; i < buf.Length; i++) temp1[i] = (float)buf[i];
            RectFilt(ref temp1, ref temp2);
            RectFilt(ref temp2, ref temp1);
            RectFilt(ref temp1, ref temp2);
            for (i = 0; i < buf.Length; i++) buf[i] = (int)temp1[i];
        }

        void RectFilt(ref float[] inbuf, ref float[] outbuf)
        {
            int i;
            int sum;

            sum = (int)(inbuf[0] + inbuf[1]);
            outbuf[0] = inbuf[0];
            for (i = 1; i < inbuf.Length - 1; i++)
            {
                sum += (int)(inbuf[i + 1]);
                outbuf[i] = sum / 3.0F;
                sum -= (int)(inbuf[i - 1]);
            }
            outbuf[inbuf.Length - 1] = inbuf[inbuf.Length - 1];
        }

        public void AddPlots(Graphics g, Rectangle rectangle, int[] XProfile, int[] YProfile, EDGEBUF EdgesX, EDGEBUF EdgesY, TARGBUF TargetX, TARGBUF TargetY)
        {
            int i, r, c, x, y, x0 = 0, y0 = 0;
            double Scale = -rectangle.Height * 0.6 / (bytes_per_pixel * width * 255);
            double Offset = rectangle.Top + rectangle.Height * 0.99;

            if (XProfile == null) return;

            Pen PlotPen = new Pen(Color.FromArgb(255, 0, 255, 0), 2);

            // Plot Vertical plot

            for (c = 0; c < XProfile.Length; c++)
            {
                // map profile value to 
                x = rectangle.Left + (c * rectangle.Width) / width;
                y = (int)(Offset + Scale * XProfile[c]);
                ClipXY(ref rectangle, ref x, ref y);

                if (c > 0)
                {
                    g.DrawLine(PlotPen, x0, y0, x, y);
                }

                x0 = x;
                y0 = y;
            }



            // Draw circles around the center of the edges
            if (TargetX.TargetFound)
            {
                for (i = 0; i < EdgesX.EdgesFound; i++)
                {
                    if (i == TargetX.TargetL || i == TargetX.TargetR)
                    {
                        float xc = EdgesX.EdgeC[i];
                        float yc = InterpolateY(XProfile, xc);
                        float xp = rectangle.Left + (xc * rectangle.Width) / width;
                        float yp = (float)(yc * Scale + Offset);

                        g.DrawEllipse(PlotPen, xp - 4.0f, yp - 4.0f, 8.0f, 8.0f);
                    }
                }
            }



            // Plot Vertical plot

            Scale = rectangle.Width * 0.15 / (bytes_per_pixel * height * 255);
            Offset = rectangle.Left + rectangle.Width * 0.01;

            for (r = 0; r < height; r++)
            {
                // map profile value to 
                y = rectangle.Top + ((height - 1 - r) * rectangle.Height) / height;
                x = (int)(Offset + Scale * YProfile[r]);
                ClipXY(ref rectangle, ref x, ref y);

                if (r > 0)
                {
                    g.DrawLine(PlotPen, x0, y0, x, y);
                }

                x0 = x;
                y0 = y;
            }

            // Draw circles around the center of the edges
            if (TargetY.TargetFound)
            {
                for (i = 0; i < EdgesY.EdgesFound; i++)
                {
                    if (i == TargetY.TargetL || i == TargetY.TargetR)
                    {
                        float yc = EdgesY.EdgeC[i];
                        float xc = InterpolateY(YProfile, yc);
                        float yp = rectangle.Top + ((height - 1 - yc) * rectangle.Height) / height;
                        float xp = (float)(xc * Scale + Offset);

                        g.DrawEllipse(PlotPen, xp - 4.0f, yp - 4.0f, 8.0f, 8.0f);
                    }
                }
            }

            PlotPen.Dispose();
        }


        public void AddPlotsDouble(Graphics g, Rectangle rectangle, int[] XProfile, int[] YProfile, EDGEBUF EdgesX, EDGEBUF EdgesY, TARGBUF TargetX, TARGBUF TargetY)
        {
            int i, r, x, y, x0 = 0, y0 = 0;
            double Scale = -rectangle.Height * 0.6 / (bytes_per_pixel * width * 255);
            double Offset = rectangle.Top + rectangle.Height * 0.99;

            if (XProfile == null) return;

            Pen PlotPen = new Pen(Color.FromArgb(255, 0, 255, 0), 2);

            // Plot Vertical plot

            Scale = rectangle.Width * 0.15 / (bytes_per_pixel * height * 255);
            Offset = rectangle.Left + rectangle.Width * 0.01;

            for (r = 0; r < height; r++)
            {
                // map profile value to 
                y = rectangle.Top + ((height - 1 - r) * rectangle.Height) / height;
                x = (int)(Offset + Scale * YProfile[r]);
                ClipXY(ref rectangle, ref x, ref y);

                if (r > 0)
                {
                    g.DrawLine(PlotPen, x0, y0, x, y);
                }

                x0 = x;
                y0 = y;
            }

            // Draw circles around the center of the edges
            if (TargetY.TargetFound)
            {
                for (i = 0; i < EdgesY.EdgesFound; i++)
                {
                    if (i == TargetY.TargetL || i == TargetY.TargetR)
                    {
                        float yc = EdgesY.EdgeC[i];
                        float xc = InterpolateY(YProfile, yc);
                        float yp = rectangle.Top + ((height - 1 - yc) * rectangle.Height) / height;
                        float xp = (float)(xc * Scale + Offset);

                        g.DrawEllipse(PlotPen, xp - 4.0f, yp - 4.0f, 8.0f, 8.0f);
                    }
                }
            }


            // Plot Vertical plot

            Scale = rectangle.Width * 0.15 / (bytes_per_pixel * height * 255);
            Offset = rectangle.Left + rectangle.Width * 0.8;

            for (r = 0; r < height; r++)
            {
                // map profile value to 
                y = rectangle.Top + ((height - 1 - r) * rectangle.Height) / height;
                x = (int)(Offset + Scale * YProfile[r]);
                ClipXY(ref rectangle, ref x, ref y);

                if (r > 0)
                {
                    g.DrawLine(PlotPen, x0, y0, x, y);
                }

                x0 = x;
                y0 = y;
            }

            // Draw circles around the center of the edges
            if (TargetY.TargetFound)
            {
                for (i = 0; i < EdgesY.EdgesFound; i++)
                {
                    if (i == TargetY.TargetL || i == TargetY.TargetR)
                    {
                        float yc = EdgesY.EdgeC[i];
                        float xc = InterpolateY(YProfile, yc);
                        float yp = rectangle.Top + ((height - 1 - yc) * rectangle.Height) / height;
                        float xp = (float)(xc * Scale + Offset);

                        g.DrawEllipse(PlotPen, xp - 4.0f, yp - 4.0f, 8.0f, 8.0f);
                    }
                }
            }

            PlotPen.Dispose();
        }


        public void AddPlotsDiagonal(Graphics g, Rectangle rectangle, int[] XProfile, int[] YProfile, EDGEBUF EdgesX, EDGEBUF EdgesY, TARGBUF TargetX, TARGBUF TargetY)
        {
            int i, r, c, x, y, x0 = 0, y0 = 0;
            double Scale = -rectangle.Height * 0.6 / (bytes_per_pixel * width * 255);
            double Offset = rectangle.Top + rectangle.Height * 0.99;

            if (XProfile == null) return;

            Pen PlotPen = new Pen(Color.FromArgb(255, 0, 255, 0), 2);

            // Plot upward sloping plot

            for (c = 0; c < XProfile.Length; c++)
            {
                // map profile value to 
                double w = c;
                double v = Scale * (XProfile[c] - (0.5 * bytes_per_pixel * width * 255));

                x = (int)(rectangle.Left + (w + v) * 0.5f * Diagonaldx * rectangle.Width / width);
                y = (int)((rectangle.Top + rectangle.Height) - (w - v) * 0.5f * Diagonaldy * rectangle.Height / height);

                ClipXY(ref rectangle, ref x, ref y);

                if (c > 0)
                {
                    g.DrawLine(PlotPen, x0, y0, x, y);
                }

                x0 = x;
                y0 = y;
            }

            // Draw circles around the center of the edges
            if (TargetX.TargetFound)
            {
                for (i = 0; i < EdgesX.EdgesFound; i++)
                {
                    if (i == TargetX.TargetL || i == TargetX.TargetR)
                    {
                        double w = EdgesX.EdgeC[i];
                        double v = Scale * (InterpolateY(XProfile, (int)(EdgesX.EdgeC[i] + 0.5f)) - (0.5 * bytes_per_pixel * width * 255));

                        int xp = (int)(rectangle.Left + (w + v) * 0.5f * Diagonaldx * rectangle.Width / width);
                        int yp = (int)((rectangle.Top + rectangle.Height) - (w - v) * 0.5f * Diagonaldy * rectangle.Height / height);

                        g.DrawEllipse(PlotPen, xp - 4.0f, yp - 4.0f, 8.0f, 8.0f);
                    }
                }
            }



            // Plot Downward sloping plot

            Scale = rectangle.Width * 0.15 / (bytes_per_pixel * height * 255);
            Offset = rectangle.Left + rectangle.Width * 0.01;

            for (r = 0; r < YProfile.Length; r++)
            {
                // map profile value to 
                double w = r;
                double v = Scale * (YProfile[r] - (0.5 * bytes_per_pixel * width * 255));

                x = (int)(rectangle.Left + (w + v) * 0.5f * Diagonaldx * rectangle.Width / width);
                y = (int)(rectangle.Top + (w - v) * 0.5f * Diagonaldy * rectangle.Height / height);
                ClipXY(ref rectangle, ref x, ref y);

                if (r > 0)
                {
                    g.DrawLine(PlotPen, x0, y0, x, y);
                }

                x0 = x;
                y0 = y;
            }

            // Draw circles around the center of the edges
            if (TargetY.TargetFound)
            {
                for (i = 0; i < EdgesY.EdgesFound; i++)
                {
                    if (i == TargetY.TargetL || i == TargetY.TargetR)
                    {
                        double w = EdgesY.EdgeC[i];
                        double v = Scale * (InterpolateY(YProfile, (int)(EdgesY.EdgeC[i] + 0.5f)) - (0.5 * bytes_per_pixel * width * 255));

                        int xp = (int)(rectangle.Left + (w + v) * 0.5f * Diagonaldx * rectangle.Width / width);
                        int yp = (int)(rectangle.Top + (w - v) * 0.5f * Diagonaldy * rectangle.Height / height);

                        g.DrawEllipse(PlotPen, xp - 4.0f, yp - 4.0f, 8.0f, 8.0f);
                    }
                }
            }

            PlotPen.Dispose();
        }


        public void AddTarget(Graphics g, Rectangle rectangle, TARGBUF TargetX, TARGBUF TargetY)
        {
            int x = 0, y = 0, x0 = 0, y0 = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0;
            double LineLength = 0.1;
            double Scale = -rectangle.Height / (double)height;
            double Offset = rectangle.Top + rectangle.Height * (0.5 - LineLength);

            Pen PlotPen = new Pen(Color.FromArgb(255, 64, 64, 255), 4);

            // Plot Vertical mark

            if (TargetX.TargetFound)
            {
                // map profile value to 
                x = (int)(rectangle.Left + (TargetX.TargetC * rectangle.Width) / width);
                y = (int)(Offset);
                ClipXY(ref rectangle, ref x, ref y);

                x0 = x;
                y0 = y;

                Offset = rectangle.Top + rectangle.Height * (0.5 + LineLength);

                x = (int)(rectangle.Left + (TargetX.TargetC * rectangle.Width) / width);
                y = (int)(Offset);
                ClipXY(ref rectangle, ref x, ref y);

                if (!TargetY.TargetFound) g.DrawLine(PlotPen, x0, y0, x, y);
                x1 = x;
                y1 = y;
            }


            // Plot Vertical mark

            Scale = rectangle.Width / (double)width;
            Offset = rectangle.Left + rectangle.Width * (0.5 - LineLength);

            if (TargetY.TargetFound)
            {
                // map profile value to 
                y = (int)(rectangle.Top + ((height - 1 - TargetY.TargetC) * rectangle.Height) / height);
                x = (int)(Offset);
                ClipXY(ref rectangle, ref x, ref y);

                x2 = x;
                y2 = y;

                Offset = rectangle.Left + rectangle.Width * (0.5 + LineLength);

                // map profile value to 
                y = (int)(rectangle.Top + ((height - 1 - TargetY.TargetC) * rectangle.Height) / height);
                x = (int)(Offset);
                ClipXY(ref rectangle, ref x, ref y);

                if (!TargetX.TargetFound) g.DrawLine(PlotPen, x2, y2, x, y);
            }

            if (TargetX.TargetFound && TargetY.TargetFound)
            {
                g.DrawLine(PlotPen, x0, (int)(y2 - rectangle.Height * LineLength), x0, (int)(y2 + rectangle.Height * LineLength));
                g.DrawLine(PlotPen, (int)(x0 - rectangle.Height * LineLength), y2, (int)(x0 + rectangle.Height * LineLength), y2);
            }

            PlotPen.Dispose();
        }

        public void AddDiagonalTarget(Graphics g, Rectangle rectangle, TARGBUF TargetX, TARGBUF TargetY)
        {
            int x, y;
            float OffsetX = rectangle.Left + rectangle.Width / 2;
            float OffsetY = rectangle.Top + rectangle.Height / 2;

            float LineLength = 0.07f;
            int LEN = (int)(rectangle.Height * LineLength);

            float w, v;

            Pen PlotPen = new Pen(Color.FromArgb(255, 64, 64, 255), 4);

            // Plot w v mark

            if (TargetX.TargetFound)
                w = TargetX.Adj;
            else
                w = 0;

            if (TargetY.TargetFound)
                v = TargetY.Adj;
            else
                v = 0;

            // w = x dx + y dy
            // v = x dx - y dy
            //
            // x = (w + v)/2dx
            // y = (w - v)/2dy

            x = (int)(OffsetX + (w + v) * 0.5f * Diagonaldx * rectangle.Width / width);
            y = (int)(OffsetY + -(w - v) * 0.5f * Diagonaldy * rectangle.Height / height);

            ClipXY(ref rectangle, ref x, ref y);

            if (TargetX.TargetFound) g.DrawLine(PlotPen, x - LEN, y - LEN, x + LEN, y + LEN);
            if (TargetY.TargetFound) g.DrawLine(PlotPen, x - LEN, y + LEN, x + LEN, y - LEN);

            PlotPen.Dispose();
        }

        private static void ClipXY(ref Rectangle rectangle, ref int x, ref int y)
        {
            if (x < rectangle.Left) x = rectangle.Left;
            if (y < rectangle.Top) y = rectangle.Top;
            if (x >= rectangle.Width + rectangle.Left) x = rectangle.Width + rectangle.Left - 1;
            if (y >= rectangle.Height + rectangle.Top) x = rectangle.Height + rectangle.Top - 1;
        }

        private float InterpolateY(int[] Profile, float x)
        {
            int ix = (int)x;

            if (ix < 0) return Profile[0];
            if (ix > Profile.Length - 2) return Profile[Profile.Length - 1];
            return Profile[ix] + ((float)Profile[ix + 1] - (float)Profile[ix]) * (x - ix);
        }
    }
}
