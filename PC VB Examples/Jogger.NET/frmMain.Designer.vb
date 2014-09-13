<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> Partial Class frmMain
#Region "Windows Form Designer generated code "
	<System.Diagnostics.DebuggerNonUserCode()> Public Sub New()
		MyBase.New()
		'This call is required by the Windows Form Designer.
		InitializeComponent()
	End Sub
	'Form overrides dispose to clean up the component list.
	<System.Diagnostics.DebuggerNonUserCode()> Protected Overloads Overrides Sub Dispose(ByVal Disposing As Boolean)
		If Disposing Then
			If Not components Is Nothing Then
				components.Dispose()
			End If
		End If
		MyBase.Dispose(Disposing)
	End Sub
	'Required by the Windows Form Designer
	Private components As System.ComponentModel.IContainer
	Public ToolTip1 As System.Windows.Forms.ToolTip
	Public WithEvents Timer1 As System.Windows.Forms.Timer
	Public WithEvents _cmdANeg_6 As System.Windows.Forms.Button
	Public WithEvents _cmdAPos_5 As System.Windows.Forms.Button
	Public WithEvents _cmdZDown_4 As System.Windows.Forms.Button
	Public WithEvents _cmdZUp_3 As System.Windows.Forms.Button
	Public WithEvents _cmdYFront_2 As System.Windows.Forms.Button
	Public WithEvents _cmdXRight_1 As System.Windows.Forms.Button
	Public WithEvents cmdYBack As System.Windows.Forms.Button
	Public WithEvents _cmdXLeft_0 As System.Windows.Forms.Button
	Public WithEvents USBLocation As System.Windows.Forms.Label
	Public WithEvents txtAPos As System.Windows.Forms.Label
	Public WithEvents txtZPos As System.Windows.Forms.Label
	Public WithEvents txtYPos As System.Windows.Forms.Label
	Public WithEvents txtXPos As System.Windows.Forms.Label
	Public WithEvents Label3 As System.Windows.Forms.Label
	Public WithEvents Label2 As System.Windows.Forms.Label
	Public WithEvents Label1 As System.Windows.Forms.Label
	Public WithEvents cmdANeg As Microsoft.VisualBasic.Compatibility.VB6.ButtonArray
	Public WithEvents cmdAPos As Microsoft.VisualBasic.Compatibility.VB6.ButtonArray
	Public WithEvents cmdXLeft As Microsoft.VisualBasic.Compatibility.VB6.ButtonArray
	Public WithEvents cmdXRight As Microsoft.VisualBasic.Compatibility.VB6.ButtonArray
	Public WithEvents cmdYFront As Microsoft.VisualBasic.Compatibility.VB6.ButtonArray
	Public WithEvents cmdZDown As Microsoft.VisualBasic.Compatibility.VB6.ButtonArray
	Public WithEvents cmdZUp As Microsoft.VisualBasic.Compatibility.VB6.ButtonArray
	'NOTE: The following procedure is required by the Windows Form Designer
	'It can be modified using the Windows Form Designer.
	'Do not modify it using the code editor.
	<System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container
        Me.ToolTip1 = New System.Windows.Forms.ToolTip(Me.components)
        Me.TrackBarX = New System.Windows.Forms.TrackBar
        Me.Timer1 = New System.Windows.Forms.Timer(Me.components)
        Me._cmdANeg_6 = New System.Windows.Forms.Button
        Me._cmdAPos_5 = New System.Windows.Forms.Button
        Me._cmdZDown_4 = New System.Windows.Forms.Button
        Me._cmdZUp_3 = New System.Windows.Forms.Button
        Me._cmdYFront_2 = New System.Windows.Forms.Button
        Me._cmdXRight_1 = New System.Windows.Forms.Button
        Me.cmdYBack = New System.Windows.Forms.Button
        Me._cmdXLeft_0 = New System.Windows.Forms.Button
        Me.USBLocation = New System.Windows.Forms.Label
        Me.txtAPos = New System.Windows.Forms.Label
        Me.txtZPos = New System.Windows.Forms.Label
        Me.txtYPos = New System.Windows.Forms.Label
        Me.txtXPos = New System.Windows.Forms.Label
        Me.Label3 = New System.Windows.Forms.Label
        Me.Label2 = New System.Windows.Forms.Label
        Me.Label1 = New System.Windows.Forms.Label
        Me.cmdANeg = New Microsoft.VisualBasic.Compatibility.VB6.ButtonArray(Me.components)
        Me.cmdAPos = New Microsoft.VisualBasic.Compatibility.VB6.ButtonArray(Me.components)
        Me.cmdXLeft = New Microsoft.VisualBasic.Compatibility.VB6.ButtonArray(Me.components)
        Me.cmdXRight = New Microsoft.VisualBasic.Compatibility.VB6.ButtonArray(Me.components)
        Me.cmdYFront = New Microsoft.VisualBasic.Compatibility.VB6.ButtonArray(Me.components)
        Me.cmdZDown = New Microsoft.VisualBasic.Compatibility.VB6.ButtonArray(Me.components)
        Me.cmdZUp = New Microsoft.VisualBasic.Compatibility.VB6.ButtonArray(Me.components)
        Me.CFile = New System.Windows.Forms.RichTextBox
        Me.CThread = New System.Windows.Forms.RichTextBox
        Me.CompileLoadCoff = New System.Windows.Forms.Button
        Me.Label4 = New System.Windows.Forms.Label
        Me.Label5 = New System.Windows.Forms.Label
        Me.CompilerOutput = New System.Windows.Forms.RichTextBox
        Me.Execute = New System.Windows.Forms.Button
        Me.SetConsoleCallback = New System.Windows.Forms.Button
        Me.ConsoleMessages = New System.Windows.Forms.RichTextBox
        Me.Label6 = New System.Windows.Forms.Label
        Me.Label7 = New System.Windows.Forms.Label
        Me.ClearMessages = New System.Windows.Forms.Button
        Me.OpenCFile = New System.Windows.Forms.OpenFileDialog
        Me.BrowseCFile = New System.Windows.Forms.Button
        Me.TrackBarY = New System.Windows.Forms.TrackBar
        Me.TrackBarZ = New System.Windows.Forms.TrackBar
        Me.TrackBarA = New System.Windows.Forms.TrackBar
        CType(Me.TrackBarX, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cmdANeg, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cmdAPos, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cmdXLeft, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cmdXRight, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cmdYFront, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cmdZDown, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cmdZUp, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.TrackBarY, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.TrackBarZ, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.TrackBarA, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'TrackBarX
        '
        Me.TrackBarX.Location = New System.Drawing.Point(12, 190)
        Me.TrackBarX.Maximum = 20
        Me.TrackBarX.Name = "TrackBarX"
        Me.TrackBarX.Size = New System.Drawing.Size(90, 45)
        Me.TrackBarX.TabIndex = 26
        Me.ToolTip1.SetToolTip(Me.TrackBarX, "X Speed")
        Me.TrackBarX.Value = 10
        '
        'Timer1
        '
        Me.Timer1.Enabled = True
        Me.Timer1.Interval = 300
        '
        '_cmdANeg_6
        '
        Me._cmdANeg_6.BackColor = System.Drawing.SystemColors.Control
        Me._cmdANeg_6.Cursor = System.Windows.Forms.Cursors.Default
        Me._cmdANeg_6.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me._cmdANeg_6.ForeColor = System.Drawing.SystemColors.ControlText
        Me.cmdANeg.SetIndex(Me._cmdANeg_6, CType(6, Short))
        Me._cmdANeg_6.Location = New System.Drawing.Point(328, 104)
        Me._cmdANeg_6.Name = "_cmdANeg_6"
        Me._cmdANeg_6.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me._cmdANeg_6.Size = New System.Drawing.Size(53, 33)
        Me._cmdANeg_6.TabIndex = 7
        Me._cmdANeg_6.Text = "Neg"
        Me._cmdANeg_6.UseVisualStyleBackColor = False
        '
        '_cmdAPos_5
        '
        Me._cmdAPos_5.BackColor = System.Drawing.SystemColors.Control
        Me._cmdAPos_5.Cursor = System.Windows.Forms.Cursors.Default
        Me._cmdAPos_5.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me._cmdAPos_5.ForeColor = System.Drawing.SystemColors.ControlText
        Me.cmdAPos.SetIndex(Me._cmdAPos_5, CType(5, Short))
        Me._cmdAPos_5.Location = New System.Drawing.Point(328, 8)
        Me._cmdAPos_5.Name = "_cmdAPos_5"
        Me._cmdAPos_5.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me._cmdAPos_5.Size = New System.Drawing.Size(53, 33)
        Me._cmdAPos_5.TabIndex = 6
        Me._cmdAPos_5.Text = "Pos"
        Me._cmdAPos_5.UseVisualStyleBackColor = False
        '
        '_cmdZDown_4
        '
        Me._cmdZDown_4.BackColor = System.Drawing.SystemColors.Control
        Me._cmdZDown_4.Cursor = System.Windows.Forms.Cursors.Default
        Me._cmdZDown_4.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me._cmdZDown_4.ForeColor = System.Drawing.SystemColors.ControlText
        Me.cmdZDown.SetIndex(Me._cmdZDown_4, CType(4, Short))
        Me._cmdZDown_4.Location = New System.Drawing.Point(235, 104)
        Me._cmdZDown_4.Name = "_cmdZDown_4"
        Me._cmdZDown_4.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me._cmdZDown_4.Size = New System.Drawing.Size(53, 33)
        Me._cmdZDown_4.TabIndex = 5
        Me._cmdZDown_4.Text = "Down"
        Me._cmdZDown_4.UseVisualStyleBackColor = False
        '
        '_cmdZUp_3
        '
        Me._cmdZUp_3.BackColor = System.Drawing.SystemColors.Control
        Me._cmdZUp_3.Cursor = System.Windows.Forms.Cursors.Default
        Me._cmdZUp_3.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me._cmdZUp_3.ForeColor = System.Drawing.SystemColors.ControlText
        Me.cmdZUp.SetIndex(Me._cmdZUp_3, CType(3, Short))
        Me._cmdZUp_3.Location = New System.Drawing.Point(235, 8)
        Me._cmdZUp_3.Name = "_cmdZUp_3"
        Me._cmdZUp_3.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me._cmdZUp_3.Size = New System.Drawing.Size(53, 33)
        Me._cmdZUp_3.TabIndex = 4
        Me._cmdZUp_3.Text = "Up"
        Me._cmdZUp_3.UseVisualStyleBackColor = False
        '
        '_cmdYFront_2
        '
        Me._cmdYFront_2.BackColor = System.Drawing.SystemColors.Control
        Me._cmdYFront_2.Cursor = System.Windows.Forms.Cursors.Default
        Me._cmdYFront_2.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me._cmdYFront_2.ForeColor = System.Drawing.SystemColors.ControlText
        Me.cmdYFront.SetIndex(Me._cmdYFront_2, CType(2, Short))
        Me._cmdYFront_2.Location = New System.Drawing.Point(88, 104)
        Me._cmdYFront_2.Name = "_cmdYFront_2"
        Me._cmdYFront_2.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me._cmdYFront_2.Size = New System.Drawing.Size(53, 33)
        Me._cmdYFront_2.TabIndex = 3
        Me._cmdYFront_2.Text = "Front"
        Me._cmdYFront_2.UseVisualStyleBackColor = False
        '
        '_cmdXRight_1
        '
        Me._cmdXRight_1.BackColor = System.Drawing.SystemColors.Control
        Me._cmdXRight_1.Cursor = System.Windows.Forms.Cursors.Default
        Me._cmdXRight_1.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me._cmdXRight_1.ForeColor = System.Drawing.SystemColors.ControlText
        Me.cmdXRight.SetIndex(Me._cmdXRight_1, CType(1, Short))
        Me._cmdXRight_1.Location = New System.Drawing.Point(152, 56)
        Me._cmdXRight_1.Name = "_cmdXRight_1"
        Me._cmdXRight_1.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me._cmdXRight_1.Size = New System.Drawing.Size(53, 33)
        Me._cmdXRight_1.TabIndex = 2
        Me._cmdXRight_1.Text = "Right"
        Me._cmdXRight_1.UseVisualStyleBackColor = False
        '
        'cmdYBack
        '
        Me.cmdYBack.BackColor = System.Drawing.SystemColors.Control
        Me.cmdYBack.Cursor = System.Windows.Forms.Cursors.Default
        Me.cmdYBack.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.cmdYBack.ForeColor = System.Drawing.SystemColors.ControlText
        Me.cmdYBack.Location = New System.Drawing.Point(88, 8)
        Me.cmdYBack.Name = "cmdYBack"
        Me.cmdYBack.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.cmdYBack.Size = New System.Drawing.Size(53, 33)
        Me.cmdYBack.TabIndex = 1
        Me.cmdYBack.Text = "Back"
        Me.cmdYBack.UseVisualStyleBackColor = False
        '
        '_cmdXLeft_0
        '
        Me._cmdXLeft_0.BackColor = System.Drawing.SystemColors.Control
        Me._cmdXLeft_0.Cursor = System.Windows.Forms.Cursors.Default
        Me._cmdXLeft_0.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me._cmdXLeft_0.ForeColor = System.Drawing.SystemColors.ControlText
        Me.cmdXLeft.SetIndex(Me._cmdXLeft_0, CType(0, Short))
        Me._cmdXLeft_0.Location = New System.Drawing.Point(24, 56)
        Me._cmdXLeft_0.Name = "_cmdXLeft_0"
        Me._cmdXLeft_0.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me._cmdXLeft_0.Size = New System.Drawing.Size(53, 33)
        Me._cmdXLeft_0.TabIndex = 0
        Me._cmdXLeft_0.Text = "Left"
        Me._cmdXLeft_0.UseVisualStyleBackColor = False
        '
        'USBLocation
        '
        Me.USBLocation.BackColor = System.Drawing.SystemColors.Control
        Me.USBLocation.Cursor = System.Windows.Forms.Cursors.Default
        Me.USBLocation.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.USBLocation.ForeColor = System.Drawing.SystemColors.ControlText
        Me.USBLocation.Location = New System.Drawing.Point(112, 238)
        Me.USBLocation.Name = "USBLocation"
        Me.USBLocation.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.USBLocation.Size = New System.Drawing.Size(169, 17)
        Me.USBLocation.TabIndex = 19
        Me.USBLocation.Text = "USB Location"
        '
        'txtAPos
        '
        Me.txtAPos.BackColor = System.Drawing.SystemColors.Control
        Me.txtAPos.Cursor = System.Windows.Forms.Cursors.Default
        Me.txtAPos.Font = New System.Drawing.Font("Arial", 13.5!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtAPos.ForeColor = System.Drawing.SystemColors.ControlText
        Me.txtAPos.Location = New System.Drawing.Point(320, 152)
        Me.txtAPos.Name = "txtAPos"
        Me.txtAPos.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.txtAPos.Size = New System.Drawing.Size(65, 25)
        Me.txtAPos.TabIndex = 18
        Me.txtAPos.Text = "-01.234"
        '
        'txtZPos
        '
        Me.txtZPos.BackColor = System.Drawing.SystemColors.Control
        Me.txtZPos.Cursor = System.Windows.Forms.Cursors.Default
        Me.txtZPos.Font = New System.Drawing.Font("Arial", 13.5!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtZPos.ForeColor = System.Drawing.SystemColors.ControlText
        Me.txtZPos.Location = New System.Drawing.Point(224, 152)
        Me.txtZPos.Name = "txtZPos"
        Me.txtZPos.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.txtZPos.Size = New System.Drawing.Size(65, 25)
        Me.txtZPos.TabIndex = 17
        Me.txtZPos.Text = "-01.234"
        '
        'txtYPos
        '
        Me.txtYPos.BackColor = System.Drawing.SystemColors.Control
        Me.txtYPos.Cursor = System.Windows.Forms.Cursors.Default
        Me.txtYPos.Font = New System.Drawing.Font("Arial", 13.5!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtYPos.ForeColor = System.Drawing.SystemColors.ControlText
        Me.txtYPos.Location = New System.Drawing.Point(126, 152)
        Me.txtYPos.Name = "txtYPos"
        Me.txtYPos.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.txtYPos.Size = New System.Drawing.Size(65, 25)
        Me.txtYPos.TabIndex = 16
        Me.txtYPos.Text = "-01.234"
        '
        'txtXPos
        '
        Me.txtXPos.BackColor = System.Drawing.SystemColors.Control
        Me.txtXPos.Cursor = System.Windows.Forms.Cursors.Default
        Me.txtXPos.Font = New System.Drawing.Font("Arial", 13.5!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtXPos.ForeColor = System.Drawing.SystemColors.ControlText
        Me.txtXPos.Location = New System.Drawing.Point(16, 152)
        Me.txtXPos.Name = "txtXPos"
        Me.txtXPos.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.txtXPos.Size = New System.Drawing.Size(65, 25)
        Me.txtXPos.TabIndex = 15
        Me.txtXPos.Text = "-01.234"
        '
        'Label3
        '
        Me.Label3.BackColor = System.Drawing.SystemColors.Control
        Me.Label3.Cursor = System.Windows.Forms.Cursors.Default
        Me.Label3.Font = New System.Drawing.Font("Arial", 13.5!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label3.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label3.Location = New System.Drawing.Point(336, 64)
        Me.Label3.Name = "Label3"
        Me.Label3.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Label3.Size = New System.Drawing.Size(25, 25)
        Me.Label3.TabIndex = 14
        Me.Label3.Text = "A"
        Me.Label3.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'Label2
        '
        Me.Label2.BackColor = System.Drawing.SystemColors.Control
        Me.Label2.Cursor = System.Windows.Forms.Cursors.Default
        Me.Label2.Font = New System.Drawing.Font("Arial", 13.5!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label2.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label2.Location = New System.Drawing.Point(248, 64)
        Me.Label2.Name = "Label2"
        Me.Label2.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Label2.Size = New System.Drawing.Size(25, 25)
        Me.Label2.TabIndex = 13
        Me.Label2.Text = "Z"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'Label1
        '
        Me.Label1.BackColor = System.Drawing.SystemColors.Control
        Me.Label1.Cursor = System.Windows.Forms.Cursors.Default
        Me.Label1.Font = New System.Drawing.Font("Arial", 13.5!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label1.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label1.Location = New System.Drawing.Point(24, 16)
        Me.Label1.Name = "Label1"
        Me.Label1.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Label1.Size = New System.Drawing.Size(57, 25)
        Me.Label1.TabIndex = 12
        Me.Label1.Text = "X && Y"
        '
        'cmdANeg
        '
        '
        'cmdAPos
        '
        '
        'cmdXLeft
        '
        '
        'cmdXRight
        '
        '
        'cmdYFront
        '
        '
        'cmdZDown
        '
        '
        'cmdZUp
        '
        '
        'CFile
        '
        Me.CFile.Location = New System.Drawing.Point(8, 275)
        Me.CFile.Name = "CFile"
        Me.CFile.Size = New System.Drawing.Size(385, 22)
        Me.CFile.TabIndex = 20
        Me.CFile.Text = "C:\KMotionSrc\C Programs\Print.c"
        '
        'CThread
        '
        Me.CThread.Location = New System.Drawing.Point(59, 305)
        Me.CThread.Name = "CThread"
        Me.CThread.Size = New System.Drawing.Size(33, 24)
        Me.CThread.TabIndex = 21
        Me.CThread.Text = "1"
        '
        'CompileLoadCoff
        '
        Me.CompileLoadCoff.Location = New System.Drawing.Point(98, 303)
        Me.CompileLoadCoff.Name = "CompileLoadCoff"
        Me.CompileLoadCoff.Size = New System.Drawing.Size(111, 25)
        Me.CompileLoadCoff.TabIndex = 22
        Me.CompileLoadCoff.Text = "Compile Load Coff"
        Me.CompileLoadCoff.UseVisualStyleBackColor = True
        '
        'Label4
        '
        Me.Label4.BackColor = System.Drawing.SystemColors.Control
        Me.Label4.Cursor = System.Windows.Forms.Cursors.Default
        Me.Label4.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label4.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label4.Location = New System.Drawing.Point(12, 308)
        Me.Label4.Name = "Label4"
        Me.Label4.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Label4.Size = New System.Drawing.Size(41, 16)
        Me.Label4.TabIndex = 19
        Me.Label4.Text = "Thread"
        '
        'Label5
        '
        Me.Label5.BackColor = System.Drawing.SystemColors.Control
        Me.Label5.Cursor = System.Windows.Forms.Cursors.Default
        Me.Label5.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label5.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label5.Location = New System.Drawing.Point(12, 256)
        Me.Label5.Name = "Label5"
        Me.Label5.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Label5.Size = New System.Drawing.Size(41, 16)
        Me.Label5.TabIndex = 19
        Me.Label5.Text = "C File"
        '
        'CompilerOutput
        '
        Me.CompilerOutput.Location = New System.Drawing.Point(8, 358)
        Me.CompilerOutput.Name = "CompilerOutput"
        Me.CompilerOutput.Size = New System.Drawing.Size(385, 36)
        Me.CompilerOutput.TabIndex = 20
        Me.CompilerOutput.Text = ""
        '
        'Execute
        '
        Me.Execute.Location = New System.Drawing.Point(224, 303)
        Me.Execute.Name = "Execute"
        Me.Execute.Size = New System.Drawing.Size(73, 25)
        Me.Execute.TabIndex = 22
        Me.Execute.Text = "Execute"
        Me.Execute.UseVisualStyleBackColor = True
        '
        'SetConsoleCallback
        '
        Me.SetConsoleCallback.Location = New System.Drawing.Point(10, 400)
        Me.SetConsoleCallback.Name = "SetConsoleCallback"
        Me.SetConsoleCallback.Size = New System.Drawing.Size(119, 25)
        Me.SetConsoleCallback.TabIndex = 22
        Me.SetConsoleCallback.Text = "Set Console Callback"
        Me.SetConsoleCallback.UseVisualStyleBackColor = True
        '
        'ConsoleMessages
        '
        Me.ConsoleMessages.Location = New System.Drawing.Point(8, 431)
        Me.ConsoleMessages.Name = "ConsoleMessages"
        Me.ConsoleMessages.Size = New System.Drawing.Size(385, 91)
        Me.ConsoleMessages.TabIndex = 23
        Me.ConsoleMessages.Text = ""
        '
        'Label6
        '
        Me.Label6.BackColor = System.Drawing.SystemColors.Control
        Me.Label6.Cursor = System.Windows.Forms.Cursors.Default
        Me.Label6.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label6.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label6.Location = New System.Drawing.Point(7, 339)
        Me.Label6.Name = "Label6"
        Me.Label6.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Label6.Size = New System.Drawing.Size(118, 19)
        Me.Label6.TabIndex = 19
        Me.Label6.Text = "Compiler messages"
        '
        'Label7
        '
        Me.Label7.BackColor = System.Drawing.SystemColors.Control
        Me.Label7.Cursor = System.Windows.Forms.Cursors.Default
        Me.Label7.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label7.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label7.Location = New System.Drawing.Point(155, 411)
        Me.Label7.Name = "Label7"
        Me.Label7.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Label7.Size = New System.Drawing.Size(118, 22)
        Me.Label7.TabIndex = 19
        Me.Label7.Text = "Console Messages"
        '
        'ClearMessages
        '
        Me.ClearMessages.Location = New System.Drawing.Point(298, 404)
        Me.ClearMessages.Name = "ClearMessages"
        Me.ClearMessages.Size = New System.Drawing.Size(87, 21)
        Me.ClearMessages.TabIndex = 24
        Me.ClearMessages.Text = "clear"
        Me.ClearMessages.UseVisualStyleBackColor = True
        '
        'OpenCFile
        '
        Me.OpenCFile.DefaultExt = "c"
        Me.OpenCFile.FileName = "OpenCFile"
        Me.OpenCFile.Filter = "Text files (*.c)|*.c|All files (*.*)|*.*"
        Me.OpenCFile.Title = "Open C File To Compile"
        '
        'BrowseCFile
        '
        Me.BrowseCFile.Location = New System.Drawing.Point(312, 303)
        Me.BrowseCFile.Name = "BrowseCFile"
        Me.BrowseCFile.Size = New System.Drawing.Size(71, 25)
        Me.BrowseCFile.TabIndex = 25
        Me.BrowseCFile.Text = "Browse..."
        Me.BrowseCFile.UseVisualStyleBackColor = True
        '
        'TrackBarY
        '
        Me.TrackBarY.Location = New System.Drawing.Point(109, 190)
        Me.TrackBarY.Maximum = 20
        Me.TrackBarY.Name = "TrackBarY"
        Me.TrackBarY.Size = New System.Drawing.Size(90, 45)
        Me.TrackBarY.TabIndex = 27
        Me.ToolTip1.SetToolTip(Me.TrackBarY, "Y Speed")
        Me.TrackBarY.Value = 10
        '
        'TrackBarZ
        '
        Me.TrackBarZ.Location = New System.Drawing.Point(206, 190)
        Me.TrackBarZ.Maximum = 20
        Me.TrackBarZ.Name = "TrackBarZ"
        Me.TrackBarZ.Size = New System.Drawing.Size(90, 45)
        Me.TrackBarZ.TabIndex = 27
        Me.ToolTip1.SetToolTip(Me.TrackBarZ, "Z Speed")
        Me.TrackBarZ.Value = 10
        '
        'TrackBarA
        '
        Me.TrackBarA.Location = New System.Drawing.Point(303, 190)
        Me.TrackBarA.Maximum = 20
        Me.TrackBarA.Name = "TrackBarA"
        Me.TrackBarA.Size = New System.Drawing.Size(90, 45)
        Me.TrackBarA.TabIndex = 27
        Me.ToolTip1.SetToolTip(Me.TrackBarA, "A Speed")
        Me.TrackBarA.Value = 10
        '
        'frmMain
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 14.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.BackColor = System.Drawing.SystemColors.Control
        Me.ClientSize = New System.Drawing.Size(404, 534)
        Me.Controls.Add(Me.TrackBarA)
        Me.Controls.Add(Me.TrackBarZ)
        Me.Controls.Add(Me.TrackBarY)
        Me.Controls.Add(Me.TrackBarX)
        Me.Controls.Add(Me.BrowseCFile)
        Me.Controls.Add(Me.ClearMessages)
        Me.Controls.Add(Me.ConsoleMessages)
        Me.Controls.Add(Me.SetConsoleCallback)
        Me.Controls.Add(Me.Execute)
        Me.Controls.Add(Me.CompileLoadCoff)
        Me.Controls.Add(Me.CThread)
        Me.Controls.Add(Me.CompilerOutput)
        Me.Controls.Add(Me.CFile)
        Me.Controls.Add(Me._cmdANeg_6)
        Me.Controls.Add(Me._cmdAPos_5)
        Me.Controls.Add(Me._cmdZDown_4)
        Me.Controls.Add(Me._cmdZUp_3)
        Me.Controls.Add(Me._cmdYFront_2)
        Me.Controls.Add(Me._cmdXRight_1)
        Me.Controls.Add(Me.cmdYBack)
        Me.Controls.Add(Me._cmdXLeft_0)
        Me.Controls.Add(Me.Label7)
        Me.Controls.Add(Me.Label6)
        Me.Controls.Add(Me.Label5)
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.USBLocation)
        Me.Controls.Add(Me.txtAPos)
        Me.Controls.Add(Me.txtZPos)
        Me.Controls.Add(Me.txtYPos)
        Me.Controls.Add(Me.txtXPos)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Label1)
        Me.Cursor = System.Windows.Forms.Cursors.Default
        Me.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.Location = New System.Drawing.Point(3, 22)
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmMain"
        Me.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Text = "Jogger"
        CType(Me.TrackBarX, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cmdANeg, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cmdAPos, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cmdXLeft, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cmdXRight, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cmdYFront, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cmdZDown, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cmdZUp, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.TrackBarY, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.TrackBarZ, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.TrackBarA, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents CFile As System.Windows.Forms.RichTextBox
    Friend WithEvents CThread As System.Windows.Forms.RichTextBox
    Friend WithEvents CompileLoadCoff As System.Windows.Forms.Button
    Public WithEvents Label4 As System.Windows.Forms.Label
    Public WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents CompilerOutput As System.Windows.Forms.RichTextBox
    Friend WithEvents Execute As System.Windows.Forms.Button
    Friend WithEvents SetConsoleCallback As System.Windows.Forms.Button
    Friend WithEvents ConsoleMessages As System.Windows.Forms.RichTextBox
    Public WithEvents Label6 As System.Windows.Forms.Label
    Public WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents ClearMessages As System.Windows.Forms.Button
    Friend WithEvents OpenCFile As System.Windows.Forms.OpenFileDialog
    Friend WithEvents BrowseCFile As System.Windows.Forms.Button
    Friend WithEvents TrackBarX As System.Windows.Forms.TrackBar
    Friend WithEvents TrackBarY As System.Windows.Forms.TrackBar
    Friend WithEvents TrackBarZ As System.Windows.Forms.TrackBar
    Friend WithEvents TrackBarA As System.Windows.Forms.TrackBar
#End Region 
End Class