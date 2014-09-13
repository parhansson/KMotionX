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



KM_Controller_Interop.cs 
*/
using System;
using System.Runtime.InteropServices; 
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace KMotion_dotNet
{
    #region Enums
    /// <summary>
    /// Represents the axis pair for executing arc segments
    /// 
    /// <remarks>
    /// This enum can be parsed to the NIST enums
    /// </remarks>
    /// </summary>
    public enum CANON_PLANE : int
    {
        /// <summary>
        /// Constrains an arc to be executed via X-Y interpolation
        /// </summary>
        CANON_PLANE_XY = 1,
        /// <summary>
        /// Constrains an arc to be executed via Y-Z interpolation
        /// </summary>
        CANON_PLANE_YZ = 2,
        /// <summary>
        /// Constrains an arc to be executed via X-Z interpolation
        /// </summary>
        CANON_PLANE_XZ = 3
    }

    /// <summary>
    /// Units used by the g code file
    /// 
    /// <remarks>
    /// The RS274 interpreter will translate these to inches relative to the CPI
    /// parameters in struct MotionParam
    /// </remarks>
    /// </summary>
    public enum CANON_UNITS : int
    {
        /// <summary>
        /// Inches 1:1
        /// </summary>
        CANON_UNITS_INCHES = 1,
        /// <summary>
        /// Milimeters 25.4:1
        /// </summary>
        CANON_UNITS_MM = 2,
        /// <summary>
        /// Centimeters 2.54:1
        /// </summary>
        CANON_UNITS_CM = 3
    }

    /// <summary>
    /// Motion profile for contouring
    /// </summary>
    public enum CANON_MOTION_MODE : int
    {
        /// <summary>
        /// Exact Stop
        /// <remarks>
        /// Will perform accel/slew/decel on each segment
        /// <para>
        /// Most accurate type of motion
        /// </para>
        /// </remarks>
        /// </summary>
        CANON_EXACT_STOP = 1,
        /// <summary>
        /// Exact Path
        /// <remarks>
        /// Creates an optimized path limited by vector accels and velocities
        /// </remarks>
        /// </summary>
        CANON_EXACT_PATH = 2,
        /// <summary>
        /// Continuous Contouring
        /// <remarks>
        /// Maintains a constant velocity over a series of path segments
        /// This is not an accurate machining profile, but useful for fast, smooth motion
        /// </remarks>
        /// </summary>
        CANON_CONTINUOUS = 3
    }

    /// <summary>
    /// Feed synchronization mode
    /// </summary>
    public enum CANON_SPEED_FEED_MODE : int
    {
        /// <summary>
        /// Synchronized
        /// <remarks>
        ///  slaves an axis with a spindle encoder
        /// </remarks>
        /// </summary>
        CANON_SYNCHED = 1,
        /// <summary>
        /// Independent Feed
        /// 
        /// <remarks>
        /// Motion is commanded without regard to external signals
        /// </remarks>
        /// </summary>
        CANON_INDEPENDENT = 2
    }

    /// <summary>
    /// Spindle Direction
    /// </summary>
    public enum CANON_DIRECTION : int
    {
        /// <summary>
        /// Stopped
        /// 
        /// <remarks>
        /// This is the state for the spindle when not spinning
        /// </remarks>
        /// </summary>
        CANON_STOPPED = 1,
        /// <summary>
        /// Clockwise
        /// 
        /// <remarks>
        /// This is the state for the spindle when spinning Clockwise
        /// <para>
        /// The rotation is relative from the tool's view
        /// </para>
        /// </remarks>
        /// </summary>
        CANON_CLOCKWISE = 2,
        /// <summary>
        /// CounterClockwise
        /// 
        /// <remarks>
        /// This is the state for the spindle when spinning CounterClockwise
        /// <para>
        /// The rotation is relative from the tool's view
        /// </para>
        /// </remarks>
        /// </summary>
        CANON_COUNTERCLOCKWISE = 3
    }

    /// <summary>
    /// Feed reference
    /// 
    /// <remarks>
    /// The meaning of feed rate changes depending on the feed reference mode
    /// </remarks>
    /// </summary>
    public enum CANON_FEED_REFERENCE : int
    {
        /// <summary>
        /// Workpiece Relative
        /// 
        /// <remarks>
        /// Feed rate is from the point of view of the workpiece
        /// </remarks>
        /// </summary>
        CANON_WORKPIECE = 1,
        /// <summary>
        /// XYZ Relative
        /// <remarks>
        /// Feedrate is in terms of axis motion
        /// <para>
        /// Some (perhaps most or all) existing dialects of RS274 use only the CANON_XYZ mode
        /// </para>
        /// </remarks>
        /// </summary>
        CANON_XYZ = 2
    }

    /// <summary>
    /// Canon Comp Side
    /// 
    /// <remarks>
    /// Defines the tool offset direction
    /// </remarks>
    /// </summary>
    public enum CANON_SIDE : int
    {
        /// <summary>
        /// Right Comp
        /// </summary>
        CANON_SIDE_RIGHT = 1,
        /// <summary>
        /// Left Comp
        /// </summary>
        CANON_SIDE_LEFT = 2,
        /// <summary>
        /// No Comp
        /// </summary>
        CANON_SIDE_OFF = 0
    }

    /// <summary>
    /// Canon Axis
    /// </summary>
    public enum CANON_AXIS : int
    {
        /// <summary>
        /// X Axis
        /// <remarks>
        /// Traverse Axis
        /// </remarks>
        /// </summary>
        CANON_AXIS_X = 1,
        /// <summary>
        /// Y Axis
        /// <remarks>
        /// Transverse Axis
        /// </remarks>
        /// </summary>
        CANON_AXIS_Y = 2,
        /// <summary>
        /// Z Axis
        /// <remarks>
        /// Depth Axis
        /// </remarks>
        /// </summary>
        CANON_AXIS_Z = 3,
        /// <summary>
        /// A Axis
        /// <remarks>
        /// Rotary Axis
        /// <para>
        /// Rotation is paralell to the X Axis
        /// </para>
        /// </remarks>
        /// </summary>
        CANON_AXIS_A = 4,
        /// <summary>
        /// B Axis
        /// <remarks>
        /// Rotary Axis
        /// <para>
        /// Rotation is paralell to the Y Axis
        /// </para>
        /// </remarks>
        /// </summary>
        CANON_AXIS_B = 5,
        /// <summary>
        /// C Axis
        /// <remarks>
        /// Rotary Axis
        /// <para>
        /// Rotation is paralell to the Z Axis
        /// </para>
        /// </remarks>
        /// </summary>
        CANON_AXIS_C = 6
    }

    /// <summary>
    /// Retract Mode
    /// 
    /// <remarks>
    /// Used for canned cycles
    /// </remarks>
    /// </summary>
    public enum RETRACT_MODE : int
    {
        /// <summary>
        /// R Plane
        /// 
        /// <remarks>
        /// Uses a supplied R value for retracting
        /// </remarks>
        /// </summary>
        R_PLANE = 0,
        /// <summary>
        /// Previous Z
        /// 
        /// <remarks>
        /// Uses the previous Z value for retracting
        /// </remarks>
        /// </summary>
        OLD_Z = 1
    }

    /// <summary>
    /// Distance Mode
    /// 
    /// <remarks>
    /// Defines how coordinates are interpreted
    /// </remarks>
    /// </summary>
    public enum DISTANCE_MODE : int
    {
        /// <summary>
        /// Absolute
        /// 
        /// <remarks>
        /// Coordinate values are absolute to the machine datum
        /// </remarks>
        /// </summary>
        MODE_ABSOLUTE = 0,
        /// <summary>
        /// Incremental
        /// 
        /// <remarks>
        /// Coordinate values are deltas from previous positions
        /// </remarks>
        /// </summary>
        MODE_INCREMENTAL = 1
    }
    /// <summary>
    /// IO Types
    /// 
    /// <remarks>
    /// Defines the direction of IO
    /// <para>
    /// Currently only digital inputs and digital outputs are supported
    /// </para>
    /// </remarks>
    /// </summary>
    public enum IO_TYPE : int
    {
        /// <summary>
        /// Undefined
        /// <remarks>
        /// Default value of uninitialized IO
        /// </remarks>
        /// </summary>
        UNDEFINED,
        /// <summary>
        /// Digital Input
        /// </summary>
        DIGITAL_IN,
        /// <summary>
        /// Digital Output
        /// </summary>
        DIGITAL_OUT,
        /// <summary>
        /// Analog Input
        /// </summary>
        ANALOG_IN,
        /// <summary>
        /// Analog Output
        /// </summary>
        ANALOG_OUT
    }

    /// <summary>
    /// KMotionLocked Return Codes
    /// </summary>
    public enum KMOTION_TOKEN : int// KMotionLocked Return Codes
    {
        /// <summary>
        /// (token is locked) if KMotion is available for use
        /// </summary>
        KMOTION_LOCKED = 0, 
        /// <summary>
        /// if already in use
        /// </summary>
        KMOTION_IN_USE = 1, 
        /// <summary>
        /// if no board connected
        /// </summary>
        KMOTION_NOT_CONNECTED = 2 
    }

    /// <summary>
    /// Possible M Code Action Types 
    /// </summary>
    public enum MCODE_TYPE : int
    {
        /// <summary>
        /// do nothing 
        /// </summary>
        M_Action_None = 0,				
        /// <summary>
        /// Set a bit high or low
        /// </summary>
        M_Action_Setbit = 1,			
        /// <summary>
        /// Set two bits either high or low
        /// </summary>
        M_Action_SetTwoBits = 2,		
        /// <summary>
        /// output a value to a DAC
        /// </summary>
        M_Action_DAC = 3,				
        /// <summary>
        /// run a KMotion User C program
        /// </summary>
        M_Action_Program = 4,			   
        /// <summary>
        /// run a KMotion User C program wait til finished
        /// </summary>
        M_Action_Program_wait = 5,		
        /// <summary>
        /// run a KMotion User C program wait til finished, resync positions
        /// </summary>
        M_Action_Program_wait_sync = 6,	
        /// <summary>
        /// run a Windows program wait til finished
        /// </summary>
        M_Action_Program_PC = 7,
        /// <summary>
        /// Callback to the User Application
        /// </summary>
        M_Action_Callback = 8,
        /// <summary>
        /// Wait/Hold until a bit is high or low
        /// </summary>
        M_Action_Waitbit = 9,
    }
    /// <summary>
    /// Possible Previously Stopping Types after an Interpreter Halt 
    /// </summary>
    public enum PREV_STOP_TYPE : int
    {
        /// <summary>
        /// Was not stopped or should attempt to resume 
        /// </summary>
        Prev_Stopped_None = 0,
        /// <summary>
        /// Was Halted while doing an indpendent motion (Rapid)
        /// </summary>
        Prev_Stopped_Indep = 1,
        /// <summary>
        /// Was Halted while doing a coordinated motion
        /// </summary>
        Prev_Stopped_Coord = 2,
        /// <summary>
        /// Was Halted while doing a coordinated motion - but completely finished the motion
        /// </summary>
        Prev_Stopped_Coord_Finished = 3,
    }
    
    
    #endregion

    public partial class KM_Controller : IDisposable
    {  
        #region KMotion_dotNet_Interop.dll Function References
        #region KMotionDLL References
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_New(ref IntPtr handle, int board);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_Free(ref IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_WriteLineReadLine(IntPtr handle, string command, ref string message);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_WriteLine(IntPtr handle, string command);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_WriteLineWithEcho(IntPtr handle, string command);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_ReadLineTimeOut(IntPtr handle, ref string message, int timeout_ms);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_WaitToken(IntPtr handle, int timeout);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_KMotionLock(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_KMotionLockRecovery(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_ReleaseToken(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_Failed(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_Disconnect(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CheckForReady(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_LoadCoff(IntPtr handle, int threadID, string name);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_LoadCoffPack(IntPtr handle, int threadID, string name, int PackToFlash);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_SimpleCompileAndLoadCoff(IntPtr handle, int threadID, string name);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CompileAndLoadCoff(IntPtr handle, int threadID, string name, ref string error, int _ErrorLength);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_Compile(IntPtr handle, int boardtype, int threadID, string name, ref string outfile, ref string error, int _ErrorLength);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_ServiceConsole(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_SetConsoleCallback(IntPtr handle, KMConsoleHandler handler);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_SetErrorCallback(IntPtr handle, KMErrorHandler handler);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_USBLocation(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_ListLocations(IntPtr handle, ref int nlocations, ref int list);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_GetFirmwareVersion(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CheckCoffSize(IntPtr handle, string name, ref int size_text, ref int size_bss, ref int size_data,ref int size_total);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_GetLoadAddress(IntPtr handle, int thread);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_ConvertToOut(IntPtr handle, int thread, string infile, string outfile, int maxlength);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CheckKMotionVersion(IntPtr handle, ref int type, bool getboardtypeonly);
        
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_ExtractCoffVersionString(IntPtr handle, string file, ref string version);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_MainStatus_GetStatus(IntPtr handle, bool locked,
        ref int versionandsize,
        [MarshalAs(UnmanagedType.LPArray, SizeConst = 24)] int[] adc,
        [MarshalAs(UnmanagedType.LPArray, SizeConst = 8)] int[] dac,
        [MarshalAs(UnmanagedType.LPArray, SizeConst = 16)]  int[] pwm,
        [MarshalAs(UnmanagedType.LPArray, SizeConst = 8)] double[] position,
        [MarshalAs(UnmanagedType.LPArray, SizeConst = 8)] double[] destination,
        [MarshalAs(UnmanagedType.LPArray, SizeConst = 8)] int[] outputchan0,
        ref  int inputmodes,
        ref  int inputmodes2,
        ref int outputmodes,
        ref  int outputmodes2,
        ref int enables,
        ref int axisdone,
        [MarshalAs(UnmanagedType.LPArray, SizeConst = 2)]  int[] bitsdirection,
        [MarshalAs(UnmanagedType.LPArray, SizeConst = 2)]  int[] bitsstate,
        ref  int snapbitsdirection0,
        ref  int snapbitsdirection1,
        ref  int snapbitsstate0,
        ref  int snapbitsstate1,
        ref  int kanalgobitsstateinputs,
        ref  int kanalogbitsstateoutputs,
        ref  int runonstartup,
        ref int threadactive,
        ref int stopimmediatestate,
        ref double timestamp,
        [MarshalAs(UnmanagedType.LPArray, SizeConst = 8)] int[] pccomm,
        ref int virtualbits,
        ref int virtualbitsex0);
        #endregion 
        #endregion
    }
}
