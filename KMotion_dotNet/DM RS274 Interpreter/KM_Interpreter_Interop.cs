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



KM_Interpreter_Interop.cs 
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
        /// Delegate reperesenting the
        /// 
        /// KM_dotNet_Interop  netG_COMPLETE_CALLBACK Function Pointer
        /// </summary>
        /// <param name="status">Exit Status Code</param>
        /// <param name="lineno">Final processed line number</param>
        /// <param name="sequence_number">Block sequence</param>
        /// <param name="err">Error message ( if any )</param>
        public delegate void KM_GCodeInterpreterCompleteHandler(int status, int lineno, int sequence_number, string err);

        /// <summary>
        /// Delegate reperesenting the
        /// 
        /// KM_dotNet_Interop  netG_STATUS_CALLBACK Function Pointer
        /// </summary>
        /// <param name="lineno">current line number of the interpreted block</param>
        /// <param name="msg">Block Data</param>
        public delegate void KM_GCodeInterpreterStatusHandler(int lineno, string msg);

        /// <summary>
        /// Delegate reperesenting the
        /// 
        /// KM_dotNet_Interop  netG_USER_CALLBACK Function Pointer
        /// </summary>
        /// <param name="msg">Interpreted User Callback Message</param>
        public delegate void KM_GCodeInterpreterUserCallbackHandler(string msg);

        /// <summary>
        /// Delegate reperesenting the
        /// 
        /// KM_dotNet_Interop  netG_USER_MCODE_CALLBACK Function Pointer
        /// </summary>
        /// <param name="code">Interpreted User MCODE Callback code</param>
        public delegate int KM_GCodeInterpreterUserMcodeCallbackHandler(int code);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_GCodeInterpreter_New(IntPtr coordmotion, ref IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Free(ref IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_InitializeInterpreter(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern bool KM_dotnet_Interop_GCodeInterpreter_GetInitializeOnExecute(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_SetInitializeOnExecute(IntPtr handle, bool value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern bool KM_dotnet_Interop_GCodeInterpreter_GetReadToolFile(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_SetReadToolFile(IntPtr handle, bool value);


        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_GCodeInterpreter_SetOrigin(IntPtr handle, int index, double x, double y, double z, double a, double b, double c);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_GetOrigin(IntPtr handle, int index, ref double x, ref double y, ref double z, ref double a, ref double b, ref double c);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Set_M_CodeAction(IntPtr handle, int index, int type, double val1, double val2, double val3, double val4, double val5, string name);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Get_M_CodeAction(IntPtr handle, int index, ref int type, ref double val1, ref double val2, ref double val3, ref double val4, ref double val5, ref string name);
        //Resume Parameters


        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern bool KM_dotnet_Interop_GCodeInterpreter_Get_CanResume(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Set_CanResume(IntPtr handle, bool value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeZ(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeZ(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeRelAbs(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeRelAbs(IntPtr handle, int value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern bool KM_dotnet_Interop_GCodeInterpreter_Get_ResumeMoveToSafeZ(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Set_ResumeMoveToSafeZ(IntPtr handle, bool value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern bool KM_dotnet_Interop_GCodeInterpreter_Get_ResumeTraverseXY(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Set_ResumeTraverseXY(IntPtr handle, bool value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_GCodeInterpreter_Get_ResumeTraverseSafeX(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Set_ResumeTraverseSafeX(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_GCodeInterpreter_Get_ResumeTraverseSafeY(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Set_ResumeTraverseSafeY(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern bool KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeStartSpindle(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeStartSpindle(IntPtr handle, bool value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeSpindleCWCCW(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeSpindleCWCCW(IntPtr handle, int value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern bool KM_dotnet_Interop_GCodeInterpreter_Get_ResumeDoSafeFeedZ(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Set_ResumeDoSafeFeedZ(IntPtr handle, bool value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_GCodeInterpreter_Get_ResumeFeedSafeZ(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Set_ResumeFeedSafeZ(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_GCodeInterpreter_Get_ResumeResumeFeedRate(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Set_ResumeResumeFeedRate(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_GCodeInterpreter_Get_ResumeZFeedRate(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Set_ResumeZFeedRate(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern bool KM_dotnet_Interop_GCodeInterpreter_Get_ResumeRestoreFeedRate(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Set_ResumeRestoreFeedRate(IntPtr handle, bool value);


        //Functions....
 

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Set_G_COMPLETE_CALLBACK(IntPtr handle, KM_GCodeInterpreterCompleteHandler handler);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Set_G_STATUS_CALLBACK(IntPtr handle, KM_GCodeInterpreterStatusHandler handler);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Set_G_USER_CALLBACK(IntPtr handle, KM_GCodeInterpreterUserCallbackHandler handler);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_Set_G_USER_MCODE_CALLBACK(IntPtr handle, KM_GCodeInterpreterUserMcodeCallbackHandler handler);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_CoordMotion_SetStraightTraverseCallback(IntPtr handle, KM_CoordMotionStraightTraverseSixAxisHandler handler);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_CoordMotion_SetStraightFeedCallback(IntPtr handle, KM_CoordMotionStraightFeedSixAxisHandler handler);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_CoordMotion_SetArcFeedCallback(IntPtr handle, KM_CoordMotionArcFeedSixAxisHandler handler);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_GCodeInterpreter_ChangeFixtureNumber(IntPtr handle, int fixture);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_GCodeInterpreter_InchesToUserUnits(IntPtr handle, double inches);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_GCodeInterpreter_UserUnitsToInches(IntPtr handle, double units);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsX(IntPtr handle, double x);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsY(IntPtr handle, double y);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsZ(IntPtr handle, double z);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsA(IntPtr handle, double a);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsB(IntPtr handle, double b);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsC(IntPtr handle, double c);


        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_GCodeInterpreter_InvokeAction(IntPtr handle, int i, bool flushbeforeunbufferedoperation);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_GCodeInterpreter_Interpret(IntPtr handle,
              int board_type,
              string fname,
              int start, int end,
              int restart);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_GCodeInterpreter_ExecutePC(IntPtr handle, string name);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_GCodeInterpreter_DoExecute(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_GCodeInterpreter_DoExecuteComplete(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_SetHalt(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern bool KM_dotnet_Interop_GCodeInterpreter_GetHalt(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_SetAbort(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern bool KM_dotnet_Interop_GCodeInterpreter_GetAbort(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_ReadAndSyncCurPositions
            (IntPtr handle, ref double x, ref double y, ref double z, ref double a, ref double b, ref double c);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_ReadCurInterpreterPosition
            (IntPtr handle, ref double x, ref double y, ref double z, ref double a, ref double b, ref double c);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_ReadCurMachinePosition
            (IntPtr handle, ref double x, ref double y, ref double z, ref double a, ref double b, ref double c);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_ConvertAbsoluteToMachine
            (IntPtr handle, double x, double y, double z, double a, double b, double c, 
            ref double xp, ref double yp, ref double zp, ref double ap, ref double bp, ref double cp);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_ConvertAbsoluteToInterpreterCoord
            (IntPtr handle, double x, double y, double z, double a, double b, double c,
            ref double xp, ref double yp, ref double zp, ref double ap, ref double bp, ref double cp);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_SetToolFile(IntPtr handle, string name);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern string KM_dotnet_Interop_GCodeInterpreter_GetToolFile(IntPtr handle);


        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_SetSetupFile(IntPtr handle, string name);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern string KM_dotnet_Interop_GCodeInterpreter_GetSetupFile(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_GCodeInterpreter_SetVarsFile(IntPtr handle, string name);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern string KM_dotnet_Interop_GCodeInterpreter_GetVarsFile(IntPtr handle);
    }
}