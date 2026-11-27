//#define TEST_BIG_ARRAY 
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
        static extern IntPtr FindWindow(string lpClassName, string lpWindowName);

        // Overload for WM_COPYDATA (lParam is a pointer to the struct)
        [DllImport("user32.dll")]
        static extern IntPtr SendMessage(IntPtr hWnd, uint Msg, IntPtr wParam, ref COPYDATASTRUCT lParam);

        // Overload for WM_COMMAND (plain integer params, but still pointer-sized)
        [DllImport("user32.dll")]
        static extern IntPtr SendMessage(IntPtr hWnd, uint Msg, IntPtr wParam, IntPtr lParam); 
        

        private const uint WM_COPYDATA = 0x4A;

        String MainPath;
        KM_Controller KM;
        Double JogSpeed = 200;
        bool JoggingX = false;
        static Mutex ConsoleMutex = new Mutex();
        static string ConsoleMessageReceived;
        bool Connected = false;
        int skip = 1000; // so we check for board immediately
        int IP_Addr;
        int[] List;
        int nBoards;
#if TEST_BIG_ARRAY
        static int ASize = 500000000;
        Double[] Big = new Double[ASize];
        Double[] Big2 = new Double[ASize];
        Double sum = 0;
#endif

        public Form1(int IP)
        {
            IP_Addr = IP;
            InitializeComponent();
            MainPath = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            MainPath = System.IO.Path.GetDirectoryName(MainPath);
            MainPath = System.IO.Path.GetDirectoryName(MainPath);

            CFileName.Text = MainPath + "\\C Programs\\KStep\\InitKStep3Axis.c";
            KM = new KMotion_dotNet.KM_Controller(IP_Addr);

            //Set the callback for general messages
            KM.MessageReceived += new KMConsoleHandler(ConsoleMessageHandler);

#if TEST_BIG_ARRAY
            for (uint i = 0; i < ASize; i++) Big[i] = Big2[i] = (double)i * (double)i; //tktk
            for (uint i = 0; i < ASize; i++) sum += Big[i] + Big2[i];
#endif
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

            // with extra FTDI Drivers this can take a long time
            // so only call occasionally when not connected
            if (skip++ > 20)
            {
                skip = 0;
                // check how many boards connected
                List = KM.GetBoards(out nBoards);
            }

            if (KM.WaitToken(100) == KMOTION_TOKEN.KMOTION_LOCKED)
            {
                Connected = true;
                KM_MainStatus MainStatus;

                try
                {
                    MainStatus = KM.GetStatus(false);  // we already have a lock
                    KM.ReleaseToken();
                    XPos.Text = String.Format("{0:F1}", MainStatus.GetDestination(0));
                    XEnabled.Checked = MainStatus.GetAxisEnabled(0) != 0;
                }
                catch (DMException ex) // in case disconnect in the middle of reading status
                {
                    Connected = false;
                    KM.ReleaseToken();
                    MessageBox.Show(ex.InnerException.Message);
                }
            }
            else
            {
                Connected = false;
            }

            if (Connected)
                Text = String.Format("Dynomotion C# Forms App - Connected {0}", BoardString(KM.GetUSBLocation()));
            else
                Text = "Dynomotion C# Forms App - Disconnected";

            ConsoleMutex.WaitOne();  // make sure we are thread safe
            if (ConsoleMessageReceived != null && ConsoleMessageReceived != "")
            {
                ConsoleTextBox.AppendText(ConsoleMessageReceived);
                ConsoleMessageReceived = "";
            }
            ConsoleMutex.ReleaseMutex();
        }

        private String BoardString(int board)
        {
            String s;

            if (board <= 0 && board >= -15)
            {
                s = String.Format("{0}", board); // show as decimal number
            }
            else if ((uint)board < 0x00FFFFFF) // USB?
            {
                s = String.Format("KFLOP 0x{0:X}", board); // show as hex USB ID number
            }

            else // assume IP Address
            {
                String sn = "";
                for (int i = 0; i < nBoards; i++)  // check for match and if found add SN
                {
                    if ((uint)board > 0x00FFFFFF)
                    {
                        if (board == List[i])
                        {
                            sn = String.Format(" - SN{0}", List[i + 1] & 0xFFF);
                            break;
                        }
                        i++; // skip SN
                    }
                }
                s = String.Format("Kogna {0}.{1}.{2}.{3}{4}",
                    (board >> 24) & 0xff, (board >> 16) & 0xff, (board >> 8) & 0xff, board & 0xff, sn);
            }
            return s;
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
                    KMOTION_CHECK_READY status = KMOTION_CHECK_READY.ERROR;
                    try
                    {
                        status = KM.CheckIsReady();
                    }
                    catch (DMException ex) // in case disconnect in the middle of reading status
                    {
                        MessageBox.Show(ex.InnerException.Message);
                    }

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
            bool tokenHeld = false;
            string errorMessage = null;
            TestUSB.Enabled = false;
            try
            {
                int N = 1000000;
                bool isKflop = KM.GetBoardType() == BOARD_TYPE.KFLOP;
                if (isKflop) N = Math.Min(N, 100000);
                string BOARD = isKflop ? "KFLOP" : "Kogna";
                int L = isKflop ? 8 : 256;
                int[] data = new int[N];
                int[] data2 = new int[N];

                string s = "";
                int nchars_sent = 0;
                for (int i = 0; i < N; i++) data[i] = i;

                if (KM.WaitToken(1000000) != KMOTION_TOKEN.KMOTION_LOCKED)
                    throw new InvalidOperationException("Unable to acquire the board token for the upload.");
                tokenHeld = true;
                long T0 = System.Diagnostics.Stopwatch.GetTimestamp();

                KM.WriteLine(String.Format("SetGatherHex {0} {1}", 0, N));
                for (int i = 0; i < N; i++)
                {
                    s = s + data[i].ToString("X8");

                    if (((i % L) == L - 1) || i == N - 1)
                    {
                        KM.WriteLine(s);
                        nchars_sent += s.Length;
                        s = "";
                    }
                    else
                    {
                        s = s + " ";
                    }
                }
                tokenHeld = false;
                KM.ReleaseToken();

                long T1 = System.Diagnostics.Stopwatch.GetTimestamp();
                double dt = (T1 - T0) / (double)System.Diagnostics.Stopwatch.Frequency;
                TestResults1.Text = String.Format("PC->{0} N={1} Int32, Time={2:F3} sec, {3:F0}KBytes/sec",
                    BOARD, N, dt, nchars_sent / dt / 1000.0);

                int nchars_received = 0;
                if (KM.WaitToken(1000000) != KMOTION_TOKEN.KMOTION_LOCKED)
                    throw new InvalidOperationException("Unable to acquire the board token for the download.");
                tokenHeld = true;
                T0 = System.Diagnostics.Stopwatch.GetTimestamp();
                KM.WriteLine(String.Format("GetGatherHex {0} {1}", 0, N));

                s = "";
                for (int i = 0; i < N; i++)
                {
                    if (s.Length < 8)
                    {
                        bool gotLine = KM.ReadLineTimeout(ref s, 100000000);
                        if (!gotLine) throw new InvalidOperationException("Timed out reading benchmark data.");
                        nchars_received += s.Length;
                    }

                    string vs = s.Substring(0, 8);
                    if (s[8] == ' ')
                        s = s.Remove(0, 9);
                    else
                        s = s.Remove(0, 8);

                    data2[i] = int.Parse(vs, System.Globalization.NumberStyles.HexNumber);
                    if (data[i] != data2[i]) throw new InvalidOperationException("BAD DATA at word " + i);
                }
                tokenHeld = false;
                KM.ReleaseToken();

                T1 = System.Diagnostics.Stopwatch.GetTimestamp();
                dt = (T1 - T0) / (double)System.Diagnostics.Stopwatch.Frequency;
                TestResults2.Text = String.Format("{0}->PC N={1} Int32, Time={2:F3} sec, {3:F0}KBytes/sec",
                    BOARD, N, dt, nchars_received / dt / 1000.0);
            }
            catch (Exception ex)
            {
                errorMessage = ex.InnerException == null ? ex.Message : ex.Message + "\r\n" + ex.InnerException.Message;
            }
            finally
            {
                try
                {
                    if (tokenHeld)
                    {
                        tokenHeld = false;
                        KM.ReleaseToken();
                    }
                }
                catch (Exception ex)
                {
                    errorMessage += "\r\nUnable to release the board token: " + ex.Message;
                }
                finally
                {
                    TestUSB.Enabled = true;
                }
            }
            // Complete cleanup before a modal dialog can pause this UI thread.
            if (errorMessage != null) MessageBox.Show(errorMessage);
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
            KM2 = new KMotion_dotNet.KM_Controller((192<<24)+(168<<16)+(68<<8)+118);
            if (KM2.WriteLineReadLine("ReadBit47") == "0")
                KM2.WriteLine("SetBit47");
            else
                KM2.WriteLine("ClearBit47");
        }

        [StructLayout(LayoutKind.Sequential)]
        struct COPYDATASTRUCT
        {
            public IntPtr dwData;   // ULONG_PTR  — pointer-sized
            public int cbData;   // DWORD      — stays 32-bit
            public IntPtr lpData;   // PVOID      — pointer-sized
        }

        private void TestOpenKMotionCNC_click(object sender, EventArgs e)
        {
            String message = MainPath + "\\GCode Programs\\Dynomotion.ngc";

            IntPtr KMotionCNCWindow = FindWindow("KMotionCNC", null);
            if (KMotionCNCWindow == IntPtr.Zero)
            {
                MessageBox.Show("KMotionCNC not found running");
                return;
            }

            COPYDATASTRUCT cds;
            cds.dwData = IntPtr.Zero;
            cds.lpData = Marshal.StringToHGlobalUni(message);   // no (int) cast
            cds.cbData = message.Length * 2 + 2;

            try
            {
                SendMessage(KMotionCNCWindow, WM_COPYDATA, this.Handle, ref cds);
            }
            finally
            {
                Marshal.FreeHGlobal(cds.lpData);                // already an IntPtr
            }

            uint WM_COMMAND = 0x0111;
            int ID_OpenGCodeFile = 33018;
            SendMessage(KMotionCNCWindow, WM_COMMAND, (IntPtr)ID_OpenGCodeFile, IntPtr.Zero);
        }

        private void FormClosingEvent(object sender, FormClosingEventArgs e)
        {
            KM.Dispose();
        }

        private void FlashFirmware_Click(object sender, EventArgs e)
        {
            String firmwareFileName = MainPath + "\\DSP_Kogna\\DSPKOGNA.out";

            if (KM.GetBoardType() == BOARD_TYPE.KFLOP) 
            {
                firmwareFileName = MainPath + "\\DSP_KFLOP\\DSPKFLOP.out";
            }
            else
            {
                firmwareFileName = MainPath + "\\DSP_KOGNA\\DSPKOGNA.out";
            }

            int result = KM.LoadCoff(-1, firmwareFileName, 1);

            if (result != 0)
            {
                MessageBox.Show("Firmware Download failed"); 
            }
            else
            {
                KMotion_dotNet.KMOTION_TOKEN token = KMotion_dotNet.KMOTION_TOKEN.KMOTION_NOT_CONNECTED;

                token = KM.WaitToken(5);

                if (token == KMotion_dotNet.KMOTION_TOKEN.KMOTION_LOCKED)
                {
                    KM.WriteLineWithEcho("ProgFlashImage");
                    KMOTION_CHECK_READY ready;
                    do
                    {
                        Thread.Sleep(100);
                        ready = KM.CheckIsReady();
                    }
                    while (ready != KMOTION_CHECK_READY.READY);

                    KM.ReleaseToken();
                }

            }
        }

        // Cubic knot (coordinated segment) download rate test.
        //
        // Streams N synthetic LinearHexEx segments exactly the way CoordMotion does:
        // 21 hex-encoded floats per segment, several segments per line joined with
        // ';' up to the MAX_LINE limit.  OpenBuf clears the buffer first and
        // ExecBuf is NEVER sent, so nothing moves; the DSP just parses each segment
        // and drops it into its 35000 entry ring (which wraps).  WriteLine blocks
        // when TCP backs up, so the PC-side rate is the end to end rate the
        // interpreter could sustain.  Run MeasureKnotRate.c on the board at the
        // same time to see the DSP's own parse/insert rate (and to be the "one
        // user thread running" load).
        private static string HexFloat(float f)
        {
            return BitConverter.ToInt32(BitConverter.GetBytes(f), 0).ToString("X");
        }

        private void KnotRate_Click(object sender, EventArgs e)
        {
            const int N = 50000;         // segments to send
            const int MAX_LINE = 2560;   // KMotionDLL line limit
            bool tokenHeld = false;
            KnotRate.Enabled = false;
            try
            {
                if (KM.GetBoardType() == BOARD_TYPE.KFLOP)
                    throw new InvalidOperationException("Kogna only (KFLOP buffer is smaller and slower to fill safely).");

                if (KM.WaitToken(1000000) != KMOTION_TOKEN.KMOTION_LOCKED)
                    throw new InvalidOperationException("Unable to acquire the board token.");
                tokenHeld = true;

                KM.WriteLine("OpenBuf");

                // tiny 1ms segments along X: from (i*dx) to ((i+1)*dx), other axes 0,
                // parametric a b c d = 0 0 1 0 (constant speed), t = 0.001
                const float dx = 0.001f, tseg = 0.001f;
                string zero = HexFloat(0.0f), one = HexFloat(1.0f), t = HexFloat(tseg);
                var line = new System.Text.StringBuilder(MAX_LINE);
                int lines = 0, chars = 0;

                long T0 = System.Diagnostics.Stopwatch.GetTimestamp();
                for (int i = 0; i < N; i++)
                {
                    string seg = "LinearHexEx " + HexFloat(i * dx) + " " + zero + " " + zero + " " + zero + " " + zero + " " + zero + " " + zero + " " + zero
                               + " " + HexFloat((i + 1) * dx) + " " + zero + " " + zero + " " + zero + " " + zero + " " + zero + " " + zero + " " + zero
                               + " " + zero + " " + zero + " " + one + " " + zero + " " + t;
                    if (line.Length + seg.Length + 1 > MAX_LINE - 10)
                    {
                        KM.WriteLine(line.ToString()); ++lines; chars += line.Length;
                        line.Clear();
                    }
                    if (line.Length > 0) line.Append(';');
                    line.Append(seg);
                }
                if (line.Length > 0) { KM.WriteLine(line.ToString()); ++lines; chars += line.Length; }
                long T1 = System.Diagnostics.Stopwatch.GetTimestamp();

                KM.WriteLine("OpenBuf");   // leave the buffer empty
                tokenHeld = false;
                KM.ReleaseToken();

                double dt = (T1 - T0) / (double)System.Diagnostics.Stopwatch.Frequency;
                KnotResults.Text = String.Format("PC->Kogna {0} segments in {1:F3} s = {2:F0} knots/sec ({3} lines, {4:F0} KB/s)",
                    N, dt, N / dt, lines, chars / dt / 1000.0);
            }
            catch (Exception ex)
            {
                if (tokenHeld) { try { KM.ReleaseToken(); } catch { } }
                KnotResults.Text = "Knot rate test failed: " + ex.Message;
            }
            finally { KnotRate.Enabled = true; }
        }
    }

}
