Imports System.IO

Public Class ctr_Macro_Editor

    Private Enum enum_Grid_Columns
        Line
        Block
        Comment
        Error_Message
    End Enum

    Private Enum enum_Program_Cursor_Commands
        Clear_All
        Clear
        Show
    End Enum

#Region "Declarations"

    Public File_Name As String = ""
    Public Macro As String = ""
    Private Running As Boolean = False
    Private Cell_Original_Text As String
    Private Edits_Made As Boolean = False
    Private Initialized As Boolean = False
    Private Ignore_Cell_Enter_Event As Boolean = False

#End Region


#Region "Initalize, FInish"

    Public Sub Initialize(Optional File_Name As String = "")
        Exit Sub

        pnl_Editor.Width = Settings.Get_Setting(Me.Name, "Width", 200, "Integer", True)

        File_Name = "C:\Chip_Files\Macros\Test_Macro.txt"

        If File_Name <> "" Then
            G_Code.Load_G_Code(File_Name)
        Else
            G_Code.Load_G_Code_From_String(Macro)
            ts_Save.Visible = False
            ts_Open.Visible = False
        End If

        Compile()

        Initialized = True

    End Sub

    Public Sub Finish()
        Settings.Put_Setting(Me.Name, "Width", pnl_Editor.Width, True)
    End Sub


#End Region

#Region "Toolstrip"

    Private Sub ts_G_Code_ItemClicked(sender As Object, e As ToolStripItemClickedEventArgs) Handles ts_Macro.ItemClicked
        Dim Current_Row As Integer

        If Not IsNothing(grid_Macro.CurrentRow) Then
            Current_Row = grid_Macro.CurrentRow.Index
        Else
            Current_Row = 0
        End If

        Select Case e.ClickedItem.Text

            Case "Open"
                Dim dlg As New OpenFileDialog

                dlg.InitialDirectory = Chip_Macro_Program_Folder
                dlg.FileName = File_Name
                dlg.Filter = "Text Files (*.txt)|*.txt|All files (*.*)|*.*"

                Ignore_Cell_Enter_Event = True
                lst_Errors.Items.Clear()
                txt_Help.Text = ""

                If dlg.ShowDialog() = Windows.Forms.DialogResult.OK Then
                    G_Code.Load_G_Code(dlg.FileName)
                    Compile()
                End If
                Ignore_Cell_Enter_Event = False

            Case "Run"
                Enable_Toolstrip(True)
                Application.DoEvents()
                G_Code.Execute_Macro(Class_Macro_Interpreter.enum_Run_Commands.Continuous)

            Case "Halt"
                G_Code.Macro.Halt = True
                Enable_Toolstrip(False)

            Case "Step"
                Single_Step()

            Case "Start Here"
                Show_Program_Counter(enum_Program_Cursor_Commands.Clear_All)
                G_Code.Execute_Macro(Class_Macro_Interpreter.enum_Run_Commands.Start_At, grid_Macro.CurrentRow.Index)
                Show_Program_Counter(enum_Program_Cursor_Commands.Show, G_Code.Macro.Program_Counter)

            Case "Save"
                Save_File()

            Case "Delete"
                If Not IsNothing(grid_Macro.CurrentRow) Then
                    G_Code.Macro.Delete_Line(grid_Macro.CurrentRow.Index)
                    Compile()
                    Class_Grid_Handler.Move_To_Cell(grid_Macro, Current_Row)
                End If

            Case "Move Up"
                If Not IsNothing(grid_Macro.CurrentRow) Then
                    If grid_Macro.CurrentRow.Index > 1 Then
                        G_Code.Macro.Move_Line_Up(grid_Macro.CurrentRow.Index)
                        Compile()
                        Class_Grid_Handler.Move_To_Cell(grid_Macro, Current_Row - 1)
                    End If
                End If

            Case "Move Down"
                If Not IsNothing(grid_Macro.CurrentRow) Then
                    If grid_Macro.CurrentRow.Index < grid_Macro.RowCount - 2 Then
                        G_Code.Macro.Move_Line_Down(grid_Macro.CurrentRow.Index)
                        Compile()
                        Class_Grid_Handler.Move_To_Cell(grid_Macro, Current_Row + 1)
                    End If
                End If

        End Select
    End Sub


    Private Sub Enable_Toolstrip(Running As Boolean)

        For I = 0 To ts_Macro.Items.Count - 1
            Select Case ts_Macro.Items(I).Text
                Case "Run", "Halt"
                    If Running Then
                        Dim Bm As New Bitmap(Chip_Icons_Folder & "NoAction.bmp")
                        ts_Macro.Items(I).Text = "Halt"
                        ts_Macro.Items(I).Image = Bm
                    Else
                        Dim Bm As New Bitmap(Chip_Icons_Folder & "run.bmp")
                        ts_Macro.Items(I).Text = "Run"
                        ts_Macro.Items(I).Image = Bm
                    End If

                Case "Exit", "Step"
                Case Else
                    ts_Macro.Items(I).Enabled = Not Running
            End Select
        Next

    End Sub

    Private Sub ts_Insert_DropDownItemClicked(sender As Object, e As System.Windows.Forms.ToolStripItemClickedEventArgs) Handles ts_Insert.DropDownItemClicked
        Dim Idx As Integer = 0

        If IsNothing(grid_Macro.CurrentRow) Then
            Idx = 0
        Else
            Idx = grid_Macro.CurrentRow.Index + 1
        End If

        'If Not IsNothing(grid_Macro.CurrentRow) Then

        Dim Cursor_Pos As Integer = 1
        Dim Line_Pos As Integer = 0

        Select Case e.ClickedItem.Text
            Case "Expression"
                G_Code.Macro.Insert_Line(Idx)

            Case "IF"
                G_Code.Macro.Insert_Line(Idx, "")
                G_Code.Macro.Insert_Line(Idx, "END_IF")
                G_Code.Macro.Insert_Line(Idx, "")
                G_Code.Macro.Insert_Line(Idx, "IF  THEN")
                Cursor_Pos = 4

            Case "ELSE"
                G_Code.Macro.Insert_Line(Idx, "")
                G_Code.Macro.Insert_Line(Idx, "ELSE")
                Line_Pos = 1

            Case "FOR"
                G_Code.Macro.Insert_Line(Idx, "")
                G_Code.Macro.Insert_Line(Idx, "NEXT")
                G_Code.Macro.Insert_Line(Idx, "")
                G_Code.Macro.Insert_Line(Idx, "FOR TO")
                Line_Pos = 4

            Case "WHILE"
                G_Code.Macro.Insert_Line(Idx, "")
                G_Code.Macro.Insert_Line(Idx, "END_WHILE")
                G_Code.Macro.Insert_Line(Idx, "")
                G_Code.Macro.Insert_Line(Idx, "WHILE ")
                Cursor_Pos = 7

            Case "DO"
                G_Code.Macro.Insert_Line(Idx, "")
                G_Code.Macro.Insert_Line(Idx, "UNTIL")
                G_Code.Macro.Insert_Line(Idx, "")
                G_Code.Macro.Insert_Line(Idx, "DO")

            Case "GOTO"
                G_Code.Macro.Insert_Line(Idx, "")
                G_Code.Macro.Insert_Line(Idx, "GOTO ")
                Cursor_Pos = 6

            Case "LABEL"
                G_Code.Macro.Insert_Line(Idx, "")
                G_Code.Macro.Insert_Line(Idx, ":")
                Cursor_Pos = 2

            Case "CALL"
                G_Code.Macro.Insert_Line(Idx, "")
                G_Code.Macro.Insert_Line(Idx, "CALL( )")
                Cursor_Pos = 6

            Case "Local INTEGER Variable"
                G_Code.Macro.Insert_Line(Idx, "INTEGER ")
                Cursor_Pos = 9

            Case "Local FLOAT Variable"
                G_Code.Macro.Insert_Line(Idx, "FLOAT ")
                Cursor_Pos = 7

            Case "Local STRING Variable"
                G_Code.Macro.Insert_Line(Idx, "STRING ")
                Cursor_Pos = 8

            Case "Global Variable"
                'Symbols.Symbol_Query("Action*")

            Case "BREAKPOINT"
                G_Code.Macro.Insert_Line(Idx, "")
                G_Code.Macro.Insert_Line(Idx, "BREAKPOINT")

        End Select

        Ignore_Cell_Enter_Event = True

        Compile()
        Class_Grid_Handler.Move_To_Cell(grid_Macro, Idx + Line_Pos, 1)

        Dim Box As TextBox
        grid_Macro.BeginEdit(False)
        Box = grid_Macro.EditingControl
        For I = 0 To Box.Text.Length - 1
            If Box.Text(I) <> " " Then
                Cursor_Pos += I - 1
                Exit For
            End If
        Next
        Box.SelectionStart = Cursor_Pos
        Box.SelectionLength = 0
        Cell_Original_Text = grid_Macro.CurrentCell.Value

        Ignore_Cell_Enter_Event = False

        Edits_Made = True

        'End If

    End Sub

    Private Sub Single_Step()
        Enable_Toolstrip(False)
        Show_Program_Counter(enum_Program_Cursor_Commands.Clear_All)
        G_Code.Execute_Macro(Class_Macro_Interpreter.enum_Run_Commands.Single_Step)
        Show_Program_Counter(enum_Program_Cursor_Commands.Show, G_Code.Macro.Program_Counter)

        lst_Errors.Items.Clear()

        For I = 0 To G_Code.Macro.Local_Variable_Names.Count - 1
            lst_Errors.Items.Add(G_Code.Macro.Local_Variable_Names(I) & "=" & G_Code.Macro.Local_Variables(I).Value)
        Next

    End Sub

#End Region

#Region "Keyboard events"

    Private Sub dlg_G_Code_KeyDown(sender As Object, e As KeyEventArgs) Handles Me.KeyDown
        If e.KeyCode = Keys.F8 Then
            Single_Step()
        End If
    End Sub

#End Region

#Region "Grid events and routines"

    Private Sub grid_G_Code_Program_(sender As Object, e As System.EventArgs) Handles grid_Macro.CurrentCellChanged
        If Not Initialized Then Exit Sub
        If Ignore_Cell_Enter_Event Then Exit Sub
        G_Code.Highlight_Grid(grid_Macro)
    End Sub

    Private Sub grid_Macro_CellEndEdit(sender As Object, e As System.Windows.Forms.DataGridViewCellEventArgs) Handles grid_Macro.CellEndEdit
        If Ignore_Cell_Enter_Event Then Exit Sub
        If IsNothing(grid_Macro.CurrentRow) Then Exit Sub

        Dim Idx As Integer = grid_Macro.CurrentRow.Index
        If grid_Macro.CurrentCell.Value = Cell_Original_Text Then
            Exit Sub
        End If

        If grid_Macro.Rows(Idx).Cells(enum_Grid_Columns.Comment).Value = "'" Then
            grid_Macro.Rows(Idx).Cells(enum_Grid_Columns.Comment).Value = ""
        Else
            If grid_Macro.Rows(Idx).Cells(enum_Grid_Columns.Comment).Value <> "" Then
                If Mid(grid_Macro.Rows(Idx).Cells(enum_Grid_Columns.Comment).Value, 1, 1) <> "'" Then
                    grid_Macro.Rows(Idx).Cells(enum_Grid_Columns.Comment).Value = "'" & grid_Macro.Rows(Idx).Cells(enum_Grid_Columns.Comment).Value
                End If
            End If
        End If

        G_Code.Macro.Program(Idx).Raw_Text = grid_Macro.Rows(Idx).Cells(enum_Grid_Columns.Block).Value & " " & grid_Macro.Rows(Idx).Cells(enum_Grid_Columns.Comment).Value
        Compile()
        Edits_Made = True
    End Sub

    Private Sub CellEnter(sender As Object, e As System.Windows.Forms.DataGridViewCellEventArgs) Handles grid_Macro.CellEnter
        If Not Initialized Then Exit Sub
        If Ignore_Cell_Enter_Event Then Exit Sub

        Ignore_Cell_Enter_Event = True

        If e.ColumnIndex > enum_Grid_Columns.Line Then
            Dim Box As TextBox
            grid_Macro.BeginEdit(False)
            Box = grid_Macro.EditingControl
            Box.SelectionStart = Box.Text.Length
            Cell_Original_Text = grid_Macro.CurrentCell.Value
        End If

        Ignore_Cell_Enter_Event = False

    End Sub

    Private Sub Show_Program_Counter(Command As enum_Program_Cursor_Commands, Optional Line_Number As Integer = -1)

        Select Case Command
            Case enum_Program_Cursor_Commands.Clear_All
                For R = 0 To grid_Macro.RowCount - 1
                    grid_Macro.Rows(R).Cells(enum_Grid_Columns.Line).Style.BackColor = Color.White
                Next

            Case enum_Program_Cursor_Commands.Clear
                grid_Macro.Rows(Line_Number).Cells(enum_Grid_Columns.Line).Style.BackColor = Color.White

            Case enum_Program_Cursor_Commands.Show
                grid_Macro.Rows(Line_Number).Cells(enum_Grid_Columns.Line).Style.BackColor = Color.GreenYellow

        End Select

    End Sub


#End Region

#Region "Error list events"

    Private Sub lst_G_Code_Errors_SelectedIndexChanged(sender As Object, e As EventArgs) Handles lst_Errors.SelectedIndexChanged
        If Not Initialized Then Exit Sub

        Dim Idx As Integer = lst_Errors.SelectedIndex
        Dim S() As String
        Dim T As String

        If lst_Errors.SelectedIndex < 0 Then Exit Sub

        Ignore_Cell_Enter_Event = True

        T = lst_Errors.Items(lst_Errors.SelectedIndex)
        S = T.Split(":")
        If (S.Count > 0) And S(0) <> "" Then
            If IsNumeric(S(0)) Then
                Idx = Int(S(0))
                Class_Grid_Handler.Move_To_Cell(grid_Macro, Idx, 1)
                txt_Help.Text = G_Code.Get_Help(Idx)
            End If
        End If

        Ignore_Cell_Enter_Event = False

    End Sub

#End Region

#Region "Utility routines"

    Private Function Compile()
        If G_Code.Compile_Macro(lst_Errors) Then
            tab_Errors.Text = "Watch"
        Else
            tab_Errors.Text = "Errors"
        End If
        G_Code.Load_Grid(grid_Macro, txt_Help)
        grid_Macro.ClearSelection()
        Return True

    End Function

    Private Function OK_To_Continue() As Boolean

        If Edits_Made Then
            Message_Box.ShowDialog("Edits were made, save?", "Warning", MessageBoxButtons.YesNoCancel)
            Select Case Message_Box.DialogResult
                Case Windows.Forms.DialogResult.Yes
                    Save_File()
                Case Windows.Forms.DialogResult.Cancel
                    Return False
            End Select
        End If

        Return True

    End Function

    Private Sub Save_File()
        Dim dlg As New SaveFileDialog

        grid_Macro.EndEdit()

        dlg.InitialDirectory = Chip_Macro_Program_Folder
        dlg.FileName = Path.GetFileName(File_Name)
        dlg.Filter = "Text Files (*.txt)|*.txt|All files (*.*)|*.*"
        If dlg.ShowDialog = Windows.Forms.DialogResult.OK Then

            For R = 0 To G_Code.Macro.Program.Count - 1

                If G_Code.Macro.Program(R).Type = Class_Macro_Interpreter.enum_Component_Type._COMMENT Then
                    G_Code.Macro.Program(R).Raw_Text = G_Code.Macro.Program(R).Comment
                Else
                    G_Code.Macro.Program(R).Raw_Text = grid_Macro.Rows(R).Cells(enum_Grid_Columns.Block).Value
                End If

            Next

            G_Code.Save_G_Code(dlg.FileName)
            Edits_Made = False
        End If

    End Sub

#End Region



End Class
