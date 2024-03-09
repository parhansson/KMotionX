Public Class dlg_Key_Test

    Private WithEvents Key_Board_Hook As New Class_Keyboard_Hook

    Private Sub dlg_Key_Test_Shown(sender As Object, e As EventArgs) Handles Me.Shown

    End Sub

    Private Sub btn_Clear_Click(sender As Object, e As EventArgs) Handles btn_Clear.Click
        txt_Trace.Text = ""
        txtz_Output.Text = ""
    End Sub

    Private Sub Hot_Key_Event(ByVal Command As String) Handles Key_Board_Hook.Hot_Key_Event
        txt_Trace.Text &= Command & vbCrLf
    End Sub

End Class