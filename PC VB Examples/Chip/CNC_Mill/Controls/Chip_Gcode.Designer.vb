<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class Chip_Gcode
    Inherits System.Windows.Forms.UserControl

    'UserControl overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()>
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
    <System.Diagnostics.DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Dim DataGridViewCellStyle13 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim DataGridViewCellStyle17 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim DataGridViewCellStyle18 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim DataGridViewCellStyle14 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim DataGridViewCellStyle15 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim DataGridViewCellStyle16 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(Chip_Gcode))
        Me.grid = New System.Windows.Forms.DataGridView()
        Me.Column1 = New System.Windows.Forms.DataGridViewButtonColumn()
        Me.Statuw = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.DataGridViewTextBoxColumn17 = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.DataGridViewTextBoxColumn18 = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.DataGridViewTextBoxColumn19 = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.col_Point_Index = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.ts_G_Code = New System.Windows.Forms.ToolStrip()
        Me.ts_Manual = New System.Windows.Forms.ToolStripButton()
        Me.ts_Auto = New System.Windows.Forms.ToolStripButton()
        Me.ts_Load = New System.Windows.Forms.ToolStripButton()
        Me.ts_Save = New System.Windows.Forms.ToolStripButton()
        Me.ts_Save_As = New System.Windows.Forms.ToolStripButton()
        Me.ts_Continuous = New System.Windows.Forms.ToolStripButton()
        Me.ts_Single_Step = New System.Windows.Forms.ToolStripButton()
        Me.ts_Rewind = New System.Windows.Forms.ToolStripButton()
        Me.ts_Start_Here = New System.Windows.Forms.ToolStripButton()
        Me.ts_Insert = New System.Windows.Forms.ToolStripButton()
        Me.ts_Delete = New System.Windows.Forms.ToolStripButton()
        Me.txt_Information = New System.Windows.Forms.TextBox()
        Me.Stat = New System.Windows.Forms.StatusStrip()
        Me.Stat_Time_Label = New System.Windows.Forms.ToolStripStatusLabel()
        Me.Stat_Time_Progress = New System.Windows.Forms.ToolStripProgressBar()
        Me.Stat_Time = New System.Windows.Forms.ToolStripStatusLabel()
        Me.stat_Last_Block = New System.Windows.Forms.ToolStripStatusLabel()
        Me.stat_Go_To = New System.Windows.Forms.ToolStripStatusLabel()
        Me.StatusStrip1 = New System.Windows.Forms.StatusStrip()
        Me.Stat_Block_Label = New System.Windows.Forms.ToolStripStatusLabel()
        Me.Stat_Block_Progress = New System.Windows.Forms.ToolStripProgressBar()
        Me.Stat_Blocks = New System.Windows.Forms.ToolStripStatusLabel()
        Me.stat_lab_Last_Block = New System.Windows.Forms.ToolStripStatusLabel()
        Me.split = New System.Windows.Forms.Splitter()
        CType(Me.grid, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.ts_G_Code.SuspendLayout()
        Me.Stat.SuspendLayout()
        Me.StatusStrip1.SuspendLayout()
        Me.SuspendLayout()
        '
        'grid
        '
        DataGridViewCellStyle13.BackColor = System.Drawing.Color.White
        Me.grid.AlternatingRowsDefaultCellStyle = DataGridViewCellStyle13
        Me.grid.ColumnHeadersHeight = 30
        Me.grid.Columns.AddRange(New System.Windows.Forms.DataGridViewColumn() {Me.Column1, Me.Statuw, Me.DataGridViewTextBoxColumn17, Me.DataGridViewTextBoxColumn18, Me.DataGridViewTextBoxColumn19, Me.col_Point_Index})
        DataGridViewCellStyle17.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft
        DataGridViewCellStyle17.BackColor = System.Drawing.SystemColors.Window
        DataGridViewCellStyle17.Font = New System.Drawing.Font("Courier New", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        DataGridViewCellStyle17.ForeColor = System.Drawing.SystemColors.ControlText
        DataGridViewCellStyle17.SelectionBackColor = System.Drawing.SystemColors.Highlight
        DataGridViewCellStyle17.SelectionForeColor = System.Drawing.SystemColors.HighlightText
        DataGridViewCellStyle17.WrapMode = System.Windows.Forms.DataGridViewTriState.[False]
        Me.grid.DefaultCellStyle = DataGridViewCellStyle17
        Me.grid.Dock = System.Windows.Forms.DockStyle.Fill
        Me.grid.EditMode = System.Windows.Forms.DataGridViewEditMode.EditOnEnter
        Me.grid.Location = New System.Drawing.Point(0, 123)
        Me.grid.Name = "grid"
        DataGridViewCellStyle18.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft
        DataGridViewCellStyle18.BackColor = System.Drawing.SystemColors.Control
        DataGridViewCellStyle18.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        DataGridViewCellStyle18.ForeColor = System.Drawing.SystemColors.WindowText
        DataGridViewCellStyle18.Padding = New System.Windows.Forms.Padding(0, 0, 0, 10)
        DataGridViewCellStyle18.SelectionBackColor = System.Drawing.SystemColors.Highlight
        DataGridViewCellStyle18.SelectionForeColor = System.Drawing.SystemColors.HighlightText
        DataGridViewCellStyle18.WrapMode = System.Windows.Forms.DataGridViewTriState.[True]
        Me.grid.RowHeadersDefaultCellStyle = DataGridViewCellStyle18
        Me.grid.Size = New System.Drawing.Size(708, 310)
        Me.grid.TabIndex = 114
        '
        'Column1
        '
        Me.Column1.HeaderText = ""
        Me.Column1.Name = "Column1"
        Me.Column1.Text = ""
        Me.Column1.Width = 25
        '
        'Statuw
        '
        Me.Statuw.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.None
        DataGridViewCellStyle14.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter
        Me.Statuw.DefaultCellStyle = DataGridViewCellStyle14
        Me.Statuw.HeaderText = ""
        Me.Statuw.Name = "Statuw"
        Me.Statuw.ReadOnly = True
        Me.Statuw.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable
        Me.Statuw.Width = 25
        '
        'DataGridViewTextBoxColumn17
        '
        Me.DataGridViewTextBoxColumn17.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.None
        DataGridViewCellStyle15.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft
        Me.DataGridViewTextBoxColumn17.DefaultCellStyle = DataGridViewCellStyle15
        Me.DataGridViewTextBoxColumn17.HeaderText = "Line"
        Me.DataGridViewTextBoxColumn17.Name = "DataGridViewTextBoxColumn17"
        Me.DataGridViewTextBoxColumn17.ReadOnly = True
        Me.DataGridViewTextBoxColumn17.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable
        Me.DataGridViewTextBoxColumn17.Width = 75
        '
        'DataGridViewTextBoxColumn18
        '
        Me.DataGridViewTextBoxColumn18.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.None
        DataGridViewCellStyle16.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft
        Me.DataGridViewTextBoxColumn18.DefaultCellStyle = DataGridViewCellStyle16
        Me.DataGridViewTextBoxColumn18.HeaderText = "Block"
        Me.DataGridViewTextBoxColumn18.Name = "DataGridViewTextBoxColumn18"
        Me.DataGridViewTextBoxColumn18.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable
        Me.DataGridViewTextBoxColumn18.Width = 350
        '
        'DataGridViewTextBoxColumn19
        '
        Me.DataGridViewTextBoxColumn19.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.None
        Me.DataGridViewTextBoxColumn19.HeaderText = "Comment"
        Me.DataGridViewTextBoxColumn19.Name = "DataGridViewTextBoxColumn19"
        Me.DataGridViewTextBoxColumn19.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable
        Me.DataGridViewTextBoxColumn19.Visible = False
        Me.DataGridViewTextBoxColumn19.Width = 24
        '
        'col_Point_Index
        '
        Me.col_Point_Index.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.None
        Me.col_Point_Index.HeaderText = "Point"
        Me.col_Point_Index.Name = "col_Point_Index"
        Me.col_Point_Index.ReadOnly = True
        Me.col_Point_Index.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable
        Me.col_Point_Index.Visible = False
        '
        'ts_G_Code
        '
        Me.ts_G_Code.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ts_Manual, Me.ts_Auto, Me.ts_Load, Me.ts_Save, Me.ts_Save_As, Me.ts_Continuous, Me.ts_Single_Step, Me.ts_Rewind, Me.ts_Start_Here, Me.ts_Insert, Me.ts_Delete})
        Me.ts_G_Code.LayoutStyle = System.Windows.Forms.ToolStripLayoutStyle.HorizontalStackWithOverflow
        Me.ts_G_Code.Location = New System.Drawing.Point(0, 85)
        Me.ts_G_Code.Name = "ts_G_Code"
        Me.ts_G_Code.Size = New System.Drawing.Size(708, 38)
        Me.ts_G_Code.TabIndex = 115
        Me.ts_G_Code.Text = "ts_Enable"
        '
        'ts_Manual
        '
        Me.ts_Manual.Image = CType(resources.GetObject("ts_Manual.Image"), System.Drawing.Image)
        Me.ts_Manual.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Manual.Name = "ts_Manual"
        Me.ts_Manual.Size = New System.Drawing.Size(51, 35)
        Me.ts_Manual.Text = "Manual"
        Me.ts_Manual.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Auto
        '
        Me.ts_Auto.BackColor = System.Drawing.Color.FromArgb(CType(CType(255, Byte), Integer), CType(CType(128, Byte), Integer), CType(CType(0, Byte), Integer))
        Me.ts_Auto.Image = CType(resources.GetObject("ts_Auto.Image"), System.Drawing.Image)
        Me.ts_Auto.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Auto.Name = "ts_Auto"
        Me.ts_Auto.Size = New System.Drawing.Size(37, 35)
        Me.ts_Auto.Text = "Auto"
        Me.ts_Auto.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Load
        '
        Me.ts_Load.Image = CType(resources.GetObject("ts_Load.Image"), System.Drawing.Image)
        Me.ts_Load.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Load.Name = "ts_Load"
        Me.ts_Load.Size = New System.Drawing.Size(37, 35)
        Me.ts_Load.Text = "Load"
        Me.ts_Load.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Save
        '
        Me.ts_Save.Image = CType(resources.GetObject("ts_Save.Image"), System.Drawing.Image)
        Me.ts_Save.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Save.Name = "ts_Save"
        Me.ts_Save.Size = New System.Drawing.Size(35, 35)
        Me.ts_Save.Text = "Save"
        Me.ts_Save.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Save_As
        '
        Me.ts_Save_As.Image = CType(resources.GetObject("ts_Save_As.Image"), System.Drawing.Image)
        Me.ts_Save_As.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Save_As.Name = "ts_Save_As"
        Me.ts_Save_As.Size = New System.Drawing.Size(51, 35)
        Me.ts_Save_As.Text = "Save As"
        Me.ts_Save_As.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Continuous
        '
        Me.ts_Continuous.Image = CType(resources.GetObject("ts_Continuous.Image"), System.Drawing.Image)
        Me.ts_Continuous.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Continuous.Name = "ts_Continuous"
        Me.ts_Continuous.Size = New System.Drawing.Size(73, 35)
        Me.ts_Continuous.Text = "Continuous"
        Me.ts_Continuous.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Single_Step
        '
        Me.ts_Single_Step.Image = CType(resources.GetObject("ts_Single_Step.Image"), System.Drawing.Image)
        Me.ts_Single_Step.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Single_Step.Name = "ts_Single_Step"
        Me.ts_Single_Step.Size = New System.Drawing.Size(69, 35)
        Me.ts_Single_Step.Text = "Single Step"
        Me.ts_Single_Step.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Rewind
        '
        Me.ts_Rewind.Image = CType(resources.GetObject("ts_Rewind.Image"), System.Drawing.Image)
        Me.ts_Rewind.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Rewind.Name = "ts_Rewind"
        Me.ts_Rewind.Size = New System.Drawing.Size(50, 35)
        Me.ts_Rewind.Text = "Rewind"
        Me.ts_Rewind.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Start_Here
        '
        Me.ts_Start_Here.Image = CType(resources.GetObject("ts_Start_Here.Image"), System.Drawing.Image)
        Me.ts_Start_Here.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Start_Here.Name = "ts_Start_Here"
        Me.ts_Start_Here.Size = New System.Drawing.Size(63, 35)
        Me.ts_Start_Here.Text = "Start Here"
        Me.ts_Start_Here.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Insert
        '
        Me.ts_Insert.Image = CType(resources.GetObject("ts_Insert.Image"), System.Drawing.Image)
        Me.ts_Insert.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Insert.Name = "ts_Insert"
        Me.ts_Insert.Size = New System.Drawing.Size(40, 35)
        Me.ts_Insert.Text = "Insert"
        Me.ts_Insert.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Delete
        '
        Me.ts_Delete.Image = CType(resources.GetObject("ts_Delete.Image"), System.Drawing.Image)
        Me.ts_Delete.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Delete.Name = "ts_Delete"
        Me.ts_Delete.Size = New System.Drawing.Size(44, 35)
        Me.ts_Delete.Text = "Delete"
        Me.ts_Delete.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'txt_Information
        '
        Me.txt_Information.BackColor = System.Drawing.Color.White
        Me.txt_Information.Dock = System.Windows.Forms.DockStyle.Top
        Me.txt_Information.Font = New System.Drawing.Font("Microsoft Sans Serif", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Information.Location = New System.Drawing.Point(0, 0)
        Me.txt_Information.Multiline = True
        Me.txt_Information.Name = "txt_Information"
        Me.txt_Information.ReadOnly = True
        Me.txt_Information.ScrollBars = System.Windows.Forms.ScrollBars.Both
        Me.txt_Information.Size = New System.Drawing.Size(708, 84)
        Me.txt_Information.TabIndex = 116
        '
        'Stat
        '
        Me.Stat.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.Stat_Time_Label, Me.Stat_Time_Progress, Me.Stat_Time, Me.stat_Last_Block, Me.stat_Go_To})
        Me.Stat.Location = New System.Drawing.Point(0, 459)
        Me.Stat.Name = "Stat"
        Me.Stat.Size = New System.Drawing.Size(708, 26)
        Me.Stat.TabIndex = 119
        '
        'Stat_Time_Label
        '
        Me.Stat_Time_Label.Font = New System.Drawing.Font("Segoe UI", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Stat_Time_Label.Name = "Stat_Time_Label"
        Me.Stat_Time_Label.Size = New System.Drawing.Size(48, 21)
        Me.Stat_Time_Label.Text = "Time "
        '
        'Stat_Time_Progress
        '
        Me.Stat_Time_Progress.Name = "Stat_Time_Progress"
        Me.Stat_Time_Progress.Size = New System.Drawing.Size(100, 20)
        '
        'Stat_Time
        '
        Me.Stat_Time.Font = New System.Drawing.Font("Segoe UI", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Stat_Time.Name = "Stat_Time"
        Me.Stat_Time.Size = New System.Drawing.Size(83, 21)
        Me.Stat_Time.Text = "0 of 0 : 0%"
        '
        'stat_Last_Block
        '
        Me.stat_Last_Block.AutoSize = False
        Me.stat_Last_Block.Font = New System.Drawing.Font("Segoe UI", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.stat_Last_Block.Name = "stat_Last_Block"
        Me.stat_Last_Block.Size = New System.Drawing.Size(50, 21)
        Me.stat_Last_Block.Text = "0"
        '
        'stat_Go_To
        '
        Me.stat_Go_To.Font = New System.Drawing.Font("Segoe UI", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.stat_Go_To.Name = "stat_Go_To"
        Me.stat_Go_To.Size = New System.Drawing.Size(80, 21)
        Me.stat_Go_To.Text = "<-- Go To"
        '
        'StatusStrip1
        '
        Me.StatusStrip1.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.Stat_Block_Label, Me.Stat_Block_Progress, Me.Stat_Blocks, Me.stat_lab_Last_Block})
        Me.StatusStrip1.Location = New System.Drawing.Point(0, 433)
        Me.StatusStrip1.Name = "StatusStrip1"
        Me.StatusStrip1.Size = New System.Drawing.Size(708, 26)
        Me.StatusStrip1.TabIndex = 120
        Me.StatusStrip1.Text = "StatusStrip1"
        '
        'Stat_Block_Label
        '
        Me.Stat_Block_Label.Font = New System.Drawing.Font("Segoe UI", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Stat_Block_Label.Name = "Stat_Block_Label"
        Me.Stat_Block_Label.Size = New System.Drawing.Size(47, 21)
        Me.Stat_Block_Label.Text = "Block"
        '
        'Stat_Block_Progress
        '
        Me.Stat_Block_Progress.Name = "Stat_Block_Progress"
        Me.Stat_Block_Progress.Size = New System.Drawing.Size(100, 20)
        '
        'Stat_Blocks
        '
        Me.Stat_Blocks.Font = New System.Drawing.Font("Segoe UI", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Stat_Blocks.Name = "Stat_Blocks"
        Me.Stat_Blocks.Size = New System.Drawing.Size(83, 21)
        Me.Stat_Blocks.Text = "0 of 0 : 0%"
        '
        'stat_lab_Last_Block
        '
        Me.stat_lab_Last_Block.Font = New System.Drawing.Font("Segoe UI", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.stat_lab_Last_Block.Name = "stat_lab_Last_Block"
        Me.stat_lab_Last_Block.Size = New System.Drawing.Size(144, 21)
        Me.stat_lab_Last_Block.Text = "Last Block Executed"
        '
        'split
        '
        Me.split.BackColor = System.Drawing.Color.Firebrick
        Me.split.Dock = System.Windows.Forms.DockStyle.Top
        Me.split.Location = New System.Drawing.Point(0, 84)
        Me.split.Name = "split"
        Me.split.Size = New System.Drawing.Size(708, 1)
        Me.split.TabIndex = 121
        Me.split.TabStop = False
        '
        'Chip_Gcode
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink
        Me.Controls.Add(Me.grid)
        Me.Controls.Add(Me.StatusStrip1)
        Me.Controls.Add(Me.Stat)
        Me.Controls.Add(Me.ts_G_Code)
        Me.Controls.Add(Me.split)
        Me.Controls.Add(Me.txt_Information)
        Me.Name = "Chip_Gcode"
        Me.Size = New System.Drawing.Size(708, 485)
        CType(Me.grid, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ts_G_Code.ResumeLayout(False)
        Me.ts_G_Code.PerformLayout()
        Me.Stat.ResumeLayout(False)
        Me.Stat.PerformLayout()
        Me.StatusStrip1.ResumeLayout(False)
        Me.StatusStrip1.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents grid As System.Windows.Forms.DataGridView
    Friend WithEvents ts_G_Code As System.Windows.Forms.ToolStrip
    Friend WithEvents ts_Load As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Save As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Save_As As System.Windows.Forms.ToolStripButton
    Friend WithEvents txt_Information As System.Windows.Forms.TextBox
    Friend WithEvents ts_Manual As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Auto As System.Windows.Forms.ToolStripButton
    Friend WithEvents Stat As System.Windows.Forms.StatusStrip
    Friend WithEvents Stat_Time_Label As System.Windows.Forms.ToolStripStatusLabel
    Friend WithEvents Stat_Time As System.Windows.Forms.ToolStripStatusLabel
    Friend WithEvents Stat_Time_Progress As System.Windows.Forms.ToolStripProgressBar
    Friend WithEvents StatusStrip1 As System.Windows.Forms.StatusStrip
    Friend WithEvents Stat_Block_Label As System.Windows.Forms.ToolStripStatusLabel
    Friend WithEvents Stat_Blocks As System.Windows.Forms.ToolStripStatusLabel
    Friend WithEvents Stat_Block_Progress As System.Windows.Forms.ToolStripProgressBar
    Friend WithEvents Column1 As DataGridViewButtonColumn
    Friend WithEvents Statuw As DataGridViewTextBoxColumn
    Friend WithEvents DataGridViewTextBoxColumn17 As DataGridViewTextBoxColumn
    Friend WithEvents DataGridViewTextBoxColumn18 As DataGridViewTextBoxColumn
    Friend WithEvents DataGridViewTextBoxColumn19 As DataGridViewTextBoxColumn
    Friend WithEvents col_Point_Index As DataGridViewTextBoxColumn
    Friend WithEvents ts_Continuous As ToolStripButton
    Friend WithEvents ts_Single_Step As ToolStripButton
    Friend WithEvents ts_Rewind As ToolStripButton
    Friend WithEvents ts_Start_Here As ToolStripButton
    Friend WithEvents ts_Insert As ToolStripButton
    Friend WithEvents ts_Delete As ToolStripButton
    Friend WithEvents stat_lab_Last_Block As ToolStripStatusLabel
    Friend WithEvents stat_Last_Block As ToolStripStatusLabel
    Friend WithEvents stat_Go_To As ToolStripStatusLabel
    Friend WithEvents split As Splitter
End Class
