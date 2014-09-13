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


namespace SimpleFormsCS
{
    public partial class Form1 : Form
    {
        String MainPath;
        KM_Controller KM;
        Double JogSpeed = 200;
        bool JoggingX = false;
        static Mutex ConsoleMutex = new Mutex();
        static string ConsoleMessageReceived;

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
            int []List;
            int nBoards = 0;

            // check how many boards connected
            List = KM.GetBoards(out nBoards);
            if (nBoards > 0)
            {
                this.Text = String.Format("Dynomotion C# Forms App - Connected - USB location {0:X}",List[0]);
            }
            else
            {
                this.Text = "Dynomotion C# Forms App - Disconnected";
            }

            if (KM.WaitToken(100) == KMOTION_TOKEN.KMOTION_LOCKED)
            {
                KM_MainStatus MainStatus;

                try
                {
                    MainStatus = KM.GetStatus(false);  // we already have a lock
                    KM.ReleaseToken();
                    XPos.Text = String.Format("{0:F1}", MainStatus.Destination[0]);
                    XEnabled.Checked = (MainStatus.Enables & (1<<0)) != 0;
                }
                catch (DMException ex) // in case disconnect in the middle of reading status
                {
                    KM.ReleaseToken();
                    MessageBox.Show(ex.InnerException.Message);
                }
            }

            ConsoleMutex.WaitOne();  // make sure we are thread safe
            if (ConsoleMessageReceived != "")
            {
                ConsoleTextBox.Text += ConsoleMessageReceived;
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
            KM.WriteLineWithEcho(Command.Text);
            KM.CheckIsReady();
        }



        private void TestUSB_Click(object sender, EventArgs e)
        {
            int N=100000;  //size of block to test
            int nchars_sent = 0;
            int []data=new int[N];
            int []data2=new int[N];

            // fill simple buffer with a ramp
            for (int i = 0; i < N; i++) data[i] = i;

            // first get the token for the board to allow uninterrupted access

            if (KM.WaitToken(1000000) != KMOTION_TOKEN.KMOTION_LOCKED) return;

            DateTime T0 = DateTime.Now;

            try
            {
                // tell the board we will send N (32 bit ) words at offset 0
                KM.WriteLine(String.Format("SetGatherHex {0} {1}", 0,N));

                // send the data (simple ramp)  (8 hex words per line)

                string s = "";
                for (int i = 0; i < N; i++)
                {
                    s = s + data[i].ToString("X8");

                    if (((i % 8) == 7) || i == N - 1)  // every 8 or on the last send it
                    {
                        KM.WriteLine(s);
                        nchars_sent += s.Length;
                        s="";
                    }
                    else
                    {
                        s = s + " ";  // otherwise insert a space
                    }
                }

                DateTime T1 = DateTime.Now;
                double dt = (T1.Ticks - T0.Ticks) / 1e7;
                TestResults1.Text = String.Format("PC->KFLOP N={0} Int32, Time={1:F3} sec, {2:F0}KBytes/sec", N, dt, nchars_sent/dt/1000.0);



                // tell the board we will read N (32 bit ) words at offset 0
                KM.WriteLine(String.Format("GetGatherHex {0} {1}", 0, N));

                // send the data (simple ramp)  (8 hex words per line)

                s="";
                for (int i = 0; i < N; i++)
                {
                    if (s.Length < 8)
                        KM.ReadLineTimeout(ref s,100000000);
                
                    string vs = s.Substring(0,8);
                    if (s[8]==' ')
                        s=s.Remove(0,9);
                    else
                        s=s.Remove(0,8);

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
    } 
}
