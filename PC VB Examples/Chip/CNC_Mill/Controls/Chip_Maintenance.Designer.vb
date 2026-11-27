<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Chip_Maintenance
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
        Dim DataGridViewCellStyle1 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim DataGridViewCellStyle2 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim DataGridViewCellStyle3 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Me.grid_Maint = New System.Windows.Forms.DataGridView()
        Me.txt_Time_In_Cycle = New System.Windows.Forms.TextBox()
        Me.col_Maint_Task = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.col_Maint_Period = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.col_Maint_Time_Left = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.col_Date = New System.Windows.Forms.DataGridViewTextBoxColumn()
        CType(Me.grid_Maint, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grid_Maint
        '
        Me.grid_Maint.AllowUserToResizeRows = False
        Me.grid_Maint.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.grid_Maint.Columns.AddRange(New System.Windows.Forms.DataGridViewColumn() {Me.col_Maint_Task, Me.col_Maint_Period, Me.col_Maint_Time_Left, Me.col_Date})
        Me.grid_Maint.Dock = System.Windows.Forms.DockStyle.Fill
        Me.grid_Maint.EditMode = System.Windows.Forms.DataGridViewEditMode.EditOnEnter
        Me.grid_Maint.Location = New System.Drawing.Point(0, 24)
        Me.grid_Maint.Name = "grid_Maint"
        Me.grid_Maint.Size = New System.Drawing.Size(951, 362)
        Me.grid_Maint.TabIndex = 0
        '
        'txt_Time_In_Cycle
        '
        Me.txt_Time_In_Cycle.BackColor = System.Drawing.Color.White
        Me.txt_Time_In_Cycle.Dock = System.Windows.Forms.DockStyle.Top
        Me.txt_Time_In_Cycle.Font = New System.Drawing.Font("Microsoft Sans Serif", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Time_In_Cycle.Location = New System.Drawing.Point(0, 0)
        Me.txt_Time_In_Cycle.Name = "txt_Time_In_Cycle"
        Me.txt_Time_In_Cycle.ReadOnly = True
        Me.txt_Time_In_Cycle.Size = New System.Drawing.Size(951, 24)
        Me.txt_Time_In_Cycle.TabIndex = 117
        '
        'col_Maint_Task
        '
        Me.col_Maint_Task.HeaderText = "Task"
        Me.col_Maint_Task.Name = "col_Maint_Task"
        Me.col_Maint_Task.Width = 400
        '
        'col_Maint_Period
        '
        DataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleRight
        Me.col_Maint_Period.DefaultCellStyle = DataGridViewCellStyle1
        Me.col_Maint_Period.HeaderText = "Period"
        Me.col_Maint_Period.Name = "col_Maint_Period"
        '
        'col_Maint_Time_Left
        '
        DataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleRight
        Me.col_Maint_Time_Left.DefaultCellStyle = DataGridViewCellStyle2
        Me.col_Maint_Time_Left.HeaderText = "Time Left"
        Me.col_Maint_Time_Left.Name = "col_Maint_Time_Left"
        '
        'col_Date
        '
        DataGridViewCellStyle3.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleRight
        Me.col_Date.DefaultCellStyle = DataGridViewCellStyle3
        Me.col_Date.HeaderText = "Date Last Done"
        Me.col_Date.Name = "col_Date"
        Me.col_Date.Width = 150
        '
        'Ctrl_Maintenance
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.Controls.Add(Me.grid_Maint)
        Me.Controls.Add(Me.txt_Time_In_Cycle)
        Me.Name = "Ctrl_Maintenance"
        Me.Size = New System.Drawing.Size(951, 386)
        CType(Me.grid_Maint, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents grid_Maint As System.Windows.Forms.DataGridView
    Friend WithEvents txt_Time_In_Cycle As System.Windows.Forms.TextBox
    Friend WithEvents col_Maint_Task As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents col_Maint_Period As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents col_Maint_Time_Left As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents col_Date As System.Windows.Forms.DataGridViewTextBoxColumn

End Class
