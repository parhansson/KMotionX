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



KM_Controller.cs 
*/

using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Reflection;
using System.IO;
using System.Windows.Forms;
using System.Configuration;
using System.Diagnostics;
//GAC Post Build = "$(FrameworkSDKDir)bin\gacutil.exe"  /i "$(TargetPath)" /f
namespace KMotion_dotNet
{
    /// <summary>
    /// Callback delegate for KM_Controller returns
    /// </summary>
    /// <param name="message">message from KM_Controller function</param>
    /// <returns></returns>
    public delegate int KMConsoleHandler(string message);
    /// <summary>
    /// callback for low level KMotion error messages
    /// </summary>
    public delegate void KMErrorHandler(string message);
    /// <summary>
    /// Exception class for handling specific DynoMotion.net exceptions
    /// </summary>
    public class DMException : Exception
    {
        /// <summary>
        /// Object where executoin of exception was encountered 
        /// </summary>
        private object _Sender;
        /// <summary>
        /// Actual exception thrown
        /// </summary>
        private Exception _InnerException;
        /// <summary>
        /// Additional information about exception
        /// </summary>
        private string _Message;

        /// <summary>
        /// Object where execution of exception was encountered 
        /// </summary>
        public object Sender
        {
            get
            {
                return _Sender;
            }
        }
        /// <summary>
        /// Actual exception thrown
        /// </summary>
        public new Exception InnerException
        {
            get
            {
                return _InnerException;
            }
        }
        /// <summary>
        /// Additional information about exception
        /// </summary>
        public override string Message
        {
            get
            {
                return _Message;
            }
        }

        /// <summary>
        /// Primary constructor for DM Exception
        /// </summary>
        /// <param name="sender">Current execution object on the stack</param>
        /// <param name="ex">original exception encountered</param>
        /// <param name="message">additional information</param>
        public DMException(object sender, Exception ex, string message)
        {
            _Sender = sender;
            _InnerException = ex;
            _Message = message;
        }
    }

    /// <summary>
    /// Special Dynomotion USB Error Exception
    /// </summary>
    public class DM_USB_Exception : Exception
    {
        /// <summary>
        /// Special Dynomotion USB Error Exception
        /// </summary>
       public DM_USB_Exception(string message)
            : base(message)
        {

        }
    }
    /// <summary>
    /// Special Dynomotion KmotionServer Error Exception
    /// </summary>
    public class DM_Firmware_Exception : Exception
    {
        /// <summary>
        /// Special Dynomotion KmotionServer Error Exception
        /// </summary>
        public DM_Firmware_Exception(string message)
            : base(message)
        {

        }
    }
    /// <summary>
    /// Special Dynomotion Compiler Error Exception
    /// </summary>
    public class DM_Compiler_Exception : Exception
    {
        /// <summary>
        /// Special Dynomotion Compiler Error Exception
        /// </summary>
        public DM_Compiler_Exception(string message)
            : base(message)
        {

        }
    }
    /// <summary>
    /// Special Dynomotion Board Disconnected Exception
    /// </summary>
    public class DM_Disconnected_Exception : Exception
    {
        /// <summary>
        /// Special Dynomotion Board Disconnected Exception
        /// </summary>
        public DM_Disconnected_Exception(string message)
            : base(message)
        {

        }
    }
    /// <summary>
    /// This is the primary object for interfacing with a Dynomotion product in code
    /// 
    /// You can think of this object as representing the board and an access point for features of the hardware
    /// 
    /// There should only be one KM_Controller object per board, per application domain at any given time
    /// 
    /// To set project to update the GAC: "$(FrameworkSdkDir)\bin\gacutil.exe" /i "$(TargetPath)" /f
    /// </summary>
    public partial class KM_Controller : IDisposable
    {
        #region Fields
        /// <summary>
        /// KMotion_DLL class instance pointer
        /// </summary>
        private IntPtr _InstanceHandle = new IntPtr(0L);
        /// <summary>
        /// Maximum allowed return error string length
        /// </summary>
        private int _ErrorLength = 100;
        /// <summary>
        /// Board USB ID
        /// </summary>
        private int _BoardID = -1;
        /// <summary>
        /// Board number (for use in multiple board scenarios)
        /// </summary>
        private int _BoardNumber = 0;
        /// <summary>
        /// Dynomotion board device type
        /// </summary>
        private int _BoardType = -1;
        private bool _Loaded = false;


        private bool _ThrowExceptions = true;

        /// <summary>
        /// Contains all messages until cleared
        /// </summary>
        private string _MessageLog = "";
        /// <summary>
        /// Flag to determine that this object has been properly initialized
        /// </summary>
        private string _ErrorLog = "";

        /// <summary>
        /// Internal callback handler for the c++ interop KMotion.DLL class
        /// 
        /// <remarks>
        /// <para>
        /// This delegate for the callback prevents the Garbage Collector from releasing the 
        /// unmanaged function pointer from being destroyed
        /// </para>
        /// </remarks>
        /// </summary>
        private  KMConsoleHandler KMCallBackhandler;
        /// <summary>
        /// Internal error message pump for the c++ interop KMotion.DLL class
        /// 
        /// <remarks>
        /// <para>
        /// This delegate for the callback prevents the Garbage Collector from releasing the 
        /// unmanaged function pointer from being destroyed
        /// </para>
        /// </remarks>
        /// </summary>
        private KMErrorHandler KMErrorHandler;
 
        /// <summary>
        /// All general messages are pumped out from the internal callback handler thru this event
        /// </summary>
        public event KMConsoleHandler MessageReceived;

        /// <summary>
        /// Error Messages are routed through this event
        /// </summary>
        public event KMErrorHandler ErrorReceived;

        /// <summary>
        /// Coordinated Motion Component
        /// </summary>
        private KM_CoordMotion _CoordMotion = null;

        /// <summary>
        /// Flags whether to obtain a lock token while gathering the status data
        /// </summary>
        private bool _LockStatusOnUpdate = true;

        /// <summary>
        /// Maximum time in ms between refreshing the status
        /// </summary>
        private int _StatusUpdateInterval = 100;
        /// <summary>
        /// Locally maintained Main_Status struct
        /// </summary>
        private KM_MainStatus _CurrentStatus;
        #endregion 

        #region Properties

        /// <summary>
        /// KMotion_DLL class instance pointer
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
        /// If this flag is true, exceptions will be generated from the ErrorMsg handler
        /// </summary>
        public bool ThrowExceptions
        {
            get
            {
                return _ThrowExceptions;
            }
            set
            {
                _ThrowExceptions = value;
            }
        }
        /// <summary>
        /// Board USB ID
        /// </summary>
        public int BoardID
        {
            get
            {
                return _BoardID;
            }
            set
            {
                _BoardID = value;
            }
        }
        /// <summary>
        /// Board number (for use in multiple board scenarios)
        /// </summary>
        public int BoardNumber
        {
            get
            {
                return _BoardNumber;
            }
            set
            {
                _BoardNumber = value;
                UpdateBoardID();
            }
        } 
        /// <summary>
        /// Dynomotion board device type
        /// </summary>
        public int BoardType
        {
            get
            {
                return _BoardType;
            }
            set
            {
                _BoardType = value;
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
            private set
            {
                _Loaded = value;
            }
        }
        /// <summary>
        /// Contains all messages until cleared
        /// </summary>
        public string MessageLog
        {
            get
            {
                return _MessageLog;
            }
            set
            {
                _MessageLog = value;
            }
        }
        /// <summary>
        /// Contains all error messages until cleared
        /// </summary>
        public string ErrorLog
        {
            get
            {
                return _ErrorLog;
            }
            set
            {
                _ErrorLog = value;
            }
        }
        /// <summary>
        /// Coordinated Motion Component
        /// </summary>
        public KM_CoordMotion CoordMotion
        {
            get
            {
                if (_CoordMotion == null)
                {
                    _CoordMotion = new KM_CoordMotion(this);

                    //Initialize....
                }
            
                return _CoordMotion;
            }
            set
            {
                _CoordMotion = value;
            }
        }

        /// <summary>
        /// Flags whether to obtain a lock token while gathering the status data
        /// </summary>
        public bool LockStatusOnUpdate
        {
            get
            {
                return _LockStatusOnUpdate;
            }
            set
            {
                _LockStatusOnUpdate = value;
            }
        }

        /// <summary>
        /// Maximum time in ms between refreshing the status
        /// </summary>
        public int StatusUpdateInterval
        {
            get
            {
                return _StatusUpdateInterval;
            }
            set
            {
                _StatusUpdateInterval = value;
            }
        }
        /// <summary>
        /// Locally maintained Main_Status struct
        /// </summary>
        public KM_MainStatus CurrentStatus
        {
            get
            {
                int duration = Math.Abs(Environment.TickCount - _CurrentStatus.TicksAtUpdate); 

                if (duration > _StatusUpdateInterval)
                {
                    _CurrentStatus = GetStatus(_LockStatusOnUpdate);
                }
                return _CurrentStatus;
            }
        }
        #endregion

        #region Ctor
        /// <summary>
        /// Default constructor for the KM_Controller object
        /// 
        /// Use this for single board applications
        /// </summary>
        public KM_Controller()
        {
            try
            { 
                _BoardNumber = 0;
                KM_dotnet_Interop_New(ref _InstanceHandle, _BoardNumber);
                _BoardType = GetBoardType();

                int boardcount = -1;
                var boards = GetBoards(out boardcount);
               
                _BoardID = boards[0];

                _CoordMotion = new KM_CoordMotion(this); 

                SetErrorHandler();
                SetCallbackHandler();
// don't think we want to necessarily do all this communication on creation
//                _CurrentStatus = GetStatus(false);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "KM_Controller"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "KM_Controller"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "KM_Controller"));
            } 
        }

        /// <summary>
        /// Multiple board constructor for attaching this object to a specific board address
        /// </summary>
        /// <param name="boardnumber">device ID</param>
        public KM_Controller(int boardnumber)
        {
            try
            { 
                KM_dotnet_Interop_New(ref _InstanceHandle, boardnumber);
                _BoardType = GetBoardType();
                int boardcount = -1;
                var boards = GetBoards(out boardcount);
                if (boardcount > 0)
                {
                    if (boardnumber < boardcount)
                    {
                        _BoardID = boards[boardnumber];
                        _BoardNumber = boardnumber;
                    }
                    else
                    {
                        _BoardID = boards[0];
                        _BoardNumber = 0;
                    }
                    if (boardcount == 1)
                    {
                        _BoardID = 0;
                    }
                    _CoordMotion = new KM_CoordMotion(this); 
                } 
                else
                {
                    throw new Exception("No KMotion Devices Found!!");
                }

                SetErrorHandler();
                SetCallbackHandler();
                _CurrentStatus = GetStatus(false);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "KM_Controller"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "KM_Controller"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "KM_Controller"));
            }
        } 
        #endregion

        #region Scripting Command Communications
        /// <summary>
        /// Sends a console command to the board and waits for the return value
        /// </summary>
        /// <param name="command">script command to send</param>
        /// <returns>return value (if any)</returns>
        public string WriteLineReadLine(string command)
        { 
            string response = "";
            try
            {
                MarshalPre(ref response);
                KM_dotnet_Interop_WriteLineReadLine(_InstanceHandle, command, ref response);
                MarshalPost(ref response);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "WriteLineReadLine"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "WriteLineReadLine"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "WriteLineReadLine"));
            }
            return response;
        }
        /// <summary>
        /// Sends a console command to the board
        /// </summary>
        /// <param name="command">script command to send</param>
        public void WriteLine(string command)
        { 
            try
            {
                KM_dotnet_Interop_WriteLine(_InstanceHandle, command);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "WriteLine"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "WriteLine"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "WriteLine"));
            }
        }
        /// <summary>
        /// Sends a console command to the board while echoing commands back to the callback handler
        /// </summary>
        /// <param name="command">script command to send</param>
        public void WriteLineWithEcho(string command)
        { 
            try
            {
                KM_dotnet_Interop_WriteLineWithEcho(_InstanceHandle, command);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "WriteLineWithEcho"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "WriteLineWithEcho"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "WriteLineWithEcho"));
            }
        }
        /// <summary>
        /// Block the calling thread while waiting for the response to complete within a specified timeout
        /// </summary>
        /// <param name="message">return message value</param>
        /// <param name="timeout_ms">time in ms to wait for message return to complete</param>
        /// <returns>true is completed within the timeout</returns>
        public bool ReadLineTimeout(ref string message, int timeout_ms)
        { 
            int success = -1;
            try
            {
                MarshalPre(ref message);
                success = KM_dotnet_Interop_ReadLineTimeOut(_InstanceHandle, ref message, timeout_ms);
                MarshalPost(ref message);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ReadLineTimout"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ReadLineTimout"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ReadLineTimout"));
            }
            return success == 0;
        }
        /// <summary>
        /// Flushes the console buffer
        /// </summary>
        /// <returns>true if successful</returns>
        public bool ServiceConsole()
        {
            int success = -1;
            try
            { 
                success = KM_dotnet_Interop_ServiceConsole(_InstanceHandle); 
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ReadLineTimout"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ReadLineTimout"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ReadLineTimout"));
            }
            return success == 0;
        }
        /// <summary>
        /// Returns the value of a WriteLineReadLine(), converted to the requested type
        /// </summary>
        /// <typeparam name="T">System.Type to convert return value to</typeparam>
        /// <param name="command">Console script statement</param>
        /// <param name="throwexception">flag determine whether or not to throw an exception</param>
        /// <returns>the returned string value of the command converted to the requested type</returns>
        public T GetCommandValue<T>(string command, bool throwexception)
        {
            var value = WriteLineReadLine(command);

            try
            {
                return (T)Convert.ChangeType(value, typeof(T));
            }
            catch (Exception ex)
            {
                if (throwexception)
                {
                    throw new Exception(String.Format("Could not convert command return value ( {0} = {1} ) to requested type ( {2} )", value, typeof(T).ToString()), ex);
                }
                else
                {
                    return default(T);
                }
            }
        }
        #endregion

        #region Locking
        /// <summary>
        /// Prevents outside access to the board until ReleaseToken is commanded
        /// </summary>
        /// <param name="timeout">ms to wait</param>
        /// <returns>true if lock was successful</returns>
        public KMOTION_TOKEN WaitToken(int timeout)
        { 
            try
            {
                var result = KM_dotnet_Interop_WaitToken(_InstanceHandle, timeout);
                return (KMOTION_TOKEN)result;
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "WaitToken"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "WaitToken"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "WaitToken"));
            }
        }
        /// <summary>
        /// Prevents outside access to the board
        /// This function is similar to the WaitToken function, except that it returns immediately (instead of waiting) if the board is already locked
        /// </summary>
        public int KMotionLock()
        { 
            try
            {
                return KM_dotnet_Interop_KMotionLock(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "KMotionLock"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "KMotionLock"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "KMotionLock"));
            }
        }
        /// <summary>
        /// Recovers controller from a motion lock state
        /// </summary>
        public void RecoverKMotionLock()
        { 
            try
            {
                KM_dotnet_Interop_KMotionLockRecovery(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "KMotionLock"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ReleaseKMotionLock"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ReleaseKMotionLock"));
            }
        }
        /// <summary>
        /// Allow normal access to the board
        /// </summary>
        public void ReleaseToken()
        { 
            try
            {
                KM_dotnet_Interop_ReleaseToken(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "ReleaseToken"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "ReleaseToken"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "ReleaseToken"));
            }
        }

        #endregion

        #region Status
        /// <summary>
        /// Tries to obtain a lock, and if a lock is obtained can execute optional actions before releasing the lock token.
        /// </summary>
        /// <param name="actions">delegates to execute while the board is locked</param>
        /// <returns>true if the lock status was not KMOTION_TOKEN.KMOTION_NOT_CONNECTED</returns>
        public bool CheckConnected(params Action[] actions)
        {
            var result = WaitToken(100);
            var conected = result != KMOTION_TOKEN.KMOTION_NOT_CONNECTED;
            if (result == KMOTION_TOKEN.KMOTION_LOCKED)
            {
                foreach (var action in actions)
                {
                    action.Invoke();
                }
                ReleaseToken();
            }
            return conected;
        }
        /// <summary>
        /// Get Boards checks the list of connected board locations and returns the total board count
        /// </summary>
        /// <param name="numberofboards">returned total board count</param>
        /// <returns>array of USB locations for all boards</returns>
        public int[] GetBoards(out int numberofboards)
        {  
            try
            {
                int[] list = new int[256];
                numberofboards = 0;
                int boards = -1;
                KM_dotnet_Interop_ListLocations(_InstanceHandle, ref boards, ref  list[0]);
                numberofboards = (int)boards;
                return list;
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "GetBoards"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "GetBoards"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "GetBoards"));
            }
        }
        /// <summary>
        /// USB tree node location
        /// </summary>
        /// <returns>USB location address</returns>
        public int GetUSBLocation()
        { 
            try
            {
                var result = KM_dotnet_Interop_USBLocation(_InstanceHandle);
                return result;
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "GetUSBLocation"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "GetUSBLocation"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "GetUSBLocation"));
            } 
        }
        /// <summary>
        /// Places the controller in a failed state, prevents further commands from processing
        /// </summary>
        public void SetFailed()
        { 
            try
            {
                KM_dotnet_Interop_Failed(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "SetFailed"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "SetFailed"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "SetFailed"));
            }
        }
        /// <summary>
        /// Disconnect the application from the board
        /// </summary>
        public void Disconnect()
        { 
            try
            {
                KM_dotnet_Interop_Disconnect(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Disconnect"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Disconnect"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Disconnect"));
            }
        }
        /// <summary>
        /// Call this to check if a "Ready" response came back
        /// after a command was manually typed in and we don't
        /// know what the response will be if any
        /// 
        /// Note:There is a possibility that other DSP threads are
        /// printing to the console screen
        ///
        /// First check if any input is available
        /// if not, return instantly (KMOTION_OK)
        /// if so, then a whole line might not be available
        /// but should be in transmission and be in very soon,
        /// so wait up to a short while to get in an entire line
        /// if the line doesn't come in for some reason return (KMOTION_TIMEOUT)
        /// check if the line should go to the console (esc)
        /// if so print it and repeat the above as long as 
        /// there is still input available
        /// if the line is not console data, check for "Ready",
        /// if it is "Ready" send it to the console and return KMOTION_READY;
        /// otherwise send it to the console        /// </summary>
        /// <returns>true if board has received "Ready"</returns>
        public bool CheckIsReady()
        { 
            int ready = -1;
            try
            {
                ready = KM_dotnet_Interop_CheckForReady(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "CheckIsReady"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "CheckIsReady"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "CheckIsReady"));
            }
            return ready == 0;
        }
        /// <summary>
        /// Query device for the loaded firmware version
        /// </summary>
        /// <returns>Firmware version</returns>
        public int GetFirmwareVersion()
        { 
            try
            {
                var result = KM_dotnet_Interop_GetFirmwareVersion(_InstanceHandle);
                return result;
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "GetFirmwareVersion"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "GetFirmwareVersion"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "GetFirmwareVersion"));
            } 
        }

        /// <summary>
        /// Determines and returns the section sizes of various sections in a KFLOP coff (common object file format) file
        /// </summary>
        /// <param name="name">file name of KFLOP coff</param>
        /// <param name="size_text">variable to return the text (code) size in bytes</param>
        /// <param name="size_bss">variable to return the bss (global variables) size in bytes</param>
        /// <param name="size_data">variable to return the data (global constants) size in bytes</param>
        /// <param name="size_total">variable to return the total size of all 3 sections in bytes</param>
        /// <returns>integer 0 if sucessful</returns>
        public int CheckCoffSize(string name, out int size_text, out int size_bss, out int size_data, out int size_total)
        {
            try
            {
                int text = 0;
                int bss = 0;
                int data = 0;
                int total = 0;

                var result = KM_dotnet_Interop_CheckCoffSize(_InstanceHandle, name, ref text, ref bss, ref data, ref total);
                size_text = text;
                size_bss = bss;
                size_data = data;
                size_total = total;
                return result;
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown : Caller - [{0}] :: Member - [{1}]",
                this.ToString(), "CheckCoffSize"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown : Caller - [{0}] :: Member - [{1}]",
                this.ToString(), "CheckCoffSize"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown : Caller - [{0}] :: Member - [{1}]",
                this.ToString(), "CheckCoffSize"));
            }
        }
        /// <summary>
        /// Gets the board type
        /// 1 = KMotion
        /// 2 = KFlop
        /// </summary>
        /// <returns>Board Type we are talking to</returns>
        public int GetBoardType()
        { 
            try
            {
                int result = -1;
                KM_dotnet_Interop_CheckKMotionVersion(_InstanceHandle, ref result, true);
                return result;
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "GetBoardType"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "GetBoardType"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "GetBoardType"));
            } 
        }
        /// <summary>
        /// Generates and returns an updated copy of the MAIN_STATUS structure
        /// </summary>
        /// <param name="locked">flag to determine whether to place a lock on the card while generating the MAIN_STATUS data</param>
        /// <returns>KM_MainStatus, which is member compatible with the unmanaged MAIN_STATUS struct</returns>
        public KM_MainStatus GetStatus(bool locked)
        {
            KM_MainStatus mainstatus = new KM_MainStatus();
            try
            {
                int versionandsize = 0;
                int[] adc = new int[24];
                int[] dac = new int[8];
                int[] pwm = new int[26];
                double[] position = new double[8];
                double[] destination = new double[8];
                int[] outputchan0 = new int[8];
                int inputmodes = 0;
                int inputmodes2 = 0;
                int outputmodes = 0;
                int outputmodes2 = 0;
                int enables = 0;
                int axisdone = 0;
                int[] bitsdirection = new int[2];
                int[] bitsstate = new int[2];
                int snapbitsdirection0 = 0;
                int snapbitsdirection1 = 0;
                int snapbitsstate0 = 0;
                int snapbitsstate1 = 0;
                int kanalgobitsstateinputs = 0;
                int kanalogbitsstateoutputs = 0;
                int runonstartup = 0;
                int threadactive = 0;
                int stopimmediatestate = 0;
                double timestamp = 0;
                int[] pccomm = new int[8];
                int virtualbits = 0;
                int virtualbitsex0 = 0;

                KM_dotnet_Interop_MainStatus_GetStatus(_InstanceHandle,
                    locked,
                    ref versionandsize,
                     adc,
                     dac,
                     pwm,
                     position,
                     destination,
                     outputchan0,
                    ref inputmodes,
                    ref inputmodes2,
                    ref outputmodes,
                    ref outputmodes2,
                    ref enables,
                    ref axisdone,
                     bitsdirection,
                     bitsstate,
                    ref snapbitsdirection0,
                    ref snapbitsdirection1,
                    ref snapbitsstate0,
                    ref snapbitsstate1,
                    ref kanalgobitsstateinputs,
                    ref kanalogbitsstateoutputs,
                    ref runonstartup,
                    ref threadactive,
                    ref stopimmediatestate,
                    ref timestamp,
                    pccomm,
                    ref virtualbits,
                    ref virtualbitsex0);

                mainstatus = KM_MainStatus.GetStatus(versionandsize,
                    adc,
                    dac,
                    pwm,
                    position,
                    destination,
                    outputchan0,
                    inputmodes,
                    inputmodes2,
                    outputmodes,
                    outputmodes2,
                    enables,
                    axisdone,
                    bitsdirection,
                    bitsstate,
                    snapbitsdirection0,
                    snapbitsdirection1,
                    snapbitsstate0,
                    snapbitsstate1,
                    kanalgobitsstateinputs,
                    kanalogbitsstateoutputs,
                    runonstartup,
                    threadactive,
                    stopimmediatestate,
                    timestamp,
                    pccomm,
                    virtualbits,
                    virtualbitsex0);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "GetStatus"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "GetStatus"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "GetStatus"));
            } 
            return mainstatus;
        }

        /// <summary>
        /// Checks whether a User Program Thread is currently executing.  
        /// </summary>
        /// <returns>Returns true if executing, false if not executing</returns>
        public bool ThreadExecuting(int thread)
        {
            var val = WriteLineReadLine(String.Format("CHECKTHREAD{0}", thread));
            int value = 0;

            if (Int32.TryParse(val, out value))
            {
                return value == 1;
            }
            else
            {
                return false;
            }
        }

        /// <summary>
        /// Waits for a User Program Thread to finish executing.  
        /// </summary>
        /// <returns>Returns true if successful, false if timed out</returns>
        public bool WaitForThreadComplete(int thread, int timeout)
        {
            try
            {
                Stopwatch sw = new Stopwatch(); 
                bool Executing = true; 
                bool istimedout = false;
                sw.Start();
                while (Executing)
                {
                    Executing = ThreadExecuting(thread);
                    if (!Executing)
                    {
                        break;
                    }
                    if (timeout > 0)//Determines whether or not to check for a timeout condition
                    {
                        if (timeout < sw.ElapsedMilliseconds)
                        {
                            istimedout = true;
                            break;
                        }
                    }
                    Thread.Sleep(1); //This should be optimized to make sure we do not clog USB traffic
                }
                return !istimedout;
            }
            catch (Exception ex)
            {
                throw new Exception(String.Format("Problem waiting for thread to complete [{0}]", thread), ex);
            }
        }
 
        #endregion

        #region Program Compiling and Loading
        /// <summary>
        /// Load a compiled C program into the board
        /// </summary>
        /// <param name="thread">thread address to load into(must match the thread the C program was compiled to)</param>
        /// <param name="programname">file name of the C program</param>
        public void LoadCoff(int thread, string programname)
        { 
            try
            {
                KM_dotnet_Interop_LoadCoff(_InstanceHandle, thread, programname);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "LoadCoff"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "LoadCoff"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "LoadCoff"));
            }
        }
        /// <summary>
        /// Load a compiled C program into the board (with PackToFlash arg)
        /// </summary>
        /// <param name="thread">thread address to load into(must match the thread the C program was compiled to)</param>
        /// <param name="programname">file name of the C program</param>
        /// <param name="PackToFlash">flag for packing in special format for bootup firmware</param>
        public int LoadCoff(int thread, string programname, int PackToFlash)
        {
            try
            {
                return KM_dotnet_Interop_LoadCoffPack(_InstanceHandle, thread, programname, PackToFlash);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "LoadCoffPack"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "LoadCoffPack"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "LoadCoffPack"));
            }
        }
        /// <summary>
        /// Compiles a source file and loads it into the board
        /// </summary>
        /// <param name="thread">thread address to load into</param>
        /// <param name="programname">file name of the C program</param>
        /// <param name="bypasserror">determines whether to collect any error returns</param>
        /// <returns>return message from the C program's execution, including any errors</returns>
        public string CompileAndLoadCoff(int thread, string programname, bool bypasserror)
        { 
            string error = "";
            try
            {
                if (!bypasserror)
                {
                    MarshalPre(ref error);
                    int result=KM_dotnet_Interop_CompileAndLoadCoff(_InstanceHandle, thread, programname, ref error, _ErrorLength);
                    if (result!=0 && error=="") error="Error Compiling and Loading Program";
                    MarshalPost(ref error);
                }
                else
                {
                    KM_dotnet_Interop_SimpleCompileAndLoadCoff(_InstanceHandle, thread, programname);
                }
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "CompileAndLoadCoff"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "CompileAndLoadCoff"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "CompileAndLoadCoff"));
            }
            return error;
        }
        /// <summary>
        /// Compiles a source file
        /// </summary>
        /// <param name="thread">thread address to load into</param>
        /// <param name="programname">file name of the C program</param>
        /// <returns>program output</returns>
        public string Compile(int thread, string programname)
        { 
            try
            {
                string result = "";
                string error = "";
                KM_dotnet_Interop_Compile(_InstanceHandle, _BoardType, thread, programname, ref result, ref error, _ErrorLength);
                _ErrorLog += error;
                return result;
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Compile"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Compile"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Compile"));
            }
        }

        /// <summary>
        /// Executes a previously loaded program
        /// </summary>
        /// <param name="thread">program thread in the kflop to run</param>
        public void ExecuteProgram(int thread)
        { 
            WriteLine(String.Format("Execute{0}", thread));  
        }

        /// <summary>
        /// Loads and executes a program from file
        /// </summary>
        /// <param name="thread">program thread in the kflop to run</param>
        /// <param name="programname">complete path to th .c file to execute</param>
        /// <param name="bypasserror">determines whether to collect any error returns</param>
        /// <returns>compile data</returns>
        public string ExecuteProgram(int thread, string programname, bool bypasserror)
        {
            var retval = CompileAndLoadCoff(thread, programname, bypasserror);
            if (retval == "") WriteLine(String.Format("Execute{0}", thread)); 
            return retval;
        }

        /// <summary>
        /// Halts the execution of each supplied thread
        /// </summary>
        /// <param name="threads">threads to kill</param>
        public void KillProgramThreads(params int[] threads)
        {
            if (threads != null)
            {
                for (int i = 0; i < threads.Length; i++)
                {
                    if (threads[i] < 8 && threads[i] > 0)
                    {
                        WriteLine(String.Format("Kill{0}", threads[i]));
                        Thread.Sleep(0);
                    }
                }
            }
        }
        #endregion

        #region UserData
        /// <summary>
        /// Sets a 32 bit word of integer user data
        /// </summary>
        /// <param name="index">array offset (0-199)</param>
        /// <param name="data">32bits of data to store</param>
        public void SetUserData(int index, int data)
        {
            WriteLine(String.Format("SetPersistDec{0} {1}", index, data));
        }

        /// <summary>
        /// Sets a 32 bit word with a 32-bit float of user data
        /// 
        /// To get the value in KFLOP in C do:
        /// float f = *(float *)&amp;persist.UserData[index];
        /// </summary>
        /// <param name="index">array offset (0-199)</param>
        /// <param name="fdata">32bits float data to store</param>
        public void SetUserDataFloat(int index, float fdata)
        {
            Byte[] bytes = BitConverter.GetBytes(fdata);
            int data = BitConverter.ToInt32(bytes, 0);

            WriteLine(String.Format("SetPersistDec{0} {1}", index, data));
        }

        /// <summary>
        /// Sets a pair of two 32 bit words with a 64-bit double of user data
        /// 
        /// To get the value in KFLOP in C do:
        /// double f = *(double *)&amp;persist.UserData[index*2];
        /// </summary>
        /// <param name="index">array offset (0-99) even pair of 32-bit regs</param>
        /// <param name="ddata">64bit double data to store</param>
        public void SetUserDataDouble(int index, double ddata)
        {
            Byte[] bytes = BitConverter.GetBytes(ddata);
            int data0 = BitConverter.ToInt32(bytes, 0);
            int data1 = BitConverter.ToInt32(bytes, 4);

            WriteLine(String.Format("SetPersistDec{0} {1}", index*2,   data0));
            WriteLine(String.Format("SetPersistDec{0} {1}", index*2+1, data1));
        }

        /// <summary>
        /// Gets a 32 bit word of user data
        /// </summary>
        /// <param name="index">array offset (0-199)</param>
        /// <returns>Int32 data from the array</returns>
        public int GetUserData(int index)
        {
            int retval = 0;
            var val = WriteLineReadLine(String.Format("GetPersistDec {0}", index));

            Int32.TryParse(val, out retval);

            return retval;
        }

        /// <summary>
        /// Gets a 32 bit float word of user data
        ///
        /// To set float value in KFLOP in C do:
        ///  *(float *)&amp;persist.UserData[index] = f;
        /// </summary>
        /// <param name="index">array offset (0-199)</param>
        /// <returns>float data from the array</returns>
        public float GetUserDataFloat(int index)
        {
            int retval = 0;
            var val = WriteLineReadLine(String.Format("GetPersistDec {0}", index));

            Int32.TryParse(val, out retval);

            Byte[] bytes = BitConverter.GetBytes(retval);
            return BitConverter.ToSingle(bytes, 0);
        }


        /// <summary>
        /// Gets a 64 bit double from a KFLOP user data pair of 32 bit vars
        ///
        /// To set double value in KFLOP in C do:
        ///  *(double *)&amp;persist.UserData[index*2] = d;
        /// </summary>
        /// <param name="index">array offset (0-99)</param>
        /// <returns>double data from the array</returns>
        public double GetUserDataDouble(int index)
        {
            UInt32 ival0 = 0;
            UInt32 ival1 = 0;
            var val0 = WriteLineReadLine(String.Format("GetPersistDec {0}", index*2));
            var val1 = WriteLineReadLine(String.Format("GetPersistDec {0}", index*2+1));

            UInt32.TryParse(val0, out ival0);
            UInt32.TryParse(val1, out ival1);

            Byte[] bytes = BitConverter.GetBytes(((UInt64)ival1<<32) | (UInt64)ival0);
            return BitConverter.ToDouble(bytes, 0);
        }


        /// <summary>
        /// Sets all words of user data to 0
        /// </summary>
        public void ResetUserData()
        {
            for (int i = 0; i < 200; i++)
            {
                WriteLine(String.Format("SetPersistDec{0} {1}", i, 0));
                Thread.Sleep(10);
            }
        }
        #endregion

        #region Message and Error Handling
        /// <summary>
        /// Sets the internal c++ interop callback handler
        /// Separate delegate for this callback prevents the Garbage Collector from releasing the 
        /// unmanaged function pointer from being destroyed
        /// </summary>
        private void SetCallbackHandler()
        {
            try
            {
                KMCallBackhandler = new KMConsoleHandler(OnKMCallback);
                var returnval = KM_dotnet_Interop_SetConsoleCallback(_InstanceHandle, KMCallBackhandler);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "SetCallbackHandler"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "SetCallbackHandler"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "SetCallbackHandler"));
            }
        }

        private void SetErrorHandler()
        {
            try
            {
                KMErrorHandler = new KMErrorHandler(OnKMError);
                KM_dotnet_Interop_SetErrorCallback(_InstanceHandle, KMErrorHandler);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "SetCallbackHandler"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "SetCallbackHandler"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "SetCallbackHandler"));
            }
        }
        /// <summary>
        /// Used to accept messages back from KM device
        /// </summary>
        /// <param name="message">message from KM device</param>
        /// <returns>0 with no errors</returns>
        private int OnKMCallback(string message)
        {
            OnMessage(message);
            return 0;
        }
        private void OnKMError(string message)
        {
            OnError(message);
        }
        /// <summary>
        /// Routes messages to event subscribers
        /// </summary>
        /// <param name="message">message receieved</param>
        private void OnMessage(string message)
        {
            if (message.Trim() != "")
            {
                var temp = this.MessageReceived;
                if (temp != null)
                {
                    temp(message);
                }
            }
        }

        /// <summary>
        /// Routes error messages to event subscribers
        /// </summary>
        /// <param name="error">error receieved</param>
        private void OnError(string error)
        {
            if (error.Trim() != "")
            {
                var temp = this.ErrorReceived;
                if (temp != null)
                {
                    temp(error);
                }
            }
            if (_ThrowExceptions)
            {
                throw new Exception(error);
            }
        }
        /// <summary>
        /// Checks if any messages are in qeue
        /// </summary>
        /// <returns>true if Message log ! = ""</returns>
        public bool HasMessages()
        {
            return _MessageLog.Length > 0;
        }
        /// <summary>
        /// Checks if any error messages are in qeue
        /// </summary>
        /// <returns>true if Error log ! = ""</returns>
        public bool HasErrors()
        {
            return _ErrorLog.Length > 0;
        }
        /// <summary>
        /// Clears the Message log
        /// </summary>
        public void ClearMessages()
        {
            _MessageLog = "";
        }
        /// <summary>
        /// Clears the Error log
        /// </summary>
        public void ClearError()
        {
            _ErrorLog = "";
        }

        /// <summary>
        /// Routes Error callback messages and generates specific exceptions based on the source
        /// </summary>
        /// <param name="message">string message from ErrorMsg callback</param>
        private void GenerateException(string message)
        {
            switch (message)
            {
                case "Unable to open KMotion device":
                    throw new DM_USB_Exception("Unable to open KMotion device");

                case "KMotion Received Line too long":
                    throw new DM_USB_Exception("KMotion Received Line too long");

                case "Unable to set USB Event Character":
                    throw new DM_USB_Exception("Unable to set USB Event Character");

                case "Unable to set USB Latency timer":
                    throw new DM_USB_Exception("Unable to set USB Latency timer");

                case "KMotion present but not responding\r\rCorrect problem and restart application":
                    throw new DM_USB_Exception("KMotion present but not responding\r\rCorrect problem and restart application");

                case "Read Failed - Auto Disconnect":
                    throw new DM_Disconnected_Exception("Read Failed - Auto Disconnect");

                case "Unable to Connect to KMotion Server":
                    throw new DM_Disconnected_Exception("Read Failed - Auto Disconnect");

                case "Unable to execute:\r\rKMotionServer.exe\r\rTry re-installing software or copy this file to the same location as KMotion.exe":
                    throw new DM_Firmware_Exception("Unable to execute:\r\rKMotionServer.exe\r\rTry re-installing software or copy this file to the same location as KMotion.exe");

                case "Error Locating TCC67.exe Compiler":
                    throw new DM_Compiler_Exception("Error Locating TCC67.exe Compiler");

                default:
                    break;
            }
        }
        #endregion

        #region IDisposable Members
        /// <summary>
        /// Releases object resources
        /// </summary>
        public void Dispose()
        {
            Disconnect();
            try
            { 
                this.CoordMotion.Dispose();
                KM_dotnet_Interop_Free(ref _InstanceHandle);

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

        #region Util
        /// <summary>
        /// formats a message begining for callback
        /// </summary>
        /// <param name="s">message</param>
        private void MarshalPre(ref string s)
        {
            s = new string(' ', 255);
        }
        /// <summary>
        /// formats a message end for callback
        /// </summary>
        /// <param name="s">message</param>
        private void MarshalPost(ref string s)
        {
            if (s != null)
            {
                if (s.Contains((char)0))
                {
                    int nullstring = s.LastIndexOf((char)0);
                    s = s.Substring(0, nullstring - 1);
                }
            }
        }
        /// <summary>
        /// Assigns the board ID
        /// 
        /// This is called when the BoardID property is changed
        /// </summary>
        private void UpdateBoardID()
        {
            int boardcount = -1;
            var boards = GetBoards(out boardcount);
            if (_BoardNumber < boardcount)
            {
                _BoardID = boards[_BoardNumber]; 
            }
            else
            {
                _BoardID = boards[0];
                _BoardNumber = 0;
            }
        }

        #endregion

        #region Components
        /// <summary>
        /// Creates a new KM_IO bit
        /// </summary>
        /// <param name="bit">nit index</param>
        /// <param name="type">IO type for bit</param>
        /// <param name="name">Name of bit</param>
        /// <returns></returns>
        public KM_IO GetIO(int bit, IO_TYPE type, string name)
        {
            return new KM_IO(this, bit, name, type);
        }

        /// <summary>
        /// Creates a KM_Axis object
        /// </summary>
        /// <param name="channel">KFLOP Axis Channel</param>
        /// <param name="name">descriptive name</param>
        /// <returns></returns>
        public KM_Axis GetAxis(int channel, string name)
        {
            return new KM_Axis(this, channel, name);
        }
        /// <summary>
        /// Creates a KM_AxisGroup
        /// </summary>
        /// <param name="id">integer id</param>
        /// <param name="name">descriptive name</param>
        /// <param name="axislist">List of KM_Axis to place in the group</param>
        /// <returns>Created KM_AxisGroup</returns>
        public KM_AxisGroup GetAxisGroup(int id, string name, params KM_Axis[] axislist)
        {
            var group = new KM_AxisGroup(this, axislist);
            group.Name = name;
            group.ID = id;

            return group;
        }
        #endregion

    }
}
