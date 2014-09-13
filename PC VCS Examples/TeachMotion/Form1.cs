using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using OpenTK.Graphics;
using OpenTK.Graphics.OpenGL;
using KMotion_dotNet;
using System.Threading;
using ForceFeedback;

namespace TeachMotion
{
    public partial class Form1 : Form
    {
        KM_Controller KM;
        KM_MainStatus MainStatus;
        String MainPath;
        double[] X;  // original values
        double[] Y;
        double[] X2; // offset values
        double DeltaY;
        int Npts=0;
        bool loaded = false;
        double xp = -7;
        double yp = -4;
        double xbias = 0;
        double ybias = 0;
        double scalex = 1;
        double scaley = 1;
        double XResolution = 3000.0;
        double YResolution = 3000.0;
        List<CPoint> CapList = new List<CPoint>();
        ForceFeedbackJoystick FFJoy;
        private double lastY = 0;
        private double filter_out = 0;
        JOYSTICK_TYPE JoystickType = JOYSTICK_TYPE.NO_JOYSTICK;
        double gminx = -10.0, gmaxx = -4.0;
        double gminy = -5.0, gmaxy = -3.0;

        public Form1()
        {
            InitializeComponent();
            MainPath = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            MainPath = System.IO.Path.GetDirectoryName(MainPath);
            MainPath = System.IO.Path.GetDirectoryName(MainPath);

            CFileName.Text = MainPath + @"\PC VCS Examples\TeachMotion\Probedata.txt";
            KM = new KMotion_dotNet.KM_Controller();
            FFJoy = new ForceFeedbackJoystick();
            JoystickType = FFJoy.Connect(this);
        }

        private void WriteLineHandleException(string s)
        {
            try
            {
                KM.WriteLine(s);
            }
            catch (DMException ex) // in case disconnect in the middle of reading status
            {
                MessageBox.Show(ex.InnerException.Message);
            }
        }

        private void OpenCFileDir_Click(object sender, EventArgs e)
        {
            // Show the dialog and get result.
            openCFileDialog.InitialDirectory = MainPath + "\\C Programs";
            DialogResult result = openCFileDialog.ShowDialog();
            if (result == DialogResult.OK) // Test result.
            {
                CFileName.Text = openCFileDialog.FileName;
            }
        }

        private void ReadData_Click(object sender, EventArgs e)
        {
            try
            {
                string[] lines = System.IO.File.ReadAllLines(CFileName.Text);  // read the file
                // skip the first line
                var LineList = new List<String>(lines);
                LineList.RemoveAt(0);
                lines = LineList.ToArray();

                Npts = lines.Length;
                X = new double[Npts];
                Y = new double[Npts];

                int i = 0;
                foreach (string s in lines)
                {
                    int i0 = s.IndexOf('X');
                    int i1 = s.IndexOf(' ');

                    X[i] = Convert.ToDouble(s.Substring(i0 + 1, i1 - i0-1));

                    i0 = s.IndexOf('Y');
                    Y[i++] = Convert.ToDouble(s.Substring(i0+1));
                }

                DeltaY=Y[0]-Y[1];
                
                ReadStatus.Text = String.Format("{0} points read, Initial Y value = {1}, delta = {2:n3}", Npts, Y[0], DeltaY);

                OffsetData(Convert.ToDouble(XOffset.Text));
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void OffsetData(double R)
        {
            double Increment = 0.01;
            X2 = (double[])X.Clone();  // copy original

            // apply a circle to every measured point and
            // push neighbor points out beyond the circle
            for (int i = 0; i < Npts; i++)
            {
                // offset forward
                double k = i + Increment;
                while (k < Npts - 1)
                {
                    // determine how far away the neighbor is in Y
                    double dy = (k - i) * DeltaY;

                    // compute extent outward in X
                    double d = R * R - dy * dy;
                    if (d < 0) break; // beyond circel radius - no need to go further
                    double dx = Math.Sqrt(d);

                    //interpolate to find X[k] where K is a fraction
                    int ki = (int)k;
                    double Xk = X[ki] + (X[ki + 1] - X[ki]) * (k - ki);

                    // if x value is within circle push out
                    if (X2[i] < Xk + dx) X2[i] = Xk + dx;
                    k += Increment;
                }
                // offset backward
                k = i - Increment;
                while (k > 0)
                {
                    // determine how far away the neighbor is in Y
                    double dy = (k - i) * DeltaY;

                    // compute extent outward in X
                    double d = R * R - dy * dy;
                    if (d < 0) break; // beyond circel radius - no need to go further
                    double dx = Math.Sqrt(d);

                    //interpolate to find X[k] where K is a fraction
                    int ki = (int)k;
                    double Xk = X[ki] + (X[ki + 1] - X[ki]) * (k - ki);

                    // if x value is within circle push out
                    if (X2[i] < Xk + dx) X2[i] = Xk + dx;
                    k -= Increment;
                }
            }
        }



        private void WriteOffsetData_Click(object sender, EventArgs e)
        {
            OffsetData(Convert.ToDouble(XOffset.Text));

            //for (int i = 0; i < Npts; i++)
            //    if (i > 5 && i < 30)
            //        X[i] = (i-5)*3*0.05;
            //    else
            //        X[i] = 0;


            // add "off into the input file name"
            String NewFile = System.IO.Path.GetFileNameWithoutExtension(CFileName.Text);
            NewFile = System.IO.Path.GetDirectoryName(CFileName.Text) + "\\" + NewFile + "Off" + System.IO.Path.GetExtension(CFileName.Text);
 
            System.IO.StreamWriter file = new System.IO.StreamWriter(NewFile);

            for (int i = 0; i < Npts; i++)
                file.WriteLine(String.Format("Y {0:n4} X {1:n4} X2 {2:n4}", Y[i], X[i], X2[i]));

            file.Close();
        }

        private void glControl1_Load(object sender, EventArgs e)
        {
            loaded = true;
            GL.ClearColor(Color.SkyBlue); // Yey! .NET Colors can be used directly!
            SetupViewport();
        }

        private void glControl1_Paint(object sender, PaintEventArgs e)
        {
            if (!loaded) // Play nice
                return;

            GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);
            GL.MatrixMode(MatrixMode.Modelview);
            GL.LoadIdentity();


            GL.Color3(Color.Red);
            GL.Begin(BeginMode.LineStrip);
            for (int i = 0; i < Npts; i++)
                VertexTran(Y[i], X[i]);
            GL.End();


            GL.Color3(Color.White);
            GL.Begin(BeginMode.LineStrip);
            for (int i = 0; i < Npts; i++)
                VertexTran(Y[i], X2[i]);
            GL.End();

            if (ShowData.Checked)
            {
                GL.Color3(Color.Violet);
                GL.Begin(BeginMode.LineStrip);
                foreach (CPoint c in CapList)
                    VertexTran(c.x, c.y);
                GL.End();
            }

            double x=xp, y=yp;

            if (x < gminx       ) x = gminx;
            if (x > gmaxx       ) x = gmaxx;
            if (y < gminy - 0.05) y = gminy - 0.05;
            if (y > gmaxy - 0.05) y = gmaxy - 0.05;

            // Draw Tool
            GL.Color3(Color.Black);
            GL.Begin(BeginMode.Polygon);
            VertexTran(x,        y);
            VertexTran(x - 0.05, y + 0.05);
            VertexTran(x - 0.05, y + 0.20);
            VertexTran(x + 0.05, y + 0.20);
            VertexTran(x + 0.05, y + 0.05);
            GL.End();
            glControl1.SwapBuffers();

        }

        private void VertexTran(double x, double y)
        {
            GL.Vertex2(x*scalex+xbias, y*scaley+ybias);
        }

        private void SetupViewport()
        {
            int w = glControl1.Width;
            int h = glControl1.Height;
            GL.MatrixMode(MatrixMode.Projection);
            GL.LoadIdentity();
            GL.Ortho(0, w, 0, h, -1, 1); // Bottom-left corner pixel has coordinate (0, 0)
            GL.Viewport(0, 0, w, h); // Use all of the glControl painting area

            double minx = -10.0, maxx = -4.0;
            double miny = -5.0, maxy = -3.0;

            scalex = w/(maxx - minx);
            scaley = h/(maxy - miny);

            if (scalex > scaley)
                scalex = scaley;
            else
                scaley = scalex;

            scaley = -scaley;

            double CenterX = (maxx + minx) / 2;
            double CenterY = (maxy + miny) / 2;


            // biases are calulated that map the center point 
            // on the display to the center in cad space position

            double CenterPixelx = w / 2;
            double CenterPixely = h / 2;

            xbias = CenterPixelx - (CenterX * scalex);
            ybias = CenterPixely - (CenterY * scaley);

            // set global plot range 
            gminx = (0 - xbias) / scalex;
            gmaxx = (w - xbias) / scalex;
            gminy = (h - ybias) / scaley;
            gmaxy = (0 - ybias) / scaley;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            double dx, dy;
            double max=10.0/60.0*0.05;  // limit speed to 10ipm

            dx =  joystick1.CursorLocation.X / 1000.0;
            dy = -joystick1.CursorLocation.Y / 1000.0;

            if (CaptureOn.Checked)
            {
                if (dx >  max) dx =  max;  // limit speed
                if (dx < -max) dx = -max;
                if (dy >  max) dy =  max;
                if (dy < -max) dy = -max;
            }

            xp += dx;
            yp += dy;




            double Xf=0, Yf=0;
            byte[] buttons=null;

            FFJoy.PollXY(ref Xf, ref Yf, ref buttons);

            if (JoystickType==JOYSTICK_TYPE.FORCE_FEEDBACK && buttons[0] == 0)
            {
                Xf = Yf = 0;
            }

            xp += Xf/30.0;
            yp += Yf/30.0;



            double ForceX = 0;
            double ForceY = 0;

            if (Npts > 0)
            {
                //find where we are in the Table
                // (note x-y confusion - screen x is machine Y)
                // (table is backwards - right to left)
                double k = (Y[0] - xp) / DeltaY;

                // if outside Table use endpooints
                int ki = (int)k;
                if (ki < 0) k = ki = 0;
                if (ki > Npts - 2) k = ki = Npts - 2;

                //Interpolate
                double Xk = X2[ki] + (X2[ki + 1] - X2[ki]) * (k - ki);




                double Vel = (Yf - lastY);

                ForceY = Vel * 0.5;

                double dist = yp - Xk;


                if (dist < 0.2)
                {
                    ForceY += (dist - 0.2) * 5;
                }

                filter_out = 0.9 * filter_out + 0.1 * ForceY;

                if (ForceY >  1.0) ForceY =  1.0;
                if (ForceY < -1.0) ForceY = -1.0;

                lastY = Yf;
                
                if (yp < Xk) yp = Xk;
            }


            FFJoy.ApplyForce(ForceX, ForceY);



            NPtsCaptured.Text = String.Format("{0}", CapList.Count);
 
            glControl1.Invalidate();

            UpdateStatus();

            DoMotion(xp, yp);
        }


        private void DoMotion(double xp, double yp)
        {
            double Tau=0.3;
            if (SyncEnable.Checked)
            {
                try
                {
                    KM.WriteLine(String.Format("MoveExp0={0} {1}", yp * XResolution, Tau));
                    KM.WriteLine(String.Format("MoveExp1={0} {1}", xp * YResolution, Tau));
                }
                catch (DMException ex) // in case disconnect in the middle of reading status
                {
                    KM.ReleaseToken();
                    MessageBox.Show(ex.InnerException.Message);
                }
            }
        }


        private void UpdateStatus()
        {
            int[] List;
            int nBoards = 0;

            // check how many boards connected
            List = KM.GetBoards(out nBoards);
            if (nBoards > 0)
            {
                this.Text = String.Format("Teach Motion - Connected - USB location {0:X}", List[0]);
            }
            else
            {
                this.Text = "Teach Motion - Disconnected";
            }

            if (KM.WaitToken(100) == KMOTION_TOKEN.KMOTION_LOCKED)
            {
                try
                {
                    MainStatus = KM.GetStatus(false);  // we already have a lock
                    KM.ReleaseToken();
                    XPos.Text = String.Format("{0:F4}", MainStatus.Destination[0] / XResolution);
                    YPos.Text = String.Format("{0:F4}", MainStatus.Destination[1] / YResolution);
                    XEnabled.Checked = (MainStatus.Enables & (1 << 0)) != 0;
                    YEnabled.Checked = (MainStatus.Enables & (1 << 1)) != 0;

                    if (CaptureOn.Checked)
                        CapList.Add(new CPoint(MainStatus.Destination[1] / YResolution, 
                                               MainStatus.Destination[0] / XResolution, 
                                               MainStatus.TimeStamp));
                }
                catch (DMException ex) // in case disconnect in the middle of reading status
                {
                    KM.ReleaseToken();
                    MessageBox.Show(ex.InnerException.Message);
                }
            }
        }

        private void XEnabled_Click(object sender, EventArgs e)
        {
            if (XEnabled.Checked)
                WriteLineHandleException("EnableAxis0");
            else
                WriteLineHandleException("DisableAxis0");
        }

        private void YEnabled_Click(object sender, EventArgs e)
        {
            if (YEnabled.Checked)
                WriteLineHandleException("EnableAxis1");
            else
                WriteLineHandleException("DisableAxis1");
        }

        private void INIT(object sender, EventArgs e)
        {
            try
            {
                SyncEnable.Checked = false;
                String Result = KM.CompileAndLoadCoff(1, MainPath + @"\C Programs\KStep\InitKStep3Axis.c", false);
                if (Result != "")
                {
                    MessageBox.Show(Result, "Compile Error");
                }
                else
                {
                    // everything ok, execute the Thread
                    KM.WriteLine("Execute1");
                }
            }
            catch (DMException ex)
            {
                MessageBox.Show(ex.InnerException.Message);
            }
        }

        private void CaptureOn_CheckedChanged(object sender, EventArgs e)
        {
            if (CaptureOn.Checked)
                WriteLineHandleException("SetBit46");
            else
                WriteLineHandleException("ClearBit46");
        }

        private void SyncEnable_CheckedChanged(object sender, EventArgs e)
        {
            if (SyncEnable.Checked)
            {
                xp = MainStatus.Destination[1] / YResolution;
                yp = MainStatus.Destination[0] / XResolution;
            }
        }

        private void ClearData_Click(object sender, EventArgs e)
        {
            CapList.Clear();
        }

        private void WriteData_Click(object sender, EventArgs e)
        {
            if (saveTeachFileDlg.ShowDialog() == DialogResult.OK)
            {
                System.IO.StreamWriter file = new System.IO.StreamWriter(saveTeachFileDlg.FileName);

                file.WriteLine(String.Format("G0 X {0:n4} Y {1:n4}", CapList[0].x, CapList[0].y));
                file.WriteLine("F30");
                foreach (CPoint c in CapList)
                    file.WriteLine(String.Format("G1 X {0:n4} Y {1:n4} (Time={2:n4})", c.x, c.y, c.t-CapList[0].t));

                file.WriteLine("M30");
                
                file.Close();
            }
        }
    }
}


// temp class to show example
class CPoint
{
    public double x, y, t;

    public CPoint(double X, double Y, double T)
    {
        x = X; y = Y; t = T;
    }
}