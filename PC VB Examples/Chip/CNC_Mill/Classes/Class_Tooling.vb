Imports System.IO

Public Class Class_Tooling
    Public Structure struct_Tool
        Public Order As Integer
        Public Tool As String
        Public Max_Speed As String
        Public Tool_Seat_X As String
        Public Tool_Seat_Y As String
        Public Tool_Seat_Z As String
        Public Tool_Measure_X As String
        Public Tool_Measure_Y As String
        Public Tool_Measure_Z As String
    End Structure

    Public Tool_Table As New List(Of struct_Tool)

    Public Function Get_Tool_Index(Tool_Name As String) As Integer
        For I = 0 To Tool_Table.Count - 1
            If Tool_Table(I).Tool = Tool_Name Then
                Return I
            End If
        Next
        Return -1
    End Function

    Public Sub Load_Tool_Table()
        Dim Buff() As String
        Dim S() As String
        Tool_Table.Clear()

        If File.Exists(Chip_Symbols_File) Then
            Buff = File.ReadAllLines(Chip_Tool_Table_File)

            For R = 0 To Buff.Count - 1
                If Buff(R) <> "" Then
                    Dim T As New struct_Tool
                    S = Buff(R).Split("~")
                    T.Order = R
                    T.Tool = S(0)
                    T.Max_Speed = S(1)
                    T.Tool_Seat_X = S(2)
                    T.Tool_Seat_Y = S(3)
                    T.Tool_Seat_Z = S(4)
                    T.Tool_Measure_X = S(5)
                    T.Tool_Measure_Y = S(6)
                    T.Tool_Measure_Z = S(7)
                    Tool_Table.Add(T)
                End If
            Next

        Else
            Show_Error("Symbol table file not found : " & Chip_Tool_Table_File)
        End If

    End Sub

    Public Sub Save_Tool_Table()
        Dim Buff As New List(Of String)
        Dim S As String
        Dim DF As New Class_Data_File
        DF.Backup_File(Chip_Tool_Table_File, True)

        For I = 0 To Tool_Table.Count - 1
            S = Tool_Table(I).Tool & "~" & Tool_Table(I).Max_Speed & "~" &
                Tool_Table(I).Tool_Seat_X & "~" & Tool_Table(I).Tool_Seat_Y & "~" & Tool_Table(I).Tool_Seat_Z & "~" &
                Tool_Table(I).Tool_Measure_X & "~" & Tool_Table(I).Tool_Measure_Y & "~" & Tool_Table(I).Tool_Measure_Z & "~"
            Buff.Add(S)
        Next

        File.WriteAllLines(Chip_Tool_Table_File, Buff)

    End Sub


End Class
