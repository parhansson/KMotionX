namespace ExtendedLoggingKflop.Views
{
    using Anotar.Catel;
    using Catel.Windows;
    using ICSharpCode.AvalonEdit.Highlighting;
    using Services;
    public partial class MainWindow : Window
    {

        public MainWindow() : base()
        {
            InitializeComponent();
            textEditor.TextArea.IndentationStrategy = new ICSharpCode.AvalonEdit.Indentation.CSharp.CSharpIndentationStrategy();
            //set up tracking and apply state for the main window
            Services.Tracker.Configure(this)
                .AddProperties<MainWindow>(w => w.Height, w => w.Width, w => w.Left, w => w.Top, w => w.WindowState)
                .IdentifyAs("MainWindow")//not really needed since only one instance of MainWindow will ever exist, the default id is the name of the type
                .RegisterPersistTrigger("Closed")//not really needed in main window since the tracker will detect the application is closing and persist automatically
                .Apply();
        }

            

        private void Window_Activated(object sender, System.EventArgs e)
        {
            
        }
    }
}
