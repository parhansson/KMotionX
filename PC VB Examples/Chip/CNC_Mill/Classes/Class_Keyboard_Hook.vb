Imports System.Runtime.InteropServices

Public Class Class_Keyboard_Hook

    <DllImport("User32.dll", CharSet:=CharSet.Auto, CallingConvention:=CallingConvention.StdCall)>
    Private Overloads Shared Function SetWindowsHookEx(ByVal idHook As Integer, ByVal HookProc As KBDLLHookProc, ByVal hInstance As IntPtr, ByVal wParam As Integer) As Integer
    End Function
    <DllImport("User32.dll", CharSet:=CharSet.Auto, CallingConvention:=CallingConvention.StdCall)>
    Private Overloads Shared Function CallNextHookEx(ByVal idHook As Integer, ByVal nCode As Integer, ByVal wParam As IntPtr, ByVal lParam As IntPtr) As Integer
    End Function
    <DllImport("User32.dll", CharSet:=CharSet.Auto, CallingConvention:=CallingConvention.StdCall)>
    Private Overloads Shared Function UnhookWindowsHookEx(ByVal idHook As Integer) As Boolean
    End Function

    <DllImport("User32.dll", CharSet:=CharSet.Auto, CallingConvention:=CallingConvention.StdCall)>
    Private Overloads Shared Function GetAsyncKeyState(ByVal vKey As Integer) As Boolean
    End Function

    <StructLayout(LayoutKind.Sequential)>
    Private Structure KBDLLHOOKSTRUCT
        Public vkCode As UInt32
        Public scanCode As UInt32
        Public flags As KBDLLHOOKSTRUCTFlags
        Public time As UInt32
        Public dwExtraInfo As UIntPtr
    End Structure

    <Flags()>
    Private Enum KBDLLHOOKSTRUCTFlags As UInt32
        LLKHF_EXTENDED = &H1
        LLKHF_INJECTED = &H10
        LLKHF_ALTDOWN = &H20
        LLKHF_UP = &H80
    End Enum

    Public Shared Event Hot_Key_Event(ByVal Command As String)

    Private Const WH_KEYBOARD_LL As Integer = 13
    Private Const HC_ACTION As Integer = 0
    Private Const WM_KEYDOWN = &H100
    Private Const WM_KEYUP = &H101
    Private Const WM_SYSKEYDOWN = &H104
    Private Const WM_SYSKEYUP = &H105

    Public Const VK_TAB = &H9
    Public Const VK_CONTROL = &H11
    Public Const VK_ESCAPE = &H1B
    Public Const VK_DELETE = &H2E

    Private Delegate Function KBDLLHookProc(ByVal nCode As Integer, ByVal wParam As IntPtr, ByVal lParam As IntPtr) As Integer

    Private KBDLLHookProcDelegate As KBDLLHookProc = New KBDLLHookProc(AddressOf KeyboardProc)
    Private HHookID As IntPtr = IntPtr.Zero

    Private Command_Shadow As String = ""

    Private Function KeyboardProc(ByVal nCode As Integer, ByVal wParam As IntPtr, ByVal lParam As IntPtr) As Integer

        If (nCode = HC_ACTION) Then

            Dim Cmd As String = ""
            Dim Param As Integer = wParam
            Dim Mess As String = ""
            Dim Hook_Struct As KBDLLHOOKSTRUCT
            Hook_Struct = CType(Marshal.PtrToStructure(lParam, Hook_Struct.GetType()), KBDLLHOOKSTRUCT)
            Dim Code As Integer = Hook_Struct.vkCode

            Select Case Code
                Case 33
                    Cmd = "Z+"
                Case 34
                    Cmd = "Z-"
                Case 39
                    Cmd = "X+"
                Case 37
                    Cmd = "X-"
                Case 38
                    Cmd = "Y+"
                Case 40
                    Cmd = "Y-"

                Case 164 'ALT Key, Ignore

                Case Else
                    If Code >= 65 And Code <= 86 Then
                        Cmd = "G" & Trim((Code - 64).ToString)
                    End If

            End Select

            If Cmd <> "" Then

                Select Case wParam
                    Case WM_SYSKEYDOWN
                        Cmd &= " DN"
                    Case WM_SYSKEYUP
                        Cmd &= " UP"
                End Select

                If Cmd <> Command_Shadow Then 'Takes out repeated commands
                    RaiseEvent Hot_Key_Event(Cmd)
                End If

                Command_Shadow = Cmd
                    Return 0

                End If

            End If

        Return CallNextHookEx(IntPtr.Zero, nCode, wParam, lParam) 'Letst other apps see key

    End Function

    Public Sub New()
        HHookID = SetWindowsHookEx(WH_KEYBOARD_LL, KBDLLHookProcDelegate, 0, 0)
        If HHookID = IntPtr.Zero Then
            Throw New Exception("Could not set keyboard hook")
        End If
    End Sub

    Protected Overrides Sub Finalize()
        If Not HHookID = IntPtr.Zero Then
            UnhookWindowsHookEx(HHookID)
        End If
        MyBase.Finalize()
    End Sub

End Class
