Imports System.IO

Public Class Class_Grid_List

    Private Grid As DataGridView

    Private Class Class_Row
        Public Cells As List(Of String)
    End Class

    Public List_Columns As New List(Of String)
    Private List_Rows As New List(Of Class_Row)

    Private Ignore_Row_Enter As Boolean = False

    Public Sub New(Grid_View As DataGridView)
        Grid = Grid_View
        For I = 0 To Grid.Columns.Count - 1
            List_Columns.Add(Grid.Columns(I).HeaderText)
        Next
    End Sub

    Public Function List_Rows_Count() As Integer
        Return List_Rows.Count
    End Function

    Public Sub Add_List_Row(Data() As String, Start_Index As Integer)
        Dim New_Row As New Class_Row
        Dim Cells As New List(Of String)
        For I = Start_Index To Data.Count - 1
            If (I = 12) Then
                Data(I) = Data(I).Replace("$$DEFAULT_BUILD_ROOT$$", Chip_Root_Path & "\Chip_Build")
            End If
            Cells.Add(Data(I))
        Next
        New_Row.Cells = Cells
        List_Rows.Add(New_Row)
    End Sub

    Public Function Get_List_Row(Index As Integer) As List(Of String)
        Return List_Rows(Index).Cells
    End Function

    Public Sub Clear()
        List_Rows.Clear()
        Grid.Rows.Clear()
    End Sub

    Public Sub Load_Grid()
        Grid.Rows.Clear()
        For I = 0 To List_Rows.Count - 1
            Grid.Rows.Add(List_Rows(I).Cells.ToArray)
        Next
    End Sub

    Public Sub Insert_Row(Index As Integer)
        Ignore_Row_Enter = True
        Dim New_Row As New Class_Row
        New_Row.Cells = New List(Of String)
        For I = 0 To Grid.Columns.Count - 1
            New_Row.Cells.Add("")
        Next
        If Index = List_Rows.Count Then
            List_Rows.Add(New_Row)
            Grid.Rows.Add()
        Else
            List_Rows.Insert(Index, New_Row)
            Grid.Rows.Insert(Index)
        End If
        Ignore_Row_Enter = False
        Move_To_Cell(Index, 0)
    End Sub

    Public Sub Delete_Row(Index As Integer)
        Ignore_Row_Enter = True
        List_Rows.RemoveAt(Index)
        Grid.Rows.RemoveAt(Index)
        Ignore_Row_Enter = False
    End Sub

    Public Sub Copy_Row(Index As Integer)
        Insert_Row(Index + 1)
        For I = 0 To Grid.Columns.Count - 1
            List_Rows(Index + 1).Cells(I) = List_Rows(Index).Cells(I)
            Grid.Rows(Index + 1).Cells(I).Value = Grid.Rows(Index).Cells(I).Value
        Next
        Move_To_Cell(Index, 0)
        Move_To_Cell(Index + 1, 0)
    End Sub

    Public Sub Move_Row_Up(Index As Integer, ByRef Change_Made As Boolean)
        Change_Made = False
        If Index < 1 Then Exit Sub
        Ignore_Row_Enter = True
        Dim S_1 As String
        Dim S_2 As String

        For I = 0 To List_Rows(Index).Cells.Count - 1
            S_1 = List_Rows(Index).Cells(I)
            S_2 = List_Rows(Index - 1).Cells(I)
            List_Rows(Index).Cells(I) = S_2
            List_Rows(Index - 1).Cells(I) = S_1
            Grid.Rows(Index).Cells(I).Value = S_2
            Grid.Rows(Index - 1).Cells(I).Value = S_1
        Next
        Ignore_Row_Enter = False
        Move_To_Cell(Index - 1, 0)
    End Sub

    Public Sub Move_Row_Down(Index As Integer, ByRef Change_Made As Boolean)
        Change_Made = False
        If Index > List_Rows.Count - 2 Then Exit Sub
        Ignore_Row_Enter = True
        Dim S_1 As String
        Dim S_2 As String
        For I = 0 To List_Rows(Index).Cells.Count - 1
            S_1 = List_Rows(Index).Cells(I)
            S_2 = List_Rows(Index + 1).Cells(I)
            List_Rows(Index).Cells(I) = S_2
            List_Rows(Index + 1).Cells(I) = S_1
            Grid.Rows(Index).Cells(I).Value = S_2
            Grid.Rows(Index + 1).Cells(I).Value = S_1
        Next
        Ignore_Row_Enter = False
        Move_To_Cell(Index + 1, 0)
    End Sub

    Public Sub Move_To_Cell(Index As Integer, Cell As Integer)
        Ignore_Row_Enter = True
        If Grid.RowCount = 0 Then Exit Sub
        If Grid.RowCount = 1 Then Index = 0
        If Index >= Grid.RowCount Then Index = Grid.RowCount - 1
        Grid.CurrentCell = Grid.Rows(Index).Cells(Cell)
        Ignore_Row_Enter = False
    End Sub

    Public Overloads Function Get_Cell_Value(Index As Integer, Header As String) As String
        For I = 0 To List_Columns.Count - 1
            If List_Columns(I) = Header Then
                Return List_Rows(Index).Cells(I)
            End If
        Next
        Return ""
    End Function

    Public Overloads Function Get_Cell_Value(Row_Index As Integer, Cell_Index As Integer) As String
        If Row_Index > List_Rows.Count - 1 Then Return ""
        Return List_Rows(Row_Index).Cells(Cell_Index)
    End Function

    Public Overloads Sub Set_Cell_Value(Row_Index As Integer, Header As String, Value As String)
        If Ignore_Row_Enter Then Exit Sub
        For I = 0 To List_Columns.Count - 1
            If List_Columns(I) = Header Then
                List_Rows(Row_Index).Cells(I) = Value
            End If
        Next
    End Sub

    Public Overloads Sub Set_Cell_Value(Row_Index As Integer, Cell_Index As Integer, Value As String)
        If Ignore_Row_Enter Then Exit Sub
        If List_Rows.Count < 1 Then Exit Sub
        List_Rows(Row_Index).Cells(Cell_Index) = Value
    End Sub

    Public Function Get_Packed_List_Row(Index As Integer) As String
        Dim S As String = ""
        For C = 0 To List_Columns.Count - 1
            S &= List_Rows(Index).Cells(C)
            If C < List_Columns.Count - 1 Then S &= "~"
        Next
        Return S
    End Function

    Public Shared Function Copy_List(Source_List As Class_Grid_List, grid As DataGridView) As Class_Grid_List
        Dim Dest_List As New Class_Grid_List(grid)

        For I = 0 To Source_List.List_Rows_Count - 1
            Dim Row As New Class_Row
            Row.Cells = New List(Of String)
            For C = 0 To Source_List.List_Columns.Count - 1
                Row.Cells.Add(Source_List.List_Rows(I).Cells(C))
            Next
            Dest_List.List_Rows.Add(Row)
        Next

        Return Dest_List

    End Function

    Public Shared Sub Move_List_Up(Lists As List(Of Class_Grid_List), Index As Integer)
        Dim List_1 As Class_Grid_List
        Dim List_2 As Class_Grid_List
        List_1 = Lists(Index)
        List_2 = Lists(Index - 1)
        Lists(Index) = List_2
        Lists(Index - 1) = List_1
    End Sub

    Public Shared Sub Move_List_Down(Lists As List(Of Class_Grid_List), Index As Integer)
        Dim List_1 As Class_Grid_List
        Dim List_2 As Class_Grid_List
        List_1 = Lists(Index)
        List_2 = Lists(Index + 1)
        Lists(Index) = List_2
        Lists(Index + 1) = List_1
    End Sub

End Class
