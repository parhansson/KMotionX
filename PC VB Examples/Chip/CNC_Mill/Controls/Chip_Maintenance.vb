Imports System.IO

Public Class Chip_Maintenance

#Region "Declarations"

    Private Enum enum_Grid_Cols
        Task
        Period
        Time_Left
        Last_Date
    End Enum

    Private Cycle_Time As TimeSpan
    Private Initialized As Boolean = False

    Public Persistant_Settings As New List(Of String)

#End Region

#Region "Grid Events"

    Private Sub Initialize()
        Initialized = True
    End Sub

    Private Sub grid_Maint_CellClick(sender As Object, e As DataGridViewCellEventArgs) Handles grid_Maint.CellClick
        If Not Initialized Then Exit Sub
        If e.RowIndex < 0 Then Exit Sub

        If e.ColumnIndex = enum_Grid_Cols.Time_Left Or e.ColumnIndex = enum_Grid_Cols.Last_Date Then
            Message_Box.ShowDialog("Set date to today and reset hours?", "Warning", MessageBoxButtons.YesNo)
            If Message_Box.DialogResult = DialogResult.Yes Then
                Dim T As String
                Dim S() As String
                Dim Time_Left As New TimeSpan(0)

                T = grid_Maint.Rows(e.RowIndex).Cells(enum_Grid_Cols.Period).Value

                If T = "" Then
                    Time_Left = New TimeSpan(0)
                Else
                    S = T.Split(":")
                    Select Case S.Count
                        Case 1
                            Time_Left = New TimeSpan(S(0), 0, 0)
                        Case 2
                            Time_Left = New TimeSpan(S(0), S(1), 0)
                        Case 3
                            Time_Left = New TimeSpan(S(0), S(1), S(2))
                    End Select
                End If
                grid_Maint.CurrentRow.Cells(enum_Grid_Cols.Time_Left).Value = Math.Truncate(Time_Left.TotalHours) & ":" & Time_Left.Minutes & ":" & Time_Left.Seconds

                grid_Maint.CurrentRow.Cells(enum_Grid_Cols.Last_Date).Value = Today.ToShortDateString
                grid_Maint.CurrentRow.Cells(enum_Grid_Cols.Time_Left).Style.BackColor = Color.White
                grid_Maint.CurrentCell = grid_Maint.CurrentRow.Cells(0)
                Save_Maintenance_File()
            End If
        End If
    End Sub

    Private Sub grid_Maint_CellEndEdit(sender As Object, e As DataGridViewCellEventArgs) Handles grid_Maint.CellEndEdit
        If Not Initialized Then Exit Sub
        Save_Maintenance_File()
    End Sub

#End Region

#Region "Update_Times, Check_Times"

    Public Sub Update_Times(In_Cycle_Time As TimeSpan)
        Dim T As String
        Dim S() As String
        Dim Time_Left As New TimeSpan(0)

        Cycle_Time = Cycle_Time.Add(In_Cycle_Time)

        txt_Time_In_Cycle.Text = "Time in Cycle : " & Math.Truncate(Cycle_Time.TotalHours) & ":" & Cycle_Time.Minutes & ":" & Cycle_Time.Seconds

        For I = 0 To grid_Maint.RowCount - 2
            T = grid_Maint.Rows(I).Cells(enum_Grid_Cols.Time_Left).Value
            S = T.Split(":")
            Time_Left = New TimeSpan(S(0), S(1), S(2))

            Time_Left = Time_Left.Subtract(In_Cycle_Time)

            T = Time_Left.Days

            grid_Maint.Rows(I).Cells(enum_Grid_Cols.Time_Left).Value = Math.Truncate(Time_Left.TotalHours) & ":" & Time_Left.Minutes & ":" & Time_Left.Seconds

        Next

        Save_Maintenance_File()
        Check_Times(True)

    End Sub

    Public Sub Check_Times(Flash As Boolean)
        Dim T As String
        Dim S() As String
        Dim Maintenance_Due As String = ""
        Dim Time_Left As New TimeSpan(0)

        For I = 0 To grid_Maint.RowCount - 2
            T = grid_Maint.Rows(I).Cells(enum_Grid_Cols.Time_Left).Value
            S = T.Split(":")
            Time_Left = New TimeSpan(S(0), S(1), S(2))

            If Time_Left.Hours < 0 Or Time_Left.Minutes < 0 Or Time_Left.Seconds < 0 Then
                grid_Maint.Rows(I).Cells(enum_Grid_Cols.Time_Left).Style.BackColor = Color.Red
                Maintenance_Due &= grid_Maint.Rows(I).Cells(enum_Grid_Cols.Task).Value & vbCrLf
            End If
        Next

        If Maintenance_Due <> "" Then
            If Flash Then
                Flash_Message("Maintenance due : " & vbCrLf & vbCrLf & Maintenance_Due)
            Else
                Message_Box.ShowDialog("Maintenance due : " & vbCrLf & vbCrLf & Maintenance_Due)
            End If
        End If

    End Sub

#End Region

#Region "Load, Save"

    Public Sub Load_Maintenance_File()
        Dim S() As String = Nothing
        Dim T() As String = Nothing
        Dim Buff() As String

        Buff = File.ReadAllLines(Chip_Maintenance_File_Name)
        grid_Maint.Rows.Clear()

        S = Buff(0).Split("~")
        txt_Time_In_Cycle.Text = "Time in Cycle : " & S(1)

        T = S(1).Split(":")
        Cycle_Time = New TimeSpan(T(0), T(1), T(2))

        For Row = 1 To Buff.Count - 1
            If Buff(Row) <> "" Then
                S = Buff(Row).Split("~")
                grid_Maint.Rows.Add(S)
            End If
        Next

        Check_Times(False)

    End Sub

    Public Sub Save_Maintenance_File()
        Dim Buff As New List(Of String)
        Dim S As String = ""

        Buff.Clear()
        Buff.Add("Time in Cycle~" & Math.Truncate(Cycle_Time.TotalHours) & ":" & Cycle_Time.Minutes & ":" & Cycle_Time.Seconds)

        For Row = 0 To grid_Maint.RowCount - 2
            S = grid_Maint.Rows(Row).Cells(enum_Grid_Cols.Task).Value
            For I = enum_Grid_Cols.Period To enum_Grid_Cols.Last_Date
                S &= "~" & grid_Maint.Rows(Row).Cells(I).Value
            Next
            Buff.Add(S)
        Next
        File.WriteAllLines(Chip_Maintenance_File_Name, Buff)

    End Sub

#End Region

End Class
