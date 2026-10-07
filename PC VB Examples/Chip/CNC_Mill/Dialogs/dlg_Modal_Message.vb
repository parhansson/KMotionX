
Public Class dlg_Modal_Message

    Public Overloads Sub Show(Message As String)
        txt_Prompt.Text = Message
        Me.Width = txt_Prompt.Width + 50
        Show()
    End Sub

End Class