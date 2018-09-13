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
using System.Runtime.InteropServices; // DsEvCode



namespace SimpleFormsCS
{
    public partial class Form1 : Form
    {
        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        static extern int SendMessage(IntPtr hwnd, int wMsg, int wParam, ref COPYDATASTRUCT lParam);
        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        static extern int SendMessage(IntPtr hwnd, int wMsg, IntPtr wParam, ref COPYDATASTRUCT lParam);
        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        static extern int SendMessage(IntPtr hwnd, int wMsg, int wParam, int lParam);
        [DllImport("User32.dll", SetLastError = true)]
        public static extern IntPtr FindWindow(String lpClassName, String lpWindowName);

        private const int WM_COPYDATA = 0x4A;

        String MainPath;
        KM_Controller KM;
        Double JogSpeed = 200;
        bool JoggingX = false;
        static Mutex ConsoleMutex = new Mutex();
        static string ConsoleMessageReceived;
        bool Connected = false;
        int skip = 0;

        public Form1()
        {
            InitializeComponent();
            MainPath = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            MainPath = System.IO.Path.GetDirectoryName(MainPath);
            MainPath = System.IO.Path.GetDirectoryName(MainPath);

            CFileName.Text = MainPath + "\\C Programs\\KStep\\InitKStep3Axis.c";
            KM = new KMotion_dotNet.KM_Controller();

            //Set the callback for general messages
            KM.MessageReceived += new KMConsoleHandler(ConsoleMessageHandler);

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

        private void CompileLoadExec_Click(object sender, EventArgs e)
        {
            try
            {
                String Result = KM.CompileAndLoadCoff(1, CFileName.Text, false);
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

        private void timer1_Tick(object sender, EventArgs e)
        {
            int[] List;
            int nBoards = 0;

            // with extra FTDI Drivers this can take a long time
            // so only call occasionally when not connected
            if (!Connected && ++skip==10)
            {
                skip = 0;

                // check how many boards connected
                List = KM.GetBoards(out nBoards);
                if (nBoards > 0)
                {
                    Connected = true;
                    this.Text = String.Format("Dynomotion C# Forms App - Connected - USB location {0:X}", List[0]);
                }
                else
                {
                    Connected = false;
                    this.Text = "Dynomotion C# Forms App - Disconnected";
                }
            }

            if (Connected && KM.WaitToken(100) == KMOTION_TOKEN.KMOTION_LOCKED)
            {
                KM_MainStatus MainStatus;

                try
                {
                    MainStatus = KM.GetStatus(false);  // we already have a lock
                    KM.ReleaseToken();
                    XPos.Text = String.Format("{0:F1}", MainStatus.Destination[0]);
                    XEnabled.Checked = (MainStatus.Enables & (1 << 0)) != 0;
                }
                catch (DMException ex) // in case disconnect in the middle of reading status
                {
                    KM.ReleaseToken();
                    MessageBox.Show(ex.InnerException.Message);
                }
            }
            else
            {
                Connected = false;
            }

            ConsoleMutex.WaitOne();  // make sure we are thread safe
            if (ConsoleMessageReceived != null && ConsoleMessageReceived != "")
            {
                ConsoleTextBox.AppendText(ConsoleMessageReceived);
                ConsoleMessageReceived = "";
            }
            ConsoleMutex.ReleaseMutex();
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

        private void XEnabled_Clicked(object sender, EventArgs e)
        {
            if (XEnabled.Checked)
                WriteLineHandleException("EnableAxis0");
            else
                WriteLineHandleException("DisableAxis0");
        }


        /// Receives Console Message callbacks from Kmotion.DLL funtions
        static private int ConsoleMessageHandler(string message)
        {
            ConsoleMutex.WaitOne();
            ConsoleMessageReceived += message;
            ConsoleMutex.ReleaseMutex();
            return 0;
        }

        private void SendCommand_Click(object sender, EventArgs e)
        {
            var result = KM.WaitToken(1000);

            if (result == KMOTION_TOKEN.KMOTION_LOCKED)
            {
                KM.WriteLineWithEcho(Command.Text);
                while (true)
                {
                    var status = KM.CheckIsReady();

                    if (status == KMOTION_CHECK_READY.READY)
                    {
                        KM.ReleaseToken();
                        break;
                    }

                    if (status == KMOTION_CHECK_READY.TIMEOUT ||
                        status == KMOTION_CHECK_READY.ERROR)
                    {
                        KM.ReleaseToken();
                        MessageBox.Show("Response Error");
                        break;
                    }
                    Thread.Sleep(10);
                }
            }
            else
            {
                MessageBox.Show("Unable to Send Command to Board");
            }
        }



        private void TestUSB_Click(object sender, EventArgs e)
        {
            int N = 100000;  //size of block to test
            int nchars_sent = 0;
            int[] data = new int[N];
            int[] data2 = new int[N];

            // fill simple buffer with a ramp
            for (int i = 0; i < N; i++) data[i] = i;

            // first get the token for the board to allow uninterrupted access

            if (KM.WaitToken(1000000) != KMOTION_TOKEN.KMOTION_LOCKED) return;

            DateTime T0 = DateTime.Now;

            try
            {
                // tell the board we will send N (32 bit ) words at offset 0
                KM.WriteLine(String.Format("SetGatherHex {0} {1}", 0, N));

                // send the data (simple ramp)  (8 hex words per line)

                string s = "";
                for (int i = 0; i < N; i++)
                {
                    s = s + data[i].ToString("X8");

                    if (((i % 8) == 7) || i == N - 1)  // every 8 or on the last send it
                    {
                        KM.WriteLine(s);
                        nchars_sent += s.Length;
                        s = "";
                    }
                    else
                    {
                        s = s + " ";  // otherwise insert a space
                    }
                }

                DateTime T1 = DateTime.Now;
                double dt = (T1.Ticks - T0.Ticks) / 1e7;
                TestResults1.Text = String.Format("PC->KFLOP N={0} Int32, Time={1:F3} sec, {2:F0}KBytes/sec", N, dt, nchars_sent / dt / 1000.0);


                T0 = DateTime.Now;

                // tell the board we will read N (32 bit ) words at offset 0
                KM.WriteLine(String.Format("GetGatherHex {0} {1}", 0, N));

                // send the data (simple ramp)  (8 hex words per line)

                s = "";
                for (int i = 0; i < N; i++)
                {
                    if (s.Length < 8)
                        KM.ReadLineTimeout(ref s, 100000000);

                    string vs = s.Substring(0, 8);
                    if (s[8] == ' ')
                        s = s.Remove(0, 9);
                    else
                        s = s.Remove(0, 8);

                    data2[i] = int.Parse(vs, System.Globalization.NumberStyles.HexNumber);

                    if (data[i] != data2[i])
                    {
                        MessageBox.Show("BAD DATA");
                        KM.ReleaseToken();
                        return;
                    }

                }

                // release our access to the board
                KM.ReleaseToken();

                T1 = DateTime.Now;
                dt = (T1.Ticks - T0.Ticks) / 1e7;
                TestResults2.Text = String.Format("KFLOP->PC N={0} Int32, Time={1:F3} sec, {2:F0}KBytes/sec", N, dt, nchars_sent / dt / 1000.0);
            }
            catch (DMException ex) // in case disconnect in the middle of reading status
            {
                KM.ReleaseToken();
                MessageBox.Show(ex.InnerException.Message);
            }

        }

        private void MoveTo_Click(object sender, EventArgs e)
        {
            KM_Axis XAxis = new KM_Axis(KM, 0, "X");
            XAxis.Velocity = 1000.0;
            XAxis.StartMoveTo(double.Parse(MoveToValue.Text));
        }

        private void TestBoard2_click(object sender, EventArgs e)
        {
            KM_Controller KM2;
            KM2 = new KMotion_dotNet.KM_Controller(530);
            if (KM2.WriteLineReadLine("ReadBit47") == "0")
                KM2.WriteLine("SetBit47");
            else
                KM2.WriteLine("ClearBit47");
        }

        struct COPYDATASTRUCT
        {
            public int dwData;
            public int cbData;
            public int lpData;
        }

        private void TestOpenKMotionCNC_click(object sender, EventArgs e)
        {
            String message = MainPath + "\\GCode Programs\\Dynomotion.ngc";
            COPYDATASTRUCT cds;
            cds.dwData = 0;
            cds.lpData = (int)Marshal.StringToHGlobalUni(message);
            cds.cbData = message.Length * 2 + 2;

            IntPtr KMotionCNCWindow = FindWindow("KMotionCNC", null);
            SendMessage(KMotionCNCWindow, (int)WM_COPYDATA, this.Handle, ref cds);
            Marshal.FreeHGlobal((IntPtr)cds.lpData);

            uint WM_COMMAND = 0x0111;
            int ID_OpenGCodeFile = 33018;
            SendMessage(KMotionCNCWindow, (int)WM_COMMAND, ID_OpenGCodeFile, 0);
        }

        private void FormClosingEvent(object sender, FormClosingEventArgs e)
        {
            KM.Dispose();
        }
    }

}
