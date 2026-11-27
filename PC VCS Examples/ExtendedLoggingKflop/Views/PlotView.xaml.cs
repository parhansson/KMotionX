namespace ExtendedLoggingKflop.Views
{   
    using Catel.Windows;
    public partial class PlotView:Window
    {
        public PlotView() : base()
        {
            InitializeComponent();
            this.Owner = null;//this allows the main window to cover the plot
        }
    }
}
