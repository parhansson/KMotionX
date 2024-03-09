Imports System.IO

Public Class Chip_Gcode

#Region "enums"

    Public Enum enum_Grid_Colums
        Pause
        Status
        Line_Number
        Block
        Comment
        Point
    End Enum

    Public Enum enum_State
        Idle
        Stopped
        Stepping
        Running
        Done
    End Enum

    Public Enum enum_Backup_Command
        No_Backup
        Backup
        Backup_To_Subdirectory
    End Enum

    Public Enum enum_Operation_Mode
        Manual
        SemiAuto
        Auto
    End Enum

#End Region

#Region "Structures"

    Private Structure struct_Pause
        Public In_Progress As Boolean
        Public Pause_Start_Time As Long
        Public Pause_Elapsed_Time As Long
    End Structure

#End Region

#Region "Declarations"

    Private Z_Color As Color = Color.PaleGreen

    Public State As enum_State
    Public All_Blocks_Sent As Boolean = False

    Public Operation_Mode As enum_Operation_Mode

    Private Value_Before_Edit As String
    Private Edits_Made As Boolean = False

    Public G_Code_File_Path As String = ""
    Public Routing_File_Path As String = ""
    Public Operation As String = ""
    Public Tool As String = ""
    Public Speed As String = ""
    Public Current_Tool As String
    Public Current_Speed As String
    Public Max_Z_Depth As Single

    Private Ignore_Cell_Event As Boolean = True
    Private Ignore_Grid_Event As Boolean = False

    Private Pause As struct_Pause

    Private Initialized As Boolean = False

    Private Sent_Blocks As Integer = 0

    Private Stop_Watch As New Stopwatch
    Private Time_Shadow As Integer = 0

    Private Turn_On_Spindle As Boolean = False

    Private Last_Sent_Line As Integer = 0
    Private Last_Block_Line As Integer = 0

    Private Drawing_Control As Chip_Drawing

#End Region

    Private Sub Chip_Gcode_Load(sender As Object, e As EventArgs) Handles Me.Load
        txt_Information.Height = 100
    End Sub


#Region "Initialize, Setup, Apply_Settings, Finish"

    Public Sub Initialize()
        Ignore_Cell_Event = True
        grid.Font = GCode_Grid_Font
        ts_Manual.Visible = True
        ts_Auto.Visible = False
        Initialized = True
    End Sub

    Public Sub Setup(ctrl_Drawing As Chip_Drawing)
        Drawing_Control = ctrl_Drawing

        State = enum_State.Stopped

        Ignore_Cell_Event = True

        ts_Manual.Visible = True
        ts_Auto.Visible = False

        ts_Single_Step.Visible = False
        ts_Continuous.Visible = True

        txt_Information.Left = ts_G_Code.Left + ts_G_Code.Width
        ts_Continuous.Visible = True

        Ignore_Cell_Event = True
        Move_Cursor(0)
        Ignore_Cell_Event = False

    End Sub

    Public Sub Finish()

        grid.EndEdit()

        If Edits_Made Then
            Message_Box.ShowDialog("G Code changed." & vbCrLf &
                                G_Code_File_Path & vbCrLf & "Save file?", "", MessageBoxButtons.YesNo)
            If Message_Box.DialogResult = DialogResult.Yes Then
                Save_G_Code(G_Code_File_Path, enum_Backup_Command.Backup_To_Subdirectory)
            End If
        End If

    End Sub

#End Region


#Region "Properties"

    Public Property Information_Window_Height As Integer
        Get
            Return txt_Information.Height
        End Get
        Set(ByVal value As Integer)
            txt_Information.Height = value
        End Set
    End Property

#End Region

#Region "Interface to main program"

    Public Function Current_Block(Line_Number As Integer) As String
        Return grid.Rows(Line_Number).Cells(enum_Grid_Colums.Block).Value
    End Function

    Public Sub Update_Sent_Line_Number(Line_Number As Integer)
        If Last_Sent_Line < Line_Number Then
            For I = Last_Sent_Line + 1 To Line_Number
                grid.Rows(I).Cells(enum_Grid_Colums.Status).Value = "*"
            Next
        End If
        Last_Sent_Line = Line_Number
    End Sub

    Public Sub Update_Line_Number(Line_Number As Integer)

        If Last_Block_Line < Line_Number Then
            Move_Cursor(Line_Number)
            For I = Last_Block_Line To Line_Number - 1
                grid.Rows(I).Cells(enum_Grid_Colums.Status).Value = "|"
            Next
            grid.Rows(Line_Number).Cells(enum_Grid_Colums.Status).Value = ">"
            Last_Block_Line = Line_Number

            If grid.Rows(Line_Number).Cells(enum_Grid_Colums.Pause).Value = "#" Then
                Kflop.Feed_Hold = True
            End If

            stat_Last_Block.Text = Line_Number

            If Stop_Watch.ElapsedMilliseconds > Time_Shadow + 1000 Then
                Time_Shadow = Stop_Watch.ElapsedMilliseconds
                Dim ts As TimeSpan = Stop_Watch.Elapsed

                Dim T_Elapsed As String = ts.ToString("hh\:mm\:ss")
                Dim Elapsed_Secs As Integer = Get_Seconds(T_Elapsed)

                Dim T_Total As String = Main_Form.ctl_Build.GCode_Execution_Time
                Dim Total_Secs As Integer = Main_Form.ctl_Build.GCode_Execution_Seconds



                If Total_Secs > 0 Then
                    Stat_Time.Text = T_Elapsed & " of " & T_Total & " : " & CInt((Elapsed_Secs / Total_Secs) * 100) & "%"
                Else
                    Stat_Time.Text = T_Elapsed & " of " & T_Total & " : 0%"
                End If

                If Elapsed_Secs < Stat_Time_Progress.Maximum Then
                    Stat_Time_Progress.Value = Elapsed_Secs
                Else
                    Stat_Time_Progress.Maximum = Elapsed_Secs + 1
                    Stat_Time_Progress.Value = Elapsed_Secs
                End If

                Stat_Blocks.Text = grid.CurrentRow.Index + 1 & " of " & grid.RowCount - 1 & " : " & CInt(grid.CurrentRow.Index / (grid.RowCount - 1) * 100) & "%"

                Stat_Block_Progress.Value = CInt(grid.CurrentRow.Index / (grid.RowCount - 1) * 100)  'grid.CurrentRow.Index - 1
            End If

        End If
    End Sub

    Public Sub Rewind()
        State = enum_State.Stopped
        Last_Sent_Line = 0
        Last_Block_Line = 0


        For R = 0 To grid.RowCount - 1
            grid.Rows(R).Cells(enum_Grid_Colums.Status).Value = ""
        Next
        Move_Cursor(0)
        grid.Rows(0).Cells(enum_Grid_Colums.Status).Value = ">"

        If Not IsNothing(Drawing_Control) Then
            Drawing_Control.Part_Drawing.Start_Drawing()
            Drawing_Control.Repaint()
        End If

    End Sub

    Public Function Start_Execution() As Boolean
        grid.EndEdit()

        If grid.CurrentRow.Index <> 0 Then
            Dim Mess As String = ""

            If Main_Form.btn_Spindle.State <> "On" Then
                Mess = "Spindle is not on." & vbCrLf
            End If

            If Main_Form.btn_Coolant.State <> "On" Then
                Mess &= "Coolant is not on."
            End If

            If Mess <> "" Then
                Dim dlg As New dlg_Message_Box
                dlg.ShowDialog(Mess, "Warning")
                Return False
            End If
        End If

        Stop_Watch.Reset()
        Time_Shadow = 0
        Stat_Blocks.Text = grid.RowCount - 1 & " of " & grid.CurrentRow.Index - 1 & " : 100%"
        Stat_Block_Progress.Value = 0
        Dim T As String = ""
        Dim S As String = ""
        Main_Form.ctl_Build.Get_Execution_Time(T, s)
        Stat_Time.Text = "0  of " & T & " : 0%"
        Stat_Time_Progress.Value = 0
        Stop_Watch.Start()
        Return True
    End Function

    Public Sub Execution_Complete()
        Stop_Watch.Stop()

        Dim R As Integer = grid.CurrentRow.Index
        Dim Rs As Integer = grid.Rows.Count

        If Kflop.Run_Mode = Class_KFLOP.enum_Run_Mode.Continuous Then
            For I = grid.CurrentRow.Index To grid.Rows.Count - 2
                Trace_Control.Log_Trace_Message(String.Format("[Block] N{0}  {1}", I + 1, Current_Block(I)) & vbCrLf)
            Next
        End If

        If Kflop.Run_Mode = Class_KFLOP.enum_Run_Mode.Continuous Then
            Rewind()
        Else
            If Kflop.If_Is_Active_M_Code(2) Then
                Rewind()
            End If
        End If

        Stat_Blocks.Text = grid.RowCount - 1 & " of " & grid.RowCount - 1 & " : 100%"
        Stat_Block_Progress.Value = 0

        Main_Form.ctl_Build.Set_Execution_Time(Stop_Watch.Elapsed)

        Stat_Time_Progress.Value = 0

    End Sub

#End Region

#Region "Toolstrip"

    Private Sub ts_G_Code_ItemClicked(sender As Object, e As ToolStripItemClickedEventArgs) Handles ts_G_Code.ItemClicked

        Select Case e.ClickedItem.Text

            Case "Manual"
                ts_Manual.Visible = False
                ts_Auto.Visible = True

            Case "Auto"
                ts_Manual.Visible = True
                ts_Auto.Visible = False

            Case "Load"
                Dim Df As New Class_Data_File
                Dim File As String = Df.Select_Input_File(G_Code_File_Path, "G Code Files (*.chp)|*.chp|G Code Files (*.chp)|*.chp|All files (*.*)|*.*")
                If File <> "" Then
                    Load_G_Code(File, True)
                End If

                Show_Error("*******************************************" & vbCrLf &
                           "**** Offsets are probably Not correct. ****" & vbCrLf &
                           "*******************************************")

            Case "Save"
                Save_G_Code(G_Code_File_Path, enum_Backup_Command.Backup_To_Subdirectory)

            Case "Save As"
                Dim dlg As New SaveFileDialog
                dlg.InitialDirectory = Path.GetDirectoryName(G_Code_File_Path)
                dlg.FileName = Path.GetFileName(G_Code_File_Path)

                dlg.Filter = "Chip Files (*.chp)|*.chp|Other (*.ngc)|*.ngc|G Code Files (*.chp;*.ngs)|*.chp;*.ngc|All files (*.*)|*.*"
                If dlg.ShowDialog() = Windows.Forms.DialogResult.OK Then
                    Save_G_Code(dlg.FileName, enum_Backup_Command.No_Backup)
                    If Not IsNothing(Drawing_Control) Then
                        Drawing_Control.Reset()
                    End If
                End If

            Case "Continuous"
                ts_Continuous.Visible = False
                ts_Single_Step.Visible = True
                Kflop.Run_Mode = Class_KFLOP.enum_Run_Mode.Single_Step

            Case "Single Step"
                ts_Single_Step.Visible = False
                ts_Continuous.Visible = True
                Kflop.Run_Mode = Class_KFLOP.enum_Run_Mode.Continuous

            Case "Rewind"
                Rewind()

            Case "Start Here"
                Start_Here(grid.CurrentRow.Index)

            Case "Insert"
                Dim P(3) As String
                For I = 0 To 3
                    P(I) = ""
                Next
                grid.Rows.Insert(grid.CurrentRow.Index, P)
                Renumber()

            Case "Delete"
                If Not IsNothing(grid.CurrentRow) Then
                    grid.Rows.RemoveAt(grid.CurrentRow.Index)
                End If
                Renumber()

        End Select

    End Sub

#End Region

    Public Sub Start_Here(Row_Index)
        Dim S As String

        S = grid.CurrentRow.Cells(enum_Grid_Colums.Block).Value

        If (Not (S.Contains("X"))) And (Not (S.Contains("Y"))) And S.Contains("Z") Then
            For R = 0 To grid.Rows.Count - 1
                If R < Row_Index Then grid.Rows(R).Cells(enum_Grid_Colums.Status).Value = "|"
                If R = Row_Index Then grid.Rows(R).Cells(enum_Grid_Colums.Status).Value = ">"
                If R > Row_Index Then grid.Rows(R).Cells(enum_Grid_Colums.Status).Value = ""
            Next
            Move_Cursor(Row_Index)
            grid.Rows(Row_Index).Cells(enum_Grid_Colums.Status).Value = ">"
            KFlop.Start_Here_Line = Row_Index
            Last_Block_Line = KFlop.Start_Here_Line - 1
        Else
            Show_Error("May not be safe to start here." & vbCrLf & "Block does not move Z to safe place.")
        End If

    End Sub

#Region "Grid"

    Private Sub grid_CellClick(sender As Object, e As System.Windows.Forms.DataGridViewCellMouseEventArgs) Handles grid.CellMouseDown
        If e.ColumnIndex < 0 Then Exit Sub
        If e.RowIndex < 0 Then Exit Sub

        If e.RowIndex >= 0 Then
            Dim B As String = grid.Rows(e.RowIndex).Cells(enum_Grid_Colums.Line_Number).Value
            If Not IsNothing(B) Then
                B = B.Replace("N", "")
                Drawing_Control.Part_Drawing.Position_Block = B
                Drawing_Control.Part_Drawing.Repaint()
            End If
        End If

    End Sub

    Private Sub grid_CellEnter(sender As Object, e As System.Windows.Forms.DataGridViewCellEventArgs) Handles grid.CellEnter
        If Not Initialized Then Exit Sub
        If Ignore_Cell_Event Then Exit Sub
        If e.ColumnIndex = enum_Grid_Colums.Block Then
            Value_Before_Edit = grid.Rows(e.RowIndex).Cells(e.ColumnIndex).Value
        End If
        If e.RowIndex >= 0 Then
            Dim B As String = grid.Rows(e.RowIndex).Cells(enum_Grid_Colums.Line_Number).Value
            If Not IsNothing(B) Then
                B = B.Replace("N", "")
                Drawing_Control.Part_Drawing.Position_Block = B
                Drawing_Control.Part_Drawing.Repaint()
            End If
        End If
    End Sub

    Private Sub grid_CellBeginEdit(sender As Object, e As System.Windows.Forms.DataGridViewCellCancelEventArgs) Handles grid.CellBeginEdit
        Edits_Made = True
    End Sub

    Private Sub grid_CellValidated(sender As Object, e As System.Windows.Forms.DataGridViewCellEventArgs) Handles grid.CellValidated
        If Ignore_Cell_Event Then Exit Sub
        If Not Initialized Then Exit Sub
        If e.ColumnIndex = enum_Grid_Colums.Block Then
            Dim Blk As New Class_GCode
            Blk.Block = grid.CurrentCell.Value

            If grid.Rows(e.RowIndex).Cells(e.ColumnIndex).Value <> Value_Before_Edit Then
                Edits_Made = True
                Save_G_Code(G_Code_File_Path, enum_Backup_Command.No_Backup)
            End If

            grid.CurrentCell.Value = Blk.Formatted_Block
            Renumber()
            Refresh_Drawing()
        End If

    End Sub

    Private Sub grid_Rows_Added(sender As Object, e As System.Windows.Forms.DataGridViewRowsAddedEventArgs) Handles grid.RowsAdded
        If Ignore_Grid_Event Then Exit Sub
        If Not Initialized Then Exit Sub
        Refresh_Drawing()
    End Sub

    Private Sub grid_Rows_Removed(sender As Object, e As System.Windows.Forms.DataGridViewRowsRemovedEventArgs) Handles grid.RowsRemoved
        If Ignore_Grid_Event Then Exit Sub
        If Not Initialized Then Exit Sub
        Refresh_Drawing()
    End Sub

    Public Sub Clear_Rows()
        Ignore_Grid_Event = True
        grid.Rows.Clear()
        Refresh_Drawing()
        Ignore_Grid_Event = False
    End Sub

    Public Sub Insert_Row(Optional Block As String = "")
        Ignore_Grid_Event = True
        Dim Cells(4) As String
        Cells(1) = ""
        Cells(2) = ""
        Cells(3) = Block
        Cells(4) = ""
        If IsNothing(grid.CurrentRow) Then
            grid.Rows.Add(Cells)
            Exit Sub
        End If
        Dim Idx As Integer = grid.CurrentRow.Index
        grid.Rows.Insert(Idx, Cells)
        Renumber()
        Refresh_Drawing()
        Ignore_Grid_Event = False
    End Sub

    Public Sub Delete_Row()
        Ignore_Grid_Event = True
        Dim Idx As Integer = grid.CurrentRow.Index
        grid.Rows.RemoveAt(Idx)
        Renumber()
        Refresh_Drawing()
        Ignore_Grid_Event = False
    End Sub

    Public Sub Collapse()
        Dim Block As String
        For I = 0 To grid.RowCount - 1
            Block = grid.Rows(I).Cells(enum_Grid_Colums.Block).Value
            If Not IsNothing(Block) Then
                If Not Mid(Block, 1, 1) = "(" Then
                    grid.Rows(I).Visible = False
                End If
            End If
        Next
    End Sub

    Public Sub Expand()
        For I = 0 To grid.RowCount - 1
            grid.Rows(I).Visible = True
        Next
    End Sub

    Private Sub Move_Cursor(Row As Integer)
        If grid.RowCount < 0 Then Exit Sub
        If IsNothing(grid.CurrentRow) Then Exit Sub
        If grid.CurrentRow.Index = Row Then Exit Sub
        Ignore_Cell_Event = True
        If Row < grid.RowCount Then
            grid.CurrentCell = grid.Rows(Row).Cells(0)
        End If
        Ignore_Cell_Event = False
    End Sub

#End Region

    Public Function Check_GCode() As Boolean
        Dim Block As String
        Dim M03_Found As Boolean = False

        For I = 0 To grid.RowCount - 1
            Block = grid.Rows(I).Cells(enum_Grid_Colums.Block).Value
            If Not IsNothing(Block) Then
                If Block.Contains("M03") Then
                    M03_Found = True
                End If
            End If
        Next

        If Not M03_Found Then
            Message_Box.ShowDialog("M03 (Spindle on) was Not detected." & vbCrLf & "Do you still want to exit?", "Warning", MessageBoxButtons.YesNo)
            If Message_Box.DialogResult = DialogResult.Yes Then
                Return True
            Else
                Return False
            End If
        End If

        Return True

    End Function

    Public Sub Refresh_Drawing()
        If Not IsNothing(Drawing_Control) Then
            Drawing_Control.Repaint()

            '            Drawing_Control.Part_Drawing.Load_Drawing_Points(Me)
            '           Drawing_Control.Part_Drawing.Draw_Cutter_Path()
        End If
    End Sub

    Public Sub Set_Grid_Point_Cell(ByRef Row As Integer, Point_Index As Integer)
        Ignore_Cell_Event = True
        grid.Rows(Row).Cells(enum_Grid_Colums.Point).Value = Point_Index
        Ignore_Cell_Event = False
    End Sub

    Private Sub Renumber()
        Dim S As String
        Dim Z As Double
        Dim G_Code As New Class_GCode


        Ignore_Cell_Event = True
        For R = 0 To grid.Rows.Count - 2
            grid.Rows(R).Cells(enum_Grid_Colums.Line_Number).Value = "N" & Trim(Str(R + 1))
            S = grid.Rows(R).Cells(enum_Grid_Colums.Block).Value
            If Not IsNothing(S) Then

                If (Not (S.Contains("X"))) And (Not (S.Contains("Y"))) And S.Contains("Z") Then
                    G_Code.Block = S
                    Z = G_Code.Get_Value("Z")
                    If Z > 0 Then
                        grid.Rows(R).Cells(enum_Grid_Colums.Block).Style.BackColor = Z_Color
                    Else
                        grid.Rows(R).Cells(enum_Grid_Colums.Block).Style.BackColor = Color.White
                    End If
                End If
            End If
        Next
        Ignore_Cell_Event = False
    End Sub

#Region "Load/Save file"

    Private Sub Add_Row(Block As Class_GCode, Optional Insert As Boolean = False)
        Dim Cells(4) As String
        Dim T() As String = Nothing

        For I = 0 To Cells.Count - 1
            Cells(I) = ""
        Next

        If Block.Original_Block.Contains("%") Then Exit Sub 'Filter out % blocks

        If Block.Comment = "END" Then
            Block.Comment = "(END)"
        End If

        Cells(enum_Grid_Colums.Line_Number) = "N" & Trim(grid.RowCount)
        Cells(enum_Grid_Colums.Block) = Block.Formatted_Block
        Cells(enum_Grid_Colums.Comment) = Block.Comment

        If Insert Then
            If Not IsNothing(grid.CurrentRow) Then
                Dim Idx As Integer = grid.CurrentRow.Index
                grid.Rows.Insert(Idx, Cells)
            Else
                Show_Error("No row selected, click on grid")
            End If
        End If

        grid.Rows.Add(Cells)

    End Sub

    Private Sub Show_Info()
        txt_Information.Text = "File : " & Path.GetFileName(G_Code_File_Path) & vbCrLf

        If Routing_File_Path <> "" Then
            txt_Information.Text &= "Route : " & Path.GetFileName(Routing_File_Path) &
                                    "    Operation : " & Operation & vbCrLf
            txt_Information.Text &= "Route Folder : " & Path.GetDirectoryName(Routing_File_Path) & vbCrLf
        End If
        txt_Information.Text &= "G_Code Folder : " & Path.GetDirectoryName(G_Code_File_Path)

        txt_Information.Text &= vbCrLf
        If Max_Z_Depth > 0 Then
            txt_Information.Text &= "Max Z Depth : +" & Max_Z_Depth
        Else
            txt_Information.Text &= "Max Z Depth : " & Max_Z_Depth
        End If

    End Sub


    Public Function Program_Loaded() As Boolean
        If grid.Rows.Count > 1 Then
            Return True
        End If
        Return False
    End Function

    Public Function Load_G_Code(File_Name As String, Optional Show_Warning As Boolean = False) As Boolean
        Dim G_Code_Block As Class_GCode
        Dim Buff() As String
        Dim Cells(3) As String
        Dim Ignore_Warning As Boolean = False
        Dim Load_OK As Boolean = True
        Dim Save As Boolean = False
        Dim Tool As String = ""
        Dim Speed As String = ""
        Dim Directions As String = ""
        Dim Min_Z As Single = 6

        Ignore_Cell_Event = True
        Ignore_Grid_Event = True
        Edits_Made = False

        If File_Name = "" Then
            Return False
        End If

        If Not File.Exists(File_Name) Then
            If Show_Warning Then
                Show_Error("File Not found : " & File_Name)
            End If
            Return False
        End If

        Buff = File.ReadAllLines(File_Name)
        Max_Z_Depth = 0

        Dim dlg As New dlg_Progress
        dlg.bar.Minimum = 0
        dlg.bar.Maximum = Buff.Count
        dlg.bar.Step = 1
        dlg.Show("Loading : " & File_Name)

        grid.Rows.Clear()

        If Not IsNothing(Drawing_Control) Then Drawing_Control.Part_Drawing.Clear_Drawing()

        For R = 0 To Buff.Count - 1
            dlg.Update_Progress()

            If Mid(Buff(R), 1, 1) <> "'" Then

                G_Code_Block = New Class_GCode
                G_Code_Block.Block = Buff(R)
                If G_Code_Block.Block.Contains("Z") Then
                    Dim S() As String = G_Code_Block.Block.Split(" ")

                    For I = 0 To S.Count - 1
                        If S(I).Contains("Z") Then
                            S(I) = S(I).Replace("Z", "")
                            If S(I) < Min_Z Then
                                Min_Z = S(I)
                            End If
                        End If
                    Next
                End If

                If R = Buff.Count - 1 Then
                    If G_Code_Block.Block <> "" Then
                        Add_Row(G_Code_Block)
                    End If
                Else
                    Add_Row(G_Code_Block)
                End If

            End If
Skip:
        Next

        Max_Z_Depth = Min_Z + Main_Form.ctl_Build.Z_Part_Offset

        Renumber()

        Ignore_Cell_Event = True
        If grid.Rows.Count > 0 Then
            grid.Rows(0).Cells(enum_Grid_Colums.Status).Value = ">"
            grid.CurrentCell = grid.Rows(0).Cells(enum_Grid_Colums.Line_Number)
        End If

        Ignore_Cell_Event = True
        If Not IsNothing(Drawing_Control) Then
            Drawing_Control.Part_Drawing.Load_Drawing_Points(Me)
        End If

        dlg.Close()

        Tool = Main_Form.ctl_Build.Get_Tool_Name
        If Tool <> Current_Tool Then
            Message_Box.ShowDialog("Change Tool to : " & Tool)
        End If

        Directions = Main_Form.ctl_Build.Get_Directions
        If Directions <> "" Then
            Message_Box.ShowDialog("Directions : " & vbCrLf & vbCrLf & Directions)
        End If

        Current_Tool = Tool
        Current_Speed = Speed

        Ignore_Cell_Event = False
        Ignore_Grid_Event = False

        Reset_GCode()

        Stat_Blocks.Text = grid.RowCount - 1 & " of " & grid.RowCount - 1 & " : 100%"
        Stat_Block_Progress.Value = 0

        Dim T As String = Main_Form.ctl_Build.GCode_Execution_Time
        Stat_Time.Text = "0  of " & T & " : 0%"

        Stat_Time_Progress.Value = 0

        G_Code_File_Path = File_Name
        Kflop.Interpreter_File_Name = File_Name
        Drawing_Control.Part_Drawing.Start_Drawing()

        Main_Form.track_Feed_Rate_Override.Value = 100

        Main_Form.track_Spindle_Speed_Override.Value = 100
        Main_Form.ctl_C_Interface.Set_Spindle_Speed(0)

        Show_Info()

        Me.Cursor = Cursors.Arrow

          Return Load_OK

    End Function

    Public Sub Reset_GCode()

        If Not IsNothing(Drawing_Control) Then
            Drawing_Control.Reset()
        End If

    End Sub

    Public Sub Save_G_Code(File_Name As String, Backup_Command As enum_Backup_Command)
        Dim Buff As New List(Of String)
        Dim S As String

        'If Routings.Current_Part.File_Path = "" Then
        '    Dim ext As String = Symbol.Get_Value("Sys.GCode_File_Extension")
        '    If ext = "" Then ext = ".Txt"
        '    Dim dlg As New SaveFileDialog
        '    dlg.InitialDirectory = Chip_Macro_Program_Folder
        '    dlg.FileName = Path.GetFileName(Routings.Current_Part.File_Path)
        '    dlg.Filter = "GCode Files (*." & ext & ")|*." & ext & "|All files (*.*)|*.*"
        '    If dlg.ShowDialog = Windows.Forms.DialogResult.OK Then
        '        Routings.Set_Part_File_Path(dlg.FileName)
        '        Symbol.Set_Value(Control_Symbol.Name, Routings.Current_Part.File_Path)
        '    Else
        '        Exit Sub
        '    End If
        'Else
        '    Select Case Backup_Command
        '        Case enum_Backup_Command.No_Backup
        '        Case enum_Backup_Command.Backup
        '            Dim DF As New Class_Data_File
        '            DF.Backup_File(Routings.Current_Part.Folder)
        '        Case enum_Backup_Command.Backup_To_Subdirectory
        '            Dim DF As New Class_Data_File
        '            DF.Backup_File(Routings.Current_Part.File_Name, True)
        '    End Select
        'End If

        grid.EndEdit()

        'Add header information if any
        'If Not IsNothing(GCode_File_Header) Then
        '    For I = 0 To GCode_File_Header.Count - 1
        '        Buff.Add(GCode_File_Header(I))
        '    Next
        'End If

        'Add blocks
        For R = 0 To grid.RowCount - 1
            S = grid.Rows(R).Cells(enum_Grid_Colums.Block).Value
            If grid.Rows(R).Cells(enum_Grid_Colums.Comment).Value <> "" Then
                S &= ";" & grid.Rows(R).Cells(enum_Grid_Colums.Comment).Value
            End If
            If (Not IsNothing(S)) And S <> "" Then Buff.Add(S)
        Next

        'File.WriteAllLines(Routings.Current_Part.File_Path, Buff)

        File.WriteAllLines(File_Name, Buff)

        Edits_Made = False

    End Sub

#End Region


    Private Sub stat_lab_Last_Block_Click(sender As Object, e As EventArgs) Handles stat_lab_Last_Block.Click
        Dim blk As Integer = CInt(stat_Last_Block.Text)
        Move_Cursor(blk)
    End Sub

    Private Sub stat_Go_To_Click(sender As Object, e As EventArgs) Handles stat_Go_To.Click
        Dim blk As Integer = CInt(stat_Last_Block.Text)
        If blk > 0 Then Move_Cursor(blk - 1)
    End Sub

End Class
