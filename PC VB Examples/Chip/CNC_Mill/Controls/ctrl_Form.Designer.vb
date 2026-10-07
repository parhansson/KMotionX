<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class ctrl_Form
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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(ctrl_Form))
        Me.ts_Main = New System.Windows.Forms.ToolStrip()
        Me.ts_Exit = New System.Windows.Forms.ToolStripButton()
        Me.ts_Design = New System.Windows.Forms.ToolStripButton()
        Me.ts_Symbols = New System.Windows.Forms.ToolStripButton()
        Me.ToolStripButton1 = New System.Windows.Forms.ToolStripButton()
        Me.stat_Main = New System.Windows.Forms.StatusStrip()
        Me.stat_Main_Stat = New System.Windows.Forms.ToolStripStatusLabel()
        Me.tab_Top = New System.Windows.Forms.TabControl()
        Me.split_Top = New System.Windows.Forms.Splitter()
        Me.tab_Bottom = New System.Windows.Forms.TabControl()
        Me.split_Bottom = New System.Windows.Forms.Splitter()
        Me.tab_Left = New System.Windows.Forms.TabControl()
        Me.tab_Right = New System.Windows.Forms.TabControl()
        Me.split_Left = New System.Windows.Forms.Splitter()
        Me.split_Right = New System.Windows.Forms.Splitter()
        Me.tab_Middle = New System.Windows.Forms.TabControl()
        Me.ts_Main.SuspendLayout()
        Me.stat_Main.SuspendLayout()
        Me.SuspendLayout()
        '
        'ts_Main
        '
        Me.ts_Main.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ts_Exit, Me.ts_Design, Me.ts_Symbols, Me.ToolStripButton1})
        Me.ts_Main.Location = New System.Drawing.Point(0, 0)
        Me.ts_Main.Name = "ts_Main"
        Me.ts_Main.Size = New System.Drawing.Size(584, 38)
        Me.ts_Main.TabIndex = 6
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
        'ts_Design
        '
        Me.ts_Design.Image = CType(resources.GetObject("ts_Design.Image"), System.Drawing.Image)
        Me.ts_Design.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Design.Name = "ts_Design"
        Me.ts_Design.Size = New System.Drawing.Size(47, 35)
        Me.ts_Design.Text = "Design"
        Me.ts_Design.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Symbols
        '
        Me.ts_Symbols.Image = CType(resources.GetObject("ts_Symbols.Image"), System.Drawing.Image)
        Me.ts_Symbols.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Symbols.Name = "ts_Symbols"
        Me.ts_Symbols.Size = New System.Drawing.Size(56, 35)
        Me.ts_Symbols.Text = "Symbols"
        Me.ts_Symbols.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ToolStripButton1
        '
        Me.ToolStripButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.ToolStripButton1.Image = CType(resources.GetObject("ToolStripButton1.Image"), System.Drawing.Image)
        Me.ToolStripButton1.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ToolStripButton1.Name = "ToolStripButton1"
        Me.ToolStripButton1.Size = New System.Drawing.Size(23, 35)
        Me.ToolStripButton1.Text = "ToolStripButton1"
        '
        'stat_Main
        '
        Me.stat_Main.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.stat_Main_Stat})
        Me.stat_Main.Location = New System.Drawing.Point(0, 334)
        Me.stat_Main.Name = "stat_Main"
        Me.stat_Main.Size = New System.Drawing.Size(584, 22)
        Me.stat_Main.TabIndex = 13
        Me.stat_Main.Text = "StatusStrip1"
        '
        'stat_Main_Stat
        '
        Me.stat_Main_Stat.Name = "stat_Main_Stat"
        Me.stat_Main_Stat.Size = New System.Drawing.Size(26, 17)
        Me.stat_Main_Stat.Text = "stat"
        '
        'tab_Top
        '
        Me.tab_Top.Dock = System.Windows.Forms.DockStyle.Top
        Me.tab_Top.Location = New System.Drawing.Point(0, 38)
        Me.tab_Top.Name = "tab_Top"
        Me.tab_Top.SelectedIndex = 0
        Me.tab_Top.Size = New System.Drawing.Size(584, 62)
        Me.tab_Top.TabIndex = 21
        '
        'split_Top
        '
        Me.split_Top.BackColor = System.Drawing.Color.Firebrick
        Me.split_Top.Dock = System.Windows.Forms.DockStyle.Top
        Me.split_Top.Location = New System.Drawing.Point(0, 100)
        Me.split_Top.Name = "split_Top"
        Me.split_Top.Size = New System.Drawing.Size(584, 3)
        Me.split_Top.TabIndex = 22
        Me.split_Top.TabStop = False
        '
        'tab_Bottom
        '
        Me.tab_Bottom.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.tab_Bottom.Location = New System.Drawing.Point(0, 270)
        Me.tab_Bottom.Name = "tab_Bottom"
        Me.tab_Bottom.SelectedIndex = 0
        Me.tab_Bottom.Size = New System.Drawing.Size(584, 64)
        Me.tab_Bottom.TabIndex = 23
        '
        'split_Bottom
        '
        Me.split_Bottom.BackColor = System.Drawing.Color.Firebrick
        Me.split_Bottom.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.split_Bottom.Location = New System.Drawing.Point(0, 267)
        Me.split_Bottom.Name = "split_Bottom"
        Me.split_Bottom.Size = New System.Drawing.Size(584, 3)
        Me.split_Bottom.TabIndex = 24
        Me.split_Bottom.TabStop = False
        '
        'tab_Left
        '
        Me.tab_Left.Dock = System.Windows.Forms.DockStyle.Left
        Me.tab_Left.Location = New System.Drawing.Point(0, 103)
        Me.tab_Left.Name = "tab_Left"
        Me.tab_Left.SelectedIndex = 0
        Me.tab_Left.Size = New System.Drawing.Size(67, 164)
        Me.tab_Left.TabIndex = 25
        '
        'tab_Right
        '
        Me.tab_Right.Dock = System.Windows.Forms.DockStyle.Right
        Me.tab_Right.Location = New System.Drawing.Point(508, 103)
        Me.tab_Right.Name = "tab_Right"
        Me.tab_Right.SelectedIndex = 0
        Me.tab_Right.Size = New System.Drawing.Size(76, 164)
        Me.tab_Right.TabIndex = 26
        '
        'split_Left
        '
        Me.split_Left.BackColor = System.Drawing.Color.Firebrick
        Me.split_Left.Location = New System.Drawing.Point(67, 103)
        Me.split_Left.Name = "split_Left"
        Me.split_Left.Size = New System.Drawing.Size(3, 164)
        Me.split_Left.TabIndex = 27
        Me.split_Left.TabStop = False
        '
        'split_Right
        '
        Me.split_Right.BackColor = System.Drawing.Color.Firebrick
        Me.split_Right.Dock = System.Windows.Forms.DockStyle.Right
        Me.split_Right.Location = New System.Drawing.Point(505, 103)
        Me.split_Right.Name = "split_Right"
        Me.split_Right.Size = New System.Drawing.Size(3, 164)
        Me.split_Right.TabIndex = 28
        Me.split_Right.TabStop = False
        '
        'tab_Middle
        '
        Me.tab_Middle.Dock = System.Windows.Forms.DockStyle.Fill
        Me.tab_Middle.Location = New System.Drawing.Point(70, 103)
        Me.tab_Middle.Name = "tab_Middle"
        Me.tab_Middle.SelectedIndex = 0
        Me.tab_Middle.Size = New System.Drawing.Size(435, 164)
        Me.tab_Middle.TabIndex = 29
        '
        'ctrl_Form
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.Controls.Add(Me.tab_Middle)
        Me.Controls.Add(Me.split_Right)
        Me.Controls.Add(Me.split_Left)
        Me.Controls.Add(Me.tab_Right)
        Me.Controls.Add(Me.tab_Left)
        Me.Controls.Add(Me.split_Bottom)
        Me.Controls.Add(Me.tab_Bottom)
        Me.Controls.Add(Me.split_Top)
        Me.Controls.Add(Me.tab_Top)
        Me.Controls.Add(Me.stat_Main)
        Me.Controls.Add(Me.ts_Main)
        Me.Name = "ctrl_Form"
        Me.Size = New System.Drawing.Size(584, 356)
        Me.ts_Main.ResumeLayout(False)
        Me.ts_Main.PerformLayout()
        Me.stat_Main.ResumeLayout(False)
        Me.stat_Main.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents ts_Main As System.Windows.Forms.ToolStrip
    Friend WithEvents ts_Exit As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Design As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Symbols As System.Windows.Forms.ToolStripButton
    Friend WithEvents ToolStripButton1 As System.Windows.Forms.ToolStripButton
    Friend WithEvents stat_Main As System.Windows.Forms.StatusStrip
    Friend WithEvents stat_Main_Stat As System.Windows.Forms.ToolStripStatusLabel
    Friend WithEvents tab_Top As System.Windows.Forms.TabControl
    Friend WithEvents split_Top As System.Windows.Forms.Splitter
    Friend WithEvents tab_Bottom As System.Windows.Forms.TabControl
    Friend WithEvents split_Bottom As System.Windows.Forms.Splitter
    Friend WithEvents tab_Left As System.Windows.Forms.TabControl
    Friend WithEvents tab_Right As System.Windows.Forms.TabControl
    Friend WithEvents split_Left As System.Windows.Forms.Splitter
    Friend WithEvents split_Right As System.Windows.Forms.Splitter
    Friend WithEvents tab_Middle As System.Windows.Forms.TabControl

End Class
