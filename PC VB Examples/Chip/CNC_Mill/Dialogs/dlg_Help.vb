
Public Class dlg_Help



    Public Overloads Sub Show(Message As String)
        txt_Message.Text = Message
        Show()
    End Sub

    Private Sub btn_OK_Click(sender As Object, e As EventArgs) Handles btn_OK.Click
        Close()
    End Sub

End Class