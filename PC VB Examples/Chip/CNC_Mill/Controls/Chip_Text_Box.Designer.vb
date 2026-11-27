<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class Chip_Text_Box
    Inherits System.Windows.Forms.UserControl

    'UserControl overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()>
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
    <System.Diagnostics.DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Me.txt_Chip = New System.Windows.Forms.TextBox()
        Me.SuspendLayout()
        '
        'txt_Chip
        '
        Me.txt_Chip.Dock = System.Windows.Forms.DockStyle.Fill
        Me.txt_Chip.Location = New System.Drawing.Point(0, 0)
        Me.txt_Chip.Name = "txt_Chip"
        Me.txt_Chip.Size = New System.Drawing.Size(150, 20)
        Me.txt_Chip.TabIndex = 1
        '
        'Chip_Text_Box
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.Controls.Add(Me.txt_Chip)
        Me.Name = "Chip_Text_Box"
        Me.Size = New System.Drawing.Size(150, 24)
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Friend WithEvents txt_Chip As TextBox
End Class
