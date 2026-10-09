
Public Class dlg_Select_Node

    'Public Node As TreeNode = Nothing
    'Public Mounts As List(Of String)

    'Private Node_Tag As ctrl_Product_Tree.Class_Tree.struct_Tag = Nothing

    'Private Sub dlg_Select_Mount_Shown(sender As Object, e As EventArgs) Handles Me.Shown
    '    Node_Tag = Node.Tag
    '    Dim Fields As ctrl_Product_Tree.Class_Tree.struct_Mount
    '    Fields = Node_Tag.Fields


    '    Select Case Node_Tag.Type
    '        Case "Header"

    '        Case "Mount"
    '            Dim Fixture_Node As TreeNode = Fields.Fixture_Node
    '            Dim Fixture_Tag As ctrl_Product_Tree.Class_Tree.struct_Tag = Fixture_Node.Tag
    '            Dim Fixture As String = Fixture_Tag.Fields

    '            txt_Name.Text = Fixture

    '            For I = 0 To Mounts.Count - 1
    '                cmb_Mount.Items.Add(Mounts(I))
    '            Next
    '            If cmb_Mount.Items.Contains(Fixture) Then cmb_Mount.Text = Fixture


    '            'If Product_Tree.Combo_Contains_Mount(cmb_Mount, Fixture, Node_Tag.Name) Then cmb_Mount.Text = Node_Tag.Name

    '    End Select


    'End Sub

    'Private Sub btn_OK_Click(sender As Object, e As EventArgs) Handles btn_OK.Click

    '    Select Case Node_Tag.Type
    '        Case "Header"
    '        Case "Mount"
    '            Node_Tag.Name = cmb_Mount.Text
    '    End Select

    '    Node.Tag = Node_Tag
    '    DialogResult = Windows.Forms.DialogResult.OK
    '    Close()
    'End Sub

    'Private Sub btn_Cancel_Click(sender As Object, e As EventArgs) Handles btn_Cancel.Click
    '    DialogResult = Windows.Forms.DialogResult.Cancel
    '    Close()
    'End Sub

End Class