namespace ExtendedLoggingKflop.Views
{
    using Catel.Windows;
    using ViewModels;

    public partial class SeriesDefinitionView
    {
        public SeriesDefinitionView()
            : this(null) { }

        public SeriesDefinitionView(SeriesDefinitionViewModel viewModel)
            : base(viewModel)
        {
            InitializeComponent();
        }
    }
}
