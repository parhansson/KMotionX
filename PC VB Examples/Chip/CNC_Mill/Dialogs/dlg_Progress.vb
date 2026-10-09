Public Class dlg_Progress

    Public Sub Prompt(P As String)
        txt_Prompt.Text = P
    End Sub

    Public Overloads Sub Show(Prompt As String)
        Me.Top = Main_Form.Top + 100
        Me.Left = Main_Form.Left + (Main_Form.Width / 2) - (Me.Width / 2)
        txt_Prompt.Text = Prompt
        bar.Focus()
        Show()
        Me.Refresh()
    End Sub

    Public Sub Show_Dialog(Prompt As String)
        Me.Top = Main_Form.Top
        Me.Left = Main_Form.Left
        txt_Prompt.Text = Prompt
        bar.Focus()
        ShowDialog()
        Me.Refresh()
    End Sub

    Public Sub Update_Progress()
        bar.PerformStep()
    End Sub

    Public Sub Set_Value(Value As Integer)
        bar.Value = Value
    End Sub

End Class