#define DSHOWNET
#define VS2003

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using KMotion_dotNet;
using System.Threading;
using System.Diagnostics;
using System.Reflection;
using DirectX.Capture;
using DShowNET;
using System.Runtime.InteropServices; // DsEvCode



namespace MeasureFiducials
{
    public partial class Form1 : Form
    {
        [DllImport("User32.dll", SetLastError = true)]
        public static extern IntPtr FindWindow(String lpClassName, String lpWindowName);


        [return: MarshalAs(UnmanagedType.Bool)]
        [DllImport("user32.dll", SetLastError = true)]
        public static extern bool PostMessage(IntPtr hWnd, uint Msg, int wParam, int lParam);

        String MainPath;
        KM_Controller KM;
        bool JoggingX = false;
        bool JoggingY = false;

        // note: internal values are all in inches
        // Settings file is all in inches
        // displayed values on screen will be inches or mm
        // Fiducial files are in inches or mm

        double XLeftTheo = 0;
        double YLeftTheo = 0;
        double XRightTheo = 0;
        double YRightTheo = 0;

        double XLeftMeasured = 0;
        double YLeftMeasured = 0;
        double XRightMeasured = 0;
        double YRightMeasured = 0;
   
        double XRef = 0;
        double YRef = 0;
        double XView = 0;
        double YView = 0;
        double XMicroOffset = 0;
        double YMicroOffset = 0;

        double theta=0;
        double scale=1;
        double offsetx=0;
        double offsety=0;

        string SettingsFile;
        string MicroScopeOffsetFile;
        double JogSpeed = 400;
        double XResolution = 10000;
        double YResolution = 10000;
        int VideoDevice = 1;
        double MouseMag = 0.25;
        bool Unitsmm = false;
        string UnitsFormat;


        KM_MainStatus MainStatus;

        private Capture capture = null;
        private Filters filters = new Filters();


        public Form1()
        {
            InitializeComponent();

            MainPath = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            MainPath = System.IO.Path.GetDirectoryName(MainPath);
            MainPath = System.IO.Path.GetDirectoryName(MainPath);

            FiducialsName.Text = MainPath + "\\PC VCS Examples\\MeasureFiducials\\FiducialLocations.txt";
            SettingsFile = MainPath + "\\PC VCS Examples\\MeasureFiducials\\MeasureFiducialSettings.txt";
            MicroScopeOffsetFile = MainPath + "\\PC VCS Examples\\MeasureFiducials\\MicroscopeOffset.txt";
            KM = new KMotion_dotNet.KM_Controller();
            ReadSettings();
            ReadFiducials();

            ViewCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
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

        private void OpenFiducialsDir_Click(object sender, EventArgs e)
        {
            // Show the dialog and get result.
            openFiducialsDialog.InitialDirectory = MainPath + "\\PC VCS Examples\\MeasureFiducials";
            openFiducialsDialog.FileName = System.IO.Path.GetFileName(FiducialsName.Text);
            openFiducialsDialog.AutoUpgradeEnabled = false;
	        DialogResult result = openFiducialsDialog.ShowDialog();
	        if (result == DialogResult.OK) // Test result.
	        {
                FiducialsName.Text = openFiducialsDialog.FileName;
                ReadFiducials();
	        }
        }
        private void SaveFiducialsDir_Click(object sender, EventArgs e)
        {
            // Show the dialog and get result.
            saveFiducialsDialog.InitialDirectory = MainPath + "\\PC VCS Examples\\MeasureFiducials";
            saveFiducialsDialog.FileName = System.IO.Path.GetFileName(FiducialsName.Text);
            saveFiducialsDialog.AutoUpgradeEnabled = false;
            DialogResult result = saveFiducialsDialog.ShowDialog();
            if (result == DialogResult.OK) // Test result.
            {
                FiducialsName.Text = saveFiducialsDialog.FileName;
                WriteFiducials();
            }
        }

        private void WriteFiducials()
        {
            try
            {
                XLeftTheo = ConvertToInches(Convert.ToDouble(LeftX.Text));
                YLeftTheo = ConvertToInches(Convert.ToDouble(LeftY.Text));
                XRightTheo = ConvertToInches(Convert.ToDouble(RightX.Text));
                YRightTheo = ConvertToInches(Convert.ToDouble(RightY.Text));
                System.IO.StreamWriter file = new System.IO.StreamWriter(FiducialsName.Text);
                file.WriteLine(String.Format("Left {0:F4} {1:F4}", ConvertToUser(XLeftTheo), ConvertToUser(YLeftTheo)));
                file.WriteLine(String.Format("Right {0:F4} {1:F4}", ConvertToUser(XRightTheo), ConvertToUser(YRightTheo)));
                file.Close();
            }
            catch
            {
                MessageBox.Show(String.Format("Unable to write file: {0}", FiducialsName.Text));
            }
        }


        private void ReadFiducials()
        {
            try
            {
                System.IO.StreamReader file = new System.IO.StreamReader(FiducialsName.Text);
                GetSetting2(file, "Left", ref XLeftTheo, ref YLeftTheo);
                GetSetting2(file, "Right", ref XRightTheo, ref YRightTheo);
                file.Close();

                XLeftTheo = ConvertToInches(XLeftTheo);
                YLeftTheo = ConvertToInches(YLeftTheo);
                XRightTheo = ConvertToInches(XRightTheo);
                YRightTheo = ConvertToInches(YRightTheo);

                LeftX.Text = ConvertToUser(XLeftTheo).ToString(UnitsFormat);
                LeftY.Text = ConvertToUser(YLeftTheo).ToString(UnitsFormat);
                RightX.Text = ConvertToUser(XRightTheo).ToString(UnitsFormat);
                RightY.Text = ConvertToUser(YRightTheo).ToString(UnitsFormat);
            }
            catch
            {
                MessageBox.Show(String.Format("Unable to read file: {0}", FiducialsName.Text));
            }
        }

        private void SaveMicroscopeOff_Click(object sender, EventArgs e)
        {
            try
            {
                XRef = ConvertToInches(Convert.ToDouble(RefX.Text));
                YRef = ConvertToInches(Convert.ToDouble(RefY.Text));
                XView = ConvertToInches(Convert.ToDouble(ViewX.Text));
                YView = ConvertToInches(Convert.ToDouble(ViewY.Text));
                System.IO.StreamWriter file = new System.IO.StreamWriter(MicroScopeOffsetFile);
                file.WriteLine(String.Format("Reference {0:F4} {1:F4}", ConvertToUser(XRef), ConvertToUser(YRef)));
                file.WriteLine(String.Format("View {0:F4} {1:F4}", ConvertToUser(XView), ConvertToUser(YView)));
                file.Close();
            }
            catch
            {
                MessageBox.Show(String.Format("Unable to write file: {0}", MicroScopeOffsetFile));
            }
        }


        private void ReadMicroscopeOffset()
        {
            try
            {
                System.IO.StreamReader file = new System.IO.StreamReader(MicroScopeOffsetFile);
                GetSetting2(file, "Reference", ref XRef, ref YRef);
                GetSetting2(file, "View", ref XView, ref YView);
                file.Close();

                XRef = ConvertToInches(XRef);
                YRef = ConvertToInches(YRef);
                XView = ConvertToInches(XView);
                YView = ConvertToInches(YView);
                XMicroOffset = XRef - XView;
                YMicroOffset = YRef - YView;
                RefX.Text = ConvertToUser(XRef).ToString(UnitsFormat);
                RefY.Text = ConvertToUser(YRef).ToString(UnitsFormat);
                ViewX.Text = ConvertToUser(XView).ToString(UnitsFormat);
                ViewY.Text = ConvertToUser(YView).ToString(UnitsFormat);
                MicroOffsetX.Text = ConvertToUser(XMicroOffset).ToString(UnitsFormat);
                MicroOffsetY.Text = ConvertToUser(YMicroOffset).ToString(UnitsFormat);
            }
            catch
            {
                MessageBox.Show(String.Format("Unable to read file: {0}", MicroScopeOffsetFile));
            }
        }

        private void GetSetting(System.IO.StreamReader file, string param, ref double X)
        {
            string s = file.ReadLine();

            int i0 = s.IndexOf(param + " ");
            if (i0 != 0) MessageBox.Show(String.Format("Invalid Parameter {0}", param));
            i0 += param.Length + 1;  // after parameter + space
            X = Convert.ToDouble(s.Substring(i0));
        }

        private void GetSetting2(System.IO.StreamReader file, string param, ref double X, ref double Y)
        {
            string s = file.ReadLine();

            int i0 = s.IndexOf(param + " ");
            if (i0 != 0) MessageBox.Show(String.Format("Invalid Parameter {0}", param));
            i0 += param.Length + 1;  // after parameter + space
            int i1 = s.IndexOf(' ', i0);

            X = Convert.ToDouble(s.Substring(i0, i1 - i0));
            Y = Convert.ToDouble(s.Substring(i1 + 1));
        }

        private void ReadSettings()
        {
            try
            {
                System.IO.StreamReader file = new System.IO.StreamReader(SettingsFile);
                double dUnitsmm = 0;
                GetSetting(file, "Unitsmm", ref dUnitsmm);
                Unitsmm = (dUnitsmm != 0);
                radioInch.Checked = !Unitsmm;
                radiomm.Checked = Unitsmm;
                GetSetting(file, "JogSpeed", ref JogSpeed);
                GetSetting2(file, "Resolution", ref XResolution, ref YResolution);
                double d=0;
                GetSetting(file, "VideoDevice", ref d);
                GetSetting(file, "MouseMag", ref MouseMag);
                MagFactor.Text = MouseMag.ToString();
                VideoDevice = (int)d;
                file.Close();
            }
            catch
            {
                MessageBox.Show(String.Format("Unable to read file: {0}", SettingsFile));
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            int []List;
            int nBoards = 0;

            // check how many boards connected
            List = KM.GetBoards(out nBoards);
            if (nBoards > 0)
            {
                this.Text = String.Format("Dynomotion Fiducial Alignment - Connected");
            }
            else
            {
                this.Text = "Dynomotion Fiducial Alignment - Disconnected";
            }

            if (KM.WaitToken(100) == KMOTION_TOKEN.KMOTION_LOCKED)
            {
                try
                {
                    MainStatus = KM.GetStatus(false);  // we already have a lock
                    KM.ReleaseToken();
                    XPos.Text = String.Format("{0:F4}", ConvertToUser(MainStatus.Destination[0] / XResolution));
                    YPos.Text = String.Format("{0:F4}", ConvertToUser(MainStatus.Destination[1] / YResolution));
                }
                catch (DMException ex) // in case disconnect in the middle of reading status
                {
                    KM.ReleaseToken();
                    MessageBox.Show(ex.InnerException.Message);
                }
            }

        }

        private void JogXPosMouseDown(object sender, MouseEventArgs e)
        {
            WriteLineHandleException(String.Format("Jog0={0}", JogSpeed));
            JoggingX = true;
        }

        private void JogXNegMouseDown(object sender, MouseEventArgs e)
        {
            WriteLineHandleException(String.Format("Jog0={0}", -JogSpeed));
            JoggingX = true;
        }

        private void JogXStop(object sender, EventArgs e)
        {
            if (JoggingX)
            {
                WriteLineHandleException(String.Format("Jog0={0}", 0));
                JoggingX = false;
            }
        }

        private void JogYPosMouseDown(object sender, MouseEventArgs e)
        {
            WriteLineHandleException(String.Format("Jog1={0}", JogSpeed));
            JoggingY = true;
        }

        private void JogYNegMouseDown(object sender, MouseEventArgs e)
        {
            WriteLineHandleException(String.Format("Jog1={0}", -JogSpeed));
            JoggingY = true;
        }

        private void JogYStop(object sender, EventArgs e)
        {
            if (JoggingY)
            {
                WriteLineHandleException(String.Format("Jog1={0}", 0));
                JoggingY = false;
            }
        }

        private void GoLeft_Click(object sender, EventArgs e)
        {
            XLeftTheo = ConvertToInches(Convert.ToDouble(LeftX.Text));
            YLeftTheo = ConvertToInches(Convert.ToDouble(LeftY.Text));
            WriteLineHandleException(String.Format("Move0={0}", XLeftTheo * XResolution));
            WriteLineHandleException(String.Format("Move1={0}", YLeftTheo * YResolution));
        }

        private void GoRight_Click(object sender, EventArgs e)
        {
            XRightTheo = ConvertToInches(Convert.ToDouble(RightX.Text));
            YRightTheo = ConvertToInches(Convert.ToDouble(RightY.Text));
            WriteLineHandleException(String.Format("Move0={0}", XRightTheo * XResolution));
            WriteLineHandleException(String.Format("Move1={0}", YRightTheo * YResolution));
        }

        private void GoLeftMeas_Click(object sender, EventArgs e)
        {
            XLeftMeasured = ConvertToInches(Convert.ToDouble(LeftXMeas.Text));
            YLeftMeasured = ConvertToInches(Convert.ToDouble(LeftYMeas.Text));
            WriteLineHandleException(String.Format("Move0={0}", XLeftMeasured * XResolution));
            WriteLineHandleException(String.Format("Move1={0}", YLeftMeasured * YResolution));
        }

        private void GoRightMeas_Click(object sender, EventArgs e)
        {
            XRightMeasured = ConvertToInches(Convert.ToDouble(RightXMeas.Text));
            YRightMeasured = ConvertToInches(Convert.ToDouble(RightYMeas.Text));
            WriteLineHandleException(String.Format("Move0={0}", XRightMeasured * XResolution));
            WriteLineHandleException(String.Format("Move1={0}", YRightMeasured * YResolution));
        }

        private void GoRef_Click(object sender, EventArgs e)
        {
            XRef = ConvertToInches(Convert.ToDouble(RefX.Text));
            YRef = ConvertToInches(Convert.ToDouble(RefY.Text));
            WriteLineHandleException(String.Format("Move0={0}", XRef * XResolution));
            WriteLineHandleException(String.Format("Move1={0}", YRef * YResolution));
        }

        private void GoView_Click(object sender, EventArgs e)
        {
            XView = ConvertToInches(Convert.ToDouble(ViewX.Text));
            YView = ConvertToInches(Convert.ToDouble(ViewY.Text));
            WriteLineHandleException(String.Format("Move0={0}", XView * XResolution));
            WriteLineHandleException(String.Format("Move1={0}", YView * YResolution));
        }

        private void MeasureLeft_Click(object sender, EventArgs e)
        {
            XLeftMeasured = MainStatus.Destination[0] / XResolution;
            YLeftMeasured = MainStatus.Destination[1] / XResolution;
            LeftXMeas.Text = ConvertToUser(XLeftMeasured).ToString(UnitsFormat);
            LeftYMeas.Text = ConvertToUser(YLeftMeasured).ToString(UnitsFormat);
        }

        private void MeasureRight_Click(object sender, EventArgs e)
        {
            XRightMeasured = MainStatus.Destination[0] / XResolution;
            YRightMeasured = MainStatus.Destination[1] / XResolution;
            RightXMeas.Text = ConvertToUser(XRightMeasured).ToString(UnitsFormat);
            RightYMeas.Text = ConvertToUser(YRightMeasured).ToString(UnitsFormat);
        }

        private void MeasureRef_Click(object sender, EventArgs e)
        {
            XRef = MainStatus.Destination[0] / XResolution;
            YRef = MainStatus.Destination[1] / XResolution;
            RefX.Text = ConvertToUser(XRef).ToString(UnitsFormat);
            RefY.Text = ConvertToUser(YRef).ToString(UnitsFormat);
        }

        private void MeasureView_Click(object sender, EventArgs e)
        {
            XView = MainStatus.Destination[0] / XResolution;
            YView = MainStatus.Destination[1] / XResolution;
            ViewX.Text = ConvertToUser(XView).ToString(UnitsFormat);
            ViewY.Text = ConvertToUser(YView).ToString(UnitsFormat);
        }

        private void Compute_Click(object sender, EventArgs e)
        {
            // Compute Theta, scale, offset on how to convert a Theoretical point to a Measured Point
            //
            // Starting with a theoretical point 
            //
            // Step #1 rotate it about the origin
            // Step #2 Scale it about the origin
            // Step #3 offset
            //
            double theta_theo = Angle(XRightTheo - XLeftTheo, YRightTheo - YLeftTheo);
            double theta_meas = Angle(XRightMeasured - XLeftMeasured, YRightMeasured - YLeftMeasured);
            theta = NormalizeAngle(theta_meas - theta_theo);

            double length_theo = Length(XRightTheo - XLeftTheo, YRightTheo - YLeftTheo);
            double length_meas = Length(XRightMeasured - XLeftMeasured, YRightMeasured - YLeftMeasured);
            scale = length_meas / length_theo;

            offsetx = XLeftMeasured - scale * (XLeftTheo * Math.Cos(theta) - YLeftTheo * Math.Sin(theta));
            offsety = YLeftMeasured - scale * (XLeftTheo * Math.Sin(theta) + YLeftTheo * Math.Cos(theta));
            WriteGeo();
        }


        private void ComputeUnity_click(object sender, EventArgs e)
        {
            theta = 0;
            scale = 1.0;
            offsetx = 0.0;
            offsety = 0.0;
            WriteGeo();
        }


        private void WriteGeo()
        {
            string mapfile = MainPath + "\\KMotion\\Data\\FiducialsMap.txt";
            System.IO.StreamWriter file = new System.IO.StreamWriter(mapfile);
            file.WriteLine("2,2");        // rows, cols
            file.WriteLine("1,1");        // gridx gridy
            file.WriteLine("-0.5,-0.5");  // centerX Centery

            WriteGrid(0, 0, ref file);
            WriteGrid(0, 1, ref file);
            WriteGrid(1, 0, ref file);
            WriteGrid(1, 1, ref file);

            file.Close();

            MapFile.Text = String.Format("{0} Created", mapfile);

            IntPtr KMotionCNCWindow = FindWindow("KMotionCNC", null);

            uint WM_COMMAND = 0x0111;
            int ID_ReloadGeoCorrection = 33016;
            PostMessage(KMotionCNCWindow, WM_COMMAND, ID_ReloadGeoCorrection, 0);

            Theta.Text = (theta * 180.0 / Math.PI).ToString("F4");
            ScaleFactor.Text = scale.ToString("F4");
            XOff.Text = ConvertToUser(offsetx).ToString(UnitsFormat);
            YOff.Text = ConvertToUser(offsety).ToString(UnitsFormat);
        }



        private void WriteGrid(int row, int col, ref System.IO.StreamWriter f)
        {
            double Centerx = 0.5;
            double Centery = 0.5;
            double x = col - Centerx;
            double y = row - Centery;
            f.WriteLine(String.Format("{0},{1},{2:n6},{3:n6},0", row, col, TransformX(x, y), TransformY(x, y)));
        }


        private double TransformX(double x, double y)
        {
            return (scale * (x * Math.Cos(theta) - y * Math.Sin(theta))) + offsetx;
        }
        
        private double TransformY(double x, double y)
        {
            return (scale * (x * Math.Sin(theta) + y * Math.Cos(theta))) + offsety;
        }

 
        private double NormalizeAngle(double theta)
        {
            while (theta < Math.PI) theta += 2 * Math.PI;
            while (theta > Math.PI) theta -= 2 * Math.PI;
            return theta;
        }

        private double Length(double dx, double dy)
        {
            return Math.Sqrt(dx*dx+dy*dy);
        }

        private double Angle(double dx, double dy)
        {
            double theta;

            if (Math.Abs(dx) < Math.Abs(dy))
                theta = Math.Atan2(dy, dx);
            else
                theta = Math.PI/2 - Math.Atan2(dx, dy);

            return theta;
        }


        private int SelectVideoDevice(int Index)
        {
            try
            {
                // Get current devices and dispose of capture object
                // because the video and audio device can only be changed
                // by creating a new Capture object.
                Filter videoDevice = null;
                Filter audioDevice = null;
                if (capture != null)
                {
                    videoDevice = capture.VideoDevice;
                    audioDevice = capture.AudioDevice;
                    capture.Dispose();
                    capture = null;
                }

                // Get new video device
                if (filters.VideoInputDevices.Count <= Index) return 1;

                videoDevice = filters.VideoInputDevices[Index];

                // Create capture object
                if ((videoDevice != null) || (audioDevice != null))
                {
                    capture = new Capture(videoDevice, audioDevice, false);
                    capture.AllowSampleGrabber = true;
                    capture.CaptureComplete += new EventHandler(OnCaptureComplete);

                    SampleGrabber(true);
                    
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Video device not supported.\n\n" + ex.Message + "\n\n" + ex.ToString());
                return 1;
            }
            return 0;
        }

        private void OnCaptureComplete(object sender, EventArgs e)
        {
            // Demonstrate the Capture.CaptureComplete event.
        }


        private void SampleGrabber(bool Enable)
        {
            if (Enable)
            {
                if (this.capture != null)
                {
                    // Set flag only if capture device is initialized
                    this.capture.FrameEvent2 += new Capture.HeFrame(this.CaptureDone);
                }
            }
            else
            {
                if (this.capture != null)
                {
                    this.capture.DisableEvent();
                    this.capture.FrameEvent2 -= new Capture.HeFrame(this.CaptureDone);
                }
            }
        }



        private void CaptureDone(System.Drawing.Bitmap e)
        {
            ViewScreen.Image = e;
            // Show only the selected frame ...
            // If you want to capture all frames, then remove the next line
            //this.capture.FrameEvent2 -= new Capture.HeFrame(this.CaptureDone); 
        }



        int DragMouseStartX = 0;
        int DragMouseStartY = 0;
        double DragAxisStartX = 0;
        double DragAxisStartY = 0;

        private void DragDown(object sender, MouseEventArgs e)
        {
            DragMouseStartX = e.X;
            DragMouseStartY = e.Y;
            if (MainStatus.Destination != null)
            {
                DragAxisStartX = MainStatus.Destination[0] / XResolution;
                DragAxisStartY = MainStatus.Destination[1] / XResolution;
            }
        }

        private void DragMove(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left && MainStatus.Destination != null)
            {
                PropertyInfo pInfo = ViewScreen.GetType().GetProperty("ImageRectangle",
                    System.Reflection.BindingFlags.Public |
                    System.Reflection.BindingFlags.NonPublic |
                    System.Reflection.BindingFlags.Instance);

                Rectangle rectangle = (Rectangle)pInfo.GetValue(ViewScreen, null);
                MouseMag = Convert.ToDouble(MagFactor.Text) / rectangle.Width;  // microscope magnification inches per pixel

                double dMouseX = e.X - DragMouseStartX;
                double dMouseY = e.Y - DragMouseStartY;

                double NewX = DragAxisStartX - MouseMag * dMouseX;
                double NewY = DragAxisStartY + MouseMag * dMouseY;

                WriteLineHandleException(String.Format("MoveExp0={0} 0.2", NewX * XResolution));
                WriteLineHandleException(String.Format("MoveExp1={0} 0.2", NewY * YResolution));
            }
        }

        private void DragUp(object sender, MouseEventArgs e)
        {
            WriteLineHandleException(String.Format("Jog0=0"));
            WriteLineHandleException(String.Format("Jog1=0"));
        }

        private void ViewCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            try
            {
                if (ViewCheckBox.Checked)
                {
                    if (capture == null || capture.PreviewWindow == null)
                    {
                        if (SelectVideoDevice(VideoDevice)!=0) return;
                        capture.PreviewWindow = panelVideo;
                        this.capture.GrapImg();
                        panelVideo.Visible = false;
                    }
                }
                else
                {
                    if (capture != null && capture.PreviewWindow != null)
                        capture.PreviewWindow = null;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Unable to enable/disable preview. Please submit a bug report.\n\n" + ex.Message + "\n\n" + ex.ToString());
            }
        }

        private void View_screen_paint(object sender, PaintEventArgs e)
        {
            if (Sight.Checked)
            {
                Graphics g = e.Graphics;

                Pen redPen = new Pen(Color.FromArgb(128, 255, 0, 0), 2);

                Type pboxType = ViewScreen.GetType();
                PropertyInfo irProperty = pboxType.GetProperty("ImageRectangle", BindingFlags.GetProperty | BindingFlags.NonPublic | BindingFlags.Instance);
                Rectangle rectangle = (Rectangle)irProperty.GetValue(ViewScreen, null);

                int H = rectangle.Height - 1;
                int W = rectangle.Width - 1;
                int X = rectangle.Left;
                int Y = rectangle.Top;
                int Small = W / 4;
                int Large = W / 2;
                g.DrawLine(redPen, 0+X, 0+Y, W+X, H+Y);
                g.DrawLine(redPen, 0+X, H+Y, W+X, 0+Y);
                g.DrawLine(redPen, W / 2+X, H+Y, W / 2+X, 0+Y);
                g.DrawLine(redPen, 0+X, H / 2+Y, W+X, H / 2+Y);
                g.DrawEllipse(redPen, W / 2 - Small / 2+X, H / 2 - Small / 2+Y, Small, Small);
                g.DrawEllipse(redPen, W / 2 - Large / 2+X, H / 2 - Large / 2+Y, Large, Large);
                
                redPen.Dispose();
            }
        }

        private double ConvertToUser(double v)
        {
            if (Unitsmm)
                return v * 25.4;
            else
                return v;
        }

        private double ConvertToInches(double v)
        {
            if (Unitsmm)
                return v / 25.4;
            else
                return v;
        }

        private void radioInch_CheckedChanged(object sender, EventArgs e)
        {
            Unitsmm = radiomm.Checked;
            if (Unitsmm) UnitsFormat = "F4"; else UnitsFormat = "F3"; 
            LeftX.Text = ConvertToUser(XLeftTheo).ToString(UnitsFormat);
            LeftY.Text = ConvertToUser(YLeftTheo).ToString(UnitsFormat);
            RightX.Text = ConvertToUser(XRightTheo).ToString(UnitsFormat);
            RightY.Text = ConvertToUser(YRightTheo).ToString(UnitsFormat);
            LeftXMeas.Text = ConvertToUser(XLeftMeasured).ToString(UnitsFormat);
            LeftYMeas.Text = ConvertToUser(YLeftMeasured).ToString(UnitsFormat);
            RightXMeas.Text = ConvertToUser(XRightMeasured).ToString(UnitsFormat);
            RightYMeas.Text = ConvertToUser(YRightMeasured).ToString(UnitsFormat);
        }

        private void RefX_TextChanged(object sender, EventArgs e)
        {
            try
            {
                XRef = ConvertToInches(Convert.ToDouble(RefX.Text));
            }
            catch  // no error if invalid
            {
                XRef = 0;
            }

            XMicroOffset = XRef - XView;
            MicroOffsetX.Text = ConvertToUser(XMicroOffset).ToString(UnitsFormat);
        }

        private void ViewX_TextChanged(object sender, EventArgs e)
        {
            try
            {
                XView = ConvertToInches(Convert.ToDouble(ViewX.Text));
            }
            catch  // no error if invalid
            {
                XView = 0;
            }

            XMicroOffset = XRef - XView;
            MicroOffsetX.Text = ConvertToUser(XMicroOffset).ToString(UnitsFormat);
        }

        private void RefY_TextChanged(object sender, EventArgs e)
        {
            try
            {
                YRef = ConvertToInches(Convert.ToDouble(RefY.Text));
            }
            catch  // no error if invalid
            {
                YRef = 0;
            }

            YMicroOffset = YRef - YView;
            MicroOffsetY.Text = ConvertToUser(YMicroOffset).ToString(UnitsFormat);
        }

        private void ViewY_TextChanged(object sender, EventArgs e)
        {
            try
            {
                YView = ConvertToInches(Convert.ToDouble(ViewY.Text));
            }
            catch  // no error if invalid
            {
                YView = 0;
            }

            YMicroOffset = YRef - YView;
            MicroOffsetY.Text = ConvertToUser(YMicroOffset).ToString(UnitsFormat);
        }
    }
}
