/*
Copyright © 2011 Brad Murry
All rights reserved.

BSD License:

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



KM_Axis.cs 
*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Diagnostics;
using System.Reflection;
using System.IO;

namespace KMotion_dotNet
{
    /// <summary>
    /// Axis class for KM_Controller
    /// Use to control a single axis channel
    ///  
    /// The axis channel can be configured via a c program and the KM_Controller
    /// 
    /// or
    /// 
    /// Use the configuration methods to set the desired parameters
    /// </summary>
    public partial class KM_Axis
    { 
        #region Fields
        /// <summary>
        /// KM_Controller Object for used for device access
        /// </summary>
        protected KM_Controller _Controller;
        /// <summary>
        /// Axis Channel Number
        /// Valid range = ( 0 - 7 )
        /// </summary>
        protected int _ID = -1;
        /// <summary>
        /// Call Name
        /// Use to describe what the Axis controls
        /// i.e.
        /// Positioning Fence 1, X, etc...
        /// </summary>
        protected string _Name = "";
        /// <summary>
        /// Velocity to use during positioning
        /// Jogging uses its own velocity passed in
        /// </summary>
        protected double _Velocity = 0.0;
        /// <summary>
        /// Multiplier for the velocity
        /// </summary>
        protected double _CPU = 1.0; 
        /// <summary>
        /// The level of jog percenatage computed from the commanded jog velocity
        /// </summary>
        protected double _JogPercent = 1.0;
        /// <summary>
        /// Commanded jog velocity
        /// </summary>
        protected double _JogVelocity = 0.0;
        /// <summary>
        /// Data memeber for thr homing sequence attributes
        /// </summary>
        protected KM_Axis_HomingParams _HomingParams;
         
        /// <summary>
        /// Accessor to the axis channel's tuning parameters
        /// </summary>
        protected KM_Axis_TuningParams _TuningParams;
        #endregion

        #region Properties
        /// <summary>
        /// Axis Channel Number
        /// Valid range = ( 0 - 7 )
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
        /// Use to describe what the Axis controls
        /// i.e.
        /// Positioning Fence 1, X, etc...
        /// </summary>
        /// 
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
                _Controller.WriteLine(String.Format("Vel{0}={1}", _ID, _Velocity));
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
                return _Controller.GetCommandValue<double>(String.Format("Accel{0}", _ID), true);
            }
            set
            { 
                _Controller.WriteLine(String.Format("Accel{0}={1}", _ID, value));
            }
        }


        /// <summary>
        /// Counts per unit
        /// </summary>
        public double CPU
        {
            get
            {
                return _CPU;
            }
            set
            {
                _CPU = value;
            }
        }

        /// <summary>
        /// The level of jog percenatage computed from the commanded jog velocity
        /// </summary>
        public double JogPercent
        {
            get
            {
                return _JogPercent;
            }
            set
            {
                _JogPercent = value;
            }
        }

        /// <summary>
        /// Commanded jog velocity
        /// </summary>
        public double JogVelocity
        {
            get
            {
                return _JogVelocity;
            }
            set
            {
                _JogVelocity = value;
            }
        }

        /// <summary>
        /// Data memeber for thr homing sequence attributes
        /// </summary>
        public KM_Axis_HomingParams HomingParams
        {
            get
            {
                return _HomingParams;
            }
        }

        /// <summary>
        /// Accessor to the axis channel's tuning parameters
        /// </summary>
        public KM_Axis_TuningParams TuningParams
        {
            get
            {
                return _TuningParams;
            } 
        }
        #endregion


        /// <summary>
        /// Default constructor for the Axis Object
        /// </summary>
        /// <param name="controller"></param>
        /// <param name="id"></param>
        /// <param name="name"></param>
        public KM_Axis(KM_Controller controller, int id, string name)
        {
            _Controller = controller;
            _ID = id;
            _Name = name;
            _TuningParams = new KM_Axis_TuningParams(controller, id);
            _HomingParams = new KM_Axis_HomingParams();
            _HomingParams.DefaultThread = (id > 5) ? 1 : id;
            while (_HomingParams.DefaultThread < 1)
            {
                _HomingParams.DefaultThread++;
            }
        }



        /// <summary>
        /// Sets the Axis' current 0.0 point
        /// </summary>
        public void ZeroAxis()
        {
            _Controller.WriteLine(String.Format("Zero{0}", _ID)); 
        }

        /// <summary>
        /// Sets the axis node's current position
        /// Can be used to "zero" the axis
        /// </summary>
        /// <param name="pos">new value for the current position</param>
        public void SetCurrentPosition(double pos)
        {
            try
            {
                _Controller.WriteLine(String.Format("Pos{0}={1}", _ID, pos * _CPU)); 
            }
            catch (Exception ex)
            {
                throw new Exception(String.Format("Problem with  setting current position for  axis [{0}]", _Name, pos), ex);
            }
        }


        /// <summary>
        /// Calls a move to a given absolute position and blocks the calling thread until the position has been acheived
        /// </summary>
        /// <param name="pos">absolute position to move to</param> 
        public void MoveTo(double pos)
        {
            try
            {
                _Controller.WriteLine(String.Format("MoveAtVel{0}={1} {2}", _ID, pos * _CPU, _Velocity * (Math.Abs(_CPU) / 60.0)));
                WaitforMotionComplete(0);
            }
            catch (Exception ex)
            {
                throw new Exception(String.Format("Problem with  move to axis [{0}]", _Name), ex);
            }
        }

        /// <summary>
        /// Calls a non-blocking move to a given absolute position
        /// </summary>
        /// <param name="pos">absolute position to move to</param> 
        public void StartMoveTo(double pos)
        {
            try
            {
                _Controller.WriteLine(String.Format("MoveAtVel{0}={1} {2}", _ID, pos * _CPU, _Velocity * (Math.Abs(_CPU) / 60.0)));
            }
            catch (Exception ex)
            {
                throw new Exception(String.Format("Problem with  move to axis [{0}]", _Name), ex);
            }
        }

        /// <summary>
        /// Calls a move to a given relative position and blocks the calling thread until the position has been acheived
        /// </summary>
        /// <param name="pos">position delta from current position to move to</param> 
        public void RelativeMoveTo(double pos)
        {
            try
            {
                _Controller.WriteLine(String.Format("MoveRel{0}={1}", _ID, pos * _CPU));
                WaitforMotionComplete(0);
            }
            catch (Exception ex)
            {
                throw new Exception(String.Format("Problem with relative move to axis [{0}]", _Name), ex);
            }
        }

        /// <summary>
        /// Calls a non-blocking move to a given relative position
        /// </summary>
        /// <param name="pos">position delta from current position to move to</param> 
        public void StartRelativeMoveTo(double pos)
        {
            try
            {
                _Controller.WriteLine(String.Format("MoveRel{0}={1}", _ID, pos * _CPU));
            }
            catch (Exception ex)
            {
                throw new Exception(String.Format("Problem with relative move to axis [{0}]", _Name), ex);
            }
        }



        /// <summary>
        /// Checks if the commanded axis motion is completed
        /// </summary>
        /// <returns>true if the axis is idle</returns>
        public bool MotionComplete()
        {
            try
            {
                int val = -1;
                var value = _Controller.WriteLineReadLine(String.Format("CheckDone{0}", _ID));
                var success = Int32.TryParse(value, out val);
                if (success)
                {
                    return val == 1;
                }
                else
                {
                    throw new Exception(String.Format("Bad return value for CheckDone value = [{0}].... expected 0 or 1", value)); 
                }
            }
            catch (Exception ex)
            {
                throw new Exception(String.Format("Problem checking if motion is complete [{0}]", _Name), ex);
            }
        }
        /// <summary>
        /// Blocks the calling thread until the axis is in position
        /// within a specified time limit
        /// </summary>
        /// <param name="timeout">time in ms to wait for axis to acheive position, 0 will wait indefinitely</param>
        /// <returns>true if completed</returns>
        public bool WaitforMotionComplete(int timeout)
        {
            try
            {
                Stopwatch sw = new Stopwatch();
                string value = "";
                bool success = false;
                int val = -1;
                bool istimedout = false;
                sw.Start();
                while (val != 1)
                {

                    value = _Controller.WriteLineReadLine(String.Format("CheckDone{0}", _ID));

                    success = Int32.TryParse(value, out val);
                    if (!success)
                    {
                        throw new Exception(String.Format("Bad return value for [CheckDone] function in axis {0} :: value = {1}", _Name, val));
                    }
                    if (timeout > 0)//Determines whether or not to check for a timeout condition
                    {
                        if (timeout < sw.ElapsedMilliseconds)
                        {
                            istimedout = true;
                            break;
                        }
                    }
                    Thread.Sleep(100); //This should be optimized to make sure we do not clog USB traffic
                }
                return val == 1 & !istimedout;
            }
            catch (Exception ex)
            {
                throw new Exception(String.Format("Problem checking if axis is done [{0}]", _Name), ex);
            }
        }

     
        /// <summary>
        /// Jog axis at a given velocity
        /// </summary>
        /// <param name="velocity">velocity to jog at : a negative value will reverse the motion</param>
        public void Jog(double velocity)
        {
            try
            {
                _Controller.WriteLine(String.Format("Jog{0}={1}", _ID, velocity * _CPU));
            }
            catch (Exception ex)
            {
                throw new Exception(String.Format("Problem jogging axis [{0}]", _Name), ex);
            }
        }

        /// <summary>
        /// Jog axis at the commanded preset jogvelocity * jogpercentage
        /// </summary>
        public void Jog()
        {
            try
            {
                _Controller.WriteLine(String.Format("Jog{0}={1}", _ID, _JogVelocity  * _JogPercent * _CPU));
            }
            catch (Exception ex)
            {
                throw new Exception(String.Format("Problem jogging axis [{0}]", _Name), ex);
            }
        }
     
        /// <summary>
        /// Stop the current motion
        /// </summary>
        public void Stop()
        {
            try
            {
                _Controller.WriteLine(String.Format("Jog{0}=0.0", _ID)); 
            }
            catch (Exception ex)
            {
                throw new Exception(String.Format("Problem stopping axis [{0}]", _Name), ex);
            }
        }

        /// <summary>
        /// Stop the current motion and disable the axis
        /// </summary>
        public void EStop()
        {
            Stop();
            Disable();
        }

        /// <summary> 
        /// Power the axis and bring it to an enabled state
        /// </summary>
        public void Enable()
        {
            try
            {
                _Controller.WriteLine(String.Format("EnableAxis{0}", _ID));
                Thread.Sleep(50); 
            }
            catch (Exception ex)
            {
                throw new Exception(String.Format("Problem enabling axis [{0}]", _Name), ex);
            }
        }

        /// <summary>
        /// Disable the axis 
        /// </summary>
        public void Disable()
        {
            try
            {
                _Controller.WriteLine(String.Format("DisableAxis{0}", _ID));
                Thread.Sleep(50); 
            }
            catch (Exception ex)
            {
                throw new Exception(String.Format("Problem disabling axis [{0}]", _Name), ex);
            }
        }

        /// <summary>
        /// Check if the axis is enabled
        /// </summary>
        /// <returns>true if enabled</returns>
        public bool CheckEnabled()
        {
            bool retval = false;
            try
            {
                var enabled = _Controller.WriteLineReadLine(String.Format("Enabled{0}", _ID));
                bool success = false;
                int val = -1;
                success = Int32.TryParse(enabled, out val);
                if (success)
                {
                    retval = val == 1;
                }
                else
                {
                    throw new Exception(String.Format("Bad enabled return value for [Enabled] function in axis {0} :: value = {1}", _Name, val));
                } 
            }
            catch (Exception ex)
            {
                throw new Exception(String.Format("Problem checking enabled for axis [{0}]", _Name), ex);
            }
            return retval;
        }

        /// <summary>
        /// Querys the axis channel for the actual position in counts
        /// </summary>
        /// <returns>raw counts from the channel's POS</returns>
        public double GetActualPositionCounts()
        {
            double val = _Controller.GetCommandValue<double>(String.Format("Pos{0}", _ID), true);

            return val;
        }


        /// <summary>
        /// Querys the axis channel for the actual position in user units
        /// </summary>
        /// <returns>user units from the channel's POS</returns>
        public double GetActualPosition()
        {  
            return GetActualPositionCounts() / _CPU;
        }



        /// <summary>
        /// Querys the axis channel for the commanded position in counts
        /// </summary>
        /// <returns>raw counts from the channel's DEST</returns>
        public double GetCommandedPositionCounts()
        {
            double val = _Controller.GetCommandValue<double>(String.Format("Dest{0}", _ID), true);

            return val;
        }


        /// <summary>
        /// Querys the axis channel for the commanded position in user units
        /// </summary>
        /// <returns>user units from the channel's DEST</returns>
        public double GetCommandedPosition()
        { 
            return GetCommandedPositionCounts() / _CPU;
        }


        /// <summary>
        /// Executes a homing sequence based on the settings within the HomingParams variable and waits until the execution is complete
        /// </summary>
        /// <returns>true if home routine was successful</returns>
        public bool DoHome()
        {
            int timeout = 5 * 60 * 1000;
            bool success = false;
             StartDoHome();
            _Controller.WaitForThreadComplete(_HomingParams.DefaultThread, 0);
            Stopwatch sw = new Stopwatch();
            sw.Start();
            while (!success)
            {
                success = ((HOMING_STATE)_Controller.GetUserData(_HomingParams.StatusBit) == HOMING_STATE.COMPLETE);
                if (sw.ElapsedMilliseconds > timeout)
                {
                    break;
                }
                Thread.Sleep(100);
            }
            return success;
        }

        /// <summary>
        /// Executes a homing sequence based on the settings within the HomingParams variable
        /// </summary> 
        public void StartDoHome()
        {
            _Controller.SetUserData(_HomingParams.StatusBit, (int)HOMING_STATE.NOT_HOMED);
            string autofile = "";
            switch (_HomingParams.SourceType)
            {
                case HOMING_ROUTINE_SOURCE_TYPE.AUTO:
                    autofile = WriteHomingToFile(GetAutoHomingFileSource()); 
                    break;
                case HOMING_ROUTINE_SOURCE_TYPE.FROM_FILE:
                    autofile = _HomingParams.Homing_c_Program; 
                    break;
                case HOMING_ROUTINE_SOURCE_TYPE.FROM_STRING:
                    autofile = WriteHomingToFile(_HomingParams.Homing_c_Program);
                    break;
                case HOMING_ROUTINE_SOURCE_TYPE.FROM_THREAD:
                    _Controller.ExecuteProgram(_HomingParams.DefaultThread);
                    return;
                default:
                    break;
            }
            _Controller.ExecuteProgram(_HomingParams.DefaultThread, autofile, true);
        }
        
        /// <summary>
        /// Generates the automated simple homing routine for this axis
        /// </summary>
        /// <returns>complete source code for homing .c file</returns>
        private string GetAutoHomingFileSource()
        {
            var retval = GetHomeFileSource(); 
            retval = retval.Replace("//MAINSEQUENCE", GetHomeSequence()); 
            return retval;
        }

        /// <summary>
        /// Creates the lines of code to place in the main() method
        /// </summary>
        /// <returns>partial source code for homing .c file</returns>
        public string GetHomeSequence()
            {
                StringBuilder sb = new StringBuilder();

                sb.AppendLine("//Initial Limit Switch rolling ");
                sb.AppendLine(String.Format("    EnableAxis({0});", _ID));
                sb.AppendLine(String.Format("    persist.UserData[{0}] = {1};", _HomingParams.StatusBit, (int)HOMING_STATE.MOVING_FROM_SENSOR));
                sb.AppendLine(String.Format("    MoveToStateAtVel({0}, {1}, {2}, {3}, {4}, {5});",
               _ID, (_HomingParams.HomeFastVel * _CPU) / 60, (int)_HomingParams.GetDirection() * -1, _HomingParams.HomeLimitBit, _HomingParams.GetPolarity(true), _HomingParams.SensorOffset));

                sb.AppendLine(String.Format("    persist.UserData[{0}] = {1};", _HomingParams.StatusBit, (int)HOMING_STATE.MOVING_TO_SENSOR));
                sb.AppendLine(String.Format("    MoveToStateAtVel({0}, {1}, {2}, {3}, {4}, {5});",
               _ID, (_HomingParams.HomeFastVel * _CPU) / 60, (int)_HomingParams.GetDirection(), _HomingParams.HomeLimitBit, _HomingParams.GetPolarity(false), _HomingParams.SensorOffset));

                sb.AppendLine(String.Format("    persist.UserData[{0}] = {1};", _HomingParams.StatusBit, (int)HOMING_STATE.MOVING_FROM_SENSOR));
                sb.AppendLine(String.Format("    MoveToStateAtVel({0}, {1}, {2}, {3}, {4}, {5});",
               _ID, (_HomingParams.HomeSlowVel * _CPU) / 60, (int)_HomingParams.GetDirection() * -1, _HomingParams.HomeLimitBit, _HomingParams.GetPolarity(true), _HomingParams.SensorOffset));

       

                if (_HomingParams.RepeatHomeAtSlowerRate)
                {
                    sb.AppendLine("    //Home to switch at slower rate ");
                    sb.AppendLine(String.Format("    persist.UserData[{0}] = {1};", _HomingParams.StatusBit, (int)HOMING_STATE.MOVING_TO_SENSOR));
                    sb.AppendLine(String.Format("    MoveToStateAtVel({0}, {1}, {2}, {3}, {4}, {5});",
                   _ID, (_HomingParams.HomeSlowVel * _CPU) / 60, (int)_HomingParams.GetDirection(), _HomingParams.HomeLimitBit, _HomingParams.GetPolarity(false), _HomingParams.SensorOffset));

                    sb.AppendLine(String.Format("    persist.UserData[{0}] = {1};", _HomingParams.StatusBit, (int)HOMING_STATE.MOVING_FROM_SENSOR));
                    sb.AppendLine(String.Format("    MoveToStateAtVel({0}, {1}, {2}, {3}, {4}, {5});",
                   _ID, (_HomingParams.HomeSlowVel * _CPU) / 60, (int)_HomingParams.GetDirection() * -1, _HomingParams.HomeLimitBit, _HomingParams.GetPolarity(true), _HomingParams.SensorOffset));

                }


                if (_HomingParams.SequenceType == HOMING_TYPE.HOME_TO_LIMIT_THEN_MOVE_TO_POSITION)
                {
                    sb.AppendLine("//Move To Origin ");
                    sb.AppendLine(String.Format("    persist.UserData[{0}] = {1};", _HomingParams.StatusBit, (int)HOMING_STATE.MOVING_TO_FINAL));
                    sb.AppendLine(String.Format("    MoveAtVel({0}, {1}, {2});",
                    _ID, _HomingParams.Origin * _CPU, (_HomingParams.HomeFastVel * _CPU) / 60));
                }

                sb.AppendLine(String.Format("    EnableAxisDest({0}, {1});", _ID, 0));
            
                return sb.ToString();
            }
 
        /// <summary>
        /// Creates the source code for the .c file with an empty main() method
        /// </summary>
        /// <returns>partial source code for homing .c file</returns>
        public string GetHomeFileSource()
        {
            StringBuilder sb = new StringBuilder();

            sb.AppendLine("#include \"KMotionDef.h\"");
            sb.AppendLine();
            sb.AppendLine();
            sb.AppendLine("void MoveToStateAtVel(int axis, float speed, int dir, int bit, int polarity, float sensoroffset);");
            sb.AppendLine();
            sb.AppendLine();
            sb.AppendLine("int main()");
            sb.AppendLine("  {");
            sb.AppendLine("    //MAINSEQUENCE");
            sb.AppendLine("  }");
            sb.AppendLine();
            sb.AppendLine();
            sb.AppendLine();
            sb.AppendLine(GetMoveToStateAtVel());
            sb.AppendLine();  
            return sb.ToString();
        }


        /// <summary>
        /// Auto Generates KFLOP C Code to move until an input bit changes
        /// </summary>
        /// <returns>generated code as a string</returns>
        public string GetMoveToStateAtVel()
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine("void MoveToStateAtVel(int axis, float speed, int dir, int bit, int polarity, float sensoroffset)");
            sb.AppendLine("  {");
            sb.AppendLine("    float oppdir = dir * -1;");
            sb.AppendLine("    if(ReadBit(bit)!=polarity)");
            sb.AppendLine("     {");
            sb.AppendLine("       Jog(axis, speed * dir);                                 // start moving");
            sb.AppendLine("       while (ReadBit(bit) != polarity);                       // wait for switch to change");
            sb.AppendLine("       MoveRelAtVel(axis,sensoroffset * dir,speed);            // additional distance to move onto sensor");
            sb.AppendLine("       while (!CheckDone(axis));                               // wait for position");
            sb.AppendLine("     }");
            sb.AppendLine("  }");
            return sb.ToString();
        }
 
        /// <summary>
        /// Creates a new text file in ASCII format containing the source code
        /// </summary>
        /// <param name="filesourcecode">source code for the file</param>
        /// <returns>complete path to generated file</returns>
        private string WriteHomingToFile(string filesourcecode)
        {
            string root = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            string filename = String.Format("AutoHome_Axis({0})_Channel({1})_Thread({2}).{3}", _Name, _ID, _HomingParams.DefaultThread, "c");
            string path = Path.Combine(root, filename);
            File.WriteAllText(path, filesourcecode, Encoding.ASCII);
            return path;
        }
    }
}
