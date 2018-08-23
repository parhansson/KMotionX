using System;
using System.Windows;
using System.Windows.Media;
using System.Reflection;
using KMotion_dotNet;

namespace SimpleGCodeWPF
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        static bool ExecutionInProgress = false;
        static KMotion_dotNet.KM_Controller KM;
        static bool Connected = false;
        static int skip = 0;
        static string MainPath;

        public MainWindow()
        {
            InitializeComponent();

            //Create an instance of the KM the same instance should be used throughout the app 
            try
            {
                KM = new KMotion_dotNet.KM_Controller();
            }
            catch(Exception e)
            {
                MessageBox.Show("Unable to load KMotion_dotNet Libraries.  Check Windows PATH or .exe location\r\r"+e.Message);
                System.Windows.Application.Current.Shutdown();
                return;
            }
            //Add all various callbacks
            AddHandlers();

            //Load a C program, first figure out what directory we are installed into
            string codeBase = Assembly.GetExecutingAssembly().CodeBase;
            UriBuilder uri = new UriBuilder(codeBase);
            MainPath = Uri.UnescapeDataString(uri.Path);
            MainPath = System.IO.Path.GetDirectoryName(MainPath);
            MainPath = System.IO.Path.GetDirectoryName(MainPath);
            MainPath = System.IO.Path.GetDirectoryName(MainPath);

            // pick one of the examples to configure KFLOP
            String TheCFile = MainPath + @"\C Programs\KSTEP\InitKStep3Axis.c";

            //Execute it
            try
            {
                KM.ExecuteProgram(1, TheCFile, false);
            }
            catch (DMException e)
            {
                MessageBox.Show("Unable to Execute C Program in KFLOP\r\r" + e.InnerException.Message);
            }

            // default a simple GCode File
            GCodeFile.Text = MainPath + @"\GCode Programs\box.ngc";

            // Start a Timer for status updates
            System.Windows.Threading.DispatcherTimer Timer = new System.Windows.Threading.DispatcherTimer();
            Timer.Tick += new EventHandler(dispatcherTimer_Tick);
            Timer.Interval = TimeSpan.FromMilliseconds(100);
            Timer.Tick += dispatcherTimer_Tick;
            Timer.Start();

            SetMotionParameters(); // Set some motion Parameters
        }

        // update status 
        private void dispatcherTimer_Tick(object sender, EventArgs e)
        {
            int[] List;
            int nBoards = 0;

            // with extra FTDI Drivers this can take a long time
            // so only call occasionally when not connected
            if (!Connected && ++skip == 10)
            {
                skip = 0;

                // check how many boards connected
                List = KM.GetBoards(out nBoards);
                if (nBoards > 0)
                {
                    Connected = true;
                    Title = String.Format("Dynomotion C# Forms App - Connected - USB location {0:X}", List[0]);
                }
                else
                {
                    Connected = false;
                    Title = "Dynomotion C# Forms App - Disconnected";
                }
            }

            if (Connected && KM.WaitToken(100) == KMOTION_TOKEN.KMOTION_LOCKED)
            {
                try
                {
                    KM_MainStatus MainStatus = KM.GetStatus(false);  // we already have a lock
                    KM.ReleaseToken();

                    if ((MainStatus.Enables & 1) != 0)  // Set DRO colors based on enables
                        DROX.Foreground = Brushes.Green;
                    else
                        DROX.Foreground = Brushes.Orange;

                    if ((MainStatus.Enables & 2) != 0)
                        DROY.Foreground = Brushes.Green;
                    else
                        DROY.Foreground = Brushes.Orange;

                    if ((MainStatus.Enables & 4) != 0)
                        DROZ.Foreground = Brushes.Green;
                    else
                        DROZ.Foreground = Brushes.Orange;

                    // Get Absolute Machine Coordinates and display in DROs
                    double x=0, y=0, z=0, a=0, b=0, c=0;
                    KM.CoordMotion.UpdateCurrentPositionsABS(ref x, ref y, ref z, ref a, ref b, ref c, false);
                    
                    DROX.Text = String.Format("{0:F4}", x);
                    DROY.Text = String.Format("{0:F4}", y);
                    DROZ.Text = String.Format("{0:F4}", z);

                    // Set Feedhold color bases on state
                    if (KM.WriteLineReadLine("GetStopState") == "0")
                        Feedhold.Background = Brushes.Yellow;
                    else
                        Feedhold.Background = Brushes.Orange;
                }
                catch (DMException) // in case disconnect in the middle of reading status
                {
                    KM.ReleaseToken();  // make sure token is released
                }
            }
            else
            {
                Connected = false;
            }

            // update Run button color/enable
            Run.IsEnabled = !ExecutionInProgress;
        }

        // Browse for GCode File
        private void BrowseGCode_Click(object sender, RoutedEventArgs e)
        {
            // Create OpenFileDialog 
            Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();

            // Set filter for file extension and default file extension 
            dlg.DefaultExt = ".ngc";
            dlg.Filter = "ngc Files (*.ngc)|*.ngc|txt Files (*.txt)|*.txt";
            
            // Show Dialog and get the selected file name and display in a TextBox 
            if (dlg.ShowDialog() == true) GCodeFile.Text = dlg.FileName;
        }

        // Cycle Start
        private void Run_Click(object sender, RoutedEventArgs e)
        {
            if (ExecutionInProgress) return;  // ignore if we are already executing

            ExecutionInProgress = true;
            KM.CoordMotion.Abort();
            KM.CoordMotion.ClearAbort();

            KM.CoordMotion.Interpreter.InitializeInterpreter();

            Set_Fixture_Offset(2, 2, 3, 0);  // set XYZ offsets for G55

            KM.CoordMotion.Interpreter.Interpret(GCodeFile.Text);  // Execute the File!
        }

        private void Set_Fixture_Offset(int Fixture_Number, double X, double Y, double Z)
        {
            // Set GVars for Offsets
            KM.CoordMotion.Interpreter.SetOrigin(Fixture_Number,
                KM.CoordMotion.Interpreter.InchesToUserUnits(X),
                KM.CoordMotion.Interpreter.InchesToUserUnits(Y),
                KM.CoordMotion.Interpreter.InchesToUserUnits(Z), 0, 0, 0);

            KM.CoordMotion.Interpreter.SetupParams.OriginIndex = -1;  // force update from GCode Vars
            KM.CoordMotion.Interpreter.ChangeFixtureNumber(Fixture_Number);  // Load offsets for fixture
        }

        private void Feedhold_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (KM.WriteLineReadLine("GetStopState") == "0")
                    KM.Feedhold();
                else
                    KM.ResumeFeedhold();
            }
            catch (Exception)
            {
                KM.ReleaseToken();
            }
        }

        private void Halt_Click(object sender, RoutedEventArgs e)
        {
            KM.CoordMotion.Interpreter.Halt();
        }

        /// <summary>
        /// Handler for the error message pump
        /// </summary>
        /// <param name="message">error string</param>
        static void KM_ErrorUpdated(string message)
        {
            MessageBox.Show(message);
        }

        static void Interpreter_InterpreterCompleted(int status, int lineno, int sequence_number, string err)
        {
            if (status !=0 && status != 1005)  MessageBox.Show(err);  //status 1005 = successful halt
            ExecutionInProgress = false;
        }

        private void AddHandlers()
        {
            //Set the callback for Errors
            KM.ErrorReceived += new KMotion_dotNet.KMErrorHandler(KM_ErrorUpdated);

            //Set the Interpreter's callbacks
            KM.CoordMotion.Interpreter.InterpreterCompleted += new KMotion_dotNet.KM_Interpreter.KM_GCodeInterpreterCompleteHandler(Interpreter_InterpreterCompleted);
        }

        // configure motion parameters
        private void SetMotionParameters()
        {
            KM.CoordMotion.MotionParams.BreakAngle = 30;
            KM.CoordMotion.MotionParams.MaxAccelX = 1;
            KM.CoordMotion.MotionParams.MaxAccelY = 1;
            KM.CoordMotion.MotionParams.MaxAccelZ = 1;
            KM.CoordMotion.MotionParams.MaxAccelA = 1;
            KM.CoordMotion.MotionParams.MaxAccelB = 1;
            KM.CoordMotion.MotionParams.MaxAccelC = 1;
            KM.CoordMotion.MotionParams.MaxVelX = 1;
            KM.CoordMotion.MotionParams.MaxVelY = 1;
            KM.CoordMotion.MotionParams.MaxVelZ = 1;
            KM.CoordMotion.MotionParams.MaxVelA = 1;
            KM.CoordMotion.MotionParams.MaxVelB = 1;
            KM.CoordMotion.MotionParams.MaxVelC = 1;
            KM.CoordMotion.MotionParams.CountsPerInchX = 100;
            KM.CoordMotion.MotionParams.CountsPerInchY = 100;
            KM.CoordMotion.MotionParams.CountsPerInchZ = 100;
            KM.CoordMotion.MotionParams.CountsPerInchA = 100;
            KM.CoordMotion.MotionParams.CountsPerInchB = 100;
            KM.CoordMotion.MotionParams.CountsPerInchC = 100;
            KM.CoordMotion.MotionParams.DegreesA = false;
            KM.CoordMotion.MotionParams.DegreesB = false;
            KM.CoordMotion.MotionParams.DegreesC = false;
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            KM.Dispose();
        }
    }
}
