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



KM_IO.cs 
*/ 
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace KMotion_dotNet
{
    /// <summary>
    /// IO class for KM_Controller
    /// Use to control a single point(bit) of IO
    /// 
    /// 
    /// Currently supported IO:
    /// 
    /// Digital Input,
    /// Digital Output
    /// </summary>
    public class KM_IO
    {
        #region Fields
        /// <summary>
        /// KM_Controller Object for used for device access
        /// </summary>
        protected KM_Controller _Controller;

        /// <summary>
        /// Bit Number
        /// </summary>
        protected int _ID = -1;
        
        /// <summary>
        /// Call Name
        /// Use to describe what the IO controls
        /// i.e.
        /// Start Button, Clamp Pneumatic, etc...
        /// </summary>
        protected string _Name = "";

        /// <summary>
        /// Direction of IO
        /// 
        /// Only
        /// Digital Input and Digital Output 
        /// Are currently supported
        /// </summary>
        protected IO_TYPE _IOType = IO_TYPE.UNDEFINED; 
        #endregion

        #region Properties
        /// <summary>
        /// Bit Number
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
        /// Use to describe what the IO controls
        /// i.e.
        /// Start Button, Clamp Pneumatic, etc...
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
        /// Direction of IO
        /// </summary>
        public IO_TYPE IOType
        {
            get
            {
                return _IOType;
            }
            set
            {
                _IOType = value;
                UpdateIOType();
            }
        } 
        #endregion
 
        /// <summary>
        /// Primary Construtor for IO object
        /// </summary>
        /// <param name="controller"></param>
        /// <param name="id">Bit Number</param>
        /// <param name="name">Call Name</param>
        /// <param name="iotype">Sets the IO direction</param>
        public KM_IO(KM_Controller controller, int id, string name, IO_TYPE iotype)
        {
            _Controller = controller;
            _ID = id;
            _Name = name;
            _IOType = iotype;
            UpdateIOType();
        }

        /// <summary>
        /// Gets the current status of a point of IO
        /// </summary>
        /// <returns>Current digital value</returns>
        public bool GetDigitalValue()
        {
            if (_IOType != IO_TYPE.DIGITAL_IN && _IOType != IO_TYPE.DIGITAL_OUT)
            {
                throw new DMException(this, new Exception("Cannot get digital value on a non digital point of IO"), "Problem getting IO");
            }
            else
            {
                bool retval = false;

                bool success = false;
                int value = -1;
                var val = _Controller.WriteLineReadLine(String.Format("ReadBit{0}", _ID));
                success = Int32.TryParse(val, out value);
                if (success)
                {
                    retval = (value == 1);
                }
                else
                {
                    throw new Exception("Bad return value");
                }

                return retval;
            }
        }

        /// <summary>
        /// Sets the value if the IOType is Digital output
        /// Otherwise will throw an exception
        /// </summary>
        /// <param name="value">new value for IO point</param>
        public void SetDigitalValue(bool value)
        {
            if (_IOType != IO_TYPE.DIGITAL_OUT)
            {
                throw new DMException(this, new Exception("Cannot set digital value on a non digital output point of IO"), "Problem setting IO");
            }
            else
            {
                if (value)
                {
                    _Controller.WriteLine(String.Format("SetBit{0}", _ID));
                }
                else
                {
                    _Controller.WriteLine(String.Format("ClearBit{0}", _ID));
                }
            }
        }

        /// <summary>
        /// Reads the digital expression of an analog value
        /// </summary>
        /// <returns>-2048...2047</returns>
        public int GetAnalogValue()
        {

            if (_IOType != IO_TYPE.ANALOG_IN && _IOType != IO_TYPE.ANALOG_OUT)
            {
                throw new DMException(this, new Exception("Cannot get digital value on a non analog point of IO"), "Problem getting IO");
            }
            else
            {
                int retval = 0;

                bool success = false;
                int value = -1;
                var val = _Controller.WriteLineReadLine(String.Format("ADC {0}", _ID));
                success = Int32.TryParse(val, out value);
                if (success)
                {
                    retval = (value);
                }
                else
                {
                    throw new Exception("Bad return value");
                }

                return retval;
            }
        }

        /// <summary>
        /// Sets the output level of an analog output
        /// </summary>
        /// <param name="value">-2048...2047</param>
        public void SetAnalogValue(int value)
        {
            if (_IOType != IO_TYPE.ANALOG_OUT)
            {
                throw new DMException(this, new Exception("Cannot set digital value on a non analog output point of IO"), "Problem setting IO");
            }
            else
            {
                _Controller.WriteLine(String.Format("DAC{0}=", _ID, value));
            }
        }


        /// <summary>
        /// Uptdates the IO direction
        /// </summary>
        protected void UpdateIOType()
        {

            switch (_IOType)
            {
                case IO_TYPE.DIGITAL_IN:
                    _Controller.WriteLine(String.Format("SetBitDirection{0}={1}", _ID, 0));
                    break;

                case IO_TYPE.DIGITAL_OUT:
                    _Controller.WriteLine(String.Format("SetBitDirection{0}={1}", _ID, 1));
                    break;

                default:
                    break;
            }
         
        }
        /// <summary>
        /// Toggle a Digital Output Bit
        /// </summary>
        public void Toggle()
        {
            SetDigitalValue(!GetDigitalValue());
        }

    }
}
