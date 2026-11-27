<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Chip_Button
    Inherits System.Windows.Forms.UserControl

    'UserControl overrides dispose to clean up the component list.
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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(Chip_Button))
        Me.btn_Chip = New System.Windows.Forms.Button()
        Me.SuspendLayout()
        '
        'btn_Chip
        '
        Me.btn_Chip.BackgroundImage = CType(resources.GetObject("btn_Chip.BackgroundImage"), System.Drawing.Image)
        Me.btn_Chip.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.btn_Chip.Dock = System.Windows.Forms.DockStyle.Fill
        Me.btn_Chip.Location = New System.Drawing.Point(0, 0)
        Me.btn_Chip.Name = "btn_Chip"
        Me.btn_Chip.Size = New System.Drawing.Size(100, 100)
        Me.btn_Chip.TabIndex = 1
        Me.btn_Chip.Text = "Button"
        Me.btn_Chip.UseVisualStyleBackColor = True
        '
        'Chip_Button
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.Controls.Add(Me.btn_Chip)
        Me.Name = "Chip_Button"
        Me.Size = New System.Drawing.Size(100, 100)
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents btn_Chip As Button
End Class
