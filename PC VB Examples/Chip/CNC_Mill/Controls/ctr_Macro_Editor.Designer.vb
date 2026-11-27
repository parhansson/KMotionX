<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class ctr_Macro_Editor
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
        Dim DataGridViewCellStyle2 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim DataGridViewCellStyle1 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(ctr_Macro_Editor))
        Me.grid_Macro = New System.Windows.Forms.DataGridView()
        Me.col_G_Code_Line_No = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.col_G_Code = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.col_G_Code_Comment = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.col_G_Code_Error = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.ts_Macro = New System.Windows.Forms.ToolStrip()
        Me.ts_Open = New System.Windows.Forms.ToolStripButton()
        Me.ts_Save = New System.Windows.Forms.ToolStripButton()
        Me.ts_Run = New System.Windows.Forms.ToolStripButton()
        Me.ts_Step = New System.Windows.Forms.ToolStripButton()
        Me.ts_Start_Here = New System.Windows.Forms.ToolStripButton()
        Me.ts_Insert = New System.Windows.Forms.ToolStripDropDownButton()
        Me.ts_Insert_Expression = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Insert_IF = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Insert_ELSE = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Insert_FOR = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Insert_WHILE = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Insert_DO = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Insert_GOTO = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Insert_LABEL = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Insert_CALL = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Insert_INTEGER_Variable = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Insert_FLOAT_Variable = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Insert_STRING_Variable = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Insert_INTEGER_List = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Insert_FLOAT_List = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Insert_STRING_List = New System.Windows.Forms.ToolStripMenuItem()
        Me.GlobalVariableToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.BREAKToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Delete = New System.Windows.Forms.ToolStripButton()
        Me.ts_Move_Up = New System.Windows.Forms.ToolStripButton()
        Me.ts_Move_Down = New System.Windows.Forms.ToolStripButton()
        Me.pnl_Editor = New System.Windows.Forms.Panel()
        Me.split_Left = New System.Windows.Forms.Splitter()
        Me.ts_Help = New System.Windows.Forms.ToolStrip()
        Me.ToolStripLabel1 = New System.Windows.Forms.ToolStripLabel()
        Me.ts_Macros = New System.Windows.Forms.ToolStripButton()
        Me.ts_Syntax = New System.Windows.Forms.ToolStripButton()
        Me.ts_Errors = New System.Windows.Forms.ToolStripButton()
        Me.ts_G_Codes = New System.Windows.Forms.ToolStripButton()
        Me.ts_TinyG = New System.Windows.Forms.ToolStripButton()
        Me.txt_Help = New System.Windows.Forms.TextBox()
        Me.tab_Macro = New System.Windows.Forms.TabControl()
        Me.tab_Errors = New System.Windows.Forms.TabPage()
        Me.lst_Errors = New System.Windows.Forms.ListBox()
        Me.tab_Help = New System.Windows.Forms.TabPage()
        Me.pnl_Errors = New System.Windows.Forms.Panel()
        CType(Me.grid_Macro, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.ts_Macro.SuspendLayout()
        Me.pnl_Editor.SuspendLayout()
        Me.ts_Help.SuspendLayout()
        Me.tab_Macro.SuspendLayout()
        Me.tab_Errors.SuspendLayout()
        Me.tab_Help.SuspendLayout()
        Me.pnl_Errors.SuspendLayout()
        Me.SuspendLayout()
        '
        'grid_Macro
        '
        Me.grid_Macro.AllowUserToAddRows = False
        Me.grid_Macro.AllowUserToDeleteRows = False
        Me.grid_Macro.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.grid_Macro.Columns.AddRange(New System.Windows.Forms.DataGridViewColumn() {Me.col_G_Code_Line_No, Me.col_G_Code, Me.col_G_Code_Comment, Me.col_G_Code_Error})
        DataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft
        DataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Window
        DataGridViewCellStyle2.Font = New System.Drawing.Font("Courier New", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        DataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.ControlText
        DataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight
        DataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.HighlightText
        DataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.[False]
        Me.grid_Macro.DefaultCellStyle = DataGridViewCellStyle2
        Me.grid_Macro.Dock = System.Windows.Forms.DockStyle.Fill
        Me.grid_Macro.EditMode = System.Windows.Forms.DataGridViewEditMode.EditOnEnter
        Me.grid_Macro.Location = New System.Drawing.Point(0, 38)
        Me.grid_Macro.Name = "grid_Macro"
        Me.grid_Macro.Size = New System.Drawing.Size(456, 434)
        Me.grid_Macro.TabIndex = 107
        '
        'col_G_Code_Line_No
        '
        Me.col_G_Code_Line_No.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells
        DataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleRight
        Me.col_G_Code_Line_No.DefaultCellStyle = DataGridViewCellStyle1
        Me.col_G_Code_Line_No.HeaderText = "Line"
        Me.col_G_Code_Line_No.Name = "col_G_Code_Line_No"
        Me.col_G_Code_Line_No.ReadOnly = True
        Me.col_G_Code_Line_No.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable
        Me.col_G_Code_Line_No.Width = 33
        '
        'col_G_Code
        '
        Me.col_G_Code.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells
        Me.col_G_Code.HeaderText = "Block"
        Me.col_G_Code.Name = "col_G_Code"
        Me.col_G_Code.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable
        Me.col_G_Code.Width = 40
        '
        'col_G_Code_Comment
        '
        Me.col_G_Code_Comment.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells
        Me.col_G_Code_Comment.HeaderText = "Comment"
        Me.col_G_Code_Comment.Name = "col_G_Code_Comment"
        Me.col_G_Code_Comment.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable
        Me.col_G_Code_Comment.Width = 57
        '
        'col_G_Code_Error
        '
        Me.col_G_Code_Error.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells
        Me.col_G_Code_Error.HeaderText = "Error"
        Me.col_G_Code_Error.Name = "col_G_Code_Error"
        Me.col_G_Code_Error.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable
        Me.col_G_Code_Error.Width = 35
        '
        'ts_Macro
        '
        Me.ts_Macro.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ts_Open, Me.ts_Save, Me.ts_Run, Me.ts_Step, Me.ts_Start_Here, Me.ts_Insert, Me.ts_Delete, Me.ts_Move_Up, Me.ts_Move_Down})
        Me.ts_Macro.Location = New System.Drawing.Point(0, 0)
        Me.ts_Macro.Name = "ts_Macro"
        Me.ts_Macro.Size = New System.Drawing.Size(456, 38)
        Me.ts_Macro.TabIndex = 108
        '
        'ts_Open
        '
        Me.ts_Open.Image = CType(resources.GetObject("ts_Open.Image"), System.Drawing.Image)
        Me.ts_Open.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Open.Name = "ts_Open"
        Me.ts_Open.Size = New System.Drawing.Size(40, 35)
        Me.ts_Open.Text = "Open"
        Me.ts_Open.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
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
        'ts_Run
        '
        Me.ts_Run.Image = CType(resources.GetObject("ts_Run.Image"), System.Drawing.Image)
        Me.ts_Run.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Run.Name = "ts_Run"
        Me.ts_Run.Size = New System.Drawing.Size(32, 35)
        Me.ts_Run.Text = "Run"
        Me.ts_Run.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Step
        '
        Me.ts_Step.Image = CType(resources.GetObject("ts_Step.Image"), System.Drawing.Image)
        Me.ts_Step.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Step.Name = "ts_Step"
        Me.ts_Step.Size = New System.Drawing.Size(34, 35)
        Me.ts_Step.Text = "Step"
        Me.ts_Step.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
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
        Me.ts_Insert.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ts_Insert_Expression, Me.ts_Insert_IF, Me.ts_Insert_ELSE, Me.ts_Insert_FOR, Me.ts_Insert_WHILE, Me.ts_Insert_DO, Me.ts_Insert_GOTO, Me.ts_Insert_LABEL, Me.ts_Insert_CALL, Me.ts_Insert_INTEGER_Variable, Me.ts_Insert_FLOAT_Variable, Me.ts_Insert_STRING_Variable, Me.ts_Insert_INTEGER_List, Me.ts_Insert_FLOAT_List, Me.ts_Insert_STRING_List, Me.GlobalVariableToolStripMenuItem, Me.BREAKToolStripMenuItem})
        Me.ts_Insert.Image = CType(resources.GetObject("ts_Insert.Image"), System.Drawing.Image)
        Me.ts_Insert.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Insert.Name = "ts_Insert"
        Me.ts_Insert.Size = New System.Drawing.Size(49, 35)
        Me.ts_Insert.Text = "Insert"
        Me.ts_Insert.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Insert_Expression
        '
        Me.ts_Insert_Expression.Name = "ts_Insert_Expression"
        Me.ts_Insert_Expression.Size = New System.Drawing.Size(195, 22)
        Me.ts_Insert_Expression.Text = "Expression"
        '
        'ts_Insert_IF
        '
        Me.ts_Insert_IF.Name = "ts_Insert_IF"
        Me.ts_Insert_IF.Size = New System.Drawing.Size(195, 22)
        Me.ts_Insert_IF.Text = "IF"
        '
        'ts_Insert_ELSE
        '
        Me.ts_Insert_ELSE.Name = "ts_Insert_ELSE"
        Me.ts_Insert_ELSE.Size = New System.Drawing.Size(195, 22)
        Me.ts_Insert_ELSE.Text = "ELSE"
        '
        'ts_Insert_FOR
        '
        Me.ts_Insert_FOR.Name = "ts_Insert_FOR"
        Me.ts_Insert_FOR.Size = New System.Drawing.Size(195, 22)
        Me.ts_Insert_FOR.Text = "FOR"
        '
        'ts_Insert_WHILE
        '
        Me.ts_Insert_WHILE.Name = "ts_Insert_WHILE"
        Me.ts_Insert_WHILE.Size = New System.Drawing.Size(195, 22)
        Me.ts_Insert_WHILE.Text = "WHILE"
        '
        'ts_Insert_DO
        '
        Me.ts_Insert_DO.Name = "ts_Insert_DO"
        Me.ts_Insert_DO.Size = New System.Drawing.Size(195, 22)
        Me.ts_Insert_DO.Text = "DO"
        '
        'ts_Insert_GOTO
        '
        Me.ts_Insert_GOTO.Name = "ts_Insert_GOTO"
        Me.ts_Insert_GOTO.Size = New System.Drawing.Size(195, 22)
        Me.ts_Insert_GOTO.Text = "GOTO"
        '
        'ts_Insert_LABEL
        '
        Me.ts_Insert_LABEL.Name = "ts_Insert_LABEL"
        Me.ts_Insert_LABEL.Size = New System.Drawing.Size(195, 22)
        Me.ts_Insert_LABEL.Text = "LABEL"
        '
        'ts_Insert_CALL
        '
        Me.ts_Insert_CALL.Name = "ts_Insert_CALL"
        Me.ts_Insert_CALL.Size = New System.Drawing.Size(195, 22)
        Me.ts_Insert_CALL.Text = "CALL"
        '
        'ts_Insert_INTEGER_Variable
        '
        Me.ts_Insert_INTEGER_Variable.Name = "ts_Insert_INTEGER_Variable"
        Me.ts_Insert_INTEGER_Variable.Size = New System.Drawing.Size(195, 22)
        Me.ts_Insert_INTEGER_Variable.Text = "Local INTEGER Variable"
        '
        'ts_Insert_FLOAT_Variable
        '
        Me.ts_Insert_FLOAT_Variable.Name = "ts_Insert_FLOAT_Variable"
        Me.ts_Insert_FLOAT_Variable.Size = New System.Drawing.Size(195, 22)
        Me.ts_Insert_FLOAT_Variable.Text = "Local FLOAT Variable"
        '
        'ts_Insert_STRING_Variable
        '
        Me.ts_Insert_STRING_Variable.Name = "ts_Insert_STRING_Variable"
        Me.ts_Insert_STRING_Variable.Size = New System.Drawing.Size(195, 22)
        Me.ts_Insert_STRING_Variable.Text = "Local STRING Variable"
        '
        'ts_Insert_INTEGER_List
        '
        Me.ts_Insert_INTEGER_List.Name = "ts_Insert_INTEGER_List"
        Me.ts_Insert_INTEGER_List.Size = New System.Drawing.Size(195, 22)
        Me.ts_Insert_INTEGER_List.Text = "ts_Insert_INTEGER_List"
        '
        'ts_Insert_FLOAT_List
        '
        Me.ts_Insert_FLOAT_List.Name = "ts_Insert_FLOAT_List"
        Me.ts_Insert_FLOAT_List.Size = New System.Drawing.Size(195, 22)
        Me.ts_Insert_FLOAT_List.Text = "ts_Insert_FLOAT_List"
        '
        'ts_Insert_STRING_List
        '
        Me.ts_Insert_STRING_List.Name = "ts_Insert_STRING_List"
        Me.ts_Insert_STRING_List.Size = New System.Drawing.Size(195, 22)
        Me.ts_Insert_STRING_List.Text = "ts_Insert_STRING_List"
        '
        'GlobalVariableToolStripMenuItem
        '
        Me.GlobalVariableToolStripMenuItem.Name = "GlobalVariableToolStripMenuItem"
        Me.GlobalVariableToolStripMenuItem.Size = New System.Drawing.Size(195, 22)
        Me.GlobalVariableToolStripMenuItem.Text = "Global Variable"
        '
        'BREAKToolStripMenuItem
        '
        Me.BREAKToolStripMenuItem.Name = "BREAKToolStripMenuItem"
        Me.BREAKToolStripMenuItem.Size = New System.Drawing.Size(195, 22)
        Me.BREAKToolStripMenuItem.Text = "BREAKPOINT"
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
        'ts_Move_Up
        '
        Me.ts_Move_Up.Image = CType(resources.GetObject("ts_Move_Up.Image"), System.Drawing.Image)
        Me.ts_Move_Up.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Move_Up.Name = "ts_Move_Up"
        Me.ts_Move_Up.Size = New System.Drawing.Size(59, 35)
        Me.ts_Move_Up.Text = "Move Up"
        Me.ts_Move_Up.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Move_Down
        '
        Me.ts_Move_Down.Image = CType(resources.GetObject("ts_Move_Down.Image"), System.Drawing.Image)
        Me.ts_Move_Down.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Move_Down.Name = "ts_Move_Down"
        Me.ts_Move_Down.Size = New System.Drawing.Size(75, 35)
        Me.ts_Move_Down.Text = "Move Down"
        Me.ts_Move_Down.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'pnl_Editor
        '
        Me.pnl_Editor.Controls.Add(Me.grid_Macro)
        Me.pnl_Editor.Controls.Add(Me.ts_Macro)
        Me.pnl_Editor.Dock = System.Windows.Forms.DockStyle.Left
        Me.pnl_Editor.Location = New System.Drawing.Point(0, 0)
        Me.pnl_Editor.Name = "pnl_Editor"
        Me.pnl_Editor.Size = New System.Drawing.Size(456, 472)
        Me.pnl_Editor.TabIndex = 12
        '
        'split_Left
        '
        Me.split_Left.BackColor = System.Drawing.Color.Firebrick
        Me.split_Left.Location = New System.Drawing.Point(456, 0)
        Me.split_Left.Name = "split_Left"
        Me.split_Left.Size = New System.Drawing.Size(3, 472)
        Me.split_Left.TabIndex = 13
        Me.split_Left.TabStop = False
        '
        'ts_Help
        '
        Me.ts_Help.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ToolStripLabel1, Me.ts_Macros, Me.ts_Syntax, Me.ts_Errors, Me.ts_G_Codes, Me.ts_TinyG})
        Me.ts_Help.Location = New System.Drawing.Point(0, 0)
        Me.ts_Help.Name = "ts_Help"
        Me.ts_Help.Size = New System.Drawing.Size(412, 38)
        Me.ts_Help.TabIndex = 109
        '
        'ToolStripLabel1
        '
        Me.ToolStripLabel1.Name = "ToolStripLabel1"
        Me.ToolStripLabel1.Size = New System.Drawing.Size(52, 35)
        Me.ToolStripLabel1.Text = "Manuals"
        '
        'ts_Macros
        '
        Me.ts_Macros.Image = CType(resources.GetObject("ts_Macros.Image"), System.Drawing.Image)
        Me.ts_Macros.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Macros.Name = "ts_Macros"
        Me.ts_Macros.Size = New System.Drawing.Size(50, 35)
        Me.ts_Macros.Text = "Macros"
        Me.ts_Macros.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Syntax
        '
        Me.ts_Syntax.Image = CType(resources.GetObject("ts_Syntax.Image"), System.Drawing.Image)
        Me.ts_Syntax.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Syntax.Name = "ts_Syntax"
        Me.ts_Syntax.Size = New System.Drawing.Size(45, 35)
        Me.ts_Syntax.Text = "Syntax"
        Me.ts_Syntax.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Errors
        '
        Me.ts_Errors.Image = CType(resources.GetObject("ts_Errors.Image"), System.Drawing.Image)
        Me.ts_Errors.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Errors.Name = "ts_Errors"
        Me.ts_Errors.Size = New System.Drawing.Size(41, 35)
        Me.ts_Errors.Text = "Errors"
        Me.ts_Errors.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_G_Codes
        '
        Me.ts_G_Codes.Image = CType(resources.GetObject("ts_G_Codes.Image"), System.Drawing.Image)
        Me.ts_G_Codes.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_G_Codes.Name = "ts_G_Codes"
        Me.ts_G_Codes.Size = New System.Drawing.Size(55, 35)
        Me.ts_G_Codes.Text = "G Codes"
        Me.ts_G_Codes.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_TinyG
        '
        Me.ts_TinyG.Image = CType(resources.GetObject("ts_TinyG.Image"), System.Drawing.Image)
        Me.ts_TinyG.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_TinyG.Name = "ts_TinyG"
        Me.ts_TinyG.Size = New System.Drawing.Size(42, 35)
        Me.ts_TinyG.Text = "TinyG"
        Me.ts_TinyG.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'txt_Help
        '
        Me.txt_Help.Dock = System.Windows.Forms.DockStyle.Fill
        Me.txt_Help.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Help.Location = New System.Drawing.Point(3, 3)
        Me.txt_Help.Multiline = True
        Me.txt_Help.Name = "txt_Help"
        Me.txt_Help.ScrollBars = System.Windows.Forms.ScrollBars.Both
        Me.txt_Help.Size = New System.Drawing.Size(398, 402)
        Me.txt_Help.TabIndex = 111
        Me.txt_Help.WordWrap = False
        '
        'tab_Macro
        '
        Me.tab_Macro.Controls.Add(Me.tab_Errors)
        Me.tab_Macro.Controls.Add(Me.tab_Help)
        Me.tab_Macro.Dock = System.Windows.Forms.DockStyle.Fill
        Me.tab_Macro.Location = New System.Drawing.Point(0, 38)
        Me.tab_Macro.Name = "tab_Macro"
        Me.tab_Macro.SelectedIndex = 0
        Me.tab_Macro.Size = New System.Drawing.Size(412, 434)
        Me.tab_Macro.TabIndex = 112
        '
        'tab_Errors
        '
        Me.tab_Errors.Controls.Add(Me.lst_Errors)
        Me.tab_Errors.Location = New System.Drawing.Point(4, 22)
        Me.tab_Errors.Name = "tab_Errors"
        Me.tab_Errors.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_Errors.Size = New System.Drawing.Size(450, 408)
        Me.tab_Errors.TabIndex = 0
        Me.tab_Errors.Text = "Errors"
        Me.tab_Errors.UseVisualStyleBackColor = True
        '
        'lst_Errors
        '
        Me.lst_Errors.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lst_Errors.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lst_Errors.FormattingEnabled = True
        Me.lst_Errors.ItemHeight = 16
        Me.lst_Errors.Location = New System.Drawing.Point(3, 3)
        Me.lst_Errors.Name = "lst_Errors"
        Me.lst_Errors.Size = New System.Drawing.Size(444, 402)
        Me.lst_Errors.TabIndex = 0
        '
        'tab_Help
        '
        Me.tab_Help.Controls.Add(Me.txt_Help)
        Me.tab_Help.Location = New System.Drawing.Point(4, 22)
        Me.tab_Help.Name = "tab_Help"
        Me.tab_Help.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_Help.Size = New System.Drawing.Size(404, 408)
        Me.tab_Help.TabIndex = 1
        Me.tab_Help.Text = "Help"
        Me.tab_Help.UseVisualStyleBackColor = True
        '
        'pnl_Errors
        '
        Me.pnl_Errors.Controls.Add(Me.tab_Macro)
        Me.pnl_Errors.Controls.Add(Me.ts_Help)
        Me.pnl_Errors.Dock = System.Windows.Forms.DockStyle.Fill
        Me.pnl_Errors.Location = New System.Drawing.Point(459, 0)
        Me.pnl_Errors.Name = "pnl_Errors"
        Me.pnl_Errors.Size = New System.Drawing.Size(412, 472)
        Me.pnl_Errors.TabIndex = 113
        '
        'ctr_Macro_Editor
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6!, 13!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.Controls.Add(Me.pnl_Errors)
        Me.Controls.Add(Me.split_Left)
        Me.Controls.Add(Me.pnl_Editor)
        Me.Name = "ctr_Macro_Editor"
        Me.Size = New System.Drawing.Size(871, 472)
        CType(Me.grid_Macro,System.ComponentModel.ISupportInitialize).EndInit
        Me.ts_Macro.ResumeLayout(false)
        Me.ts_Macro.PerformLayout
        Me.pnl_Editor.ResumeLayout(false)
        Me.pnl_Editor.PerformLayout
        Me.ts_Help.ResumeLayout(false)
        Me.ts_Help.PerformLayout
        Me.tab_Macro.ResumeLayout(false)
        Me.tab_Errors.ResumeLayout(false)
        Me.tab_Help.ResumeLayout(false)
        Me.tab_Help.PerformLayout
        Me.pnl_Errors.ResumeLayout(false)
        Me.pnl_Errors.PerformLayout
        Me.ResumeLayout(false)

End Sub
    Friend WithEvents grid_Macro As System.Windows.Forms.DataGridView
    Friend WithEvents col_G_Code_Line_No As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents col_G_Code As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents col_G_Code_Comment As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents col_G_Code_Error As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents ts_Macro As System.Windows.Forms.ToolStrip
    Friend WithEvents ts_Open As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Save As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Run As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Step As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Start_Here As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Insert As System.Windows.Forms.ToolStripDropDownButton
    Friend WithEvents ts_Insert_Expression As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_Insert_IF As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_Insert_ELSE As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_Insert_FOR As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_Insert_WHILE As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_Insert_DO As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_Insert_GOTO As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_Insert_LABEL As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_Insert_CALL As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_Insert_INTEGER_Variable As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_Insert_FLOAT_Variable As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_Insert_STRING_Variable As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_Insert_INTEGER_List As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_Insert_FLOAT_List As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_Insert_STRING_List As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents GlobalVariableToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents BREAKToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_Delete As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Move_Up As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Move_Down As System.Windows.Forms.ToolStripButton
    Friend WithEvents pnl_Editor As System.Windows.Forms.Panel
    Friend WithEvents split_Left As System.Windows.Forms.Splitter
    Friend WithEvents ts_Help As System.Windows.Forms.ToolStrip
    Friend WithEvents ts_Macros As System.Windows.Forms.ToolStripButton
    Friend WithEvents txt_Help As System.Windows.Forms.TextBox
    Friend WithEvents tab_Macro As System.Windows.Forms.TabControl
    Friend WithEvents tab_Errors As System.Windows.Forms.TabPage
    Friend WithEvents tab_Help As System.Windows.Forms.TabPage
    Friend WithEvents ToolStripLabel1 As System.Windows.Forms.ToolStripLabel
    Friend WithEvents ts_Syntax As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Errors As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_G_Codes As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_TinyG As System.Windows.Forms.ToolStripButton
    Friend WithEvents pnl_Errors As System.Windows.Forms.Panel
    Friend WithEvents lst_Errors As System.Windows.Forms.ListBox

End Class
