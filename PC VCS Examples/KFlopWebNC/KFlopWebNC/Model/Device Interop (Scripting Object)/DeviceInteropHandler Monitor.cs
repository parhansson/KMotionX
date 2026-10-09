using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using KMotion_dotNet;
using System.Windows.Forms;
using System.Reflection;
using System.IO;
using KFlopWebNC.DOM_Helpers;
using System.ComponentModel;
using System.Threading;

namespace KFlopWebNC.Model
{ 
    public partial class DeviceInteropHandler
    {
        BackgroundWorker _StatusMonitor;

        /// <summary>
        /// Starts the monitor thread
        /// </summary>
        public void StartMonitor()
        {
            if (_StatusMonitor == null)
            {
                _StatusMonitor = new BackgroundWorker();

                _StatusMonitor.WorkerSupportsCancellation = true;
                _StatusMonitor.DoWork += new DoWorkEventHandler(_StatusMonitor_DoWork);
                _StatusMonitor.RunWorkerAsync();
            }
        }

        /// <summary>
        /// Updates the status objects from the board
        /// </summary>
        /// <param name="sender">background worker's dispatch object</param>
        /// <param name="e">background worker arguments</param>
        public void _StatusMonitor_DoWork(object sender, DoWorkEventArgs e)
        {
            int interval = 100;
            int interval2 = 5;

            int secondary = interval2; 
            while (!_StatusMonitor.CancellationPending)
            {
                try
                {
                    var locked = _Controller.WaitToken(100);
                    if (locked == KMOTION_TOKEN.KMOTION_LOCKED)
                    {
                        _Controller.ServiceConsole();
                        _Status = _Controller.GetStatus(false);
                        _Controller.ReleaseToken();
                        IsConnected = true;
                        if (secondary < 1)
                        {
                            UpdateSecondaryControls();
                            secondary = interval2;
                        }
                        UpdateAxisData();
                    }
                    else if (locked == KMOTION_TOKEN.KMOTION_NOT_CONNECTED)
                    {
                        IsConnected = false;
                        CheckAllEnabled();
                    }
                    else
                    {
                        IsConnected = true;
                    }
                    Thread.Sleep(interval);
                    secondary--;
                }
                catch (Exception ex)
                {
                    throw new Exception(String.Format("StatusMonitor_DoWork"),ex);
                }
            }

            _StatusMonitor = null;
        }

        /// <summary>
        /// Stops the monitor thread
        /// </summary>
        public void StopMonitor()
        {
            _StatusMonitor.CancelAsync();
        }


        /// <summary>
        /// Updates the secondary controls that do not need to be updated as frequently
        /// </summary>
        public void UpdateSecondaryControls()
        {
            UpdateDocument(new Action(delegate()
            {
                UpdateConnectionStatus();
                UpdateFeedRate();
                UpdateJogRates();
           })); 
        }

        /// <summary>
        /// Updates the axis positions
        /// </summary>
        public void UpdateAxisData()
        { 

            double x = 0.0;
            double y = 0.0;
            double z = 0.0;
            double a = 0.0;
            double b = 0.0;
            double c = 0.0;

            double px = 0.0;
            double py = 0.0;
            double pz = 0.0;
            double pa = 0.0;
            double pb = 0.0;
            double pc = 0.0;

            UpdateDocument(new Action(delegate()
            {

                CheckAllEnabled();

                if (IsConnected)
                {
                    if (ShowMachineCoords)
                    {
                        _Controller.CoordMotion.Interpreter.ReadCurMachinePosition(ref x, ref y, ref z, ref a, ref b, ref c);
                    }
                    else
                    {
                        _Controller.CoordMotion.Interpreter.ReadCurInterpreterPosition(ref x, ref y, ref z, ref a, ref b, ref c);
                    }
            
      
                }
                else
                {
                    x = _Controller.CoordMotion.Interpreter.SetupParams.X_AxisPosition;
                    y = _Controller.CoordMotion.Interpreter.SetupParams.Y_AxisPosition;
                    z = _Controller.CoordMotion.Interpreter.SetupParams.Z_AxisPosition;
                    a = _Controller.CoordMotion.Interpreter.SetupParams.A_AxisPosition;
                    b = _Controller.CoordMotion.Interpreter.SetupParams.B_AxisPosition;
                    c = _Controller.CoordMotion.Interpreter.SetupParams.C_AxisPosition;
 
                }


                if (_AxisContainer.ContainsKey("X"))
                {
                    _GuiHost.SetElementText(Elements.XDest, Math.Round(x, 3).ToString("000.000"));
                    if (ShowPosition)
                    {
                        px = _AxisContainer["X"].Axis.GetActualPosition();
                        _GuiHost.SetElementText(Elements.XPos, Math.Round(px, 3).ToString("000.000"));
                    }
                }
                if (_AxisContainer.ContainsKey("Y"))
                {
                    _GuiHost.SetElementText(Elements.YDest, Math.Round(y, 3).ToString("000.000"));
                    if (ShowPosition)
                    {
                        py = _AxisContainer["Y"].Axis.GetActualPosition();
                        _GuiHost.SetElementText(Elements.YPos, Math.Round(py, 3).ToString("000.000"));
                    } 
                }
                if (_AxisContainer.ContainsKey("Z"))
                {
                    _GuiHost.SetElementText(Elements.ZDest, Math.Round(z, 3).ToString("000.000"));
                    if (ShowPosition)
                    {
                        pz = _AxisContainer["Z"].Axis.GetActualPosition();
                        _GuiHost.SetElementText(Elements.ZPos, Math.Round(pz, 3).ToString("000.000"));
                    } 
                }
                if (_AxisContainer.ContainsKey("A"))
                {
                    _GuiHost.SetElementText(Elements.ADest, Math.Round(a, 3).ToString("000.000"));
                    if (ShowPosition)
                    {
                        pa = _AxisContainer["A"].Axis.GetActualPosition();
                        _GuiHost.SetElementText(Elements.APos, Math.Round(pa, 3).ToString("000.000"));
                    } 
                }
                if (_AxisContainer.ContainsKey("B"))
                {
                    _GuiHost.SetElementText(Elements.BDest, Math.Round(b, 3).ToString("000.000"));
                    if (ShowPosition)
                    {
                        pb = _AxisContainer["B"].Axis.GetActualPosition();
                        _GuiHost.SetElementText(Elements.BPos, Math.Round(pb, 3).ToString("000.000"));
                    } 
                }
                if (_AxisContainer.ContainsKey("C"))
                {
                    _GuiHost.SetElementText(Elements.CDest, Math.Round(c, 3).ToString("000.000"));
                    if (ShowPosition)
                    {
                        pc = _AxisContainer["C"].Axis.GetActualPosition();
                        _GuiHost.SetElementText(Elements.CPos, Math.Round(pc, 3).ToString("000.000"));
                    } 
                }
           
          
            }));
        }

    }
}
