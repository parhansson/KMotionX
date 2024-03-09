Imports System.IO

Public Class Class_Jobs

    Private WithEvents ts As ToolStrip
    Private WithEvents ts_2 As ToolStrip
    Private WithEvents tree As TreeView
    Private Grid As DataGridView

    Private Data_File As New Class_Data_File
    Private Data_Table As New DataTable

    Public Sub Initialize(Tree_View As TreeView, Data_Grid_View As DataGridView, Tool_Bar As ToolStrip, Tool_Bar_2 As ToolStrip)
        tree = Tree_View
        Grid = Data_Grid_View
        Grid.Font = Grid_Font
        Tree_View.Font = Grid_Font

        ts = Tool_Bar
        ts_2 = Tool_Bar_2

        Tree_View.Nodes.Clear()

        Dim B As ToolStripDropDownButton = ts.Items(0)

        For I = 0 To B.DropDownItems.Count - 1
            RemoveHandler B.DropDownItemClicked, AddressOf DropDownItem_Clicked
            AddHandler B.DropDownItemClicked, AddressOf DropDownItem_Clicked
        Next

        Load_Jobs()

    End Sub

    Public Sub Load_Jobs()
        Data_File.Load_Table_From_Delimited_File(Data_Table, Settings_Path & "\Data_Base\Jobs.txt", "", True)
        Data_File.Load_Grid_From_Table(Grid, Data_Table, "")
        Data_File.Load_Tree_From_Table(tree, Data_Table, "Jobs", 3)
        tree.ExpandAll()
    End Sub

    Public Sub Save_Jobs()
        Data_File.Save_Grid_To_Table(Grid, Data_Table)
        Data_File.Save_Table_To_Delimited_File(Data_Table, Settings_Path & "\Data_Base\Jobs.txt", True)
    End Sub

    Private Sub Setup_Add_Button()
        Select Case tree.SelectedNode.Name
            Case ""
        End Select
    End Sub

    Private Sub DropDownItem_Clicked(sender As Object, e As System.Windows.Forms.ToolStripItemClickedEventArgs)
        Select Case e.ClickedItem.Name
            Case "Job"

            Case "Operation"
        End Select
    End Sub


    Private Sub Load()

    End Sub

    Private Sub Save()

    End Sub

    ' Set_Color(tv_1.Nodes(0), Color.White)
    'tv_1.CollapseAll()
    'tv_1.Nodes(0).Expand()

#Region "Toolstrip and Radio Buttons"

    Private Sub ts_ItemClicked(sender As Object, e As ToolStripItemClickedEventArgs) Handles ts.ItemClicked

        Dim Node As TreeNode
        Node = tree.SelectedNode

        Select Case e.ClickedItem.Text
            Case "Job"

            Case "Add Job"
                tree.Nodes.Add("Job")

            Case "Add Item"
                If Not IsNothing(Node) Then
                    Node.Nodes.Add("Item")
                End If

            Case "Move Up"
            Case "Move Down"
            Case "Delete"
        End Select

        tree.ExpandAll()
        tree.Refresh()

    End Sub


    Private Sub ts_2_ItemClicked(sender As Object, e As ToolStripItemClickedEventArgs) Handles ts_2.ItemClicked
        Dim Node As TreeNode
        Node = tree.SelectedNode


        Select Case e.ClickedItem.Text
            Case "Expand"
                Node.Expand()
                For N = 0 To Node.Nodes.Count - 1
                    Node.Nodes(N).Expand()
                Next
            Case "Collapse"
                For N = 0 To Node.Nodes.Count - 1
                    Node.Nodes(N).Collapse()
                Next

        End Select


    End Sub

#End Region

    '    Private Sub tv_1_NodeMouseClick(sender As Object, e As System.Windows.Forms.TreeNodeMouseClickEventArgs) Handles tv_1.NodeMouseClick
    '        If Not IsNothing(e.Node) Then
    '            Dim P() As String
    '            P = e.Node.Text.Split(" ")
    '            ts_Where_Used_Pn.Text = P(0)
    '            ts_Tree_1_Pn.Text = e.Node.Text
    '            Compare_Node_1 = e.Node
    '        End If
    '    End Sub

    '    Private Sub tv_2_NodeMouseClick(sender As Object, e As System.Windows.Forms.TreeNodeMouseClickEventArgs) Handles tv_2.NodeMouseClick
    '        If Not IsNothing(e.Node) Then
    '            Dim P() As String
    '            P = e.Node.Text.Split(" ")
    '            ts_Where_Used_Pn.Text = P(0)
    '            ts_Tree_2_Pn.Text = e.Node.Text
    '            Compare_Node_2 = e.Node
    '        End If
    '    End Sub

    '    Private Sub tv_3_NodeMouseClick(sender As Object, e As System.Windows.Forms.TreeNodeMouseClickEventArgs) Handles tv_3.NodeMouseClick
    '        If Not IsNothing(e.Node) Then
    '            Dim P() As String
    '            P = e.Node.Text.Split(" ")
    '            ts_Where_Used_Pn.Text = P(0)
    '            ts_Tree_3_Pn.Text = e.Node.Text
    '            Compare_Node_3 = e.Node
    '        End If
    '    End Sub

    '#End Region



    '    Private Sub Compare(Node_1 As TreeNode, Optional Node_2 As TreeNode = Nothing, Optional Node_3 As TreeNode = Nothing)
    '        Dim Pn As String
    '        Dim Found_1 As Boolean
    '        Dim Found_2 As Boolean
    '        Dim Found_3 As Boolean

    '        If tv_1.Nodes.Count > 0 Then Set_Color(tv_1.Nodes(0), Color.White)
    '        If tv_2.Nodes.Count > 0 Then Set_Color(tv_2.Nodes(0), Color.White)
    '        If tv_3.Nodes.Count > 0 Then Set_Color(tv_3.Nodes(0), Color.White)

    '        'Compare Node_1 to Node_2 and Node_3
    '        If Not IsNothing(Node_1) Then
    '            For I = 0 To Node_1.Nodes.Count - 1
    '                Pn = Node_1.Nodes(I).Text
    '                If Not IsNothing(Node_2) Then
    '                    Found_2 = Search_Node(Pn, Node_2)
    '                    If Not Found_2 Then
    '                        If Node_1.Nodes(I).Level > 1 Then
    '                            Node_1.Nodes(I).BackColor = Found_Color
    '                        Else
    '                            Node_1.Nodes(I).BackColor = Fault_Color
    '                        End If
    '                    End If
    '                End If

    '                If Not IsNothing(Node_3) Then
    '                    Found_3 = Search_Node(Pn, Node_3)
    '                    If Not Found_3 Then
    '                        If Node_1.Nodes(I).Level > 1 Then
    '                            Node_1.Nodes(I).BackColor = Found_Color
    '                        Else
    '                            Node_1.Nodes(I).BackColor = Fault_Color
    '                        End If
    '                    End If
    '                End If
    '            Next
    '        End If


    '        'Compare Node_2 to Node_1 and Node_3
    '        If Not IsNothing(Node_2) Then
    '            For I = 0 To Node_2.Nodes.Count - 1
    '                Pn = Node_2.Nodes(I).Text
    '                If Not IsNothing(Node_1) Then
    '                    Found_1 = Search_Node(Pn, Node_1)
    '                    If Not Found_1 Then
    '                        If Node_2.Nodes(I).Level > 1 Then
    '                            Node_2.Nodes(I).BackColor = Found_Color
    '                        Else
    '                            Node_2.Nodes(I).BackColor = Fault_Color
    '                        End If
    '                    End If
    '                End If

    '                If Not IsNothing(Node_3) Then
    '                    Found_3 = Search_Node(Pn, Node_3)
    '                    If Not Found_3 Then
    '                        If Node_2.Nodes(I).Level > 1 Then
    '                            Node_2.Nodes(I).BackColor = Found_Color
    '                        Else
    '                            Node_2.Nodes(I).BackColor = Fault_Color
    '                        End If
    '                    End If

    '                End If
    '            Next
    '        End If

    '        'Compare Node_3 to Node_1 and Node_2
    '        If Not IsNothing(Node_3) Then
    '            For I = 0 To Node_3.Nodes.Count - 1
    '                Pn = Node_3.Nodes(I).Text
    '                If Not IsNothing(Node_1) Then
    '                    Found_1 = Search_Node(Pn, Node_1)
    '                    If Not Found_1 Then
    '                        If Node_3.Nodes(I).Level > 1 Then
    '                            Node_3.Nodes(I).BackColor = Found_Color
    '                        Else
    '                            Node_3.Nodes(I).BackColor = Fault_Color
    '                        End If
    '                    End If
    '                End If

    '                If Not IsNothing(Node_2) Then
    '                    Found_2 = Search_Node(Pn, Node_2)
    '                    If Not Found_2 Then
    '                        If Node_3.Nodes(I).Level > 1 Then
    '                            Node_3.Nodes(I).BackColor = Found_Color
    '                        Else
    '                            Node_3.Nodes(I).BackColor = Fault_Color
    '                        End If
    '                    End If
    '                End If
    '            Next

    '        End If

    '    End Sub

    '    Private Function Search_Node(Part_Number As String, Node As TreeNode) As Boolean
    '        Dim Found As Boolean = False

    '        For I = 0 To Node.Nodes.Count - 1
    '            If Node.Nodes(I).Text = Part_Number Then
    '                Found = True
    '                Exit For
    '            End If
    '        Next

    '        Return Found

    '    End Function

    '    Private Sub Set_Color(Node As TreeNode, Node_Color As Color)

    '        For I = 0 To Node.Nodes.Count - 1
    '            If Node.Level > 0 Then
    '                Node.Nodes(I).BackColor = Node_Color
    '            End If
    '            If Node.Nodes(I).Nodes.Count > 0 Then
    '                Set_Color(Node.Nodes(I), Node_Color)
    '            End If
    '        Next

    '    End Sub


    '    Private Sub Find_Where_Used(Part_Number As String, Node As TreeNode)

    '        If Node.Nodes.Count < 1 Then Exit Sub

    '        'Set_Color(Node.Nodes(0), Color.White)

    '        If Node.Nodes.Count > 0 Then

    '            For I = 0 To Node.Nodes.Count - 1

    '                Dim P() As String
    '                P = Node.Nodes(I).Text.Split(" ")

    '                If P(0) = Part_Number Then
    '                    Node.Nodes(I).BackColor = Found_Color
    '                    Node.Nodes(I).Parent.Expand()
    '                End If

    '                If Node.Nodes(I).Nodes.Count > 0 Then
    '                    Find_Where_Used(Part_Number, Node.Nodes(I))
    '                End If

    '            Next

    '        End If

    '    End Sub




End Class
