<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Chip_Drawing
    Inherits System.Windows.Forms.UserControl

    'UserControl overrides dispose to clean up the component list.
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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(Chip_Drawing))
        Me.temp_Dwg_pnl_Main = New System.Windows.Forms.Panel()
        Me.Panel1 = New System.Windows.Forms.Panel()
        Me.pnl_Viewport = New System.Windows.Forms.Panel()
        Me.pnl_Canvas = New System.Windows.Forms.PictureBox()
        Me.pnl_Top = New System.Windows.Forms.Panel()
        Me.txt_Mouse = New System.Windows.Forms.TextBox()
        Me.btn_Min = New System.Windows.Forms.Button()
        Me.btn_Max = New System.Windows.Forms.Button()
        Me.lab_Z_Scale = New System.Windows.Forms.Label()
        Me.ud_Z_Scale = New System.Windows.Forms.NumericUpDown()
        Me.txt_Scale = New System.Windows.Forms.TextBox()
        Me.ts_Drawing = New System.Windows.Forms.ToolStrip()
        Me.ts_Reset = New System.Windows.Forms.ToolStripButton()
        Me.ts_Show_Rapid = New System.Windows.Forms.ToolStripButton()
        Me.ts_Show_Z_Plus = New System.Windows.Forms.ToolStripButton()
        Me.ts_Offsets_On = New System.Windows.Forms.ToolStripButton()
        Me.ts_Offsets_Off = New System.Windows.Forms.ToolStripButton()
        Me.ts_Redraw = New System.Windows.Forms.ToolStripButton()
        Me.ts_Show_Part = New System.Windows.Forms.ToolStripButton()
        Me.lab_Scale = New System.Windows.Forms.Label()
        Me.lab_Z = New System.Windows.Forms.Label()
        Me.ud_Z_Angle = New System.Windows.Forms.NumericUpDown()
        Me.lab_Y = New System.Windows.Forms.Label()
        Me.ud_Y_Angle = New System.Windows.Forms.NumericUpDown()
        Me.lab_X = New System.Windows.Forms.Label()
        Me.ud_X_Angle = New System.Windows.Forms.NumericUpDown()
        Me.temp_Dwg_pnl_Main.SuspendLayout()
        Me.Panel1.SuspendLayout()
        Me.pnl_Viewport.SuspendLayout()
        CType(Me.pnl_Canvas, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.pnl_Top.SuspendLayout()
        CType(Me.ud_Z_Scale, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.ts_Drawing.SuspendLayout()
        CType(Me.ud_Z_Angle, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.ud_Y_Angle, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.ud_X_Angle, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'temp_Dwg_pnl_Main
        '
        Me.temp_Dwg_pnl_Main.BackColor = System.Drawing.Color.WhiteSmoke
        Me.temp_Dwg_pnl_Main.Controls.Add(Me.Panel1)
        Me.temp_Dwg_pnl_Main.Controls.Add(Me.pnl_Top)
        Me.temp_Dwg_pnl_Main.Dock = System.Windows.Forms.DockStyle.Fill
        Me.temp_Dwg_pnl_Main.Location = New System.Drawing.Point(0, 0)
        Me.temp_Dwg_pnl_Main.Name = "temp_Dwg_pnl_Main"
        Me.temp_Dwg_pnl_Main.Size = New System.Drawing.Size(972, 579)
        Me.temp_Dwg_pnl_Main.TabIndex = 156
        '
        'Panel1
        '
        Me.Panel1.Controls.Add(Me.pnl_Viewport)
        Me.Panel1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Panel1.Location = New System.Drawing.Point(0, 38)
        Me.Panel1.Name = "Panel1"
        Me.Panel1.Size = New System.Drawing.Size(972, 541)
        Me.Panel1.TabIndex = 158
        '
        'pnl_Viewport
        '
        Me.pnl_Viewport.BackColor = System.Drawing.Color.DarkSlateGray
        Me.pnl_Viewport.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center
        Me.pnl_Viewport.Controls.Add(Me.pnl_Canvas)
        Me.pnl_Viewport.Dock = System.Windows.Forms.DockStyle.Fill
        Me.pnl_Viewport.Location = New System.Drawing.Point(0, 0)
        Me.pnl_Viewport.Name = "pnl_Viewport"
        Me.pnl_Viewport.Size = New System.Drawing.Size(972, 541)
        Me.pnl_Viewport.TabIndex = 156
        '
        'pnl_Canvas
        '
        Me.pnl_Canvas.BackColor = System.Drawing.Color.DarkSlateGray
        Me.pnl_Canvas.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom
        Me.pnl_Canvas.Location = New System.Drawing.Point(3, 3)
        Me.pnl_Canvas.Name = "pnl_Canvas"
        Me.pnl_Canvas.Size = New System.Drawing.Size(605, 333)
        Me.pnl_Canvas.TabIndex = 103
        Me.pnl_Canvas.TabStop = False
        '
        'pnl_Top
        '
        Me.pnl_Top.Controls.Add(Me.txt_Mouse)
        Me.pnl_Top.Controls.Add(Me.btn_Min)
        Me.pnl_Top.Controls.Add(Me.btn_Max)
        Me.pnl_Top.Controls.Add(Me.lab_Z_Scale)
        Me.pnl_Top.Controls.Add(Me.ud_Z_Scale)
        Me.pnl_Top.Controls.Add(Me.txt_Scale)
        Me.pnl_Top.Controls.Add(Me.ts_Drawing)
        Me.pnl_Top.Controls.Add(Me.lab_Scale)
        Me.pnl_Top.Controls.Add(Me.lab_Z)
        Me.pnl_Top.Controls.Add(Me.ud_Z_Angle)
        Me.pnl_Top.Controls.Add(Me.lab_Y)
        Me.pnl_Top.Controls.Add(Me.ud_Y_Angle)
        Me.pnl_Top.Controls.Add(Me.lab_X)
        Me.pnl_Top.Controls.Add(Me.ud_X_Angle)
        Me.pnl_Top.Dock = System.Windows.Forms.DockStyle.Top
        Me.pnl_Top.Location = New System.Drawing.Point(0, 0)
        Me.pnl_Top.Name = "pnl_Top"
        Me.pnl_Top.Size = New System.Drawing.Size(972, 38)
        Me.pnl_Top.TabIndex = 157
        '
        'txt_Mouse
        '
        Me.txt_Mouse.Location = New System.Drawing.Point(654, 9)
        Me.txt_Mouse.Name = "txt_Mouse"
        Me.txt_Mouse.Size = New System.Drawing.Size(91, 20)
        Me.txt_Mouse.TabIndex = 165
        '
        'btn_Min
        '
        Me.btn_Min.Location = New System.Drawing.Point(605, 18)
        Me.btn_Min.Name = "btn_Min"
        Me.btn_Min.Size = New System.Drawing.Size(43, 19)
        Me.btn_Min.TabIndex = 164
        Me.btn_Min.Text = "Min"
        Me.btn_Min.UseVisualStyleBackColor = True
        '
        'btn_Max
        '
        Me.btn_Max.Location = New System.Drawing.Point(605, 0)
        Me.btn_Max.Name = "btn_Max"
        Me.btn_Max.Size = New System.Drawing.Size(43, 19)
        Me.btn_Max.TabIndex = 163
        Me.btn_Max.Text = "Max"
        Me.btn_Max.UseVisualStyleBackColor = True
        '
        'lab_Z_Scale
        '
        Me.lab_Z_Scale.AutoSize = True
        Me.lab_Z_Scale.Location = New System.Drawing.Point(545, 2)
        Me.lab_Z_Scale.Name = "lab_Z_Scale"
        Me.lab_Z_Scale.Size = New System.Drawing.Size(44, 13)
        Me.lab_Z_Scale.TabIndex = 162
        Me.lab_Z_Scale.Text = "Z Scale"
        '
        'ud_Z_Scale
        '
        Me.ud_Z_Scale.Location = New System.Drawing.Point(548, 18)
        Me.ud_Z_Scale.Maximum = New Decimal(New Integer() {25, 0, 0, 0})
        Me.ud_Z_Scale.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.ud_Z_Scale.Name = "ud_Z_Scale"
        Me.ud_Z_Scale.Size = New System.Drawing.Size(30, 20)
        Me.ud_Z_Scale.TabIndex = 161
        Me.ud_Z_Scale.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.ud_Z_Scale.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'txt_Scale
        '
        Me.txt_Scale.Location = New System.Drawing.Point(482, 17)
        Me.txt_Scale.Name = "txt_Scale"
        Me.txt_Scale.Size = New System.Drawing.Size(50, 20)
        Me.txt_Scale.TabIndex = 159
        '
        'ts_Drawing
        '
        Me.ts_Drawing.Dock = System.Windows.Forms.DockStyle.None
        Me.ts_Drawing.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ts_Reset, Me.ts_Show_Rapid, Me.ts_Show_Z_Plus, Me.ts_Offsets_On, Me.ts_Offsets_Off, Me.ts_Show_Part, Me.ts_Redraw})
        Me.ts_Drawing.LayoutStyle = System.Windows.Forms.ToolStripLayoutStyle.Flow
        Me.ts_Drawing.Location = New System.Drawing.Point(0, 0)
        Me.ts_Drawing.Name = "ts_Drawing"
        Me.ts_Drawing.Size = New System.Drawing.Size(469, 38)
        Me.ts_Drawing.TabIndex = 158
        Me.ts_Drawing.Text = "ts_Gcode"
        '
        'ts_Reset
        '
        Me.ts_Reset.Image = CType(resources.GetObject("ts_Reset.Image"), System.Drawing.Image)
        Me.ts_Reset.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Reset.Name = "ts_Reset"
        Me.ts_Reset.Size = New System.Drawing.Size(39, 35)
        Me.ts_Reset.Text = "Reset"
        Me.ts_Reset.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Show_Rapid
        '
        Me.ts_Show_Rapid.Image = CType(resources.GetObject("ts_Show_Rapid.Image"), System.Drawing.Image)
        Me.ts_Show_Rapid.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Show_Rapid.Name = "ts_Show_Rapid"
        Me.ts_Show_Rapid.Size = New System.Drawing.Size(73, 35)
        Me.ts_Show_Rapid.Text = "Show Rapid"
        Me.ts_Show_Rapid.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Show_Z_Plus
        '
        Me.ts_Show_Z_Plus.Image = CType(resources.GetObject("ts_Show_Z_Plus.Image"), System.Drawing.Image)
        Me.ts_Show_Z_Plus.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Show_Z_Plus.Name = "ts_Show_Z_Plus"
        Me.ts_Show_Z_Plus.Size = New System.Drawing.Size(58, 35)
        Me.ts_Show_Z_Plus.Text = "Show Z+"
        Me.ts_Show_Z_Plus.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Offsets_On
        '
        Me.ts_Offsets_On.Image = CType(resources.GetObject("ts_Offsets_On.Image"), System.Drawing.Image)
        Me.ts_Offsets_On.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Offsets_On.Name = "ts_Offsets_On"
        Me.ts_Offsets_On.Size = New System.Drawing.Size(67, 35)
        Me.ts_Offsets_On.Text = "Offsets On"
        Me.ts_Offsets_On.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Offsets_Off
        '
        Me.ts_Offsets_Off.Image = CType(resources.GetObject("ts_Offsets_Off.Image"), System.Drawing.Image)
        Me.ts_Offsets_Off.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Offsets_Off.Name = "ts_Offsets_Off"
        Me.ts_Offsets_Off.Size = New System.Drawing.Size(68, 35)
        Me.ts_Offsets_Off.Text = "Offsets Off"
        Me.ts_Offsets_Off.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Redraw
        '
        Me.ts_Redraw.Image = CType(resources.GetObject("ts_Redraw.Image"), System.Drawing.Image)
        Me.ts_Redraw.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Redraw.Name = "ts_Redraw"
        Me.ts_Redraw.Size = New System.Drawing.Size(50, 35)
        Me.ts_Redraw.Text = "Redraw"
        Me.ts_Redraw.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Show_Part
        '
        Me.ts_Show_Part.Image = CType(resources.GetObject("ts_Show_Part.Image"), System.Drawing.Image)
        Me.ts_Show_Part.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Show_Part.Name = "ts_Show_Part"
        Me.ts_Show_Part.Size = New System.Drawing.Size(82, 35)
        Me.ts_Show_Part.Text = "Hide Material"
        Me.ts_Show_Part.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'lab_Scale
        '
        Me.lab_Scale.AutoSize = True
        Me.lab_Scale.Location = New System.Drawing.Point(488, 2)
        Me.lab_Scale.Name = "lab_Scale"
        Me.lab_Scale.Size = New System.Drawing.Size(34, 13)
        Me.lab_Scale.TabIndex = 112
        Me.lab_Scale.Text = "Scale"
        '
        'lab_Z
        '
        Me.lab_Z.AutoSize = True
        Me.lab_Z.Location = New System.Drawing.Point(419, 2)
        Me.lab_Z.Name = "lab_Z"
        Me.lab_Z.Size = New System.Drawing.Size(44, 13)
        Me.lab_Z.TabIndex = 110
        Me.lab_Z.Text = "Z Angle"
        '
        'ud_Z_Angle
        '
        Me.ud_Z_Angle.Location = New System.Drawing.Point(426, 18)
        Me.ud_Z_Angle.Maximum = New Decimal(New Integer() {360, 0, 0, 0})
        Me.ud_Z_Angle.Minimum = New Decimal(New Integer() {360, 0, 0, -2147483648})
        Me.ud_Z_Angle.Name = "ud_Z_Angle"
        Me.ud_Z_Angle.Size = New System.Drawing.Size(30, 20)
        Me.ud_Z_Angle.TabIndex = 109
        Me.ud_Z_Angle.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'lab_Y
        '
        Me.lab_Y.AutoSize = True
        Me.lab_Y.Location = New System.Drawing.Point(363, 2)
        Me.lab_Y.Name = "lab_Y"
        Me.lab_Y.Size = New System.Drawing.Size(44, 13)
        Me.lab_Y.TabIndex = 108
        Me.lab_Y.Text = "Y Angle"
        '
        'ud_Y_Angle
        '
        Me.ud_Y_Angle.Location = New System.Drawing.Point(370, 18)
        Me.ud_Y_Angle.Maximum = New Decimal(New Integer() {360, 0, 0, 0})
        Me.ud_Y_Angle.Minimum = New Decimal(New Integer() {360, 0, 0, -2147483648})
        Me.ud_Y_Angle.Name = "ud_Y_Angle"
        Me.ud_Y_Angle.Size = New System.Drawing.Size(30, 20)
        Me.ud_Y_Angle.TabIndex = 107
        Me.ud_Y_Angle.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'lab_X
        '
        Me.lab_X.AutoSize = True
        Me.lab_X.Location = New System.Drawing.Point(317, 3)
        Me.lab_X.Name = "lab_X"
        Me.lab_X.Size = New System.Drawing.Size(44, 13)
        Me.lab_X.TabIndex = 106
        Me.lab_X.Text = "X Angle"
        '
        'ud_X_Angle
        '
        Me.ud_X_Angle.Location = New System.Drawing.Point(322, 18)
        Me.ud_X_Angle.Maximum = New Decimal(New Integer() {360, 0, 0, 0})
        Me.ud_X_Angle.Minimum = New Decimal(New Integer() {360, 0, 0, -2147483648})
        Me.ud_X_Angle.Name = "ud_X_Angle"
        Me.ud_X_Angle.Size = New System.Drawing.Size(30, 20)
        Me.ud_X_Angle.TabIndex = 105
        Me.ud_X_Angle.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Chip_Drawing
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink
        Me.Controls.Add(Me.temp_Dwg_pnl_Main)
        Me.Name = "Chip_Drawing"
        Me.Size = New System.Drawing.Size(972, 579)
        Me.temp_Dwg_pnl_Main.ResumeLayout(False)
        Me.Panel1.ResumeLayout(False)
        Me.pnl_Viewport.ResumeLayout(False)
        CType(Me.pnl_Canvas, System.ComponentModel.ISupportInitialize).EndInit()
        Me.pnl_Top.ResumeLayout(False)
        Me.pnl_Top.PerformLayout()
        CType(Me.ud_Z_Scale, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ts_Drawing.ResumeLayout(False)
        Me.ts_Drawing.PerformLayout()
        CType(Me.ud_Z_Angle, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.ud_Y_Angle, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.ud_X_Angle, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents temp_Dwg_pnl_Main As System.Windows.Forms.Panel
    Friend WithEvents pnl_Viewport As System.Windows.Forms.Panel
    Friend WithEvents pnl_Canvas As System.Windows.Forms.PictureBox
    Friend WithEvents pnl_Top As System.Windows.Forms.Panel
    Friend WithEvents ud_X_Angle As System.Windows.Forms.NumericUpDown
    Friend WithEvents lab_Z As System.Windows.Forms.Label
    Friend WithEvents ud_Z_Angle As System.Windows.Forms.NumericUpDown
    Friend WithEvents lab_Y As System.Windows.Forms.Label
    Friend WithEvents ud_Y_Angle As System.Windows.Forms.NumericUpDown
    Friend WithEvents lab_X As System.Windows.Forms.Label
    Friend WithEvents lab_Scale As System.Windows.Forms.Label
    Friend WithEvents ts_Drawing As System.Windows.Forms.ToolStrip
    Friend WithEvents ts_Reset As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Show_Rapid As System.Windows.Forms.ToolStripButton
    Friend WithEvents txt_Scale As System.Windows.Forms.TextBox
    Friend WithEvents lab_Z_Scale As System.Windows.Forms.Label
    Friend WithEvents ud_Z_Scale As System.Windows.Forms.NumericUpDown
    Friend WithEvents ts_Offsets_On As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Offsets_Off As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Show_Part As System.Windows.Forms.ToolStripButton
    Friend WithEvents Panel1 As System.Windows.Forms.Panel
    Friend WithEvents btn_Min As System.Windows.Forms.Button
    Friend WithEvents btn_Max As System.Windows.Forms.Button
    Friend WithEvents ts_Show_Z_Plus As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Redraw As System.Windows.Forms.ToolStripButton
    Friend WithEvents txt_Mouse As System.Windows.Forms.TextBox

End Class
