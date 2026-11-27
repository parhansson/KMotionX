
Public Class dlg_Camera_Controls

#Region "Declarations"

    Public Enum enum_Operation
        None
        Top
        Left
        Size
        Transparancy
        Radius
    End Enum

    Public Operation As enum_Operation

    Private g As Graphics
    Private Bit_Map As Bitmap
    Private ctrl As PictureBox

    Public Draw_Color As Color = Color.Red

    Private Cross As Boolean = True
    Private X_Cross As Boolean = True
    Private Circle As Boolean = True
    Public Radius As Integer = 10
    Public Cross_Angle As Single = 0

    Public Units_Per_Pixel As Single = -1

    Private Aspect_Ratio As Single
    Private X_dpi As Integer
    Private Y_dpi As Integer

    Private Start_X As Single = 0
    Private Start_Y As Single = 0
    Private Start_Z As Single = 0
    Private Speed As Integer = 10

    Private Distance As Single
    Private Angle As Single

    Private Ignore_Key_Down As Boolean = False

#End Region

#Region "Shown, closing"

    Private Sub Camera_Controls_Shown(sender As Object, e As EventArgs) Handles Me.Shown
        Cross = My.Settings.Camera_Cross
        X_Cross = My.Settings.Camera_X_Cross
        Circle = My.Settings.Camera_Circle
        Radius = My.Settings.Camera_Radius
        Draw_Color = My.Settings.Camera_Color
        Camera.Top = My.Settings.Camera_Top
        Camera.Left = My.Settings.Camera_Left
        Camera.Width = My.Settings.Camera_Width
        Camera.Height = My.Settings.Camera_Height
        Units_Per_Pixel = My.Settings.Camera_Units_Per_Pixel

        If Radius <= 20 Then Radius = 20
        If Draw_Color = Color.Empty Then Draw_Color = Color.Red

        If Camera.Width < 20 Then
            If Screen.PrimaryScreen.WorkingArea.Width >= 600 And _
               Screen.PrimaryScreen.WorkingArea.Height >= 600 Then
                Camera.Width = 600
                Camera.Height = 600
            Else
                Camera.Width = Screen.PrimaryScreen.WorkingArea.Height - 100
                Camera.Height = Camera.Width
            End If
        End If

        If Camera.Top <= 0 Then Camera.Top = (Screen.PrimaryScreen.WorkingArea.Height - Camera.Height) / 2
        If Camera.Left <= 0 Then Camera.Left = (Screen.PrimaryScreen.WorkingArea.Width - Camera.Height) / 2

        ctrl = Camera.View_Port
        g = ctrl.CreateGraphics()
        X_dpi = g.DpiX
        Y_dpi = g.DpiY
        Aspect_Ratio = Y_dpi / X_dpi

        Bit_Map = New Bitmap(Camera.View_Port.Width, Camera.View_Port.Height, g)
        g.Dispose()

        Camera.Show()
        Operation = enum_Operation.Radius
        ts_Radius.Text = Calc.Format_Number(Radius * Units_Per_Pixel)

        Draw_Graticule()

        For I = 0 To Me.Controls.Count - 1
            RemoveHandler Controls(I).GotFocus, AddressOf Focus_Handler
            AddHandler Controls(I).GotFocus, AddressOf focus_Handler

            'Select Case Controls(I).GetType.Name
            '    Case "Button"
            '        Dim btn As Button
            '        btn = Controls(I)
            '        RemoveHandler btn.MouseDown, AddressOf Control_Mouse_Down_Handler
            '        AddHandler btn.MouseDown, AddressOf Control_Mouse_Down_Handler
            '    Case "TextBox"
            '        Dim txt As TextBox
            '        txt = Controls(I)
            '        RemoveHandler txt.MouseDown, AddressOf Control_Mouse_Down_Handler
            '        AddHandler txt.MouseDown, AddressOf Control_Mouse_Down_Handler
            'End Select
        Next

        ts_Feed.Text = Main_Form.Text_Box_Jog_Rate_Selected.Text

        txt_Focus.Width = 0
        txt_Focus.Focus()

        Me.Focus()
    End Sub

    Private Sub Camera_Controls_FormClosing(sender As Object, e As FormClosingEventArgs) Handles Me.FormClosing
        My.Settings.Camera_Cross = Cross
        My.Settings.Camera_X_Cross = X_Cross
        My.Settings.Camera_Circle = Circle
        My.Settings.Camera_Radius = Radius
        My.Settings.Camera_Color = Draw_Color
        My.Settings.Camera_Top = Camera.Top
        My.Settings.Camera_Left = Camera.Left
        My.Settings.Camera_Width = Camera.Width
        My.Settings.Camera_Height = Camera.Height
        My.Settings.Camera_Units_Per_Pixel = Units_Per_Pixel
        My.Settings.Save()
        e.Cancel = True
        Camera.Hide()
        Me.Hide()
        Main_Form.Show()
    End Sub

#End Region

#Region "Tool Bar"

    Private Sub ts_Main_ItemClicked(sender As Object, e As ToolStripItemClickedEventArgs) Handles ts_Main.ItemClicked
        Operation = enum_Operation.Radius

        Select Case Trim(e.ClickedItem.Text)
            Case "Exit"
                Close()

            Case "Show Overlay"
                ts_Show_Overlay.Text = "Hide Overlay"
                Camera.Show()

            Case "Hide Overlay"
                ts_Show_Overlay.Text = "Show Overlay"
                Camera.Hide()

            Case "Cross"
                Cross = Not Cross

            Case "X"
                X_Cross = Not X_Cross

            Case "Circle"
                Circle = Not Circle

            Case "Fast"
                ts_Speed.Text = "Slow"
                Speed = 10

            Case "Slow"
                ts_Speed.Text = "Fast"
                Speed = 1

            Case "Set Radius"
                Dim dlg As New dlg_Input
                dlg.On_Top = True
                dlg.Input_Value = ""
                dlg.ShowDialog("Set Radius")
                If dlg.DialogResult = Windows.Forms.DialogResult.OK Then
                    Units_Per_Pixel = dlg.Input_Value / Radius
                End If
                ts_Radius.Text = Calc.Format_Number(Radius * Units_Per_Pixel)

            Case "Setup"
                Dim dlg As New dlg_Camera_Setup
                dlg.ShowDialog()
                Operation = enum_Operation.Radius

        End Select

        Draw_Graticule()


    End Sub

#End Region

#Region "Draw"

    Public Sub Draw_Graticule(Optional Draw_Cross As Boolean = False)
        Dim Drawing_Pen As New Pen(Draw_Color, 2)
        Dim Border_Pen As New Pen(Draw_Color, 10)

        ctrl = Camera.View_Port
        g = ctrl.CreateGraphics()
        Bit_Map = New Bitmap(Camera.View_Port.Width, Camera.View_Port.Height, g)
        g = Graphics.FromImage(Bit_Map)

        'Draw Border
        g.DrawLine(Border_Pen, 0, 0, Camera.View_Port.Width, 0) 'Top
        g.DrawLine(Border_Pen, 0, Camera.View_Port.Height, Camera.View_Port.Width, Camera.View_Port.Height) 'Bottom
        g.DrawLine(Border_Pen, 0, 0, 0, Camera.View_Port.Height) 'Left
        g.DrawLine(Border_Pen, Camera.View_Port.Width, 0, Camera.View_Port.Width, Camera.View_Port.Height) 'Right

        If Radius > Camera.Width Then Radius = Camera.Width
        If Radius > Camera.Height Then Radius = Camera.Height

        Dim X As Single = Camera.Width / 2
        Dim Y As Single = Camera.Height / 2
        Dim Center_X As Single = X - (Radius / 2) - 1
        Dim Center_Y As Single = Y - (Radius / 2) - 1


        If Cross Or Draw_Cross Then
            g.DrawLine(Drawing_Pen, X, 0, X, Camera.Height) 'Vertical
            g.DrawLine(Drawing_Pen, 0, Y, Camera.Width, Y) 'Horizontal
        End If

        If X_Cross Then
            g.DrawLine(Drawing_Pen, 0, 0, Camera.Width, Camera.Height) 'Vertical
            g.DrawLine(Drawing_Pen, 0, Camera.Height, Camera.Width, 0) 'Horizontal
        End If

        If Circle Then
            g.DrawEllipse(Drawing_Pen, Center_X, Center_Y, Radius, Radius)
        End If

        ctrl.Image = Bit_Map
        g.Dispose()
    End Sub

#End Region


    Public Sub Set_Speeed(Speed_Increment As Integer)
        Speed = Speed_Increment
    End Sub

#Region "Mouse"

    Private Sub Mouse_Wheel(sender As Object, e As MouseEventArgs) Handles Me.MouseWheel
        Mouse_Wheel_Handler(e)
    End Sub

    Private Sub Mouse_Down_Handler(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles Me.MouseDown
    End Sub

    Public Sub Mouse_Wheel_Handler(e As MouseEventArgs)
        Dim Inc As Integer = Speed

        Select Case Operation
            Case enum_Operation.None

            Case enum_Operation.Top
                If e.Delta > 0 Then Camera.Top += Inc Else Camera.Top -= Inc
                Draw_Graticule(True)

            Case enum_Operation.Left
                If e.Delta > 0 Then Camera.Left += Inc Else Camera.Left -= Inc
                Draw_Graticule(True)

            Case enum_Operation.Size
                If e.Delta > 0 Then
                    Camera.Width = Camera.Width + (2 * Inc)
                    Camera.Height = Camera.Width
                    Camera.Left -= Inc
                    Camera.Top -= Inc
                Else
                    Camera.Width = Camera.Width - (2 * Inc)
                    Camera.Height = Camera.Width
                    Camera.Left += Inc
                    Camera.Top += Inc
                End If
                Draw_Graticule(True)

            Case enum_Operation.Transparancy

                If e.Delta > 0 Then
                    Camera.Opacity += 0.1
                Else
                    If Camera.Opacity > 0.2 Then Camera.Opacity -= 0.1
                End If

                Draw_Graticule(True)

            Case enum_Operation.Radius
                If Circle Then
                    If e.Delta > 0 Then
                        Radius += Inc
                    Else
                        If Radius > 2 Then
                            Radius -= Inc
                        End If
                    End If
                End If

                If Units_Per_Pixel > 0 Then
                    ts_Radius.Text = Calc.Format_Number(Radius * Units_Per_Pixel)
                End If

                Draw_Graticule()

        End Select

    End Sub

#End Region

#Region "Interface to main form"

    Public Sub Update_Positions()
        ts_Feed.Text = Main_Form.Text_Box_Jog_Rate_Selected.Text

        txt_Absolute_X.Text = CNC.X.Abs
        txt_Absolute_Y.Text = CNC.Y.Abs
        txt_Absolute_Z.Text = CNC.Z.Abs

        txt_Work_X.Text = CNC.X.Work
        txt_Work_Y.Text = CNC.Y.Work
        txt_Work_Z.Text = CNC.Z.Work

        txt_Delta_X.Text = Calc.Format_Number(Math.Abs(CNC.X.Abs - Start_X))
        txt_Delta_Y.Text = Calc.Format_Number(Math.Abs(CNC.Y.Abs - Start_Y))
        txt_Delta_Z.Text = Calc.Format_Number(Math.Abs(CNC.Z.Abs - Start_Z))

        txt_Start_X.Text = Calc.Format_Number(Start_X)
        txt_Start_Y.Text = Calc.Format_Number(Start_Y)
        txt_Start_Z.Text = Calc.Format_Number(Start_Z)

        Calc.Cartesian_To_Polar(Start_X, Start_Y, CNC.X.Abs, CNC.Y.Abs, Distance, Angle)
        txt_Distance.Text = Calc.Format_Number(Distance)
        txt_Angle.Text = Math.Round(Angle, 0, MidpointRounding.AwayFromZero)


    End Sub

#End Region

#Region "Keyboard"

    Private Sub Focus_Handler(sender As Object, e As System.EventArgs)
        If sender.name <> "txt_Focus" Then
            txt_Focus.Focus()
        End If
    End Sub

    Private Sub txt_Focus_KeyDown(sender As Object, e As EventArgs) Handles txt_Focus.KeyDown
        Me_KeyDown(sender, e)
    End Sub

    Private Sub txt_Focus_Keyup(sender As Object, e As EventArgs) Handles txt_Focus.KeyUp
        Me_KeyUp(sender, e)
    End Sub

    Private Sub Me_KeyDown(sender As Object, e As KeyEventArgs) Handles Me.KeyDown

        If e.Alt And (e.KeyCode <> Keys.Menu) Then

            If Ignore_Key_Down Then
                Exit Sub
            End If
            Ignore_Key_Down = True


            If Display.Hot_Key_Handler(e, True) Then
                e.SuppressKeyPress = True
                e.Handled = True
                Exit Sub
            End If

        Else

            If e.KeyCode = Keys.Up Or e.KeyCode = Keys.Down Then
                Select Case e.KeyCode
                    Case Keys.Up
                        Radius += 1
                    Case Keys.Down
                        Radius -= 1
                End Select

                If Units_Per_Pixel > 0 Then ts_Radius.Text = Calc.Format_Number(Radius * Units_Per_Pixel)

                Draw_Graticule()
            End If

        End If


    End Sub

    Private Sub Me_KeyUp(sender As Object, e As System.Windows.Forms.KeyEventArgs) Handles Me.KeyUp
        Ignore_Key_Down = False

        If Display.Hot_Key_Handler(e, False) Then
            e.SuppressKeyPress = True
            e.Handled = True
        End If

    End Sub

    Private Sub ts_Radius_Click(sender As Object, e As EventArgs) Handles ts_Radius.Click
        txt_Focus.Focus()
    End Sub

    Private Sub ts_Feed_Click(sender As Object, e As EventArgs) Handles ts_Feed.Click
        txt_Focus.Focus()
    End Sub

#End Region

    Private Sub dlg_Camera_Controls_VisibleChanged(sender As Object, e As EventArgs) Handles Me.VisibleChanged
        Update_Positions()
    End Sub

    Private Sub btn_Set_Start_Click(sender As Object, e As EventArgs) Handles btn_Set_Start.Click
        Start_X = CNC.X.Abs
        Start_Y = CNC.Y.Abs
        Start_Z = CNC.Z.Abs
        txt_Start_X.Text = Start_X
        txt_Start_Y.Text = Start_Y
        txt_Start_Z.Text = Start_Z
        Update_Positions()
        txt_Focus.Focus()
    End Sub

End Class