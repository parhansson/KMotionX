Public Class dlg_Startup

    Public Enum enum_Results
        Current
        Home_Z
        Home_All
    End Enum

    Public Result As enum_Results

    Private Sub dlg_Startup_Shown(sender As Object, e As EventArgs) Handles Me.Shown
        rad_Current.Checked = True
    End Sub

    Private Sub btn_OK_Click(sender As Object, e As EventArgs) Handles btn_OK.Click
        If rad_Current.Checked Then Result = enum_Results.Current
        If rad_Home_Z.Checked Then Result = enum_Results.Home_Z
        If rad_Home_All.Checked Then Result = enum_Results.Home_All
        DialogResult = Windows.Forms.DialogResult.OK
        Close()
    End Sub

End Class