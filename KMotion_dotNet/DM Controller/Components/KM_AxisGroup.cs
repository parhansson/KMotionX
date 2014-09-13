using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

namespace KMotion_dotNet
{
        /// <summary>
        /// KM_Controller Object for grouping Axis channels
        /// </summary>
    public class KM_AxisGroup
    {
        /// <summary>
        /// KM_Controller Object for used for device access
        /// </summary>
        protected KM_Controller _Controller;
        /// <summary>
        /// Axis Group Number
        /// </summary>
        protected int _ID = -1;
        /// <summary>
        /// Call Name
        /// Use to describe what the Axis Group controls
        /// i.e.
        /// Spindle 1 Carriage, Wrist, etc...
        /// </summary>
        protected string _Name = "";
        /// <summary>
        /// Velocity to use during positioning
        /// </summary>
        protected double _Velocity = 0.0;
        /// <summary>
        /// Maximum velocity to use during positioning and coordinated motion
        /// </summary>
        protected double _MaxVelocity = 0.0;
        /// <summary>
        /// Acceleration to use during positioning 
        /// </summary>
        protected double _Acceleration = 0.0;
        /// <summary>
        /// Axis objects that the group is comprised of
        /// </summary>
        protected List<KM_Axis> _AxisList = new List<KM_Axis>();
        /// <summary>
        /// Trajectory Planner Plan/Look ahead in seconds
        /// </summary>
        protected double _LookAhead = 1;
        /// <summary>
        /// Trajectory Planner Break Angle, larger angles require full stop
        /// </summary>
        protected double _BreakAngle = 1;
        /// <summary>
        /// Trajectory Planner Collinear tolerance at which in-line segments can be combined
        /// </summary>
        protected double _CollinearTolerance = 1;
        /// <summary>
        /// Trajectory Planner Collinear tolerance at which in-line segments can be combined
        /// </summary>
        protected double _CornerTolerance = 1;
        /// <summary>
        /// Trajectory Planner angles to be used when corner rounding
        /// </summary>
        protected double _FacetAngle = 1;
        /// <summary>
        /// Trajectory Planner max allowed length for individual segments
        /// (long linear segments can cause cord errors in non-linear systems)
        /// </summary>
        protected double _MaxLength = 1;
        /// <summary>
        /// Axis Group Number
        /// </summary>
        protected bool _DegreesA = false;
        /// <summary>
        /// Axis A is in Degrees
        /// </summary>
        protected bool _DegreesB = false;
        /// <summary>
        /// Axis B is in Degrees
        /// </summary>
        protected bool _DegreesC = false;
        /// <summary>
        /// Axis C is in Degrees
        /// </summary>
        public int ID
        {
            get
            {
                return _ID;
            }
            set
            {
                _ID = value;
            }
        } 
        /// <summary>
        /// Call Name
        /// Use to describe what the Axis Group controls
        /// i.e.
        /// Spindle 1 Carriage, Wrist, etc...
        /// </summary>
        public string Name
        {
            get
            {
                return _Name;
            }
            set
            {
                _Name = value;
            }
        } 
        /// <summary>
        /// Commanded velocity to use during positioning 
        /// </summary>
        public double Velocity
        {
            get
            {
                return _Velocity;
            }
            set
            {
                _Velocity = value;
                _Controller.WriteLine(String.Format("Vel{0}={1}", _ID, value));
            }
        } 
        /// <summary>
        /// Maximum velocity to use during positioning and coordinated motion
        /// </summary>
        public double MaxVelocity
        {
            get
            {
                return _Velocity;
            }
            set
            {
                _Velocity = value;
                _Controller.WriteLine(String.Format("Vel{0}={1}", _ID, value));
            }
        } 
        /// <summary>
        /// Velocity to use during positioning
        /// Jogging uses its own velocity passed in
        /// </summary>
        public double Acceleration
        {
            get
            {
                return _Acceleration;
            }
            set
            {
                _Acceleration = value;
                _Controller.WriteLine(String.Format("Accel{0}={1}", _ID, value));
            }
        }
        /// <summary>
        /// Axis objects that the group is comprised of
        /// </summary>
        public List<KM_Axis> AxisList
        {
            get
            {
                return _AxisList;
            }
            set
            {
                _AxisList = value;
            }
        }
        /// <summary>
        /// Trajectory Planner Lookahead/buffer time
        /// </summary>
        public double LookAhead
        {
            get
            {
                return _LookAhead;
            }
            set
            {
                _LookAhead = value; 
            }
        }
        /// <summary>
        /// Trajectory Planner Break Angle, larger angles require full stop
        /// </summary>
        public double BreakAngle
        {
            get
            {
                return _BreakAngle;
            }
            set
            {
                _BreakAngle = value;
            }
        }
        /// <summary>
        /// Trajectory Planner Collinear tolerance at which in-line segments can be combined
        /// </summary>
        public double CollinearTolerance
        {
            get
            {
                return _CollinearTolerance;
            }
            set
            {
                _CollinearTolerance = value;
            }
        }
        /// <summary>
        /// Trajectory Planner Collinear tolerance at which in-line segments can be combined
        /// </summary>
        public double CornerTolerance
        {
            get
            {
                return _CornerTolerance;
            }
            set
            {
                _CornerTolerance = value;
            }
        }
        /// <summary>
        /// Trajectory Planner angles to be used when corner rounding
        /// </summary>
        public double FacetAngle
        {
            get
            {
                return _FacetAngle;
            }
            set
            {
                _FacetAngle = value;
            }
        }
        /// <summary>
        /// Trajectory Planner max allowed length for individual segments
        /// (long linear segments can cause cord errors in non-linear systems)
        /// </summary>
        public double MaxLength
        {
            get
            {
                return _MaxLength;
            }
            set
            {
                _MaxLength = value;
            }
        }
        /// <summary>
        /// Coordinated Motion Abort State
        /// </summary>
        public bool IsAborted
        {
            get
            {
                return _Controller.CoordMotion.IsAbort;
            }
        }
        /// <summary>
        /// Coordinated Motion Halt State
        /// </summary>
        public bool IsHalted
        {
            get
            {
                return _Controller.CoordMotion.IsHalt;
            }
        }
        /// <summary>
        /// Get/Set Coordinated Motion Simulation Mode
        /// </summary>
        public bool Simulate
        {
            get
            {
                return _Controller.CoordMotion.IsSimulation;
            }
            set
            {
                _Controller.CoordMotion.IsSimulation = value;
            }
        }

        /// <summary>
        /// Get/Set Previous Stopped after Halt State
        /// </summary>
        public PREV_STOP_TYPE PreviousStopped
        {
            get
            {
                return _Controller.CoordMotion.IsPreviouslyStopped;
            }
            set
            {
                _Controller.CoordMotion.IsPreviouslyStopped = value;
            }
        }

        
        
        /// <summary>
        /// Coordinated Motion Feed Rate Setting Multiplier
        /// </summary>
        public double FeedRateOverride
        {
            get
            {
                return _Controller.CoordMotion.FeedRateOverride;
            }
            set
            {
                _Controller.CoordMotion.FeedRateOverride = value;
            }
        }

        /// <summary>
        /// Creates group of axes
        /// </summary>
        public KM_AxisGroup(KM_Controller controller, params KM_Axis[] axislist)
        {
            _Controller = controller;
            if (axislist != null)
            {
                for (int i = 0; i < axislist.Length; i++)
                {
                    if (axislist[i] == null)
                    {
                        _AxisList.Add(new KM_Axis(_Controller, -1, "not used"));
                    }
                    else
                    {
                        _AxisList.Add(axislist[i]);
                    }
                }
            }
        }

        /// <summary>
        /// Define, configure, and enable an axis group
        /// </summary>
        public void EnableGroup()
        {
            _Controller.CoordMotion.ClearAbort();
            if (AxisList.Count < 2)
            {
                throw new DMException(this,
                    new Exception("More than one Axis Required!"), 
                    "Cannot create an axis group with less than two axis!");
            }
            else
            {
                var axismap = new List<int> { -1, -1, -1, -1, -1, -1, -1, -1 };
                _Controller.WriteLine(String.Format("DefineCS = {0} {1} {2} {3} {4} {5}",
                axismap[0], axismap[1], axismap[2], axismap[3], axismap[4], axismap[5]));

                for (int i = 0; i < _AxisList.Count; i++)
                {
                    if (_AxisList[i].ID > -1)
                    {
                        switch (i)
                        {
                            case 0:
                                _Controller.CoordMotion.MotionParams.MaxAccelX = _AxisList[i].Acceleration;
                                _Controller.CoordMotion.MotionParams.MaxVelX = _AxisList[i].Velocity;
                                _Controller.CoordMotion.MotionParams.CountsPerInchX = _AxisList[i].CPU;
                                break;
                            case 1:
                                _Controller.CoordMotion.MotionParams.MaxAccelY = _AxisList[i].Acceleration;
                                _Controller.CoordMotion.MotionParams.MaxVelY = _AxisList[i].Velocity;
                                _Controller.CoordMotion.MotionParams.CountsPerInchY = _AxisList[i].CPU;
                                break;
                            case 2:
                                _Controller.CoordMotion.MotionParams.MaxAccelZ = _AxisList[i].Acceleration;
                                _Controller.CoordMotion.MotionParams.MaxVelZ = _AxisList[i].Velocity;
                                _Controller.CoordMotion.MotionParams.CountsPerInchZ = _AxisList[i].CPU;
                                break;
                            case 3:
                                _Controller.CoordMotion.MotionParams.MaxAccelA = _AxisList[i].Acceleration;
                                _Controller.CoordMotion.MotionParams.MaxVelA = _AxisList[i].Velocity;
                                _Controller.CoordMotion.MotionParams.CountsPerInchA = _AxisList[i].CPU;
                                break;
                            case 4:
                                _Controller.CoordMotion.MotionParams.MaxAccelB = _AxisList[i].Acceleration;
                                _Controller.CoordMotion.MotionParams.MaxVelB = _AxisList[i].Velocity;
                                _Controller.CoordMotion.MotionParams.CountsPerInchB = _AxisList[i].CPU;
                                break;
                            case 5:
                                _Controller.CoordMotion.MotionParams.MaxAccelC = _AxisList[i].Acceleration;
                                _Controller.CoordMotion.MotionParams.MaxVelC = _AxisList[i].Velocity;
                                _Controller.CoordMotion.MotionParams.CountsPerInchC = _AxisList[i].CPU;
                                break;
                            default:
                                break;
                        }
                    }
                }
                _Controller.CoordMotion.MotionParams.BreakAngle = _BreakAngle;
                _Controller.CoordMotion.MotionParams.CollinearTolerance = _CollinearTolerance;
                _Controller.CoordMotion.MotionParams.CornerTolerance = _CornerTolerance;
                _Controller.CoordMotion.MotionParams.FacetAngle = _FacetAngle;
                _Controller.CoordMotion.MotionParams.MaxLinearLength = _MaxLength;
                _Controller.CoordMotion.MotionParams.TPLookahead = _LookAhead;
                _Controller.CoordMotion.MotionParams.DegreesA = _DegreesA;
                _Controller.CoordMotion.MotionParams.DegreesB = _DegreesB;
                _Controller.CoordMotion.MotionParams.DegreesC = _DegreesC;
            }
        }
        /// <summary>
        /// Abort and disable an entire coordinated group
        /// </summary>
        public void DisableGroup()
        {
            Abort(); 
            _Controller.CoordMotion.FlushSegments();
        }
        /// <summary>
        /// Perform Rapid Move (independent motion) of multiple axis to a desired destination
        /// and wait until motion is completed
        /// </summary>
        public void RapidTo(double[] position)
        {
            double[] pos = new double[] { 0, 0, 0, 0, 0, 0 };

            for (int i = 0; i < position.Length; i++)
            {
                if (i < pos.Length)
                {
                    pos[i] = position[i];
                }
            }
            _Controller.CoordMotion.StraightTraverse(pos[0], pos[1], pos[2], pos[3], pos[4], pos[5], false);
            _Controller.CoordMotion.WaitForMoveXYZABCFinished();
        }
        /// <summary>
        /// Perform Start of Rapid Move (independent motion) of multiple axis to a desired destination
        /// </summary>
        public void StartRapidTo(double[] position)
        {
            double[] pos = new double[] { 0, 0, 0, 0, 0, 0 };

            for (int i = 0; i < position.Length; i++)
            {
                if (i < pos.Length)
                {
                    pos[i] = position[i];
                }
            }
            _Controller.CoordMotion.StartTraverse(pos[0], pos[1], pos[2], pos[3], pos[4], pos[5], false, this);
        }

        /// <summary>
        /// Flush/clear coordinated motion buffer and wait for any rapids to complete
        /// </summary>
         public void StartPath()
        {
            _Controller.CoordMotion.FlushSegments();
            _Controller.CoordMotion.WaitForMoveXYZABCFinished();

        }
        /// <summary>
        /// Add a linear motion to the coordinated motion buffer
        /// </summary>
        public void AddLine(double[] nextpoint)
        {
            double[] pos = new double[] { 0, 0, 0, 0, 0, 0 };

            for (int i = 0; i < nextpoint.Length; i++)
            {
                if (i < pos.Length)
                {
                    pos[i] = nextpoint[i];
                }
            }
            _Controller.CoordMotion.StraightFeed(_Velocity, 
                pos[0], pos[1], pos[2], pos[3], pos[4], pos[5], 0, 0);
        }

        /// <summary>
        /// Perform Coordinated Arc Move of multiple axis to a desired destination
        /// </summary>
        public void AddArc(double endx, double endy, double centerx, double centery, 
            bool clockwise, double finalz)
        {
            var direction = clockwise ? KMotion_dotNet.CANON_DIRECTION.CANON_CLOCKWISE : CANON_DIRECTION.CANON_COUNTERCLOCKWISE;
            _Controller.CoordMotion.ArcFeed(_Velocity,
                (int)KMotion_dotNet.CANON_PLANE.CANON_PLANE_XY, endx, endy, centerx, centery,
                (int)direction, finalz, 0, 0, 0, 0, 0);
        }
        /// <summary>
        /// Perform Coordinated Arc Move (in specified plane) of multiple axis to a desired destination
        /// </summary>
        public void AddArc(double endx, double endy, double centerx, double centery,
           CANON_PLANE plane, bool clockwise, double finalz)
        {
            var direction = clockwise ? KMotion_dotNet.CANON_DIRECTION.CANON_CLOCKWISE : CANON_DIRECTION.CANON_COUNTERCLOCKWISE;
            _Controller.CoordMotion.ArcFeed(_Velocity,
                (int)plane, endx, endy, centerx, centery,
                (int)direction, finalz, 0, 0, 0, 0, 0);
        }
        /// <summary>
        /// Perform Coordinated Arc Move (in specified plane) of multiple axis to a desired destination
        /// with aditional a,b,c, axes linearly interpolated
        /// </summary>
        public void AddArc(double endx, double endy, double centerx, double centery,
            CANON_PLANE plane, bool clockwise, double finalz, double a, double b, double c)
        {
            var direction = clockwise ? KMotion_dotNet.CANON_DIRECTION.CANON_CLOCKWISE : CANON_DIRECTION.CANON_COUNTERCLOCKWISE;
            _Controller.CoordMotion.ArcFeed(_Velocity,
                (int)plane, endx, endy, centerx, centery,
                (int)direction, finalz, a, b, c, 0, 0);
        }
        /// <summary>
        /// Insert special embedded IO commaand into the Coordinated moition buffer
        /// </summary>
        public void AddCommand(int command)
        {
            _Controller.CoordMotion.DoSpecialCommand(command);
        }
        /// <summary>
        /// Finalize any motion that resides in the coordinated motion buffer
        /// </summary>
        public void CompletePath()
        {
            _Controller.CoordMotion.FlushSegments();
            _Controller.CoordMotion.WaitForMoveXYZABCFinished();
        }
        /// <summary>
        /// Halt any Coordinated Motion
        /// </summary>
        public void Halt()
        {
            _Controller.CoordMotion.Halt();
        }
        /// <summary>
        /// Clear any Coordinated Motion Halt condition
        /// </summary>
        public void ClearHalt()
        {
            _Controller.CoordMotion.ClearHalt();
        }
        /// <summary>
        /// Abort any Coordinated Motion
        /// </summary>
        public void Abort()
        {
            _Controller.CoordMotion.Abort();
        }
        /// <summary>
        /// Clear any Coordinated Motion Abort condition
        /// </summary>
        public void ClearAbort()
        {
            _Controller.CoordMotion.ClearAbort();
        }

        /// <summary>
        /// Checks if the commanded axis group motion is completed
        /// </summary>
        /// <returns>true if the axis is idle</returns>
        public bool MotionComplete()
        {
            try
            { 
                int val = -1;
                var value = _Controller.WriteLineReadLine("CheckDoneXYZABC");
                var success = Int32.TryParse(value, out val);
                if (success)
                {
                    return val == 1;
                }
                else
                {
                    throw new Exception(String.Format("Bad return value for CheckDoneXYZABC value = [{0}].... expected 0 or 1", value));
                }
            }
            catch (Exception ex)
            {
                throw new Exception(String.Format("Problem checking if motion is complete [{0}]", _Name), ex);
            }
        }
        /// <summary>
        /// Blocks the calling thread until the axis group is in position
        /// within a specified time limit
        /// </summary>
        /// <param name="timeout">time in ms to wait for axis group to acheive position, 0 will wait indefinitely</param>
        /// <returns>true if completed</returns>
        public bool WaitforMotionComplete(int timeout)
        {
            try
            {
                Stopwatch sw = new Stopwatch();
           
                sw.Start();

                _Controller.CoordMotion.WaitForMoveXYZABCFinished();
                return sw.ElapsedMilliseconds <= timeout;
            }
            catch (Exception ex)
            {
                throw new Exception(String.Format("Problem checking if axis is done [{0}]", _Name), ex);
            }
        }


        //Interpreter Commands....

        /// <summary>
        /// Launch GCode Interpreter executing specified GCode file
        /// </summary>
        /// <param name="file">Gcode File Path</param>
        public void RunGcodeFile(string file)
        {
            _Controller.CoordMotion.Interpreter.Interpret(file);
        }

        /// <summary>
        /// Launch GCode Interpreter executing specified GCode file
        /// </summary>
        /// <param name="file">Gcode File Path</param>
        /// <param name="firstline">Gcode line number to start</param>
        /// <param name="lastline">Gcode line number to stop</param>
        public void RunGcodeFileSegment(string file, int firstline, int lastline)
        {
            _Controller.CoordMotion.Interpreter.Interpret(file, firstline, lastline, 0);
        }
        /// <summary>
        /// Set Tool File name to be used by the GCode Interpreter
        /// </summary>
        /// <param name="toolFile">Tool File Path String</param>
        public void SetToolFile(string toolFile)
        {
            _Controller.CoordMotion.Interpreter.ToolFile = toolFile;
        }
        /// <summary>
        /// Set Params File name to be used by the GCode Interpreter
        /// </summary>
        /// <param name="paramsfile">Params File Path String</param>
        public void SetParamsFile(string paramsfile)
        {
            _Controller.CoordMotion.Interpreter.SetupFile = paramsfile;
        }
        /// <summary>
        /// Set Tool Table Entry to be used by the GCode Interpreter
        /// </summary>
        /// <param name="num">Tool Number</param>
        /// <param name="length">Tool length offset</param>
        /// <param name="diameter">Tool diameter used in radius compensation</param>
        public void SetToolTableEntry(int num, double length, double diameter)
        {
            _Controller.CoordMotion.Interpreter.SetupParams.SetTool(num, length, diameter);
        }
        /// <summary>
        /// Set Work Fixture Offset to be used by the GCode Interpreter
        /// </summary>
        /// <param name="num">Fixture Number</param>
        /// <param name="x">x offset</param>
        /// <param name="y">y offset</param>
        /// <param name="z">z offset</param>
        /// <param name="a">a offset</param>
        /// <param name="b">b offset</param>
        /// <param name="c">c offset</param>
        public void SetOffsetData(int num, double x, double y, double z, double a, double b, double c)
        {
            _Controller.CoordMotion.Interpreter.SetOrigin(num, x, y, z, a, b, c);
        }
        /// <summary>
        /// Specifies a specific M Code action to be performed for a encountered
        /// MCodeSet. Parameters and data are specific to the action type
        /// </summary>
        /// <param name="num">MCode Mapping Number</param>
        /// <param name="type">type of action to be performed</param>
        /// <param name="p1">parameter #1</param>
        /// <param name="p2">parameter #3</param>
        /// <param name="p3">parameter #3</param>
        /// <param name="p4">parameter #4</param>
        /// <param name="p5">parameter #5</param>
        /// <param name="data">string parameter - typically a file name</param>
        public void SetMCodeAction(int num, MCODE_TYPE type, 
            double p1, double p2, double p3, double p4, double p5, string data)
        {
            _Controller.CoordMotion.Interpreter.SetMcodeAction(num, type, p1, p2, p3, p4, p5, data);
        }
    }
}
