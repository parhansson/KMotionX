<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class dlg_Input
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
        Me.txt_Surface = New System.Windows.Forms.TextBox()
        Me.txt_Current_Position = New System.Windows.Forms.TextBox()
        Me.txt_Default = New System.Windows.Forms.TextBox()
        Me.btn_Surface = New System.Windows.Forms.Button()
        Me.lab_Prompt = New System.Windows.Forms.Label()
        Me.btn_Current_Position = New System.Windows.Forms.Button()
        Me.btn_Default = New System.Windows.Forms.Button()
        Me.txt_Value = New System.Windows.Forms.TextBox()
        Me.btn_Cancel = New System.Windows.Forms.Button()
        Me.btn_OK = New System.Windows.Forms.Button()
        Me.txt_Note = New System.Windows.Forms.TextBox()
        Me.SuspendLayout()
        '
        'txt_Surface
        '
        Me.txt_Surface.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Surface.Location = New System.Drawing.Point(305, 102)
        Me.txt_Surface.Name = "txt_Surface"
        Me.txt_Surface.ReadOnly = True
        Me.txt_Surface.Size = New System.Drawing.Size(90, 26)
        Me.txt_Surface.TabIndex = 23
        Me.txt_Surface.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Current_Position
        '
        Me.txt_Current_Position.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Current_Position.Location = New System.Drawing.Point(305, 68)
        Me.txt_Current_Position.Name = "txt_Current_Position"
        Me.txt_Current_Position.ReadOnly = True
        Me.txt_Current_Position.Size = New System.Drawing.Size(90, 26)
        Me.txt_Current_Position.TabIndex = 22
        Me.txt_Current_Position.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Default
        '
        Me.txt_Default.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Default.Location = New System.Drawing.Point(305, 33)
        Me.txt_Default.Name = "txt_Default"
        Me.txt_Default.ReadOnly = True
        Me.txt_Default.Size = New System.Drawing.Size(90, 26)
        Me.txt_Default.TabIndex = 21
        Me.txt_Default.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'btn_Surface
        '
        Me.btn_Surface.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Surface.Location = New System.Drawing.Point(401, 101)
        Me.btn_Surface.Name = "btn_Surface"
        Me.btn_Surface.Size = New System.Drawing.Size(164, 28)
        Me.btn_Surface.TabIndex = 19
        Me.btn_Surface.Text = "Surface"
        Me.btn_Surface.UseVisualStyleBackColor = True
        '
        'lab_Prompt
        '
        Me.lab_Prompt.AutoSize = True
        Me.lab_Prompt.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lab_Prompt.Location = New System.Drawing.Point(8, 7)
        Me.lab_Prompt.Name = "lab_Prompt"
        Me.lab_Prompt.Size = New System.Drawing.Size(284, 20)
        Me.lab_Prompt.TabIndex = 18
        Me.lab_Prompt.Text = "Enter value or click a button to the right"
        '
        'btn_Current_Position
        '
        Me.btn_Current_Position.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Current_Position.Location = New System.Drawing.Point(401, 67)
        Me.btn_Current_Position.Name = "btn_Current_Position"
        Me.btn_Current_Position.Size = New System.Drawing.Size(164, 28)
        Me.btn_Current_Position.TabIndex = 17
        Me.btn_Current_Position.Text = "Current Position"
        Me.btn_Current_Position.UseVisualStyleBackColor = True
        '
        'btn_Default
        '
        Me.btn_Default.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Default.Location = New System.Drawing.Point(401, 31)
        Me.btn_Default.Name = "btn_Default"
        Me.btn_Default.Size = New System.Drawing.Size(164, 30)
        Me.btn_Default.TabIndex = 16
        Me.btn_Default.Text = "Default"
        Me.btn_Default.UseVisualStyleBackColor = True
        '
        'txt_Value
        '
        Me.txt_Value.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Value.Location = New System.Drawing.Point(12, 35)
        Me.txt_Value.Name = "txt_Value"
        Me.txt_Value.Size = New System.Drawing.Size(280, 26)
        Me.txt_Value.TabIndex = 15
        '
        'btn_Cancel
        '
        Me.btn_Cancel.Location = New System.Drawing.Point(306, 234)
        Me.btn_Cancel.Name = "btn_Cancel"
        Me.btn_Cancel.Size = New System.Drawing.Size(119, 52)
        Me.btn_Cancel.TabIndex = 14
        Me.btn_Cancel.Text = "Cancel"
        Me.btn_Cancel.UseVisualStyleBackColor = True
        '
        'btn_OK
        '
        Me.btn_OK.Location = New System.Drawing.Point(143, 234)
        Me.btn_OK.Name = "btn_OK"
        Me.btn_OK.Size = New System.Drawing.Size(119, 52)
        Me.btn_OK.TabIndex = 13
        Me.btn_OK.Text = "OK"
        Me.btn_OK.UseVisualStyleBackColor = True
        '
        'txt_Note
        '
        Me.txt_Note.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Note.Location = New System.Drawing.Point(60, 135)
        Me.txt_Note.Multiline = True
        Me.txt_Note.Name = "txt_Note"
        Me.txt_Note.ReadOnly = True
        Me.txt_Note.Size = New System.Drawing.Size(505, 74)
        Me.txt_Note.TabIndex = 24
        '
        'dlg_Input
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(574, 304)
        Me.ControlBox = False
        Me.Controls.Add(Me.txt_Note)
        Me.Controls.Add(Me.txt_Surface)
        Me.Controls.Add(Me.txt_Current_Position)
        Me.Controls.Add(Me.txt_Default)
        Me.Controls.Add(Me.btn_Surface)
        Me.Controls.Add(Me.lab_Prompt)
        Me.Controls.Add(Me.btn_Current_Position)
        Me.Controls.Add(Me.btn_Default)
        Me.Controls.Add(Me.txt_Value)
        Me.Controls.Add(Me.btn_Cancel)
        Me.Controls.Add(Me.btn_OK)
        Me.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.KeyPreview = True
        Me.Name = "dlg_Input"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "User Input"
        Me.TopMost = True
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents txt_Surface As TextBox
    Friend WithEvents txt_Current_Position As TextBox
    Friend WithEvents txt_Default As TextBox
    Friend WithEvents btn_Surface As Button
    Friend WithEvents lab_Prompt As Label
    Friend WithEvents btn_Current_Position As Button
    Friend WithEvents btn_Default As Button
    Friend WithEvents txt_Value As TextBox
    Friend WithEvents btn_Cancel As Button
    Friend WithEvents btn_OK As Button
    Friend WithEvents txt_Note As TextBox
End Class
