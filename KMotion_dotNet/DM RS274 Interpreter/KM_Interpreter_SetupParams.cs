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



KM_Interpreter_SetupParams.cs 
*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace KMotion_dotNet
{

    public partial class KM_Interpreter : IDisposable
    {
        /// <summary>
        /// Direct settings access for the EMC internal settings
        /// Warning, use at own risk
        /// Make sure to examine the Interpreter.cpp and related files to ensure
        /// you are aware of the possible repercussions when using these parameters
        /// </summary>
        public partial class KM_Interpreter_SetupParams
        {
            private IntPtr _InstanceHandle = new IntPtr(0L);
            /// <summary>
            /// Constructor for GCode interpreter internal setup parameters interface
            /// </summary>
            public KM_Interpreter_SetupParams(IntPtr handle)
            {
                _InstanceHandle = handle;
            }

            #region Properties
            /// <summary>
            /// A Axis Offset
            /// </summary>
            public double A_AxisOffset
            {
                get
                {
                    return Get_A_AxisOffset();
                }
                set
                {
                    Set_A_AxisOffset(value);
                }
            }
            /// <summary>
            /// Current A Axis Positon
            /// </summary>
            public double A_AxisPosition
            {
                get
                {
                    return Get_A_AxisPosition();
                }
                set
                {
                    Set_A_AxisPosition(value);
                }
            }
            /// <summary>
            /// A axis Origin Offset
            /// </summary>
            public double A_OriginOffset
            {
                get
                {
                    return Get_A_OriginOffset();
                }
                set
                {
                    Set_A_OriginOffset(value);
                }
            }
            /// <summary>
            /// B Axis Offset
            /// </summary>
            public double B_AxisOffset
            {
                get
                {
                    return Get_B_AxisOffset();
                }
                set
                {
                    Set_B_AxisOffset(value);
                }
            }
            /// <summary>
            /// Current B Axis Positon
            /// </summary>
            public double B_AxisPosition
            {
                get
                {
                    return Get_B_AxisPosition();
                }
                set
                {
                    Set_B_AxisPosition(value);
                }
            }
            /// <summary>
            /// B axis Origin Offset
            /// </summary>
            public double B_OriginOffset
            {
                get
                {
                    return Get_B_OriginOffset();
                }
                set
                {
                    Set_B_OriginOffset(value);
                }
            }
            /// <summary>
            /// C Axis Offset
            /// </summary>
            public double C_AxisOffset
            {
                get
                {
                    return Get_C_AxisOffset();
                }
                set
                {
                    Set_C_AxisOffset(value);
                }
            }
            /// <summary>
            /// Current C Axis Positon
            /// </summary>
            public double C_AxisPosition
            {
                get
                {
                    return Get_C_AxisPosition();
                }
                set
                {
                    Set_C_AxisPosition(value);
                }
            }
            /// <summary>
            /// C axis Origin Offset
            /// </summary>
            public double C_OriginOffset
            {
                get
                {
                    return Get_C_OriginOffset();
                }
                set
                {
                    Set_C_OriginOffset(value);
                }
            }
            /// <summary>
            /// X Axis Offset
            /// </summary>
            public double X_AxisOffset
            {
                get
                {
                    return Get_X_AxisOffset();
                }
                set
                {
                    Set_X_AxisOffset(value);
                }
            }
            /// <summary>
            /// Current X Axis Positon
            /// </summary>
            public double X_AxisPosition
            {
                get
                {
                    return Get_X_AxisPosition();
                }
                set
                {
                    Set_X_AxisPosition(value);
                }
            }
            /// <summary>
            /// X axis Origin Offset
            /// </summary>
            public double X_OriginOffset
            {
                get
                {
                    return Get_X_OriginOffset();
                }
                set
                {
                    Set_X_OriginOffset(value);
                }
            }
            /// <summary>
            /// Y Axis Offset
            /// </summary>
            public double Y_AxisOffset
            {
                get
                {
                    return Get_Y_AxisOffset();
                }
                set
                {
                    Set_Y_AxisOffset(value);
                }
            }
            /// <summary>
            /// Current Y Axis Positon
            /// </summary>
            public double Y_AxisPosition
            {
                get
                {
                    return Get_Y_AxisPosition();
                }
                set
                {
                    Set_Y_AxisPosition(value);
                }
            }
            /// <summary>
            /// Y axis Origin Offset
            /// </summary>
            public double Y_OriginOffset
            {
                get
                {
                    return Get_Y_OriginOffset();
                }
                set
                {
                    Set_Y_OriginOffset(value);
                }
            }
            /// <summary>
            /// Z Axis Offset
            /// </summary>
            public double Z_AxisOffset
            {
                get
                {
                    return Get_Z_AxisOffset();
                }
                set
                {
                    Set_Z_AxisOffset(value);
                }
            }
            /// <summary>
            /// Current Z Axis Positon
            /// </summary>
            public double Z_AxisPosition
            {
                get
                {
                    return Get_Z_AxisPosition();
                }
                set
                {
                    Set_Z_AxisPosition(value);
                }
            }
            /// <summary>
            /// Z axis Origin Offset
            /// </summary>
            public double Z_OriginOffset
            {
                get
                {
                    return Get_Z_OriginOffset();
                }
                set
                {
                    Set_Z_OriginOffset(value);
                }
            }
            /// <summary>
            /// Cannon Control Mode 
            /// CANON_EXACT_STOP = 1,
            /// CANON_EXACT_PATH = 2,
            /// CANON_CONTINUOUS = 3
            /// </summary>
            public CANON_MOTION_MODE ControlMode
            {
                get
                {
                    return (CANON_MOTION_MODE)Get_ControlMode();
                }
                set
                {
                    Set_ControlMode((int)value);
                }
            }
            /// <summary>
            /// Canon Length Units
            /// CANON_UNITS_INCHES = 1, 
            /// CANON_UNITS_MM = 2,
            /// CANON_UNITS_CM = 3
            /// </summary>
            public CANON_UNITS LengthUnits
            {
                get
                {
                    return (CANON_UNITS)Get_LengthUnits();
                }
                set
                {
                    Set_LengthUnits((int)value);
                }
            }
            /// <summary>
            /// Origin Index
            /// G54, 55, etc...
            /// </summary>
            public int OriginIndex
            {
                get
                {
                    return Get_OriginIndex();
                }
                set
                {
                    Set_OriginIndex(value);
                }
            }
            /// <summary>
            /// Canon Work Plane
            /// CANON_PLANE_XY = 1,
            /// CANON_PLANE_YZ = 2,
            /// CANON_PLANE_XZ = 3
            /// </summary>
            public CANON_PLANE WorkPlane
            {
                get
                {
                    return (CANON_PLANE)Get_WorkPlane();
                }
                set
                {
                    Set_WorkPlane((int)value);
                }
            }
            /// <summary>
            /// Flags true if the file had a percentage sign
            /// </summary>
            public bool PercentFlag
            {
                get
                {
                    return Get_PercentFlag() > 0;
                }
            }
            /// <summary>
            /// Gets and sets whether a probe operation is complete
            /// </summary>
            public bool ProbeComplete
            {
                get
                {
                    return Get_ProbeComplete() > 0;
                }
                set
                {
                    Set_ProbeComplete((value == false) ? 0 : 1);
                }
            }
            /// <summary>
            /// Program's X position
            /// </summary>
            public double ProgramX
            {
                get
                {
                    return Get_ProgramX();
                }
                set
                {
                    Set_ProgramX(value);
                }
            }
            /// <summary>
            /// Program's Y position
            /// </summary>
            public double ProgramY
            {
                get
                {
                    return Get_ProgramY();
                }
                set
                {
                    Set_ProgramY(value);
                }
            }
            /// <summary>
            /// Current sequence of the file being interpreted
            /// </summary>
            public int SequenceNumber
            {
                get
                {
                    return Get_SequenceNumber();
                }
            }
            /// <summary>
            ///Current line number of the file being interpreted
            /// </summary>
            public int CurrentLine
            {
                get
                {
                    return Get_CurrentLine();
                }
                set
                {
                    Set_CurrentLine(value);
                }
            }
            /// <summary>
            /// Spped to use during G00 traverse operations
            /// </summary>
            public double TraverseSpeed
            {
                get
                {
                    return Get_TraverseSpeed();
                }
                set
                {
                    Set_TraverseSpeed(value);
                }
            }
            /// <summary>
            /// Character length of the current line
            /// </summary>
            public int LineLength
            {
                get
                {
                    return Get_LineLength();
                }
            }
            /// <summary>
            /// Current File name being interpreted
            /// </summary>
            public string FileName
            {
                get
                {
                    return Get_FileName();
                }
            }
            /// <summary>
            /// Text of the currently interpreted line
            /// </summary>
            public string LineText
            {
                get
                {
                    return Get_LineText();
                }
            }
            /// <summary>
            /// Current G code for motion
            /// </summary>
            public int CurrentMotionMode
            {
                get
                {
                    return Get_CurrentMotionMode();
                }
                set
                {
                    Set_CurrentMotionMode(value);
                }
            }
            /// <summary>
            /// All currently active G codes
            /// </summary>
            public List<string> Active_GCodes
            {
                get
                {
                    return Get_Active_GCodes();
                }
            }
            /// <summary>
            /// All currently active M codes
            /// </summary>
            public List<string> Active_MCodes
            {
                get
                {
                    return Get_Active_MCodes();
                }
            }
            /// <summary>
            /// Need clarification
            /// </summary>
            public double[] Active_Settings
            {
                get
                {
                    return Get_Active_Settings();
                }
            }
            /// <summary>
            /// Unit distance mode
            /// MODE_ABSOLUTE = 0,
            /// MODE_INCREMENTAL = 1
            /// </summary>
            public DISTANCE_MODE DistanceMode
            {
                get
                {
                    return (DISTANCE_MODE)Get_DistanceMode();
                }
                set
                {
                    Set_DistanceMode((int)value);
                }
            }
            /// <summary>
            /// G_93 (inverse time) or G_94 units/min
            /// </summary>
            public int FeedMode
            {
                get
                {
                    return Get_FeedMode();
                }
                set
                {
                    Set_FeedMode(value);
                }
            }

            /// <summary>
            /// Flood Coolant State from Interpreter
            /// </summary>
            public bool FloodCoolantOn
            {
                get
                {
                    return Get_FloodCoolantOn() > 0;
                }
                set
                {
                    Set_FloodCoolantOn((value == false) ? 0 : 1);
                }
            }
            /// <summary> 
            /// Gets and sets whether Mist Coolant is on
            /// </summary>
            public bool MistCoolantOn
            {
                get
                {
                    return Get_MistCoolantOn() > 0;
                }
                set
                {
                    Set_MistCoolantOn((value == false) ? 0 : 1);
                }
            }
            /// <summary>
            /// Enable/Disable Block Delete
            /// </summary>
            public bool BlockDelete
            {
                get
                {
                    return Get_BlockDelete() > 0;
                }
                set
                {
                    Set_BlockDelete((value == false) ? 0 : 1);
                }
            }
            /// <summary>
            /// Tool Changer slot number of current tool
            /// </summary>
            public int CurrentToolSlot
            {
                get
                {
                    return Get_CurrentToolSlot();
                }
                set
                {
                    Set_CurrentToolSlot(value);
                }
            }
            /// <summary>
            ///  Tool slot selected but not active
            /// </summary>
            public int SelectedToolSlot
            {
                get
                {
                    return Get_SelectedToolSlot();
                }
                set
                {
                    Set_SelectedToolSlot(value);
                }
            }
            /// <summary>
            /// Current tool length offset
            /// </summary>
            public double ToolLengthOffset
            {
                get
                {
                    return Get_ToolLengthOffset();
                }
                set
                {
                    Set_ToolLengthOffset(value);
                }
            }
            /// <summary>
            /// Row in tool table for current Length Offset
            /// </summary>
            public int ToolLengthOffsetIndex
            {
                get
                {
                    return Get_ToolLengthOffsetIndex();
                }
                set
                {
                    Set_ToolLengthOffsetIndex(value);
                }
            }
            /// <summary>
            /// Radius compensation mode
            /// </summary>
            public int RadiusComp
            {
                get
                {
                    return Get_RadiusComp();
                }
                set
                {
                    Set_RadiusComp(value);
                }
            }
            /// <summary>
            /// Radius compensation value
            /// </summary>
            public double RadiusCompValue
            {
                get
                {
                    return Get_RadiusCompValue();
                }
                set
                {
                    Set_RadiusCompValue(value);
                }
            }
            /// <summary>
            /// Radius compensation side
            /// CANON_SIDE_RIGHT = 1,
            /// CANON_SIDE_LEFT = 2,
            /// CANON_SIDE_OFF = 3
            /// </summary>
            public CANON_SIDE RadiusCompSide
            {
                get
                {
                    return (CANON_SIDE)Get_RadiusCompSide();
                }
                set
                {
                    Set_RadiusCompSide((int)value);
                }
            }
            /// <summary>
            /// Gets/sets whether to allow spindle feed override
            /// </summary>
            public bool AllowSpindleSpeedOverride
            {
                get
                {
                    return Get_AllowSpindleSpeedOverride() > 0;
                }
                set
                {
                    Set_AllowSpindleSpeedOverride((value == false) ? 0 : 1);
                }
            }
            /// <summary>
            /// Current Speed override
            /// </summary>
            public double SpindleSpeed
            {
                get
                {
                    return Get_SpindleSpeed();
                }
                set
                {
                    Set_SpindleSpeed(value);
                }
            }
            /// <summary>
            /// Spindle rotation direction
            /// CANON_STOPPED = 1,
            /// CANON_CLOCKWISE = 2,
            /// CANON_COUNTERCLOCKWISE = 3
            /// </summary>
            public CANON_DIRECTION SpindleDirection
            {
                get
                {
                    return (CANON_DIRECTION)Get_SpindleDirection();
                }
                set
                {
                    Set_SpindleDirection((int)value);
                }
            }
            /// <summary>
            /// Gets/sets canned Cycle C value
            /// </summary>
            public double Cycle_CC
            {
                get
                {
                    return Get_Cycle_CC();
                }
                set
                {
                    Set_Cycle_CC(value);
                }
            }
            /// <summary>
            /// Gets/sets canned Cycle I value
            /// </summary>
            public double Cycle_I
            {
                get
                {
                    return Get_Cycle_I();
                }
                set
                {
                    Set_Cycle_I(value);
                }
            }
            /// <summary>
            /// Gets/sets canned Cycle J value
            /// </summary>
            public double Cycle_J
            {
                get
                {
                    return Get_Cycle_J();
                }
                set
                {
                    Set_Cycle_J(value);
                }
            }
            /// <summary>
            /// Gets/sets canned Cycle K value
            /// </summary>
            public double Cycle_K
            {
                get
                {
                    return Get_Cycle_K();
                }
                set
                {
                    Set_Cycle_K(value);
                }
            }
            /// <summary>
            /// Gets/sets canned Cycle L value
            /// </summary>
            public double Cycle_L
            {
                get
                {
                    return Get_Cycle_L();
                }
                set
                {
                    Set_Cycle_L(value);
                }
            }
            /// <summary>
            /// Gets/sets canned Cycle P value
            /// </summary>
            public double Cycle_P
            {
                get
                {
                    return Get_Cycle_P();
                }
                set
                {
                    Set_Cycle_P(value);
                }
            }
            /// <summary>
            /// Gets/sets canned Cycle Q value
            /// </summary>
            public double Cycle_Q
            {
                get
                {
                    return Get_Cycle_Q();
                }
                set
                {
                    Set_Cycle_Q(value);
                }
            }
            /// <summary>
            /// Gets/sets canned Cycle R value
            /// </summary>
            public double Cycle_R
            {
                get
                {
                    return Get_Cycle_R();
                }
                set
                {
                    Set_Cycle_R(value);
                }
            }
            /// <summary>
            /// Gets/sets canned Cycle Z value
            /// </summary>
            public double Cycle_Z
            {
                get
                {
                    return Get_Cycle_Z();
                }
                set
                {
                    Set_Cycle_Z(value);
                }
            }
            /// <summary>
            /// Gets/sets canned Retract Mode
            /// R_PLANE = 0,
            /// OLD_Z = 1
            /// </summary>
            public RETRACT_MODE Cycle_RetractMode
            {
                get
                {
                    return (RETRACT_MODE)Get_Cycle_RetractMode();
                }
                set
                {
                    Set_Cycle_RetractMode((int)value);
                }
            }
            #endregion

            #region Property Dispatchers
            private double Get_A_AxisOffset()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_A_Axis_Offset(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_A_AxisOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_A_AxisOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_A_AxisOffset"));
                }
            }
            private void Set_A_AxisOffset(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_A_Axis_Offset(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_A_AxisOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_A_AxisOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_A_AxisOffset"));
                }
            }

            private double Get_A_AxisPosition()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_A_Current_Position(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_A_AxisPosition"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_A_AxisPosition"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_A_AxisPosition"));
                }
            }
            private void Set_A_AxisPosition(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_A_Current_Position(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_A_AxisPosition"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_A_AxisPosition"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_A_AxisPosition"));
                }
            }

            private double Get_A_OriginOffset()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_A_Origin_Offset(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_A_OriginOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_A_OriginOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_A_OriginOffset"));
                }
            }
            private void Set_A_OriginOffset(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_A_Origin_Offset(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_A_OriginOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_A_OriginOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_A_OriginOffset"));
                }
            }


            private double Get_B_AxisOffset()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_B_Axis_Offset(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_B_AxisOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_B_AxisOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_B_AxisOffset"));
                }
            }
            private void Set_B_AxisOffset(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_B_Axis_Offset(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_B_AxisOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_B_AxisOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_B_AxisOffset"));
                }
            }

            private double Get_B_AxisPosition()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_B_Current_Position(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_B_AxisPosition"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_B_AxisPosition"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_B_AxisPosition"));
                }
            }
            private void Set_B_AxisPosition(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_B_Current_Position(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_B_AxisPosition"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_B_AxisPosition"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_B_AxisPosition"));
                }
            }

            private double Get_B_OriginOffset()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_B_Origin_Offset(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_B_OriginOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_B_OriginOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_B_OriginOffset"));
                }
            }
            private void Set_B_OriginOffset(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_B_Origin_Offset(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_B_OriginOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_B_OriginOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_B_OriginOffset"));
                }
            }


            private double Get_C_AxisOffset()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_C_Axis_Offset(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_C_AxisOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_C_AxisOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_C_AxisOffset"));
                }
            }
            private void Set_C_AxisOffset(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_C_Axis_Offset(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_C_AxisOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_C_AxisOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_C_AxisOffset"));
                }
            }

            private double Get_C_AxisPosition()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_C_Current_Position(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_C_AxisPosition"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_C_AxisPosition"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_C_AxisPosition"));
                }
            }
            private void Set_C_AxisPosition(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_C_Current_Position(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_C_AxisPosition"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_C_AxisPosition"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_C_AxisPosition"));
                }
            }

            private double Get_C_OriginOffset()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_C_Origin_Offset(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_C_OriginOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_C_OriginOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_C_OriginOffset"));
                }
            }
            private void Set_C_OriginOffset(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_C_Origin_Offset(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_C_OriginOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_C_OriginOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_C_OriginOffset"));
                }
            }


            private double Get_X_AxisOffset()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_X_Axis_Offset(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_X_AxisOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_X_AxisOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_X_AxisOffset"));
                }
            }
            private void Set_X_AxisOffset(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_X_Axis_Offset(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_X_AxisOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_X_AxisOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_X_AxisOffset"));
                }
            }

            private double Get_X_AxisPosition()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_X_Current_Position(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_X_AxisPosition"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_X_AxisPosition"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_X_AxisPosition"));
                }
            }
            private void Set_X_AxisPosition(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_X_Current_Position(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_X_AxisPosition"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_X_AxisPosition"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_X_AxisPosition"));
                }
            }

            private double Get_X_OriginOffset()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_X_Origin_Offset(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_X_OriginOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_X_OriginOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_X_OriginOffset"));
                }
            }
            private void Set_X_OriginOffset(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_X_Origin_Offset(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_X_OriginOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_X_OriginOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_X_OriginOffset"));
                }
            }


            private double Get_Y_AxisOffset()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_Y_Axis_Offset(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_Y_AxisOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_Y_AxisOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_Y_AxisOffset"));
                }
            }
            private void Set_Y_AxisOffset(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_Y_Axis_Offset(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_Y_AxisOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_Y_AxisOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_Y_AxisOffset"));
                }
            }

            private double Get_Y_AxisPosition()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_Y_Current_Position(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_Y_AxisPosition"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_Y_AxisPosition"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_Y_AxisPosition"));
                }
            }
            private void Set_Y_AxisPosition(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_Y_Current_Position(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_Y_AxisPosition"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_Y_AxisPosition"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_Y_AxisPosition"));
                }
            }

            private double Get_Y_OriginOffset()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_Y_Origin_Offset(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_Y_OriginOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_Y_OriginOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_Y_OriginOffset"));
                }
            }
            private void Set_Y_OriginOffset(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_Y_Origin_Offset(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_Y_OriginOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_Y_OriginOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_Y_OriginOffset"));
                }
            }


            private double Get_Z_AxisOffset()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_Z_Axis_Offset(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_Z_AxisOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_Z_AxisOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_Z_AxisOffset"));
                }
            }
            private void Set_Z_AxisOffset(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_Z_Axis_Offset(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_Z_AxisOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_Z_AxisOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_Z_AxisOffset"));
                }
            }

            private double Get_Z_AxisPosition()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_Z_Current_Position(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_Z_AxisPosition"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_Z_AxisPosition"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_Z_AxisPosition"));
                }
            }
            private void Set_Z_AxisPosition(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_Z_Current_Position(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_Z_AxisPosition"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_Z_AxisPosition"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_Z_AxisPosition"));
                }
            }

            private double Get_Z_OriginOffset()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_Z_Origin_Offset(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_Z_OriginOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_Z_OriginOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_Z_OriginOffset"));
                }
            }
            private void Set_Z_OriginOffset(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_Z_Origin_Offset(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_Z_OriginOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_Z_OriginOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_Z_OriginOffset"));
                }
            }



            private int Get_ControlMode()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_ControlMode(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_ControlMode"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_ControlMode"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_ControlMode"));
                }
            }
            private void Set_ControlMode(int value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_ControlMode(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_ControlMode"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_ControlMode"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_ControlMode"));
                }
            }

            private int Get_LengthUnits()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_LengthUnits(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_LengthUnits"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_LengthUnits"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_LengthUnits"));
                }
            }
            private void Set_LengthUnits(int value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_LengthUnits(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_LengthUnits"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_LengthUnits"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_LengthUnits"));
                }
            }

            private int Get_OriginIndex()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_OriginIndex(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_OriginIndex"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_OriginIndex"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_OriginIndex"));
                }
            }
            private void Set_OriginIndex(int value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_OriginIndex(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_OriginIndex"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_OriginIndex"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_OriginIndex"));
                }
            }

            private int Get_WorkPlane()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_WorkPlane(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_WorkPlane"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_WorkPlane"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_WorkPlane"));
                }
            }
            private void Set_WorkPlane(int value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_WorkPlane(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_WorkPlane"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_WorkPlane"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_WorkPlane"));
                }
            }

            private int Get_PercentFlag()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_PercentFlag(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_PercentFlag"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_PercentFlag"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_PercentFlag"));
                }
            }

            private int Get_ProbeComplete()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_ProbeComplete(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_ProbeComplete"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_ProbeComplete"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_ProbeComplete"));
                }
            }
            private void Set_ProbeComplete(int value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_ProbeComplete(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_ProbeComplete"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_ProbeComplete"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_ProbeComplete"));
                }
            }

            private double Get_ProgramX()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_ProgramX(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_ProgramX"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_ProgramX"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_ProgramX"));
                }
            }
            private void Set_ProgramX(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_ProgramX(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_ProgramX"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_ProgramX"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_ProgramX"));
                }
            }

            private double Get_ProgramY()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_ProgramY(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_ProgramY"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_ProgramY"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_ProgramY"));
                }
            }
            private void Set_ProgramY(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_ProgramY(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_ProgramY"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_ProgramY"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_ProgramY"));
                }
            }

            private int Get_SequenceNumber()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_SequenceNumber(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_SequenceNumber"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_SequenceNumber"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_SequenceNumber"));
                }
            }

            private int Get_CurrentLine()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_CurrentLine(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_CurrentLine"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_CurrentLine"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_CurrentLine"));
                }
            }
            private void Set_CurrentLine(int value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_CurrentLine(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_CurrentLine"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_CurrentLine"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_CurrentLine"));
                }
            }

            private double Get_TraverseSpeed()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_TraverseSpeed(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_TraversSpeed"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_TraversSpeed"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_TraversSpeed"));
                }
            }
            private void Set_TraverseSpeed(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_TraverseSpeed(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_TraverseSpeed"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_TraverseSpeed"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_TraverseSpeed"));
                }
            }

            private int Get_LineLength()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_LineLength(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_LineLength"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_LineLength"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_LineLength"));
                }
            }

            private string Get_FileName()
            {
                try
                {
                    string value = "";
                    KM_dotnet_Interop_GCodeInterpreter_Get_FileName(_InstanceHandle, ref value);
                    return value;
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_FileName"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_FileName"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_FileName"));
                }
            }

            private string Get_LineText()
            {
                try
                {
                    string value = "";
                    KM_dotnet_Interop_GCodeInterpreter_Get_LineText(_InstanceHandle, ref value);
                    return value;
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_LineText"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_LineText"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_LineText"));
                }
            }



            private int Get_CurrentMotionMode()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_CurrentMotionMode(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_CurrentMotionMode"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_CurrentMotionMode"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_CurrentMotionMode"));
                }
            }
            private void Set_CurrentMotionMode(int value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_CurrentMotionMode(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_CurrentMotionMode"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_CurrentMotionMode"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_CurrentMotionMode"));
                }
            }

            private List<string> Get_Active_GCodes()
            {
                try
                {
                    List<string> retval = new List<string>();
                    for (int i = 0; i < 12; i++)
                    {
                        var code = KM_dotnet_Interop_GCodeInterpreter_Get_Active_GCode(_InstanceHandle, i);
                        string value = "";
                        switch (code)
                        {
                            case 0:
                                value = "G0";
                                break;
                            case 10:
                                value = "G1";
                                break;
                            case 20:
                                value = "G2";
                                break;
                            case 30:
                                value = "G3";
                                break;
                            case 40:
                                value = "G4";
                                break;
                            case 100:
                                value = "G10";
                                break;
                            case 170:
                                value = "G17";
                                break;
                            case 180:
                                value = "G18";
                                break;
                            case 190:
                                value = "G19";
                                break;
                            case 200:
                                value = "G20";
                                break;
                            case 210:
                                value = "G21";
                                break;
                            case 280:
                                value = "G28";
                                break;
                            case 300:
                                value = "G30";
                                break;
                            case 330:
                                value = "G33";
                                break;
                            case 382:
                                value = "G38.2";
                                break;
                            case 400:
                                value = "G40";
                                break;
                            case 410:
                                value = "G41";
                                break;
                            case 420:
                                value = "G42";
                                break;
                            case 430:
                                value = "G43";
                                break;
                            case 490:
                                value = "G49";
                                break;
                            case 530:
                                value = "G53";
                                break;
                            case 540:
                                value = "G54";
                                break;
                            case 550:
                                value = "G55";
                                break;
                            case 560:
                                value = "G56";
                                break;
                            case 570:
                                value = "G57";
                                break;
                            case 580:
                                value = "G58";
                                break;
                            case 590:
                                value = "G59";
                                break;
                            case 591:
                                value = "G59.1";
                                break;
                            case 592:
                                value = "G59.2";
                                break;
                            case 593:
                                value = "G59.3";
                                break;
                            case 610:
                                value = "G61";
                                break;
                            case 611:
                                value = "G61.1";
                                break;
                            case 640:
                                value = "G64";
                                break;
                            case 800:
                                value = "G80";
                                break;
                            case 810:
                                value = "G81";
                                break;
                            case 820:
                                value = "G82";
                                break;
                            case 830:
                                value = "G83";
                                break;
                            case 840:
                                value = "G84";
                                break;
                            case 850:
                                value = "G85";
                                break;
                            case 860:
                                value = "G86";
                                break;
                            case 870:
                                value = "G87";
                                break;
                            case 880:
                                value = "G88";
                                break;
                            case 890:
                                value = "G89";
                                break;
                            case 900:
                                value = "G90";
                                break;
                            case 910:
                                value = "G91";
                                break;
                            case 920:
                                value = "G92";
                                break;
                            case 921:
                                value = "G92.1";
                                break;
                            case 922:
                                value = "G92.2";
                                break;
                            case 923:
                                value = "G92.3";
                                break;
                            case 930:
                                value = "G93";
                                break;
                            case 940:
                                value = "G94";
                                break;
                            case 980:
                                value = "G98";
                                break;
                            case 990:
                                value = "G90";
                                break;
                            default:
                                value = "";
                                break;
	
	                    }
                        if (value != "")
                            retval.Add(value);
                    }
                    return retval;
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_Active_GCodes"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_Active_GCodes"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_Active_GCodes"));
                }
            }
            private List<string> Get_Active_MCodes()
            {
                try
                {
                    List<string> retval = new List<string>();
                    for (int i = 0; i < 7; i++)
                    {
                        if (i > 0) //Skip first, as it is the line #
                        {
                            var code = KM_dotnet_Interop_GCodeInterpreter_Get_Active_MCode(_InstanceHandle, i);
                            if (code >= 0)
                            {
                                var codedata = code.ToString();
                                if (codedata != "")
                                {
                                    retval.Add("M" + codedata);
                                }
                            }
                        }
                    }
                    return retval;
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_Active_MCodes"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_Active_MCodes"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_Active_MCodes"));
                }
            }
            private double[] Get_Active_Settings()
            {
                try
                {
                    return new double[3];
                    //return KM_dotnet_Interop_GCodeInterpreter_Get_Active_Setting(_InstanceHandle, 1);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_Active_Settings"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_Active_Settings"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_Active_Settings"));
                }
            }

            private int Get_DistanceMode()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_DistanceMode(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_DistanceMode"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_DistanceMode"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_DistanceMode"));
                }
            }
            private void Set_DistanceMode(int value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_DistanceMode(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_DistanceMode"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_DistanceMode"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_DistanceMode"));
                }
            }

            private int Get_FeedMode()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_FeedMode(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_FeedMode"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_FeedMode"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_FeedMode"));
                }
            }
            private void Set_FeedMode(int value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_FeedMode(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_FeedMode"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_FeedMode"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_FeedMode"));
                }
            }
 
            private int Get_FloodCoolantOn()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_FloodCoolantOn(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_FloodCoolantOn"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_FloodCoolantOn"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_FloodCoolantOn"));
                }
            }
            private void Set_FloodCoolantOn(int value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_FloodCoolantOn(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_FloodCoolantOn"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_FloodCoolantOn"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_FloodCoolantOn"));
                }
            }

            private int Get_MistCoolantOn()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_MistCoolantOn(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_MistCoolantOn"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_MistCoolantOn"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_MistCoolantOn"));
                }
            }
            private void Set_MistCoolantOn(int value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_MistCoolantOn(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_MistCoolantOn"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_MistCoolantOn"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_MistCoolantOn"));
                }
            }

            private int Get_BlockDelete()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_BlockDelete(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_BlockDelete"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_BlockDelete"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_BlockDelete"));
                }
            }
            private void Set_BlockDelete(int value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_BlockDelete(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_BlockDelete"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_BlockDelete"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_BlockDelete"));
                }
            }


            //Tooling Params  

            private int Get_CurrentToolSlot()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_CurrentToolSlot(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_CurrentToolSlot"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_CurrentToolSlot"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_CurrentToolSlot"));
                }
            }
            private void Set_CurrentToolSlot(int value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_CurrentToolSlot(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_CurrentToolSlot"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_CurrentToolSlot"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_CurrentToolSlot"));
                }
            }

            private int Get_SelectedToolSlot()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_SelectedToolSlot(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_SelectedToolSlot"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_SelectedToolSlot"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_SelectedToolSlot"));
                }
            }
            private void Set_SelectedToolSlot(int value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_SelectedToolSlot(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_SelectedToolSlot"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_SelectedToolSlot"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_SelectedToolSlot"));
                }
            }

            private double Get_ToolLengthOffset()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_ToolLengthOffset(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_ToolLengthOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_ToolLengthOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_ToolLengthOffset"));
                }
            }
            private void Set_ToolLengthOffset(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_ToolLengthOffset(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_ToolLengthOffset"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_ToolLengthOffset"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_ToolLengthOffset"));
                }
            }

            private int Get_ToolLengthOffsetIndex()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_ToolLengthOffsetIndex(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "ToolLengthOffsetIndex"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "ToolLengthOffsetIndex"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "ToolLengthOffsetIndex"));
                }
            }
            private void Set_ToolLengthOffsetIndex(int value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_ToolLengthOffsetIndex(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_ToolLengthOffsetIndex"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_ToolLengthOffsetIndex"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_ToolLengthOffsetIndex"));
                }
            }

            private int Get_RadiusComp()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_RadiusComp(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_RadiusComp"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_RadiusComp"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_RadiusComp"));
                }
            }
            private void Set_RadiusComp(int value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_RadiusComp(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_RadiusComp"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_RadiusComp"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_RadiusComp"));
                }
            }

            private double Get_RadiusCompValue()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_RadiusCompValue(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_RadiusCompValue"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_RadiusCompValue"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_RadiusCompValue"));
                }
            }
            private void Set_RadiusCompValue(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_RadiusCompValue(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_RadiusCompValue"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_RadiusCompValue"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_RadiusCompValue"));
                }
            }

            private int Get_RadiusCompSide()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_RadiusCompSide(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_RadiusCompSide"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_RadiusCompSide"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_RadiusCompSide"));
                }
            }
            private void Set_RadiusCompSide(int value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_RadiusCompSide(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_RadiusCompSide"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_RadiusCompSide"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_RadiusCompSide"));
                }
            }

            private int Get_AllowSpindleSpeedOverride()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_AllowSpindleSpeedOverride(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_AllowSpindleSpeedOverride"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_AllowSpindleSpeedOverride"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_AllowSpindleSpeedOverride"));
                }
            }
            private void Set_AllowSpindleSpeedOverride(int value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_AllowSpindleSpeedOverride(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_AllowSpindleSpeedOverride"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_AllowSpindleSpeedOverride"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_AllowSpindleSpeedOverride"));
                }
            }

            private double Get_SpindleSpeed()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_SpindleSpeed(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_SpindleSpeed"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_SpindleSpeed"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_SpindleSpeed"));
                }
            }
            private void Set_SpindleSpeed(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_SpindleSpeed(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_SpindleSpeed"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_SpindleSpeed"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_SpindleSpeed"));
                }
            }

            private int Get_SpindleDirection()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_SpindleDirection(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_SpindleDirection"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_SpindleDirection"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_SpindleDirection"));
                }
            }
            private void Set_SpindleDirection(int value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_SpindleDirection(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_SpindleDirection"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_SpindleDirection"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_SpindleDirection"));
                }
            }


            private double Get_Cycle_CC()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_CC(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_Cycle_CC"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_Cycle_CC"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_Cycle_CC"));
                }
            }
            private void Set_Cycle_CC(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_CC(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_Cycle_CC"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_Cycle_CC"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_Cycle_CC"));
                }
            }

            private double Get_Cycle_I()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_I(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_Cycle_I"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_Cycle_I"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_Cycle_I"));
                }
            }
            private void Set_Cycle_I(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_I(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_Cycle_I"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_Cycle_I"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_Cycle_I"));
                }
            }

            private double Get_Cycle_J()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_J(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_Cycle_J"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_Cycle_J"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_Cycle_J"));
                }
            }
            private void Set_Cycle_J(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_J(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_Cycle_J"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_Cycle_J"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_Cycle_J"));
                }
            }

            private double Get_Cycle_K()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_K(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_Cycle_K"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_Cycle_K"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_Cycle_K"));
                }
            }
            private void Set_Cycle_K(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_K(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_Cycle_K"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_Cycle_K"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_Cycle_K"));
                }
            }

            private double Get_Cycle_L()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_L(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_Cycle_L"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_Cycle_L"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_Cycle_L"));
                }
            }
            private void Set_Cycle_L(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_L(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_Cycle_L"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_Cycle_L"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_Cycle_L"));
                }
            }

            private double Get_Cycle_P()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_P(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_Cycle_P"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_Cycle_P"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_Cycle_P"));
                }
            }
            private void Set_Cycle_P(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_P(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_Cycle_P"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_Cycle_P"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_Cycle_P"));
                }
            }

            private double Get_Cycle_Q()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_Q(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_Cycle_Q"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_Cycle_Q"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_Cycle_Q"));
                }
            }
            private void Set_Cycle_Q(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_Q(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_Cycle_Q"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_Cycle_Q"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_Cycle_Q"));
                }
            }

            private double Get_Cycle_R()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_R(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_Cycle_R"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_Cycle_R"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_Cycle_R"));
                }
            }
            private void Set_Cycle_R(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_R(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_Cycle_R"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_Cycle_R"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_Cycle_R"));
                }
            }

            private double Get_Cycle_Z()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_Z(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_Cycle_Z"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_Cycle_Z"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_Cycle_Z"));
                }
            }
            private void Set_Cycle_Z(double value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_Z(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_Cycle_Z"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_Cycle_Z"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_Cycle_Z"));
                }
            }

            private int Get_Cycle_RetractMode()
            {
                try
                {
                    return KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_RetractMode(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_Cycle_RetractMode"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_Cycle_RetractMode"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_Cycle_RetractMode"));
                }
            }
            private void Set_Cycle_RetractMode(int value)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_RetractMode(_InstanceHandle, value);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_Cycle_RetractMode"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_Cycle_RetractMode"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_Cycle_RetractMode"));
                }
            }
            #endregion

            /// <summary>
            /// Updates the given tool table location
            /// </summary>
            /// <param name="id">Slot position</param>
            /// <param name="length">Tool Length Offset</param>
            /// <param name="diameter">Tool Diameter Offset</param>
            public void SetTool(int id, double length, double diameter)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_SetTool(_InstanceHandle, id, length, diameter);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Set_Tool"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Set_Tool"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Set_Tool"));
                }
            }


            /// <summary>
            /// Get info on specific tool
            /// </summary>
            /// <param name="id">Slot position</param>
            /// <param name="length">Tool Length Offset</param>
            /// <param name="diameter">Tool Diameter Offset</param>
            public void GetTool(int id, ref double length, ref double diameter)
            {
                try
                {
                    KM_dotnet_Interop_GCodeInterpreter_GetTool(_InstanceHandle, id, ref length, ref diameter);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "Get_Tool"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "Get_Tool"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "Get_Tool"));
                }
            }
        }
    }
}
