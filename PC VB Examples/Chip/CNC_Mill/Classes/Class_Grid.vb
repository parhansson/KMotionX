
Public Class Class_Grid_Handler

    Private Structure struct_Filter
        Public Combo_Box As ComboBox
        Public Column_Index As Integer
    End Structure

    'The first two columns in the DataGridView and DataTable must be Index and Sort_Order
    'The Index column is a pointer to the DataTable row

    Private WithEvents Grid As DataGridView
    Private Table As DataTable
    Private Filters As New List(Of struct_Filter)
    Private Symbol_Filter_Box As TextBox
    Private Records_Label As Label

    Private Inhibit_Filters As Boolean = False

    Public Sub New(ByRef Data_Grid_View As DataGridView, ByRef Data_Table As DataTable)
        Grid = Data_Grid_View
        Table = Data_Table
    End Sub


    'Public Sub Grid_CellClick(sender As Object, e As System.Windows.Forms.DataGridViewCellEventArgs) Handles Grid.CellClick
    '    If e.RowIndex < Grid.RowCount - 1 Then
    '        Move_To_Cell(Grid, e.RowIndex + 1, class_Symbol_Table.enum_Table_Columns.Value)
    '    End If
    '    Grid.Invalidate()
    'End Sub

    'Public Sub Load_Grid_From_Table()
    '    Dim P(Table.Columns.Count - 1) As String
    '    Dim Show_Row As Boolean = True
    '    Dim T As String
    '    Dim Target As String = UCase(Symbol_Filter_Box.Text)

    '    Grid.Rows.Clear()

    '    For R = 0 To Table.Rows.Count - 1
    '        Show_Row = True

    '        For F = 0 To Filters.Count - 1
    '            If Filters(F).Combo_Box.Text <> "" And Filters(F).Combo_Box.Text <> "All" Then
    '                If Filters(F).Combo_Box.Text <> Table.Rows(R).Item(Filters(F).Column_Index) Then
    '                    Show_Row = False
    '                    Exit For
    '                End If
    '            End If
    '        Next

    '        If Symbol_Filter_Box.Text <> "" Then
    '            T = UCase(Table.Rows(R).Item(class_Symbol_Table_OLD.enum_Table_Columns.Symbol_Name))
    '            If Not T.Contains(Target) Then
    '                Show_Row = False
    '            End If
    '        End If

    '        If Show_Row Then
    '            For C = 0 To Table.Columns.Count - 1

    '                If IsDBNull(Table.Rows(R).Item(C)) Then
    '                    P(C) = ""
    '                ElseIf IsNothing(Table.Rows(R).Item(C)) Then
    '                    P(C) = ""
    '                Else
    '                    P(C) = Table.Rows(R).Item(C)
    '                End If
    '            Next

    '            Grid.Rows.Add(P)

    '            If Not IsDBNull(Table.Rows(R).Item(class_Symbol_Table_OLD.enum_Table_Columns.Read_Only)) Then 'Read only informaion
    '                T = Table.Rows(R).Item(class_Symbol_Table_OLD.enum_Table_Columns.Read_Only)
    '                For C = class_Symbol_Table_OLD.enum_Table_Columns.Category To class_Symbol_Table_OLD.enum_Table_Columns.Help_Files
    '                    Grid.Rows(Grid.RowCount - 1).Cells(C).Style.BackColor = Color.Honeydew
    '                Next

    '                If T <> "" Then
    '                    If T.Contains("A") Then 'All but value
    '                        For C = class_Symbol_Table_OLD.enum_Table_Columns.Category To class_Symbol_Table_OLD.enum_Table_Columns.Help_Files
    '                            If C <> class_Symbol_Table_OLD.enum_Table_Columns.Value Then
    '                                Grid.Rows(Grid.RowCount - 1).Cells(C).Style.BackColor = Color.White
    '                                Grid.Rows(Grid.RowCount - 1).Cells(C).ReadOnly = True
    '                            End If
    '                        Next
    '                    End If

    '                    If T.Contains("H") Then 'Header
    '                        For C = class_Symbol_Table_OLD.enum_Table_Columns.Category To class_Symbol_Table_OLD.enum_Table_Columns.Symbol_Type
    '                            Grid.Rows(Grid.RowCount - 1).Cells(C).Style.BackColor = Color.White
    '                            Grid.Rows(Grid.RowCount - 1).Cells(C).ReadOnly = True
    '                        Next
    '                    End If

    '                    If T.Contains("S") Then 'Symbol Name
    '                        Grid.Rows(Grid.RowCount - 1).Cells(class_Symbol_Table_OLD.enum_Table_Columns.Symbol_Name).Style.BackColor = Color.White
    '                        Grid.Rows(Grid.RowCount - 1).Cells(class_Symbol_Table_OLD.enum_Table_Columns.Symbol_Name).ReadOnly = True
    '                    End If

    '                    If T.Contains("V") Then 'Value
    '                        Grid.Rows(Grid.RowCount - 1).Cells(class_Symbol_Table_OLD.enum_Table_Columns.Value).Style.BackColor = Color.White
    '                        Grid.Rows(Grid.RowCount - 1).Cells(class_Symbol_Table_OLD.enum_Table_Columns.Value).ReadOnly = True
    '                    End If

    '                    If T.Contains("C") Then 'Choices
    '                        Grid.Rows(Grid.RowCount - 1).Cells(class_Symbol_Table_OLD.enum_Table_Columns.Choices).Style.BackColor = Color.White
    '                        Grid.Rows(Grid.RowCount - 1).Cells(class_Symbol_Table_OLD.enum_Table_Columns.Choices).ReadOnly = True
    '                    End If

    '                    If T.Contains("H") Then 'Hot Key
    '                        Grid.Rows(Grid.RowCount - 1).Cells(class_Symbol_Table_OLD.enum_Table_Columns.Hot_Key).Style.BackColor = Color.White
    '                        Grid.Rows(Grid.RowCount - 1).Cells(class_Symbol_Table_OLD.enum_Table_Columns.Hot_Key).ReadOnly = True
    '                    End If

    '                    If T.Contains("T") Then 'Comment
    '                        Grid.Rows(Grid.RowCount - 1).Cells(class_Symbol_Table_OLD.enum_Table_Columns.Comment).Style.BackColor = Color.White
    '                        Grid.Rows(Grid.RowCount - 1).Cells(class_Symbol_Table_OLD.enum_Table_Columns.Comment).ReadOnly = True
    '                    End If

    '                End If
    '            End If


    '        End If
    '    Next

    '    Adjust_Filters()

    '    Records_Label.Text = Grid.RowCount

    'End Sub

    Public Sub Adjust_Filters()
        Dim TBL As Integer = 0

        For F = 0 To Filters.Count - 1
            Dim L As Integer = 0
            For C = 0 To Filters(F).Column_Index - 1
                If Grid.Columns(C).Visible Then
                    L += Grid.Columns(C).Width
                    If L > TBL Then TBL = L
                End If
            Next
            Filters(F).Combo_Box.Width = Grid.Columns(Filters(F).Column_Index).Width
            Filters(F).Combo_Box.Left = L + Grid.RowHeadersWidth
        Next

        Symbol_Filter_Box.Left = TBL + Filters(Filters.Count - 1).Combo_Box.Width + Grid.RowHeadersWidth
    End Sub

    'Public Sub Add_Filter_Clear_Button_Symbol_Filter_TextBox(Clear_Button As Button, Filter_Text_Box As TextBox, Record_Count_Label As Label)
    '    RemoveHandler Clear_Button.Click, AddressOf Clear_Filters
    '    AddHandler Clear_Button.Click, AddressOf Clear_Filters

    '    Symbol_Filter_Box = Filter_Text_Box
    '    Symbol_Filter_Box.Text = ""
    '    RemoveHandler Filter_Text_Box.TextChanged, AddressOf Filter_Symbol
    '    AddHandler Filter_Text_Box.TextChanged, AddressOf Filter_Symbol
    '    Records_Label = Record_Count_Label
    'End Sub

    'Public Sub Add_Filter_Combo(ByRef Combo_Box As ComboBox, Column_Index As Integer)
    '    Dim Filter As New struct_Filter
    '    Filter.Combo_Box = Combo_Box
    '    Filter.Column_Index = Column_Index
    '    Filter.Combo_Box.Text = "All"
    '    Filters.Add(Filter)

    '    RemoveHandler Combo_Box.DropDown, AddressOf Filter_Dropdown
    '    AddHandler Combo_Box.DropDown, AddressOf Filter_Dropdown

    '    RemoveHandler Combo_Box.SelectedIndexChanged, AddressOf Filter_Selected_Index_Changed
    '    AddHandler Combo_Box.SelectedIndexChanged, AddressOf Filter_Selected_Index_Changed
    'End Sub

    'Public Sub Fill()
    '    Load_Grid_From_Table()
    'End Sub


    'Public Sub Filter_Symbol(sender As Object, e As System.EventArgs)
    '    If Inhibit_Filters Then Exit Sub
    '    Load_Grid_From_Table()
    'End Sub


    'Public Sub Clear_Filters(sender As Object, e As System.EventArgs)
    '    Inhibit_Filters = True
    '    For F = 0 To Filters.Count - 1
    '        Filters(F).Combo_Box.Text = "All"
    '    Next
    '    Symbol_Filter_Box.Text = ""
    '    Load_Grid_From_Table()
    '    Inhibit_Filters = False
    'End Sub

    'Public Sub Filter_Selected_Index_Changed(sender As Object, e As System.EventArgs)
    '    If Inhibit_Filters Then Exit Sub
    '    Load_Grid_From_Table()
    'End Sub

    Public Sub Filter_Dropdown(sender As Object, e As System.EventArgs)
        For I = 0 To Filters.Count - 1
            If Filters(I).Combo_Box Is sender Then
                Load_Filter_Combo(Filters(I).Combo_Box, Filters(I).Column_Index)
            End If
        Next
    End Sub

    Public Sub Load_Filter_Combo(ByRef cmb As ComboBox, Column_Index As Integer)
        cmb.Items.Clear()
        cmb.Items.Add("All")
        For R = 0 To Grid.RowCount - 1
            If Not cmb.Items.Contains(Grid.Rows(R).Cells(Column_Index).Value) Then
                cmb.Items.Add(Grid.Rows(R).Cells(Column_Index).Value)
            End If
        Next
    End Sub

    Public Sub Update_Data_Table_By_Index()
        Dim P(Grid.ColumnCount - 1) As String
        Dim Idx As Integer

        Grid.EndEdit()

        For R = 0 To Grid.RowCount - 1
            Idx = Grid.Rows(R).Cells(0).Value
            For C = 0 To Grid.ColumnCount - 1
                If IsDBNull(Grid.Rows(R).Cells(C).Value) Then
                    Table.Rows(Idx).Item(C) = ""
                ElseIf IsNothing(Grid.Rows(R).Cells(C).Value) Then
                    Table.Rows(Idx).Item(C) = ""
                Else
                    Table.Rows(Idx).Item(C) = Grid.Rows(R).Cells(C).Value
                End If
            Next
        Next

    End Sub

    Public Sub Create_Data_Table_From_Grid()
        Dim P(Grid.ColumnCount - 1) As String
        Grid.EndEdit()

        Table.Rows.Clear()

        For R = 0 To Grid.RowCount - 1
            For C = 0 To Grid.ColumnCount - 1
                If IsDBNull(Grid.Rows(R).Cells(C).Value) Then
                    P(C) = ""
                ElseIf IsNothing(Grid.Rows(R).Cells(C).Value) Then
                    P(C) = ""
                Else
                    P(C) = Grid.Rows(R).Cells(C).Value
                End If
            Next
            Table.Rows.Add(P)
        Next

    End Sub


    Public Function Sort(Data_Table As DataTable, Column_Name As String, Direction As String) As DataTable
        Data_Table.DefaultView.Sort = Column_Name & " " & Direction
        Data_Table = Data_Table.DefaultView.ToTable
        Return Data_Table
    End Function

    Public Sub Insert_Row(Grid As DataGridView)
        Dim R As DataRow

        If Grid.RowCount = 0 Then
            Grid.Rows.Insert(0, 1)

            R = Table.NewRow
            Table.Rows.InsertAt(R, 0)
            GoTo Early_Exit
        End If

        If IsNothing(Grid.CurrentRow) Then
            Message_Box.ShowDialog("Row must be selected to insert.")
            Exit Sub
        End If

        Dim Idx As Integer = Grid.CurrentRow.Cells(0).Value
        Grid.Rows.Insert(Grid.CurrentRow.Index, 1)
        R = Table.NewRow
        Table.Rows.InsertAt(R, 0)

Early_Exit:

        Move_To_Cell(Grid, Grid.CurrentRow.Index - 1, Grid.CurrentCell.ColumnIndex)
    End Sub

    Public Sub Delete_Row(Grid As DataGridView)
        If Grid.RowCount = 0 Then Exit Sub

        If IsNothing(Grid.CurrentRow.Index) Then
            Message_Box.ShowDialog("Row must be selected to insert.")
            Exit Sub
        End If

        Grid.Rows.RemoveAt(Grid.CurrentRow.Index)

        Dim Idx As Integer = Grid.CurrentRow.Cells(0).Value
        Table.Rows.RemoveAt(Idx)

    End Sub

    Public Sub Move_Row_Up(Grid As DataGridView)
        If IsNothing(Grid.CurrentRow) Then Exit Sub
        If Grid.CurrentRow.Index = 0 Then Exit Sub
        Grid.EndEdit()
        Dim R1 As New List(Of Object)
        Dim R2 As New List(Of Object)
        R1 = Save_Row(Grid, Grid.CurrentRow.Index - 1)
        R2 = Save_Row(Grid, Grid.CurrentRow.Index)
        Load_Row(Grid, Grid.CurrentRow.Index - 1, R2)
        Load_Row(Grid, Grid.CurrentRow.Index, R1)
        Move_To_Cell(Grid, Grid.CurrentRow.Index - 1, Grid.CurrentCell.ColumnIndex)
    End Sub

    Public Sub Move_Row_Down(Grid As DataGridView)
        If IsNothing(Grid.CurrentRow) Then Exit Sub
        If Grid.CurrentRow.Index = Grid.RowCount Then Exit Sub
        Grid.EndEdit()
        Dim R1 As New List(Of Object)
        Dim R2 As New List(Of Object)
        R1 = Save_Row(Grid, Grid.CurrentRow.Index)
        R2 = Save_Row(Grid, Grid.CurrentRow.Index + 1)
        Load_Row(Grid, Grid.CurrentRow.Index, R2)
        Load_Row(Grid, Grid.CurrentRow.Index + 1, R1)
        Move_To_Cell(Grid, Grid.CurrentRow.Index + 1, Grid.CurrentCell.ColumnIndex)
    End Sub

    Private Function Save_Row(Grid As DataGridView, Row As Integer) As List(Of Object)
        Dim V As New List(Of Object)
        For C = 0 To Grid.Columns.Count - 1
            V.Add(Grid.Rows(Row).Cells(C).Value)
        Next
        Return V
    End Function

    Private Sub Load_Row(Grid As DataGridView, Row As Integer, Values As List(Of Object))
        For C = 0 To Grid.Columns.Count - 1
            Grid.Rows(Row).Cells(C).Value = Values(C)
        Next
    End Sub

    Public Shared Sub Move_To_Cell(Grid As DataGridView, ByVal Row As Integer, Optional ByVal Cell As Integer = 0)
        If Row > Grid.RowCount - 1 Or Row < 0 Then Exit Sub
        If Cell > Grid.ColumnCount - 1 Then Exit Sub

        Grid.EndEdit()

        If IsNothing(Grid.Rows(Row)) Then Exit Sub
        If IsNothing(Grid.Rows(Row).Cells(Cell)) Then Exit Sub

        Try
            If Cell >= 0 Then
                If Grid.Rows(Row).Cells(Cell).Visible Then
                    Grid.CurrentCell = Grid.Rows(Row).Cells(Cell)
                End If
            Else
                'Move to first visible cell
                Dim C As Integer
                For C = 0 To Grid.Columns.Count - 1
                    If Grid.Columns(C).Visible Then
                        Grid.CurrentCell = Grid.Rows(Row).Cells(C)
                        Exit For
                    End If
                Next
            End If
        Catch ex As Exception
            'Ignore faults
        End Try

    End Sub

End Class
