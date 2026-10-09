namespace ExtendedLoggingKflop.Views
{
    using Catel.Windows;
    using ViewModels;
    public partial class AxisDefinitionView : DataWindow
    {
        public AxisDefinitionView(AxisDefinitionViewModel viewModel) : base(viewModel)
        {
            InitializeComponent();
        }
    }
}
