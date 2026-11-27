using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ExtendedLoggingKflop.Services
{
    using Shared;
    public class MessageQueue
    {

        /// <summary>
        /// Holds a BlockingCollection to use as a message queue and 
        /// the logging format for that queue.  Mupltiple plots can use the
        /// same format.  But each plot has their own queue.
        /// </summary>
        /// <param name="logFormat"></param>
        /// <param name="logId"></param>
        public MessageQueue(string logFormat, int logId)
        {
            Format = new MessageFormat(logFormat, logId);
            Queue = new BlockingCollection<string[]>();
        }
        public MessageFormat Format {get;set;}
        /// <summary>
        /// The blocking queue to store the messages in.
        /// </summary>
        public BlockingCollection<string[]> Queue { get; set; }
        
    }
    public interface ILoggingService
    {
        bool Start();
        bool Stop();
        bool Pause { get; set; }
       // string LogFormat { get; set; }
        int MessageCount { get;}
        int MessageByteCount { get; }
        void RegisterQueue(MessageQueue queue);
        void UnRegisterQueue(MessageQueue queue);
        //BlockingCollection<string> Data { get; }

//        event EventHandler OnDataLogged;
    }
}
