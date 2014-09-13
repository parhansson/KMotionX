using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using KMotion_dotNet;
using System.Windows.Forms;
using System.Reflection;
using System.IO;
using KFlopWebNC.DOM_Helpers;
using System.Threading;

namespace KFlopWebNC.Model
{ 
    public partial class DeviceInteropHandler
    {
        private string _GcodeFile = "";
        private string _MDIFile = "";

        #region Properties
        public string GcodeFile
        {
            get
            {
                return _GcodeFile;
            }
            set
            {
                _GcodeFile = value;
            }
        }
        public string MDIFile
        {
            get
            {
                return _MDIFile;
            }
            set
            {
                _MDIFile = value;
            }
        }


        public InterpreterData InterpreterDataFile { get; set; }
        public bool IsSimulationMode { get; set; }
        public bool BlockDeleteActive { get; set; }
        public bool IsInterpreterPaused { get; set; }

        public bool RunningGcode { get; set; }
        public string InterpreterFileName { get; set; }
        public int CurrentLine { get; set; }
        public int MaxLines { get; set; }
        public List<string> GcodeLines { get; set; } 
        #endregion


        /// <summary>
        /// Command dispatcher to set the Interpreter's feed rate override
        /// </summary>
        /// <param name="percent"></param>
        public void SetFeedRate(double percent)
        {
            if (percent > 0)
            {
                _Controller.CoordMotion.FeedRateOverride = percent / 100.0;
            }
        }

        /// <summary>
        /// Forces an update to the element designated
        /// </summary>
        public void UpdateFeedRate()
        {
            var fro = "100%";

            fro = String.Format("{0}%", Math.Round(_Controller.CoordMotion.FeedRateOverride * 100, 0));
            UpdateDocument(new Action(delegate()
            {
                _GuiHost.SetElementText(Elements.ValueFRO, fro); 
            })); 
        }


        /// <summary>
        /// Forces an update of the interpreter display settings
        /// </summary>
        public void RefreshInterpreterState()
        {
            var action = new Action(delegate()
                {
                    while (!Loaded)
                    {
                        Thread.Sleep(10);
                    }
                    UpdateGcodeDisplay();
                    UpdateDocument(new Action(delegate()
                    {
                        _GuiHost.SetElementText(Elements.GCodeProgramName, Path.GetFileNameWithoutExtension(InterpreterFileName));
                        _GuiHost.SetElementText(Elements.GCodeLineToRun, CurrentLine.ToString());
                    }));
                });
            action.BeginInvoke(null, null);
        }
 

        /// <summary>
        /// Command dispatcher to execute a single RS274 command
        /// </summary>
        public void ExecuteMDI()
        {
            var mdi = _GuiHost.GetAttribute(Elements.MDICommand, Attribs.Value);

            if (mdi !=null && mdi != "")
            {
                var file = _MDIFile;
                using (StreamWriter SW = File.CreateText(file))
                {
                    SW.WriteLine(mdi);
                    SW.Close();
                }
                Thread.Sleep(100);
                _Controller.CoordMotion.Interpreter.Interpret(file);
            }
        }


        /// <summary>
        /// Command dispatcher to open a file dialog and select the gcode file
        /// </summary>
        public void LoadGcodeFile()
        {
            UpdateDocument(new Action(delegate()
            {
                var ofd = new OpenFileDialog();
                ofd.Multiselect = false;
                if (ofd.ShowDialog() == DialogResult.OK)
                {
                    var file = ofd.FileName;
                    if (File.Exists(file))
                    {
                        File.Copy(file, _GcodeFile, true);
                        InterpreterFileName = file;

                        _GuiHost.SetElementText(Elements.GCodeProgramName, Path.GetFileNameWithoutExtension(InterpreterFileName));

                        ResetGcodeDisplay();
                    }
                }
            }));
        }
   
        /// <summary>
        /// Command dispatcher 'rewind' the interpreter file
        /// </summary>
        public void RewindProgram()
        {
            if (!RunningGcode)
            {
                ResetGcodeDisplay();
            }
        }

        /// <summary>
        /// Command dispatcher to execute the gcode from a specific line
        /// </summary>
        public void RunFromLine()
        {
            UpdateDocument(new Action(delegate()
            {
                var line = _GuiHost.GetAttribute(Elements.GCodeLineToRun, Attribs.Value);
                if (line != "")
                {
                    if (GcodeLines == null || GcodeLines.Count < 1)
                    {
                        ResetGcodeDisplay();
                    }
                    if (GcodeLines.Count > 0)
                    {
                        int lineno = 0;
                        if (Int32.TryParse(line, out lineno))
                        {
                            CurrentLine = lineno;
                            UpdateGcodeDisplay();


                            _GuiHost.Update();

                            _Controller.CoordMotion.Interpreter.Interpret(InterpreterFileName,
                            CurrentLine, GcodeLines.Count - 1, 0);
                        }
                    }
                }
            }));
        }

        /// <summary>
        /// Command dispatcher to toggle whether the interpreter should simulate the file
        /// </summary>
        public void ToggleSimulate()
        {
            _Controller.CoordMotion.IsSimulation = !_Controller.CoordMotion.IsSimulation;
            IsSimulationMode = _Controller.CoordMotion.IsSimulation;
        }

        /// <summary>
        /// Command dispatcher to toggle whether the interpreter will skip escaped blocks
        /// </summary>
        public void ToggleBlockDelete()
        {
            _Controller.CoordMotion.Interpreter.SetupParams.BlockDelete = !_Controller.CoordMotion.Interpreter.SetupParams.BlockDelete;
            BlockDeleteActive = _Controller.CoordMotion.Interpreter.SetupParams.BlockDelete;
        }

        /// <summary>
        /// Command dispatcher to invoke a command via the interpreter buffer
        /// </summary>
        /// <param name="command">command to to run contained in the Mcode Action list</param>
        public void RunUserCommand(int command)
        {
            if (IsConnected)
            {
                UpdateDocument(new Action(delegate()
                {

                    _Controller.CoordMotion.Interpreter.InvokeAction(command, false);

                }));
            }
        }

 
        /// <summary>
        /// Command dispatcher for interpreting a gcode file
        /// </summary>
        public void CycleStart()
        {
            RunningGcode = true;
            _Controller.CoordMotion.Interpreter.Interpret(InterpreterFileName);
        }
        
        /// <summary>
        /// Command dispatcher for pausing/unpausing the interpreter
        /// </summary>
        public void CyclePause()
        {
            if (IsConnected)
            {
                var response = _Controller.WriteLineReadLine("GetStopState");

                if (response == "0")
                {
                    _Controller.WriteLine("StopImmediate0");
                    IsInterpreterPaused = true;
                }
                else
                {
                    _Controller.WriteLine("StopImmediate1");
                    IsInterpreterPaused = false;
                }
            }
        }

        /// <summary>
        /// Command dispatcher for halting the interpreter
        /// </summary>
        public void StopInterpreter()
        {
            RunningGcode = false;
            _Controller.CoordMotion.Interpreter.Halt();
        }




        /// <summary>
        /// Discovers the maximum display lines and loads the Gcode file into the display buffer
        /// </summary>
        public void ResetGcodeDisplay()
        {
            UpdateDocument(new Action(delegate()
           {
               CurrentLine = 1;
               GcodeLines = new List<string>();

               int lines = 10;
               var maxlines = _GuiHost.GetAttribute(Elements.GcodeDisplay, Attribs.MaxLines);

               if (maxlines != "")
               {
                   if (Int32.TryParse(maxlines, out lines))
                   {
                       MaxLines = lines;
                   }
               }
               var line = "";
               using (StreamReader sr = new StreamReader(_GcodeFile))
               {
                   while (line != null)
                   {
                       line = sr.ReadLine();
                       if (line != null)
                       {
                           GcodeLines.Add(line);
                       }
                   }
                   sr.Close();
               }
           }));
            UpdateGcodeDisplay();
        }
 
        /// <summary>
        /// Updates the designated gcode display dv with the current gcode being executed
        /// </summary>
        public void UpdateGcodeDisplay()
        {
            if (IsSimulationMode)
            {
                return;
            }
            UpdateDocument(new Action(delegate()
            {
                if (GcodeLines != null)
                {
                    StringBuilder sb = new StringBuilder();
                    if (CurrentLine > MaxLines)
                    {
                        var start = CurrentLine - (MaxLines / 2);
                        for (int i = start; i < start + MaxLines; i++)
                        {
                            if (GcodeLines.Count > i)
                            {
                                if (i == CurrentLine - 1)//Selected Line
                                {
                                    sb.AppendLine(String.Format("<p class=\"{0}\">{1}</p>", Classes.CurrentGcodeLine, GcodeLines[i]));
                                }
                                else
                                {
                                    sb.AppendLine(String.Format("<p class=\"{0}\">{1}</p>", Classes.GcodeLine, GcodeLines[i]));
                                }
                            }
                        }
                    }
                    else
                    {
                        var start = CurrentLine - (MaxLines / 2);
                        if (start > 0)
                        {
                            for (int i = start; i < start + MaxLines; i++)
                            {
                                if (GcodeLines.Count > i)
                                {
                                    if (i == CurrentLine - 1)//Selected Line
                                    {
                                        sb.AppendLine(String.Format("<p class=\"{0}\">{1}</p>", Classes.CurrentGcodeLine, GcodeLines[i]));
                                    }
                                    else
                                    {
                                        sb.AppendLine(String.Format("<p class=\"{0}\">{1}</p>", Classes.GcodeLine, GcodeLines[i]));
                                    }
                                }
                            }
                        }
                        else
                        {
                            for (int i = 0; i < MaxLines; i++)
                            {
                                if (GcodeLines.Count > i)
                                {
                                    if (i == CurrentLine - 1)//Selected Line
                                    {
                                        sb.AppendLine(String.Format("<p class=\"{0}\">{1}</p>", Classes.CurrentGcodeLine, GcodeLines[i]));
                                    }
                                    else
                                    {
                                        sb.AppendLine(String.Format("<p class=\"{0}\">{1}</p>", Classes.GcodeLine, GcodeLines[i]));
                                    }
                                }
                            }
                        }
                    }

                    _GuiHost.SetElementHTML(Elements.GcodeDisplay, sb.ToString());

                }
            }));
        }




        /// <summary>
        /// Updates the trajectory planner editor HTML from the data file
        /// </summary>
        public void LoadInterpreterTPEditor()
        {
            UpdateDocument(new Action(delegate()
            {
                _GuiHost.SetAttribute("tplannerbreakangle", Attribs.Value, InterpreterDataFile.BreakAngle);
                _GuiHost.SetAttribute("tplannerlookahead", Attribs.Value, InterpreterDataFile.LookAhead);
                _GuiHost.SetAttribute("tplannercollineartolerance", Attribs.Value, InterpreterDataFile.CollinearTolerance);
                _GuiHost.SetAttribute("tplannercornertolerance", Attribs.Value, InterpreterDataFile.CornerTolerance);
                _GuiHost.SetAttribute("tplannerfacetangle", Attribs.Value, InterpreterDataFile.FacetAngle);
            }));
        }

        /// <summary>
        /// Updates the data file trajectory planner from the HTML document
        /// </summary>
        public void SaveInterpreterTPEditor()
        {
            UpdateDocument(new Action(delegate()
            {
                double offsetvalue = 0.0;
                if (_GuiHost.Document.GetElementById("tplannerbreakangle") != null)
                {
                    if (Double.TryParse(_GuiHost.GetAttribute("tplannerbreakangle", Attribs.Value), out offsetvalue))
                    {
                        InterpreterDataFile.BreakAngle = offsetvalue;
                    }
                }
                if (_GuiHost.Document.GetElementById("tplannerlookahead") != null)
                {
                    if (Double.TryParse(_GuiHost.GetAttribute("tplannerlookahead", Attribs.Value), out offsetvalue))
                    {
                        InterpreterDataFile.LookAhead = offsetvalue;
                    }
                }
                if (_GuiHost.Document.GetElementById("tplannercollineartolerance") != null)
                {
                    if (Double.TryParse(_GuiHost.GetAttribute("tplannercollineartolerance", Attribs.Value), out offsetvalue))
                    {
                        InterpreterDataFile.CollinearTolerance = offsetvalue;
                    }
                }
                if (_GuiHost.Document.GetElementById("tplannercornertolerance") != null)
                {
                    if (Double.TryParse(_GuiHost.GetAttribute("tplannercornertolerance", Attribs.Value), out offsetvalue))
                    {
                        InterpreterDataFile.CornerTolerance = offsetvalue;
                    }
                }
                if (_GuiHost.Document.GetElementById("tplannerfacetangle") != null)
                {
                    if (Double.TryParse(_GuiHost.GetAttribute("tplannerfacetangle", Attribs.Value), out offsetvalue))
                    {
                        InterpreterDataFile.FacetAngle = offsetvalue;
                    }
                }
            }));
        }

        /// <summary>
        /// Updates the axis editor HTML from the data file
        /// </summary>
        public void LoadInterpreterAxisEditor()
        {
            UpdateDocument(new Action(delegate()
              {
                  if (_GuiHost.Document.GetElementById("axisnamelabelX") != null)
                  {
                      _GuiHost.SetAttribute("axisindexX", Attribs.Value, InterpreterDataFile.XaxisIndex);
                      _GuiHost.SetAttribute("axiscountsX", Attribs.Value, InterpreterDataFile.XaxisCPI);
                      _GuiHost.SetAttribute("axisaccelX", Attribs.Value, InterpreterDataFile.XaxisAccel);
                      _GuiHost.SetAttribute("axisvelX", Attribs.Value, InterpreterDataFile.XaxisVel);
                      _GuiHost.SetAttribute("axisjogvelX", Attribs.Value, InterpreterDataFile.XaxisJogVel);
                  }
                  if (_GuiHost.Document.GetElementById("axisnamelabelY") != null)
                  {
                      _GuiHost.SetAttribute("axisindexY", Attribs.Value, InterpreterDataFile.YaxisIndex);
                      _GuiHost.SetAttribute("axiscountsY", Attribs.Value, InterpreterDataFile.YaxisCPI);
                      _GuiHost.SetAttribute("axisaccelY", Attribs.Value, InterpreterDataFile.YaxisAccel);
                      _GuiHost.SetAttribute("axisvelY", Attribs.Value, InterpreterDataFile.YaxisVel);
                      _GuiHost.SetAttribute("axisjogvelY", Attribs.Value, InterpreterDataFile.YaxisJogVel);
                  }
                  if (_GuiHost.Document.GetElementById("axisnamelabelZ") != null)
                  {
                      _GuiHost.SetAttribute("axisindexZ", Attribs.Value, InterpreterDataFile.ZaxisIndex);
                      _GuiHost.SetAttribute("axiscountsZ", Attribs.Value, InterpreterDataFile.ZaxisCPI);
                      _GuiHost.SetAttribute("axisaccelZ", Attribs.Value, InterpreterDataFile.ZaxisAccel);
                      _GuiHost.SetAttribute("axisvelZ", Attribs.Value, InterpreterDataFile.ZaxisVel);
                      _GuiHost.SetAttribute("axisjogvelZ", Attribs.Value, InterpreterDataFile.ZaxisJogVel);
                  }
                  if (_GuiHost.Document.GetElementById("axisnamelabelA") != null)
                  {
                      _GuiHost.SetAttribute("axisindexA", Attribs.Value, InterpreterDataFile.AaxisIndex);
                      _GuiHost.SetAttribute("axiscountsA", Attribs.Value, InterpreterDataFile.AaxisCPI);
                      _GuiHost.SetAttribute("axisaccelA", Attribs.Value, InterpreterDataFile.AaxisAccel);
                      _GuiHost.SetAttribute("axisvelA", Attribs.Value, InterpreterDataFile.AaxisVel);
                      _GuiHost.SetAttribute("axisjogvelA", Attribs.Value, InterpreterDataFile.AaxisJogVel);
                  }
                  if (_GuiHost.Document.GetElementById("axisnamelabelB") != null)
                  {
                      _GuiHost.SetAttribute("axisindexB", Attribs.Value, InterpreterDataFile.BaxisIndex);
                      _GuiHost.SetAttribute("axiscountsB", Attribs.Value, InterpreterDataFile.BaxisCPI);
                      _GuiHost.SetAttribute("axisaccelB", Attribs.Value, InterpreterDataFile.BaxisAccel);
                      _GuiHost.SetAttribute("axisvelB", Attribs.Value, InterpreterDataFile.BaxisVel);
                      _GuiHost.SetAttribute("axisjogvelB", Attribs.Value, InterpreterDataFile.BaxisJogVel);
                  }
                  if (_GuiHost.Document.GetElementById("axisnamelabelC") != null)
                  {
                      _GuiHost.SetAttribute("axisindexC", Attribs.Value, InterpreterDataFile.CaxisIndex);
                      _GuiHost.SetAttribute("axiscountsC", Attribs.Value, InterpreterDataFile.CaxisCPI);
                      _GuiHost.SetAttribute("axisaccelC", Attribs.Value, InterpreterDataFile.CaxisAccel);
                      _GuiHost.SetAttribute("axisvelC", Attribs.Value, InterpreterDataFile.CaxisVel);
                      _GuiHost.SetAttribute("axisjogvelC", Attribs.Value, InterpreterDataFile.CaxisJogVel);
                  }
              }));
        }

        /// <summary>
        /// Updates the data file axis settings from the HTML document
        /// </summary>
        public void SaveInterpreterAxisEditor()
        {
            UpdateDocument(new Action(delegate()
              {
                  double offsetvalue = 0.0;
                  int indexvalue = 0;
                  if (_GuiHost.Document.GetElementById("axisnamelabelX") != null)
                  {
                      if (_GuiHost.Document.GetElementById("axisindexX") != null)
                      {
                          if (Int32.TryParse(_GuiHost.GetAttribute("axisindexX", Attribs.Value), out indexvalue))
                          {
                              InterpreterDataFile.XaxisIndex = indexvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axiscountsX") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axiscountsX", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.XaxisCPI = offsetvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axisaccelX") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axisaccelX", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.XaxisAccel = offsetvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axisvelX") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axisvelX", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.XaxisVel = offsetvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axisjogvelX") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axisjogvelX", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.XaxisJogVel = offsetvalue;
                          }
                      }
                  }
                  if (_GuiHost.Document.GetElementById("axisnamelabelY") != null)
                  {
                      if (_GuiHost.Document.GetElementById("axisindexY") != null)
                      {
                          if (Int32.TryParse(_GuiHost.GetAttribute("axisindexY", Attribs.Value), out indexvalue))
                          {
                              InterpreterDataFile.YaxisIndex = indexvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axiscountsY") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axiscountsY", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.YaxisCPI = offsetvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axisaccelY") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axisaccelY", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.YaxisAccel = offsetvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axisvelY") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axisvelY", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.YaxisVel = offsetvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axisjogvelY") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axisjogvelY", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.YaxisJogVel = offsetvalue;
                          }
                      }
                  }
                  if (_GuiHost.Document.GetElementById("axisnamelabelZ") != null)
                  {
                      if (_GuiHost.Document.GetElementById("axisindexZ") != null)
                      {
                          if (Int32.TryParse(_GuiHost.GetAttribute("axisindexZ", Attribs.Value), out indexvalue))
                          {
                              InterpreterDataFile.ZaxisIndex = indexvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axiscountsZ") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axiscountsZ", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.ZaxisCPI = offsetvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axisaccelZ") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axisaccelZ", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.ZaxisAccel = offsetvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axisvelZ") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axisvelZ", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.ZaxisVel = offsetvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axisjogvelZ") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axisjogvelZ", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.ZaxisJogVel = offsetvalue;
                          }
                      }
                  }
                  if (_GuiHost.Document.GetElementById("axisnamelabelA") != null)
                  {
                      if (_GuiHost.Document.GetElementById("axisindexA") != null)
                      {
                          if (Int32.TryParse(_GuiHost.GetAttribute("axisindexA", Attribs.Value), out indexvalue))
                          {
                              InterpreterDataFile.AaxisIndex = indexvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axiscountsA") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axiscountsA", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.AaxisCPI = offsetvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axisaccelA") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axisaccelA", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.AaxisAccel = offsetvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axisvelA") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axisvelA", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.AaxisVel = offsetvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axisjogvelA") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axisjogvelA", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.AaxisJogVel = offsetvalue;
                          }
                      }
                  }
                  if (_GuiHost.Document.GetElementById("axisnamelabelB") != null)
                  {
                      if (_GuiHost.Document.GetElementById("axisindexB") != null)
                      {
                          if (Int32.TryParse(_GuiHost.GetAttribute("axisindexB", Attribs.Value), out indexvalue))
                          {
                              InterpreterDataFile.BaxisIndex = indexvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axiscountsB") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axiscountsB", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.BaxisCPI = offsetvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axisaccelB") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axisaccelB", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.BaxisAccel = offsetvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axisvelB") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axisvelB", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.BaxisVel = offsetvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axisjogvelB") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axisjogvelB", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.BaxisJogVel = offsetvalue;
                          }
                      }
                  }
                  if (_GuiHost.Document.GetElementById("axisnamelabelC") != null)
                  {
                      if (_GuiHost.Document.GetElementById("axisindexC") != null)
                      {
                          if (Int32.TryParse(_GuiHost.GetAttribute("axisindexC", Attribs.Value), out indexvalue))
                          {
                              InterpreterDataFile.CaxisIndex = indexvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axiscountsC") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axiscountsC", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.CaxisCPI = offsetvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axisaccelC") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axisaccelC", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.CaxisAccel = offsetvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axisvelC") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axisvelC", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.CaxisVel = offsetvalue;
                          }
                      }
                      if (_GuiHost.Document.GetElementById("axisjogvelC") != null)
                      {
                          if (Double.TryParse(_GuiHost.GetAttribute("axisjogvelC", Attribs.Value), out offsetvalue))
                          {
                              InterpreterDataFile.CaxisJogVel = offsetvalue;
                          }
                      }
                  }
              }));
        }


        /// <summary>
        /// Transfers command data from the data file to the HTML within the supplied range of elements
        /// </summary>
        /// <param name="start">beginning command to load</param>
        /// <param name="count">last command to update</param>
        public void LoadCommandEditor(int start, int count)
        {
            bool needssave = false;
            UpdateDocument(new Action(delegate()
            {
                for (int i = start; i <= count; i++)
                {
                    var elementname = String.Format("editcommand{0}", i);
                    var element = _GuiHost.Document.GetElementById(elementname);
                    if (element != null)
                    {
                        var command = InterpreterDataFile.Commands.FirstOrDefault(x => x.Index == i);
                        if (command == null)
                        {
                            command = new CommandData { Index = i };
                            InterpreterDataFile.Commands.Add(command);
                            needssave = true;
                        }
                        var showindex = Convert.ToBoolean(_GuiHost.GetAttribute(elementname, Attribs.ShowCommandIndex));
                        var editname = Convert.ToBoolean(_GuiHost.GetAttribute(elementname, Attribs.EditCommandName));
                        if (!editname)
                        {
                            command.Name = _GuiHost.GetAttribute(elementname, Attribs.CommandName);
                        }
                        var editor = GetEditorForCommand(command, showindex, editname);
                        _GuiHost.SetElementHTML(elementname, editor);
           
                      
                        _GuiHost.SetAttribute(string.Format("commandtype{0}", i), Attribs.Value, command.Type.ToString());
                    } 
                }
            }));
            if (needssave)
            {
                SaveInterpreterData();
            }
        }

        /// <summary>
        /// Updates the data file commands from the HTML document within the supplied range 
        /// </summary>
        /// <param name="start">beginning command to save</param>
        /// <param name="count">last command to update </param>
        public void SaveCommandEditor(int start, int count)
        {
            UpdateDocument(new Action(delegate()
            { 
                for (int i = start; i <= count; i++)
                {
                    var elementname = String.Format("editcommand{0}", i);
                    var element = _GuiHost.Document.GetElementById(elementname);
                    if (element != null)
                    { 
                        int indexvalue = -1;
                        var command = InterpreterDataFile.Commands.FirstOrDefault(x => x.Index == i);
                        if (command == null)
                        {
                            command = new CommandData { Index = i };
                            InterpreterDataFile.Commands.Add(command);
                        }
                        if (_GuiHost.Document.GetElementById(String.Format("commandname{0}", i)) != null)
                        {
                            command.Name = _GuiHost.GetAttribute(String.Format("commandname{0}", i), Attribs.Value);
                        }
                        if (_GuiHost.Document.GetElementById(String.Format("commandtype{0}", i)) != null)
                        {
                            var typeofcommand = _GuiHost.GetAttribute(String.Format("commandtype{0}", i), Attribs.Value).ToString();
                            var commandtype = (MCODE_TYPE)Enum.Parse(typeof(MCODE_TYPE), typeofcommand);
                            command.Type = commandtype; 
                        }
                        if (_GuiHost.Document.GetElementById(String.Format("commandsetbit1{0}", i)) != null)
                        {
                            if (Int32.TryParse(_GuiHost.GetAttribute(String.Format("commandsetbit1{0}", i), Attribs.Value), out indexvalue))
                            {
                                command.Bit1 = indexvalue;
                            }
                        }
                        if (_GuiHost.Document.GetElementById(String.Format("commandstatebit1{0}", i)) != null)
                        {
                            if (Int32.TryParse(_GuiHost.GetAttribute(String.Format("commandstatebit1{0}", i), Attribs.Value), out indexvalue))
                            {
                                command.State1 = indexvalue > 0;
                            }
                        }
                        if (_GuiHost.Document.GetElementById(String.Format("commandsetbit2{0}", i)) != null)
                        {
                            if (Int32.TryParse(_GuiHost.GetAttribute(String.Format("commandsetbit2{0}", i), Attribs.Value), out indexvalue))
                            {
                                command.Bit2 = indexvalue;
                            }
                        }
                        if (_GuiHost.Document.GetElementById(String.Format("commandstatebit2{0}", i)) != null)
                        {
                            if (Int32.TryParse(_GuiHost.GetAttribute(String.Format("commandstatebit2{0}", i), Attribs.Value), out indexvalue))
                            {
                                command.State2 = indexvalue > 0;
                            }
                        }

                        if (_GuiHost.Document.GetElementById(String.Format("commandsetdacbit{0}", i)) != null)
                        {
                            if (Int32.TryParse(_GuiHost.GetAttribute(String.Format("commandsetdacbit{0}", i), Attribs.Value), out indexvalue))
                            {
                                command.Bit1 = indexvalue;
                            }
                        }

                        if (_GuiHost.Document.GetElementById(String.Format("commanddacscale{0}", i)) != null)
                        {
                            if (Int32.TryParse(_GuiHost.GetAttribute(String.Format("commanddacscale{0}", i), Attribs.Value), out indexvalue))
                            {
                                command.DacScale = indexvalue;
                            }
                        }
                        if (_GuiHost.Document.GetElementById(String.Format("commanddacoffset{0}", i)) != null)
                        {
                            if (Int32.TryParse(_GuiHost.GetAttribute(String.Format("commanddacoffset{0}", i), Attribs.Value), out indexvalue))
                            {
                                command.DacOffset = indexvalue;
                            }
                        }
                        if (_GuiHost.Document.GetElementById(String.Format("commanddacmin{0}", i)) != null)
                        {
                            if (Int32.TryParse(_GuiHost.GetAttribute(String.Format("commanddacmin{0}", i), Attribs.Value), out indexvalue))
                            {
                                command.DacMin = indexvalue;
                            }
                        }
                        if (_GuiHost.Document.GetElementById(String.Format("commanddacmax{0}", i)) != null)
                        {
                            if (Int32.TryParse(_GuiHost.GetAttribute(String.Format("commanddacmax{0}", i), Attribs.Value), out indexvalue))
                            {
                                command.DacMax = indexvalue;
                            }
                        }
                        if (_GuiHost.Document.GetElementById(String.Format("commandthread{0}", i)) != null)
                        {
                            if (Int32.TryParse(_GuiHost.GetAttribute(String.Format("commandthread{0}", i), Attribs.Value), out indexvalue))
                            {
                                command.Thread = indexvalue;
                            }
                        }
                        if (_GuiHost.Document.GetElementById(String.Format("commandvar{0}", i)) != null)
                        {
                            if (Int32.TryParse(_GuiHost.GetAttribute(String.Format("commandvar{0}", i), Attribs.Value), out indexvalue))
                            {
                                command.VAR = indexvalue;
                            }
                        }
                        if (_GuiHost.Document.GetElementById(String.Format("commandfilename{0}", i)) != null)
                        {
                            var filename = _GuiHost.GetAttribute(String.Format("commandfilename{0}", i), Attribs.Value).ToString();

                            command.FileName = filename;
                        }
                    }
                }
            }));
        }
 
        /// <summary>
        /// Opens a file dialog and inserts the selection into the command's filename property
        /// </summary>
        /// <param name="commandindex">command to set file name for</param>
        public void SetCommandFileName(int commandindex)
        {
            UpdateDocument(new Action(delegate()
           {
               if (_GuiHost.Document.GetElementById(String.Format("commandfilename{0}", commandindex)) != null)
               { 
                   var ofd = new OpenFileDialog();
                   ofd.Multiselect = false;
                   if (ofd.ShowDialog() == DialogResult.OK)
                   {
                       var filename = ofd.FileName;
                       if (File.Exists(filename))
                       { 
                           _GuiHost.SetAttribute(String.Format("commandfilename{0}", commandindex), Attribs.Value, filename);
 
                       }
                   }
               }
           }));
        }

        /// <summary>
        /// Generates the specific editor for a Mcode Action command based on its type
        /// </summary>
        /// <param name="command">command source to generate editor for</param>
        /// <param name="showindex">flag to determine whether or not to create a p element displaying the command index</param>
        /// <param name="editname">flag to determine whether the command name text is editable or not</param>
        /// <returns>html command editor</returns>
        public string GetEditorForCommand(CommandData command, bool showindex,  bool editname)
        {
            StringBuilder sb = new StringBuilder();

            sb.AppendLine("<div class=\"editorforcommand\">");

            if (showindex)
            {
                sb.AppendLine(String.Format("<p class=\"commandlabel\" id=\"commandindexlabel{0}\">{0}</p>", command.Index));
            }

            if (editname)
            {
                sb.AppendLine("<p class=\"commanddescription\">Name:</p>");
                sb.AppendLine(String.Format("<input type=\"text\" class=\"commandtext\" id=\"commandname{0}\" value=\"{1}\"/>", command.Index, command.Name));
            }
            else
            {
                sb.AppendLine(String.Format("<p class=\"commandlabel\" id=\"commandnamelabel{0}\">{1} :</p>", command.Index, command.Name));
            } 


            //sb.AppendLine(String.Format("<select class=\"commandtype\" id=\"commandtype{0}\">", command.Index));
            //sb.AppendLine("<option isselected value=\"M_Action_None\">None</option>");
            //sb.AppendLine("<option value=\"M_Action_Setbit\">Set Bit</option>");
            //sb.AppendLine("<option value=\"M_Action_SetTwoBits\">Set 2 Bits</option>");
            //sb.AppendLine("<option value=\"M_Action_DAC\">DAC</option>");
            //sb.AppendLine("<option value=\"M_Action_Program\">Run Program</option>");
            //sb.AppendLine("<option value=\"M_Action_Program_wait\">Run/Wait</option>");
            //sb.AppendLine("<option value=\"M_Action_Program_wait_sync\">Run/Wait/Sync</option>");
            //sb.AppendLine("<option value=\"M_Action_Program_PC\">Run EXE</option>");
            //sb.AppendLine("<option value=\"M_Action_Callback\">User Callback</option>");
            //sb.AppendLine("<option value=\"M_Action_Waitbit\">Wait Bit</option>");
            //sb.AppendLine("</select>"); 

                                  
            switch (command.Type)
            {
                case MCODE_TYPE.M_Action_Setbit:
                    sb.AppendLine("<p class=\"commanddescription\">Set Bit</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandtext\" id=\"commandsetbit1{0}\" value=\"{1}\"/>", command.Index, command.Bit1));
                    sb.AppendLine("<p class=\"commanddescription\">to</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandtext\" id=\"commandstatebit1{0}\" value=\"{1}\"/>", command.Index, command.State1 ? 1 : 0));
                    break;
                case MCODE_TYPE.M_Action_SetTwoBits:
                    sb.AppendLine("<p class=\"commanddescription\">Set Bit</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandtext\" id=\"commandsetbit1{0}\" value=\"{1}\"/>", command.Index, command.Bit1));
                    sb.AppendLine("<p class=\"commanddescription\">to</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandtext\" id=\"commandstatebit1{0}\" value=\"{1}\"/>", command.Index, command.State1 ? 1 : 0));
                    sb.AppendLine("<p class=\"commanddescription\">and</p>");
                    sb.AppendLine("<p class=\"commanddescription\">Set Bit</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandtext\" id=\"commandsetbit2{0}\" value=\"{1}\"/>", command.Index, command.Bit2));
                    sb.AppendLine("<p class=\"commanddescription\">to</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandtext\" id=\"commandstatebit2{0}\" value=\"{1}\"/>", command.Index, command.State2 ? 1 : 0));
                    break;
                case MCODE_TYPE.M_Action_DAC:
                    sb.AppendLine("<p class=\"commanddescription\">Set DAC</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandtext\" id=\"commandsetdacbit{0}\" value=\"{1}\"/>", command.Index, command.Bit1));
                    sb.AppendLine("<p class=\"commanddescription\">scale</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandtext\" id=\"commanddacscale{0}\" value=\"{1}\"/>", command.Index, command.DacScale));
                    sb.AppendLine("<p class=\"commanddescription\">offset</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandtext\" id=\"commanddacoffset{0}\" value=\"{1}\"/>", command.Index, command.DacOffset));
                    sb.AppendLine("<p class=\"commanddescription\">min</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandtext\" id=\"commanddacmin{0}\" value=\"{1}\"/>", command.Index, command.DacMin));
                    sb.AppendLine("<p class=\"commanddescription\">max</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandtext\" id=\"commanddacmax{0}\" value=\"{1}\"/>", command.Index, command.DacMax));
                    break;
                case MCODE_TYPE.M_Action_Program:
                    sb.AppendLine("<p class=\"commanddescription\">Thread</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandtext\" id=\"commandthread{0}\" value=\"{1}\"/>", command.Index, command.Thread));
                    sb.AppendLine("<p class=\"commanddescription\">VAR</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandtext\" id=\"commandvar{0}\" value=\"{1}\"/>", command.Index, command.VAR));
                    sb.AppendLine("<p class=\"commanddescription\">File</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandfiletext\" id=\"commandfilename{0}\" value=\"{1}\"/>", command.Index, command.FileName));
                    sb.AppendLine(String.Format("<button class=\"commandbutton\" id=\"commandgetfilename{0}\" data-commandid=\"{0}\">{1}</button>", command.Index, "..."));
                    break;
                case MCODE_TYPE.M_Action_Program_wait:
                    sb.AppendLine("<p class=\"commanddescription\">Thread</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandtext\" id=\"commandthread{0}\" value=\"{1}\"/>", command.Index, command.Thread));
                    sb.AppendLine("<p class=\"commanddescription\">VAR</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandtext\" id=\"commandvar{0}\" value=\"{1}\"/>", command.Index, command.VAR));
                    sb.AppendLine("<p class=\"commanddescription\">File</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandfiletext\" id=\"commandfilename{0}\" value=\"{1}\"/>", command.Index, command.FileName));
                    sb.AppendLine(String.Format("<button class=\"commandbutton\" id=\"commandgetfilename{0}\" data-commandid=\"{0}\">{1}</button>", command.Index, "..."));
                    break;
                case MCODE_TYPE.M_Action_Program_wait_sync:
                    sb.AppendLine("<p class=\"commanddescription\">Thread</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandtext\" id=\"commandthread{0}\" value=\"{1}\"/>", command.Index, command.Thread));
                    sb.AppendLine("<p class=\"commanddescription\">VAR</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandtext\" id=\"commandvar{0}\" value=\"{1}\"/>", command.Index, command.VAR));
                    sb.AppendLine("<p class=\"commanddescription\">File</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandfiletext\" id=\"commandfilename{0}\" value=\"{1}\"/>", command.Index, command.FileName));
                    sb.AppendLine(String.Format("<button class=\"commandbutton\" id=\"commandgetfilename{0}\" data-commandid=\"{0}\">{1}</button>", command.Index, "..."));
                    break;
                case MCODE_TYPE.M_Action_Program_PC:
                    sb.AppendLine("<p class=\"commanddescription\">File</p>");
                    sb.AppendLine(String.Format("<input type=\"text\" class=\"commandfiletext\" id=\"commandfilename{0}\" value=\"{1}\"/>", command.Index, command.FileName));
                    sb.AppendLine(String.Format("<button class=\"commandbutton\" id=\"commandgetfilename{0}\" data-commandid=\"{0}\">{1}</button>", command.Index, "..."));
                    break;
                default:
                    break;
            }
            sb.AppendLine("</div>");
            return sb.ToString();
        }

        /// <summary>
        /// Helper method to generate MCode Actions for an HTML select element
        /// </summary>
        /// <returns>MCode Action enumeration in HTML option format</returns>
        public string GetCommandOptions()
        {

            StringBuilder sb = new StringBuilder();
            sb.AppendLine("<option value=\"M_Action_None\">None</option>");
            sb.AppendLine("<option value=\"M_Action_Setbit\">Set Bit</option>");
            sb.AppendLine("<option value=\"M_Action_SetTwoBits\">Set 2 Bits</option>");
            sb.AppendLine("<option value=\"M_Action_DAC\">DAC</option>");
            sb.AppendLine("<option value=\"M_Action_Program\">Run Program</option>");
            sb.AppendLine("<option value=\"M_Action_Program_wait\">Run/Wait</option>");
            sb.AppendLine("<option value=\"M_Action_Program_wait_sync\">Run/Wait/Sync</option>");
            sb.AppendLine("<option value=\"M_Action_Program_PC\">Run EXE</option>");
            sb.AppendLine("<option value=\"M_Action_Callback\">User Callback</option>");
            sb.AppendLine("<option value=\"M_Action_Waitbit\">Wait Bit</option>");

            return sb.ToString();
        }

        /// <summary>
        /// Updates the button text for user commands within the supplied range of elements
        /// </summary>
        /// <param name="start">beginning button to update</param>
        /// <param name="count">last button to update </param>
        public void UpdateCommandButtonText(int start, int count)
        {
            UpdateDocument(new Action(delegate()
            {
                for (int i = start; i <= count; i++)
                {
                    if (_GuiHost.Document.GetElementById(String.Format("userbutton{0}", i)) != null)
                    {
                        var command = InterpreterDataFile.Commands.FirstOrDefault(x => x.Index == i);
                        if (command != null)
                        {
                            if (command.Name != "")
                            {
                                _GuiHost.SetElementText(String.Format("userbutton{0}", i), command.Name);
                            }
                        }
                    }
                }
            }));
        }


        /// <summary>
        /// Transfers tool data from the data file to the HTML within the supplied range of elements
        /// </summary>
        /// <param name="start">beginning tool to load</param>
        /// <param name="count">last tool to load </param>
        public void LoadToolEditor(int start, int count)
        {
            bool needssave = false;
            UpdateDocument(new Action(delegate()
                {
                    for (int i = start; i <= count; i++)
                    {
                        var element = _GuiHost.Document.GetElementById(String.Format("toolname{0}", i));
                        if (element != null)
                        {
                            var tool = InterpreterDataFile.Tools.FirstOrDefault(x => x.Index == i);
                            if (tool == null)
                            {
                                tool = new ToolData { Index = i };
                                InterpreterDataFile.Tools.Add(tool);
                                needssave = true;
                            }
                            _GuiHost.SetAttribute(String.Format("toolname{0}", i), Attribs.Value, tool.ToolName);
                            _GuiHost.SetAttribute(String.Format("toolslot{0}", i), Attribs.Value, tool.SlotNumber);
                            _GuiHost.SetAttribute(String.Format("toolid{0}", i), Attribs.Value, tool.ToolNumber);
                            _GuiHost.SetAttribute(String.Format("toollength{0}", i), Attribs.Value, tool.LengthOffset);
                            _GuiHost.SetAttribute(String.Format("tooldiameter{0}", i), Attribs.Value, tool.DiameterOffset);
                        }
                    }
                }));
            if (needssave)
            {
                SaveInterpreterData();
            }
        }

        /// <summary>
        /// Updates the data file tools from the HTML document within the supplied range 
        /// </summary>
        /// <param name="start">beginning tool to save</param>
        /// <param name="count">last tool to update </param>
        public void SaveToolEditor(int start, int count)
        {
            UpdateDocument(new Action(delegate()
            {
                for (int i = start; i <= count; i++)
                {
                    var element = _GuiHost.Document.GetElementById(String.Format("toolname{0}", i));
                    if (element != null)
                    {
                        var tool = InterpreterDataFile.Tools.FirstOrDefault(x => x.Index == i);
                        if (tool == null)
                        {
                            tool = new ToolData { Index = i };
                            InterpreterDataFile.Tools.Add(tool);
                        }
                        double offsetvalue = 0.0;
                        int indexvalue = 0;

                        tool.ToolName = _GuiHost.GetAttribute(String.Format("toolname{0}", i), Attribs.Value);
                        if (_GuiHost.Document.GetElementById(String.Format("toolslot{0}", i)) != null)
                        {
                            if (Int32.TryParse(_GuiHost.GetAttribute(String.Format("toolslot{0}", i), Attribs.Value), out indexvalue))
                            {
                                tool.SlotNumber = indexvalue;
                            }
                        }
                        if (_GuiHost.Document.GetElementById(String.Format("toolid{0}", i)) != null)
                        {
                            if (Int32.TryParse(_GuiHost.GetAttribute(String.Format("toolid{0}", i), Attribs.Value), out indexvalue))
                            {
                                tool.ToolNumber = indexvalue;
                            }
                        }
                        if (_GuiHost.Document.GetElementById(String.Format("toollength{0}", i)) != null)
                        {
                            if (Double.TryParse(_GuiHost.GetAttribute(String.Format("toollength{0}", i), Attribs.Value), out offsetvalue))
                            {
                                tool.LengthOffset = offsetvalue;
                            }
                        }
                        if (_GuiHost.Document.GetElementById(String.Format("tooldiameter{0}", i)) != null)
                        {
                            if (Double.TryParse(_GuiHost.GetAttribute(String.Format("tooldiameter{0}", i), Attribs.Value), out offsetvalue))
                            {
                                tool.DiameterOffset = offsetvalue;
                            }
                        }  
                    }
                }
            })); 
        }

        /// <summary>
        /// Transfers offset data from the data file to the HTML within the supplied range of elements
        /// </summary>
        /// <param name="start">beginning offset to load</param>
        /// <param name="count">last offset to load </param>
        public void LoadOffsetEditor(int start, int count)
        {
            bool needssave = false;
            UpdateDocument(new Action(delegate()
            {
                for (int i = start; i <= count; i++)
                {
                    var element = _GuiHost.Document.GetElementById(String.Format("offsetname{0}", i));
                    if (element != null)
                    {
                        var offset = InterpreterDataFile.Offsets.FirstOrDefault(x => x.Index == i);
                        if (offset == null)
                        {
                            offset = new OffsetData { Index = i };
                            InterpreterDataFile.Offsets.Add(offset);
                            needssave = true;
                        }
                        _GuiHost.SetAttribute(String.Format("offsetname{0}", i), Attribs.Value, offset.Name);
                        _GuiHost.SetAttribute(String.Format("offsetid{0}", i), Attribs.Value, offset.Index);
                        _GuiHost.SetAttribute(String.Format("offsetX{0}", i), Attribs.Value, offset.X);
                        _GuiHost.SetAttribute(String.Format("offsetY{0}", i), Attribs.Value, offset.Y);
                        _GuiHost.SetAttribute(String.Format("offsetZ{0}", i), Attribs.Value, offset.Z);
                        _GuiHost.SetAttribute(String.Format("offsetA{0}", i), Attribs.Value, offset.A);
                        _GuiHost.SetAttribute(String.Format("offsetB{0}", i), Attribs.Value, offset.B);
                        _GuiHost.SetAttribute(String.Format("offsetC{0}", i), Attribs.Value, offset.C);
                    }
                    else
                    {
                        element = _GuiHost.Document.GetElementById(String.Format("offsetlabel{0}", i));
                        if (element != null)
                        {
                            var offset = InterpreterDataFile.Offsets.FirstOrDefault(x => x.Index == i);
                            if (offset == null)
                            {
                                offset = new OffsetData { Index = i };
                                InterpreterDataFile.Offsets.Add(offset);
                                needssave = true;
                            }
                            _GuiHost.SetAttribute(String.Format("offsetX{0}", i), Attribs.Value, offset.X);
                            _GuiHost.SetAttribute(String.Format("offsetY{0}", i), Attribs.Value, offset.Y);
                            _GuiHost.SetAttribute(String.Format("offsetZ{0}", i), Attribs.Value, offset.Z);
                            _GuiHost.SetAttribute(String.Format("offsetA{0}", i), Attribs.Value, offset.A);
                            _GuiHost.SetAttribute(String.Format("offsetB{0}", i), Attribs.Value, offset.B);
                            _GuiHost.SetAttribute(String.Format("offsetC{0}", i), Attribs.Value, offset.C);
                        }
                    }
                }
            }));
            if (needssave)
            {
                SaveInterpreterData();
            }
        }
       
        /// <summary>
        /// Updates the data file offsets from the HTML document within the supplied range 
        /// </summary>
        /// <param name="start">beginning offset to save</param>
        /// <param name="count">last offset to update </param>
        public void SaveOffsetEditor(int start, int count)
        {
            UpdateDocument(new Action(delegate()
            {
                for (int i = start; i <= count; i++)
                { 
                    if (_GuiHost.Document.GetElementById(String.Format("offsetname{0}", i)) != null
                        || _GuiHost.Document.GetElementById(String.Format("offsetlabel{0}", i)) != null)
                    {
                        var offset = InterpreterDataFile.Offsets.FirstOrDefault(x => x.Index == i);
                        if (offset == null)
                        {
                            offset = new OffsetData { Index = i };
                            InterpreterDataFile.Offsets.Add(offset);
                        }
                        double offsetvalue = 0.0;
                        int indexvalue = 0;

                        if (_GuiHost.Document.GetElementById(String.Format("offsetname{0}", i)) != null)
                        {
                            offset.Name = _GuiHost.GetAttribute(String.Format("offsetname{0}", i), Attribs.Value);
                        }
                        else if (_GuiHost.Document.GetElementById(String.Format("offsetlabel{0}", i)) != null)
                        {
                            offset.Name = _GuiHost.GetElementText(String.Format("offsetname{0}", i));
                        }


                        if (_GuiHost.Document.GetElementById(String.Format("offsetid{0}", i)) != null)
                        {
                            if (Int32.TryParse(_GuiHost.GetAttribute(String.Format("offsetid{0}", i), Attribs.Value), out indexvalue))
                            {
                                offset.Index = indexvalue;
                            }
                        }
                        else
                        {
                            offset.Index = i;
                        }

                        if (_GuiHost.Document.GetElementById(String.Format("offsetX{0}", i)) != null)
                        {
                            if (Double.TryParse(_GuiHost.GetAttribute(String.Format("offsetX{0}", i), Attribs.Value), out offsetvalue))
                            {
                                offset.X = offsetvalue;
                            }
                        }
                        if (_GuiHost.Document.GetElementById(String.Format("offsetY{0}", i)) != null)
                        {
                            if (Double.TryParse(_GuiHost.GetAttribute(String.Format("offsetY{0}", i), Attribs.Value), out offsetvalue))
                            {
                                offset.Y = offsetvalue;
                            }
                        }
                        if (_GuiHost.Document.GetElementById(String.Format("offsetZ{0}", i)) != null)
                        {
                            if (Double.TryParse(_GuiHost.GetAttribute(String.Format("offsetZ{0}", i), Attribs.Value), out offsetvalue))
                            {
                                offset.Z = offsetvalue;
                            }
                        }
                        if (_GuiHost.Document.GetElementById(String.Format("offsetA{0}", i)) != null)
                        {
                            if (Double.TryParse(_GuiHost.GetAttribute(String.Format("offsetA{0}", i), Attribs.Value), out offsetvalue))
                            {
                                offset.A = offsetvalue;
                            }
                        }
                        if (_GuiHost.Document.GetElementById(String.Format("offsetB{0}", i)) != null)
                        {
                            if (Double.TryParse(_GuiHost.GetAttribute(String.Format("offsetB{0}", i), Attribs.Value), out offsetvalue))
                            {
                                offset.B = offsetvalue;
                            }
                        }
                        if (_GuiHost.Document.GetElementById(String.Format("offsetC{0}", i)) != null)
                        {
                            if (Double.TryParse(_GuiHost.GetAttribute(String.Format("offsetC{0}", i), Attribs.Value), out offsetvalue))
                            {
                                offset.C = offsetvalue;
                            }
                        } 
                    }
                }
            }));
        }

        /// <summary>
        /// Persists Interpreter data to the file
        /// </summary>
        public void SaveInterpreterData()
        {

            var datafile = Path.Combine(_SystemPaths["AppData"], "InterpreterData.xml");

            InterpreterData.SaveXML(datafile, InterpreterDataFile);
        }

        /// <summary>
        /// This will update the Interpreter TP params, axis, offset, tools and MCode Actions from the data file
        /// </summary>
        public void InitInterpreterData()
        { 
            double p0 = 0;
            double p1 = 0;
            double p2 = 0;
            double p3 = 0;
            double p4 = 0; 
            string sdata = "";
            bool init = true;


            _Controller.CoordMotion.MotionParams.TPLookahead = InterpreterDataFile.LookAhead;
            _Controller.CoordMotion.MotionParams.BreakAngle = InterpreterDataFile.BreakAngle;
            _Controller.CoordMotion.MotionParams.CollinearTolerance = InterpreterDataFile.CollinearTolerance;
            _Controller.CoordMotion.MotionParams.CornerTolerance = InterpreterDataFile.CornerTolerance;
            _Controller.CoordMotion.MotionParams.FacetAngle = InterpreterDataFile.FacetAngle;

            _Controller.CoordMotion.MotionParams.CountsPerInchX = InterpreterDataFile.XaxisCPI;
            _Controller.CoordMotion.MotionParams.CountsPerInchY = InterpreterDataFile.YaxisCPI;
            _Controller.CoordMotion.MotionParams.CountsPerInchZ = InterpreterDataFile.ZaxisCPI;
            _Controller.CoordMotion.MotionParams.CountsPerInchA = InterpreterDataFile.AaxisCPI;
            _Controller.CoordMotion.MotionParams.CountsPerInchB = InterpreterDataFile.BaxisCPI;
            _Controller.CoordMotion.MotionParams.CountsPerInchC = InterpreterDataFile.CaxisCPI;

            _Controller.CoordMotion.MotionParams.MaxVelX = InterpreterDataFile.XaxisVel;
            _Controller.CoordMotion.MotionParams.MaxVelY = InterpreterDataFile.YaxisVel;
            _Controller.CoordMotion.MotionParams.MaxVelZ = InterpreterDataFile.ZaxisVel;
            _Controller.CoordMotion.MotionParams.MaxVelA = InterpreterDataFile.AaxisVel;
            _Controller.CoordMotion.MotionParams.MaxVelB = InterpreterDataFile.BaxisVel;
            _Controller.CoordMotion.MotionParams.MaxVelC = InterpreterDataFile.CaxisVel;

            _Controller.CoordMotion.MotionParams.MaxAccelX = InterpreterDataFile.XaxisAccel;
            _Controller.CoordMotion.MotionParams.MaxAccelY = InterpreterDataFile.YaxisAccel;
            _Controller.CoordMotion.MotionParams.MaxAccelZ = InterpreterDataFile.ZaxisAccel;
            _Controller.CoordMotion.MotionParams.MaxAccelA = InterpreterDataFile.AaxisAccel;
            _Controller.CoordMotion.MotionParams.MaxAccelB = InterpreterDataFile.BaxisAccel;
            _Controller.CoordMotion.MotionParams.MaxAccelC = InterpreterDataFile.CaxisAccel;

            InitAxis();
           

            foreach (var offset in InterpreterDataFile.Offsets)
            {
                _Controller.CoordMotion.Interpreter.SetOrigin(offset.Index, offset.X, offset.Y, offset.Z, offset.A, offset.B, offset.C);
            }
            foreach (var tool in InterpreterDataFile.Tools)
            {
                _Controller.CoordMotion.Interpreter.SetupParams.SetTool(tool.ToolNumber, tool.LengthOffset, tool.DiameterOffset);
            }
            foreach (var command in InterpreterDataFile.Commands)
            {
                init = true;
                switch (command.Type)
                {
                    case MCODE_TYPE.M_Action_Callback:

                        break;
                    case MCODE_TYPE.M_Action_DAC:
                        p0 = command.Bit1;
                        p1 = command.DacScale;
                        p2 = command.DacOffset; 
                        p3 = command.DacMin;
                        p4 = command.DacMax;
                        break;
                    case MCODE_TYPE.M_Action_Program:
                        p0 = command.Thread;
                        p1 = command.VAR;

                        sdata = command.FileName;
                        break;
                    case MCODE_TYPE.M_Action_Program_PC:
                        p0 = command.Thread;
                        p1 = command.VAR;

                        sdata = command.FileName;
                        break;
                    case MCODE_TYPE.M_Action_Program_wait:
                        p0 = command.Thread;
                        p1 = command.VAR;

                        sdata = command.FileName;
                        break;
                    case MCODE_TYPE.M_Action_Program_wait_sync:

                        sdata = command.FileName;
                        break;
                    case MCODE_TYPE.M_Action_Setbit:
                        p0 = command.Bit1;
                        p1 = command.State1 ? 1 : 0;
                        break;
                    case MCODE_TYPE.M_Action_Waitbit:
                        p0 = command.Bit1;
                        p1 = command.State1 ? 1 : 0;
                        break;
                    case MCODE_TYPE.M_Action_SetTwoBits:
                        p0 = command.Bit1;
                        p1 = command.State1 ? 1:0;
                        p2 = command.Bit2;
                        p3 = command.State2 ? 1:0;
                        break;
                    default:
                        init = false;
                        break;
                }
                if (init)
                {
                    _Controller.CoordMotion.Interpreter.SetMcodeAction(
                        command.Index, command.Type, p0, p1, p2, p3, p4, sdata);
                }
            }
        }

        /// <summary>
        /// This method will create/update individual axis objects based on the Interpreter axis settings
        /// </summary>
        public void InitAxis()
        {
            if (IsConnected)
            {
                _Controller.WriteLine(String.Format("DefineCS = {0} {1} {2} {3} {4} {5}",
                   InterpreterDataFile.XaxisIndex, InterpreterDataFile.YaxisIndex, InterpreterDataFile.ZaxisIndex,
                   InterpreterDataFile.AaxisIndex, InterpreterDataFile.BaxisIndex, InterpreterDataFile.CaxisIndex));
            }

            if (InterpreterDataFile.XaxisIndex > -1)
            {
                if (!_AxisContainer.ContainsKey("X"))
                {
                    _AxisContainer.Add("X", new AxisNode(new KM_Axis(_Controller, InterpreterDataFile.XaxisIndex, "X")));
                }

                _AxisContainer["X"].Axis.ID = InterpreterDataFile.XaxisIndex;
                _AxisContainer["X"].JogVelocity = InterpreterDataFile.XaxisJogVel;
                _AxisContainer["X"].Axis.Velocity = InterpreterDataFile.XaxisVel;
                _AxisContainer["X"].Axis.Acceleration = InterpreterDataFile.XaxisAccel;
                _AxisContainer["X"].Axis.CPU = InterpreterDataFile.XaxisCPI;
            }
            else
            {
                if (_AxisContainer.ContainsKey("X"))
                {
                    _AxisContainer.Remove("X");
                }
            }

            if (InterpreterDataFile.YaxisIndex > -1)
            {
                if (!_AxisContainer.ContainsKey("Y"))
                {
                    _AxisContainer.Add("Y", new AxisNode(new KM_Axis(_Controller, InterpreterDataFile.YaxisIndex, "Y")));
                }

                _AxisContainer["Y"].Axis.ID = InterpreterDataFile.YaxisIndex;
                _AxisContainer["Y"].JogVelocity = InterpreterDataFile.YaxisJogVel;
                _AxisContainer["Y"].Axis.Velocity = InterpreterDataFile.YaxisVel;
                _AxisContainer["Y"].Axis.Acceleration = InterpreterDataFile.YaxisAccel;
                _AxisContainer["Y"].Axis.CPU = InterpreterDataFile.YaxisCPI;
            }
            else
            {
                if (_AxisContainer.ContainsKey("Y"))
                {
                    _AxisContainer.Remove("Y");
                }
            }

            if (InterpreterDataFile.ZaxisIndex > -1)
            {
                if (!_AxisContainer.ContainsKey("Z"))
                {
                    _AxisContainer.Add("Z", new AxisNode(new KM_Axis(_Controller, InterpreterDataFile.ZaxisIndex, "Z")));
                }

                _AxisContainer["Z"].Axis.ID = InterpreterDataFile.ZaxisIndex;
                _AxisContainer["Z"].JogVelocity = InterpreterDataFile.ZaxisJogVel;
                _AxisContainer["Z"].Axis.Velocity = InterpreterDataFile.ZaxisVel;
                _AxisContainer["Z"].Axis.Acceleration = InterpreterDataFile.ZaxisAccel;
                _AxisContainer["Z"].Axis.CPU = InterpreterDataFile.ZaxisCPI;
            }
            else
            {
                if (_AxisContainer.ContainsKey("Z"))
                {
                    _AxisContainer.Remove("Z");
                }
            }

            if (InterpreterDataFile.AaxisIndex > -1)
            {
                if (!_AxisContainer.ContainsKey("A"))
                {
                    _AxisContainer.Add("A", new AxisNode(new KM_Axis(_Controller, InterpreterDataFile.AaxisIndex, "A")));
                }

                _AxisContainer["A"].Axis.ID = InterpreterDataFile.AaxisIndex;
                _AxisContainer["A"].JogVelocity = InterpreterDataFile.AaxisJogVel;
                _AxisContainer["A"].Axis.Velocity = InterpreterDataFile.AaxisVel;
                _AxisContainer["A"].Axis.Acceleration = InterpreterDataFile.AaxisAccel;
                _AxisContainer["A"].Axis.CPU = InterpreterDataFile.AaxisCPI;
            }
            else
            {
                if (_AxisContainer.ContainsKey("A"))
                {
                    _AxisContainer.Remove("A");
                }
            }

            if (InterpreterDataFile.BaxisIndex > -1)
            {
                if (!_AxisContainer.ContainsKey("B"))
                {
                    _AxisContainer.Add("B", new AxisNode(new KM_Axis(_Controller, InterpreterDataFile.BaxisIndex, "B")));
                }

                _AxisContainer["B"].Axis.ID = InterpreterDataFile.BaxisIndex;
                _AxisContainer["B"].JogVelocity = InterpreterDataFile.BaxisJogVel;
                _AxisContainer["B"].Axis.Velocity = InterpreterDataFile.BaxisVel;
                _AxisContainer["B"].Axis.Acceleration = InterpreterDataFile.BaxisAccel;
                _AxisContainer["B"].Axis.CPU = InterpreterDataFile.BaxisCPI;
            }
            else
            {
                if (_AxisContainer.ContainsKey("B"))
                {
                    _AxisContainer.Remove("B");
                }
            }

            if (InterpreterDataFile.CaxisIndex > -1)
            {
                if (!_AxisContainer.ContainsKey("C"))
                {
                    _AxisContainer.Add("C", new AxisNode(new KM_Axis(_Controller, InterpreterDataFile.CaxisIndex, "C")));
                }

                _AxisContainer["C"].Axis.ID = InterpreterDataFile.CaxisIndex;
                _AxisContainer["C"].JogVelocity = InterpreterDataFile.CaxisJogVel;
                _AxisContainer["C"].Axis.Velocity = InterpreterDataFile.CaxisVel;
                _AxisContainer["C"].Axis.Acceleration = InterpreterDataFile.CaxisAccel;
                _AxisContainer["C"].Axis.CPU = InterpreterDataFile.CaxisCPI;
            }
            else
            {
                if (_AxisContainer.ContainsKey("C"))
                {
                    _AxisContainer.Remove("C");
                }
            }
        }



        #region Interpreter Callbacks
        private int Interpreter_InterpreterUserMCodeCallbackRequested(int code)
        {
            return 0;
        }

        private void Interpreter_InterpreterUserCallbackRequested(string msg)
        {

        }

        private void Interpreter_InterpreterStatusUpdated(int lineno, string msg)
        {
            CurrentLine = lineno;
            UpdateDocument(new Action(delegate()
            {
                _GuiHost.SetElementText(Elements.GCodeLineToRun, CurrentLine.ToString());
            }));

            UpdateGcodeDisplay();
        }

        private void Interpreter_InterpreterCompleted(int status, int lineno, int sequence_number, string err)
        {
            RunningGcode = false;
        }

        private void Interpreter_Interpreter_CoordMotionStraightTraverse(double x, double y, double z, int sequence_number)
        {

        }

        private void Interpreter_Interpreter_CoordMotionStraightFeed(double DesiredFeedRate_in_per_sec, double x, double y, double z, int sequence_number, int ID)
        {

        }

        private void Interpreter_Interpreter_CoordMotionArcFeed(bool ZeroLenAsFullCircles, double DesiredFeedRate_in_per_sec, int plane, double first_end, double second_end, double first_axis, double second_axis, int rotation, double axis_end_point, double first_start, double second_start, double axis_start_point, int sequence_number, int ID)
        {

        } 
        #endregion
    }
}
