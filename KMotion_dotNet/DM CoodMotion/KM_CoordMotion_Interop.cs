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



KM_CoordMotion_Interop.cs 
*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace KMotion_dotNet
{
    /// <summary>
    /// Delegate reperesenting the KM_dotNet_Interop  netSTRAIGHT_TRAVERSE_CALLBACK Function Pointer
    /// </summary>
    /// <param name="x">X destination</param>
    /// <param name="y">Y destination</param>
    /// <param name="z">Z destination</param>
    /// <param name="sequence_number">User supplied sequence</param>
    public delegate void KM_CoordMotionStraightTraverseHandler(double x, double y, double z, int sequence_number);

    /// <summary>
    /// Delegate reperesenting the KM_dotNet_Interop  netSTRAIGHT_TRAVERSE_SIX_AXIS_CALLBACK Function Pointer
    /// </summary>
    /// <param name="x">X destination</param>
    /// <param name="y">Y destination</param>
    /// <param name="z">Z destination</param>
    /// <param name="a">A destination</param>
    /// <param name="b">B destination</param>
    /// <param name="c">C destination</param>
    /// <param name="sequence_number">User supplied sequence</param>
    public delegate void KM_CoordMotionStraightTraverseSixAxisHandler(double x, double y, double z, double a, double b, double c, int sequence_number);

    /// <summary>
    /// Delegate reperesenting the KM_dotNet_Interop  netSTRAIGHT_FEED_CALLBACK Function Pointer
    /// </summary>
    /// <param name="DesiredFeedRate_in_per_sec">Requested Feedrate (may have been adjusted by the Tplanner)</param>
    /// <param name="x">X destination</param>
    /// <param name="y">Y destination</param>
    /// <param name="z">Z destination</param>
    /// <param name="sequence_number">User supplied sequence</param>
    /// <param name="ID">User supplied geometry ID</param>
    public delegate void KM_CoordMotionStraightFeedHandler(double DesiredFeedRate_in_per_sec,
                                                                                   double x, double y, double z, int sequence_number, int ID);
    /// <summary>
    /// Delegate reperesenting the KM_dotNet_Interop  netSTRAIGHT_FEED_SIX_AXIS_CALLBACK Function Pointer
    /// </summary>
    /// <param name="DesiredFeedRate_in_per_sec">Requested Feedrate (may have been adjusted by the Tplanner)</param>
    /// <param name="x">X destination</param>
    /// <param name="y">Y destination</param>
    /// <param name="z">Z destination</param>
    /// <param name="a">A destination</param>
    /// <param name="b">B destination</param>
    /// <param name="c">C destination</param>
    /// <param name="sequence_number">User supplied sequence</param>
    /// <param name="ID">User supplied geometry ID</param>
    public delegate void KM_CoordMotionStraightFeedSixAxisHandler(double DesiredFeedRate_in_per_sec,
                                                                                   double x, double y, double z, double a, double b, double c,int sequence_number, int ID);
    /// <summary>
    /// Delegate reperesenting the KM_dotNet_Interop  netARC_FEED_CALLBACK Function Pointer
    /// </summary>
    /// <param name="ZeroLenAsFullCircles"></param>
    /// <param name="DesiredFeedRate_in_per_sec">Requested Feedrate (may have been adjusted by the Tplanner)</param>
    /// <param name="plane">CANON_PLANE arc is being commanded</param>
    /// <param name="first_end">Localized "X" end point</param>
    /// <param name="second_end">Localized "Y" end point</param>
    /// <param name="first_axis">Localized "X" center point</param>
    /// <param name="second_axis">Localized "Y" center point</param>
    /// <param name="rotation">Rotation of arc 0=CW 1 = CCW</param>
    /// <param name="axis_end_point">Final Z position for helix calls</param>
    /// <param name="first_start">Localized "X" start point</param>
    /// <param name="second_start">Localized "Y" start point</param>
    /// <param name="axis_start_point">Initial Z position for helix calls</param>
    /// <param name="sequence_number">User supplied sequence</param>
    /// <param name="ID">User supplied geometry ID</param>
    public delegate void KM_CoordMotionArcFeedHandler(bool ZeroLenAsFullCircles, double DesiredFeedRate_in_per_sec,
                                                                            int plane, double first_end, double second_end,
                                                                            double first_axis, double second_axis, int rotation,
                                                                            double axis_end_point, double first_start, double second_start,
                                                                            double axis_start_point, int sequence_number, int ID);

    /// <summary>
    /// Delegate reperesenting the KM_dotNet_Interop  netARC_FEED_CALLBACK Function Pointer
    /// </summary>
    /// <param name="ZeroLenAsFullCircles"></param>
    /// <param name="DesiredFeedRate_in_per_sec">Requested Feedrate (may have been adjusted by the Tplanner)</param>
    /// <param name="plane">CANON_PLANE arc is being commanded</param>
    /// <param name="first_end">Localized "X" end point</param>
    /// <param name="second_end">Localized "Y" end point</param>
    /// <param name="first_axis">Localized "X" center point</param>
    /// <param name="second_axis">Localized "Y" center point</param>
    /// <param name="rotation">Rotation of arc 0=CW 1 = CCW</param>
    /// <param name="axis_end_point">Final Z position for helix calls</param>
    /// <param name="a">Final A position </param>
    /// <param name="b">Final B position </param>
    /// <param name="c">Final C position </param>
    /// <param name="first_start">Localized "X" start point</param>
    /// <param name="second_start">Localized "Y" start point</param>
    /// <param name="axis_start_point">Initial Z position for helix calls</param>
    /// <param name="sequence_number">User supplied sequence</param>
    /// <param name="ID">User supplied geometry ID</param>
    public delegate void KM_CoordMotionArcFeedSixAxisHandler(bool ZeroLenAsFullCircles, double DesiredFeedRate_in_per_sec,
                                                                            int plane, double first_end, double second_end,
                                                                            double first_axis, double second_axis, int rotation,
                                                                            double axis_end_point, double a, double b, double c, 
                                                                            double first_start, double second_start,
                                                                            double axis_start_point, int sequence_number, int ID);

    /// <summary>
    /// Delegate that gets fired off on an async Straight Traverse return call
    /// </summary>
    /// <param name="callback">User supplied object that gets returned when complete.  This can be used to keep track of several calls</param>
    public delegate void TraverseCompletedHandler(object callback);
    

    #region Coordinated Motion References
    public partial class KM_CoordMotion : IDisposable
    {  
        [DllImport("KMotion_dotNet_Interop.dll")]
    static extern void KM_dotnet_Interop_CoordMotion_New(IntPtr controller, ref IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_Free(ref IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_DownloadInit(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_CheckMotionHalt(IntPtr handle, bool Coord);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_ExecutionStop(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_Get_FeedRateOverride(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_Set_FeedRateOverride(IntPtr handle, double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_Get_AxisDestination(IntPtr handle, int axis, ref double value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_Get_AxisPosition(IntPtr handle, int axis, ref double value);

        [return: MarshalAs(UnmanagedType.I1)]
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern bool KM_dotnet_Interop_CoordMotion_Get_Abort(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_Set_Abort(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_Clear_Abort(IntPtr handle);

        [return: MarshalAs(UnmanagedType.I1)]
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern bool KM_dotnet_Interop_CoordMotion_Get_Halt(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_Set_Halt(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_Clear_Halt(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_FlushSegments(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_WaitForSegmentsFinished(IntPtr handle, bool NoErrorOnDisable);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_WaitForMoveXYZABCFinished(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_DoKMotionCmd(IntPtr handle, string command, bool FlushBeforeUnbufferedOperation);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_DoKMotionBufCmd(IntPtr handle, string command);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_MeasurePointAppendToFile(IntPtr handle, string command);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_StraightTraverse
            (IntPtr handle, double x, double y, double z, double a, double b, double c, bool NoCallback);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_ArcFeed
            (IntPtr handle, double DesiredFeedRate_in_per_sec, int plane,
            double first_end, double second_end,
            double first_axis, double second_axis, int rotation,
            double axis_end_point, double a, double b, double c, int sequence_number, int ID);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_ArcFeedAccel
            (IntPtr handle, double DesiredFeedRate_in_per_sec, double DesiredAccel, int plane,
            double first_end, double second_end,
            double first_axis, double second_axis, int rotation,
            double axis_end_point, double a, double b, double c, int sequence_number, int ID);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_StraightFeed
            (IntPtr handle, double DesiredFeedRate_in_per_sec,
            double x, double y, double z, double a, double b, double c,
            int sequence_number, int ID);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_StraightFeedAccel
            (IntPtr handle, double DesiredFeedRate_in_per_sec, double DesiredAccel,
            double x, double y, double z, double a, double b, double c,
            int sequence_number, int ID);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_SetStraightTraverseCallback(IntPtr handle, KM_CoordMotionStraightTraverseSixAxisHandler handler);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_SetStraightFeedCallback(IntPtr handle, KM_CoordMotionStraightFeedSixAxisHandler handler);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_SetArcFeedCallback(IntPtr handle, KM_CoordMotionArcFeedSixAxisHandler handler);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_DownloadDoneSegments(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_OutputSegment(IntPtr handle, int iseg);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_DoSpecialCommand(IntPtr handle, int iseg);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_DoRateAdjustments(IntPtr handle, int i0, int i1, double FeedRate);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_GetAxisDefinitions
            (IntPtr handle, ref int x, ref int y, ref int z, ref int a, ref int b, ref int c);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_ReadCurAbsPosition
            (IntPtr handle, ref double x, ref double y, ref double z, ref double a, ref double b, ref double c, bool snap);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_ReadAndSyncCurPositions
            (IntPtr handle, ref double x, ref double y, ref double z, ref double a, ref double b, ref double c);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_SetTPParams(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_Dwell(IntPtr handle, double seconds, int sequence_number);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern int KM_dotnet_Interop_CoordMotion_DownloadedSegmentCount(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_TotalDownloadedTime(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_TimeAlreadyExecuted(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern double KM_dotnet_Interop_CoordMotion_RemainingTime(IntPtr handle);

        [return: MarshalAs(UnmanagedType.I1)]
        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern bool KM_dotnet_Interop_CoordMotion_Get_Simulate(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_Set_Simulate(IntPtr handle, bool value);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern PREV_STOP_TYPE KM_dotnet_Interop_CoordMotion_Get_PreviouslyStopped(IntPtr handle);

        [DllImport("KMotion_dotNet_Interop.dll")]
        static extern void KM_dotnet_Interop_CoordMotion_Set_PreviouslyStopped(IntPtr handle, PREV_STOP_TYPE value);

    }
    #endregion
}
