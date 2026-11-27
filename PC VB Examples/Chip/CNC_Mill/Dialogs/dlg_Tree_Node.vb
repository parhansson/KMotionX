Public Class dlg_Tree_Node

    Private Sub ts_Exit_Click(sender As Object, e As EventArgs) Handles ts_Exit.Click
        DialogResult = Windows.Forms.DialogResult.OK
        Close()
    End Sub

    Private Sub dlg_Tree_Node_KeyDown(sender As Object, e As KeyEventArgs) Handles Me.KeyDown

        If e.KeyCode = Keys.Enter Then
            DialogResult = Windows.Forms.DialogResult.OK
            Close()
        End If
    End Sub

End Class