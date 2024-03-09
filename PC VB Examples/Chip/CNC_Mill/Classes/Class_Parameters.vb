Imports System.Xml
Imports System.IO

Public Class Class_Parameters

    Public Enum enum_Param_Col
        Section
        Block
        Name
        Alias_Name
        Default_Value
        Value
        Comment
    End Enum

    Public Structure struct_Param
        Public Section As String
        Public Block As String
        Public Name As String
        Public Alias_Name As String
        Public Default_Value As String
        Public Value As String
        Public Comment As String
    End Structure


    Public Enum enum_Table_Columm
        Table_Name
        Parameter_Name
        Comment
        Value
    End Enum


    Private Data As DataSet

    Public Grid_Param As DataGridView
    Public Grid_G_Code As DataGridView
    Public Grid_Form As DataGridView

    Public Table_Param As DataTable
    Public Table_G_Code As DataTable
    Public Tabel_Form As DataTable

    Public Params As New List(Of List(Of String))

    Public Sub Initialize(Parameter_Grid As DataGridView, G_Code_Grid As DataGridView, Form_Controls_Grid As DataGridView)
        Grid_Param = Parameter_Grid
        Grid_G_Code = G_Code_Grid
        Grid_Form = Form_Controls_Grid
    End Sub

    Public Function Text_OK(Text As String) As Boolean
        Dim OK As Boolean = True
        If Text.Contains("~") Then Return False
        If Text.Contains("\") Then Return False
        Return OK

    End Function

    Public Function Add_Node(Tree As TreeView, Path As String) As TreeNode
        Dim Target_Path As String = ""
        Dim Target_Node As TreeNode = Nothing
        Dim Parent_Path As String = ""
        Dim Parent_Node As TreeNode = Nothing


        Dim Root_Node = New TreeNode
        'Root_Node.Name = Root_Name
        'Root_Node.Text = Root_Name
        Tree.Nodes.Add(Root_Node)

        'For R = 0 To Table.Rows.Count - 1

        '    Target_Path = Root_Name
        '    Parent_Path = Root_Name
        '    Parent_Node = Root_Node

        '    For L = 0 To Levels - 1
        '        Target_Path &= "\" & Table.Rows(R).Item(L)
        '        If IsNothing(Parent_Node.Nodes(Target_Path)) Then
        '            Parent_Node.Nodes.Add(Target_Path, Table.Rows(R).Item(L))
        '        End If
        '        Parent_Path &= "\" & Table.Rows(R).Item(L)
        '        Parent_Node = Parent_Node.Nodes(Parent_Path)
        '    Next

        'Next

        Root_Node.Expand()

        'For I = 0 To Root_Node.Nodes.Count - 1
        '    Parent_Node = Root_Node.Nodes(I)
        '    Parent_Node.FirstNode.Expand()
        '    For J = 0 To Parent_Node.Nodes.Count - 1
        '        Target_Node = Parent_Node.Nodes(J)
        '        Target_Node.FirstNode.Expand()
        '    Next
        'Next

        Tree.ResumeLayout()

        Return Target_Node

    End Function

    Public Sub Build_Symbol_Tree(tree As TreeView)
        Dim Node As TreeNode = Nothing

        tree.SuspendLayout()
        tree.Nodes.Clear()
        tree.PathSeparator = "`"

        ' Node = Add_Node(tree, Path)

        tree.Nodes(0).FirstNode.Expand()

        'For I = 0 To Root_Node.Nodes.Count - 1
        '    Parent_Node = Root_Node.Nodes(I)
        '    Parent_Node.FirstNode.Expand()
        '    For J = 0 To Parent_Node.Nodes.Count - 1
        '        Target_Node = Parent_Node.Nodes(J)
        '        Target_Node.FirstNode.Expand()
        '    Next
        'Next

        tree.ResumeLayout()
    End Sub

    Public Sub Load()
        'Dim S As String = ""
        'Dim Data_File As New Class_Data_File

        'Exit Sub

        'Table_Param = New DataTable
        'Table_Param.TableName = "Parameters"
        'Table_Param.Columns.Add("Type")
        'Table_Param.Columns.Add("Block")
        'Table_Param.Columns.Add("Number")
        'Table_Param.Columns.Add("Name")
        'Table_Param.Columns.Add("Alias")
        'Table_Param.Columns.Add("Default")
        'Table_Param.Columns.Add("Value")
        'Table_Param.Columns.Add("Comment")
        'Table_Param.Columns.Add("Overflow_1")
        'Table_Param.Columns.Add("Overflow_2")
        'Table_Param.Columns.Add("Overflow_3")


        'Data_File.Load_Table_From_Delimited_File(Table_Param, Base_Path & CNC_Configuration_File_Name, "", False)
        'Data_File.Load_Grid_From_Table(Grid_Param, Table_Param, "")

        'For I = 0 To Table_Param.Rows.Count - 1
        '    S = Table_Param.Rows(I).Item(enum_Param_Col.Name)
        'Next

    End Sub

    Public Sub Save()
        Dim Data_File As New Class_Data_File
        'Dim Tables As New List(Of DataTable)

        'Grid_Handler.Update_Data_Table(Grid_Param, Table_Param)

        ''Grid_Handler.Update_Datatable(X_Axis_Configuration_Grid, X_Axis_Configuration_Table)
        ''Grid_Handler.Update_Datatable(Y_Axis_Configuration_Grid, Y_Axis_Configuration_Table)
        ''Grid_Handler.Update_Datatable(Z_Axis_Configuration_Grid, Z_Axis_Configuration_Table)

        'Tables.Add(Table_Param)
        ''Tables.Add(X_Axis_Configuration_Table)
        ''Tables.Add(Y_Axis_Configuration_Table)
        ''Tables.Add(Z_Axis_Configuration_Table)

        'Data_File.Save_Tables_To_Delimited_File(Tables, Base_Path & CNC_Configuration_File_Name, False)
        'Flash_Message("Parameters saved.")

    End Sub



End Class
