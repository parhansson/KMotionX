using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ExtendedLoggingKflop.Shared
{
    public class MessageFormat
    {
        public MessageFormat(string logFormat, int logId)
        {
            if (logId > 15)
            {
                throw new Exception("Invalid LogId, must be between 0 and 15");
            }
            LogID = logId;
            LogFormat = logFormat;
            LogIdentifier = string.Format("@{0}", logId.ToString("X"));
            FullLogFormat = LogIdentifier + "," + LogFormat;
            //PrintfLogFormat = string.Format("printf(\"{0}\\n\")", FullLogFormat);

            LogDelimitCount = FullLogFormat.Split(',').Count();
            StringBuilder sb = new StringBuilder("printf(\"{0}\\n\",time");

            for (int i = 0; i < LogDelimitCount - 2; i++)//-2 for LogIdentifier and time
            {
                string s = string.Format(",param{0:D}", i + 1);
                sb.Append(s);
            }
            sb.Append(")");

            PrintfLogFormat = string.Format(sb.ToString(), FullLogFormat);
            if (LogDelimitCount < 3)//3 for logidentifier, time, somethingelse
            {
                //throw new Exception("Log format is invalid, must be at least 2 values (time,somethingelse)");
                IsValid = false;
            }
            else
            {
                IsValid = true;
            }

        }
        /// <summary>
        /// The format of the log data after the LogIdentifier 
        /// example "%f,%f,%f,%f" would represent a printf statement
        /// on the kflop of printf("LogIdentifier,%f,%f,%f,%f\n", time,data1,data2,data3);
        /// Where LogIdentifier is the '@' sign followed by a single hex
        /// digit to uniquely identify which plot this data is 
        /// for.  Single hex digit means max of 16 unique logging
        /// formats. Mupltiple plots can use the same format.
        /// </summary>
        public string LogFormat { get; private set; }
        /// <summary>
        /// LogIdentifier is the '@' sign followed by a single hex
        /// digit to uniquely identify which plot this data is 
        /// for. Example "@F
        /// </summary>
        public string LogIdentifier { get; private set; }
        /// <summary>
        /// Log ID, the single hex digit following the '@' converted
        /// to an int.
        /// </summary>
        public int LogID { get; private set; }
        /// <summary>
        /// Full log message format, LogIdentifier + LogFormat
        /// </summary>
        public string FullLogFormat { get; private set; }
        public string PrintfLogFormat { get; private set; }
        /// <summary>
        /// the length of a delimited array for this log message 
        /// including the LogIdentifier
        /// </summary>
        public int LogDelimitCount { get; private set; }
        public bool IsValid { get; private set; }
    }
}
