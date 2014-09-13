using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using KMotion_dotNet;
using System.Windows.Forms;
using System.Reflection;
using System.IO;

namespace KFlopWebNC.Model
{
    [System.Runtime.InteropServices.ComVisibleAttribute(true)]
    public partial class DeviceInteropHandler
    {
        private Dictionary<string, AxisNode> _AxisContainer = new Dictionary<string, AxisNode>();
        private Dictionary<string, KM_IO> _IOContainer = new Dictionary<string, KM_IO>();
        private Dictionary<string, string> _SystemPaths = new Dictionary<string, string>(); 
        private KM_Controller _Controller;
        private KM_MainStatus _Status;
        private ZoomBrowser.ZoomBrowser _GuiHost;

        public bool Loaded { get; set; }
        public Dictionary<string, AxisNode> AxisContainer
        {
            get
            {
                return _AxisContainer;
            }
            set
            {
                _AxisContainer = value;
            }
        }
        public Dictionary<string, KM_IO> IOContainer
        {
            get
            {
                return _IOContainer;
            }
            set
            {
                _IOContainer = value;
            }
        }
        public Dictionary<string, string> SystemPaths
        {
            get
            {
                return _SystemPaths;
            }
            set
            {
                _SystemPaths = value;
            }
        }
 

        public KM_Controller Controller
        {
            get
            {
                return _Controller;
            }
            set
            {
                _Controller = value;
            }
        }
        public ZoomBrowser.ZoomBrowser GuiHost
        {
            get
            {
                return _GuiHost;
            }
            set
            {
                _GuiHost = value;
            }
        }

        public DeviceInteropHandler(ZoomBrowser.ZoomBrowser browser)
        {
            try
            {
                ShowPosition = false;
                ShowMachineCoords = false;
                IsConnected = false;
                Loaded = false;
                _GuiHost = browser;
                _GuiHost.AllowWebBrowserDrop = false;
                //_GuiHost.IsWebBrowserContextMenuEnabled = false;
                //_GuiHost.WebBrowserShortcutsEnabled = false;
                _GuiHost.ObjectForScripting = this;
                _GuiHost.Navigating += new WebBrowserNavigatingEventHandler(_GuiHost_Navigating);
                _GuiHost.Navigated += new WebBrowserNavigatedEventHandler(_GuiHost_Navigated);
                _GuiHost.DocumentTitleChanged += new EventHandler(_GuiHost_DocumentTitleChanged);
                _GuiHost.DocumentCompleted += new WebBrowserDocumentCompletedEventHandler(_GuiHost_DocumentCompleted);
                ValidateFolders();
                InitDevice();
                LoadUI();

                StartMonitor();
            }
            catch (Exception ex)
            {
                _GuiHost.DocumentText = ex.Message;
            }
        }




        private void InitDevice()
        {
            _Controller = new KM_Controller();

            _Controller.MessageReceived += new KMConsoleHandler(_Controller_MessageUpdated);
            _Controller.ErrorReceived += new KMErrorHandler(_Controller_ErrorReceived);
            _Controller.CoordMotion.CoordMotionArcFeed += new KM_CoordMotionArcFeedHandler(Interpreter_Interpreter_CoordMotionArcFeed);
            _Controller.CoordMotion.CoordMotionStraightFeed += new KM_CoordMotionStraightFeedHandler(Interpreter_Interpreter_CoordMotionStraightFeed);
            _Controller.CoordMotion.CoordMotionStraightTraverse += new KM_CoordMotionStraightTraverseHandler(Interpreter_Interpreter_CoordMotionStraightTraverse);
            _Controller.CoordMotion.Interpreter.InterpreterCompleted += new KM_Interpreter.KM_GCodeInterpreterCompleteHandler(Interpreter_InterpreterCompleted);
            _Controller.CoordMotion.Interpreter.InterpreterStatusUpdated += new KM_Interpreter.KM_GCodeInterpreterStatusHandler(Interpreter_InterpreterStatusUpdated);
            _Controller.CoordMotion.Interpreter.InterpreterUserCallbackRequested += new KM_Interpreter.KM_GCodeInterpreterUserCallbackHandler(Interpreter_InterpreterUserCallbackRequested);
            _Controller.CoordMotion.Interpreter.InterpreterUserMCodeCallbackRequested += new KM_Interpreter.KM_GCodeInterpreterUserMcodeCallbackHandler(Interpreter_InterpreterUserMCodeCallbackRequested);




            var datafile = Path.Combine(_SystemPaths["AppData"], "InterpreterData.xml");
            if (!File.Exists(datafile))
            {
                InterpreterData data = new InterpreterData();
                InterpreterData.SaveXML(datafile, data);
                InterpreterDataFile = data;
            }
            else
            {
                InterpreterDataFile = InterpreterData.LoadXML(datafile);
            }
        }



        private int _Controller_MessageUpdated(string message)
        {

            return message.Length;
        }
       private void _Controller_ErrorReceived(string message)
        {
           /// ...
        }
        private void LoadUI()
        {
            var file = Path.Combine(_SystemPaths["Site"], "index.html"); 
            if (File.Exists(file))
            { 
                _GuiHost.Navigate(file); 
            }
            else
            {
                StringBuilder index = new StringBuilder();
                index.AppendLine("<html>"); 
                index.AppendLine("\t<head><title>Kflop - Default Screen</title></head>"); 
                index.AppendLine("\t<body>");
                index.AppendLine("\t\t<button onclick=\"window.external.UpdateBoadVersion()\"> Get KFlop Version </button>");
                index.AppendLine("\t\t<p id=\"kflopversion\"></p>");
                index.AppendLine("\t</body>");
                index.AppendLine("</html>");
 
                _GuiHost.DocumentText = index.ToString(); 
            }
        }

        private void ValidateFolders()
        {
            _SystemPaths.Add("Root", Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location));
            _SystemPaths.Add("Site", Path.Combine(_SystemPaths["Root"], "Site"));
            _SystemPaths.Add("Content", Path.Combine(_SystemPaths["Site"], "Content"));
            _SystemPaths.Add("Themes", Path.Combine(_SystemPaths["Content"], "Themes"));
            _SystemPaths.Add("Images", Path.Combine(_SystemPaths["Content"], "Images"));
            _SystemPaths.Add("Scripts", Path.Combine(_SystemPaths["Site"], "Scripts"));
            _SystemPaths.Add("AppData", Path.Combine(_SystemPaths["Site"], "AppData"));
            foreach (var path in _SystemPaths.Values)
            {
                if (!Directory.Exists(path))
                {
                    Directory.CreateDirectory(path);
                }
            }

            _MDIFile = Path.Combine(_SystemPaths["Root"], "MDI.ngc");
            _GcodeFile = Path.Combine(_SystemPaths["Root"], "GCODE.ngc");

        }

        public void NavigateToPage(string pagename)
        { 
            Loaded = false;
            _GuiHost.Navigate(Path.Combine(_SystemPaths["Site"], pagename));
        } 




        private void _GuiHost_DocumentTitleChanged(object sender, EventArgs e)
        {
            var window = _GuiHost.Parent as Form;
            var browser = sender as WebBrowser;
            if (browser != null && window != null)
            {
                window.Text = "KMotion CNC.net    " + browser.Document.Title;
            }
        }

        private void _GuiHost_Navigated(object sender, WebBrowserNavigatedEventArgs e)
        {
           
        }

        private void _GuiHost_Navigating(object sender, WebBrowserNavigatingEventArgs e)
        {
            Loaded = false;
        }

        private void _GuiHost_DocumentCompleted(object sender, WebBrowserDocumentCompletedEventArgs e)
        {
            Loaded = true;
        }

        public void UpdateDocument(Action action)
        {
            if (_GuiHost.Parent.InvokeRequired)
            {
                _GuiHost.Parent.Invoke(action);
            }
            else
            {
                action();
            }
        }
    }
}
