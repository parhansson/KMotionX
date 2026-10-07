Public Class dlg_Message_Box

    Dim btns As System.Windows.Forms.MessageBoxButtons

    Public Overloads Sub ShowDialog(Message As String, Message_Font As Font)
        txt_Message.Font = Message_Font
        txt_Message.Text = Message
        ShowDialog()
    End Sub

    Public Overloads Sub ShowDialog(Message As String, Optional Title As String = "Warning", _
                                    Optional Buttons As System.Windows.Forms.MessageBoxButtons = MessageBoxButtons.OK)

        Me.Top = 0
        Me.Left = 0

        Dim Fnt As New Font(" Microsoft Sans Serif", 11, FontStyle.Regular)

        Me.Text = Title
        txt_Message.Font = Fnt
        txt_Message.Text = Message
        txt_Message.Refresh()

        btns = Buttons

        btn_1.Visible = False
        btn_2.Visible = False
        btn_3.Visible = False

        ShowDialog()

    End Sub

    Private Sub dlg_Message_Box_Shown(sender As Object, e As EventArgs) Handles Me.Shown

        Select Case btns
            Case MessageBoxButtons.OK
                btn_2.Visible = True
                btn_2.Text = "OK"
                btn_2.Focus()

            Case MessageBoxButtons.OKCancel
                btn_1.Visible = True
                btn_1.Text = "OK"
                btn_3.Visible = True
                btn_3.Text = "Cancel"
                btn_1.Focus()

            Case MessageBoxButtons.YesNo
                btn_1.Visible = True
                btn_1.Text = "Yes"
                btn_3.Visible = True
                btn_3.Text = "No"
                btn_1.Focus()

            Case MessageBoxButtons.YesNoCancel
                btn_1.Visible = True
                btn_1.Text = "Yes"
                btn_2.Visible = True
                btn_2.Text = "No"
                btn_3.Visible = True
                btn_3.Text = "Cancel"
                btn_1.Focus()

        End Select

    End Sub

    Private Sub btn_1_Click(sender As Object, e As EventArgs) Handles btn_1.Click

        Select Case btns
            Case MessageBoxButtons.OKCancel
                DialogResult = Windows.Forms.DialogResult.OK

            Case MessageBoxButtons.YesNo
                DialogResult = Windows.Forms.DialogResult.Yes

            Case MessageBoxButtons.YesNoCancel
                DialogResult = Windows.Forms.DialogResult.Yes

        End Select

        Close()

    End Sub

    Private Sub btn_2_Click(sender As Object, e As EventArgs) Handles btn_2.Click

        Select Case btns
            Case MessageBoxButtons.OK
                DialogResult = Windows.Forms.DialogResult.OK

            Case MessageBoxButtons.YesNoCancel
                DialogResult = Windows.Forms.DialogResult.No

        End Select

        Close()

    End Sub

    Private Sub btn_3_Click(sender As Object, e As EventArgs) Handles btn_3.Click

        Select Case btns

            Case MessageBoxButtons.OKCancel
                DialogResult = Windows.Forms.DialogResult.Cancel

            Case MessageBoxButtons.YesNo
                DialogResult = Windows.Forms.DialogResult.No

            Case MessageBoxButtons.YesNoCancel
                DialogResult = Windows.Forms.DialogResult.Cancel

        End Select

        Close()

    End Sub



End Class