/*********************************************************************/
/*         Copyright (c) 2003-2016  DynoMotion Incorporated          */
/*********************************************************************/
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
    public partial class MainForm : Form
    {
        String MainPath;
        KM_Controller KM;
        HostComm Host;
        Grab Grabber;
        Align Aligner;

        bool JoggingX = false;
        bool JoggingY = false;
        bool JoggingZ = false;
        bool JoggingC = false;
        double JoggingXSpeed = 0.0;
        double JoggingYSpeed = 0.0;
        double JoggingZSpeed = 0.0;
        double JoggingCSpeed = 0.0;

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

        double theta = 0;
        double scale = 1;
        double offsetx = 0;
        double offsety = 0;

        string SettingsFile;
        string MicroScopeOffsetFile;
        string TargetParameterFile;
        double JogSpeed = 400;
        int VideoDevice = 1;
        double MouseMag = 0.25;
        bool Unitsmm = false;
        string UnitsFormat;

        double ZFactor = 0.744*2;
        double ThetaFactor = 1.29;


        // Target Alignment Parameters
        double TargetBarWidthX;
        double TargetBarWidthY;
        double TargetBarWidthTolX;
        double TargetBarWidthTolY;
        double TargetBarMinHeightX;
        double TargetBarMinHeightY;
        double CalRows, CalCols, CalGridX, CalGridY;
        int OrthoValue;
        int FilterLevel;
        double CalDelay;

        int FrameCount = 0;
        KM_MainStatus MainStatus;

        private Capture capture = null;
        private Filters filters = new Filters();

        double Status_x=0;
        double Status_y=0;
        double Status_z=0;
        double Status_c=0;

        public MainForm()
        {
            Host = new HostComm(Handle);
            Grabber = new Grab();
            Aligner = new Align();

            InitializeComponent();

            MainPath = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            MainPath = System.IO.Path.GetDirectoryName(MainPath);
            MainPath = System.IO.Path.GetDirectoryName(MainPath);

            FiducialsName.Text = MainPath + "\\PC VCS Examples\\MeasureFiducials\\FiducialLocations.txt";
            SettingsFile = MainPath + "\\PC VCS Examples\\MeasureFiducials\\MeasureFiducialSettings.txt";
            MicroScopeOffsetFile = MainPath + "\\PC VCS Examples\\MeasureFiducials\\MicroscopeOffset.txt";
            TargetParameterFile = MainPath + "\\PC VCS Examples\\MeasureFiducials\\TargetParameterFile.txt";
            KM = new KMotion_dotNet.KM_Controller();
            ReadSettings();
            ReadFiducials();
            ReadTargetParameters();
            ReadMicroscopeOffset();

            ViewCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;

            ProfileRadioXY.Checked = true;
            ProfileRadioZ.Checked = ProfileRadioTheta.Checked = false;
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
                double d = 0;
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
            int[] List;
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

                    UpdatePositionsWorkerThread();
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
            JoggingXSpeed = JogSpeed;
            JoggingX = true;
            Host.DoJog(JoggingXSpeed, JoggingYSpeed, JoggingZSpeed, JoggingCSpeed);
        }


        private void JogXNegMouseDown(object sender, MouseEventArgs e)
        {
            JoggingXSpeed = -JogSpeed;
            JoggingX = true;
            Host.DoJog(JoggingXSpeed, JoggingYSpeed, JoggingZSpeed, JoggingCSpeed);
        }

        private void JogXStop(object sender, EventArgs e)
        {
            if (JoggingX)
            {
                JoggingXSpeed = 0.0;
                JoggingX = false;
                Host.DoJog(JoggingXSpeed, JoggingYSpeed, JoggingZSpeed, JoggingCSpeed);
            }
        }

        private void JogYPosMouseDown(object sender, MouseEventArgs e)
        {
            JoggingYSpeed = JogSpeed;
            JoggingY = true;
            Host.DoJog(JoggingXSpeed, JoggingYSpeed, JoggingZSpeed, JoggingCSpeed);
        }

        private void JogYNegMouseDown(object sender, MouseEventArgs e)
        {
            JoggingYSpeed = -JogSpeed;
            JoggingY = true;
            Host.DoJog(JoggingXSpeed, JoggingYSpeed, JoggingZSpeed, JoggingCSpeed);
        }
        private void JogCPosMouseDown(object sender, MouseEventArgs e)
        {
            JoggingCSpeed = JogSpeed;
            JoggingC = true;
            Host.DoJog(JoggingXSpeed, JoggingYSpeed, JoggingZSpeed, JoggingCSpeed);
        }

        private void JogZPosMouseDown(object sender, MouseEventArgs e)
        {
            JoggingZSpeed = JogSpeed * 0.25;
            JoggingZ = true;
            Host.DoJog(JoggingXSpeed, JoggingYSpeed, JoggingZSpeed, JoggingCSpeed);
        }

        private void JogCNegMouseDown(object sender, MouseEventArgs e)
        {
            JoggingCSpeed = -JogSpeed;
            JoggingC = true;
            Host.DoJog(JoggingXSpeed, JoggingYSpeed, JoggingZSpeed, JoggingCSpeed);
        }

        private void JogZNegMouseDown(object sender, MouseEventArgs e)
        {
            JoggingZSpeed = -JogSpeed * 0.25;
            JoggingZ = true;
            Host.DoJog(JoggingXSpeed, JoggingYSpeed, JoggingZSpeed, JoggingCSpeed);
        }



        private void JogYStop(object sender, EventArgs e)
        {
            if (JoggingY)
            {
                JoggingYSpeed = 0.0;
                JoggingY = false;
                Host.DoJog(JoggingXSpeed, JoggingYSpeed, JoggingZSpeed, JoggingCSpeed);
            }
        }

        private void JogCStop(object sender, EventArgs e)
        {
            if (JoggingC)
            {
                JoggingCSpeed = 0.0;
                JoggingC = false;
                Host.DoJog(JoggingXSpeed, JoggingYSpeed, JoggingZSpeed, JoggingCSpeed);
            }
        }

        private void JogZStop(object sender, EventArgs e)
        {
            if (JoggingZ)
            {
                JoggingZSpeed = 0.0;
                JoggingZ = false;
                Host.DoJog(JoggingXSpeed, JoggingYSpeed, JoggingZSpeed, JoggingZSpeed);
            }

        }



        int DragMouseStartX = 0;
        int DragMouseStartY = 0;
        double DragAxisStartX = 0;
        double DragAxisStartY = 0;

        private void DragDown(object sender, MouseEventArgs e)
        {
            DragMouseStartX = e.X;
            DragMouseStartY = e.Y;

            DragAxisStartX = Status_x;
            DragAxisStartY = Status_y;
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
                double MouseMagScreenPixel = Convert.ToDouble(MagFactor.Text) / rectangle.Width;  // microscope magnification inches per pixel

                double dMouseX = e.X - DragMouseStartX;
                double dMouseY = e.Y - DragMouseStartY;

                double NewX = DragAxisStartX - MouseMagScreenPixel * dMouseX;
                double NewY = DragAxisStartY + MouseMagScreenPixel * dMouseY;

                Host.MoveExpXY(NewX, NewY);
            }
        }

        private void DragUp(object sender, MouseEventArgs e)
        {
            Host.JogStop();
        }

        private void GoLeft_Click(object sender, EventArgs e)
        {
            XLeftTheo = ConvertToInches(Convert.ToDouble(LeftX.Text));
            YLeftTheo = ConvertToInches(Convert.ToDouble(LeftY.Text));
            Host.MoveXY(XLeftTheo, YLeftTheo);
        }


        private void GoRight_Click(object sender, EventArgs e)
        {
            XRightTheo = ConvertToInches(Convert.ToDouble(RightX.Text));
            YRightTheo = ConvertToInches(Convert.ToDouble(RightY.Text));
            Host.MoveXY(XRightTheo, YRightTheo);
        }

        private void GoLeftMeas_Click(object sender, EventArgs e)
        {
            XLeftMeasured = ConvertToInches(Convert.ToDouble(LeftXMeas.Text));
            YLeftMeasured = ConvertToInches(Convert.ToDouble(LeftYMeas.Text));
            Host.MoveXY(XLeftMeasured, YLeftMeasured);
        }

        private void GoRightMeas_Click(object sender, EventArgs e)
        {
            XRightMeasured = ConvertToInches(Convert.ToDouble(RightXMeas.Text));
            YRightMeasured = ConvertToInches(Convert.ToDouble(RightYMeas.Text));
            Host.MoveXY(XRightMeasured, YRightMeasured);
        }

        private void GoRef_Click(object sender, EventArgs e)
        {
            XRef = ConvertToInches(Convert.ToDouble(RefX.Text));
            YRef = ConvertToInches(Convert.ToDouble(RefY.Text));
            Host.MoveXY(XRef, YRef);
        }

        private void GoView_Click(object sender, EventArgs e)
        {
            XView = ConvertToInches(Convert.ToDouble(ViewX.Text));
            YView = ConvertToInches(Convert.ToDouble(ViewY.Text));
            Host.MoveXY(XView, YView);
        }

        private void MeasureLeft_Click(object sender, EventArgs e)
        {
            XLeftMeasured = Status_x;
            YLeftMeasured = Status_y;
            LeftXMeas.Text = ConvertToUser(XLeftMeasured).ToString(UnitsFormat);
            LeftYMeas.Text = ConvertToUser(YLeftMeasured).ToString(UnitsFormat);
        }

        private void MeasureRight_Click(object sender, EventArgs e)
        {
            XRightMeasured = Status_x;
            YRightMeasured = Status_y;
            RightXMeas.Text = ConvertToUser(XRightMeasured).ToString(UnitsFormat);
            RightYMeas.Text = ConvertToUser(YRightMeasured).ToString(UnitsFormat);
        }

        private void MeasureRef_Click(object sender, EventArgs e)
        {
            XRef = Status_x;
            YRef = Status_y;
            RefX.Text = ConvertToUser(XRef).ToString(UnitsFormat);
            RefY.Text = ConvertToUser(YRef).ToString(UnitsFormat);
        }

        private void MeasureView_Click(object sender, EventArgs e)
        {
            XView = Status_x;
            YView = Status_y;
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

            Host.SendMessageService(HostComm.ID_ReloadGeoCorrection);

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
            return Math.Sqrt(dx * dx + dy * dy);
        }

        private double Angle(double dx, double dy)
        {
            double theta;

            if (Math.Abs(dx) < Math.Abs(dy))
                theta = Math.Atan2(dy, dx);
            else
                theta = Math.PI / 2 - Math.Atan2(dx, dy);

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



                    capture.FrameRate = 15;
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

            //           ViewScreen.Image = e;

            if (++FrameCount > 4)  // throw away some frames
            {
                FrameCount = 0;
                Grabber.SaveImage(e);
                ViewScreen.Invalidate();
            }

            // Show only the selected frame ...
            // If you want to capture all frames, then remove the next line
            //this.capture.FrameEvent2 -= new Capture.HeFrame(this.CaptureDone); 
        }


        private void ViewCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            try
            {
                if (ViewCheckBox.Checked)
                {
                    if (capture == null || capture.PreviewWindow == null)
                    {
                        if (SelectVideoDevice(VideoDevice) != 0) return;
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
            Bitmap map;
            String err = "";

            map = Grabber.MakeBitmap();

            if (map != null)
            {
                Grabber.GetImage(map, (float)(OrthoTrackBar.Value / 100.0));
                Grabber.FilterProfiles(FilterLevel);
                ViewScreen.Image = map;
            }


            Graphics g = e.Graphics;
            Type pboxType = ViewScreen.GetType();
            PropertyInfo irProperty = pboxType.GetProperty("ImageRectangle", BindingFlags.GetProperty | BindingFlags.NonPublic | BindingFlags.Instance);
            Rectangle rectangle = (Rectangle)irProperty.GetValue(ViewScreen, null);

            if (Sight.Checked)
            {
                Pen redPen = new Pen(Color.FromArgb(128, 255, 0, 0), 2);
                int H = rectangle.Height - 1;
                int W = rectangle.Width - 1;
                int X = rectangle.Left;
                int Y = rectangle.Top;
                int Small = W / 4;
                int Large = W / 2;
                g.DrawLine(redPen, 0 + X, 0 + Y, W + X, H + Y);
                g.DrawLine(redPen, 0 + X, H + Y, W + X, 0 + Y);
                g.DrawLine(redPen, W / 2 + X, H + Y, W / 2 + X, 0 + Y);
                g.DrawLine(redPen, 0 + X, H / 2 + Y, W + X, H / 2 + Y);
                g.DrawEllipse(redPen, W / 2 - Small / 2 + X, H / 2 - Small / 2 + Y, Small, Small);
                g.DrawEllipse(redPen, W / 2 - Large / 2 + X, H / 2 - Large / 2 + Y, Large, Large);

                redPen.Dispose();
            }


            if (TargetCheckBox.Checked)
            {
                // Align Cross
                Aligner.AlignALL(Grabber, TargetBarWidthX, TargetBarWidthY, TargetBarWidthTolX, TargetBarWidthTolY, TargetBarMinHeightX, TargetBarMinHeightY, ref err);

                Grabber.AddTarget(g, rectangle, Aligner.TargetX, Aligner.TargetY);
                Grabber.AddDiagonalTarget(g, rectangle, Aligner.TargetDiagonalX, Aligner.TargetDiagonalY);

                double heightXL = Math.Abs(Aligner.EdgeBufX.EdgeH[Aligner.TargetX.TargetL]);
                double heightXR = Math.Abs(Aligner.EdgeBufX.EdgeH[Aligner.TargetX.TargetR]);
                double heightYL = Math.Abs(Aligner.EdgeBufY.EdgeH[Aligner.TargetY.TargetL]);
                double heightYR = Math.Abs(Aligner.EdgeBufY.EdgeH[Aligner.TargetY.TargetR]);
                double widthX = Aligner.EdgeBufX.EdgeC[Aligner.TargetX.TargetR] - Aligner.EdgeBufX.EdgeC[Aligner.TargetX.TargetL];
                double widthY = Aligner.EdgeBufY.EdgeC[Aligner.TargetY.TargetR] - Aligner.EdgeBufY.EdgeC[Aligner.TargetY.TargetL];

                // mouse mag is the size in inches of the width of the image
                double AdjX = ConvertToUser(Aligner.TargetX.Adj) / Aligner.TargetX.ImageSizeInPixels * MouseMag;
                double AdjY = ConvertToUser(Aligner.TargetY.Adj) / Aligner.TargetX.ImageSizeInPixels * MouseMag;
                double AdjDX = ConvertToUser(Aligner.TargetDiagonalX.Adj) / Aligner.TargetDiagonalX.ImageSizeInPixels * MouseMag;
                double AdjDY = ConvertToUser(Aligner.TargetDiagonalY.Adj) / Aligner.TargetDiagonalX.ImageSizeInPixels * MouseMag;

                double Theta = (Aligner.TargetLeft.Adj - Aligner.TargetRight.Adj) / Aligner.TargetX.ImageSizeInPixels * MouseMag * 180 / Math.PI;

                //               if (err == "")
                {
                    if (Unitsmm)
                    {
                        AlignResults.Text = FigureSpace(String.Format("Widths{0,3:F0} {1,3:F0} Heights {2,3:F0} {3,3:F0} XY {4:##.000} {5:##.000} mm", widthX, widthY, Math.Min(heightXL, heightXR), Math.Min(heightYL, heightYR), AdjX, AdjY));

                        AlignResults2.Text = FigureSpace(String.Format("Z {0:##.000} {1:##.000} mm  ThetaL{2:##.000} ThetaR{3:##.000} Theta{4:##.000}", AdjDX, AdjDY, Aligner.TargetLeft.Adj, Aligner.TargetRight.Adj, Theta));
                    }
                    else
                    {
                        AlignResults.Text = FigureSpace(String.Format("Widths{0,3:F0} {1,3:F0} Heights {2,3:F0} {3,3:F0} XY {4:##.000} {5:##.000} in", widthX, widthY, Math.Min(heightXL, heightXR), Math.Min(heightYL, heightYR), AdjX, AdjY));

                        AlignResults2.Text = FigureSpace(String.Format("Z {0:##.000} {1:##.000} in  ThetaLR{2:##.000} {3:##.000} Theta{4:##.000}", AdjDX, AdjDY, Aligner.TargetLeft.Adj, Aligner.TargetRight.Adj, Theta));

                    }
                }
                //               else
                //                {
                //                    AlignResults.Text = err;
                //                }
            }

            if (ProfileRadioXY.Checked)
                Grabber.AddPlots(g, rectangle, Grabber.XProfile, Grabber.YProfile, Aligner.EdgeBufX, Aligner.EdgeBufY, Aligner.TargetX, Aligner.TargetY);
            if (ProfileRadioZ.Checked)
                Grabber.AddPlotsDiagonal(g, rectangle, Grabber.XDiagonalProfile, Grabber.YDiagonalProfile, Aligner.EdgeBufDiagonalX, Aligner.EdgeBufDiagonalY, Aligner.TargetDiagonalX, Aligner.TargetDiagonalY);
            if (ProfileRadioTheta.Checked)
                Grabber.AddPlotsDouble(g, rectangle, Grabber.LeftProfile, Grabber.RightProfile, Aligner.EdgeBufLeft, Aligner.EdgeBufRight, Aligner.TargetLeft, Aligner.TargetRight);

        }

        private string FigureSpace(string v)
        {
            return v.Replace(" ", "  ");
        }

        private void AlignXY_click(object sender, EventArgs e)
        {
            // Create the thread object. This does not start the thread.
            Thread workerThread = new Thread(DoAlignXY);
            // Start the worker thread.
            workerThread.Start();
        }

        private void DoAlignXY()
        {
            String err = "";

            Aligner.AlignALL(Grabber, TargetBarWidthX, TargetBarWidthY, TargetBarWidthTolX, TargetBarWidthTolY, TargetBarMinHeightX, TargetBarMinHeightY, ref err);

            if (Aligner.TargetX.TargetFound && Aligner.TargetY.TargetFound)
            {
                double AdjX = Aligner.TargetX.Adj / Aligner.TargetX.ImageSizeInPixels * MouseMag;
                double AdjY = Aligner.TargetY.Adj / Aligner.TargetX.ImageSizeInPixels * MouseMag;
                Host.MoveXYRel(AdjX, AdjY);
            }
        }

        private void AlignZ_click(object sender, EventArgs e)
        {
            // Create the thread object. This does not start the thread.
            Thread workerThread = new Thread(DoAlignZ);
            // Start the worker thread.
            workerThread.Start();
        }

        private void DoAlignZ()
        {
            String err = "";

            Aligner.AlignALL(Grabber, TargetBarWidthX, TargetBarWidthY, TargetBarWidthTolX, TargetBarWidthTolY, TargetBarMinHeightX, TargetBarMinHeightY, ref err);

            if (Aligner.TargetDiagonalX.TargetFound && Aligner.TargetDiagonalY.TargetFound)
            {
                double w = Aligner.TargetDiagonalX.Adj;
                double v = Aligner.TargetDiagonalY.Adj;
                double x = (w + v) * 0.5f * Grabber.Diagonaldx;  // now we have X offset in Frame Grabber Pixels

                double z = x / Aligner.TargetX.ImageSizeInPixels * MouseMag * ZFactor;  // laser Angle factor
                Host.MoveZRel(z);
            }
        }


        private void AlignC_click(object sender, EventArgs e)
        {
            // Create the thread object. This does not start the thread.
            Thread workerThread = new Thread(DoAlignC);
            // Start the worker thread.
            workerThread.Start();
        }

        private void DoAlignC()
        {
            String err = "";
            Aligner.AlignALL(Grabber, TargetBarWidthX, TargetBarWidthY, TargetBarWidthTolX, TargetBarWidthTolY, TargetBarMinHeightX, TargetBarMinHeightY, ref err);

            if (Aligner.TargetLeft.TargetFound && Aligner.TargetRight.TargetFound)
            {
                double Theta = (Aligner.TargetLeft.Adj - Aligner.TargetRight.Adj) / Aligner.TargetX.ImageSizeInPixels * MouseMag * 180 / Math.PI * ThetaFactor;
                Host.MoveCRel(-Theta);
            }
        }

        private void AlignAll_click(object sender, EventArgs e)
        {
            // Create the thread object. This does not start the thread.
            Thread workerThread = new Thread(DoAlignAll);
            // Start the worker thread.
            workerThread.Start();
        }

        private void DoAlignAll()
        {
            String err = "";
            double AdjX = 0, AdjY = 0, z = 0, Theta = 0;

            Aligner.AlignALL(Grabber, TargetBarWidthX, TargetBarWidthY, TargetBarWidthTolX, TargetBarWidthTolY, TargetBarMinHeightX, TargetBarMinHeightY, ref err);

            if (Aligner.TargetX.TargetFound && Aligner.TargetY.TargetFound)
            {
                AdjX = Aligner.TargetX.Adj / Aligner.TargetX.ImageSizeInPixels * MouseMag;
                AdjY = Aligner.TargetY.Adj / Aligner.TargetX.ImageSizeInPixels * MouseMag;
            }

            if (Aligner.TargetDiagonalX.TargetFound && Aligner.TargetDiagonalY.TargetFound)
            {
                double w = Aligner.TargetDiagonalX.Adj;
                double v = Aligner.TargetDiagonalY.Adj;
                double x = (w + v) * 0.5f * Grabber.Diagonaldx;  // now we have X offset in Frame Grabber Pixels

                z = x / Aligner.TargetX.ImageSizeInPixels * MouseMag * ZFactor;  // laser Angle factor
            }

            if (Aligner.TargetLeft.TargetFound && Aligner.TargetRight.TargetFound)
            {
                Theta = (Aligner.TargetLeft.Adj - Aligner.TargetRight.Adj) / Aligner.TargetX.ImageSizeInPixels * MouseMag * 180 / Math.PI * ThetaFactor;
            }

            Host.MoveXYZTRel(AdjX, AdjY, z, -Theta);
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


        protected override void WndProc(ref Message m)
        {
            switch (m.Msg)
            {
                case HostComm.WM_COPYDATA:
                    COPYDATASTRUCT_RECEIVE mystr = new COPYDATASTRUCT_RECEIVE();
                    Type mytype = mystr.GetType();
                    mystr = (COPYDATASTRUCT_RECEIVE)m.GetLParam(mytype);
                    Host.InterProcessString = mystr.lpData;
                    Host.InterProcessStringReceived = true;
                    break;
            }
            base.WndProc(ref m);
        }

        private bool ApplyTargetParameters()
        {
            try
            {
                TargetBarWidthX = Convert.ToDouble(TargetBarWidthTextBoxX.Text);
                TargetBarWidthY = Convert.ToDouble(TargetBarWidthTextBoxY.Text);
                TargetBarWidthTolX = Convert.ToDouble(TargetBarWidthTolTextBoxX.Text);
                TargetBarWidthTolY = Convert.ToDouble(TargetBarWidthTolTextBoxY.Text);
                TargetBarMinHeightX = Convert.ToDouble(TargetBarMinHeightTextBoxX.Text);
                TargetBarMinHeightY = Convert.ToDouble(TargetBarMinHeightTextBoxY.Text);
                FilterLevel = Convert.ToInt32(FilterLevelTextBox.Text);
                CalCols = Convert.ToInt32(CalColsTextBox.Text);
                CalRows = Convert.ToInt32(CalRowsTextBox.Text);
                CalGridX = Convert.ToInt32(CalGridXTextBox.Text);
                CalGridY = Convert.ToInt32(CalGridYTextBox.Text);
                CalDelay = Convert.ToDouble(CalDelayTextBox.Text);
                OrthoValue = Convert.ToInt32(OrthoTextBox.Text);
            }
            catch (Exception)
            {
                MessageBox.Show("Invalid Target Parameter");
                return false;
            }

            return true;
        }

        private void TabChanged(object sender, EventArgs e)
        {
            ApplyTargetParameters();
        }

        private void SaveTargetParameters(object sender, EventArgs e)
        {
            if (ApplyTargetParameters())
            {
                try
                {
                    System.IO.StreamWriter file = new System.IO.StreamWriter(TargetParameterFile);
                    file.WriteLine(String.Format("TargetBarWidth {0:F4} {1:F4}", TargetBarWidthX, TargetBarWidthY));
                    file.WriteLine(String.Format("TargetBarWidthTol {0:F4} {1:F4}", TargetBarWidthTolX, TargetBarWidthTolY));
                    file.WriteLine(String.Format("TargetBarMinHeight {0:F4} {1:F4}", TargetBarMinHeightX, TargetBarMinHeightY));
                    file.WriteLine(String.Format("FilterLevel {0}", FilterLevel));
                    file.WriteLine(String.Format("CalRC {0:F4} {1:F4}", CalRows, CalCols));
                    file.WriteLine(String.Format("CalGrid {0:F4} {1:F4}", CalGridX, CalGridY));
                    file.WriteLine(String.Format("CalDelay {0:F4}", CalDelay));
                    file.WriteLine(String.Format("OrthoValue {0:F4}", OrthoValue));
                    file.Close();
                }
                catch
                {
                    MessageBox.Show(String.Format("Unable to write file: {0}", TargetParameterFile));
                }
            }
        }

        private void ReadTargetParameters()
        {
            try
            {
                System.IO.StreamReader file = new System.IO.StreamReader(TargetParameterFile);
                GetSetting2(file, "TargetBarWidth", ref TargetBarWidthX, ref TargetBarWidthY);
                TargetBarWidthTextBoxX.Text = TargetBarWidthX.ToString();
                TargetBarWidthTextBoxY.Text = TargetBarWidthX.ToString();
                GetSetting2(file, "TargetBarWidthTol", ref TargetBarWidthTolX, ref TargetBarWidthTolY);
                TargetBarWidthTolTextBoxX.Text = TargetBarWidthTolX.ToString();
                TargetBarWidthTolTextBoxY.Text = TargetBarWidthTolY.ToString();
                GetSetting2(file, "TargetBarMinHeight", ref TargetBarMinHeightX, ref TargetBarMinHeightY);
                TargetBarMinHeightTextBoxX.Text = TargetBarMinHeightX.ToString();
                TargetBarMinHeightTextBoxY.Text = TargetBarMinHeightY.ToString();
                double v = 0;
                GetSetting(file, "FilterLevel", ref v);
                FilterLevel = (int)v;
                FilterLevelTextBox.Text = FilterLevel.ToString();
                GetSetting2(file, "CalRC", ref CalRows, ref CalCols);
                CalRowsTextBox.Text = CalRows.ToString();
                CalColsTextBox.Text = CalCols.ToString();
                GetSetting2(file, "CalGrid", ref CalGridX, ref CalGridY);
                CalGridXTextBox.Text = CalGridX.ToString();
                CalGridYTextBox.Text = CalGridY.ToString();
                GetSetting(file, "CalDelay", ref CalDelay);
                CalDelayTextBox.Text = CalDelay.ToString();
                GetSetting(file, "OrthoValue", ref v);
                OrthoValue = (int)v;
                OrthoTextBox.Text = OrthoValue.ToString();
                OrthoTrackBar.Value = OrthoValue;

                file.Close();

                FilterLevelTextBox.Text = FilterLevel.ToString();
            }
            catch
            {
                MessageBox.Show(String.Format("Unable to read file: {0}", TargetParameterFile));
            }
        }

        private void MouseMagLeaveFocus(object sender, EventArgs e)
        {
            try
            {
                MouseMag = Convert.ToDouble(MagFactor.Text);
            }
            catch (Exception)
            {

                throw;
            }
        }

        bool CalibrateAbort;

        private void Calibrate_click(object sender, EventArgs e)
        {
            if (CalibrateButton.Text == "Calibrate")
            {
                CalibrateAbort = false;
                CalibrateButton.Text = "Abort";
                // Create the thread object. This does not start the thread.
                Thread workerThread = new Thread(Calibrate);

                // Start the worker thread.
                workerThread.Start();
            }
            else
            {
                CalibrateAbort = true;
                CalibrateButton.Text = "Calibrate";
            }
        }


        int CalibEntryCount = 0;

        private void Calibrate()
        {
            string Calfile = MainPath + "\\KMotion\\Data\\FiducialsMap.txt";
            string CalfileTemp = MainPath + "\\KMotion\\Data\\FiducialsMapTemp.txt";

            if (CalibEntryCount > 0) return;
            CalibEntryCount++;
            System.IO.StreamWriter file = new System.IO.StreamWriter(CalfileTemp);

            Calibrate2(file);
            CalibEntryCount--;
            file.Close();
            Invoke((MethodInvoker)delegate
            {
                CalibrateButton.Text = "Calibrate";

                if (!CalibrateAbort)
                {
                    if (MessageBox.Show("Reload Geo File?", "Calibration", MessageBoxButtons.YesNo) == DialogResult.Yes)
                    {
                        if (System.IO.File.Exists(Calfile))
                        {
                            System.IO.File.Delete(Calfile);
                        }
                        System.IO.File.Move(CalfileTemp, Calfile);


                        Host.SendMessageService(HostComm.ID_ReloadGeoCorrection);
                    }
                }
            });   
        }

        private void Calibrate2(System.IO.StreamWriter file)
        {
            bool Done = false;
            double LastX=0,LastY=0,LastZ=0,LastT=0;
            double BegRowX=0, BegRowY=0, BegRowZ=0, BegRowT=0;
            file.WriteLine(String.Format("{0},{1}", CalRows, CalCols));
            file.WriteLine(String.Format("{0:F4},{1:F4}", CalGridX, CalGridY));
            double x = CalGridX * (-0.5 * (CalCols - 1));
            double y = CalGridY * (-0.5 * (CalRows - 1));
            double z, t;
            file.WriteLine(String.Format("{0:F6},{1:F6}", x, y));

            for (int r = 0; r < CalRows; ++r)
            {
                for (int c = 0; c < CalCols; c++)
                {
                    x = CalGridX * (c - 0.5 * (CalCols - 1)) - XMicroOffset;
                    y = CalGridY * (r - 0.5 * (CalRows - 1)) - YMicroOffset;
                    z = 0.3;
                    t = 0;

                    if (c==0 && r>0) //new row but not first?
                    {
                        x = BegRowX;
                        y = BegRowY + CalGridY;
                        z = BegRowZ;
                        t = BegRowT;
                    }
                    else if (c>0) //use previous along row
                    {
                        x = LastX + CalGridX;
                        y = LastY;
                        z = LastZ;
                        t = LastT;
                    }

                    Host.MoveXYZT(x, y , z, t);
                    Host.WaitLastMoveComplete(ref Done);
                    if (CalibrateAbort) return;

                    int n;

                    if (r == 0 && c == 0)
                        n = 6;
                    else
                        n = 3;


                    for (int i = 0; i < n; i++)
                    {
                        if (c == 0) Thread.Sleep((int)(CalDelay * 1000.0));
                        Thread.Sleep((int)(CalDelay * 1000.0));
                        if (CalibrateAbort) return;
                        DoAlignAll();
                        Host.WaitLastMoveComplete(ref Done);
                        if (CalibrateAbort) return;
                    }

                    double xr = 0, yr = 0, zr = 0, ar = 0, br = 0, cr = 0;
                    bool result;

                    result = Host.GetPositionsNoGeo(ref xr, ref yr, ref zr, ref ar, ref br, ref cr, true);
                    file.WriteLine(String.Format("{0},{1},{2:F4},{3:F4},{4:F4},{5:F4}", r, c, xr + XMicroOffset, yr + YMicroOffset, zr, cr));

                    result = Host.GetPositions(ref xr, ref yr, ref zr, ref ar, ref br, ref cr);

                    if (c == 0)  // did beginning of row?
                    {
                        BegRowX = xr;
                        BegRowY = yr;
                        BegRowZ = zr;
                        BegRowT = cr;
                    }
                    LastX = xr;
                    LastY = yr;
                    LastZ = zr;
                    LastT = cr;
                }
            }
        }

        private void MainFormClosing(object sender, FormClosingEventArgs e)
        {
            Host.TerminateAll = true;
            Thread.Sleep(100);
        }

        private void OrthoTrackBarChanged(object sender, EventArgs e)
        { 
            OrthoTextBox.Text = OrthoTrackBar.Value.ToString();
        }

        int UpdateEntryCount = 0;

        void UpdatePositionsWorkerThread()
        {
            if (UpdateEntryCount > 0) return;
            UpdateEntryCount++;
            // Create the thread object. This does not start the thread.
            Thread workerThread = new Thread(UpdatePositions);

            // Start the worker thread.
            workerThread.Start();
        }

        void UpdatePositions()
        {
            double x = 0, y = 0, z = 0, a = 0, b = 0, c = 0;

            if (Host.GetPositions(ref x, ref y, ref z, ref a, ref b, ref c))
            {
                if (Host.TerminateAll) return;
                Status_x = x;
                Status_y = y;
                Status_z = z;
                Status_c = c;

                try
                {
                    Invoke((MethodInvoker)delegate
                    {
                        XPos.Text = String.Format("{0:F4}", ConvertToUser(x));
                        YPos.Text = String.Format("{0:F4}", ConvertToUser(y));
                        ZPos.Text = String.Format("{0:F4}", ConvertToUser(z));
                        CPos.Text = String.Format("{0:F4}", c);
                    });
                }
                catch (Exception)
                {
                    if (!Host.TerminateAll) throw;
                }
            }
            UpdateEntryCount--;

        }
    }
}
