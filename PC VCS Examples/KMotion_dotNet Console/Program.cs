using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Linq;
using System.Reflection;
using KMotion_dotNet;
using System.Diagnostics;
using System.Windows.Forms;
using System.IO;

namespace sandbox
{
    class Program
    {
 //       static bool complete = false;
        static KMotion_dotNet.KM_Controller _Controller;

        #region Controller Example
        /// <summary>
        /// Initialize Controller object and perfor tests
        /// </summary>
        /// <param name="args">N/A</param>
        static void Main(string[] args)
        {
            //Create an instance of the _Controller
            //The same instance should be used throughout the app 
            _Controller = new KMotion_dotNet.KM_Controller();
            //Add all various callbacks
            AddHandlers();
            Console.WriteLine(String.Format("Board Type = [{0}]", _Controller.BoardType));

 
            //Load a C program
            //
            
            // Figure out what directory we are installed into
            string codeBase = Assembly.GetExecutingAssembly().CodeBase;
            UriBuilder uri = new UriBuilder(codeBase);
            string path = Uri.UnescapeDataString(uri.Path);
            path = Path.GetDirectoryName(path);
            path = Path.GetDirectoryName(path);
            path = Path.GetDirectoryName(path); 

            // pick one of the examples
            String TheCFile = path  + @"\C Programs\KSTEP\InitKStep3Axis.c";
 
            //************NEW program execution model***********
            String result = _Controller.ExecuteProgram(1, TheCFile, false);
            if (result != "") MessageBox.Show(result);

            //Also have a look at these::
            //_Controller.ExecuteProgram(1); 
            //_Controller.WaitForThreadComplete(1, 400);
            //_Controller.KillProgramThreads(1);
            //_Controller.KillProgramThreads(3, 5, 6);

            // Put a 64 bit float value into KFLOP UserData
            // vars 26&27 and read it back.
            // in KFLOP read it with:
            //  printf("UserData %f\n", *(double*)&persist.UserData[13 * 2]);

            _Controller.SetUserDataDouble(13, 123.456789);
            double d = _Controller.GetUserDataDouble(13);

            //Get Firmware version
            Console.WriteLine(_Controller.GetCommandValue<string>("Version", false));
            

            RunMain_StatusExample();
            //RunHomingRoutineExample();
            RunIOExample();
            //RunAxisExample();
            //RunCoordinatedMotionExample();
            //RunInterpreterExample(); 
            
            _Controller.Dispose();
            
            Console.ReadLine();
        }

        /// <summary>
        /// Demonstrates very basic automatic homing routine generation and execution
        /// </summary>
        static void RunHomingRoutineExample()
        {
            var axis = _Controller.GetAxis(0, "X");

            axis.CPU = 2000;

            axis.HomingParams.SourceType = HOMING_ROUTINE_SOURCE_TYPE.AUTO;
            axis.HomingParams.HomeFastVel = 1000;
            axis.HomingParams.HomeSlowVel = 100;
            axis.HomingParams.HomeLimitBit = 16;
            axis.HomingParams.HomeLimitState = true;
            axis.HomingParams.RepeatHomeAtSlowerRate = true;
            axis.HomingParams.SequencePriority = 1;
            axis.StartDoHome();
            // axis.DoHome();
        }

        /// <summary>
        /// Test basic IO manipulation
        /// </summary>
        static void RunIOExample()
        {

            var LED1 = _Controller.GetIO(46, IO_TYPE.DIGITAL_OUT, "LED1");
            var LED2 = _Controller.GetIO(47, IO_TYPE.DIGITAL_OUT, "LED2");

            LED1.SetDigitalValue(false);
            LED2.SetDigitalValue(true);

            for (int i = 0; i < 16; i++)
            {
                LED1.Toggle();
                Thread.Sleep(100);
                LED2.Toggle();
                Thread.Sleep(100);
            }

            LED1.SetDigitalValue(true);
            LED2.SetDigitalValue(true);
        }

        /// <summary>
        /// Test basic axis channel manipulation
        /// </summary>
        static void RunAxisExample()
        {
            var axis = _Controller.GetAxis(0, "X");
            axis.Acceleration = 10000;
            axis.TuningParams.Jerk = 10000;
            axis.Velocity = 10000;
            axis.Enable();
            axis.CPU = 100; 
            
            axis.MoveTo(50);
            axis.Jog(400);
            axis.Stop();

            axis.ZeroAxis();
        }


        static void RunMain_StatusExample()
        {
            Console.WriteLine(String.Format("Default Main Status Update Interval = {0}", _Controller.StatusUpdateInterval));
            Console.WriteLine(String.Format("Default Lock Status During Update = {0}", _Controller.LockStatusOnUpdate));

            Console.WriteLine(String.Format("Main_Status Version/Size = {0}", _Controller.CurrentStatus.VersionAndSize));

            Console.WriteLine(String.Format("TimeStamp = {0}", _Controller.CurrentStatus.TimeStamp));
            Console.WriteLine(String.Format("TimeStamp = {0}  (Same Data)", _Controller.CurrentStatus.TimeStamp));
            Console.WriteLine("Sleeping on thread.....(150ms)"); ;
            Thread.Sleep(150);
            Console.WriteLine(String.Format("TimeStamp = {0}  (Auto updated based on supplied interval)", _Controller.CurrentStatus.TimeStamp));
        }


         /// <summary>
         /// Receives general callbacks from Kmotion.DLL funtions
         /// </summary>
         /// <param name="message">returned message from contoller object</param>
         /// <returns>Motion Board Message</returns>
        static int _Controller_MessageUpdated(string message)
        {
            Console.WriteLine(message);
            return 0;
        }

        /// <summary>
        /// Handler for the error message pump
        /// </summary>
        /// <param name="message">error string</param>
        static void _Controller_ErrorUpdated(string message)
        {
            Console.WriteLine("#########################  ERROR  #########################");
            Console.WriteLine(message);
            Console.WriteLine("#########################  ERROR  #########################");
        }
        #endregion

        #region CoordMotion Example
        /// <summary>
        /// Test Arcs, Lines and Traverse
        /// </summary>
        static void RunCoordinatedMotionExample()
        {
            _Controller.CoordMotion.Abort();
            _Controller.CoordMotion.ClearAbort();
            _Controller.WriteLine(String.Format("DefineCS = {0} {1} {2} {3} {4} {5}", 0, 1, 2, -1, -1, -1));
            _Controller.WriteLine(String.Format("EnableAxis{0}", 0));
            _Controller.WriteLine(String.Format("EnableAxis{0}", 1));
            _Controller.WriteLine(String.Format("EnableAxis{0}", 2));
            _Controller.CoordMotion.MotionParams.BreakAngle = 30;
            _Controller.CoordMotion.MotionParams.RadiusA = 5;
            _Controller.CoordMotion.MotionParams.RadiusB = 5;
            _Controller.CoordMotion.MotionParams.RadiusC = 5;
            _Controller.CoordMotion.MotionParams.MaxAccelX = 30000;
            _Controller.CoordMotion.MotionParams.MaxAccelY = 3000;
            _Controller.CoordMotion.MotionParams.MaxAccelZ = 3000;
            _Controller.CoordMotion.MotionParams.MaxAccelA = 30;
            _Controller.CoordMotion.MotionParams.MaxAccelB = 30;
            _Controller.CoordMotion.MotionParams.MaxAccelC = 30;
            _Controller.CoordMotion.MotionParams.MaxVelX = 3000;
            _Controller.CoordMotion.MotionParams.MaxVelY = 30;
            _Controller.CoordMotion.MotionParams.MaxVelA = 30;
            _Controller.CoordMotion.MotionParams.MaxVelB = 30;
            _Controller.CoordMotion.MotionParams.MaxVelC = 30;
            _Controller.CoordMotion.MotionParams.CountsPerInchX = 300;
            _Controller.CoordMotion.MotionParams.CountsPerInchY = 300;
            _Controller.CoordMotion.MotionParams.CountsPerInchZ = 300;
            _Controller.CoordMotion.MotionParams.CountsPerInchA = 30;
            _Controller.CoordMotion.MotionParams.CountsPerInchB = 30;
            _Controller.CoordMotion.MotionParams.CountsPerInchC = 30;
            _Controller.CoordMotion.MotionParams.DegreesA = false;
            _Controller.CoordMotion.MotionParams.DegreesB = false;
            _Controller.CoordMotion.MotionParams.DegreesC = false;
            double speed = 85;

            _Controller.CoordMotion.StraightTraverse(6.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, false);
            int isCCW = 1;
            _Controller.CoordMotion.StraightFeed(speed, 5.0D, 0.0D, 0.0D, 0.0D, 0.0D, 0.0D, 0, 0);

            _Controller.CoordMotion.ArcFeed(speed, 1, 0.0000, 5.000, 5.0000, 0.2500, isCCW, 0.0000, 0.0000, 0.0000, 0.0000, 0, 0);
            _Controller.CoordMotion.ArcFeed(speed, 1, 0.0000, 0.0000, 0.0000, 0.2500, isCCW, 0.0000, 0.0000, 0.0000, 0.0000, 0, 0);

            _Controller.CoordMotion.Dwell(2,0);
            _Controller.CoordMotion.FlushSegments();
        }

        static void CoordMotion_CoordMotionStraightTranverse(double x, double y, double z, int sequence_number)
        {
            Console.WriteLine("CoordMotion Straight Tranverse::  {0} | {1} | {2} | {3}", x, y, z, sequence_number);
        }

        static void CoordMotion_CoordMotionStraightFeed(double DesiredFeedRate_in_per_sec, double x, double y, double z, int sequence_number, int ID)
        {
            Console.WriteLine("CoordMotion Straight Feed::  {0} | {1} | {2} | {3} | {4} | {5}", DesiredFeedRate_in_per_sec, x, y, z, sequence_number, ID);
        }

        static void CoordMotion_CoordMotionArcFeed(bool ZeroLenAsFullCircles, double DesiredFeedRate_in_per_sec, int plane, double first_end, double second_end, double first_axis,
            double second_axis, int rotation, double axis_end_point, double first_start, double second_start, double axis_start_point, int sequence_number, int ID)
        {
            Console.WriteLine("CoordMotion Arc Feed::  {0} | {1} | {2} | {3} | {4} | {5} | {6} | {7} | {8} | {9} | {10} | {11} | {12}",
                ZeroLenAsFullCircles,
                DesiredFeedRate_in_per_sec,
                plane, first_end,
                second_end,
                first_axis,
                second_axis,
                rotation,
                axis_end_point,
                first_start,
                second_start,
                axis_start_point,
                sequence_number,
                ID);
        } 
        #endregion
 
        #region Interpreter Example
        /// <summary>
        /// Process an RS274 file
        /// **Change paths for your configuration where appropriate
        /// </summary>
        static void RunInterpreterExample()
        {
            //Dont forget to map and enable the axes you want to use via the KMotion app 
            _Controller.WriteLine(String.Format("DefineCS = {0} {1} {2} {3} {4} {5}", 0, 1, 2, -1, -1, -1));
            _Controller.WriteLine(String.Format("EnableAxis{0}", 0));
            _Controller.WriteLine(String.Format("EnableAxis{0}", 1));
            _Controller.WriteLine(String.Format("EnableAxis{0}", 2));
 //           complete = false;
            _Controller.CoordMotion.Abort();
            _Controller.CoordMotion.ClearAbort();
            _Controller.CoordMotion.MotionParams.BreakAngle = 30;
            _Controller.CoordMotion.MotionParams.MaxAccelX = 1;
            _Controller.CoordMotion.MotionParams.MaxAccelY = 1;
            _Controller.CoordMotion.MotionParams.MaxAccelZ = 1;
            _Controller.CoordMotion.MotionParams.MaxAccelA = 1;
            _Controller.CoordMotion.MotionParams.MaxAccelB = 1;
            _Controller.CoordMotion.MotionParams.MaxAccelC = 1;
            _Controller.CoordMotion.MotionParams.MaxVelX = 1;
            _Controller.CoordMotion.MotionParams.MaxVelY = 1;
            _Controller.CoordMotion.MotionParams.MaxVelZ = 1;
            _Controller.CoordMotion.MotionParams.MaxVelA = 1;
            _Controller.CoordMotion.MotionParams.MaxVelB = 1;
            _Controller.CoordMotion.MotionParams.MaxVelC = 1;
            _Controller.CoordMotion.MotionParams.CountsPerInchX = 100;
            _Controller.CoordMotion.MotionParams.CountsPerInchY = 100;
            _Controller.CoordMotion.MotionParams.CountsPerInchZ = 100;
            _Controller.CoordMotion.MotionParams.CountsPerInchA = 100;
            _Controller.CoordMotion.MotionParams.CountsPerInchB = 100;
            _Controller.CoordMotion.MotionParams.CountsPerInchC = 100;
            _Controller.CoordMotion.MotionParams.DegreesA = false;
            _Controller.CoordMotion.MotionParams.DegreesB = false;
            _Controller.CoordMotion.MotionParams.DegreesC = false;


            // Here we are setting the Default setup params that normally get loaded via the SetupFile(Default.set)
            // The intent is for the machine class to house an XML data member that will set these
            // Legacy file support for the setup and the tool file may be included if there is a request
            // There are still a couple int members that need to be swapped out for enums

            _Controller.CoordMotion.Interpreter.SetupParams.LengthUnits = KMotion_dotNet.CANON_UNITS.CANON_UNITS_INCHES;
            _Controller.CoordMotion.Interpreter.SetupParams.X_AxisOffset = 10.0;
            _Controller.CoordMotion.Interpreter.SetupParams.Y_AxisOffset = 20.0;
            _Controller.CoordMotion.Interpreter.SetupParams.Z_AxisOffset = 30.0;

            _Controller.CoordMotion.Interpreter.SetupParams.BlockDelete = true;

            _Controller.CoordMotion.Interpreter.SetupParams.X_AxisPosition = 0.0;
            _Controller.CoordMotion.Interpreter.SetupParams.Y_AxisPosition = 0.0;
            _Controller.CoordMotion.Interpreter.SetupParams.Z_AxisPosition = 0.0;

            _Controller.CoordMotion.Interpreter.SetupParams.RadiusCompSide = KMotion_dotNet.CANON_SIDE.CANON_SIDE_OFF;
            _Controller.CoordMotion.Interpreter.SetupParams.Cycle_R = 0.0;
            _Controller.CoordMotion.Interpreter.SetupParams.Cycle_Z = 0.0;

            _Controller.CoordMotion.Interpreter.SetupParams.DistanceMode = KMotion_dotNet.DISTANCE_MODE.MODE_ABSOLUTE;

            _Controller.CoordMotion.Interpreter.SetupParams.FeedMode = 2;

            _Controller.CoordMotion.Interpreter.SetupParams.FloodCoolantOn = false;
            _Controller.CoordMotion.Interpreter.SetupParams.MistCoolantOn = false;

            _Controller.CoordMotion.Interpreter.SetupParams.CurrentMotionMode = 80;

            _Controller.CoordMotion.Interpreter.SetupParams.WorkPlane = KMotion_dotNet.CANON_PLANE.CANON_PLANE_XY;

            _Controller.CoordMotion.Interpreter.SetupParams.ToolLengthOffsetIndex = 1;
            _Controller.CoordMotion.Interpreter.SetupParams.RadiusComp = 1;

            _Controller.CoordMotion.Interpreter.SetupParams.SelectedToolSlot = 1;
            _Controller.CoordMotion.Interpreter.SetupParams.TraverseSpeed = 200;
            _Controller.CoordMotion.Interpreter.SetupParams.SpindleDirection = KMotion_dotNet.CANON_DIRECTION.CANON_STOPPED;
            _Controller.CoordMotion.Interpreter.SetupParams.ToolLengthOffset = 0.0;
 
 //           complete = false;

            // Figure out what directory we are installed into
            string codeBase = Assembly.GetExecutingAssembly().CodeBase;
            UriBuilder uri = new UriBuilder(codeBase);
            string path = Uri.UnescapeDataString(uri.Path);
            path = Path.GetDirectoryName(path);
            path = Path.GetDirectoryName(path);
            path = Path.GetDirectoryName(path);

            // pick one of the examples
            String TheGFile = path + @"\GCode Programs\Circles.ngc";

            _Controller.CoordMotion.Interpreter.Interpret(TheGFile);


            //while (!complete)
            //{
            //    var modals = _Controller.Interpreter.SetupParams.Active_GCodes;
            //    var miscs = _Controller.Interpreter.SetupParams.Active_MCodes;
            //    Console.WriteLine(String.Format("Active G-Codes :: {0}", string.Join(",", modals.Select(s => s.ToString()).ToArray())));
            //    Console.WriteLine(String.Format("Active M-Codes :: {0}", string.Join(",", miscs.Select(s => s.ToString()).ToArray())));
            //    Thread.Sleep(150);
            //}



        }

        static void Interpreter_Interpreter_CoordMotionStraightTranverse(double x, double y, double z, int sequence_number)
        {
            Console.WriteLine("Interpreter CoordMotion Straight Tranverse::  {0} | {1} | {2} | {3}", x, y, z, sequence_number);
        }

        static void Interpreter_Interpreter_CoordMotionStraightFeed(double DesiredFeedRate_in_per_sec, double x, double y, double z, int sequence_number, int ID)
        {
            Console.WriteLine("Interpreter CoordMotion Straight Feed::  {0} | {1} | {2} | {3} | {4} | {5}", DesiredFeedRate_in_per_sec, x, y, z, sequence_number, ID);
        }

        static void Interpreter_Interpreter_CoordMotionArcFeed(bool ZeroLenAsFullCircles, double DesiredFeedRate_in_per_sec, int plane, double first_end, double second_end, double first_axis, double second_axis, int rotation, double axis_end_point, double first_start, double second_start, double axis_start_point, int sequence_number, int ID)
        {
            Console.WriteLine("Interpreter CoordMotion Arc Feed::  {0} | {1} | {2} | {3} | {4} | {5} | {6} | {7} | {8} | {9} | {10} | {11} | {12}",
                ZeroLenAsFullCircles,
                DesiredFeedRate_in_per_sec,
                plane, first_end,
                second_end,
                first_axis,
                second_axis,
                rotation,
                axis_end_point,
                first_start,
                second_start,
                axis_start_point,
                sequence_number,
                ID);
        }

        static void Interpreter_InterpreterCompleted(int status, int lineno, int sequence_number, string err)
        {
            Console.WriteLine(String.Format("Interpreter Completed::  {0} | {1} | {2} | {3}", status, lineno, sequence_number, err));
//            complete = true;
        }

        static void Interpreter_InterpreterStatusUpdated(int lineno, string msg)
        {
            Console.WriteLine("Interpreter Status Update:");
            Console.WriteLine(lineno);
            Console.WriteLine(msg);
        }

        static void Interpreter_InterpreterUserCallbackRequested(string msg)
        {
            Console.WriteLine("Interpreter User Callback:");
            Console.WriteLine(msg);
        } 

        static int Interpreter_InterpreterUserMCodeCallbackRequested(int code)
        {
            throw new NotImplementedException();
        }
        #endregion
 

        static void AddHandlers()
        {


            //Set the callback for general messages
            _Controller.MessageReceived += new KMotion_dotNet.KMConsoleHandler(_Controller_MessageUpdated);
            //And Errors
            _Controller.ErrorReceived += new KMotion_dotNet.KMErrorHandler(_Controller_ErrorUpdated);


            //CoordMotion Callbacks 
            _Controller.CoordMotion.CoordMotionStraightTraverse += new KMotion_dotNet.KM_CoordMotionStraightTraverseHandler(CoordMotion_CoordMotionStraightTranverse);
            _Controller.CoordMotion.CoordMotionArcFeed += new KMotion_dotNet.KM_CoordMotionArcFeedHandler(CoordMotion_CoordMotionArcFeed);
            _Controller.CoordMotion.CoordMotionStraightFeed += new KMotion_dotNet.KM_CoordMotionStraightFeedHandler(CoordMotion_CoordMotionStraightFeed);


            //Set the Interpreter's callbacks
            _Controller.CoordMotion.Interpreter.InterpreterStatusUpdated += new KMotion_dotNet.KM_Interpreter.KM_GCodeInterpreterStatusHandler(Interpreter_InterpreterStatusUpdated);
            _Controller.CoordMotion.Interpreter.InterpreterCompleted += new KMotion_dotNet.KM_Interpreter.KM_GCodeInterpreterCompleteHandler(Interpreter_InterpreterCompleted);
            _Controller.CoordMotion.Interpreter.InterpreterUserCallbackRequested += new KMotion_dotNet.KM_Interpreter.KM_GCodeInterpreterUserCallbackHandler(Interpreter_InterpreterUserCallbackRequested);
            _Controller.CoordMotion.Interpreter.InterpreterUserMCodeCallbackRequested += new KMotion_dotNet.KM_Interpreter.KM_GCodeInterpreterUserMcodeCallbackHandler(Interpreter_InterpreterUserMCodeCallbackRequested);
        }
    }
}
