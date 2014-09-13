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
    /// Method to be used for creating a home sequence
    /// </summary>
    public enum HOMING_ROUTINE_SOURCE_TYPE : int
    {
        /// <summary>
        /// Runs homing sequence from a pre-existing file
        /// </summary>
        FROM_FILE,
        /// <summary>
        /// Creates a file from the Homing_c_Program contents
        /// </summary>
        FROM_STRING,
        /// <summary>
        /// Simply executes a thread
        /// </summary>
        FROM_THREAD,
        /// <summary>
        /// Generates a .c file based on the Homing Params
        /// </summary>
        AUTO
    }
    /// <summary>
    /// direction factor for homing
    /// </summary>
    public enum HOMING_DIRECTION : int
    {
        /// <summary>
        /// move in positive(rising) direction
        /// </summary>
        POSITIVE = 1,
        /// <summary>
        /// move in negative(falling) direction
        /// </summary>
        NEGATIVE = -1
    }
    /// <summary>
    /// enum for homing types
    /// </summary>
    public enum HOMING_TYPE : int
    {
        /// <summary>
        /// Simply home to switch and roll back off
        /// </summary>
        HOME_TO_LIMIT_THEN_ROLL_OFF,
        /// <summary>
        /// Perform the roll  off sensor and then move to a set position
        /// </summary>
        HOME_TO_LIMIT_THEN_MOVE_TO_POSITION
    }
    /// <summary>
    /// enum for homing state
    /// </summary>
   public enum HOMING_STATE : int
    {
        /// <summary>
        /// initial state
        /// </summary>
        NOT_HOMED,
        /// <summary>
        /// indicates the sensor bit has been set to the requested state 
        /// </summary>
        ON_SENSOR,
        /// <summary>
        /// axis is moving towards the sensor bit
        /// </summary>
        MOVING_TO_SENSOR,
        /// <summary>
        /// axis is moving away from the sensor bit
        /// </summary>
        MOVING_FROM_SENSOR,
        /// <summary>
        /// axis is moving to the final position
        /// </summary>
        MOVING_TO_FINAL,
        /// <summary>
        /// homing sequence has been completed
        /// </summary>
        COMPLETE,
        /// <summary>
        /// an error occured during the homing routine
        /// </summary>
        ERROR
    }

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
        /// Multiple parameters to describe the homing sequence for the axis
        /// so that a proper sequence can be auto-generated in some circumstances.
        /// (This has not been fully implemented)
        /// </summary>
        public class KM_Axis_HomingParams
        {
            #region Fields
            private int _StatusBit = 0;
            private int _DefaultThread = 1;
            private int _SequencePriority = -1;

            private bool _HomeNegative = true;
            private bool _RepeateHomeAtSlowerRate = true;
            private HOMING_TYPE _SequenceType = HOMING_TYPE.HOME_TO_LIMIT_THEN_ROLL_OFF;

            private int _HomeLimitBit = -1;
            private bool _HomeLimitState = true;
            private int _EncoderBit = -1;

            private double _HomeFastVel = 0;
            private double _HomeSlowVel = 0;
            private double _Origin = 0;
            private double _SensorOffset = 0;
            private bool _SetToZero = true;
            private HOMING_ROUTINE_SOURCE_TYPE _SourceType = HOMING_ROUTINE_SOURCE_TYPE.AUTO;
            private string _Homing_c_Program = "";
            #endregion

            #region Properties
            /// <summary>
            /// Which bit is to be used as Home input
            /// </summary>
            public int StatusBit
            {
                get
                {
                    return _StatusBit;
                }
                set
                {
                    _StatusBit = value;
                }
            }
            /// <summary>
            /// Which KFLOP Thread should be used to run the Home C Program in
            /// </summary>
            public int DefaultThread
            {
                get
                {
                    return _DefaultThread;
                }
                set
                {
                    _DefaultThread = value;
                }
            }
            /// <summary>
            /// Determines the order of axes to be homed
            /// </summary>
            public int SequencePriority
            {
                get
                {
                    return _SequencePriority;
                }
                set
                {
                    _SequencePriority = value;
                }
            }

            /// <summary>
            /// Direction to be homed
            /// </summary>
            public bool HomeNegative
            {
                get
                {
                    return _HomeNegative;
                }
                set
                {
                    _HomeNegative = value;
                }
            }
            /// <summary>
            /// Determines if Home sequence is to be performed twice
            /// with the second pass at a slower speed
            /// </summary>
            public bool RepeatHomeAtSlowerRate
            {
                get
                {
                    return _RepeateHomeAtSlowerRate;
                }
                set
                {
                    _RepeateHomeAtSlowerRate = value;
                }
            }
            /// <summary>
            /// Enumerated Code describing the overall homing type for the axis
            /// </summary>
            public HOMING_TYPE SequenceType
            {
                get
                {
                    return _SequenceType;
                }
                set
                {
                    _SequenceType = value;
                }
            }

            /// <summary>
            /// Which bit is to be used as a Limit while Homing
            /// </summary>
            public int HomeLimitBit
            {
                get
                {
                    return _HomeLimitBit;
                }
                set
                {
                    _HomeLimitBit = value;
                }
            }
            /// <summary>
            /// State (polarity) when Input is actively in sensor
            /// </summary>
            public bool HomeLimitState
            {
                get
                {
                    return _HomeLimitState;
                }
                set
                {
                    _HomeLimitState = value;
                }
            }
            /// <summary>
            /// Which Input Bit is to be used for any encoder index mark homing 
            /// </summary>
            public int EncoderBit
            {
                get
                {
                    return _EncoderBit;
                }
                set
                {
                    _EncoderBit = value;
                }
            }

            /// <summary>
            /// Initial Fast Homing Velocity
            /// </summary>
            public double HomeFastVel
            {
                get
                {
                    return _HomeFastVel;
                }
                set
                {
                    _HomeFastVel = value;
                }
            }
            /// <summary>
            /// Secondary Slow Homing Velocity
            /// </summary>
            public double HomeSlowVel
            {
                get
                {
                    return _HomeSlowVel;
                }
                set
                {
                    _HomeSlowVel = value;
                }
            }
            /// <summary>
            /// Origin value to be set after homing
            /// </summary>
            public double Origin
            {
                get
                {
                    return _Origin;
                }
                set
                {
                    _Origin = value;
                }
            }
            /// <summary>
            /// Amount to move after finding Home Switch
            /// </summary>
            public double SensorOffset
            {
                get
                {
                    return _SensorOffset;
                }
                set
                {
                    _SensorOffset = value;
                }
            }
            /// <summary>
            /// Option to Zero Command and Position
            /// </summary>
            public bool SetToZero
            {
                get
                {
                    return _SetToZero;
                }
                set
                {
                    _SetToZero = value;
                }
            }

            /// <summary>
            /// Determines method of homing - Fixed C file or Autogenerated C code
            /// </summary>
            public HOMING_ROUTINE_SOURCE_TYPE SourceType
            {
                get
                {
                    return _SourceType;
                }
                set
                {
                    _SourceType = value;
                }
            }
            /// <summary>
            /// C Program Filename for Homing
            /// </summary>
            public string Homing_c_Program
            {
                get
                {
                    return _Homing_c_Program;
                }
                set
                {
                    _Homing_c_Program = value;
                }
            }
            #endregion 

            /// <summary>
            /// Returns direction to move for homing
            /// </summary>
            public HOMING_DIRECTION GetDirection()
            {
                return (_HomeNegative) ? HOMING_DIRECTION.NEGATIVE : HOMING_DIRECTION.POSITIVE;
            }

            /// <summary>
            /// 
            /// </summary>
            public int GetPolarity(bool reverse)
            {
                if (reverse)
                {
                    return (_HomeLimitState) ? 0 : 1;
                }
                else
                {
                    return (_HomeLimitState) ? 1 : 0;
                }
            }
        }
    }
}
