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



KM_CoordMotion_MotionParams_Interop.cs 
*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace KMotion_dotNet
{
    /// <summary>
    /// Trajectory Planner Motion Parameter Settings
    /// </summary>
    public partial class KM_MotionParams
    {
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_BreakAngle(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_BreakAngle(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CollinearTol(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CollinearTol(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CornerTol(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CornerTol(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_FacetAngle(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_FacetAngle(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_TPLookahead(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_TPLookahead(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_RadiusA(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_RadiusA(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_RadiusB(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_RadiusB(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_RadiusC(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_RadiusC(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelX(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelX(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelY(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelY(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelZ(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelZ(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelA(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelA(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelB(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelB(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelC(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelC(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelX(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelX(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelY(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelY(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelZ(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelZ(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelA(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelA(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelB(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelB(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelC(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelC(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchX(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchX(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchY(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchY(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchZ(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchZ(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchA(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchA(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchB(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchB(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchC(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchC(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxLinearLength(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxLinearLength(IntPtr handle, double value);

        [return: MarshalAs(UnmanagedType.I1)]
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern bool KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_DegreesA(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_DegreesA(IntPtr handle, bool value);

        [return: MarshalAs(UnmanagedType.I1)]
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern bool KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_DegreesB(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_DegreesB(IntPtr handle, bool value);

        [return: MarshalAs(UnmanagedType.I1)]
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern bool KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_DegreesC(IntPtr handle);
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_DegreesC(IntPtr handle, bool value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_SetTPParams(IntPtr handle);
    }
}
