using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace KMotion_dotNet
{
    /// <summary>
    /// Bulk status record uploaded from KFLOP
    /// </summary>
    public struct KM_MainStatus
    {
        #region Fields
        private int _VersionAndSize;
        private int[] _ADC;
        private int[] _DAC;
        private int[] _PWM;
        private double[] _Position;
        private double[] _Destination;
        private int[] _OutputChan0;

        private int _InputModes;
        private int _InputModes2;
        private int _OutputModes;
        private int _OutputModes2;
        private int _Enables;
        private int _AxisDone;

        private int[] _BitsDirection;
        private int[] _BitsState;

        private int _SnapBitsDirection0;
        private int _SnapBitsDirection1;

        private int _SnapBitsState0;
        private int _SnapBitsState1;

        private int _KanalogBitsStateInputs;
        private int _KanalogBitsStateOutputs;

        private int _RunOnStartUp;

        private int _ThreadActive;

        private int _StopImmediateState;

        private double _TimeStamp;

        private int[] _PC_comm;

        private int _VirtualBits;
        private int _VirtualBitsEx0; 



        private int _TicksAtUpdate; 
        #endregion

        #region Properties
        /// <summary>
        /// Bulk status record packed version and size info 
        /// </summary>
        public int VersionAndSize
        {
            get
            {
                return _VersionAndSize;
            }
        }

        /// <summary>
        /// Bulk status record ADC values (Kanalog)
        /// </summary>
        public int[] ADC
        {
            get
            {
                return _ADC;
            }
        }
        /// <summary>
        /// Bulk status record DAC values (Kanalog)
        /// </summary>
        public int[] DAC
        {
            get
            {
                return _DAC;
            }
        }
        /// <summary>
        /// Bulk status record PWM values (SnapAmp)
        /// </summary>
        public int[] PWM
        {
            get
            {
                return _PWM;
            }
        }
        /// <summary>
        /// Bulk status record Axis Measured positions (Encoder or other sensing device)
        /// </summary>
        public double[] Position
        {
            get
            {
                return _Position;
            }
        }
        /// <summary>
        /// Bulk status record Axis Commanded Destinations
        /// </summary>
        public double[] Destination
        {
            get
            {
                return _Destination;
            }
        }
        /// <summary>
        /// Bulk status record Axis First output channel specified
        /// </summary>
        public int[] OutputChan0
        {
            get
            {
                return _OutputChan0;
            }
        }
        /// <summary>
        /// Bulk status record feedback selected input modes
        /// </summary>
        public int InputModes
        {
            get
            {
                return _InputModes;
            }
        }
        /// <summary>
        /// Bulk status record feedback selected input modes #2
        /// </summary>
        public int InputModes2
        {
            get
            {
                return _InputModes2;
            }
        }
        /// <summary>
        /// Bulk status record feedback selected output modes
        /// </summary>
        public int OutputModes
        {
            get
            {
                return _OutputModes;
            }
        }
        /// <summary>
        /// Bulk status record feedback selected output modes #2
        /// </summary>
        public int OutputModes2
        {
            get
            {
                return _OutputModes2;
            }
        }

        /// <summary>
        /// Bulk status record Current axis enables
        /// </summary>
        public int Enables
        {
            get
            {
                return _Enables;
            }
        }
        /// <summary>
        /// Bulk status record Current axis completes
        /// </summary>
        public int AxisDone
        {
            get
            {
                return _AxisDone;
            }
        }
        /// <summary>
        /// Bulk status record Current KFLOP IO Bit directions (inputs vs outputs)
        /// </summary>
        public int[] BitsDirection
        {
            get
            {
                return _BitsDirection;
            }
        }
        /// <summary>
        /// Bulk status record Current IO Bit states
        /// </summary>
        public int[] BitsState
        {
            get
            {
                return _BitsState;
            }
        }
        /// <summary>
        /// Bulk status record Current SnapAmp #0 IO Bit directions (inputs vs outputs)
        /// </summary>
        public int SnapBitsDirection0
        {
            get
            {
                return _SnapBitsDirection0;
            }
        }
        /// <summary>
        /// Bulk status record Current SnapAmp #1 IO Bit directions (inputs vs outputs)
        /// </summary>
        public int SnapBitsDirection1
        {
            get
            {
                return _SnapBitsDirection1;
            }
        }
        /// <summary>
        /// Bulk status record Current SnapAmp #0 IO Bit States
        /// </summary>
        public int SnapBitsState0
        {
            get
            {
                return _SnapBitsState0;
            }
        }
        /// <summary>
        /// Bulk status record Current SnapAmp #1 IO Bit States
        /// </summary>
        public int SnapBitsState1
        {
            get
            {
                return _SnapBitsState1;
            }
        }
        /// <summary>
        /// Bulk status record Current Kanalog Input Bit States
        /// </summary>
        public int KanalogBitsStateInputs
        {
            get
            {
                return _KanalogBitsStateInputs;
            }
        }
        /// <summary>
        /// Bulk status record Current Kanalog Output Bit States
        /// </summary>
        public int KanalogBitsStateOutputs
        {
            get
            {
                return _KanalogBitsStateOutputs;
            }
        }
        /// <summary>
        /// Bulk status record Current Boot Thread Startup Enables 
        /// </summary>
        public int RunOnStartUp
        {
            get
            {
                return _RunOnStartUp;
            }
        }
        /// <summary>
        /// Bulk status record Current Thread Execution State 
        /// </summary>
        public int ThreadActive
        {
            get
            {
                return _ThreadActive;
            }
        }
        /// <summary>
        /// Bulk status record current StopImmediate (feedhold) state 
        /// </summary>
        public int StopImmediateState
        {
            get
            {
                return _StopImmediateState;
            }
        }
        /// <summary>
        /// Bulk status record Time Stamp of when status was acquired 
        /// </summary>
        public double TimeStamp
        {
            get
            {
                return _TimeStamp;
            }
        }
        /// <summary>
        /// Bulk status record KFLOP-PC Communication variables 
        /// </summary>
        public int[] PC_comm
        {
            get
            {
                return _PC_comm;
            }
        }
        /// <summary>
        /// Bulk status record Virtual bits packed 16 per word
        /// </summary>
        public int VirtualBits
        {
            get
            {
                return _VirtualBits;
            }
        }
        /// <summary>
        /// Bulk status record Extended Virtual bits packed 16 per word
        /// </summary>
        public int VirtualBitsEx0
        {
            get
            {
                return _VirtualBitsEx0;
            }
        } 
        /// <summary>
        /// Bulk status record Windows Time when status was updates (millisec since Windows Started)
        /// </summary>
        public int TicksAtUpdate
        {
            get
            {
                return _TicksAtUpdate;
            }
        } 
        #endregion


        /// <summary>
        /// Bulk status record ADC values (Kanalog)
        /// </summary>
        public int GetADC(int index)
        {
            if (index < _ADC.Length)
            {
                return _ADC[index];
            }
            else
            {
                return 0;
            }
        }
        /// <summary>
        /// Bulk status record DAC values (Kanalog)
        /// </summary>
        public int GetDAC(int index)
        {
            if (index < _DAC.Length)
            {
                return _DAC[index];
            }
            else
            {
                return 0;
            }
        }
        /// <summary>
        /// Bulk status record PWM values (SnapAmp)
        /// </summary>
        public int GetPWM(int index)
        {
            if (index < _PWM.Length)
            {
                return _PWM[index];
            }
            else
            {
                return 0;
            }
        }


        /// <summary>
        /// Bulk status record Axis Measured positions (Encoder or other sensing device)
        /// </summary>
        public double GetPosition(int index)
        {
            if (index < _Position.Length)
            {
                return _Position[index];
            }
            else
            {
                return 0;
            }
        }
        /// <summary>
        /// Bulk status record Axis Commanded Destinations
        /// </summary>
        public double GetDestination(int index)
        {
            if (index < _Destination.Length)
            {
                return _Destination[index];
            }
            else
            {
                return 0;
            }
        }

        /// <summary>
        /// Bulk status record Axis First output channel specified
        /// </summary>
        public double GetOutputChannel0(int index)
        {
            if (index < _OutputChan0.Length)
            {
                return _OutputChan0[index];
            }
            else
            {
                return 0;
            }
        }


        /// <summary>
        /// Bulk status record feedback selected input mode
        /// </summary>
        public int GetInputMode(int axischannel)
        {
            int channel = FourBitAxisIndex(axischannel);
            if (axischannel > 3)
            {
                return GetValue(_InputModes, channel, 4);
            }
            else
            {
                return GetValue(_InputModes2, channel, 4);
            }
        }
        /// <summary>
        /// Bulk status record feedback selected output mode
        /// </summary>
        public int GetOutputMode(int axischannel)
        {
            int channel = FourBitAxisIndex(axischannel);
            if (axischannel > 3)
            {
                return GetValue(_OutputModes, channel, 4);
            }
            else
            {
                return GetValue(_OutputModes2, channel, 4);
            }
        }
        /// <summary>
        /// Bulk status record Current axis enables
        /// </summary>
        public int GetAxisEnabled(int axischannel)
        {
            return GetValue(_Enables, axischannel, 1);
        }
        /// <summary>
        /// Bulk status record Current axis completes
        /// </summary>
        public int GetAxisComplete(int axischannel)
        {
            return GetValue(_AxisDone, axischannel, 1);
        }
        /// <summary>
        /// Bulk status record Current KFLOP IO Bit directions (inputs vs outputs)
        /// </summary>
        public int GetKFlopBitDirection(int index)
        {
            if (index > 31)
            {
                return GetValue(_BitsDirection[1], index - 31, 1);
            }
            else
            {
                return GetValue(_BitsDirection[0], index, 1);
            } 
        }

        /// <summary>
        /// Bulk status record Current IO Bit states
        /// </summary>
        public int GetKFlopBitState(int index)
        {
            if (index > 31)
            {
                return GetValue(_BitsState[1], index - 32, 1);
            }
            else
            {
                return GetValue(_BitsState[0], index, 1);
            }
        }


        /// <summary>
        /// Bulk status record Current SnapAmp #0 IO Bit directions (inputs vs outputs)
        /// </summary>
        public int GetSnapBitsDirection0(int index)
        {
            return GetValue(_SnapBitsDirection0, index, 1);
        }
        /// <summary>
        /// Bulk status record Current SnapAmp #1 IO Bit directions (inputs vs outputs)
        /// </summary>
        public int GetSnapBitsDirection1(int index)
        {
            return GetValue(_SnapBitsDirection1, index, 1);
        }
        /// <summary>
        /// Bulk status record Current SnapAmp #0 IO Bit States
        /// </summary>
        public int GetSnapBitsState0(int index)
        {
            return GetValue(_SnapBitsState0, index, 1);
        }
        /// <summary>
        /// Bulk status record Current SnapAmp #1 IO Bit States
        /// </summary>
        public int GetSnapBitsState1(int index)
        {
            return GetValue(_SnapBitsState1, index, 1);
        }
        /// <summary>
        /// Bulk status record Current Kanalog Input Bit States
        /// </summary>
        public int GetKanalogBitsStateInputs(int index)
        {
            return GetValue(_KanalogBitsStateInputs, index, 1);
        }
        /// <summary>
        /// Bulk status record Current Kanalog Output Bit States
        /// </summary>
        public int GetKanalogBitsStateOutputs(int index)
        {
            return GetValue(_KanalogBitsStateOutputs, index, 1);
        }
        /// <summary>
        /// Bulk status record Current Boot Thread Startup Enables 
        /// </summary>
        public int GetRunOnStartUp(int index)
        {
            return GetValue(_RunOnStartUp, index, 1);
        }
        /// <summary>
        /// Bulk status record Current Thread Execution State 
        /// </summary>
        public int GetThreadActive(int index)
        {
            return GetValue(_ThreadActive, index, 1);
        }
        /// <summary>
        /// Bulk status record Current PC-KFLOP Comm regs 
        /// </summary>
        public int GetPC_comm(int index)
        {
            if (PC_comm.Length > index)
            {
                return PC_comm[index];
            }
            else
            {
                return 0;
            }
        }

        /// <summary>
        /// Bulk status record Current Virtual Bit States 
        /// </summary>
        public int GetVirtualBits(int index)
        {
            return GetValue(_VirtualBits, index, 1);
        }
        /// <summary>
        /// Bulk status record Current Extended Virtual Bit States 
        /// </summary>
        public int GetVirtualBitsEx0(int index)
        {
            return GetValue(_VirtualBitsEx0, index, 1);
        }


        private int FourBitAxisIndex(int axischannel)
        {
            switch (axischannel)
            { 
                case 1:
                    return 4;
                case 2:
                    return 8;
                case 3:
                    return 12;
                case 4:
                    return 0;
                case 5:
                    return 4;
                case 6:
                    return 8;
                case 7:
                    return 12;
                default:
                    return 0;
            }
        }
        private int GetValue(int source, int start, int length)
        {
            return ((source >> start) & length);
        }

        #region Static Methods
        /// <summary>
        /// Create Bulk status record  
        /// </summary>
        public static KM_MainStatus GetStatus
    (
    int versionandsize,
    int[] adc,
    int[] dac,
    int[] pwm,
    double[] position,
    double[] destination,
    int[] outputchan0,
    int inputmodes,
    int inputmodes2,
    int outputmodes,
    int outputmodes2,
    int enables,
    int axisdone,
    int[] bitsdirection,
    int[] bitsstate,
    int snapbitsdirection0,
    int snapbitsdirection1,
    int snapbitsstate0,
    int snapbitsstate1,
    int kanalgobitsstateinputs,
    int kanalogbitsstateoutputs,
    int runonstartup,
    int threadactive,
    int stopimmediatestate,
    double timestamp,
    int[] pccomm,
    int virtualbits,
    int virtualbitsex0
    )
        {
            KM_MainStatus retval = new KM_MainStatus();
            retval._VersionAndSize = versionandsize;
            retval._ADC = adc;
            retval._DAC = dac;
            retval._PWM = pwm;
            retval._Position = position;
            retval._Destination = destination;
            retval._OutputChan0 = outputchan0;
            retval._InputModes = inputmodes;
            retval._InputModes2 = inputmodes2;
            retval._OutputModes = outputmodes;
            retval._OutputModes2 = outputmodes2;
            retval._Enables = enables;
            retval._AxisDone = axisdone;
            retval._BitsDirection = bitsdirection;
            retval._BitsState = bitsstate;
            retval._SnapBitsDirection0 = snapbitsdirection0;
            retval._SnapBitsDirection1 = snapbitsdirection1;
            retval._SnapBitsState0 = snapbitsstate0;
            retval._SnapBitsState1 = snapbitsstate1;
            retval._KanalogBitsStateInputs = kanalgobitsstateinputs;
            retval._KanalogBitsStateOutputs = kanalogbitsstateoutputs;
            retval._RunOnStartUp = runonstartup;
            retval._ThreadActive = threadactive;
            retval._StopImmediateState = stopimmediatestate;
            retval._TimeStamp = timestamp;
            retval._PC_comm = pccomm;
            retval._VirtualBits = virtualbits;
            retval._VirtualBitsEx0 = virtualbitsex0;

            retval._TicksAtUpdate = Environment.TickCount;
            return retval;
        } 
        #endregion

        
    }
}







