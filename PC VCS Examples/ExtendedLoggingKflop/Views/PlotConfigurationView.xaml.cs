namespace ExtendedLoggingKflop.Views
{
    using Catel.Windows;
    using ViewModels;

    public partial class PlotConfigurationView
    {
        public PlotConfigurationView()
            : this(null) { }

        public PlotConfigurationView(PlotConfigurationViewModel viewModel)
            : base(viewModel)
        {
            AddCustomButton(new DataWindowButton("Load", viewModel.LoadConfiguration));
            AddCustomButton(new DataWindowButton("Save", viewModel.SaveConfiguration));
            
            InitializeComponent();
            
        }
    }
}
