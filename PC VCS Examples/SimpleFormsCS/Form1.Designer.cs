namespace SimpleFormsCS
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
            this.openCFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.CFileName = new System.Windows.Forms.TextBox();
            this.CompileLoadExec = new System.Windows.Forms.Button();
            this.OpenCFileDir = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.JogXPos = new System.Windows.Forms.Button();
            this.JogXNeg = new System.Windows.Forms.Button();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.label1 = new System.Windows.Forms.Label();
            this.XPos = new System.Windows.Forms.TextBox();
            this.XEnabled = new System.Windows.Forms.CheckBox();
            this.label2 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.SendCommand = new System.Windows.Forms.Button();
            this.Command = new System.Windows.Forms.TextBox();
            this.ConsoleTextBox = new System.Windows.Forms.TextBox();
            this.TestUSB = new System.Windows.Forms.Button();
            this.TestResults2 = new System.Windows.Forms.TextBox();
            this.TestResults1 = new System.Windows.Forms.TextBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // openCFileDialog
            // 
            this.openCFileDialog.DefaultExt = "c";
            this.openCFileDialog.FileName = "KSTEPInit3Axis.c";
            this.openCFileDialog.Title = "Open C Program";
            // 
            // CFileName
            // 
            this.CFileName.Location = new System.Drawing.Point(25, 35);
            this.CFileName.Name = "CFileName";
            this.CFileName.Size = new System.Drawing.Size(416, 20);
            this.CFileName.TabIndex = 0;
            // 
            // CompileLoadExec
            // 
            this.CompileLoadExec.Location = new System.Drawing.Point(187, 61);
            this.CompileLoadExec.Name = "CompileLoadExec";
            this.CompileLoadExec.Size = new System.Drawing.Size(146, 30);
            this.CompileLoadExec.TabIndex = 2;
            this.CompileLoadExec.Text = "Compile/Load/Execute";
            this.CompileLoadExec.UseVisualStyleBackColor = true;
            this.CompileLoadExec.Click += new System.EventHandler(this.CompileLoadExec_Click);
            // 
            // OpenCFileDir
            // 
            this.OpenCFileDir.Location = new System.Drawing.Point(447, 31);
            this.OpenCFileDir.Name = "OpenCFileDir";
            this.OpenCFileDir.Size = new System.Drawing.Size(75, 26);
            this.OpenCFileDir.TabIndex = 3;
            this.OpenCFileDir.Text = "Open...";
            this.OpenCFileDir.UseVisualStyleBackColor = true;
            this.OpenCFileDir.Click += new System.EventHandler(this.OpenCFileDir_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.OpenCFileDir);
            this.groupBox1.Controls.Add(this.CompileLoadExec);
            this.groupBox1.Controls.Add(this.CFileName);
            this.groupBox1.Location = new System.Drawing.Point(18, 181);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(536, 102);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "C File";
            // 
            // JogXPos
            // 
            this.JogXPos.Location = new System.Drawing.Point(301, 105);
            this.JogXPos.Name = "JogXPos";
            this.JogXPos.Size = new System.Drawing.Size(65, 51);
            this.JogXPos.TabIndex = 5;
            this.JogXPos.Text = "X +";
            this.JogXPos.UseVisualStyleBackColor = true;
            this.JogXPos.MouseLeave += new System.EventHandler(this.JogXStop);
            this.JogXPos.Click += new System.EventHandler(this.JogXStop);
            this.JogXPos.MouseDown += new System.Windows.Forms.MouseEventHandler(this.JogXPosMouseDown);
            // 
            // JogXNeg
            // 
            this.JogXNeg.Location = new System.Drawing.Point(109, 105);
            this.JogXNeg.Name = "JogXNeg";
            this.JogXNeg.Size = new System.Drawing.Size(65, 51);
            this.JogXNeg.TabIndex = 5;
            this.JogXNeg.Text = "X -";
            this.JogXNeg.UseVisualStyleBackColor = true;
            this.JogXNeg.MouseLeave += new System.EventHandler(this.JogXStop);
            this.JogXNeg.Click += new System.EventHandler(this.JogXStop);
            this.JogXNeg.MouseDown += new System.Windows.Forms.MouseEventHandler(this.JogXNegMouseDown);
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(33, 21);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(32, 31);
            this.label1.TabIndex = 6;
            this.label1.Text = "X";
            // 
            // XPos
            // 
            this.XPos.BackColor = System.Drawing.SystemColors.Control;
            this.XPos.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.XPos.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.XPos.Location = new System.Drawing.Point(71, 21);
            this.XPos.Name = "XPos";
            this.XPos.Size = new System.Drawing.Size(155, 31);
            this.XPos.TabIndex = 7;
            this.XPos.Text = "0.000";
            this.XPos.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // XEnabled
            // 
            this.XEnabled.AutoSize = true;
            this.XEnabled.Location = new System.Drawing.Point(261, 30);
            this.XEnabled.Name = "XEnabled";
            this.XEnabled.Size = new System.Drawing.Size(75, 17);
            this.XEnabled.TabIndex = 8;
            this.XEnabled.Text = "X Enabled";
            this.XEnabled.UseVisualStyleBackColor = true;
            this.XEnabled.Click += new System.EventHandler(this.XEnabled_Clicked);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(212, 113);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(58, 31);
            this.label2.TabIndex = 9;
            this.label2.Text = "Jog";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.XEnabled);
            this.groupBox2.Controls.Add(this.XPos);
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Location = new System.Drawing.Point(62, 12);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(397, 67);
            this.groupBox2.TabIndex = 10;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Status";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.SendCommand);
            this.groupBox3.Controls.Add(this.Command);
            this.groupBox3.Controls.Add(this.ConsoleTextBox);
            this.groupBox3.Location = new System.Drawing.Point(18, 300);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(533, 192);
            this.groupBox3.TabIndex = 11;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Console";
            // 
            // SendCommand
            // 
            this.SendCommand.Location = new System.Drawing.Point(463, 161);
            this.SendCommand.Name = "SendCommand";
            this.SendCommand.Size = new System.Drawing.Size(63, 29);
            this.SendCommand.TabIndex = 2;
            this.SendCommand.Text = "send";
            this.SendCommand.UseVisualStyleBackColor = true;
            this.SendCommand.Click += new System.EventHandler(this.SendCommand_Click);
            // 
            // Command
            // 
            this.Command.Location = new System.Drawing.Point(23, 166);
            this.Command.Name = "Command";
            this.Command.Size = new System.Drawing.Size(429, 20);
            this.Command.TabIndex = 1;
            this.Command.Text = "Version";
            // 
            // ConsoleTextBox
            // 
            this.ConsoleTextBox.Location = new System.Drawing.Point(23, 19);
            this.ConsoleTextBox.Multiline = true;
            this.ConsoleTextBox.Name = "ConsoleTextBox";
            this.ConsoleTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.ConsoleTextBox.Size = new System.Drawing.Size(503, 131);
            this.ConsoleTextBox.TabIndex = 0;
            // 
            // TestUSB
            // 
            this.TestUSB.Location = new System.Drawing.Point(18, 513);
            this.TestUSB.Name = "TestUSB";
            this.TestUSB.Size = new System.Drawing.Size(65, 21);
            this.TestUSB.TabIndex = 5;
            this.TestUSB.Text = "Test USB";
            this.TestUSB.UseVisualStyleBackColor = true;
            this.TestUSB.Click += new System.EventHandler(this.TestUSB_Click);
            // 
            // TestResults2
            // 
            this.TestResults2.BackColor = System.Drawing.SystemColors.Control;
            this.TestResults2.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.TestResults2.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.TestResults2.Location = new System.Drawing.Point(89, 518);
            this.TestResults2.Name = "TestResults2";
            this.TestResults2.Size = new System.Drawing.Size(462, 16);
            this.TestResults2.TabIndex = 7;
            this.TestResults2.Text = "Not Tested";
            this.TestResults2.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // TestResults1
            // 
            this.TestResults1.BackColor = System.Drawing.SystemColors.Control;
            this.TestResults1.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.TestResults1.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.TestResults1.Location = new System.Drawing.Point(89, 498);
            this.TestResults1.Name = "TestResults1";
            this.TestResults1.Size = new System.Drawing.Size(462, 16);
            this.TestResults1.TabIndex = 7;
            this.TestResults1.Text = "Not Tested";
            this.TestResults1.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // Form1
            // 
            this.AcceptButton = this.SendCommand;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(579, 549);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.TestResults1);
            this.Controls.Add(this.TestResults2);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.JogXNeg);
            this.Controls.Add(this.TestUSB);
            this.Controls.Add(this.JogXPos);
            this.Controls.Add(this.groupBox1);
            this.Name = "Form1";
            this.Text = "Dynomotion C# Forms App";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.OpenFileDialog openCFileDialog;
        private System.Windows.Forms.TextBox CFileName;
        private System.Windows.Forms.Button CompileLoadExec;
        private System.Windows.Forms.Button OpenCFileDir;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button JogXPos;
        private System.Windows.Forms.Button JogXNeg;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox XPos;
        private System.Windows.Forms.CheckBox XEnabled;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.TextBox ConsoleTextBox;
        private System.Windows.Forms.Button SendCommand;
        private System.Windows.Forms.TextBox Command;
        private System.Windows.Forms.Button TestUSB;
        private System.Windows.Forms.TextBox TestResults2;
        private System.Windows.Forms.TextBox TestResults1;
    }
}

