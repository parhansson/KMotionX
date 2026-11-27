<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Chip_Digital_Input
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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(Chip_Digital_Input))
        Me.led_Chip = New System.Windows.Forms.Button()
        Me.SuspendLayout()
        '
        'led_Chip
        '
        Me.led_Chip.BackgroundImage = CType(resources.GetObject("led_Chip.BackgroundImage"), System.Drawing.Image)
        Me.led_Chip.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.led_Chip.Dock = System.Windows.Forms.DockStyle.Fill
        Me.led_Chip.Location = New System.Drawing.Point(0, 0)
        Me.led_Chip.Name = "led_Chip"
        Me.led_Chip.Size = New System.Drawing.Size(50, 50)
        Me.led_Chip.TabIndex = 2
        Me.led_Chip.Text = "Led"
        Me.led_Chip.UseVisualStyleBackColor = True
        '
        'Chip_Digital_Input
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.Controls.Add(Me.led_Chip)
        Me.Name = "Chip_Digital_Input"
        Me.Size = New System.Drawing.Size(50, 50)
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents led_Chip As Button
End Class
