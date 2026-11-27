using Anotar.Catel;
using KMotion_dotNet;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace ExtendedLoggingKflop.Services
{
    class LoggingService : ILoggingService
    {
        private IKmotionService _kMotionService;       
        //private string _format;
        //private string[] _delimitedFormat;
        private bool _started = false;
 //       event EventHandler OnDataLoggedEvent;
        private object objectLock = new Object();
        private Task _msgTask;
        private CancellationTokenSource _cancelSource;
        private int _msgCount;
        private int _msgByteCount;
        private bool _pause = false;
        private List<MessageQueue> _msgQueues;

        public LoggingService (IKmotionService kserv)
        {
            _kMotionService = kserv;
            _msgQueues = new List<MessageQueue>();           
        }

        //public string LogFormat
        //{
        //    get
        //    {
        //        return _format;
        //    }
        //    set
        //    {
        //        _format = value;
        //        _delimitedFormat = null;
        //        if (!string.IsNullOrEmpty(LogFormat))
        //        {
        //            _delimitedFormat = _format.Split(',');
        //        }
        //    }
        //}
        //event EventHandler ILoggingService.OnDataLogged
        //{
        //    add
        //    {
        //        lock (objectLock)
        //        {
        //            OnDataLoggedEvent += value;
        //        }
        //    }
        //    remove
        //    {
        //        lock (objectLock)
        //        {
        //            OnDataLoggedEvent -= value;
        //        }
        //    }
        //}
        public bool Pause
        {
            get
            {
                return _pause;
            }
            set
            {
                lock (objectLock)
                {
                    _pause = value;
                    MessageCount = 0;
                    MessageByteCount = 0;
                    if (!_pause)
                    {
                        //start server if it isn't already started
                        Start();
                    }
                }
            }
        }

        public int MessageCount
        {
            get
            {
                return _msgCount;
            }
            private set
            {
                lock (objectLock)
                {
                    _msgCount = value;
                }
            }
        }
        public int MessageByteCount
        {
            get
            {
                return _msgByteCount;
            }
            private set
            {
                lock (objectLock)
                {
                    _msgByteCount = value;
                }
            }
        }
        public void RegisterQueue(MessageQueue queue)
        {
            lock (objectLock)
            {
                _msgQueues.Add(queue);
            }
        }
        public void UnRegisterQueue(MessageQueue queue)
        {
            lock (objectLock)
            {
                _msgQueues.Remove(queue);
            }
        }
        

        
        /// <summary>
        /// This returns a delimited string so that we only do the string.Split once per log message
        /// </summary>
        /// <param name="message"></param>
        /// <returns></returns>
        private string[] IsLogMessage (string message)
        {
            string[] delimitedstr = message.Split(',');
            if (delimitedstr.Length < 2) return null;
            if (delimitedstr[0].StartsWith("@")) return delimitedstr;
            return null;
        }
        
        /// Receives Console Message callbacks from Kmotion.DLL funtions
        private int ConsoleMessageHandler(string message)
        {
            lock (objectLock)
            {
                if (!Pause)
                {
                    string[] delmsg = IsLogMessage(message);
                    if (delmsg != null)
                    {
#if DEBUGLOGMESSAGES
                    LogTo.Info("Kflop Message: {0}", message);
#endif
                        //lock (objectLock)
                        {
                            IEnumerable<MessageQueue> matchedQueues = _msgQueues.Where(x => x.Format.LogIdentifier == delmsg[0]);
                            foreach (MessageQueue msgQueue in matchedQueues)
                            {
                                msgQueue.Queue.Add(delmsg);
                                MessageCount++;
                                MessageByteCount += message.Length;
                            }
                        }
                    }
                    else
                    {
                        LogTo.Info("Kflop Message: {0}", message);                                                
                    }

                }
                else
                {
                    string[] delmsg = IsLogMessage(message);
                    if (delmsg != null)
                    {
                        //not a log message, send it console
                        LogTo.Info("Kflop Message: {0}", message);
                    }
                    else
                    {
                        //ignore message that would be plotted if we weren't paused
#if DEBUGLOGMESSAGES
                    LogTo.Info("Logging paused, Kflop Message: {0}", message);
#endif
                    }
                }
            }
            return 1;
        }
        private void ConsoleErrorHandler(string message)
        {
            LogTo.Error("Kflop Error: {0}", message);           
        }
        private void UnRegisterAllListeners()
        {
            lock (objectLock)
            {
                foreach (MessageQueue bc in _msgQueues)
                {
                    bc.Queue.CompleteAdding();
                }
                _msgQueues.Clear();
            }
        }
        async Task MsgTask()
        {
            LogTo.Info("Enter LoggingService.MsgTask");
            //Tell kmotion to service the console every 10ms
            while (!_cancelSource.IsCancellationRequested)
            {
                bool havelock = false;
                try
                {
                    try
                    {
                        //if ((KMOTION_TOKEN)_kMotionService.KMController.KMotionLock() == KMOTION_TOKEN.KMOTION_LOCKED)
                        {
                            havelock = true;
                            _kMotionService.KMController.ServiceConsole();
                        }
                        //else
                        //we didn't get a lock, something else must have been communicating with kmotion/kflop
                        //just wait until next time through and try again.
                    }
                    finally
                    {
                        if (havelock)
                        {
                            //_kMotionService.KMController.ReleaseToken();
                        }
                    }
                    await Task.Delay(10);
                }
                catch (OperationCanceledException)
                {
                    break;
                }
            }
            LogTo.Info("Exit LoggingService.MsgTask");
        }
        public bool Start()
        {
            lock (objectLock)
            {
                //Pause = false;
                if (_started)
                {
                    return true;
                }
                LogTo.Info("Starting logging service...");
                //LogTo.Info("Grabbing KMotion context...");
                LogTo.Info("Registering Console callback.");
                _cancelSource = new CancellationTokenSource();
                _kMotionService.KMController.ClearMessages();
                _kMotionService.KMController.MessageReceived += new KMConsoleHandler(ConsoleMessageHandler);
                _kMotionService.KMController.ErrorReceived += new KMErrorHandler(ConsoleErrorHandler);
                Task<Task> task = Task.Factory.StartNew(
                    function: MsgTask,
                    cancellationToken: _cancelSource.Token,
                    creationOptions: TaskCreationOptions.LongRunning,
                    scheduler: TaskScheduler.Default);

                _msgTask = task.Unwrap();
                _started = true;
                LogTo.Info("Started logging service.");
            }
            return true;
        }

        public bool Stop()
        {
            lock (objectLock)
            {
                if (!_started) return true;
                LogTo.Info("Stopping logging service...");
                Pause = true;
                if (_cancelSource != null)
                    _cancelSource.Cancel();
                _msgTask.Wait();
                
                _kMotionService.KMController.MessageReceived -= ConsoleMessageHandler;
                _kMotionService.KMController.ErrorReceived -= ConsoleErrorHandler;
                //_data.CompleteAdding();  
                UnRegisterAllListeners();
                
                _started = false;
                LogTo.Info("Stopped logging service.");
            }
            return true;
        }
    }
}
