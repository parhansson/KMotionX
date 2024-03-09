Imports System.IO


Public Class ctrl_Product_Tree

    Public Products As New Class_Tree
    Private Initialized As Boolean


    Public Sub Initialize()

    End Sub

    Public Sub Finish()
        Products.Save_Tree(tree_Products, Chip_Build_Save_Path)
    End Sub

    Public Sub Load_Tree(File_Name As String, Time_Filter As Boolean, Folder_Filter As Boolean)
        Products.Load_Tree(tree_Products, File_Name, Time_Filter, Folder_Filter)
    End Sub

    Private Sub tree_Products_AfterSelect(sender As Object, e As System.Windows.Forms.TreeViewEventArgs) Handles tree_Products.AfterSelect
        If Not Initialized Then Exit Sub
        Nop()
    End Sub

    Private Sub tree_Products_DoubleClick(sender As Object, e As System.EventArgs) Handles tree_Products.DoubleClick
        Dim Node As TreeNode = tree_Products.SelectedNode
        Dim Node_Tag As Class_Tree.struct_Tag = Node.Tag

        Select Case Node_Tag.Type
            Case "Drawing"
                Dim dlg As New OpenFileDialog
                dlg.InitialDirectory = Node_Tag.Fields
                dlg.ShowDialog()

                If dlg.FileName <> "" Then
                    Dim Proc = New System.Diagnostics.Process
                    Proc.StartInfo.FileName = dlg.FileName
                    Proc.StartInfo.Verb = "Open"
                    Try
                        Proc.Start()
                    Catch ex As Exception
                        Show_Error(ex.Message)
                    End Try
                End If

            Case "Operation"
                Dim dlg As New dlg_Edit_Operation
                dlg.Node = Node
                dlg.ShowDialog()

        End Select

    End Sub

    Private Sub ts_Tree_ItemClicked(sender As Object, e As ToolStripItemClickedEventArgs) Handles ts_Tree.ItemClicked
        Dim Selected_Node As TreeNode = tree_Products.SelectedNode
        Dim Parent As TreeNode = tree_Products.SelectedNode.Parent

        If e.ClickedItem.Text = "Save" Then
            Products.Save_Tree(tree_Products, Chip_Build_Save_Path)
            Exit Sub
        End If

        Select Case e.ClickedItem.Text

            Case "Edit"
                Products.Edit_Node(tree_Products.SelectedNode)

            Case "Insert Header"
                Products.Insert_Node(Selected_Node)

            Case "Insert Node"
                Products.Insert_Node(Selected_Node)

            Case "Copy"
                Products.Copy_Node(Selected_Node)

            Case "Delete"
                Products.Delete_Node(Selected_Node)

            Case "Move Up"
                If IsNothing(Parent) Then Exit Sub
                If Selected_Node.Index = 0 Then Exit Sub
                Parent.Nodes.RemoveByKey(Selected_Node.Name)
                Parent.Nodes.Insert(Selected_Node.Index - 1, Selected_Node)
                tree_Products.SelectedNode = Selected_Node
                Products.Changes_Made = True

            Case "Move Down"
                If IsNothing(Parent) Then Exit Sub
                If Selected_Node.Index = Parent.Nodes.Count - 1 Then Exit Sub
                Parent.Nodes.RemoveByKey(Selected_Node.Name)
                Parent.Nodes.Insert(Selected_Node.Index + 1, Selected_Node)
                Parent.Nodes.Find(Selected_Node.Name, True)
                tree_Products.SelectedNode = Selected_Node
                Products.Changes_Made = True

        End Select

        tree_Products.Refresh()

    End Sub

    Private Sub ts_Tree_Insert_Click(sender As Object, e As System.Windows.Forms.ToolStripItemClickedEventArgs)

        Select Case e.ClickedItem.Text
            Case "Header"
            Case "Product"
            Case "Part"
            Case "Route"
            Case "Operation"
            Case "Mount"
            Case "Task"
            Case "Fixture"
            Case "Tool"

        End Select

    End Sub
    Public Class Class_Tree

#Region "enums"

        Public Enum enum_Fields
            Path
            Type
            Time
            Name
            Params
        End Enum

        Public Enum enum_Params
            File
            Folder
        End Enum

#End Region

#Region "structures"

        Public Structure struct_Tag
            Public Type As String
            Public Name As String
            Public Time As String
            Public Fields As Object
            Public Pad As Integer
        End Structure

        Public Structure struct_Operation
            Public Tool As String
            Public Speed As String
            Public Fixed As Boolean
        End Structure

        Public Structure struct_Mount
            Public Fixture_Node As TreeNode
        End Structure

        Public Structure struct_Fixture_Mount
            Public X_Offset As String
            Public Y_Offset As String
            Public Z_Delta As String
        End Structure

        Public Structure struct_Tool
            Public Max_Speed As String
            Public Seat_X As String
            Public Seat_Y As String
            Public Seat_Z As String
            Public Measure_X As String
            Public Measure_Y As String
            Public Measure_Z As String
        End Structure

#End Region

#Region "Declarations"

        Public Default_Product_Folder As String = ""
        Public Changes_Made As Boolean = False

        Private Name_Pad As Integer = 32
        Private Param_Pad As Integer = 10

        Private Data_File As New Class_Data_File
        Private Fixture_Root_Node As TreeNode
        Private Tool_Root_Node As New TreeNode
        Private Fixture_Node As TreeNode
        Private Routing_Fixture_Node As TreeNode

#End Region

        Public Sub Load_Tree(ByRef Tree As TreeView, File_Name As String, Time_Filter As Boolean, Folder_Filter As Boolean)
            Dim Buffer() As String
            Dim L() As String
            Dim Tag As struct_Tag = Nothing
            Dim New_Node As TreeNode
            Dim Level_Pad As Integer

            Changes_Made = False

            Tree.BeginUpdate()

            Tree.PathSeparator = "."

            Buffer = File.ReadAllLines(File_Name)

            For B = 0 To Buffer.Count - 1
                If Buffer(B) <> "" Then
                    L = Buffer(B).Split("~")

                    If L(enum_Fields.Type) = "Setting" Then
                        Select Case L(enum_Fields.Type + 1)
                            Case "Default_Product_Folder"
                                Default_Product_Folder = L(enum_Fields.Type + 2)
                        End Select
                        GoTo Skip
                    End If

                    New_Node = Add_Node(Tree, L(enum_Fields.Path))

                    Level_Pad = 0
                    Select Case New_Node.Level
                        Case 0
                            Level_Pad = 14
                        Case 1
                            Level_Pad = 12
                        Case 2
                            Level_Pad = 10
                        Case 3
                            Level_Pad = 8
                        Case 4
                            Level_Pad = 6
                        Case 5
                            Level_Pad = 4
                        Case 6
                            Level_Pad = 2
                    End Select

                    If IsNothing(New_Node) Then
                        Dim Mess As String = "Add_Node -- Tree Node does not exist" & vbCrLf
                        For m = 0 To L.Count - 1
                            Mess &= L(m) & vbCrLf
                        Next
                        Show_Error(Mess)
                        Exit Sub
                    Else
                        Tag.Type = L(enum_Fields.Type)
                        Tag.Name = L(enum_Fields.Name)
                        Tag.Time = L(enum_Fields.Time)
                        Tag.Pad = Level_Pad

                        Select Case Tag.Type

                            Case "Header"

                                Select Case Tag.Name
                                    Case "Fixtures"
                                        Fixture_Root_Node = New_Node
                                    Case "Tools"
                                        Tool_Root_Node = New_Node
                                End Select

                            Case "Product"
                                Tag.Fields = L(enum_Fields.Params)

                            Case "Drawing"
                                Tag.Fields = L(enum_Fields.Params)

                            Case "Part"

                            Case "Route"
                                Tag.Fields = L(enum_Fields.Params)
                                Routing_Fixture_Node = New_Node

                            Case "Operation"
                                Dim Operation As New struct_Operation
                                Operation.Tool = L(enum_Fields.Params)
                                Operation.Speed = L(enum_Fields.Params + 1)
                                Operation.Fixed = L(enum_Fields.Params + 2)
                                Tag.Fields = Operation

                            Case "Mount"
                                Dim Mount As New struct_Mount
                                Mount.Fixture_Node = Routing_Fixture_Node
                                Tag.Fields = Mount

                            Case "Task"
                                Tag.Fields = L(enum_Fields.Params)

                            Case "Fixture"
                                Fixture_Node = New_Node

                            Case "Fixture_Mount"
                                Dim Mount As New struct_Fixture_Mount
                                Mount.X_Offset = L(enum_Fields.Params)
                                Mount.Y_Offset = L(enum_Fields.Params + 1)
                                Mount.Z_Delta = L(enum_Fields.Params + 2)
                                Tag.Fields = Mount

                            Case "Tool"
                                Dim Fields = New struct_Tool
                                Fields.Max_Speed = L(enum_Fields.Params)
                                Fields.Seat_X = L(enum_Fields.Params + 1)
                                Fields.Seat_Y = L(enum_Fields.Params + 2)
                                Fields.Seat_Z = L(enum_Fields.Params + 3)
                                Fields.Measure_X = L(enum_Fields.Params + 4)
                                Fields.Measure_Y = L(enum_Fields.Params + 5)
                                Fields.Measure_Z = L(enum_Fields.Params + 6)
                                Tag.Fields = Fields
                        End Select

                        New_Node.Tag = Tag
                        Format_Text(New_Node, Time_Filter, Folder_Filter)

                    End If

                End If

Skip:

            Next

            Tree.EndUpdate()

        End Sub

        Private Sub Format_Text(ByRef Node As TreeNode, Time_Filter As Boolean, Folder_Filter As Boolean)
            Dim Tag As struct_Tag = Node.Tag

            Node.Text = Tag.Name.PadRight(Name_Pad + Tag.Pad)

            Select Case Tag.Type

                Case "Product"
                    Node.Text &= "Folder : " & Tag.Fields

                Case "Drawing"
                    Node.Text &= "Folder : " & Tag.Fields

                Case "Route"
                    Node.Text &= "Fixture : " & Tag.Fields

                Case "Operation"
                    Dim Fields As struct_Operation = Tag.Fields
                    Node.Text &= "  Tool : " & Fields.Tool.PadRight(20) & "Speed : " & Fields.Speed.PadRight(4) & "Fixed : " & Fields.Fixed

                Case "Mount"

                Case "Task"
                    Node.Text &= "Folder : " & Tag.Fields

                Case "Header"
                    Select Case Tag.Name

                        Case "Mounts"
                            Node.Text = Tag.Name.PadRight(Name_Pad)
                            Dim Parent_Tag As struct_Tag = Node.Parent.Tag
                            If Parent_Tag.Type = "Fixture" Then
                                Node.Text &= "X Offset".PadRight(Param_Pad)
                                Node.Text &= "Y Offset".PadRight(Param_Pad)
                                Node.Text &= "Z Delta".PadRight(Param_Pad)
                            End If

                        Case "Tools"
                            Node.Text = Tag.Name.PadRight(Name_Pad)
                            Node.Text &= "Max Speed".PadRight(Param_Pad)
                            Node.Text &= "X Seat".PadRight(Param_Pad)
                            Node.Text &= "Y Seat".PadRight(Param_Pad)
                            Node.Text &= "Z Seat".PadRight(Param_Pad)
                            Node.Text &= "X Measure".PadRight(Param_Pad)
                            Node.Text &= "Y Measure".PadRight(Param_Pad)
                            Node.Text &= "Z Measure".PadRight(Param_Pad)
                    End Select

                Case "Tool"
                    Dim Fields As struct_Tool = Tag.Fields
                    Node.Text = Tag.Name.PadRight(Name_Pad)
                    Node.Text &= Fields.Max_Speed.PadRight(Param_Pad)
                    Node.Text &= Fields.Seat_X.PadRight(Param_Pad)
                    Node.Text &= Fields.Seat_Y.PadRight(Param_Pad)
                    Node.Text &= Fields.Seat_Z.PadRight(Param_Pad)
                    Node.Text &= Fields.Measure_X.PadRight(Param_Pad)
                    Node.Text &= Fields.Measure_Y.PadRight(Param_Pad)
                    Node.Text &= Fields.Measure_Z.PadRight(Param_Pad)

                Case "Fixture_Mount"
                    Dim Mount As struct_Fixture_Mount = Tag.Fields
                    Node.Text = Tag.Name.PadRight(Name_Pad)
                    Node.Text &= Mount.X_Offset.PadRight(Param_Pad)
                    Node.Text &= Mount.Y_Offset.PadRight(Param_Pad)
                    Node.Text &= Mount.Z_Delta.PadRight(Param_Pad)
            End Select

            If Time_Filter Then Node.Text &= Tag.Time

        End Sub

        Public Function Add_Node(ByRef Tree As TreeView, Name As String) As TreeNode
            Dim P() As String = Name.Split(".")
            Dim Parent_Node As TreeNode = Nothing
            Dim Parent_Name As String = ""
            Dim New_Node As TreeNode = Nothing

            If P.Count = 0 Then
                Show_Error("Add_Node -- No parameters")
                Return Nothing
            End If

            If P.Count = 1 Then
                Parent_Node = Tree.Nodes.Item(P(0))
                If IsNothing(Parent_Node) Then
                    New_Node = New TreeNode
                    New_Node.Name = P(0)
                    Tree.Nodes.Add(New_Node)
                End If
            Else

                For I = 0 To P.Count - 2
                    If I = 0 Then
                        Parent_Name &= P(I)
                    Else
                        Parent_Name &= "." & P(I)
                    End If
                Next

                Parent_Node = Find_Node(Tree.Nodes(P(0)), Parent_Name)

                If Not IsNothing(Parent_Node) Then
                    New_Node = New TreeNode
                    New_Node.Name = Name
                    Parent_Node.Nodes.Add(New_Node)
                End If

            End If

            Return New_Node

        End Function

        Private Function Find_Node(Target_Node As TreeNode, Name As String) As TreeNode

            If Target_Node.Name = Name Then
                Return Target_Node
            End If

            For I = 0 To Target_Node.Nodes.Count - 1
                If Target_Node.Nodes(I).Name = Name Then
                    Return Target_Node.Nodes(I)
                Else
                    If Target_Node.Nodes(I).Nodes.Count > 0 Then
                        Dim Node As TreeNode = Find_Node(Target_Node.Nodes(I), Name)
                        If Not IsNothing(Node) Then
                            Return Node
                        End If
                    End If
                End If
            Next

            Return Nothing

        End Function

        Public Sub Save_Tree(ByRef Tree As TreeView, ByVal File_Name As String)
            Data_File.Open_Output_File(File_Name)
            Data_File.Write_Line("0~Setting~Default_Product_Folder~" & Default_Product_Folder)
            For I = 0 To Tree.Nodes.Count - 1
                Save_Node(Tree.Nodes(I))
            Next
            Data_File.Close_Output_File()
            Changes_Made = False
            'Flash_Message("Tree saved to : " & File_Name)
        End Sub

        Private Sub Save_Node(ByVal Node As TreeNode)
            Dim Tag As struct_Tag = Node.Tag
            Dim Line As String = Node.Name & "~" & Tag.Type & "~" & Tag.Time
            Dim Key As String = ""

            Build_Key(Node, Key)
            Line = Key & "~" & Tag.Type & "~" & Tag.Time

            Select Case Tag.Type
                Case "Header"
                    Line &= "~" & Tag.Name

                Case "Product"
                    Line &= "~" & Tag.Name & "~" & Tag.Fields

                Case "Drawing"
                    Line &= "~" & Tag.Name & "~" & Tag.Fields

                Case "Part"
                    Line &= "~" & Tag.Name

                Case "Route"
                    Line &= "~" & Tag.Name & "~" & Tag.Fields

                Case "Operation"
                    Dim Operation As struct_Operation = Tag.Fields
                    Line &= "~" & Tag.Name & "~" & Operation.Tool & "~" & Operation.Speed & "~" & Operation.Fixed

                Case "Mount"
                    Line &= "~" & Tag.Name

                Case "Task"
                    Line &= "~" & Tag.Name & "~" & Tag.Fields

                Case "Fixture"
                    Line &= "~" & Tag.Name

                Case "Fixture_Mount"
                    Dim Mount As struct_Fixture_Mount = Tag.Fields
                    Line &= "~" & Tag.Name & "~" & Mount.X_Offset & "~" & Mount.Y_Offset & "~" & Mount.Z_Delta

                Case "Tool"
                    Dim Tool As struct_Tool = Tag.Fields
                    Line &= "~" & Tag.Name & "~" & Tool.Max_Speed &
                        "~" & Tool.Seat_X & "~" & Tool.Seat_Y & "~" & Tool.Seat_Z &
                        "~" & Tool.Measure_X & "~" & Tool.Measure_Y & "~" & Tool.Measure_Z
            End Select

            Data_File.Write_Line(Line)

            For I = 0 To Node.Nodes.Count - 1
                Save_Node(Node.Nodes(I))
            Next

        End Sub

        Sub Build_Key(ByVal Node As TreeNode, ByRef Key As String)
            If Key = "" Then
                Key = Trim(Node.Index)
            Else
                Key = Trim(Node.Index) & "." & Key
            End If
            If Not IsNothing(Node.Parent) Then Build_Key(Node.Parent, Key)
        End Sub

        Public Function Edit_Node(ByRef Node As TreeNode) As Boolean
            Dim Tag As Class_Tree.struct_Tag = Node.Tag
            Dim dlg As Object = Nothing

            If Tag.Type = "Header" Then
                Flash_Message("Cannot edit header")
                Return False
            End If

            Dim Node_Tag As Class_Tree.struct_Tag = Nothing
            Node_Tag = Node.Tag

            Select Case Node_Tag.Type
                Case "Product"
                    dlg = New dlg_Edit_File_Name
                Case "Drawing"
                    dlg = New dlg_Edit_File_Name
                Case "Part"
                    dlg = New dlg_Edit_File_Name
                Case "Route"
                    dlg = New dlg_Edit_Route
                Case "Operation"
                    dlg = New dlg_Edit_Operation
                Case "Mount"
                    dlg = New dlg_Select_Node
                Case "Task"
                    dlg = New dlg_Edit_File_Name
                Case "Fixture"
                    dlg = New dlg_Edit_File_Name
                Case "Fixture_Mount"
                    dlg = New dlg_Edit_Mount
                Case "Tool"
                    dlg = dlg_Edit_Tool
            End Select

            dlg.Node = Node
            dlg.ShowDialog()
            Format_Text(Node, False, False)
            If dlg.DialogResult = DialogResult.OK Then
                Changes_Made = True
                Return True
            End If
            Return False

        End Function

        Public Sub Delete_Node(ByRef Node As TreeNode)
            Dim Tag As struct_Tag = Node.Tag
            Message_Box.ShowDialog("This will delete node : " & Tag.Name & vbCrLf & "and all children." & vbCrLf & "Continue?", "", MessageBoxButtons.YesNo)
            Node.Parent.Nodes.Remove(Node)
            Changes_Made = True
        End Sub

        Public Sub Insert_Header(ByRef Node As TreeNode)
            Dim Node_Tag As struct_Tag = Node.Tag
            'Dim Node_Tool As struct_Tool
            Dim New_Node As New TreeNode
            Dim New_Tag As New struct_Tag
            Dim New_Tool As New struct_Tool
            Dim Insert_Type As String = ""

            New_Tag.Fields = New_Tool
            New_Node.Tag = New_Tag

            Select Case Node_Tag.Type
                Case "Header"
                    Select Case Node_Tag.Name
                        Case "Products"
                            Insert_Type = "Product"
                        Case "Mounts"
                            Insert_Type = "Fixture_Mount"
                        Case "Tools"
                            Insert_Type = "Tool"
                    End Select
                Case Else
                    Insert_Type = Node_Tag.Type
            End Select

            'cmb_Type.Items.Add("")

            Select Case Insert_Type
                Case "Product"


                Case "Drawing"

                Case "Part"

                Case "Route"

                Case "Operation"

                Case "Mount"

                Case "Task"

                Case "Fixture"

                Case "Fixture_Mount"
                    Dim dlg As New dlg_Edit_Mount
                    dlg.Node = Node
                    dlg.ShowDialog()
                    If dlg.DialogResult = DialogResult.OK Then

                    End If

                Case "Tool"
                    If Node_Tag.Type = "Header" Then
                        Dim Tool As New struct_Tool
                        Dim Tag As New struct_Tag

                        Tag = Node.Nodes(0).Tag
                        Tool = Tag.Fields

                        New_Node.Text = Node.Nodes(0).Text
                        New_Tag.Name = Tag.Name

                        New_Tool.Max_Speed = Tool.Max_Speed
                        New_Tool.Seat_X = Tool.Seat_X
                        New_Tool.Seat_Y = Tool.Seat_Y
                        New_Tool.Seat_Z = Tool.Seat_Z
                        New_Tool.Measure_X = Tool.Measure_X
                        New_Tool.Measure_Y = Tool.Measure_Y
                        New_Tool.Measure_Z = Tool.Measure_Z
                    Else
                        Dim Test_Node = Node.Clone
                        Dim dlgx As New dlg_Edit_Tool
                        dlgx.Node = Test_Node
                        dlgx.ShowDialog()

                        If dlgx.DialogResult = DialogResult.OK Then
                            Format_Text(Test_Node, False, False)
                            If Node_Tag.Type = "Header" Then
                                Node.Nodes.Insert(0, Test_Node)
                            Else
                                Node.Parent.Nodes.Insert(Node.Index + 1, Test_Node)
                            End If
                        End If


                        'Node_Tool = Node_Tag.Fields
                        'New_Tag.Name = Node_Tag.Name
                        'New_Tool.Max_Speed = Node_Tool.Max_Speed
                        'New_Tool.Seat_X = Node_Tool.Seat_X
                        'New_Tool.Seat_Y = Node_Tool.Seat_Y
                        'New_Tool.Seat_Z = Node_Tool.Seat_Z
                        'New_Tool.Measure_X = Node_Tool.Measure_X
                        'New_Tool.Measure_Y = Node_Tool.Measure_Y
                        'New_Tool.Measure_Z = Node_Tool.Measure_Z
                    End If

                    New_Tag.Type = "Tool"
                    New_Tag.Fields = New_Tool
                    New_Node.Tag = New_Tag

                    Dim dlg As New dlg_Edit_Tool
                    dlg.Node = New_Node
                    dlg.ShowDialog()
                    If dlg.DialogResult = DialogResult.OK Then
                        Format_Text(New_Node, False, False)
                        If Node_Tag.Type = "Header" Then
                            Node.Nodes.Insert(0, New_Node)
                        Else
                            Node.Parent.Nodes.Insert(Node.Index + 1, New_Node)
                        End If
                    End If

            End Select

            Changes_Made = True


        End Sub

        Public Sub Insert_Node(ByRef Node As TreeNode)
            Dim Node_Tag As struct_Tag = Node.Tag
            Dim New_Node As New TreeNode
            Dim New_Tag As New struct_Tag
            Dim Insert_Type As String = ""
            Dim Insert_After_Header As Boolean = True
            Dim Result As DialogResult = DialogResult.Cancel

            New_Tag.Type = Insert_Type
            New_Tag.Pad = Param_Pad
            New_Tag.Time = "0:0:0"

            If Node_Tag.Type = "Header" Then
                Insert_Type = Mid(Node_Tag.Name, 1, Node_Tag.Name.Length - 1)
                Insert_After_Header = True
            Else
                Insert_Type = Node_Tag.Type
                Insert_After_Header = False
            End If


            Select Case Insert_Type

                Case "Product"
                    Dim Folder As String = Default_Product_Folder
                    New_Tag.Name = "New Product"
                    New_Tag.Type = "Product"
                    New_Tag.Fields = Folder
                    New_Node.Tag = New_Tag
                    Dim dlg As New dlg_Edit_File_Name
                    dlg.Node = New_Node
                    Result = dlg.ShowDialog()

                Case "Drawing"
                    Dim Drawing As String = Default_Product_Folder
                    New_Tag.Name = "New Drawing"
                    New_Tag.Type = "Drawing"
                    New_Tag.Fields = Drawing
                    New_Node.Tag = New_Tag
                    Dim dlg As New dlg_Edit_File_Name
                    dlg.Node = New_Node
                    Result = dlg.ShowDialog()

                Case "Fixture"
                    New_Tag.Name = "New Fixture"
                    New_Node.Tag = New_Tag
                    New_Tag.Type = "Fixture"
                    Dim dlg As New dlg_Edit_File_Name
                    dlg.Node = New_Node
                    Result = dlg.ShowDialog()

                Case "Tool"
                    Dim Tool As New struct_Tool
                    New_Tag.Name = "New Tool"
                    New_Tag.Type = "Tool"
                    New_Tag.Fields = Tool
                    New_Tag.Pad = Param_Pad
                    New_Tag.Time = "0:0:0"
                    New_Node.Tag = New_Tag

                    Dim dlg As New dlg_Edit_Tool
                    dlg.Node = New_Node
                    Result = dlg.ShowDialog()

            End Select

            If Result = DialogResult.OK Then
                Format_Text(New_Node, False, False)
                If Insert_After_Header Then
                    Node.Nodes.Insert(0, New_Node)
                Else
                    Node.Parent.Nodes.Insert(Node.Index + 1, New_Node)
                End If

                Changes_Made = True
            End If


        End Sub

        Public Sub Copy_Node(ByRef Node As TreeNode)
            If IsNothing(Node) Then Exit Sub
            Dim New_Node As New TreeNode
            New_Node = Node.Clone
            Node.Parent.Nodes.Insert(Node.Index + 1, New_Node)
        End Sub

        Private Function Header_Exists(Header As String, Node As TreeNode) As Boolean
            Dim Tag As Class_Tree.struct_Tag = Node.Tag
            For N = 0 To Node.Nodes.Count - 1
                Tag = Node.Nodes(N).Tag
                If (Tag.Type = "Header") And Node.Nodes(N).Text = Header Then
                    Return True
                End If
            Next
            Return False
        End Function

        Public Sub Fill_Fixture_Combo(cmb As ComboBox)
            cmb.Items.Clear()
            Dim Tag As struct_Tag = Fixture_Root_Node.Tag
            For I = 0 To Fixture_Root_Node.Nodes.Count - 1
                Tag = Fixture_Root_Node.Nodes(I).Tag
                cmb.Items.Add(Tag.Name)
            Next
        End Sub

        Public Function Combo_Contains_Fixture(cmb As ComboBox, Fixture As String) As Boolean
            Dim Found As Boolean = False

            Dim Tag As struct_Tag = Fixture_Root_Node.Tag
            For I = 0 To Fixture_Root_Node.Nodes.Count - 1
                Tag = Fixture_Root_Node.Nodes(I).Tag
                If Tag.Name = Fixture Then Found = True
            Next

            If Found Then
                cmb.Text = Fixture
                Return True
            Else
                Show_Error("Unknown Fixture : " & Fixture)
                Return False
            End If
        End Function

        Public Sub Fill_Mount_Combo(ByRef cmb As ComboBox, ByRef Fixture As String)
            cmb.Items.Clear()
            Dim Tag As struct_Tag = Fixture_Root_Node.Tag
            For I = 0 To Fixture_Root_Node.Nodes.Count - 1
                Tag = Fixture_Root_Node.Nodes(I).Tag
                If Tag.Name = Fixture Then
                    Dim Header As TreeNode = Fixture_Root_Node.Nodes(I).Nodes(0)
                    For J = 0 To Header.Nodes.Count - 1
                        Dim Mount_Tag As Class_Tree.struct_Tag = Header.Nodes(J).Tag
                        cmb.Items.Add(Mount_Tag.Name)
                    Next
                End If
            Next
        End Sub

        Public Function Combo_Contains_Mount(cmb As ComboBox, Fixture As String, Mount As String) As Boolean
            Dim Found As Boolean = False

            Dim Tag As struct_Tag = Fixture_Root_Node.Tag
            For I = 0 To Fixture_Root_Node.Nodes.Count - 1
                Tag = Fixture_Root_Node.Nodes(I).Tag
                If Tag.Name = Fixture Then
                    Dim Header As TreeNode = Fixture_Root_Node.Nodes(I).Nodes(0)
                    For J = 0 To Header.Nodes.Count - 1
                        Dim Mount_Tag As Class_Tree.struct_Tag = Header.Nodes(J).Tag
                        If Mount_Tag.Name = Mount Then Found = True
                    Next
                End If
            Next

            If Found Then
                cmb.Text = Mount
                Return True
            Else
                Show_Error("Unknown Mount : " & Mount)
                Return False
            End If
        End Function

        Public Sub Fill_Tool_Combo(cmb As ComboBox)
            cmb.Items.Clear()
            Dim Tag As struct_Tag = Tool_Root_Node.Tag
            For I = 0 To Tool_Root_Node.Nodes.Count - 1
                Tag = Tool_Root_Node.Nodes(I).Tag
                cmb.Items.Add(Tag.Name)
            Next
        End Sub

        Public Function Combo_Contains_Tool(cmb As ComboBox, Tool As String) As Boolean
            Dim Found As Boolean = False

            Dim Tag As struct_Tag = Tool_Root_Node.Tag
            For I = 0 To Tool_Root_Node.Nodes.Count - 1
                Tag = Tool_Root_Node.Nodes(I).Tag
                If Tag.Name = Tool Then Found = True
            Next

            If Found Then
                cmb.Text = Tool
                Return True
            Else
                Show_Error("Unknown Tool : " & Tool)
                Return False
            End If
        End Function

    End Class





End Class
