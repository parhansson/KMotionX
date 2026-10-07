
Imports System.IO

Public Class dlg_Wizards

#Region "Declarations"

    Private Enum enum_Arc_Direction
        CC
        CCW
    End Enum

    Private Structure Coordinate
        Dim X As Single
        Dim Y As Single
        Dim Z As Single
    End Structure

    Private Structure struct_Control_Properties
        Public Type As String
        Public Value As String
    End Structure

    Private Block_Number As Integer
    Private Flatten_Changes_Made As Boolean
    Private Hole_Changes_Made As Boolean
    Private GCode As New List(Of String)
    Private Changes_Made As Boolean = False
    Private Ignore_Event As Boolean = False
    Private Check_Gcode As Boolean = True

    Private Control_List As New Dictionary(Of String, struct_Control_Properties)
    Private Dialog_Symbol As Class_Symbols.class_Symbol

    Private Dialog_Settings As New Class_Settings

    Private Ignore_Tab_Select As Boolean = False

    Public Wizard_GCode_File_Name As String


#End Region

#Region "Shown, Closing"

    Private Sub dlg_Wizards_Shown(sender As Object, e As EventArgs) Handles Me.Shown

        Dim Transparent_Color As Color = Color.FromName(Symbol.Get_Value("Sys.Color.Transparent"))
        Display.Make_Form_Buttons_Transparent(Me, Transparent_Color)

        Dialog_Symbol = Symbol.Get_Symbol("Wizard.Dialog")

        Load_Settings()

        Dialog_Settings.Set_Form_Size_and_Location(Me)

        Dim GCode_sym As Class_Symbols.class_Symbol = Symbol.Get_Symbol("Wizard_Dialog.GCode")
        ctl_GCode.Initialize(GCode_sym, ctl_Drawing, Dialog_Symbol)

        ctl_GCode.Configure_For_Dialog()
        Dim Dwg_sym As Class_Symbols.class_Symbol = Symbol.Get_Symbol("Wizard_Dialog.Drawing")

        'Do not move  or rearrange these tow lines, drawing will not work, fill must be done last
        ctl_Drawing.Initialize(Dwg_sym, Symbol.Get_Value("Sys.Max_X"), Symbol.Get_Value("Sys.Max_Y"), CNC.Z.Max, ctl_GCode)
        ctl_Drawing.Dock = DockStyle.Fill

        ctl_Drawing.Setup()
        Load_GCode(Wizard_GCode_File_Name)

        Frame_Initialize()

        Show_Horizontal_Cutter_Path(True)
        Draw_Cutter_Path()

        Initialize_Arc()
        Initialize_Flatten()

        ctl_GCode.Focus()

    End Sub

    Private Sub dlg_Wizards_FormClosing(sender As Object, e As FormClosingEventArgs) Handles Me.FormClosing

        Dialog_Settings.Save_Form_Size_and_Location(Me)

        ctl_GCode.Finish()

        If Check_Gcode Then
            If Not ctl_GCode.Check_GCode() Then
                e.Cancel = True
                Exit Sub
            End If
        End If

        Wizard_GCode_File_Name = Routings.Current_Part.File_Name


        If Changes_Made Then
            Message_Box.ShowDialog("Changes have been made, but not saved." & vbCrLf & _
                                   "Do yo want to save the file?", "Warning", MessageBoxButtons.YesNo)

            If Message_Box.DialogResult = Windows.Forms.DialogResult.Yes Then
                If Routings.Current_Part.File_Path = "" Then
                    Save_GCode_Dialog(Routings.Current_Part.File_Path)
                Else
                    Save_GCode(Routings.Current_Part.File_Path)
                    Changes_Made = False
                End If
            End If

        End If

        Dim s As String = txt_Flatten_Height.Text

        If DialogResult = Windows.Forms.DialogResult.OK Then
            Save_Settings()
            Dialog_Settings.Save(Chip_Wizards_File)
        End If

    End Sub

#End Region

#Region "Toolstrip"

    Private Sub ts_Wizard_ItemClicked(sender As Object, e As ToolStripItemClickedEventArgs) Handles ts_Wizard.ItemClicked
        ts_Info.Focus()

        Select Case e.ClickedItem.Text
            Case "Exit"
                DialogResult = Windows.Forms.DialogResult.OK

                Close()
            Case "Cancel"
                DialogResult = Windows.Forms.DialogResult.Cancel
                Check_Gcode = False
                Close()

            Case "Load"
                Load_GCode_Dialog(Routings.Current_Part.File_Path)
                Changes_Made = False

            Case "Save"
                Dim Message As String = ctl_GCode.Check_M_Codes
                If Message <> "" Then
                    Message &= vbCrLf & "Continue save?"
                    Message_Box.ShowDialog(Message, "", MessageBoxButtons.YesNo)
                    If Message_Box.DialogResult = Windows.Forms.DialogResult.No Then Exit Sub
                End If

                If Routings.Current_Part.File_Path = "" Then
                    Save_GCode_Dialog(Routings.Current_Part.File_Path)
                Else
                    Save_GCode(Routings.Current_Part.File_Path)
                    Changes_Made = False
                End If

            Case "Save As"
                Dim Message As String = ctl_GCode.Check_M_Codes
                If Message <> "" Then
                    Message &= vbCrLf & "Continue save?"
                    Message_Box.ShowDialog(Message, "", MessageBoxButtons.YesNo)
                    If Message_Box.DialogResult = Windows.Forms.DialogResult.No Then Exit Sub
                End If
                Save_GCode_Dialog(Routings.Current_Part.File_Path, False)

            Case "Append GCode"
                Append_GCode()
                Changes_Made = True

            Case "Delete Line"
                Delete_Line()

            Case "Expand"
                ctl_GCode.Expand()

            Case "Collapse"
                ctl_GCode.Collapse()

            Case "Clear"
                ctl_GCode.Clear_Rows()
                GCode.Clear()
                Changes_Made = False

            Case "Header"
                ts_Header.Text = "No Header"

            Case "No Header"
                ts_Header.Text = "Header"

        End Select

    End Sub

    Private Sub ts_Insert_DropDownItemClicked(sender As Object, e As System.Windows.Forms.ToolStripItemClickedEventArgs) Handles ts_Insert.DropDownItemClicked
        ts_Info.Focus()

        Select Case e.ClickedItem.Name

            Case "ts_Insert_Blank_Line"
                ctl_GCode.Insert_Row("")

            Case "ts_Insert_G0"
                ctl_GCode.Insert_Row("G00 X0 Y0")

            Case "ts_Insert_G1"
                ctl_GCode.Insert_Row("G01 X0 Y0 F1")

            Case "ts_Insert_M03_M08"
                ctl_GCode.Insert_Row("M08 G04 P" & Symbol.Get_Value("Sys.Coolant_Delay"))
                ctl_GCode.Insert_Row("M03 G04 P" & Symbol.Get_Value("Sys.Spindle_Delay"))

            Case "ts_Insert_M05_M09"
                ctl_GCode.Insert_Row("M09 G04 P1")
                ctl_GCode.Insert_Row("M05")

            Case "ts_Insert_End"
                ctl_GCode.Insert_Row("M30")

        End Select

        Changes_Made = True

    End Sub

#End Region

#Region "Utility functions"

    Private Sub Load_GCode(File_Name)
        If File_Name <> "" Then
            Dim F As New Class_Data_File
            Dim Buff As String = ""
            Dim Wizard As Boolean = False

            If F.Open_Input_File(File_Name) Then
                While Not F.End_Of_File
                    F.Read_Line(Buff)
                    If Mid(Buff, 1, 1) = "'" Then
                        If Buff.Contains("Wizard") Then
                            Wizard = True
                            GoTo Early_Exit
                        End If
                    Else
                        GoTo Early_Exit
                    End If
                End While
            Else
                Exit Sub
            End If
Early_Exit:
            F.Close_Input_File()

            'If Wizard Then
            '    ctl_GCode.Load_G_Code(False)
            '    Load_File_Header()
            'End If
        End If
    End Sub

    Private Sub Load_GCode_Dialog(File_Name)
        ' If File_Name <> "" Then
        ' ctl_GCode.Load_G_Code_Dialog()
        Load_File_Header()
        ' End If
    End Sub

    Private Sub Save_GCode(File_Name)
        Create_File_Header(tab_Wizard.SelectedTab)
        ctl_GCode.Save_G_Code(ctrl_Gcode.enum_Backup_Command.Backup_To_Subdirectory)
    End Sub

    Private Sub Save_GCode_Dialog(File_Name As String, Optional Use_Backup_Directory As Boolean = False)
        Dim fn As String

        Create_File_Header(tab_Wizard.SelectedTab)

        fn = ctl_GCode.Save_G_Code_Dialog(Use_Backup_Directory)
        If fn <> "" Then Changes_Made = False

    End Sub

    Private Sub Draw_Cutter_Path()
        ctl_Drawing.Load_Drawing_Points(ctl_GCode)
        ctl_Drawing.Draw_Cutter_Path()
    End Sub

    Private Function Calculate() As Boolean

        Select Case tab_Wizard.SelectedTab.Text
            Case "Move"
                Return Calculate_Move()

            Case "Flatten"
                Return Calculate_Flatten()

            Case "Hole"
                Return Calculate_Hole()

            Case "Line"
                Return Calculate_Line()

            Case "Frame"
                Return Calculate_Frame()

            Case "Circle"
                Return Calculate_Circle()

            Case "Arc"
                Return Calculate_arc

            Case "Translate"
                Return Calculate_Translation()

            Case "Feather"
                Return Calculate_Feather()

        End Select

        Draw_Cutter_Path()
        Return False

    End Function

    Private Function Get_Num(txt As TextBox, ByRef Number As Single, Optional Digits As Integer = 3) As Boolean
        If IsNumeric(txt.Text) Then
            Number = Calc.Round(CSng(txt.Text), Digits)
            Return True
        Else
            Dim N As String = txt.Name
            N = N.Replace("txt_" & tab_Wizard.TabPages(tab_Wizard.TabIndex).Text & "_", "")
            Show_Error(N & " not a number")
            Return False
        End If
    End Function

    Private Sub Append_GCode()
        If Calculate() Then
            ctl_GCode.Append_Rows(GCode)
        End If
    End Sub

    Private Sub Insert_GCode()
        If Calculate() Then
            ctl_GCode.Insert_Rows(GCode)
        End If
    End Sub

    Private Sub Insert_Line()
        ctl_GCode.Insert_Row()
        ctl_Drawing.Reset(False)
    End Sub

    Public Sub Delete_Line()
        ctl_GCode.Delete_Row()
        ctl_Drawing.Reset(False)
    End Sub

    Private Sub Add_Block(Block As String)
        Dim GC As New Class_GCode
        GC.Block = Block
        Block_Number += 1
        GCode.Add(GC.Formatted_Block)
    End Sub

    Private Function Decimal_To_Fraction(Number As String, Optional Precision As Integer = 16) As String
        Dim Whole As Integer = 0
        Dim Fraction As Single
        Dim Numerator As Single
        Dim Denominator As Single
        Dim LCD As Integer = 0
        Dim Best_Numerator As Integer
        Dim Best_Denominator As Integer

        If Number = "" Then Return "0"

        If Not IsNumeric(Number) Then
            Show_Error(Number & " not in decimal format")
            Return ""
        End If
        If Not Number.Contains(".") Then Return Calc.Format_Number(CSng(Number))

        Whole = Math.Truncate(CSng(Number))
        Fraction = Number - Whole

        Best_Numerator = 1
        Best_Denominator = Precision

        Dim E As Single = 1
        Dim Err As Single = 1
        Dim T As Single

        For Numerator = 1 To Precision
            For Denominator = 1 To Precision
                T = Numerator / Denominator
                E = Math.Abs(Fraction - T)
                If E < Err Then
                    Err = E
                    Best_Numerator = Numerator
                    Best_Denominator = Denominator
                End If
            Next
        Next

        If Whole = 0 Then
            Return Trim(Best_Numerator) & "/" & Trim(Best_Denominator)
        Else
            Return Trim(Whole) & " " & Trim(Best_Numerator) & "/" & Trim(Best_Denominator)
        End If

    End Function

    Private Function Fraction_To_Decimal(Number As String) As Single
        Dim S() As String
        Dim Denominator As Single = 0
        Dim Numerator As Single = 0
        Dim Whole As Single = 0

        If Number = "" Then Return "0"

        S = Number.Split("/")

        Select Case S.Length
            Case 0
                GoTo Early_Exit
            Case 1
                If Not IsNumeric(S(0)) Then GoTo Early_Exit
                Return Number
            Case 2
                If Not IsNumeric(S(1)) Then GoTo Early_Exit
                Denominator = S(1)
                S = S(0).Split(" ")

                Select Case S.Length
                    Case 0
                        GoTo Early_Exit
                    Case 1
                        If Not IsNumeric(S(0)) Then GoTo Early_Exit
                        Numerator = S(0)
                        Return Calc.Round(Numerator / Denominator)
                    Case 2
                        If Not IsNumeric(S(0)) Then GoTo Early_Exit
                        If Not IsNumeric(S(1)) Then GoTo Early_Exit
                        Whole = S(0)
                        Numerator = S(1)
                        Return Whole + Calc.Round(Numerator / Denominator)
                    Case Else
                        GoTo Early_Exit
                End Select

            Case Else
                GoTo Early_Exit
        End Select

Early_Exit:
        Show_Error(Number & " not in fraction format")
        Return 0

    End Function

#End Region

#Region "Load/Save settings"

    Private Sub Load_Settings()
        ts_Insert.Enabled = False
        ts_Append_GCode.Enabled = False

        Ignore_Tab_Select = True
        Dialog_Settings.Load(Chip_Wizards_File)
        Dialog_Settings.Load_Control_Values(tab_Wizard, Dialog_Settings, False)
        Ignore_Tab_Select = False

        For T = 0 To tab_Wizard.TabPages.Count - 1
            tab_Wizard.TabPages(T).Tag = False 'Flag for page edited
        Next

        If tab_Wizard.SelectedTab.Text = "Drawing" Then
            ts_Insert.Enabled = False
            ts_Append_GCode.Enabled = False
        Else
            ts_Insert.Enabled = True
            ts_Append_GCode.Enabled = True
        End If

    End Sub

    Private Sub Save_Settings()
        Dialog_Settings.Save_Control_Values(tab_Wizard, Dialog_Settings)
        Dialog_Settings.Save(Chip_Wizards_File)
    End Sub

    Public Sub Create_File_Header(Tab_Page As Control)
        Dim ctrl As Control
        Dim Cnt As Integer = 0
        Dim Done As Boolean = False
        Dim Line As String = ""
        Dim S As String

        If Tab_Page.Controls.Count = 0 Then Exit Sub

        ctl_GCode.GCode_File_Header.Clear()
        ctl_GCode.GCode_File_Header.Add("'Wizard -- " & Tab_Page.Text)

        For I = 0 To Tab_Page.Controls.Count - 1

            If Cnt = 0 Then Line = "' "
            ctrl = Tab_Page.Controls(I)

            If ctrl.GetType.Name = "TextBox" Then
                S = ctrl.Name.Replace("txt_" & Tab_Page.Text & "_", "")
                If Cnt <> 0 Then Line &= "~"
                Line &= S & "=" & ctrl.Text
                Cnt += 1
                If (Cnt = 3) Or (I = Tab_Page.Controls.Count - 1) Then
                    If Line <> "" Then ctl_GCode.GCode_File_Header.Add(Line)
                    Line = ""
                    Cnt = 0
                End If
            End If
        Next

        If Line.Length > 2 Then ctl_GCode.GCode_File_Header.Add(Line)

    End Sub

    Public Sub Load_File_Header()
        Dim ctrl As Control
        Dim Lines As New List(Of String)
        Dim Lst As New Class_Tagged_List
        Dim S() As String
        Dim P() As String
        Dim T As String
        Dim Idx As Integer
        Dim Page_Name As String = ""
        Dim Cnt As Integer = 0

        If ctl_GCode.GCode_File_Header.Count < 1 Then Exit Sub

        For I = 0 To ctl_GCode.GCode_File_Header.Count - 1
            If Mid(ctl_GCode.GCode_File_Header(I), 1, 7) = "'Wizard" Then
                Page_Name = ctl_GCode.GCode_File_Header(I).Replace("'Wizard -- ", "")
                Exit For
            End If
        Next

        If Page_Name = "" Then Exit Sub

        For I = 0 To ctl_GCode.GCode_File_Header.Count - 1
            If Mid(ctl_GCode.GCode_File_Header(I), 1, 2) = "' " Then
                '                ctl_GCode.GCode_File_Header(I) = ctl_GCode.GCode_File_Header(I).Replace(" ", "")

                S = ctl_GCode.GCode_File_Header(I).Split("~")
                S(0) = S(0).Replace("'", "")
                For J = 0 To S.Count - 1
                    T = S(J).Replace(" ", "")
                    If T <> "" Then
                        P = S(J).Split("=")
                        Lst.Set_Value("txt_" & tab_Wizard.SelectedTab.Text & "_" & P(0), P(1))
                    End If
                Next
            End If
        Next

        For I = 1 To tab_Wizard.TabPages.Count - 1
            If tab_Wizard.TabPages(I).Text = Page_Name Then
                For C = 0 To tab_Wizard.TabPages(I).Controls.Count - 1
                    ctrl = tab_Wizard.TabPages(I).Controls(C)
                    If ctrl.GetType.Name = "TextBox" Then
                        Idx = Lst.Get_Index(ctrl.Name)
                        If Idx >= 0 Then
                            ctrl.Text = Lst.Get_Value(Idx)
                        End If
                    End If
                Next
            End If
        Next

    End Sub

#End Region

#Region "Control Events"

    Private Sub tab_Wizard_SelectedIndexChanged(sender As Object, e As System.EventArgs) Handles tab_Wizard.SelectedIndexChanged
        If Ignore_Tab_Select Then Exit Sub

        If tab_Wizard.SelectedTab.Text = "Drawing" Then
            ts_Insert.Enabled = False
            ts_Append_GCode.Enabled = False
        Else
            ts_Insert.Enabled = True
            ts_Append_GCode.Enabled = True
        End If

        Select Case tab_Wizard.SelectedTab.Text

            Case "Translate"
                If txt_Translate_Input_File.Text <> "" Then
                    If File.Exists(txt_Translate_Input_File.Text) Then
                        Trans_Buffer = File.ReadAllLines(txt_Translate_Input_File.Text)
                    Else
                        Show_Error("File : " & txt_Translate_Input_File.Text & " not found.")
                    End If

                End If
        End Select

    End Sub

#End Region

#Region "Move Wizard"

    Private Function Calculate_Move() As Boolean
        Dim End_X As Single
        Dim End_Y As Single
        Dim End_Z As Single
        Dim Feed_Rate As Single
        Dim Block As String

        If Not Get_Num(txt_Move_X, End_X) Then Return False
        If Not Get_Num(txt_Move_Y, End_Y) Then Return False
        If Not Get_Num(txt_Move_Z, End_Z) Then Return False
        If Not Get_Num(txt_Move_Feed_Rate, Feed_Rate) Then Return False

        Block_Number = 0
        GCode.Clear()

        If ts_Header.Text = "Header" Then
            If chk_Move_Rapid.Checked Then
                Add_Block("(Rapid to X" & End_X & " Y" & End_Y & " Z" & End_Z & ")")
            Else
                Add_Block("(Move to X" & End_X & " Y" & End_Y & " Z" & End_Z & " F" & Feed_Rate & ")")
            End If
        End If


        If chk_Move_Rapid.Checked Then Block = "G00" Else Block = "G01"
        If chk_Move_Z_First.Checked And chk_Move_Z.Checked Then Add_Block(Block & " Z" & End_Z)

        If chk_Move_X.Checked Then Block &= " X" & End_X
        If chk_Move_Y.Checked Then Block &= " Y" & End_Y
        If chk_Move_Z.Checked Then Block &= " Z" & End_Z

        Add_Block(Block)

        Return True

    End Function

    Private Sub btn_Move_Current_XY_Click(sender As Object, e As EventArgs) Handles btn_Move_Current_XY.Click
        txt_Move_X.Text = CNC.X.Abs
        txt_Move_Y.Text = CNC.Y.Abs
    End Sub

    Private Sub btn_Move_Current_Z_Click(sender As Object, e As EventArgs) Handles btn_Move_Current_Z.Click
        txt_Move_Z.Text = CNC.Z.Abs
    End Sub

#End Region

#Region "Flatten Wizard"

    Private Sub Initialize_Flatten()
        Dim ctrl As Control
        Dim T As String

        For I = 0 To tab_Flatten.Controls.Count - 1
            ctrl = tab_Flatten.Controls(I)
            T = ctrl.GetType.ToString
            If T = "System.Windows.Forms.TextBox" Then
                If ctrl.Text = "" Then ctrl.Text = 0
            End If
        Next
    End Sub

    Private Function Calculate_Flatten() As Boolean
        Dim Tool_Diameter As Single
        Dim Tool_Radius As Single
        Dim Start_X As Single = 0
        Dim End_X As Single
        Dim Start_Y As Single = 0
        Dim End_Y As Single
        Dim Start_Pos As Single
        Dim End_Pos As Single
        Dim Start_Pos_Sec As Single
        Dim End_Pos_Sec As Single
        Dim Passes As Integer
        Dim Z_Passes As Integer = 0
        Dim Pos As Single
        Dim Pos_Sec As Single
        Dim Pass_Depth As Single
        Dim Cut_Depth As Single
        Dim Thickness As Single
        Dim Feed_Rate As Single
        Dim Plunge_Rate As Single
        Dim Step_Over As Single
        Dim Safe_Z As Single
        Dim Axis As String
        Dim Sec_Axis As String
        Dim Z As Single = 0
        Dim Direction As Boolean = True
        Dim XY_Done As Boolean = False
        Dim Z_Done As Boolean = False
        Dim First_Pass As Boolean = True
        Dim Park_X As Single
        Dim Park_Y As Single
        Dim Park_Z As Single


        If Not Get_Num(txt_Flatten_Tool_Diameter, Tool_Diameter) Then Return False
        If Not Get_Num(txt_Flatten_X_Start, Start_X) Then Return False
        If Not Get_Num(txt_Flatten_Y_Start, Start_Y) Then Return False
        If Not Get_Num(txt_Flatten_Width, End_X) Then Return False
        If Not Get_Num(txt_Flatten_Height, End_Y) Then Return False
        If Not Get_Num(txt_Flatten_Pass_Depth, Pass_Depth) Then Return False
        If Not Get_Num(txt_Flatten_Cut_Depth, Cut_Depth) Then Return False
        If Not Get_Num(txt_Flatten_Start_Thickness, Thickness) Then Return False
        If Not Get_Num(txt_Flatten_Feed_Rate, Feed_Rate) Then Return False
        If Not Get_Num(txt_Flatten_Plunge_Rate, Plunge_Rate) Then Return False
        If Not Get_Num(txt_Flatten_Step_Over, Step_Over) Then Return False
        If Not Get_Num(txt_Flatten_Safe_Z, Safe_Z) Then Return False
        If Not Get_Num(txt_Flatten_Park_X, Park_X) Then Return False
        If Not Get_Num(txt_Flatten_Park_Y, Park_Y) Then Return False
        If Not Get_Num(txt_Flatten_Park_Z, Park_Z) Then Return False

        If Pass_Depth > CSng(0.1) Then
            Show_Error("Max Pass Depth is 0.1")
            Return False
        End If

        If Pass_Depth > CSng(0.1) Then
            Show_Error("Max Pass Depth is 0.1")
            Return False
        End If

        If Feed_Rate > 50 Then
            Show_Error("Max Reed Rate is 50")
            Return False
        End If

        If Safe_Z < ".25" Then
            Show_Error("Safe_Z must be at least .25")
            Return False
        End If

        If Park_Z < Safe_Z Then
            Show_Error("Park Z must be greater than Safe_Z")
            Return False
        End If


        If Feed_Rate <= 0 Or Plunge_Rate <= 0 Then
            Show_Error("Feedrate and Plunge rate must be greater than 0")
            Return False
        End If

        End_X += Start_X
        End_Y += Start_Y

        Tool_Radius = Tool_Diameter / 2
        Block_Number = 0
        GCode.Clear()
        Z = Thickness

        'If ts_Header.Text = "Header" Then Add_Block("(Flatten " & Start_X & "," & Start_Y & " to " & End_X & "," & End_Y & ")")

        Direction = True
        If rad_Flatten_X_Direction.Checked Then 'Feed in X direction
            Start_Pos = Start_X - Tool_Radius
            End_Pos = End_X + Tool_Radius
            Start_Pos_Sec = Start_Y + Tool_Radius
            End_Pos_Sec = End_Y - Tool_Radius '+ Start_Y
            Axis = "X"
            Sec_Axis = "Y"
        Else  'Feed in Y direction
            Start_Pos = Start_Y - Tool_Radius
            End_Pos = End_Y + Tool_Radius
            Start_Pos_Sec = Start_X + Tool_Radius
            End_Pos_Sec = End_X - Tool_Radius '+ Start_X
            Axis = "Y"
            Sec_Axis = "X"
        End If

        If (End_Pos_Sec + Tool_Radius) - (Start_Pos_Sec - Tool_Radius) < Tool_Diameter Then
            Message_Box.ShowDialog("Cutter too big for area, continue?", , MessageBoxButtons.YesNo)
            If Message_Box.DialogResult <> Windows.Forms.DialogResult.Yes Then Return False
            Passes = 1
        End If

        Add_Block("G00 Z" & Safe_Z + Thickness)
        Add_Block("G00 " & Axis & Start_Pos & " " & Sec_Axis & Start_Pos_Sec)
        Add_Block("M08 M03")
        Add_Block("G04 P1")


        'RWC Pause causes problems with program not pausing after a rewind
        'Add_Block("G04 P" & Symbol.Get_Value("Sys.Coolant_Delay"))
        'Add_Block("M08")
        'Add_Block("G04 P" & Symbol.Get_Value("Sys.Spindle_Delay"))
        'Add_Block("M03")

        If Pass_Depth > Cut_Depth Then
            Z = -Cut_Depth
        Else
            Z = -Pass_Depth
        End If

        Add_Block("G00 Z" & Safe_Z + Thickness)

        While Not Z_Done

            Add_Block("G00 " & Axis & Start_Pos & " " & Sec_Axis & Start_Pos_Sec)

            Z_Passes += 1
            Add_Block("G01 Z" & Math.Round(Thickness + Z, 3) & " F" & Plunge_Rate)

            Pos = Start_Pos
            Pos_Sec = Start_Pos_Sec
            XY_Done = False

            While Not XY_Done

                If Pos < End_Pos Then
                    Pos = End_Pos
                Else
                    Pos = Start_Pos
                End If
                Add_Block("G01 " & Axis & Pos & " F" & Feed_Rate)

                Passes += 1

                If Pos_Sec >= End_Pos_Sec Then Exit While

                If Pos_Sec + Step_Over > End_Pos_Sec Then
                    Pos_Sec = End_Pos_Sec
                Else
                    Pos_Sec += Step_Over
                End If

                Add_Block("G01 " & Sec_Axis & Pos_Sec & " F" & Feed_Rate)

            End While

            If Z <= -Cut_Depth Then Exit While

            Z -= Pass_Depth
            If Z <= -Cut_Depth Then
                Z = -Cut_Depth
            End If

            Z = Math.Round(Z, 3)

        End While


        Add_Block("G00 Z" & Park_Z)
        Add_Block("M05 M09")
        Add_Block("G00 X" & Park_X & " Y" & Park_Y)
        Add_Block("M30")

        txt_Flatten_XY_Passes.Text = Passes
        txt_Flatten_Z_Passes.Text = Z_Passes

        Return True

    End Function

    Private Sub txt_Flatten_Cut_Depth_Validated(sender As Object, e As EventArgs) Handles txt_Flatten_Cut_Depth.Validated

        If IsNumeric(txt_Flatten_Cut_Depth.Text) Then
            txt_Flatten_Start_Thickness.Text = Math.Round(CSng(txt_Flatten_End_Thickness.Text) + CSng(txt_Flatten_Cut_Depth.Text), 3)
        Else
            Show_Error("Cut Depth is not a number.")
        End If

    End Sub

    Private Sub txt_Flatten_Start_Thickness_Validated(sender As Object, e As EventArgs) Handles txt_Flatten_Start_Thickness.Validated
        If IsNumeric(txt_Flatten_Start_Thickness.Text) Then
            txt_Flatten_Cut_Depth.Text = Math.Round(CSng(txt_Flatten_Start_Thickness.Text) - CSng(txt_Flatten_End_Thickness.Text), 3)
        Else
            Show_Error("Thickness At Start is not a number.")
        End If

    End Sub

    Private Sub txt_Flatten_End_Thickness_Validated(sender As Object, e As EventArgs) Handles txt_Flatten_End_Thickness.Validated
        If IsNumeric(txt_Flatten_End_Thickness.Text) Then
            txt_Flatten_Cut_Depth.Text = Math.Round(CSng(txt_Flatten_Start_Thickness.Text) - CSng(txt_Flatten_End_Thickness.Text), 3)
        Else
            Show_Error("Thickness After Cut is not a number.")
        End If
    End Sub

    Private Sub btn_Flatten_Measure_Click(sender As Object, e As EventArgs) Handles btn_Flatten_Material_Set.Click
        txt_Flatten_Start_Thickness.Text = Symbol.Get_Value("Text_Box.Probe.Material.Round_3")
        txt_Flatten_Cut_Depth.Text = Math.Round(CSng(txt_Flatten_Start_Thickness.Text) - CSng(txt_Flatten_End_Thickness.Text), 3)

    End Sub

#End Region

#Region "Hole Wizard"

    Private Function Calculate_Spiral_Hole() As Boolean
        'X = cx + R * cos(a)
        'Y = cy + R * sin(a)
        'Where r is the radius, cx,cy the origin, and a the angle.

        Return True
    End Function

    Private Sub Calculate_Hole_Circle(Center_X As Single, Center_Y As Single, Hole_Diameter As Single, Tool_Diameter As Single, _
                                 Depth As Single, Plunge_Rate As Single, Feedrate As Single)
        Dim Start_X As Single
        Dim End_X As Single
        Dim Start_Y As Single
        Dim End_Y As Single
        Dim Hole_Radius As Single = Hole_Diameter / 2
        Dim Tool_Radius As Single = Tool_Diameter / 2
        Dim I As Single
        Dim J As Single

        'Goto start point
        Start_X = Center_X
        Start_Y = Center_Y + Hole_Radius - Tool_Radius
        Add_Block("G01 X" & Start_X & " Y" & Start_Y & " F" & Feedrate)

        'Move Z to cut depth
        Add_Block("G01 Z" & Depth & " F" & Plunge_Rate)

        'Top of right
        Start_X = Center_X
        Start_Y = Center_Y + Hole_Radius - Tool_Radius
        End_X = Center_X + Hole_Radius - Tool_Radius
        End_Y = Center_Y
        I = Calc.Round(Center_X - Start_X)
        J = Calc.Round(Center_Y - Start_Y)
        Add_Block("G02 X" & End_X & " Y" & End_Y & " I" & I & " J" & J & " F" & Feedrate)

        'Right to bottom
        Start_X = End_X
        Start_Y = End_Y
        End_X = Center_X
        End_Y = Center_Y - Hole_Radius + Tool_Radius
        I = Calc.Round(Center_X - Start_X)
        J = Calc.Round(Center_Y - Start_Y)
        Add_Block("G02 X" & End_X & " Y" & End_Y & " I" & I & " J" & J & " F" & Feedrate)

        'Bottom to Left
        Start_X = End_X
        Start_Y = End_Y
        End_X = Center_X - Hole_Radius + Tool_Radius
        End_Y = Center_Y
        I = Calc.Round(Center_X - Start_X)
        J = Calc.Round(Center_Y - Start_Y)
        Add_Block("G02 X" & End_X & " Y" & End_Y & " I" & I & " J" & J & " F" & Feedrate)

        'Left to top
        Start_X = End_X
        Start_Y = End_Y
        End_X = Center_X
        End_Y = Center_Y + Hole_Radius - Tool_Radius
        I = Calc.Round(Center_X - Start_X)
        J = Calc.Round(Center_Y - Start_Y)
        Add_Block("G02 X" & End_X & " Y" & End_Y & " I" & I & " J" & J & " F" & Feedrate)

    End Sub

    Private Function Calculate_Hole() As Boolean
        Dim Center_X As Single
        Dim Center_Y As Single
        Dim Hole_Diameter As Single
        Dim Tool_Diameter As Single
        Dim Feed_Rate As Single
        Dim Plunge_Rate As Single
        Dim Cut_Depth As Single
        Dim Pass_Depth As Single
        Dim Step_Over As Single
        Dim Safe_Z As Single
        Dim Z As Single = 0

        If Not Get_Num(txt_Hole_Center_X, Center_X) Then Return False
        If Not Get_Num(txt_Hole_Center_Y, Center_Y) Then Return False
        If Not Get_Num(txt_Hole_Diameter, Hole_Diameter) Then Return False
        If Not Get_Num(txt_Hole_Tool_Diameter, Tool_Diameter) Then Return False
        If Not Get_Num(txt_Hole_Feed_Rate, Feed_Rate) Then Return False
        If Not Get_Num(txt_Hole_Plunge_Rate, Plunge_Rate) Then Return False
        If Not Get_Num(txt_Hole_Cut_Depth, Cut_Depth) Then Return False
        If Not Get_Num(txt_Hole_Pass_Depth, Pass_Depth) Then Return False
        If Not Get_Num(txt_Hole_Step_Over, Step_Over) Then Return False
        If Not Get_Num(txt_Hole_Safe_Z, Safe_Z) Then Return False

        Block_Number = 0
        GCode.Clear()

        'Move Z to safe height
        Add_Block("G00 Z" & Safe_Z)

        'Go to start point
        Add_Block("G00 X" & Center_X & " Y" & Center_Y)

        Z = Pass_Depth
        Do
            Z -= Pass_Depth
            If Z < -Cut_Depth Then Z = -Cut_Depth
            Calculate_Hole_Circle(Center_X, Center_X, Hole_Diameter, Tool_Diameter, Z, Plunge_Rate, Feed_Rate)
            Add_Block("G00 X" & Center_X & " Y" & Center_Y & " F" & Feed_Rate)
        Loop Until Z = -Cut_Depth


        Return True

    End Function

    Private Sub btn_Hole_Deeper_Click(sender As Object, e As EventArgs) Handles btn_Hole_Deeper.Click
        Dim X As Single
        If Not Get_Num(txt_Hole_Cut_Depth, X) Then Exit Sub
        X += 0.001
        txt_Hole_Cut_Depth.Text = X
    End Sub

    Private Sub btn_Hole_Shallower_Click(sender As Object, e As EventArgs) Handles btn_Hole_Shallower.Click
        Dim X As Single
        If Not Get_Num(txt_Hole_Cut_Depth, X) Then Exit Sub
        X -= 0.001
        txt_Hole_Cut_Depth.Text = X
    End Sub

    Private Sub btn_txt_Hole_Current_Click(sender As Object, e As EventArgs) Handles btn_txt_Hole_Current.Click
        txt_Hole_Center_X.Text = CNC.X.Abs
        txt_Hole_Center_Y.Text = CNC.Y.Abs
    End Sub

#End Region

#Region "Line Wizard"

    Private Function Calculate_Line() As Boolean
        'Dim Tool_Diameter As Single
        'Dim Feed_Rate As Single
        'Dim Plunge_Rate As Single
        'Dim Pass_Depth As Single
        'Dim Cut_Depth As Single
        'Dim Start_X As Single
        'Dim Start_Y As Single
        'Dim End_X As Single
        'Dim End_Y As Single
        'Dim Start_Angle As Single
        'Dim End_Angle As Single
        'Dim Length As Single

        'If Not Get_Num(txt_Straight_Tool_Diameter, Tool_Diameter) Then Return False
        'If Not Get_Num(txt_Straight_Pass_Depth, Pass_Depth) Then Return False
        'If Not Get_Num(txt_Straight_Cut_Depth, Cut_Depth) Then Return False
        'If Not Get_Num(txt_Straight_Feed_Rate, Feed_Rate) Then Return False
        'If Not Get_Num(txt_Straight_Plunge_Rate, Plunge_Rate) Then Return False
        'If Not Get_Num(txt_Horizontal_End_X, Start_X) Then Return False
        'If Not Get_Num(txt_Straight_Start_Y, Start_Y) Then Return False
        'If Not Get_Num(txt_Horizontal_Start_X, End_X) Then Return False
        'If Not Get_Num(txt_Line_End_Y, End_Y) Then Return False
        'If Not Get_Num(txt_Line_Start_Angle, Start_Angle) Then Return False
        'If Not Get_Num(txt_Line_End_Angle, End_Angle) Then Return False
        'If Not Get_Num(txt_Line_Length, Length) Then Return False

        'If ts_Header.Text = "Header" Then
        '    If chk_Move_Rapid.Checked Then
        '        Add_Block("(Rapid to X" & End_X & " Y" & End_Y & " Z" & End_Z & ")")
        '    Else
        '        Add_Block("(Move to X" & End_X & " Y" & End_Y & " Z" & End_Z & " F" & Feed_Rate & ")")
        '    End If
        'End If

        'If End_X < Start_X Then
        '    Show_Error("X End less than X Start")
        '    Return False
        'End If

        'If End_Y < Start_Y Then
        '    Show_Error("Y End less than Y Start")
        '    Return False
        'End If

        'If Pass_Depth > CSng(0.1) Then
        '    Show_Error("Max Pass Depth is 0.1")
        '    Return False
        'End If

        'If Feed_Rate > 60 Then
        '    Show_Error("Max Reed Rate is 60")
        '    Return False
        'End If

        'Dim X As Single = Start_X
        'Dim Y As Single = Start_Y
        'Dim Z As Single = 0
        'Dim X_Direction As Boolean = True

        'Block_Number = 0
        'GCode.Clear()
        'Z = 0

        'Do
        '    X_Direction = True
        '    X = Start_X
        '    Y = Start_Y
        '    Z -= Pass_Depth
        '    If Z < -Cut_Depth Then
        '        Z = -Cut_Depth
        '    End If

        '    X_Direction = True

        '    'Add_Block("G01 Z2 F" & Plunge_Rate)
        '    'Add_Block("G01 X" & X & " Y" & Y & " M03 F" & Feed_Rate)
        '    'Add_Block("G01 Z" & Z & " F" & Plunge_Rate)

        '    'While Y < End_Y

        '    '    If X_Direction Then
        '    '        Add_Block("G01 X" & End_X & " F" & Feed_Rate)
        '    '    Else
        '    '        Add_Block("G01 X" & Start_X & " F" & Feed_Rate)
        '    '    End If
        '    '    X_Direction = Not X_Direction

        '    'End While

        'Loop Until Z = -Cut_Depth

        'Add_Block("G00 Z2 F" & Plunge_Rate)
        'Add_Block("G00 X" & Start_X & " Y" & Start_Y)
        'Add_Block("M05 M09")

        Return True

    End Function

    Private Sub btn_Horizontal_Cutter_Path_Click(sender As Object, e As EventArgs) Handles btn_Horizontal_Cutter_Path.Click
        Show_Horizontal_Cutter_Path(False)
    End Sub

    Private Sub Show_Horizontal_Cutter_Path(Reset As Boolean)
        If Reset Then
            Horizontal_Cutter_Top_Left.Visible = True
            Horizontal_Cutter_Top_Right.Visible = False
            Horizontal_Cutter_Bottom_Left.Visible = False
            Horizontal_Cutter_Bottom_Right.Visible = False
        Else
            If Horizontal_Cutter_Top_Left.Visible Then
                Horizontal_Cutter_Top_Left.Visible = False
                Horizontal_Cutter_Top_Right.Visible = True
            ElseIf Horizontal_Cutter_Top_Right.Visible Then
                Horizontal_Cutter_Top_Right.Visible = False
                Horizontal_Cutter_Bottom_Right.Visible = True
            ElseIf Horizontal_Cutter_Bottom_Right.Visible Then
                Horizontal_Cutter_Bottom_Right.Visible = False
                Horizontal_Cutter_Bottom_Left.Visible = True
            ElseIf Horizontal_Cutter_Bottom_Left.Visible Then
                Horizontal_Cutter_Bottom_Left.Visible = False
                Horizontal_Cutter_Top_Left.Visible = True
            End If
        End If

    End Sub


    Private Sub btn_Line_Shallower_Click(sender As Object, e As EventArgs) Handles btn_Line_Shallower.Click
        Dim X As Single
        If Not Get_Num(txt_Straight_Cut_Depth, X) Then Exit Sub
        X += 0.001
        txt_Hole_Cut_Depth.Text = X
    End Sub

    Private Sub btn_Line_Deeper_Click(sender As Object, e As EventArgs) Handles btn_Line_Deeper.Click
        Dim X As Single
        If Not Get_Num(txt_Straight_Cut_Depth, X) Then Exit Sub
        X -= 0.001
        txt_Hole_Cut_Depth.Text = X
    End Sub

    Private Sub btn_Horizontal_Current_Start_Click(sender As Object, e As EventArgs) Handles btn_Horizontal_Current_Start.Click
        txt_Horizontal_Start_X.Text = CNC.X.Abs
    End Sub

    Private Sub btn_Horizontal_Current_End_Click(sender As Object, e As EventArgs) Handles btn_Horizontal_Current_End.Click
        txt_Horizontal_End_X.Text = CNC.X.Abs
    End Sub

    Private Sub Polar_To_Cartisian(Anchor As String)
        'Dim Start_X As Single
        'Dim Start_Y As Single
        'Dim End_X As Single
        'Dim End_Y As Single
        'Dim Start_Angle As Single
        'Dim End_Angle As Single
        'Dim Length As Single

        'If Not Get_Num(txt_Line_Start_Angle, Start_Angle) Then Exit Sub
        'If Not Get_Num(txt_Line_End_Angle, End_Angle) Then Exit Sub
        'If Not Get_Num(txt_Line_Length, Length) Then Exit Sub

        'If Anchor = "Start" Then
        '    'x = r cosq,   y = r sinq


        'Else

        'End If

        'txt_Horizontal_End_X.Text = Start_X
        'txt_Straight_Start_Y.Text = Start_Y
        'txt_Horizontal_Start_X.Text = End_X
        'txt_Line_End_Y.Text = End_Y

    End Sub

    Private Sub Cartisian_To_Polar()
        'Dim Start_X As Single
        'Dim Start_Y As Single
        'Dim End_X As Single
        'Dim End_Y As Single
        'Dim Angle As Single
        'Dim Radians As Single
        'Dim Length As Single

        'If Not Get_Num(txt_Horizontal_End_X, Start_X) Then Exit Sub
        'If Not Get_Num(txt_Straight_Start_Y, Start_Y) Then Exit Sub
        'If Not Get_Num(txt_Horizontal_Start_X, End_X) Then Exit Sub
        'If Not Get_Num(txt_Line_End_Y, End_Y) Then Exit Sub

        'Length = Calc.Format_Number(Math.Sqrt((End_X - Start_X) ^ 2 + (End_Y - Start_Y) ^ 2))
        'Radians = Math.Atan2(End_Y - Start_Y, End_X - Start_X)
        'Angle = Calc.Format_Number(Radians * (180 / Math.PI))

        'txt_Line_Start_Angle.Text = Angle
        'txt_Line_End_Angle.Text = Angle - 90

        'txt_Line_Length.Text = Length

    End Sub


    'Private Sub txt_Line_Start_X_Validated(sender As Object, e As EventArgs)
    '    Cartisian_To_Polar()
    'End Sub

    'Private Sub txt_Line_Start_Y_Validated(sender As Object, e As EventArgs)
    '    Cartisian_To_Polar()
    'End Sub

    'Private Sub txt_Line_End_X_Validated(sender As Object, e As EventArgs)
    '    Cartisian_To_Polar()
    'End Sub

    'Private Sub txt_Line_End_Y_Validated(sender As Object, e As EventArgs)
    '    Cartisian_To_Polar()
    'End Sub

    'Private Sub txt_Line_Angle_Validated(sender As Object, e As EventArgs) Handles txt_Angle_Start_Angle.Validated

    'End Sub

    'Private Sub txt_Line_Length_Validated(sender As Object, e As EventArgs)

    'End Sub

#End Region

#Region "Frame"


    Private Sub Frame_Initialize()
        rad_Frame_Cut_Down.Checked = True
        If txt_Frame_Mat_Width.Text = "" Then txt_Frame_Mat_Width.Text = "0"
        If txt_Frame_Frame_Width.Text = "" Then txt_Frame_Frame_Width.Text = "0"
        If txt_Frame_Rail_Width.Text = "" Then txt_Frame_Rail_Width.Text = "0"
        If txt_Frame_Rough_Cut.Text = "" Then txt_Frame_Rough_Cut.Text = "0"
        If txt_Frame_X_Offset.Text = "" Then txt_Frame_X_Offset.Text = "0"
        If txt_Frame_Y_Offset.Text = "" Then txt_Frame_Y_Offset.Text = "0"
        Update_Fractional_Measurents()

    End Sub

    Private Function Calculate_Frame_Dimensions(txt As TextBox) As Boolean
        Dim Frame_Width As Single
        Dim Mat_Width As Single
        Dim Rail_Width As Single

        Dim Notch As Single = 0.25

        Select Case txt.Name

            Case "txt_Frame_Mat_Width", "txt_Frame_Mat_Width_Fractional"
                If txt.Name = "txt_Frame_Mat_Width_Fractional" Then Update_Decimal_Measurements()
                If Not Get_Num(txt_Frame_Mat_Width, Mat_Width) Then Return False
                If Not Get_Num(txt_Frame_Rail_Width, Rail_Width) Then Return False
                Frame_Width = Mat_Width + (Rail_Width * 2) - (Notch * 2)
                txt_Frame_Frame_Width.Text = Calc.Format_Number(Frame_Width)

            Case "txt_Frame_Rail_Width", "txt_Frame_Rail_Width_Fractional"
                If txt.Name = "txt_Frame_Rail_Width_Fractional" Then Update_Decimal_Measurements()
                If Not Get_Num(txt_Frame_Mat_Width, Mat_Width) Then Return False
                If Not Get_Num(txt_Frame_Rail_Width, Rail_Width) Then Return False
                Frame_Width = Mat_Width + (Rail_Width * 2) - (Notch * 2)
                txt_Frame_Frame_Width.Text = Calc.Format_Number(Frame_Width)

            Case "txt_Frame_Frame_Width", "txt_Frame_Frame_Width_Fractional"
                If txt.Name = "txt_Frame_Width_Fractional" Then Update_Decimal_Measurements()
                If Not Get_Num(txt_Frame_Frame_Width, Frame_Width) Then Return False
                If Not Get_Num(txt_Frame_Rail_Width, Rail_Width) Then Return False
                Frame_Width = Mat_Width + (Rail_Width * 2) - (Notch * 2)
                txt_Frame_Mat_Width.Text = Calc.Format_Number(Mat_Width)

        End Select

        txt_Frame_Rough_Cut.Text = Calc.Format_Number(Frame_Width + 0.25)
        txt_Frame_Upper_Left.Text = Calc.Format_Number(Rail_Width) & ", " & Calc.Format_Number(Rail_Width)
        txt_Frame_Lower_Right.Text = Calc.Format_Number(Frame_Width) & ", 0"
        txt_Frame_Upper_Right.Text = Calc.Format_Number(Frame_Width - Rail_Width) & ", " & Calc.Format_Number(Rail_Width)

        Update_Fractional_Measurents()

        lab_Frame_Lower_Left.Focus()

        Return True

    End Function

    Private Function Calculate_Frame() As Boolean
        Dim Tool_Diameter As Single
        Dim Tool_Radius As Single
        Dim Cut_Depth As Single
        Dim Feed_Rate As Single
        Dim Safe_Z As Single
        Dim Lower_Left As Coordinate
        Dim Upper_Left As Coordinate
        Dim Lower_Right As Coordinate
        Dim Upper_Right As Coordinate
        Dim Frame_Width As Single
        Dim Rail_Width As Single
        Dim X_Offset As Single
        Dim Y_Offset As Single
        Dim X As Single
        Dim Y As Single


        If Not Get_Num(txt_Frame_Tool_Diameter, Tool_Diameter) Then Return False
        If Not Get_Num(txt_Frame_Cut_Depth, Cut_Depth) Then Return False
        If Not Get_Num(txt_Frame_Feedrate, Feed_Rate) Then Return False
        If Not Get_Num(txt_Frame_Safe_Z, Safe_Z) Then Return False
        Tool_Radius = Tool_Diameter / 2

        If Not Get_Num(txt_Frame_Rail_Width, Rail_Width) Then Return False
        If Not Get_Num(txt_Frame_Frame_Width, Frame_Width) Then Return False

        If Not Get_Num(txt_Frame_X_Offset, X_Offset) Then Return False
        If Not Get_Num(txt_Frame_Y_Offset, Y_Offset) Then Return False

        If Feed_Rate > 60 Then
            Show_Error("Max Reed Rate is 60")
            Return False
        End If

        If Safe_Z < ".25" Then
            Show_Error("Safe_Z must be at least .25")
            Return False
        End If

        Dim Raidus As Single = 0.125

        Calc.Polar_To_Cartesian(Raidus, 135, X, Y)
        Lower_Left.X = X
        Lower_Left.Y = Y
        Upper_Left.X = Rail_Width + X
        Upper_Left.Y = Rail_Width + Y

        Calc.Polar_To_Cartesian(Raidus, 45, X, Y)
        Lower_Right.X = Frame_Width + X
        Lower_Right.Y = Y
        Upper_Right.X = Frame_Width - Rail_Width + X
        Upper_Right.Y = Rail_Width + Y

        'Fudge the top so the cutter clears on an oversize part
        Upper_Left.X += 0.125
        Upper_Left.Y += 0.125
        Upper_Right.X -= 0.125
        Upper_Right.Y += 0.125

        Upper_Left.X += X_Offset
        Upper_Left.Y += Y_Offset
        Lower_Left.X += X_Offset
        Lower_Left.Y += Y_Offset
        Upper_Right.X += X_Offset
        Upper_Right.Y += Y_Offset
        Lower_Right.X += X_Offset
        Lower_Right.Y += Y_Offset

        Block_Number = 0
        GCode.Clear()

        If ts_Header.Text = "Header" Then Add_Block("(Frame " & txt_Frame_Frame_Width_Fractional.Text & " x " & txt_Frame_Rail_Width_Fractional.Text & ")")

        Add_Block("G01 Z" & Safe_Z & " F30")
        Add_Block("G00 X0 Y0")
        Add_Block("G01 Z" & Safe_Z & " F30")
        Add_Block("M08 G04 P" & Symbol.Get_Value("Sys.Coolant_Delay"))
        Add_Block("M03 G04 P" & Symbol.Get_Value("Sys.Spindle_Delay"))

        Add_Block("G01 X" & Upper_Left.X & " Y" & Upper_Left.Y & " F50")
        Add_Block("G01 Z" & -Cut_Depth & "F15")
        Add_Block("G01 X" & Lower_Left.X & " Y" & Lower_Left.Y & "F" & Feed_Rate)
        Add_Block("G00 Z" & Safe_Z & " F30")

        If rad_Frame_Cut_Down.Checked Then
            Add_Block("G00 X" & Upper_Right.X & "Y" & Upper_Right.Y & " F50")
            Add_Block("G01 Z" & -Cut_Depth & "F15")
            Add_Block("G01 X" & Lower_Right.X & " Y" & Lower_Right.Y & "F" & Feed_Rate)
        Else
            Add_Block("G01 X" & Lower_Right.X & " Y" & Lower_Right.Y & "F" & Feed_Rate)
            Add_Block("G01 Z" & -Cut_Depth & "F15")
            Add_Block("G00 X" & Upper_Right.X & "Y" & Upper_Right.Y & " F50")
        End If

        Add_Block("G00 Z" & Safe_Z)
        Add_Block("M05")
        Add_Block("M09")

        Add_Block("G00 X0 Y0")
        Add_Block("M30")

        Return True
    End Function

    Private Sub Update_Fractional_Measurents()
        txt_Frame_Mat_Width_Fractional.Text = Decimal_To_Fraction(txt_Frame_Mat_Width.Text)
        txt_Frame_Rail_Width_Fractional.Text = Decimal_To_Fraction(txt_Frame_Rail_Width.Text)
        txt_Frame_Frame_Width_Fractional.Text = Decimal_To_Fraction(txt_Frame_Frame_Width.Text)
        txt_Frame_Rough_Cut_Fractional.Text = Decimal_To_Fraction(txt_Frame_Rough_Cut.Text)
    End Sub

    Private Sub Update_Decimal_Measurements()
        txt_Frame_Mat_Width.Text = Fraction_To_Decimal(txt_Frame_Mat_Width_Fractional.Text)
        txt_Frame_Rail_Width.Text = Fraction_To_Decimal(txt_Frame_Rail_Width_Fractional.Text)
        txt_Frame_Frame_Width.Text = Fraction_To_Decimal(txt_Frame_Frame_Width_Fractional.Text)
        txt_Frame_Rough_Cut.Text = Fraction_To_Decimal(txt_Frame_Rough_Cut_Fractional.Text)
    End Sub

    Private Sub txt_Frame_Mat_Width_Validated(sender As Object, e As EventArgs) Handles txt_Frame_Mat_Width.Validated
        Calculate_Frame_Dimensions(txt_Frame_Mat_Width)
    End Sub

    Private Sub txt_Frame_Rail_Width_Validated(sender As Object, e As EventArgs) Handles txt_Frame_Rail_Width.Validated
        Calculate_Frame_Dimensions(txt_Frame_Rail_Width)
    End Sub

    Private Sub txt_Frame_Width_Validated(sender As Object, e As EventArgs) Handles txt_Frame_Frame_Width.Validated
        Calculate_Frame_Dimensions(txt_Frame_Frame_Width)
    End Sub

    Private Sub txt_Frame_Mat_Width_Fractional_Validated(sender As Object, e As EventArgs) Handles txt_Frame_Mat_Width_Fractional.Validated
        Calculate_Frame_Dimensions(txt_Frame_Mat_Width_Fractional)
    End Sub

    Private Sub txt_Frame_Rail_Width_Fractional_Validated(sender As Object, e As EventArgs) Handles txt_Frame_Rail_Width_Fractional.Validated
        Calculate_Frame_Dimensions(txt_Frame_Rail_Width_Fractional)
    End Sub

    'Private Sub txt_Frame_Width_Fractional_Validated(sender As Object, e As EventArgs) Handles txt_Frame_Width_Fractional.Validated
    '    Calculate_Frame_Dimensions(txt_Frame_Width_Fractional)
    'End Sub

    Private Sub txt_Frame_KeyDown(sender As Object, e As System.Windows.Forms.KeyEventArgs) Handles _
        txt_Frame_Mat_Width.KeyDown, txt_Frame_Mat_Width_Fractional.KeyDown, _
        txt_Frame_Frame_Width.KeyDown, txt_Frame_Frame_Width_Fractional.KeyDown, _
        txt_Frame_Rail_Width.KeyDown, txt_Frame_Rail_Width_Fractional.KeyDown

        If e.KeyCode = Keys.Enter Then
            lab_Frame_Lower_Left.Focus()
            e.Handled = True
        End If
    End Sub

#End Region

#Region "Circle"

    Private Function Interpolate_Circle(Center_X As Single, Center_Y As Single, Radius As Single) As List(Of Coordinate)
        Dim Pt As Coordinate
        Dim Points As New List(Of Coordinate)
        Dim Cords As Single
        Dim Angle_Step As Single
        Dim Angle As Single
        Dim Circumference As Single = Math.PI * Radius * 2
        Cords = Circumference / 0.05 'Number of .05 inch cords
        Angle_Step = 360 / Cords

        If 360 / Angle_Step < 36 Then
            Angle_Step = 360 / 36
        End If

        Angle = 0

        While Angle <= 360
            Pt = New Coordinate
            Calc.Polar_To_Cartesian(Radius, Angle, Pt.X, Pt.Y)
            Pt.X += Center_X
            Pt.Y += Center_Y
            Points.Add(Pt)
            If Angle = 360 Then Exit While
            Angle += Angle_Step
            If Angle > 360 Then
                Angle = 360
            End If
        End While

        Return Points

    End Function

    Private Function Calculate_Circle() As Boolean
        Dim Center_X As Single = txt_Circle_Center_X.Text
        Dim Center_Y As Single = txt_Circle_Center_Y.Text
        Dim Diameter As Single = txt_Circle_Diameter.Text
        Dim Radius As Single
        Dim Tool_Diameter As Single = txt_Circle_Tool_Diameter.Text
        Dim Cut_Depth As Single = txt_Circle_Cut_Depth.Text
        Dim Pass_Depth As Single = txt_Circle_Pass_Depth.Text
        Dim Safe_Z As Single = txt_Circle_Safe_Z.Text
        Dim Feedrate As Single = txt_Circle_Feed_Rate.Text
        Dim Plunge_Rate As Single = txt_Circle_Plunge_Rate.Text
        Dim Start_Depth As Single = txt_Circle_Start_Depth.Text
        Dim Z As Single = Start_Depth

        Dim Points As List(Of Coordinate)

        Radius = (Diameter / 2) - (Tool_Diameter / 2)
        Points = Interpolate_Circle(Center_X, Center_Y, Radius)

        Add_Block("G00 Z" & Safe_Z)
        Add_Block("G00 X" & Points(0).X & " Y" & Points(0).Y)

        'Add_Block("M08 G04 P" & CNC.Coolant_Delay)
        'Add_Block("M03 G04 P" & CNC.Spindle_Delay)

        Add_Block("G00 Z" & Start_Depth + 0.1) 'Rapid down close to start depth
        Add_Block("G64") 'Continuous, ie. not exact path

        Do
            Z -= Pass_Depth
            If Z < -Cut_Depth Then
                Z = -Cut_Depth
            End If

            Add_Block("G01 Z" & Z & " F" & Plunge_Rate)
            Add_Block("G01 X" & Points(0).X & " Y" & Points(0).Y & " F" & Feedrate)

            For P = 1 To Points.Count - 1
                Add_Block("G01 X" & Points(P).X & " Y" & Points(P).Y)
            Next

        Loop Until Z = -Cut_Depth

        Add_Block("G00 Z" & Safe_Z)

        Return True

    End Function

#End Region

#Region "Translate"

    Private Trans_Buffer() As String

    Private Function Calculate_Translation() As Boolean
        Dim Block As String
        Dim Comment As String
        Dim pts As List(Of Class_Calc.struct_Coordinate)
        Dim X As Single
        Dim Y As Single
        Dim GC As New Class_GCode
        Dim Cord_Length As Single

        If File.Exists(txt_Translate_Input_File.Text) Then
            Trans_Buffer = File.ReadAllLines(txt_Translate_Input_File.Text)
        Else
            Show_Error("File not found.")
            Return False
        End If

        If txt_Translate_Cord_Length.Text = "" Then
            Cord_Length = 0
        Else
            If Not Get_Num(txt_Translate_Cord_Length, Cord_Length) Then Return False
        End If

        GCode.Clear()

        For B = 0 To Trans_Buffer.Count - 1
            GC.Block = Trans_Buffer(B)
            Block = GC.Formatted_Block

            If GC.G.Count > 0 Then
                If GC.G(0).Contains("03") Or GC.G(0).Contains("02") Then
                    Comment = " (" & Block & ")"
                    pts = Calc.Interpolate_Arc(Block, X, Y, Cord_Length)
                    For P = 0 To pts.Count - 1
                        Block = "G01 X" & pts(P).X & " Y" & pts(P).Y
                        'If P = 0 Then Block = Block & Comment
                        Add_Block(Block)
                    Next
                Else
                    Add_Block(Block)
                End If
            Else
                Add_Block(Block)
            End If

            If GC.X <> "" Then X = CSng(GC.X) 'Last X position
            If GC.Y <> "" Then Y = CSng(GC.Y) 'Last Y position

        Next

        Return True

    End Function

    Private Sub btn_Translate_Load_File_Click(sender As Object, e As EventArgs) Handles btn_Translate_Open_File.Click
        Dim dlg As New OpenFileDialog

        If txt_Translate_Input_File.Text = "" Then
            dlg.InitialDirectory = Chip_Wizard_G_Code_Folder
        Else
            dlg.InitialDirectory = Path.GetDirectoryName(txt_Translate_Input_File.Text)
        End If

        dlg.Filter = "Chip Files (*.chp)|*.chp|Other (*.ngc)|*.ngc|G Code Files (*.chp;*.ngs)|*.chp;*.ngc|All files (*.*)|*.*"
        If dlg.ShowDialog() = Windows.Forms.DialogResult.OK Then
            txt_Translate_Input_File.Text = dlg.FileName
        End If

    End Sub

#End Region

#Region "Arc"

    Dim Arc_Center_Pixel_X As Integer
    Dim Arc_Center_Pixel_Y As Integer
    Dim Arc_Radius_Pixel As Integer

    Private Sub Initialize_Arc()
        Arc_Center_Pixel_X = line_Arc_Start_Vector.X1
        Arc_Center_Pixel_Y = line_Arc_Start_Vector.Y1
        Arc_Radius_Pixel = line_Arc_Start_Vector.X2 - line_Arc_Start_Vector.X1
        Show_Angles()
    End Sub

    Private Sub Show_Angles()
        If IsNumeric(txt_Arc_Center_X.Text) And IsNumeric(txt_Arc_Center_Y.Text) And _
            IsNumeric(txt_Arc_Start_Angle.Text) And IsNumeric(txt_Arc_End_Angle.Text) And _
            IsNumeric(txt_Arc_Diameter.Text) Then

            txt_Arc_Radius.Text = FormatNumber(Calc.Format_Number(txt_Arc_Diameter.Text / 2))

            Dim End_PX As Single
            Dim End_PY As Single

            Calc.Polar_To_Cartesian(Arc_Radius_Pixel, -Calc.Round(txt_Arc_Start_Angle.Text), End_PX, End_PY)
            line_Arc_Start_Vector.X2 = CInt(End_PX + Arc_Center_Pixel_X)
            line_Arc_Start_Vector.Y2 = CInt(End_PY + Arc_Center_Pixel_Y)

            Calc.Polar_To_Cartesian(Arc_Radius_Pixel, -Calc.Round(txt_Arc_End_Angle.Text), End_PX, End_PY)
            line_Arc_End_Vector.X2 = CInt(End_PX + Arc_Center_Pixel_X)
            line_Arc_End_Vector.Y2 = CInt(End_PY + Arc_Center_Pixel_Y)
        End If

    End Sub

    Private Sub Calculate_Blue()
        Dim X As Single
        Dim Y As Single

        If cmb_Arc_Tool_Direction.Text = "CW" Then
            txt_Arc_Radius.Text = txt_Arc_Diameter.Text / 2

            Calc.Polar_To_Cartesian(txt_Arc_Radius.Text, txt_Arc_Start_Angle.Text, X, Y)
            txt_Arc_Start_X.Text = FormatNumber(X, 3)
            txt_Arc_Start_Y.Text = FormatNumber(Y, 3)

            Calc.Polar_To_Cartesian(txt_Arc_Radius.Text, txt_Arc_End_Angle.Text, X, Y)
            txt_Arc_End_X.Text = FormatNumber(X, 3)
            txt_Arc_End_Y.Text = FormatNumber(Y, 3)

            txt_Arc_I.Text = FormatNumber(txt_Arc_Center_X.Text - txt_Arc_Start_X.Text, 3)
            txt_Arc_J.Text = FormatNumber(txt_Arc_Center_Y.Text - txt_Arc_Start_Y.Text)

            txt_Arc_Block.Text = "G02 X" & FormatNumber(X, 3) & " Y" & FormatNumber(Y, 3) & " I" & txt_Arc_I.Text & " J" & txt_Arc_J.Text
        Else
            txt_Arc_Block.Text = "G03"
            Show_Error("Not implemented yet")
        End If

        Show_Angles()
    End Sub

    Private Sub Calculate_Yellow()
        Dim Start_X As Single = txt_Arc_Start_X.Text
        Dim Start_Y As Single = txt_Arc_Start_Y.Text
        Dim End_X As Single = txt_Arc_End_X.Text
        Dim End_Y As Single = txt_Arc_End_Y.Text
        Dim I As Single = txt_Arc_I.Text
        Dim J As Single = txt_Arc_J.Text
        Dim Center_X As Single
        Dim Center_Y As Single
        Dim Radius As Single
        Dim Distance As Single
        Dim Start_Angle As Single
        Dim End_Angle As Single

        If cmb_Arc_Tool_Direction.Text = "CW" Then
            Center_X = FormatNumber(Start_X + I)
            Center_Y = FormatNumber(Start_Y + J)

            Calc.Cartesian_To_Polar(Center_X, Center_Y, Start_X, Start_Y, Distance, Start_Angle)
            Calc.Cartesian_To_Polar(Center_X, Center_Y, End_X, End_Y, Distance, End_Angle)
            Radius = Distance / 2

            txt_Arc_Center_X.Text = FormatNumber(Center_X, 3)
            txt_Arc_Center_Y.Text = FormatNumber(Center_Y, 3)
            txt_Arc_Radius.Text = FormatNumber(Radius, 3)
            txt_Arc_Start_Angle.Text = FormatNumber(Start_Angle, 3)
            txt_Arc_End_Angle.Text = FormatNumber(End_Angle, 3)
            txt_Arc_Diameter.Text = FormatNumber(Distance, 3)

            txt_Arc_Block.Text = "G02 X" & FormatNumber(End_X, 3) & " Y" & FormatNumber(End_Y, 3) & " I" & FormatNumber(I, 3) & " J" & FormatNumber(J, 3)
        Else

            txt_Arc_Block.Text = "G03"
            Show_Error("Not implemented yet")
        End If
        Show_Angles()
    End Sub

    Private Function Calculate_Arc() As Boolean
        Dim GC As New Class_GCode
        GC.Block = txt_Arc_Block.Text
        Calc.Interpolate_Arc(GC.Formatted_Block, txt_Arc_Start_X.Text, txt_Arc_Start_Y.Text, 0)
        Return True
    End Function

    Private Sub txt_Arc_Tool_Diameter_TextChanged(sender As Object, e As System.ComponentModel.CancelEventArgs) Handles txt_Arc_Tool_Diameter.Validating

    End Sub

    Private Sub cmb_Arc_Tool_Position_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cmb_Arc_Tool_Position.SelectedIndexChanged
        If cmb_Arc_Tool_Position.SelectedItem = "Inside" Then
            lab_Arc_Tool_Inside.Visible = True
            lab_Arc_Tooi_Inside_Direction.Visible = True
            lab_Arc_Tool_Outside.Visible = False
            lab_Arc_Tooi_Outside_Direction.Visible = False
        Else
            lab_Arc_Tool_Inside.Visible = False
            lab_Arc_Tooi_Inside_Direction.Visible = False
            lab_Arc_Tool_Outside.Visible = True
            lab_Arc_Tooi_Outside_Direction.Visible = True
        End If

    End Sub

    Private Sub cmb_Arc_Tool_Direction_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cmb_Arc_Tool_Direction.SelectedIndexChanged
        If cmb_Arc_Tool_Direction.Text = "CW" Then
            lab_Arc_Tooi_Inside_Direction.Text = ">"
            lab_Arc_Tooi_Outside_Direction.Text = ">"
        Else
            lab_Arc_Tooi_Inside_Direction.Text = "<"
            lab_Arc_Tooi_Outside_Direction.Text = "<"
        End If
    End Sub

    Private Sub txt_Blue_Validating(sender As Object, e As System.ComponentModel.CancelEventArgs) Handles _
        txt_Arc_Start_X.Validating, txt_Arc_Start_Y.Validating, txt_Arc_End_X.Validating, txt_Arc_End_Y.Validating, _
        txt_Arc_I.Validating, txt_Arc_J.Validating
        Dim Name As String = sender.name
        Name = Name.Replace("txt_Arc_", "")
        Name = Name.Replace("_", "")
        If Not IsNumeric(sender.Text) Then
            Show_Error(Name & " is not a leagal number.")
            e.Cancel = True
        End If
        Calculate_Yellow()
    End Sub

    Private Sub txt_Yellow_Validating(sender As Object, e As System.ComponentModel.CancelEventArgs) Handles _
       txt_Arc_Center_X.Validating, txt_Arc_Center_Y.Validating, txt_Arc_Diameter.Validating, txt_Arc_End_Angle.Validating
        Dim Name As String = sender.name
        Name = Name.Replace("txt_Arc_", "")
        Name = Name.Replace("_", "")
        If Not IsNumeric(sender.Text) Then
            Show_Error(Name & " is not a leagal number.")
            e.Cancel = True
        End If
        Calculate_Blue()
    End Sub



#End Region


    Private Sub btn_Translate_Current_File_Click(sender As Object, e As EventArgs) Handles btn_Translate_Current_File.Click
        txt_Translate_Input_File.Text = ctl_GCode.txt_Operation.Text & "\" & ctl_GCode.txt_File_Name.Text
    End Sub


#Region "Feather"

    Private Sub btn_Feather_Open_File_Click(sender As Object, e As EventArgs) Handles btn_Feather_Open_File.Click
        Dim dlg As New OpenFileDialog

        If txt_Feather_Input_File.Text = "" Then
            dlg.InitialDirectory = Chip_Wizard_G_Code_Folder
        Else
            dlg.InitialDirectory = Path.GetDirectoryName(txt_Feather_Input_File.Text)
        End If

        dlg.Filter = "Chip Files (*.chp)|*.chp|Other (*.ngc)|*.ngc|G Code Files (*.chp;*.ngs)|*.chp;*.ngc|All files (*.*)|*.*"
        If dlg.ShowDialog() = Windows.Forms.DialogResult.OK Then
            txt_Feather_Input_File.Text = dlg.FileName
        End If

    End Sub

    Private Function Calculate_Feather() As Boolean
        Dim Block As String
        Dim X As Single
        Dim X_Max As Single
        Dim X_Min As Single
        Dim Y As Single
        Dim Y_Max As Single
        Dim Y_Min As Single
        Dim Z As Single
        Dim Z_Min As Single
        Dim Z_Max As Single
        Dim Z_Range As Single
        Dim Ratio As Single
        Dim Start_Block As Integer = -1
        Dim End_Block As Integer = -1
        Dim G_Code_List As New List(Of Class_GCode)
        Dim Home_Position As String = ""


        If Not Get_Num(txt_Feather_Min_Z, Z_Min) Then Return False
        If Not Get_Num(txt_Feather_Max_Z, Z_Max) Then Return False
        Z_Range = Z_Max - Z_Min

        If File.Exists(txt_Feather_Input_File.Text) Then
            Trans_Buffer = File.ReadAllLines(txt_Feather_Input_File.Text)
        Else
            Show_Error("File not found.")
            Return False
        End If

        For B = 0 To Trans_Buffer.Count - 1
            Dim BK As New Class_GCode
            BK.Block = Trans_Buffer(B)

            If Home_Position = "" Then
                If BK.G_Contains(0) And (BK.Z <> "") Then
                    Home_Position = BK.Block
                End If
            End If

            If (BK.Z <> "") And (BK.Block <> Home_Position) Then
                If BK.Z > 0 Then 'Move above material
                    If Start_Block >= 0 Then
                        G_Code_List(Start_Block).X_Min = X_Min
                        G_Code_List(Start_Block).Y_Min = Y_Min
                        G_Code_List(Start_Block).X_Range = FormatNumber(X_Max - X_Min, 3) 'X range
                        G_Code_List(Start_Block).Y_Range = FormatNumber(Y_Max - Y_Min, 3) 'Y range
                    End If
                    X_Min = Single.MaxValue
                    X_Max = Single.MinValue
                    Y_Min = Single.MaxValue
                    Y_Max = Single.MinValue
                Else
                    Start_Block = B
                End If
            End If

            If BK.M.Count = 0 Then
                If BK.X <> "" Then
                    If BK.X > X_Max Then X_Max = BK.X
                    If BK.X < X_Min Then X_Min = BK.X
                End If

                If BK.Y <> "" Then
                    If BK.Y > Y_Max Then Y_Max = BK.Y
                    If BK.Y < Y_Min Then Y_Min = BK.Y
                End If
            End If

            G_Code_List.Add(BK)
        Next

        Start_Block = -1
        End_Block = -1

        For B = 0 To G_Code_List.Count - 1

            If G_Code_List(B).G_Contains(1) Then

                If G_Code_List(B).Z <> "" Then

                    If G_Code_List(B).Z < 0 Then

                        Start_Block = B

                        If rad_Feather_X.Checked Then
                            If G_Code_List(Start_Block).X_Range = 0 Then
                                Ratio = 0
                            Else
                                Ratio = X - G_Code_List(Start_Block).X_Min
                                Ratio = Ratio / G_Code_List(Start_Block).X_Range
                            End If
                        Else
                            If G_Code_List(Start_Block).X_Range = 0 Then
                                Ratio = 0
                            Else
                                Ratio = Y - G_Code_List(B).Y_Min
                                Ratio = Ratio / G_Code_List(Start_Block).Y_Range
                            End If
                        End If

                        Z = (Z_Range * Ratio) + Z_Min
                        If Z < Z_Min Then Z = Z_Min
                        If Z > Z_Max Then Z = Z_Max
                        G_Code_List(B).Block = " G01 Z-" & FormatNumber(Z, 3) & " F" & G_Code_List(B).F

                    End If

                Else
                    Ratio = Single.NaN
                    If rad_Feather_X.Checked Then

                        If G_Code_List(B).X <> "" Then
                            Ratio = G_Code_List(B).X - G_Code_List(Start_Block).X_Min
                            Ratio = Ratio / G_Code_List(Start_Block).X_Range
                        End If
                    Else
                        If G_Code_List(B).Y <> "" Then
                            Ratio = G_Code_List(B).Y - G_Code_List(Start_Block).Y_Min
                            Ratio = Ratio / G_Code_List(Start_Block).Y_Range
                        End If
                    End If

                    If Not Single.IsNaN(Ratio) Then
                        Z = (Z_Range * Ratio) + Z_Min
                        If Z < Z_Min Then Z = Z_Min
                        If Z > Z_Max Then Z = Z_Max

                        Block = " G01"
                        If G_Code_List(B).X <> "" Then
                            Block &= " X" & FormatNumber(G_Code_List(B).X, 3)
                        End If
                        If G_Code_List(B).Y <> "" Then
                            Block &= " Y" & FormatNumber(G_Code_List(B).Y, 3)
                        End If

                        Block &= " Z-" & FormatNumber(Z, 3)

                        If G_Code_List(B).F <> "" Then
                            Block &= " F" & FormatNumber(G_Code_List(B).F, 3)
                        End If

                        G_Code_List(B).Block = Block
                    End If

                End If

            End If

            If G_Code_List(B).X <> "" Then
                X = G_Code_List(B).X
            End If

            If G_Code_List(B).Y <> "" Then
                Y = G_Code_List(B).Y
            End If

        Next

        GCode.Clear()
        For B = 0 To G_Code_List.Count - 1
            Block = G_Code_List(B).Block
            Add_Block(Block)
        Next

        Return True

    End Function

#End Region
   
End Class

