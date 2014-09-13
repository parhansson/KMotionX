<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Form1
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container
        Me.txtXAxisCommanded = New System.Windows.Forms.TextBox
        Me.Label1 = New System.Windows.Forms.Label
        Me.Label2 = New System.Windows.Forms.Label
        Me.txtYAxisCommanded = New System.Windows.Forms.TextBox
        Me.Label3 = New System.Windows.Forms.Label
        Me.txtZAxisCommanded = New System.Windows.Forms.TextBox
        Me.grpAxisPositions = New System.Windows.Forms.GroupBox
        Me.chkEnableZ = New System.Windows.Forms.CheckBox
        Me.btnStopAll = New System.Windows.Forms.Button
        Me.btnMoveZ = New System.Windows.Forms.Button
        Me.chkEnableY = New System.Windows.Forms.CheckBox
        Me.btnMoveY = New System.Windows.Forms.Button
        Me.chkEnableX = New System.Windows.Forms.CheckBox
        Me.btnMoveX = New System.Windows.Forms.Button
        Me.GroupBox1 = New System.Windows.Forms.GroupBox
        Me.lblZJogPercent = New System.Windows.Forms.Label
        Me.Label9 = New System.Windows.Forms.Label
        Me.sldJogZ = New System.Windows.Forms.TrackBar
        Me.lblYJogPercent = New System.Windows.Forms.Label
        Me.Label7 = New System.Windows.Forms.Label
        Me.sldJogY = New System.Windows.Forms.TrackBar
        Me.lblXJogPercent = New System.Windows.Forms.Label
        Me.Label4 = New System.Windows.Forms.Label
        Me.sldJogX = New System.Windows.Forms.TrackBar
        Me.GroupBox2 = New System.Windows.Forms.GroupBox
        Me.Label10 = New System.Windows.Forms.Label
        Me.Label11 = New System.Windows.Forms.Label
        Me.txtXAxisPos = New System.Windows.Forms.TextBox
        Me.txtZAxisPos = New System.Windows.Forms.TextBox
        Me.txtYAxisPos = New System.Windows.Forms.TextBox
        Me.Label12 = New System.Windows.Forms.Label
        Me.GroupBox3 = New System.Windows.Forms.GroupBox
        Me.chkToggleLED_1 = New System.Windows.Forms.CheckBox
        Me.chkToggleLED_0 = New System.Windows.Forms.CheckBox
        Me.Timer1 = New System.Windows.Forms.Timer(Me.components)
        Me.GroupBox4 = New System.Windows.Forms.GroupBox
        Me.Button1 = New System.Windows.Forms.Button
        Me.SyncPositions = New System.Windows.Forms.Button
        Me.Label23 = New System.Windows.Forms.Label
        Me.Label21 = New System.Windows.Forms.Label
        Me.txtFeedRate = New System.Windows.Forms.TextBox
        Me.btnClearCoordLog = New System.Windows.Forms.Button
        Me.lblCoordStatus = New System.Windows.Forms.TextBox
        Me.GroupBox8 = New System.Windows.Forms.GroupBox
        Me.Label20 = New System.Windows.Forms.Label
        Me.txtArcCX = New System.Windows.Forms.TextBox
        Me.txtArcCY = New System.Windows.Forms.TextBox
        Me.Label22 = New System.Windows.Forms.Label
        Me.btnExecuteArc = New System.Windows.Forms.Button
        Me.Label17 = New System.Windows.Forms.Label
        Me.Label18 = New System.Windows.Forms.Label
        Me.txtArcEX = New System.Windows.Forms.TextBox
        Me.txtArcEZ = New System.Windows.Forms.TextBox
        Me.txtArcEY = New System.Windows.Forms.TextBox
        Me.Label19 = New System.Windows.Forms.Label
        Me.GroupBox7 = New System.Windows.Forms.GroupBox
        Me.btnExecuteStraight = New System.Windows.Forms.Button
        Me.Label14 = New System.Windows.Forms.Label
        Me.Label15 = New System.Windows.Forms.Label
        Me.txtStraightX = New System.Windows.Forms.TextBox
        Me.txtStraightZ = New System.Windows.Forms.TextBox
        Me.txtStraightY = New System.Windows.Forms.TextBox
        Me.Label16 = New System.Windows.Forms.Label
        Me.GroupBox6 = New System.Windows.Forms.GroupBox
        Me.btnExecuteTraverse = New System.Windows.Forms.Button
        Me.Label6 = New System.Windows.Forms.Label
        Me.Label13 = New System.Windows.Forms.Label
        Me.txtTraverseX = New System.Windows.Forms.TextBox
        Me.txtTraverseZ = New System.Windows.Forms.TextBox
        Me.txtTraverseY = New System.Windows.Forms.TextBox
        Me.Label8 = New System.Windows.Forms.Label
        Me.GroupBox5 = New System.Windows.Forms.GroupBox
        Me.Panel1 = New System.Windows.Forms.Panel
        Me.lblInterpreterFeedBack = New System.Windows.Forms.TextBox
        Me.btnRunInterpreter = New System.Windows.Forms.Button
        Me.chkSimInterpreter = New System.Windows.Forms.CheckBox
        Me.btnPauseInterpreter = New System.Windows.Forms.Button
        Me.btnHaltInterpreter = New System.Windows.Forms.Button
        Me.btnGetInterpreterFileName = New System.Windows.Forms.Button
        Me.txtInterpreterFileName = New System.Windows.Forms.TextBox
        Me.Label5 = New System.Windows.Forms.Label
        Me.grpAxisPositions.SuspendLayout()
        Me.GroupBox1.SuspendLayout()
        CType(Me.sldJogZ, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.sldJogY, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.sldJogX, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox2.SuspendLayout()
        Me.GroupBox3.SuspendLayout()
        Me.GroupBox4.SuspendLayout()
        Me.GroupBox8.SuspendLayout()
        Me.GroupBox7.SuspendLayout()
        Me.GroupBox6.SuspendLayout()
        Me.GroupBox5.SuspendLayout()
        Me.Panel1.SuspendLayout()
        Me.SuspendLayout()
        '
        'txtXAxisCommanded
        '
        Me.txtXAxisCommanded.Location = New System.Drawing.Point(68, 67)
        Me.txtXAxisCommanded.Name = "txtXAxisCommanded"
        Me.txtXAxisCommanded.Size = New System.Drawing.Size(100, 20)
        Me.txtXAxisCommanded.TabIndex = 0
        Me.txtXAxisCommanded.Text = "0.0"
        Me.txtXAxisCommanded.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(68, 51)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(14, 13)
        Me.Label1.TabIndex = 1
        Me.Label1.Text = "X"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(68, 90)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(14, 13)
        Me.Label2.TabIndex = 3
        Me.Label2.Text = "Y"
        '
        'txtYAxisCommanded
        '
        Me.txtYAxisCommanded.Location = New System.Drawing.Point(68, 106)
        Me.txtYAxisCommanded.Name = "txtYAxisCommanded"
        Me.txtYAxisCommanded.Size = New System.Drawing.Size(100, 20)
        Me.txtYAxisCommanded.TabIndex = 2
        Me.txtYAxisCommanded.Text = "0.0"
        Me.txtYAxisCommanded.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Location = New System.Drawing.Point(68, 129)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(14, 13)
        Me.Label3.TabIndex = 5
        Me.Label3.Text = "Z"
        '
        'txtZAxisCommanded
        '
        Me.txtZAxisCommanded.Location = New System.Drawing.Point(68, 145)
        Me.txtZAxisCommanded.Name = "txtZAxisCommanded"
        Me.txtZAxisCommanded.Size = New System.Drawing.Size(100, 20)
        Me.txtZAxisCommanded.TabIndex = 4
        Me.txtZAxisCommanded.Text = "0.0"
        Me.txtZAxisCommanded.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'grpAxisPositions
        '
        Me.grpAxisPositions.BackColor = System.Drawing.Color.White
        Me.grpAxisPositions.Controls.Add(Me.chkEnableZ)
        Me.grpAxisPositions.Controls.Add(Me.btnStopAll)
        Me.grpAxisPositions.Controls.Add(Me.btnMoveZ)
        Me.grpAxisPositions.Controls.Add(Me.chkEnableY)
        Me.grpAxisPositions.Controls.Add(Me.btnMoveY)
        Me.grpAxisPositions.Controls.Add(Me.chkEnableX)
        Me.grpAxisPositions.Controls.Add(Me.btnMoveX)
        Me.grpAxisPositions.Controls.Add(Me.Label1)
        Me.grpAxisPositions.Controls.Add(Me.Label3)
        Me.grpAxisPositions.Controls.Add(Me.txtXAxisCommanded)
        Me.grpAxisPositions.Controls.Add(Me.txtZAxisCommanded)
        Me.grpAxisPositions.Controls.Add(Me.txtYAxisCommanded)
        Me.grpAxisPositions.Controls.Add(Me.Label2)
        Me.grpAxisPositions.Location = New System.Drawing.Point(12, 286)
        Me.grpAxisPositions.Name = "grpAxisPositions"
        Me.grpAxisPositions.Size = New System.Drawing.Size(263, 185)
        Me.grpAxisPositions.TabIndex = 6
        Me.grpAxisPositions.TabStop = False
        Me.grpAxisPositions.Text = "Manual Axis Positoning"
        '
        'chkEnableZ
        '
        Me.chkEnableZ.AutoSize = True
        Me.chkEnableZ.Location = New System.Drawing.Point(6, 146)
        Me.chkEnableZ.Name = "chkEnableZ"
        Me.chkEnableZ.Size = New System.Drawing.Size(59, 17)
        Me.chkEnableZ.TabIndex = 12
        Me.chkEnableZ.Text = "Enable"
        Me.chkEnableZ.UseVisualStyleBackColor = True
        '
        'btnStopAll
        '
        Me.btnStopAll.BackColor = System.Drawing.Color.Gray
        Me.btnStopAll.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnStopAll.ForeColor = System.Drawing.Color.Black
        Me.btnStopAll.Location = New System.Drawing.Point(6, 19)
        Me.btnStopAll.Name = "btnStopAll"
        Me.btnStopAll.Size = New System.Drawing.Size(246, 27)
        Me.btnStopAll.TabIndex = 9
        Me.btnStopAll.Text = "Stop"
        Me.btnStopAll.UseVisualStyleBackColor = False
        '
        'btnMoveZ
        '
        Me.btnMoveZ.BackColor = System.Drawing.Color.Gainsboro
        Me.btnMoveZ.Location = New System.Drawing.Point(177, 145)
        Me.btnMoveZ.Name = "btnMoveZ"
        Me.btnMoveZ.Size = New System.Drawing.Size(75, 23)
        Me.btnMoveZ.TabIndex = 8
        Me.btnMoveZ.Text = "Move"
        Me.btnMoveZ.UseVisualStyleBackColor = False
        '
        'chkEnableY
        '
        Me.chkEnableY.AutoSize = True
        Me.chkEnableY.Location = New System.Drawing.Point(6, 107)
        Me.chkEnableY.Name = "chkEnableY"
        Me.chkEnableY.Size = New System.Drawing.Size(59, 17)
        Me.chkEnableY.TabIndex = 11
        Me.chkEnableY.Text = "Enable"
        Me.chkEnableY.UseVisualStyleBackColor = True
        '
        'btnMoveY
        '
        Me.btnMoveY.BackColor = System.Drawing.Color.Gainsboro
        Me.btnMoveY.Location = New System.Drawing.Point(177, 106)
        Me.btnMoveY.Name = "btnMoveY"
        Me.btnMoveY.Size = New System.Drawing.Size(75, 23)
        Me.btnMoveY.TabIndex = 7
        Me.btnMoveY.Text = "Move"
        Me.btnMoveY.UseVisualStyleBackColor = False
        '
        'chkEnableX
        '
        Me.chkEnableX.AutoSize = True
        Me.chkEnableX.Location = New System.Drawing.Point(6, 67)
        Me.chkEnableX.Name = "chkEnableX"
        Me.chkEnableX.Size = New System.Drawing.Size(59, 17)
        Me.chkEnableX.TabIndex = 10
        Me.chkEnableX.Text = "Enable"
        Me.chkEnableX.UseVisualStyleBackColor = True
        '
        'btnMoveX
        '
        Me.btnMoveX.BackColor = System.Drawing.Color.Gainsboro
        Me.btnMoveX.Location = New System.Drawing.Point(177, 67)
        Me.btnMoveX.Name = "btnMoveX"
        Me.btnMoveX.Size = New System.Drawing.Size(75, 23)
        Me.btnMoveX.TabIndex = 6
        Me.btnMoveX.Text = "Move"
        Me.btnMoveX.UseVisualStyleBackColor = False
        '
        'GroupBox1
        '
        Me.GroupBox1.BackColor = System.Drawing.Color.White
        Me.GroupBox1.Controls.Add(Me.lblZJogPercent)
        Me.GroupBox1.Controls.Add(Me.Label9)
        Me.GroupBox1.Controls.Add(Me.sldJogZ)
        Me.GroupBox1.Controls.Add(Me.lblYJogPercent)
        Me.GroupBox1.Controls.Add(Me.Label7)
        Me.GroupBox1.Controls.Add(Me.sldJogY)
        Me.GroupBox1.Controls.Add(Me.lblXJogPercent)
        Me.GroupBox1.Controls.Add(Me.Label4)
        Me.GroupBox1.Controls.Add(Me.sldJogX)
        Me.GroupBox1.Location = New System.Drawing.Point(281, 12)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(312, 287)
        Me.GroupBox1.TabIndex = 7
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "Jogging"
        '
        'lblZJogPercent
        '
        Me.lblZJogPercent.AutoSize = True
        Me.lblZJogPercent.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblZJogPercent.Location = New System.Drawing.Point(247, 187)
        Me.lblZJogPercent.Name = "lblZJogPercent"
        Me.lblZJogPercent.Size = New System.Drawing.Size(19, 20)
        Me.lblZJogPercent.TabIndex = 16
        Me.lblZJogPercent.Text = "Z"
        Me.lblZJogPercent.TextAlign = System.Drawing.ContentAlignment.TopRight
        '
        'Label9
        '
        Me.Label9.AutoSize = True
        Me.Label9.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label9.Location = New System.Drawing.Point(143, 187)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(22, 24)
        Me.Label9.TabIndex = 15
        Me.Label9.Text = "Z"
        '
        'sldJogZ
        '
        Me.sldJogZ.Location = New System.Drawing.Point(2, 214)
        Me.sldJogZ.Maximum = 100
        Me.sldJogZ.Minimum = -100
        Me.sldJogZ.Name = "sldJogZ"
        Me.sldJogZ.Size = New System.Drawing.Size(306, 45)
        Me.sldJogZ.TabIndex = 14
        Me.sldJogZ.TickFrequency = 10
        '
        'lblYJogPercent
        '
        Me.lblYJogPercent.AutoSize = True
        Me.lblYJogPercent.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblYJogPercent.Location = New System.Drawing.Point(247, 112)
        Me.lblYJogPercent.Name = "lblYJogPercent"
        Me.lblYJogPercent.Size = New System.Drawing.Size(20, 20)
        Me.lblYJogPercent.TabIndex = 13
        Me.lblYJogPercent.Text = "Y"
        Me.lblYJogPercent.TextAlign = System.Drawing.ContentAlignment.TopRight
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label7.Location = New System.Drawing.Point(143, 112)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(22, 24)
        Me.Label7.TabIndex = 12
        Me.Label7.Text = "Y"
        '
        'sldJogY
        '
        Me.sldJogY.Location = New System.Drawing.Point(2, 139)
        Me.sldJogY.Maximum = 100
        Me.sldJogY.Minimum = -100
        Me.sldJogY.Name = "sldJogY"
        Me.sldJogY.Size = New System.Drawing.Size(306, 45)
        Me.sldJogY.TabIndex = 11
        Me.sldJogY.TickFrequency = 10
        '
        'lblXJogPercent
        '
        Me.lblXJogPercent.AutoSize = True
        Me.lblXJogPercent.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblXJogPercent.Location = New System.Drawing.Point(247, 37)
        Me.lblXJogPercent.Name = "lblXJogPercent"
        Me.lblXJogPercent.Size = New System.Drawing.Size(20, 20)
        Me.lblXJogPercent.TabIndex = 10
        Me.lblXJogPercent.Text = "X"
        Me.lblXJogPercent.TextAlign = System.Drawing.ContentAlignment.TopRight
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label4.Location = New System.Drawing.Point(143, 37)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(24, 24)
        Me.Label4.TabIndex = 9
        Me.Label4.Text = "X"
        '
        'sldJogX
        '
        Me.sldJogX.Location = New System.Drawing.Point(2, 64)
        Me.sldJogX.Maximum = 100
        Me.sldJogX.Minimum = -100
        Me.sldJogX.Name = "sldJogX"
        Me.sldJogX.Size = New System.Drawing.Size(306, 45)
        Me.sldJogX.TabIndex = 8
        Me.sldJogX.TickFrequency = 10
        '
        'GroupBox2
        '
        Me.GroupBox2.BackColor = System.Drawing.Color.White
        Me.GroupBox2.Controls.Add(Me.Label10)
        Me.GroupBox2.Controls.Add(Me.Label11)
        Me.GroupBox2.Controls.Add(Me.txtXAxisPos)
        Me.GroupBox2.Controls.Add(Me.txtZAxisPos)
        Me.GroupBox2.Controls.Add(Me.txtYAxisPos)
        Me.GroupBox2.Controls.Add(Me.Label12)
        Me.GroupBox2.Location = New System.Drawing.Point(12, 12)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(263, 268)
        Me.GroupBox2.TabIndex = 8
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "DRO"
        '
        'Label10
        '
        Me.Label10.AutoSize = True
        Me.Label10.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label10.Location = New System.Drawing.Point(14, 51)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(25, 24)
        Me.Label10.TabIndex = 7
        Me.Label10.Text = "X"
        '
        'Label11
        '
        Me.Label11.AutoSize = True
        Me.Label11.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label11.Location = New System.Drawing.Point(14, 187)
        Me.Label11.Name = "Label11"
        Me.Label11.Size = New System.Drawing.Size(23, 24)
        Me.Label11.TabIndex = 11
        Me.Label11.Text = "Z"
        '
        'txtXAxisPos
        '
        Me.txtXAxisPos.BackColor = System.Drawing.Color.Azure
        Me.txtXAxisPos.Enabled = False
        Me.txtXAxisPos.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtXAxisPos.Location = New System.Drawing.Point(74, 48)
        Me.txtXAxisPos.Name = "txtXAxisPos"
        Me.txtXAxisPos.Size = New System.Drawing.Size(150, 29)
        Me.txtXAxisPos.TabIndex = 6
        Me.txtXAxisPos.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtZAxisPos
        '
        Me.txtZAxisPos.BackColor = System.Drawing.Color.Azure
        Me.txtZAxisPos.Enabled = False
        Me.txtZAxisPos.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtZAxisPos.Location = New System.Drawing.Point(74, 184)
        Me.txtZAxisPos.Name = "txtZAxisPos"
        Me.txtZAxisPos.Size = New System.Drawing.Size(150, 29)
        Me.txtZAxisPos.TabIndex = 10
        Me.txtZAxisPos.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtYAxisPos
        '
        Me.txtYAxisPos.BackColor = System.Drawing.Color.Azure
        Me.txtYAxisPos.Enabled = False
        Me.txtYAxisPos.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtYAxisPos.Location = New System.Drawing.Point(74, 118)
        Me.txtYAxisPos.Name = "txtYAxisPos"
        Me.txtYAxisPos.Size = New System.Drawing.Size(150, 29)
        Me.txtYAxisPos.TabIndex = 8
        Me.txtYAxisPos.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label12
        '
        Me.Label12.AutoSize = True
        Me.Label12.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label12.Location = New System.Drawing.Point(14, 121)
        Me.Label12.Name = "Label12"
        Me.Label12.Size = New System.Drawing.Size(23, 24)
        Me.Label12.TabIndex = 9
        Me.Label12.Text = "Y"
        '
        'GroupBox3
        '
        Me.GroupBox3.BackColor = System.Drawing.Color.White
        Me.GroupBox3.Controls.Add(Me.chkToggleLED_1)
        Me.GroupBox3.Controls.Add(Me.chkToggleLED_0)
        Me.GroupBox3.Location = New System.Drawing.Point(283, 305)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.Size = New System.Drawing.Size(310, 166)
        Me.GroupBox3.TabIndex = 9
        Me.GroupBox3.TabStop = False
        Me.GroupBox3.Text = "IO"
        '
        'chkToggleLED_1
        '
        Me.chkToggleLED_1.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkToggleLED_1.BackColor = System.Drawing.Color.DarkGray
        Me.chkToggleLED_1.Font = New System.Drawing.Font("Microsoft Sans Serif", 18.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.chkToggleLED_1.Location = New System.Drawing.Point(25, 90)
        Me.chkToggleLED_1.Name = "chkToggleLED_1"
        Me.chkToggleLED_1.Size = New System.Drawing.Size(256, 60)
        Me.chkToggleLED_1.TabIndex = 12
        Me.chkToggleLED_1.Text = "LED 1 (io # 47)"
        Me.chkToggleLED_1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkToggleLED_1.UseVisualStyleBackColor = False
        '
        'chkToggleLED_0
        '
        Me.chkToggleLED_0.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkToggleLED_0.BackColor = System.Drawing.Color.DarkGray
        Me.chkToggleLED_0.Font = New System.Drawing.Font("Microsoft Sans Serif", 18.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.chkToggleLED_0.Location = New System.Drawing.Point(25, 24)
        Me.chkToggleLED_0.Name = "chkToggleLED_0"
        Me.chkToggleLED_0.Size = New System.Drawing.Size(256, 60)
        Me.chkToggleLED_0.TabIndex = 11
        Me.chkToggleLED_0.Text = "LED 0 (io # 46)"
        Me.chkToggleLED_0.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkToggleLED_0.UseVisualStyleBackColor = False
        '
        'Timer1
        '
        '
        'GroupBox4
        '
        Me.GroupBox4.BackColor = System.Drawing.Color.White
        Me.GroupBox4.Controls.Add(Me.Button1)
        Me.GroupBox4.Controls.Add(Me.SyncPositions)
        Me.GroupBox4.Controls.Add(Me.Label23)
        Me.GroupBox4.Controls.Add(Me.Label21)
        Me.GroupBox4.Controls.Add(Me.txtFeedRate)
        Me.GroupBox4.Controls.Add(Me.btnClearCoordLog)
        Me.GroupBox4.Controls.Add(Me.lblCoordStatus)
        Me.GroupBox4.Controls.Add(Me.GroupBox8)
        Me.GroupBox4.Controls.Add(Me.GroupBox7)
        Me.GroupBox4.Controls.Add(Me.GroupBox6)
        Me.GroupBox4.Location = New System.Drawing.Point(12, 477)
        Me.GroupBox4.Name = "GroupBox4"
        Me.GroupBox4.Size = New System.Drawing.Size(985, 235)
        Me.GroupBox4.TabIndex = 10
        Me.GroupBox4.TabStop = False
        Me.GroupBox4.Text = "CoordMotion"
        '
        'Button1
        '
        Me.Button1.Location = New System.Drawing.Point(440, 138)
        Me.Button1.Name = "Button1"
        Me.Button1.Size = New System.Drawing.Size(128, 27)
        Me.Button1.TabIndex = 19
        Me.Button1.Text = "Execute Coord Motion"
        Me.Button1.UseVisualStyleBackColor = True
        '
        'SyncPositions
        '
        Me.SyncPositions.Location = New System.Drawing.Point(440, 102)
        Me.SyncPositions.Name = "SyncPositions"
        Me.SyncPositions.Size = New System.Drawing.Size(129, 26)
        Me.SyncPositions.TabIndex = 18
        Me.SyncPositions.Text = "Sync Start Positions"
        Me.SyncPositions.UseVisualStyleBackColor = True
        '
        'Label23
        '
        Me.Label23.AutoSize = True
        Me.Label23.Location = New System.Drawing.Point(234, 99)
        Me.Label23.Name = "Label23"
        Me.Label23.Size = New System.Drawing.Size(20, 13)
        Me.Label23.TabIndex = 17
        Me.Label23.Text = "ips"
        '
        'Label21
        '
        Me.Label21.AutoSize = True
        Me.Label21.Location = New System.Drawing.Point(103, 99)
        Me.Label21.Name = "Label21"
        Me.Label21.Size = New System.Drawing.Size(65, 13)
        Me.Label21.TabIndex = 17
        Me.Label21.Text = "Feed Speed"
        '
        'txtFeedRate
        '
        Me.txtFeedRate.Location = New System.Drawing.Point(173, 96)
        Me.txtFeedRate.Name = "txtFeedRate"
        Me.txtFeedRate.Size = New System.Drawing.Size(55, 20)
        Me.txtFeedRate.TabIndex = 16
        Me.txtFeedRate.Text = "2"
        Me.txtFeedRate.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'btnClearCoordLog
        '
        Me.btnClearCoordLog.BackColor = System.Drawing.Color.Gainsboro
        Me.btnClearCoordLog.Location = New System.Drawing.Point(907, 193)
        Me.btnClearCoordLog.Name = "btnClearCoordLog"
        Me.btnClearCoordLog.Size = New System.Drawing.Size(66, 23)
        Me.btnClearCoordLog.TabIndex = 14
        Me.btnClearCoordLog.Text = "Clear Log"
        Me.btnClearCoordLog.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        Me.btnClearCoordLog.UseVisualStyleBackColor = False
        '
        'lblCoordStatus
        '
        Me.lblCoordStatus.BackColor = System.Drawing.Color.Azure
        Me.lblCoordStatus.Location = New System.Drawing.Point(596, 12)
        Me.lblCoordStatus.Multiline = True
        Me.lblCoordStatus.Name = "lblCoordStatus"
        Me.lblCoordStatus.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.lblCoordStatus.Size = New System.Drawing.Size(377, 154)
        Me.lblCoordStatus.TabIndex = 11
        Me.lblCoordStatus.WordWrap = False
        '
        'GroupBox8
        '
        Me.GroupBox8.Controls.Add(Me.Label20)
        Me.GroupBox8.Controls.Add(Me.txtArcCX)
        Me.GroupBox8.Controls.Add(Me.txtArcCY)
        Me.GroupBox8.Controls.Add(Me.Label22)
        Me.GroupBox8.Controls.Add(Me.btnExecuteArc)
        Me.GroupBox8.Controls.Add(Me.Label17)
        Me.GroupBox8.Controls.Add(Me.Label18)
        Me.GroupBox8.Controls.Add(Me.txtArcEX)
        Me.GroupBox8.Controls.Add(Me.txtArcEZ)
        Me.GroupBox8.Controls.Add(Me.txtArcEY)
        Me.GroupBox8.Controls.Add(Me.Label19)
        Me.GroupBox8.Location = New System.Drawing.Point(10, 171)
        Me.GroupBox8.Name = "GroupBox8"
        Me.GroupBox8.Size = New System.Drawing.Size(613, 54)
        Me.GroupBox8.TabIndex = 10
        Me.GroupBox8.TabStop = False
        Me.GroupBox8.Text = "Arc Feed"
        '
        'Label20
        '
        Me.Label20.AutoSize = True
        Me.Label20.Location = New System.Drawing.Point(303, 22)
        Me.Label20.Name = "Label20"
        Me.Label20.Size = New System.Drawing.Size(21, 13)
        Me.Label20.TabIndex = 15
        Me.Label20.Text = "CX"
        '
        'txtArcCX
        '
        Me.txtArcCX.Location = New System.Drawing.Point(330, 19)
        Me.txtArcCX.Name = "txtArcCX"
        Me.txtArcCX.Size = New System.Drawing.Size(55, 20)
        Me.txtArcCX.TabIndex = 14
        Me.txtArcCX.Text = "3"
        Me.txtArcCX.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtArcCY
        '
        Me.txtArcCY.Location = New System.Drawing.Point(430, 19)
        Me.txtArcCY.Name = "txtArcCY"
        Me.txtArcCY.Size = New System.Drawing.Size(55, 20)
        Me.txtArcCY.TabIndex = 16
        Me.txtArcCY.Text = "0"
        Me.txtArcCY.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label22
        '
        Me.Label22.AutoSize = True
        Me.Label22.Location = New System.Drawing.Point(403, 22)
        Me.Label22.Name = "Label22"
        Me.Label22.Size = New System.Drawing.Size(21, 13)
        Me.Label22.TabIndex = 17
        Me.Label22.Text = "CY"
        '
        'btnExecuteArc
        '
        Me.btnExecuteArc.BackColor = System.Drawing.Color.Gainsboro
        Me.btnExecuteArc.Location = New System.Drawing.Point(504, 17)
        Me.btnExecuteArc.Name = "btnExecuteArc"
        Me.btnExecuteArc.Size = New System.Drawing.Size(88, 23)
        Me.btnExecuteArc.TabIndex = 13
        Me.btnExecuteArc.Text = "Add to Buffer"
        Me.btnExecuteArc.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        Me.btnExecuteArc.UseVisualStyleBackColor = False
        '
        'Label17
        '
        Me.Label17.AutoSize = True
        Me.Label17.Location = New System.Drawing.Point(6, 22)
        Me.Label17.Name = "Label17"
        Me.Label17.Size = New System.Drawing.Size(21, 13)
        Me.Label17.TabIndex = 3
        Me.Label17.Text = "EX"
        '
        'Label18
        '
        Me.Label18.AutoSize = True
        Me.Label18.Location = New System.Drawing.Point(198, 22)
        Me.Label18.Name = "Label18"
        Me.Label18.Size = New System.Drawing.Size(21, 13)
        Me.Label18.TabIndex = 7
        Me.Label18.Text = "EZ"
        '
        'txtArcEX
        '
        Me.txtArcEX.Location = New System.Drawing.Point(33, 19)
        Me.txtArcEX.Name = "txtArcEX"
        Me.txtArcEX.Size = New System.Drawing.Size(55, 20)
        Me.txtArcEX.TabIndex = 2
        Me.txtArcEX.Text = "6"
        Me.txtArcEX.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtArcEZ
        '
        Me.txtArcEZ.Location = New System.Drawing.Point(226, 19)
        Me.txtArcEZ.Name = "txtArcEZ"
        Me.txtArcEZ.Size = New System.Drawing.Size(55, 20)
        Me.txtArcEZ.TabIndex = 6
        Me.txtArcEZ.Text = "0"
        Me.txtArcEZ.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtArcEY
        '
        Me.txtArcEY.Location = New System.Drawing.Point(131, 19)
        Me.txtArcEY.Name = "txtArcEY"
        Me.txtArcEY.Size = New System.Drawing.Size(55, 20)
        Me.txtArcEY.TabIndex = 4
        Me.txtArcEY.Text = "0"
        Me.txtArcEY.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label19
        '
        Me.Label19.AutoSize = True
        Me.Label19.Location = New System.Drawing.Point(104, 22)
        Me.Label19.Name = "Label19"
        Me.Label19.Size = New System.Drawing.Size(21, 13)
        Me.Label19.TabIndex = 5
        Me.Label19.Text = "EY"
        '
        'GroupBox7
        '
        Me.GroupBox7.Controls.Add(Me.btnExecuteStraight)
        Me.GroupBox7.Controls.Add(Me.Label14)
        Me.GroupBox7.Controls.Add(Me.Label15)
        Me.GroupBox7.Controls.Add(Me.txtStraightX)
        Me.GroupBox7.Controls.Add(Me.txtStraightZ)
        Me.GroupBox7.Controls.Add(Me.txtStraightY)
        Me.GroupBox7.Controls.Add(Me.Label16)
        Me.GroupBox7.Location = New System.Drawing.Point(10, 121)
        Me.GroupBox7.Name = "GroupBox7"
        Me.GroupBox7.Size = New System.Drawing.Size(405, 45)
        Me.GroupBox7.TabIndex = 9
        Me.GroupBox7.TabStop = False
        Me.GroupBox7.Text = "Straight Feed"
        '
        'btnExecuteStraight
        '
        Me.btnExecuteStraight.BackColor = System.Drawing.Color.Gainsboro
        Me.btnExecuteStraight.Location = New System.Drawing.Point(306, 16)
        Me.btnExecuteStraight.Name = "btnExecuteStraight"
        Me.btnExecuteStraight.Size = New System.Drawing.Size(86, 23)
        Me.btnExecuteStraight.TabIndex = 13
        Me.btnExecuteStraight.Text = "Add to Buffer"
        Me.btnExecuteStraight.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        Me.btnExecuteStraight.UseVisualStyleBackColor = False
        '
        'Label14
        '
        Me.Label14.AutoSize = True
        Me.Label14.Location = New System.Drawing.Point(6, 22)
        Me.Label14.Name = "Label14"
        Me.Label14.Size = New System.Drawing.Size(14, 13)
        Me.Label14.TabIndex = 3
        Me.Label14.Text = "X"
        '
        'Label15
        '
        Me.Label15.AutoSize = True
        Me.Label15.Location = New System.Drawing.Point(206, 22)
        Me.Label15.Name = "Label15"
        Me.Label15.Size = New System.Drawing.Size(14, 13)
        Me.Label15.TabIndex = 7
        Me.Label15.Text = "Z"
        '
        'txtStraightX
        '
        Me.txtStraightX.Location = New System.Drawing.Point(26, 19)
        Me.txtStraightX.Name = "txtStraightX"
        Me.txtStraightX.Size = New System.Drawing.Size(55, 20)
        Me.txtStraightX.TabIndex = 2
        Me.txtStraightX.Text = "1"
        Me.txtStraightX.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtStraightZ
        '
        Me.txtStraightZ.Location = New System.Drawing.Point(226, 19)
        Me.txtStraightZ.Name = "txtStraightZ"
        Me.txtStraightZ.Size = New System.Drawing.Size(55, 20)
        Me.txtStraightZ.TabIndex = 6
        Me.txtStraightZ.Text = "3"
        Me.txtStraightZ.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtStraightY
        '
        Me.txtStraightY.Location = New System.Drawing.Point(125, 19)
        Me.txtStraightY.Name = "txtStraightY"
        Me.txtStraightY.Size = New System.Drawing.Size(55, 20)
        Me.txtStraightY.TabIndex = 4
        Me.txtStraightY.Text = "2"
        Me.txtStraightY.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label16
        '
        Me.Label16.AutoSize = True
        Me.Label16.Location = New System.Drawing.Point(105, 22)
        Me.Label16.Name = "Label16"
        Me.Label16.Size = New System.Drawing.Size(14, 13)
        Me.Label16.TabIndex = 5
        Me.Label16.Text = "Y"
        '
        'GroupBox6
        '
        Me.GroupBox6.Controls.Add(Me.btnExecuteTraverse)
        Me.GroupBox6.Controls.Add(Me.Label6)
        Me.GroupBox6.Controls.Add(Me.Label13)
        Me.GroupBox6.Controls.Add(Me.txtTraverseX)
        Me.GroupBox6.Controls.Add(Me.txtTraverseZ)
        Me.GroupBox6.Controls.Add(Me.txtTraverseY)
        Me.GroupBox6.Controls.Add(Me.Label8)
        Me.GroupBox6.Location = New System.Drawing.Point(9, 19)
        Me.GroupBox6.Name = "GroupBox6"
        Me.GroupBox6.Size = New System.Drawing.Size(406, 45)
        Me.GroupBox6.TabIndex = 8
        Me.GroupBox6.TabStop = False
        Me.GroupBox6.Text = "Straight Traverse"
        '
        'btnExecuteTraverse
        '
        Me.btnExecuteTraverse.BackColor = System.Drawing.Color.Gainsboro
        Me.btnExecuteTraverse.Location = New System.Drawing.Point(307, 15)
        Me.btnExecuteTraverse.Name = "btnExecuteTraverse"
        Me.btnExecuteTraverse.Size = New System.Drawing.Size(85, 23)
        Me.btnExecuteTraverse.TabIndex = 13
        Me.btnExecuteTraverse.Text = "Execute Now"
        Me.btnExecuteTraverse.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        Me.btnExecuteTraverse.UseVisualStyleBackColor = False
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Location = New System.Drawing.Point(6, 21)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(14, 13)
        Me.Label6.TabIndex = 3
        Me.Label6.Text = "X"
        '
        'Label13
        '
        Me.Label13.AutoSize = True
        Me.Label13.Location = New System.Drawing.Point(206, 21)
        Me.Label13.Name = "Label13"
        Me.Label13.Size = New System.Drawing.Size(14, 13)
        Me.Label13.TabIndex = 7
        Me.Label13.Text = "Z"
        '
        'txtTraverseX
        '
        Me.txtTraverseX.Location = New System.Drawing.Point(26, 18)
        Me.txtTraverseX.Name = "txtTraverseX"
        Me.txtTraverseX.Size = New System.Drawing.Size(55, 20)
        Me.txtTraverseX.TabIndex = 2
        Me.txtTraverseX.Text = "0"
        Me.txtTraverseX.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtTraverseZ
        '
        Me.txtTraverseZ.Location = New System.Drawing.Point(226, 18)
        Me.txtTraverseZ.Name = "txtTraverseZ"
        Me.txtTraverseZ.Size = New System.Drawing.Size(55, 20)
        Me.txtTraverseZ.TabIndex = 6
        Me.txtTraverseZ.Text = "0"
        Me.txtTraverseZ.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtTraverseY
        '
        Me.txtTraverseY.Location = New System.Drawing.Point(125, 18)
        Me.txtTraverseY.Name = "txtTraverseY"
        Me.txtTraverseY.Size = New System.Drawing.Size(55, 20)
        Me.txtTraverseY.TabIndex = 4
        Me.txtTraverseY.Text = "0"
        Me.txtTraverseY.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Location = New System.Drawing.Point(105, 21)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(14, 13)
        Me.Label8.TabIndex = 5
        Me.Label8.Text = "Y"
        '
        'GroupBox5
        '
        Me.GroupBox5.BackColor = System.Drawing.Color.White
        Me.GroupBox5.Controls.Add(Me.Panel1)
        Me.GroupBox5.Controls.Add(Me.btnRunInterpreter)
        Me.GroupBox5.Controls.Add(Me.chkSimInterpreter)
        Me.GroupBox5.Controls.Add(Me.btnPauseInterpreter)
        Me.GroupBox5.Controls.Add(Me.btnHaltInterpreter)
        Me.GroupBox5.Controls.Add(Me.btnGetInterpreterFileName)
        Me.GroupBox5.Controls.Add(Me.txtInterpreterFileName)
        Me.GroupBox5.Controls.Add(Me.Label5)
        Me.GroupBox5.Location = New System.Drawing.Point(599, 12)
        Me.GroupBox5.Name = "GroupBox5"
        Me.GroupBox5.Size = New System.Drawing.Size(398, 459)
        Me.GroupBox5.TabIndex = 11
        Me.GroupBox5.TabStop = False
        Me.GroupBox5.Text = "Interpreter"
        '
        'Panel1
        '
        Me.Panel1.AutoScroll = True
        Me.Panel1.BackColor = System.Drawing.Color.Azure
        Me.Panel1.Controls.Add(Me.lblInterpreterFeedBack)
        Me.Panel1.Location = New System.Drawing.Point(9, 84)
        Me.Panel1.Name = "Panel1"
        Me.Panel1.Size = New System.Drawing.Size(377, 369)
        Me.Panel1.TabIndex = 13
        '
        'lblInterpreterFeedBack
        '
        Me.lblInterpreterFeedBack.BackColor = System.Drawing.Color.Azure
        Me.lblInterpreterFeedBack.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lblInterpreterFeedBack.Location = New System.Drawing.Point(0, 0)
        Me.lblInterpreterFeedBack.Multiline = True
        Me.lblInterpreterFeedBack.Name = "lblInterpreterFeedBack"
        Me.lblInterpreterFeedBack.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.lblInterpreterFeedBack.Size = New System.Drawing.Size(377, 369)
        Me.lblInterpreterFeedBack.TabIndex = 0
        Me.lblInterpreterFeedBack.WordWrap = False
        '
        'btnRunInterpreter
        '
        Me.btnRunInterpreter.BackColor = System.Drawing.Color.Gainsboro
        Me.btnRunInterpreter.Location = New System.Drawing.Point(213, 17)
        Me.btnRunInterpreter.Name = "btnRunInterpreter"
        Me.btnRunInterpreter.Size = New System.Drawing.Size(51, 23)
        Me.btnRunInterpreter.TabIndex = 12
        Me.btnRunInterpreter.Text = "Run >"
        Me.btnRunInterpreter.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        Me.btnRunInterpreter.UseVisualStyleBackColor = False
        '
        'chkSimInterpreter
        '
        Me.chkSimInterpreter.AutoSize = True
        Me.chkSimInterpreter.Location = New System.Drawing.Point(125, 21)
        Me.chkSimInterpreter.Name = "chkSimInterpreter"
        Me.chkSimInterpreter.Size = New System.Drawing.Size(66, 17)
        Me.chkSimInterpreter.TabIndex = 11
        Me.chkSimInterpreter.Text = "Simulate"
        Me.chkSimInterpreter.UseVisualStyleBackColor = True
        '
        'btnPauseInterpreter
        '
        Me.btnPauseInterpreter.BackColor = System.Drawing.Color.Gainsboro
        Me.btnPauseInterpreter.Location = New System.Drawing.Point(270, 17)
        Me.btnPauseInterpreter.Name = "btnPauseInterpreter"
        Me.btnPauseInterpreter.Size = New System.Drawing.Size(60, 23)
        Me.btnPauseInterpreter.TabIndex = 10
        Me.btnPauseInterpreter.Text = "Pause | |"
        Me.btnPauseInterpreter.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        Me.btnPauseInterpreter.UseVisualStyleBackColor = False
        '
        'btnHaltInterpreter
        '
        Me.btnHaltInterpreter.BackColor = System.Drawing.Color.Gainsboro
        Me.btnHaltInterpreter.Location = New System.Drawing.Point(336, 17)
        Me.btnHaltInterpreter.Name = "btnHaltInterpreter"
        Me.btnHaltInterpreter.Size = New System.Drawing.Size(50, 23)
        Me.btnHaltInterpreter.TabIndex = 9
        Me.btnHaltInterpreter.Text = "Halt [ ]"
        Me.btnHaltInterpreter.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        Me.btnHaltInterpreter.UseVisualStyleBackColor = False
        '
        'btnGetInterpreterFileName
        '
        Me.btnGetInterpreterFileName.BackColor = System.Drawing.Color.Gainsboro
        Me.btnGetInterpreterFileName.Location = New System.Drawing.Point(354, 54)
        Me.btnGetInterpreterFileName.Name = "btnGetInterpreterFileName"
        Me.btnGetInterpreterFileName.Size = New System.Drawing.Size(32, 23)
        Me.btnGetInterpreterFileName.TabIndex = 7
        Me.btnGetInterpreterFileName.Text = ". . ."
        Me.btnGetInterpreterFileName.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        Me.btnGetInterpreterFileName.UseVisualStyleBackColor = False
        '
        'txtInterpreterFileName
        '
        Me.txtInterpreterFileName.Location = New System.Drawing.Point(9, 55)
        Me.txtInterpreterFileName.Name = "txtInterpreterFileName"
        Me.txtInterpreterFileName.Size = New System.Drawing.Size(338, 20)
        Me.txtInterpreterFileName.TabIndex = 1
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Location = New System.Drawing.Point(6, 36)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(63, 13)
        Me.Label5.TabIndex = 0
        Me.Label5.Text = "File Name ::"
        '
        'Form1
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.BackColor = System.Drawing.Color.WhiteSmoke
        Me.ClientSize = New System.Drawing.Size(1008, 718)
        Me.Controls.Add(Me.GroupBox5)
        Me.Controls.Add(Me.GroupBox4)
        Me.Controls.Add(Me.GroupBox3)
        Me.Controls.Add(Me.GroupBox2)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.grpAxisPositions)
        Me.Name = "Form1"
        Me.Text = "DynoMotion VB.net"
        Me.grpAxisPositions.ResumeLayout(False)
        Me.grpAxisPositions.PerformLayout()
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        CType(Me.sldJogZ, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.sldJogY, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.sldJogX, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox2.PerformLayout()
        Me.GroupBox3.ResumeLayout(False)
        Me.GroupBox4.ResumeLayout(False)
        Me.GroupBox4.PerformLayout()
        Me.GroupBox8.ResumeLayout(False)
        Me.GroupBox8.PerformLayout()
        Me.GroupBox7.ResumeLayout(False)
        Me.GroupBox7.PerformLayout()
        Me.GroupBox6.ResumeLayout(False)
        Me.GroupBox6.PerformLayout()
        Me.GroupBox5.ResumeLayout(False)
        Me.GroupBox5.PerformLayout()
        Me.Panel1.ResumeLayout(False)
        Me.Panel1.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents txtXAxisCommanded As System.Windows.Forms.TextBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents txtYAxisCommanded As System.Windows.Forms.TextBox
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents txtZAxisCommanded As System.Windows.Forms.TextBox
    Friend WithEvents grpAxisPositions As System.Windows.Forms.GroupBox
    Friend WithEvents chkEnableZ As System.Windows.Forms.CheckBox
    Friend WithEvents btnStopAll As System.Windows.Forms.Button
    Friend WithEvents btnMoveZ As System.Windows.Forms.Button
    Friend WithEvents chkEnableY As System.Windows.Forms.CheckBox
    Friend WithEvents btnMoveY As System.Windows.Forms.Button
    Friend WithEvents chkEnableX As System.Windows.Forms.CheckBox
    Friend WithEvents btnMoveX As System.Windows.Forms.Button
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents sldJogX As System.Windows.Forms.TrackBar
    Friend WithEvents lblZJogPercent As System.Windows.Forms.Label
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents sldJogZ As System.Windows.Forms.TrackBar
    Friend WithEvents lblYJogPercent As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents sldJogY As System.Windows.Forms.TrackBar
    Friend WithEvents lblXJogPercent As System.Windows.Forms.Label
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents Label10 As System.Windows.Forms.Label
    Friend WithEvents Label11 As System.Windows.Forms.Label
    Friend WithEvents txtXAxisPos As System.Windows.Forms.TextBox
    Friend WithEvents txtZAxisPos As System.Windows.Forms.TextBox
    Friend WithEvents txtYAxisPos As System.Windows.Forms.TextBox
    Friend WithEvents Label12 As System.Windows.Forms.Label
    Friend WithEvents GroupBox3 As System.Windows.Forms.GroupBox
    Friend WithEvents chkToggleLED_1 As System.Windows.Forms.CheckBox
    Friend WithEvents chkToggleLED_0 As System.Windows.Forms.CheckBox
    Friend WithEvents Timer1 As System.Windows.Forms.Timer
    Friend WithEvents GroupBox4 As System.Windows.Forms.GroupBox
    Friend WithEvents GroupBox5 As System.Windows.Forms.GroupBox
    Friend WithEvents btnGetInterpreterFileName As System.Windows.Forms.Button
    Friend WithEvents txtInterpreterFileName As System.Windows.Forms.TextBox
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents btnRunInterpreter As System.Windows.Forms.Button
    Friend WithEvents chkSimInterpreter As System.Windows.Forms.CheckBox
    Friend WithEvents btnPauseInterpreter As System.Windows.Forms.Button
    Friend WithEvents btnHaltInterpreter As System.Windows.Forms.Button
    Friend WithEvents Panel1 As System.Windows.Forms.Panel
    Friend WithEvents lblInterpreterFeedBack As System.Windows.Forms.TextBox
    Friend WithEvents GroupBox7 As System.Windows.Forms.GroupBox
    Friend WithEvents btnExecuteStraight As System.Windows.Forms.Button
    Friend WithEvents Label14 As System.Windows.Forms.Label
    Friend WithEvents Label15 As System.Windows.Forms.Label
    Friend WithEvents txtStraightX As System.Windows.Forms.TextBox
    Friend WithEvents txtStraightZ As System.Windows.Forms.TextBox
    Friend WithEvents txtStraightY As System.Windows.Forms.TextBox
    Friend WithEvents Label16 As System.Windows.Forms.Label
    Friend WithEvents GroupBox6 As System.Windows.Forms.GroupBox
    Friend WithEvents btnExecuteTraverse As System.Windows.Forms.Button
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents Label13 As System.Windows.Forms.Label
    Friend WithEvents txtTraverseX As System.Windows.Forms.TextBox
    Friend WithEvents txtTraverseZ As System.Windows.Forms.TextBox
    Friend WithEvents txtTraverseY As System.Windows.Forms.TextBox
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents GroupBox8 As System.Windows.Forms.GroupBox
    Friend WithEvents btnExecuteArc As System.Windows.Forms.Button
    Friend WithEvents Label17 As System.Windows.Forms.Label
    Friend WithEvents Label18 As System.Windows.Forms.Label
    Friend WithEvents txtArcEX As System.Windows.Forms.TextBox
    Friend WithEvents txtArcEZ As System.Windows.Forms.TextBox
    Friend WithEvents txtArcEY As System.Windows.Forms.TextBox
    Friend WithEvents Label19 As System.Windows.Forms.Label
    Friend WithEvents Label20 As System.Windows.Forms.Label
    Friend WithEvents txtArcCX As System.Windows.Forms.TextBox
    Friend WithEvents txtArcCY As System.Windows.Forms.TextBox
    Friend WithEvents Label22 As System.Windows.Forms.Label
    Friend WithEvents lblCoordStatus As System.Windows.Forms.TextBox
    Friend WithEvents btnClearCoordLog As System.Windows.Forms.Button
    Friend WithEvents Label21 As System.Windows.Forms.Label
    Friend WithEvents txtFeedRate As System.Windows.Forms.TextBox
    Friend WithEvents Button1 As System.Windows.Forms.Button
    Friend WithEvents SyncPositions As System.Windows.Forms.Button
    Friend WithEvents Label23 As System.Windows.Forms.Label

End Class
