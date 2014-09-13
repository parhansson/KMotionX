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



KM_CoordMotion.cs 
*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace KMotion_dotNet
{

    /// <summary>
    /// Coordinated Motion class representing the CoordMotion.cpp object
    /// from the C++ Dynotion Code
    /// 
    /// Use to directly control multiple axis motion
    /// 
    /// Notably::
    /// 
    /// Straight Traverse
    /// 
    /// Straight Feed
    /// 
    /// Arc Feed
    /// </summary>
    public partial class KM_CoordMotion : IDisposable
    {
        #region Fields
        /// <summary>
        /// CCoordMotion class instance pointer
        /// </summary>
        private IntPtr _InstanceHandle = new IntPtr(0L);

 	   private KM_Controller _Controller;

        /// <summary>
        /// Maximum allowed return error string length
        /// </summary>
        private int _ErrorLength = 100;

        /// <summary>
        /// Flag to determine that this object has been properly initialized
        /// </summary>
        private bool _Loaded = false;

        /// <summary>
        /// Accessor to the various MotionParams
        /// </summary>
        private KM_MotionParams _MotionParams;

        /// <summary>
        /// Internal Straight Traverse callback handler for the CoordMotion.cpp class
        /// 
        /// this delegate for the callback prevents the Garbage Collector from releasing the 
        /// unmanaged function pointer from being destroyed
        /// </summary>
        private KM_CoordMotionStraightTraverseSixAxisHandler CoordMotionStraightTraverseHandler;

        /// <summary>
        /// Internal Straight Feed callback handler for the CoordMotion.cpp class
        /// 
        /// this delegate for the callback prevents the Garbage Collector from releasing the 
        /// unmanaged function pointer from being destroyed
        /// </summary>
        private KM_CoordMotionStraightFeedSixAxisHandler CoordMotionStraightFeedHandler;

        /// <summary>
        /// Internal Arc Feed callback handler for the CoordMotion.cpp class
        /// 
        /// this delegate for the callback prevents the Garbage Collector from releasing the 
        /// unmanaged function pointer from being destroyed
        /// </summary>
        private KM_CoordMotionArcFeedSixAxisHandler CoordMotionArcFeedHandler;


        /// <summary>
        /// Event handler to signal the download of a Straight Traverse segment into the Coordmotion buffer
        /// </summary>
        public event KM_CoordMotionStraightTraverseHandler CoordMotionStraightTraverse;

        /// <summary>
        /// Event handler to signal the download of a Straight Traverse segment into the Coordmotion buffer
        /// </summary>
        public event KM_CoordMotionStraightTraverseSixAxisHandler CoordMotionStraightTraverseSixAxis;

        /// <summary>
        /// Event handler to signal the download of a Straight Feed segment into the Coordmotion buffer
        /// </summary>
        public event KM_CoordMotionStraightFeedHandler CoordMotionStraightFeed;

        /// <summary>
        /// Event handler to signal the download of a Straight Feed segment into the Coordmotion buffer
        /// </summary>
        public event KM_CoordMotionStraightFeedSixAxisHandler CoordMotionStraightFeedSixAxis;

        /// <summary>
        /// Event handler to signal the download of a Arc Feed segment into the Coordmotion buffer
        /// </summary>
        public event KM_CoordMotionArcFeedHandler CoordMotionArcFeed;

        /// <summary>
        /// Event handler to signal the download of a Arc Feed segment into the Coordmotion buffer
        /// </summary>
        public event KM_CoordMotionArcFeedSixAxisHandler CoordMotionArcFeedSixAxis;

        /// <summary>
        /// Event handler to signal calling application when an async traverse has completed
        /// </summary>
        public event TraverseCompletedHandler AsyncTraverseCompleted;


    /// <summary>
    /// RS274 Interpreter Component
    /// </summary>
    private KM_Interpreter _Interpreter = null;

        #endregion

        #region Properties
        /// <summary>
        /// CCoordMotion class instance pointer
        /// </summary>
    public IntPtr InstanceHandle
        {
            get
            {
            return _InstanceHandle;
            }
        }
        /// <summary>
        /// Maximum allowed return error string length
        /// </summary>
        public int MaxErrorLength
        {
            get
            {
                return _ErrorLength;
            }
            set
            {
                _ErrorLength = value;
            }
        }
        /// <summary>
        /// Board number (for use in multiple board scenarios)
        /// </summary>
        public int BoardNumber
        {
            get
            {
                return _Controller.BoardNumber;
            }
        }
        /// <summary>
        /// Flag to determine that this object has been properly initialized
        /// </summary>
        public bool Loaded
        {
            get
            {
                return _Loaded;
            }
            set
            {
                _Loaded = value;
            }
        }
        /// <summary>
        /// Gets/Sets the Feedrate override for all axes
        /// </summary>
        public double FeedRateOverride
        {
            get
            {
                return GetFeedRateOverride(); ;
            }
            set
            {
                SetFeedRateOverride(value);
            }
        }
        /// <summary>
        /// Returns true if the CoordMotion is aborting
        /// </summary>
        public bool IsAbort
        {
            get
            {
                return GetIsAbort();
            }
        }
        /// <summary>
        /// Returns true if the CoordMotion is halted
        /// </summary>
        public bool IsHalt
        {
            get
            {
                return GetIsHalt();
            }
        }
        /// <summary>
        /// Flags whether to Simulate commands or not
        /// </summary>
        public bool IsSimulation
        {
            get
            {
                return Get_Simulate();
            }
            set
            {
                Set_Simulate(value);
            }
        }
        /// <summary>
        /// Result of previous Interpreter Halt
        /// </summary>
        public PREV_STOP_TYPE IsPreviouslyStopped
        {
            get
            {
                return Get_PreviouslyStopped();
            }
            set
            {
                Set_PreviouslyStopped(value);
            }
        }
        /// <summary>
        /// Accessor to the various MotionParams
        /// </summary>
        public KM_MotionParams MotionParams
        {
            get
            {
                if (_MotionParams == null)
                {
                    _MotionParams = new KM_MotionParams(_InstanceHandle);
                }
                return _MotionParams;
            }
        }


        /// <summary>
    /// RS274 Interpreter Component
        /// </summary>
    public KM_Interpreter Interpreter
    {
        get
        {
            if (_Interpreter == null)
            {
                _Interpreter = new KM_Interpreter(this);

                //Initialize....
            }

            return _Interpreter;
            }
        set
            {
            _Interpreter = value;
            }
        }

    #endregion

        /// <summary>
    /// Primary constructor for KM_CoordMotion object
        /// </summary>
    public KM_CoordMotion(KM_Controller controller)
        {
            try
            {
                _Controller = controller;
                KM_dotnet_Interop_CoordMotion_New(_Controller.InstanceHandle, ref _InstanceHandle);

                Abort();
                ClearAbort();
                SetStraightTraverseCallback();
                SetStraightFeedCallback();
                SetArcFeedCallback();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "KM_CoordMotion"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "KM_CoordMotion"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "KM_CoordMotion"));
            }
        }

        /// <summary>
        /// Gets how may segments are currently loaded into the CoordMotion buffer
        /// </summary>
        public int BufferedSegments
        {
            get
            {
                try
                {
                    return KM_dotnet_Interop_CoordMotion_DownloadedSegmentCount(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "BufferedSegments"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "BufferedSegments"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "BufferedSegments"));
                }
            }
        }

        /// <summary>
        /// Gets time Total time downloaded into the CoordMotion buffer
        /// </summary>
        public double TotalTimeInBuffer
        {
            get
            {
                try
                {
                    return KM_dotnet_Interop_CoordMotion_TotalDownloadedTime(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "TotalTimeInBuffer"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "TotalTimeInBuffer"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "TotalTimeInBuffer"));
                }
            }
        }

        /// <summary>
        /// Gets how many seconds have already been executed
        /// </summary>
        public double TimeExecuted
        {
            get
            {
                try
                {
                    return KM_dotnet_Interop_CoordMotion_TimeAlreadyExecuted(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "TimeExecuted"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "TimeExecuted"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "TimeExecuted"));
                }
            }
        }

        /// <summary>
        /// Gets time Total time downloaded into the CoordMotion buffer
        /// </summary>
        public double RemainingTimeInBuffer
        {
            get
            {
                try
                {
                    return KM_dotnet_Interop_CoordMotion_RemainingTime(_InstanceHandle);
                }
                catch (DllNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                        this.ToString(), "RemainingTimeInBuffer"));
                }
                catch (EntryPointNotFoundException e)
                {
                    throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                       this.ToString(), "RemainingTimeInBuffer"));
                }
                catch (Exception e)
                {
                    throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                      this.ToString(), "RemainingTimeInBuffer"));
                }
            }
        }

        /// <summary>
        /// Initializes download
        /// </summary>
        public void DownloadInit()
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_DownloadInit(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "DownloadInit"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "DownloadInit"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "DownloadInit"));
            }
        }
        /// <summary>
        /// Cehecks for a motion halt
        /// </summary>
        /// <param name="coord"> specify axis mapped axes only</param>
        /// <returns>true if a halt exists</returns>
        public int CheckMotionHalt(bool coord)
        {
            int retval = -1;
            try
            {
                retval = KM_dotnet_Interop_CoordMotion_CheckMotionHalt(_InstanceHandle, coord);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "CheckMotionHalt"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "CheckMotionHalt"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "CheckMotionHalt"));
            }
            return retval;
        }
        /// <summary>
        /// Stop the execution of current motion in a controlled number
        /// </summary>
        public void ExecutionStop()
        {
            try
            {
                var returnvalue = KM_dotnet_Interop_CoordMotion_ExecutionStop(_InstanceHandle);
                // Handle returnvalue
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ExecutionStop"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ExecutionStop"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ExecutionStop"));
            }
        }

        /// <summary>
        /// Gets the current Feedrate override
        /// </summary>
        /// <returns>override percentage</returns>
        private double GetFeedRateOverride()
        {
            double feedoverride = 1;
            try
            {
                feedoverride = KM_dotnet_Interop_CoordMotion_Get_FeedRateOverride(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "GetFeedRateOverride"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "GetFeedRateOverride"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "GetFeedRateOverride"));
            }
            return feedoverride;
        }

        /// <summary>
        /// Sets the current Feedrate override
        /// </summary>
        /// <param name="value">override percentage</param>
        private void SetFeedRateOverride(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_Set_FeedRateOverride(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "SetFeedRateOverride"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "SetFeedRateOverride"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "SetFeedRateOverride"));
            }
        }
        /// <summary>
        /// Gets the axis destination
        /// </summary>
        /// <param name="axis">Axis map index</param>
        /// <returns>actual axis destination</returns>
        public double GetAxisDestination(int axis)
        {
            double dest = 0.0;
            try
            {
                var returnval = KM_dotnet_Interop_CoordMotion_Get_AxisDestination(_InstanceHandle, axis, ref dest);
                //process  returnval
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "GetAxisDestination"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "GetAxisDestination"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "GetAxisDestination"));
            }
            return dest;
        }
        /// <summary>
        /// Gets the axis position
        /// </summary>
        /// <param name="axis">Axis map index</param>
        /// <returns>actual axis position</returns>
        public double GetAxisPosition(int axis)
        {
            double dest = 0.0;
            try
            {
                var returnval = KM_dotnet_Interop_CoordMotion_Get_AxisPosition(_InstanceHandle, axis, ref dest);
                //process  returnval
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "GetAxisPosition"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "GetAxisPosition"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "GetAxisPosition"));
            }
            return dest;
        }

        /// <summary>
        /// Property accessor to query Abort condition
        /// </summary>
        /// <returns>true if device is in an Abort state</returns>
        private bool GetIsAbort()
        {
            bool abort = false;
            try
            {
                abort = KM_dotnet_Interop_CoordMotion_Get_Abort(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "GetIsAbort"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "GetIsAbort"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "GetIsAbort"));
            }
            return abort;
        }
        /// <summary>
        /// Abort causes all motion to abruptly stop usually due to something catastrophic like a communication error
        /// Use in an E-Stop condition
        /// </summary>
        public void Abort()
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_Set_Abort(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Abort"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Abort"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Abort"));
            }
        }
        /// <summary>
        /// Clears any existing Abort conditions
        /// </summary>
        public void ClearAbort()
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_Clear_Abort(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ClearAbort"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ClearAbort"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ClearAbort"));
            }
        }


        /// <summary>
        /// Property accessor to query Halt condition
        /// </summary>
        /// <returns>true if device is in an Halt state</returns>
        private bool GetIsHalt()
        {
            bool halt = false;
            try
            {
                halt = KM_dotnet_Interop_CoordMotion_Get_Halt(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "GetIsHalt"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "GetIsHalt"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "GetIsHalt"));
            }
            return halt;
        }
        /// <summary>
        /// Halt cause motion to stop in a controlled manner.  A feed hold is commanded so that the motion decellerates along the intended path. 
        /// After the system completely stops, the exact point in the Coordinated Motion Buffer where the stop actually happened is uploaded and the GCode Interpreter is rewound to that point. 
        /// </summary>
        public void Halt()
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_Set_Halt(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Halt"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Halt"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Halt"));
            }
        }
        /// <summary>
        /// Clear any exisiting halts
        /// </summary>
        public void ClearHalt()
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_Clear_Halt(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ClearHalt"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ClearHalt"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ClearHalt"));
            }
        }
        /// <summary>
        /// Flushes the segment buffer 
        /// </summary>
        public void FlushSegments()
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_FlushSegments(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "FlushSegments"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "FlushSegments"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "FlushSegments"));
            }
        }

        /// <summary>
        /// Waits for all buffered segments to complete
        /// Blocks calling thread until the buffer is complete
        /// </summary>
        /// <param name="suppressdisablederror">suppress errors cause by commanding a disabled axis</param>
        public void WaitForSegmentsFinished(bool suppressdisablederror)
        {
            try
            {
                var returnvalue = KM_dotnet_Interop_CoordMotion_WaitForSegmentsFinished(_InstanceHandle, suppressdisablederror);
                // process returnvalue
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "WaitForSegmentsFinished"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "WaitForSegmentsFinished"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "WaitForSegmentsFinished"));
            }
        }
        /// <summary>
        /// Blocks calling thread until all commanded motion is complete
        /// </summary>
        public void WaitForMoveXYZABCFinished()
        {
            try
            {
                var returnvalue = KM_dotnet_Interop_CoordMotion_WaitForMoveXYZABCFinished(_InstanceHandle);
                //process return value
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "WaitForMoveXYZABCFinished"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "WaitForMoveXYZABCFinished"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "WaitForMoveXYZABCFinished"));
            }
        }


        /// <summary>
        /// Processes a console script command
        /// </summary>
        /// <param name="command">console script</param>
        /// <param name="flushbufferfirst">Flush current buffered commands before adding a new one</param>
        public void DoKMotionCmd(string command, bool flushbufferfirst)
        {
            try
            {
                var returnvalue = KM_dotnet_Interop_CoordMotion_DoKMotionCmd(_InstanceHandle, command, flushbufferfirst);
                //process return value
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "DoKMotionCmd"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "DoKMotionCmd"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "DoKMotionCmd"));
            }
        }
        /// <summary>
        /// Inserts a console script command in the buffer
        /// </summary>
        /// <param name="command">command</param>
        public void DoKMotionBufCmd(string command)
        {
            try
            {
                var returnvalue = KM_dotnet_Interop_CoordMotion_DoKMotionBufCmd(_InstanceHandle, command);
                //process return value
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "DoKMotionBufCmd"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "DoKMotionBufCmd"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "DoKMotionBufCmd"));
            }
        }


        /// <summary>
        /// Write position data to file
        /// </summary>
        /// <param name="command">file name to append to</param>
        public void MeasurePointAppendToFile(string command)
        {
            try
            {
                var returnvalue = KM_dotnet_Interop_CoordMotion_MeasurePointAppendToFile(_InstanceHandle, command);
                //process return value
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "MeasurePointAppendToFile"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "MeasurePointAppendToFile"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "MeasurePointAppendToFile"));
            }
        }

        /// <summary>
        /// Fires an async delegate to move axes at traversal speeds non-interpolated
        /// </summary>
        /// <param name="x">x position</param>
        /// <param name="y">y position</param>
        /// <param name="z">z position</param>
        /// <param name="a">a position</param>
        /// <param name="b">b position</param>
        /// <param name="c">c position</param>
        /// <param name="suppresscallback">Prevents callback information from being dispatched</param>
        /// <param name="usercallback">User supplied object that gets returned when traverse is complete</param>
        public void StartTraverse(double x, double y, double z, double a, double b, double c, bool suppresscallback, object usercallback)
        {
            var action = new Action(delegate()
                {
                    StraightTraverse(x, y, z, a, b, c, suppresscallback);
                });
            action.BeginInvoke(OnTraverseCompletes, usercallback);
        }

        /// <summary>
        /// Returns after the Async Traverse is called
        /// </summary>
        /// <param name="result">User supplied object that gets returned when traverse is complete</param>
        protected void OnTraverseCompletes(IAsyncResult result)
        {
            var caller = result.AsyncState;

            var temp = this.AsyncTraverseCompleted;

            if (temp != null)
            {
                temp(caller);
            }
        }

        /// <summary>
        /// Move axes at traversal speeds non-interpolated
        /// </summary>
        /// <param name="x">x position</param>
        /// <param name="y">y position</param>
        /// <param name="z">z position</param>
        /// <param name="a">a position</param>
        /// <param name="b">b position</param>
        /// <param name="c">c position</param>
        /// <param name="suppresscallback">Prevents callback information from being dispatched</param>
        public void StraightTraverse(double x, double y, double z, double a, double b, double c, bool suppresscallback)
        {
            try
            {
                var returnvalue = KM_dotnet_Interop_CoordMotion_StraightTraverse(_InstanceHandle, x, y, z, a, b, c, suppresscallback);
                //process return value
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "StraightTraverse"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "StraightTraverse"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "StraightTraverse"));
            }
        }
        /// <summary>
        /// Perform arc motion at a specified feed rate
        /// </summary>
        /// <param name="feedrate">Desired feed rate</param>
        /// <param name="plane">plane to perform arc (determines axis pair to use)</param>
        /// <param name="planarXend">X end point of arc</param>
        /// <param name="planarYend">Y end point of arc</param>
        /// <param name="planarXcenter">X center of rotation of arc</param>
        /// <param name="planarYcenter">Y center of rotation of arc</param>
        /// <param name="rotation">CW or ACW</param>
        /// <param name="planarZ">End point of perpindicular axis(for helical motion)</param>
        /// <param name="a">final end position of a axis</param>
        /// <param name="b">final end position of a axis</param>
        /// <param name="c">final end position of a axis</param>
        /// <param name="sequence_number">internal reference</param>
        /// <param name="ID">internal reference</param>
        public void ArcFeed(double feedrate, int plane, double planarXend, double planarYend,
          double planarXcenter, double planarYcenter, int rotation, double planarZ, double a, double b, double c, int sequence_number, int ID)
        {
            try
            {
                var returnvalue = KM_dotnet_Interop_CoordMotion_ArcFeed(_InstanceHandle, feedrate, plane, planarXend, planarYend, planarXcenter, planarYcenter, rotation, planarZ, a, b, c, sequence_number, ID);
                //process return value
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ArcFeed"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ArcFeed"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ArcFeed"));
            }
        }
        /// <summary>
        /// Perform arc motion at a specified feed rate and Acceleration
        /// </summary>
        /// <param name="feedrate">Desired feed rate</param>
        /// <param name="accel">Desired acceleration rate</param>
        /// <param name="plane">plane to perform arc (determines axis pair to use)</param>
        /// <param name="planarXend">X end point of arc</param>
        /// <param name="planarYend">Y end point of arc</param>
        /// <param name="planarXcenter">X center of rotation of arc</param>
        /// <param name="planarYcenter">Y center of rotation of arc</param>
        /// <param name="rotation">CW or ACW</param>
        /// <param name="planarZ">End point of perpindicular axis(for helical motion)</param>
        /// <param name="a">final end position of a axis</param>
        /// <param name="b">final end position of a axis</param>
        /// <param name="c">final end position of a axis</param>
        /// <param name="sequence_number">internal reference</param>
        /// <param name="ID">internal reference</param>
        public void ArcFeedAccel(double feedrate, double accel, int plane, double planarXend, double planarYend,
          double planarXcenter, double planarYcenter, int rotation, double planarZ, double a, double b, double c, int sequence_number, int ID)
        {
            try
            {
                var returnvalue = KM_dotnet_Interop_CoordMotion_ArcFeedAccel(_InstanceHandle, feedrate, accel, plane, planarXend, planarYend, planarXcenter, planarYcenter, rotation, planarZ, a, b, c, sequence_number, ID);
                //process return value
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ArcFeedAccel"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ArcFeedAccel"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ArcFeedAccel"));
            }
        }
        /// <summary>
        /// Perform linear motion at a specified feed rate
        /// </summary>
        /// <param name="feedrate">Desired feed rate</param>
        /// <param name="x">X end point</param>
        /// <param name="y">Y end point</param>
        /// <param name="z">Z end point</param>
        /// <param name="a">A end point</param>
        /// <param name="b">B end point</param>
        /// <param name="c">C end point</param>
        /// <param name="sequence_number">internal reference</param>
        /// <param name="ID">internal reference</param>
        public void StraightFeed(double feedrate, double x, double y, double z, double a, double b, double c, int sequence_number, int ID)
        {
            try
            {
                var returnvalue = KM_dotnet_Interop_CoordMotion_StraightFeed(_InstanceHandle, feedrate, x, y, z, a, b, c, sequence_number, ID);
                //process return value
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "StraightFeed"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "StraightFeed"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "StraightFeed"));
            }
        }
        /// <summary>
        /// Perform linear motion at a specified feed rate and Acceleration
        /// </summary>
        /// <param name="feedrate">Desired feed rate</param>
        /// <param name="accel">Desired acceleration rate</param>
        /// <param name="x">X end point</param>
        /// <param name="y">Y end point</param>
        /// <param name="z">Z end point</param>
        /// <param name="a">A end point</param>
        /// <param name="b">B end point</param>
        /// <param name="c">C end point</param>
        /// <param name="sequence_number">internal reference</param>
        /// <param name="ID">internal reference</param>
        public void StraightFeedAccel(double feedrate, double accel, double x, double y, double z, double a, double b, double c, int sequence_number, int ID)
        {
            try
            {
                var returnvalue = KM_dotnet_Interop_CoordMotion_StraightFeedAccel(_InstanceHandle, feedrate, accel, x, y, z, a, b, c, sequence_number, ID);
                //process return value
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "StraightFeedAccel"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "StraightFeedAccel"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "StraightFeedAccel"));
            }
        }
        /// <summary>
        /// Download all computed segments from the Trajectory Planner to the device buffer
        /// </summary>
        public void DownloadDoneSegments()
        {
            try
            {
                var returnvalue = KM_dotnet_Interop_CoordMotion_DownloadDoneSegments(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "DownloadDoneSegments"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "DownloadDoneSegments"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "DownloadDoneSegments"));
            }
        }
        /// <summary>
        /// Output given segment
        /// </summary>
        /// <param name="segment">segment index to output</param>
        public void OutputSegment(int segment)
        {
            try
            {
                var returnvalue = KM_dotnet_Interop_CoordMotion_OutputSegment(_InstanceHandle, segment);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "OutputSegment"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "OutputSegment"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "OutputSegment"));
            }
        }
        /// <summary>
        /// Perform segment execution
        /// </summary>
        /// <param name="segment">segment index</param>
        public void DoSpecialCommand(int segment)
        {
            try
            {
                var returnvalue = KM_dotnet_Interop_CoordMotion_DoSpecialCommand(_InstanceHandle, segment);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "DoSpecialCommand"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "DoSpecialCommand"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "DoSpecialCommand"));
            }
        }
        /// <summary>
        /// Update the Trajectory Planner's segment feedrates.
        /// </summary>
        /// <param name="i0"></param>
        /// <param name="i1"></param>
        /// <param name="feedrate"></param>
        public void DoRateAdjustments(int i0, int i1, double feedrate)
        {
            try
            {
                var returnvalue = KM_dotnet_Interop_CoordMotion_DoRateAdjustments(_InstanceHandle, i0, i1, feedrate);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "DoRateAdjustments"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "DoRateAdjustments"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "DoRateAdjustments"));
            }
        }
        /// <summary>
        /// Waiting for additional info....
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="z"></param>
        /// <param name="a"></param>
        /// <param name="b"></param>
        /// <param name="c"></param>
        public void GetAxisDefinitions(ref int x, ref int y, ref int z, ref int a, ref int b, ref int c)
        {
            try
            {
                var returnvalue = KM_dotnet_Interop_CoordMotion_GetAxisDefinitions(_InstanceHandle, ref  x, ref  y, ref  z, ref  a, ref  b, ref  c);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "GetAxisDefinitions"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "GetAxisDefinitions"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "GetAxisDefinitions"));
            }
        }
        /// <summary>
        /// Force the Trajectory Planner to update it's parameters
        /// </summary>
        public void SetTPParams()
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_SetTPParams(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "SetTPParams"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "SetTPParams"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "SetTPParams"));
            }
        }
        /// <summary>
        /// Pause motion fow given amount of time 
        /// </summary>
        /// <param name="seconds">time in seconds</param>
        /// <param name="sequence_number">sequence number to be associated with the dwell</param>
        public void Dwell(double seconds, int sequence_number)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_Dwell(_InstanceHandle, seconds, sequence_number);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Dwell"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Dwell"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Dwell"));
            }
        }

        /// <summary>
        /// Sets the passed variables to the CoordMotion's Absolute positions
        /// </summary>
        /// <param name="x">X axis value</param>
        /// <param name="y">Y axis value</param>
        /// <param name="z">Z axis value</param>
        /// <param name="a">A axis value</param>
        /// <param name="b">B axis value</param>
        /// <param name="c">C axis value</param>
        /// <param name="snap">snap to exact previously commanded postion (if close) to avoid rounding errors</param>
        public void UpdateCurrentPositionsABS(ref double x, ref double y, ref double z, ref double a, ref double b, ref double c, bool snap)
        {
            try
            {
                var returnvalue = KM_dotnet_Interop_CoordMotion_ReadCurAbsPosition(_InstanceHandle, ref  x, ref  y, ref  z, ref  a, ref  b, ref  c, snap);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "UpdateCurrentPositionsABS"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "UpdateCurrentPositionsABS"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "UpdateCurrentPositionsABS"));
            }
        }
        /// <summary>
        /// First Syncs the Coor Motion Library to the current axis destinations then 
        /// sets the passed variables to the CoordMotion's Absolute positions
        /// </summary>
        /// <param name="x">X axis value</param>
        /// <param name="y">Y axis value</param>
        /// <param name="z">Z axis value</param>
        /// <param name="a">A axis value</param>
        /// <param name="b">B axis value</param>
        /// <param name="c">C axis value</param>
        public void ReadAndSyncCurPositions(ref double x, ref double y, ref double z, ref double a, ref double b, ref double c)
        {
            try
            {
                var returnvalue = KM_dotnet_Interop_CoordMotion_ReadAndSyncCurPositions(_InstanceHandle, ref  x, ref  y, ref  z, ref  a, ref  b, ref  c);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ReadAndSyncCurPositions"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ReadAndSyncCurPositions"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ReadAndSyncCurPositions"));
            }
        }

        /// <summary>
        /// Sets the internal Straight Traverse callback for the CoordMotion.cpp object
        /// Separate delegate for this callback prevents the Garbage Collector from releasing the 
        /// unmanaged function pointer from being destroyed
        /// </summary>
        private void SetStraightTraverseCallback()
        {
            try
            {
                CoordMotionStraightTraverseHandler = new KM_CoordMotionStraightTraverseSixAxisHandler(OnCoordMotionStraightTraverse);
                KM_dotnet_Interop_CoordMotion_SetStraightTraverseCallback(_InstanceHandle, CoordMotionStraightTraverseHandler);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "SetStraightTraverseCallback"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "SetStraightTraverseCallback"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "SetStraightTraverseCallback"));
            }
        }

        /// <summary>
        /// Sets the internal Straight Feed callback for the CoordMotion.cpp object
        /// Separate delegate for this callback prevents the Garbage Collector from releasing the 
        /// unmanaged function pointer from being destroyed
        /// </summary>
        private void SetStraightFeedCallback()
        {
            try
            {
                CoordMotionStraightFeedHandler = new KM_CoordMotionStraightFeedSixAxisHandler(OnCoordMotionStraightFeed);
                KM_dotnet_Interop_CoordMotion_SetStraightFeedCallback(_InstanceHandle, CoordMotionStraightFeedHandler);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "SetStraightFeedCallback"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "SetStraightFeedCallback"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "SetStraightFeedCallback"));
            }
        }

        /// <summary>
        /// Sets the internal Arc Feed callback for the CoordMotion.cpp object
        /// Separate delegate for this callback prevents the Garbage Collector from releasing the 
        /// unmanaged function pointer from being destroyed
        /// </summary>
        private void SetArcFeedCallback()
        {
            try
            {
                CoordMotionArcFeedHandler = new KM_CoordMotionArcFeedSixAxisHandler(OnCoordMotionArcFeed);
                KM_dotnet_Interop_CoordMotion_SetArcFeedCallback(_InstanceHandle, CoordMotionArcFeedHandler);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "SetArcFeedCallback"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "SetArcFeedCallback"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "SetArcFeedCallback"));
            }
        }

        /// <summary>
        /// Dispatches Straight Traverse callback values to 
        /// CoordMotionStraightTraverse event subscribers
        /// 
        /// </summary>
        /// <param name="x">X destination</param>
        /// <param name="y">Y destination</param>
        /// <param name="z">Z destination</param>
        /// <param name="a">A destination</param>
        /// <param name="b">B destination</param>
        /// <param name="c">C destination</param>
        /// <param name="sequence_number">User supplied sequence</param>
        private void OnCoordMotionStraightTraverse(double x, double y, double z, double a, double b, double c, int sequence_number)
        {
            var temp = this.CoordMotionStraightTraverse;
            if (temp != null)
            {
                temp(x, y, z, sequence_number);
            }
            var temp2 = this.CoordMotionStraightTraverseSixAxis;
            if (temp2 != null)
            {
                temp2(x, y, z, a, b, c, sequence_number);
            }
        }

        /// <summary>
        /// Dispatches Straight Feed callback values to 
        /// CoordMotionStraightFeed event subscribers
        /// 
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
    private void OnCoordMotionStraightFeed(double DesiredFeedRate_in_per_sec, double x, double y, double z, double a, double b, double c, int sequence_number, int ID)
        {
            var temp = this.CoordMotionStraightFeed;
            if (temp != null)
            {
                temp(DesiredFeedRate_in_per_sec, x, y, z, sequence_number, ID);
            }
            var temp2 = this.CoordMotionStraightFeedSixAxis;
            if (temp2 != null)
            {
                temp2(DesiredFeedRate_in_per_sec, x, y, z, a, b, c, sequence_number, ID);
            }
        }

        
        /// <summary>
        /// Dispatches Arc Feed callback values to 
        /// CoordMotionArcFeed event subscribers
        /// 
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
        /// <param name="a">Final A position</param>
        /// <param name="b">Final B position</param>
        /// <param name="c">Final C position</param>
        /// <param name="first_start">Localized "X" start point</param>
        /// <param name="second_start">Localized "Y" start point</param>
        /// <param name="axis_start_point">Initial Z position for helix calls</param>
        /// <param name="sequence_number">User supplied sequence</param>
        /// <param name="ID">User supplied geometry ID</param>
        private void OnCoordMotionArcFeed(bool ZeroLenAsFullCircles, double DesiredFeedRate_in_per_sec,
                                                                            int plane, double first_end, double second_end,
                                                                            double first_axis, double second_axis, int rotation,
                                                                            double axis_end_point, double a, double b, double c, 
                                                                            double first_start, double second_start,
                                                                            double axis_start_point, int sequence_number, int ID)
        {
            var temp = this.CoordMotionArcFeed;
            if (temp != null)
            {
                temp(ZeroLenAsFullCircles, DesiredFeedRate_in_per_sec, plane, first_end, second_end, first_axis, second_axis, rotation,
                   axis_end_point, first_start, second_start, axis_start_point, sequence_number, ID);
            }
            var temp2 = this.CoordMotionArcFeedSixAxis;
            if (temp2 != null)
            {
                temp2(ZeroLenAsFullCircles, DesiredFeedRate_in_per_sec, plane, first_end, second_end, first_axis, second_axis, rotation,
                   axis_end_point, a, b, c, first_start, second_start, axis_start_point, sequence_number, ID);
            }
        }

        /// <summary>
        /// Property getter Accessor for simulation
        /// </summary>
        /// <returns>true if CoordMotion is simulating</returns>
        private bool Get_Simulate()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_Get_Simulate(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_Simulate"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_Simulate"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_Simulate"));
            }
        }
        /// <summary>
        /// Property setter Accessor for simulation
        /// </summary>
        /// <param name="value">flag to determine simulation state</param>
        private void Set_Simulate(bool value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_Set_Simulate(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_Simulate"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_Simulate"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_Simulate"));
            }
        }
        /// <summary>
        /// Property getter Accessor for PreviouslyStopped
        /// </summary>
        /// <returns>code indicating type of stop that occurred</returns>
        private PREV_STOP_TYPE Get_PreviouslyStopped()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_Get_PreviouslyStopped(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_PreviouslyStopped"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_PreviouslyStopped"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_PreviouslyStopped"));
            }
        }
        /// <summary>
        /// Property setter Accessor for PreviouslyStopped
        /// </summary>
        /// <param name="value">code indicating type of stop that occurred</param>
        private void Set_PreviouslyStopped(PREV_STOP_TYPE value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_Set_PreviouslyStopped(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_PreviouslyStopped"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_PreviouslyStopped"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_PreviouslyStopped"));
            }
        }

        #region IDisposable Members
        /// <summary>
        /// Releases object resources
        /// </summary>
        public void Dispose()
        {
            try
            {
        	    this.Interpreter.Dispose();
                KM_dotnet_Interop_CoordMotion_Free(ref _InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Dispose"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Dispose"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Dispose"));
            }
        }

        #endregion
    }
}