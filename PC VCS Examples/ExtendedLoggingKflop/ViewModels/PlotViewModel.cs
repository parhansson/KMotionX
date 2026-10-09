namespace ExtendedLoggingKflop.ViewModels
{
    using System.Windows.Data;
    using Anotar.Catel;
    using Catel.Messaging;
    using Catel.MVVM;
    using OxyPlot;
    using OxyPlot.Axes;
    using OxyPlot.Series;
    using Services;
    using System;
    using System.Collections.Generic;
    using System.Collections.ObjectModel;
    using System.IO;
    using System.Threading;
    using System.Threading.Tasks;
    using System.Windows.Threading;
    using Models;
    using Catel.Services;
    using System.Collections.Concurrent;
    using System.Text;
    using System.Linq;
    using Catel.Fody;
    using Catel.IoC;
    using Catel.Data;
    using OxyPlot.Wpf;
    using System.Collections.Specialized;
    using System.ComponentModel;
    using Shared;
    using Catel.Collections;
    using System.Runtime.InteropServices;
    using System.Globalization;
    [StructLayout(LayoutKind.Explicit)]
    struct LongToDouble
    {
        public LongToDouble(double dvalue)
        {
            ul = 0;
            d = dvalue;           
        }
        [FieldOffset(0)]
        public double d;
        [FieldOffset(0)]
        public ulong ul;
    }
    public class PlotViewModel : ViewModelBase
    {
        private ILoggingService _logService;
        private IKmotionService _kserv;
        private Task _msgTask;
        private Task _updatePlotTask;
        private CancellationTokenSource _cancelSource;
        object _lockObject = new object();
        
        //SynchronizationContext _context;        
        private Dispatcher _userInterfaceDispatcher;
        private IUIVisualizerService _uiVisualizerService;

        private List<string[]> _dataLog;//all the messages
        private MessageQueue _data;
        
        //syncing plots
        private IAvailablePlotsService _plotService;
        
        public PlotViewModel(Models.PlotModel plotModel)
        {
            _uiVisualizerService = ServiceLocator.Default.ResolveType<IUIVisualizerService>();
            _logService = ServiceLocator.Default.ResolveType<ILoggingService>();
            _kserv = ServiceLocator.Default.ResolveType<IKmotionService>();
            _plotService = ServiceLocator.Default.ResolveType<IAvailablePlotsService>();
            Messages = new List<string[]>(1024);
            _dataLog = new List<string[]>(20000);

            OxyPlotModel = new OxyPlot.PlotModel();
            PlotModel = plotModel;

            StartStopCommand = new TaskCommand<bool>(OnStartStopCommandExecuteAsync, OnStartStopCommandCanExecute);
            Clear = new TaskCommand(OnClearExecute);
            ZeroCommand = new TaskCommand(OnZeroCommandExecuteAsync, OnZeroCommandCanExecute);
            SaveCommand = new TaskCommand(OnSaveCommandExecuteAsync, OnSaveCommandCanExecute);
            LoadCommand = new TaskCommand(OnLoadCommandExecuteAsync, OnLoadCommandCanExecute);
            ConfigCommand = new TaskCommand(OnConfigCommandExecuteAsync, OnConfigCommandCanExecute);
            ExportPngCommand = new TaskCommand(OnExportPngCommandExecute);
                       
            // this instance is created on the UI thread
            //_context = SynchronizationContext.Current;

            _userInterfaceDispatcher = Dispatcher.CurrentDispatcher;            
            _cancelSource = new CancellationTokenSource();

            //syncing plots
            _plotService.AvailablePlots.Add(OxyPlotModel);

            //messages strings or hex dump of memory 
            FromHex = false;

        }
               
        public OxyPlot.PlotModel OxyPlotModel { get; set; }


        /// <summary>
            /// Gets or sets the property value.
            /// </summary>
        //[Model]
        
        //public Models.PlotModel PlotModel
        //{
        //    get { return GetValue<Models.PlotModel>(PlotModelProperty); }
        //    private set { SetValue(PlotModelProperty, value); }
        //}

        ///// <summary>
        ///// Register the name property so it is known in the class.
        ///// </summary>
        //public static readonly PropertyData PlotModelProperty = RegisterProperty("PlotModel", typeof(Models.PlotModel));


        [Model]
        [Expose("PlotConfiguration")]
        [Expose("LogID")]
        public ExtendedLoggingKflop.Models.PlotModel PlotModel { get; set; }

        /// <summary>
            /// Gets or sets the property value.
            /// </summary>
        [ViewModelToModel("PlotModel")]
        public string LogFormat
        {
            get { return GetValue<string>(LogFormatProperty); }
            set { SetValue(LogFormatProperty, value); }
        }

        /// <summary>
        /// Register the name property so it is known in the class.
        /// </summary>
        public static readonly PropertyData LogFormatProperty = RegisterProperty("LogFormat", typeof(string));

        /// <summary>
            /// Gets or sets the property value.
            /// </summary>
        [ViewModelToModel("PlotModel")]
        public int MaxPlotPoints
        {
            get { return GetValue<int>(MaxPlotPointsProperty); }
            set { SetValue(MaxPlotPointsProperty, value); }
        }

        /// <summary>
        /// Register the name property so it is known in the class.
        /// </summary>
        public static readonly PropertyData MaxPlotPointsProperty = RegisterProperty("MaxPlotPoints", typeof(int));

        /// <summary>
            /// Gets or sets the property value.
            /// </summary>
        //[ViewModelToModel("PlotModel")]
        //public int LogID
        //{
        //    get { return GetValue<int>(LogIDProperty); }
        //    set { SetValue(LogIDProperty, value); }
        //}

        ///// <summary>
        ///// Register the name property so it is known in the class.
        ///// </summary>
        //public static readonly PropertyData LogIDProperty = RegisterProperty("LogID", typeof(int));
        public string PrintfLogString
        {
            get
            {
                try
                {
                    MessageQueue mq = new MessageQueue(LogFormat, PlotModel.LogID);
                    return mq.Format.PrintfLogFormat;
                }
                catch (Exception ex)
                {
                    return ex.Message;                   
                }
               
            }
        }         

        private List<string[]> Messages { get; set; }
        public bool Started { get; set; }
        private bool FromHex { get; set; }

        public override string Title { get { return "Plot View Model"; } }
       
        public TaskCommand ConfigCommand { get; private set; }
        private bool OnConfigCommandCanExecute()
        {
            return (!Started && PlotModel.PlotConfiguration != null);
        }

        /// <summary>
        /// Method to invoke when the Edit command is executed.
        /// </summary>
        private async Task OnConfigCommandExecuteAsync()
        {
            var viewModel = new PlotConfigurationViewModel(PlotModel.PlotConfiguration);
            if (await _uiVisualizerService.ShowDialogAsync(viewModel) ?? false)
            {
                if (PlotModel.PlotConfiguration != viewModel.Config)
                {
                    PlotModel.PlotConfiguration = viewModel.Config;//this triggers OnPlotConfigurationChanged()
                }
                else
                {
                    UpdatePlotConfiguration();
                }
                ViewModelCommandManager.InvalidateCommands(true);
            }
        }
        

        /// <summary>
            /// Gets the StartStopCommand command.
            /// </summary>
        public TaskCommand<bool> StartStopCommand { get; private set; }

        

        /// <summary>
        /// Method to check whether the StartStopCommand command can be executed.
        /// </summary>
        /// <returns><c>true</c> if the command can be executed; otherwise <c>false</c></returns>
        private bool OnStartStopCommandCanExecute(bool isChecked)
        {
            //isChecked is the "Start" state, 
            //isChecked == false == we want start
            //if (!isChecked)
            //{
            //    bool isFormatOK = _data==null?false:_data.Format.IsValid;                
            //    return isFormatOK && !Started && _kserv.KMController.CheckConnected();
            //}
            ////stop
            //return Started;
            return true;
        }

        /// <summary>
        /// Method to invoke when the StartStopCommand command is executed.
        /// </summary>
#pragma warning disable CS1998
        private async Task OnStartStopCommandExecuteAsync(bool start)
        {
            // TODO: Handle command logic here
            if (start)
            {
                lock (_lockObject)
                {
                    _cancelSource = new CancellationTokenSource();
                    Task<Task> task = Task.Factory.StartNew(
                    function: MsgTask,
                    cancellationToken: _cancelSource.Token,
                    creationOptions: TaskCreationOptions.LongRunning,
                    scheduler: TaskScheduler.Default);

                    _msgTask = task.Unwrap();

                    task = Task.Factory.StartNew(
                        function: PlotTask,
                        cancellationToken: _cancelSource.Token,
                        creationOptions: TaskCreationOptions.LongRunning,
                        scheduler: TaskScheduler.Default);

                    _updatePlotTask = task.Unwrap();
                    //start the logService if it hasn't already been started
                    _logService.Start();
                    Started = true;
                }
            }
            else
            {
                //we don't grab a lock here because that could cause
                //the _msgTask/_updatePlotTask to dead lock waiting
                //for it after _cancelSource.Cancel() is called.
                _cancelSource.Cancel();
                if (_msgTask != null)
                {
                    _msgTask.Wait();
                    _msgTask.Dispose();
                    _msgTask = null;
                }
                if (_updatePlotTask != null)
                {
                    _updatePlotTask.Wait();
                    _updatePlotTask.Dispose();
                    _updatePlotTask = null;
                }
                Started = false;
            }
        }
        public TaskCommand Clear { get; private set; }
        /// <summary>
        /// Method to invoke when the Edit command is executed.
        /// </summary>
        private async Task OnClearExecute()
        {
            lock (_lockObject)
            {
                Messages.Clear();
                _dataLog.Clear();
                foreach (OxyPlot.Series.LineSeries series in OxyPlotModel.Series)
                {
                    series.Points.Clear();
                }
                foreach (var ax in OxyPlotModel.Axes)
                    ax.Maximum = ax.Minimum = Double.NaN;
                OxyPlotModel.ResetAllAxes();               
                OxyPlotModel.InvalidatePlot(true);
                ViewModelCommandManager.InvalidateCommands(true);
            }
        }
        /// <summary>
            /// Gets the ZeroCommand command.
            /// </summary>
        public TaskCommand ZeroCommand { get; private set; }
        
        /// <summary>
        /// Method to check whether the ZeroCommand command can be executed.
        /// </summary>
        /// <returns><c>true</c> if the command can be executed; otherwise <c>false</c></returns>
        private bool OnZeroCommandCanExecute()
        {
            return _dataLog.Count() > 0 && Started == false;
        }

        /// <summary>
        /// Method to invoke when the ZeroCommand command is executed.
        /// </summary>
        private async Task OnZeroCommandExecuteAsync()
        {
            lock (_lockObject)
            {
                List<string[]> currentlog = _dataLog;
                _dataLog = new List<string[]>();
                Messages.Clear();
                foreach (OxyPlot.Series.LineSeries series in OxyPlotModel.Series)
                {
                    series.Points.Clear();
                }
                double timeZero = 0;
                bool result = Double.TryParse(currentlog[0][1], out timeZero);
                if (!result)
                {
                    throw new Exception("Unable to zero data.");//should never happen...
                }
                int count = 0;
                FromHex = false;
                foreach (string[] msg in currentlog)
                {
                    double msgtime = 0;
                    Double.TryParse(msg[1], out msgtime);
                    msg[1] = (msgtime - timeZero).ToString();
                    Messages.Add(msg);
                    count++;
                    if (count % 1000 == 0)
                    {
                        if (!PlotMessages())
                        {
                            throw new Exception("Unable to plot messages, see log for details.");
                        }
                    }
                }
                PlotMessages();
            }
        }
        public TaskCommand SaveCommand { get; private set; }
        private bool OnSaveCommandCanExecute()
        {
            return _dataLog.Count() > 0;
            
        }

        /// <summary>
        /// Method to invoke when the Edit command is executed.
        /// </summary>
        private async Task OnSaveCommandExecuteAsync()
        {
            var saveFileService = ServiceLocator.Default.ResolveType<ISaveFileService>();
            try
            {
                saveFileService.Filter = "Plot Data|*.csv";
                if (!string.IsNullOrEmpty(PlotModel.SaveCsvPath))
                {
                    saveFileService.InitialDirectory = Path.GetDirectoryName(PlotModel.SaveCsvPath);
                    if (!Directory.Exists(saveFileService.InitialDirectory))
                    {
                        saveFileService.InitialDirectory = Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, @"..\Data"));
                    }
                    saveFileService.FileName = Path.GetFileNameWithoutExtension(PlotModel.SaveCsvPath);
                }
                else
                {
                    saveFileService.InitialDirectory = Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, @"..\Data"));
                }
                if (saveFileService.DetermineFile())
                {
                    lock (_lockObject)
                    {
                        using (var w = new StreamWriter(saveFileService.FileName))
                        {
                            int shortestlength = int.MaxValue;
                            //Dictionary<int, string> headerDict = new Dictionary<int, string>();
                            //headerDict.Add(0, "Time");
                            var header = new StringBuilder("Time");
                            for (int i = 2; i < _data.Format.LogDelimitCount; i++)//skip first, time
                            {
                                header.Append(",");
                                //see if there is a series unquie for this result index
                                SeriesDefinitionModel series = PlotModel.PlotConfiguration.Series.FirstOrDefault(x => x.ResultIndex1 == i && x.ResultIndex2 == null);
                                if (series != null)
                                {
                                    //headerDict.Add(i, series.SeriesTitle);
                                    header.Append(series.SeriesTitle);
                                }
                                else
                                {
                                    //default header
                                    //headerDict.Add(i, string.Format("data{0}", i.ToString()));
                                    header.Append(string.Format("data{0}", i.ToString()));
                                }
                            }
                            w.WriteLine(header);
                            foreach (string[] datapoint in _dataLog)
                            {
                                ArraySegment<string> realdata = new ArraySegment<string>(datapoint, 1, datapoint.Count() - 1);
                                string msg = String.Join(",", realdata);
                                w.WriteLine(msg);
                            }
                            //foreach (OxyPlot.Series.LineSeries series in OxyPlotModel.Series)
                            //{
                            //    header.Append(",");
                            //    header.Append(series.Title);                                
                            //    if (series.Points.Count() < shortestlength)
                            //    {
                            //        //should all be same length... but just in case
                            //        shortestlength = series.Points.Count();
                            //    }
                            //}
                            //w.WriteLine(header);
                            //for (int i = 0; i < shortestlength; i++)
                            //{
                            //    var points = new StringBuilder(((OxyPlot.Series.LineSeries)(OxyPlotModel.Series[0])).Points[i].X.ToString());

                            //    foreach (OxyPlot.Series.LineSeries series in OxyPlotModel.Series)
                            //    {
                            //        points.Append(",");
                            //        points.Append(series.Points[i].Y);
                            //    }
                            //}           

                            w.Flush();
                            LogTo.Info("Wrote {0} points to CSV file.", shortestlength.ToString());
                        }
                        
                    }
                    if (PlotModel.SaveCsvPath != saveFileService.FileName)
                    {
                        PlotModel.SaveCsvPath = saveFileService.FileName;
                    }
                    LogTo.Info("Saved Plot data to: {0}", PlotModel.SaveCsvPath);
                }
            }
            catch (Exception ex)
            {
                string errmsg = string.Format("Error saving Plot data: {0}", ex.Message);
                LogTo.Error(errmsg);
                await Shared.Utility.ShowErrorMsgAsync(this, errmsg);
            }
            
        }
        /// <summary>
            /// Gets the LoadCommand command.
            /// </summary>
        public TaskCommand LoadCommand { get; private set; }
       
        bool OnLoadCommandCanExecute()
        {
            return !Started && OxyPlotModel.Series.Count > 0 && OxyPlotModel.Axes.Count > 0;//if actively logging we shouldn't try to add points from CSV file.
        }
        /// <summary>
        /// Method to invoke when the LoadCommand command is executed.
        /// </summary>
        private async Task OnLoadCommandExecuteAsync()
        {
            try
            {
                var openFileService = ServiceLocator.Default.ResolveType<IOpenFileService>();
                openFileService.Filter = "Plot Data| *.csv";
                if (!string.IsNullOrEmpty(PlotModel.SaveCsvPath))
                {
                    openFileService.InitialDirectory = Path.GetDirectoryName(PlotModel.SaveCsvPath);
                    openFileService.FileName = Path.GetFileNameWithoutExtension(PlotModel.SaveCsvPath);
                }
                else
                {
                    openFileService.InitialDirectory = Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, @"..\Data"));

                }

                if (openFileService.DetermineFile())
                {
                    using (var reader = new StreamReader(openFileService.FileName))
                    {
                        lock(_lockObject)
                        {
                            string headers = reader.ReadLine();
                            string[] delheaders = headers.Split(',');
                            //count unquie indexes in configured plot
                            List<int> uindexes = GetUnquieIndexesInSeries(PlotModel.PlotConfiguration.Series);
                            
                            if (uindexes.DefaultIfEmpty(0).Max() != delheaders.Length)//+1 for log identifier and time
                            {
                                throw new Exception("Current Plot Configuration does not match CSV file columns");
                            }
                            FromHex = false;
                            string line = reader.ReadLine();
                            int count = 0;
                            while (line!=null) 
                            {                                                                
                                string strwithid = line.Insert(0, "@F,");
                                string[] delline = strwithid.Split(',');
                                Messages.Add(delline);
                                count++;
                                if (count % 1000 == 0)
                                {
                                    if (!PlotMessages())
                                    {
                                        throw new Exception("Unable to plot messages, see log for details.");
                                    }
                                }
                                line = reader.ReadLine();
                            }
                            if (!PlotMessages())
                            {
                                throw new Exception("Unable to plot messages, see log for details.");
                            }
                            FromHex = true;
                            LogTo.Info("Plotted {0} points.", count.ToString());
                       }
                    }
                    
                    PlotModel.SaveCsvPath = openFileService.FileName;                  
                    LogTo.Info("Loaded Plot data from: {0}", PlotModel.SaveCsvPath);
                    ViewModelCommandManager.InvalidateCommands(true);

                }
            }
            catch (Exception ex)
            {
                string errmsg = string.Format("Error loading Plot data: {0}", ex.Message);
                LogTo.Error(errmsg);
                await Shared.Utility.ShowErrorMsgAsync(this, errmsg);
            }
        }
        /// <summary>
        /// Gets the ExportPngCommand command.
        /// </summary>
        public TaskCommand ExportPngCommand { get; private set; }
            

        /// <summary>
        /// Method to invoke when the ExportPngCommand command is executed.
        /// </summary>
        private async Task OnExportPngCommandExecute()
        {
            var saveFileService = ServiceLocator.Default.ResolveType<ISaveFileService>();
            try
            {
                saveFileService.Filter = "Plot Image|*.png";
                if (!string.IsNullOrEmpty(PlotModel.SavePngPath))
                {
                    saveFileService.InitialDirectory = Path.GetDirectoryName(PlotModel.SavePngPath);
                    if (!Directory.Exists(saveFileService.InitialDirectory))
                    {
                        saveFileService.InitialDirectory = Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, @"..\Data"));
                    }
                    saveFileService.FileName = Path.GetFileNameWithoutExtension(PlotModel.SavePngPath);
                }
                else
                {
                    saveFileService.InitialDirectory = Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, @"..\Data"));
                }
                if (saveFileService.DetermineFile())
                {                                        
                    var pngExporter = new OxyPlot.Wpf.PngExporter { Width = (int)OxyPlotModel.Width, Height = (int)OxyPlotModel.Height, Background = OxyColors.White };
                    pngExporter.ExportToFile(OxyPlotModel, saveFileService.FileName);                   
                    if (PlotModel.SavePngPath != saveFileService.FileName)
                    {
                        PlotModel.SavePngPath = saveFileService.FileName;
                    }                    
                    LogTo.Info("Saved Plot image to: {0}", PlotModel.SavePngPath);
                }
            }
            catch (Exception ex)
            {
                string errmsg = string.Format("Error saving Plot image: {0}", ex.Message);
                LogTo.Error(errmsg);
                await Shared.Utility.ShowErrorMsgAsync(this, errmsg);
            }
        }
        /// <summary>
        /// Continually read from the BlockingCollection taking message sent from 
        /// Kflop and saving them to "Messages", the messages will then be
        /// added to the plots at regular intervals in the PlotTask thread.
        /// </summary>
        /// <returns></returns>
        async Task MsgTask()
        {
            LogTo.Info("Enter MsgTask");
            while (!_cancelSource.IsCancellationRequested)
            {
                string[] msg;
                try
                {
                    
                    {
                        if (_data.Queue.TryTake(out msg, 100, _cancelSource.Token))
                        {
                            lock (_lockObject)
                            {
                                Messages.Add(msg);
                            }
                        }
                    }
                }
                catch (OperationCanceledException)
                {
                    break;
                }
            }
            LogTo.Info("Exit MsgTask");
        }

        private bool PlotMessages()
        {
            var seriespoints = new List<List<DataPoint>>();
            foreach (SeriesDefinitionModel series in PlotModel.PlotConfiguration.Series)
            {
                seriespoints.Add(new List<DataPoint>());
            }

            foreach (string[] msg in Messages)
            {
                //Typical log statement:
                //Sample,Time,Command,Position,Output
                //printf("@F,%g,%g,%g,%g\n",tickcurr,CHANNELTOTEST->Dest,CHANNELTOTEST->Position,CHANNELTOTEST->Output);
                //where @F is the log identifier, so msg[0] == "@F", which we ignore here, data starts at index 1, which
                //is always time
                //PlotDefinition.Series.ToList().Zip(OxyPlotModel.Series, (n, w) => new { Number = n, Word = w });
                if (msg.Count() != _data.Format.LogDelimitCount)
                {
                    string errmsg = string.Format("Invalid message for this plot, format length {0:D} does not equal message length {1:D}.", _data.Format.LogDelimitCount, msg.Count());
                    LogTo.Error(errmsg);
                    Messages.Clear();
                    return false;
                }

                double x, y1, y2;
                if (FromHex)
                {
                    LongToDouble xh = new LongToDouble(0.0d);
                    ulong.TryParse(msg[1], NumberStyles.HexNumber, CultureInfo.InvariantCulture, out xh.ul);
                    x = xh.d;
                    //double dd =  0;
                    //Double.TryParse(msg[1], NumberStyles.HexNumber, CultureInfo.InvariantCulture, out dd);
                }
                else
                {
                    Double.TryParse(msg[1], out x);//first param is always time, x axis
                }
            
                var zip = PlotModel.PlotConfiguration.Series.ToList().Zip(seriespoints, (n, w) => new { series = n, seriesPoints = w });
                foreach (var defAndSeries in zip)
                //foreach (SeriesDefinitionModel series in PlotDefinition.Series)
                {
                    if (FromHex)
                    {
                        LongToDouble y1h = new LongToDouble(0.0d);
                        ulong.TryParse(msg[defAndSeries.series.ResultIndex1], NumberStyles.HexNumber, CultureInfo.InvariantCulture, out y1h.ul);
                        y1 = y1h.d;
                    }
                    else
                    {
                        Double.TryParse(msg[defAndSeries.series.ResultIndex1], out y1);//first param is always time, x axis
                    }
                    //Double.TryParse(msg[defAndSeries.series.ResultIndex1], out y1);
                    DataPoint p = new DataPoint(x, y1);
                    if (defAndSeries.series.ResultOperator != PlotAxisOperatorEnum.None)
                    {
                        if (FromHex)
                        {
                            LongToDouble y2h = new LongToDouble(0.0d);
                            ulong.TryParse(msg[defAndSeries.series.ResultIndex2.Value], NumberStyles.HexNumber, CultureInfo.InvariantCulture, out y2h.ul);
                            y2 = y2h.d;
                        }
                        else
                        {
                            Double.TryParse(msg[defAndSeries.series.ResultIndex2.Value], out y2);//first param is always time, x axis
                        }
                        //Double.TryParse(msg[defAndSeries.series.ResultIndex2.Value], out y2);
                        switch (defAndSeries.series.ResultOperator)
                        {
                            case PlotAxisOperatorEnum.Add:
                                p = new DataPoint(x, y1 + y2);
                                break;
                            case PlotAxisOperatorEnum.Subtract:
                                p = new DataPoint(x, y1 - y2);
                                break;
                            case PlotAxisOperatorEnum.Multiple:
                                p = new DataPoint(x, y1 * y2);
                                break;
                            case PlotAxisOperatorEnum.Divide:
                                p = new DataPoint(x, y1 / y2);
                                break;
                            default:
                                LogTo.Error("This should never happen.");
                                break;
                        }
                    }

                    defAndSeries.seriesPoints.Add(p);
                }

            }
            _dataLog.AddRange(Messages);
            Messages.Clear();

            if (seriespoints.Count > 0 && seriespoints[0] != null && seriespoints[0].Count > 0)
            {
                //Update the plot on the GUI thread
                _userInterfaceDispatcher.Invoke(() =>
                {
                    var zip = OxyPlotModel.Series.Zip(seriespoints, (n, w) => new { series = n, seriesPoints = w });
                    foreach (var seriesAndPoints in zip)
                    {
                        OxyPlot.Series.LineSeries ls = seriesAndPoints.series as OxyPlot.Series.LineSeries;
                        if (ls.Points.Count >= MaxPlotPoints)
                        {
                            ls.Points.RemoveRange(0, seriesAndPoints.seriesPoints.Count);
                        }
                        ls.Points.AddRange(seriesAndPoints.seriesPoints);

                    }
                    OxyPlotModel.InvalidatePlot(true);
                });
            }
            return true;
        }
        /// <summary>
        /// Every 100ms loop through the Messages and add the points to plots.  
        /// Messages are cleared.
        /// </summary>
        /// <returns></returns>
        async Task PlotTask()
        {
            LogTo.Info("Enter PlotTask");
            while (!_cancelSource.IsCancellationRequested)
            {
                try
                {
                    await Task.Delay(100, _cancelSource.Token);
                    lock (_lockObject)
                    {
                        PlotMessages();
                    }
                }
                catch (OperationCanceledException)
                {
                    break;
                }
            }
            LogTo.Info("Exit PlotTask");
        }
       
        private void OnLogIDChanged()
        {
            OnLogFormatChanged();
            LogTo.Info("LogID: {0}", PlotModel.LogID);
        }
        private void OnLogFormatChanged()
        {
            
            lock (_lockObject)
            {
                Messages.Clear();
                _dataLog.Clear();
                if (_data != null)
                {
                    _logService.UnRegisterQueue(_data);
                }
                _data = new MessageQueue(PlotModel.LogFormat, PlotModel.LogID);
                _logService.RegisterQueue(_data);                               
            }             
        }
        
        private void ClearPlot()
        {
            OxyPlotModel.Series.Clear();
            if (OxyPlotModel.Axes.Count > 0)
            {
                //should always have an X axis
                OxyPlotModel.Axes[0].AxisChanged -= _plotService.OnAxisChanged;// OnAxisChanged;
            }            
            OxyPlotModel.Axes.Clear();
            var xAxis = new OxyPlot.Axes.LinearAxis { Position = AxisPosition.Bottom, Title = "Time", Unit = "sec" };            
            OxyPlotModel.Axes.Add(xAxis);
            OxyPlotModel.Axes[0].AxisChanged += _plotService.OnAxisChanged; //OnAxisChanged;
            OxyPlotModel.LegendOrientation = LegendOrientation.Horizontal;
            OxyPlotModel.LegendPlacement = LegendPlacement.Outside;
            OxyPlotModel.LegendPosition = LegendPosition.TopCenter;
        }

        private OxyPlot.Axes.AxisPosition ConvertAxisToOxyPlot(PlotAxisPositionEnum position)
        {
            switch (position)
            {
                case PlotAxisPositionEnum.YAxisLeft:
                    return AxisPosition.Left;
                case PlotAxisPositionEnum.YAxisRight:
                    return AxisPosition.Right;
                default:
                    throw (new Exception("Unknown Axis Position."));
            }
        }
        private List<int> GetUnquieIndexesInSeries(IEnumerable<SeriesDefinitionModel> allSeries)
        {
            List<int> uindexes = new List<int>();
            foreach (SeriesDefinitionModel series in allSeries)
            {
                if (!uindexes.Contains(series.ResultIndex1))
                {
                    uindexes.Add(series.ResultIndex1);
                }
                if (series.ResultOperator != PlotAxisOperatorEnum.None &&
                    series.ResultIndex2.HasValue &&
                    !uindexes.Contains(series.ResultIndex2.Value))
                {
                    uindexes.Add(series.ResultIndex2.Value);
                }
            }
            return uindexes;
        }
        private void AppendLogFormatFromSeries(StringBuilder logFormat, IEnumerable<SeriesDefinitionModel> allSeries)
        {
            List<int> uindexes = GetUnquieIndexesInSeries(allSeries);
            for (int i = 0; i< uindexes.DefaultIfEmpty(0).Max();i++)
            {
                if (i == 0)
                {
                    logFormat.Append(@"%g");
                }
                else
                {
                    logFormat.Append(@",%g");
                }
            }
        }
        private void OnPlotConfigurationChanged()
        {         
            ClearPlot();
            if (PlotModel.PlotConfiguration == null)
            {                
                return;
            }

            foreach (AxisDefinitionModel axis in PlotModel.PlotConfiguration.Axes)
            {
                var yAxis = new OxyPlot.Axes.LinearAxis { Position = ConvertAxisToOxyPlot(axis.AxisPosition), Title = axis.AxisTitle, Unit = axis.Unit, Key=axis.Key };
                yAxis.Tag = axis;
                axis.OxyAxis = yAxis;
                yAxis.AxisChanged += _plotService.OnAxisChanged;
                OxyPlotModel.Axes.Add(yAxis);
            }
            StringBuilder logFormat = new StringBuilder();
            foreach(SeriesDefinitionModel series in PlotModel.PlotConfiguration.Series)
            {
                var oxseries = new OxyPlot.Series.LineSeries
                { Title = series.SeriesTitle,
                    Color = OxyColor.FromArgb(series.Color.A, series.Color.R, series.Color.G, series.Color.B),
                    YAxisKey = series.YAxisKey };
                oxseries.Tag = series;
                series.OxySeries = oxseries;
                OxyPlotModel.Series.Add(oxseries);
                //oxseries.Smooth;add option for this
            }
            AppendLogFormatFromSeries(logFormat, PlotModel.PlotConfiguration.Series);
            PlotModel.LogFormat = logFormat.ToString();
            OnLogFormatChanged();
            //string tmp = this.PrintfLogString;
            OxyPlotModel.Title = PlotModel.PlotConfiguration.PlotName;
            OxyPlotModel.InvalidatePlot(true);
        }
        
        protected void UpdatePlotConfiguration()
        {
            
            // "PlotName"
            {
                OxyPlotModel.Title = PlotModel.PlotConfiguration.PlotName;
               
            }
            // "Axes"
            {
                //remove deleted Axes first
                var toRemoveAxis = new List<OxyPlot.Axes.LinearAxis>();
                foreach (OxyPlot.Axes.LinearAxis oxaxis in OxyPlotModel.Axes)
                {
                    if (oxaxis.Position != AxisPosition.Bottom)
                    {
                        var axis = PlotModel.PlotConfiguration.Axes.FirstOrDefault(x => x.OxyAxis == oxaxis);//don't remove xaxis
                        if (axis == null)
                        {
                            toRemoveAxis.Add(oxaxis);
                        }
                    }
                }
                foreach (OxyPlot.Axes.LinearAxis oxaxis in toRemoveAxis)
                {
                    OxyPlotModel.Axes.Remove(oxaxis);
                }

                foreach (AxisDefinitionModel axis in PlotModel.PlotConfiguration.Axes)
                {
                    //can't use FirstOrDefault as OxyPlot sets default to first axis
                    OxyPlot.Axes.LinearAxis oxaxis = null;
                    foreach (OxyPlot.Axes.LinearAxis oa in OxyPlotModel.Axes)
                    {
                        if ((AxisDefinitionModel)oa.Tag == axis)
                        {
                            oxaxis = oa;
                            break;
                        }
                    }
                    //OxyPlot.Axes.LinearAxis oxaxis = OxyPlotModel.Axes.FirstOrDefault(x => x.Tag == axis) as OxyPlot.Axes.LinearAxis;
                    if (oxaxis == null)
                    {
                        var yAxis = new OxyPlot.Axes.LinearAxis { Position = ConvertAxisToOxyPlot(axis.AxisPosition), Title = axis.AxisTitle, Unit = axis.Unit, Key = axis.Key };
                        yAxis.Tag = axis;
                        OxyPlotModel.Axes.Add(yAxis);
                    }
                    else
                    {
                        //update
                        oxaxis.Position = ConvertAxisToOxyPlot(axis.AxisPosition);
                        oxaxis.Title = axis.AxisTitle;
                        oxaxis.Unit = axis.Unit;
                        oxaxis.Key = axis.Key;
                    }
                }
            }
            //"Series"
            {
                //delete removed series first.
                var toRemoveSeries = new List<OxyPlot.Series.LineSeries>();
                foreach (OxyPlot.Series.LineSeries oxseries in OxyPlotModel.Series)
                {                    
                    {
                        var series = PlotModel.PlotConfiguration.Series.FirstOrDefault(x => x.OxySeries == oxseries);//don't remove xaxis
                        if (series == null)
                        {
                            toRemoveSeries.Add(oxseries);
                        }
                    }
                }
                foreach (OxyPlot.Series.LineSeries oxseries in toRemoveSeries)
                {
                    OxyPlotModel.Series.Remove(oxseries);
                }
                
                foreach (SeriesDefinitionModel series in PlotModel.PlotConfiguration.Series)
                {
                    //can't use FirstOrDefault as OxyPlot sets default to first axis
                    OxyPlot.Series.LineSeries oxseries = null;
                    foreach (OxyPlot.Series.LineSeries os in OxyPlotModel.Series)
                    {
                        if ((AxisDefinitionModel)os.Tag == series)
                        {
                            oxseries = os;
                            break;
                        }
                    }
                    //var oxseries = OxyPlotModel.Series.FirstOrDefault(x => x.Tag == series) as OxyPlot.Series.LineSeries;
                    if (oxseries == null)
                    {
                        oxseries = new OxyPlot.Series.LineSeries
                        {
                            Title = series.SeriesTitle,
                            Color = OxyColor.FromArgb(series.Color.A, series.Color.R, series.Color.G, series.Color.B),
                            YAxisKey = series.YAxisKey
                        };
                        oxseries.Tag = series;
                        OxyPlotModel.Series.Add(oxseries);
                    } 
                    else
                    {
                        //update
                        oxseries.Title = series.SeriesTitle;
                        oxseries.Color = OxyColor.FromArgb(series.Color.A, series.Color.R, series.Color.G, series.Color.B);
                        oxseries.YAxisKey = series.YAxisKey;
                    }                        
                }
                StringBuilder logFormat = new StringBuilder();//always have time as first parameter
                AppendLogFormatFromSeries(logFormat, PlotModel.PlotConfiguration.Series);
                PlotModel.LogFormat = logFormat.ToString();
                OnLogFormatChanged();
            }

            OxyPlotModel.InvalidatePlot(true);
            
        }         
        
        void OnStatisticsTimer(object sender, EventArgs e)
        {
            
        }
        protected override async Task InitializeAsync()
        {
            await base.InitializeAsync();
                   
            
        }
        protected override async Task CloseAsync()
        {
            // TODO: unsubscribe from events here
            if (OxyPlotModel.Axes != null)
            {
                foreach (var axis in OxyPlotModel.Axes)
                {
                    axis.AxisChanged -= _plotService.OnAxisChanged;
                }            
            }
            _plotService.AvailablePlots.Remove(OxyPlotModel);
            _cancelSource.Cancel();                    
            await base.CloseAsync();
        }
    }
}
