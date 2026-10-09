Imports System.IO

Public Class Class_Display_Handlers

    ' Should not change these unless absolutely necessary *******************************************************

#Region "Enums"

    Public Enum enum_Event_Type
        Initialize
        Reset
        Finish
        Update
        Validate
        Cycle_Start
        Cycle_Stop
        Left_Mouse_Button_Down
        Left_Mouse_Button_Up
        Right_Mouse_Button_Down
        Right_Mouse_Button_Up
        Middle_Mouse_Button_Down
        Middle_Mouse_Button_Up
    End Enum

    Public Enum enum_Home_Type
        None
        X_Axis
        Y_Axis
        Z_Axis
        All_Axes
    End Enum

    Public Enum enum_Home_Status
        Not_Homing
        Homing_X_Axis
        Homing_Y_Axis
        Homing_Z_Axis
        Homing_All_Axes
        X_Axis_Homed
        Y_Axis_Homed
        Z_Axis_Homed
        All_Axes_Homed
    End Enum

#End Region

#Region "Definitions"

    Private Current_Block As Integer
    Private Settings_Buffer As String = ""
    Private Settings_Lines() As String
    Private Settings As New List(Of String)
    Private Settings_Keys As New List(Of String)

    Public Max_Spindle_RPM As Double = 24000
    Public Max_Feedrate As Integer = 50

    Public Maximum_X As Double = 33.5
    Public Minimum_X As Double = 0

    Public Maximum_Y As Double = 27.5
    Public Minimum_Y As Double = 0

    Public Maximum_Z As Double = 6.0
    Public Minimum_Z As Double = 1.5

    Public X_Pos_Soft_Limit As Double = 33.5
    Public X_Neg_Soft_Limit As Double = -0.01

    Public Y_Pos_Soft_Limit As Double = 27.5
    Public Y_Neg_Soft_Limit As Double = -0.01

    Public Z_Pos_Soft_Limit As Double = 6.01
    Public Z_Neg_Soft_Limit As Double = 1.5

    Public Startup_X As Double
    Public Startup_Y As Double
    Public Startup_Z As Double

    Public Programed_Spindle_Speed As Double = 0

    Public Homing_Type As enum_Home_Type = enum_Home_Type.None
    Public Homing_Status As enum_Home_Status = enum_Home_Status.Not_Homing

    Public Help_Folder As String = "C:\Chip\Manuals\"

#End Region

#Region "Initialize, Finish"

    Public Sub Load_Settings_File()
        Dim DF As New Class_Data_File
        Dim Buffer As String
        Dim S() As String
        Buffer = DF.Read_All_Text(Chip_Settings_File)
        Settings_Lines = Buffer.Split(vbCrLf)

        For I = 0 To Settings_Lines.Count - 1
            Settings_Lines(I) = Settings_Lines(I).Replace(vbLf, "")
            Settings_Lines(I) = Settings_Lines(I).Replace("$$DEFAULT_BUILD_ROOT$$", Chip_Root_Path & "\Chip_Build")
            If Settings_Lines(I).Contains("=") Then
                S = Settings_Lines(I).Split("=")
                Settings.Add(S(1))
                Settings_Keys.Add(S(0))
            End If
        Next

        'Initialize_Controls(Contrl)

    End Sub

    Public Sub Finish(ByRef Contrl As Control)
        Dim DF As New Class_Data_File
        Settings_Buffer = ""
        Save_Persistant_Settings()
        DF.Write_All_Text(Chip_Settings_File, Settings_Buffer)
    End Sub

    Public Sub Initialize_Controls(ByRef Contrl As Control)

        Select Case Contrl.GetType.ToString

            Case "Chip.Main_Form", "System.Windows.Forms.Panel", "System.Windows.Forms.TabControl",
                  "System.Windows.Forms.TabPage", "System.Windows.Forms.GroupBox"
                For I = 0 To Contrl.Controls.Count - 1
                    Initialize_Controls(Contrl.Controls(I)) 'Recursive call
                Next

            Case "Chip.Chip_Text_Box"
                Dim txt As Chip_Text_Box = Contrl
                txt.Initialize()

            Case "Chip.Chip_Button"
                Dim btn As Chip_Button = Contrl
                btn.Initialize()

            Case "Chip.Chip_Digital_Input"
                Dim inp As Chip_Digital_Input = Contrl
                inp.Initialize()

            Case "Chip.Chip_Digital_Output"
                If Not SIMULATE Then
                    Dim out As Chip_Digital_Output = Contrl
                    out.Initialize()
                End If

            Case "Chip.Chip_Gcode"
                Dim ctl As Chip_Gcode = Contrl
                ctl.Initialize()

            Case "Chip.Chip_Drawing"
                Dim ctl As Chip_Drawing = Contrl
                ctl.Initialize(Maximum_X, Maximum_Y, Maximum_Z)

            Case "Chip.Chip_Fixture_Offsets"
                Dim ctl As Chip_Fixture_Offsets = Contrl
                ctl.Initialize()

            Case "Chip.Chip_Build"
                Dim ctl As Chip_Build = Contrl
                ctl.Initialize()

            Case "Chip.Chip_C_Interface"
                Dim ctl As Chip_C_Interface = Contrl
                ctl.Initialize()

            Case Else

        End Select

    End Sub

    Public Sub Set_Axis_Persistant_Positions()
        Kflop.X_Abs = Startup_X
        Kflop.Y_Abs = Startup_Y
        Kflop.Z_Abs = Startup_Z
    End Sub

#End Region

#Region "Persistant Settings"

    Public Sub Save_Persistant_Settings()

        '********************************************************************************************************
        'These settings are for the main form and some of the more complicated controls
        Save_Setting("Build_Root_Directory", Main_Form.txt_Build_Root_Directory.Text)
        Save_Setting("Main_Form.Top", Main_Form.Top)
        Save_Setting("Main_Form.Left", Main_Form.Left)
        Save_Setting("Main_Form.Width", Main_Form.Width)
        Save_Setting("Main_Form.Height", Main_Form.Height)
        For I = 0 To Main_Form.grd_Tab_Enables.Rows.Count - 1
            Save_Setting("Main_Tab.Page(" & Trim(Str(I)) & ")", Main_Form.grd_Tab_Enables.Rows(I).Cells(0).Value)
        Next

        Save_Setting("pnl_G_Code.Width", Main_Form.pnl_G_Code.Width)
        Save_Setting("ctl_Gcode.Information_Window_Height", Main_Form.ctl_Gcode.Information_Window_Height)
        Save_Setting("ctl_Gcode.G_Code_File_Path", Main_Form.ctl_Gcode.G_Code_File_Path)
        Save_Setting("ctl_Gcode.Routing_File_Path", Main_Form.ctl_Gcode.Routing_File_Path)
        Save_Setting("ctl_Gcode.Operation", Main_Form.ctl_Gcode.Operation)
        Save_Setting("ctl_Trace.Trace_Log_Limit", Main_Form.ctl_Trace.Trace_Log_Limit)

        Dim Params As New List(Of String)
        Main_Form.ctl_C_Interface.Get_Persistant_Data(Params)
        For I = 0 To Params.Count - 1
            Save_Setting("ctl_C_Interface.GoTo", Params(I))
        Next

        Save_Setting("ctl_Build.File_List_Width", Main_Form.ctl_Build.File_Display_Width)
        Save_Setting("ctl_Build.Directory_Tree_Width", Main_Form.ctl_Build.Directory_Tree_Width)
        Save_Setting("ctl_Build.Operations_Grid_Height", Main_Form.ctl_Build.Operations_Grid_Height)
        Save_Setting("ctl_Build.Route_File", Main_Form.ctl_Build.Route_File)

        Params = New List(Of String)
        Main_Form.ctl_Build.Get_Persistant_Node_Data(Params)
        For I = 0 To Params.Count - 1
            Save_Setting("ctl_Build.Expanded_Node", Params(I))
        Next
        Save_Setting("ctl_Build.Current_Directory", Main_Form.ctl_Build.Current_Directory)

        '********************************************************************************************************

        Save_Setting("txt_X_Abs.Text", Main_Form.txt_X_Abs.Text)
        Save_Setting("txt_Y_Abs.Text", Main_Form.txt_Y_Abs.Text)
        Save_Setting("txt_Z_Abs.Text", Main_Form.txt_Z_Abs.Text)

        Save_Setting("txt_Jog_Rate_Fast.Text", Main_Form.txt_Jog_Rate_Fast.Text)
        Save_Setting("txt_Jog_Rate_Slow.Text", Main_Form.txt_Jog_Rate_Slow.Text)
        Save_Setting("txt_Jog_Rate_Creep.Text", Main_Form.txt_Jog_Rate_Creep.Text)
        Save_Setting("txt_Jog_Rate_Big_Step.Text", Main_Form.txt_Jog_Rate_Big_Step.Text)
        Save_Setting("txt_Jog_Rate_Small_Step.Text", Main_Form.txt_Jog_Rate_Small_Step.Text)

        Save_Setting("txt_Probe_Diameter.Text", Main_Form.txt_Probe_Diameter.Text)
        Save_Setting("txt_Surface_X.Text", Main_Form.txt_Surface_X.Text)
        Save_Setting("txt_Surface_Y.Text", Main_Form.txt_Surface_Y.Text)
        Save_Setting("txt_Surface_Z.Text", Main_Form.txt_Surface_Z.Text)

        Save_Setting("txt_Edge_Finder_Diameter.Text", Main_Form.txt_Edge_Finder_Diameter.Text)
        Save_Setting("txt_Edge_Finder_RPM.Text", Main_Form.txt_Edge_Finder_RPM.Text)

        Save_Setting("txt_Tram_X_Plus.Text", Main_Form.txt_Tram_X_Plus.Text)
        Save_Setting("txt_Tram_X_Minus.Text", Main_Form.txt_Tram_X_Minus.Text)
        Save_Setting("txt_Tram_Y_Plus.Text", Main_Form.txt_Tram_Y_Plus.Text)
        Save_Setting("txt_Tram_Y_Minus.Text", Main_Form.txt_Tram_Y_Minus.Text)
        Save_Setting("txt_Tram_Z_Up.Text", Main_Form.txt_Tram_Z_Plus.Text)
        Save_Setting("txt_Tram_Z_Down.Text", Main_Form.txt_Tram_Z_Minus.Text)

        Save_Setting("txt_Tram_X_Plus.Text", Main_Form.txt_Tram_X_Plus.Text)
        Save_Setting("txt_Tram_X_Minus.Text", Main_Form.txt_Tram_X_Minus.Text)
        Save_Setting("txt_Tram_Y_Plus.Text", Main_Form.txt_Tram_Y_Plus.Text)
        Save_Setting("txt_Tram_Y_Minus.Text", Main_Form.txt_Tram_Y_Minus.Text)
        Save_Setting("txt_Tram_X_Plus.Text", Main_Form.txt_Tram_Z_Plus.Text)
        Save_Setting("txt_Tram_X_Minus.Text", Main_Form.txt_Tram_Z_Minus.Text)
        Save_Setting("txt_Tramming_Feedrate_XY.Text", Main_Form.txt_Tramming_Feedrate_XY.Text)
        Save_Setting("txt_Tramming_Feedrate_Z_Up.Text", Main_Form.txt_Tramming_Feedrate_Z_Up.Text)
        Save_Setting("txt_Tramming_Feedrate_Z_Down.Text", Main_Form.txt_Tramming_Feedrate_Z_Down.Text)

        Save_Setting("txt_Fixed_Tool_Setter_Bottom.Text", Main_Form.txt_Fixed_Tool_Setter_Bottom.Text)
        Save_Setting("txt_Fixed_Tool_Setter_Top.Text", Main_Form.txt_Fixed_Tool_Setter_Top.Text)
        Save_Setting("txt_Fixed_Tool_Setter_Measured_Height.Text", Main_Form.txt_Fixed_Tool_Setter_Measured_Height.Text)
        Save_Setting("txt_Fixed_Tool_Setter_Height.Text", Main_Form.txt_Fixed_Tool_Setter_Height.Text)

        Save_Setting("txt_Movable_Tool_Setter_Bottom.Text", Main_Form.txt_Movable_Tool_Setter_Bottom.Text)
        Save_Setting("txt_Movable_Tool_Setter_Top.Text", Main_Form.txt_Movable_Tool_Setter_Top.Text)
        Save_Setting("txt_Movable_Tool_Setter_Measured_Height.Text", Main_Form.txt_Movable_Tool_Setter_Measured_Height.Text)
        Save_Setting("txt_Movable_Tool_Setter_Height.Text", Main_Form.txt_Movable_Tool_Setter_Height.Text)

        Save_Setting("txt_Tool_Setter_Fixed_Surface.Text", Main_Form.txt_Tool_Setter_Fixed_Surface.Text)
        Save_Setting("txt_Tool_Setter_Fixture_Surface.Text", Main_Form.txt_Tool_Setter_Fixture_Surface.Text)
        Save_Setting("txt_Tool_Setter_Mount_Z_Delta.Text", Main_Form.txt_Tool_Setter_Mount_Z_Delta.Text)
        Save_Setting("txt_Tool_Setter_Material_Bottom.Text", Main_Form.txt_Tool_Setter_Material_Bottom.Text)
        Save_Setting("txt_Tool_Setter_Material_Top.Text", Main_Form.txt_Tool_Setter_Material_Top.Text)
        Save_Setting("txt_Tool_Setter_Material_Thickness.Text", Main_Form.txt_Tool_Setter_Material_Thickness.Text)


        '*******************************************************

        Save_Setting("ctl_Fixture_Offsets.Fixture.Text", Main_Form.ctl_Fixture_Offsets.txt_Fixture.Text)
        Save_Setting("ctl_Fixture_Offsets.Mount.Text", Main_Form.ctl_Fixture_Offsets.txt_Mount.Text)
        Save_Setting("ctl_Fixture_Offsets.Part.Text", Main_Form.ctl_Fixture_Offsets.txt_Part.Text)

        Save_Setting("ctl_Fixture_Offsets.X_Offset.Text", Main_Form.ctl_Fixture_Offsets.txt_X_Offset.Text)
        Save_Setting("ctl_Fixture_Offsets.Y_Offset.Text", Main_Form.ctl_Fixture_Offsets.txt_Y_Offset.Text)
        Save_Setting("ctl_Fixture_Offsets.Z_Offset.Text", Main_Form.ctl_Fixture_Offsets.txt_Z_Offset.Text)

        Save_Setting("ctl_Fixture_Offsets.X_Mount_Offset.Text", Main_Form.ctl_Fixture_Offsets.txt_X_Mount_Offset.Text)
        Save_Setting("ctl_Fixture_Offsets.Y_Mount_Offset.Text", Main_Form.ctl_Fixture_Offsets.txt_Y_Mount_Offset.Text)
        Save_Setting("ctl_Fixture_Offsets.Z_Mount_Offset.Text", Main_Form.ctl_Fixture_Offsets.txt_Z_Mount_Offset.Text)

        Save_Setting("ctl_Fixture_Offsets.X_Part_Offset.Text", Main_Form.ctl_Fixture_Offsets.txt_X_Part_Offset.Text)
        Save_Setting("ctl_Fixture_Offsets.Y_Part_Offset.Text", Main_Form.ctl_Fixture_Offsets.txt_Y_Part_Offset.Text)
        Save_Setting("ctl_Fixture_Offsets.Z_Part_Offset.Text", Main_Form.ctl_Fixture_Offsets.txt_Z_Part_Offset.Text)

        Save_Setting("ctl_Fixture_Offsets.txt_Adjust_Increment.Text", Main_Form.ctl_Fixture_Offsets.txt_Adjust_Increment.Text)

        Save_Setting("ctl_Fixture_Offsets.txt_Z_Tool_Measurement.Text", Main_Form.ctl_Fixture_Offsets.txt_Z_Tool_Measurement_Fixed.Text)
        Save_Setting("ctl_Fixture_Offsets.txt_Z_Fixed_Offset.Text", Main_Form.ctl_Fixture_Offsets.txt_Z_Fixed_Offset.Text)
        Save_Setting("ctl_Fixture_Offsets.txt_Z_Movable_Offset.Text", Main_Form.ctl_Fixture_Offsets.txt_Z_Movable_Offset.Text)
        Save_Setting("ctl_Fixture_Offsets.txt_Z_Tool_Measurement_Movable.Text", Main_Form.ctl_Fixture_Offsets.txt_Z_Tool_Measurement_Movable.Text)

        Save_Setting("ctl_Fixture_Offsets.txt_Fixed_Tool_Setter_Height.Text", Main_Form.txt_Fixed_Tool_Setter_Height.Text)
        Save_Setting("ctl_Fixture_Offsets.txt_Movable_Tool_Setter_Height.Text", Main_Form.ctl_Fixture_Offsets.txt_Movable_Tool_Setter_Height.Text)
        Save_Setting("ctl_Fixture_Offsets.txt_Z_Surface.Text", Main_Form.ctl_Fixture_Offsets.txt_Z_Surface_Offset.Text)
        Save_Setting("ctl_Fixture_Offsets.txt_Tool_Setter_Mount_Z_Delta.Text", Main_Form.ctl_Fixture_Offsets.txt_Mount_Delta_Z.Text)

        Save_Setting("txt_X_1.Text", Main_Form.txt_X_1.Text)
        Save_Setting("txt_X_2.Text", Main_Form.txt_X_2.Text)
        Save_Setting("txt_Y_1.Text", Main_Form.txt_Y_1.Text)
        Save_Setting("txt_Y_2.Text", Main_Form.txt_Y_2.Text)
        Save_Setting("txt_X_Difference", Main_Form.txt_X_Difference.Text)
        Save_Setting("txt_Y_Difference", Main_Form.txt_Y_Difference.Text)
        Save_Setting("txt_X_Center", Main_Form.txt_X_Center.Text)
        Save_Setting("txt_Y_Center", Main_Form.txt_Y_Center.Text)

        Save_Setting("txt_Square_Ratio", Main_Form.txt_Square_Ratio.Text)
        Save_Setting("txt_Square_Adjust", Main_Form.txt_Square_Adjust.Text)
        Save_Setting("txt_Square_Adjust_Pos", Main_Form.txt_Square_Adjust_Pos.Text)

        If Main_Form.ctl_Trace.chk_Enable_Tracing.Checked Then
            Save_Setting("chk_Enable_Tracing", "TRUE")
        Else
            Save_Setting("chk_Enable_Tracing", "FALSE")
        End If


    End Sub

    Public Sub Load_Persistant_Settings()

        ' Show_Error("Initializing")

        Startup_X = Get_Setting("txt_X_Abs.Text", 4)
        Startup_Y = Get_Setting("txt_Y_Abs.Text", 4)
        Startup_Z = Get_Setting("txt_Z_Abs.Text", 4)

        '***** These settings are for the main form and some of the more complicated controls
        Main_Form.txt_Build_Root_Directory.Text = Get_Setting("Build_Root_Directory", False)
        Main_Form.Top = Get_Setting("Main_Form.Top", True)
        Main_Form.Left = Get_Setting("Main_Form.Left", True)
        Main_Form.Width = Get_Setting("Main_Form.Width", True)
        Main_Form.Height = Get_Setting("Main_Form.Height", True)
        For I = 0 To Main_Form.grd_Tab_Enables.Rows.Count - 1
            If Get_Setting("Main_Tab.Page(" & Trim(Str(I)) & ")", False) = "Hidden" Then
                Main_Form.grd_Tab_Enables.Rows(I).Cells(0).Value = "Hidden"
                Main_Form.Tab_Main.TabPages.Remove(Main_Form.grd_Tab_Enables.Rows(I).Tag)
            End If
        Next

        Main_Form.pnl_G_Code.Width = Get_Setting("pnl_G_Code.Width", True)
        Main_Form.ctl_GCode.Information_Window_Height = Get_Setting("ctl_Gcode.Information_Window_Height", True)
        Main_Form.ctl_GCode.G_Code_File_Path = Get_Setting("ctl_Gcode.G_Code_File_Path", False)
        Main_Form.ctl_GCode.Routing_File_Path = Get_Setting("ctl_Gcode.Routing_File_Path", False)
        Main_Form.ctl_GCode.Operation = Get_Setting("ctl_Gcode.Operation", False)

        Main_Form.ctl_Trace.Trace_Log_Limit = Get_Setting("ctl_Trace.Trace_Log_Limit", True)

        Dim Params As New List(Of String)
        Params = Get_Settings("ctl_C_Interface.GoTo")
        Main_Form.ctl_C_Interface.Set_Persistant_Data(Params)

        Main_Form.ctl_Build.File_Display_Width = Get_Setting("ctl_Build.File_List_Width", True)
        Main_Form.ctl_Build.Directory_Tree_Width = Get_Setting("ctl_Build.Directory_Tree_Width", True)
        Main_Form.ctl_Build.Operations_Grid_Height = Get_Setting("ctl_Build.Operations_Grid_Height", True)
        Main_Form.ctl_Build.Route_File = Get_Setting("ctl_Build.Route_File", False)

        Params = New List(Of String)
        Params = Get_Settings("ctl_Build.Expanded_Node")
        Main_Form.ctl_Build.Set_Persistant_Data(Params)
        Main_Form.ctl_Build.Current_Directory = Get_Setting("ctl_Build.Current_Directory", False)


        Main_Form.txt_Jog_Rate_Fast.Text = Get_Setting("txt_Jog_Rate_Fast.Text", True)
        Main_Form.txt_Jog_Rate_Slow.Text = Get_Setting("txt_Jog_Rate_Slow.Text", True)
        Main_Form.txt_Jog_Rate_Creep.Text = Get_Setting("txt_Jog_Rate_Creep.Text", True)
        Main_Form.txt_Jog_Rate_Big_Step.Text = Get_Setting("txt_Jog_Rate_Big_Step.Text", True)
        Main_Form.txt_Jog_Rate_Small_Step.Text = Get_Setting("txt_Jog_Rate_Small_Step.Text", True, 4)

        Main_Form.txt_Probe_Diameter.Text = Get_Setting("txt_Probe_Diameter.Text", True)
        Main_Form.txt_Surface_X.Text = Get_Setting("txt_Surface_X.Text", True)
        Main_Form.txt_Surface_Y.Text = Get_Setting("txt_Surface_Y.Text", True)
        Main_Form.txt_Surface_Z.Text = Get_Setting("txt_Surface_Z.Text", True)

        Main_Form.txt_Edge_Finder_Diameter.Text = Get_Setting("txt_Edge_Finder_Diameter.Text", True)
        Main_Form.txt_Edge_Finder_RPM.Text = Get_Setting("txt_Edge_Finder_RPM.Text", True, 0)

        Main_Form.txt_Tram_X_Plus.Text = Get_Setting("txt_Tram_X_Plus.Text", True)
        Main_Form.txt_Tram_X_Minus.Text = Get_Setting("txt_Tram_X_Minus.Text", True)
        Main_Form.txt_Tram_Y_Plus.Text = Get_Setting("txt_Tram_Y_Plus.Text", True)
        Main_Form.txt_Tram_Y_Minus.Text = Get_Setting("txt_Tram_Y_Minus.Text", True)
        Main_Form.txt_Tram_Z_Plus.Text = Get_Setting("txt_Tram_Z_Up.Text", True)
        Main_Form.txt_Tram_Z_Minus.Text = Get_Setting("txt_Tram_Z_Down.Text", True)
        Main_Form.txt_Tramming_Feedrate_XY.Text = Get_Setting("txt_Tramming_Feedrate_XY.Text", True)
        Main_Form.txt_Tramming_Feedrate_Z_Up.Text = Get_Setting("txt_Tramming_Feedrate_Z_Up.Text", True)
        Main_Form.txt_Tramming_Feedrate_Z_Down.Text = Get_Setting("txt_Tramming_Feedrate_Z_Down.Text", True)

        Main_Form.txt_Fixed_Tool_Setter_Bottom.Text = Get_Setting("txt_Fixed_Tool_Setter_Bottom.Text", True, 4)
        Main_Form.txt_Fixed_Tool_Setter_Top.Text = Get_Setting("txt_Fixed_Tool_Setter_Top.Text", True, 4)
        Main_Form.txt_Fixed_Tool_Setter_Measured_Height.Text = Get_Setting("txt_Fixed_Tool_Setter_Measured_Height.Text", True, 4)
        Main_Form.txt_Fixed_Tool_Setter_Height.Text = Get_Setting("txt_Fixed_Tool_Setter_Height.Text", True, 4)

        Main_Form.txt_Movable_Tool_Setter_Bottom.Text = Get_Setting("txt_Movable_Tool_Setter_Bottom.Text", True, 4)
        Main_Form.txt_Movable_Tool_Setter_Top.Text = Get_Setting("txt_Movable_Tool_Setter_Top.Text", True, 4)
        Main_Form.txt_Movable_Tool_Setter_Measured_Height.Text = Get_Setting("txt_Movable_Tool_Setter_Measured_Height.Text", True, 4)
        Main_Form.txt_Movable_Tool_Setter_Height.Text = Get_Setting("txt_Movable_Tool_Setter_Height.Text", True, 4)

        Main_Form.txt_Tool_Setter_Fixed_Surface.Text = Get_Setting("txt_Tool_Setter_Fixed_Surface.Text", True, 4)
        Main_Form.txt_Tool_Setter_Fixture_Surface.Text = Get_Setting("txt_Tool_Setter_Fixture_Surface.Text", True, 4)
        Main_Form.txt_Tool_Setter_Mount_Z_Delta.Text = Get_Setting("txt_Tool_Setter_Mount_Z_Delta.Text", True, 4)
        Main_Form.txt_Tool_Setter_Material_Bottom.Text = Get_Setting("txt_Tool_Setter_Material_Bottom.Text", True, 4)
        Main_Form.txt_Tool_Setter_Material_Top.Text = Get_Setting("txt_Tool_Setter_Material_Top.Text", True, 4)
        Main_Form.txt_Tool_Setter_Material_Thickness.Text = Get_Setting("txt_Tool_Setter_Material_Thickness.Text", True, 4)

        'These must come after tool setter information
        '************************************************************************************************

        Main_Form.ctl_Fixture_Offsets.txt_Fixture.Text = Get_Setting("ctl_Fixture_Offsets.txt_Fixture.Text", False)
        Main_Form.ctl_Fixture_Offsets.txt_Mount.Text = Get_Setting("ctl_Fixture_Offsets.txt_Mount.Text", False)
        Main_Form.ctl_Fixture_Offsets.txt_Part.Text = Get_Setting("ctl_Fixture_Offsets.txt_Part.Text", False)

        Main_Form.ctl_Fixture_Offsets.txt_X_Offset.Text = Get_Setting("ctl_Fixture_Offsets.txt_X_Offset.Text", True, 4)
        Main_Form.ctl_Fixture_Offsets.txt_Y_Offset.Text = Get_Setting("ctl_Fixture_Offsets.txt_Y_Offset.Text", True, 4)
        Main_Form.ctl_Fixture_Offsets.txt_Z_Offset.Text = Get_Setting("ctl_Fixture_Offsets.txt_Z_Offset.Text", True, 4)

        Main_Form.ctl_Fixture_Offsets.txt_X_Mount_Offset.Text = Get_Setting("ctl_Fixture_Offsets.txt_X_Mount_Offset.Text", True, 4)
        Main_Form.ctl_Fixture_Offsets.txt_Y_Mount_Offset.Text = Get_Setting("ctl_Fixture_Offsets.txt_Y_Mount_Offset.Text", True, 4)
        Main_Form.ctl_Fixture_Offsets.txt_Z_Mount_Offset.Text = Get_Setting("ctl_Fixture_Offsets.txt_Z_Mount_Offset.Text", True, 4)

        Main_Form.ctl_Fixture_Offsets.txt_X_Part_Offset.Text = Get_Setting("ctl_Fixture_Offsets.txt_X_Part_Offset.Text", True, 4)
        Main_Form.ctl_Fixture_Offsets.txt_Y_Part_Offset.Text = Get_Setting("ctl_Fixture_Offsets.txt_Y_Part_Offset.Text", True, 4)
        Main_Form.ctl_Fixture_Offsets.txt_Z_Part_Offset.Text = Get_Setting("ctl_Fixture_Offsets.txt_Z_Part_Offset.Text", True, 4)

        Main_Form.ctl_Fixture_Offsets.txt_Adjust_Increment.Text = Get_Setting("ctl_Fixture_Offsets.txt_Adjust_Increment.Text", True)

        Main_Form.ctl_Fixture_Offsets.txt_Z_Tool_Measurement_Fixed.Text = Get_Setting("ctl_Fixture_Offsets.txt_Z_Tool_Measurement.Text", True, 4)
        Main_Form.ctl_Fixture_Offsets.txt_Z_Fixed_Offset.Text = Get_Setting("ctl_Fixture_Offsets.txt_Z_Fixed_Offset.Text", True, 4)
        Main_Form.ctl_Fixture_Offsets.txt_Z_Movable_Offset.Text = Get_Setting("ctl_Fixture_Offsets.txt_Z_Movable_Offset.Text", True)
        Main_Form.ctl_Fixture_Offsets.txt_Z_Tool_Measurement_Movable.Text = Get_Setting("ctl_Fixture_Offsets.txt_Z_Tool_Measurement_Movable.Text", True)

        Main_Form.ctl_Fixture_Offsets.Set_Fixed_Tool_Setter_Height(Get_Setting("ctl_Fixture_Offsets.txt_Fixed_Tool_Setter_Height.Text", True, 4))
        Main_Form.ctl_Fixture_Offsets.Set_Movable_Tool_Setter_Height(Get_Setting("ctl_Fixture_Offsets.txt_Movable_Tool_Setter_Height.Text", True, 4))
        Main_Form.ctl_Fixture_Offsets.txt_Z_Surface_Offset.Text = Get_Setting("ctl_Fixture_Offsets.txt_Z_Surface.Text", True, 4)

        Main_Form.ctl_Fixture_Offsets.txt_Mount_Delta_Z.Text = Get_Setting("ctl_Fixture_Offsets.txt_Tool_Setter_Mount_Z_Delta.Text", True)

        Main_Form.txt_X_1.Text = Get_Setting("txt_X_1.Text", True, 4)
        Main_Form.txt_X_2.Text = Get_Setting("txt_X_2.Text", True, 4)
        Main_Form.txt_Y_1.Text = Get_Setting("txt_Y_1.Text", True, 4)
        Main_Form.txt_Y_2.Text = Get_Setting("txt_Y_2.Text", True, 4)
        Main_Form.txt_X_Difference.Text = Get_Setting("txt_X_Difference", True, 4)
        Main_Form.txt_Y_Difference.Text = Get_Setting("txt_Y_Difference", True, 4)
        Main_Form.txt_X_Center.Text = Get_Setting("txt_X_Center", True, 4)
        Main_Form.txt_Y_Center.Text = Get_Setting("txt_Y_Center", True, 4)

        Main_Form.txt_Square_Ratio.Text = Get_Setting("txt_Square_Ratio", True, 4)
        Main_Form.txt_Square_Adjust.Text = Get_Setting("txt_Square_Adjust", True, 4)
        Main_Form.txt_Square_Adjust_Pos.Text = Get_Setting("txt_Square_Adjust_Pos", True, 4)

        If Get_Setting("chk_Enable_Tracing", False) = "TRUE" Then
            Main_Form.ctl_Trace.chk_Enable_Tracing.Checked = True
        Else
            Main_Form.ctl_Trace.chk_Enable_Tracing.Checked = False
        End If

    End Sub

    Public Sub Save_Setting(Key As String, Value As String)
        Value = Value.Replace(Chip_Root_Path & "\Chip_Build", "$$DEFAULT_BUILD_ROOT$$")
        Settings_Buffer &= Key & "=" & Value & vbCrLf
    End Sub

    Public Function Get_Setting(Key As String, Number As Boolean, Optional Digits As Integer = 3) As String
        Dim Idx As Integer = Settings_Keys.IndexOf(Key)
        If Idx >= 0 Then
            Dim S As String = Settings(Idx)
            If Number Then
                If Not IsNumeric(S) Then
                    Return 0
                Else
                    Return FormatNumber(S, Digits)
                End If
            Else
                Return S
            End If
        Else
            If Number Then
                Return "0"
            Else
                Return ""
            End If
        End If

    End Function

    Public Function Get_Settings(Key As String) As List(Of String)
        Dim Params As New List(Of String)

        For I = 0 To Settings_Keys.Count - 1
            If Settings_Keys(I) = Key Then
                Params.Add(Settings(I))
            End If
        Next
        Return Params
    End Function

#End Region

#Region "Utility Routines"

    Private Sub Log_Trace(Message As String)
        Trace_Control.Log_Trace_Message("[Handler] " & Message & vbCrLf)
    End Sub

    Public Function Run_Handler(ctl As Object, Evnt As enum_Event_Type, Warn As Boolean) As Boolean
        Try
            Return CallByName(Display, ctl.Handler, Microsoft.VisualBasic.CallType.Method, ctl, Evnt)
        Catch ex As Exception
            If Warn Then
                If ctl.handler <> "" Then
                    Show_Error("Error  : " & ctl.Name & vbCrLf &
                               "Caused by call to handler : " & ctl.Handler & vbCrLf & vbCrLf &
                               "Causes :" & vbCrLf & vbCrLf &
                               "Handler '" & ctl.Handler & "'       Does not exist" & vbCrLf & vbCrLf &
                               "Calling handler for wrong type of control ie. calling a Chip_Button handler with a Chip_Text_Box")
                End If
            End If
            Return False
        End Try

    End Function

    Public Function Get_Text_Number(S As String) As Single
        If IsNumeric(S) Then
            Return CSng(S)
        Else
            Return 0
        End If
    End Function

    Private Sub Open_Help_File(File_Name As String)
        Dim PDF_Proc = New System.Diagnostics.Process
        PDF_Proc.StartInfo.FileName = Help_Folder & File_Name & ".pdf"
        PDF_Proc.StartInfo.Verb = "Open"
        PDF_Proc.Start()
    End Sub

#End Region

    '*******************************************************************************************"


#Region "Control Handlers *******************************************************************************************"

    Public Sub Reset_Handlers()
        Jog_Buttons(Nothing, enum_Event_Type.Reset)
    End Sub

    Public Function C_Interface_Handler(Ctrl As Chip_C_Interface, Evnt As enum_Event_Type) As Object
        'Load Kflop C programs that will be used, with user data, which is used as parameters for C programs.

        Select Case Evnt
            Case enum_Event_Type.Initialize
                If SIMULATE Then Return Nothing

                Ctrl.Load_C_Program(KFlop.Initialiaze_C_Program, Class_KFLOP.enum_KFlop_Threads.Initialize)
                Ctrl.Load_C_Program(KFlop.Home_Goto_Probe_C_Program, Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)
                Ctrl.Load_C_Program(KFlop.Probe_Toolsetter_Saftey_C_Program, Class_KFLOP.enum_KFlop_Threads.Probe_Toolsetter_Saftety)

                Ctrl.Run_C_Program(Class_KFLOP.enum_KFlop_Threads.Initialize)
                Ctrl.Run_C_Program(Class_KFLOP.enum_KFlop_Threads.Probe_Toolsetter_Saftety)

                KFlop.Enable_Soft_Limits()

                'Spindle Control
                Ctrl.Load_C_Program(KFlop.Spindle_C_Program, Class_KFLOP.enum_KFlop_Threads.Spindle_Control)
                Ctrl.Set_Spindle_Speed(0)
                Ctrl.Set_Spindle_Override(100)


                'Commands to C programs
                KFlop.Set_User_Data_Integer(Chip_C_Interface.enum_User_Data_Address.Command, Chip_C_Interface.enum_Commands.Idle)

                'Destination of move
                KFlop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.X_Params + Chip_C_Interface.enum_User_Data_Offsets.Dest_1, 0)
                KFlop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.X_Params + Chip_C_Interface.enum_User_Data_Offsets.Dest_2, 0)
                KFlop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.Y_Params + Chip_C_Interface.enum_User_Data_Offsets.Dest_1, 0)
                KFlop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.Y_Params + Chip_C_Interface.enum_User_Data_Offsets.Dest_2, 0)
                KFlop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.Z_Params + Chip_C_Interface.enum_User_Data_Offsets.Dest_1, 6)
                KFlop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.Z_Params + Chip_C_Interface.enum_User_Data_Offsets.Dest_2, 6)

                'Feedrates
                KFlop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.X_Params + Chip_C_Interface.enum_User_Data_Offsets.Feed_1, 0)
                KFlop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.X_Params + Chip_C_Interface.enum_User_Data_Offsets.Feed_2, 0)
                KFlop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.Y_Params + Chip_C_Interface.enum_User_Data_Offsets.Feed_1, 0)
                KFlop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.Y_Params + Chip_C_Interface.enum_User_Data_Offsets.Feed_2, 0)
                KFlop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.Z_Params + Chip_C_Interface.enum_User_Data_Offsets.Feed_1, 0)
                KFlop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.Z_Params + Chip_C_Interface.enum_User_Data_Offsets.Feed_2, 0)

                'Position of switch, probe or toolsetter touch
                KFlop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.Pos_1, 0)
                KFlop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.Pos_2, 0)

                Ctrl.Update()

        End Select

        Return Nothing

    End Function

    Public Sub Update_Display()

        '**************************************************************************************
        'The following are manditory updates to the main form, CHANGE AT YOUR OWN RISK
        Main_Form.ctl_C_Interface.Update_Status()
        Main_Form.txt_Active_G_Codes.Text = KFlop.Active_G_Codes
        Main_Form.txt_Active_M_Codes.Text = KFlop.Active_M_Codes & "  Feed : " & KFlop.Feed_Rate & " Speed : " & KFlop.Spindle_Speed

        If KFlop.GCode_Program_Done Then
            Main_Form.ctl_GCode.Execution_Complete()
            KFlop.GCode_Program_Done = False
            Main_Form.btn_Cycle_Start.State = "Cycle Stop"
        End If
        Pendant_Update_Display()
        '**************************************************************************************


        'THE FOLLOWING ARE SET UP TO UPDATE DEVELOPER WRITTEN HANDLERS
        '**************************************************************************************
        'The following handlers run Kflop C Progams, must be followed by Kflop.Sync_C_Program()
        'Kflop.Sync_C_Program acknowoleges the sync signal that indicates the end of the c program.
        If KFlop.Status_Pending Then
            Home_Buttons(Nothing, enum_Event_Type.Update)
            Goto_Buttons(Nothing, enum_Event_Type.Update)
            Tramming_Buttons(Nothing, enum_Event_Type.Update)
            Probe_Buttons(Nothing, enum_Event_Type.Update)
            Tool_Setter_Buttons(Nothing, enum_Event_Type.Update)
            Square_Buttons(Nothing, enum_Event_Type.Update)
            Spindle_Handler(enum_Event_Type.Update)

            Main_Form.ctl_Fixture_Offsets.Status_Update()

            Dim Mess As String = ""
            Dim Major_Stat As String = KFlop.C_Program_Status.ToString
            Dim Minor_Stat As String = KFlop.C_Program_Status.ToString
            Dim Major_Num As Chip_C_Interface.enum_Status
            Dim Minor_Num As Chip_C_Interface.enum_Moving_To_Position_Status_Offsets

            Major_Stat = Mid(Major_Stat, 1, Major_Stat.Length - 1) & "0"
            Minor_Stat = Mid(Minor_Stat, Minor_Stat.Length, 1)

            Major_Num = CInt(Major_Stat)
            Minor_Num = CInt(Minor_Stat)

            Mess = "(" & KFlop.C_Program_Status.ToString & "-" & Major_Stat & "-" & Minor_Stat & ")   " & Major_Num.ToString & " : " & Minor_Num.ToString

            'RWC Uncomment for debug
            'Trace_Control.Log_Trace_Message("[C_Program_Status] " & Mess & vbCrLf)

            KFlop.Clear_Status_Pending()
        End If

        '**************************************************************************************

        'THE FOLLOWING ARE SET UP TO UPDATE DEVELOPER WRITTEN HANDLERS
        '**************************************************************************************
        'Put updates to developer written handlers here
        Digital_Outputs(Nothing, enum_Event_Type.Update)
        Digital_Inputs(Nothing, enum_Event_Type.Update)
        Position_Text_Boxes(Nothing, enum_Event_Type.Update)
        Axis_Enable_Buttons(Nothing, enum_Event_Type.Update)
        Cycle_Buttons(Nothing, enum_Event_Type.Update)
        '**************************************************************************************

    End Sub

    Public Function Toolsetter_Tripped() As Boolean

        Dim Mess As String = ""
        If Main_Form.led_Fixed_Tool_Setter.State = "On" Then
            Mess = "Fixed toolsetter in contact." & vbCrLf
        End If
        If Main_Form.led_Movable_Tool_Setter.State = "On" Then
            Mess &= "Movable toolsetter in contact." & vbCrLf
        End If

        If Mess <> "" Then

            Mess &= vbCrLf & "At startup toolsetters must be touched to be reset," & vbCrLf &
                    "Otherwise something is in contact with toolsetter," & vbCrLf &
                    "or batteries need to be changed."
            Show_Error(Mess)
            Return True
        End If

        Return False

    End Function

    Public Function Digital_Inputs(Input As Chip_Digital_Input, Evnt As enum_Event_Type) As Object

        Select Case Evnt
            Case enum_Event_Type.Initialize
                Input.State = Input.Get_Input_State("On", "Off")

            Case enum_Event_Type.Update
                Main_Form.led_X_Home.State = Main_Form.led_X_Home.Get_Input_State("On", "Off")
                Main_Form.led_X_Limit.State = Main_Form.led_X_Limit.Get_Input_State("On", "Off")
                Main_Form.led_Y_Home.State = Main_Form.led_Y_Home.Get_Input_State("On", "Off")
                Main_Form.led_Y_Limit.State = Main_Form.led_Y_Limit.Get_Input_State("On", "Off")
                Main_Form.led_Z_Home.State = Main_Form.led_Z_Home.Get_Input_State("On", "Off")
                Main_Form.led_Z_Limit.State = Main_Form.led_Z_Limit.Get_Input_State("On", "Off")
                Main_Form.led_Probe.State = Main_Form.led_Probe.Get_Input_State("On", "Off")
                Main_Form.led_Movable_Tool_Setter.State = Main_Form.led_Movable_Tool_Setter.Get_Input_State("On", "Off")
                Main_Form.led_Fixed_Tool_Setter.State = Main_Form.led_Fixed_Tool_Setter.Get_Input_State("On", "Off")
                Main_Form.led_E_Stop_Button.State = Main_Form.led_E_Stop_Button.Get_Input_State("On", "Off")
        End Select

        Return Nothing
    End Function

    Public Function Digital_Outputs(Output As Chip_Digital_Output, Evnt As enum_Event_Type) As Object

        Select Case Evnt
            Case enum_Event_Type.Initialize
                Output.State = Output.Get_Boolean_Output("On", "Off")

            Case enum_Event_Type.Update
                Main_Form.led_Axis_Enable.State = Main_Form.led_Axis_Enable.Get_Boolean_Output("On", "Off")
                Main_Form.led_Spindle_Enable.State = Main_Form.led_Spindle_Enable.Get_Boolean_Output("On", "Off")
                Main_Form.led_Coolant_Enable.State = Main_Form.led_Coolant_Enable.Get_Boolean_Output("On", "Off")

        End Select
        Return Nothing
    End Function


    Public Function Spindle_Handler(Evnt As enum_Event_Type) As Object

        If KFlop.Controller.ThreadExecuting(Class_KFLOP.enum_KFlop_Threads.Spindle_Control) Then

            Select Case Evnt
                Case enum_Event_Type.Initialize
                    Main_Form.txt_Spindle_Rpm.Text = "0"

                Case enum_Event_Type.Update
                    Select Case KFlop.C_Program_Status
                        Case Chip_C_Interface.enum_Status.Idle

                        Case Chip_C_Interface.enum_Status.Spindle_Status + Chip_C_Interface.enum_Spindle_Status_Offsets.RPM_Change
                            Dim RPM As Single = KFlop.Get_User_Data_Float((Chip_C_Interface.enum_User_Data_Address.Spindle_RPM))
                            If RPM = 0 Then
                                Main_Form.txt_Spindle_Rpm.Text = "0"
                            Else
                                Main_Form.txt_Spindle_Rpm.Text = FormatNumber(RPM, 0, TriState.False, TriState.True)
                            End If
                        Case Chip_C_Interface.enum_Status.Spindle_Status + Chip_C_Interface.enum_Spindle_Status_Offsets.RPM_Too_High
                            Main_Form.txt_Spindle_Rpm.Text = FormatNumber(Max_Spindle_RPM, 0, TriState.False, TriState.True)
                            Main_Form.track_Spindle_Speed_Override.Value = (Max_Spindle_RPM / KFlop.Spindle_Speed) * 100



                            'Flash_Message("RPM too high. Max RPM = " & Max_Spindle_RPM)
                    End Select

            End Select

        End If

        Return Nothing

    End Function

    Public Sub Cycle_Stop()

        If KFlop.GCode_Program_Executing Then
            KFlop.Feed_Hold = True
            KFlop.GCode_Program_Aborted = True
            KFlop.Cycle_Stop()
        Else
            KFlop.GCode_Program_Aborted = False
            KFlop.X_Stop()
            KFlop.Y_Stop()
            KFlop.Z_Stop()
            KFlop.Kill_All_C_Programs()
            KFlop.Enable_Soft_Limits()
        End If
        Main_Form.btn_Cycle_Start.State = "Cycle Stop"
        Axis_Enable_Buttons(Nothing, enum_Event_Type.Cycle_Stop)
        Cycle_Buttons(Nothing, enum_Event_Type.Cycle_Stop)
        Home_Buttons(Nothing, enum_Event_Type.Cycle_Stop)
        Goto_Buttons(Nothing, enum_Event_Type.Cycle_Stop)
        Tramming_Buttons(Nothing, enum_Event_Type.Cycle_Stop)
        Probe_Buttons(Nothing, enum_Event_Type.Cycle_Stop)
        Tool_Setter_Buttons(Nothing, enum_Event_Type.Cycle_Stop)
        Main_Form.ctl_Fixture_Offsets.Cycle_Stop()

    End Sub

    Public Function Axis_Enable_Buttons(ByRef Ctrl As Chip_Button, ByVal Evnt As enum_Event_Type) As Object

        Select Case Evnt

            Case enum_Event_Type.Initialize

            Case enum_Event_Type.Left_Mouse_Button_Down

                If Main_Form.btn_Cycle_Start.State = "In Cycle" Then
                    Flash_Message("Cannot disable/enable axes while in Cycle.")
                End If

                Select Case Ctrl.Name

                    Case "btn_Disable_Limits"
                        If Main_Form.btn_Disable_Limits.State = "Off" Then
                            Main_Form.btn_Disable_Limits.State = "On"
                            Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Disable Limit Switches", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)
                        Else
                            Main_Form.btn_Disable_Limits.State = "Off"
                            Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Enable Limit Switches", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)
                        End If

                    Case "btn_Enable_All"
                        If KFlop.Axis_Master_Enable Then
                            KFlop.Axis_Master_Enable = False
                        Else
                            KFlop.Axis_Master_Enable = True
                            KFlop.Enable_Soft_Limits()
                        End If

                    Case "btn_Enable_X"
                        If KFlop.X_Enable Then
                            KFlop.X_Enable = False
                        Else
                            KFlop.X_Enable = True
                            KFlop.Enable_Soft_Limits()
                        End If

                    Case "btn_Enable_Y"
                        If KFlop.Y_Enable Then
                            KFlop.Y_Enable = False
                        Else
                            KFlop.Y_Enable = True
                            KFlop.Enable_Soft_Limits()
                        End If

                    Case "btn_Enable_Z"
                        If KFlop.Z_Enable Then
                            KFlop.Z_Enable = False
                        Else
                            KFlop.Z_Enable = True
                            KFlop.Enable_Soft_Limits()
                        End If

                End Select

            Case enum_Event_Type.Left_Mouse_Button_Up


            Case enum_Event_Type.Update
                If KFlop.Axis_Master_Enable Then
                    Main_Form.btn_Enable_All.State = "Enabled"
                Else
                    Main_Form.btn_Enable_All.State = "Disabled"
                End If

                If KFlop.X_Enable Then
                    Main_Form.btn_Enable_X.State = "Enabled"
                Else
                    Main_Form.btn_Enable_X.State = "Disabled"
                End If

                If KFlop.Y_Enable Then
                    Main_Form.btn_Enable_Y.State = "Enabled"
                Else
                    Main_Form.btn_Enable_Y.State = "Disabled"
                End If

                If KFlop.Z_Enable Then
                    Main_Form.btn_Enable_Z.State = "Enabled"
                Else
                    Main_Form.btn_Enable_Z.State = "Disabled"
                End If

        End Select

        Return True
    End Function

    Public Function Cycle_Buttons(ByRef Ctrl As Chip_Button, ByVal Evnt As enum_Event_Type) As Object
        Static Raise_Z As Boolean = False

        Select Case Evnt

            Case enum_Event_Type.Initialize

            Case enum_Event_Type.Cycle_Stop
                Main_Form.led_Spindle_Enable.Set_Boolean_Output(False, "Off")
                Main_Form.ctl_C_Interface.Set_Spindle_Speed(0)
                Main_Form.led_Coolant_Enable.Set_Boolean_Output(False, "Off")

            Case enum_Event_Type.Left_Mouse_Button_Down

                Select Case Ctrl.Name
                    Case "btn_E_Stop"
                        If Ctrl.State = "Off" Then Ctrl.State = "On" Else Ctrl.State = "Off"

                    Case "btn_Cycle_Start"

                        If Ctrl.State = "In Cycle" Then 'Cycle Stop
                            'If Not KFlop.Controller.CoordMotion.Interpreter.IsHalt Then Raise_Z = True 'Program running, raise Z
                            Programed_Spindle_Speed = KFlop.Spindle_Speed

                            Cycle_Stop()

                            Return True
                        End If

                        If Main_Form.btn_Tram_Enable.State <> "Off" Then
                            Show_Error("Cannot cycle start with tramming enabled.")
                            Return Nothing
                        End If

                        If Main_Form.led_Probe.State = "On" Then
                            Show_Error("Cannot cycle start with probe mounted.")
                            Return Nothing
                        End If

                        If Main_Form.led_Probe.State = "On" Then
                            Show_Error("Cannot cycle start with probe mounted.")
                            Return Nothing
                        End If

                        If Main_Form.btn_Probe_Enable.State = "On" Then
                            Show_Error("Cannot cycle start with probe enabled.")
                            Return Nothing
                        End If

                        If Main_Form.btn_Edge_Finder_Enable.State = "On" Then
                            Show_Error("Cannot cycle start with edge finder enabled.")
                            Return Nothing
                        End If

                        If Main_Form.btn_Spindle_Test.State = "On" Then
                            Show_Error("Cannot cycle start in Spindle Test mode.")
                            Return Nothing
                        End If


                        If Not Main_Form.ctl_GCode.Program_Loaded Then
                            Show_Error("No Program Loaded.")
                            Return False
                        End If

                        If Not Main_Form.ctl_Fixture_Offsets.Offsets_OK Then
                            Return False
                        End If

                        If Main_Form.ctl_Build.Fixture_Offset_Type = "Current XY" Then
                            Dim dlg As New dlg_Message_Box
                            dlg.ShowDialog("This program will run at current X, Y" & vbCrLf & "Continue?", "Warning", MessageBoxButtons.YesNo)
                            If dlg.DialogResult = DialogResult.Yes Then
                                Dim Z_Offset As Single = Main_Form.ctl_Fixture_Offsets.Get_Z_Offset
                                KFlop.Set_Fixture_Offsets(Class_KFLOP.enum_Coorinate_System.G55, KFlop.X_Abs, KFlop.Y_Abs, Get_Text_Number(Z_Offset))
                            Else
                                Return False
                            End If
                        End If

                        'Position cursor on Cycle button so stopping cycle is easier if necessary
                        Dim X As Integer = Main_Form.Left + Main_Form.Panel_Top.Left + Main_Form.btn_Cycle_Start.Left + (Main_Form.btn_Cycle_Start.Width / 2)
                        Dim Y As Integer = Main_Form.Top + Main_Form.Panel_Top.Top + Main_Form.btn_Cycle_Start.Top + (Main_Form.btn_Cycle_Start.Height / 2)
                        Mouse_Control.Move_Cursor(X, Y)

                        Save_X = Math.Round(KFlop.X_Abs, 4)
                        Save_Y = Math.Round(KFlop.Y_Abs, 4)
                        Save_Z = Math.Round(KFlop.Z_Abs, 4)


                        If KFlop.Cycle_Start() Then
                            Ctrl.State = "In Cycle"
                            KFlop.Feed_Hold = False
                        End If

                    Case "btn_Feed"
                        If Ctrl.State = "Off" Then
                            Ctrl.State = "On"
                            Programed_Spindle_Speed = KFlop.Spindle_Speed
                            KFlop.Feed_Hold = True
                        Else
                            If Main_Form.btn_Cycle_Start.State = "In Cycle" Then
                                If Main_Form.btn_Spindle.State = "Off" Then
                                    Flash_Message("Cannot resume feed when in cycle and spindle stopped.")
                                    Return Nothing
                                End If
                            End If

                            Ctrl.State = "Off"
                            KFlop.Feed_Hold = False
                        End If

                    Case "btn_Spindle_Test"
                        If Main_Form.btn_Spindle_Test.State = "Off" Then
                            If Main_Form.led_Probe.State = "On" Then
                                Show_Error("Cannot start spindle with probe mounted.")
                                Return Nothing
                            End If

                            If Main_Form.btn_Cycle_Start.State = "In Cycle" Then
                                If Main_Form.btn_Feed.State = "Off" Then
                                    Flash_Message("Cannot do this function while in Cycle.")
                                    Return Nothing
                                End If
                            End If
                            Main_Form.track_Spindle_Speed.Value = 0
                            Main_Form.btn_Spindle_Test.State = "On"

                        Else
                            Main_Form.led_Spindle_Enable.Set_Boolean_Output(False, "Off")
                            Main_Form.ctl_C_Interface.Set_Spindle_Speed(0)
                            Main_Form.track_Spindle_Speed.Value = 0
                            Main_Form.btn_Spindle_Test.State = "Off"
                        End If

                    Case "btn_Spindle"

                        If Main_Form.led_Probe.State = "On" Then
                            Show_Error("Cannot start spindle with probe mounted.")
                            Return Nothing
                        End If

                        If Main_Form.btn_Cycle_Start.State = "In Cycle" Then
                            If Main_Form.btn_Feed.State = "Off" Then
                                Flash_Message("Cannot stop spindle when in cycle and feeding.")
                                Return Nothing
                            End If
                        End If

                        If Main_Form.led_Spindle_Enable.State = "Off" Then
                            If Main_Form.btn_Spindle_Test.State = "On" Then

                                If Main_Form.btn_Spindle_Test.State = "On" Then
                                    Main_Form.ctl_C_Interface.Set_Spindle_Speed(Main_Form.track_Spindle_Speed.Value)
                                    Main_Form.led_Spindle_Enable.Set_Boolean_Output(True, "On")
                                End If
                            Else
                                Dim dlg As New dlg_Message_Box
                                dlg.ShowDialog("Current programmed speed : " & FormatNumber(Programed_Spindle_Speed, 0) & vbCrLf &
                                               "Start Spindle ?", "Warning", MessageBoxButtons.YesNo)
                                If dlg.DialogResult = DialogResult.Yes Then
                                    Main_Form.ctl_C_Interface.Set_Spindle_Speed(Programed_Spindle_Speed)
                                    Main_Form.led_Spindle_Enable.Set_Boolean_Output(True, "On")
                                Else
                                    Return Nothing
                                End If

                            End If

                        Else
                            Main_Form.led_Spindle_Enable.Set_Boolean_Output(False, "Off")
                            Main_Form.ctl_C_Interface.Set_Spindle_Speed(0)
                            Main_Form.btn_Edge_Finder_Start_Spindle.State = "Off"
                        End If

                    Case "btn_Coolant"
                        If Main_Form.led_Coolant_Enable.State = "Disabled" Then Return False
                        If Main_Form.led_Coolant_Enable.State = "Off" Then
                            Main_Form.led_Coolant_Enable.Set_Boolean_Output(True, "On")
                        Else
                            Main_Form.led_Coolant_Enable.Set_Boolean_Output(False, "Off")
                        End If

                End Select

            Case enum_Event_Type.Left_Mouse_Button_Up


            Case enum_Event_Type.Update

                If Main_Form.led_Spindle_Enable.State = "On" Then
                    Main_Form.btn_Spindle.State = "On"
                Else
                    Main_Form.btn_Spindle.State = "Off"
                End If

                If Main_Form.led_Coolant_Enable.State = "On" Then
                    Main_Form.btn_Coolant.State = "On"
                Else
                    Main_Form.btn_Coolant.State = "Off"
                End If

        End Select

        Return True

    End Function

    Public Function Home_Buttons(ByRef Ctrl As Chip_Button, Evnt As enum_Event_Type) As Object

        Select Case Evnt

            Case enum_Event_Type.Initialize


            Case enum_Event_Type.Cycle_Stop
                Homing_Type = enum_Home_Type.None
                Homing_Status = enum_Home_Status.Not_Homing
                'Main_Form.btn_Home.State = "Not Homed"
                'Main_Form.btn_Home_All.State = "Not Homed"
                'Main_Form.btn_Home_X.State = "Not Homed"
                'Main_Form.btn_Home_Y.State = "Not Homed"
                'Main_Form.btn_Home_Z.State = "Not Homed"
                Return True

            Case enum_Event_Type.Left_Mouse_Button_Down
                If Toolsetter_Tripped() Then
                    Main_Form.btn_Cycle_Start.State = "Cycle Stop"
                    Homing_Status = enum_Home_Status.Not_Homing
                    Homing_Type = enum_Home_Type.None
                    Return Nothing
                End If

                If Homing_Type <> enum_Home_Type.None Then
                    Cycle_Stop()
                    Return Nothing
                End If

                If Main_Form.btn_Cycle_Start.State = "In Cycle" Then
                    Show_Error("Cannot Home while in cycle.")
                    Return Nothing
                End If

                KFlop.Feed_Hold = False 'May be in feed hold because of soft limits

                Select Case Ctrl.Name

                    Case "btn_Home_All", "btn_Home"
                        Homing_Type = enum_Home_Type.All_Axes
                        Homing_Status = enum_Home_Status.Homing_All_Axes
                        Main_Form.btn_Home.State = "Not Homed"
                        Main_Form.btn_Home_All.State = "Not Homed"
                        Main_Form.btn_Home_X.State = "Not Homed"
                        Main_Form.btn_Home_Y.State = "Not Homed"
                        Main_Form.btn_Home_Z.State = "Not Homed"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Home All", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)

                    Case "btn_Home_X"
                        Homing_Type = enum_Home_Type.X_Axis
                        Homing_Status = enum_Home_Status.Homing_X_Axis
                        Ctrl.State = "Not Homed"
                        Main_Form.btn_Home.State = "Not Homed"
                        Main_Form.btn_Home_All.State = "Not Homed"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Home X", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)

                    Case "btn_Home_Y"
                        Homing_Type = enum_Home_Type.Y_Axis
                        Homing_Status = enum_Home_Status.Homing_Y_Axis
                        Ctrl.State = "Not Homed"
                        Main_Form.btn_Home.State = "Not Homed"
                        Main_Form.btn_Home_All.State = "Not Homed"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Home Y", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)

                    Case "btn_Home_Z"
                        Homing_Type = enum_Home_Type.Z_Axis
                        Homing_Status = enum_Home_Status.Homing_Z_Axis
                        Ctrl.State = "Not Homed"
                        Main_Form.btn_Home.State = "Not Homed"
                        Main_Form.btn_Home_All.State = "Not Homed"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Home Z", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)

                End Select

            Case enum_Event_Type.Left_Mouse_Button_Up

            Case enum_Event_Type.Update

                If KFlop.Controller.ThreadExecuting(Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe) Then

                    Select Case KFlop.C_Program_Status
                        Case Chip_C_Interface.enum_Status.Idle

                        Case Chip_C_Interface.enum_Status.Home_X_Status + Chip_C_Interface.enum_Moving_To_Switch_Status_Offsets.Moving_To_Switch
                            Main_Form.btn_Home.State = "Homing X -"
                            Main_Form.btn_Home_All.State = "Homing X -"
                            Main_Form.btn_Home_X.State = "Moving To Switch"

                        Case Chip_C_Interface.enum_Status.Home_X_Status + Chip_C_Interface.enum_Moving_To_Switch_Status_Offsets.Moving_To_Latch
                            Main_Form.btn_Home_X.State = "Moving To Latch"

                        Case Chip_C_Interface.enum_Status.Home_X_Status + Chip_C_Interface.enum_Moving_To_Switch_Status_Offsets.Moving_Off_Switch
                            Main_Form.btn_Home.State = "Homing X +"
                            Main_Form.btn_Home_All.State = "Homing X +"
                            Main_Form.btn_Home_X.State = "Moving Off Switch"

                        Case Chip_C_Interface.enum_Status.Home_X_Status + Chip_C_Interface.enum_Moving_To_Switch_Status_Offsets.Moving_To_Backoff
                            Main_Form.btn_Home_X.State = "Moving To Backoff"

                        Case Chip_C_Interface.enum_Status.Home_X_Status + Chip_C_Interface.enum_Moving_To_Switch_Status_Offsets.Homed
                            Homing_Status = enum_Home_Status.X_Axis_Homed
                            Main_Form.btn_Home_X.State = "Homed"
                            Main_Form.btn_Home.State = "Not Homed"
                            Main_Form.btn_Home_All.State = "Not Homed"

                        Case Chip_C_Interface.enum_Status.Home_Y_Status + Chip_C_Interface.enum_Moving_To_Switch_Status_Offsets.Moving_To_Switch
                            Main_Form.btn_Home.State = "Homing Y -"
                            Main_Form.btn_Home_All.State = "Homing Y -"
                            Main_Form.btn_Home_Y.State = "Moving To Switch"

                        Case Chip_C_Interface.enum_Status.Home_Y_Status + Chip_C_Interface.enum_Moving_To_Switch_Status_Offsets.Moving_To_Latch
                            Main_Form.btn_Home_Y.State = "Moving To Latch"

                        Case Chip_C_Interface.enum_Status.Home_Y_Status + Chip_C_Interface.enum_Moving_To_Switch_Status_Offsets.Moving_Off_Switch
                            Main_Form.btn_Home.State = "Homing Y +"
                            Main_Form.btn_Home_All.State = "Homing Y +"
                            Main_Form.btn_Home_Y.State = "Moving Off Switch"

                        Case Chip_C_Interface.enum_Status.Home_Y_Status + Chip_C_Interface.enum_Moving_To_Switch_Status_Offsets.Moving_To_Backoff
                            Main_Form.btn_Home_Y.State = "Moving To Backoff"

                        Case Chip_C_Interface.enum_Status.Home_Y_Status + Chip_C_Interface.enum_Moving_To_Switch_Status_Offsets.Homed
                            Homing_Status = enum_Home_Status.Y_Axis_Homed
                            Main_Form.btn_Home_Y.State = "Homed"
                            Main_Form.btn_Home.State = "Not Homed"
                            Main_Form.btn_Home_All.State = "Not Homed"

                        Case Chip_C_Interface.enum_Status.Home_Z_Status + Chip_C_Interface.enum_Moving_To_Switch_Status_Offsets.Moving_To_Switch
                            Main_Form.btn_Home.State = "Homing Z +"
                            Main_Form.btn_Home_All.State = "Homing Z +"
                            Main_Form.btn_Home_Z.State = "Moving To Switch"

                        Case Chip_C_Interface.enum_Status.Home_Z_Status + Chip_C_Interface.enum_Moving_To_Switch_Status_Offsets.Moving_To_Latch
                            Main_Form.btn_Home_Z.State = "Moving To Latch"

                        Case Chip_C_Interface.enum_Status.Home_Z_Status + Chip_C_Interface.enum_Moving_To_Switch_Status_Offsets.Moving_Off_Switch
                            Main_Form.btn_Home.State = "Homing Z -"
                            Main_Form.btn_Home_All.State = "Homing Z -"
                            Main_Form.btn_Home_Z.State = "Moving Off Switch"

                        Case Chip_C_Interface.enum_Status.Home_Z_Status + Chip_C_Interface.enum_Moving_To_Switch_Status_Offsets.Moving_To_Backoff
                            Main_Form.btn_Home_Z.State = "Moving To Backoff"

                        Case Chip_C_Interface.enum_Status.Home_Z_Status + Chip_C_Interface.enum_Moving_To_Switch_Status_Offsets.Homed
                            Homing_Status = enum_Home_Status.Z_Axis_Homed
                            Main_Form.btn_Home_Z.State = "Homed"
                            Main_Form.btn_Home.State = "Not Homed"
                            Main_Form.btn_Home_All.State = "Not Homed"
                            KFlop.Z_Abs = Maximum_Z
                    End Select

                    Select Case Homing_Type
                        Case enum_Home_Type.All_Axes

                        Case enum_Home_Type.X_Axis
                            If Homing_Status = enum_Home_Status.X_Axis_Homed Then
                                Main_Form.btn_Cycle_Start.State = "Cycle Stop"
                                Homing_Status = enum_Home_Status.Not_Homing
                                Homing_Type = enum_Home_Type.None
                            End If
                        Case enum_Home_Type.Y_Axis
                            If Homing_Status = enum_Home_Status.Y_Axis_Homed Then
                                Main_Form.btn_Cycle_Start.State = "Cycle Stop"
                                Homing_Status = enum_Home_Status.Not_Homing
                                Homing_Type = enum_Home_Type.None
                            End If
                        Case enum_Home_Type.Z_Axis
                            If Homing_Status = enum_Home_Status.Z_Axis_Homed Then
                                Main_Form.btn_Cycle_Start.State = "Cycle Stop"
                                Homing_Status = enum_Home_Status.Not_Homing
                                Homing_Type = enum_Home_Type.None
                            End If
                    End Select

                    If Main_Form.btn_Home_X.State = "Homed" And
                       Main_Form.btn_Home_Y.State = "Homed" And
                       Main_Form.btn_Home_Z.State = "Homed" Then
                        Main_Form.btn_Home.State = "Homed"
                        Main_Form.btn_Home_All.State = "Homed"
                        Main_Form.btn_Cycle_Start.State = "Cycle Stop"
                        Homing_Status = enum_Home_Status.Not_Homing
                        Homing_Type = enum_Home_Type.None
                    End If

                End If

        End Select

        Return Nothing
    End Function

    Public Function Goto_Buttons(ByRef Ctrl As Chip_Button, Evnt As enum_Event_Type) As Object
        Static Dim Active_Button As Chip_Button

        Select Case Evnt

            Case enum_Event_Type.Initialize

            Case enum_Event_Type.Cycle_Stop
                If Not IsNothing(Active_Button) Then
                    Active_Button.State = "Up"
                    Active_Button = Nothing
                    Return Nothing
                End If

            Case enum_Event_Type.Left_Mouse_Button_Down

                If IsNothing(Active_Button) Then
                    If Main_Form.btn_Cycle_Start.State = "In Cycle" Then
                        Show_Error("Cannot do this function while in cycle.")
                        Return Nothing
                    End If
                Else
                    If Active_Button.State <> "Up" Then
                        Cycle_Stop()
                        Return Nothing
                    End If
                End If

                Active_Button = Ctrl
                Ctrl.State = "Down"

                If KFlop.GCode_Program_Executing Then Return Nothing

                If (Main_Form.btn_Enable_All.State <> "Enabled") Or
                   (Main_Form.btn_Enable_X.State <> "Enabled") Or
                   (Main_Form.btn_Enable_Y.State <> "Enabled") Or
                   (Main_Form.btn_Enable_Z.State <> "Enabled") Then
                    Flash_Message("Axis not enabled.")
                    Return Nothing
                End If

                Select Case Ctrl.Name

                    Case "btn_Go_To_Home"
                        Ctrl.State = "Moving"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Go to Home", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)

                    Case "btn_Go_To_Park"
                        Ctrl.State = "Moving"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Go to Park", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)

                    Case "btn_Go_To_Tool_Seat"
                        If Main_Form.txt_Tool_Measurement_Type.Text = "None" Then
                            Show_Error("G Code program not loaded, or tool measure type not set.")
                            Ctrl.State = "Up"
                            Return False
                        End If
                        Ctrl.State = "Moving"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Go to Tool Seat", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)

                    Case "btn_Go_To_Tool_Measure"
                        If Main_Form.txt_Tool_Measurement_Type.Text = "None" Then
                            Show_Error("G Code program not loaded, or tool measure type not set.")
                            Ctrl.State = "Up"
                            Return False
                        End If
                        Ctrl.State = "Moving"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Go to Tool Measure", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)

                    Case "btn_Go_To_Offset"
                        If Main_Form.txt_Tool_Measurement_Type.Text = "None" Then
                            Show_Error("G Code program not loaded, or tool measure type not set.")
                            Ctrl.State = "Up"
                            Return False
                        End If

                        Dim X_Offset As Double = CDbl(Main_Form.txt_X_Work_Offset.Text)
                        Dim Y_Offset As Double = CDbl(Main_Form.txt_Y_Work_Offset.Text)

                        Ctrl.State = "Moving"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Main_Form.ctl_C_Interface.Set_Parameter_Position("Go to Offset", X_Offset, Y_Offset, Display.Maximum_Z, Display.Maximum_Z)
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Go to Offset", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe, Chip_C_Interface.enum_Commands.Go_To)

                End Select

            Case enum_Event_Type.Left_Mouse_Button_Up

            Case enum_Event_Type.Update

                Select Case KFlop.C_Program_Status
                    Case Chip_C_Interface.enum_Status.Idle

                    Case Chip_C_Interface.enum_Status.Move_Status + Chip_C_Interface.enum_Moving_To_Position_Status_Offsets.Move_Done
                        If IsNothing(Active_Button) Then Return Nothing
                        Main_Form.btn_Cycle_Start.State = "Cycle Stop"
                        Active_Button.State = "Up"
                        Main_Form.ctl_C_Interface.Kill_C_Program(Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)
                End Select

        End Select

        Return Nothing

    End Function

    Public Function Square_Buttons(ByRef Ctrl As Chip_Button, Evnt As enum_Event_Type) As Object
        Dim X1, Y1, X2, Y2, Adjust, Y_Adjust_Pos As Single

        Select Case Evnt

            Case enum_Event_Type.Initialize

            Case enum_Event_Type.Left_Mouse_Button_Down

                If Main_Form.btn_Square_X.State = "Squaring" Then
                    Cycle_Stop()
                    Return Nothing
                End If

                Select Case Ctrl.Name

                    Case "btn_Set_X1_Point"
                        X1 = Get_Number(Main_Form.txt_Surface_X.Text)
                        Main_Form.txt_X_1.Text = FormatNumber(X1, 4)
                        Calc_Centers()
                        Ctrl.State = "Down"

                    Case "btn_Set_X2_Point"
                        X2 = Get_Number(Main_Form.txt_Surface_X.Text)
                        Main_Form.txt_X_2.Text = FormatNumber(X2, 4)
                        Calc_Centers()
                        Ctrl.State = "Down"

                    Case "btn_Set_Y1_Point"
                        Y1 = Get_Number(Main_Form.txt_Surface_Y.Text)
                        Main_Form.txt_Y_1.Text = FormatNumber(Y1, 4)
                        Calc_Centers()
                        Ctrl.State = "Down"

                    Case "btn_Set_Y2_Point"
                        Y2 = Get_Number(Main_Form.txt_Surface_Y.Text)
                        Main_Form.txt_Y_2.Text = FormatNumber(Y2, 4)
                        Calc_Centers()
                        Ctrl.State = "Down"

                    Case "btn_Square_X"
                        Message_Box.ShowDialog("This function will move the Y Master. Continue?", "Warning", MessageBoxButtons.YesNo)
                        If Message_Box.DialogResult <> DialogResult.Yes Then
                            Return Nothing
                        End If

                        Adjust = Get_Number(Main_Form.txt_Square_Adjust.Text)

                        If Math.Abs(Adjust) > 0.5 Then
                            Flash_Message("Squaring distance > 0.5")
                        Else
                            Ctrl.State = "Squaring"
                            Y_Adjust_Pos = Get_Number(Main_Form.txt_Square_Adjust_Pos.Text)
                            Kflop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.Y_Params + Chip_C_Interface.enum_User_Data_Offsets.Dest_1, Y_Adjust_Pos)
                            Kflop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.Y_Params + Chip_C_Interface.enum_User_Data_Offsets.Feed_1, 1)
                            Main_Form.ctl_C_Interface.Run_C_Program(Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe, Chip_C_Interface.enum_Commands.Move_Y_Master)
                        End If

                End Select

            Case enum_Event_Type.Left_Mouse_Button_Up
                Ctrl.State = "Up"

            Case enum_Event_Type.Update

                Select Case Kflop.C_Program_Status
                    Case Chip_C_Interface.enum_Status.Idle

                    Case Chip_C_Interface.enum_Status.Move_Status + Chip_C_Interface.enum_Moving_To_Position_Status_Offsets.Move_Done
                        Main_Form.btn_Square_X.State = "Up"

                End Select

            Case Else
                Nop()

        End Select

        Return Nothing

    End Function

    Private Sub Calc_Centers()
        Dim X1, Y1, X2, Y2, Xd, Yd, Y_Adjust_Pos As Single

        X1 = Get_Number(Main_Form.txt_X_1.Text)
        Y1 = Get_Number(Main_Form.txt_Y_1.Text)

        X2 = Get_Number(Main_Form.txt_X_2.Text)
        Y2 = Get_Number(Main_Form.txt_Y_2.Text)

        Xd = X2 - X1
        Yd = Y2 - Y1

        Main_Form.txt_X_Difference.Text = FormatNumber(Xd, 4)
        Main_Form.txt_Y_Difference.Text = FormatNumber(Yd, 4)

        Main_Form.txt_X_Center.Text = FormatNumber(X1 + ((X2 - X1) / 2), 4)
        Main_Form.txt_Y_Center.Text = FormatNumber(Y1 + ((Y2 - Y1) / 2), 4)

        Y_Adjust_Pos = KFlop.Y_Abs + Yd
        Main_Form.txt_Square_Adjust_Pos.Text = FormatNumber(Y_Adjust_Pos, 4)

    End Sub

    'Private Sub Calc_Square()
    '    Dim X1, Y1, X2, Y2, Xd, Yd, Ratio, Adjust, Y_Adjust_Pos As Single

    '    X1 = Get_Number(Main_Form.txt_X_1.Text)
    '    Y1 = Get_Number(Main_Form.txt_Y_1.Text)

    '    X2 = Get_Number(Main_Form.txt_X_2.Text)
    '    Y2 = Get_Number(Main_Form.txt_Y_2.Text)

    '    Xd = X2 - X1
    '    Yd = Y2 - Y1
    '    Ratio = Yd / Xd

    '    Adjust = Xd * Ratio
    '    Y_Adjust_Pos = KFlop.Y_Abs + Adjust

    '    Main_Form.txt_Square_Ratio.Text = FormatNumber(Ratio, 4)
    '    Main_Form.txt_Square_Adjust.Text = FormatNumber(Adjust, 4)
    '    Main_Form.txt_Square_Adjust_Pos.Text = FormatNumber(Y_Adjust_Pos, 4)

    'End Sub


    Public Function Soft_Limit_Fault() As Boolean
        Dim X_Pos As Double, X_Neg As Double, X_Slave_Pos As Double, X_Slave_Neg As Double,
            Y_Pos As Double, Y_Neg As Single, Y_Slave_Pos As Double, Y_Slave_Neg As Double,
            Z_Pos As Double

        Kflop.Get_Soft_Limits(X_Pos, X_Neg, X_Slave_Pos, X_Slave_Neg, Y_Pos, Y_Neg, Y_Slave_Pos, Y_Slave_Neg, Z_Pos, False)

        If Kflop.X_Abs > X_Pos Or Kflop.X_Abs < X_Neg Then Return True
        If Kflop.Y_Abs > Y_Pos Or Kflop.Y_Abs < Y_Neg Then Return True
        If Kflop.Z_Abs > Z_Pos Then Return True

        Return False

    End Function


    Public Function Tool_Setter_Buttons(ByRef Ctrl As Chip_Button, Evnt As enum_Event_Type) As Object
        Static Dim Active_Button As Chip_Button 'Static variable does keeps value between calls

        Select Case Evnt

            Case enum_Event_Type.Initialize

            Case enum_Event_Type.Cycle_Stop
                If Not IsNothing(Active_Button) Then
                    Active_Button.State = "Up"
                    Active_Button = Nothing
                    Return Nothing
                End If

            Case enum_Event_Type.Left_Mouse_Button_Down

                If IsNothing(Active_Button) Then
                    If Main_Form.btn_Cycle_Start.State = "In Cycle" Then
                        Show_Error("Cannot do this function while in cycle.")
                        Return Nothing
                    End If
                Else
                    If Active_Button.State <> "Up" Then
                        Cycle_Stop()
                        Return Nothing
                    End If
                End If


                Active_Button = Ctrl
                Ctrl.State = "Down"

                If (Main_Form.ctl_Fixture_Offsets.Get_X_Offset <> 0) Or
                   (Main_Form.ctl_Fixture_Offsets.Get_Y_Offset <> 0) Or
                   (Main_Form.ctl_Fixture_Offsets.Get_Z_Offset <> 0) Then
                    Dim dlg As New dlg_Message_Box
                    dlg.ShowDialog("Reset Offset Adjustments?", "Warning", MessageBoxButtons.YesNo)
                    If dlg.DialogResult = DialogResult.Yes Then
                        Main_Form.ctl_Fixture_Offsets.Reset_Offset_Adjustments()
                    End If

                End If

                Select Case Ctrl.Name

                    Case "btn_Measure_with_Fixed_Toolsetter"
                        If Soft_Limit_Fault() Then
                            Flash_Message("Soft limit fault.")
                        Else
                            Main_Form.btn_Cycle_Start.State = "In Cycle"
                            Main_Form.ctl_Fixture_Offsets.rad_Fixed_Toolsetter.Checked = True
                            Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Tool Setter Fixed", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)
                        End If

                    Case "btn_Measure_with_Movable_Toolsetter"
                        Dim dlg As New dlg_Message_Box
                        dlg.ShowDialog("Put movable toolsetter on top of material.", "Warning", MessageBoxButtons.OKCancel)
                        If dlg.DialogResult = DialogResult.Cancel Then
                            Return False
                        End If
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Main_Form.ctl_Fixture_Offsets.rad_Movable_Toolsetter.Checked = True
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Tool Setter Movable", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)

                    Case "btn_Measure_Tool_Current_Z"
                        If Soft_Limit_Fault() Then
                            Flash_Message("Soft limit fault.")
                        Else
                            Message_Box.ShowDialog("If tool tip is on surface to be measured then continue, else cancel.", "", MessageBoxButtons.OKCancel)
                            If Message_Box.DialogResult = DialogResult.OK Then
                                Main_Form.ctl_Fixture_Offsets.rad_Surface.Checked = True
                                Main_Form.ctl_Fixture_Offsets.Set_Tool_Offset(Kflop.Z_Abs, "Surface")
                            End If
                        End If

                    Case "btn_Measure_Tool"
                        If Main_Form.txt_Tool_Measurement_Type.Text = "None" Then
                            Show_Error("G Code program not loaded, or tool measure type not set.")
                            Return False
                        End If

                        If Soft_Limit_Fault() Then
                            Flash_Message("Soft limit fault.")
                        Else
                            Select Case Main_Form.txt_Tool_Measurement_Type.Text
                                Case "None"
                                    Flash_Message("Tool measurement type not selected.")
                                Case "Surface"
                                    Message_Box.ShowDialog("If tool tip is on surface to be measured then continue, else cancel.", "", MessageBoxButtons.OKCancel)
                                    If Message_Box.DialogResult = DialogResult.OK Then
                                        Main_Form.ctl_Fixture_Offsets.Set_Tool_Offset(Kflop.Z_Abs, "Surface")
                                    End If
                                Case "Fixed"
                                    Main_Form.btn_Cycle_Start.State = "In Cycle"
                                    Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Tool Setter Fixed", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)
                                Case "Movable"
                                    Dim dlg As New dlg_Message_Box
                                    dlg.ShowDialog("Put movable toolsetter on top of material.", "Warning", MessageBoxButtons.OKCancel)
                                    If dlg.DialogResult = DialogResult.Cancel Then
                                        Return False
                                    End If
                                    Main_Form.btn_Cycle_Start.State = "In Cycle"
                                    Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Tool Setter Movable", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)
                            End Select
                        End If

                    Case "btn_Tool_Setter_Measure_Bottom"
                        If Main_Form.rad_Fixed_Toolsetter.Checked Then
                            Main_Form.txt_Fixed_Tool_Setter_Bottom.Text = FormatNumber(KFlop.Z_Abs, 4)
                            Main_Form.txt_Fixed_Tool_Setter_Top.Text = 0
                            Main_Form.txt_Fixed_Tool_Setter_Measured_Height.Text = 0
                        End If

                        If Main_Form.rad_Movable_Toolsetter.Checked Then
                            Main_Form.txt_Movable_Tool_Setter_Bottom.Text = FormatNumber(Kflop.Z_Abs, 4)
                            Main_Form.txt_Movable_Tool_Setter_Top.Text = 0
                            Main_Form.txt_Movable_Tool_Setter_Measured_Height.Text = 0
                        End If

                    Case "btn_Tool_Setter_Measure_Top"
                        If Main_Form.rad_Fixed_Toolsetter.Checked Then
                            Main_Form.btn_Cycle_Start.State = "In Cycle"
                            Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Tool Setter Fixed", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)
                        End If

                        If Main_Form.rad_Movable_Toolsetter.Checked Then
                            Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Tool Setter Movable", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)
                        End If

                    Case "btn_Fixed_Tool_Setter_Set_Height"
                        Dim Pos As String
                        Ctrl.State = "Up"
                        Pos = Get_User_Input(Main_Form.txt_Fixed_Tool_Setter_Height.Text, Main_Form.txt_Fixed_Tool_Setter_Measured_Height.Text,
                                             "", "", "", "This button also sets the Toolsetter height on the Fixture Offsets Tab.")
                        If Pos = "" Then Pos = 0
                        Main_Form.txt_Fixed_Tool_Setter_Height.Text = Pos
                        Main_Form.ctl_Fixture_Offsets.Set_Fixed_Tool_Setter_Height(Pos)

                    Case "btn_Movable_Tool_Setter_Set_Height"
                        Dim Pos As String
                        Ctrl.State = "Up"
                        Pos = Get_User_Input(Main_Form.txt_Movable_Tool_Setter_Height.Text, Main_Form.txt_Movable_Tool_Setter_Measured_Height.Text,
                                             "", "", "", "This button also sets the Toolsetter height on the Fixture Offsets Tab.")
                        Main_Form.txt_Movable_Tool_Setter_Height.Text = Pos
                        If Pos = "" Then Pos = 0
                        Main_Form.ctl_Fixture_Offsets.Set_Movable_Tool_Setter_Height(Pos)


                    Case "btn_Tool_Setter_Measure_Fixed_Surface"
                        Main_Form.txt_Tool_Setter_Fixture_Surface.Text = 0
                        Main_Form.txt_Tool_Setter_Mount_Z_Delta.Text = 0
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Tool Setter Fixed", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)

                    Case "btn_Tool_Setter_Measure_Mount_Surface"
                        Main_Form.txt_Tool_Setter_Mount_Z_Delta.Text = 0
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Tool Setter Fixed", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)


                    Case "btn_Tool_Setter_Measure_Material_Bottom"
                        Main_Form.txt_Tool_Setter_Material_Top.Text = 0
                        Main_Form.txt_Tool_Setter_Material_Thickness.Text = 0
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Tool Setter Movable", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)

                    Case "btn_Tool_Setter_Measure_Material_Top"
                        Main_Form.txt_Tool_Setter_Material_Thickness.Text = 0
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Tool Setter Movable", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)

                End Select

            Case enum_Event_Type.Left_Mouse_Button_Up

                Select Case Ctrl.Name

                    Case "btn_Measure_Tool_Current_Z"
                        Ctrl.State = "Up"

                    Case "btn_Tool_Setter_Measure_Bottom"
                        Ctrl.State = "Up"

                    Case "btn_Tool_Setter_Set_Height"
                        Ctrl.State = "Up"

                End Select

            Case enum_Event_Type.Update

                If Kflop.Controller.ThreadExecuting(Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe) Then
                    Dim Pos As Single = Kflop.Get_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.Pos_1)
                    Dim Diam As Single = Get_Text_Number(Main_Form.txt_Probe_Diameter.Text)

                    Select Case KFlop.C_Program_Status
                        Case Chip_C_Interface.enum_Status.Idle

                        Case Chip_C_Interface.enum_Status.Tool_Setter_Fixed_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Overtravel,
                             Chip_C_Interface.enum_Status.Tool_Setter_Movable_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Overtravel
                            Show_Error("Toolsetter measurement Failed, overtravel")
                            Main_Form.btn_Cycle_Start.State = "Cycle Stop"
                            Active_Button.State = "Up"
                            Active_Button = Nothing

                        Case Chip_C_Interface.enum_Status.Tool_Setter_Fixed_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_In_Contact,
                             Chip_C_Interface.enum_Status.Tool_Setter_Movable_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_In_Contact
                            Show_Error("Toolsetter measurement, already in contact.")
                            Main_Form.btn_Cycle_Start.State = "Cycle Stop"
                            Active_Button.State = "Up"
                            Active_Button = Nothing

                        Case Chip_C_Interface.enum_Status.Tool_Setter_Fixed_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Extend,
                             Chip_C_Interface.enum_Status.Tool_Setter_Movable_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Extend
                            If Not IsNothing(Active_Button) Then
                                Active_Button.State = "Measuring"
                            End If

                        Case Chip_C_Interface.enum_Status.Tool_Setter_Fixed_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Done,
                             Chip_C_Interface.enum_Status.Tool_Setter_Movable_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Done
                            If IsNothing(Active_Button) Then Return Nothing
                            Main_Form.btn_Cycle_Start.State = "Cycle Stop"
                            Active_Button.State = "Up"

                            Select Case Active_Button.Name

                                Case "btn_Measure_Tool"
                                    If Main_Form.txt_Tool_Measurement_Type.Text = "Fixed" Then
                                        Main_Form.ctl_Fixture_Offsets.Set_Tool_Offset(Pos, "Fixed")
                                    End If
                                    If Main_Form.txt_Tool_Measurement_Type.Text = "Movable" Then
                                        Main_Form.ctl_Fixture_Offsets.Set_Tool_Offset(Pos, "Movable")
                                    End If

                                    If Main_Form.chk_Auto_Go_To_Offset.Checked Then
                                        Goto_Buttons(Main_Form.btn_Go_To_Offset, enum_Event_Type.Left_Mouse_Button_Down)
                                    End If

                                Case "btn_Measure_with_Movable_Toolsetter"
                                    Main_Form.ctl_Fixture_Offsets.Set_Tool_Offset(Pos, "Movable")

                                Case "btn_Measure_with_Fixed_Toolsetter"
                                    Main_Form.ctl_Fixture_Offsets.Set_Tool_Offset(Pos, "Fixed")


                                Case "btn_Tool_Setter_Measure_Top"
                                    Dim Top, Bottom As Single
                                    If Main_Form.rad_Fixed_Toolsetter.Checked Then
                                        Main_Form.txt_Fixed_Tool_Setter_Top.Text = FormatNumber(Pos, 4)
                                        Top = Get_Text_Number(Main_Form.txt_Fixed_Tool_Setter_Top.Text)
                                        Bottom = Get_Text_Number(Main_Form.txt_Fixed_Tool_Setter_Bottom.Text)
                                        Main_Form.txt_Fixed_Tool_Setter_Measured_Height.Text = FormatNumber(Top - Bottom, 4)
                                    End If
                                    If Main_Form.rad_Movable_Toolsetter.Checked Then
                                        Main_Form.txt_Movable_Tool_Setter_Top.Text = FormatNumber(Pos, 4)
                                        Top = Get_Text_Number(Main_Form.txt_Movable_Tool_Setter_Top.Text)
                                        Bottom = Get_Text_Number(Main_Form.txt_Movable_Tool_Setter_Bottom.Text)
                                        Main_Form.txt_Movable_Tool_Setter_Measured_Height.Text = FormatNumber(Top - Bottom, 4)
                                    End If

                                Case "btn_Tool_Setter_Measure_Fixed_Surface"
                                    Main_Form.txt_Tool_Setter_Fixed_Surface.Text = FormatNumber(Pos, 4)

                                Case "btn_Tool_Setter_Measure_Mount_Surface"
                                    Dim Fixed As Single
                                    Main_Form.txt_Tool_Setter_Fixture_Surface.Text = FormatNumber(Pos, 4)
                                    Fixed = Get_Text_Number(Main_Form.txt_Tool_Setter_Fixed_Surface.Text)
                                    Main_Form.txt_Tool_Setter_Mount_Z_Delta.Text = FormatNumber(Pos - Fixed, 4)

                                Case "btn_Tool_Setter_Measure_Material_Bottom"
                                    Main_Form.txt_Tool_Setter_Material_Bottom.Text = FormatNumber(Pos, 4)

                                Case "btn_Tool_Setter_Measure_Material_Top"
                                    Dim Material_Bottom As Single
                                    Main_Form.txt_Tool_Setter_Material_Top.Text = FormatNumber(Pos, 4)
                                    Material_Bottom = Get_Text_Number(Main_Form.txt_Tool_Setter_Material_Bottom.Text)
                                    Main_Form.txt_Tool_Setter_Material_Thickness.Text = FormatNumber(Pos - Material_Bottom, 4)
                            End Select

                            Active_Button = Nothing

                    End Select

                End If

        End Select

        Return Nothing
    End Function


    Public Function Probe_Buttons(ByRef Ctrl As Chip_Button, Evnt As enum_Event_Type) As Object


        Select Case Evnt
            Case enum_Event_Type.Initialize
                Main_Form.btn_Probe_Enable.State = "Off"

            Case enum_Event_Type.Cycle_Stop
                Main_Form.btn_Probe_X_Plus.State = "Off"
                Main_Form.btn_Probe_X_Minus.State = "Off"
                Main_Form.btn_Probe_Y_Plus.State = "Off"
                Main_Form.btn_Probe_Y_Minus.State = "Off"
                Main_Form.btn_Probe_Z_Minus.State = "Off"
                Return Nothing

            Case enum_Event_Type.Left_Mouse_Button_Down
                If (Main_Form.btn_Probe_X_Plus.State <> "Off") Or
                   (Main_Form.btn_Probe_X_Minus.State <> "Off") Or
                   (Main_Form.btn_Probe_Y_Plus.State <> "Off") Or
                   (Main_Form.btn_Probe_Y_Minus.State <> "Off") Or
                   (Main_Form.btn_Probe_Z_Minus.State <> "Off") Then
                    Cycle_Stop()
                    Return Nothing
                End If

                If (Ctrl.Name <> "btn_Probe_Enable") And (Ctrl.Name <> "btn_Probe_Saftey_Enable") Then
                    If Main_Form.btn_Probe_Enable.State = "Off" Then
                        Flash_Message("Probe not enabled.")
                        Return Nothing
                    End If

                    Dim Diam As Single = Get_Text_Number(Main_Form.txt_Probe_Diameter.Text)
                    If Diam > 0.25 Then
                        Show_Error("Probe tip diameter too high, must be 0.250 maximum.")
                        Return Nothing
                    End If
                End If

                Select Case Ctrl.Name

                    Case "btn_Probe_Enable"

                        If Main_Form.btn_Cycle_Start.State = "In Cycle" Then
                            Main_Form.Post_Error("Cannot use probe while in cycle.")
                            Flash_Message("Cannot use probe while in cycle.")
                            Return Nothing
                        End If

                        If Main_Form.btn_Probe_Enable.State = "On" Then
                            Main_Form.btn_Probe_Enable.State = "Off"
                            Main_Form.btn_Probe_X_Plus.State = "Off"
                            Main_Form.btn_Probe_X_Minus.State = "Off"
                            Main_Form.btn_Probe_Y_Plus.State = "Off"
                            Main_Form.btn_Probe_Y_Minus.State = "Off"
                            Main_Form.btn_Probe_Z_Minus.State = "Off"
                        Else
                            Main_Form.btn_Probe_Enable.State = "On"
                            Main_Form.btn_Edge_Finder_Enable.State = "Off"
                            Main_Form.btn_Edge_Finder_Start_Spindle.Visible = False
                            Main_Form.ctl_C_Interface.Set_Spindle_Speed(0)
                            Main_Form.btn_Edge_Finder_Start_Spindle.State = "Off"
                            Main_Form.led_Spindle_Enable.Set_Boolean_Output(False, "Off")
                        End If

                    Case "btn_Probe_Saftey_Enable"

                        If Main_Form.btn_Probe_Saftey_Enable.State = "On" Then
                            Main_Form.btn_Probe_Saftey_Enable.State = "Off"
                            Main_Form.ctl_C_Interface.Kill_C_Program(Class_KFLOP.enum_KFlop_Threads.Probe_Toolsetter_Saftety)
                        Else
                            Main_Form.btn_Probe_Saftey_Enable.State = "On"
                            Main_Form.ctl_C_Interface.Run_C_Program(Class_KFLOP.enum_KFlop_Threads.Probe_Toolsetter_Saftety)
                        End If

                    Case "btn_Probe_X_Plus"
                        If Main_Form.btn_Probe_Enable.State = "On" Then
                            Main_Form.btn_Cycle_Start.State = "In Cycle"
                            Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Probe X Plus", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)
                        End If

                    Case "btn_Probe_X_Minus"
                        If Main_Form.btn_Probe_Enable.State = "On" Then
                            Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Probe X Minus", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)
                            Main_Form.btn_Cycle_Start.State = "In Cycle"
                        End If

                    Case "btn_Probe_Y_Plus"
                        If Main_Form.btn_Probe_Enable.State = "On" Then
                            Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Probe Y Plus", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)
                            Main_Form.btn_Cycle_Start.State = "In Cycle"
                        End If

                    Case "btn_Probe_Y_Minus"
                        If Main_Form.btn_Probe_Enable.State = "On" Then
                            Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Probe Y Minus", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)
                            Main_Form.btn_Cycle_Start.State = "In Cycle"
                        End If

                    Case "btn_Probe_Z_Minus"
                        If Main_Form.btn_Probe_Enable.State = "On" Then
                            Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Probe Z Minus", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)
                            Main_Form.btn_Cycle_Start.State = "In Cycle"
                        End If

                End Select

            Case enum_Event_Type.Left_Mouse_Button_Up

            Case enum_Event_Type.Update

                If Kflop.Controller.ThreadExecuting(Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe) Then
                    Dim Pos As Single = Kflop.Get_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.Pos_1)
                    Dim Diam As Single = Get_Text_Number(Main_Form.txt_Probe_Diameter.Text)

                    Select Case Kflop.C_Program_Status
                        Case Chip_C_Interface.enum_Status.Idle

                        Case Chip_C_Interface.enum_Status.Probe_X_Plus_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Overtravel,
                             Chip_C_Interface.enum_Status.Probe_X_Minus_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Overtravel,
                             Chip_C_Interface.enum_Status.Probe_Y_Plus_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Overtravel,
                             Chip_C_Interface.enum_Status.Probe_Y_Minus_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Overtravel,
                             Chip_C_Interface.enum_Status.Probe_Z_Minus_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Overtravel
                            Main_Form.btn_Probe_X_Plus.State = "Off"
                            Main_Form.btn_Probe_X_Minus.State = "Off"
                            Main_Form.btn_Probe_Y_Plus.State = "Off"
                            Main_Form.btn_Probe_Y_Minus.State = "Off"
                            Main_Form.btn_Probe_Z_Minus.State = "Off"
                            Main_Form.btn_Cycle_Start.State = "Cycle Stop"
                            Main_Form.Post_Error("Probe Failed, overtravel")
                            Flash_Message("Probe Failed, overtravel")

                        Case Chip_C_Interface.enum_Status.Probe_X_Plus_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_In_Contact,
                             Chip_C_Interface.enum_Status.Probe_X_Minus_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_In_Contact,
                             Chip_C_Interface.enum_Status.Probe_Y_Plus_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_In_Contact,
                             Chip_C_Interface.enum_Status.Probe_Y_Minus_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_In_Contact,
                             Chip_C_Interface.enum_Status.Probe_Z_Minus_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_In_Contact
                            Main_Form.btn_Probe_X_Plus.State = "Off"
                            Main_Form.btn_Probe_X_Minus.State = "Off"
                            Main_Form.btn_Probe_Y_Plus.State = "Off"
                            Main_Form.btn_Probe_Y_Minus.State = "Off"
                            Main_Form.btn_Probe_Z_Minus.State = "Off"
                            Main_Form.btn_Cycle_Start.State = "Cycle Stop"
                            Main_Form.Post_Error("Probe already in contact.")
                            Flash_Message("Probe Failed, already in contact.")

                        Case Chip_C_Interface.enum_Status.Probe_X_Plus_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Extend
                            Main_Form.btn_Probe_X_Plus.State = "Probing"

                        Case Chip_C_Interface.enum_Status.Probe_X_Minus_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Extend
                            Main_Form.btn_Probe_X_Minus.State = "Probing"

                        Case Chip_C_Interface.enum_Status.Probe_X_Plus_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Done
                            Main_Form.btn_Cycle_Start.State = "Cycle Stop"
                            Main_Form.btn_Probe_X_Plus.State = "Off"
                            Main_Form.txt_Surface_X.Text = FormatNumber(Pos + Diam / 2, 4)

                        Case Chip_C_Interface.enum_Status.Probe_X_Minus_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Done
                            Main_Form.btn_Cycle_Start.State = "Cycle Stop"
                            Main_Form.btn_Probe_X_Minus.State = "Off"
                            Main_Form.txt_Surface_X.Text = FormatNumber(Pos - Diam / 2, 4)

                        Case Chip_C_Interface.enum_Status.Probe_Y_Plus_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Extend
                            Main_Form.btn_Probe_Y_Plus.State = "Probing"

                        Case Chip_C_Interface.enum_Status.Probe_Y_Minus_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Extend
                            Main_Form.btn_Probe_Y_Minus.State = "Probing"

                        Case Chip_C_Interface.enum_Status.Probe_Y_Plus_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Done
                            Main_Form.btn_Cycle_Start.State = "Cycle Stop"
                            Main_Form.btn_Probe_Y_Plus.State = "Off"
                            Main_Form.txt_Surface_Y.Text = FormatNumber(Pos + Diam / 2, 4)

                        Case Chip_C_Interface.enum_Status.Probe_Y_Minus_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Done
                            Main_Form.btn_Cycle_Start.State = "Cycle Stop"
                            Main_Form.btn_Probe_Y_Minus.State = "Off"
                            Main_Form.txt_Surface_Y.Text = FormatNumber(Pos - Diam / 2, 4)

                        Case Chip_C_Interface.enum_Status.Probe_Z_Minus_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Extend
                            Main_Form.btn_Probe_Z_Minus.State = "Probing"

                        Case Chip_C_Interface.enum_Status.Probe_Z_Minus_Status + Chip_C_Interface.enum_Probe_Status_Offsets.Probe_Done
                            Main_Form.btn_Cycle_Start.State = "Cycle Stop"
                            Main_Form.btn_Probe_Z_Minus.State = "Off"
                            Main_Form.txt_Surface_Z.Text = FormatNumber(Pos + Diam / 2, 4)

                    End Select

                End If

        End Select

        Return Nothing
    End Function

    Public Function Edge_Finder_Buttons(ByRef Ctrl As Chip_Button, Evnt As enum_Event_Type) As Object

        Select Case Evnt

            Case enum_Event_Type.Initialize
                Main_Form.btn_Edge_Finder_Enable.State = "Off"
                Main_Form.btn_Edge_Finder_Start_Spindle.State = "Off"
                Main_Form.btn_Edge_Finder_Start_Spindle.Visible = False

            Case enum_Event_Type.Left_Mouse_Button_Down
                Dim Diam As Single = Get_Text_Number(Main_Form.txt_Edge_Finder_Diameter.Text)

                If Diam > 0.5 Then
                    Show_Error("Edge Finder diameter too high, must be 0.500 maximum.")
                    Return Nothing
                End If

                Select Case Ctrl.Name
                    Case "btn_Edge_Finder_Enable"

                        If Main_Form.btn_Cycle_Start.State = "In Cycle" Then
                            Show_Error("Cannot use Edge Finder while in cycle.")
                            Return Nothing
                        End If

                        If Main_Form.btn_Edge_Finder_Enable.State = "On" Then
                            Main_Form.btn_Edge_Finder_Enable.State = "Off"
                            Main_Form.btn_Edge_Finder_Start_Spindle.Visible = False
                        Else
                            Main_Form.btn_Probe_Enable.State = "Off"
                            Main_Form.btn_Edge_Finder_Enable.State = "On"
                            Main_Form.btn_Edge_Finder_Start_Spindle.Visible = True
                        End If


                    Case "btn_Edge_Finder_Start_Spindle"

                        If Main_Form.led_Probe.State = "On" Then
                            Show_Error("Cannot cycle start with probe mounted.")
                            Return Nothing
                        End If

                        If Get_Text_Number(Main_Form.txt_Edge_Finder_RPM.Text) > 400 Then
                            Show_Error("RPM too high, must be 400 maximum.")
                            Return Nothing
                        End If

                        If Main_Form.btn_Edge_Finder_Start_Spindle.State = "Off" Then
                            Main_Form.ctl_C_Interface.Set_Spindle_Speed(Get_Text_Number(Main_Form.txt_Edge_Finder_RPM.Text))
                            Main_Form.ctl_C_Interface.Set_Spindle_Override(100)

                            Main_Form.btn_Edge_Finder_Start_Spindle.State = "On"
                            Main_Form.led_Spindle_Enable.Set_Boolean_Output(True, "On")
                        Else
                            Main_Form.ctl_C_Interface.Set_Spindle_Speed(0)
                            Main_Form.btn_Edge_Finder_Start_Spindle.State = "Off"
                            Main_Form.led_Spindle_Enable.Set_Boolean_Output(False, "Off")
                        End If

                    Case "btn_Edge_Finder_X_Plus"
                        Main_Form.txt_Surface_X.Text = FormatNumber(KFlop.X_Abs + (Diam / 2), 4)
                        Main_Form.btn_Probe_X_Plus.State = "Set"

                    Case "btn_Edge_Finder_X_Minus"
                        Main_Form.txt_Surface_X.Text = FormatNumber(KFlop.X_Abs - (Diam / 2), 4)
                        Main_Form.btn_Probe_X_Minus.State = "Set"

                    Case "btn_Edge_Finder_Y_Plus"
                        Main_Form.txt_Surface_Y.Text = FormatNumber(KFlop.Y_Abs + (Diam / 2), 4)
                        Main_Form.btn_Probe_Y_Plus.State = "Set"

                    Case "btn_Edge_Finder_Y_Minus"
                        Main_Form.txt_Surface_Y.Text = FormatNumber(KFlop.Y_Abs - (Diam / 2), 4)
                        Main_Form.btn_Probe_Y_Minus.State = "Set"

                End Select

            Case enum_Event_Type.Left_Mouse_Button_Up

                Select Case Ctrl.Name
                    Case "btn_Edge_Finder_X_Plus"
                        Main_Form.btn_Probe_X_Plus.State = "Off"
                    Case "btn_Edge_Finder_X_Minus"
                        Main_Form.btn_Probe_X_Minus.State = "Off"
                    Case "btn_Edge_Finder_Y_Plus"
                        Main_Form.btn_Probe_Y_Plus.State = "Off"
                    Case "btn_Edge_Finder_Y_Minus"
                        Main_Form.btn_Probe_Y_Minus.State = "Off"
                End Select

        End Select

        Return Nothing
    End Function

    Public Function Jog_Buttons(ByRef Ctrl As Chip_Button, Evnt As enum_Event_Type) As Object

        If Main_Form.btn_Cycle_Start.State = "In Cycle" Then
            Flash_Message("Cannot do this function while in cycle.")
            Return Nothing
        End If

        Select Case Evnt

            Case enum_Event_Type.Reset
                Kflop.Jog_Feedrate = Main_Form.txt_Jog_Rate_Fast.Number(4)
                Main_Form.txt_Jog_Rate_Selected.Text = Kflop.Jog_Feedrate

            Case enum_Event_Type.Left_Mouse_Button_Down
                If Toolsetter_Tripped() Then
                    Return Nothing
                End If

                If Ctrl.Name = "btn_Soft_Limit_Enable" Then
                    If Main_Form.btn_Soft_Limit_Enable.State = "On" Then
                        Main_Form.btn_Soft_Limit_Enable.State = "Off"
                        Kflop.Enable_Soft_Limits()
                    Else
                        Main_Form.btn_Soft_Limit_Enable.State = "On"
                        Kflop.Disable_Soft_Limits()
                    End If
                    Return Nothing
                End If

                Ctrl.State = "Down"

                If Main_Form.btn_Enable_All.State <> "Enabled" Then
                    Flash_Message("Axis not enabled.")
                    Return Nothing
                End If

                If (Main_Form.btn_Enable_X.State <> "Enabled" And Ctrl.Name.Contains("Jog_X")) Or
                     (Main_Form.btn_Enable_Y.State <> "Enabled" And Ctrl.Name.Contains("Jog_Y")) Or
                      (Main_Form.btn_Enable_Z.State <> "Enabled" And Ctrl.Name.Contains("Jog_Z")) Then
                    Flash_Message("Axis not enabled.")
                    Return Nothing
                End If

                If Main_Form.btn_Jog_Rate_Big_Step.State = "On" Or Main_Form.btn_Jog_Rate_Little_Step.State = "On" Then

                    Dim Increment As Double = 0
                    Dim N As Single = 0

                    If Main_Form.btn_Jog_Rate_Big_Step.State = "On" Then
                        Increment = Main_Form.txt_Jog_Rate_Big_Step.Number(4)
                    Else
                        Increment = Main_Form.txt_Jog_Rate_Small_Step.Number(4)
                    End If

                    Dim Step_Feedrate = Get_Text_Number(Main_Form.txt_Jog_Rate_Fast.Text)

                    Select Case Ctrl.Name
                        Case "btn_Jog_X_Plus"
                            KFlop.X_Move_Relative(Increment, Step_Feedrate)
                        Case "btn_Jog_X_Minus"
                            KFlop.X_Move_Relative(-Increment, Step_Feedrate)
                        Case "btn_Jog_Y_Plus"
                            KFlop.Y_Move_Relative(Increment, Step_Feedrate)
                        Case "btn_Jog_Y_Minus"
                            KFlop.Y_Move_Relative(-Increment, Step_Feedrate)
                        Case "btn_Jog_Z_Plus"
                            KFlop.Z_Move_Relative(Increment, Step_Feedrate)
                        Case "btn_Jog_Z_Minus"
                            KFlop.Z_Move_Relative(-Increment, Step_Feedrate)
                    End Select

                Else 'Continuous jog

                    Select Case Ctrl.Name
                        Case "btn_Jog_X_Plus"
                            KFlop.X_Jog(KFlop.Jog_Feedrate)
                        Case "btn_Jog_X_Minus"
                            KFlop.X_Jog(-KFlop.Jog_Feedrate)
                        Case "btn_Jog_Y_Plus"
                            KFlop.Y_Jog(KFlop.Jog_Feedrate)
                        Case "btn_Jog_Y_Minus"
                            KFlop.Y_Jog(-KFlop.Jog_Feedrate)
                        Case "btn_Jog_Z_Plus"
                            KFlop.Z_Jog(KFlop.Jog_Feedrate)
                        Case "btn_Jog_Z_Minus"
                            KFlop.Z_Jog(-KFlop.Jog_Feedrate)
                    End Select

                End If

            Case enum_Event_Type.Left_Mouse_Button_Up
                If Ctrl.Name = "btn_Soft_Limit_Enable" Then Return Nothing

                Ctrl.State = "Up"
                If Main_Form.btn_Jog_Rate_Big_Step.State = "Off" And
                   Main_Form.btn_Jog_Rate_Little_Step.State = "Off" Then

                    'When using pendant sometimes commands are missed, so if any jog button is detected up stop all axes
                    Kflop.X_Jog(0)
                    Kflop.Y_Jog(0)
                    Kflop.Z_Jog(0)

                    If Main_Form.btn_Soft_Limit_Enable.State = "On" Then
                        Main_Form.btn_Soft_Limit_Enable.State = "Off"
                        Kflop.Enable_Soft_Limits()
                    End If

                End If

        End Select

        Return Nothing

    End Function

    Public Function Jog_Select_Buttons(ByRef Ctrl As Chip_Button, Evnt As enum_Event_Type) As Object

        Select Case Evnt

            Case enum_Event_Type.Initialize
                Main_Form.btn_Jog_Rate_Fast.State = "On"
                Main_Form.btn_Jog_Rate_Slow.State = "Off"
                Main_Form.btn_Jog_Rate_Creep.State = "Off"
                Main_Form.btn_Jog_Rate_Big_Step.State = "Off"
                Main_Form.btn_Jog_Rate_Little_Step.State = "Off"

            Case enum_Event_Type.Left_Mouse_Button_Down
                'Turn off all jog rate buttons
                Main_Form.btn_Jog_Rate_Fast.State = "Off"
                Main_Form.btn_Jog_Rate_Slow.State = "Off"
                Main_Form.btn_Jog_Rate_Creep.State = "Off"
                Main_Form.btn_Jog_Rate_Big_Step.State = "Off"
                Main_Form.btn_Jog_Rate_Little_Step.State = "Off"

                'Turn on selected one
                Select Case Ctrl.Name
                    Case "btn_Jog_Rate_Fast"
                        Kflop.Jog_Feedrate = Main_Form.txt_Jog_Rate_Fast.Number(4)
                        Ctrl.State = "On"
                    Case "btn_Jog_Rate_Slow"
                        Kflop.Jog_Feedrate = Main_Form.txt_Jog_Rate_Slow.Number(4)
                        Ctrl.State = "On"
                    Case "btn_Jog_Rate_Creep"
                        Kflop.Jog_Feedrate = Main_Form.txt_Jog_Rate_Creep.Number(4)
                        Ctrl.State = "On"
                    Case "btn_Jog_Rate_Big_Step"
                        Kflop.Jog_Feedrate = Main_Form.txt_Jog_Rate_Big_Step.Number(4)
                        Ctrl.State = "On"
                    Case "btn_Jog_Rate_Little_Step"
                        Kflop.Jog_Feedrate = Main_Form.txt_Jog_Rate_Small_Step.Number(4)
                        Ctrl.State = "On"
                End Select

                Main_Form.txt_Jog_Rate_Selected.Text = Kflop.Jog_Feedrate

        End Select

        Return Nothing
    End Function

    Public Function Jog_Select_Text_Boxes(ByRef Ctrl As Chip_Text_Box, Evnt As enum_Event_Type) As Object

        Select Case Evnt
            Case enum_Event_Type.Left_Mouse_Button_Down
                Dim Pos As String
                Pos = Get_User_Input(Ctrl.Text, Ctrl.Default_Value)
                Ctrl.Text = Pos
                Main_Form.txt_Jog_Rate_Selected.Text = Ctrl.Text
                Kflop.Jog_Feedrate = Get_Text_Number(Ctrl.Text)
        End Select

        Return Nothing
    End Function

    Public Function Tramming_Buttons(ByRef Ctrl As Chip_Button, Evnt As enum_Event_Type) As Object

        Select Case Evnt

            Case enum_Event_Type.Cycle_Stop
                Main_Form.btn_Tram_X_Minus.State = "Off"
                Main_Form.btn_Tram_X_Plus.State = "Off"
                Main_Form.btn_Tram_Y_Minus.State = "Off"
                Main_Form.btn_Tram_Y_Plus.State = "Off"
                Main_Form.btn_Tram_Z_Minus.State = "Off"
                Main_Form.btn_Tram_Z_Plus.State = "Off"
                Return True

            Case enum_Event_Type.Left_Mouse_Button_Down
                If Ctrl.State = "Moving" Then
                    Cycle_Stop()
                    Return Nothing
                End If

                If Main_Form.btn_Cycle_Start.State = "In Cycle" Then
                    Flash_Message("Cannot do this function while in cycle.")
                    Return Nothing
                End If

                If Ctrl.Name = "btn_Tram_Enable" Then
                    If Main_Form.btn_Tram_Enable.State = "On" Then
                        Main_Form.btn_Tram_Enable.State = "Off"
                    Else
                        Main_Form.btn_Tram_Enable.State = "On"
                    End If
                    Return Nothing
                Else
                    If Main_Form.btn_Tram_Enable.State = "Off" Then
                        Show_Error("Tramming not enabled.")
                        Return Nothing
                    End If
                End If

                Ctrl.State = "Down"

                Dim X As Double = Kflop.X_Abs
                Dim Y As Double = Kflop.Y_Abs
                Dim Z_1 As Double = Kflop.Z_Abs
                Dim Z_2 As Double = Kflop.Z_Abs

                Dim Feedrate_XY As Double = Get_Text_Number(Main_Form.txt_Tramming_Feedrate_XY.Text)
                Dim Feedrate_Z As Double = Get_Text_Number(Main_Form.txt_Tramming_Feedrate_Z_Up.Text)

                If Main_Form.chk_Move_Raise_Z.Checked Then
                    Z_1 = Get_Text_Number(Main_Form.txt_Tram_Z_Plus.Text)
                    Z_2 = Get_Text_Number(Main_Form.txt_Tram_Z_Minus.Text)
                End If

                Main_Form.ctl_C_Interface.Set_Feed_Rate("Tram", "X", Get_Text_Number(Main_Form.txt_Tramming_Feedrate_XY.Text))
                Main_Form.ctl_C_Interface.Set_Feed_Rate("Tram", "Y", Get_Text_Number(Main_Form.txt_Tramming_Feedrate_XY.Text))
                Main_Form.ctl_C_Interface.Set_Feed_Rate("Tram", "Z", Get_Text_Number(Main_Form.txt_Tramming_Feedrate_Z_Up.Text),
                                                                     Get_Text_Number(Main_Form.txt_Tramming_Feedrate_Z_Down.Text))

                Select Case Ctrl.Name
                    Case "btn_Tram_X_Plus"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        X = Get_Text_Number(Main_Form.txt_Tram_X_Plus.Text)
                        Main_Form.ctl_C_Interface.Set_Parameter_Position("Tram", X, Y, Z_1, Z_2)
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Tram", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)
                        Ctrl.State = "Moving"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Tram", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)

                    Case "btn_Tram_X_Minus"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        X = Get_Text_Number(Main_Form.txt_Tram_X_Minus.Text)
                        Main_Form.ctl_C_Interface.Set_Parameter_Position("Tram", X, Y, Z_1, Z_2)
                        Ctrl.State = "Moving"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Tram", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)

                    Case "btn_Tram_Y_Plus"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Y = Get_Text_Number(Main_Form.txt_Tram_Y_Plus.Text)
                        Main_Form.ctl_C_Interface.Set_Parameter_Position("Tram", X, Y, Z_1, Z_2)
                        Ctrl.State = "Moving"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Tram", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)

                    Case "btn_Tram_Y_Minus"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Y = Get_Text_Number(Main_Form.txt_Tram_Y_Minus.Text)
                        Main_Form.ctl_C_Interface.Set_Parameter_Position("Tram", X, Y, Z_1, Z_2)
                        Ctrl.State = "Moving"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Tram", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)

                    Case "btn_Tram_Z_Plus"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Z_1 = Get_Text_Number(Main_Form.txt_Tram_Z_Plus.Text)
                        Z_2 = Z_1
                        Main_Form.ctl_C_Interface.Set_Feed_Rate("Tram", "Z", Get_Text_Number(Main_Form.txt_Tramming_Feedrate_Z_Up.Text))
                        Main_Form.ctl_C_Interface.Set_Parameter_Position("Tram", X, Y, Z_1, Z_2)
                        Ctrl.State = "Moving"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Tram", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)

                    Case "btn_Tram_Z_Minus"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Z_1 = Get_Text_Number(Main_Form.txt_Tram_Z_Minus.Text)
                        Z_2 = Z_1
                        Main_Form.ctl_C_Interface.Set_Feed_Rate("Tram", "Z", Get_Text_Number(Main_Form.txt_Tramming_Feedrate_Z_Down.Text))
                        Main_Form.ctl_C_Interface.Set_Parameter_Position("Tram", X, Y, Z_1, Z_2)
                        Ctrl.State = "Moving"
                        Main_Form.btn_Cycle_Start.State = "In Cycle"
                        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Tram", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)

                End Select

            Case enum_Event_Type.Left_Mouse_Button_Up
                'If Ctrl.Name <> "btn_Tram_Enable" Then Ctrl.State = "Up"

            Case enum_Event_Type.Update

                If Kflop.Controller.ThreadExecuting(Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe) Then
                    Select Case Kflop.C_Program_Status
                        Case Chip_C_Interface.enum_Status.Idle

                        Case Chip_C_Interface.enum_Status.Move_Status + Chip_C_Interface.enum_Moving_To_Position_Status_Offsets.Move_Done
                            Main_Form.btn_Cycle_Start.State = "Cycle Stop"

                            Main_Form.btn_Tram_X_Minus.State = "Off"
                            Main_Form.btn_Tram_X_Plus.State = "Off"
                            Main_Form.btn_Tram_Y_Minus.State = "Off"
                            Main_Form.btn_Tram_Y_Plus.State = "Off"

                            Main_Form.btn_Tram_Z_Minus.State = "Off"
                            Main_Form.btn_Tram_Z_Plus.State = "Off"

                    End Select

                End If


        End Select

        Return Nothing
    End Function

    Public Function Tramming_Text_Boxes(ByRef Ctrl As Chip_Text_Box, Evnt As enum_Event_Type) As Object

        Select Case Evnt

            Case enum_Event_Type.Left_Mouse_Button_Down
                Dim Pos As String = ""
                Dim Default_Value As String = ""
                Dim Value As String = ""

                Select Case Ctrl.Name
                    Case "txt_Tram_X_Plus"
                        Value = FormatNumber(Get_Text_Number(Main_Form.txt_Tram_X_Plus.Text), 4)
                        Pos = FormatNumber(Kflop.X_Abs, 4)
                    Case "txt_Tram_X_Minus"
                        Value = FormatNumber(Get_Text_Number(Main_Form.txt_Tram_X_Minus.Text), 4)
                        Pos = FormatNumber(Kflop.X_Abs, 4)
                    Case "txt_Tram_Y_Plus"
                        Value = FormatNumber(Get_Text_Number(Main_Form.txt_Tram_Y_Plus.Text), 4)
                        Pos = FormatNumber(Kflop.Y_Abs, 4)
                    Case "txt_Tram_Y_Minus"
                        Value = FormatNumber(Get_Text_Number(Main_Form.txt_Tram_Y_Minus.Text), 4)
                        Pos = FormatNumber(Kflop.Y_Abs, 4)
                    Case "txt_Tram_Z_Plus"
                        Value = FormatNumber(Get_Text_Number(Main_Form.txt_Tram_Z_Plus.Text), 4)
                        Pos = FormatNumber(Kflop.Z_Abs, 4)
                        Default_Value = 6
                    Case "txt_Tram_Z_Minus"
                        Value = FormatNumber(Get_Text_Number(Main_Form.txt_Tram_Z_Minus.Text), 4)
                        Pos = FormatNumber(Kflop.Z_Abs, 4)
                        Default_Value = 6
                    Case "txt_Tramming_Feedrate_XY"
                        Value = Get_Text_Number(Main_Form.txt_Tramming_Feedrate_XY.Text)
                        Default_Value = 60
                    Case "txt_Tramming_Feedrate_Z_Up"
                        Value = Get_Text_Number(Main_Form.txt_Tramming_Feedrate_Z_Up.Text)
                        Default_Value = 30
                    Case "txt_Tramming_Feedrate_Z_Down"
                        Value = Get_Text_Number(Main_Form.txt_Tramming_Feedrate_Z_Down.Text)
                        Default_Value = 10
                End Select

                Pos = Get_User_Input(Value, Default_Value, Pos)

                If IsNumeric(Pos) Then
                    Value = CSng(Pos)
                    Select Case Ctrl.Name
                        Case "txt_Tram_X_Plus"
                            Main_Form.txt_Tram_X_Plus.Text = FormatNumber(Value, 4)
                        Case "txt_Tram_X_Minus"
                            Main_Form.txt_Tram_X_Minus.Text = FormatNumber(Value, 4)
                        Case "txt_Tram_Y_Plus"
                            Main_Form.txt_Tram_Y_Plus.Text = FormatNumber(Value, 4)
                        Case "txt_Tram_Y_Minus"
                            Main_Form.txt_Tram_Y_Minus.Text = FormatNumber(Value, 4)
                        Case "txt_Tram_Z_Plus"
                            Main_Form.txt_Tram_Z_Plus.Text = FormatNumber(Value, 4)
                        Case "txt_Tram_Z_Minus"
                            Main_Form.txt_Tram_Z_Minus.Text = FormatNumber(Value, 4)
                        Case "txt_Tramming_Feedrate_XY"
                            Main_Form.txt_Tramming_Feedrate_XY.Text = FormatNumber(Value, 4)
                        Case "txt_Tramming_Feedrate_Z_Up"
                            Main_Form.txt_Tramming_Feedrate_Z_Up.Text = FormatNumber(Value, 4)
                        Case "txt_Tramming_Feedrate_Z_Down"
                            Main_Form.txt_Tramming_Feedrate_Z_Down.Text = FormatNumber(Value, 4)
                    End Select
                Else
                    If Not IsNothing(Pos) Then Flash_Message("Value not a number")
                End If

        End Select

        Return Nothing

    End Function

    Public Function Position_Text_Boxes(ByRef Ctrl As Chip_Text_Box, Evnt As enum_Event_Type) As Object

        Select Case Evnt

            Case enum_Event_Type.Update
                Dim FR As Single = Main_Form.txt_Feed_Rate_Override.Text


                Main_Form.txt_Feedrate.Text = FormatNumber(KFlop.Feed_Rate * (FR / 100), 0)

                Main_Form.txt_X_Abs.Text = FormatNumber(Kflop.X_Abs, 4)
                Main_Form.txt_Y_Abs.Text = FormatNumber(Kflop.Y_Abs, 4)
                Main_Form.txt_Z_Abs.Text = FormatNumber(Kflop.Z_Abs, 4)

                Main_Form.txt_X_Work.Text = FormatNumber(Kflop.X_Work, 4)
                Main_Form.txt_Y_Work.Text = FormatNumber(Kflop.Y_Work, 4)
                Main_Form.txt_Z_Work.Text = FormatNumber(Kflop.Z_Work, 4)


                Select Case Kflop.Get_Current_Coordinate_System_GCode
                    Case "G54"
                        Main_Form.txt_Selected_Coord_System.Back_Color = Color.White
                        Main_Form.txt_Selected_Coord_System.Text = "G55 Off"
                    Case "G55"
                        Main_Form.txt_Selected_Coord_System.Back_Color = Color.MistyRose
                        Main_Form.txt_Selected_Coord_System.Text = "G55 On"
                    Case Else
                        Main_Form.txt_Selected_Coord_System.Text = "Error"
                End Select

                Main_Form.txt_X_Work_Offset.Text = FormatNumber(Kflop.X_Work_Offset, 4)
                Main_Form.txt_Y_Work_Offset.Text = FormatNumber(Kflop.Y_Work_Offset, 4)
                Main_Form.txt_Z_Work_Offset.Text = FormatNumber(Kflop.Z_Work_Offset, 4)

                Main_Form.txt_Tool_Measurement_Type.Text = "None"
                Main_Form.txt_Tool_Measurement_Type.BackColor = Color.MistyRose

                If Main_Form.ctl_Fixture_Offsets.rad_Fixed_Toolsetter.Checked Then
                    Main_Form.txt_Tool_Measurement_Type.Text = "Fixed"
                    Main_Form.txt_Tool_Measurement_Type.BackColor = Color.White
                End If

                If Main_Form.ctl_Fixture_Offsets.rad_Movable_Toolsetter.Checked Then
                    Main_Form.txt_Tool_Measurement_Type.Text = "Movable"
                    Main_Form.txt_Tool_Measurement_Type.BackColor = Color.White
                End If

                If Main_Form.ctl_Fixture_Offsets.rad_Surface.Checked Then
                    Main_Form.txt_Tool_Measurement_Type.Text = "Surface"
                    Main_Form.txt_Tool_Measurement_Type.BackColor = Color.Gold
                End If

                If Main_Form.txt_Offset_Type.Text = "Current XY" Then
                    Main_Form.txt_Offset_Type.BackColor = Color.Gold
                Else
                    Main_Form.txt_Offset_Type.BackColor = Color.White
                End If

            Case enum_Event_Type.Left_Mouse_Button_Down
                Dim Pos As String = ""
                Dim Default_Value As Single = 0
                Dim Value As Single = 0

                Select Case Ctrl.Name
                    Case "txt_X_Abs"
                        Pos = FormatNumber(Kflop.X_Abs, 4)
                    Case "txt_Y_Abs"
                        Pos = FormatNumber(Kflop.Y_Abs, 4)
                    Case "txt_Z_Abs"
                        Pos = FormatNumber(Kflop.Z_Abs, 4)
                        Default_Value = 6
                    Case Else
                        Return False
                End Select

                Pos = Get_User_Input(Pos, Default_Value)
                'Value = CSng(Pos)
                'Value = Kflop.Counts(Value)

                Select Case Ctrl.Name
                    Case "txt_X_Abs"
                        Kflop.X_Abs = CDbl(Pos)
                    Case "txt_Y_Abs"
                        Kflop.Y_Abs = CDbl(Pos)
                    Case "txt_Z_Abs"
                        Kflop.Z_Abs = CDbl(Pos)
                End Select

        End Select

        Return Nothing

    End Function

    Public Function Help_Buttons(ByRef Ctrl As Chip_Button, Evnt As enum_Event_Type) As Object

        Select Case Evnt
            Case enum_Event_Type.Left_Mouse_Button_Down
                Select Case Ctrl.Name
                    Case "btn_Help_Measure_Setter_Height"
                        Open_Help_File(Ctrl.Name)
                End Select
        End Select

        Return Nothing
    End Function


#End Region

#Region "Hot Keys ***************************************************************************************************"

    Public Function Hot_Key_Handler(Command As String) As Boolean
        Pendant_Update_Display()

        Select Case Command

            'Jog buttons need both key down and key up events
            'Jog functions
            Case "X+ DN"
                Main_Form.btn_Jog_X_Plus.Fire_Event(enum_Event_Type.Left_Mouse_Button_Down)
            Case "X+ UP"
                Main_Form.btn_Jog_X_Plus.Fire_Event(enum_Event_Type.Left_Mouse_Button_Up)
            Case "X- DN"
                Main_Form.btn_Jog_X_Minus.Fire_Event(enum_Event_Type.Left_Mouse_Button_Down)
            Case "X- UP"
                Main_Form.btn_Jog_X_Minus.Fire_Event(enum_Event_Type.Left_Mouse_Button_Up)

            Case "Y+ DN"
                Main_Form.btn_Jog_Y_Plus.Fire_Event(enum_Event_Type.Left_Mouse_Button_Down)
            Case "Y+ UP"
                Main_Form.btn_Jog_Y_Plus.Fire_Event(enum_Event_Type.Left_Mouse_Button_Up)
            Case "Y- DN"
                Main_Form.btn_Jog_Y_Minus.Fire_Event(enum_Event_Type.Left_Mouse_Button_Down)
            Case "Y- UP"
                Main_Form.btn_Jog_Y_Minus.Fire_Event(enum_Event_Type.Left_Mouse_Button_Up)

            Case "Z+ DN"
                Main_Form.btn_Jog_Z_Plus.Fire_Event(enum_Event_Type.Left_Mouse_Button_Down)
            Case "Z+ UP"
                Main_Form.btn_Jog_Z_Plus.Fire_Event(enum_Event_Type.Left_Mouse_Button_Up)
            Case "Z- DN"
                Main_Form.btn_Jog_Z_Minus.Fire_Event(enum_Event_Type.Left_Mouse_Button_Down)
            Case "Z- UP"
                Main_Form.btn_Jog_Z_Minus.Fire_Event(enum_Event_Type.Left_Mouse_Button_Up)

            'These buttons only need key down events
            'Jog Speed Selection
            Case "G1 DN"
                Main_Form.btn_Jog_Rate_Fast.Fire_Event(enum_Event_Type.Left_Mouse_Button_Down)
            Case "G2 DN"
                Main_Form.btn_Jog_Rate_Slow.Fire_Event(enum_Event_Type.Left_Mouse_Button_Down)
            Case "G3 DN"
                Main_Form.btn_Jog_Rate_Creep.Fire_Event(enum_Event_Type.Left_Mouse_Button_Down)
            Case "G4 DN"
                Main_Form.btn_Jog_Rate_Big_Step.Fire_Event(enum_Event_Type.Left_Mouse_Button_Down)
            Case "G5 DN"
                Main_Form.btn_Jog_Rate_Little_Step.Fire_Event(enum_Event_Type.Left_Mouse_Button_Down)

            Case "G15 DN"
                Main_Form.btn_Measure_Tool.Fire_Event(enum_Event_Type.Left_Mouse_Button_Down)

            Case "G16 DN"
                Main_Form.btn_Go_To_Tool_Measure.Fire_Event(enum_Event_Type.Left_Mouse_Button_Down)

            Case "G17 DN"
                Main_Form.btn_Go_To_Tool_Seat.Fire_Event(enum_Event_Type.Left_Mouse_Button_Down)

            Case "G20 DN" 'Feed Hold
                Main_Form.btn_Feed.Fire_Event(enum_Event_Type.Left_Mouse_Button_Down)

            Case "G22 DN" 'Cycle Start and Cycle Stop
                Main_Form.btn_Cycle_Start.Fire_Event(enum_Event_Type.Left_Mouse_Button_Down)

            Case "G14 DN" 'Help Button
                If Pendant_Mode <> enum_Pendant_Mode.Help Then
                    Pendant_Mode = enum_Pendant_Mode.Help
                Else
                    Pendant_Mode = enum_Pendant_Mode.Jog
                End If

        End Select

        Return True

    End Function

#End Region

#Region "Pendant Routines *******************************************************************************************"

    Public Enum enum_Pendant_Mode
        Help
        Jog
    End Enum
    Private Pendant_Mode As enum_Pendant_Mode = enum_Pendant_Mode.Jog

    Public Pendant_Message_Pending As Boolean = False

    Public Function Pendant_Query(Message As String) As Boolean
        Dim dlg As New dlg_Pendant
        Logitech_G13.Display_Message(Message, "", "", "")
        dlg.Message = Message
        dlg.ShowDialog()
        If dlg.DialogResult = DialogResult.OK Then Return True Else Return False
    End Function

    Private Function F_Num(Number As Double) As String
        Return FormatNumber(Number, 4).PadLeft(8)
    End Function

    Public Sub Pendant_Update_Display()
        Static Dim Jog_Rate As String = ""

        If Not Logitech_G13.Enabled Then Exit Sub
        If Pendant_Message_Pending Then Exit Sub

        If Main_Form.btn_Jog_Rate_Fast.State = "On" Then Jog_Rate = "F"
        If Main_Form.btn_Jog_Rate_Slow.State = "On" Then Jog_Rate = "S"
        If Main_Form.btn_Jog_Rate_Creep.State = "On" Then Jog_Rate = "C"
        If Main_Form.btn_Jog_Rate_Big_Step.State = "On" Then Jog_Rate = "B"
        If Main_Form.btn_Jog_Rate_Little_Step.State = "On" Then Jog_Rate = "L"

        Select Case Pendant_Mode
            Case enum_Pendant_Mode.Help
                Logitech_G13.Display_Message("F   S   C   B   L   -  Mode",
                                             "-   -   -   -   -   -  Help",
                                             "    -   -   -   -   -      ",
                                             "   Feed      -     Cycle   ")
                Logitech_G13.Update_Display()

            Case enum_Pendant_Mode.Jog
                Logitech_G13.Set_Color(75, 0, 0)
                Logitech_G13.Set_Text(0, Jog_Rate & " Absolute Work    Offset")

                Logitech_G13.Set_Text(1, "X" & F_Num(KFlop.X_Abs) & F_Num(KFlop.X_Work) & F_Num(KFlop.X_Work_Offset))
                Logitech_G13.Set_Text(2, "Y" & F_Num(KFlop.Y_Abs) & F_Num(KFlop.Y_Work) & F_Num(KFlop.Y_Work_Offset))
                Logitech_G13.Set_Text(3, "Z" & F_Num(KFlop.Z_Abs) & F_Num(KFlop.Z_Work) & F_Num(KFlop.Z_Work_Offset))

                Logitech_G13.Update_Display()

        End Select

    End Sub

    Private Sub Pendant_Update_Header(Optional Update_Display As Boolean = True)
        'If Pendant_Message_Displayed Then Exit Sub
        Dim Head As String

        Select Case Pendant_Mode
            Case 0 'Jog
                Logitech_G13.Set_Color(75, 0, 0)
                Head = " Absolute Work   Offset"
            Case 1 'Probe
                Head = "Probe Gap    Surface Offset"
                Logitech_G13.Set_Color(0, 50, 0)

            Case 2 'Offsets
                Head = " Offsets"
                Logitech_G13.Set_Color(50, 50, 0)

            Case Else
                Exit Sub

        End Select

        If Main_Form.btn_Jog_Rate_Fast.State = "On" Then Head = "F" & Head
        If Main_Form.btn_Jog_Rate_Slow.State = "On" Then Head = "S" & Head
        If Main_Form.btn_Jog_Rate_Creep.State = "On" Then Head = "C" & Head
        If Main_Form.btn_Jog_Rate_Big_Step.State = "On" Then Head = "B" & Head
        If Main_Form.btn_Jog_Rate_Little_Step.State = "On" Then Head = "L" & Head

        Logitech_G13.Set_Text(0, Head)

        If Update_Display Then Pendant_Update_Display()

    End Sub

#End Region

End Class
