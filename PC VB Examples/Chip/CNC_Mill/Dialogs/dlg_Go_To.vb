
Public Class dlg_Go_To

    Public Parameter_Name As String
    Public Command As String
    Public Command_Value As String
    Public X_1 As String
    Public X_2 As String
    Public Y_1 As String
    Public Y_2 As String
    Public Z_1 As String
    Public Z_2 As String
    Public Feed_X_1 As String
    Public Feed_X_2 As String
    Public Feed_Y_1 As String
    Public Feed_Y_2 As String
    Public Feed_Z_1 As String
    Public Feed_Z_2 As String

    Private Sub dlg_Go_To_Shown(sender As Object, e As EventArgs) Handles Me.Shown
        txt_Name.Text = Parameter_Name
        txt_Command_Value.Text = Command_Value
        txt_X_1.Text = Get_Number(X_1)
        txt_X_2.Text = Get_Number(X_2)
        txt_Y_1.Text = Get_Number(Y_1)
        txt_Y_2.Text = Get_Number(Y_2)
        txt_Z_1.Text = Get_Number(Z_1)
        txt_Z_2.Text = Get_Number(Z_2)
        txt_Feed_X_1.Text = Get_Number(Feed_X_1)
        txt_Feed_X_2.Text = Get_Number(Feed_X_2)
        txt_Feed_Y_1.Text = Get_Number(Feed_Y_1)
        txt_Feed_Y_2.Text = Get_Number(Feed_Y_2)
        txt_Feed_Z_1.Text = Get_Number(Feed_Z_1)
        txt_Feed_Z_2.Text = Get_Number(Feed_Z_2)

        Dim I As Chip_C_Interface.enum_Commands
        For I = 0 To Chip_C_Interface.enum_Commands.Last - 1
            cmb_Command.Items.Add(I.ToString)
        Next
        cmb_Command.Text = Command

    End Sub

    Private Sub btn_OK_Click(sender As Object, e As EventArgs) Handles btn_OK.Click
        Parameter_Name = txt_Name.Text
        Command_Value = txt_Command_Value.Text
        Command = cmb_Command.Text
        X_1 = Put_Number(txt_X_1.Text, 4)
        X_2 = Put_Number(txt_X_2.Text, 4)
        Y_1 = Put_Number(txt_Y_1.Text, 4)
        Y_2 = Put_Number(txt_Y_2.Text, 4)
        Z_1 = Put_Number(txt_Z_1.Text, 4)
        Z_2 = Put_Number(txt_Z_2.Text, 4)
        Feed_X_1 = Put_Number(txt_Feed_X_1.Text, 2)
        Feed_X_2 = Put_Number(txt_Feed_X_2.Text, 2)
        Feed_Y_1 = Put_Number(txt_Feed_Y_1.Text, 2)
        Feed_Y_2 = Put_Number(txt_Feed_Y_2.Text, 2)
        Feed_Z_1 = Put_Number(txt_Feed_Z_1.Text, 2)
        Feed_Z_2 = Put_Number(txt_Feed_Z_2.Text, 2)
        DialogResult = DialogResult.OK
        Close()
    End Sub

    Private Sub btn_Cancel_Click(sender As Object, e As EventArgs) Handles btn_Cancel.Click
        DialogResult = DialogResult.Cancel
        Close()
    End Sub

    Private Sub txt_X_1_Mouse_Click(sender As Object, e As System.Windows.Forms.MouseEventArgs) Handles txt_X_1.MouseClick
        txt_X_1.Text = Get_User_Input(txt_X_1.Text, 0, FormatNumber(Kflop.X_Abs, 4))
    End Sub

    Private Sub txt_X_2_Mouse_Click(sender As Object, e As System.Windows.Forms.MouseEventArgs) Handles txt_X_2.MouseClick
        txt_X_2.Text = Get_User_Input(txt_X_2.Text, 0, FormatNumber(Kflop.X_Abs, 4))
    End Sub

    Private Sub txt_Y_1_Mouse_Click(sender As Object, e As System.Windows.Forms.MouseEventArgs) Handles txt_Y_1.MouseClick
        txt_Y_1.Text = Get_User_Input(txt_Y_1.Text, 0, FormatNumber(Kflop.Y_Abs, 4))
    End Sub

    Private Sub txt_Y_2_Mouse_Click(sender As Object, e As System.Windows.Forms.MouseEventArgs) Handles txt_Y_2.MouseClick
        txt_Y_2.Text = Get_User_Input(txt_Y_2.Text, 0, FormatNumber(Kflop.Y_Abs, 4))
    End Sub

    Private Sub txt_Z_1_Mouse_Click(sender As Object, e As System.Windows.Forms.MouseEventArgs) Handles txt_Z_1.MouseClick
        txt_Z_1.Text = Get_User_Input(txt_Z_1.Text, 0, FormatNumber(Kflop.Z_Abs, 4))
    End Sub

    Private Sub txt_Z_2_Mouse_Click(sender As Object, e As System.Windows.Forms.MouseEventArgs) Handles txt_Z_2.MouseClick
        txt_Z_2.Text = Get_User_Input(txt_Z_2.Text, 0, FormatNumber(Kflop.Z_Abs, 4))
    End Sub

    Private Sub cmb_Command_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cmb_Command.SelectedIndexChanged
        txt_Command_Value.Text = cmb_Command.SelectedIndex
    End Sub

End Class