namespace ExtendedLoggingKflop.ViewModels
{
    using Catel.MVVM;
    using System.Collections.ObjectModel;
    using System.Threading.Tasks;
    using ICSharpCode.AvalonEdit.Document;
    //using Catel.IO;
    using System.IO;
    using ICSharpCode.AvalonEdit.Utils;
    using System.Text;
    using Anotar.Catel;
    using Services;
    using KMotion_dotNet;
    using System;
    using Catel.IoC;
    using Catel.Services;
    using System.Linq;
    using Models;
    using Catel.Data;
    using Catel.Fody;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Windows.Data;
    using System.Windows.Threading;
    using Catel.Runtime.Serialization;
    using System.Diagnostics;
    using Gat.Controls;
    using System.Windows.Media.Imaging;
    using Jot.Configuration;
    public class MainWindowViewModel : ViewModelBase
    {
        private IKmotionService _kserv;
        private ChangeNotificationWrapper _selectedCProgramWatcher;
        private DispatcherTimer _statusTimer;
        private IAvailablePlotsService _plotService;
        private ILoggingService _loggingService;
        private Stopwatch _stopwatch;
        private long _lastElapsedms;

        public MainWindowViewModel(IKmotionService kserv, IAvailablePlotsService plotserver, ILoggingService loggingService)
        {
            _kserv = kserv;
            _plotService = plotserver;
            _loggingService = loggingService;
            ThreadIDs = new ObservableCollection<int?>();
            ThreadIDs.Add(null);
            ThreadIDs.Add(1);
            ThreadIDs.Add(2);
            ThreadIDs.Add(3);
            ThreadIDs.Add(4);
            ThreadIDs.Add(5);
            ThreadIDs.Add(6);
            ThreadIDs.Add(7);
            WindowModel = new MainWindowModel();
            FreeThreads = CollectionViewSource.GetDefaultView(ThreadIDs);
            
            SelectedCProgram = null;

            AddCFile = new TaskCommand(OnAddCFileExecuteAsync);
            SaveCFile = new TaskCommand(OnSaveCFileExecuteAsync, OnSaveCFileCanExecute);
            RemoveCFile = new TaskCommand(OnRemoveCFileExecuteAsync, OnRemoveCFileCanExecute);
            Download = new TaskCommand(OnDownloadExecuteAsync, OnDownloadCanExecute);
            Execute = new TaskCommand(OnExecuteExecuteAsync, OnExecuteCanExecute);
            Hault = new TaskCommand(OnHaultExecuteAsync, OnHaultCanExecute);
            HaultAllCommand = new TaskCommand(OnHaultAllCommandExecuteAsync, OnHaultAllCommandCanExecute);

            SetThreadIDCommand = new Command<int?>(OnSetThreadIDCommandExecuteAsync, OnSetThreadIDCommandCanExecute);

            SaveConfigCommand = new TaskCommand(OnSaveConfigCommandExecuteAsync);
            NewPlotCommand = new TaskCommand(OnNewPlotCommandExecuteAsync);
            OpenConfigCommand = new TaskCommand(OnOpenConfigCommandExecuteAsync);
            StopStartCommand = new TaskCommand<bool>(OnStopStartCommandExecuteAsync, OnStopStartCommandCanExecute);
            AboutCommand = new TaskCommand(OnAboutCommandExecuteAsync);

            _stopwatch = new Stopwatch();
            _stopwatch.Start();
            _statusTimer = new DispatcherTimer();
            _statusTimer.Interval = TimeSpan.FromMilliseconds(500);
            _statusTimer.Tick += this.OnMainStatusTick;
            _statusTimer.Start();

            SyncPlotsOnX = true;
            SaveConfigPath = "";
            //get saved settings, (syncx,y,last saved config..)
            Services.Tracker.Configure(this).Apply();           
            if (!string.IsNullOrEmpty(SaveConfigPath))
            {
                try
                {
                    OpenConfiguration(SaveConfigPath);
                }
                catch (Exception ex)
                {
                    LogTo.Error(ex, "Error openeing previous configuration, ignoring.");
                }
            }                        
        }
        #region Properties
        /// <summary>
        /// Gets or sets the property value.
        /// </summary>
        [Model]
        public MainWindowModel WindowModel
        {
            get { return GetValue<MainWindowModel>(WindowModelProperty); }
            private set { SetValue(WindowModelProperty, value); }
        }

        /// <summary>
        /// Register the name property so it is known in the class.
        /// </summary>
        public static readonly PropertyData WindowModelProperty = RegisterProperty("WindowModel", typeof(MainWindowModel));

        /// <summary>
        /// Gets or sets the property value.
        /// </summary>    
        [ViewModelToModel("WindowModel")]
        public ObservableCollection<ThreadCProgram> ThreadCPrograms
        {
            get { return GetValue<ObservableCollection<ThreadCProgram>>(ThreadCProgramsProperty); }
            set { SetValue(ThreadCProgramsProperty, value); }
        }

        /// <summary>
        /// Register the name property so it is known in the class.
        /// </summary>
        public static readonly PropertyData ThreadCProgramsProperty = RegisterProperty("ThreadCPrograms", typeof(ObservableCollection<ThreadCProgram>));

        /// <summary>
        /// Gets or sets the property value.
        /// </summary>
        [ViewModelToModel("WindowModel")]
        public ObservableCollection<Models.PlotModel> Plots
        {
            get { return GetValue<ObservableCollection<Models.PlotModel>>(PlotsProperty); }
            set { SetValue(PlotsProperty, value); }
        }

        /// <summary>
        /// Register the name property so it is known in the class.
        /// </summary>
        public static readonly PropertyData PlotsProperty = RegisterProperty("Plots", typeof(ObservableCollection<Models.PlotModel>));

        public ObservableCollection<int?> ThreadIDs { get; set; }        
        public ThreadCProgram SelectedCProgram { get; set; }

        public ICollectionView FreeThreads { get; set; }
        private KM_MainStatus MainStatus { get; set; }
        public bool Connected { get; set; }
        public bool Thread1Executing { get; set; }
        public bool Thread2Executing { get; set; }
        public bool Thread3Executing { get; set; }
        public bool Thread4Executing { get; set; }
        public bool Thread5Executing { get; set; }
        public bool Thread6Executing { get; set; }
        public bool Thread7Executing { get; set; }

        [Trackable]
        public bool SyncPlotsOnX
        {
            get
            {
                return _plotService.SyncedXAxis;
            }
            set
            {
                _plotService.SyncedXAxis = value;
            }
        }
        [Trackable]
        public bool SyncPlotsOnY
        {
            get
            {
                return _plotService.SyncedYAxis;
            }
            set
            {
                _plotService.SyncedYAxis = value;
            }
        }
        public bool StartLogging
        {
            get
            {
                return !_loggingService.Pause;
            }
            set
            {
                _loggingService.Pause = !value;
            }
        }
        public int DataRate { get; set; }
        public int MaxDataRate { get; set; }
        public int MessageCount { get; set; }
        public int MessageByteCount { get; set; }
        public override string Title { get { return "ExtendedLoggingKflop"; } }
        [Trackable]
        private string LastCProgramPath { get; set; }
        [Trackable]
        public string SaveConfigPath { get; set; }
        #endregion
        #region Commands
        /// <summary>
        /// Gets the AddCFile command.
        /// </summary>
        public TaskCommand AddCFile { get; private set; }
        
        /// <summary>
        /// Method to invoke when the AddCFile command is executed.
        /// </summary>
        private async Task OnAddCFileExecuteAsync()
        {
            try
            {
                var openFileService = ServiceLocator.Default.ResolveType<IOpenFileService>();
                openFileService.Filter = "Kflop C Program File| *.c";
                if (!string.IsNullOrEmpty(LastCProgramPath))
                {
                    openFileService.InitialDirectory = Path.GetDirectoryName(LastCProgramPath);
                    openFileService.FileName = Path.GetFileNameWithoutExtension(LastCProgramPath);
                }
                else
                {
                    openFileService.InitialDirectory = Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, @"..\..\C Programs"));

                }

                if (openFileService.DetermineFile())
                {
                    
                    List<int> freeThreads = new List<int>() { 1, 2, 3, 4, 5, 6, 7 };
                    freeThreads.DefaultIfEmpty(-1);
                    foreach (ThreadCProgram cp in ThreadCPrograms)
                    {
                        if (cp.ThreadID != null)
                        {
                            freeThreads.Remove(cp.ThreadID.Value);
                        }
                    }
                    int id = freeThreads.FirstOrDefault();
                    if (id == -1)
                    {
                        LogTo.Warning("All threads in use, resuing thread ID 1 for C Program {1}.", openFileService.FileName);
                    }
                    ThreadCProgram cprog = new ThreadCProgram() { FilePath = openFileService.FileName, ThreadID=id };
                    this.ThreadCPrograms.Add(cprog);
                    
                    LastCProgramPath = openFileService.FileName;                    
                    ViewModelCommandManager.InvalidateCommands(true);
                    LogTo.Info("Loaded C Program from: {0}", LastCProgramPath);

                }
            }
            catch (Exception ex)
            {
                string errmsg = string.Format("Error loading C program: {0}", ex.Message);
                LogTo.Error(errmsg);
                await Shared.Utility.ShowErrorMsgAsync(this, errmsg);
            }
        }
        /// <summary>
            /// Gets the RemoveCFile command.
            /// </summary>
        public TaskCommand RemoveCFile { get; private set; }

        /// <summary>
        /// Method to check whether the RemoveCFile command can be executed.
        /// </summary>
        /// <returns><c>true</c> if the command can be executed; otherwise <c>false</c></returns>
        private bool OnRemoveCFileCanExecute()
        {
            return SelectedCProgram != null;
        }

        /// <summary>
        /// Method to invoke when the RemoveCFile command is executed.
        /// </summary>
#pragma warning disable CS1998
        private async Task OnRemoveCFileExecuteAsync()
        {
            ThreadCPrograms.Remove(SelectedCProgram);
        }
        public TaskCommand Hault { get; private set; }
        private bool OnHaultCanExecute()
        {
            try
            {
                if (SelectedCProgram != null 
                    && SelectedCProgram.ThreadID != null
                    && Connected
                    && Convert.ToBoolean(MainStatus.GetThreadActive(SelectedCProgram.ThreadID.Value))
                    //&& _kserv.KMController.ThreadExecuting(SelectedCProgram.ThreadID.Value)
                    ) return true;
            }
            catch (Exception ex)
            {
                LogTo.Error("Error checking thread execution: {0}", ex.Message);
            }
            //if (SelectedCProgram != null) return true;
            return false;
        }

        /// <summary>
        /// Method to invoke when the Edit command is executed.
        /// </summary>
        private async Task OnHaultExecuteAsync()
        {
            try
            {
                _kserv.KMController.KillProgramThreads(SelectedCProgram.ThreadID.Value);
            }
            catch (DMException ex)
            {
                string errmsg = string.Format("KillProgramThreads Error: {0}", ex.InnerException.Message);
                LogTo.Error(errmsg);
                await Shared.Utility.ShowErrorMsgAsync(this, errmsg);
            }
            catch (Exception ex)
            {
                string errmsg = string.Format("KillProgramThreads Error: {0}", ex.Message);
                LogTo.Error(errmsg);
                await Shared.Utility.ShowErrorMsgAsync(this, errmsg);
            }
        }
        public TaskCommand SaveCFile { get; private set; }
        private bool OnSaveCFileCanExecute()
        {            
            if (SelectedCProgram == null
                || SelectedCProgram.ThreadID == null) return false;
            return true;
        }

        /// <summary>
        /// Method to invoke when the Edit command is executed.
        /// </summary>
        private async Task OnSaveCFileExecuteAsync()
        {
            try
            {                
                int nBoards = 0;
                int[] List = _kserv.KMController.GetBoards(out nBoards);
                System.IO.File.WriteAllText(SelectedCProgram.FilePath, SelectedCProgram.Document.Text);
                return;
                //if (nBoards == 0) return;
                //string Result = _kserv.KMController.Compile(SelectedCProgram.ThreadID.Value, SelectedCProgram.FilePath);
                //if (Result != "")
                //{
                //    //MessageBox.Show(Result, "Compile Error");
                //    LogTo.Error("Compile Error: {0}, Message: {0}", Result, _kserv.KMController.ErrorLog);
                //    _kserv.KMController.ErrorLog = "";
                //    await Shared.Utility.ShowErrorMsgAsync(this, Result);
                //}
                //else
                //{                    
                //    LogTo.Info("{0} Compiled OK.", SelectedCProgram.FileName);
                //}

            }
            catch (DMException ex)
            {               
                string errmsg = string.Format("Compile Error: {0}", ex.InnerException.Message);
                LogTo.Error(errmsg);
                await Shared.Utility.ShowErrorMsgAsync(this, errmsg);
            }
            catch (Exception ex)
            {
                string errmsg = string.Format("Compile Error: {0}", ex.Message);
                LogTo.Error(errmsg);
                await Shared.Utility.ShowErrorMsgAsync(this, errmsg);
            }
        }        

        public TaskCommand Download { get; private set; }
        private bool OnDownloadCanExecute()
        {
            if (SelectedCProgram != null
                && SelectedCProgram.ThreadID != null
                && Connected
                )
                return true;
            return false;
        }

        private async Task OnDownloadExecuteAsync()
        {
            try
            {
                int nBoards = 0;
                int[] List = _kserv.KMController.GetBoards(out nBoards);
                System.IO.File.WriteAllText(SelectedCProgram.FilePath, SelectedCProgram.Document.Text);
                string Result = _kserv.KMController.CompileAndLoadCoff(SelectedCProgram.ThreadID.Value, SelectedCProgram.FilePath,false);
                if (Result != "")
                {
                    //MessageBox.Show(Result, "Compile Error");
                    LogTo.Error("Compile Error: {0}, Message: {0}", Result, _kserv.KMController.ErrorLog);
                    _kserv.KMController.ErrorLog = "";
                }
                else
                {
                    // everything ok
                    LogTo.Info("{0} Compiled OK.", SelectedCProgram.FileName);
                }
            }
            catch (DMException ex)
            {
                string errmsg = string.Format("Compile Error: {0}", ex.InnerException.Message);
                LogTo.Error(errmsg);
                await Shared.Utility.ShowErrorMsgAsync(this, errmsg);
            }
            catch (Exception ex)
            {
                string errmsg = string.Format("Compile Error: {0}", ex.Message);
                LogTo.Error(errmsg);
                await Shared.Utility.ShowErrorMsgAsync(this, errmsg);
            }
        }

        public TaskCommand Execute { get; private set; }
        private bool OnExecuteCanExecute()
        {
            if (SelectedCProgram != null
                && SelectedCProgram.ThreadID != null
                && this.Connected
                ) return true;
            return false;
        }

        private async Task OnExecuteExecuteAsync()
        {
            try
            {
                if (_kserv.KMController.ThreadExecuting(SelectedCProgram.ThreadID.Value))
                {
                    _kserv.KMController.KillProgramThreads(SelectedCProgram.ThreadID.Value);
                    return;
                }
                _kserv.KMController.ExecuteProgram(SelectedCProgram.ThreadID.Value);
            }
            catch (DMException ex)
            {
                string errmsg = string.Format("Compile Error: {0}", ex.InnerException.Message);
                LogTo.Error(errmsg);
                await Shared.Utility.ShowErrorMsgAsync(this, errmsg);
            }
            catch (Exception ex)
            {
                string errmsg = string.Format("Compile Error: {0}", ex.Message);
                LogTo.Error(errmsg);
                await Shared.Utility.ShowErrorMsgAsync(this, errmsg);
            }
        }

        /// <summary>
            /// Gets the OpenConfigCommand command.
            /// </summary>
        public TaskCommand OpenConfigCommand { get; private set; }
       
        private void OpenConfiguration(string path)
        {
            using (var fileStream = File.Open(path, FileMode.Open))
            {
                var configuration = new SerializationConfiguration
                {
                    Culture = new System.Globalization.CultureInfo("en-US")
                };
                WindowModel = MainWindowModel.Load(fileStream, SerializationMode.Xml, configuration);
            }
            //open the plots
            foreach (PlotModel pm in Plots)
            {
                var viewModel = new PlotViewModel(pm);
                viewModel.ClosedAsync += PlotViewModel_ClosedAsync;
                ServiceLocator.Default.ResolveType<IUIVisualizerService>().Show(viewModel);
            }
            SaveConfigPath = path;
            this.ClearIsDirtyOnAllChilds();
            this.IsDirty = false;
            ViewModelCommandManager.InvalidateCommands(true);
            LogTo.Info("Loaded Plot configuration from: {0}", SaveConfigPath);
        }
        /// <summary>
        /// Method to invoke when the OpenConfigCommand command is executed.
        /// </summary>
        private async Task OnOpenConfigCommandExecuteAsync()
        {
            try
            {
                var openFileService = ServiceLocator.Default.ResolveType<IOpenFileService>();
                openFileService.Filter = "Kflop Loggging Configuration File| *.kfl";
                if (!string.IsNullOrEmpty(SaveConfigPath))
                {
                    openFileService.InitialDirectory = Path.GetDirectoryName(SaveConfigPath);
                    openFileService.FileName = Path.GetFileNameWithoutExtension(SaveConfigPath);
                }
                else
                {
                    openFileService.InitialDirectory = Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, @"..\Data"));

                }

                if (openFileService.DetermineFile())
                {
                    OpenConfiguration(openFileService.FileName);
                }
            }
            catch (Exception ex)
            {
                string errmsg = string.Format("Error loading Plot configuration: {0}", ex.Message);
                LogTo.Error(errmsg);
                await Shared.Utility.ShowErrorMsgAsync(this, errmsg);
            }
        }
    
        /// <summary>
        /// Gets the SaveConfigCommand command.
        /// </summary>
        public TaskCommand SaveConfigCommand { get; private set; }

        /// <summary>
        /// Method to invoke when the SaveConfigCommand command is executed.
        /// </summary>
        private async Task OnSaveConfigCommandExecuteAsync()
        {
            
            try
            {
                var saveFileService = ServiceLocator.Default.ResolveType<ISaveFileService>();
                saveFileService.Filter = "Kflop Loggging Configuration File|*.kfl";
                if (!string.IsNullOrEmpty(SaveConfigPath))
                {
                    saveFileService.InitialDirectory = Path.GetDirectoryName(SaveConfigPath);
                    if (!Directory.Exists(saveFileService.InitialDirectory))
                    {
                        saveFileService.InitialDirectory = Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, @"..\Data"));
                    }
                    saveFileService.FileName = Path.GetFileNameWithoutExtension(SaveConfigPath);
                }
                else
                {
                    saveFileService.InitialDirectory = Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, @"..\Data"));
                }
                if (saveFileService.DetermineFile())
                { 
                    if (SaveConfigPath != saveFileService.FileName)
                    {
                        SaveConfigPath = saveFileService.FileName;
                    }
                    using (var fileStream = File.Create(saveFileService.FileName))
                    {
                        var configuration = new SerializationConfiguration
                        {
                            Culture = new System.Globalization.CultureInfo("en-US")
                        };
                        WindowModel.Save(fileStream, Catel.Data.SerializationMode.Xml, configuration);
                    }                   
                    LogTo.Info("Saved configuration to: {0}", SaveConfigPath);
                }
            }
            catch (Exception ex)
            {
                string errmsg = string.Format("Error saving configuration: {0}", ex.Message);
                LogTo.Error(errmsg);
                await Shared.Utility.ShowErrorMsgAsync(this, errmsg);
            }
        }

        /// <summary>
            /// Gets the NewPlotCommand command.
            /// </summary>
        public TaskCommand NewPlotCommand { get; private set; }

        /// <summary>
        /// Method to invoke when the NewPlotCommand command is executed.
        /// </summary>
        private async Task OnNewPlotCommandExecuteAsync()
        {
            try
            {
                var plotmodel = new Models.PlotModel();
                Plots.Add(plotmodel);
                var viewModel = new PlotViewModel(plotmodel);
                viewModel.ClosedAsync += PlotViewModel_ClosedAsync;
                ServiceLocator.Default.ResolveType<IUIVisualizerService>().Show(viewModel);                
            }
            catch(Exception ex)
            {
                string errmsg = string.Format("Error creating new plot: {0}", ex.Message);
                LogTo.Error(errmsg);
                await Shared.Utility.ShowErrorMsgAsync(this, errmsg);
            }
        }

        /// <summary>
            /// Gets the StopStartCommand command.
            /// </summary>
        public TaskCommand<bool> StopStartCommand { get; private set; }        

        /// <summary>
        /// Method to check whether the StopStartCommand command can be executed.
        /// </summary>
        /// <returns><c>true</c> if the command can be executed; otherwise <c>false</c></returns>
        private bool OnStopStartCommandCanExecute(bool isChecked)
        {
            return Connected;
        }

        /// <summary>
        /// Method to invoke when the StopStartCommand command is executed.
        /// </summary>
        private async Task OnStopStartCommandExecuteAsync(bool isChecked)
        {
            if (isChecked)
            {
                _loggingService.Start();
            }
            else
            {
                //we only stop it on exit, pausing will just drop all in coming log messages
                _loggingService.Pause = true;
            }
            
        }
        /// <summary>
            /// Gets the HaultAllCommand command.
            /// </summary>
        public TaskCommand HaultAllCommand { get; private set; }

        /// <summary>
        /// Method to check whether the HaultAllCommand command can be executed.
        /// </summary>
        /// <returns><c>true</c> if the command can be executed; otherwise <c>false</c></returns>
        private bool OnHaultAllCommandCanExecute()
        {
            return true;
//            return _kserv.KMController.CheckConnected();
        }
        
        /// <summary>
        /// Method to invoke when the HaultAllCommand command is executed.
        /// </summary>
        private async Task OnHaultAllCommandExecuteAsync()
        {
            try
            {
                _kserv.KMController.KillProgramThreads(1,2,3,4,5,6,7);
            }
            catch (DMException ex)
            {
                string errmsg = string.Format("Error: {0}", ex.InnerException.Message);
                LogTo.Error(errmsg);
                await Shared.Utility.ShowErrorMsgAsync(this, errmsg);
            }
            catch (Exception ex)
            {
                string errmsg = string.Format("Unable to stop threads: {0}", ex.Message);
                LogTo.Error(errmsg);
                await Shared.Utility.ShowErrorMsgAsync(this, errmsg);
            }
        }

        /// <summary>
        /// Gets the SetThreadIDCommand command.
        /// </summary>
        public Command<int?> SetThreadIDCommand { get; private set; }

        /// <summary>
        /// Method to check whether the SetThreadIDCommand command can be executed.
        /// </summary>
        /// <returns><c>true</c> if the command can be executed; otherwise <c>false</c></returns>
        private bool OnSetThreadIDCommandCanExecute(int? value)
        {
            return SelectedCProgram != null;
        }

        /// <summary>
        /// Method to invoke when the SetThreadIDCommand command is executed.
        /// </summary>
        private void OnSetThreadIDCommandExecuteAsync(int? value)
        {
            // TODO: Handle command logic here
            SelectedCProgram.ThreadID = value;
            ViewModelCommandManager.InvalidateCommands(true);
        }

        /// <summary>
            /// Gets the AboutCommand command.
            /// </summary>
        public TaskCommand AboutCommand { get; private set; }
        
        /// <summary>
        /// Method to invoke when the AboutCommand command is executed.
        /// </summary>
        private async Task OnAboutCommandExecuteAsync()
        {
            //this isn't a Catel window so it is shown different then other "views".
            //this is just some open source Aboutbox
            AboutControlView about = new AboutControlView();
            AboutControlViewModel vm = (AboutControlViewModel)about.FindResource("ViewModel");

            // setting several properties here
            vm.ApplicationLogo = new BitmapImage(new System.Uri("pack://application:,,,/Resources/monitor.png"));
            vm.Title = "Extended Logging for Kflop";
            
            vm.Description = "Use for extended logging of Kflop variables.  First version by Greg Carter.  " +
                "Uses several open source projects including:\n - Catel (for WPF MVVM)\n - OxyPlot (for plotting)\n" +
                " - AvalonEdit (for C program syntax display)\n - Orc Controls (for log viewer)\n - AboutBox " +
                "(for this about box)\n" + "All icons are from others see icons.txt in install directory.";
            // ...
            vm.AdditionalNotes = "Use at your own risk! Support may be provided through Yahoo Dynomotion group.";
            vm.Publisher = "Greg Carter";           
            vm.HyperlinkText = "http://www.dnyomotion.com";
            //vm.PublisherLogo = new BitmapImage(new System.Uri("pack://application:,,,/Resources/graph.png"));

            //vm.Window.Foreground = System.Windows.Media.Brushes.White;
            vm.Window.Content = about;
            vm.Window.Show();
        }
#endregion
#region Methods
        public void OnSelectedCProgramPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            ThreadCProgram cp = sender as ThreadCProgram;
            if (cp == null) return;
            if (e.PropertyName == "ThreadID")
            {
                ListCollectionView list = (ListCollectionView)FreeThreads;
                //this always returns 'null' (for no thread id) plus the unused ids
                //list.Filter = new Predicate<object>(x => !((int?)x).HasValue || ThreadCPrograms.FirstOrDefault(c=>c.ThreadID == (int?)x) == null || (int?)x==SelectedCProgram.ThreadID);
                list.Filter = new Predicate<object>(x => (!((int?)x).HasValue || ThreadCPrograms.FirstOrDefault(c => c.ThreadID == (int?)x) == null) && (int?)x != SelectedCProgram.ThreadID);
                list.Refresh();

                ViewModelCommandManager.InvalidateCommands(true);
            }
        }
        public void OnSelectedCProgramChanged()
        {
            if (SelectedCProgram != null)
            {
                
                _selectedCProgramWatcher = new ChangeNotificationWrapper(SelectedCProgram);
                _selectedCProgramWatcher.PropertyChanged += OnSelectedCProgramPropertyChanged;
                ListCollectionView list = (ListCollectionView)FreeThreads;
                list.Filter = new Predicate<object>(x => (!((int?)x).HasValue || ThreadCPrograms.FirstOrDefault(c => c.ThreadID == (int?)x) == null) && (int?)x != SelectedCProgram.ThreadID);
                list.Refresh();
                var ftb = new StringBuilder();
                foreach (int? ft in list)
                {
                    ftb.Append(Convert.ToString(ft));
                    ftb.Append(" ");
                }
                LogTo.Info("Editing C program: {0}, free threads: {1}", SelectedCProgram.FileName, ftb.ToString());
            }
        }
        private int _skip;
        private void OnMainStatusTick(object sender, EventArgs e)
        {
            int[] List;
            int nBoards = 0;

            // with extra FTDI Drivers this can take a long time
            // so only call occasionally when not connected
            if (!Connected && ++_skip == 10)
            {
                _skip = 0;

                // check how many boards connected
                List = _kserv.KMController.GetBoards(out nBoards);
                if (nBoards > 0)
                {
                    Connected = true;
                    LogTo.Info("Dynomotion C# Forms App - Connected - USB location {0:X}", List[0]);
                    string verstr = _kserv.KMController.WriteLineReadLine("Version");
                    LogTo.Info(verstr);
                    StartLogging = true;
                    //ViewModelCommandManager.InvalidateCommands();
                    _lastElapsedms = _stopwatch.ElapsedMilliseconds;
                }
                else
                {
                    Connected = false;
                    StartLogging = false;
                }
            }

            if (Connected && _kserv.KMController.WaitToken(100) == KMOTION_TOKEN.KMOTION_LOCKED)
            {                
                try
                {
                    MainStatus = _kserv.KMController.GetStatus(false);  // we already have a lock
                    _kserv.KMController.ReleaseToken();
                    Thread1Executing = Convert.ToBoolean(MainStatus.GetThreadActive(1));
                    Thread2Executing = Convert.ToBoolean(MainStatus.GetThreadActive(2));
                    Thread3Executing = Convert.ToBoolean(MainStatus.GetThreadActive(3));
                    Thread4Executing = Convert.ToBoolean(MainStatus.GetThreadActive(4));
                    Thread5Executing = Convert.ToBoolean(MainStatus.GetThreadActive(5));
                    Thread6Executing = Convert.ToBoolean(MainStatus.GetThreadActive(6));
                    Thread7Executing = Convert.ToBoolean(MainStatus.GetThreadActive(7));

                    int lastByteCount = MessageByteCount;
                    MessageByteCount = _loggingService.MessageByteCount;
                    MessageCount = _loggingService.MessageCount;
                    long nowelapsedms = _stopwatch.ElapsedMilliseconds;
                    DataRate = (int)((_loggingService.MessageByteCount  - lastByteCount)/ ((nowelapsedms - _lastElapsedms) / 1000F)/1000);//kbytes/sec
                    _lastElapsedms = nowelapsedms;
                    //_stopwatch.Start();
                    if (DataRate > MaxDataRate) MaxDataRate = DataRate;

                }
                catch (DMException ex) // in case disconnect in the middle of reading status
                {
                    _kserv.KMController.ReleaseToken();
                    LogTo.Error(ex.InnerException.Message);
                }
            }
            else
            {
                Connected = false;
            }            
        }
        private async Task PlotViewModel_ClosedAsync(object sender, ViewModelClosedEventArgs e)
        {
            PlotViewModel pvm = e.ViewModel as PlotViewModel;
            if (pvm == null) return;
            Plots.Remove(pvm.PlotModel);
        }

        protected override async Task InitializeAsync()
        {
            await base.InitializeAsync();

            // TODO: subscribe to events here
        }

        protected override async Task CloseAsync()
        {
            // TODO: unsubscribe from events here
            _statusTimer.Stop();
            _statusTimer.Tick -= this.OnMainStatusTick;
            await base.CloseAsync();
        }
#endregion
    }
}
