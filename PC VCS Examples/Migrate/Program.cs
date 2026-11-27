using System;
using System.IO;
using System.Windows.Forms;

namespace Migrate
{
    internal static class Program
    {
        [STAThread]
        private static void Main(string[] args)
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            try
            {
                string source = null;
                string destination = null;
                for (int i = 0; i < args.Length; i++)
                {
                    if (args[i] == "--help" || args[i] == "/?")
                    {
                        MessageBox.Show("Migrate.exe [--destination <installation root>] [--source <previous installation root>]\r\n\r\n" +
                            "Select Preview to review the merge. Files are copied only after you select Merge settings and confirm.",
                            "Migrate settings", MessageBoxButtons.OK, MessageBoxIcon.Information);
                        return;
                    }
                    if (args[i] != "--destination" && args[i] != "--source")
                        throw new ArgumentException("Unknown option: " + args[i]);
                    string option = args[i];
                    if (++i >= args.Length || args[i].StartsWith("--", StringComparison.Ordinal))
                        throw new ArgumentException("A folder must follow " + option + ".");
                    if (option == "--destination") destination = InstallationPaths.NormalizeRoot(args[i]);
                    else source = InstallationPaths.NormalizeRoot(args[i]);
                }
                if (destination == null) destination = DetectDestination();
                Application.Run(new MigrationForm(source, destination));
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Migrate settings", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Environment.ExitCode = 1;
            }
        }

        private static string DetectDestination()
        {
            // Installed layout: <installation>\KMotion\Release[64]\Migrate.exe.
            // An example started elsewhere asks for a destination instead of guessing.
            DirectoryInfo executableDirectory = new DirectoryInfo(Application.StartupPath);
            DirectoryInfo kmotion = executableDirectory.Parent;
            if (kmotion != null && kmotion.Name.Equals("KMotion", StringComparison.OrdinalIgnoreCase) && kmotion.Parent != null)
            {
                string root = kmotion.Parent.FullName;
                if (Directory.Exists(Path.Combine(root, "KMotion", "Data"))) return root;
            }
            return null;
        }
    }
}
