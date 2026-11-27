using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace KMotion_dotNet
{
    /// <summary>
    /// Defines form PC-DSP.h defining number of available devices
    /// </summary>
    public class StatusConsts
    {
        /// <summary>
        /// Number of Persist variable included in Status Upload
        /// </summary>
        public const int N_PC_COMM_PERSIST = 8;
        /// <summary>
        /// Number of Kanalog DACs
        /// </summary>
        public const int N_DACS = 8;         // Kanalog
        /// <summary>
        /// Number of Kanalog ADCs
        /// </summary>
        public const int N_ADCS = 8;
        /// <summary>
        /// Number 8-bit PWMS on IO Pins
        /// </summary>
        public const int N_PWMS = 8;
        /// <summary>
        /// Number Encoder Inputs on KFLOP 0 - 7
        /// </summary>
        public const int N_ENCS = 8;
        /// <summary>
        /// Number Encoder Inputs on Kogna 0 - 19
        /// </summary>
        public const int N_ENCS_KOGNA = 20;
        /// <summary>
        /// Number DACs on Kogna
        /// </summary>
        public const int N_DACS_KOGNA = 8;   // referenced after Kanalog as 8-15
        /// <summary>
        /// Number ADCs on KOGNA
        /// </summary>
        public const int N_ADCS_KOGNA = 4;   // referenced after Kanalog as 8-11

        /// <summary>
        /// Number ADCs on SnapAmp per SnapAmpA
        /// </summary>
        public const int N_ADCS_SNAP = 8;      // per snap amp 
        /// <summary>
        /// Number PWMs on SnapAmp per SnapAmpA
        /// </summary>
        public const int N_PWMS_SNAP = 4;      // per snap amp
        /// <summary>
        /// Number Encoder Inputs on SnapAmp 0 - 19
        /// </summary>
        public const int N_ENCS_SNAP = 4;
        /// <summary>
        /// Number Axis Channels available for Kogna 
        /// (KFLOP has only 8 fields for additional channels will be zero)
        /// </summary>
        public const int N_CHANNELS_KOGNA = 16;
        /// <summary>
        /// Number 8-bit PWMs for Kogna/KFLOP
        /// </summary>
        public const int N_IO_PWMS = 8;
        /// <summary>
        /// Number 12-bit (High Resolution) HRPWMs for Kogna
        /// </summary>
        public const int N_KOGNA_HRPWM = 4;
    }
    /// <summary>
    /// This structure matches exactly the binary form as the Kogna/KFLOP
    /// MAIN_STATUS Structure so it can be uploaded as a binary image.
    /// </summary>
    public struct KM_MainStatusRaw
    {
        /// <summary>
        /// Bulk status record packed version and size info 
        /// bits 16-23 = version, bits 0-15 = size in words
        /// </summary>
        public int VersionAndSize;
        /// <summary>
        /// Bulk status record ADC values (8 Kanalog + 2 x 8 SnapAmp)
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = StatusConsts.N_ADCS + 2 * StatusConsts.N_ADCS_SNAP)] public int[] ADC;
        /// <summary>
        /// Bulk status record DAC values (8 Kanalog)
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = StatusConsts.N_DACS)] public int[] DAC;
        /// <summary>
        /// Bulk status record PWM values (8 obsolete PWMS + 2 x 4 SnapAmp)
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = StatusConsts.N_PWMS + 2 * StatusConsts.N_PWMS_SNAP)] public int[] PWM;
        /// <summary>
        /// Bulk status record Axis Measured positions (Encoder or other sensing device)
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = StatusConsts.N_CHANNELS_KOGNA)] public double[] Position;
        /// <summary>
        /// Bulk status record Axis Commanded Destinations
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = StatusConsts.N_CHANNELS_KOGNA)] public double[] Destination;
        /// <summary>
        /// Bulk status record Axis First output channel specified
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = StatusConsts.N_CHANNELS_KOGNA)] public byte[] OutputChan0;

        /// <summary>
        /// Bulk status record feedback selected input modes 
        /// 4 bits for each axis (not sure why only using 16 of 32 bits) 
        /// Axes 0-3
        /// </summary>
        public int InputModes;
        /// <summary>
        /// Bulk status record feedback selected input modes #2
        /// 4 bits for each axis
        /// Axes 4-7
        /// </summary>
        public int InputModes2;
        /// <summary>
        /// Bulk status record feedback selected input modes #3
        /// 4 bits for each axis
        /// Axes 8-11
        /// </summary>
        public int InputModes3;
        /// <summary>
        /// Bulk status record feedback selected input modes #4
        /// 4 bits for each axis
        /// Axes 12-15
        /// </summary>
        public int InputModes4;
        /// <summary>
        /// Bulk status record feedback selected output modes
        /// Axes 0-3
        /// </summary>
        public int OutputModes;     // 4 bits for each axis 
        /// <summary>
        /// Bulk status record feedback selected output modes #2
        /// Axes 4-7
        /// </summary>
        public int OutputModes2;    // 4 bits for each axis 
        /// <summary>
        /// Bulk status record feedback selected output modes #3
        /// Axes 8-11
        /// </summary>
        public int OutputModes3;    // 4 bits for each axis 
        /// <summary>
        /// Bulk status record feedback selected output modes #4
        /// Axes 11-15
        /// </summary>
        public int OutputModes4;    // 4 bits for each axis 
        /// <summary>
        /// Bulk status record Current axis enables
        /// </summary>
        public int Enables;         // 1 bit  for each axis 
        /// <summary>
        /// Bulk status record Current axis completes
        /// </summary>
        public int AxisDone;        // 1 bit  for each axis 

        /// <summary>
        /// Bulk status record Current KFLOP 64 IO Bit directions (inputs vs outputs)
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)] public int[] BitsDirection;   // KFLOP - 64 bits of I/O direction 1 = output
        /// <summary>
        /// Bulk status record Current Kogna IO Bit directions (inputs vs outputs)
        /// Kogna - 24 bits 200-223 of I/O direction 1 = output
        /// </summary>
        public int BitsDirection200;                                                       // Kogna - 24 bits 200-223 of I/O direction 1 = output
        /// <summary>
        /// Bulk status record Current Kogna IO Bit directions (inputs vs outputs)
        /// Kogna - 10 bits 280-289 of I/O direction 1 = output
        /// </summary>
        public int BitsDirection280;                                                       // Kogna - 10 bits 280-289 of I/O direction 1 = output
        /// <summary>
        /// Bulk status record Current KFLOP/Kogna 64 IO Bit states
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)] public int[] BitsState;       // KFLOP - 64 bits of state lsb=I/O bit0
        /// <summary>
        /// Bulk status record Current KFLOP/Kogna 64 IO Bit states
        /// Kogna - 90 bits 200-289 of state lsb=I/O bit200
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)] public int[] BitsState200;    // Kogna - 90 bits 200-289 of state lsb=I/O bit200
        /// <summary>
        /// Bulk status record 
        /// Kogna - 10 Pin function modes 2 bits per pin 4 HRPWM + 6 SPI
        /// </summary>
        public int PinMuxModes;                                                            // Kogna - 10 Pin function modes 2 bits per pin 4 HRPWM + 6 SPI

        /// <summary>
        /// Bulk status record Kogna 4 ADCs
        /// format  12 bits data (signed extended to 16 bits)
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = StatusConsts.N_ADCS_KOGNA)] public short[] Kogna_ADC;   //  format  12 bits data (signed extended to 16 bits)
        /// <summary>
        /// Bulk status record Kogna 8 DACSs
        /// format  12 bits data (signed extended to 16 bits)
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = StatusConsts.N_DACS_KOGNA)] public short[] Kogna_DAC;   //  format  12 bits data

        /// <summary>
        /// Bulk status record Kogna 
        /// Prescale sets frequency of all 8 8-bit PWMs
        /// </summary>
        public byte Kogna_PWM_Prescale;                  // Prescale sets frequency of all 8 8-bit PWMs

        /// <summary>
        /// Bulk status record Kogna 
        /// current pulse settings of all 8 8-bit IO PWMs
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = StatusConsts.N_IO_PWMS)] public byte[] Kogna_PWM;                // current pulse settings
        /// <summary>
        /// Bulk status record Kogna 
        /// current PWM Enables to control Pin of all 8 8-bit IO PWMs
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = StatusConsts.N_IO_PWMS)] public byte[] Kogna_PWM_Enables;        // PWM Enables to control Pin?

        /// <summary>
        /// Bulk status record Kogna 
        /// current Kogna HRPWMs Periods (common for HRPWM 0 and 1)
        /// </summary>
        public UInt16 HRPWMPeriod01;  // Kogna HRPWMs Periods
        /// <summary>
        /// Bulk status record Kogna 
        /// current Kogna HRPWM Periods (for HRPWM 2)
        /// </summary>
        public UInt16 HRPWMPeriod2;
        /// <summary>
        /// Bulk status record Kogna 
        /// current Kogna HRPWM Periods (for HRPWM 3)
        /// </summary>
        public UInt16 HRPWMPeriod3;

        /// <summary>
        /// Bulk status record Kogna 
        /// current Kogna Kogna 4 HRPWMs Pulse widths
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = StatusConsts.N_KOGNA_HRPWM)] public UInt16[] HRPWM;    // Kogna HRPWMs Pulse widths

        /// <summary>
        /// Bulk status record Current SnapAmp #0 IO Bit directions (inputs vs outputs)
        /// </summary>
        public int SnapBitsDirection0;   // Snap - 32 bits of I/O direction 1=output 16-29 GPIO only, Card 0
        /// <summary>
        /// Bulk status record Current SnapAmp #1 IO Bit directions (inputs vs outputs)
        /// </summary>
        public int SnapBitsDirection1;   // Snap - 32 bits of I/O direction 1=output 16-29 GPIO only, Card 1
        /// <summary>
        /// Bulk status record Current SnapAmp #0 IO Bit States
        /// </summary>
        public int SnapBitsState0;       // Snap - 32 bits of state  16-29 GPIO 0-7 Diff 8-15 OPTO, Card 0
        /// <summary>
        /// Bulk status record Current SnapAmp #1 IO Bit States
        /// </summary>
        public int SnapBitsState1;       // Snap - 32 bits of state  16-29 GPIO 0-7 Diff 8-15 OPTO, Card 1

        /// <summary>
        /// Bulk status record Current Kanalog Input Bit States
        /// </summary>
        public int KanalogBitsStateInputs;   // Kanalog - 16 bits 128-143
        /// <summary>
        /// Bulk status record Current Kanalog Output Bit States
        /// </summary>
        public int KanalogBitsStateOutputs;  // Kanalog - 24 bits 144-167

        /// <summary>
        /// Bulk status record Current Boot Thread Startup Enables 
        /// </summary>
        public int RunOnStartUp;    // word Bits 1-7 selects which threads to execute on startup   

        /// <summary>
        /// Bulk status record Current Thread Execution State 
        /// </summary>
        public int ThreadActive;           // one bit for each thread, 1=active, bit 0 - primary
        /// <summary>
        /// Bulk status record current StopImmediate (feedhold) state 
        /// </summary>
        public int StopImmediateState;    // Status of Stop Coordinated Motion Immediately

        /// <summary>
        /// Bulk status record Time Stamp in seconds of when status was acquired 
        /// </summary>
        public double TimeStamp;   // Time in seconds (since KFlop boot) this status was aquired
        /// <summary>
        /// Bulk status record KFLOP-PC 8 Communication variables 100-107
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = StatusConsts.N_PC_COMM_PERSIST)] public int[] PC_comm;// 8 persist Variables constantly uploaded to send misc commands/data to PC

        /// <summary>
        /// Bulk status record Virtual bits packed 16 per word 48-63
        /// </summary>
        public UInt32 VirtualBits;        // Virtual I/O bits simulated in memory
        /// <summary>
        /// Bulk status record Extended Virtual bits packed 32 per word
        /// 1024-1055
        /// </summary>
        public UInt32 VirtualBitsEx0;     // only upload 32 1024 Expanded Virtual Bits 
    }


    /// <summary>
    /// Bulk status record uploaded from KFLOP
    /// </summary>
    public struct KM_MainStatus
    {
        /// <summary>        
        /// This structure matches exactly the binary form as the Kogna/KFLOP
        /// MAIN_STATUS Structure so it can be uploaded as a binary image.
        /// </summary>
        public KM_MainStatusRaw MainStatus;

        /// <summary>
        /// Time in milliseconds since Windows Started where
        /// Status record was last uploaded
        /// Note: rolls over every 49 days
        /// </summary>
        public int _TicksAtUpdate;

        #region Properties
        /// <summary>
        /// Bulk status record packed version and size info 
        /// bits 16-23 = version, bits 0-15 = size in words
        /// </summary>
        public int VersionAndSize
        {
            get
            {
                return MainStatus.VersionAndSize;
            }
        }

        /// <summary>
        /// Bulk status record ADC values (8 Kanalog + 2 x 8 SnapAmp)
        /// </summary>
        public int[] ADC
        {
            get
            {
                return MainStatus.ADC;
            }
        }
        /// <summary>
        /// Bulk status record DAC values (8 Kanalog)
        /// </summary>
        public int[] DAC
        {
            get
            {
                return MainStatus.DAC;
            }
        }
        /// <summary>
        /// Bulk status record PWM values (8 obsolete PWMS + 2 x 4 SnapAmp)
        /// </summary>
        public int[] PWM
        {
            get
            {
                return MainStatus.PWM;
            }
        }
        /// <summary>
        /// Bulk status record Axis Measured positions (Encoder or other sensing device)
        /// </summary>
        public double[] Position
        {
            get
            {
                return MainStatus.Position;
            }
        }
        /// <summary>
        /// Bulk status record Axis Commanded Destinations
        /// </summary>
        public double[] Destination
        {
            get
            {
                return MainStatus.Destination;
            }
        }
        /// <summary>
        /// Bulk status record Axis First output channel specified
        /// </summary>
        public byte[] OutputChan0
        {
            get
            {
                return MainStatus.OutputChan0;
            }
        }
        /// <summary>
        /// Bulk status record feedback selected input modes 
        /// Axes 0-3
        /// </summary>
        public int InputModes
        {
            get
            {
                return MainStatus.InputModes;
            }
        }
        /// <summary>
        /// Bulk status record feedback selected input modes #2
        /// Axes 4-7
        /// </summary>
        public int InputModes2
        {
            get
            {
                return MainStatus.InputModes2;
            }
        }
        /// <summary>
        /// Bulk status record feedback selected input modes #3
        /// Axes 8-11
        /// </summary>
        public int InputModes3
        {
            get
            {
                return MainStatus.InputModes3;
            }
        }
        /// <summary>
        /// Bulk status record feedback selected input modes #4
        /// Axes 12-15
        /// </summary>
        public int InputModes4
        {
            get
            {
                return MainStatus.InputModes4;
            }
        }
        /// <summary>
        /// Bulk status record feedback selected output modes
        /// Axes 0-3
        /// </summary>
        public int OutputModes
        {
            get
            {
                return MainStatus.OutputModes;
            }
        }
        /// <summary>
        /// Bulk status record feedback selected output modes #2
        /// Axes 4-7
        /// </summary>
        public int OutputModes2
        {
            get
            {
                return MainStatus.OutputModes2;
            }
        }
        /// <summary>
        /// Bulk status record feedback selected output modes #3
        /// Axes 8-11
        /// </summary>
        public int OutputModes3
        {
            get
            {
                return MainStatus.OutputModes3;
            }
        }
        /// <summary>
        /// Bulk status record feedback selected output modes #4
        /// Axes 11-15
        /// </summary>
        public int OutputModes4
        {
            get
            {
                return MainStatus.OutputModes4;
            }
        }
        /// <summary>
        /// Bulk status record Current axis enables
        /// </summary>
        public int Enables
        {
            get
            {
                return MainStatus.Enables;
            }
        }
        /// <summary>
        /// Bulk status record Current axis completes
        /// </summary>
        public int AxisDone
        {
            get
            {
                return MainStatus.AxisDone;
            }
        }
        /// <summary>
        /// Bulk status record Current KFLOP 64 IO Bit directions (inputs vs outputs)
        /// </summary>
        public int[] BitsDirection
        {
            get
            {
                return MainStatus.BitsDirection;
            }
        }
        /// <summary>
        /// Bulk status record Current Kogna IO Bit directions (inputs vs outputs)
        /// Kogna - 24 bits 200-223 of I/O direction 1 = output
        /// </summary>
        public int BitsDirection200
        {
            get
            {
                return MainStatus.BitsDirection200;
            }
        }
        /// <summary>
        /// Bulk status record Current Kogna IO Bit directions (inputs vs outputs)
        /// Kogna - 10 bits 280-289 of I/O direction 1 = output
        /// </summary>
        public int BitsDirection280
        {
            get
            {
                return MainStatus.BitsDirection280;
            }
        }
        /// <summary>
        /// Bulk status record Current KFLOP/Kogna 64 IO Bit states
        /// </summary>
        public int[] BitsState
        {
            get
            {
                return MainStatus.BitsState;
            }
        }
        /// <summary>
        /// Bulk status record Current KFLOP/Kogna 64 IO Bit states
        /// Kogna - 90 bits 200-289 of state lsb=I/O bit200
        /// </summary>
        public int[] BitsState200
        {
            get
            {
                return MainStatus.BitsState200;
            }
        }
        /// <summary>
        /// Bulk status record 
        /// Kogna - 10 Pin function modes 2 bits per pin 4 HRPWM + 6 SPI
        /// </summary>
        public int PinMuxModes
        {
            get
            {
                return MainStatus.PinMuxModes;
            }
        }
        /// <summary>
        /// Bulk status record Kogna 4 ADCs
        /// format  12 bits data (signed extended to 16 bits)
        /// </summary>
        public short[] Kogna_ADC
        {
            get
            {
                return MainStatus.Kogna_ADC;
            }
        }
        /// <summary>
        /// Bulk status record Kogna 8 DACSs
        /// format  12 bits data (signed extended to 16 bits)
        /// </summary>
        public short[] Kogna_DAC
        {
            get
            {
                return MainStatus.Kogna_DAC;
            }
        }
        /// <summary>
        /// Bulk status record Kogna 
        /// Prescale sets frequency of all 8 8-bit PWMs
        /// </summary>
        public byte Kogna_PWM_Prescale
        {
            get
            {
                return MainStatus.Kogna_PWM_Prescale;
            }
        }
        /// <summary>
        /// Bulk status record Kogna 
        /// current pulse settings of all 8 8-bit IO PWMs
        /// </summary>
        public byte[] Kogna_PWM
        {
            get
            {
                return MainStatus.Kogna_PWM;
            }
        }
        /// <summary>
        /// Bulk status record Kogna 
        /// current PWM Enables to control Pin of all 8 8-bit IO PWMs
        /// </summary>
        public byte[] Kogna_PWM_Enables
        {
            get
            {
                return MainStatus.Kogna_PWM_Enables;
            }
        }
        /// <summary>
        /// Bulk status record Kogna 
        /// current Kogna HRPWMs Periods (common for HRPWM 0 and 1)
        /// </summary>
        public ushort HRPWMPeriod01
        {
            get
            {
                return MainStatus.HRPWMPeriod01;
            }
        }
        /// <summary>
        /// Bulk status record Kogna 
        /// current Kogna HRPWM Periods (for HRPWM 2)
        /// </summary>
        public ushort HRPWMPeriod2
        {
            get
            {
                return MainStatus.HRPWMPeriod2;
            }
        }
        /// <summary>
        /// Bulk status record Kogna 
        /// current Kogna HRPWM Periods (for HRPWM 3)
        /// </summary>
        public ushort HRPWMPeriod3
        {
            get
            {
                return MainStatus.HRPWMPeriod3;
            }
        }
        /// <summary>
        /// Bulk status record Kogna 
        /// current Kogna Kogna 4 HRPWMs Pulse widths
        /// </summary>
        public ushort[] HRPWM
        {
            get
            {
                return MainStatus.HRPWM;
            }
        }
        /// <summary>
        /// Bulk status record Current SnapAmp #0 IO Bit directions (inputs vs outputs)
        /// </summary>
        public int SnapBitsDirection0
        {
            get
            {
                return MainStatus.SnapBitsDirection0;
            }
        }
        /// <summary>
        /// Bulk status record Current SnapAmp #1 IO Bit directions (inputs vs outputs)
        /// </summary>
        public int SnapBitsDirection1
        {
            get
            {
                return MainStatus.SnapBitsDirection1;
            }
        }
        /// <summary>
        /// Bulk status record Current SnapAmp #0 IO Bit States
        /// </summary>
        public int SnapBitsState0
        {
            get
            {
                return MainStatus.SnapBitsState0;
            }
        }
        /// <summary>
        /// Bulk status record Current SnapAmp #1 IO Bit States
        /// </summary>
        public int SnapBitsState1
        {
            get
            {
                return MainStatus.SnapBitsState1;
            }
        }
        /// <summary>
        /// Bulk status record Current Kanalog Input Bit States
        /// </summary>
        public int KanalogBitsStateInputs
        {
            get
            {
                return MainStatus.KanalogBitsStateInputs;
            }
        }
        /// <summary>
        /// Bulk status record Current Kanalog Output Bit States
        /// </summary>
        public int KanalogBitsStateOutputs
        {
            get
            {
                return MainStatus.KanalogBitsStateOutputs;
            }
        }
        /// <summary>
        /// Bulk status record Current Boot Thread Startup Enables 
        /// </summary>
        public int RunOnStartUp
        {
            get
            {
                return MainStatus.RunOnStartUp;
            }
        }
        /// <summary>
        /// Bulk status record Current Thread Execution State 
        /// </summary>
        public int ThreadActive
        {
            get
            {
                return MainStatus.ThreadActive;
            }
        }
        /// <summary>
        /// Bulk status record current StopImmediate (feedhold) state 
        /// </summary>
        public int StopImmediateState
        {
            get
            {
                return MainStatus.StopImmediateState;
            }
        }
        /// <summary>
        /// Bulk status record Time Stamp in seconds of when status was acquired 
        /// </summary>
        public double TimeStamp
        {
            get
            {
                return MainStatus.TimeStamp;
            }
        }
        /// <summary>
        /// Bulk status record KFLOP-PC 8 Communication variables 100-107
        /// </summary>
        public int[] PC_comm
        {
            get
            {
                return MainStatus.PC_comm;
            }
        }
        /// <summary>
        /// Bulk status record Virtual bits packed 16 per word 48-63
        /// </summary>
        public int VirtualBits
        {
            get
            {
                return (int)MainStatus.VirtualBits;
            }
        }
        /// <summary>
        /// Bulk status record Extended Virtual bits packed 32 per word
        /// 1024-1055
        /// </summary>
        public int VirtualBitsEx0
        {
            get
            {
                return (int)MainStatus.VirtualBitsEx0;
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
        /// <param name="index">channel number to get</param>
        /// <returns>value</returns>
        public int GetADC(int index)
        {
            if (index < MainStatus.ADC.Length)
            {
                return MainStatus.ADC[index];
            }
            else
            {
                return 0;
            }
        }
        /// <summary>
        /// Bulk status record DAC values (Kanalog)
        /// </summary>
        /// <param name="index">channel number to get</param>
        /// <returns>value</returns>
        public int GetDAC(int index)
        {
            if (index < MainStatus.DAC.Length)
            {
                return MainStatus.DAC[index];
            }
            else
            {
                return 0;
            }
        }
        /// <summary>
        /// Bulk status record PWM values (SnapAmp)
        /// </summary>
        /// <param name="index">channel number to get (8 obsolete + 2 x 4 SnapAmp)</param>
        /// <returns>value</returns>
        public int GetPWM(int index)
        {
            if (index < MainStatus.PWM.Length)
            {
                return MainStatus.PWM[index];
            }
            else
            {
                return 0;
            }
        }


        /// <summary>
        /// Bulk status record Axis Measured positions (Encoder or other sensing device)
        /// </summary>
        /// <param name="index">Axis Number</param>
        /// <returns>value</returns>
        public double GetPosition(int index)
        {
            if (index < MainStatus.Position.Length)
            {
                return MainStatus.Position[index];
            }
            else
            {
                return 0;
            }
        }
        /// <summary>
        /// Bulk status record Axis Commanded Destinations
        /// </summary>
        /// <param name="index">Axis Number</param>
        /// <returns>value</returns>
        public double GetDestination(int index)
        {
            if (index < MainStatus.Destination.Length)
            {
                return MainStatus.Destination[index];
            }
            else
            {
                return 0;
            }
        }

        /// <summary>
        /// Bulk status record Axis First output channel specified
        /// </summary>
        /// <param name="index">Axis Number</param>
        /// <returns>value</returns>
        public double GetOutputChannel0(int index)
        {
            if (index < MainStatus.OutputChan0.Length)
            {
                return MainStatus.OutputChan0[index];
            }
            else
            {
                return 0;
            }
        }


        /// <summary>
        /// Bulk status record feedback selected 4-bit input mode
        /// </summary>
        /// <param name="axischannel">Axis Number</param>
        /// <returns>value</returns>
        public int GetInputMode(int axischannel)
        {
            int channel = FourBitAxisIndex(axischannel);
            if (axischannel > 11)
            {
                return GetValue(MainStatus.InputModes4, channel, 4);
            }
            else if (axischannel > 7)
            {
                return GetValue(MainStatus.InputModes3, channel, 4);
            }
            else if (axischannel > 3)
            {
                return GetValue(MainStatus.InputModes2, channel, 4);
            }
            else
            {
                return GetValue(MainStatus.InputModes, channel, 4);
            }
        }
        /// <summary>
        /// Bulk status record 4-bit output mode
        /// </summary>
        /// <param name="axischannel">Axis Number</param>
        /// <returns>value</returns>
        public int GetOutputMode(int axischannel)
        {
            int channel = FourBitAxisIndex(axischannel);
            if (axischannel > 11)
            {
                return GetValue(MainStatus.OutputModes4, channel, 4);
            }
            else if (axischannel > 7)
            {
                return GetValue(MainStatus.OutputModes3, channel, 4);
            }
            else if (axischannel > 3)
            {
                return GetValue(MainStatus.OutputModes2, channel, 4);
            }
            else
            {
                return GetValue(MainStatus.OutputModes, channel, 4);
            }
        }
        /// <summary>
        /// Bulk status record Current axis enables
        /// </summary>
        /// <param name="axischannel">Axis Number</param>
        /// <returns>value</returns>
        public int GetAxisEnabled(int axischannel)
        {
            return GetValue(MainStatus.Enables, axischannel, 1);
        }
        /// <summary>
        /// Bulk status record Current axis completes
        /// </summary>
        /// <param name="axischannel">Axis Number</param>
        /// <returns>value</returns>
        public int GetAxisComplete(int axischannel)
        {
            return GetValue(MainStatus.AxisDone, axischannel, 1);
        }
        /// <summary>
        /// Bulk status record Current KFLOP/Kogna 64 IO Bit directions (inputs vs outputs)
        /// </summary>
        /// <param name="index">IO Bit Number</param>
        /// <returns>value 0=input 1=output</returns>
        public int GetKFlopBitDirection(int index)
        {
            if (index > 31)
            {
                return GetValue(MainStatus.BitsDirection[1], index - 31, 1);
            }
            else
            {
                return GetValue(MainStatus.BitsDirection[0], index, 1);
            } 
        }
        /// <summary>
        /// Bulk status record Current Kogna 24 IO Bit directions for Bits 200-223(inputs vs outputs)
        /// </summary>
        /// <param name="index">IO Bit Number</param>
        /// <returns>value 0=input 1=output</returns>
        public int GetBitDirection200(int index)
        {
            return GetValue(MainStatus.BitsDirection200, index, 1);
        }
        /// <summary>
        /// Bulk status record Current Kogna 10 IO Bit directions for Bits 280-289(inputs vs outputs)
        /// </summary>
        /// <param name="index">IO Bit Number</param>
        /// <returns>value 0=input 1=output</returns>
        public int GetBitDirection280(int index)
        {
            return GetValue(MainStatus.BitsDirection280, index, 1);
        }
        /// <summary>
        /// Bulk status record Current KFLOP/Kogna 64 IO Bit states
        /// </summary>
        /// <param name="index">IO Bit Number</param>
        /// <returns>value</returns>
        public int GetKFlopBitState(int index)
        {
            if (index > 31)
            {
                return GetValue(MainStatus.BitsState[1], index - 32, 1);
            }
            else
            {
                return GetValue(MainStatus.BitsState[0], index, 1);
            }
        }
        /// <summary>
        /// Bulk status record Current Kogna 90 IO Bit states (Bits 200-289)
        /// </summary>
        /// <param name="index">IO Bit Number</param>
        /// <returns>value</returns>
        public int GetBitState200(int index)
        {
            return GetValue(MainStatus.BitsState200[(index - 200) >> 5], (index - 200) & 31, 1);
        }
        /// <summary>
        /// Bulk status record Current Kogna 10 Pin function modes 2 bits per pin 4 HRPWM + 6 SPI
        /// </summary>
        /// <param name="index">device selector 0-9 (0-3 HRPWM, 4-9 SPI)</param>
        /// <returns>value</returns>
        public int GetPinMuxModes(int index)
        {
            return GetValue(MainStatus.PinMuxModes, index * 2, 2);
        }
        /// <summary>
        /// Bulk status record Current Kogna ADC
        /// </summary>
        /// <param name="index">channel 0-3</param>
        /// <returns>value</returns>
        public int GetKognaADC(int index)
        {
            return Kogna_ADC[index];
        }
        /// <summary>
        /// Bulk status record Current Kogna DAC
        /// </summary>
        /// <param name="index">channel 0-7</param>
        /// <returns>value</returns>
        public int GetKognaDAC(int index)
        {
            return Kogna_DAC[index];
        }
        /// <summary>
        /// Bulk status record Current Kogna 8-bit PWM pulse setting
        /// </summary>
        /// <param name="index">channel 0-7</param>
        /// <returns>value</returns>
        public int GetKognaPWM(int index)
        {
            return Kogna_PWM[index];
        }
        /// <summary>
        /// Bulk status record Current Kogna 8-bit PWM Enables
        /// </summary>
        /// <param name="index">channel 0-7</param>
        /// <returns>value</returns>
        public int GetKognaPWMEnables(int index)
        {
            return Kogna_PWM_Enables[index];
        }
        /// <summary>
        /// Bulk status record Current Kogna Hi-Res 12-bit PWM pulse setting
        /// </summary>
        /// <param name="index">channel 0-3</param>
        /// <returns>value</returns>
        public int GetKognaHRPWM(int index)
        {
            return HRPWM[index];
        }
        /// <summary>
        /// Bulk status record Current SnapAmp #0 IO Bit directions (inputs vs outputs)
        /// </summary>
        /// <param name="index">IO Bit Number</param>
        /// <returns>value</returns>
        public int GetSnapBitsDirection0(int index)
        {
            return GetValue(MainStatus.SnapBitsDirection0, index, 1);
        }
        /// <summary>
        /// Bulk status record Current SnapAmp #1 IO Bit directions (inputs vs outputs)
        /// </summary>
        /// <param name="index">IO Bit Number</param>
        /// <returns>value 0=input 1=output</returns>
        public int GetSnapBitsDirection1(int index)
        {
            return GetValue(MainStatus.SnapBitsDirection1, index, 1);
        }
        /// <summary>
        /// Bulk status record Current SnapAmp #0 IO Bit States
        /// </summary>
        /// <param name="index">IO Bit Number</param>
        /// <returns>value</returns>
        public int GetSnapBitsState0(int index)
        {
            return GetValue(MainStatus.SnapBitsState0, index, 1);
        }
        /// <summary>
        /// Bulk status record Current SnapAmp #1 IO Bit States
        /// </summary>
        /// <param name="index">IO Bit Number</param>
        /// <returns>value</returns>
        public int GetSnapBitsState1(int index)
        {
            return GetValue(MainStatus.SnapBitsState1, index, 1);
        }
        /// <summary>
        /// Bulk status record Current Kanalog Input Bit States
        /// </summary>
        /// <param name="index">IO Bit Number</param>
        /// <returns>value</returns>
        public int GetKanalogBitsStateInputs(int index)
        {
            return GetValue(MainStatus.KanalogBitsStateInputs, index, 1);
        }
        /// <summary>
        /// Bulk status record Current Kanalog Output Bit States
        /// </summary>
        /// <param name="index">IO Bit Number</param>
        /// <returns>value</returns>
        public int GetKanalogBitsStateOutputs(int index)
        {
            return GetValue(MainStatus.KanalogBitsStateOutputs, index, 1);
        }
        /// <summary>
        /// Bulk status record Current Boot Thread Startup Enables 
        /// </summary>
        /// <param name="index">Thread Number</param>
        /// <returns>value</returns>
        public int GetRunOnStartUp(int index)
        {
            return GetValue(MainStatus.RunOnStartUp, index, 1);
        }
        /// <summary>
        /// Bulk status record Current Thread Execution State 
        /// </summary>
        /// <param name="index">Thread Number</param>
        /// <returns>value</returns>
        public int GetThreadActive(int index)
        {
            return GetValue(MainStatus.ThreadActive, index, 1);
        }
        /// <summary>
        /// Bulk status record Current PC-KFLOP Comm regs 
        /// </summary>
        /// <param name="index">Comm Register Index</param>
        /// <returns>value</returns>
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
        /// <param name="index">Bit Offset</param>
        /// <returns>value</returns>
        public int GetVirtualBits(int index)
        {
            return GetValue((int)MainStatus.VirtualBits, index, 1);
        }
        /// <summary>
        /// Bulk status record Current Extended Virtual Bit States 
        /// </summary>
        /// <param name="index">Bit Offset 0-31 where 0 returns Virtual bit 1024</param>
        /// <returns>value</returns>
        public int GetVirtualBitsEx0(int index)
        {
            return GetValue((int)MainStatus.VirtualBitsEx0, index, 1);
        }


        private int FourBitAxisIndex(int axischannel)
        {
            return (axischannel & 3) << 2;
        }
        private int GetValue(int source, int start, int length)
        {
            return ((source >> start) & length);
        }
    }
}







