<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Chip_Trace
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
        Me.pnl_Trace = New System.Windows.Forms.Panel()
        Me.txt_Trace_Log_Limit = New System.Windows.Forms.TextBox()
        Me.chk_Trace_Block = New System.Windows.Forms.CheckBox()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.chk_Trace_Handler = New System.Windows.Forms.CheckBox()
        Me.btn_Clear_Console_Messages = New System.Windows.Forms.Button()
        Me.chk_Trace_Console = New System.Windows.Forms.CheckBox()
        Me.chk_Trace_Interpreter = New System.Windows.Forms.CheckBox()
        Me.chk_Trace_Motion = New System.Windows.Forms.CheckBox()
        Me.txt_Trace_Messages = New System.Windows.Forms.TextBox()
        Me.chk_Enable_Tracing = New System.Windows.Forms.CheckBox()
        Me.pnl_Trace.SuspendLayout()
        Me.SuspendLayout()
        '
        'pnl_Trace
        '
        Me.pnl_Trace.BackColor = System.Drawing.Color.Gainsboro
        Me.pnl_Trace.Controls.Add(Me.chk_Enable_Tracing)
        Me.pnl_Trace.Controls.Add(Me.txt_Trace_Log_Limit)
        Me.pnl_Trace.Controls.Add(Me.chk_Trace_Block)
        Me.pnl_Trace.Controls.Add(Me.Label9)
        Me.pnl_Trace.Controls.Add(Me.chk_Trace_Handler)
        Me.pnl_Trace.Controls.Add(Me.btn_Clear_Console_Messages)
        Me.pnl_Trace.Controls.Add(Me.chk_Trace_Console)
        Me.pnl_Trace.Controls.Add(Me.chk_Trace_Interpreter)
        Me.pnl_Trace.Controls.Add(Me.chk_Trace_Motion)
        Me.pnl_Trace.Dock = System.Windows.Forms.DockStyle.Top
        Me.pnl_Trace.Location = New System.Drawing.Point(0, 0)
        Me.pnl_Trace.Name = "pnl_Trace"
        Me.pnl_Trace.Size = New System.Drawing.Size(1152, 38)
        Me.pnl_Trace.TabIndex = 294
        '
        'txt_Trace_Log_Limit
        '
        Me.txt_Trace_Log_Limit.Location = New System.Drawing.Point(716, 8)
        Me.txt_Trace_Log_Limit.Name = "txt_Trace_Log_Limit"
        Me.txt_Trace_Log_Limit.Size = New System.Drawing.Size(100, 20)
        Me.txt_Trace_Log_Limit.TabIndex = 299
        Me.txt_Trace_Log_Limit.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'chk_Trace_Block
        '
        Me.chk_Trace_Block.AutoSize = True
        Me.chk_Trace_Block.Location = New System.Drawing.Point(472, 10)
        Me.chk_Trace_Block.Name = "chk_Trace_Block"
        Me.chk_Trace_Block.Size = New System.Drawing.Size(53, 17)
        Me.chk_Trace_Block.TabIndex = 298
        Me.chk_Trace_Block.Text = "Block"
        Me.chk_Trace_Block.UseVisualStyleBackColor = True
        '
        'Label9
        '
        Me.Label9.AutoSize = True
        Me.Label9.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label9.Location = New System.Drawing.Point(620, 8)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(82, 20)
        Me.Label9.TabIndex = 296
        Me.Label9.Text = "Log Limit"
        '
        'chk_Trace_Handler
        '
        Me.chk_Trace_Handler.AutoSize = True
        Me.chk_Trace_Handler.Location = New System.Drawing.Point(535, 10)
        Me.chk_Trace_Handler.Name = "chk_Trace_Handler"
        Me.chk_Trace_Handler.Size = New System.Drawing.Size(63, 17)
        Me.chk_Trace_Handler.TabIndex = 294
        Me.chk_Trace_Handler.Text = "Handler"
        Me.chk_Trace_Handler.UseVisualStyleBackColor = True
        '
        'btn_Clear_Console_Messages
        '
        Me.btn_Clear_Console_Messages.Location = New System.Drawing.Point(6, 3)
        Me.btn_Clear_Console_Messages.Name = "btn_Clear_Console_Messages"
        Me.btn_Clear_Console_Messages.Size = New System.Drawing.Size(91, 31)
        Me.btn_Clear_Console_Messages.TabIndex = 289
        Me.btn_Clear_Console_Messages.Text = "Clear"
        Me.btn_Clear_Console_Messages.UseVisualStyleBackColor = True
        '
        'chk_Trace_Console
        '
        Me.chk_Trace_Console.AutoSize = True
        Me.chk_Trace_Console.Location = New System.Drawing.Point(241, 10)
        Me.chk_Trace_Console.Name = "chk_Trace_Console"
        Me.chk_Trace_Console.Size = New System.Drawing.Size(64, 17)
        Me.chk_Trace_Console.TabIndex = 293
        Me.chk_Trace_Console.Text = "Console"
        Me.chk_Trace_Console.UseVisualStyleBackColor = True
        '
        'chk_Trace_Interpreter
        '
        Me.chk_Trace_Interpreter.AutoSize = True
        Me.chk_Trace_Interpreter.Location = New System.Drawing.Point(391, 10)
        Me.chk_Trace_Interpreter.Name = "chk_Trace_Interpreter"
        Me.chk_Trace_Interpreter.Size = New System.Drawing.Size(74, 17)
        Me.chk_Trace_Interpreter.TabIndex = 292
        Me.chk_Trace_Interpreter.Text = "Interpreter"
        Me.chk_Trace_Interpreter.UseVisualStyleBackColor = True
        '
        'chk_Trace_Motion
        '
        Me.chk_Trace_Motion.AutoSize = True
        Me.chk_Trace_Motion.Location = New System.Drawing.Point(324, 10)
        Me.chk_Trace_Motion.Name = "chk_Trace_Motion"
        Me.chk_Trace_Motion.Size = New System.Drawing.Size(58, 17)
        Me.chk_Trace_Motion.TabIndex = 291
        Me.chk_Trace_Motion.Text = "Motion"
        Me.chk_Trace_Motion.UseVisualStyleBackColor = True
        '
        'txt_Trace_Messages
        '
        Me.txt_Trace_Messages.Dock = System.Windows.Forms.DockStyle.Fill
        Me.txt_Trace_Messages.Location = New System.Drawing.Point(0, 38)
        Me.txt_Trace_Messages.Multiline = True
        Me.txt_Trace_Messages.Name = "txt_Trace_Messages"
        Me.txt_Trace_Messages.ScrollBars = System.Windows.Forms.ScrollBars.Both
        Me.txt_Trace_Messages.Size = New System.Drawing.Size(1152, 458)
        Me.txt_Trace_Messages.TabIndex = 295
        Me.txt_Trace_Messages.WordWrap = False
        '
        'chk_Enable_Tracing
        '
        Me.chk_Enable_Tracing.AutoSize = True
        Me.chk_Enable_Tracing.Location = New System.Drawing.Point(112, 10)
        Me.chk_Enable_Tracing.Name = "chk_Enable_Tracing"
        Me.chk_Enable_Tracing.Size = New System.Drawing.Size(98, 17)
        Me.chk_Enable_Tracing.TabIndex = 300
        Me.chk_Enable_Tracing.Text = "Enable Tracing"
        Me.chk_Enable_Tracing.UseVisualStyleBackColor = True
        '
        'Chip_Trace
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.Controls.Add(Me.txt_Trace_Messages)
        Me.Controls.Add(Me.pnl_Trace)
        Me.Name = "Chip_Trace"
        Me.Size = New System.Drawing.Size(1152, 496)
        Me.pnl_Trace.ResumeLayout(False)
        Me.pnl_Trace.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Friend WithEvents pnl_Trace As Panel
    Friend WithEvents txt_Trace_Log_Limit As TextBox
    Friend WithEvents chk_Trace_Block As CheckBox
    Friend WithEvents Label9 As Label
    Friend WithEvents chk_Trace_Handler As CheckBox
    Friend WithEvents btn_Clear_Console_Messages As Button
    Friend WithEvents chk_Trace_Console As CheckBox
    Friend WithEvents chk_Trace_Interpreter As CheckBox
    Friend WithEvents chk_Trace_Motion As CheckBox
    Friend WithEvents txt_Trace_Messages As TextBox
    Friend WithEvents chk_Enable_Tracing As CheckBox
End Class
