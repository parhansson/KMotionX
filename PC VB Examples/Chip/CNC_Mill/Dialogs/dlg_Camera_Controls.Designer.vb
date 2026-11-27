<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class dlg_Camera_Controls
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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(dlg_Camera_Controls))
        Me.ts_Main = New System.Windows.Forms.ToolStrip()
        Me.ts_Exit = New System.Windows.Forms.ToolStripButton()
        Me.ts_Show_Overlay = New System.Windows.Forms.ToolStripButton()
        Me.ts_Cross = New System.Windows.Forms.ToolStripButton()
        Me.ts_X_Cross = New System.Windows.Forms.ToolStripButton()
        Me.ts_Circle = New System.Windows.Forms.ToolStripButton()
        Me.ts_Speed = New System.Windows.Forms.ToolStripButton()
        Me.ts_Radius_Label = New System.Windows.Forms.ToolStripLabel()
        Me.ts_Radius = New System.Windows.Forms.ToolStripTextBox()
        Me.ts_Set_Radius = New System.Windows.Forms.ToolStripButton()
        Me.ts_Setup = New System.Windows.Forms.ToolStripButton()
        Me.ts_lab_Feed = New System.Windows.Forms.ToolStripLabel()
        Me.ts_Feed = New System.Windows.Forms.ToolStripTextBox()
        Me.lab_X = New System.Windows.Forms.Label()
        Me.txt_Absolute_X = New System.Windows.Forms.TextBox()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Work = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.txt_Work_X = New System.Windows.Forms.TextBox()
        Me.txt_Delta_X = New System.Windows.Forms.TextBox()
        Me.txt_Start_X = New System.Windows.Forms.TextBox()
        Me.txt_Start_Y = New System.Windows.Forms.TextBox()
        Me.txt_Delta_Y = New System.Windows.Forms.TextBox()
        Me.txt_Work_Y = New System.Windows.Forms.TextBox()
        Me.txt_Absolute_Y = New System.Windows.Forms.TextBox()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.txt_Start_Z = New System.Windows.Forms.TextBox()
        Me.txt_Delta_Z = New System.Windows.Forms.TextBox()
        Me.txt_Work_Z = New System.Windows.Forms.TextBox()
        Me.txt_Absolute_Z = New System.Windows.Forms.TextBox()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.txt_Distance = New System.Windows.Forms.TextBox()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.btn_Set_Start = New System.Windows.Forms.Button()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.txt_Angle = New System.Windows.Forms.TextBox()
        Me.txt_Focus = New System.Windows.Forms.TextBox()
        Me.ts_Main.SuspendLayout()
        Me.SuspendLayout()
        '
        'ts_Main
        '
        Me.ts_Main.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ts_Exit, Me.ts_Show_Overlay, Me.ts_Cross, Me.ts_X_Cross, Me.ts_Circle, Me.ts_Speed, Me.ts_Radius_Label, Me.ts_Radius, Me.ts_Set_Radius, Me.ts_Setup, Me.ts_lab_Feed, Me.ts_Feed})
        Me.ts_Main.Location = New System.Drawing.Point(0, 0)
        Me.ts_Main.Name = "ts_Main"
        Me.ts_Main.Size = New System.Drawing.Size(683, 38)
        Me.ts_Main.TabIndex = 0
        Me.ts_Main.Text = "ToolStrip1"
        '
        'ts_Exit
        '
        Me.ts_Exit.Image = CType(resources.GetObject("ts_Exit.Image"), System.Drawing.Image)
        Me.ts_Exit.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Exit.Name = "ts_Exit"
        Me.ts_Exit.Size = New System.Drawing.Size(29, 35)
        Me.ts_Exit.Text = "Exit"
        Me.ts_Exit.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Show_Overlay
        '
        Me.ts_Show_Overlay.Image = CType(resources.GetObject("ts_Show_Overlay.Image"), System.Drawing.Image)
        Me.ts_Show_Overlay.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Show_Overlay.Name = "ts_Show_Overlay"
        Me.ts_Show_Overlay.Size = New System.Drawing.Size(79, 35)
        Me.ts_Show_Overlay.Text = "Hide Overlay"
        Me.ts_Show_Overlay.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Cross
        '
        Me.ts_Cross.Image = CType(resources.GetObject("ts_Cross.Image"), System.Drawing.Image)
        Me.ts_Cross.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Cross.Name = "ts_Cross"
        Me.ts_Cross.Size = New System.Drawing.Size(40, 35)
        Me.ts_Cross.Text = "Cross"
        Me.ts_Cross.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_X_Cross
        '
        Me.ts_X_Cross.Image = CType(resources.GetObject("ts_X_Cross.Image"), System.Drawing.Image)
        Me.ts_X_Cross.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_X_Cross.Name = "ts_X_Cross"
        Me.ts_X_Cross.Size = New System.Drawing.Size(24, 35)
        Me.ts_X_Cross.Text = " X "
        Me.ts_X_Cross.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Circle
        '
        Me.ts_Circle.Image = CType(resources.GetObject("ts_Circle.Image"), System.Drawing.Image)
        Me.ts_Circle.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Circle.Name = "ts_Circle"
        Me.ts_Circle.Size = New System.Drawing.Size(41, 35)
        Me.ts_Circle.Text = "Circle"
        Me.ts_Circle.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Speed
        '
        Me.ts_Speed.Image = CType(resources.GetObject("ts_Speed.Image"), System.Drawing.Image)
        Me.ts_Speed.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Speed.Name = "ts_Speed"
        Me.ts_Speed.Size = New System.Drawing.Size(36, 35)
        Me.ts_Speed.Text = "Slow"
        Me.ts_Speed.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Radius_Label
        '
        Me.ts_Radius_Label.Name = "ts_Radius_Label"
        Me.ts_Radius_Label.Size = New System.Drawing.Size(42, 35)
        Me.ts_Radius_Label.Text = "Radius"
        '
        'ts_Radius
        '
        Me.ts_Radius.BackColor = System.Drawing.Color.White
        Me.ts_Radius.Name = "ts_Radius"
        Me.ts_Radius.ReadOnly = True
        Me.ts_Radius.Size = New System.Drawing.Size(75, 38)
        '
        'ts_Set_Radius
        '
        Me.ts_Set_Radius.Image = CType(resources.GetObject("ts_Set_Radius.Image"), System.Drawing.Image)
        Me.ts_Set_Radius.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Set_Radius.Name = "ts_Set_Radius"
        Me.ts_Set_Radius.Size = New System.Drawing.Size(65, 35)
        Me.ts_Set_Radius.Text = "Set Radius"
        Me.ts_Set_Radius.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Setup
        '
        Me.ts_Setup.Image = CType(resources.GetObject("ts_Setup.Image"), System.Drawing.Image)
        Me.ts_Setup.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Setup.Name = "ts_Setup"
        Me.ts_Setup.Size = New System.Drawing.Size(41, 35)
        Me.ts_Setup.Text = "Setup"
        Me.ts_Setup.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_lab_Feed
        '
        Me.ts_lab_Feed.Font = New System.Drawing.Font("Segoe UI", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.ts_lab_Feed.Name = "ts_lab_Feed"
        Me.ts_lab_Feed.Size = New System.Drawing.Size(41, 35)
        Me.ts_lab_Feed.Text = "Feed"
        '
        'ts_Feed
        '
        Me.ts_Feed.Font = New System.Drawing.Font("Segoe UI", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.ts_Feed.Name = "ts_Feed"
        Me.ts_Feed.Size = New System.Drawing.Size(75, 38)
        '
        'lab_X
        '
        Me.lab_X.AutoSize = True
        Me.lab_X.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lab_X.Location = New System.Drawing.Point(130, 36)
        Me.lab_X.Name = "lab_X"
        Me.lab_X.Size = New System.Drawing.Size(20, 20)
        Me.lab_X.TabIndex = 1
        Me.lab_X.Text = "X"
        '
        'txt_Absolute_X
        '
        Me.txt_Absolute_X.BackColor = System.Drawing.Color.White
        Me.txt_Absolute_X.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Absolute_X.Location = New System.Drawing.Point(90, 58)
        Me.txt_Absolute_X.Name = "txt_Absolute_X"
        Me.txt_Absolute_X.ReadOnly = True
        Me.txt_Absolute_X.Size = New System.Drawing.Size(100, 26)
        Me.txt_Absolute_X.TabIndex = 2
        Me.txt_Absolute_X.TabStop = False
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label2.Location = New System.Drawing.Point(16, 61)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(72, 20)
        Me.Label2.TabIndex = 3
        Me.Label2.Text = "Absolute"
        '
        'Work
        '
        Me.Work.AutoSize = True
        Me.Work.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Work.Location = New System.Drawing.Point(16, 88)
        Me.Work.Name = "Work"
        Me.Work.Size = New System.Drawing.Size(46, 20)
        Me.Work.TabIndex = 4
        Me.Work.Text = "Work"
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label4.Location = New System.Drawing.Point(16, 115)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(47, 20)
        Me.Label4.TabIndex = 5
        Me.Label4.Text = "Delta"
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label5.Location = New System.Drawing.Point(16, 142)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(44, 20)
        Me.Label5.TabIndex = 6
        Me.Label5.Text = "Start"
        '
        'txt_Work_X
        '
        Me.txt_Work_X.BackColor = System.Drawing.Color.White
        Me.txt_Work_X.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Work_X.Location = New System.Drawing.Point(90, 85)
        Me.txt_Work_X.Name = "txt_Work_X"
        Me.txt_Work_X.ReadOnly = True
        Me.txt_Work_X.Size = New System.Drawing.Size(100, 26)
        Me.txt_Work_X.TabIndex = 7
        Me.txt_Work_X.TabStop = False
        '
        'txt_Delta_X
        '
        Me.txt_Delta_X.BackColor = System.Drawing.Color.White
        Me.txt_Delta_X.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Delta_X.Location = New System.Drawing.Point(90, 112)
        Me.txt_Delta_X.Name = "txt_Delta_X"
        Me.txt_Delta_X.ReadOnly = True
        Me.txt_Delta_X.Size = New System.Drawing.Size(100, 26)
        Me.txt_Delta_X.TabIndex = 8
        Me.txt_Delta_X.TabStop = False
        '
        'txt_Start_X
        '
        Me.txt_Start_X.BackColor = System.Drawing.Color.White
        Me.txt_Start_X.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Start_X.Location = New System.Drawing.Point(90, 139)
        Me.txt_Start_X.Name = "txt_Start_X"
        Me.txt_Start_X.ReadOnly = True
        Me.txt_Start_X.Size = New System.Drawing.Size(100, 26)
        Me.txt_Start_X.TabIndex = 9
        Me.txt_Start_X.TabStop = False
        '
        'txt_Start_Y
        '
        Me.txt_Start_Y.BackColor = System.Drawing.Color.White
        Me.txt_Start_Y.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Start_Y.Location = New System.Drawing.Point(196, 139)
        Me.txt_Start_Y.Name = "txt_Start_Y"
        Me.txt_Start_Y.ReadOnly = True
        Me.txt_Start_Y.Size = New System.Drawing.Size(100, 26)
        Me.txt_Start_Y.TabIndex = 14
        Me.txt_Start_Y.TabStop = False
        '
        'txt_Delta_Y
        '
        Me.txt_Delta_Y.BackColor = System.Drawing.Color.White
        Me.txt_Delta_Y.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Delta_Y.Location = New System.Drawing.Point(196, 112)
        Me.txt_Delta_Y.Name = "txt_Delta_Y"
        Me.txt_Delta_Y.ReadOnly = True
        Me.txt_Delta_Y.Size = New System.Drawing.Size(100, 26)
        Me.txt_Delta_Y.TabIndex = 13
        Me.txt_Delta_Y.TabStop = False
        '
        'txt_Work_Y
        '
        Me.txt_Work_Y.BackColor = System.Drawing.Color.White
        Me.txt_Work_Y.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Work_Y.Location = New System.Drawing.Point(196, 85)
        Me.txt_Work_Y.Name = "txt_Work_Y"
        Me.txt_Work_Y.ReadOnly = True
        Me.txt_Work_Y.Size = New System.Drawing.Size(100, 26)
        Me.txt_Work_Y.TabIndex = 12
        Me.txt_Work_Y.TabStop = False
        '
        'txt_Absolute_Y
        '
        Me.txt_Absolute_Y.BackColor = System.Drawing.Color.White
        Me.txt_Absolute_Y.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Absolute_Y.Location = New System.Drawing.Point(196, 58)
        Me.txt_Absolute_Y.Name = "txt_Absolute_Y"
        Me.txt_Absolute_Y.ReadOnly = True
        Me.txt_Absolute_Y.Size = New System.Drawing.Size(100, 26)
        Me.txt_Absolute_Y.TabIndex = 11
        Me.txt_Absolute_Y.TabStop = False
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label6.Location = New System.Drawing.Point(236, 36)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(20, 20)
        Me.Label6.TabIndex = 10
        Me.Label6.Text = "Y"
        '
        'txt_Start_Z
        '
        Me.txt_Start_Z.BackColor = System.Drawing.Color.White
        Me.txt_Start_Z.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Start_Z.Location = New System.Drawing.Point(302, 139)
        Me.txt_Start_Z.Name = "txt_Start_Z"
        Me.txt_Start_Z.ReadOnly = True
        Me.txt_Start_Z.Size = New System.Drawing.Size(100, 26)
        Me.txt_Start_Z.TabIndex = 19
        Me.txt_Start_Z.TabStop = False
        '
        'txt_Delta_Z
        '
        Me.txt_Delta_Z.BackColor = System.Drawing.Color.White
        Me.txt_Delta_Z.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Delta_Z.Location = New System.Drawing.Point(302, 112)
        Me.txt_Delta_Z.Name = "txt_Delta_Z"
        Me.txt_Delta_Z.ReadOnly = True
        Me.txt_Delta_Z.Size = New System.Drawing.Size(100, 26)
        Me.txt_Delta_Z.TabIndex = 18
        Me.txt_Delta_Z.TabStop = False
        '
        'txt_Work_Z
        '
        Me.txt_Work_Z.BackColor = System.Drawing.Color.White
        Me.txt_Work_Z.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Work_Z.Location = New System.Drawing.Point(302, 85)
        Me.txt_Work_Z.Name = "txt_Work_Z"
        Me.txt_Work_Z.ReadOnly = True
        Me.txt_Work_Z.Size = New System.Drawing.Size(100, 26)
        Me.txt_Work_Z.TabIndex = 17
        Me.txt_Work_Z.TabStop = False
        '
        'txt_Absolute_Z
        '
        Me.txt_Absolute_Z.BackColor = System.Drawing.Color.White
        Me.txt_Absolute_Z.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Absolute_Z.Location = New System.Drawing.Point(302, 58)
        Me.txt_Absolute_Z.Name = "txt_Absolute_Z"
        Me.txt_Absolute_Z.ReadOnly = True
        Me.txt_Absolute_Z.Size = New System.Drawing.Size(100, 26)
        Me.txt_Absolute_Z.TabIndex = 16
        Me.txt_Absolute_Z.TabStop = False
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label7.Location = New System.Drawing.Point(342, 36)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(19, 20)
        Me.Label7.TabIndex = 15
        Me.Label7.Text = "Z"
        '
        'txt_Distance
        '
        Me.txt_Distance.BackColor = System.Drawing.Color.White
        Me.txt_Distance.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Distance.Location = New System.Drawing.Point(408, 58)
        Me.txt_Distance.Name = "txt_Distance"
        Me.txt_Distance.ReadOnly = True
        Me.txt_Distance.Size = New System.Drawing.Size(100, 26)
        Me.txt_Distance.TabIndex = 20
        Me.txt_Distance.TabStop = False
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label8.Location = New System.Drawing.Point(514, 61)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(103, 20)
        Me.Label8.TabIndex = 21
        Me.Label8.Text = "XY_Distance"
        '
        'btn_Set_Start
        '
        Me.btn_Set_Start.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Set_Start.Location = New System.Drawing.Point(412, 137)
        Me.btn_Set_Start.Name = "btn_Set_Start"
        Me.btn_Set_Start.Size = New System.Drawing.Size(75, 31)
        Me.btn_Set_Start.TabIndex = 22
        Me.btn_Set_Start.TabStop = False
        Me.btn_Set_Start.Text = "Set"
        Me.btn_Set_Start.UseVisualStyleBackColor = True
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label3.Location = New System.Drawing.Point(436, 36)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(45, 20)
        Me.Label3.TabIndex = 23
        Me.Label3.Text = "Polar"
        '
        'Label9
        '
        Me.Label9.AutoSize = True
        Me.Label9.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label9.Location = New System.Drawing.Point(514, 88)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(50, 20)
        Me.Label9.TabIndex = 25
        Me.Label9.Text = "Angle"
        '
        'txt_Angle
        '
        Me.txt_Angle.BackColor = System.Drawing.Color.White
        Me.txt_Angle.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Angle.Location = New System.Drawing.Point(408, 85)
        Me.txt_Angle.Name = "txt_Angle"
        Me.txt_Angle.ReadOnly = True
        Me.txt_Angle.Size = New System.Drawing.Size(100, 26)
        Me.txt_Angle.TabIndex = 24
        Me.txt_Angle.TabStop = False
        '
        'txt_Focus
        '
        Me.txt_Focus.BackColor = System.Drawing.Color.White
        Me.txt_Focus.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Focus.Location = New System.Drawing.Point(623, 41)
        Me.txt_Focus.Name = "txt_Focus"
        Me.txt_Focus.ReadOnly = True
        Me.txt_Focus.Size = New System.Drawing.Size(60, 26)
        Me.txt_Focus.TabIndex = 26
        Me.txt_Focus.TabStop = False
        Me.txt_Focus.Text = "Focus"
        '
        'dlg_Camera_Controls
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(683, 168)
        Me.Controls.Add(Me.txt_Focus)
        Me.Controls.Add(Me.Label9)
        Me.Controls.Add(Me.txt_Angle)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.btn_Set_Start)
        Me.Controls.Add(Me.Label8)
        Me.Controls.Add(Me.txt_Distance)
        Me.Controls.Add(Me.txt_Start_Z)
        Me.Controls.Add(Me.txt_Delta_Z)
        Me.Controls.Add(Me.txt_Work_Z)
        Me.Controls.Add(Me.txt_Absolute_Z)
        Me.Controls.Add(Me.Label7)
        Me.Controls.Add(Me.txt_Start_Y)
        Me.Controls.Add(Me.txt_Delta_Y)
        Me.Controls.Add(Me.txt_Work_Y)
        Me.Controls.Add(Me.txt_Absolute_Y)
        Me.Controls.Add(Me.Label6)
        Me.Controls.Add(Me.txt_Start_X)
        Me.Controls.Add(Me.txt_Delta_X)
        Me.Controls.Add(Me.txt_Work_X)
        Me.Controls.Add(Me.Label5)
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.Work)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.txt_Absolute_X)
        Me.Controls.Add(Me.lab_X)
        Me.Controls.Add(Me.ts_Main)
        Me.Name = "dlg_Camera_Controls"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.Manual
        Me.Text = "Bullseye    Author : Rick Caddell"
        Me.TopMost = True
        Me.ts_Main.ResumeLayout(False)
        Me.ts_Main.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents ts_Main As System.Windows.Forms.ToolStrip
    Friend WithEvents ts_Show_Overlay As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Cross As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_X_Cross As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Exit As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Circle As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Setup As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Speed As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Radius_Label As System.Windows.Forms.ToolStripLabel
    Friend WithEvents ts_Radius As System.Windows.Forms.ToolStripTextBox
    Friend WithEvents ts_Set_Radius As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_lab_Feed As System.Windows.Forms.ToolStripLabel
    Friend WithEvents ts_Feed As System.Windows.Forms.ToolStripTextBox
    Friend WithEvents lab_X As System.Windows.Forms.Label
    Friend WithEvents txt_Absolute_X As System.Windows.Forms.TextBox
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Work As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents txt_Work_X As System.Windows.Forms.TextBox
    Friend WithEvents txt_Delta_X As System.Windows.Forms.TextBox
    Friend WithEvents txt_Start_X As System.Windows.Forms.TextBox
    Friend WithEvents txt_Start_Y As System.Windows.Forms.TextBox
    Friend WithEvents txt_Delta_Y As System.Windows.Forms.TextBox
    Friend WithEvents txt_Work_Y As System.Windows.Forms.TextBox
    Friend WithEvents txt_Absolute_Y As System.Windows.Forms.TextBox
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents txt_Start_Z As System.Windows.Forms.TextBox
    Friend WithEvents txt_Delta_Z As System.Windows.Forms.TextBox
    Friend WithEvents txt_Work_Z As System.Windows.Forms.TextBox
    Friend WithEvents txt_Absolute_Z As System.Windows.Forms.TextBox
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents txt_Distance As System.Windows.Forms.TextBox
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents btn_Set_Start As System.Windows.Forms.Button
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents txt_Angle As System.Windows.Forms.TextBox
    Friend WithEvents txt_Focus As System.Windows.Forms.TextBox
End Class
