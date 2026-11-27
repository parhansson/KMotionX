using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace SimpleFormsCS
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            int IP_Addr = 0;
            int v = 0;
            bool foundi = false;
            foreach (string arg in args)
            {
                if (foundi)
                {
                    var parts = arg.Split('.');
                    if (parts.Count() != 4 || parts[0].Count() < 1 || parts[1].Count() < 1 || parts[2].Count() < 1 || parts[3].Count() < 1)
                    {
                        MessageBox.Show("Invalid Command Line Option\r\r" + arg);
                        break;
                    }
                    for (int i = 0; i < 4; i++)
                    {
                        if (!Int32.TryParse(parts[i], out v))
                        {
                            MessageBox.Show("Invalid Command Line Option\r\r" + arg);
                            break;
                        }
                        if (v < 0 || v > 255)
                        {
                            MessageBox.Show("Invalid Command Line Option\r\r" + arg);
                            break;
                        }
                        IP_Addr |= v << ((3 - i) * 8);
                    }
                }
                foundi = (arg == "/i" || arg == "-i");
            }

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1(IP_Addr));
        }
    }
}
