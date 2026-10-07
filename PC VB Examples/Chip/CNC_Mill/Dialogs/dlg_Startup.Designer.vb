<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class dlg_Startup
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
        Me.btn_OK = New System.Windows.Forms.Button()
        Me.rad_Current = New System.Windows.Forms.RadioButton()
        Me.rad_Home_All = New System.Windows.Forms.RadioButton()
        Me.rad_Home_Z = New System.Windows.Forms.RadioButton()
        Me.SuspendLayout()
        '
        'btn_OK
        '
        Me.btn_OK.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_OK.Location = New System.Drawing.Point(64, 131)
        Me.btn_OK.Name = "btn_OK"
        Me.btn_OK.Size = New System.Drawing.Size(113, 35)
        Me.btn_OK.TabIndex = 0
        Me.btn_OK.Text = "OK"
        Me.btn_OK.UseVisualStyleBackColor = True
        '
        'rad_Current
        '
        Me.rad_Current.AutoSize = True
        Me.rad_Current.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.rad_Current.Location = New System.Drawing.Point(64, 22)
        Me.rad_Current.Name = "rad_Current"
        Me.rad_Current.Size = New System.Drawing.Size(181, 24)
        Me.rad_Current.TabIndex = 1
        Me.rad_Current.TabStop = True
        Me.rad_Current.Text = "Use Current Positions"
        Me.rad_Current.UseVisualStyleBackColor = True
        '
        'rad_Home_All
        '
        Me.rad_Home_All.AutoSize = True
        Me.rad_Home_All.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.rad_Home_All.Location = New System.Drawing.Point(64, 90)
        Me.rad_Home_All.Name = "rad_Home_All"
        Me.rad_Home_All.Size = New System.Drawing.Size(91, 24)
        Me.rad_Home_All.TabIndex = 2
        Me.rad_Home_All.TabStop = True
        Me.rad_Home_All.Text = "Home All"
        Me.rad_Home_All.UseVisualStyleBackColor = True
        '
        'rad_Home_Z
        '
        Me.rad_Home_Z.AutoSize = True
        Me.rad_Home_Z.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.rad_Home_Z.Location = New System.Drawing.Point(64, 56)
        Me.rad_Home_Z.Name = "rad_Home_Z"
        Me.rad_Home_Z.Size = New System.Drawing.Size(84, 24)
        Me.rad_Home_Z.TabIndex = 3
        Me.rad_Home_Z.TabStop = True
        Me.rad_Home_Z.Text = "Home Z"
        Me.rad_Home_Z.UseVisualStyleBackColor = True
        '
        'dlg_Startup
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(284, 202)
        Me.ControlBox = False
        Me.Controls.Add(Me.rad_Home_Z)
        Me.Controls.Add(Me.rad_Home_All)
        Me.Controls.Add(Me.rad_Current)
        Me.Controls.Add(Me.btn_OK)
        Me.Name = "dlg_Startup"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "Startup"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents btn_OK As System.Windows.Forms.Button
    Friend WithEvents rad_Current As System.Windows.Forms.RadioButton
    Friend WithEvents rad_Home_All As System.Windows.Forms.RadioButton
    Friend WithEvents rad_Home_Z As System.Windows.Forms.RadioButton
End Class
