<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class dlg_Progress
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
        Me.bar = New System.Windows.Forms.ProgressBar()
        Me.txt_Prompt = New System.Windows.Forms.TextBox()
        Me.SuspendLayout()
        '
        'bar
        '
        Me.bar.Location = New System.Drawing.Point(12, 51)
        Me.bar.Name = "bar"
        Me.bar.Size = New System.Drawing.Size(611, 23)
        Me.bar.TabIndex = 0
        '
        'txt_Prompt
        '
        Me.txt_Prompt.BackColor = System.Drawing.Color.Gold
        Me.txt_Prompt.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Prompt.Location = New System.Drawing.Point(12, 12)
        Me.txt_Prompt.Name = "txt_Prompt"
        Me.txt_Prompt.ReadOnly = True
        Me.txt_Prompt.Size = New System.Drawing.Size(611, 26)
        Me.txt_Prompt.TabIndex = 1
        Me.txt_Prompt.TabStop = False
        '
        'dlg_Progress
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(635, 97)
        Me.ControlBox = False
        Me.Controls.Add(Me.txt_Prompt)
        Me.Controls.Add(Me.bar)
        Me.Name = "dlg_Progress"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.Manual
        Me.Text = "Progress"
        Me.TopMost = True
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents bar As System.Windows.Forms.ProgressBar
    Friend WithEvents txt_Prompt As System.Windows.Forms.TextBox
End Class
