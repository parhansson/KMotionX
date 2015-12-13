namespace MeasureFiducials
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.openFiducialsDialog = new System.Windows.Forms.OpenFileDialog();
            this.FiducialsName = new System.Windows.Forms.TextBox();
            this.OpenFiducialsDir = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.Save = new System.Windows.Forms.Button();
            this.JogXPos = new System.Windows.Forms.Button();
            this.JogXNeg = new System.Windows.Forms.Button();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.label1 = new System.Windows.Forms.Label();
            this.XPos = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label3 = new System.Windows.Forms.Label();
            this.YPos = new System.Windows.Forms.TextBox();
            this.JogYNeg = new System.Windows.Forms.Button();
            this.JogYPos = new System.Windows.Forms.Button();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.label13 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.LeftY = new System.Windows.Forms.TextBox();
            this.LeftYMeas = new System.Windows.Forms.TextBox();
            this.LeftXMeas = new System.Windows.Forms.TextBox();
            this.LeftX = new System.Windows.Forms.TextBox();
            this.MeasureLeft = new System.Windows.Forms.Button();
            this.GoLeftMeas = new System.Windows.Forms.Button();
            this.GoLeft = new System.Windows.Forms.Button();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.label14 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.RightY = new System.Windows.Forms.TextBox();
            this.RightYMeas = new System.Windows.Forms.TextBox();
            this.RightXMeas = new System.Windows.Forms.TextBox();
            this.RightX = new System.Windows.Forms.TextBox();
            this.MeasureRight = new System.Windows.Forms.Button();
            this.GoRightMeas = new System.Windows.Forms.Button();
            this.GoRight = new System.Windows.Forms.Button();
            this.Compute = new System.Windows.Forms.Button();
            this.Theta = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.ScaleFactor = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.XOff = new System.Windows.Forms.TextBox();
            this.YOff = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.MapFile = new System.Windows.Forms.TextBox();
            this.panelVideo = new System.Windows.Forms.Panel();
            this.ViewScreen = new System.Windows.Forms.PictureBox();
            this.Sight = new System.Windows.Forms.CheckBox();
            this.MagFactor = new System.Windows.Forms.TextBox();
            this.label11 = new System.Windows.Forms.Label();
            this.saveFiducialsDialog = new System.Windows.Forms.SaveFileDialog();
            this.ViewCheckBox = new System.Windows.Forms.CheckBox();
            this.ComputUnity = new System.Windows.Forms.Button();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.label18 = new System.Windows.Forms.Label();
            this.label16 = new System.Windows.Forms.Label();
            this.label17 = new System.Windows.Forms.Label();
            this.ViewY = new System.Windows.Forms.TextBox();
            this.MicroOffsetY = new System.Windows.Forms.TextBox();
            this.ViewX = new System.Windows.Forms.TextBox();
            this.MicroOffsetX = new System.Windows.Forms.TextBox();
            this.MeasureView = new System.Windows.Forms.Button();
            this.MeasureRef = new System.Windows.Forms.Button();
            this.RefY = new System.Windows.Forms.TextBox();
            this.RefX = new System.Windows.Forms.TextBox();
            this.GoView = new System.Windows.Forms.Button();
            this.GoRef = new System.Windows.Forms.Button();
            this.radioInch = new System.Windows.Forms.RadioButton();
            this.radiomm = new System.Windows.Forms.RadioButton();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.SaveMicroscopeOff = new System.Windows.Forms.Button();
            this.label19 = new System.Windows.Forms.Label();
            this.label20 = new System.Windows.Forms.Label();
            this.label22 = new System.Windows.Forms.Label();
            this.label23 = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ViewScreen)).BeginInit();
            this.groupBox5.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.SuspendLayout();
            // 
            // openFiducialsDialog
            // 
            this.openFiducialsDialog.DefaultExt = "txt";
            this.openFiducialsDialog.Title = "Open Fiducial List";
            // 
            // FiducialsName
            // 
            this.FiducialsName.Location = new System.Drawing.Point(19, 19);
            this.FiducialsName.Name = "FiducialsName";
            this.FiducialsName.Size = new System.Drawing.Size(384, 20);
            this.FiducialsName.TabIndex = 0;
            // 
            // OpenFiducialsDir
            // 
            this.OpenFiducialsDir.Location = new System.Drawing.Point(111, 43);
            this.OpenFiducialsDir.Name = "OpenFiducialsDir";
            this.OpenFiducialsDir.Size = new System.Drawing.Size(68, 23);
            this.OpenFiducialsDir.TabIndex = 3;
            this.OpenFiducialsDir.Text = "Open...";
            this.OpenFiducialsDir.UseVisualStyleBackColor = true;
            this.OpenFiducialsDir.Click += new System.EventHandler(this.OpenFiducialsDir_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.Save);
            this.groupBox1.Controls.Add(this.OpenFiducialsDir);
            this.groupBox1.Controls.Add(this.FiducialsName);
            this.groupBox1.Location = new System.Drawing.Point(11, 444);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(454, 79);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Fiducial Locations File";
            // 
            // Save
            // 
            this.Save.Location = new System.Drawing.Point(235, 43);
            this.Save.Name = "Save";
            this.Save.Size = new System.Drawing.Size(68, 23);
            this.Save.TabIndex = 3;
            this.Save.Text = "Save...";
            this.Save.UseVisualStyleBackColor = true;
            this.Save.Click += new System.EventHandler(this.SaveFiducialsDir_Click);
            // 
            // JogXPos
            // 
            this.JogXPos.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.JogXPos.Location = new System.Drawing.Point(721, 497);
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
            this.JogXNeg.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.JogXNeg.Location = new System.Drawing.Point(547, 497);
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
            // label2
            // 
            this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(637, 501);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(58, 31);
            this.label2.TabIndex = 9;
            this.label2.Text = "Jog";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Controls.Add(this.YPos);
            this.groupBox2.Controls.Add(this.XPos);
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Location = new System.Drawing.Point(122, 10);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(247, 118);
            this.groupBox2.TabIndex = 10;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Raw Machine Coordinates";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(33, 64);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(32, 31);
            this.label3.TabIndex = 12;
            this.label3.Text = "Y";
            // 
            // YPos
            // 
            this.YPos.BackColor = System.Drawing.SystemColors.Control;
            this.YPos.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.YPos.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.YPos.Location = new System.Drawing.Point(71, 64);
            this.YPos.Name = "YPos";
            this.YPos.Size = new System.Drawing.Size(155, 31);
            this.YPos.TabIndex = 11;
            this.YPos.Text = "0.000";
            this.YPos.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // JogYNeg
            // 
            this.JogYNeg.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.JogYNeg.Location = new System.Drawing.Point(630, 558);
            this.JogYNeg.Name = "JogYNeg";
            this.JogYNeg.Size = new System.Drawing.Size(65, 51);
            this.JogYNeg.TabIndex = 12;
            this.JogYNeg.Text = "Y -";
            this.JogYNeg.UseVisualStyleBackColor = true;
            this.JogYNeg.MouseLeave += new System.EventHandler(this.JogYStop);
            this.JogYNeg.Click += new System.EventHandler(this.JogYStop);
            this.JogYNeg.MouseDown += new System.Windows.Forms.MouseEventHandler(this.JogYNegMouseDown);
            // 
            // JogYPos
            // 
            this.JogYPos.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.JogYPos.Location = new System.Drawing.Point(630, 433);
            this.JogYPos.Name = "JogYPos";
            this.JogYPos.Size = new System.Drawing.Size(65, 51);
            this.JogYPos.TabIndex = 11;
            this.JogYPos.Text = "Y +";
            this.JogYPos.UseVisualStyleBackColor = true;
            this.JogYPos.MouseLeave += new System.EventHandler(this.JogYStop);
            this.JogYPos.Click += new System.EventHandler(this.JogYStop);
            this.JogYPos.MouseDown += new System.Windows.Forms.MouseEventHandler(this.JogYPosMouseDown);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.label13);
            this.groupBox3.Controls.Add(this.label12);
            this.groupBox3.Controls.Add(this.LeftY);
            this.groupBox3.Controls.Add(this.LeftYMeas);
            this.groupBox3.Controls.Add(this.LeftXMeas);
            this.groupBox3.Controls.Add(this.LeftX);
            this.groupBox3.Controls.Add(this.MeasureLeft);
            this.groupBox3.Controls.Add(this.GoLeftMeas);
            this.groupBox3.Controls.Add(this.GoLeft);
            this.groupBox3.Location = new System.Drawing.Point(12, 134);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(222, 164);
            this.groupBox3.TabIndex = 14;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Left Fiducial";
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(155, 21);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(54, 13);
            this.label13.TabIndex = 20;
            this.label13.Text = "Measured";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(38, 21);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(60, 13);
            this.label12.TabIndex = 20;
            this.label12.Text = "Theoretical";
            // 
            // LeftY
            // 
            this.LeftY.Location = new System.Drawing.Point(16, 61);
            this.LeftY.Name = "LeftY";
            this.LeftY.Size = new System.Drawing.Size(105, 20);
            this.LeftY.TabIndex = 18;
            this.LeftY.Text = "0";
            this.LeftY.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // LeftYMeas
            // 
            this.LeftYMeas.BackColor = System.Drawing.SystemColors.Control;
            this.LeftYMeas.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.LeftYMeas.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.LeftYMeas.Location = new System.Drawing.Point(127, 64);
            this.LeftYMeas.Name = "LeftYMeas";
            this.LeftYMeas.Size = new System.Drawing.Size(77, 15);
            this.LeftYMeas.TabIndex = 17;
            this.LeftYMeas.Text = "0.000";
            this.LeftYMeas.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // LeftXMeas
            // 
            this.LeftXMeas.BackColor = System.Drawing.SystemColors.Control;
            this.LeftXMeas.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.LeftXMeas.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.LeftXMeas.Location = new System.Drawing.Point(127, 38);
            this.LeftXMeas.Name = "LeftXMeas";
            this.LeftXMeas.Size = new System.Drawing.Size(77, 15);
            this.LeftXMeas.TabIndex = 16;
            this.LeftXMeas.Text = "0.000";
            this.LeftXMeas.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // LeftX
            // 
            this.LeftX.Location = new System.Drawing.Point(16, 37);
            this.LeftX.Name = "LeftX";
            this.LeftX.Size = new System.Drawing.Size(105, 20);
            this.LeftX.TabIndex = 19;
            this.LeftX.Text = "0";
            this.LeftX.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // MeasureLeft
            // 
            this.MeasureLeft.Location = new System.Drawing.Point(139, 124);
            this.MeasureLeft.Name = "MeasureLeft";
            this.MeasureLeft.Size = new System.Drawing.Size(65, 30);
            this.MeasureLeft.TabIndex = 14;
            this.MeasureLeft.Text = "Measure";
            this.MeasureLeft.UseVisualStyleBackColor = true;
            this.MeasureLeft.Click += new System.EventHandler(this.MeasureLeft_Click);
            // 
            // GoLeftMeas
            // 
            this.GoLeftMeas.Location = new System.Drawing.Point(139, 88);
            this.GoLeftMeas.Name = "GoLeftMeas";
            this.GoLeftMeas.Size = new System.Drawing.Size(65, 30);
            this.GoLeftMeas.TabIndex = 15;
            this.GoLeftMeas.Text = "Go";
            this.GoLeftMeas.UseVisualStyleBackColor = true;
            this.GoLeftMeas.Click += new System.EventHandler(this.GoLeftMeas_Click);
            // 
            // GoLeft
            // 
            this.GoLeft.Location = new System.Drawing.Point(41, 87);
            this.GoLeft.Name = "GoLeft";
            this.GoLeft.Size = new System.Drawing.Size(65, 30);
            this.GoLeft.TabIndex = 15;
            this.GoLeft.Text = "Go";
            this.GoLeft.UseVisualStyleBackColor = true;
            this.GoLeft.Click += new System.EventHandler(this.GoLeft_Click);
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.label14);
            this.groupBox4.Controls.Add(this.label15);
            this.groupBox4.Controls.Add(this.RightY);
            this.groupBox4.Controls.Add(this.RightYMeas);
            this.groupBox4.Controls.Add(this.RightXMeas);
            this.groupBox4.Controls.Add(this.RightX);
            this.groupBox4.Controls.Add(this.MeasureRight);
            this.groupBox4.Controls.Add(this.GoRightMeas);
            this.groupBox4.Controls.Add(this.GoRight);
            this.groupBox4.Location = new System.Drawing.Point(246, 134);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(219, 164);
            this.groupBox4.TabIndex = 14;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Right Fiducial";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(154, 22);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(54, 13);
            this.label14.TabIndex = 22;
            this.label14.Text = "Measured";
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(25, 22);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(60, 13);
            this.label15.TabIndex = 21;
            this.label15.Text = "Theoretical";
            // 
            // RightY
            // 
            this.RightY.Location = new System.Drawing.Point(16, 61);
            this.RightY.Name = "RightY";
            this.RightY.Size = new System.Drawing.Size(105, 20);
            this.RightY.TabIndex = 18;
            this.RightY.Text = "0";
            this.RightY.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // RightYMeas
            // 
            this.RightYMeas.BackColor = System.Drawing.SystemColors.Control;
            this.RightYMeas.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.RightYMeas.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.RightYMeas.Location = new System.Drawing.Point(127, 64);
            this.RightYMeas.Name = "RightYMeas";
            this.RightYMeas.Size = new System.Drawing.Size(77, 15);
            this.RightYMeas.TabIndex = 17;
            this.RightYMeas.Text = "0.000";
            this.RightYMeas.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // RightXMeas
            // 
            this.RightXMeas.BackColor = System.Drawing.SystemColors.Control;
            this.RightXMeas.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.RightXMeas.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.RightXMeas.Location = new System.Drawing.Point(127, 42);
            this.RightXMeas.Name = "RightXMeas";
            this.RightXMeas.Size = new System.Drawing.Size(77, 15);
            this.RightXMeas.TabIndex = 16;
            this.RightXMeas.Text = "0.000";
            this.RightXMeas.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // RightX
            // 
            this.RightX.Location = new System.Drawing.Point(16, 37);
            this.RightX.Name = "RightX";
            this.RightX.Size = new System.Drawing.Size(105, 20);
            this.RightX.TabIndex = 19;
            this.RightX.Text = "0";
            this.RightX.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // MeasureRight
            // 
            this.MeasureRight.Location = new System.Drawing.Point(139, 124);
            this.MeasureRight.Name = "MeasureRight";
            this.MeasureRight.Size = new System.Drawing.Size(65, 30);
            this.MeasureRight.TabIndex = 14;
            this.MeasureRight.Text = "Measure";
            this.MeasureRight.UseVisualStyleBackColor = true;
            this.MeasureRight.Click += new System.EventHandler(this.MeasureRight_Click);
            // 
            // GoRightMeas
            // 
            this.GoRightMeas.Location = new System.Drawing.Point(139, 87);
            this.GoRightMeas.Name = "GoRightMeas";
            this.GoRightMeas.Size = new System.Drawing.Size(65, 30);
            this.GoRightMeas.TabIndex = 15;
            this.GoRightMeas.Text = "Go";
            this.GoRightMeas.UseVisualStyleBackColor = true;
            this.GoRightMeas.Click += new System.EventHandler(this.GoRightMeas_Click);
            // 
            // GoRight
            // 
            this.GoRight.Location = new System.Drawing.Point(35, 87);
            this.GoRight.Name = "GoRight";
            this.GoRight.Size = new System.Drawing.Size(65, 30);
            this.GoRight.TabIndex = 15;
            this.GoRight.Text = "Go";
            this.GoRight.UseVisualStyleBackColor = true;
            this.GoRight.Click += new System.EventHandler(this.GoRight_Click);
            // 
            // Compute
            // 
            this.Compute.Location = new System.Drawing.Point(28, 530);
            this.Compute.Name = "Compute";
            this.Compute.Size = new System.Drawing.Size(215, 35);
            this.Compute.TabIndex = 14;
            this.Compute.Text = "Compute Geocorrection";
            this.Compute.UseVisualStyleBackColor = true;
            this.Compute.Click += new System.EventHandler(this.Compute_Click);
            // 
            // Theta
            // 
            this.Theta.BackColor = System.Drawing.SystemColors.Control;
            this.Theta.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.Theta.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Theta.Location = new System.Drawing.Point(351, 530);
            this.Theta.Name = "Theta";
            this.Theta.Size = new System.Drawing.Size(77, 15);
            this.Theta.TabIndex = 16;
            this.Theta.Text = "0.000";
            this.Theta.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(310, 530);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(35, 13);
            this.label4.TabIndex = 17;
            this.label4.Text = "Theta";
            // 
            // ScaleFactor
            // 
            this.ScaleFactor.BackColor = System.Drawing.SystemColors.Control;
            this.ScaleFactor.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.ScaleFactor.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.ScaleFactor.Location = new System.Drawing.Point(352, 551);
            this.ScaleFactor.Name = "ScaleFactor";
            this.ScaleFactor.Size = new System.Drawing.Size(77, 15);
            this.ScaleFactor.TabIndex = 16;
            this.ScaleFactor.Text = "0.000";
            this.ScaleFactor.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(311, 551);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(34, 13);
            this.label5.TabIndex = 17;
            this.label5.Text = "Scale";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(437, 532);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(25, 13);
            this.label6.TabIndex = 17;
            this.label6.Text = "deg";
            // 
            // XOff
            // 
            this.XOff.BackColor = System.Drawing.SystemColors.Control;
            this.XOff.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.XOff.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.XOff.Location = new System.Drawing.Point(352, 572);
            this.XOff.Name = "XOff";
            this.XOff.Size = new System.Drawing.Size(77, 15);
            this.XOff.TabIndex = 16;
            this.XOff.Text = "0.000";
            this.XOff.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // YOff
            // 
            this.YOff.BackColor = System.Drawing.SystemColors.Control;
            this.YOff.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.YOff.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.YOff.Location = new System.Drawing.Point(353, 593);
            this.YOff.Name = "YOff";
            this.YOff.Size = new System.Drawing.Size(77, 15);
            this.YOff.TabIndex = 16;
            this.YOff.Text = "0.000";
            this.YOff.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(311, 572);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(45, 13);
            this.label7.TabIndex = 17;
            this.label7.Text = "X Offset";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(438, 574);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(27, 13);
            this.label8.TabIndex = 17;
            this.label8.Text = "inch";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(312, 593);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(45, 13);
            this.label9.TabIndex = 17;
            this.label9.Text = "Y Offset";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(438, 595);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(27, 13);
            this.label10.TabIndex = 17;
            this.label10.Text = "inch";
            // 
            // MapFile
            // 
            this.MapFile.BackColor = System.Drawing.SystemColors.Control;
            this.MapFile.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.MapFile.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.MapFile.Location = new System.Drawing.Point(19, 644);
            this.MapFile.Name = "MapFile";
            this.MapFile.Size = new System.Drawing.Size(443, 13);
            this.MapFile.TabIndex = 16;
            this.MapFile.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // panelVideo
            // 
            this.panelVideo.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.panelVideo.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panelVideo.Location = new System.Drawing.Point(734, 433);
            this.panelVideo.Name = "panelVideo";
            this.panelVideo.Size = new System.Drawing.Size(0, 34);
            this.panelVideo.TabIndex = 18;
            // 
            // ViewScreen
            // 
            this.ViewScreen.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.ViewScreen.Location = new System.Drawing.Point(471, 3);
            this.ViewScreen.Name = "ViewScreen";
            this.ViewScreen.Size = new System.Drawing.Size(349, 370);
            this.ViewScreen.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.ViewScreen.TabIndex = 19;
            this.ViewScreen.TabStop = false;
            this.ViewScreen.MouseMove += new System.Windows.Forms.MouseEventHandler(this.DragMove);
            this.ViewScreen.MouseDown += new System.Windows.Forms.MouseEventHandler(this.DragDown);
            this.ViewScreen.Paint += new System.Windows.Forms.PaintEventHandler(this.View_screen_paint);
            this.ViewScreen.MouseUp += new System.Windows.Forms.MouseEventHandler(this.DragUp);
            // 
            // Sight
            // 
            this.Sight.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.Sight.AutoSize = true;
            this.Sight.Checked = true;
            this.Sight.CheckState = System.Windows.Forms.CheckState.Checked;
            this.Sight.Location = new System.Drawing.Point(721, 388);
            this.Sight.Name = "Sight";
            this.Sight.Size = new System.Drawing.Size(50, 17);
            this.Sight.TabIndex = 20;
            this.Sight.Text = "Sight";
            this.Sight.UseVisualStyleBackColor = true;
            // 
            // MagFactor
            // 
            this.MagFactor.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.MagFactor.Location = new System.Drawing.Point(738, 450);
            this.MagFactor.Name = "MagFactor";
            this.MagFactor.Size = new System.Drawing.Size(59, 20);
            this.MagFactor.TabIndex = 21;
            this.MagFactor.Text = "0.25";
            this.MagFactor.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label11
            // 
            this.label11.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(740, 431);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(63, 13);
            this.label11.TabIndex = 17;
            this.label11.Text = "Mouse Mag";
            // 
            // ViewCheckBox
            // 
            this.ViewCheckBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.ViewCheckBox.AutoSize = true;
            this.ViewCheckBox.Location = new System.Drawing.Point(548, 388);
            this.ViewCheckBox.Name = "ViewCheckBox";
            this.ViewCheckBox.Size = new System.Drawing.Size(49, 17);
            this.ViewCheckBox.TabIndex = 20;
            this.ViewCheckBox.Text = "View";
            this.ViewCheckBox.UseVisualStyleBackColor = true;
            this.ViewCheckBox.CheckedChanged += new System.EventHandler(this.ViewCheckBox_CheckedChanged);
            // 
            // ComputUnity
            // 
            this.ComputUnity.Location = new System.Drawing.Point(30, 571);
            this.ComputUnity.Name = "ComputUnity";
            this.ComputUnity.Size = new System.Drawing.Size(103, 28);
            this.ComputUnity.TabIndex = 5;
            this.ComputUnity.Text = "Compute Unity";
            this.ComputUnity.UseVisualStyleBackColor = true;
            this.ComputUnity.Click += new System.EventHandler(this.ComputeUnity_click);
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.label20);
            this.groupBox5.Controls.Add(this.label23);
            this.groupBox5.Controls.Add(this.label22);
            this.groupBox5.Controls.Add(this.label19);
            this.groupBox5.Controls.Add(this.label18);
            this.groupBox5.Controls.Add(this.label16);
            this.groupBox5.Controls.Add(this.label17);
            this.groupBox5.Controls.Add(this.ViewY);
            this.groupBox5.Controls.Add(this.MicroOffsetY);
            this.groupBox5.Controls.Add(this.ViewX);
            this.groupBox5.Controls.Add(this.MicroOffsetX);
            this.groupBox5.Controls.Add(this.SaveMicroscopeOff);
            this.groupBox5.Controls.Add(this.MeasureView);
            this.groupBox5.Controls.Add(this.MeasureRef);
            this.groupBox5.Controls.Add(this.RefY);
            this.groupBox5.Controls.Add(this.RefX);
            this.groupBox5.Controls.Add(this.GoView);
            this.groupBox5.Controls.Add(this.GoRef);
            this.groupBox5.Location = new System.Drawing.Point(53, 306);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(375, 130);
            this.groupBox5.TabIndex = 14;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Microscope Offset";
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Location = new System.Drawing.Point(306, 21);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(35, 13);
            this.label18.TabIndex = 20;
            this.label18.Text = "Offset";
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(176, 21);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(57, 13);
            this.label16.TabIndex = 20;
            this.label16.Text = "View Point";
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(28, 21);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(84, 13);
            this.label17.TabIndex = 20;
            this.label17.Text = "Reference Point";
            // 
            // ViewY
            // 
            this.ViewY.Location = new System.Drawing.Point(149, 61);
            this.ViewY.Name = "ViewY";
            this.ViewY.Size = new System.Drawing.Size(105, 20);
            this.ViewY.TabIndex = 18;
            this.ViewY.Text = "0";
            this.ViewY.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.ViewY.TextChanged += new System.EventHandler(this.ViewY_TextChanged);
            // 
            // MicroOffsetY
            // 
            this.MicroOffsetY.BackColor = System.Drawing.SystemColors.Control;
            this.MicroOffsetY.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.MicroOffsetY.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.MicroOffsetY.Location = new System.Drawing.Point(264, 64);
            this.MicroOffsetY.Name = "MicroOffsetY";
            this.MicroOffsetY.Size = new System.Drawing.Size(77, 15);
            this.MicroOffsetY.TabIndex = 17;
            this.MicroOffsetY.Text = "0.000";
            this.MicroOffsetY.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // ViewX
            // 
            this.ViewX.Location = new System.Drawing.Point(149, 37);
            this.ViewX.Name = "ViewX";
            this.ViewX.Size = new System.Drawing.Size(105, 20);
            this.ViewX.TabIndex = 19;
            this.ViewX.Text = "0";
            this.ViewX.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.ViewX.TextChanged += new System.EventHandler(this.ViewX_TextChanged);
            // 
            // MicroOffsetX
            // 
            this.MicroOffsetX.BackColor = System.Drawing.SystemColors.Control;
            this.MicroOffsetX.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.MicroOffsetX.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.MicroOffsetX.Location = new System.Drawing.Point(264, 42);
            this.MicroOffsetX.Name = "MicroOffsetX";
            this.MicroOffsetX.Size = new System.Drawing.Size(77, 15);
            this.MicroOffsetX.TabIndex = 16;
            this.MicroOffsetX.Text = "0.000";
            this.MicroOffsetX.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // MeasureView
            // 
            this.MeasureView.Location = new System.Drawing.Point(209, 88);
            this.MeasureView.Name = "MeasureView";
            this.MeasureView.Size = new System.Drawing.Size(61, 30);
            this.MeasureView.TabIndex = 14;
            this.MeasureView.Text = "Measure";
            this.MeasureView.UseVisualStyleBackColor = true;
            this.MeasureView.Click += new System.EventHandler(this.MeasureView_Click);
            // 
            // MeasureRef
            // 
            this.MeasureRef.Location = new System.Drawing.Point(72, 88);
            this.MeasureRef.Name = "MeasureRef";
            this.MeasureRef.Size = new System.Drawing.Size(61, 30);
            this.MeasureRef.TabIndex = 14;
            this.MeasureRef.Text = "Measure";
            this.MeasureRef.UseVisualStyleBackColor = true;
            this.MeasureRef.Click += new System.EventHandler(this.MeasureRef_Click);
            // 
            // RefY
            // 
            this.RefY.Location = new System.Drawing.Point(16, 61);
            this.RefY.Name = "RefY";
            this.RefY.Size = new System.Drawing.Size(105, 20);
            this.RefY.TabIndex = 18;
            this.RefY.Text = "0";
            this.RefY.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.RefY.TextChanged += new System.EventHandler(this.RefY_TextChanged);
            // 
            // RefX
            // 
            this.RefX.Location = new System.Drawing.Point(16, 37);
            this.RefX.Name = "RefX";
            this.RefX.Size = new System.Drawing.Size(105, 20);
            this.RefX.TabIndex = 19;
            this.RefX.Text = "0";
            this.RefX.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.RefX.TextChanged += new System.EventHandler(this.RefX_TextChanged);
            // 
            // GoView
            // 
            this.GoView.Location = new System.Drawing.Point(154, 88);
            this.GoView.Name = "GoView";
            this.GoView.Size = new System.Drawing.Size(50, 30);
            this.GoView.TabIndex = 15;
            this.GoView.Text = "Go";
            this.GoView.UseVisualStyleBackColor = true;
            this.GoView.Click += new System.EventHandler(this.GoView_Click);
            // 
            // GoRef
            // 
            this.GoRef.Location = new System.Drawing.Point(16, 88);
            this.GoRef.Name = "GoRef";
            this.GoRef.Size = new System.Drawing.Size(50, 30);
            this.GoRef.TabIndex = 15;
            this.GoRef.Text = "Go";
            this.GoRef.UseVisualStyleBackColor = true;
            this.GoRef.Click += new System.EventHandler(this.GoRef_Click);
            // 
            // radioInch
            // 
            this.radioInch.AutoSize = true;
            this.radioInch.Location = new System.Drawing.Point(14, 22);
            this.radioInch.Name = "radioInch";
            this.radioInch.Size = new System.Drawing.Size(46, 17);
            this.radioInch.TabIndex = 22;
            this.radioInch.TabStop = true;
            this.radioInch.Text = "Inch";
            this.radioInch.UseVisualStyleBackColor = true;
            this.radioInch.CheckedChanged += new System.EventHandler(this.radioInch_CheckedChanged);
            // 
            // radiomm
            // 
            this.radiomm.AutoSize = true;
            this.radiomm.Location = new System.Drawing.Point(14, 45);
            this.radiomm.Name = "radiomm";
            this.radiomm.Size = new System.Drawing.Size(41, 17);
            this.radiomm.TabIndex = 22;
            this.radiomm.TabStop = true;
            this.radiomm.Text = "mm";
            this.radiomm.UseVisualStyleBackColor = true;
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.radiomm);
            this.groupBox6.Controls.Add(this.radioInch);
            this.groupBox6.Location = new System.Drawing.Point(7, 12);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(78, 75);
            this.groupBox6.TabIndex = 23;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "units";
            // 
            // SaveMicroscopeOff
            // 
            this.SaveMicroscopeOff.Location = new System.Drawing.Point(291, 88);
            this.SaveMicroscopeOff.Name = "SaveMicroscopeOff";
            this.SaveMicroscopeOff.Size = new System.Drawing.Size(61, 30);
            this.SaveMicroscopeOff.TabIndex = 14;
            this.SaveMicroscopeOff.Text = "Save";
            this.SaveMicroscopeOff.UseVisualStyleBackColor = true;
            this.SaveMicroscopeOff.Click += new System.EventHandler(this.SaveMicroscopeOff_Click);
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label19.Location = new System.Drawing.Point(127, 35);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(19, 25);
            this.label19.TabIndex = 20;
            this.label19.Text = "-";
            // 
            // label20
            // 
            this.label20.AutoSize = true;
            this.label20.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label20.Location = new System.Drawing.Point(127, 55);
            this.label20.Name = "label20";
            this.label20.Size = new System.Drawing.Size(19, 25);
            this.label20.TabIndex = 20;
            this.label20.Text = "-";
            // 
            // label22
            // 
            this.label22.AutoSize = true;
            this.label22.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label22.Location = new System.Drawing.Point(260, 36);
            this.label22.Name = "label22";
            this.label22.Size = new System.Drawing.Size(24, 25);
            this.label22.TabIndex = 20;
            this.label22.Text = "=";
            // 
            // label23
            // 
            this.label23.AutoSize = true;
            this.label23.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label23.Location = new System.Drawing.Point(260, 57);
            this.label23.Name = "label23";
            this.label23.Size = new System.Drawing.Size(24, 25);
            this.label23.TabIndex = 20;
            this.label23.Text = "=";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(823, 618);
            this.Controls.Add(this.groupBox6);
            this.Controls.Add(this.MagFactor);
            this.Controls.Add(this.ViewCheckBox);
            this.Controls.Add(this.Sight);
            this.Controls.Add(this.ViewScreen);
            this.Controls.Add(this.panelVideo);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox5);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.YOff);
            this.Controls.Add(this.ScaleFactor);
            this.Controls.Add(this.XOff);
            this.Controls.Add(this.MapFile);
            this.Controls.Add(this.Theta);
            this.Controls.Add(this.JogYNeg);
            this.Controls.Add(this.JogYPos);
            this.Controls.Add(this.Compute);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.ComputUnity);
            this.Controls.Add(this.JogXNeg);
            this.Controls.Add(this.JogXPos);
            this.Controls.Add(this.groupBox1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimumSize = new System.Drawing.Size(790, 567);
            this.Name = "Form1";
            this.Text = "Dynomotion C# Forms App";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ViewScreen)).EndInit();
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.groupBox6.ResumeLayout(false);
            this.groupBox6.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.OpenFileDialog openFiducialsDialog;
        private System.Windows.Forms.TextBox FiducialsName;
        private System.Windows.Forms.Button OpenFiducialsDir;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button JogXPos;
        private System.Windows.Forms.Button JogXNeg;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox XPos;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox YPos;
        private System.Windows.Forms.Button JogYNeg;
        private System.Windows.Forms.Button JogYPos;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.TextBox LeftY;
        private System.Windows.Forms.TextBox LeftYMeas;
        private System.Windows.Forms.TextBox LeftXMeas;
        private System.Windows.Forms.TextBox LeftX;
        private System.Windows.Forms.Button MeasureLeft;
        private System.Windows.Forms.Button GoLeft;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.TextBox RightY;
        private System.Windows.Forms.TextBox RightYMeas;
        private System.Windows.Forms.TextBox RightXMeas;
        private System.Windows.Forms.TextBox RightX;
        private System.Windows.Forms.Button MeasureRight;
        private System.Windows.Forms.Button GoRight;
        private System.Windows.Forms.Button Compute;
        private System.Windows.Forms.TextBox Theta;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox ScaleFactor;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox XOff;
        private System.Windows.Forms.TextBox YOff;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox MapFile;
        private System.Windows.Forms.Panel panelVideo;
        private System.Windows.Forms.PictureBox ViewScreen;
        private System.Windows.Forms.CheckBox Sight;
        private System.Windows.Forms.TextBox MagFactor;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Button Save;
        private System.Windows.Forms.SaveFileDialog saveFiducialsDialog;
        private System.Windows.Forms.CheckBox ViewCheckBox;
        private System.Windows.Forms.Button ComputUnity;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Button GoLeftMeas;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Button GoRightMeas;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.TextBox RefY;
        private System.Windows.Forms.TextBox RefX;
        private System.Windows.Forms.Button GoView;
        private System.Windows.Forms.Button GoRef;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.TextBox ViewY;
        private System.Windows.Forms.TextBox MicroOffsetY;
        private System.Windows.Forms.TextBox ViewX;
        private System.Windows.Forms.TextBox MicroOffsetX;
        private System.Windows.Forms.Button MeasureView;
        private System.Windows.Forms.Button MeasureRef;
        private System.Windows.Forms.RadioButton radioInch;
        private System.Windows.Forms.RadioButton radiomm;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.Button SaveMicroscopeOff;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.Label label23;
        private System.Windows.Forms.Label label22;
    }
}

