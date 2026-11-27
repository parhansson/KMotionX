Public Class dlg_Task_Parameters

    Public Offset_X As Double
    Public Offset_Y As Double
    Public Offset_Z As Double
    Public Material_Thickness As Double
    Public Material_Length As Double
    Public Material_Width As Double
    Public All_Operations As Boolean = False

    Private Sub dlg_Task_Parameters_Shown(sender As Object, e As EventArgs) Handles Me.Shown
        txt_Offset_X.Text = FormatNumber(Offset_X, 3)
        txt_Offset_Y.Text = FormatNumber(Offset_Y, 3)
        txt_Offset_Z.Text = FormatNumber(Offset_Z, 3)
        txt_Material_Thickness.Text = FormatNumber(Material_Thickness, 3)
        txt_Material_Length.Text = FormatNumber(Material_Length, 3)
        txt_Material_Width.Text = FormatNumber(Material_Width, 3)
        rad_This.Checked = True
    End Sub

    Private Sub btn_OK_Click(sender As Object, e As EventArgs) Handles btn_OK.Click
        Offset_X = txt_Offset_X.Text
        Offset_Y = txt_Offset_Y.Text
        Offset_Z = txt_Offset_Z.Text
        Material_Thickness = txt_Material_Thickness.Text
        Material_Length = txt_Material_Length.Text
        Material_Width = txt_Material_Width.Text
        All_Operations = rad_All.Checked
        DialogResult = DialogResult.OK
    End Sub

    Private Sub btn_Cancel_Click(sender As Object, e As EventArgs) Handles btn_Cancel.Click
        DialogResult = DialogResult.Cancel
    End Sub


End Class