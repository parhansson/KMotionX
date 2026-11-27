<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class dlg_Set_Offfset
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
        Me.btn_Set_Edge_X = New System.Windows.Forms.Button()
        Me.txt_Edge_X = New System.Windows.Forms.TextBox()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.txt_X_Offset = New System.Windows.Forms.TextBox()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.txt_Current_X = New System.Windows.Forms.TextBox()
        Me.btn_Set_Position_X = New System.Windows.Forms.Button()
        Me.btn_Set_Position_Y = New System.Windows.Forms.Button()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.txt_Current_Y = New System.Windows.Forms.TextBox()
        Me.txt_Y_Offset = New System.Windows.Forms.TextBox()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.txt_Edge_Y = New System.Windows.Forms.TextBox()
        Me.btn_Set_Edge_Y = New System.Windows.Forms.Button()
        Me.btn_Set_Position_Z = New System.Windows.Forms.Button()
        Me.Label10 = New System.Windows.Forms.Label()
        Me.txt_Current_Z = New System.Windows.Forms.TextBox()
        Me.txt_Z_Offset = New System.Windows.Forms.TextBox()
        Me.Label11 = New System.Windows.Forms.Label()
        Me.Label12 = New System.Windows.Forms.Label()
        Me.txt_Probe_Z = New System.Windows.Forms.TextBox()
        Me.btn_Set_Edge_Z = New System.Windows.Forms.Button()
        Me.btn_OK = New System.Windows.Forms.Button()
        Me.btn_Cancel = New System.Windows.Forms.Button()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.SuspendLayout()
        '
        'btn_Set_Edge_X
        '
        Me.btn_Set_Edge_X.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Set_Edge_X.Location = New System.Drawing.Point(417, 117)
        Me.btn_Set_Edge_X.Name = "btn_Set_Edge_X"
        Me.btn_Set_Edge_X.Size = New System.Drawing.Size(133, 41)
        Me.btn_Set_Edge_X.TabIndex = 0
        Me.btn_Set_Edge_X.Text = "Set"
        Me.btn_Set_Edge_X.UseVisualStyleBackColor = True
        '
        'txt_Edge_X
        '
        Me.txt_Edge_X.BackColor = System.Drawing.Color.WhiteSmoke
        Me.txt_Edge_X.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Edge_X.Location = New System.Drawing.Point(418, 85)
        Me.txt_Edge_X.Name = "txt_Edge_X"
        Me.txt_Edge_X.ReadOnly = True
        Me.txt_Edge_X.Size = New System.Drawing.Size(130, 26)
        Me.txt_Edge_X.TabIndex = 1
        Me.txt_Edge_X.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label1.Location = New System.Drawing.Point(14, 127)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(101, 20)
        Me.Label1.TabIndex = 2
        Me.Label1.Text = "X Axis Offset"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label2.Location = New System.Drawing.Point(437, 59)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(92, 20)
        Me.Label2.TabIndex = 4
        Me.Label2.Text = "Edge Value"
        '
        'txt_X_Offset
        '
        Me.txt_X_Offset.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_X_Offset.Location = New System.Drawing.Point(131, 124)
        Me.txt_X_Offset.Name = "txt_X_Offset"
        Me.txt_X_Offset.Size = New System.Drawing.Size(130, 26)
        Me.txt_X_Offset.TabIndex = 5
        Me.txt_X_Offset.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label3.Location = New System.Drawing.Point(277, 59)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(122, 20)
        Me.Label3.TabIndex = 7
        Me.Label3.Text = "Current Position"
        '
        'txt_Current_X
        '
        Me.txt_Current_X.BackColor = System.Drawing.Color.WhiteSmoke
        Me.txt_Current_X.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Current_X.Location = New System.Drawing.Point(273, 85)
        Me.txt_Current_X.Name = "txt_Current_X"
        Me.txt_Current_X.ReadOnly = True
        Me.txt_Current_X.Size = New System.Drawing.Size(130, 26)
        Me.txt_Current_X.TabIndex = 6
        Me.txt_Current_X.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'btn_Set_Position_X
        '
        Me.btn_Set_Position_X.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Set_Position_X.Location = New System.Drawing.Point(272, 117)
        Me.btn_Set_Position_X.Name = "btn_Set_Position_X"
        Me.btn_Set_Position_X.Size = New System.Drawing.Size(133, 41)
        Me.btn_Set_Position_X.TabIndex = 8
        Me.btn_Set_Position_X.Text = "Set"
        Me.btn_Set_Position_X.UseVisualStyleBackColor = True
        '
        'btn_Set_Position_Y
        '
        Me.btn_Set_Position_Y.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Set_Position_Y.Location = New System.Drawing.Point(272, 240)
        Me.btn_Set_Position_Y.Name = "btn_Set_Position_Y"
        Me.btn_Set_Position_Y.Size = New System.Drawing.Size(133, 41)
        Me.btn_Set_Position_Y.TabIndex = 19
        Me.btn_Set_Position_Y.Text = "Set"
        Me.btn_Set_Position_Y.UseVisualStyleBackColor = True
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label6.Location = New System.Drawing.Point(277, 182)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(122, 20)
        Me.Label6.TabIndex = 18
        Me.Label6.Text = "Current Position"
        '
        'txt_Current_Y
        '
        Me.txt_Current_Y.BackColor = System.Drawing.Color.WhiteSmoke
        Me.txt_Current_Y.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Current_Y.Location = New System.Drawing.Point(273, 208)
        Me.txt_Current_Y.Name = "txt_Current_Y"
        Me.txt_Current_Y.ReadOnly = True
        Me.txt_Current_Y.Size = New System.Drawing.Size(130, 26)
        Me.txt_Current_Y.TabIndex = 17
        Me.txt_Current_Y.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Y_Offset
        '
        Me.txt_Y_Offset.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Y_Offset.Location = New System.Drawing.Point(131, 247)
        Me.txt_Y_Offset.Name = "txt_Y_Offset"
        Me.txt_Y_Offset.Size = New System.Drawing.Size(130, 26)
        Me.txt_Y_Offset.TabIndex = 16
        Me.txt_Y_Offset.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label7.Location = New System.Drawing.Point(437, 182)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(92, 20)
        Me.Label7.TabIndex = 15
        Me.Label7.Text = "Edge Value"
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label8.Location = New System.Drawing.Point(14, 250)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(101, 20)
        Me.Label8.TabIndex = 14
        Me.Label8.Text = "Y Axis Offset"
        '
        'txt_Edge_Y
        '
        Me.txt_Edge_Y.BackColor = System.Drawing.Color.WhiteSmoke
        Me.txt_Edge_Y.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Edge_Y.Location = New System.Drawing.Point(418, 208)
        Me.txt_Edge_Y.Name = "txt_Edge_Y"
        Me.txt_Edge_Y.ReadOnly = True
        Me.txt_Edge_Y.Size = New System.Drawing.Size(130, 26)
        Me.txt_Edge_Y.TabIndex = 13
        Me.txt_Edge_Y.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'btn_Set_Edge_Y
        '
        Me.btn_Set_Edge_Y.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Set_Edge_Y.Location = New System.Drawing.Point(417, 240)
        Me.btn_Set_Edge_Y.Name = "btn_Set_Edge_Y"
        Me.btn_Set_Edge_Y.Size = New System.Drawing.Size(133, 41)
        Me.btn_Set_Edge_Y.TabIndex = 12
        Me.btn_Set_Edge_Y.Text = "Set"
        Me.btn_Set_Edge_Y.UseVisualStyleBackColor = True
        '
        'btn_Set_Position_Z
        '
        Me.btn_Set_Position_Z.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Set_Position_Z.Location = New System.Drawing.Point(272, 363)
        Me.btn_Set_Position_Z.Name = "btn_Set_Position_Z"
        Me.btn_Set_Position_Z.Size = New System.Drawing.Size(133, 41)
        Me.btn_Set_Position_Z.TabIndex = 30
        Me.btn_Set_Position_Z.Text = "Set"
        Me.btn_Set_Position_Z.UseVisualStyleBackColor = True
        '
        'Label10
        '
        Me.Label10.AutoSize = True
        Me.Label10.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label10.Location = New System.Drawing.Point(277, 305)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(122, 20)
        Me.Label10.TabIndex = 29
        Me.Label10.Text = "Current Position"
        '
        'txt_Current_Z
        '
        Me.txt_Current_Z.BackColor = System.Drawing.Color.WhiteSmoke
        Me.txt_Current_Z.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Current_Z.Location = New System.Drawing.Point(273, 331)
        Me.txt_Current_Z.Name = "txt_Current_Z"
        Me.txt_Current_Z.ReadOnly = True
        Me.txt_Current_Z.Size = New System.Drawing.Size(130, 26)
        Me.txt_Current_Z.TabIndex = 28
        Me.txt_Current_Z.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Z_Offset
        '
        Me.txt_Z_Offset.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Z_Offset.Location = New System.Drawing.Point(131, 370)
        Me.txt_Z_Offset.Name = "txt_Z_Offset"
        Me.txt_Z_Offset.Size = New System.Drawing.Size(130, 26)
        Me.txt_Z_Offset.TabIndex = 27
        Me.txt_Z_Offset.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label11
        '
        Me.Label11.AutoSize = True
        Me.Label11.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label11.Location = New System.Drawing.Point(428, 305)
        Me.Label11.Name = "Label11"
        Me.Label11.Size = New System.Drawing.Size(110, 20)
        Me.Label11.TabIndex = 26
        Me.Label11.Text = "Probe Z Value"
        '
        'Label12
        '
        Me.Label12.AutoSize = True
        Me.Label12.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label12.Location = New System.Drawing.Point(14, 373)
        Me.Label12.Name = "Label12"
        Me.Label12.Size = New System.Drawing.Size(100, 20)
        Me.Label12.TabIndex = 25
        Me.Label12.Text = "Z Axis Offset"
        '
        'txt_Probe_Z
        '
        Me.txt_Probe_Z.BackColor = System.Drawing.Color.WhiteSmoke
        Me.txt_Probe_Z.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Probe_Z.Location = New System.Drawing.Point(418, 331)
        Me.txt_Probe_Z.Name = "txt_Probe_Z"
        Me.txt_Probe_Z.ReadOnly = True
        Me.txt_Probe_Z.Size = New System.Drawing.Size(130, 26)
        Me.txt_Probe_Z.TabIndex = 24
        Me.txt_Probe_Z.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'btn_Set_Edge_Z
        '
        Me.btn_Set_Edge_Z.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Set_Edge_Z.Location = New System.Drawing.Point(417, 363)
        Me.btn_Set_Edge_Z.Name = "btn_Set_Edge_Z"
        Me.btn_Set_Edge_Z.Size = New System.Drawing.Size(133, 41)
        Me.btn_Set_Edge_Z.TabIndex = 23
        Me.btn_Set_Edge_Z.Text = "Set"
        Me.btn_Set_Edge_Z.UseVisualStyleBackColor = True
        '
        'btn_OK
        '
        Me.btn_OK.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_OK.Location = New System.Drawing.Point(114, 445)
        Me.btn_OK.Name = "btn_OK"
        Me.btn_OK.Size = New System.Drawing.Size(151, 63)
        Me.btn_OK.TabIndex = 34
        Me.btn_OK.Text = "OK"
        Me.btn_OK.UseVisualStyleBackColor = True
        '
        'btn_Cancel
        '
        Me.btn_Cancel.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Cancel.Location = New System.Drawing.Point(326, 445)
        Me.btn_Cancel.Name = "btn_Cancel"
        Me.btn_Cancel.Size = New System.Drawing.Size(151, 63)
        Me.btn_Cancel.TabIndex = 35
        Me.btn_Cancel.Text = "Cancel"
        Me.btn_Cancel.UseVisualStyleBackColor = True
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label4.Location = New System.Drawing.Point(21, 9)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(246, 20)
        Me.Label4.TabIndex = 36
        Me.Label4.Text = "Click Set buttons or type in values"
        '
        'dlg_Set_Offfset
        '
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None
        Me.ClientSize = New System.Drawing.Size(559, 543)
        Me.ControlBox = False
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.btn_Cancel)
        Me.Controls.Add(Me.btn_OK)
        Me.Controls.Add(Me.btn_Set_Position_Z)
        Me.Controls.Add(Me.Label10)
        Me.Controls.Add(Me.txt_Current_Z)
        Me.Controls.Add(Me.txt_Z_Offset)
        Me.Controls.Add(Me.Label11)
        Me.Controls.Add(Me.Label12)
        Me.Controls.Add(Me.txt_Probe_Z)
        Me.Controls.Add(Me.btn_Set_Edge_Z)
        Me.Controls.Add(Me.btn_Set_Position_Y)
        Me.Controls.Add(Me.Label6)
        Me.Controls.Add(Me.txt_Current_Y)
        Me.Controls.Add(Me.txt_Y_Offset)
        Me.Controls.Add(Me.Label7)
        Me.Controls.Add(Me.Label8)
        Me.Controls.Add(Me.txt_Edge_Y)
        Me.Controls.Add(Me.btn_Set_Edge_Y)
        Me.Controls.Add(Me.btn_Set_Position_X)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.txt_Current_X)
        Me.Controls.Add(Me.txt_X_Offset)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.txt_Edge_X)
        Me.Controls.Add(Me.btn_Set_Edge_X)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D
        Me.Name = "dlg_Set_Offfset"
        Me.Text = "Set Offfset"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents btn_Set_Edge_X As System.Windows.Forms.Button
    Friend WithEvents txt_Edge_X As System.Windows.Forms.TextBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents txt_X_Offset As System.Windows.Forms.TextBox
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents txt_Current_X As System.Windows.Forms.TextBox
    Friend WithEvents btn_Set_Position_X As System.Windows.Forms.Button
    Friend WithEvents btn_Set_Position_Y As System.Windows.Forms.Button
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents txt_Current_Y As System.Windows.Forms.TextBox
    Friend WithEvents txt_Y_Offset As System.Windows.Forms.TextBox
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents txt_Edge_Y As System.Windows.Forms.TextBox
    Friend WithEvents btn_Set_Edge_Y As System.Windows.Forms.Button
    Friend WithEvents btn_Set_Position_Z As System.Windows.Forms.Button
    Friend WithEvents Label10 As System.Windows.Forms.Label
    Friend WithEvents txt_Current_Z As System.Windows.Forms.TextBox
    Friend WithEvents txt_Z_Offset As System.Windows.Forms.TextBox
    Friend WithEvents Label11 As System.Windows.Forms.Label
    Friend WithEvents Label12 As System.Windows.Forms.Label
    Friend WithEvents txt_Probe_Z As System.Windows.Forms.TextBox
    Friend WithEvents btn_Set_Edge_Z As System.Windows.Forms.Button
    Friend WithEvents btn_OK As System.Windows.Forms.Button
    Friend WithEvents btn_Cancel As System.Windows.Forms.Button
    Friend WithEvents Label4 As System.Windows.Forms.Label
End Class
