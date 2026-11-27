
Public Class dlg_Flash_Message

    ' Dim WithEvents Message_Timer As New Timer
    Dim Center_X As Integer
    Dim Center_Y As Integer

    Public Sub ShowMessage(ByVal Message As String, frm As Windows.Forms.Form, Optional Seconds As Single = 1)
        Dim L As Integer = frm.Left
        Dim T As Integer = frm.Top
        Dim H As Integer = frm.Height
        Dim W As Integer = frm.Width
        Dim X As Integer = L + (W / 2)
        Dim Y As Integer = T + (H / 2)

        Dim Text_Height As Integer = txt_Focus.Height
        Dim S() As String

        Message = vbCrLf & Message & vbCrLf
        S = Message.Split(vbLf)
        txt_Message.Height = S.Length * Text_Height
        Me.Height = txt_Message.Top + txt_Message.Height + Text_Height

        Dim Gr As Graphics
        Dim Fnt As Font
        Gr = txt_Message.CreateGraphics
        Fnt = txt_Message.Font

        ' Measure string. 
        Dim stringSize As New SizeF
        stringSize = Gr.MeasureString(Message, Fnt)
        txt_Message.Width = stringSize.Width + 25
        If txt_Message.Width < 300 Then txt_Message.Width = 300
        Me.Width = txt_Message.Width + 50

        txt_Focus.Width = 0
        txt_Focus.Height = 0

        Center_X = X
        Center_Y = Y

        txt_Message.Text = Message
        Me.Visible = True
        Me.Left = Center_X - (Me.Width / 2)
        Me.Top = Center_Y + (Me.Height / 2)
        txt_Focus.Focus()

        'If Seconds > 0 Then
        '    Message_Timer.Interval = Seconds * 1000
        '    Message_Timer.Enabled = True
        'End If

        Me.Show()

        Wait.Delay_Seconds(Seconds)

        Me.Hide()

    End Sub

    'Private Sub Timer_Tick() Handles Message_Timer.Tick
    '    Message_Timer.Enabled = False
    '    Me.Hide()
    '    'Me.Close()
    'End Sub

End Class