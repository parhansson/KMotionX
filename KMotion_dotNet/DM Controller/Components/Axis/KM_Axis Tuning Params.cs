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
        /// <summary>
        /// KFLOP Axis Channel parameters for Tuning and motion profiles
        /// </summary>
        public class KM_Axis_TuningParams
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
            /// Get or Set the max jerk  (for independent moves and jogs)
            /// </summary>
            public double Jerk
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("Jerk{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("Jerk{0}={1}", _ID, value));
                }
            }

            /// <summary>
            /// Get or Set PID Proportional Gain.
            /// </summary>
           public double Pgain
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("P{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("P{0}={1}", _ID, value));
                }
            }

            /// <summary>
            /// Get or Set PID Integral Gain.
            /// </summary>
            public double Igain
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("I{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("I{0}={1}", _ID, value));
                }
            }

            /// <summary>
            /// Get or Set PID Derivative Gain.
            /// </summary>
            public double Dgain
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("D{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("D{0}={1}", _ID, value));
                }
            }

            /// <summary>
            /// Set or get Acceleration feed forward for the axis.
            /// </summary>
            public double FFAccel
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("FFAccel{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("FFAccel{0}={1}", _ID, value));
                }
            }

            /// <summary>
            /// Set or get Velocity feed forward for the axis.
            /// </summary>
            public double FFVel
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("FFVel{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("FFVel{0}={1}", _ID, value));
                }
            }

            /// <summary>
            ///  Sets or gets the amount of Backlash Compensation Offset to be applied.
            ///  See also BacklashMode and BacklashRate.
            /// </summary>
            public double BacklashAmount
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("BacklashAmount{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("BacklashAmount{0}={1}", _ID, value));
                }
            }

            /// <summary>
            ///  Sets or gets the rate at which the amount of Backlash Compensation Offset will be applied.
            ///  See also BacklashMode and BacklashAmount.
            /// </summary>
            public double BacklashRate
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("BacklashRate{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("BacklashRate{0}={1}", _ID, value));
                }
            }

            /// <summary>
            ///   Sets or gets the Backlash Compensation mode from either BACKLASH_OFF (0) to BACKLASH_LINEAR (1).  
            ///   When the backlash mode is set to Linear mode, whenever the commanded destination begins moving 
            ///   in the positive direction, a positive offset of the amount, BacklashAmount, will be applied.  
            ///   The offset will be ramped upward as a linear function of time at the rate specified as the BacklashRate.  
            ///   Whenever the commanded destination begins moving in the negative direction the offset will be 
            ///   removed by ramping downward toward zero at the same rate.  If the the Backlash Compensation mode 
            ///   is set to BACKLASH_OFF (0), no backlash compensation will be applied.
            ///   See also BacklashRatee and BacklashAmount.
            /// </summary>
            public int BacklashMode
            {
                get
                {
                    return GetConsoleIntegerValue(String.Format("BacklashMode{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("BacklashMode{0}={1}", _ID, value));
                }
            }


            /// <summary>
            /// Get or Set 3 or 4 phase commutation offset.  When brushless commutation is performed, the desired Output Magnitude is distributed and applied to the various motor coils as a function of position.  The commutation offset shifts the manner in which the Output Magnitude is applied.
            /// For a 3 phase brushless output mode, commutation offset is used in the following manner.
            /// PhaseA = OutputMagnitude * sin((Position+CommutationOffset)*invDistPerCycle*2π)
            /// PhaseB = OutputMagnitude * sin((Position+CommutationOffset)*invDistPerCycle*2π + 2π/3)
            /// PhaseC = OutputMagnitude * sin((Position+CommutationOffset)*invDistPerCycle*2π + 4π/3)
            ///
            /// For a 4 phase brushless output mode, commutation offset is used in the following manner.
            /// PhaseA = OutputMagnitude *  sin((Position+CommutationOffset)*invDistPerCycle*2π)
            /// PhaseB = OutputMagnitude * cos((Position+CommutationOffset)*invDistPerCycle*2π)
            ///
            /// See also invDistPerCycle and Configuration Parameters.
            /// </summary>
            public double CommutationOffset
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("CommutationOffset{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("CommutationOffset{0}={1}", _ID, value));
                }
            }
            /// <summary>
            ///  Get or Set gain while error is within the deadband range.  See DeadBand Description.
            ///  See Servo Flow Diagram.
            /// </summary>
            public double DeadBandGain
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("DeadBandGain{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("DeadBandGain{0}={1}", _ID, value));
                }
            }
            /// <summary>
            ///  Get or Set range where deadband gain is to be applied.  See DeadBand Description.
            ///  See Servo Flow Diagram.
            /// </summary>
            public double DeadBandRange
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("DeadBandRange{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("DeadBandRange{0}={1}", _ID, value));
                }
            }



            /// <summary>
            ///  Set or get the position input mode for an axis.  See description of this parameter on the 
            ///  Configuration Screen.  Valid modes are (from PC-DSP.h): 
            /// </summary>
            public int InputMode
            {
                get
                {
                    return GetConsoleIntegerValue(String.Format("InputMode{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("InputMode{0}={1}", _ID, value));
                }
            }

            /// <summary>
            ///  Set or get first Input Offset of an axis.  See description of this parameter on the Configuration Screen.
            /// </summary>
            public double InputOffset0
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("InputOffset0{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("InputOffset0{0}={1}", _ID, value));
                }
            }

            /// <summary>
            ///  Set or get second Input Offset of an axis.  See description of this parameter on the Configuration Screen.
            /// </summary>
            public double InputOffset1
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("InputOffset1{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("InputOffset1{0}={1}", _ID, value));
                }
            }

            /// <summary>
            ///  Set or get first Input Channel of an axis.  See description of this parameter on the Configuration Screen.
            /// </summary>
            public double InputChan0
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("InputChan0{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("InputChan0{0}={1}", _ID, value));
                }
            }

            /// <summary>
            ///  Set or get second Input Channel of an axis.  See description of this parameter on the Configuration Screen.
            /// </summary>
            public double InputChan1
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("InputChan1{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("InputChan1{0}={1}", _ID, value));
                }
            }

            /// <summary>
            ///  Set or get first Input Gain of an axis.  See description of this parameter on the Configuration Screen.
            /// </summary>
            public double InputGain0
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("InputGain0{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("InputGain0{0}={1}", _ID, value));
                }
            }

            /// <summary>
            ///  Set or get second Input Gain of an axis.  See description of this parameter on the Configuration Screen.
            /// </summary>
            public double InputGain1
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("InputGain1{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("InputGain1{0}={1}", _ID, value));
                }
            }



            /// <summary>
            ///  Get or Set distance per cycle (specified as an inverse) of an axis.  
            ///  May specify the cycle of either a Stepper of Brushless Motor.  
            ///  See description of this parameter on the Configuration Screen.
            /// </summary>
            public double InvDistPerCycle
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("InvDistPerCycle{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("InvDistPerCycle{0}={1}", _ID, value));
                }
            }
            /// <summary>
            ///  Set or get Lead Compensation for an axis.  Lead Compensation is used to compensate for lag caused by motor inductance.
            /// </summary>
            public double Lead
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("Lead{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("Lead{0}={1}", _ID, value));
                }
            }

            /// <summary>
            /// Configures Limit Switch Options.  Specify Hex value as decribed in KMotionDef.h
            /// See also Configuration Screen.
            /// </summary>
            public string LimitSwitch
            {
                get
                {
                    return _Controller.WriteLineReadLine(String.Format("LimitSwitch{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("LimitSwitch{0}={1}", _ID, value));
                }
            }


            /// <summary>
            ///  Sets or gets the axis to which the current axis is to be slaved.  
            ///  The current axis becomes a slave and will follow the motion of the specified 
            ///  Master Axis.  More than one axis can be slaved to a single master axis if 
            ///  desired.  When slaved, changes in the commanded destination of the master axis 
            ///  will be mirrored as changes in the slaved axis's destination however scaled 
            ///  by the SlaveGain (as specified in the Slave Axis).  The SlaveGain my be 
            ///  negative if opposing motion is desired.
            ///  
            /// Setting the Master Axis value to -1 disables the Slave mode.
            /// </summary>
            public int MasterAxis
            {
                get
                {
                    return GetConsoleIntegerValue(String.Format("MasterAxis{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("MasterAxis{0}={1}", _ID, value));
                }
            }
            /// <summary>
            ///  Set or get Maximum Error for axis (Limits magnitude of error entering PID).
            ///  See Servo Flow Diagram and Step Response Screen for more information. 
            /// </summary>
             public double MaxErr
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("MaxErr{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("MaxErr{0}={1}", _ID, value));
                }
            }
            /// <summary>
            ///  Set or get the maximum allowed following error before disabling the axis. 
            /// </summary>
            public double MaxFollowingError
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("MaxFollowingError{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("MaxFollowingError{0}={1}", _ID, value));
                }
            }
            /// <summary>
            ///  Set or get Maximum Integrator "wind up" for axis.  Integrator saturates 
            ///  at the specified value.   See also Servo Flow Diagram and Step Response 
            ///  Screen for further information.
            /// </summary>
            public double MaxI
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("MaxI{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("MaxI{0}={1}", _ID, value));
                }
            }
            /// <summary>
            ///  Set or get Maximum Output for an axis.  Limits magnitude of servo output.  
            ///  Output saturates at the specified value.  See also Servo Flow Diagram 
            ///  and Step Response Screen for further information.
            /// </summary>
            public double MaxOutput
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("MaxOutput{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("MaxOutput{0}={1}", _ID, value));
                }
            }
            /// <summary>
            ///  Set or get the position output mode for an axis.  
            ///  See description of this parameter on the Configuration Screen.
            ///  Valid modes are (from PC_DSP.h): 
            /// </summary>
            public int OutputMode
            {
                get
                {
                    return GetConsoleIntegerValue(String.Format("OutputMode{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("OutputMode{0}={1}", _ID, value));
                }
            }
            /// <summary>
            ///  Get or Set the Output Offset of an axis.  For DAC Servo output mode the 
            ///  output (DAC) signal can be offset.   The Output Offset is applied after 
            ///  any Output Gain value.  The Output Offset can be used to reduce any DAC 
            ///  output offset or Amplifier input offset that may cause motor axis drift 
            ///  occurs when the DAC is commanded to zero (disabled).  In other output 
            ///  modes the OutputGain value will have no effect.
            /// </summary>
            public double OutputOffset
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("OutputOffset{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("OutputOffset{0}={1}", _ID, value));
                }
            }
            /// <summary>
            ///  Get or Set the first Output Channel of an axis.   
            ///  See description of this parameter on the Configuration Screen.
            /// </summary>
            public double OutputChan0
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("OutputChan0{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("OutputChan0{0}={1}", _ID, value));
                }
            }
            /// <summary>
            ///  Get or Set the second Output Channel of an axis.   
            ///  See description of this parameter on the Configuration Screen.
            /// </summary>
            public double OutputChan1
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("OutputChan1{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("OutputChan1{0}={1}", _ID, value));
                }
            }
            /// <summary>
            ///  Get or Set the Output Gain of an axis.  For Axes of Step/Dir,
            ///  CL Step Dir, or MicroStep output mode, the output motion can be scaled or reversed.
            ///  Normally there is no need to use a value other than -1.0 or +1.0.  
            ///  For DAC Servo output mode the output signal (DAC) can be scaled or reversed.   
            ///  Again, normally there is no need to use a value other than -1.0 or +1.0.  
            ///  In other output modes the OutputGain value will have no effect.
            /// </summary>
            public double OutputGain
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("OutputGain{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("OutputGain{0}={1}", _ID, value));
                }
            }
            /// <summary>
            ///  Sets or gets the Slave Gain for the axis.  See also MasterAxis for more information
            /// </summary>
            public double SlaveGain
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("SlaveGain{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("SlaveGain{0}={1}", _ID, value));
                }
            }
            /// <summary>
            ///  Set or get the nominal output magnitude used for axis if in MicroStepping 
            ///  Output Mode to the specified value.  This will be the output amplitude when 
            ///  stopped or moving slowly.  If Lead Compensation is used, the amplitude 
            ///  while moving may be higher.
            /// </summary>
            public double StepperAmplitude
            {
                get
                {
                    return GetConsoleDoubleValue(String.Format("StepperAmplitude{0}", _ID));
                }
                set
                {
                    _Controller.WriteLine(String.Format("StepperAmplitude{0}={1}", _ID, value));
                }
            }


            #endregion

            /// <summary>
            /// Associate KFLOP Axis Channel with Parameters
            /// </summary>
            public KM_Axis_TuningParams(KM_Controller controller, int id)
            {
                _Controller = controller;
                _ID = id;
            }


            /// <summary>
            /// Set or get IIR Z domain servo filter #0
            /// </summary>
            public void SetIIR0(double p1, double p2, double p3, double p4, double p5)
            {
                _Controller.WriteLine(String.Format("IIR{0} 0={1} {2} {3} {4} {5}",
                    _ID, p1, p2, p3, p4, p5));
            }
            /// <summary>
            /// Set or get IIR Z domain servo filter #1
            /// </summary>
            public void SetIIR1(double p1, double p2, double p3, double p4, double p5)
            {
                _Controller.WriteLine(String.Format("IIR{0} 1={1} {2} {3} {4} {5}",
                    _ID, p1, p2, p3, p4, p5));
            }
            /// <summary>
            /// Set or get IIR Z domain servo filter #2
            /// </summary>
            public void SetIIR2(double p1, double p2, double p3, double p4, double p5)
            {
                _Controller.WriteLine(String.Format("IIR{0} 2={1} {2} {3} {4} {5}",
                    _ID, p1, p2, p3, p4, p5));
            }

            private double GetConsoleDoubleValue(string command)
            {
                var pos = _Controller.WriteLineReadLine(command);
                bool success = false;
                double val = 0.0;
                success = Double.TryParse(pos, out val);
                if (!success)
                {
                    //Maybe write to an error log...
                }
                return val;
            }
            private int GetConsoleIntegerValue(string command)
            {
                var pos = _Controller.WriteLineReadLine(command);
                bool success = false;
                int val = 0;
                success = Int32.TryParse(pos, out val);
                if (!success)
                {
                    //Maybe write to an error log...
                }
                return val;
            }
        }
    }
}
