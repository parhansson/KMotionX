Public Class dlg_Mounts

    Public Fixture As String = ""
    Public Mounts As List(Of String)
    Public Mount As String

    Private Sub dlg_Mounts_Shown(sender As Object, e As EventArgs) Handles Me.Shown
        Me.Text = "Mounts for fixture : " & Fixture
        For I = 0 To Mounts.Count - 1
            grd_Mounts.Rows.Add(Mounts(I))
        Next
    End Sub

    Private Sub btn_OK_Click(sender As Object, e As EventArgs) Handles btn_OK.Click
        Mount = grd_Mounts.CurrentRow.Cells(0).Value
        DialogResult = DialogResult.OK
        Close()
    End Sub

    Private Sub btn_Cancel_Click(sender As Object, e As EventArgs) Handles btn_Cancel.Click
        DialogResult = DialogResult.Cancel
        Close()
    End Sub

End Class