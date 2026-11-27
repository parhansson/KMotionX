
Public Class Class_Mouse_User32_DLL
    Public Declare Auto Function SetCursorPos Lib "User32.dll" (ByVal X As Integer, ByVal Y As Integer) As Long
    Public Declare Auto Function GetCursorPos Lib "User32.dll" (ByRef lpPoint As Point) As Long
    Public Declare Sub mouse_event Lib "user32" Alias "mouse_event" (ByVal dwFlags As Int32, ByVal dx As Int32, ByVal dy As Int32, ByVal cButtons As Int32, ByVal dwExtraInfo As Int32)
    Public Const MOUSEEVENTF_LEFTDOWN = &H2 ' left button down
    Public Const MOUSEEVENTF_LEFTUP = &H4 ' left button up
    Public Const MOUSEEVENTF_MIDDLEDOWN = &H20 ' middle button down
    Public Const MOUSEEVENTF_MIDDLEUP = &H40 ' middle button up
    Public Const MOUSEEVENTF_RIGHTDOWN = &H8 ' right button down
    Public Const MOUSEEVENTF_RIGHTUP = &H10 ' right button up

    Public Sub Click_Mouse(X As Int32, Y As Int32)
        SetCursorPos(X, Y)
        'Simulate click
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0)
        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0)
    End Sub

    Public Sub Click_Mouse_Down(X As Int32, Y As Int32)
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0)
    End Sub

    Public Sub Click_Mouse_Up(X As Int32, Y As Int32)
        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0)
    End Sub

    Public Sub Move_Cursor(X As Integer, Y As Integer)
        SetCursorPos(X, Y) ' Where X and Y are in pixel
    End Sub

    Public Sub Get_Cursor_Position(ByRef Pos As Point)
        Dim tempPos As Point
        Dim R As Long = GetCursorPos(tempPos) ' You'll get your location in TempPos
    End Sub

    Public Sub Drag_Drop_Cursor(X As Integer, Y As Integer)
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0)
        SetCursorPos(X, Y) ' Where X and Y are in pixel
        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0)
    End Sub

End Class
