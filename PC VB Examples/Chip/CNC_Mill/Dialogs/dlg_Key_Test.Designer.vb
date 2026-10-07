<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class dlg_Key_Test
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
        Me.txt_Trace = New System.Windows.Forms.TextBox()
        Me.txtz_Output = New System.Windows.Forms.TextBox()
        Me.btn_Clear = New System.Windows.Forms.Button()
        Me.SuspendLayout()
        '
        'txt_Trace
        '
        Me.txt_Trace.Location = New System.Drawing.Point(48, 41)
        Me.txt_Trace.Multiline = True
        Me.txt_Trace.Name = "txt_Trace"
        Me.txt_Trace.Size = New System.Drawing.Size(273, 397)
        Me.txt_Trace.TabIndex = 0
        '
        'txtz_Output
        '
        Me.txtz_Output.Location = New System.Drawing.Point(394, 41)
        Me.txtz_Output.Multiline = True
        Me.txtz_Output.Name = "txtz_Output"
        Me.txtz_Output.Size = New System.Drawing.Size(273, 397)
        Me.txtz_Output.TabIndex = 1
        '
        'btn_Clear
        '
        Me.btn_Clear.Location = New System.Drawing.Point(48, 4)
        Me.btn_Clear.Name = "btn_Clear"
        Me.btn_Clear.Size = New System.Drawing.Size(75, 31)
        Me.btn_Clear.TabIndex = 2
        Me.btn_Clear.Text = "Clear"
        Me.btn_Clear.UseVisualStyleBackColor = True
        '
        'dlg_Key_Test
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(800, 450)
        Me.Controls.Add(Me.btn_Clear)
        Me.Controls.Add(Me.txtz_Output)
        Me.Controls.Add(Me.txt_Trace)
        Me.Name = "dlg_Key_Test"
        Me.Text = "dlg_Key_Test"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Friend WithEvents txt_Trace As TextBox
    Friend WithEvents txtz_Output As TextBox
    Friend WithEvents btn_Clear As Button
End Class
