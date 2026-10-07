Public Class dlg_Pendant

    Public Message As String

    Private Sub dlg_Pendant_Shown(sender As Object, e As EventArgs) Handles Me.Shown
        txt_Message.Text = Message
    End Sub


    Private Sub dlg_Pendant_KeyDown(sender As Object, e As KeyEventArgs) Handles Me.KeyDown
        Dim K As String = ""
        Static Dim Key_Down_Symbol As Object = Nothing

        If e.Control Then
            If e.Alt Then
                K = e.KeyCode.ToString
                Select Case e.KeyCode.ToString
                    Case "Menu"

                    Case "D7"
                        DialogResult = Windows.Forms.DialogResult.OK
                        Close()
                    Case Else
                        DialogResult = Windows.Forms.DialogResult.Cancel
                        Close()
                End Select
            End If
        End If

    End Sub

    Private Sub btn_OK_Click(sender As Object, e As EventArgs) Handles btn_OK.Click
        DialogResult = Windows.Forms.DialogResult.OK
        Close()
    End Sub

    Private Sub btn_Cancel_Click(sender As Object, e As EventArgs) Handles btn_Cancel.Click
        DialogResult = Windows.Forms.DialogResult.Cancel
        Close()
    End Sub


End Class