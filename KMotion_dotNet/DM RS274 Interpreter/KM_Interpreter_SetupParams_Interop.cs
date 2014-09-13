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



KM_Interpreter_SetupParams_Interop.cs 
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
        /// you are aware of the reprecutions when using these parameters
        /// </summary>
        public partial class KM_Interpreter_SetupParams
        {
            #region DLL Xport Refs
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_A_Axis_Offset(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_A_Axis_Offset(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_A_Current_Position(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_A_Current_Position(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_A_Origin_Offset(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_A_Origin_Offset(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_B_Axis_Offset(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_B_Axis_Offset(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_B_Current_Position(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_B_Current_Position(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_B_Origin_Offset(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_B_Origin_Offset(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_C_Axis_Offset(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_C_Axis_Offset(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_C_Current_Position(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_C_Current_Position(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_C_Origin_Offset(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_C_Origin_Offset(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_X_Axis_Offset(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_X_Axis_Offset(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_X_Current_Position(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_X_Current_Position(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_X_Origin_Offset(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_X_Origin_Offset(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_Y_Axis_Offset(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_Y_Axis_Offset(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_Y_Current_Position(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_Y_Current_Position(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_Y_Origin_Offset(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_Y_Origin_Offset(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_Z_Axis_Offset(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_Z_Axis_Offset(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_Z_Current_Position(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_Z_Current_Position(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_Z_Origin_Offset(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_Z_Origin_Offset(IntPtr handle, double value);



            //Modals

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_ControlMode(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_ControlMode(IntPtr handle, int value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_LengthUnits(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_LengthUnits(IntPtr handle, int value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_OriginIndex(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_OriginIndex(IntPtr handle, int value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_WorkPlane(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_WorkPlane(IntPtr handle, int value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_PercentFlag(IntPtr handle);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_ProbeComplete(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_ProbeComplete(IntPtr handle, int value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_ProgramX(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_ProgramX(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_ProgramY(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_ProgramY(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_SequenceNumber(IntPtr handle);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_CurrentLine(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_CurrentLine(IntPtr handle, int value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_TraverseSpeed(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_TraverseSpeed(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_LineLength(IntPtr handle);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Get_FileName(IntPtr handle, ref string file);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Get_LineText(IntPtr handle, ref string line);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_CurrentMotionMode(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_CurrentMotionMode(IntPtr handle, int value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_Active_GCode(IntPtr handle, int index);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_Active_MCode(IntPtr handle, int index);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_Active_Setting(IntPtr handle, int index);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_DistanceMode(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_DistanceMode(IntPtr handle, int value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_FeedMode(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_FeedMode(IntPtr handle, int value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_FloodCoolantOn(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_FloodCoolantOn(IntPtr handle, int value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_MistCoolantOn(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_MistCoolantOn(IntPtr handle, int value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_BlockDelete(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_BlockDelete(IntPtr handle, int value);




            //Tooling Params  

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_CurrentToolSlot(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_CurrentToolSlot(IntPtr handle, int value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_SelectedToolSlot(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_SelectedToolSlot(IntPtr handle, int value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_ToolLengthOffset(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_ToolLengthOffset(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_ToolLengthOffsetIndex(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_ToolLengthOffsetIndex(IntPtr handle, int value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_RadiusComp(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_RadiusComp(IntPtr handle, int value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_RadiusCompValue(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_RadiusCompValue(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_RadiusCompSide(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_RadiusCompSide(IntPtr handle, int value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_AllowSpindleSpeedOverride(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_AllowSpindleSpeedOverride(IntPtr handle, int value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_SpindleDirection(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_SpindleDirection(IntPtr handle, int value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_SpindleSpeed(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_SpindleSpeed(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_SetTool(IntPtr handle, int id, double length, double diameter);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_GetTool(IntPtr handle, int id, ref double length, ref double diameter);


            //Fixed Cycle Params     

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_CC(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_CC(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_I(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_I(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_J(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_J(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_K(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_K(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_L(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_L(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_P(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_P(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_Q(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_Q(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_R(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_R(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern double KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_Z(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_Z(IntPtr handle, double value);

            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern int KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_RetractMode(IntPtr handle);
            [DllImport("KMotion_dotNet_Interop.dll")]
            static extern void KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_RetractMode(IntPtr handle, int value);

            #endregion
        }
    }
}
