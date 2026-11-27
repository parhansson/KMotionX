Public Class Class_Delay

    Private WithEvents Delay_Timer As New Timer

    Public Sub Delay_Seconds(ByVal Seconds As Single)
        Dim S As Integer = Seconds * 1000
        If S = 0 Then Exit Sub
        Delay_Timer.Interval = S
        Delay_Timer.Enabled = True
        Do
            Application.DoEvents()
        Loop Until Delay_Timer.Enabled = False
    End Sub

    Private Sub Delay_Timer_Tick() Handles Delay_Timer.Tick
        Delay_Timer.Enabled = False
    End Sub

End Class
