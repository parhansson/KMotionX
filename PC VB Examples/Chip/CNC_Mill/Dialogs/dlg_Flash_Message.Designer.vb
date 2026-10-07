<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class dlg_Flash_Message
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
        Me.txt_Message = New System.Windows.Forms.TextBox()
        Me.txt_Focus = New System.Windows.Forms.TextBox()
        Me.SuspendLayout()
        '
        'txt_Message
        '
        Me.txt_Message.BackColor = System.Drawing.Color.Gold
        Me.txt_Message.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Message.Location = New System.Drawing.Point(12, 12)
        Me.txt_Message.Multiline = True
        Me.txt_Message.Name = "txt_Message"
        Me.txt_Message.Size = New System.Drawing.Size(300, 132)
        Me.txt_Message.TabIndex = 0
        Me.txt_Message.Text = "Message"
        '
        'txt_Focus
        '
        Me.txt_Focus.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Focus.Location = New System.Drawing.Point(81, 102)
        Me.txt_Focus.Name = "txt_Focus"
        Me.txt_Focus.Size = New System.Drawing.Size(208, 26)
        Me.txt_Focus.TabIndex = 1
        Me.txt_Focus.Text = "Focus"
        '
        'dlg_Flash_Message
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(334, 156)
        Me.ControlBox = False
        Me.Controls.Add(Me.txt_Focus)
        Me.Controls.Add(Me.txt_Message)
        Me.Name = "dlg_Flash_Message"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.TopMost = True
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents txt_Message As System.Windows.Forms.TextBox
    Friend WithEvents txt_Focus As System.Windows.Forms.TextBox
End Class
