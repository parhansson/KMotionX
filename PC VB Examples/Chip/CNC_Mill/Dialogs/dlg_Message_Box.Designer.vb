<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class dlg_Message_Box
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
        Me.btn_3 = New System.Windows.Forms.Button()
        Me.btn_1 = New System.Windows.Forms.Button()
        Me.btn_2 = New System.Windows.Forms.Button()
        Me.txt_Message = New System.Windows.Forms.TextBox()
        Me.SuspendLayout()
        '
        'btn_3
        '
        Me.btn_3.Anchor = System.Windows.Forms.AnchorStyles.Bottom
        Me.btn_3.Location = New System.Drawing.Point(346, 303)
        Me.btn_3.Margin = New System.Windows.Forms.Padding(4)
        Me.btn_3.Name = "btn_3"
        Me.btn_3.Size = New System.Drawing.Size(123, 47)
        Me.btn_3.TabIndex = 7
        Me.btn_3.Text = "3"
        Me.btn_3.UseVisualStyleBackColor = True
        Me.btn_3.Visible = False
        '
        'btn_1
        '
        Me.btn_1.Anchor = System.Windows.Forms.AnchorStyles.Bottom
        Me.btn_1.Location = New System.Drawing.Point(64, 303)
        Me.btn_1.Margin = New System.Windows.Forms.Padding(4)
        Me.btn_1.Name = "btn_1"
        Me.btn_1.Size = New System.Drawing.Size(123, 47)
        Me.btn_1.TabIndex = 6
        Me.btn_1.Text = "1"
        Me.btn_1.UseVisualStyleBackColor = True
        Me.btn_1.Visible = False
        '
        'btn_2
        '
        Me.btn_2.Anchor = System.Windows.Forms.AnchorStyles.Bottom
        Me.btn_2.Location = New System.Drawing.Point(204, 303)
        Me.btn_2.Margin = New System.Windows.Forms.Padding(4)
        Me.btn_2.Name = "btn_2"
        Me.btn_2.Size = New System.Drawing.Size(123, 47)
        Me.btn_2.TabIndex = 5
        Me.btn_2.Text = "2"
        Me.btn_2.UseVisualStyleBackColor = True
        Me.btn_2.Visible = False
        '
        'txt_Message
        '
        Me.txt_Message.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.txt_Message.BackColor = System.Drawing.Color.Gold
        Me.txt_Message.Font = New System.Drawing.Font("Courier New", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Message.Location = New System.Drawing.Point(13, 13)
        Me.txt_Message.Margin = New System.Windows.Forms.Padding(4)
        Me.txt_Message.Multiline = True
        Me.txt_Message.Name = "txt_Message"
        Me.txt_Message.ScrollBars = System.Windows.Forms.ScrollBars.Both
        Me.txt_Message.Size = New System.Drawing.Size(571, 269)
        Me.txt_Message.TabIndex = 4
        Me.txt_Message.TabStop = False
        '
        'dlg_Message_Box
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(606, 372)
        Me.ControlBox = False
        Me.Controls.Add(Me.btn_3)
        Me.Controls.Add(Me.btn_1)
        Me.Controls.Add(Me.btn_2)
        Me.Controls.Add(Me.txt_Message)
        Me.KeyPreview = True
        Me.Name = "dlg_Message_Box"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "Warning"
        Me.TopMost = True
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents btn_3 As System.Windows.Forms.Button
    Friend WithEvents btn_1 As System.Windows.Forms.Button
    Friend WithEvents btn_2 As System.Windows.Forms.Button
    Friend WithEvents txt_Message As System.Windows.Forms.TextBox
End Class
