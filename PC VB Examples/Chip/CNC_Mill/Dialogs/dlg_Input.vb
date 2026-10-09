
Public Class dlg_Input

    Public Input_Value As String
    Public Default_Value As String = ""
    Public Current_Position As String = ""
    Public Surface As String = ""
    Public Prompt As String = ""
    Public Note As String = ""

    Private Is_Number As Boolean = False

    Private Sub Dlg_Input_Shown(sender As Object, e As EventArgs) Handles Me.Shown

        If Note <> "" Then
            txt_Note.Text = Note
            txt_Note.Visible = True
        Else
            txt_Note.Visible = False
        End If

        If Prompt <> "" Then
            lab_Prompt.Text = Prompt
        Else
            lab_Prompt.Text = "Enter value"
        End If

        If Default_Value <> "" Then
            btn_Default.Visible = True
            txt_Default.Visible = True
            lab_Prompt.Text = "Enter value or click a button to the right"
        Else
            btn_Default.Visible = False
            txt_Default.Visible = False
        End If

        If Current_Position <> "" Then
            btn_Current_Position.Visible = True
            txt_Current_Position.Visible = True
            lab_Prompt.Text = "Enter value or click a button to the right"
        Else
            btn_Current_Position.Visible = False
            txt_Current_Position.Visible = False
        End If

        If Surface <> "" Then
            btn_Surface.Visible = True
            txt_Surface.Visible = True
            lab_Prompt.Text = "Enter value or click a button to the right"
        Else
            btn_Surface.Visible = False
            txt_Surface.Visible = False
        End If

        If IsNumeric(Input_Value) Then
            Is_Number = True
            txt_Value.TextAlign = HorizontalAlignment.Right
        End If

        txt_Value.Text = Input_Value
        txt_Default.Text = Default_Value
        txt_Current_Position.Text = Current_Position
        txt_Surface.Text = Surface

        txt_Value.Focus()
        txt_Value.SelectAll()

    End Sub

    Private Sub btn_Cancel_Click(sender As Object, e As EventArgs) Handles btn_Cancel.Click
        DialogResult = Windows.Forms.DialogResult.Cancel
        Close()
    End Sub

    Private Sub btn_OK_Click(sender As Object, e As EventArgs) Handles btn_OK.Click
        'If Number_Check(txt_Value.Text) Then
        Input_Value = txt_Value.Text
            DialogResult = Windows.Forms.DialogResult.OK
            Close()
        ' End If
    End Sub

    Private Sub btn_Default_Click(sender As Object, e As EventArgs) Handles btn_Default.Click
        If Number_Check(txt_Default.Text) Then
            Input_Value = Default_Value
            DialogResult = Windows.Forms.DialogResult.OK
            Close()
        End If
    End Sub

    Private Sub btn_Current_Position_Click(sender As Object, e As EventArgs) Handles btn_Current_Position.Click
        If Number_Check(txt_Current_Position.Text) Then
            Input_Value = Current_Position
            DialogResult = Windows.Forms.DialogResult.OK
            Close()
        End If
    End Sub

    Private Sub btn_Surface_Click(sender As Object, e As EventArgs) Handles btn_Surface.Click
        If Number_Check(txt_Surface.Text) Then
            Input_Value = Surface
            DialogResult = Windows.Forms.DialogResult.OK
            Close()
        End If
    End Sub

    Private Sub Dlg_Input_KeyDown(sender As Object, e As KeyEventArgs) Handles Me.KeyDown
        If e.KeyCode = Keys.Enter Then
            Input_Value = txt_Value.Text
            DialogResult = Windows.Forms.DialogResult.OK
            Close()
        End If
    End Sub

    Private Function Number_Check(Value As String) As Boolean
        If Is_Number Then
            If (Value = "") Or (Not IsNumeric(Value)) Then
                Show_Error("Value not a number.")
                Return False
            End If
        End If
        Return True
    End Function

End Class