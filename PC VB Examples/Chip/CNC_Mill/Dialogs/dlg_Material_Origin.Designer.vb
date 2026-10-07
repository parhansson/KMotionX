<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class dlg_Material_Origin
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
        Me.rad_Top_Left = New System.Windows.Forms.RadioButton()
        Me.rad_Bottom_Left = New System.Windows.Forms.RadioButton()
        Me.rad_Top_Right = New System.Windows.Forms.RadioButton()
        Me.rad_Bottom_Right = New System.Windows.Forms.RadioButton()
        Me.btn_OK = New System.Windows.Forms.Button()
        Me.Panel1 = New System.Windows.Forms.Panel()
        Me.btn_Cancel = New System.Windows.Forms.Button()
        Me.SuspendLayout()
        '
        'rad_Top_Left
        '
        Me.rad_Top_Left.AutoSize = True
        Me.rad_Top_Left.Location = New System.Drawing.Point(44, 27)
        Me.rad_Top_Left.Name = "rad_Top_Left"
        Me.rad_Top_Left.Size = New System.Drawing.Size(65, 17)
        Me.rad_Top_Left.TabIndex = 0
        Me.rad_Top_Left.TabStop = True
        Me.rad_Top_Left.Text = "Top Left"
        Me.rad_Top_Left.UseVisualStyleBackColor = True
        '
        'rad_Bottom_Left
        '
        Me.rad_Bottom_Left.AutoSize = True
        Me.rad_Bottom_Left.Location = New System.Drawing.Point(44, 140)
        Me.rad_Bottom_Left.Name = "rad_Bottom_Left"
        Me.rad_Bottom_Left.Size = New System.Drawing.Size(79, 17)
        Me.rad_Bottom_Left.TabIndex = 1
        Me.rad_Bottom_Left.TabStop = True
        Me.rad_Bottom_Left.Text = "Bottom Left"
        Me.rad_Bottom_Left.UseVisualStyleBackColor = True
        '
        'rad_Top_Right
        '
        Me.rad_Top_Right.AutoSize = True
        Me.rad_Top_Right.Location = New System.Drawing.Point(236, 27)
        Me.rad_Top_Right.Name = "rad_Top_Right"
        Me.rad_Top_Right.Size = New System.Drawing.Size(72, 17)
        Me.rad_Top_Right.TabIndex = 2
        Me.rad_Top_Right.TabStop = True
        Me.rad_Top_Right.Text = "Top Right"
        Me.rad_Top_Right.UseVisualStyleBackColor = True
        '
        'rad_Bottom_Right
        '
        Me.rad_Bottom_Right.AutoSize = True
        Me.rad_Bottom_Right.Location = New System.Drawing.Point(236, 140)
        Me.rad_Bottom_Right.Name = "rad_Bottom_Right"
        Me.rad_Bottom_Right.Size = New System.Drawing.Size(86, 17)
        Me.rad_Bottom_Right.TabIndex = 3
        Me.rad_Bottom_Right.TabStop = True
        Me.rad_Bottom_Right.Text = "Bottom Right"
        Me.rad_Bottom_Right.UseVisualStyleBackColor = True
        '
        'btn_OK
        '
        Me.btn_OK.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_OK.Location = New System.Drawing.Point(22, 196)
        Me.btn_OK.Name = "btn_OK"
        Me.btn_OK.Size = New System.Drawing.Size(136, 37)
        Me.btn_OK.TabIndex = 4
        Me.btn_OK.Text = "OK"
        Me.btn_OK.UseVisualStyleBackColor = True
        '
        'Panel1
        '
        Me.Panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Panel1.Location = New System.Drawing.Point(44, 50)
        Me.Panel1.Name = "Panel1"
        Me.Panel1.Size = New System.Drawing.Size(200, 84)
        Me.Panel1.TabIndex = 5
        '
        'btn_Cancel
        '
        Me.btn_Cancel.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Cancel.Location = New System.Drawing.Point(186, 196)
        Me.btn_Cancel.Name = "btn_Cancel"
        Me.btn_Cancel.Size = New System.Drawing.Size(136, 37)
        Me.btn_Cancel.TabIndex = 6
        Me.btn_Cancel.Text = "Cancel"
        Me.btn_Cancel.UseVisualStyleBackColor = True
        '
        'dlg_Material_Origin
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(364, 263)
        Me.ControlBox = False
        Me.Controls.Add(Me.btn_Cancel)
        Me.Controls.Add(Me.Panel1)
        Me.Controls.Add(Me.btn_OK)
        Me.Controls.Add(Me.rad_Bottom_Right)
        Me.Controls.Add(Me.rad_Top_Right)
        Me.Controls.Add(Me.rad_Bottom_Left)
        Me.Controls.Add(Me.rad_Top_Left)
        Me.Name = "dlg_Material_Origin"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "Set Material Origin"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Friend WithEvents rad_Top_Left As RadioButton
    Friend WithEvents rad_Bottom_Left As RadioButton
    Friend WithEvents rad_Top_Right As RadioButton
    Friend WithEvents rad_Bottom_Right As RadioButton
    Friend WithEvents btn_OK As Button
    Friend WithEvents Panel1 As Panel
    Friend WithEvents btn_Cancel As Button
End Class
