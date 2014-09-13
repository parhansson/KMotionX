namespace TeachMotion
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.CFileName = new System.Windows.Forms.TextBox();
            this.OpenCFileDir = new System.Windows.Forms.Button();
            this.openCFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.ReadData = new System.Windows.Forms.Button();
            this.XOffset = new System.Windows.Forms.TextBox();
            this.offset = new System.Windows.Forms.Label();
            this.WriteOffsetData = new System.Windows.Forms.Button();
            this.ReadStatus = new System.Windows.Forms.Label();
            this.glControl1 = new OpenTK.GLControl();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.joystick1 = new Joystick();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.YEnabled = new System.Windows.Forms.CheckBox();
            this.XEnabled = new System.Windows.Forms.CheckBox();
            this.YPos = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.XPos = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.SyncEnable = new System.Windows.Forms.CheckBox();
            this.CaptureOn = new System.Windows.Forms.CheckBox();
            this.ShowData = new System.Windows.Forms.CheckBox();
            this.ClearData = new System.Windows.Forms.Button();
            this.NPtsCaptured = new System.Windows.Forms.Label();
            this.WriteData = new System.Windows.Forms.Button();
            this.saveTeachFileDlg = new System.Windows.Forms.SaveFileDialog();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // CFileName
            // 
            this.CFileName.Location = new System.Drawing.Point(18, 259);
            this.CFileName.Name = "CFileName";
            this.CFileName.Size = new System.Drawing.Size(649, 20);
            this.CFileName.TabIndex = 1;
            // 
            // OpenCFileDir
            // 
            this.OpenCFileDir.Location = new System.Drawing.Point(673, 259);
            this.OpenCFileDir.Name = "OpenCFileDir";
            this.OpenCFileDir.Size = new System.Drawing.Size(75, 26);
            this.OpenCFileDir.TabIndex = 4;
            this.OpenCFileDir.Text = "Open...";
            this.OpenCFileDir.UseVisualStyleBackColor = true;
            this.OpenCFileDir.Click += new System.EventHandler(this.OpenCFileDir_Click);
            // 
            // openCFileDialog
            // 
            this.openCFileDialog.DefaultExt = "c";
            this.openCFileDialog.FileName = "KSTEPInit3Axis.c";
            this.openCFileDialog.Title = "Open C Program";
            // 
            // ReadData
            // 
            this.ReadData.Location = new System.Drawing.Point(18, 285);
            this.ReadData.Name = "ReadData";
            this.ReadData.Size = new System.Drawing.Size(151, 31);
            this.ReadData.TabIndex = 5;
            this.ReadData.Text = "Read Probe Data";
            this.ReadData.UseVisualStyleBackColor = true;
            this.ReadData.Click += new System.EventHandler(this.ReadData_Click);
            // 
            // XOffset
            // 
            this.XOffset.Location = new System.Drawing.Point(54, 344);
            this.XOffset.Name = "XOffset";
            this.XOffset.Size = new System.Drawing.Size(79, 20);
            this.XOffset.TabIndex = 6;
            this.XOffset.Text = "0.06";
            this.XOffset.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // offset
            // 
            this.offset.AutoSize = true;
            this.offset.Location = new System.Drawing.Point(15, 347);
            this.offset.Name = "offset";
            this.offset.Size = new System.Drawing.Size(33, 13);
            this.offset.TabIndex = 7;
            this.offset.Text = "offset";
            // 
            // WriteOffsetData
            // 
            this.WriteOffsetData.Location = new System.Drawing.Point(18, 375);
            this.WriteOffsetData.Name = "WriteOffsetData";
            this.WriteOffsetData.Size = new System.Drawing.Size(151, 31);
            this.WriteOffsetData.TabIndex = 5;
            this.WriteOffsetData.Text = "Write Offset Data";
            this.WriteOffsetData.UseVisualStyleBackColor = true;
            this.WriteOffsetData.Click += new System.EventHandler(this.WriteOffsetData_Click);
            // 
            // ReadStatus
            // 
            this.ReadStatus.AutoSize = true;
            this.ReadStatus.Location = new System.Drawing.Point(189, 294);
            this.ReadStatus.Name = "ReadStatus";
            this.ReadStatus.Size = new System.Drawing.Size(47, 13);
            this.ReadStatus.TabIndex = 8;
            this.ReadStatus.Text = "No Data";
            // 
            // glControl1
            // 
            this.glControl1.BackColor = System.Drawing.Color.Black;
            this.glControl1.Location = new System.Drawing.Point(12, 12);
            this.glControl1.Name = "glControl1";
            this.glControl1.Size = new System.Drawing.Size(760, 205);
            this.glControl1.TabIndex = 9;
            this.glControl1.VSync = false;
            this.glControl1.Load += new System.EventHandler(this.glControl1_Load);
            this.glControl1.Paint += new System.Windows.Forms.PaintEventHandler(this.glControl1_Paint);
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Interval = 50;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // joystick1
            // 
            this.joystick1.BandColor = System.Drawing.Color.MidnightBlue;
            this.joystick1.BandWidth = 3F;
            this.joystick1.CausesValidation = false;
            this.joystick1.Cursor = System.Windows.Forms.Cursors.Cross;
            this.joystick1.Diameter = 183D;
            this.joystick1.EndCap = System.Drawing.Drawing2D.LineCap.Flat;
            this.joystick1.InsideColor = System.Drawing.Color.LightGray;
            this.joystick1.Location = new System.Drawing.Point(267, 347);
            this.joystick1.Name = "joystick1";
            this.joystick1.Size = new System.Drawing.Size(183, 183);
            this.joystick1.TabIndex = 10;
            this.joystick1.Text = "joystick1";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.YEnabled);
            this.groupBox2.Controls.Add(this.XEnabled);
            this.groupBox2.Controls.Add(this.YPos);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Controls.Add(this.XPos);
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Location = new System.Drawing.Point(456, 294);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(316, 123);
            this.groupBox2.TabIndex = 11;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Status";
            // 
            // YEnabled
            // 
            this.YEnabled.AutoSize = true;
            this.YEnabled.Location = new System.Drawing.Point(197, 67);
            this.YEnabled.Name = "YEnabled";
            this.YEnabled.Size = new System.Drawing.Size(75, 17);
            this.YEnabled.TabIndex = 8;
            this.YEnabled.Text = "Y Enabled";
            this.YEnabled.UseVisualStyleBackColor = true;
            this.YEnabled.Click += new System.EventHandler(this.YEnabled_Click);
            // 
            // XEnabled
            // 
            this.XEnabled.AutoSize = true;
            this.XEnabled.Location = new System.Drawing.Point(197, 30);
            this.XEnabled.Name = "XEnabled";
            this.XEnabled.Size = new System.Drawing.Size(75, 17);
            this.XEnabled.TabIndex = 8;
            this.XEnabled.Text = "X Enabled";
            this.XEnabled.UseVisualStyleBackColor = true;
            this.XEnabled.Click += new System.EventHandler(this.XEnabled_Click);
            // 
            // YPos
            // 
            this.YPos.BackColor = System.Drawing.SystemColors.Control;
            this.YPos.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.YPos.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.YPos.Location = new System.Drawing.Point(53, 58);
            this.YPos.Name = "YPos";
            this.YPos.Size = new System.Drawing.Size(133, 31);
            this.YPos.TabIndex = 7;
            this.YPos.Text = "0.000";
            this.YPos.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(17, 59);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(32, 31);
            this.label2.TabIndex = 6;
            this.label2.Text = "Y";
            // 
            // XPos
            // 
            this.XPos.BackColor = System.Drawing.SystemColors.Control;
            this.XPos.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.XPos.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.XPos.Location = new System.Drawing.Point(53, 21);
            this.XPos.Name = "XPos";
            this.XPos.Size = new System.Drawing.Size(133, 31);
            this.XPos.TabIndex = 7;
            this.XPos.Text = "0.000";
            this.XPos.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(17, 22);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(32, 31);
            this.label1.TabIndex = 6;
            this.label1.Text = "X";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(27, 436);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(106, 55);
            this.button1.TabIndex = 12;
            this.button1.Text = "INIT";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.INIT);
            // 
            // SyncEnable
            // 
            this.SyncEnable.AutoSize = true;
            this.SyncEnable.Location = new System.Drawing.Point(27, 513);
            this.SyncEnable.Name = "SyncEnable";
            this.SyncEnable.Size = new System.Drawing.Size(130, 17);
            this.SyncEnable.TabIndex = 13;
            this.SyncEnable.Text = "Sync && Enable Motion";
            this.SyncEnable.UseVisualStyleBackColor = true;
            this.SyncEnable.CheckedChanged += new System.EventHandler(this.SyncEnable_CheckedChanged);
            // 
            // CaptureOn
            // 
            this.CaptureOn.AutoSize = true;
            this.CaptureOn.Location = new System.Drawing.Point(558, 438);
            this.CaptureOn.Name = "CaptureOn";
            this.CaptureOn.Size = new System.Drawing.Size(106, 17);
            this.CaptureOn.TabIndex = 13;
            this.CaptureOn.Text = "Data Capture On";
            this.CaptureOn.UseVisualStyleBackColor = true;
            this.CaptureOn.CheckedChanged += new System.EventHandler(this.CaptureOn_CheckedChanged);
            // 
            // ShowData
            // 
            this.ShowData.AutoSize = true;
            this.ShowData.Checked = true;
            this.ShowData.CheckState = System.Windows.Forms.CheckState.Checked;
            this.ShowData.Location = new System.Drawing.Point(558, 461);
            this.ShowData.Name = "ShowData";
            this.ShowData.Size = new System.Drawing.Size(79, 17);
            this.ShowData.TabIndex = 13;
            this.ShowData.Text = "Show Data";
            this.ShowData.UseVisualStyleBackColor = true;
            // 
            // ClearData
            // 
            this.ClearData.Location = new System.Drawing.Point(558, 487);
            this.ClearData.Name = "ClearData";
            this.ClearData.Size = new System.Drawing.Size(84, 31);
            this.ClearData.TabIndex = 5;
            this.ClearData.Text = "ClearData";
            this.ClearData.UseVisualStyleBackColor = true;
            this.ClearData.Click += new System.EventHandler(this.ClearData_Click);
            // 
            // NPtsCaptured
            // 
            this.NPtsCaptured.AutoSize = true;
            this.NPtsCaptured.Location = new System.Drawing.Point(670, 438);
            this.NPtsCaptured.Name = "NPtsCaptured";
            this.NPtsCaptured.Size = new System.Drawing.Size(13, 13);
            this.NPtsCaptured.TabIndex = 14;
            this.NPtsCaptured.Text = "0";
            // 
            // WriteData
            // 
            this.WriteData.Location = new System.Drawing.Point(664, 487);
            this.WriteData.Name = "WriteData";
            this.WriteData.Size = new System.Drawing.Size(84, 31);
            this.WriteData.TabIndex = 5;
            this.WriteData.Text = "Write Data";
            this.WriteData.UseVisualStyleBackColor = true;
            this.WriteData.Click += new System.EventHandler(this.WriteData_Click);
            // 
            // saveTeachFileDlg
            // 
            this.saveTeachFileDlg.DefaultExt = "txt";
            this.saveTeachFileDlg.Title = "Save Teach File";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(784, 540);
            this.Controls.Add(this.NPtsCaptured);
            this.Controls.Add(this.ShowData);
            this.Controls.Add(this.CaptureOn);
            this.Controls.Add(this.SyncEnable);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.joystick1);
            this.Controls.Add(this.glControl1);
            this.Controls.Add(this.ReadStatus);
            this.Controls.Add(this.offset);
            this.Controls.Add(this.XOffset);
            this.Controls.Add(this.WriteOffsetData);
            this.Controls.Add(this.WriteData);
            this.Controls.Add(this.ClearData);
            this.Controls.Add(this.ReadData);
            this.Controls.Add(this.OpenCFileDir);
            this.Controls.Add(this.CFileName);
            this.Name = "Form1";
            this.Text = "Teach Motion";
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox CFileName;
        private System.Windows.Forms.Button OpenCFileDir;
        private System.Windows.Forms.OpenFileDialog openCFileDialog;
        private System.Windows.Forms.Button ReadData;
        private System.Windows.Forms.TextBox XOffset;
        private System.Windows.Forms.Label offset;
        private System.Windows.Forms.Button WriteOffsetData;
        private System.Windows.Forms.Label ReadStatus;
        private OpenTK.GLControl glControl1;
        private System.Windows.Forms.Timer timer1;
        private Joystick joystick1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.CheckBox XEnabled;
        private System.Windows.Forms.TextBox XPos;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox YEnabled;
        private System.Windows.Forms.TextBox YPos;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.CheckBox SyncEnable;
        private System.Windows.Forms.CheckBox CaptureOn;
        private System.Windows.Forms.CheckBox ShowData;
        private System.Windows.Forms.Button ClearData;
        private System.Windows.Forms.Label NPtsCaptured;
        private System.Windows.Forms.Button WriteData;
        private System.Windows.Forms.SaveFileDialog saveTeachFileDlg;
    }
}

