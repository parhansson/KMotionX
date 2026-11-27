
Public Class dlg_Fixture

    Public Fixture As String = ""

    Public Sub Add_Fixture(Fixture As String)
        grd_Fixtures.SuspendLayout()
        grd_Fixtures.Rows.Add(Fixture)
        grd_Fixtures.ResumeLayout()
    End Sub

    Private Sub btn_OK_Click(sender As Object, e As EventArgs) Handles btn_OK.Click
        Fixture = grd_Fixtures.CurrentRow.Cells(0).Value
        DialogResult = DialogResult.OK
        Close()
    End Sub

    Private Sub btn_Cancel_Click(sender As Object, e As EventArgs) Handles btn_Cancel.Click
        DialogResult = DialogResult.Cancel
        Close()
    End Sub

End Class