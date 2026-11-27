namespace ExtendedLoggingKflop.ViewModels
{
    using Catel.MVVM;
    using System.Threading.Tasks;
    using Models;
    using System.Collections.ObjectModel;
    using Catel;
    using Catel.Fody;
    using Catel.Services;
    using Catel.IoC;
    using System.IO;
    using Catel.Data;
    using System;
    using Anotar.Catel;
    using Catel.Runtime.Serialization;
    public class PlotConfigurationViewModel : ViewModelBase
    {
        private readonly IUIVisualizerService _uiVisualizerService;
        private readonly IMessageService _messageService;
        private readonly ISaveFileService _saveFileService;
        private readonly IOpenFileService _openFileService;
        public PlotConfigurationViewModel(PlotConfigurationModel config)
        {
            Argument.IsNotNull("config", config);
            _uiVisualizerService = ServiceLocator.Default.ResolveType<IUIVisualizerService>();
            _messageService = ServiceLocator.Default.ResolveType<IMessageService>();
            _saveFileService = ServiceLocator.Default.ResolveType<ISaveFileService>();
            _openFileService = ServiceLocator.Default.ResolveType<IOpenFileService>();
            Config = config;
            AddAxis = new TaskCommand(OnAddAxisExecuteAsync);
            EditAxis = new TaskCommand(OnEditAxisExecuteAsync, OnEditAxisCanExecute);
            RemoveAxis = new TaskCommand(OnRemoveAxisExecuteAsync, OnRemoveAxisCanExecute);

            AddSeries = new TaskCommand(OnAddSeriesExecuteAsync, OnAddSeriesCanExecute);
            EditSeries = new TaskCommand(OnEditSeriesExecuteAsync, OnEditSeriesCanExecute);
            RemoveSeries = new TaskCommand(OnRemoveSeriesExecuteAsync, OnRemoveSeriesCanExecute);

            SaveConfiguration = new TaskCommand(OnSaveConfigurationExecuteAsync, OnSaveConfigurationCanExecute);
            LoadConfiguration = new TaskCommand(OnLoadConfigurationExecuteAsync);
            IsDirty = false;
        }

        public override string Title { get { return "Plot Axis and Series Configuration"; } }

        [Model]
        //[Expose("Axes")]
        [Expose("Series")]
        [Expose("PlotName")]
        [Expose("Notes")]
        [Expose("SavePath")]
        public PlotConfigurationModel Config { get; set; }
        public AxisDefinitionModel SelectedAxis { get; set; }
        public SeriesDefinitionModel SelectedSeries { get; set; }
        /// <summary>
            /// Gets or sets the property value.
            /// </summary>
        [ViewModelToModel("Config")]
        public ObservableCollection<AxisDefinitionModel> Axes
        {
            get { return GetValue<ObservableCollection<AxisDefinitionModel>>(AxesProperty); }
            set { SetValue(AxesProperty, value); }
        }

        /// <summary>
        /// Register the name property so it is known in the class.
        /// </summary>
        public static readonly PropertyData AxesProperty = RegisterProperty("Axes", typeof(ObservableCollection<AxisDefinitionModel>));

        // TODO: Register models with the vmpropmodel codesnippet
        // TODO: Register view model properties with the vmprop or vmpropviewmodeltomodel codesnippets
        // TODO: Register commands with the vmcommand or vmcommandwithcanexecute codesnippets
        public TaskCommand AddAxis { get; private set; }

        /// <summary>
        /// Method to invoke when the Add command is executed.
        /// </summary>
        private async Task OnAddAxisExecuteAsync()
        {
            var viewModel = new AxisDefinitionViewModel(new AxisDefinitionModel());
            if (await _uiVisualizerService.ShowDialogAsync(viewModel) ?? false)
            {
                Axes.Add(viewModel.AxisDefinitionModel);
                ViewModelCommandManager.InvalidateCommands(true);
            }
        }
        public TaskCommand EditAxis { get; private set; }

        /// <summary>
        /// Method to check whether the Edit command can be executed.
        /// </summary>
        /// <returns></returns>
        private bool OnEditAxisCanExecute()
        {
            return (SelectedAxis != null);
        }

        /// <summary>
        /// Method to invoke when the Edit command is executed.
        /// </summary>
        private async Task OnEditAxisExecuteAsync()
        {
            var viewModel = new AxisDefinitionViewModel(SelectedAxis);
            await _uiVisualizerService.ShowDialogAsync(viewModel);
        }

        /// <summary>
        /// Gets the Remove command.
        /// </summary>
        public TaskCommand RemoveAxis { get; private set; }

        /// <summary>
        /// Method to check whether the Remove command can be executed.
        /// </summary>
        /// <returns></returns>
        private bool OnRemoveAxisCanExecute()
        {
            return (SelectedAxis != null);
        }

        /// <summary>
        /// Method to invoke when the Remove command is executed.
        /// </summary>
        private async Task OnRemoveAxisExecuteAsync()
        {
            if (await _messageService.ShowAsync("Are you sure you want to remove this axis?", "Are you sure?", MessageButton.YesNo) == MessageResult.Yes)
            {
                Axes.Remove(SelectedAxis);
                ViewModelCommandManager.InvalidateCommands(true);
            }
        }








        public TaskCommand AddSeries { get; private set; }

        private bool OnAddSeriesCanExecute()
        {
            return Axes.Count > 0;
        }
        /// <summary>
        /// Method to invoke when the Add command is executed.
        /// </summary>
        private async Task OnAddSeriesExecuteAsync()
        {
            var viewModel = new SeriesDefinitionViewModel(Axes);
            if (await _uiVisualizerService.ShowDialogAsync(viewModel) ?? false)
            {
                Config.Series.Add(viewModel.SeriesDefinitionModel);
                ViewModelCommandManager.InvalidateCommands(true);
            }
        }
        public TaskCommand EditSeries { get; private set; }

        /// <summary>
        /// Method to check whether the Edit command can be executed.
        /// </summary>
        /// <returns></returns>
        private bool OnEditSeriesCanExecute()
        {
            return (SelectedSeries != null);
        }

        /// <summary>
        /// Method to invoke when the Edit command is executed.
        /// </summary>
        private async Task OnEditSeriesExecuteAsync()
        {
            var viewModel = new SeriesDefinitionViewModel(Axes,SelectedSeries);
            await _uiVisualizerService.ShowDialogAsync(viewModel);
        }

        /// <summary>
        /// Gets the Remove command.
        /// </summary>
        public TaskCommand RemoveSeries { get; private set; }

        /// <summary>
        /// Method to check whether the Remove command can be executed.
        /// </summary>
        /// <returns></returns>
        private bool OnRemoveSeriesCanExecute()
        {
            return (SelectedSeries != null);
        }

        /// <summary>
        /// Method to invoke when the Remove command is executed.
        /// </summary>
        private async Task OnRemoveSeriesExecuteAsync()
        {
            if (await _messageService.ShowAsync("Are you sure you want to remove this series?", "Are you sure?", MessageButton.YesNo) == MessageResult.Yes)
            {
                Config.Series.Remove(SelectedSeries);
                ViewModelCommandManager.InvalidateCommands(true);
            }
        }

        public TaskCommand SaveConfiguration { get; private set; }

        private bool OnSaveConfigurationCanExecute()
        {
            return Axes.Count > 0 && Config.Series.Count > 0 && IsDirty;
        }

        private async Task OnSaveConfigurationExecuteAsync()
        {
            try
            {
                _saveFileService.Filter = "Kflop Plot File|*.kfp";
                if (!string.IsNullOrEmpty(Config.SavePath))
                {
                    _saveFileService.InitialDirectory = Path.GetDirectoryName(Config.SavePath);
                    if (!Directory.Exists(_saveFileService.InitialDirectory))
                    {
                        _saveFileService.InitialDirectory = Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, @"..\Data"));
                    }
                    _saveFileService.FileName = Path.GetFileNameWithoutExtension(Config.SavePath);
                }
                else
                {
                    _saveFileService.InitialDirectory = Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, @"..\Data"));
                }
                if (_saveFileService.DetermineFile())
                {
                    if (Config.SavePath != _saveFileService.FileName)
                    {
                        Config.SavePath = _saveFileService.FileName;
                    }
                    using (var fileStream = File.Create(_saveFileService.FileName))
                    {
                        var configuration = new SerializationConfiguration
                        {
                            Culture = new System.Globalization.CultureInfo("en-US")
                        };
                        Config.Save(fileStream, Catel.Data.SerializationMode.Xml, configuration);
                    }
                    
                    this.ClearIsDirtyOnAllChilds();
                    this.IsDirty = false;
                    ViewModelCommandManager.InvalidateCommands(true);
                    LogTo.Info("Saved Plot configuration to: {0}", Config.SavePath);
                }
            }
            catch (Exception ex)
            {
                string errmsg = string.Format("Error saving Plot configuration: {0}", ex.Message);
                LogTo.Error(errmsg);
                await Shared.Utility.ShowErrorMsgAsync(this, errmsg);
            }
        }

        public TaskCommand LoadConfiguration { get; private set; }
        private async Task OnLoadConfigurationExecuteAsync()
        {
            try
            {
                _openFileService.Filter = "Kflop Plot File|*.kfp";
                if (!string.IsNullOrEmpty(Config.SavePath))
                {
                    _openFileService.InitialDirectory = Path.GetDirectoryName(Config.SavePath);
                    _saveFileService.FileName = Path.GetFileNameWithoutExtension(Config.SavePath);
                }
                else
                {
                    _openFileService.InitialDirectory = Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, @"..\Data"));

                }

                if (_openFileService.DetermineFile())
                {
                    using (var fileStream = File.Open(_openFileService.FileName, FileMode.Open))
                    {
                        var configuration = new SerializationConfiguration
                        {
                            Culture = new System.Globalization.CultureInfo("en-US")
                        };
                        Config = PlotConfigurationModel.Load(fileStream, SerializationMode.Xml, configuration);                        
                    }
                    Config.SavePath = _openFileService.FileName;
                    this.ClearIsDirtyOnAllChilds();
                    this.IsDirty = false;
                    ViewModelCommandManager.InvalidateCommands(true);
                    LogTo.Info("Loaded Plot configuration from: {0}", Config.SavePath);

                }
            }
            catch (Exception ex)
            {
                string errmsg = string.Format("Error loading Plot configuration: {0}", ex.Message);
                LogTo.Error(errmsg);
                await Shared.Utility.ShowErrorMsgAsync(this,errmsg);
            }
        }
        private bool OnLoadConfigurationCanExecute()
        {
            return true;
        }
        protected override async Task InitializeAsync()
        {
            await base.InitializeAsync();

            // TODO: subscribe to events here
        }

        protected override async Task CloseAsync()
        {
            // TODO: unsubscribe from events here            
            await base.CloseAsync();
        }
    }
}
