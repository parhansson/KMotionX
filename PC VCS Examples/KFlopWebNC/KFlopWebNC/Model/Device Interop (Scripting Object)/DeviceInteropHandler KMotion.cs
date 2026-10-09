using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using KMotion_dotNet;
using System.Windows.Forms;
using System.Reflection;
using System.IO;
using KFlopWebNC.DOM_Helpers;

namespace KFlopWebNC.Model
{ 
    public partial class DeviceInteropHandler
    {
        /// <summary>
        /// Flag that is updated my the monitor thread as to whether or not a valid connection exisits to the board
        /// </summary>
        public bool IsConnected { get; set; }
        
        /// <summary>
        /// Updates the HTML element designated to display the kflop version 
        /// </summary>
        public void UpdateBoadVersion()
        {
            var version = "Not Connected";

            version = _Controller.WriteLineReadLine("Version");
            _GuiHost.SetElementText(Elements.VersionDisplay, version);
        }

        /// <summary>
        /// Updates the HTML element designated to display the connection status 
        /// </summary>
        public void UpdateConnectionStatus()
        {
            if (IsConnected)
            {

                _GuiHost.SetElementText(Elements.IsConnected, "Connected");
                UpdateBoadVersion();
            }
            else
            {
                _GuiHost.SetElementText(Elements.IsConnected, "Not Connected");
                _GuiHost.SetElementText(Elements.VersionDisplay, "waiting for connection...");

            }
        }
    }
}
