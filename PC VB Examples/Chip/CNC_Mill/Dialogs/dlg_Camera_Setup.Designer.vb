<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class dlg_Camera_Setup
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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(dlg_Camera_Setup))
        Me.ts_Main = New System.Windows.Forms.ToolStrip()
        Me.ts_Exit = New System.Windows.Forms.ToolStripButton()
        Me.ts_Show_Overlay = New System.Windows.Forms.ToolStripButton()
        Me.ts_Up = New System.Windows.Forms.ToolStripButton()
        Me.ts_Down = New System.Windows.Forms.ToolStripButton()
        Me.ts_Size = New System.Windows.Forms.ToolStripButton()
        Me.ts_Transparancy = New System.Windows.Forms.ToolStripButton()
        Me.ts_Color = New System.Windows.Forms.ToolStripButton()
        Me.ts_Help = New System.Windows.Forms.ToolStripButton()
        Me.ts_Speed = New System.Windows.Forms.ToolStripButton()
        Me.ts_Main.SuspendLayout()
        Me.SuspendLayout()
        '
        'ts_Main
        '
        Me.ts_Main.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ts_Exit, Me.ts_Show_Overlay, Me.ts_Up, Me.ts_Down, Me.ts_Size, Me.ts_Speed, Me.ts_Transparancy, Me.ts_Color, Me.ts_Help})
        Me.ts_Main.Location = New System.Drawing.Point(0, 0)
        Me.ts_Main.Name = "ts_Main"
        Me.ts_Main.Size = New System.Drawing.Size(555, 38)
        Me.ts_Main.TabIndex = 1
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
        'ts_Up
        '
        Me.ts_Up.Image = CType(resources.GetObject("ts_Up.Image"), System.Drawing.Image)
        Me.ts_Up.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Up.Name = "ts_Up"
        Me.ts_Up.Size = New System.Drawing.Size(31, 35)
        Me.ts_Up.Text = "Top"
        Me.ts_Up.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Down
        '
        Me.ts_Down.Image = CType(resources.GetObject("ts_Down.Image"), System.Drawing.Image)
        Me.ts_Down.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Down.Name = "ts_Down"
        Me.ts_Down.Size = New System.Drawing.Size(31, 35)
        Me.ts_Down.Text = "Left"
        Me.ts_Down.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Size
        '
        Me.ts_Size.Image = CType(resources.GetObject("ts_Size.Image"), System.Drawing.Image)
        Me.ts_Size.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Size.Name = "ts_Size"
        Me.ts_Size.Size = New System.Drawing.Size(31, 35)
        Me.ts_Size.Text = "Size"
        Me.ts_Size.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Transparancy
        '
        Me.ts_Transparancy.Image = CType(resources.GetObject("ts_Transparancy.Image"), System.Drawing.Image)
        Me.ts_Transparancy.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Transparancy.Name = "ts_Transparancy"
        Me.ts_Transparancy.Size = New System.Drawing.Size(81, 35)
        Me.ts_Transparancy.Text = "Transparancy"
        Me.ts_Transparancy.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Color
        '
        Me.ts_Color.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Color.Name = "ts_Color"
        Me.ts_Color.Size = New System.Drawing.Size(40, 35)
        Me.ts_Color.Text = "Color"
        Me.ts_Color.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Help
        '
        Me.ts_Help.Image = CType(resources.GetObject("ts_Help.Image"), System.Drawing.Image)
        Me.ts_Help.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Help.Name = "ts_Help"
        Me.ts_Help.Size = New System.Drawing.Size(36, 35)
        Me.ts_Help.Text = "Help"
        Me.ts_Help.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
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
        'dlg_Camera_Setup
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(555, 37)
        Me.Controls.Add(Me.ts_Main)
        Me.Name = "dlg_Camera_Setup"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.Manual
        Me.Text = "Setup"
        Me.ts_Main.ResumeLayout(False)
        Me.ts_Main.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents ts_Main As System.Windows.Forms.ToolStrip
    Friend WithEvents ts_Exit As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Show_Overlay As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Up As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Down As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Size As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Transparancy As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Color As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Help As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Speed As System.Windows.Forms.ToolStripButton
End Class
