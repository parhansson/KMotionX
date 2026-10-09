Public Class dlg_Material_Origin

    Public Material_Origin As String

    Private Sub dlg_Material_Origin_Shown(sender As Object, e As EventArgs) Handles Me.Shown
        Select Case Material_Origin
            Case "Top Left"
                rad_Top_Left.Checked = True
            Case "Top Right"
                rad_Top_Right.Checked = True
            Case "Bottom Left"
                rad_Bottom_Left.Checked = True
            Case "Bottom Right"
                rad_Bottom_Right.Checked = True
        End Select
    End Sub

    Private Sub btn_OK_Click(sender As Object, e As EventArgs) Handles btn_OK.Click
        Material_Origin = ""
        If rad_Top_Left.Checked Then Material_Origin = "Top Left"
        If rad_Top_Right.Checked Then Material_Origin = "Top Right"
        If rad_Bottom_Left.Checked Then Material_Origin = "Bottom Left"
        If rad_Bottom_Right.Checked Then Material_Origin = "Bottom Right"
        DialogResult = DialogResult.OK
        Close()
    End Sub

    Private Sub btn_Cancel_Click(sender As Object, e As EventArgs) Handles btn_Cancel.Click
        DialogResult = DialogResult.Cancel
        Close()
    End Sub

End Class