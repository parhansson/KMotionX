using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Security.Permissions;
using KFlopWebNC.Model;

namespace KFlopWebNC
{
    [PermissionSet(SecurityAction.Demand, Name = "FullTrust")]
    [System.Runtime.InteropServices.ComVisibleAttribute(true)]
    public partial class KFlopHost : Form
    {
        public KFlopHost()
        {
            InitializeComponent();
            Load += new EventHandler(KFlopHost_Load);
        }

        void KFlopHost_Load(object sender, EventArgs e)
        {
            ZoomBrowser.ZoomBrowser zb = new ZoomBrowser.ZoomBrowser();
            this.Controls.Add(zb);
            zb.ScrollBarsEnabled = false;
            zb.Dock = DockStyle.Fill;
            DeviceInteropHandler handler = new DeviceInteropHandler(zb);  
        } 
    }
}
