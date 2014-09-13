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



KM_Interpreter.cs 
*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Reflection;

namespace KMotion_dotNet
{

    /// <summary>
    /// Interpreter class representing the GCodeInterpreter.cpp object
    /// from the C++ Dynotion Code
    /// 
    /// Use to control multiple axis motion from the 
    /// 
    /// NIST RS274 Interpreter
    ///  
    /// </summary>
    public partial class KM_Interpreter : IDisposable
    {
        #region Fields
        /// <summary>
        /// CCoordMotion class instance pointer
        /// </summary>
        protected IntPtr _InstanceHandle = new IntPtr(0L);
        /// <summary>
        /// Maximum allowed return error string length
        /// </summary>
        protected int _ErrorLength = 100;
        /// <summary>
        /// Flag to determine that this object has been properly initialized
        /// </summary>
        protected bool _Loaded = false;
        /// <summary>
        /// Direct Access to the EMC Setup Parameters
        /// Use caution when setting any of these properties during run
        /// </summary>
        protected KM_Interpreter_SetupParams _SetupParams;

        /// <summary>
        /// Internal Interpreter Complete callback handler for the GCodeInterpreter.cpp class
        /// 
        /// this delegate for the callback prevents the Garbage Collector from releasing the 
        /// unmanaged function pointer from being destroyed
        /// </summary>
        protected KM_GCodeInterpreterCompleteHandler InterpreterCompletedHandler;

        /// <summary>
        /// Internal Interpreter Status callback handler for the GCodeInterpreter.cpp class
        /// 
        /// this delegate for the callback prevents the Garbage Collector from releasing the 
        /// unmanaged function pointer from being destroyed
        /// </summary>
        protected KM_GCodeInterpreterStatusHandler InterpreterStatusUpdatedHandler;

        /// <summary>
        /// Internal Interpreter UserCallback callback handler for the GCodeInterpreter.cpp class
        /// 
        /// this delegate for the callback prevents the Garbage Collector from releasing the 
        /// unmanaged function pointer from being destroyed
        /// </summary>
        protected KM_GCodeInterpreterUserCallbackHandler InterpreterUserCallbackRequestedHandler;

        /// <summary>
        /// Internal Interpreter UserMcodeCallback callback handler for the GCodeInterpreter.cpp class
        /// 
        /// this delegate for the callback prevents the Garbage Collector from releasing the 
        /// unmanaged function pointer from being destroyed
        /// </summary>
        protected KM_GCodeInterpreterUserMcodeCallbackHandler InterpreterUserMCodeCallbackRequestedHandler;

        /// <summary>
        /// Event handler to signal that Interpreter has completed
        /// </summary>
        public event KM_GCodeInterpreterCompleteHandler InterpreterCompleted;

        /// <summary>
        /// Event handler to signal that Interpreter Status has been updated
        /// </summary>
        public event KM_GCodeInterpreterStatusHandler InterpreterStatusUpdated;

        /// <summary>
        /// Event handler to signal that a User Callback has been issued
        /// Blocks interpretation until handled by subscriber(s)
        /// </summary>
        public event KM_GCodeInterpreterUserCallbackHandler InterpreterUserCallbackRequested;

        /// <summary>
        /// Event handler to signal that a User MCode Callback has been issued
        /// Blocks interpretation until handled by subscriber(s)
        /// </summary>
        public event KM_GCodeInterpreterUserMcodeCallbackHandler InterpreterUserMCodeCallbackRequested;
        #endregion

        #region Properties
        /// <summary>
        /// CCoordMotion class instance pointer
        /// </summary>
        public int Handle
        {
            get
            {
                return (int)_InstanceHandle;
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
        /// Direct Access to the EMC Setup Parameters
        /// Use caution when setting any of these properties during run
        /// </summary>
        public KM_Interpreter_SetupParams SetupParams
        {
            get
            {
                if (_SetupParams == null)
                {
                    _SetupParams = new KM_Interpreter_SetupParams(_InstanceHandle);
                }
                return _SetupParams;
            }
        }
        /// <summary>
        /// Enables Resuming after a Halt or Tool Change
        /// </summary>
        public bool CanResume
        {
            get
            {
                return Get_CanResume();
            }
            set
            {
                Set_CanResume(value);
            }
        }
        /// <summary>
        /// Safe Z height to be raised to.
        /// </summary>
        public double ResumeSafeZ
        {
            get
            {
                return Get_ResumeSafeZ();
            }
            set
            {
                Set_ResumeSafeZ(value);
            }
        }
        /// <summary>
        /// Specifies if the Z heights are Absolute or Relative 
        /// </summary>
        public int ResumeSafeRelAbs
        {
            get
            {
                return Get_ResumeSafeRelAbs();
            }
            set
            {
                Set_ResumeSafeRelAbs(value);
            }
        }
        /// <summary>
        ///  Specifies whether or not the Z should be first moved up 
        /// </summary>
        public bool ResumeMoveToSafeZ
        {
            get
            {
                return Get_ResumeMoveToSafeZ();
            }
            set
            {
                Set_ResumeMoveToSafeZ(value);
            }
        }
        /// <summary>
        /// Specifies whether it is OK to move back to an XY position
        /// </summary>
        public bool ResumeTraverseXY
        {
            get
            {
                return Get_ResumeTraverseXY();
            }
            set
            {
                Set_ResumeTraverseXY(value);
            }
        }
        /// <summary>
        /// Position in X on Resume
        /// </summary>
        public double ResumeTraverseSafeX
        {
            get
            {
                return Get_ResumeTraverseSafeX();
            }
            set
            {
                Set_ResumeTraverseSafeX(value);
            }
        }
        /// <summary>
        /// Position in Y on Resume
        /// </summary>
        public double ResumeTraverseSafeY
        {
            get
            {
                return Get_ResumeTraverseSafeY();
            }
            set
            {
                Set_ResumeTraverseSafeY(value);
            }
        }
        /// <summary>
        /// Specifies if the Spindle should be re-started 
        /// </summary>
        public bool ResumeSafeStartSpindle
        {
            get
            {
                return Get_ResumeSafeStartSpindle();
            }
            set
            {
                Set_ResumeSafeStartSpindle(value);
            }
        }
        /// <summary>
        /// Specifies Spindle direction if it is to be re-started 
        /// </summary>
        public int ResumeSafeSpindleCWCCW
        {
            get
            {
                return Get_ResumeSafeSpindleCWCCW();
            }
            set
            {
                Set_ResumeSafeSpindleCWCCW(value);
            }
        }
        /// <summary>
        /// pecifies if the Z should then be fed down to some height 
        /// </summary>
        public bool ResumeDoSafeFeedZ
        {
            get
            {
                return Get_ResumeDoSafeFeedZ();
            }
            set
            {
                Set_ResumeDoSafeFeedZ(value);
            }
        }
        /// <summary>
        /// Specifies height Z should be fed down to 
        /// </summary>
        public double ResumeFeedSafeZ
        {
            get
            {
                return Get_ResumeFeedSafeZ();
            }
            set
            {
                Set_ResumeFeedSafeZ(value);
            }
        }
        /// <summary>
        /// Specifies Rate at which feed rate should be restored to
        /// </summary>
        public double ResumeFeedRate
        {
            get
            {
                return Get_ResumeFeedRate();
            }
            set
            {
                Set_ResumeFeedRate(value);
            }
        }
        /// <summary>
        /// Specifies Rate at which feed rate should be restored to
        /// </summary>
        public double ResumeZFeedRate
        {
            get
            {
                return Get_ResumeZFeedRate();
            }
            set
            {
                Set_ResumeZFeedRate(value);
            }
        }
        /// <summary>
        /// Specifies whether or not the specified feed rate should be restored 
        /// </summary>
        public bool ResumeRestoreFeedRate
        {
            get
            {
                return Get_ResumeRestoreFeedRate();
            }
            set
            {
                Set_ResumeRestoreFeedRate(value);
            }
        }

        /// <summary>
        /// Specifies whether or not the Interpreter should be intialized before execution 
        /// </summary>
        public bool InitializeOnExecute
        {
            get
            {
                return Get_InitializeOnExecute();
            }
            set
            {
                Set_InitializeOnExecute(value);
            }
        }

        /// <summary>
        /// Specifies whether or not the Interpreter should read the Tool file before execution 
        /// </summary>
        public bool ReadToolFile
        {
            get
            {
                return Get_ReadToolFile();
            }
            set
            {
                Set_ReadToolFile(value);
            }
        }


        /// <summary>
        /// File path for the EMC style Tool File
        /// </summary>
        public string ToolFile
        {
            get
            {
                return Get_ToolFile();
            }
            set
            {
                Set_ToolFile(value);
            }
        }
        /// <summary>
        /// File path for the EMC setup parameters
        /// </summary>
        public string SetupFile
        {
            get
            {
                return Get_ParameterFile();
            }
            set
            {
                Set_ParameterFile(value);
            }
        }
 
        /// <summary>
        /// File path for the EMC Vars file
        /// </summary>
        public string VarsFile
        {
            get
            {
                return Get_VarsFile();
            }
            set
            {
                Set_VarsFile(value);
            }
        }

        #endregion

        /// <summary>
        /// Primary constructor for the KM_Interpreter object
        /// </summary>
        public KM_Interpreter(KM_CoordMotion coordmotion)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_New(coordmotion.InstanceHandle, ref _InstanceHandle);

                Set_G_COMPLETE_CALLBACK();
                Set_G_STATUS_CALLBACK();
                Set_G_USER_CALLBACK();
                Set_G_USER_MCODE_CALLBACK();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Ctor"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Ctor"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Ctor"));
            }
        }

        #region Property Dispatchers
        /// <summary>
        /// KM_Interpreter Abort Condition
        /// </summary>
        protected bool GetIsAbort()
        {
            bool abort = false;
            try
            {
                abort = KM_dotnet_Interop_GCodeInterpreter_GetAbort(_InstanceHandle);
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
        /// KM_Interpreter Halt Condition
        /// </summary>
        protected bool GetIsHalt()
        {
            bool halt = false;
            try
            {
                halt = KM_dotnet_Interop_GCodeInterpreter_GetHalt(_InstanceHandle);
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
        /// Get KM_Interpreter possible to resume after a halt
        /// </summary>
        protected bool Get_CanResume()
        {
            bool resume = false;
            try
            {
                resume = KM_dotnet_Interop_GCodeInterpreter_Get_CanResume(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_CanResume"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_CanResume"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_CanResume"));
            }
            return resume;
        }

        /// <summary>
        /// Set KM_Interpreter possible to resume after a halt
        /// </summary>
        protected void Set_CanResume(bool value)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_Set_CanResume(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_CanResume"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_CanResume"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_CanResume"));
            }
        }

        /// <summary>
        /// Get KM_Interpreter Safe Z height for Resume sequence
        /// </summary>
        protected double Get_ResumeSafeZ()
        {
            double retval = 0.0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeZ(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_ResumeSafeZ"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_ResumeSafeZ"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_ResumeSafeZ"));
            }
            return retval;
        }

        /// <summary>
        /// Set KM_Interpreter Safe Z height for Resume sequence
        /// </summary>
        protected void Set_ResumeSafeZ(double value)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeZ(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_ResumeSafeZ"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_ResumeSafeZ"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_ResumeSafeZ"));
            }
        }

        /// <summary>
        /// Get KM_Interpreter Safe Z Motion Relative vs Absolute setting
        /// 1=abs 0=rel
        /// </summary>
        protected int Get_ResumeSafeRelAbs()
        {
            int retval = 0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeRelAbs(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_ResumeSafeRelAbs"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_ResumeSafeRelAbs"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_ResumeSafeRelAbs"));
            }
            return retval;
        }

        /// <summary>
        /// Set KM_Interpreter Safe Z Motion Relative vs Absolute setting
        /// 1=abs 0=rel
        /// </summary>
        protected void Set_ResumeSafeRelAbs(int value)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeRelAbs(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_ResumeSafeRelAbs"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_ResumeSafeRelAbs"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_ResumeSafeRelAbs"));
            }
        }

        /// <summary>
        /// Get KM_Interpreter Safe Z Motion to be performed
        /// </summary>
        protected bool Get_ResumeMoveToSafeZ()
        {
            bool retval = false;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_Get_ResumeMoveToSafeZ(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_ResumeMoveToSafeZ"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_ResumeMoveToSafeZ"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_ResumeMoveToSafeZ"));
            }
            return retval;
        }

        /// <summary>
        /// Set KM_Interpreter Safe Z Motion to be performed
        /// </summary>
        protected void Set_ResumeMoveToSafeZ(bool value)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_Set_ResumeMoveToSafeZ(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_ResumeMoveToSafeZ"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_ResumeMoveToSafeZ"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_ResumeMoveToSafeZ"));
            }
        }

        /// <summary>
        /// Get KM_Interpreter XY Traverse Motion to be performed
        /// </summary>
        protected bool Get_ResumeTraverseXY()
        {
            bool retval = false;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_Get_ResumeTraverseXY(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_ResumeTraverseXY"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_ResumeTraverseXY"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_ResumeTraverseXY"));
            }
            return retval;
        }

        /// <summary>
        /// Set KM_Interpreter XY Traverse Safe Motion to be performed
        /// </summary>
        protected void Set_ResumeTraverseXY(bool value)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_Set_ResumeTraverseXY(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_ResumeTraverseXY"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_ResumeTraverseXY"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_ResumeTraverseXY"));
            }
        }

        /// <summary>
        /// Get KM_Interpreter XY Traverse Safe Motion to be performed
        /// </summary>
        protected double Get_ResumeTraverseSafeX()
        {
            double retval = 0.0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_Get_ResumeTraverseSafeX(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_ResumeTraverseSafeX"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_ResumeTraverseSafeX"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_ResumeTraverseSafeX"));
            }
            return retval;
        }

        /// <summary>
        /// Set KM_Interpreter Traverse Safe Motion X Position
        /// </summary>
        protected void Set_ResumeTraverseSafeX(double value)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_Set_ResumeTraverseSafeX(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_ResumeTraverseSafeX"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_ResumeTraverseSafeX"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_ResumeTraverseSafeX"));
            }
        }

        /// <summary>
        /// Get KM_Interpreter Traverse Safe Motion Y Position
        /// </summary>
        protected double Get_ResumeTraverseSafeY()
        {
            double retval = 0.0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_Get_ResumeTraverseSafeY(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_ResumeTraverseSafeY"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_ResumeTraverseSafeY"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_ResumeTraverseSafeY"));
            }
            return retval;
        }

        /// <summary>
        /// Set KM_Interpreter Traverse Safe Motion Y Position
        /// </summary>
        protected void Set_ResumeTraverseSafeY(double value)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_Set_ResumeTraverseSafeY(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_ResumeTraverseSafeY"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_ResumeTraverseSafeY"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_ResumeTraverseSafeY"));
            }
        }

        /// <summary>
        /// Get KM_Interpreter Safe Resume Start Spindle to be performed
        /// </summary>
        protected bool Get_ResumeSafeStartSpindle()
        {
            bool retval = false;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeStartSpindle(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_ResumeSafeStartSpindle"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_ResumeSafeStartSpindle"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_ResumeSafeStartSpindle"));
            }
            return retval;
        }

        /// <summary>
        /// Set KM_Interpreter Safe Resume Start Spindle to be performed
        /// </summary>
        protected void Set_ResumeSafeStartSpindle(bool value)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeStartSpindle(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_ResumeSafeStartSpindle"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_ResumeSafeStartSpindle"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_ResumeSafeStartSpindle"));
            }
        }

        /// <summary>
        /// Get KM_Interpreter Safe Resume Start Spindle Direction to be performed
        /// </summary>
        protected int Get_ResumeSafeSpindleCWCCW()
        {
            int retval = 0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeSpindleCWCCW(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_ResumeSafeSpindleCWCCW"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_ResumeSafeSpindleCWCCW"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_ResumeSafeSpindleCWCCW"));
            }
            return retval;
        }

        /// <summary>
        /// Set KM_Interpreter Safe Resume Start Spindle Direction to be performed
        /// </summary>
        protected void Set_ResumeSafeSpindleCWCCW(int value)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeSpindleCWCCW(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_ResumeSafeSpindleCWCCW"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_ResumeSafeSpindleCWCCW"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_ResumeSafeSpindleCWCCW"));
            }
        }

        /// <summary>
        /// Get KM_Interpreter Safe Resume Z Feed to be performed
        /// </summary>
        protected bool Get_ResumeDoSafeFeedZ()
        {
            bool retval = false;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_Get_ResumeDoSafeFeedZ(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_ResumeDoSafeFeedZ"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_ResumeDoSafeFeedZ"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_ResumeDoSafeFeedZ"));
            }
            return retval;
        }

        /// <summary>
        /// Set KM_Interpreter Safe Resume Z Feed to be performed
        /// </summary>
        protected void Set_ResumeDoSafeFeedZ(bool value)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_Set_ResumeDoSafeFeedZ(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_ResumeDoSafeFeedZ"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_ResumeDoSafeFeedZ"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_ResumeDoSafeFeedZ"));
            }
        }

        /// <summary>
        /// Get KM_Interpreter Safe Resume Z Feed Rate to be used
        /// </summary>
        protected double Get_ResumeFeedSafeZ()
        {
            double retval = 0.0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_Get_ResumeFeedSafeZ(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_ResumeFeedSafeZ"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_ResumeFeedSafeZ"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_ResumeFeedSafeZ"));
            }
            return retval;
        }

        /// <summary>
        /// Set KM_Interpreter Safe Resume Z Feed Rate to be used
        /// </summary>
        protected void Set_ResumeFeedSafeZ(double value)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_Set_ResumeFeedSafeZ(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_ResumeFeedSafeZ"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_ResumeFeedSafeZ"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_ResumeFeedSafeZ"));
            }
        }

        /// <summary>
        /// Get KM_Interpreter Safe Resume Feed Rate to be used
        /// </summary>
        protected double Get_ResumeFeedRate()
        {
            double retval = 0.0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_Get_ResumeResumeFeedRate(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_ResumeFeedRate"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_ResumeFeedRate"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_ResumeFeedRate"));
            }
            return retval;
        }

        /// <summary>
        /// Set KM_Interpreter Safe Resume Feed Rate to be used
        /// </summary>
        protected void Set_ResumeFeedRate(double value)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_Set_ResumeResumeFeedRate(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_ResumeFeedRate"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_ResumeFeedRate"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_ResumeFeedRate"));
            }
        }

        /// <summary>
        /// Get KM_Interpreter Safe Resume Z Feed Rate to be used
        /// </summary>
        protected double Get_ResumeZFeedRate()
        {
            double retval = 0.0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_Get_ResumeZFeedRate(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_ResumeZFeedRate"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_ResumeZFeedRate"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_ResumeZFeedRate"));
            }
            return retval;
        }

        /// <summary>
        /// Set KM_Interpreter Safe Resume Z Feed Rate to be used
        /// </summary>
        protected void Set_ResumeZFeedRate(double value)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_Set_ResumeZFeedRate(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_ResumeZFeedRate"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_ResumeZFeedRate"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_ResumeZFeedRate"));
            }
        }

        /// <summary>
        /// Get KM_Interpreter Safe Resume Feed Rate to be restored or not
        /// </summary>
        protected bool Get_ResumeRestoreFeedRate()
        {
            bool retval = false;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_Get_ResumeRestoreFeedRate(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_ResumeRestoreFeedRate"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_ResumeRestoreFeedRate"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_ResumeRestoreFeedRate"));
            }
            return retval;
        }

        /// <summary>
        /// Set KM_Interpreter Safe Resume Feed Rate to be restored or not
        /// </summary>
        protected void Set_ResumeRestoreFeedRate(bool value)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_Set_ResumeRestoreFeedRate(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_ResumeRestoreFeedRate"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_ResumeRestoreFeedRate"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_ResumeRestoreFeedRate"));
            }
        }

        #region CoordMotion Property Dispatchers


        #endregion
        #endregion

        /// <summary>
        /// Set the current fixture
        /// </summary>
        /// <param name="fixture">Fixture Index</param>
        public void ChangeFixtureNumber(int fixture)
        {
            int retval = 0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_ChangeFixtureNumber(_InstanceHandle, fixture);
                //Process return value
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ChangeFixtureNumber"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ChangeFixtureNumber"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ChangeFixtureNumber"));
            }
        }

        /// <summary>
        /// Sets the origin offset
        /// </summary>
        /// <param name="index">Origin index - 0 for G92 offsets, 1-9 for fixture offsets</param>
        /// <param name="x">X offset</param>
        /// <param name="y">Y offset</param>
        /// <param name="z">Z offset</param>
        /// <param name="a">a offset</param>
        /// <param name="b">b offset</param>
        /// <param name="c">c offset</param>
        public void SetOrigin(int index, double x, double y, double z, double a, double b, double c)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_SetOrigin(_InstanceHandle, index, x, y, z, a, b, c);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "SetOrigin"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "SetOrigin"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "SetOrigin"));
            }
        }

        /// <summary>
        /// Gets the Origin offset
        /// </summary>
        /// <param name="index">Origin index - 0 for G92 offsets, 1-9 for fixture offsets</param>
        /// <param name="x">X offset</param>
        /// <param name="y">Y offset</param>
        /// <param name="z">Z offset</param>
        /// <param name="a">a offset</param>
        /// <param name="b">b offset</param>
        /// <param name="c">c offset</param>
        public void GetOrigin(int index, ref double x, ref double y, ref double z, ref double a, ref double b, ref double c)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_GetOrigin(_InstanceHandle, index, ref x, ref y, ref z, ref a, ref b, ref c);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "GetOrigin"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "GetOrigin"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "GetOrigin"));
            }
        }

        /// <summary>
        /// Sets an MCode Action definition
        /// </summary>
        /// <param name="index">mcode ex(M6 = 6)</param>
        /// <param name="type">MCODE_TYPE for this action</param>
        /// <param name="val1">double param 1 for action</param>
        /// <param name="val2">double param 2 for action</param>
        /// <param name="val3">double param 3 for action</param>
        /// <param name="val4">double param 4 for action</param>
        /// <param name="val5">double param 5 for action</param>
        /// <param name="name">mcode name</param>
        public void SetMcodeAction(int index, MCODE_TYPE type, double val1, double val2, double val3, double val4, double val5, string name)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_Set_M_CodeAction(_InstanceHandle, index, (int)type, val1, val2, val3, val4, val5, name);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "SetMcodeAction"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "SetMcodeAction"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "SetMcodeAction"));
            }
        }
        /// <summary>
        /// Gets an MCode Action definition
        /// </summary>
        /// <param name="index">mcode ex(M6 = 6)</param>
        /// <param name="type">MCODE_TYPE for this action</param>
        /// <param name="val1">double param 1 for action</param>
        /// <param name="val2">double param 2 for action</param>
        /// <param name="val3">double param 3 for action</param>
        /// <param name="val4">double param 4 for action</param>
        /// <param name="val5">double param 5 for action</param>
        /// <param name="name">mcode name</param>
        public void GetMcodeAction(int index, ref MCODE_TYPE type, ref double val1, ref  double val2, ref  double val3, ref double val4, ref  double val5, ref  string name)
        {
            try
            {
                int mcodetype = 0;
                KM_dotnet_Interop_GCodeInterpreter_Get_M_CodeAction(_InstanceHandle, index, ref mcodetype, ref val1, ref val2, ref val3, ref  val4, ref val5, ref name);
                type = (MCODE_TYPE)mcodetype;
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "GetMcodeAction"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "GetMcodeAction"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "GetMcodeAction"));
            }
        }
        /// <summary>
        /// Takes in an inch value and returns the value in
        /// Interpreter's current units
        /// </summary>
        /// <param name="inches">input inch value</param>
        /// <returns>Current User Units</returns>
        public double InchesToUserUnits(double inches)
        {
            double retval = 0.0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_InchesToUserUnits(_InstanceHandle, inches);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "InchesToUserUnits"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "InchesToUserUnits"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "InchesToUserUnits"));
            }
            return retval;
        }
        /// <summary>
        /// Takes in a value as Interpreter's current units and returns the value in Inches
        /// </summary>
        /// <param name="units">input unit value</param>
        /// <returns>Inches</returns>
        public double UserUnitsToInches(double units)
        {
            double retval = 0.0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_UserUnitsToInches(_InstanceHandle, units);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "UserUnitsToInches"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "UserUnitsToInches"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "UserUnitsToInches"));
            }
            return retval;
        }
        /// <summary>
        /// Takes ABS "X" value and translates to Iterpreter User Units
        /// </summary>
        /// <param name="x">ABS value</param>
        /// <returns>Current X value in User Units</returns>
        public double ConvertAbsToUserUnitsX(double x)
        {
            double retval = 0.0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsX(_InstanceHandle, x);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ConvertAbsToUserUnitsX"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ConvertAbsToUserUnitsX"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ConvertAbsToUserUnitsX"));
            }
            return retval;
        }
        /// <summary>
        /// Takes ABS "Y" value and translates to Iterpreter User Units
        /// </summary>
        /// <param name="y">ABS value</param>
        /// <returns>Current Y value in User Units</returns>
        public double ConvertAbsToUserUnitsY(double y)
        {
            double retval = 0.0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsY(_InstanceHandle, y);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ConvertAbsToUserUnitsY"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ConvertAbsToUserUnitsY"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ConvertAbsToUserUnitsY"));
            }
            return retval;
        }
        /// <summary>
        /// Takes ABS "Z" value and translates to Iterpreter User Units
        /// </summary>
        /// <param name="z">ABS value</param>
        /// <returns>Current Z value in User Units</returns>
        public double ConvertAbsToUserUnitsZ(double z)
        {
            double retval = 0.0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsZ(_InstanceHandle, z);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ConvertAbsToUserUnitsZ"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ConvertAbsToUserUnitsZ"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ConvertAbsToUserUnitsZ"));
            }
            return retval;
        }
        /// <summary>
        /// Takes ABS "A" value and translates to Iterpreter User Units
        /// </summary>
        /// <param name="a">ABS value</param>
        /// <returns>Current A value in User Units</returns>
        public double ConvertAbsToUserUnitsA(double a)
        {
            double retval = 0.0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsA(_InstanceHandle, a);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ConvertAbsToUserUnitsA"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ConvertAbsToUserUnitsA"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ConvertAbsToUserUnitsA"));
            }
            return retval;
        }
        /// <summary>
        /// Takes ABS "B" value and translates to Iterpreter User Units
        /// </summary>
        /// <param name="b">ABS value</param>
        /// <returns>Current B value in User Units</returns>
        public double ConvertAbsToUserUnitsB(double b)
        {
            double retval = 0.0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsB(_InstanceHandle, b);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ConvertAbsToUserUnitsB"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ConvertAbsToUserUnitsB"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ConvertAbsToUserUnitsB"));
            }
            return retval;
        }
        /// <summary>
        /// Takes ABS "C" value and translates to Iterpreter User Units
        /// </summary>
        /// <param name="c">ABS value</param>
        /// <returns>Current C value in User Units</returns>
        public double ConvertAbsToUserUnitsC(double c)
        {
            double retval = 0.0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsC(_InstanceHandle, c);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ConvertAbsToUserUnitsC"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ConvertAbsToUserUnitsC"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ConvertAbsToUserUnitsC"));
            }
            return retval;
        }
        /// <summary>
        /// Invokes a preset "Action"
        /// Will need clarification on how to setup these Actions
        /// </summary>
        /// <param name="i">Action index?</param>
        /// <param name="flushbeforeunbufferedoperation">Flush current operations before execution</param>
        /// <returns>1 if successful</returns>
        public int InvokeAction(int i, bool flushbeforeunbufferedoperation)
        {
            int retval = 0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_InvokeAction(_InstanceHandle, i, flushbeforeunbufferedoperation);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "InvokeAction"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "InvokeAction"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "InvokeAction"));
            }
            return retval;
        }
        /// <summary>
        /// Interpret a File with the functionality to only interpret a portion of the file
        /// (useful for resuming execution after a program stoppage)
        /// </summary>
        /// <param name="fname">Full path of the file to interpret</param>
        /// <param name="start">first line to execute</param>
        /// <param name="end">Last line to execute</param>
        /// <param name="restart">Reinitializes Interpreter Settings</param>
        /// <returns>Error Code</returns>
        public int Interpret(string fname, int start, int end, int restart)
        {
            int retval = 0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_Interpret(_InstanceHandle,
                    2, fname, start, end, restart);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Interpret 1"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Interpret 1"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Interpret 1"));
            }
            return retval;
        }
        /// <summary>
        /// Interpret an entire File
        /// </summary>
        /// <param name="fname">Full path of the file to interpret</param>
        /// <returns>Error Code</returns>
        public int Interpret(string fname)
        {
            int retval = 0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_Interpret(_InstanceHandle,
                    2, fname, 0, -1, 0);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Interpret 2"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Interpret 2"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Interpret 2"));
            }
            return retval;
        }

        /// <summary>
        /// Resets the interpreter params to the inital state
        /// </summary>
        public void InitializeInterpreter()
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_InitializeInterpreter(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "InitializeInterpreter"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "InitializeInterpreter"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "InitializeInterpreter"));
            }
        }

        /// <summary>
        /// Calling this spawns a Windows PC Application by the specified name 
        /// </summary>
        /// <param name="name">Path Name of Executable</param>
        /// <returns>1 if successful</returns>
        public int ExecutePC(string name)
        {
            int retval = 0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_ExecutePC(_InstanceHandle, name);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ExecutePC"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ExecutePC"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ExecutePC"));
            }
            return retval;
        }
        /// <summary>
        /// Internal Use only?
        /// </summary>
        /// <returns>1 if successful</returns>
        public int DoExecute()
        {
            int retval = 0;
            try
            {
                retval = KM_dotnet_Interop_GCodeInterpreter_DoExecute(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "DoExecute"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "DoExecute"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "DoExecute"));
            }
            return retval;
        }
        /// <summary>
        /// Halt Intpreter Execution
        /// </summary>
        public void Halt()
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_SetHalt(_InstanceHandle);
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
        /// Abort Intpreter Execution
        /// </summary>
        public void Abort()
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_SetAbort(_InstanceHandle);
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
        /// Syncronizes the Interpreter Position to the current Axes Positions
        /// </summary>
        /// <param name="x">X sync'ed position</param>
        /// <param name="y">Y sync'ed position</param>
        /// <param name="z">Z sync'ed position</param>
        /// <param name="a">a sync'ed position</param>
        /// <param name="b">b sync'ed position</param>
        /// <param name="c">c sync'ed position</param>
        public void ReadAndSynchCurInterpreterPosition(ref double x, ref double y, ref double z, ref double a, ref double b, ref double c)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_ReadAndSyncCurPositions
                    (_InstanceHandle, ref x, ref y, ref z, ref a, ref b, ref c);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ReadAndSynchCurInterpreterPosition"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ReadAndSynchCurInterpreterPosition"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ReadAndSynchCurInterpreterPosition"));
            }
        }

        /// <summary>
        /// Syncronizes the Interpreter Position to the current Axes Positions
        /// </summary>
        /// <param name="x">X sync'ed position</param>
        /// <param name="y">Y sync'ed position</param>
        /// <param name="z">Z sync'ed position</param>
        /// <param name="a">a sync'ed position</param>
        /// <param name="b">b sync'ed position</param>
        /// <param name="c">c sync'ed position</param>
        public void ReadCurInterpreterPosition(ref double x, ref double y, ref double z, ref double a, ref double b, ref double c)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_ReadCurInterpreterPosition
                    (_InstanceHandle, ref x, ref y, ref z, ref a, ref b, ref c);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ReadCurInterpreterPosition"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ReadCurInterpreterPosition"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ReadCurInterpreterPosition"));
            }
        }

        /// <summary>
        /// Read Hardware and convert to Absolute Machine Coordinates
        /// </summary>
        /// <param name="x">X returned Machine Coordinate</param>
        /// <param name="y">Y returned Machine Coordinate</param>
        /// <param name="z">Z returned Machine Coordinate</param>
        /// <param name="a">a returned Machine Coordinate</param>
        /// <param name="b">b returned Machine Coordinate</param>
        /// <param name="c">c returned Machine Coordinate</param>
        public void ReadCurMachinePosition(ref double x, ref double y, ref double z, ref double a, ref double b, ref double c)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_ReadCurMachinePosition
                    (_InstanceHandle, ref x, ref y, ref z, ref a, ref b, ref c);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ReadCurMachinePosition"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ReadCurMachinePosition"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ReadCurMachinePosition"));
            }
        }

        /// <summary>
        /// Convert Absolute to Machine Coordinates
        /// </summary>
        /// <param name="x">X Passed in Absolute Coordinate</param>
        /// <param name="y">Y Passed in Absolute Coordinate</param>
        /// <param name="z">Z Passed in Absolute Coordinate</param>
        /// <param name="a">a Passed in Absolute Coordinate</param>
        /// <param name="b">b Passed in Absolute Coordinate</param>
        /// <param name="c">c Passed in Absolute Coordinate</param>
        /// <param name="xp">X returned Machine Coordinate</param>
        /// <param name="yp">Y returned Machine Coordinate</param>
        /// <param name="zp">Z returned Machine Coordinate</param>
        /// <param name="ap">a returned Machine Coordinate</param>
        /// <param name="bp">b returned Machine Coordinate</param>
        /// <param name="cp">c returned Machine Coordinate</param>
        public void ConvertAbsoluteToMachine(double x, double y, double z, double a, double b, double c,
                        ref double xp, ref double yp, ref double zp, ref double ap, ref double bp, ref double cp)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_ConvertAbsoluteToMachine
                    (_InstanceHandle, x, y, z, a, b, c, ref xp, ref yp, ref zp, ref ap, ref bp, ref cp);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ConvertAbsoluteToMachine"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ConvertAbsoluteToMachine"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ConvertAbsoluteToMachine"));
            }
        }

        /// <summary>
        /// Convert Absolute to Interpreter Coordinates
        /// </summary>
        /// <param name="x">X Passed in Absolute Coordinate</param>
        /// <param name="y">Y Passed in Absolute Coordinate</param>
        /// <param name="z">Z Passed in Absolute Coordinate</param>
        /// <param name="a">a Passed in Absolute Coordinate</param>
        /// <param name="b">b Passed in Absolute Coordinate</param>
        /// <param name="c">c Passed in Absolute Coordinate</param>
        /// <param name="xp">X returned Interpreter Coordinate</param>
        /// <param name="yp">Y returned Interpreter Coordinate</param>
        /// <param name="zp">Z returned Interpreter Coordinate</param>
        /// <param name="ap">a returned Interpreter Coordinate</param>
        /// <param name="bp">b returned Interpreter Coordinate</param>
        /// <param name="cp">c returned Interpreter Coordinate</param>
        public void ConvertAbsoluteToInterpreterCoord(double x, double y, double z, double a, double b, double c,
                        ref double xp, ref double yp, ref double zp, ref double ap, ref double bp, ref double cp)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_ConvertAbsoluteToInterpreterCoord
                    (_InstanceHandle, x, y, z, a, b, c, ref xp, ref yp, ref zp, ref ap, ref bp, ref cp);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ConvertAbsoluteToInterpreterCoord"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ConvertAbsoluteToInterpreterCoord"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ConvertAbsoluteToInterpreterCoord"));
            }
        }

        private bool Get_InitializeOnExecute()
        {
            try
            {
                return KM_dotnet_Interop_GCodeInterpreter_GetInitializeOnExecute(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_InitializeOnExecute"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_InitializeOnExecute"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_InitializeOnExecute"));
            }
        }

        private void Set_InitializeOnExecute(bool value)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_SetInitializeOnExecute(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_InitializeOnExecute"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_InitializeOnExecute"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_InitializeOnExecute"));
            }
        }


        private bool Get_ReadToolFile()
        {
            try
            {
                return KM_dotnet_Interop_GCodeInterpreter_GetReadToolFile(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_ReadToolFile"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_ReadToolFile"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_ReadToolFile"));
            }
        }

        private void Set_ReadToolFile(bool value)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_SetReadToolFile(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_ReadToolFile"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_ReadToolFile"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_ReadToolFile"));
            }
        }


        private string Get_ToolFile()
        {
            try
            {
                return KM_dotnet_Interop_GCodeInterpreter_GetToolFile(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_ToolFile"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_ToolFile"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_ToolFile"));
            }
        }

        private void Set_ToolFile(string value)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_SetToolFile(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_ToolFile"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_ToolFile"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_ToolFile"));
            }
        }

        private string Get_ParameterFile()
        {
            try
            {
                return KM_dotnet_Interop_GCodeInterpreter_GetSetupFile(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_ParameterFile"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_ParameterFile"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_ParameterFile"));
            }
        }

        private void Set_ParameterFile(string value)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_SetSetupFile(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_ParameterFile"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_ParameterFile"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_ParameterFile"));
            }
        }

        /// <summary>
        /// Gets the file to be used to store/restore Interpreter GCode Variables
        /// </summary>
        private string Get_VarsFile()
        {
            try
            {
                return KM_dotnet_Interop_GCodeInterpreter_GetVarsFile(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_VarsFile"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_VarsFile"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_VarsFile"));
            }
        }

        /// <summary>
        /// Sets the file to be used to store/restore Interpreter GCode Variables
        /// </summary>
      	private void Set_VarsFile(string value)
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_SetVarsFile(_InstanceHandle, value);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_VarsFile"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_VarsFile"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_VarsFile"));
            }
        }


        /// <summary>
        /// Sets the internal Interpreter Complete callback for the GcodeInterpreter.cpp object
        /// Separate delegate for this callback prevents the Garbage Collector from releasing the 
        /// unmanaged function pointer from being destroyed
        /// </summary>
        private void Set_G_COMPLETE_CALLBACK()
        {
            try
            {
                InterpreterCompletedHandler = new KM_GCodeInterpreterCompleteHandler(OnInterpreterCompleted);
                KM_dotnet_Interop_GCodeInterpreter_Set_G_COMPLETE_CALLBACK(_InstanceHandle, InterpreterCompletedHandler);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_G_COMPLETE_CALLBACK"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_G_COMPLETE_CALLBACK"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_G_COMPLETE_CALLBACK"));
            }
        }

        /// <summary>
        /// Sets the internal Interpreter Status callback for the GcodeInterpreter.cpp object
        /// Separate delegate for this callback prevents the Garbage Collector from releasing the 
        /// unmanaged function pointer from being destroyed
        /// </summary>
        private void Set_G_STATUS_CALLBACK()
        {
            try
            {
                InterpreterStatusUpdatedHandler = new KM_GCodeInterpreterStatusHandler(OnInterpreterStatusUpdated);
                KM_dotnet_Interop_GCodeInterpreter_Set_G_STATUS_CALLBACK(_InstanceHandle, InterpreterStatusUpdatedHandler);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_G_STATUS_CALLBACK"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_G_STATUS_CALLBACK"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_G_STATUS_CALLBACK"));
            }
        }

        /// <summary>
        /// Sets the internal Interpreter User Callback callback for the GcodeInterpreter.cpp object
        /// Separate delegate for this callback prevents the Garbage Collector from releasing the 
        /// unmanaged function pointer from being destroyed
        /// </summary>
        private void Set_G_USER_CALLBACK()
        {
            try
            {
                InterpreterUserCallbackRequestedHandler = new KM_GCodeInterpreterUserCallbackHandler(OnInterpreterUserCallbackRequested);
                KM_dotnet_Interop_GCodeInterpreter_Set_G_USER_CALLBACK(_InstanceHandle, InterpreterUserCallbackRequestedHandler);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_G_USER_CALLBACK"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_G_USER_CALLBACK"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_G_USER_CALLBACK"));
            }
        }

        /// <summary>
        /// Sets the internal Interpreter User MCode  callback for the GcodeInterpreter.cpp object
        /// Separate delegate for this callback prevents the Garbage Collector from releasing the 
        /// unmanaged function pointer to be destroyed
        /// </summary>
        private void Set_G_USER_MCODE_CALLBACK()
        {
            try
            {
                InterpreterUserMCodeCallbackRequestedHandler = new KM_GCodeInterpreterUserMcodeCallbackHandler(OnInterpreterUserMcodeCallbackRequested);
                KM_dotnet_Interop_GCodeInterpreter_Set_G_USER_MCODE_CALLBACK(_InstanceHandle, InterpreterUserMCodeCallbackRequestedHandler);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_G_USER_MCODE_CALLBACK"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_G_USER_MCODE_CALLBACK"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_G_USER_MCODE_CALLBACK"));
            }
        }



        /// <summary>
        /// Dispatches Interpreter Completed callback values to 
        /// InterpreterCompleted event subscribers
        /// 
        /// </summary>
        /// <param name="status">Completion Status Code</param>
        /// <param name="lineno">last processed line number of Gcode file</param>
        /// <param name="sequence_number">User supplied sequence</param>
        /// <param name="err">Error (if any)</param>
        private void OnInterpreterCompleted(int status, int lineno, int sequence_number, string err)
        {
            var temp = this.InterpreterCompleted;
            if (temp != null)
            {
                temp(status, lineno, sequence_number, err);
            }
        }

        /// <summary>
        /// Dispatches Status Updated callback values to 
        /// InterpreterStatusUpdated event subscribers
        /// 
        /// </summary>
        /// <param name="lineno">processed line number of Gcode file</param>
        /// <param name="msg">Gcode Block Data</param>
        private void OnInterpreterStatusUpdated(int lineno, string msg)
        {
            var temp = this.InterpreterStatusUpdated;
            if (temp != null)
            {
                temp(lineno, msg);
            }
        }

        /// <summary>
        /// Dispatches User Callback Requested callback values to 
        /// InterpreterUserCallbackRequested event subscribers
        /// 
        /// </summary>
        /// <param name="msg">Imbedded User Callback</param>
        private void OnInterpreterUserCallbackRequested(string msg)
        {
            var temp = this.InterpreterUserCallbackRequested;
            if (temp != null)
            {
                temp(msg);
            }
        }

        /// <summary>
        /// Dispatches User MCode Callback Requested callback values to 
        /// InterpreterUserMcodeCallbackRequested event subscribers
        /// 
        /// </summary>
        /// <param name="mcode">MCode number </param>
        private int OnInterpreterUserMcodeCallbackRequested(int mcode)
        {
            var temp = this.InterpreterUserMCodeCallbackRequested;
            if (temp != null)
            {
                return temp(mcode);
            }
            return 0;
        }

        #region IDisposable Members

        /// <summary>
        /// Releases the object resources
        /// </summary>
        public void Dispose()
        {
            try
            {
                KM_dotnet_Interop_GCodeInterpreter_Free(ref _InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                Console.WriteLine(e.ToString());
            }
            catch (EntryPointNotFoundException e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        #endregion
    }
}
