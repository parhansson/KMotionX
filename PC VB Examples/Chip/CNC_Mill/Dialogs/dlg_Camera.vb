
Public Class dlg_Camera

    Public Form_Top As Integer
    Public Form_Left As Integer
    Public Form_Width As Integer
    Public Form_Height As Integer

    Private Sub Camera_Click(sender As Object, e As EventArgs) Handles Me.Click
        Camera_Controls.Top = 0
        Camera_Controls.Left = 0
    End Sub

    Private Sub Mouse_Wheel(sender As Object, e As MouseEventArgs) Handles Me.MouseWheel
        Camera_Controls.Mouse_Wheel_Handler(e)
    End Sub

    Private Sub View_Port_Click(sender As Object, e As EventArgs) Handles View_Port.Click
        ' Me.Hide()
    End Sub

   
End Class