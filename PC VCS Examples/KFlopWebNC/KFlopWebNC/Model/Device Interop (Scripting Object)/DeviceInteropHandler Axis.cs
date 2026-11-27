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
        public bool ShowPosition { get; set; }
        public bool ShowMachineCoords { get; set; }


        #region Axis creation helpers
        public void CreateX(int channel, double movespeed, double jogspeed)
        {
            string axisname = "X";
            SetAxis(axisname, channel, movespeed, jogspeed);
        }
        public void CreateY(int channel, double movespeed, double jogspeed)
        {
            string axisname = "Y";
            SetAxis(axisname, channel, movespeed, jogspeed);
        }
        public void CreateZ(int channel, double movespeed, double jogspeed)
        {
            string axisname = "Z";
            SetAxis(axisname, channel, movespeed, jogspeed);
        }
        public void CreateA(int channel, double movespeed, double jogspeed)
        {
            string axisname = "A";
            SetAxis(axisname, channel, movespeed, jogspeed);
        }
        public void CreateB(int channel, double movespeed, double jogspeed)
        {
            string axisname = "B";
            SetAxis(axisname, channel, movespeed, jogspeed);
        }
        public void CreateC(int channel, double movespeed, double jogspeed)
        {
            string axisname = "C";
            SetAxis(axisname, channel, movespeed, jogspeed);
        }

        public void SetAxis(string axisname, int channel, double movespeed, double jogspeed)
        {
            if (_AxisContainer.ContainsKey(axisname))
            {
                _AxisContainer[axisname].Axis.ID = channel;
                _AxisContainer[axisname].JogVelocity = jogspeed;
                _AxisContainer[axisname].Axis.Velocity = movespeed;
            }
            else
            {
                _AxisContainer.Add(axisname, new AxisNode(new KM_Axis(_Controller, channel, "X")));
                _AxisContainer[axisname].JogVelocity = jogspeed;
                _AxisContainer[axisname].Axis.Velocity = movespeed;
            }
        } 
        #endregion

        #region Axis enabled methods 
        /// <summary>
        /// Command dispatcher to enable/disable X
        /// </summary>
        public void ToggleEnableX()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("X"))
                {
                    var axis = _AxisContainer["X"].Axis;
                    if (_Status.GetAxisEnabled(0) > 0)
                    {
                        axis.Disable();
                    }
                    else
                    {
                        axis.Enable();
                    }
                }
            }
        }

        /// <summary>
        /// Command dispatcher to enable/disable Y
        /// </summary>
        public void ToggleEnableY()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("Y"))
                {
                    var axis = _AxisContainer["Y"].Axis;
                    if (_Status.GetAxisEnabled(1) > 0)
                    {
                        axis.Disable();
                    }
                    else
                    {
                        axis.Enable();
                    }
                }
            }
        }

        /// <summary>
        /// Command dispatcher to enable/disable Z
        /// </summary>
        public void ToggleEnableZ()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("Z"))
                {
                    var axis = _AxisContainer["Z"].Axis;
                    if (_Status.GetAxisEnabled(2) > 0)
                    {
                        axis.Disable();
                    }
                    else
                    {
                        axis.Enable();
                    }
                }
            }
        }

        /// <summary>
        /// Command dispatcher to enable/disable A
        /// </summary>
        public void ToggleEnableA()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("A"))
                {
                    var axis = _AxisContainer["A"].Axis;
                    if (_Status.GetAxisEnabled(3) > 0)
                    {
                        axis.Disable();
                    }
                    else
                    {
                        axis.Enable();
                    }
                }
            }
        }

        /// <summary>
        /// Command dispatcher to enable/disable B
        /// </summary>
        public void ToggleEnableB()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("B"))
                {
                    var axis = _AxisContainer["B"].Axis;
                    if (_Status.GetAxisEnabled(4) > 0)
                    {
                        axis.Disable();
                    }
                    else
                    {
                        axis.Enable();
                    }
                }
            }
        }

        /// <summary>
        /// Command dispatcher to enable/disable C
        /// </summary>
        public void ToggleEnableC()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("C"))
                {
                    var axis = _AxisContainer["C"].Axis;
                    if (_Status.GetAxisEnabled(5) > 0)
                    {
                        axis.Disable();
                    }
                    else
                    {
                        axis.Enable();
                    }
                }
            }
        }

        public bool IsEnabledX { get; set; }
        public bool IsEnabledY { get; set; }
        public bool IsEnabledZ { get; set; }
        public bool IsEnabledA { get; set; }
        public bool IsEnabledB { get; set; }
        public bool IsEnabledC { get; set; }
        
        /// <summary>
        /// Query command to determine whether the X axis is enabled
        /// </summary>
        /// <returns>true if the axis is enabled</returns>
        public void CheckEnabledX()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("X"))
                {
                    var axis = _AxisContainer["X"].Axis;
                    IsEnabledX = _Status.GetAxisEnabled(0) > 0;
                }
            }
            else
            {
                IsEnabledX = false;
            }
        }

        /// <summary>
        /// Query command to determine whether the Y axis is enabled
        /// </summary>
        /// <returns>true if the axis is enabled</returns>
        public void CheckEnabledY()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("Y"))
                {
                    var axis = _AxisContainer["Y"].Axis;
                    IsEnabledY = _Status.GetAxisEnabled(1) > 0;
                }
            }
            else
            {
                IsEnabledY = false;
            }
        }

        /// <summary>
        /// Query command to determine whether the Z axis is enabled
        /// </summary>
        /// <returns>true if the axis is enabled</returns>
        public void CheckEnabledZ()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("Z"))
                {
                    var axis = _AxisContainer["Z"].Axis;
                    IsEnabledZ = _Status.GetAxisEnabled(2) > 0;
                }
            }
            else
            {
                IsEnabledZ = false;
            }
        }

        /// <summary>
        /// Query command to determine whether the A axis is enabled
        /// </summary>
        /// <returns>true if the axis is enabled</returns>
        public void CheckEnabledA()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("A"))
                {
                    var axis = _AxisContainer["A"].Axis;
                    IsEnabledA = _Status.GetAxisEnabled(3) > 0;
                }
            }
            else
            {
                IsEnabledA = false;
            }
        }

        /// <summary>
        /// Query command to determine whether the B axis is enabled
        /// </summary>
        /// <returns>true if the axis is enabled</returns>
        public void CheckEnabledB()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("B"))
                {
                    var axis = _AxisContainer["B"].Axis;
                    IsEnabledB = _Status.GetAxisEnabled(4) > 0;
                }
            }
            else
            {
                IsEnabledB = false;
            }
        }

        /// <summary>
        /// Query command to determine whether the C axis is enabled
        /// </summary>
        /// <returns>true if the axis is enabled</returns>
        public void CheckEnabledC()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("C"))
                {
                    var axis = _AxisContainer["C"].Axis;
                    IsEnabledC = _Status.GetAxisEnabled(5) > 0;
                }
            }
            else
            {
                IsEnabledC = false;
            }
        }

        public void CheckAllEnabled()
        {
            CheckEnabledX();
            CheckEnabledY();
            CheckEnabledZ();
            CheckEnabledA();
            CheckEnabledB();
            CheckEnabledC();
        }
        #endregion

        #region Axis stopping methods

        /// <summary>
        /// Command dispatcher to stop X axis motion
        /// </summary>
        public void StopX()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("X"))
                {
                    _AxisContainer["X"].Axis.Stop();
                }
            }
        }

        /// <summary>
        /// Command dispatcher to stop Y axis motion
        /// </summary>
        public void StopY()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("Y"))
                {
                    _AxisContainer["Y"].Axis.Stop();
                }
            }
        }

        /// <summary>
        /// Command dispatcher to stop Z axis motion
        /// </summary>
        public void StopZ()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("Z"))
                {
                    _AxisContainer["Z"].Axis.Stop();
                }
            }
        }

        /// <summary>
        /// Command dispatcher to stop A axis motion
        /// </summary>
        public void StopA()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("A"))
                {
                    _AxisContainer["A"].Axis.Stop();
                }
            }
        }

        /// <summary>
        /// Command dispatcher to stop B axis motion
        /// </summary>
        public void StopB()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("B"))
                {
                    _AxisContainer["B"].Axis.Stop();
                }
            }
        }

        /// <summary>
        /// Command dispatcher to stop C axis motion
        /// </summary>
        public void StopC()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("C"))
                {
                    _AxisContainer["C"].Axis.Stop();
                }
            }
        } 
        #endregion

        #region Axis zeroing methods
        /// <summary>
        /// Command dispatcher to reset the X axis dest to 0.0
        /// </summary>
        public void ZeroX()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("X"))
                {
                    _AxisContainer["X"].Axis.ZeroAxis();
                }
            }
        }

        /// <summary>
        /// Command dispatcher to reset the Y axis dest to 0.0
        /// </summary>
        public void ZeroY()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("Y"))
                {
                    _AxisContainer["Y"].Axis.ZeroAxis();
                }
            }
        }

        /// <summary>
        /// Command dispatcher to reset the Z axis dest to 0.0
        /// </summary>
        public void ZeroZ()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("Z"))
                {
                    _AxisContainer["Z"].Axis.ZeroAxis();
                }
            }
        }

        /// <summary>
        /// Command dispatcher to reset the A axis dest to 0.0
        /// </summary>
        public void ZeroA()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("A"))
                {
                    _AxisContainer["A"].Axis.ZeroAxis();
                }
            }
        }

        /// <summary>
        /// Command dispatcher to reset the B axis dest to 0.0
        /// </summary>
        public void ZeroB()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("B"))
                {
                    _AxisContainer["B"].Axis.ZeroAxis();
                }
            }
        }

        /// <summary>
        /// Command dispatcher to reset the C axis dest to 0.0
        /// </summary>
        public void ZeroC()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("C"))
                {
                    _AxisContainer["C"].Axis.ZeroAxis();
                }
            }
        } 
        #endregion

        #region Axis jogging methods
        /// <summary>
        /// Command dispatcher to update the X axis feedrate percentage
        /// </summary>
        /// <param name="percent">percentage to derive jog speed</param>
        public void SetJogPercentX(double percent)
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("X"))
                {
                    _AxisContainer["X"].JogPercent = percent / 100;
                }
            }
        }

        /// <summary>
        /// Command dispatcher to update the Y axis feedrate percentage
        /// </summary>
        /// <param name="percent">percentage to derive jog speed</param>
        public void SetJogPercentY(double percent)
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("Y"))
                {
                    _AxisContainer["Y"].JogPercent = percent / 100;
                }
            }
        }

        /// <summary>
        /// Command dispatcher to update the Z axis feedrate percentage
        /// </summary>
        /// <param name="percent">percentage to derive jog speed</param>
        public void SetJogPercentZ(double percent)
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("Z"))
                {
                    _AxisContainer["Z"].JogPercent = percent / 100;
                }
            }
        }

        /// <summary>
        /// Command dispatcher to update the A axis feedrate percentage
        /// </summary>
        /// <param name="percent">percentage to derive jog speed</param>
        public void SetJogPercentA(double percent)
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("A"))
                {
                    _AxisContainer["A"].JogPercent = percent / 100;
                }
            }
        }

        /// <summary>
        /// Command dispatcher to update the B axis feedrate percentage
        /// </summary>
        /// <param name="percent">percentage to derive jog speed</param>
        public void SetJogPercentB(double percent)
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("B"))
                {
                    _AxisContainer["B"].JogPercent = percent / 100;
                }
            }
        }

        /// <summary>
        /// Command dispatcher to update the C axis feedrate percentage
        /// </summary>
        /// <param name="percent">percentage to derive jog speed</param>
        public void SetJogPercentC(double percent)
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("C"))
                {
                    _AxisContainer["C"].JogPercent = percent / 100;
                }
            }
        }

        /// <summary>
        /// Command dispatcher to jog the X axis in positive direction
        /// </summary>
        public void JogXPositive()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("X"))
                {
                    _AxisContainer["X"].Axis.Jog(_AxisContainer["X"].JogVelocity * _AxisContainer["X"].JogPercent);
                }
            }
        }

        /// <summary>
        /// Command dispatcher to jog the Y axis in positive direction
        /// </summary>
        public void JogYPositive()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("Y"))
                {
                    _AxisContainer["Y"].Axis.Jog(_AxisContainer["Y"].JogVelocity * _AxisContainer["Y"].JogPercent);
                }
            }
        }

        /// <summary>
        /// Command dispatcher to jog the Z axis in positive direction
        /// </summary>
        public void JogZPositive()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("Z"))
                {
                    _AxisContainer["Z"].Axis.Jog(_AxisContainer["Z"].JogVelocity * _AxisContainer["Z"].JogPercent);
                }
            }
        }

        /// <summary>
        /// Command dispatcher to jog the A axis in positive direction
        /// </summary>
        public void JogAPositive()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("A"))
                {
                    _AxisContainer["A"].Axis.Jog(_AxisContainer["A"].JogVelocity * _AxisContainer["A"].JogPercent);
                }
            }
        }

        /// <summary>
        /// Command dispatcher to jog the B axis in positive direction
        /// </summary>
        public void JogBPositive()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("B"))
                {
                    _AxisContainer["B"].Axis.Jog(_AxisContainer["B"].JogVelocity * _AxisContainer["B"].JogPercent);
                }
            }
        }

        /// <summary>
        /// Command dispatcher to jog the C axis in positive direction
        /// </summary>
        public void JogCPositive()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("C"))
                {
                    _AxisContainer["C"].Axis.Jog(_AxisContainer["C"].JogVelocity * _AxisContainer["C"].JogPercent);
                }
            }
        }


        /// <summary>
        /// Command dispatcher to jog the X axis in negative direction
        /// </summary>
        public void JogXNegative()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("X"))
                {
                    _AxisContainer["X"].Axis.Jog(-(_AxisContainer["X"].JogVelocity * _AxisContainer["X"].JogPercent));
                }
            }
        }

        /// <summary>
        /// Command dispatcher to jog the Y axis in negative direction
        /// </summary>
        public void JogYNegative()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("Y"))
                {
                    _AxisContainer["Y"].Axis.Jog(-(_AxisContainer["Y"].JogVelocity * _AxisContainer["Y"].JogPercent));
                }
            }
        }

        /// <summary>
        /// Command dispatcher to jog the Z axis in negative direction
        /// </summary>
        public void JogZNegative()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("Z"))
                {
                    _AxisContainer["Z"].Axis.Jog(-(_AxisContainer["Z"].JogVelocity * _AxisContainer["Z"].JogPercent));
                }
            }
        }

        /// <summary>
        /// Command dispatcher to jog the A axis in negative direction
        /// </summary>
        public void JogANegative()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("A"))
                {
                    _AxisContainer["A"].Axis.Jog(-(_AxisContainer["A"].JogVelocity * _AxisContainer["A"].JogPercent));
                }
            }
        }

        /// <summary>
        /// Command dispatcher to jog the B axis in negative direction
        /// </summary>
        public void JogBNegative()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("B"))
                {
                    _AxisContainer["B"].Axis.Jog(-(_AxisContainer["B"].JogVelocity * _AxisContainer["B"].JogPercent));
                }
            }
        }

        /// <summary>
        /// Command dispatcher to jog the C axis in negative direction
        /// </summary>
        public void JogCNegative()
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("C"))
                {
                    _AxisContainer["C"].Axis.Jog(-(_AxisContainer["C"].JogVelocity * _AxisContainer["C"].JogPercent));
                }
            }
        }

        /// <summary>
        /// Forces the text of designated elements to display the jog override percentage for each axis
        /// </summary>
        public void UpdateJogRates()
        {
            var jog = "100%";

            UpdateDocument(new Action(delegate()
            {
                if (_AxisContainer.ContainsKey("X"))
                {
                    jog = String.Format("{0}%", Math.Round(100 * (_AxisContainer["X"].JogPercent), 0));

                    _GuiHost.SetElementText(Elements.ValueXJogPercent, jog);

                }
                if (_AxisContainer.ContainsKey("Y"))
                {
                    jog = String.Format("{0}%", Math.Round(100 * (_AxisContainer["Y"].JogPercent), 0));

                    _GuiHost.SetElementText(Elements.ValueYJogPercent, jog);

                }
                if (_AxisContainer.ContainsKey("Z"))
                {
                    jog = String.Format("{0}%", Math.Round(100 * (_AxisContainer["Z"].JogPercent), 0));

                    _GuiHost.SetElementText(Elements.ValueZJogPercent, jog);

                }
                if (_AxisContainer.ContainsKey("A"))
                {
                    jog = String.Format("{0}%", Math.Round(100 * (_AxisContainer["A"].JogPercent), 0));

                    _GuiHost.SetElementText(Elements.ValueAJogPercent, jog);

                }
                if (_AxisContainer.ContainsKey("B"))
                {
                    jog = String.Format("{0}%", Math.Round(100 * (_AxisContainer["B"].JogPercent), 0));

                    _GuiHost.SetElementText(Elements.ValueBJogPercent, jog);

                }
                if (_AxisContainer.ContainsKey("C"))
                {
                    jog = String.Format("{0}%", Math.Round(100 * (_AxisContainer["C"].JogPercent), 0));

                    _GuiHost.SetElementText(Elements.ValueCJogPercent, jog);

                }
            }));
        } 
        #endregion
  
        #region Axis moving methods
        /// <summary>
        /// Moves the X axis to a commanded position
        /// </summary>
        /// <param name="position">new position to move axis to</param>
        public void MoveX(double position)
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("X"))
                {
                    _AxisContainer["X"].Axis.MoveTo(position);
                }
            }
        }

        /// <summary>
        /// Moves the Y axis to a commanded position
        /// </summary>
        /// <param name="position">new position to move axis to</param>
        public void MoveY(double position)
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("Y"))
                {
                    _AxisContainer["Y"].Axis.MoveTo(position);
                }
            }
        }

        /// <summary>
        /// Moves the Z axis to a commanded position
        /// </summary>
        /// <param name="position">new position to move axis to</param>
        public void MoveZ(double position)
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("Z"))
                {
                    _AxisContainer["Z"].Axis.MoveTo(position);
                }
            }
        }

        /// <summary>
        /// Moves the A axis to a commanded position
        /// </summary>
        /// <param name="position">new position to move axis to</param>
        public void MoveA(double position)
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("A"))
                {
                    _AxisContainer["A"].Axis.MoveTo(position);
                }
            }
        }

        /// <summary>
        /// Moves the B axis to a commanded position
        /// </summary>
        /// <param name="position">new position to move axis to</param>
        public void MoveB(double position)
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("B"))
                {
                    _AxisContainer["B"].Axis.MoveTo(position);
                }
            }
        }

        /// <summary>
        /// Moves the C axis to a commanded position
        /// </summary>
        /// <param name="position">new position to move axis to</param>
        public void MoveC(double position)
        {
            if (IsConnected)
            {
                if (_AxisContainer.ContainsKey("C"))
                {
                    _AxisContainer["C"].Axis.MoveTo(position);
                }
            }
        } 
        #endregion 
    }
}
