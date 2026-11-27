<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class dlg_Color_Picker
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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(dlg_Color_Picker))
        Me.pnl_Colors = New System.Windows.Forms.Panel()
        Me.ts_Main = New System.Windows.Forms.ToolStrip()
        Me.ts_Cancel = New System.Windows.Forms.ToolStripButton()
        Me.ToolStripTextBox1 = New System.Windows.Forms.ToolStripTextBox()
        Me.ts_Main.SuspendLayout()
        Me.SuspendLayout()
        '
        'pnl_Colors
        '
        Me.pnl_Colors.Dock = System.Windows.Forms.DockStyle.Fill
        Me.pnl_Colors.Location = New System.Drawing.Point(0, 38)
        Me.pnl_Colors.Name = "pnl_Colors"
        Me.pnl_Colors.Size = New System.Drawing.Size(452, 453)
        Me.pnl_Colors.TabIndex = 0
        '
        'ts_Main
        '
        Me.ts_Main.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ts_Cancel, Me.ToolStripTextBox1})
        Me.ts_Main.Location = New System.Drawing.Point(0, 0)
        Me.ts_Main.Name = "ts_Main"
        Me.ts_Main.Size = New System.Drawing.Size(452, 38)
        Me.ts_Main.TabIndex = 7
        Me.ts_Main.Text = "ToolStrip1"
        '
        'ts_Cancel
        '
        Me.ts_Cancel.Image = CType(resources.GetObject("ts_Cancel.Image"), System.Drawing.Image)
        Me.ts_Cancel.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Cancel.Name = "ts_Cancel"
        Me.ts_Cancel.Size = New System.Drawing.Size(47, 35)
        Me.ts_Cancel.Text = "Cancel"
        Me.ts_Cancel.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ToolStripTextBox1
        '
        Me.ToolStripTextBox1.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.ToolStripTextBox1.Name = "ToolStripTextBox1"
        Me.ToolStripTextBox1.Size = New System.Drawing.Size(150, 38)
        Me.ToolStripTextBox1.Text = "Click to select color"
        '
        'dlg_Color_Picker
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(452, 491)
        Me.ControlBox = False
        Me.Controls.Add(Me.pnl_Colors)
        Me.Controls.Add(Me.ts_Main)
        Me.Name = "dlg_Color_Picker"
        Me.Text = "Color"
        Me.TopMost = True
        Me.ts_Main.ResumeLayout(False)
        Me.ts_Main.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents pnl_Colors As System.Windows.Forms.Panel
    Friend WithEvents ts_Main As System.Windows.Forms.ToolStrip
    Friend WithEvents ts_Cancel As System.Windows.Forms.ToolStripButton
    Friend WithEvents ToolStripTextBox1 As System.Windows.Forms.ToolStripTextBox
End Class
