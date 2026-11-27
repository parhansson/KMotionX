Public Class dlg_Camera_Setup

#Region "Shown, Closing"

    Private Sub dlg_Setup_Shown(sender As Object, e As EventArgs) Handles Me.Shown
        Me.Top = 0
        Me.Left = 0
        Me.TopMost = True
        If IsNothing(Camera_Controls.Draw_Color) Then Camera_Controls.Draw_Color = Color.Red
        ts_Color.BackColor = Camera_Controls.Draw_Color
        Camera_Controls.Operation = dlg_Camera_Controls.enum_Operation.None
        Camera_Controls.Draw_Graticule(True)
    End Sub

#End Region

#Region "Toolstrip"

    Private Sub ts_Main_ItemClicked(sender As Object, e As ToolStripItemClickedEventArgs) Handles ts_Main.ItemClicked

        If e.ClickedItem.Text <> "Color" Then
            For I = 0 To ts_Main.Items.Count - 1
                ts_Main.Items(I).BackColor = Color.White
            Next
            e.ClickedItem.BackColor = Color.MistyRose
        End If

        Camera_Controls.Operation = dlg_Camera_Controls.enum_Operation.None

        Select Case e.ClickedItem.Text
            Case "Exit"
              Close()

            Case "Show Overlay"
                ts_Show_Overlay.Text = "Hide Overlay"
                dlg_Camera.Show()

            Case "Hide Overlay"
                ts_Show_Overlay.Text = "Show Overlay"
                dlg_Camera.Hide()

            Case "Top"
                Camera_Controls.Operation = dlg_Camera_Controls.enum_Operation.Top
                Flash_Message("Use mouse wheel to move bullseye frane up or down." & _
                              vbCrLf & "Cursor must be over toolbar or bullseye frame", 2)

            Case "Left"
                Camera_Controls.Operation = dlg_Camera_Controls.enum_Operation.Left
                Flash_Message("Use mouse wheel to move bullseye frame left or right." & _
                              vbCrLf & "Cursor must be over toolbar or bullseye frame", 2)

            Case "Size"
                Camera_Controls.Operation = dlg_Camera_Controls.enum_Operation.Size
                Flash_Message("Use mouse wheel to move bullseye frame bigger or smaller." & _
                              vbCrLf & "Cursor must be over toolbar or bullseye frame", 2)

            Case "Transparancy"
                Camera_Controls.Operation = dlg_Camera_Controls.enum_Operation.Transparancy
                Flash_Message("Use mouse wheel to move bullseye make more or less transparent." & _
                              vbCrLf & "Cursor must be over toolbar or bullseye frame", 2)
            Case "Fast"
                ts_Speed.Text = "Slow"
                Camera_Controls.Set_Speeed(1)
            Case "Slow"
                ts_Speed.Text = "Fast"
                Camera_Controls.Set_Speeed(10)

            Case "Color"
                Dim dlg As New ColorDialog
                dlg.AllowFullOpen = True
                ' Sets the initial color select to the current text color,
                dlg.Color = Camera_Controls.Draw_Color

                ' Update the text box color if the user clicks OK 
                If (dlg.ShowDialog() = Windows.Forms.DialogResult.OK) Then
                    Camera_Controls.Draw_Color = dlg.Color
                    ts_Color.BackColor = dlg.Color
                End If
                Camera_Controls.Draw_Graticule()

            Case "Help"
                Message_Box.ShowDialog("These functions are used to align the cross hairs with the camera program" & vbCrLf & _
                                       "and size the box that conains the cross hairs. Click on the Top, Left or Size" & vbCrLf & _
                                       "Buttons, then use the mouse wheel to make the adjustment." & vbCrLf & vbCrLf & _
                                       "The Slow/Fast button is used to set the rate of the mouse wheel. Use Fast to get" & vbCrLf & _
                                       "The adjustment close, then use Slow to fine tune.")

        End Select

    End Sub

#End Region

#Region "Mouse_Wheel"

    Private Sub Mouse_Wheel(sender As Object, e As MouseEventArgs) Handles Me.MouseWheel
        Camera_Controls.Mouse_Wheel_Handler(e)
    End Sub

#End Region

End Class