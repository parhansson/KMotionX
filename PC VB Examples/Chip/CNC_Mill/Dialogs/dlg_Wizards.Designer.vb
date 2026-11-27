<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class dlg_Wizards
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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(dlg_Wizards))
        Me.ts_Wizard = New System.Windows.Forms.ToolStrip()
        Me.ts_Exit = New System.Windows.Forms.ToolStripButton()
        Me.ts_Cancel = New System.Windows.Forms.ToolStripButton()
        Me.ts_Load = New System.Windows.Forms.ToolStripButton()
        Me.ts_Save = New System.Windows.Forms.ToolStripButton()
        Me.ts_Save_As = New System.Windows.Forms.ToolStripButton()
        Me.ts_Append_GCode = New System.Windows.Forms.ToolStripButton()
        Me.ts_Insert = New System.Windows.Forms.ToolStripDropDownButton()
        Me.ts_Insert_Blank_Line = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Insert_G0 = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Insert_G1 = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Insert_M03_M08 = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Insert_M05_M09 = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Insert_End = New System.Windows.Forms.ToolStripMenuItem()
        Me.ts_Delete_Line = New System.Windows.Forms.ToolStripButton()
        Me.ts_Clear = New System.Windows.Forms.ToolStripButton()
        Me.ts_Header = New System.Windows.Forms.ToolStripButton()
        Me.ts_Expand = New System.Windows.Forms.ToolStripButton()
        Me.ts_Collapse = New System.Windows.Forms.ToolStripButton()
        Me.ts_Help = New System.Windows.Forms.ToolStripButton()
        Me.ts_Info = New System.Windows.Forms.ToolStripTextBox()
        Me.tab_Wizard = New System.Windows.Forms.TabControl()
        Me.tab_Drawing = New System.Windows.Forms.TabPage()
        Me.ctl_Drawing = New Chip.ctrl_Drawing()
        Me.tab_Move = New System.Windows.Forms.TabPage()
        Me.btn_Move_Current_Z = New System.Windows.Forms.Button()
        Me.chk_Move_Z = New System.Windows.Forms.CheckBox()
        Me.chk_Move_Y = New System.Windows.Forms.CheckBox()
        Me.chk_Move_X = New System.Windows.Forms.CheckBox()
        Me.btn_Move_Current_XY = New System.Windows.Forms.Button()
        Me.chk_Move_Z_First = New System.Windows.Forms.CheckBox()
        Me.chk_Move_Rapid = New System.Windows.Forms.CheckBox()
        Me.Label38 = New System.Windows.Forms.Label()
        Me.txt_Move_Feed_Rate = New System.Windows.Forms.TextBox()
        Me.txt_Move_Z = New System.Windows.Forms.TextBox()
        Me.txt_Move_Y = New System.Windows.Forms.TextBox()
        Me.txt_Move_X = New System.Windows.Forms.TextBox()
        Me.tab_Flatten = New System.Windows.Forms.TabPage()
        Me.Label90 = New System.Windows.Forms.Label()
        Me.txt_Flatten_Park_Z = New System.Windows.Forms.TextBox()
        Me.Label89 = New System.Windows.Forms.Label()
        Me.Label88 = New System.Windows.Forms.Label()
        Me.txt_Flatten_Park_Y = New System.Windows.Forms.TextBox()
        Me.Label87 = New System.Windows.Forms.Label()
        Me.txt_Flatten_Park_X = New System.Windows.Forms.TextBox()
        Me.btn_Flatten_Material_Set = New System.Windows.Forms.Button()
        Me.Label86 = New System.Windows.Forms.Label()
        Me.txt_Flatten_End_Thickness = New System.Windows.Forms.TextBox()
        Me.Label85 = New System.Windows.Forms.Label()
        Me.Label84 = New System.Windows.Forms.Label()
        Me.txt_Flatten_Start_Thickness = New System.Windows.Forms.TextBox()
        Me.Label18 = New System.Windows.Forms.Label()
        Me.Label80 = New System.Windows.Forms.Label()
        Me.txt_Flatten_X_Start = New System.Windows.Forms.TextBox()
        Me.Label79 = New System.Windows.Forms.Label()
        Me.txt_Flatten_Y_Start = New System.Windows.Forms.TextBox()
        Me.txt_Flatten_Z_Passes = New System.Windows.Forms.TextBox()
        Me.Label78 = New System.Windows.Forms.Label()
        Me.txt_Flatten_XY_Passes = New System.Windows.Forms.TextBox()
        Me.Label15 = New System.Windows.Forms.Label()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.rad_Flatten_Y_Direction = New System.Windows.Forms.RadioButton()
        Me.rad_Flatten_X_Direction = New System.Windows.Forms.RadioButton()
        Me.Label21 = New System.Windows.Forms.Label()
        Me.txt_Flatten_Safe_Z = New System.Windows.Forms.TextBox()
        Me.Label19 = New System.Windows.Forms.Label()
        Me.txt_Flatten_Plunge_Rate = New System.Windows.Forms.TextBox()
        Me.Label17 = New System.Windows.Forms.Label()
        Me.Label16 = New System.Windows.Forms.Label()
        Me.txt_Flatten_Height = New System.Windows.Forms.TextBox()
        Me.txt_Flatten_Width = New System.Windows.Forms.TextBox()
        Me.Label10 = New System.Windows.Forms.Label()
        Me.txt_Flatten_Cut_Depth = New System.Windows.Forms.TextBox()
        Me.Label11 = New System.Windows.Forms.Label()
        Me.txt_Flatten_Pass_Depth = New System.Windows.Forms.TextBox()
        Me.Label12 = New System.Windows.Forms.Label()
        Me.txt_Flatten_Feed_Rate = New System.Windows.Forms.TextBox()
        Me.Label13 = New System.Windows.Forms.Label()
        Me.txt_Flatten_Step_Over = New System.Windows.Forms.TextBox()
        Me.Label14 = New System.Windows.Forms.Label()
        Me.txt_Flatten_Tool_Diameter = New System.Windows.Forms.TextBox()
        Me.ShapeContainer1 = New Microsoft.VisualBasic.PowerPacks.ShapeContainer()
        Me.RectangleShape1 = New Microsoft.VisualBasic.PowerPacks.RectangleShape()
        Me.tab_Hole = New System.Windows.Forms.TabPage()
        Me.Label27 = New System.Windows.Forms.Label()
        Me.txt_Hole_Safe_Z = New System.Windows.Forms.TextBox()
        Me.btn_Hole_Cutter_Path = New System.Windows.Forms.Button()
        Me.btn_Hole_Shallower = New System.Windows.Forms.Button()
        Me.btn_Wider = New System.Windows.Forms.Button()
        Me.rad_Hole_Pocket = New System.Windows.Forms.RadioButton()
        Me.Label20 = New System.Windows.Forms.Label()
        Me.txt_Hole_Plunge_Rate = New System.Windows.Forms.TextBox()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.txt_Hole_Diameter = New System.Windows.Forms.TextBox()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.btn_txt_Hole_Current = New System.Windows.Forms.Button()
        Me.txt_Hole_Center_Y = New System.Windows.Forms.TextBox()
        Me.txt_Hole_Center_X = New System.Windows.Forms.TextBox()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.txt_Hole_Cut_Depth = New System.Windows.Forms.TextBox()
        Me.btn_Hole_Deeper = New System.Windows.Forms.Button()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.txt_Hole_Pass_Depth = New System.Windows.Forms.TextBox()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.txt_Hole_Feed_Rate = New System.Windows.Forms.TextBox()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.txt_Hole_Step_Over = New System.Windows.Forms.TextBox()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.txt_Hole_Tool_Diameter = New System.Windows.Forms.TextBox()
        Me.ShapeContainer2 = New Microsoft.VisualBasic.PowerPacks.ShapeContainer()
        Me.OvalShape3 = New Microsoft.VisualBasic.PowerPacks.OvalShape()
        Me.OvalShape2 = New Microsoft.VisualBasic.PowerPacks.OvalShape()
        Me.LineShape6 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.LineShape5 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.LineShape4 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.LineShape3 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.LineShape2 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.LineShape1 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.X_Line = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.Circle = New Microsoft.VisualBasic.PowerPacks.OvalShape()
        Me.tab_Line = New System.Windows.Forms.TabPage()
        Me.tab_Straight_Options = New System.Windows.Forms.TabControl()
        Me.tab_Move_Horizontal = New System.Windows.Forms.TabPage()
        Me.btn_Horizontal_Cutter_Path = New System.Windows.Forms.Button()
        Me.btn_Horizontal_Current_End = New System.Windows.Forms.Button()
        Me.Label28 = New System.Windows.Forms.Label()
        Me.txt_Horizontal_Start_X = New System.Windows.Forms.TextBox()
        Me.btn_Horizontal_Current_Start = New System.Windows.Forms.Button()
        Me.Label22 = New System.Windows.Forms.Label()
        Me.txt_Horizontal_End_X = New System.Windows.Forms.TextBox()
        Me.ShapeContainer4 = New Microsoft.VisualBasic.PowerPacks.ShapeContainer()
        Me.Horizontal_Cutter_Top_Right = New Microsoft.VisualBasic.PowerPacks.OvalShape()
        Me.Horizontal_Cutter_Bottom_Right = New Microsoft.VisualBasic.PowerPacks.OvalShape()
        Me.Horizontal_Cutter_Bottom_Left = New Microsoft.VisualBasic.PowerPacks.OvalShape()
        Me.Horizontal_Cutter_Top_Left = New Microsoft.VisualBasic.PowerPacks.OvalShape()
        Me.LineShape10 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.tab_Move_Vertical = New System.Windows.Forms.TabPage()
        Me.tab_Move_Angle = New System.Windows.Forms.TabPage()
        Me.Label33 = New System.Windows.Forms.Label()
        Me.TextBox1 = New System.Windows.Forms.TextBox()
        Me.Label35 = New System.Windows.Forms.Label()
        Me.TextBox2 = New System.Windows.Forms.TextBox()
        Me.Label36 = New System.Windows.Forms.Label()
        Me.TextBox3 = New System.Windows.Forms.TextBox()
        Me.Button1 = New System.Windows.Forms.Button()
        Me.Label37 = New System.Windows.Forms.Label()
        Me.Label39 = New System.Windows.Forms.Label()
        Me.TextBox5 = New System.Windows.Forms.TextBox()
        Me.TextBox6 = New System.Windows.Forms.TextBox()
        Me.Button2 = New System.Windows.Forms.Button()
        Me.Label40 = New System.Windows.Forms.Label()
        Me.Label41 = New System.Windows.Forms.Label()
        Me.TextBox7 = New System.Windows.Forms.TextBox()
        Me.TextBox8 = New System.Windows.Forms.TextBox()
        Me.ShapeContainer3 = New Microsoft.VisualBasic.PowerPacks.ShapeContainer()
        Me.LineShape9 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.LineShape7 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.LineShape8 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.OvalShape1 = New Microsoft.VisualBasic.PowerPacks.OvalShape()
        Me.Label29 = New System.Windows.Forms.Label()
        Me.txt_Straight_Plunge_Rate = New System.Windows.Forms.TextBox()
        Me.btn_Line_Shallower = New System.Windows.Forms.Button()
        Me.Label23 = New System.Windows.Forms.Label()
        Me.txt_Straight_Cut_Depth = New System.Windows.Forms.TextBox()
        Me.btn_Line_Deeper = New System.Windows.Forms.Button()
        Me.Label24 = New System.Windows.Forms.Label()
        Me.txt_Straight_Pass_Depth = New System.Windows.Forms.TextBox()
        Me.Label25 = New System.Windows.Forms.Label()
        Me.txt_Straight_Feed_Rate = New System.Windows.Forms.TextBox()
        Me.Label26 = New System.Windows.Forms.Label()
        Me.txt_Straight_Tool_Diameter = New System.Windows.Forms.TextBox()
        Me.tab_Frame = New System.Windows.Forms.TabPage()
        Me.rad_Frame_Cut_Up = New System.Windows.Forms.RadioButton()
        Me.rad_Frame_Cut_Down = New System.Windows.Forms.RadioButton()
        Me.txt_Frame_Y_Offset = New System.Windows.Forms.TextBox()
        Me.Label82 = New System.Windows.Forms.Label()
        Me.txt_Frame_X_Offset = New System.Windows.Forms.TextBox()
        Me.Label81 = New System.Windows.Forms.Label()
        Me.txt_Frame_Lower_Right = New System.Windows.Forms.TextBox()
        Me.txt_Frame_Upper_Left = New System.Windows.Forms.TextBox()
        Me.txt_Frame_Upper_Right = New System.Windows.Forms.TextBox()
        Me.Label77 = New System.Windows.Forms.Label()
        Me.txt_Frame_Safe_Z = New System.Windows.Forms.TextBox()
        Me.Label76 = New System.Windows.Forms.Label()
        Me.txt_Frame_Cut_Depth = New System.Windows.Forms.TextBox()
        Me.Label75 = New System.Windows.Forms.Label()
        Me.txt_Frame_Feedrate = New System.Windows.Forms.TextBox()
        Me.Label72 = New System.Windows.Forms.Label()
        Me.txt_Frame_Tool_Diameter = New System.Windows.Forms.TextBox()
        Me.txt_Frame_Rough_Cut_Fractional = New System.Windows.Forms.TextBox()
        Me.txt_Frame_Frame_Width_Fractional = New System.Windows.Forms.TextBox()
        Me.txt_Frame_Rail_Width_Fractional = New System.Windows.Forms.TextBox()
        Me.txt_Frame_Mat_Width_Fractional = New System.Windows.Forms.TextBox()
        Me.Label43 = New System.Windows.Forms.Label()
        Me.txt_Frame_Rough_Cut = New System.Windows.Forms.TextBox()
        Me.Label46 = New System.Windows.Forms.Label()
        Me.lab_Frame_Lower_Left = New System.Windows.Forms.Label()
        Me.txt_Frame_Mat_Width = New System.Windows.Forms.TextBox()
        Me.Label34 = New System.Windows.Forms.Label()
        Me.txt_Frame_Frame_Width = New System.Windows.Forms.TextBox()
        Me.Label31 = New System.Windows.Forms.Label()
        Me.Label30 = New System.Windows.Forms.Label()
        Me.txt_Frame_Rail_Width = New System.Windows.Forms.TextBox()
        Me.ShapeContainer5 = New Microsoft.VisualBasic.PowerPacks.ShapeContainer()
        Me.LineShape15 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.RectangleShape2 = New Microsoft.VisualBasic.PowerPacks.RectangleShape()
        Me.LineShape14 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.LineShape13 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.LineShape12 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.LineShape11 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.tab_Circle = New System.Windows.Forms.TabPage()
        Me.Label52 = New System.Windows.Forms.Label()
        Me.txt_Circle_Plunge_Rate = New System.Windows.Forms.TextBox()
        Me.Label51 = New System.Windows.Forms.Label()
        Me.txt_Circle_Safe_Z = New System.Windows.Forms.TextBox()
        Me.Label50 = New System.Windows.Forms.Label()
        Me.txt_Circle_Start_Depth = New System.Windows.Forms.TextBox()
        Me.Label45 = New System.Windows.Forms.Label()
        Me.txt_Circle_Cut_Depth = New System.Windows.Forms.TextBox()
        Me.Label47 = New System.Windows.Forms.Label()
        Me.txt_Circle_Pass_Depth = New System.Windows.Forms.TextBox()
        Me.Label48 = New System.Windows.Forms.Label()
        Me.txt_Circle_Feed_Rate = New System.Windows.Forms.TextBox()
        Me.Label49 = New System.Windows.Forms.Label()
        Me.txt_Circle_Tool_Diameter = New System.Windows.Forms.TextBox()
        Me.Label32 = New System.Windows.Forms.Label()
        Me.txt_Circle_Diameter = New System.Windows.Forms.TextBox()
        Me.Label42 = New System.Windows.Forms.Label()
        Me.Label44 = New System.Windows.Forms.Label()
        Me.txt_Circle_Center_Y = New System.Windows.Forms.TextBox()
        Me.txt_Circle_Center_X = New System.Windows.Forms.TextBox()
        Me.ShapeContainer6 = New Microsoft.VisualBasic.PowerPacks.ShapeContainer()
        Me.LineShape22 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.OvalShape6 = New Microsoft.VisualBasic.PowerPacks.OvalShape()
        Me.LineShape21 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.LineShape20 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.LineShape19 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.LineShape18 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.LineShape17 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.LineShape16 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.OvalShape5 = New Microsoft.VisualBasic.PowerPacks.OvalShape()
        Me.OvalShape4 = New Microsoft.VisualBasic.PowerPacks.OvalShape()
        Me.tab_Arc = New System.Windows.Forms.TabPage()
        Me.Label58 = New System.Windows.Forms.Label()
        Me.txt_Arc_Block = New System.Windows.Forms.TextBox()
        Me.Label57 = New System.Windows.Forms.Label()
        Me.txt_Arc_Radius = New System.Windows.Forms.TextBox()
        Me.Label56 = New System.Windows.Forms.Label()
        Me.TextBox9 = New System.Windows.Forms.TextBox()
        Me.Label55 = New System.Windows.Forms.Label()
        Me.TextBox4 = New System.Windows.Forms.TextBox()
        Me.Label54 = New System.Windows.Forms.Label()
        Me.Label67 = New System.Windows.Forms.Label()
        Me.Label74 = New System.Windows.Forms.Label()
        Me.txt_Arc_I = New System.Windows.Forms.TextBox()
        Me.txt_Arc_J = New System.Windows.Forms.TextBox()
        Me.Label66 = New System.Windows.Forms.Label()
        Me.Label64 = New System.Windows.Forms.Label()
        Me.Label73 = New System.Windows.Forms.Label()
        Me.lab_Arc_Tooi_Outside_Direction = New System.Windows.Forms.Label()
        Me.lab_Arc_Tooi_Inside_Direction = New System.Windows.Forms.Label()
        Me.Label61 = New System.Windows.Forms.Label()
        Me.txt_Arc_Diameter = New System.Windows.Forms.TextBox()
        Me.Label71 = New System.Windows.Forms.Label()
        Me.Label70 = New System.Windows.Forms.Label()
        Me.cmb_Arc_Tool_Direction = New System.Windows.Forms.ComboBox()
        Me.cmb_Arc_Tool_Position = New System.Windows.Forms.ComboBox()
        Me.Label68 = New System.Windows.Forms.Label()
        Me.txt_Arc_Start_Angle = New System.Windows.Forms.TextBox()
        Me.Label69 = New System.Windows.Forms.Label()
        Me.txt_Arc_End_Angle = New System.Windows.Forms.TextBox()
        Me.txt_Arc_Start_X = New System.Windows.Forms.TextBox()
        Me.txt_Arc_End_X = New System.Windows.Forms.TextBox()
        Me.Label65 = New System.Windows.Forms.Label()
        Me.txt_Arc_Start_Y = New System.Windows.Forms.TextBox()
        Me.txt_Arc_End_Y = New System.Windows.Forms.TextBox()
        Me.Label59 = New System.Windows.Forms.Label()
        Me.txt_Arc_Feed_Rate = New System.Windows.Forms.TextBox()
        Me.Label60 = New System.Windows.Forms.Label()
        Me.txt_Arc_Tool_Diameter = New System.Windows.Forms.TextBox()
        Me.Label62 = New System.Windows.Forms.Label()
        Me.Label63 = New System.Windows.Forms.Label()
        Me.txt_Arc_Center_Y = New System.Windows.Forms.TextBox()
        Me.txt_Arc_Center_X = New System.Windows.Forms.TextBox()
        Me.ShapeContainer7 = New Microsoft.VisualBasic.PowerPacks.ShapeContainer()
        Me.LineShape31 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.LineShape30 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.line_Arc_Start_Vector = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.line_Arc_End_Vector = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.lab_Arc_Tool_Outside = New Microsoft.VisualBasic.PowerPacks.OvalShape()
        Me.lab_Arc_Tool_Inside = New Microsoft.VisualBasic.PowerPacks.OvalShape()
        Me.LineShape29 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.LineShape28 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.LineShape27 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.LineShape26 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.line_Arc_Vert_Cross = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.line_Arc_Horz_Cross = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.OvalShape7 = New Microsoft.VisualBasic.PowerPacks.OvalShape()
        Me.LineShape23 = New Microsoft.VisualBasic.PowerPacks.LineShape()
        Me.tab_Translate = New System.Windows.Forms.TabPage()
        Me.Label83 = New System.Windows.Forms.Label()
        Me.Cord_Length = New System.Windows.Forms.Label()
        Me.txt_Translate_Cord_Length = New System.Windows.Forms.TextBox()
        Me.btn_Translate_Current_File = New System.Windows.Forms.Button()
        Me.txt_Translate_Input_File = New System.Windows.Forms.TextBox()
        Me.btn_Translate_Open_File = New System.Windows.Forms.Button()
        Me.Label53 = New System.Windows.Forms.Label()
        Me.tab_Feather = New System.Windows.Forms.TabPage()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.rad_Feather_Y = New System.Windows.Forms.RadioButton()
        Me.rad_Feather_X = New System.Windows.Forms.RadioButton()
        Me.Label93 = New System.Windows.Forms.Label()
        Me.txt_Feather_Max_Z = New System.Windows.Forms.TextBox()
        Me.txt_Feather_Input_File = New System.Windows.Forms.TextBox()
        Me.btn_Feather_Open_File = New System.Windows.Forms.Button()
        Me.Label92 = New System.Windows.Forms.Label()
        Me.Label91 = New System.Windows.Forms.Label()
        Me.txt_Feather_Min_Z = New System.Windows.Forms.TextBox()
        Me.Splitter1 = New System.Windows.Forms.Splitter()
        Me.ctl_GCode = New Chip.ctrl_Gcode()
        Me.ts_Wizard.SuspendLayout()
        Me.tab_Wizard.SuspendLayout()
        Me.tab_Drawing.SuspendLayout()
        Me.tab_Move.SuspendLayout()
        Me.tab_Flatten.SuspendLayout()
        Me.GroupBox1.SuspendLayout()
        Me.tab_Hole.SuspendLayout()
        Me.tab_Line.SuspendLayout()
        Me.tab_Straight_Options.SuspendLayout()
        Me.tab_Move_Horizontal.SuspendLayout()
        Me.tab_Move_Angle.SuspendLayout()
        Me.tab_Frame.SuspendLayout()
        Me.tab_Circle.SuspendLayout()
        Me.tab_Arc.SuspendLayout()
        Me.tab_Translate.SuspendLayout()
        Me.tab_Feather.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        Me.SuspendLayout()
        '
        'ts_Wizard
        '
        Me.ts_Wizard.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ts_Exit, Me.ts_Cancel, Me.ts_Load, Me.ts_Save, Me.ts_Save_As, Me.ts_Append_GCode, Me.ts_Insert, Me.ts_Delete_Line, Me.ts_Clear, Me.ts_Header, Me.ts_Expand, Me.ts_Collapse, Me.ts_Help, Me.ts_Info})
        Me.ts_Wizard.Location = New System.Drawing.Point(0, 0)
        Me.ts_Wizard.Name = "ts_Wizard"
        Me.ts_Wizard.Size = New System.Drawing.Size(1258, 38)
        Me.ts_Wizard.TabIndex = 6
        Me.ts_Wizard.Text = "ToolStrip1"
        '
        'ts_Exit
        '
        Me.ts_Exit.Image = CType(resources.GetObject("ts_Exit.Image"), System.Drawing.Image)
        Me.ts_Exit.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Exit.Name = "ts_Exit"
        Me.ts_Exit.Size = New System.Drawing.Size(29, 35)
        Me.ts_Exit.Text = "Exit"
        Me.ts_Exit.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Cancel
        '
        Me.ts_Cancel.Image = CType(resources.GetObject("ts_Cancel.Image"), System.Drawing.Image)
        Me.ts_Cancel.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Cancel.Name = "ts_Cancel"
        Me.ts_Cancel.Size = New System.Drawing.Size(47, 35)
        Me.ts_Cancel.Text = "Cancel"
        Me.ts_Cancel.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
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
        Me.ts_Save.Visible = False
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
        'ts_Append_GCode
        '
        Me.ts_Append_GCode.Image = CType(resources.GetObject("ts_Append_GCode.Image"), System.Drawing.Image)
        Me.ts_Append_GCode.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Append_GCode.Name = "ts_Append_GCode"
        Me.ts_Append_GCode.Size = New System.Drawing.Size(92, 35)
        Me.ts_Append_GCode.Text = "Append GCode"
        Me.ts_Append_GCode.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Insert
        '
        Me.ts_Insert.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ts_Insert_Blank_Line, Me.ts_Insert_G0, Me.ts_Insert_G1, Me.ts_Insert_M03_M08, Me.ts_Insert_M05_M09, Me.ts_Insert_End})
        Me.ts_Insert.Image = CType(resources.GetObject("ts_Insert.Image"), System.Drawing.Image)
        Me.ts_Insert.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Insert.Name = "ts_Insert"
        Me.ts_Insert.Size = New System.Drawing.Size(74, 35)
        Me.ts_Insert.Text = "Insert Line"
        Me.ts_Insert.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Insert_Blank_Line
        '
        Me.ts_Insert_Blank_Line.Name = "ts_Insert_Blank_Line"
        Me.ts_Insert_Blank_Line.Size = New System.Drawing.Size(251, 22)
        Me.ts_Insert_Blank_Line.Text = "Blank Line"
        '
        'ts_Insert_G0
        '
        Me.ts_Insert_G0.Name = "ts_Insert_G0"
        Me.ts_Insert_G0.Size = New System.Drawing.Size(251, 22)
        Me.ts_Insert_G0.Text = "G0"
        '
        'ts_Insert_G1
        '
        Me.ts_Insert_G1.Name = "ts_Insert_G1"
        Me.ts_Insert_G1.Size = New System.Drawing.Size(251, 22)
        Me.ts_Insert_G1.Text = "G1"
        '
        'ts_Insert_M03_M08
        '
        Me.ts_Insert_M03_M08.Name = "ts_Insert_M03_M08"
        Me.ts_Insert_M03_M08.Size = New System.Drawing.Size(251, 22)
        Me.ts_Insert_M03_M08.Text = "Spindle && Coolant On (M03 M08)"
        '
        'ts_Insert_M05_M09
        '
        Me.ts_Insert_M05_M09.Name = "ts_Insert_M05_M09"
        Me.ts_Insert_M05_M09.Size = New System.Drawing.Size(251, 22)
        Me.ts_Insert_M05_M09.Text = "Spindle && Coolant Off (M05 M09)"
        '
        'ts_Insert_End
        '
        Me.ts_Insert_End.Name = "ts_Insert_End"
        Me.ts_Insert_End.Size = New System.Drawing.Size(251, 22)
        Me.ts_Insert_End.Text = "Program End (M30)"
        '
        'ts_Delete_Line
        '
        Me.ts_Delete_Line.Image = CType(resources.GetObject("ts_Delete_Line.Image"), System.Drawing.Image)
        Me.ts_Delete_Line.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Delete_Line.Name = "ts_Delete_Line"
        Me.ts_Delete_Line.Size = New System.Drawing.Size(69, 35)
        Me.ts_Delete_Line.Text = "Delete Line"
        Me.ts_Delete_Line.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Clear
        '
        Me.ts_Clear.Image = CType(resources.GetObject("ts_Clear.Image"), System.Drawing.Image)
        Me.ts_Clear.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Clear.Name = "ts_Clear"
        Me.ts_Clear.Size = New System.Drawing.Size(38, 35)
        Me.ts_Clear.Text = "Clear"
        Me.ts_Clear.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Header
        '
        Me.ts_Header.Image = CType(resources.GetObject("ts_Header.Image"), System.Drawing.Image)
        Me.ts_Header.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Header.Name = "ts_Header"
        Me.ts_Header.Size = New System.Drawing.Size(49, 35)
        Me.ts_Header.Text = "Header"
        Me.ts_Header.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Expand
        '
        Me.ts_Expand.Image = CType(resources.GetObject("ts_Expand.Image"), System.Drawing.Image)
        Me.ts_Expand.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Expand.Name = "ts_Expand"
        Me.ts_Expand.Size = New System.Drawing.Size(49, 35)
        Me.ts_Expand.Text = "Expand"
        Me.ts_Expand.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Collapse
        '
        Me.ts_Collapse.Image = CType(resources.GetObject("ts_Collapse.Image"), System.Drawing.Image)
        Me.ts_Collapse.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Collapse.Name = "ts_Collapse"
        Me.ts_Collapse.Size = New System.Drawing.Size(56, 35)
        Me.ts_Collapse.Text = "Collapse"
        Me.ts_Collapse.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Help
        '
        Me.ts_Help.Image = CType(resources.GetObject("ts_Help.Image"), System.Drawing.Image)
        Me.ts_Help.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Help.Name = "ts_Help"
        Me.ts_Help.Size = New System.Drawing.Size(36, 35)
        Me.ts_Help.Text = "Help"
        Me.ts_Help.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Info
        '
        Me.ts_Info.Name = "ts_Info"
        Me.ts_Info.Size = New System.Drawing.Size(100, 38)
        '
        'tab_Wizard
        '
        Me.tab_Wizard.Controls.Add(Me.tab_Drawing)
        Me.tab_Wizard.Controls.Add(Me.tab_Move)
        Me.tab_Wizard.Controls.Add(Me.tab_Flatten)
        Me.tab_Wizard.Controls.Add(Me.tab_Hole)
        Me.tab_Wizard.Controls.Add(Me.tab_Line)
        Me.tab_Wizard.Controls.Add(Me.tab_Frame)
        Me.tab_Wizard.Controls.Add(Me.tab_Circle)
        Me.tab_Wizard.Controls.Add(Me.tab_Arc)
        Me.tab_Wizard.Controls.Add(Me.tab_Translate)
        Me.tab_Wizard.Controls.Add(Me.tab_Feather)
        Me.tab_Wizard.Dock = System.Windows.Forms.DockStyle.Fill
        Me.tab_Wizard.Font = New System.Drawing.Font("Microsoft Sans Serif", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.tab_Wizard.Location = New System.Drawing.Point(372, 38)
        Me.tab_Wizard.Name = "tab_Wizard"
        Me.tab_Wizard.SelectedIndex = 0
        Me.tab_Wizard.Size = New System.Drawing.Size(886, 728)
        Me.tab_Wizard.TabIndex = 7
        '
        'tab_Drawing
        '
        Me.tab_Drawing.BackColor = System.Drawing.Color.DarkKhaki
        Me.tab_Drawing.Controls.Add(Me.ctl_Drawing)
        Me.tab_Drawing.Location = New System.Drawing.Point(4, 27)
        Me.tab_Drawing.Name = "tab_Drawing"
        Me.tab_Drawing.Size = New System.Drawing.Size(878, 697)
        Me.tab_Drawing.TabIndex = 2
        Me.tab_Drawing.Text = "Drawing"
        '
        'ctl_Drawing
        '
        Me.ctl_Drawing.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink
        Me.ctl_Drawing.Dock = System.Windows.Forms.DockStyle.Fill
        Me.ctl_Drawing.Location = New System.Drawing.Point(0, 0)
        Me.ctl_Drawing.Margin = New System.Windows.Forms.Padding(4)
        Me.ctl_Drawing.Name = "ctl_Drawing"
        Me.ctl_Drawing.Part_Drawing = Nothing
        Me.ctl_Drawing.Size = New System.Drawing.Size(878, 697)
        Me.ctl_Drawing.TabIndex = 0
        '
        'tab_Move
        '
        Me.tab_Move.BackColor = System.Drawing.Color.DarkKhaki
        Me.tab_Move.Controls.Add(Me.btn_Move_Current_Z)
        Me.tab_Move.Controls.Add(Me.chk_Move_Z)
        Me.tab_Move.Controls.Add(Me.chk_Move_Y)
        Me.tab_Move.Controls.Add(Me.chk_Move_X)
        Me.tab_Move.Controls.Add(Me.btn_Move_Current_XY)
        Me.tab_Move.Controls.Add(Me.chk_Move_Z_First)
        Me.tab_Move.Controls.Add(Me.chk_Move_Rapid)
        Me.tab_Move.Controls.Add(Me.Label38)
        Me.tab_Move.Controls.Add(Me.txt_Move_Feed_Rate)
        Me.tab_Move.Controls.Add(Me.txt_Move_Z)
        Me.tab_Move.Controls.Add(Me.txt_Move_Y)
        Me.tab_Move.Controls.Add(Me.txt_Move_X)
        Me.tab_Move.Location = New System.Drawing.Point(4, 27)
        Me.tab_Move.Name = "tab_Move"
        Me.tab_Move.Size = New System.Drawing.Size(878, 697)
        Me.tab_Move.TabIndex = 4
        Me.tab_Move.Text = "Move"
        '
        'btn_Move_Current_Z
        '
        Me.btn_Move_Current_Z.BackgroundImage = CType(resources.GetObject("btn_Move_Current_Z.BackgroundImage"), System.Drawing.Image)
        Me.btn_Move_Current_Z.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.btn_Move_Current_Z.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Move_Current_Z.Location = New System.Drawing.Point(216, 86)
        Me.btn_Move_Current_Z.Name = "btn_Move_Current_Z"
        Me.btn_Move_Current_Z.Size = New System.Drawing.Size(104, 40)
        Me.btn_Move_Current_Z.TabIndex = 42
        Me.btn_Move_Current_Z.Text = "Current Z"
        Me.btn_Move_Current_Z.UseVisualStyleBackColor = True
        '
        'chk_Move_Z
        '
        Me.chk_Move_Z.AutoSize = True
        Me.chk_Move_Z.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.chk_Move_Z.Location = New System.Drawing.Point(249, 29)
        Me.chk_Move_Z.Name = "chk_Move_Z"
        Me.chk_Move_Z.Size = New System.Drawing.Size(38, 24)
        Me.chk_Move_Z.TabIndex = 41
        Me.chk_Move_Z.Text = "Z"
        Me.chk_Move_Z.UseVisualStyleBackColor = True
        '
        'chk_Move_Y
        '
        Me.chk_Move_Y.AutoSize = True
        Me.chk_Move_Y.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.chk_Move_Y.Location = New System.Drawing.Point(155, 29)
        Me.chk_Move_Y.Name = "chk_Move_Y"
        Me.chk_Move_Y.Size = New System.Drawing.Size(39, 24)
        Me.chk_Move_Y.TabIndex = 40
        Me.chk_Move_Y.Text = "Y"
        Me.chk_Move_Y.UseVisualStyleBackColor = True
        '
        'chk_Move_X
        '
        Me.chk_Move_X.AutoSize = True
        Me.chk_Move_X.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.chk_Move_X.Location = New System.Drawing.Point(59, 29)
        Me.chk_Move_X.Name = "chk_Move_X"
        Me.chk_Move_X.Size = New System.Drawing.Size(39, 24)
        Me.chk_Move_X.TabIndex = 39
        Me.chk_Move_X.Text = "X"
        Me.chk_Move_X.UseVisualStyleBackColor = True
        '
        'btn_Move_Current_XY
        '
        Me.btn_Move_Current_XY.BackgroundImage = CType(resources.GetObject("btn_Move_Current_XY.BackgroundImage"), System.Drawing.Image)
        Me.btn_Move_Current_XY.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.btn_Move_Current_XY.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Move_Current_XY.Location = New System.Drawing.Point(60, 86)
        Me.btn_Move_Current_XY.Name = "btn_Move_Current_XY"
        Me.btn_Move_Current_XY.Size = New System.Drawing.Size(124, 40)
        Me.btn_Move_Current_XY.TabIndex = 38
        Me.btn_Move_Current_XY.Text = "Current XY"
        Me.btn_Move_Current_XY.UseVisualStyleBackColor = True
        '
        'chk_Move_Z_First
        '
        Me.chk_Move_Z_First.AutoSize = True
        Me.chk_Move_Z_First.Location = New System.Drawing.Point(526, 56)
        Me.chk_Move_Z_First.Name = "chk_Move_Z_First"
        Me.chk_Move_Z_First.Size = New System.Drawing.Size(110, 22)
        Me.chk_Move_Z_First.TabIndex = 37
        Me.chk_Move_Z_First.Text = "Move Z First"
        Me.chk_Move_Z_First.UseVisualStyleBackColor = True
        '
        'chk_Move_Rapid
        '
        Me.chk_Move_Rapid.AutoSize = True
        Me.chk_Move_Rapid.Location = New System.Drawing.Point(430, 56)
        Me.chk_Move_Rapid.Name = "chk_Move_Rapid"
        Me.chk_Move_Rapid.Size = New System.Drawing.Size(65, 22)
        Me.chk_Move_Rapid.TabIndex = 27
        Me.chk_Move_Rapid.Text = "Rapid"
        Me.chk_Move_Rapid.UseVisualStyleBackColor = True
        '
        'Label38
        '
        Me.Label38.AutoSize = True
        Me.Label38.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label38.Location = New System.Drawing.Point(325, 31)
        Me.Label38.Name = "Label38"
        Me.Label38.Size = New System.Drawing.Size(85, 20)
        Me.Label38.TabIndex = 26
        Me.Label38.Text = "Feed Rate"
        '
        'txt_Move_Feed_Rate
        '
        Me.txt_Move_Feed_Rate.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Move_Feed_Rate.Location = New System.Drawing.Point(333, 54)
        Me.txt_Move_Feed_Rate.Name = "txt_Move_Feed_Rate"
        Me.txt_Move_Feed_Rate.Size = New System.Drawing.Size(69, 26)
        Me.txt_Move_Feed_Rate.TabIndex = 25
        Me.txt_Move_Feed_Rate.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Move_Z
        '
        Me.txt_Move_Z.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Move_Z.Location = New System.Drawing.Point(234, 54)
        Me.txt_Move_Z.Name = "txt_Move_Z"
        Me.txt_Move_Z.Size = New System.Drawing.Size(69, 26)
        Me.txt_Move_Z.TabIndex = 23
        Me.txt_Move_Z.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Move_Y
        '
        Me.txt_Move_Y.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Move_Y.Location = New System.Drawing.Point(140, 54)
        Me.txt_Move_Y.Name = "txt_Move_Y"
        Me.txt_Move_Y.Size = New System.Drawing.Size(69, 26)
        Me.txt_Move_Y.TabIndex = 21
        Me.txt_Move_Y.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Move_X
        '
        Me.txt_Move_X.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Move_X.Location = New System.Drawing.Point(44, 54)
        Me.txt_Move_X.Name = "txt_Move_X"
        Me.txt_Move_X.Size = New System.Drawing.Size(69, 26)
        Me.txt_Move_X.TabIndex = 19
        Me.txt_Move_X.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'tab_Flatten
        '
        Me.tab_Flatten.BackColor = System.Drawing.Color.DarkKhaki
        Me.tab_Flatten.Controls.Add(Me.Label90)
        Me.tab_Flatten.Controls.Add(Me.txt_Flatten_Park_Z)
        Me.tab_Flatten.Controls.Add(Me.Label89)
        Me.tab_Flatten.Controls.Add(Me.Label88)
        Me.tab_Flatten.Controls.Add(Me.txt_Flatten_Park_Y)
        Me.tab_Flatten.Controls.Add(Me.Label87)
        Me.tab_Flatten.Controls.Add(Me.txt_Flatten_Park_X)
        Me.tab_Flatten.Controls.Add(Me.btn_Flatten_Material_Set)
        Me.tab_Flatten.Controls.Add(Me.Label86)
        Me.tab_Flatten.Controls.Add(Me.txt_Flatten_End_Thickness)
        Me.tab_Flatten.Controls.Add(Me.Label85)
        Me.tab_Flatten.Controls.Add(Me.Label84)
        Me.tab_Flatten.Controls.Add(Me.txt_Flatten_Start_Thickness)
        Me.tab_Flatten.Controls.Add(Me.Label18)
        Me.tab_Flatten.Controls.Add(Me.Label80)
        Me.tab_Flatten.Controls.Add(Me.txt_Flatten_X_Start)
        Me.tab_Flatten.Controls.Add(Me.Label79)
        Me.tab_Flatten.Controls.Add(Me.txt_Flatten_Y_Start)
        Me.tab_Flatten.Controls.Add(Me.txt_Flatten_Z_Passes)
        Me.tab_Flatten.Controls.Add(Me.Label78)
        Me.tab_Flatten.Controls.Add(Me.txt_Flatten_XY_Passes)
        Me.tab_Flatten.Controls.Add(Me.Label15)
        Me.tab_Flatten.Controls.Add(Me.GroupBox1)
        Me.tab_Flatten.Controls.Add(Me.Label21)
        Me.tab_Flatten.Controls.Add(Me.txt_Flatten_Safe_Z)
        Me.tab_Flatten.Controls.Add(Me.Label19)
        Me.tab_Flatten.Controls.Add(Me.txt_Flatten_Plunge_Rate)
        Me.tab_Flatten.Controls.Add(Me.Label17)
        Me.tab_Flatten.Controls.Add(Me.Label16)
        Me.tab_Flatten.Controls.Add(Me.txt_Flatten_Height)
        Me.tab_Flatten.Controls.Add(Me.txt_Flatten_Width)
        Me.tab_Flatten.Controls.Add(Me.Label10)
        Me.tab_Flatten.Controls.Add(Me.txt_Flatten_Cut_Depth)
        Me.tab_Flatten.Controls.Add(Me.Label11)
        Me.tab_Flatten.Controls.Add(Me.txt_Flatten_Pass_Depth)
        Me.tab_Flatten.Controls.Add(Me.Label12)
        Me.tab_Flatten.Controls.Add(Me.txt_Flatten_Feed_Rate)
        Me.tab_Flatten.Controls.Add(Me.Label13)
        Me.tab_Flatten.Controls.Add(Me.txt_Flatten_Step_Over)
        Me.tab_Flatten.Controls.Add(Me.Label14)
        Me.tab_Flatten.Controls.Add(Me.txt_Flatten_Tool_Diameter)
        Me.tab_Flatten.Controls.Add(Me.ShapeContainer1)
        Me.tab_Flatten.Location = New System.Drawing.Point(4, 27)
        Me.tab_Flatten.Name = "tab_Flatten"
        Me.tab_Flatten.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_Flatten.Size = New System.Drawing.Size(878, 697)
        Me.tab_Flatten.TabIndex = 1
        Me.tab_Flatten.Text = "Flatten"
        '
        'Label90
        '
        Me.Label90.AutoSize = True
        Me.Label90.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label90.Location = New System.Drawing.Point(11, 254)
        Me.Label90.Name = "Label90"
        Me.Label90.Size = New System.Drawing.Size(19, 20)
        Me.Label90.TabIndex = 114
        Me.Label90.Text = "Z"
        '
        'txt_Flatten_Park_Z
        '
        Me.txt_Flatten_Park_Z.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Flatten_Park_Z.Location = New System.Drawing.Point(37, 251)
        Me.txt_Flatten_Park_Z.Name = "txt_Flatten_Park_Z"
        Me.txt_Flatten_Park_Z.Size = New System.Drawing.Size(69, 26)
        Me.txt_Flatten_Park_Z.TabIndex = 113
        Me.txt_Flatten_Park_Z.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label89
        '
        Me.Label89.AutoSize = True
        Me.Label89.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label89.Location = New System.Drawing.Point(50, 160)
        Me.Label89.Name = "Label89"
        Me.Label89.Size = New System.Drawing.Size(41, 20)
        Me.Label89.TabIndex = 112
        Me.Label89.Text = "Park"
        '
        'Label88
        '
        Me.Label88.AutoSize = True
        Me.Label88.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label88.Location = New System.Drawing.Point(11, 222)
        Me.Label88.Name = "Label88"
        Me.Label88.Size = New System.Drawing.Size(20, 20)
        Me.Label88.TabIndex = 111
        Me.Label88.Text = "Y"
        '
        'txt_Flatten_Park_Y
        '
        Me.txt_Flatten_Park_Y.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Flatten_Park_Y.Location = New System.Drawing.Point(37, 219)
        Me.txt_Flatten_Park_Y.Name = "txt_Flatten_Park_Y"
        Me.txt_Flatten_Park_Y.Size = New System.Drawing.Size(69, 26)
        Me.txt_Flatten_Park_Y.TabIndex = 110
        Me.txt_Flatten_Park_Y.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label87
        '
        Me.Label87.AutoSize = True
        Me.Label87.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label87.Location = New System.Drawing.Point(11, 190)
        Me.Label87.Name = "Label87"
        Me.Label87.Size = New System.Drawing.Size(20, 20)
        Me.Label87.TabIndex = 109
        Me.Label87.Text = "X"
        '
        'txt_Flatten_Park_X
        '
        Me.txt_Flatten_Park_X.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Flatten_Park_X.Location = New System.Drawing.Point(37, 187)
        Me.txt_Flatten_Park_X.Name = "txt_Flatten_Park_X"
        Me.txt_Flatten_Park_X.Size = New System.Drawing.Size(69, 26)
        Me.txt_Flatten_Park_X.TabIndex = 108
        Me.txt_Flatten_Park_X.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'btn_Flatten_Material_Set
        '
        Me.btn_Flatten_Material_Set.BackgroundImage = CType(resources.GetObject("btn_Flatten_Material_Set.BackgroundImage"), System.Drawing.Image)
        Me.btn_Flatten_Material_Set.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.btn_Flatten_Material_Set.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Flatten_Material_Set.Location = New System.Drawing.Point(739, 37)
        Me.btn_Flatten_Material_Set.Name = "btn_Flatten_Material_Set"
        Me.btn_Flatten_Material_Set.Size = New System.Drawing.Size(87, 34)
        Me.btn_Flatten_Material_Set.TabIndex = 107
        Me.btn_Flatten_Material_Set.Text = "Set"
        Me.btn_Flatten_Material_Set.UseVisualStyleBackColor = True
        '
        'Label86
        '
        Me.Label86.AutoSize = True
        Me.Label86.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label86.Location = New System.Drawing.Point(662, 77)
        Me.Label86.Name = "Label86"
        Me.Label86.Size = New System.Drawing.Size(73, 20)
        Me.Label86.TabIndex = 106
        Me.Label86.Text = "After Cut"
        '
        'txt_Flatten_End_Thickness
        '
        Me.txt_Flatten_End_Thickness.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Flatten_End_Thickness.Location = New System.Drawing.Point(587, 74)
        Me.txt_Flatten_End_Thickness.Name = "txt_Flatten_End_Thickness"
        Me.txt_Flatten_End_Thickness.Size = New System.Drawing.Size(69, 26)
        Me.txt_Flatten_End_Thickness.TabIndex = 105
        Me.txt_Flatten_End_Thickness.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label85
        '
        Me.Label85.AutoSize = True
        Me.Label85.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label85.Location = New System.Drawing.Point(662, 43)
        Me.Label85.Name = "Label85"
        Me.Label85.Size = New System.Drawing.Size(64, 20)
        Me.Label85.TabIndex = 104
        Me.Label85.Text = "At Start"
        '
        'Label84
        '
        Me.Label84.AutoSize = True
        Me.Label84.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label84.Location = New System.Drawing.Point(576, 17)
        Me.Label84.Name = "Label84"
        Me.Label84.Size = New System.Drawing.Size(80, 20)
        Me.Label84.TabIndex = 103
        Me.Label84.Text = "Thickness"
        '
        'txt_Flatten_Start_Thickness
        '
        Me.txt_Flatten_Start_Thickness.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Flatten_Start_Thickness.Location = New System.Drawing.Point(587, 40)
        Me.txt_Flatten_Start_Thickness.Name = "txt_Flatten_Start_Thickness"
        Me.txt_Flatten_Start_Thickness.Size = New System.Drawing.Size(69, 26)
        Me.txt_Flatten_Start_Thickness.TabIndex = 102
        Me.txt_Flatten_Start_Thickness.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label18
        '
        Me.Label18.AutoSize = True
        Me.Label18.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label18.Location = New System.Drawing.Point(283, 341)
        Me.Label18.Name = "Label18"
        Me.Label18.Size = New System.Drawing.Size(31, 20)
        Me.Label18.TabIndex = 80
        Me.Label18.Text = "0,0"
        Me.Label18.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label80
        '
        Me.Label80.AutoSize = True
        Me.Label80.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label80.Location = New System.Drawing.Point(125, 347)
        Me.Label80.Name = "Label80"
        Me.Label80.Size = New System.Drawing.Size(59, 20)
        Me.Label80.TabIndex = 101
        Me.Label80.Text = "X Start"
        '
        'txt_Flatten_X_Start
        '
        Me.txt_Flatten_X_Start.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Flatten_X_Start.Location = New System.Drawing.Point(199, 344)
        Me.txt_Flatten_X_Start.Name = "txt_Flatten_X_Start"
        Me.txt_Flatten_X_Start.Size = New System.Drawing.Size(69, 26)
        Me.txt_Flatten_X_Start.TabIndex = 99
        Me.txt_Flatten_X_Start.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label79
        '
        Me.Label79.AutoSize = True
        Me.Label79.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label79.Location = New System.Drawing.Point(349, 377)
        Me.Label79.Name = "Label79"
        Me.Label79.Size = New System.Drawing.Size(59, 20)
        Me.Label79.TabIndex = 98
        Me.Label79.Text = "Y Start"
        '
        'txt_Flatten_Y_Start
        '
        Me.txt_Flatten_Y_Start.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Flatten_Y_Start.Location = New System.Drawing.Point(274, 374)
        Me.txt_Flatten_Y_Start.Name = "txt_Flatten_Y_Start"
        Me.txt_Flatten_Y_Start.Size = New System.Drawing.Size(69, 26)
        Me.txt_Flatten_Y_Start.TabIndex = 97
        Me.txt_Flatten_Y_Start.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Flatten_Z_Passes
        '
        Me.txt_Flatten_Z_Passes.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Flatten_Z_Passes.Location = New System.Drawing.Point(237, 115)
        Me.txt_Flatten_Z_Passes.Name = "txt_Flatten_Z_Passes"
        Me.txt_Flatten_Z_Passes.Size = New System.Drawing.Size(69, 26)
        Me.txt_Flatten_Z_Passes.TabIndex = 96
        Me.txt_Flatten_Z_Passes.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label78
        '
        Me.Label78.AutoSize = True
        Me.Label78.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label78.Location = New System.Drawing.Point(228, 92)
        Me.Label78.Name = "Label78"
        Me.Label78.Size = New System.Drawing.Size(75, 20)
        Me.Label78.TabIndex = 95
        Me.Label78.Text = "Z Passes"
        '
        'txt_Flatten_XY_Passes
        '
        Me.txt_Flatten_XY_Passes.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Flatten_XY_Passes.Location = New System.Drawing.Point(137, 115)
        Me.txt_Flatten_XY_Passes.Name = "txt_Flatten_XY_Passes"
        Me.txt_Flatten_XY_Passes.Size = New System.Drawing.Size(69, 26)
        Me.txt_Flatten_XY_Passes.TabIndex = 94
        Me.txt_Flatten_XY_Passes.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label15
        '
        Me.Label15.AutoSize = True
        Me.Label15.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label15.Location = New System.Drawing.Point(128, 92)
        Me.Label15.Name = "Label15"
        Me.Label15.Size = New System.Drawing.Size(87, 20)
        Me.Label15.TabIndex = 47
        Me.Label15.Text = "XY Passes"
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.rad_Flatten_Y_Direction)
        Me.GroupBox1.Controls.Add(Me.rad_Flatten_X_Direction)
        Me.GroupBox1.Location = New System.Drawing.Point(482, 281)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(82, 89)
        Me.GroupBox1.TabIndex = 45
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "Direction"
        '
        'rad_Flatten_Y_Direction
        '
        Me.rad_Flatten_Y_Direction.AutoSize = True
        Me.rad_Flatten_Y_Direction.Location = New System.Drawing.Point(17, 24)
        Me.rad_Flatten_Y_Direction.Name = "rad_Flatten_Y_Direction"
        Me.rad_Flatten_Y_Direction.Size = New System.Drawing.Size(46, 22)
        Me.rad_Flatten_Y_Direction.TabIndex = 44
        Me.rad_Flatten_Y_Direction.TabStop = True
        Me.rad_Flatten_Y_Direction.Text = "Y ^"
        Me.rad_Flatten_Y_Direction.UseVisualStyleBackColor = True
        '
        'rad_Flatten_X_Direction
        '
        Me.rad_Flatten_X_Direction.AutoSize = True
        Me.rad_Flatten_X_Direction.Location = New System.Drawing.Point(17, 52)
        Me.rad_Flatten_X_Direction.Name = "rad_Flatten_X_Direction"
        Me.rad_Flatten_X_Direction.Size = New System.Drawing.Size(49, 22)
        Me.rad_Flatten_X_Direction.TabIndex = 43
        Me.rad_Flatten_X_Direction.TabStop = True
        Me.rad_Flatten_X_Direction.Text = "X >"
        Me.rad_Flatten_X_Direction.UseVisualStyleBackColor = True
        '
        'Label21
        '
        Me.Label21.AutoSize = True
        Me.Label21.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label21.Location = New System.Drawing.Point(43, 92)
        Me.Label21.Name = "Label21"
        Me.Label21.Size = New System.Drawing.Size(57, 20)
        Me.Label21.TabIndex = 42
        Me.Label21.Text = "Safe Z"
        '
        'txt_Flatten_Safe_Z
        '
        Me.txt_Flatten_Safe_Z.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Flatten_Safe_Z.Location = New System.Drawing.Point(37, 115)
        Me.txt_Flatten_Safe_Z.Name = "txt_Flatten_Safe_Z"
        Me.txt_Flatten_Safe_Z.Size = New System.Drawing.Size(69, 26)
        Me.txt_Flatten_Safe_Z.TabIndex = 41
        Me.txt_Flatten_Safe_Z.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label19
        '
        Me.Label19.AutoSize = True
        Me.Label19.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label19.Location = New System.Drawing.Point(323, 17)
        Me.Label19.Name = "Label19"
        Me.Label19.Size = New System.Drawing.Size(97, 20)
        Me.Label19.TabIndex = 36
        Me.Label19.Text = "Plunge Rate"
        '
        'txt_Flatten_Plunge_Rate
        '
        Me.txt_Flatten_Plunge_Rate.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Flatten_Plunge_Rate.Location = New System.Drawing.Point(337, 40)
        Me.txt_Flatten_Plunge_Rate.Name = "txt_Flatten_Plunge_Rate"
        Me.txt_Flatten_Plunge_Rate.Size = New System.Drawing.Size(69, 26)
        Me.txt_Flatten_Plunge_Rate.TabIndex = 3
        Me.txt_Flatten_Plunge_Rate.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label17
        '
        Me.Label17.AutoSize = True
        Me.Label17.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label17.Location = New System.Drawing.Point(325, 190)
        Me.Label17.Name = "Label17"
        Me.Label17.Size = New System.Drawing.Size(56, 20)
        Me.Label17.TabIndex = 31
        Me.Label17.Text = "Height"
        '
        'Label16
        '
        Me.Label16.AutoSize = True
        Me.Label16.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label16.Location = New System.Drawing.Point(533, 220)
        Me.Label16.Name = "Label16"
        Me.Label16.Size = New System.Drawing.Size(50, 20)
        Me.Label16.TabIndex = 29
        Me.Label16.Text = "Width"
        '
        'txt_Flatten_Height
        '
        Me.txt_Flatten_Height.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Flatten_Height.Location = New System.Drawing.Point(383, 187)
        Me.txt_Flatten_Height.Name = "txt_Flatten_Height"
        Me.txt_Flatten_Height.Size = New System.Drawing.Size(69, 26)
        Me.txt_Flatten_Height.TabIndex = 9
        Me.txt_Flatten_Height.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Flatten_Width
        '
        Me.txt_Flatten_Width.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Flatten_Width.Location = New System.Drawing.Point(461, 217)
        Me.txt_Flatten_Width.Name = "txt_Flatten_Width"
        Me.txt_Flatten_Width.Size = New System.Drawing.Size(69, 26)
        Me.txt_Flatten_Width.TabIndex = 8
        Me.txt_Flatten_Width.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label10
        '
        Me.Label10.AutoSize = True
        Me.Label10.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label10.Location = New System.Drawing.Point(662, 112)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(82, 20)
        Me.Label10.TabIndex = 23
        Me.Label10.Text = "Cut Depth"
        '
        'txt_Flatten_Cut_Depth
        '
        Me.txt_Flatten_Cut_Depth.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Flatten_Cut_Depth.Location = New System.Drawing.Point(587, 109)
        Me.txt_Flatten_Cut_Depth.Name = "txt_Flatten_Cut_Depth"
        Me.txt_Flatten_Cut_Depth.Size = New System.Drawing.Size(69, 26)
        Me.txt_Flatten_Cut_Depth.TabIndex = 5
        Me.txt_Flatten_Cut_Depth.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label11
        '
        Me.Label11.AutoSize = True
        Me.Label11.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label11.Location = New System.Drawing.Point(425, 17)
        Me.Label11.Name = "Label11"
        Me.Label11.Size = New System.Drawing.Size(92, 20)
        Me.Label11.TabIndex = 20
        Me.Label11.Text = "Pass Depth"
        '
        'txt_Flatten_Pass_Depth
        '
        Me.txt_Flatten_Pass_Depth.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Flatten_Pass_Depth.Location = New System.Drawing.Point(437, 40)
        Me.txt_Flatten_Pass_Depth.Name = "txt_Flatten_Pass_Depth"
        Me.txt_Flatten_Pass_Depth.Size = New System.Drawing.Size(69, 26)
        Me.txt_Flatten_Pass_Depth.TabIndex = 4
        Me.txt_Flatten_Pass_Depth.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label12
        '
        Me.Label12.AutoSize = True
        Me.Label12.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label12.Location = New System.Drawing.Point(229, 17)
        Me.Label12.Name = "Label12"
        Me.Label12.Size = New System.Drawing.Size(85, 20)
        Me.Label12.TabIndex = 18
        Me.Label12.Text = "Feed Rate"
        '
        'txt_Flatten_Feed_Rate
        '
        Me.txt_Flatten_Feed_Rate.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Flatten_Feed_Rate.Location = New System.Drawing.Point(237, 40)
        Me.txt_Flatten_Feed_Rate.Name = "txt_Flatten_Feed_Rate"
        Me.txt_Flatten_Feed_Rate.Size = New System.Drawing.Size(69, 26)
        Me.txt_Flatten_Feed_Rate.TabIndex = 2
        Me.txt_Flatten_Feed_Rate.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label13
        '
        Me.Label13.AutoSize = True
        Me.Label13.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label13.Location = New System.Drawing.Point(131, 17)
        Me.Label13.Name = "Label13"
        Me.Label13.Size = New System.Drawing.Size(80, 20)
        Me.Label13.TabIndex = 16
        Me.Label13.Text = "Step Over"
        '
        'txt_Flatten_Step_Over
        '
        Me.txt_Flatten_Step_Over.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Flatten_Step_Over.Location = New System.Drawing.Point(137, 40)
        Me.txt_Flatten_Step_Over.Name = "txt_Flatten_Step_Over"
        Me.txt_Flatten_Step_Over.Size = New System.Drawing.Size(69, 26)
        Me.txt_Flatten_Step_Over.TabIndex = 1
        Me.txt_Flatten_Step_Over.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label14
        '
        Me.Label14.AutoSize = True
        Me.Label14.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label14.Location = New System.Drawing.Point(17, 17)
        Me.Label14.Name = "Label14"
        Me.Label14.Size = New System.Drawing.Size(108, 20)
        Me.Label14.TabIndex = 14
        Me.Label14.Text = "Tool Diameter"
        '
        'txt_Flatten_Tool_Diameter
        '
        Me.txt_Flatten_Tool_Diameter.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Flatten_Tool_Diameter.Location = New System.Drawing.Point(37, 40)
        Me.txt_Flatten_Tool_Diameter.Name = "txt_Flatten_Tool_Diameter"
        Me.txt_Flatten_Tool_Diameter.Size = New System.Drawing.Size(69, 26)
        Me.txt_Flatten_Tool_Diameter.TabIndex = 0
        Me.txt_Flatten_Tool_Diameter.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'ShapeContainer1
        '
        Me.ShapeContainer1.Location = New System.Drawing.Point(3, 3)
        Me.ShapeContainer1.Margin = New System.Windows.Forms.Padding(0)
        Me.ShapeContainer1.Name = "ShapeContainer1"
        Me.ShapeContainer1.Shapes.AddRange(New Microsoft.VisualBasic.PowerPacks.Shape() {Me.RectangleShape1})
        Me.ShapeContainer1.Size = New System.Drawing.Size(872, 691)
        Me.ShapeContainer1.TabIndex = 0
        Me.ShapeContainer1.TabStop = False
        '
        'RectangleShape1
        '
        Me.RectangleShape1.Location = New System.Drawing.Point(271, 214)
        Me.RectangleShape1.Name = "RectangleShape1"
        Me.RectangleShape1.Size = New System.Drawing.Size(177, 152)
        '
        'tab_Hole
        '
        Me.tab_Hole.BackColor = System.Drawing.Color.DarkKhaki
        Me.tab_Hole.Controls.Add(Me.Label27)
        Me.tab_Hole.Controls.Add(Me.txt_Hole_Safe_Z)
        Me.tab_Hole.Controls.Add(Me.btn_Hole_Cutter_Path)
        Me.tab_Hole.Controls.Add(Me.btn_Hole_Shallower)
        Me.tab_Hole.Controls.Add(Me.btn_Wider)
        Me.tab_Hole.Controls.Add(Me.rad_Hole_Pocket)
        Me.tab_Hole.Controls.Add(Me.Label20)
        Me.tab_Hole.Controls.Add(Me.txt_Hole_Plunge_Rate)
        Me.tab_Hole.Controls.Add(Me.Label7)
        Me.tab_Hole.Controls.Add(Me.txt_Hole_Diameter)
        Me.tab_Hole.Controls.Add(Me.Label9)
        Me.tab_Hole.Controls.Add(Me.Label8)
        Me.tab_Hole.Controls.Add(Me.Label6)
        Me.tab_Hole.Controls.Add(Me.btn_txt_Hole_Current)
        Me.tab_Hole.Controls.Add(Me.txt_Hole_Center_Y)
        Me.tab_Hole.Controls.Add(Me.txt_Hole_Center_X)
        Me.tab_Hole.Controls.Add(Me.Label5)
        Me.tab_Hole.Controls.Add(Me.txt_Hole_Cut_Depth)
        Me.tab_Hole.Controls.Add(Me.btn_Hole_Deeper)
        Me.tab_Hole.Controls.Add(Me.Label4)
        Me.tab_Hole.Controls.Add(Me.txt_Hole_Pass_Depth)
        Me.tab_Hole.Controls.Add(Me.Label3)
        Me.tab_Hole.Controls.Add(Me.txt_Hole_Feed_Rate)
        Me.tab_Hole.Controls.Add(Me.Label2)
        Me.tab_Hole.Controls.Add(Me.txt_Hole_Step_Over)
        Me.tab_Hole.Controls.Add(Me.Label1)
        Me.tab_Hole.Controls.Add(Me.txt_Hole_Tool_Diameter)
        Me.tab_Hole.Controls.Add(Me.ShapeContainer2)
        Me.tab_Hole.Location = New System.Drawing.Point(4, 27)
        Me.tab_Hole.Name = "tab_Hole"
        Me.tab_Hole.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_Hole.Size = New System.Drawing.Size(878, 697)
        Me.tab_Hole.TabIndex = 0
        Me.tab_Hole.Text = "Hole"
        '
        'Label27
        '
        Me.Label27.AutoSize = True
        Me.Label27.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label27.Location = New System.Drawing.Point(49, 79)
        Me.Label27.Name = "Label27"
        Me.Label27.Size = New System.Drawing.Size(57, 20)
        Me.Label27.TabIndex = 84
        Me.Label27.Text = "Safe Z"
        '
        'txt_Hole_Safe_Z
        '
        Me.txt_Hole_Safe_Z.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Hole_Safe_Z.Location = New System.Drawing.Point(43, 102)
        Me.txt_Hole_Safe_Z.Name = "txt_Hole_Safe_Z"
        Me.txt_Hole_Safe_Z.Size = New System.Drawing.Size(69, 26)
        Me.txt_Hole_Safe_Z.TabIndex = 83
        Me.txt_Hole_Safe_Z.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'btn_Hole_Cutter_Path
        '
        Me.btn_Hole_Cutter_Path.BackgroundImage = CType(resources.GetObject("btn_Hole_Cutter_Path.BackgroundImage"), System.Drawing.Image)
        Me.btn_Hole_Cutter_Path.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.btn_Hole_Cutter_Path.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Hole_Cutter_Path.Location = New System.Drawing.Point(188, 122)
        Me.btn_Hole_Cutter_Path.Name = "btn_Hole_Cutter_Path"
        Me.btn_Hole_Cutter_Path.Size = New System.Drawing.Size(122, 40)
        Me.btn_Hole_Cutter_Path.TabIndex = 82
        Me.btn_Hole_Cutter_Path.Text = "Cutter Path"
        Me.btn_Hole_Cutter_Path.UseVisualStyleBackColor = True
        '
        'btn_Hole_Shallower
        '
        Me.btn_Hole_Shallower.BackgroundImage = CType(resources.GetObject("btn_Hole_Shallower.BackgroundImage"), System.Drawing.Image)
        Me.btn_Hole_Shallower.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.btn_Hole_Shallower.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Hole_Shallower.Location = New System.Drawing.Point(688, 13)
        Me.btn_Hole_Shallower.Name = "btn_Hole_Shallower"
        Me.btn_Hole_Shallower.Size = New System.Drawing.Size(110, 42)
        Me.btn_Hole_Shallower.TabIndex = 44
        Me.btn_Hole_Shallower.Text = "Shallower"
        Me.btn_Hole_Shallower.UseVisualStyleBackColor = True
        '
        'btn_Wider
        '
        Me.btn_Wider.BackgroundImage = CType(resources.GetObject("btn_Wider.BackgroundImage"), System.Drawing.Image)
        Me.btn_Wider.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.btn_Wider.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Wider.Location = New System.Drawing.Point(607, 223)
        Me.btn_Wider.Name = "btn_Wider"
        Me.btn_Wider.Size = New System.Drawing.Size(94, 39)
        Me.btn_Wider.TabIndex = 43
        Me.btn_Wider.Text = "Wider"
        Me.btn_Wider.UseVisualStyleBackColor = True
        '
        'rad_Hole_Pocket
        '
        Me.rad_Hole_Pocket.BackgroundImage = CType(resources.GetObject("rad_Hole_Pocket.BackgroundImage"), System.Drawing.Image)
        Me.rad_Hole_Pocket.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.rad_Hole_Pocket.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.rad_Hole_Pocket.Location = New System.Drawing.Point(53, 166)
        Me.rad_Hole_Pocket.Name = "rad_Hole_Pocket"
        Me.rad_Hole_Pocket.Size = New System.Drawing.Size(90, 24)
        Me.rad_Hole_Pocket.TabIndex = 42
        Me.rad_Hole_Pocket.TabStop = True
        Me.rad_Hole_Pocket.Text = "Pocket"
        Me.rad_Hole_Pocket.UseVisualStyleBackColor = True
        '
        'Label20
        '
        Me.Label20.AutoSize = True
        Me.Label20.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label20.Location = New System.Drawing.Point(364, 17)
        Me.Label20.Name = "Label20"
        Me.Label20.Size = New System.Drawing.Size(97, 20)
        Me.Label20.TabIndex = 38
        Me.Label20.Text = "Plunge Rate"
        '
        'txt_Hole_Plunge_Rate
        '
        Me.txt_Hole_Plunge_Rate.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Hole_Plunge_Rate.Location = New System.Drawing.Point(378, 40)
        Me.txt_Hole_Plunge_Rate.Name = "txt_Hole_Plunge_Rate"
        Me.txt_Hole_Plunge_Rate.Size = New System.Drawing.Size(69, 26)
        Me.txt_Hole_Plunge_Rate.TabIndex = 3
        Me.txt_Hole_Plunge_Rate.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label7.Location = New System.Drawing.Point(603, 198)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(74, 20)
        Me.Label7.TabIndex = 22
        Me.Label7.Text = "Diameter"
        '
        'txt_Hole_Diameter
        '
        Me.txt_Hole_Diameter.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Hole_Diameter.Location = New System.Drawing.Point(529, 195)
        Me.txt_Hole_Diameter.Name = "txt_Hole_Diameter"
        Me.txt_Hole_Diameter.Size = New System.Drawing.Size(69, 26)
        Me.txt_Hole_Diameter.TabIndex = 8
        Me.txt_Hole_Diameter.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label9
        '
        Me.Label9.AutoSize = True
        Me.Label9.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label9.Location = New System.Drawing.Point(200, 266)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(20, 20)
        Me.Label9.TabIndex = 20
        Me.Label9.Text = "Y"
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label8.Location = New System.Drawing.Point(321, 356)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(20, 20)
        Me.Label8.TabIndex = 19
        Me.Label8.Text = "X"
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label6.Location = New System.Drawing.Point(216, 346)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(45, 20)
        Me.Label6.TabIndex = 17
        Me.Label6.Text = "(0, 0)"
        '
        'btn_txt_Hole_Current
        '
        Me.btn_txt_Hole_Current.BackgroundImage = CType(resources.GetObject("btn_txt_Hole_Current.BackgroundImage"), System.Drawing.Image)
        Me.btn_txt_Hole_Current.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.btn_txt_Hole_Current.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_txt_Hole_Current.Location = New System.Drawing.Point(63, 328)
        Me.btn_txt_Hole_Current.Name = "btn_txt_Hole_Current"
        Me.btn_txt_Hole_Current.Size = New System.Drawing.Size(132, 40)
        Me.btn_txt_Hole_Current.TabIndex = 16
        Me.btn_txt_Hole_Current.Text = "Current XY"
        Me.btn_txt_Hole_Current.UseVisualStyleBackColor = True
        '
        'txt_Hole_Center_Y
        '
        Me.txt_Hole_Center_Y.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Hole_Center_Y.Location = New System.Drawing.Point(224, 263)
        Me.txt_Hole_Center_Y.Name = "txt_Hole_Center_Y"
        Me.txt_Hole_Center_Y.Size = New System.Drawing.Size(69, 26)
        Me.txt_Hole_Center_Y.TabIndex = 6
        Me.txt_Hole_Center_Y.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Hole_Center_X
        '
        Me.txt_Hole_Center_X.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Hole_Center_X.Location = New System.Drawing.Point(297, 327)
        Me.txt_Hole_Center_X.Name = "txt_Hole_Center_X"
        Me.txt_Hole_Center_X.Size = New System.Drawing.Size(69, 26)
        Me.txt_Hole_Center_X.TabIndex = 7
        Me.txt_Hole_Center_X.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label5.Location = New System.Drawing.Point(600, 17)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(82, 20)
        Me.Label5.TabIndex = 12
        Me.Label5.Text = "Cut Depth"
        '
        'txt_Hole_Cut_Depth
        '
        Me.txt_Hole_Cut_Depth.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Hole_Cut_Depth.Location = New System.Drawing.Point(607, 40)
        Me.txt_Hole_Cut_Depth.Name = "txt_Hole_Cut_Depth"
        Me.txt_Hole_Cut_Depth.Size = New System.Drawing.Size(69, 26)
        Me.txt_Hole_Cut_Depth.TabIndex = 5
        Me.txt_Hole_Cut_Depth.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'btn_Hole_Deeper
        '
        Me.btn_Hole_Deeper.BackColor = System.Drawing.Color.DarkKhaki
        Me.btn_Hole_Deeper.BackgroundImage = CType(resources.GetObject("btn_Hole_Deeper.BackgroundImage"), System.Drawing.Image)
        Me.btn_Hole_Deeper.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.btn_Hole_Deeper.FlatAppearance.BorderColor = System.Drawing.Color.DarkKhaki
        Me.btn_Hole_Deeper.FlatAppearance.BorderSize = 0
        Me.btn_Hole_Deeper.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Hole_Deeper.Location = New System.Drawing.Point(688, 55)
        Me.btn_Hole_Deeper.Name = "btn_Hole_Deeper"
        Me.btn_Hole_Deeper.Size = New System.Drawing.Size(110, 41)
        Me.btn_Hole_Deeper.TabIndex = 10
        Me.btn_Hole_Deeper.Text = "Deeper"
        Me.btn_Hole_Deeper.UseVisualStyleBackColor = False
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label4.Location = New System.Drawing.Point(482, 17)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(92, 20)
        Me.Label4.TabIndex = 8
        Me.Label4.Text = "Pass Depth"
        '
        'txt_Hole_Pass_Depth
        '
        Me.txt_Hole_Pass_Depth.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Hole_Pass_Depth.Location = New System.Drawing.Point(494, 40)
        Me.txt_Hole_Pass_Depth.Name = "txt_Hole_Pass_Depth"
        Me.txt_Hole_Pass_Depth.Size = New System.Drawing.Size(69, 26)
        Me.txt_Hole_Pass_Depth.TabIndex = 4
        Me.txt_Hole_Pass_Depth.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label3.Location = New System.Drawing.Point(256, 17)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(85, 20)
        Me.Label3.TabIndex = 6
        Me.Label3.Text = "Feed Rate"
        '
        'txt_Hole_Feed_Rate
        '
        Me.txt_Hole_Feed_Rate.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Hole_Feed_Rate.Location = New System.Drawing.Point(264, 40)
        Me.txt_Hole_Feed_Rate.Name = "txt_Hole_Feed_Rate"
        Me.txt_Hole_Feed_Rate.Size = New System.Drawing.Size(69, 26)
        Me.txt_Hole_Feed_Rate.TabIndex = 2
        Me.txt_Hole_Feed_Rate.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label2.Location = New System.Drawing.Point(145, 17)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(80, 20)
        Me.Label2.TabIndex = 4
        Me.Label2.Text = "Step Over"
        '
        'txt_Hole_Step_Over
        '
        Me.txt_Hole_Step_Over.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Hole_Step_Over.Location = New System.Drawing.Point(151, 40)
        Me.txt_Hole_Step_Over.Name = "txt_Hole_Step_Over"
        Me.txt_Hole_Step_Over.Size = New System.Drawing.Size(69, 26)
        Me.txt_Hole_Step_Over.TabIndex = 1
        Me.txt_Hole_Step_Over.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label1.Location = New System.Drawing.Point(18, 17)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(108, 20)
        Me.Label1.TabIndex = 2
        Me.Label1.Text = "Tool Diameter"
        '
        'txt_Hole_Tool_Diameter
        '
        Me.txt_Hole_Tool_Diameter.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Hole_Tool_Diameter.Location = New System.Drawing.Point(38, 40)
        Me.txt_Hole_Tool_Diameter.Name = "txt_Hole_Tool_Diameter"
        Me.txt_Hole_Tool_Diameter.Size = New System.Drawing.Size(69, 26)
        Me.txt_Hole_Tool_Diameter.TabIndex = 0
        Me.txt_Hole_Tool_Diameter.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'ShapeContainer2
        '
        Me.ShapeContainer2.Location = New System.Drawing.Point(3, 3)
        Me.ShapeContainer2.Margin = New System.Windows.Forms.Padding(0)
        Me.ShapeContainer2.Name = "ShapeContainer2"
        Me.ShapeContainer2.Shapes.AddRange(New Microsoft.VisualBasic.PowerPacks.Shape() {Me.OvalShape3, Me.OvalShape2, Me.LineShape6, Me.LineShape5, Me.LineShape4, Me.LineShape3, Me.LineShape2, Me.LineShape1, Me.X_Line, Me.Circle})
        Me.ShapeContainer2.Size = New System.Drawing.Size(872, 691)
        Me.ShapeContainer2.TabIndex = 13
        Me.ShapeContainer2.TabStop = False
        '
        'OvalShape3
        '
        Me.OvalShape3.Location = New System.Drawing.Point(386, 67)
        Me.OvalShape3.Name = "OvalShape3"
        Me.OvalShape3.Size = New System.Drawing.Size(36, 36)
        '
        'OvalShape2
        '
        Me.OvalShape2.Location = New System.Drawing.Point(386, 105)
        Me.OvalShape2.Name = "OvalShape2"
        Me.OvalShape2.Size = New System.Drawing.Size(36, 36)
        '
        'LineShape6
        '
        Me.LineShape6.Name = "LineShape6"
        Me.LineShape6.X1 = 562
        Me.LineShape6.X2 = 562
        Me.LineShape6.Y1 = 103
        Me.LineShape6.Y2 = 304
        '
        'LineShape5
        '
        Me.LineShape5.BorderColor = System.Drawing.Color.Gray
        Me.LineShape5.Name = "LineShape5"
        Me.LineShape5.X1 = 401
        Me.LineShape5.X2 = 587
        Me.LineShape5.Y1 = 305
        Me.LineShape5.Y2 = 305
        '
        'LineShape4
        '
        Me.LineShape4.BorderColor = System.Drawing.Color.Gray
        Me.LineShape4.Name = "LineShape4"
        Me.LineShape4.X1 = 404
        Me.LineShape4.X2 = 587
        Me.LineShape4.Y1 = 103
        Me.LineShape4.Y2 = 103
        '
        'LineShape3
        '
        Me.LineShape3.BorderColor = System.Drawing.Color.Gray
        Me.LineShape3.Name = "LineShape3"
        Me.LineShape3.X1 = 255
        Me.LineShape3.X2 = 255
        Me.LineShape3.Y1 = 204
        Me.LineShape3.Y2 = 347
        '
        'LineShape2
        '
        Me.LineShape2.BorderColor = System.Drawing.Color.Gray
        Me.LineShape2.Name = "LineShape2"
        Me.LineShape2.X1 = 243
        Me.LineShape2.X2 = 404
        Me.LineShape2.Y1 = 337
        Me.LineShape2.Y2 = 337
        '
        'LineShape1
        '
        Me.LineShape1.Name = "LineShape1"
        Me.LineShape1.X1 = 403
        Me.LineShape1.X2 = 403
        Me.LineShape1.Y1 = 190
        Me.LineShape1.Y2 = 336
        '
        'X_Line
        '
        Me.X_Line.Name = "X_Line"
        Me.X_Line.X1 = 255
        Me.X_Line.X2 = 416
        Me.X_Line.Y1 = 204
        Me.X_Line.Y2 = 204
        '
        'Circle
        '
        Me.Circle.Location = New System.Drawing.Point(303, 104)
        Me.Circle.Name = "Circle"
        Me.Circle.Size = New System.Drawing.Size(200, 200)
        '
        'tab_Line
        '
        Me.tab_Line.BackColor = System.Drawing.Color.DarkKhaki
        Me.tab_Line.Controls.Add(Me.tab_Straight_Options)
        Me.tab_Line.Controls.Add(Me.Label29)
        Me.tab_Line.Controls.Add(Me.txt_Straight_Plunge_Rate)
        Me.tab_Line.Controls.Add(Me.btn_Line_Shallower)
        Me.tab_Line.Controls.Add(Me.Label23)
        Me.tab_Line.Controls.Add(Me.txt_Straight_Cut_Depth)
        Me.tab_Line.Controls.Add(Me.btn_Line_Deeper)
        Me.tab_Line.Controls.Add(Me.Label24)
        Me.tab_Line.Controls.Add(Me.txt_Straight_Pass_Depth)
        Me.tab_Line.Controls.Add(Me.Label25)
        Me.tab_Line.Controls.Add(Me.txt_Straight_Feed_Rate)
        Me.tab_Line.Controls.Add(Me.Label26)
        Me.tab_Line.Controls.Add(Me.txt_Straight_Tool_Diameter)
        Me.tab_Line.Location = New System.Drawing.Point(4, 27)
        Me.tab_Line.Name = "tab_Line"
        Me.tab_Line.Size = New System.Drawing.Size(878, 697)
        Me.tab_Line.TabIndex = 3
        Me.tab_Line.Text = "Straight"
        '
        'tab_Straight_Options
        '
        Me.tab_Straight_Options.Controls.Add(Me.tab_Move_Horizontal)
        Me.tab_Straight_Options.Controls.Add(Me.tab_Move_Vertical)
        Me.tab_Straight_Options.Controls.Add(Me.tab_Move_Angle)
        Me.tab_Straight_Options.Location = New System.Drawing.Point(3, 102)
        Me.tab_Straight_Options.Name = "tab_Straight_Options"
        Me.tab_Straight_Options.SelectedIndex = 0
        Me.tab_Straight_Options.Size = New System.Drawing.Size(706, 590)
        Me.tab_Straight_Options.TabIndex = 76
        '
        'tab_Move_Horizontal
        '
        Me.tab_Move_Horizontal.BackColor = System.Drawing.Color.DarkKhaki
        Me.tab_Move_Horizontal.Controls.Add(Me.btn_Horizontal_Cutter_Path)
        Me.tab_Move_Horizontal.Controls.Add(Me.btn_Horizontal_Current_End)
        Me.tab_Move_Horizontal.Controls.Add(Me.Label28)
        Me.tab_Move_Horizontal.Controls.Add(Me.txt_Horizontal_Start_X)
        Me.tab_Move_Horizontal.Controls.Add(Me.btn_Horizontal_Current_Start)
        Me.tab_Move_Horizontal.Controls.Add(Me.Label22)
        Me.tab_Move_Horizontal.Controls.Add(Me.txt_Horizontal_End_X)
        Me.tab_Move_Horizontal.Controls.Add(Me.ShapeContainer4)
        Me.tab_Move_Horizontal.Location = New System.Drawing.Point(4, 27)
        Me.tab_Move_Horizontal.Name = "tab_Move_Horizontal"
        Me.tab_Move_Horizontal.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_Move_Horizontal.Size = New System.Drawing.Size(698, 559)
        Me.tab_Move_Horizontal.TabIndex = 1
        Me.tab_Move_Horizontal.Text = "Horizontal"
        '
        'btn_Horizontal_Cutter_Path
        '
        Me.btn_Horizontal_Cutter_Path.BackgroundImage = CType(resources.GetObject("btn_Horizontal_Cutter_Path.BackgroundImage"), System.Drawing.Image)
        Me.btn_Horizontal_Cutter_Path.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.btn_Horizontal_Cutter_Path.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Horizontal_Cutter_Path.Location = New System.Drawing.Point(23, 26)
        Me.btn_Horizontal_Cutter_Path.Name = "btn_Horizontal_Cutter_Path"
        Me.btn_Horizontal_Cutter_Path.Size = New System.Drawing.Size(122, 40)
        Me.btn_Horizontal_Cutter_Path.TabIndex = 81
        Me.btn_Horizontal_Cutter_Path.Text = "Cutter Path"
        Me.btn_Horizontal_Cutter_Path.UseVisualStyleBackColor = True
        '
        'btn_Horizontal_Current_End
        '
        Me.btn_Horizontal_Current_End.BackgroundImage = CType(resources.GetObject("btn_Horizontal_Current_End.BackgroundImage"), System.Drawing.Image)
        Me.btn_Horizontal_Current_End.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.btn_Horizontal_Current_End.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Horizontal_Current_End.Location = New System.Drawing.Point(479, 151)
        Me.btn_Horizontal_Current_End.Name = "btn_Horizontal_Current_End"
        Me.btn_Horizontal_Current_End.Size = New System.Drawing.Size(122, 40)
        Me.btn_Horizontal_Current_End.TabIndex = 80
        Me.btn_Horizontal_Current_End.Text = "Current X"
        Me.btn_Horizontal_Current_End.UseVisualStyleBackColor = True
        '
        'Label28
        '
        Me.Label28.AutoSize = True
        Me.Label28.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label28.Location = New System.Drawing.Point(74, 101)
        Me.Label28.Name = "Label28"
        Me.Label28.Size = New System.Drawing.Size(20, 20)
        Me.Label28.TabIndex = 78
        Me.Label28.Text = "X"
        '
        'txt_Horizontal_Start_X
        '
        Me.txt_Horizontal_Start_X.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Horizontal_Start_X.Location = New System.Drawing.Point(50, 124)
        Me.txt_Horizontal_Start_X.Name = "txt_Horizontal_Start_X"
        Me.txt_Horizontal_Start_X.Size = New System.Drawing.Size(69, 26)
        Me.txt_Horizontal_Start_X.TabIndex = 76
        Me.txt_Horizontal_Start_X.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'btn_Horizontal_Current_Start
        '
        Me.btn_Horizontal_Current_Start.BackgroundImage = CType(resources.GetObject("btn_Horizontal_Current_Start.BackgroundImage"), System.Drawing.Image)
        Me.btn_Horizontal_Current_Start.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.btn_Horizontal_Current_Start.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Horizontal_Current_Start.Location = New System.Drawing.Point(22, 151)
        Me.btn_Horizontal_Current_Start.Name = "btn_Horizontal_Current_Start"
        Me.btn_Horizontal_Current_Start.Size = New System.Drawing.Size(124, 40)
        Me.btn_Horizontal_Current_Start.TabIndex = 63
        Me.btn_Horizontal_Current_Start.Text = "Current X"
        Me.btn_Horizontal_Current_Start.UseVisualStyleBackColor = True
        '
        'Label22
        '
        Me.Label22.AutoSize = True
        Me.Label22.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label22.Location = New System.Drawing.Point(530, 101)
        Me.Label22.Name = "Label22"
        Me.Label22.Size = New System.Drawing.Size(20, 20)
        Me.Label22.TabIndex = 57
        Me.Label22.Text = "X"
        '
        'txt_Horizontal_End_X
        '
        Me.txt_Horizontal_End_X.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Horizontal_End_X.Location = New System.Drawing.Point(506, 124)
        Me.txt_Horizontal_End_X.Name = "txt_Horizontal_End_X"
        Me.txt_Horizontal_End_X.Size = New System.Drawing.Size(69, 26)
        Me.txt_Horizontal_End_X.TabIndex = 61
        Me.txt_Horizontal_End_X.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'ShapeContainer4
        '
        Me.ShapeContainer4.Location = New System.Drawing.Point(3, 3)
        Me.ShapeContainer4.Margin = New System.Windows.Forms.Padding(0)
        Me.ShapeContainer4.Name = "ShapeContainer4"
        Me.ShapeContainer4.Shapes.AddRange(New Microsoft.VisualBasic.PowerPacks.Shape() {Me.Horizontal_Cutter_Top_Right, Me.Horizontal_Cutter_Bottom_Right, Me.Horizontal_Cutter_Bottom_Left, Me.Horizontal_Cutter_Top_Left, Me.LineShape10})
        Me.ShapeContainer4.Size = New System.Drawing.Size(692, 553)
        Me.ShapeContainer4.TabIndex = 82
        Me.ShapeContainer4.TabStop = False
        '
        'Horizontal_Cutter_Top_Right
        '
        Me.Horizontal_Cutter_Top_Right.Location = New System.Drawing.Point(418, 98)
        Me.Horizontal_Cutter_Top_Right.Name = "Horizontal_Cutter_Top_Right"
        Me.Horizontal_Cutter_Top_Right.Size = New System.Drawing.Size(36, 36)
        '
        'Horizontal_Cutter_Bottom_Right
        '
        Me.Horizontal_Cutter_Bottom_Right.Location = New System.Drawing.Point(418, 135)
        Me.Horizontal_Cutter_Bottom_Right.Name = "Horizontal_Cutter_Bottom_Right"
        Me.Horizontal_Cutter_Bottom_Right.Size = New System.Drawing.Size(36, 36)
        '
        'Horizontal_Cutter_Bottom_Left
        '
        Me.Horizontal_Cutter_Bottom_Left.Location = New System.Drawing.Point(168, 136)
        Me.Horizontal_Cutter_Bottom_Left.Name = "Horizontal_Cutter_Bottom_Left"
        Me.Horizontal_Cutter_Bottom_Left.Size = New System.Drawing.Size(36, 36)
        '
        'Horizontal_Cutter_Top_Left
        '
        Me.Horizontal_Cutter_Top_Left.Location = New System.Drawing.Point(168, 99)
        Me.Horizontal_Cutter_Top_Left.Name = "Horizontal_Cutter_Top_Left"
        Me.Horizontal_Cutter_Top_Left.Size = New System.Drawing.Size(36, 36)
        '
        'LineShape10
        '
        Me.LineShape10.Name = "LineShape10"
        Me.LineShape10.X1 = 171
        Me.LineShape10.X2 = 462
        Me.LineShape10.Y1 = 135
        Me.LineShape10.Y2 = 135
        '
        'tab_Move_Vertical
        '
        Me.tab_Move_Vertical.BackColor = System.Drawing.Color.DarkKhaki
        Me.tab_Move_Vertical.Location = New System.Drawing.Point(4, 27)
        Me.tab_Move_Vertical.Name = "tab_Move_Vertical"
        Me.tab_Move_Vertical.Size = New System.Drawing.Size(698, 559)
        Me.tab_Move_Vertical.TabIndex = 2
        Me.tab_Move_Vertical.Text = "Vertical"
        '
        'tab_Move_Angle
        '
        Me.tab_Move_Angle.BackColor = System.Drawing.Color.DarkKhaki
        Me.tab_Move_Angle.Controls.Add(Me.Label33)
        Me.tab_Move_Angle.Controls.Add(Me.TextBox1)
        Me.tab_Move_Angle.Controls.Add(Me.Label35)
        Me.tab_Move_Angle.Controls.Add(Me.TextBox2)
        Me.tab_Move_Angle.Controls.Add(Me.Label36)
        Me.tab_Move_Angle.Controls.Add(Me.TextBox3)
        Me.tab_Move_Angle.Controls.Add(Me.Button1)
        Me.tab_Move_Angle.Controls.Add(Me.Label37)
        Me.tab_Move_Angle.Controls.Add(Me.Label39)
        Me.tab_Move_Angle.Controls.Add(Me.TextBox5)
        Me.tab_Move_Angle.Controls.Add(Me.TextBox6)
        Me.tab_Move_Angle.Controls.Add(Me.Button2)
        Me.tab_Move_Angle.Controls.Add(Me.Label40)
        Me.tab_Move_Angle.Controls.Add(Me.Label41)
        Me.tab_Move_Angle.Controls.Add(Me.TextBox7)
        Me.tab_Move_Angle.Controls.Add(Me.TextBox8)
        Me.tab_Move_Angle.Controls.Add(Me.ShapeContainer3)
        Me.tab_Move_Angle.Location = New System.Drawing.Point(4, 27)
        Me.tab_Move_Angle.Name = "tab_Move_Angle"
        Me.tab_Move_Angle.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_Move_Angle.Size = New System.Drawing.Size(698, 559)
        Me.tab_Move_Angle.TabIndex = 0
        Me.tab_Move_Angle.Text = "Angle"
        '
        'Label33
        '
        Me.Label33.AutoSize = True
        Me.Label33.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label33.Location = New System.Drawing.Point(475, 112)
        Me.Label33.Name = "Label33"
        Me.Label33.Size = New System.Drawing.Size(50, 20)
        Me.Label33.TabIndex = 91
        Me.Label33.Text = "Angle"
        '
        'TextBox1
        '
        Me.TextBox1.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.TextBox1.Location = New System.Drawing.Point(458, 135)
        Me.TextBox1.Name = "TextBox1"
        Me.TextBox1.Size = New System.Drawing.Size(69, 26)
        Me.TextBox1.TabIndex = 90
        Me.TextBox1.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label35
        '
        Me.Label35.AutoSize = True
        Me.Label35.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label35.Location = New System.Drawing.Point(188, 173)
        Me.Label35.Name = "Label35"
        Me.Label35.Size = New System.Drawing.Size(59, 20)
        Me.Label35.TabIndex = 89
        Me.Label35.Text = "Length"
        '
        'TextBox2
        '
        Me.TextBox2.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.TextBox2.Location = New System.Drawing.Point(250, 170)
        Me.TextBox2.Name = "TextBox2"
        Me.TextBox2.Size = New System.Drawing.Size(69, 26)
        Me.TextBox2.TabIndex = 81
        Me.TextBox2.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label36
        '
        Me.Label36.AutoSize = True
        Me.Label36.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label36.Location = New System.Drawing.Point(246, 247)
        Me.Label36.Name = "Label36"
        Me.Label36.Size = New System.Drawing.Size(50, 20)
        Me.Label36.TabIndex = 88
        Me.Label36.Text = "Angle"
        '
        'TextBox3
        '
        Me.TextBox3.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.TextBox3.Location = New System.Drawing.Point(237, 270)
        Me.TextBox3.Name = "TextBox3"
        Me.TextBox3.Size = New System.Drawing.Size(69, 26)
        Me.TextBox3.TabIndex = 80
        Me.TextBox3.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Button1
        '
        Me.Button1.BackgroundImage = CType(resources.GetObject("Button1.BackgroundImage"), System.Drawing.Image)
        Me.Button1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.Button1.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Button1.Location = New System.Drawing.Point(546, 23)
        Me.Button1.Name = "Button1"
        Me.Button1.Size = New System.Drawing.Size(122, 40)
        Me.Button1.TabIndex = 87
        Me.Button1.Text = "Current XY"
        Me.Button1.UseVisualStyleBackColor = True
        '
        'Label37
        '
        Me.Label37.AutoSize = True
        Me.Label37.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label37.Location = New System.Drawing.Point(617, 82)
        Me.Label37.Name = "Label37"
        Me.Label37.Size = New System.Drawing.Size(20, 20)
        Me.Label37.TabIndex = 86
        Me.Label37.Text = "Y"
        '
        'Label39
        '
        Me.Label39.AutoSize = True
        Me.Label39.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label39.Location = New System.Drawing.Point(439, 40)
        Me.Label39.Name = "Label39"
        Me.Label39.Size = New System.Drawing.Size(20, 20)
        Me.Label39.TabIndex = 85
        Me.Label39.Text = "X"
        '
        'TextBox5
        '
        Me.TextBox5.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.TextBox5.Location = New System.Drawing.Point(546, 79)
        Me.TextBox5.Name = "TextBox5"
        Me.TextBox5.Size = New System.Drawing.Size(69, 26)
        Me.TextBox5.TabIndex = 79
        Me.TextBox5.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'TextBox6
        '
        Me.TextBox6.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.TextBox6.Location = New System.Drawing.Point(462, 37)
        Me.TextBox6.Name = "TextBox6"
        Me.TextBox6.Size = New System.Drawing.Size(69, 26)
        Me.TextBox6.TabIndex = 78
        Me.TextBox6.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Button2
        '
        Me.Button2.BackgroundImage = CType(resources.GetObject("Button2.BackgroundImage"), System.Drawing.Image)
        Me.Button2.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.Button2.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Button2.Location = New System.Drawing.Point(9, 315)
        Me.Button2.Name = "Button2"
        Me.Button2.Size = New System.Drawing.Size(124, 40)
        Me.Button2.TabIndex = 84
        Me.Button2.Text = "Current XY"
        Me.Button2.UseVisualStyleBackColor = True
        '
        'Label40
        '
        Me.Label40.AutoSize = True
        Me.Label40.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label40.Location = New System.Drawing.Point(27, 276)
        Me.Label40.Name = "Label40"
        Me.Label40.Size = New System.Drawing.Size(20, 20)
        Me.Label40.TabIndex = 83
        Me.Label40.Text = "Y"
        '
        'Label41
        '
        Me.Label41.AutoSize = True
        Me.Label41.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label41.Location = New System.Drawing.Point(213, 325)
        Me.Label41.Name = "Label41"
        Me.Label41.Size = New System.Drawing.Size(20, 20)
        Me.Label41.TabIndex = 82
        Me.Label41.Text = "X"
        '
        'TextBox7
        '
        Me.TextBox7.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.TextBox7.Location = New System.Drawing.Point(49, 273)
        Me.TextBox7.Name = "TextBox7"
        Me.TextBox7.Size = New System.Drawing.Size(69, 26)
        Me.TextBox7.TabIndex = 76
        Me.TextBox7.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'TextBox8
        '
        Me.TextBox8.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.TextBox8.Location = New System.Drawing.Point(141, 322)
        Me.TextBox8.Name = "TextBox8"
        Me.TextBox8.Size = New System.Drawing.Size(69, 26)
        Me.TextBox8.TabIndex = 77
        Me.TextBox8.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'ShapeContainer3
        '
        Me.ShapeContainer3.Location = New System.Drawing.Point(3, 3)
        Me.ShapeContainer3.Margin = New System.Windows.Forms.Padding(0)
        Me.ShapeContainer3.Name = "ShapeContainer3"
        Me.ShapeContainer3.Shapes.AddRange(New Microsoft.VisualBasic.PowerPacks.Shape() {Me.LineShape9, Me.LineShape7, Me.LineShape8, Me.OvalShape1})
        Me.ShapeContainer3.Size = New System.Drawing.Size(692, 553)
        Me.ShapeContainer3.TabIndex = 0
        Me.ShapeContainer3.TabStop = False
        '
        'LineShape9
        '
        Me.LineShape9.Name = "LineShape9"
        Me.LineShape9.X1 = 536
        Me.LineShape9.X2 = 536
        Me.LineShape9.Y1 = 72
        Me.LineShape9.Y2 = 163
        '
        'LineShape7
        '
        Me.LineShape7.Name = "LineShape7"
        Me.LineShape7.X1 = 138
        Me.LineShape7.X2 = 536
        Me.LineShape7.Y1 = 299
        Me.LineShape7.Y2 = 73
        '
        'LineShape8
        '
        Me.LineShape8.Name = "LineShape8"
        Me.LineShape8.X1 = 139
        Me.LineShape8.X2 = 288
        Me.LineShape8.Y1 = 300
        Me.LineShape8.Y2 = 300
        '
        'OvalShape1
        '
        Me.OvalShape1.Location = New System.Drawing.Point(135, 252)
        Me.OvalShape1.Name = "OvalShape1"
        Me.OvalShape1.Size = New System.Drawing.Size(36, 36)
        '
        'Label29
        '
        Me.Label29.AutoSize = True
        Me.Label29.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label29.Location = New System.Drawing.Point(246, 21)
        Me.Label29.Name = "Label29"
        Me.Label29.Size = New System.Drawing.Size(97, 20)
        Me.Label29.TabIndex = 69
        Me.Label29.Text = "Plunge Rate"
        '
        'txt_Straight_Plunge_Rate
        '
        Me.txt_Straight_Plunge_Rate.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Straight_Plunge_Rate.Location = New System.Drawing.Point(260, 44)
        Me.txt_Straight_Plunge_Rate.Name = "txt_Straight_Plunge_Rate"
        Me.txt_Straight_Plunge_Rate.Size = New System.Drawing.Size(69, 26)
        Me.txt_Straight_Plunge_Rate.TabIndex = 2
        Me.txt_Straight_Plunge_Rate.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'btn_Line_Shallower
        '
        Me.btn_Line_Shallower.BackgroundImage = CType(resources.GetObject("btn_Line_Shallower.BackgroundImage"), System.Drawing.Image)
        Me.btn_Line_Shallower.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.btn_Line_Shallower.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Line_Shallower.Location = New System.Drawing.Point(564, 13)
        Me.btn_Line_Shallower.Name = "btn_Line_Shallower"
        Me.btn_Line_Shallower.Size = New System.Drawing.Size(110, 42)
        Me.btn_Line_Shallower.TabIndex = 54
        Me.btn_Line_Shallower.Text = "Shallower"
        Me.btn_Line_Shallower.UseVisualStyleBackColor = True
        '
        'Label23
        '
        Me.Label23.AutoSize = True
        Me.Label23.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label23.Location = New System.Drawing.Point(470, 21)
        Me.Label23.Name = "Label23"
        Me.Label23.Size = New System.Drawing.Size(82, 20)
        Me.Label23.TabIndex = 53
        Me.Label23.Text = "Cut Depth"
        '
        'txt_Straight_Cut_Depth
        '
        Me.txt_Straight_Cut_Depth.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Straight_Cut_Depth.Location = New System.Drawing.Point(477, 44)
        Me.txt_Straight_Cut_Depth.Name = "txt_Straight_Cut_Depth"
        Me.txt_Straight_Cut_Depth.Size = New System.Drawing.Size(69, 26)
        Me.txt_Straight_Cut_Depth.TabIndex = 4
        Me.txt_Straight_Cut_Depth.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'btn_Line_Deeper
        '
        Me.btn_Line_Deeper.BackColor = System.Drawing.Color.DarkKhaki
        Me.btn_Line_Deeper.BackgroundImage = CType(resources.GetObject("btn_Line_Deeper.BackgroundImage"), System.Drawing.Image)
        Me.btn_Line_Deeper.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.btn_Line_Deeper.FlatAppearance.BorderColor = System.Drawing.Color.DarkKhaki
        Me.btn_Line_Deeper.FlatAppearance.BorderSize = 0
        Me.btn_Line_Deeper.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Line_Deeper.Location = New System.Drawing.Point(564, 55)
        Me.btn_Line_Deeper.Name = "btn_Line_Deeper"
        Me.btn_Line_Deeper.Size = New System.Drawing.Size(110, 41)
        Me.btn_Line_Deeper.TabIndex = 52
        Me.btn_Line_Deeper.Text = "Deeper"
        Me.btn_Line_Deeper.UseVisualStyleBackColor = False
        '
        'Label24
        '
        Me.Label24.AutoSize = True
        Me.Label24.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label24.Location = New System.Drawing.Point(355, 21)
        Me.Label24.Name = "Label24"
        Me.Label24.Size = New System.Drawing.Size(92, 20)
        Me.Label24.TabIndex = 51
        Me.Label24.Text = "Pass Depth"
        '
        'txt_Straight_Pass_Depth
        '
        Me.txt_Straight_Pass_Depth.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Straight_Pass_Depth.Location = New System.Drawing.Point(367, 44)
        Me.txt_Straight_Pass_Depth.Name = "txt_Straight_Pass_Depth"
        Me.txt_Straight_Pass_Depth.Size = New System.Drawing.Size(69, 26)
        Me.txt_Straight_Pass_Depth.TabIndex = 3
        Me.txt_Straight_Pass_Depth.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label25
        '
        Me.Label25.AutoSize = True
        Me.Label25.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label25.Location = New System.Drawing.Point(148, 21)
        Me.Label25.Name = "Label25"
        Me.Label25.Size = New System.Drawing.Size(85, 20)
        Me.Label25.TabIndex = 50
        Me.Label25.Text = "Feed Rate"
        '
        'txt_Straight_Feed_Rate
        '
        Me.txt_Straight_Feed_Rate.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Straight_Feed_Rate.Location = New System.Drawing.Point(156, 44)
        Me.txt_Straight_Feed_Rate.Name = "txt_Straight_Feed_Rate"
        Me.txt_Straight_Feed_Rate.Size = New System.Drawing.Size(69, 26)
        Me.txt_Straight_Feed_Rate.TabIndex = 1
        Me.txt_Straight_Feed_Rate.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label26
        '
        Me.Label26.AutoSize = True
        Me.Label26.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label26.Location = New System.Drawing.Point(26, 21)
        Me.Label26.Name = "Label26"
        Me.Label26.Size = New System.Drawing.Size(108, 20)
        Me.Label26.TabIndex = 47
        Me.Label26.Text = "Tool Diameter"
        '
        'txt_Straight_Tool_Diameter
        '
        Me.txt_Straight_Tool_Diameter.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Straight_Tool_Diameter.Location = New System.Drawing.Point(46, 44)
        Me.txt_Straight_Tool_Diameter.Name = "txt_Straight_Tool_Diameter"
        Me.txt_Straight_Tool_Diameter.Size = New System.Drawing.Size(69, 26)
        Me.txt_Straight_Tool_Diameter.TabIndex = 0
        Me.txt_Straight_Tool_Diameter.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'tab_Frame
        '
        Me.tab_Frame.BackColor = System.Drawing.Color.DarkKhaki
        Me.tab_Frame.Controls.Add(Me.rad_Frame_Cut_Up)
        Me.tab_Frame.Controls.Add(Me.rad_Frame_Cut_Down)
        Me.tab_Frame.Controls.Add(Me.txt_Frame_Y_Offset)
        Me.tab_Frame.Controls.Add(Me.Label82)
        Me.tab_Frame.Controls.Add(Me.txt_Frame_X_Offset)
        Me.tab_Frame.Controls.Add(Me.Label81)
        Me.tab_Frame.Controls.Add(Me.txt_Frame_Lower_Right)
        Me.tab_Frame.Controls.Add(Me.txt_Frame_Upper_Left)
        Me.tab_Frame.Controls.Add(Me.txt_Frame_Upper_Right)
        Me.tab_Frame.Controls.Add(Me.Label77)
        Me.tab_Frame.Controls.Add(Me.txt_Frame_Safe_Z)
        Me.tab_Frame.Controls.Add(Me.Label76)
        Me.tab_Frame.Controls.Add(Me.txt_Frame_Cut_Depth)
        Me.tab_Frame.Controls.Add(Me.Label75)
        Me.tab_Frame.Controls.Add(Me.txt_Frame_Feedrate)
        Me.tab_Frame.Controls.Add(Me.Label72)
        Me.tab_Frame.Controls.Add(Me.txt_Frame_Tool_Diameter)
        Me.tab_Frame.Controls.Add(Me.txt_Frame_Rough_Cut_Fractional)
        Me.tab_Frame.Controls.Add(Me.txt_Frame_Frame_Width_Fractional)
        Me.tab_Frame.Controls.Add(Me.txt_Frame_Rail_Width_Fractional)
        Me.tab_Frame.Controls.Add(Me.txt_Frame_Mat_Width_Fractional)
        Me.tab_Frame.Controls.Add(Me.Label43)
        Me.tab_Frame.Controls.Add(Me.txt_Frame_Rough_Cut)
        Me.tab_Frame.Controls.Add(Me.Label46)
        Me.tab_Frame.Controls.Add(Me.lab_Frame_Lower_Left)
        Me.tab_Frame.Controls.Add(Me.txt_Frame_Mat_Width)
        Me.tab_Frame.Controls.Add(Me.Label34)
        Me.tab_Frame.Controls.Add(Me.txt_Frame_Frame_Width)
        Me.tab_Frame.Controls.Add(Me.Label31)
        Me.tab_Frame.Controls.Add(Me.Label30)
        Me.tab_Frame.Controls.Add(Me.txt_Frame_Rail_Width)
        Me.tab_Frame.Controls.Add(Me.ShapeContainer5)
        Me.tab_Frame.Location = New System.Drawing.Point(4, 27)
        Me.tab_Frame.Name = "tab_Frame"
        Me.tab_Frame.Size = New System.Drawing.Size(878, 697)
        Me.tab_Frame.TabIndex = 5
        Me.tab_Frame.Text = "Frame"
        '
        'rad_Frame_Cut_Up
        '
        Me.rad_Frame_Cut_Up.AutoSize = True
        Me.rad_Frame_Cut_Up.Location = New System.Drawing.Point(710, 411)
        Me.rad_Frame_Cut_Up.Name = "rad_Frame_Cut_Up"
        Me.rad_Frame_Cut_Up.Size = New System.Drawing.Size(93, 22)
        Me.rad_Frame_Cut_Up.TabIndex = 107
        Me.rad_Frame_Cut_Up.TabStop = True
        Me.rad_Frame_Cut_Up.Text = "Start Here"
        Me.rad_Frame_Cut_Up.UseVisualStyleBackColor = True
        '
        'rad_Frame_Cut_Down
        '
        Me.rad_Frame_Cut_Down.AutoSize = True
        Me.rad_Frame_Cut_Down.Location = New System.Drawing.Point(619, 311)
        Me.rad_Frame_Cut_Down.Name = "rad_Frame_Cut_Down"
        Me.rad_Frame_Cut_Down.Size = New System.Drawing.Size(93, 22)
        Me.rad_Frame_Cut_Down.TabIndex = 106
        Me.rad_Frame_Cut_Down.TabStop = True
        Me.rad_Frame_Cut_Down.Text = "Start Here"
        Me.rad_Frame_Cut_Down.UseVisualStyleBackColor = True
        '
        'txt_Frame_Y_Offset
        '
        Me.txt_Frame_Y_Offset.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Frame_Y_Offset.Location = New System.Drawing.Point(103, 484)
        Me.txt_Frame_Y_Offset.Name = "txt_Frame_Y_Offset"
        Me.txt_Frame_Y_Offset.Size = New System.Drawing.Size(81, 26)
        Me.txt_Frame_Y_Offset.TabIndex = 105
        Me.txt_Frame_Y_Offset.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label82
        '
        Me.Label82.AutoSize = True
        Me.Label82.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label82.Location = New System.Drawing.Point(109, 462)
        Me.Label82.Name = "Label82"
        Me.Label82.Size = New System.Drawing.Size(68, 20)
        Me.Label82.TabIndex = 104
        Me.Label82.Text = "Y Offset"
        '
        'txt_Frame_X_Offset
        '
        Me.txt_Frame_X_Offset.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Frame_X_Offset.Location = New System.Drawing.Point(41, 376)
        Me.txt_Frame_X_Offset.Name = "txt_Frame_X_Offset"
        Me.txt_Frame_X_Offset.Size = New System.Drawing.Size(81, 26)
        Me.txt_Frame_X_Offset.TabIndex = 103
        Me.txt_Frame_X_Offset.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label81
        '
        Me.Label81.AutoSize = True
        Me.Label81.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label81.Location = New System.Drawing.Point(47, 354)
        Me.Label81.Name = "Label81"
        Me.Label81.Size = New System.Drawing.Size(68, 20)
        Me.Label81.TabIndex = 102
        Me.Label81.Text = "X Offset"
        '
        'txt_Frame_Lower_Right
        '
        Me.txt_Frame_Lower_Right.BackColor = System.Drawing.Color.DarkKhaki
        Me.txt_Frame_Lower_Right.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.txt_Frame_Lower_Right.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Frame_Lower_Right.Location = New System.Drawing.Point(573, 425)
        Me.txt_Frame_Lower_Right.Name = "txt_Frame_Lower_Right"
        Me.txt_Frame_Lower_Right.Size = New System.Drawing.Size(119, 19)
        Me.txt_Frame_Lower_Right.TabIndex = 101
        Me.txt_Frame_Lower_Right.Text = "0,0"
        Me.txt_Frame_Lower_Right.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Frame_Upper_Left
        '
        Me.txt_Frame_Upper_Left.BackColor = System.Drawing.Color.DarkKhaki
        Me.txt_Frame_Upper_Left.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.txt_Frame_Upper_Left.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Frame_Upper_Left.Location = New System.Drawing.Point(239, 300)
        Me.txt_Frame_Upper_Left.Name = "txt_Frame_Upper_Left"
        Me.txt_Frame_Upper_Left.Size = New System.Drawing.Size(119, 19)
        Me.txt_Frame_Upper_Left.TabIndex = 100
        Me.txt_Frame_Upper_Left.Text = "0,0"
        '
        'txt_Frame_Upper_Right
        '
        Me.txt_Frame_Upper_Right.BackColor = System.Drawing.Color.DarkKhaki
        Me.txt_Frame_Upper_Right.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.txt_Frame_Upper_Right.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Frame_Upper_Right.Location = New System.Drawing.Point(478, 300)
        Me.txt_Frame_Upper_Right.Name = "txt_Frame_Upper_Right"
        Me.txt_Frame_Upper_Right.Size = New System.Drawing.Size(119, 19)
        Me.txt_Frame_Upper_Right.TabIndex = 99
        Me.txt_Frame_Upper_Right.Text = "0,0"
        Me.txt_Frame_Upper_Right.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label77
        '
        Me.Label77.AutoSize = True
        Me.Label77.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label77.Location = New System.Drawing.Point(37, 81)
        Me.Label77.Name = "Label77"
        Me.Label77.Size = New System.Drawing.Size(57, 20)
        Me.Label77.TabIndex = 98
        Me.Label77.Text = "Safe Z"
        '
        'txt_Frame_Safe_Z
        '
        Me.txt_Frame_Safe_Z.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Frame_Safe_Z.Location = New System.Drawing.Point(31, 104)
        Me.txt_Frame_Safe_Z.Name = "txt_Frame_Safe_Z"
        Me.txt_Frame_Safe_Z.Size = New System.Drawing.Size(69, 26)
        Me.txt_Frame_Safe_Z.TabIndex = 97
        Me.txt_Frame_Safe_Z.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label76
        '
        Me.Label76.AutoSize = True
        Me.Label76.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label76.Location = New System.Drawing.Point(232, 13)
        Me.Label76.Name = "Label76"
        Me.Label76.Size = New System.Drawing.Size(82, 20)
        Me.Label76.TabIndex = 96
        Me.Label76.Text = "Cut Depth"
        '
        'txt_Frame_Cut_Depth
        '
        Me.txt_Frame_Cut_Depth.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Frame_Cut_Depth.Location = New System.Drawing.Point(239, 36)
        Me.txt_Frame_Cut_Depth.Name = "txt_Frame_Cut_Depth"
        Me.txt_Frame_Cut_Depth.Size = New System.Drawing.Size(69, 26)
        Me.txt_Frame_Cut_Depth.TabIndex = 95
        Me.txt_Frame_Cut_Depth.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label75
        '
        Me.Label75.AutoSize = True
        Me.Label75.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label75.Location = New System.Drawing.Point(123, 13)
        Me.Label75.Name = "Label75"
        Me.Label75.Size = New System.Drawing.Size(85, 20)
        Me.Label75.TabIndex = 94
        Me.Label75.Text = "Feed Rate"
        '
        'txt_Frame_Feedrate
        '
        Me.txt_Frame_Feedrate.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Frame_Feedrate.Location = New System.Drawing.Point(131, 36)
        Me.txt_Frame_Feedrate.Name = "txt_Frame_Feedrate"
        Me.txt_Frame_Feedrate.Size = New System.Drawing.Size(69, 26)
        Me.txt_Frame_Feedrate.TabIndex = 93
        Me.txt_Frame_Feedrate.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label72
        '
        Me.Label72.AutoSize = True
        Me.Label72.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label72.Location = New System.Drawing.Point(11, 13)
        Me.Label72.Name = "Label72"
        Me.Label72.Size = New System.Drawing.Size(108, 20)
        Me.Label72.TabIndex = 92
        Me.Label72.Text = "Tool Diameter"
        '
        'txt_Frame_Tool_Diameter
        '
        Me.txt_Frame_Tool_Diameter.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Frame_Tool_Diameter.Location = New System.Drawing.Point(31, 36)
        Me.txt_Frame_Tool_Diameter.Name = "txt_Frame_Tool_Diameter"
        Me.txt_Frame_Tool_Diameter.Size = New System.Drawing.Size(69, 26)
        Me.txt_Frame_Tool_Diameter.TabIndex = 91
        Me.txt_Frame_Tool_Diameter.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Frame_Rough_Cut_Fractional
        '
        Me.txt_Frame_Rough_Cut_Fractional.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Frame_Rough_Cut_Fractional.Location = New System.Drawing.Point(388, 481)
        Me.txt_Frame_Rough_Cut_Fractional.Name = "txt_Frame_Rough_Cut_Fractional"
        Me.txt_Frame_Rough_Cut_Fractional.ReadOnly = True
        Me.txt_Frame_Rough_Cut_Fractional.Size = New System.Drawing.Size(81, 26)
        Me.txt_Frame_Rough_Cut_Fractional.TabIndex = 90
        Me.txt_Frame_Rough_Cut_Fractional.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Frame_Frame_Width_Fractional
        '
        Me.txt_Frame_Frame_Width_Fractional.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Frame_Frame_Width_Fractional.Location = New System.Drawing.Point(388, 434)
        Me.txt_Frame_Frame_Width_Fractional.Name = "txt_Frame_Frame_Width_Fractional"
        Me.txt_Frame_Frame_Width_Fractional.ReadOnly = True
        Me.txt_Frame_Frame_Width_Fractional.Size = New System.Drawing.Size(81, 26)
        Me.txt_Frame_Frame_Width_Fractional.TabIndex = 89
        Me.txt_Frame_Frame_Width_Fractional.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Frame_Rail_Width_Fractional
        '
        Me.txt_Frame_Rail_Width_Fractional.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Frame_Rail_Width_Fractional.Location = New System.Drawing.Point(388, 364)
        Me.txt_Frame_Rail_Width_Fractional.Name = "txt_Frame_Rail_Width_Fractional"
        Me.txt_Frame_Rail_Width_Fractional.Size = New System.Drawing.Size(81, 26)
        Me.txt_Frame_Rail_Width_Fractional.TabIndex = 88
        Me.txt_Frame_Rail_Width_Fractional.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Frame_Mat_Width_Fractional
        '
        Me.txt_Frame_Mat_Width_Fractional.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Frame_Mat_Width_Fractional.Location = New System.Drawing.Point(370, 104)
        Me.txt_Frame_Mat_Width_Fractional.Name = "txt_Frame_Mat_Width_Fractional"
        Me.txt_Frame_Mat_Width_Fractional.Size = New System.Drawing.Size(81, 26)
        Me.txt_Frame_Mat_Width_Fractional.TabIndex = 87
        Me.txt_Frame_Mat_Width_Fractional.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label43
        '
        Me.Label43.AutoSize = True
        Me.Label43.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label43.Location = New System.Drawing.Point(410, 295)
        Me.Label43.Name = "Label43"
        Me.Label43.Size = New System.Drawing.Size(41, 20)
        Me.Label43.TabIndex = 86
        Me.Label43.Text = "1/4 """
        '
        'txt_Frame_Rough_Cut
        '
        Me.txt_Frame_Rough_Cut.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Frame_Rough_Cut.Location = New System.Drawing.Point(476, 481)
        Me.txt_Frame_Rough_Cut.Name = "txt_Frame_Rough_Cut"
        Me.txt_Frame_Rough_Cut.ReadOnly = True
        Me.txt_Frame_Rough_Cut.Size = New System.Drawing.Size(66, 26)
        Me.txt_Frame_Rough_Cut.TabIndex = 84
        Me.txt_Frame_Rough_Cut.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label46
        '
        Me.Label46.AutoSize = True
        Me.Label46.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label46.Location = New System.Drawing.Point(299, 484)
        Me.Label46.Name = "Label46"
        Me.Label46.Size = New System.Drawing.Size(86, 20)
        Me.Label46.TabIndex = 83
        Me.Label46.Text = "Rough Cut"
        '
        'lab_Frame_Lower_Left
        '
        Me.lab_Frame_Lower_Left.AutoSize = True
        Me.lab_Frame_Lower_Left.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lab_Frame_Lower_Left.Location = New System.Drawing.Point(118, 426)
        Me.lab_Frame_Lower_Left.Name = "lab_Frame_Lower_Left"
        Me.lab_Frame_Lower_Left.Size = New System.Drawing.Size(31, 20)
        Me.lab_Frame_Lower_Left.TabIndex = 79
        Me.lab_Frame_Lower_Left.Text = "0,0"
        Me.lab_Frame_Lower_Left.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'txt_Frame_Mat_Width
        '
        Me.txt_Frame_Mat_Width.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Frame_Mat_Width.Location = New System.Drawing.Point(458, 104)
        Me.txt_Frame_Mat_Width.Name = "txt_Frame_Mat_Width"
        Me.txt_Frame_Mat_Width.Size = New System.Drawing.Size(66, 26)
        Me.txt_Frame_Mat_Width.TabIndex = 78
        Me.txt_Frame_Mat_Width.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label34
        '
        Me.Label34.AutoSize = True
        Me.Label34.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label34.Location = New System.Drawing.Point(286, 107)
        Me.Label34.Name = "Label34"
        Me.Label34.Size = New System.Drawing.Size(81, 20)
        Me.Label34.TabIndex = 77
        Me.Label34.Text = "Mat Width"
        '
        'txt_Frame_Frame_Width
        '
        Me.txt_Frame_Frame_Width.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Frame_Frame_Width.Location = New System.Drawing.Point(476, 434)
        Me.txt_Frame_Frame_Width.Name = "txt_Frame_Frame_Width"
        Me.txt_Frame_Frame_Width.ReadOnly = True
        Me.txt_Frame_Frame_Width.Size = New System.Drawing.Size(66, 26)
        Me.txt_Frame_Frame_Width.TabIndex = 74
        Me.txt_Frame_Frame_Width.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label31
        '
        Me.Label31.AutoSize = True
        Me.Label31.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label31.Location = New System.Drawing.Point(303, 367)
        Me.Label31.Name = "Label31"
        Me.Label31.Size = New System.Drawing.Size(81, 20)
        Me.Label31.TabIndex = 73
        Me.Label31.Text = "Rail Width"
        '
        'Label30
        '
        Me.Label30.AutoSize = True
        Me.Label30.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label30.Location = New System.Drawing.Point(279, 437)
        Me.Label30.Name = "Label30"
        Me.Label30.Size = New System.Drawing.Size(100, 20)
        Me.Label30.TabIndex = 71
        Me.Label30.Text = "Frame Width"
        '
        'txt_Frame_Rail_Width
        '
        Me.txt_Frame_Rail_Width.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Frame_Rail_Width.Location = New System.Drawing.Point(476, 364)
        Me.txt_Frame_Rail_Width.Name = "txt_Frame_Rail_Width"
        Me.txt_Frame_Rail_Width.Size = New System.Drawing.Size(66, 26)
        Me.txt_Frame_Rail_Width.TabIndex = 70
        Me.txt_Frame_Rail_Width.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'ShapeContainer5
        '
        Me.ShapeContainer5.Location = New System.Drawing.Point(0, 0)
        Me.ShapeContainer5.Margin = New System.Windows.Forms.Padding(0)
        Me.ShapeContainer5.Name = "ShapeContainer5"
        Me.ShapeContainer5.Shapes.AddRange(New Microsoft.VisualBasic.PowerPacks.Shape() {Me.LineShape15, Me.RectangleShape2, Me.LineShape14, Me.LineShape13, Me.LineShape12, Me.LineShape11})
        Me.ShapeContainer5.Size = New System.Drawing.Size(878, 697)
        Me.ShapeContainer5.TabIndex = 72
        Me.ShapeContainer5.TabStop = False
        '
        'LineShape15
        '
        Me.LineShape15.Name = "LineShape15"
        Me.LineShape15.X1 = 426
        Me.LineShape15.X2 = 406
        Me.LineShape15.Y1 = 310
        Me.LineShape15.Y2 = 327
        '
        'RectangleShape2
        '
        Me.RectangleShape2.Location = New System.Drawing.Point(229, 154)
        Me.RectangleShape2.Name = "RectangleShape2"
        Me.RectangleShape2.Size = New System.Drawing.Size(374, 178)
        '
        'LineShape14
        '
        Me.LineShape14.Name = "LineShape14"
        Me.LineShape14.X1 = 140
        Me.LineShape14.X2 = 690
        Me.LineShape14.Y1 = 421
        Me.LineShape14.Y2 = 421
        '
        'LineShape13
        '
        Me.LineShape13.Name = "LineShape13"
        Me.LineShape13.X1 = 239
        Me.LineShape13.X2 = 595
        Me.LineShape13.Y1 = 323
        Me.LineShape13.Y2 = 323
        '
        'LineShape12
        '
        Me.LineShape12.BorderColor = System.Drawing.Color.Red
        Me.LineShape12.Name = "LineShape12"
        Me.LineShape12.X1 = 691
        Me.LineShape12.X2 = 595
        Me.LineShape12.Y1 = 420
        Me.LineShape12.Y2 = 323
        '
        'LineShape11
        '
        Me.LineShape11.BorderColor = System.Drawing.Color.Red
        Me.LineShape11.Name = "LineShape11"
        Me.LineShape11.X1 = 238
        Me.LineShape11.X2 = 141
        Me.LineShape11.Y1 = 323
        Me.LineShape11.Y2 = 419
        '
        'tab_Circle
        '
        Me.tab_Circle.BackColor = System.Drawing.Color.DarkKhaki
        Me.tab_Circle.Controls.Add(Me.Label52)
        Me.tab_Circle.Controls.Add(Me.txt_Circle_Plunge_Rate)
        Me.tab_Circle.Controls.Add(Me.Label51)
        Me.tab_Circle.Controls.Add(Me.txt_Circle_Safe_Z)
        Me.tab_Circle.Controls.Add(Me.Label50)
        Me.tab_Circle.Controls.Add(Me.txt_Circle_Start_Depth)
        Me.tab_Circle.Controls.Add(Me.Label45)
        Me.tab_Circle.Controls.Add(Me.txt_Circle_Cut_Depth)
        Me.tab_Circle.Controls.Add(Me.Label47)
        Me.tab_Circle.Controls.Add(Me.txt_Circle_Pass_Depth)
        Me.tab_Circle.Controls.Add(Me.Label48)
        Me.tab_Circle.Controls.Add(Me.txt_Circle_Feed_Rate)
        Me.tab_Circle.Controls.Add(Me.Label49)
        Me.tab_Circle.Controls.Add(Me.txt_Circle_Tool_Diameter)
        Me.tab_Circle.Controls.Add(Me.Label32)
        Me.tab_Circle.Controls.Add(Me.txt_Circle_Diameter)
        Me.tab_Circle.Controls.Add(Me.Label42)
        Me.tab_Circle.Controls.Add(Me.Label44)
        Me.tab_Circle.Controls.Add(Me.txt_Circle_Center_Y)
        Me.tab_Circle.Controls.Add(Me.txt_Circle_Center_X)
        Me.tab_Circle.Controls.Add(Me.ShapeContainer6)
        Me.tab_Circle.Location = New System.Drawing.Point(4, 27)
        Me.tab_Circle.Name = "tab_Circle"
        Me.tab_Circle.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_Circle.Size = New System.Drawing.Size(878, 697)
        Me.tab_Circle.TabIndex = 6
        Me.tab_Circle.Text = "Circle"
        '
        'Label52
        '
        Me.Label52.AutoSize = True
        Me.Label52.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label52.Location = New System.Drawing.Point(245, 15)
        Me.Label52.Name = "Label52"
        Me.Label52.Size = New System.Drawing.Size(97, 20)
        Me.Label52.TabIndex = 46
        Me.Label52.Text = "Plunge Rate"
        '
        'txt_Circle_Plunge_Rate
        '
        Me.txt_Circle_Plunge_Rate.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Circle_Plunge_Rate.Location = New System.Drawing.Point(259, 38)
        Me.txt_Circle_Plunge_Rate.Name = "txt_Circle_Plunge_Rate"
        Me.txt_Circle_Plunge_Rate.Size = New System.Drawing.Size(69, 26)
        Me.txt_Circle_Plunge_Rate.TabIndex = 45
        Me.txt_Circle_Plunge_Rate.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label51
        '
        Me.Label51.AutoSize = True
        Me.Label51.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label51.Location = New System.Drawing.Point(33, 79)
        Me.Label51.Name = "Label51"
        Me.Label51.Size = New System.Drawing.Size(57, 20)
        Me.Label51.TabIndex = 44
        Me.Label51.Text = "Safe Z"
        '
        'txt_Circle_Safe_Z
        '
        Me.txt_Circle_Safe_Z.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Circle_Safe_Z.Location = New System.Drawing.Point(27, 102)
        Me.txt_Circle_Safe_Z.Name = "txt_Circle_Safe_Z"
        Me.txt_Circle_Safe_Z.Size = New System.Drawing.Size(69, 26)
        Me.txt_Circle_Safe_Z.TabIndex = 43
        Me.txt_Circle_Safe_Z.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label50
        '
        Me.Label50.AutoSize = True
        Me.Label50.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label50.Location = New System.Drawing.Point(363, 15)
        Me.Label50.Name = "Label50"
        Me.Label50.Size = New System.Drawing.Size(92, 20)
        Me.Label50.TabIndex = 38
        Me.Label50.Text = "Start Depth"
        '
        'txt_Circle_Start_Depth
        '
        Me.txt_Circle_Start_Depth.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Circle_Start_Depth.Location = New System.Drawing.Point(370, 38)
        Me.txt_Circle_Start_Depth.Name = "txt_Circle_Start_Depth"
        Me.txt_Circle_Start_Depth.Size = New System.Drawing.Size(69, 26)
        Me.txt_Circle_Start_Depth.TabIndex = 37
        Me.txt_Circle_Start_Depth.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label45
        '
        Me.Label45.AutoSize = True
        Me.Label45.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label45.Location = New System.Drawing.Point(591, 15)
        Me.Label45.Name = "Label45"
        Me.Label45.Size = New System.Drawing.Size(82, 20)
        Me.Label45.TabIndex = 36
        Me.Label45.Text = "Cut Depth"
        '
        'txt_Circle_Cut_Depth
        '
        Me.txt_Circle_Cut_Depth.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Circle_Cut_Depth.Location = New System.Drawing.Point(598, 38)
        Me.txt_Circle_Cut_Depth.Name = "txt_Circle_Cut_Depth"
        Me.txt_Circle_Cut_Depth.Size = New System.Drawing.Size(69, 26)
        Me.txt_Circle_Cut_Depth.TabIndex = 33
        Me.txt_Circle_Cut_Depth.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label47
        '
        Me.Label47.AutoSize = True
        Me.Label47.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label47.Location = New System.Drawing.Point(473, 15)
        Me.Label47.Name = "Label47"
        Me.Label47.Size = New System.Drawing.Size(92, 20)
        Me.Label47.TabIndex = 35
        Me.Label47.Text = "Pass Depth"
        '
        'txt_Circle_Pass_Depth
        '
        Me.txt_Circle_Pass_Depth.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Circle_Pass_Depth.Location = New System.Drawing.Point(485, 38)
        Me.txt_Circle_Pass_Depth.Name = "txt_Circle_Pass_Depth"
        Me.txt_Circle_Pass_Depth.Size = New System.Drawing.Size(69, 26)
        Me.txt_Circle_Pass_Depth.TabIndex = 32
        Me.txt_Circle_Pass_Depth.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label48
        '
        Me.Label48.AutoSize = True
        Me.Label48.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label48.Location = New System.Drawing.Point(132, 15)
        Me.Label48.Name = "Label48"
        Me.Label48.Size = New System.Drawing.Size(85, 20)
        Me.Label48.TabIndex = 34
        Me.Label48.Text = "Feed Rate"
        '
        'txt_Circle_Feed_Rate
        '
        Me.txt_Circle_Feed_Rate.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Circle_Feed_Rate.Location = New System.Drawing.Point(140, 38)
        Me.txt_Circle_Feed_Rate.Name = "txt_Circle_Feed_Rate"
        Me.txt_Circle_Feed_Rate.Size = New System.Drawing.Size(69, 26)
        Me.txt_Circle_Feed_Rate.TabIndex = 30
        Me.txt_Circle_Feed_Rate.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label49
        '
        Me.Label49.AutoSize = True
        Me.Label49.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label49.Location = New System.Drawing.Point(7, 15)
        Me.Label49.Name = "Label49"
        Me.Label49.Size = New System.Drawing.Size(108, 20)
        Me.Label49.TabIndex = 31
        Me.Label49.Text = "Tool Diameter"
        '
        'txt_Circle_Tool_Diameter
        '
        Me.txt_Circle_Tool_Diameter.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Circle_Tool_Diameter.Location = New System.Drawing.Point(27, 38)
        Me.txt_Circle_Tool_Diameter.Name = "txt_Circle_Tool_Diameter"
        Me.txt_Circle_Tool_Diameter.Size = New System.Drawing.Size(69, 26)
        Me.txt_Circle_Tool_Diameter.TabIndex = 29
        Me.txt_Circle_Tool_Diameter.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label32
        '
        Me.Label32.AutoSize = True
        Me.Label32.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label32.Location = New System.Drawing.Point(484, 231)
        Me.Label32.Name = "Label32"
        Me.Label32.Size = New System.Drawing.Size(74, 20)
        Me.Label32.TabIndex = 28
        Me.Label32.Text = "Diameter"
        '
        'txt_Circle_Diameter
        '
        Me.txt_Circle_Diameter.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Circle_Diameter.Location = New System.Drawing.Point(410, 228)
        Me.txt_Circle_Diameter.Name = "txt_Circle_Diameter"
        Me.txt_Circle_Diameter.Size = New System.Drawing.Size(69, 26)
        Me.txt_Circle_Diameter.TabIndex = 25
        Me.txt_Circle_Diameter.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label42
        '
        Me.Label42.AutoSize = True
        Me.Label42.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label42.Location = New System.Drawing.Point(81, 299)
        Me.Label42.Name = "Label42"
        Me.Label42.Size = New System.Drawing.Size(20, 20)
        Me.Label42.TabIndex = 27
        Me.Label42.Text = "Y"
        '
        'Label44
        '
        Me.Label44.AutoSize = True
        Me.Label44.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label44.Location = New System.Drawing.Point(202, 389)
        Me.Label44.Name = "Label44"
        Me.Label44.Size = New System.Drawing.Size(20, 20)
        Me.Label44.TabIndex = 26
        Me.Label44.Text = "X"
        '
        'txt_Circle_Center_Y
        '
        Me.txt_Circle_Center_Y.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Circle_Center_Y.Location = New System.Drawing.Point(105, 296)
        Me.txt_Circle_Center_Y.Name = "txt_Circle_Center_Y"
        Me.txt_Circle_Center_Y.Size = New System.Drawing.Size(69, 26)
        Me.txt_Circle_Center_Y.TabIndex = 23
        Me.txt_Circle_Center_Y.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Circle_Center_X
        '
        Me.txt_Circle_Center_X.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Circle_Center_X.Location = New System.Drawing.Point(178, 360)
        Me.txt_Circle_Center_X.Name = "txt_Circle_Center_X"
        Me.txt_Circle_Center_X.Size = New System.Drawing.Size(69, 26)
        Me.txt_Circle_Center_X.TabIndex = 24
        Me.txt_Circle_Center_X.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'ShapeContainer6
        '
        Me.ShapeContainer6.Location = New System.Drawing.Point(3, 3)
        Me.ShapeContainer6.Margin = New System.Windows.Forms.Padding(0)
        Me.ShapeContainer6.Name = "ShapeContainer6"
        Me.ShapeContainer6.Shapes.AddRange(New Microsoft.VisualBasic.PowerPacks.Shape() {Me.LineShape22, Me.OvalShape6, Me.LineShape21, Me.LineShape20, Me.LineShape19, Me.LineShape18, Me.LineShape17, Me.LineShape16, Me.OvalShape5, Me.OvalShape4})
        Me.ShapeContainer6.Size = New System.Drawing.Size(872, 691)
        Me.ShapeContainer6.TabIndex = 0
        Me.ShapeContainer6.TabStop = False
        '
        'LineShape22
        '
        Me.LineShape22.BorderColor = System.Drawing.Color.Gray
        Me.LineShape22.Name = "LineShape22"
        Me.LineShape22.X1 = 137
        Me.LineShape22.X2 = 137
        Me.LineShape22.Y1 = 236
        Me.LineShape22.Y2 = 379
        '
        'OvalShape6
        '
        Me.OvalShape6.Location = New System.Drawing.Point(186, 136)
        Me.OvalShape6.Name = "OvalShape6"
        Me.OvalShape6.Size = New System.Drawing.Size(200, 200)
        '
        'LineShape21
        '
        Me.LineShape21.Name = "LineShape21"
        Me.LineShape21.X1 = 137
        Me.LineShape21.X2 = 307
        Me.LineShape21.Y1 = 236
        Me.LineShape21.Y2 = 236
        '
        'LineShape20
        '
        Me.LineShape20.Name = "LineShape20"
        Me.LineShape20.X1 = 285
        Me.LineShape20.X2 = 286
        Me.LineShape20.Y1 = 222
        Me.LineShape20.Y2 = 388
        '
        'LineShape19
        '
        Me.LineShape19.BorderColor = System.Drawing.Color.Gray
        Me.LineShape19.Name = "LineShape19"
        Me.LineShape19.X1 = 125
        Me.LineShape19.X2 = 286
        Me.LineShape19.Y1 = 369
        Me.LineShape19.Y2 = 369
        '
        'LineShape18
        '
        Me.LineShape18.BorderColor = System.Drawing.Color.Gray
        Me.LineShape18.Name = "LineShape18"
        Me.LineShape18.X1 = 286
        Me.LineShape18.X2 = 469
        Me.LineShape18.Y1 = 135
        Me.LineShape18.Y2 = 135
        '
        'LineShape17
        '
        Me.LineShape17.BorderColor = System.Drawing.Color.Gray
        Me.LineShape17.Name = "LineShape17"
        Me.LineShape17.X1 = 283
        Me.LineShape17.X2 = 469
        Me.LineShape17.Y1 = 337
        Me.LineShape17.Y2 = 337
        '
        'LineShape16
        '
        Me.LineShape16.Name = "LineShape16"
        Me.LineShape16.X1 = 444
        Me.LineShape16.X2 = 444
        Me.LineShape16.Y1 = 135
        Me.LineShape16.Y2 = 336
        '
        'OvalShape5
        '
        Me.OvalShape5.Location = New System.Drawing.Point(268, 137)
        Me.OvalShape5.Name = "OvalShape5"
        Me.OvalShape5.Size = New System.Drawing.Size(36, 36)
        '
        'OvalShape4
        '
        Me.OvalShape4.Location = New System.Drawing.Point(268, 99)
        Me.OvalShape4.Name = "OvalShape4"
        Me.OvalShape4.Size = New System.Drawing.Size(36, 36)
        '
        'tab_Arc
        '
        Me.tab_Arc.AccessibleDescription = "aaaa"
        Me.tab_Arc.BackColor = System.Drawing.Color.DarkKhaki
        Me.tab_Arc.Controls.Add(Me.Label58)
        Me.tab_Arc.Controls.Add(Me.txt_Arc_Block)
        Me.tab_Arc.Controls.Add(Me.Label57)
        Me.tab_Arc.Controls.Add(Me.txt_Arc_Radius)
        Me.tab_Arc.Controls.Add(Me.Label56)
        Me.tab_Arc.Controls.Add(Me.TextBox9)
        Me.tab_Arc.Controls.Add(Me.Label55)
        Me.tab_Arc.Controls.Add(Me.TextBox4)
        Me.tab_Arc.Controls.Add(Me.Label54)
        Me.tab_Arc.Controls.Add(Me.Label67)
        Me.tab_Arc.Controls.Add(Me.Label74)
        Me.tab_Arc.Controls.Add(Me.txt_Arc_I)
        Me.tab_Arc.Controls.Add(Me.txt_Arc_J)
        Me.tab_Arc.Controls.Add(Me.Label66)
        Me.tab_Arc.Controls.Add(Me.Label64)
        Me.tab_Arc.Controls.Add(Me.Label73)
        Me.tab_Arc.Controls.Add(Me.lab_Arc_Tooi_Outside_Direction)
        Me.tab_Arc.Controls.Add(Me.lab_Arc_Tooi_Inside_Direction)
        Me.tab_Arc.Controls.Add(Me.Label61)
        Me.tab_Arc.Controls.Add(Me.txt_Arc_Diameter)
        Me.tab_Arc.Controls.Add(Me.Label71)
        Me.tab_Arc.Controls.Add(Me.Label70)
        Me.tab_Arc.Controls.Add(Me.cmb_Arc_Tool_Direction)
        Me.tab_Arc.Controls.Add(Me.cmb_Arc_Tool_Position)
        Me.tab_Arc.Controls.Add(Me.Label68)
        Me.tab_Arc.Controls.Add(Me.txt_Arc_Start_Angle)
        Me.tab_Arc.Controls.Add(Me.Label69)
        Me.tab_Arc.Controls.Add(Me.txt_Arc_End_Angle)
        Me.tab_Arc.Controls.Add(Me.txt_Arc_Start_X)
        Me.tab_Arc.Controls.Add(Me.txt_Arc_End_X)
        Me.tab_Arc.Controls.Add(Me.Label65)
        Me.tab_Arc.Controls.Add(Me.txt_Arc_Start_Y)
        Me.tab_Arc.Controls.Add(Me.txt_Arc_End_Y)
        Me.tab_Arc.Controls.Add(Me.Label59)
        Me.tab_Arc.Controls.Add(Me.txt_Arc_Feed_Rate)
        Me.tab_Arc.Controls.Add(Me.Label60)
        Me.tab_Arc.Controls.Add(Me.txt_Arc_Tool_Diameter)
        Me.tab_Arc.Controls.Add(Me.Label62)
        Me.tab_Arc.Controls.Add(Me.Label63)
        Me.tab_Arc.Controls.Add(Me.txt_Arc_Center_Y)
        Me.tab_Arc.Controls.Add(Me.txt_Arc_Center_X)
        Me.tab_Arc.Controls.Add(Me.ShapeContainer7)
        Me.tab_Arc.Location = New System.Drawing.Point(4, 27)
        Me.tab_Arc.Name = "tab_Arc"
        Me.tab_Arc.Size = New System.Drawing.Size(878, 697)
        Me.tab_Arc.TabIndex = 8
        Me.tab_Arc.Text = "Arc"
        '
        'Label58
        '
        Me.Label58.AutoSize = True
        Me.Label58.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label58.Location = New System.Drawing.Point(358, 445)
        Me.Label58.Name = "Label58"
        Me.Label58.Size = New System.Drawing.Size(48, 20)
        Me.Label58.TabIndex = 106
        Me.Label58.Text = "Block"
        '
        'txt_Arc_Block
        '
        Me.txt_Arc_Block.BackColor = System.Drawing.Color.White
        Me.txt_Arc_Block.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Arc_Block.Location = New System.Drawing.Point(362, 467)
        Me.txt_Arc_Block.Name = "txt_Arc_Block"
        Me.txt_Arc_Block.ReadOnly = True
        Me.txt_Arc_Block.Size = New System.Drawing.Size(346, 26)
        Me.txt_Arc_Block.TabIndex = 105
        Me.txt_Arc_Block.TabStop = False
        '
        'Label57
        '
        Me.Label57.AutoSize = True
        Me.Label57.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label57.Location = New System.Drawing.Point(636, 282)
        Me.Label57.Name = "Label57"
        Me.Label57.Size = New System.Drawing.Size(59, 20)
        Me.Label57.TabIndex = 104
        Me.Label57.Text = "Radius"
        '
        'txt_Arc_Radius
        '
        Me.txt_Arc_Radius.BackColor = System.Drawing.Color.White
        Me.txt_Arc_Radius.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Arc_Radius.Location = New System.Drawing.Point(631, 303)
        Me.txt_Arc_Radius.Name = "txt_Arc_Radius"
        Me.txt_Arc_Radius.ReadOnly = True
        Me.txt_Arc_Radius.Size = New System.Drawing.Size(69, 26)
        Me.txt_Arc_Radius.TabIndex = 103
        Me.txt_Arc_Radius.TabStop = False
        Me.txt_Arc_Radius.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label56
        '
        Me.Label56.AutoSize = True
        Me.Label56.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label56.Location = New System.Drawing.Point(492, 20)
        Me.Label56.Name = "Label56"
        Me.Label56.Size = New System.Drawing.Size(118, 20)
        Me.Label56.TabIndex = 102
        Me.Label56.Text = "Spiral Diameter"
        '
        'TextBox9
        '
        Me.TextBox9.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.TextBox9.Location = New System.Drawing.Point(517, 43)
        Me.TextBox9.Name = "TextBox9"
        Me.TextBox9.Size = New System.Drawing.Size(69, 26)
        Me.TextBox9.TabIndex = 5
        Me.TextBox9.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label55
        '
        Me.Label55.AutoSize = True
        Me.Label55.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label55.Location = New System.Drawing.Point(392, 20)
        Me.Label55.Name = "Label55"
        Me.Label55.Size = New System.Drawing.Size(91, 20)
        Me.Label55.TabIndex = 100
        Me.Label55.Text = "Helix Depth"
        '
        'TextBox4
        '
        Me.TextBox4.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.TextBox4.Location = New System.Drawing.Point(400, 43)
        Me.TextBox4.Name = "TextBox4"
        Me.TextBox4.Size = New System.Drawing.Size(69, 26)
        Me.TextBox4.TabIndex = 4
        Me.TextBox4.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label54
        '
        Me.Label54.AutoSize = True
        Me.Label54.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label54.Location = New System.Drawing.Point(422, 189)
        Me.Label54.Name = "Label54"
        Me.Label54.Size = New System.Drawing.Size(255, 40)
        Me.Label54.TabIndex = 98
        Me.Label54.Text = "Changing a blue box wll recalculate" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "the yellow boxes and vice versa"
        '
        'Label67
        '
        Me.Label67.AccessibleDescription = "a"
        Me.Label67.AutoSize = True
        Me.Label67.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label67.ForeColor = System.Drawing.Color.Black
        Me.Label67.Location = New System.Drawing.Point(470, 93)
        Me.Label67.Name = "Label67"
        Me.Label67.Size = New System.Drawing.Size(17, 20)
        Me.Label67.TabIndex = 97
        Me.Label67.Text = "J"
        '
        'Label74
        '
        Me.Label74.AccessibleDescription = "a"
        Me.Label74.AutoSize = True
        Me.Label74.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label74.ForeColor = System.Drawing.Color.Black
        Me.Label74.Location = New System.Drawing.Point(389, 93)
        Me.Label74.Name = "Label74"
        Me.Label74.Size = New System.Drawing.Size(14, 20)
        Me.Label74.TabIndex = 96
        Me.Label74.Text = "I"
        '
        'txt_Arc_I
        '
        Me.txt_Arc_I.AccessibleDescription = "a"
        Me.txt_Arc_I.BackColor = System.Drawing.Color.AliceBlue
        Me.txt_Arc_I.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Arc_I.Location = New System.Drawing.Point(362, 115)
        Me.txt_Arc_I.Name = "txt_Arc_I"
        Me.txt_Arc_I.Size = New System.Drawing.Size(69, 26)
        Me.txt_Arc_I.TabIndex = 10
        Me.txt_Arc_I.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Arc_J
        '
        Me.txt_Arc_J.AccessibleDescription = "a"
        Me.txt_Arc_J.BackColor = System.Drawing.Color.AliceBlue
        Me.txt_Arc_J.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Arc_J.Location = New System.Drawing.Point(444, 115)
        Me.txt_Arc_J.Name = "txt_Arc_J"
        Me.txt_Arc_J.Size = New System.Drawing.Size(69, 26)
        Me.txt_Arc_J.TabIndex = 11
        Me.txt_Arc_J.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label66
        '
        Me.Label66.AccessibleDescription = "a"
        Me.Label66.AutoSize = True
        Me.Label66.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label66.ForeColor = System.Drawing.Color.Firebrick
        Me.Label66.Location = New System.Drawing.Point(285, 93)
        Me.Label66.Name = "Label66"
        Me.Label66.Size = New System.Drawing.Size(58, 20)
        Me.Label66.TabIndex = 93
        Me.Label66.Text = "End_Y"
        '
        'Label64
        '
        Me.Label64.AccessibleDescription = "a"
        Me.Label64.AutoSize = True
        Me.Label64.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label64.ForeColor = System.Drawing.Color.Firebrick
        Me.Label64.Location = New System.Drawing.Point(206, 93)
        Me.Label64.Name = "Label64"
        Me.Label64.Size = New System.Drawing.Size(53, 20)
        Me.Label64.TabIndex = 92
        Me.Label64.Text = "End X"
        '
        'Label73
        '
        Me.Label73.AccessibleDescription = "a"
        Me.Label73.AutoSize = True
        Me.Label73.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label73.ForeColor = System.Drawing.Color.DarkGreen
        Me.Label73.Location = New System.Drawing.Point(121, 93)
        Me.Label73.Name = "Label73"
        Me.Label73.Size = New System.Drawing.Size(59, 20)
        Me.Label73.TabIndex = 91
        Me.Label73.Text = "Start Y"
        '
        'lab_Arc_Tooi_Outside_Direction
        '
        Me.lab_Arc_Tooi_Outside_Direction.AccessibleDescription = "a"
        Me.lab_Arc_Tooi_Outside_Direction.AutoSize = True
        Me.lab_Arc_Tooi_Outside_Direction.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lab_Arc_Tooi_Outside_Direction.Location = New System.Drawing.Point(219, 189)
        Me.lab_Arc_Tooi_Outside_Direction.Name = "lab_Arc_Tooi_Outside_Direction"
        Me.lab_Arc_Tooi_Outside_Direction.Size = New System.Drawing.Size(18, 20)
        Me.lab_Arc_Tooi_Outside_Direction.TabIndex = 88
        Me.lab_Arc_Tooi_Outside_Direction.Text = "<"
        '
        'lab_Arc_Tooi_Inside_Direction
        '
        Me.lab_Arc_Tooi_Inside_Direction.AccessibleDescription = "a"
        Me.lab_Arc_Tooi_Inside_Direction.AutoSize = True
        Me.lab_Arc_Tooi_Inside_Direction.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lab_Arc_Tooi_Inside_Direction.Location = New System.Drawing.Point(219, 227)
        Me.lab_Arc_Tooi_Inside_Direction.Name = "lab_Arc_Tooi_Inside_Direction"
        Me.lab_Arc_Tooi_Inside_Direction.Size = New System.Drawing.Size(18, 20)
        Me.lab_Arc_Tooi_Inside_Direction.TabIndex = 87
        Me.lab_Arc_Tooi_Inside_Direction.Text = ">"
        '
        'Label61
        '
        Me.Label61.AutoSize = True
        Me.Label61.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label61.Location = New System.Drawing.Point(354, 282)
        Me.Label61.Name = "Label61"
        Me.Label61.Size = New System.Drawing.Size(74, 20)
        Me.Label61.TabIndex = 86
        Me.Label61.Text = "Diameter"
        '
        'txt_Arc_Diameter
        '
        Me.txt_Arc_Diameter.BackColor = System.Drawing.Color.PaleGoldenrod
        Me.txt_Arc_Diameter.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Arc_Diameter.Location = New System.Drawing.Point(357, 303)
        Me.txt_Arc_Diameter.Name = "txt_Arc_Diameter"
        Me.txt_Arc_Diameter.Size = New System.Drawing.Size(69, 26)
        Me.txt_Arc_Diameter.TabIndex = 14
        Me.txt_Arc_Diameter.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label71
        '
        Me.Label71.AccessibleDescription = " "
        Me.Label71.AutoSize = True
        Me.Label71.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label71.Location = New System.Drawing.Point(311, 21)
        Me.Label71.Name = "Label71"
        Me.Label71.Size = New System.Drawing.Size(72, 20)
        Me.Label71.TabIndex = 83
        Me.Label71.Text = "Direction"
        '
        'Label70
        '
        Me.Label70.AutoSize = True
        Me.Label70.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label70.Location = New System.Drawing.Point(201, 21)
        Me.Label70.Name = "Label70"
        Me.Label70.Size = New System.Drawing.Size(99, 20)
        Me.Label70.TabIndex = 82
        Me.Label70.Text = "Tool Position"
        '
        'cmb_Arc_Tool_Direction
        '
        Me.cmb_Arc_Tool_Direction.AccessibleDescription = " "
        Me.cmb_Arc_Tool_Direction.FormattingEnabled = True
        Me.cmb_Arc_Tool_Direction.Items.AddRange(New Object() {"CW", "CCW"})
        Me.cmb_Arc_Tool_Direction.Location = New System.Drawing.Point(312, 44)
        Me.cmb_Arc_Tool_Direction.Name = "cmb_Arc_Tool_Direction"
        Me.cmb_Arc_Tool_Direction.Size = New System.Drawing.Size(70, 26)
        Me.cmb_Arc_Tool_Direction.TabIndex = 3
        '
        'cmb_Arc_Tool_Position
        '
        Me.cmb_Arc_Tool_Position.FormattingEnabled = True
        Me.cmb_Arc_Tool_Position.Items.AddRange(New Object() {"Inside", "Outside"})
        Me.cmb_Arc_Tool_Position.Location = New System.Drawing.Point(204, 44)
        Me.cmb_Arc_Tool_Position.Name = "cmb_Arc_Tool_Position"
        Me.cmb_Arc_Tool_Position.Size = New System.Drawing.Size(92, 26)
        Me.cmb_Arc_Tool_Position.TabIndex = 2
        '
        'Label68
        '
        Me.Label68.AccessibleDescription = "a"
        Me.Label68.AutoSize = True
        Me.Label68.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label68.ForeColor = System.Drawing.Color.DarkGreen
        Me.Label68.Location = New System.Drawing.Point(435, 282)
        Me.Label68.Name = "Label68"
        Me.Label68.Size = New System.Drawing.Size(89, 20)
        Me.Label68.TabIndex = 79
        Me.Label68.Text = "Start Angle"
        '
        'txt_Arc_Start_Angle
        '
        Me.txt_Arc_Start_Angle.AccessibleDescription = "a"
        Me.txt_Arc_Start_Angle.BackColor = System.Drawing.Color.PaleGoldenrod
        Me.txt_Arc_Start_Angle.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Arc_Start_Angle.Location = New System.Drawing.Point(445, 303)
        Me.txt_Arc_Start_Angle.Name = "txt_Arc_Start_Angle"
        Me.txt_Arc_Start_Angle.Size = New System.Drawing.Size(69, 26)
        Me.txt_Arc_Start_Angle.TabIndex = 15
        Me.txt_Arc_Start_Angle.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label69
        '
        Me.Label69.AccessibleDescription = "a"
        Me.Label69.AutoSize = True
        Me.Label69.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label69.ForeColor = System.Drawing.Color.Firebrick
        Me.Label69.Location = New System.Drawing.Point(531, 282)
        Me.Label69.Name = "Label69"
        Me.Label69.Size = New System.Drawing.Size(83, 20)
        Me.Label69.TabIndex = 77
        Me.Label69.Text = "End Angle"
        '
        'txt_Arc_End_Angle
        '
        Me.txt_Arc_End_Angle.AccessibleDescription = "a"
        Me.txt_Arc_End_Angle.BackColor = System.Drawing.Color.PaleGoldenrod
        Me.txt_Arc_End_Angle.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Arc_End_Angle.Location = New System.Drawing.Point(538, 303)
        Me.txt_Arc_End_Angle.Name = "txt_Arc_End_Angle"
        Me.txt_Arc_End_Angle.Size = New System.Drawing.Size(69, 26)
        Me.txt_Arc_End_Angle.TabIndex = 16
        Me.txt_Arc_End_Angle.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Arc_Start_X
        '
        Me.txt_Arc_Start_X.AccessibleDescription = "a"
        Me.txt_Arc_Start_X.BackColor = System.Drawing.Color.AliceBlue
        Me.txt_Arc_Start_X.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Arc_Start_X.Location = New System.Drawing.Point(34, 115)
        Me.txt_Arc_Start_X.Name = "txt_Arc_Start_X"
        Me.txt_Arc_Start_X.Size = New System.Drawing.Size(69, 26)
        Me.txt_Arc_Start_X.TabIndex = 6
        Me.txt_Arc_Start_X.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Arc_End_X
        '
        Me.txt_Arc_End_X.AccessibleDescription = "a"
        Me.txt_Arc_End_X.BackColor = System.Drawing.Color.AliceBlue
        Me.txt_Arc_End_X.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Arc_End_X.Location = New System.Drawing.Point(198, 115)
        Me.txt_Arc_End_X.Name = "txt_Arc_End_X"
        Me.txt_Arc_End_X.Size = New System.Drawing.Size(69, 26)
        Me.txt_Arc_End_X.TabIndex = 8
        Me.txt_Arc_End_X.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label65
        '
        Me.Label65.AccessibleDescription = "a"
        Me.Label65.AutoSize = True
        Me.Label65.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label65.ForeColor = System.Drawing.Color.DarkGreen
        Me.Label65.Location = New System.Drawing.Point(39, 93)
        Me.Label65.Name = "Label65"
        Me.Label65.Size = New System.Drawing.Size(59, 20)
        Me.Label65.TabIndex = 71
        Me.Label65.Text = "Start X"
        '
        'txt_Arc_Start_Y
        '
        Me.txt_Arc_Start_Y.AccessibleDescription = "a"
        Me.txt_Arc_Start_Y.BackColor = System.Drawing.Color.AliceBlue
        Me.txt_Arc_Start_Y.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Arc_Start_Y.Location = New System.Drawing.Point(116, 115)
        Me.txt_Arc_Start_Y.Name = "txt_Arc_Start_Y"
        Me.txt_Arc_Start_Y.Size = New System.Drawing.Size(69, 26)
        Me.txt_Arc_Start_Y.TabIndex = 7
        Me.txt_Arc_Start_Y.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Arc_End_Y
        '
        Me.txt_Arc_End_Y.AccessibleDescription = "a"
        Me.txt_Arc_End_Y.BackColor = System.Drawing.Color.AliceBlue
        Me.txt_Arc_End_Y.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Arc_End_Y.Location = New System.Drawing.Point(280, 115)
        Me.txt_Arc_End_Y.Name = "txt_Arc_End_Y"
        Me.txt_Arc_End_Y.Size = New System.Drawing.Size(69, 26)
        Me.txt_Arc_End_Y.TabIndex = 9
        Me.txt_Arc_End_Y.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label59
        '
        Me.Label59.AutoSize = True
        Me.Label59.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label59.Location = New System.Drawing.Point(111, 21)
        Me.Label59.Name = "Label59"
        Me.Label59.Size = New System.Drawing.Size(85, 20)
        Me.Label59.TabIndex = 58
        Me.Label59.Text = "Feed Rate"
        '
        'txt_Arc_Feed_Rate
        '
        Me.txt_Arc_Feed_Rate.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Arc_Feed_Rate.Location = New System.Drawing.Point(119, 44)
        Me.txt_Arc_Feed_Rate.Name = "txt_Arc_Feed_Rate"
        Me.txt_Arc_Feed_Rate.Size = New System.Drawing.Size(69, 26)
        Me.txt_Arc_Feed_Rate.TabIndex = 1
        Me.txt_Arc_Feed_Rate.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label60
        '
        Me.Label60.AutoSize = True
        Me.Label60.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label60.Location = New System.Drawing.Point(26, 21)
        Me.Label60.Name = "Label60"
        Me.Label60.Size = New System.Drawing.Size(84, 20)
        Me.Label60.TabIndex = 55
        Me.Label60.Text = "Tool Diam."
        '
        'txt_Arc_Tool_Diameter
        '
        Me.txt_Arc_Tool_Diameter.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Arc_Tool_Diameter.Location = New System.Drawing.Point(34, 44)
        Me.txt_Arc_Tool_Diameter.Name = "txt_Arc_Tool_Diameter"
        Me.txt_Arc_Tool_Diameter.Size = New System.Drawing.Size(69, 26)
        Me.txt_Arc_Tool_Diameter.TabIndex = 0
        Me.txt_Arc_Tool_Diameter.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label62
        '
        Me.Label62.AccessibleDescription = "a"
        Me.Label62.AutoSize = True
        Me.Label62.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label62.Location = New System.Drawing.Point(26, 367)
        Me.Label62.Name = "Label62"
        Me.Label62.Size = New System.Drawing.Size(20, 20)
        Me.Label62.TabIndex = 51
        Me.Label62.Text = "Y"
        '
        'Label63
        '
        Me.Label63.AccessibleDescription = "a"
        Me.Label63.AutoSize = True
        Me.Label63.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label63.Location = New System.Drawing.Point(147, 467)
        Me.Label63.Name = "Label63"
        Me.Label63.Size = New System.Drawing.Size(20, 20)
        Me.Label63.TabIndex = 50
        Me.Label63.Text = "X"
        '
        'txt_Arc_Center_Y
        '
        Me.txt_Arc_Center_Y.AccessibleDescription = "a"
        Me.txt_Arc_Center_Y.BackColor = System.Drawing.Color.PaleGoldenrod
        Me.txt_Arc_Center_Y.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Arc_Center_Y.Location = New System.Drawing.Point(50, 364)
        Me.txt_Arc_Center_Y.Name = "txt_Arc_Center_Y"
        Me.txt_Arc_Center_Y.Size = New System.Drawing.Size(69, 26)
        Me.txt_Arc_Center_Y.TabIndex = 12
        Me.txt_Arc_Center_Y.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Arc_Center_X
        '
        Me.txt_Arc_Center_X.AccessibleDescription = "a"
        Me.txt_Arc_Center_X.BackColor = System.Drawing.Color.PaleGoldenrod
        Me.txt_Arc_Center_X.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Arc_Center_X.Location = New System.Drawing.Point(123, 438)
        Me.txt_Arc_Center_X.Name = "txt_Arc_Center_X"
        Me.txt_Arc_Center_X.Size = New System.Drawing.Size(69, 26)
        Me.txt_Arc_Center_X.TabIndex = 13
        Me.txt_Arc_Center_X.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'ShapeContainer7
        '
        Me.ShapeContainer7.Location = New System.Drawing.Point(0, 0)
        Me.ShapeContainer7.Margin = New System.Windows.Forms.Padding(0)
        Me.ShapeContainer7.Name = "ShapeContainer7"
        Me.ShapeContainer7.Shapes.AddRange(New Microsoft.VisualBasic.PowerPacks.Shape() {Me.LineShape31, Me.LineShape30, Me.line_Arc_Start_Vector, Me.line_Arc_End_Vector, Me.lab_Arc_Tool_Outside, Me.lab_Arc_Tool_Inside, Me.LineShape29, Me.LineShape28, Me.LineShape27, Me.LineShape26, Me.line_Arc_Vert_Cross, Me.line_Arc_Horz_Cross, Me.OvalShape7, Me.LineShape23})
        Me.ShapeContainer7.Size = New System.Drawing.Size(878, 697)
        Me.ShapeContainer7.TabIndex = 84
        Me.ShapeContainer7.TabStop = False
        '
        'LineShape31
        '
        Me.LineShape31.BorderColor = System.Drawing.Color.Gray
        Me.LineShape31.Name = "LineShape31"
        Me.LineShape31.X1 = 127
        Me.LineShape31.X2 = 77
        Me.LineShape31.Y1 = 317
        Me.LineShape31.Y2 = 317
        '
        'LineShape30
        '
        Me.LineShape30.BorderColor = System.Drawing.Color.Gray
        Me.LineShape30.Name = "LineShape30"
        Me.LineShape30.X1 = 228
        Me.LineShape30.X2 = 228
        Me.LineShape30.Y1 = 418
        Me.LineShape30.Y2 = 458
        '
        'line_Arc_Start_Vector
        '
        Me.line_Arc_Start_Vector.BorderColor = System.Drawing.Color.DarkGreen
        Me.line_Arc_Start_Vector.Name = "line_Arc_Start_Vector"
        Me.line_Arc_Start_Vector.X1 = 228
        Me.line_Arc_Start_Vector.X2 = 328
        Me.line_Arc_Start_Vector.Y1 = 317
        Me.line_Arc_Start_Vector.Y2 = 317
        '
        'line_Arc_End_Vector
        '
        Me.line_Arc_End_Vector.BorderColor = System.Drawing.Color.Firebrick
        Me.line_Arc_End_Vector.Name = "line_Arc_End_Vector"
        Me.line_Arc_End_Vector.X1 = 228
        Me.line_Arc_End_Vector.X2 = 328
        Me.line_Arc_End_Vector.Y1 = 317
        Me.line_Arc_End_Vector.Y2 = 317
        '
        'lab_Arc_Tool_Outside
        '
        Me.lab_Arc_Tool_Outside.Location = New System.Drawing.Point(210, 180)
        Me.lab_Arc_Tool_Outside.Name = "lab_Arc_Tool_Outside"
        Me.lab_Arc_Tool_Outside.Size = New System.Drawing.Size(36, 36)
        '
        'lab_Arc_Tool_Inside
        '
        Me.lab_Arc_Tool_Inside.Location = New System.Drawing.Point(210, 218)
        Me.lab_Arc_Tool_Inside.Name = "lab_Arc_Tool_Inside"
        Me.lab_Arc_Tool_Inside.Size = New System.Drawing.Size(36, 36)
        '
        'LineShape29
        '
        Me.LineShape29.Name = "LineShape29"
        Me.LineShape29.X1 = 392
        Me.LineShape29.X2 = 392
        Me.LineShape29.Y1 = 217
        Me.LineShape29.Y2 = 418
        '
        'LineShape28
        '
        Me.LineShape28.BorderColor = System.Drawing.Color.Gray
        Me.LineShape28.Name = "LineShape28"
        Me.LineShape28.X1 = 229
        Me.LineShape28.X2 = 415
        Me.LineShape28.Y1 = 418
        Me.LineShape28.Y2 = 418
        '
        'LineShape27
        '
        Me.LineShape27.BorderColor = System.Drawing.Color.Gray
        Me.LineShape27.Name = "LineShape27"
        Me.LineShape27.X1 = 222
        Me.LineShape27.X2 = 405
        Me.LineShape27.Y1 = 217
        Me.LineShape27.Y2 = 217
        '
        'LineShape26
        '
        Me.LineShape26.BorderColor = System.Drawing.Color.Gray
        Me.LineShape26.Name = "LineShape26"
        Me.LineShape26.X1 = 73
        Me.LineShape26.X2 = 234
        Me.LineShape26.Y1 = 451
        Me.LineShape26.Y2 = 451
        '
        'line_Arc_Vert_Cross
        '
        Me.line_Arc_Vert_Cross.Name = "line_Arc_Vert_Cross"
        Me.line_Arc_Vert_Cross.X1 = 228
        Me.line_Arc_Vert_Cross.X2 = 228
        Me.line_Arc_Vert_Cross.Y1 = 301
        Me.line_Arc_Vert_Cross.Y2 = 331
        '
        'line_Arc_Horz_Cross
        '
        Me.line_Arc_Horz_Cross.Name = "line_Arc_Horz_Cross"
        Me.line_Arc_Horz_Cross.X1 = 209
        Me.line_Arc_Horz_Cross.X2 = 247
        Me.line_Arc_Horz_Cross.Y1 = 317
        Me.line_Arc_Horz_Cross.Y2 = 317
        '
        'OvalShape7
        '
        Me.OvalShape7.Location = New System.Drawing.Point(128, 217)
        Me.OvalShape7.Name = "OvalShape7"
        Me.OvalShape7.Size = New System.Drawing.Size(200, 200)
        '
        'LineShape23
        '
        Me.LineShape23.BorderColor = System.Drawing.Color.Gray
        Me.LineShape23.Name = "LineShape23"
        Me.LineShape23.X1 = 85
        Me.LineShape23.X2 = 85
        Me.LineShape23.Y1 = 317
        Me.LineShape23.Y2 = 460
        '
        'tab_Translate
        '
        Me.tab_Translate.BackColor = System.Drawing.Color.DarkKhaki
        Me.tab_Translate.Controls.Add(Me.Label83)
        Me.tab_Translate.Controls.Add(Me.Cord_Length)
        Me.tab_Translate.Controls.Add(Me.txt_Translate_Cord_Length)
        Me.tab_Translate.Controls.Add(Me.btn_Translate_Current_File)
        Me.tab_Translate.Controls.Add(Me.txt_Translate_Input_File)
        Me.tab_Translate.Controls.Add(Me.btn_Translate_Open_File)
        Me.tab_Translate.Controls.Add(Me.Label53)
        Me.tab_Translate.Location = New System.Drawing.Point(4, 27)
        Me.tab_Translate.Name = "tab_Translate"
        Me.tab_Translate.Size = New System.Drawing.Size(878, 697)
        Me.tab_Translate.TabIndex = 7
        Me.tab_Translate.Text = "Translate"
        '
        'Label83
        '
        Me.Label83.AutoSize = True
        Me.Label83.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label83.Location = New System.Drawing.Point(404, 45)
        Me.Label83.Name = "Label83"
        Me.Label83.Size = New System.Drawing.Size(110, 20)
        Me.Label83.TabIndex = 90
        Me.Label83.Text = "Nominal 0.050"
        '
        'Cord_Length
        '
        Me.Cord_Length.AutoSize = True
        Me.Cord_Length.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Cord_Length.Location = New System.Drawing.Point(227, 45)
        Me.Cord_Length.Name = "Cord_Length"
        Me.Cord_Length.Size = New System.Drawing.Size(97, 20)
        Me.Cord_Length.TabIndex = 89
        Me.Cord_Length.Text = "Cord Length"
        '
        'txt_Translate_Cord_Length
        '
        Me.txt_Translate_Cord_Length.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Translate_Cord_Length.Location = New System.Drawing.Point(329, 42)
        Me.txt_Translate_Cord_Length.Name = "txt_Translate_Cord_Length"
        Me.txt_Translate_Cord_Length.Size = New System.Drawing.Size(69, 26)
        Me.txt_Translate_Cord_Length.TabIndex = 88
        Me.txt_Translate_Cord_Length.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'btn_Translate_Current_File
        '
        Me.btn_Translate_Current_File.BackgroundImage = CType(resources.GetObject("btn_Translate_Current_File.BackgroundImage"), System.Drawing.Image)
        Me.btn_Translate_Current_File.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.btn_Translate_Current_File.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Translate_Current_File.Location = New System.Drawing.Point(23, 26)
        Me.btn_Translate_Current_File.Name = "btn_Translate_Current_File"
        Me.btn_Translate_Current_File.Size = New System.Drawing.Size(183, 52)
        Me.btn_Translate_Current_File.TabIndex = 87
        Me.btn_Translate_Current_File.Text = "Use Current File"
        Me.btn_Translate_Current_File.UseVisualStyleBackColor = True
        '
        'txt_Translate_Input_File
        '
        Me.txt_Translate_Input_File.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Translate_Input_File.Location = New System.Drawing.Point(227, 103)
        Me.txt_Translate_Input_File.Name = "txt_Translate_Input_File"
        Me.txt_Translate_Input_File.Size = New System.Drawing.Size(625, 26)
        Me.txt_Translate_Input_File.TabIndex = 86
        '
        'btn_Translate_Open_File
        '
        Me.btn_Translate_Open_File.BackgroundImage = CType(resources.GetObject("btn_Translate_Open_File.BackgroundImage"), System.Drawing.Image)
        Me.btn_Translate_Open_File.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.btn_Translate_Open_File.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Translate_Open_File.Location = New System.Drawing.Point(23, 89)
        Me.btn_Translate_Open_File.Name = "btn_Translate_Open_File"
        Me.btn_Translate_Open_File.Size = New System.Drawing.Size(183, 52)
        Me.btn_Translate_Open_File.TabIndex = 83
        Me.btn_Translate_Open_File.Text = "Open"
        Me.btn_Translate_Open_File.UseVisualStyleBackColor = True
        '
        'Label53
        '
        Me.Label53.AutoSize = True
        Me.Label53.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label53.Location = New System.Drawing.Point(227, 80)
        Me.Label53.Name = "Label53"
        Me.Label53.Size = New System.Drawing.Size(75, 20)
        Me.Label53.TabIndex = 4
        Me.Label53.Text = "Input File"
        '
        'tab_Feather
        '
        Me.tab_Feather.BackColor = System.Drawing.Color.DarkKhaki
        Me.tab_Feather.Controls.Add(Me.GroupBox2)
        Me.tab_Feather.Controls.Add(Me.Label93)
        Me.tab_Feather.Controls.Add(Me.txt_Feather_Max_Z)
        Me.tab_Feather.Controls.Add(Me.txt_Feather_Input_File)
        Me.tab_Feather.Controls.Add(Me.btn_Feather_Open_File)
        Me.tab_Feather.Controls.Add(Me.Label92)
        Me.tab_Feather.Controls.Add(Me.Label91)
        Me.tab_Feather.Controls.Add(Me.txt_Feather_Min_Z)
        Me.tab_Feather.Location = New System.Drawing.Point(4, 27)
        Me.tab_Feather.Name = "tab_Feather"
        Me.tab_Feather.Padding = New System.Windows.Forms.Padding(3)
        Me.tab_Feather.Size = New System.Drawing.Size(878, 697)
        Me.tab_Feather.TabIndex = 9
        Me.tab_Feather.Text = "Feather"
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.rad_Feather_Y)
        Me.GroupBox2.Controls.Add(Me.rad_Feather_X)
        Me.GroupBox2.Location = New System.Drawing.Point(227, 116)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(82, 89)
        Me.GroupBox2.TabIndex = 113
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "Direction"
        '
        'rad_Feather_Y
        '
        Me.rad_Feather_Y.AutoSize = True
        Me.rad_Feather_Y.Location = New System.Drawing.Point(17, 24)
        Me.rad_Feather_Y.Name = "rad_Feather_Y"
        Me.rad_Feather_Y.Size = New System.Drawing.Size(46, 22)
        Me.rad_Feather_Y.TabIndex = 44
        Me.rad_Feather_Y.TabStop = True
        Me.rad_Feather_Y.Text = "Y ^"
        Me.rad_Feather_Y.UseVisualStyleBackColor = True
        '
        'rad_Feather_X
        '
        Me.rad_Feather_X.AutoSize = True
        Me.rad_Feather_X.Location = New System.Drawing.Point(17, 52)
        Me.rad_Feather_X.Name = "rad_Feather_X"
        Me.rad_Feather_X.Size = New System.Drawing.Size(49, 22)
        Me.rad_Feather_X.TabIndex = 43
        Me.rad_Feather_X.TabStop = True
        Me.rad_Feather_X.Text = "X >"
        Me.rad_Feather_X.UseVisualStyleBackColor = True
        '
        'Label93
        '
        Me.Label93.AutoSize = True
        Me.Label93.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label93.Location = New System.Drawing.Point(345, 172)
        Me.Label93.Name = "Label93"
        Me.Label93.Size = New System.Drawing.Size(52, 20)
        Me.Label93.TabIndex = 112
        Me.Label93.Text = "Max Z"
        '
        'txt_Feather_Max_Z
        '
        Me.txt_Feather_Max_Z.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Feather_Max_Z.Location = New System.Drawing.Point(400, 168)
        Me.txt_Feather_Max_Z.Name = "txt_Feather_Max_Z"
        Me.txt_Feather_Max_Z.Size = New System.Drawing.Size(69, 26)
        Me.txt_Feather_Max_Z.TabIndex = 111
        Me.txt_Feather_Max_Z.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txt_Feather_Input_File
        '
        Me.txt_Feather_Input_File.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.txt_Feather_Input_File.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Feather_Input_File.Location = New System.Drawing.Point(227, 58)
        Me.txt_Feather_Input_File.Name = "txt_Feather_Input_File"
        Me.txt_Feather_Input_File.Size = New System.Drawing.Size(625, 26)
        Me.txt_Feather_Input_File.TabIndex = 108
        '
        'btn_Feather_Open_File
        '
        Me.btn_Feather_Open_File.BackgroundImage = CType(resources.GetObject("btn_Feather_Open_File.BackgroundImage"), System.Drawing.Image)
        Me.btn_Feather_Open_File.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
        Me.btn_Feather_Open_File.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btn_Feather_Open_File.Location = New System.Drawing.Point(22, 44)
        Me.btn_Feather_Open_File.Name = "btn_Feather_Open_File"
        Me.btn_Feather_Open_File.Size = New System.Drawing.Size(183, 52)
        Me.btn_Feather_Open_File.TabIndex = 107
        Me.btn_Feather_Open_File.Text = "Open"
        Me.btn_Feather_Open_File.UseVisualStyleBackColor = True
        '
        'Label92
        '
        Me.Label92.AutoSize = True
        Me.Label92.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label92.Location = New System.Drawing.Point(227, 35)
        Me.Label92.Name = "Label92"
        Me.Label92.Size = New System.Drawing.Size(75, 20)
        Me.Label92.TabIndex = 106
        Me.Label92.Text = "Input File"
        '
        'Label91
        '
        Me.Label91.AutoSize = True
        Me.Label91.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label91.Location = New System.Drawing.Point(347, 140)
        Me.Label91.Name = "Label91"
        Me.Label91.Size = New System.Drawing.Size(48, 20)
        Me.Label91.TabIndex = 105
        Me.Label91.Text = "Min Z"
        '
        'txt_Feather_Min_Z
        '
        Me.txt_Feather_Min_Z.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt_Feather_Min_Z.Location = New System.Drawing.Point(400, 136)
        Me.txt_Feather_Min_Z.Name = "txt_Feather_Min_Z"
        Me.txt_Feather_Min_Z.Size = New System.Drawing.Size(69, 26)
        Me.txt_Feather_Min_Z.TabIndex = 104
        Me.txt_Feather_Min_Z.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Splitter1
        '
        Me.Splitter1.BackColor = System.Drawing.Color.Firebrick
        Me.Splitter1.Location = New System.Drawing.Point(369, 38)
        Me.Splitter1.Name = "Splitter1"
        Me.Splitter1.Size = New System.Drawing.Size(3, 728)
        Me.Splitter1.TabIndex = 9
        Me.Splitter1.TabStop = False
        '
        'ctl_GCode
        '
        Me.ctl_GCode.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink
        Me.ctl_GCode.Dock = System.Windows.Forms.DockStyle.Left
        Me.ctl_GCode.Location = New System.Drawing.Point(0, 38)
        Me.ctl_GCode.Name = "ctl_GCode"
        Me.ctl_GCode.Size = New System.Drawing.Size(369, 728)
        Me.ctl_GCode.TabIndex = 8
        '
        'dlg_Wizards
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(1258, 766)
        Me.Controls.Add(Me.tab_Wizard)
        Me.Controls.Add(Me.Splitter1)
        Me.Controls.Add(Me.ctl_GCode)
        Me.Controls.Add(Me.ts_Wizard)
        Me.Name = "dlg_Wizards"
        Me.Text = "Wizards"
        Me.ts_Wizard.ResumeLayout(False)
        Me.ts_Wizard.PerformLayout()
        Me.tab_Wizard.ResumeLayout(False)
        Me.tab_Drawing.ResumeLayout(False)
        Me.tab_Move.ResumeLayout(False)
        Me.tab_Move.PerformLayout()
        Me.tab_Flatten.ResumeLayout(False)
        Me.tab_Flatten.PerformLayout()
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        Me.tab_Hole.ResumeLayout(False)
        Me.tab_Hole.PerformLayout()
        Me.tab_Line.ResumeLayout(False)
        Me.tab_Line.PerformLayout()
        Me.tab_Straight_Options.ResumeLayout(False)
        Me.tab_Move_Horizontal.ResumeLayout(False)
        Me.tab_Move_Horizontal.PerformLayout()
        Me.tab_Move_Angle.ResumeLayout(False)
        Me.tab_Move_Angle.PerformLayout()
        Me.tab_Frame.ResumeLayout(False)
        Me.tab_Frame.PerformLayout()
        Me.tab_Circle.ResumeLayout(False)
        Me.tab_Circle.PerformLayout()
        Me.tab_Arc.ResumeLayout(False)
        Me.tab_Arc.PerformLayout()
        Me.tab_Translate.ResumeLayout(False)
        Me.tab_Translate.PerformLayout()
        Me.tab_Feather.ResumeLayout(False)
        Me.tab_Feather.PerformLayout()
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox2.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents ts_Wizard As System.Windows.Forms.ToolStrip
    Friend WithEvents ts_Exit As System.Windows.Forms.ToolStripButton
    Friend WithEvents tab_Wizard As System.Windows.Forms.TabControl
    Friend WithEvents tab_Hole As System.Windows.Forms.TabPage
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents txt_Hole_Feed_Rate As System.Windows.Forms.TextBox
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents txt_Hole_Step_Over As System.Windows.Forms.TextBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents txt_Hole_Tool_Diameter As System.Windows.Forms.TextBox
    Friend WithEvents tab_Flatten As System.Windows.Forms.TabPage
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents txt_Hole_Pass_Depth As System.Windows.Forms.TextBox
    Friend WithEvents btn_Hole_Deeper As System.Windows.Forms.Button
    Friend WithEvents ts_Save_As As System.Windows.Forms.ToolStripButton
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents txt_Hole_Cut_Depth As System.Windows.Forms.TextBox
    Friend WithEvents btn_txt_Hole_Current As System.Windows.Forms.Button
    Friend WithEvents txt_Hole_Center_Y As System.Windows.Forms.TextBox
    Friend WithEvents txt_Hole_Center_X As System.Windows.Forms.TextBox
    Friend WithEvents ShapeContainer2 As Microsoft.VisualBasic.PowerPacks.ShapeContainer
    Friend WithEvents LineShape1 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents X_Line As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents Circle As Microsoft.VisualBasic.PowerPacks.OvalShape
    Friend WithEvents ShapeContainer1 As Microsoft.VisualBasic.PowerPacks.ShapeContainer
    Friend WithEvents RectangleShape1 As Microsoft.VisualBasic.PowerPacks.RectangleShape
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents txt_Hole_Diameter As System.Windows.Forms.TextBox
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents LineShape6 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents LineShape5 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents LineShape4 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents LineShape3 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents LineShape2 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents Label17 As System.Windows.Forms.Label
    Friend WithEvents Label16 As System.Windows.Forms.Label
    Friend WithEvents txt_Flatten_Height As System.Windows.Forms.TextBox
    Friend WithEvents txt_Flatten_Width As System.Windows.Forms.TextBox
    Friend WithEvents Label10 As System.Windows.Forms.Label
    Friend WithEvents txt_Flatten_Cut_Depth As System.Windows.Forms.TextBox
    Friend WithEvents Label11 As System.Windows.Forms.Label
    Friend WithEvents txt_Flatten_Pass_Depth As System.Windows.Forms.TextBox
    Friend WithEvents Label12 As System.Windows.Forms.Label
    Friend WithEvents txt_Flatten_Feed_Rate As System.Windows.Forms.TextBox
    Friend WithEvents Label13 As System.Windows.Forms.Label
    Friend WithEvents txt_Flatten_Step_Over As System.Windows.Forms.TextBox
    Friend WithEvents Label14 As System.Windows.Forms.Label
    Friend WithEvents txt_Flatten_Tool_Diameter As System.Windows.Forms.TextBox
    Friend WithEvents Label19 As System.Windows.Forms.Label
    Friend WithEvents txt_Flatten_Plunge_Rate As System.Windows.Forms.TextBox
    Friend WithEvents Label20 As System.Windows.Forms.Label
    Friend WithEvents txt_Hole_Plunge_Rate As System.Windows.Forms.TextBox
    Friend WithEvents rad_Hole_Pocket As System.Windows.Forms.RadioButton
    Friend WithEvents btn_Wider As System.Windows.Forms.Button
    Friend WithEvents ts_Cancel As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Load As System.Windows.Forms.ToolStripButton
    Friend WithEvents tab_Drawing As System.Windows.Forms.TabPage
    Friend WithEvents ts_Append_GCode As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Header As System.Windows.Forms.ToolStripButton
    Friend WithEvents ctl_GCode As Chip.ctrl_Gcode
    Friend WithEvents Splitter1 As System.Windows.Forms.Splitter
    Friend WithEvents ctl_Drawing As Chip.ctrl_Drawing
    Friend WithEvents ts_Save As System.Windows.Forms.ToolStripButton
    Friend WithEvents tab_Line As System.Windows.Forms.TabPage
    Friend WithEvents btn_Hole_Shallower As System.Windows.Forms.Button
    Friend WithEvents Label22 As System.Windows.Forms.Label
    Friend WithEvents btn_Line_Shallower As System.Windows.Forms.Button
    Friend WithEvents Label23 As System.Windows.Forms.Label
    Friend WithEvents txt_Straight_Cut_Depth As System.Windows.Forms.TextBox
    Friend WithEvents btn_Line_Deeper As System.Windows.Forms.Button
    Friend WithEvents Label24 As System.Windows.Forms.Label
    Friend WithEvents txt_Straight_Pass_Depth As System.Windows.Forms.TextBox
    Friend WithEvents Label25 As System.Windows.Forms.Label
    Friend WithEvents txt_Straight_Feed_Rate As System.Windows.Forms.TextBox
    Friend WithEvents Label26 As System.Windows.Forms.Label
    Friend WithEvents txt_Straight_Tool_Diameter As System.Windows.Forms.TextBox
    Friend WithEvents Label29 As System.Windows.Forms.Label
    Friend WithEvents txt_Straight_Plunge_Rate As System.Windows.Forms.TextBox
    Friend WithEvents OvalShape2 As Microsoft.VisualBasic.PowerPacks.OvalShape
    Friend WithEvents tab_Move As System.Windows.Forms.TabPage
    Friend WithEvents chk_Move_Rapid As System.Windows.Forms.CheckBox
    Friend WithEvents Label38 As System.Windows.Forms.Label
    Friend WithEvents txt_Move_Feed_Rate As System.Windows.Forms.TextBox
    Friend WithEvents txt_Move_Z As System.Windows.Forms.TextBox
    Friend WithEvents txt_Move_Y As System.Windows.Forms.TextBox
    Friend WithEvents txt_Move_X As System.Windows.Forms.TextBox
    Friend WithEvents chk_Move_Z_First As System.Windows.Forms.CheckBox
    Friend WithEvents btn_Move_Current_Z As System.Windows.Forms.Button
    Friend WithEvents chk_Move_Z As System.Windows.Forms.CheckBox
    Friend WithEvents chk_Move_Y As System.Windows.Forms.CheckBox
    Friend WithEvents chk_Move_X As System.Windows.Forms.CheckBox
    Friend WithEvents btn_Move_Current_XY As System.Windows.Forms.Button
    Friend WithEvents tab_Straight_Options As System.Windows.Forms.TabControl
    Friend WithEvents tab_Move_Horizontal As System.Windows.Forms.TabPage
    Friend WithEvents btn_Horizontal_Cutter_Path As System.Windows.Forms.Button
    Friend WithEvents btn_Horizontal_Current_End As System.Windows.Forms.Button
    Friend WithEvents Label28 As System.Windows.Forms.Label
    Friend WithEvents txt_Horizontal_Start_X As System.Windows.Forms.TextBox
    Friend WithEvents btn_Horizontal_Current_Start As System.Windows.Forms.Button
    Friend WithEvents txt_Horizontal_End_X As System.Windows.Forms.TextBox
    Friend WithEvents ShapeContainer4 As Microsoft.VisualBasic.PowerPacks.ShapeContainer
    Friend WithEvents Horizontal_Cutter_Top_Right As Microsoft.VisualBasic.PowerPacks.OvalShape
    Friend WithEvents Horizontal_Cutter_Bottom_Right As Microsoft.VisualBasic.PowerPacks.OvalShape
    Friend WithEvents Horizontal_Cutter_Bottom_Left As Microsoft.VisualBasic.PowerPacks.OvalShape
    Friend WithEvents Horizontal_Cutter_Top_Left As Microsoft.VisualBasic.PowerPacks.OvalShape
    Friend WithEvents LineShape10 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents tab_Move_Angle As System.Windows.Forms.TabPage
    Friend WithEvents Label33 As System.Windows.Forms.Label
    Friend WithEvents TextBox1 As System.Windows.Forms.TextBox
    Friend WithEvents Label35 As System.Windows.Forms.Label
    Friend WithEvents TextBox2 As System.Windows.Forms.TextBox
    Friend WithEvents Label36 As System.Windows.Forms.Label
    Friend WithEvents TextBox3 As System.Windows.Forms.TextBox
    Friend WithEvents Button1 As System.Windows.Forms.Button
    Friend WithEvents Label37 As System.Windows.Forms.Label
    Friend WithEvents Label39 As System.Windows.Forms.Label
    Friend WithEvents TextBox5 As System.Windows.Forms.TextBox
    Friend WithEvents TextBox6 As System.Windows.Forms.TextBox
    Friend WithEvents Button2 As System.Windows.Forms.Button
    Friend WithEvents Label40 As System.Windows.Forms.Label
    Friend WithEvents Label41 As System.Windows.Forms.Label
    Friend WithEvents TextBox7 As System.Windows.Forms.TextBox
    Friend WithEvents TextBox8 As System.Windows.Forms.TextBox
    Friend WithEvents ShapeContainer3 As Microsoft.VisualBasic.PowerPacks.ShapeContainer
    Friend WithEvents LineShape9 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents LineShape7 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents LineShape8 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents OvalShape1 As Microsoft.VisualBasic.PowerPacks.OvalShape
    Friend WithEvents tab_Move_Vertical As System.Windows.Forms.TabPage
    Friend WithEvents btn_Hole_Cutter_Path As System.Windows.Forms.Button
    Friend WithEvents OvalShape3 As Microsoft.VisualBasic.PowerPacks.OvalShape
    Friend WithEvents ts_Clear As System.Windows.Forms.ToolStripButton
    Friend WithEvents Label21 As System.Windows.Forms.Label
    Friend WithEvents txt_Flatten_Safe_Z As System.Windows.Forms.TextBox
    Friend WithEvents Label27 As System.Windows.Forms.Label
    Friend WithEvents txt_Hole_Safe_Z As System.Windows.Forms.TextBox
    Friend WithEvents ts_Delete_Line As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Expand As System.Windows.Forms.ToolStripButton
    Friend WithEvents ts_Collapse As System.Windows.Forms.ToolStripButton
    Friend WithEvents tab_Frame As System.Windows.Forms.TabPage
    Friend WithEvents Label30 As System.Windows.Forms.Label
    Friend WithEvents txt_Frame_Rail_Width As System.Windows.Forms.TextBox
    Friend WithEvents ShapeContainer5 As Microsoft.VisualBasic.PowerPacks.ShapeContainer
    Friend WithEvents LineShape14 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents LineShape13 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents LineShape12 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents LineShape11 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents txt_Frame_Mat_Width As System.Windows.Forms.TextBox
    Friend WithEvents Label34 As System.Windows.Forms.Label
    Friend WithEvents txt_Frame_Frame_Width As System.Windows.Forms.TextBox
    Friend WithEvents Label31 As System.Windows.Forms.Label
    Friend WithEvents RectangleShape2 As Microsoft.VisualBasic.PowerPacks.RectangleShape
    Friend WithEvents txt_Frame_Rough_Cut As System.Windows.Forms.TextBox
    Friend WithEvents Label46 As System.Windows.Forms.Label
    Friend WithEvents lab_Frame_Lower_Left As System.Windows.Forms.Label
    Friend WithEvents Label43 As System.Windows.Forms.Label
    Friend WithEvents LineShape15 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents txt_Frame_Rough_Cut_Fractional As System.Windows.Forms.TextBox
    Friend WithEvents txt_Frame_Frame_Width_Fractional As System.Windows.Forms.TextBox
    Friend WithEvents txt_Frame_Rail_Width_Fractional As System.Windows.Forms.TextBox
    Friend WithEvents txt_Frame_Mat_Width_Fractional As System.Windows.Forms.TextBox
    Friend WithEvents tab_Circle As System.Windows.Forms.TabPage
    Friend WithEvents Label45 As System.Windows.Forms.Label
    Friend WithEvents txt_Circle_Cut_Depth As System.Windows.Forms.TextBox
    Friend WithEvents Label47 As System.Windows.Forms.Label
    Friend WithEvents txt_Circle_Pass_Depth As System.Windows.Forms.TextBox
    Friend WithEvents Label48 As System.Windows.Forms.Label
    Friend WithEvents txt_Circle_Feed_Rate As System.Windows.Forms.TextBox
    Friend WithEvents Label49 As System.Windows.Forms.Label
    Friend WithEvents txt_Circle_Tool_Diameter As System.Windows.Forms.TextBox
    Friend WithEvents Label32 As System.Windows.Forms.Label
    Friend WithEvents txt_Circle_Diameter As System.Windows.Forms.TextBox
    Friend WithEvents Label42 As System.Windows.Forms.Label
    Friend WithEvents Label44 As System.Windows.Forms.Label
    Friend WithEvents txt_Circle_Center_Y As System.Windows.Forms.TextBox
    Friend WithEvents txt_Circle_Center_X As System.Windows.Forms.TextBox
    Friend WithEvents ShapeContainer6 As Microsoft.VisualBasic.PowerPacks.ShapeContainer
    Friend WithEvents LineShape22 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents OvalShape6 As Microsoft.VisualBasic.PowerPacks.OvalShape
    Friend WithEvents LineShape21 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents LineShape20 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents LineShape19 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents LineShape18 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents LineShape17 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents LineShape16 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents OvalShape5 As Microsoft.VisualBasic.PowerPacks.OvalShape
    Friend WithEvents OvalShape4 As Microsoft.VisualBasic.PowerPacks.OvalShape
    Friend WithEvents Label50 As System.Windows.Forms.Label
    Friend WithEvents txt_Circle_Start_Depth As System.Windows.Forms.TextBox
    Friend WithEvents Label51 As System.Windows.Forms.Label
    Friend WithEvents txt_Circle_Safe_Z As System.Windows.Forms.TextBox
    Friend WithEvents Label52 As System.Windows.Forms.Label
    Friend WithEvents txt_Circle_Plunge_Rate As System.Windows.Forms.TextBox
    Friend WithEvents tab_Translate As System.Windows.Forms.TabPage
    Friend WithEvents Label53 As System.Windows.Forms.Label
    Friend WithEvents btn_Translate_Open_File As System.Windows.Forms.Button
    Friend WithEvents tab_Arc As System.Windows.Forms.TabPage
    Friend WithEvents Label59 As System.Windows.Forms.Label
    Friend WithEvents txt_Arc_Feed_Rate As System.Windows.Forms.TextBox
    Friend WithEvents Label60 As System.Windows.Forms.Label
    Friend WithEvents txt_Arc_Tool_Diameter As System.Windows.Forms.TextBox
    Friend WithEvents Label62 As System.Windows.Forms.Label
    Friend WithEvents Label63 As System.Windows.Forms.Label
    Friend WithEvents txt_Arc_Center_Y As System.Windows.Forms.TextBox
    Friend WithEvents txt_Arc_Center_X As System.Windows.Forms.TextBox
    Friend WithEvents Label68 As System.Windows.Forms.Label
    Friend WithEvents txt_Arc_Start_Angle As System.Windows.Forms.TextBox
    Friend WithEvents Label69 As System.Windows.Forms.Label
    Friend WithEvents txt_Arc_End_Angle As System.Windows.Forms.TextBox
    Friend WithEvents txt_Arc_Start_X As System.Windows.Forms.TextBox
    Friend WithEvents txt_Arc_End_X As System.Windows.Forms.TextBox
    Friend WithEvents Label65 As System.Windows.Forms.Label
    Friend WithEvents txt_Arc_Start_Y As System.Windows.Forms.TextBox
    Friend WithEvents txt_Arc_End_Y As System.Windows.Forms.TextBox
    Friend WithEvents Label71 As System.Windows.Forms.Label
    Friend WithEvents Label70 As System.Windows.Forms.Label
    Friend WithEvents cmb_Arc_Tool_Direction As System.Windows.Forms.ComboBox
    Friend WithEvents cmb_Arc_Tool_Position As System.Windows.Forms.ComboBox
    Friend WithEvents lab_Arc_Tooi_Outside_Direction As System.Windows.Forms.Label
    Friend WithEvents lab_Arc_Tooi_Inside_Direction As System.Windows.Forms.Label
    Friend WithEvents Label61 As System.Windows.Forms.Label
    Friend WithEvents txt_Arc_Diameter As System.Windows.Forms.TextBox
    Friend WithEvents ShapeContainer7 As Microsoft.VisualBasic.PowerPacks.ShapeContainer
    Friend WithEvents line_Arc_Start_Vector As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents line_Arc_End_Vector As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents lab_Arc_Tool_Outside As Microsoft.VisualBasic.PowerPacks.OvalShape
    Friend WithEvents lab_Arc_Tool_Inside As Microsoft.VisualBasic.PowerPacks.OvalShape
    Friend WithEvents LineShape29 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents LineShape28 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents LineShape27 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents LineShape26 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents line_Arc_Horz_Cross As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents OvalShape7 As Microsoft.VisualBasic.PowerPacks.OvalShape
    Friend WithEvents LineShape23 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents Label73 As System.Windows.Forms.Label
    Friend WithEvents Label66 As System.Windows.Forms.Label
    Friend WithEvents Label64 As System.Windows.Forms.Label
    Friend WithEvents LineShape31 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents LineShape30 As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents line_Arc_Vert_Cross As Microsoft.VisualBasic.PowerPacks.LineShape
    Friend WithEvents Label67 As System.Windows.Forms.Label
    Friend WithEvents Label74 As System.Windows.Forms.Label
    Friend WithEvents txt_Arc_I As System.Windows.Forms.TextBox
    Friend WithEvents txt_Arc_J As System.Windows.Forms.TextBox
    Friend WithEvents Label54 As System.Windows.Forms.Label
    Friend WithEvents Label56 As System.Windows.Forms.Label
    Friend WithEvents TextBox9 As System.Windows.Forms.TextBox
    Friend WithEvents Label55 As System.Windows.Forms.Label
    Friend WithEvents TextBox4 As System.Windows.Forms.TextBox
    Friend WithEvents ts_Help As System.Windows.Forms.ToolStripButton
    Friend WithEvents Label57 As System.Windows.Forms.Label
    Friend WithEvents txt_Arc_Radius As System.Windows.Forms.TextBox
    Friend WithEvents Label58 As System.Windows.Forms.Label
    Friend WithEvents txt_Arc_Block As System.Windows.Forms.TextBox
    Friend WithEvents ts_Insert As System.Windows.Forms.ToolStripDropDownButton
    Friend WithEvents ts_Insert_G0 As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_Insert_G1 As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_Insert_M03_M08 As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_Insert_M05_M09 As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_Insert_End As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ts_Insert_Blank_Line As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents txt_Translate_Input_File As System.Windows.Forms.TextBox
    Friend WithEvents Label77 As System.Windows.Forms.Label
    Friend WithEvents txt_Frame_Safe_Z As System.Windows.Forms.TextBox
    Friend WithEvents Label76 As System.Windows.Forms.Label
    Friend WithEvents txt_Frame_Cut_Depth As System.Windows.Forms.TextBox
    Friend WithEvents Label75 As System.Windows.Forms.Label
    Friend WithEvents txt_Frame_Feedrate As System.Windows.Forms.TextBox
    Friend WithEvents Label72 As System.Windows.Forms.Label
    Friend WithEvents txt_Frame_Tool_Diameter As System.Windows.Forms.TextBox
    Friend WithEvents txt_Frame_Lower_Right As System.Windows.Forms.TextBox
    Friend WithEvents txt_Frame_Upper_Left As System.Windows.Forms.TextBox
    Friend WithEvents txt_Frame_Upper_Right As System.Windows.Forms.TextBox
    Friend WithEvents rad_Flatten_Y_Direction As System.Windows.Forms.RadioButton
    Friend WithEvents rad_Flatten_X_Direction As System.Windows.Forms.RadioButton
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents txt_Flatten_XY_Passes As System.Windows.Forms.TextBox
    Friend WithEvents Label15 As System.Windows.Forms.Label
    Friend WithEvents txt_Flatten_Z_Passes As System.Windows.Forms.TextBox
    Friend WithEvents Label78 As System.Windows.Forms.Label
    Friend WithEvents btn_Translate_Current_File As System.Windows.Forms.Button
    Friend WithEvents Label80 As System.Windows.Forms.Label
    Friend WithEvents txt_Flatten_X_Start As System.Windows.Forms.TextBox
    Friend WithEvents Label79 As System.Windows.Forms.Label
    Friend WithEvents txt_Flatten_Y_Start As System.Windows.Forms.TextBox
    Friend WithEvents Label18 As System.Windows.Forms.Label
    Friend WithEvents txt_Frame_Y_Offset As System.Windows.Forms.TextBox
    Friend WithEvents Label82 As System.Windows.Forms.Label
    Friend WithEvents txt_Frame_X_Offset As System.Windows.Forms.TextBox
    Friend WithEvents Label81 As System.Windows.Forms.Label
    Friend WithEvents rad_Frame_Cut_Up As System.Windows.Forms.RadioButton
    Friend WithEvents rad_Frame_Cut_Down As System.Windows.Forms.RadioButton
    Friend WithEvents Label83 As System.Windows.Forms.Label
    Friend WithEvents Cord_Length As System.Windows.Forms.Label
    Friend WithEvents txt_Translate_Cord_Length As System.Windows.Forms.TextBox
    Friend WithEvents Label84 As System.Windows.Forms.Label
    Friend WithEvents txt_Flatten_Start_Thickness As System.Windows.Forms.TextBox
    Friend WithEvents Label85 As System.Windows.Forms.Label
    Friend WithEvents Label86 As System.Windows.Forms.Label
    Friend WithEvents txt_Flatten_End_Thickness As System.Windows.Forms.TextBox
    Friend WithEvents btn_Flatten_Material_Set As System.Windows.Forms.Button
    Friend WithEvents ts_Info As System.Windows.Forms.ToolStripTextBox
    Friend WithEvents Label90 As System.Windows.Forms.Label
    Friend WithEvents txt_Flatten_Park_Z As System.Windows.Forms.TextBox
    Friend WithEvents Label89 As System.Windows.Forms.Label
    Friend WithEvents Label88 As System.Windows.Forms.Label
    Friend WithEvents txt_Flatten_Park_Y As System.Windows.Forms.TextBox
    Friend WithEvents Label87 As System.Windows.Forms.Label
    Friend WithEvents txt_Flatten_Park_X As System.Windows.Forms.TextBox
    Friend WithEvents tab_Feather As System.Windows.Forms.TabPage
    Friend WithEvents txt_Feather_Input_File As System.Windows.Forms.TextBox
    Friend WithEvents btn_Feather_Open_File As System.Windows.Forms.Button
    Friend WithEvents Label92 As System.Windows.Forms.Label
    Friend WithEvents Label91 As System.Windows.Forms.Label
    Friend WithEvents txt_Feather_Min_Z As System.Windows.Forms.TextBox
    Friend WithEvents Label93 As System.Windows.Forms.Label
    Friend WithEvents txt_Feather_Max_Z As System.Windows.Forms.TextBox
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents rad_Feather_Y As System.Windows.Forms.RadioButton
    Friend WithEvents rad_Feather_X As System.Windows.Forms.RadioButton
End Class
