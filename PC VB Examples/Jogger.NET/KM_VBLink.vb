Option Strict Off
Option Explicit On
Module Module1
	Dim global_KM_Handle As Integer
	
	Private Declare Function KMViaVB_Test1 Lib "KMViaVB" () As Integer
	Private Declare Function KMViaVB_Test2 Lib "KMViaVB" (ByVal handle As Integer) As Integer
	Private Declare Function KMViaVB_Test3 Lib "KMViaVB" (ByRef handle As Integer) As Integer
	
    Private Declare Function KMViaVB_New Lib "KMViaVB" (ByRef handle As Integer, ByVal board As Integer) As Integer
	Private Declare Function KMViaVB_Free Lib "KMViaVB" (ByRef handle As Integer) As Integer
	
    Private Declare Function KMViaVB_WriteLineReadLine Lib "KMViaVB" (ByVal handle As Integer, ByVal s As String, ByRef rtrn As String) As Integer
    Private Declare Function KMViaVB_WriteLine Lib "KMViaVB" (ByVal handle As Integer, ByVal s As String) As Integer
    Private Declare Function KMViaVB_WriteLineWithEcho Lib "KMViaVB" (ByVal handle As Integer, ByVal s As String) As Integer
    Private Declare Function KMViaVB_ReadLineTimeOut Lib "KMViaVB" (ByVal handle As Integer, ByRef buf As String, ByVal TimeOutms As Integer) As Integer
    Private Declare Function KMViaVB_WaitToken Lib "KMViaVB" (ByVal handle As Integer) As Integer
    Private Declare Function KMViaVB_KMotionLock Lib "KMViaVB" (ByVal handle As Integer) As Integer
    Private Declare Function KMViaVB_USBLocation Lib "KMViaVB" (ByVal handle As Integer) As Integer
    Private Declare Sub KMViaVB_ReleaseToken Lib "KMViaVB" (ByVal handle As Integer)
    Private Declare Function KMViaVB_Failed Lib "KMViaVB" (ByVal handle As Integer) As Integer
    Private Declare Function KMViaVB_CheckForReady Lib "KMViaVB" (ByVal handle As Integer) As Integer
    Private Declare Function KMViaVB_LoadCoff Lib "KMViaVB" (ByVal handle As Long, ByVal Thread As Short, ByVal Name As String) As Integer
    Private Declare Function KMViaVB_CompileAndLoadCoff Lib "KMViaVB" (ByVal handle As Integer, ByVal Thread As Short, ByVal Name As String, ByRef Err As String, ByVal MaxErrLen As Short) As Integer
    Private Declare Function KMViaVB_ServiceConsole Lib "KMViaVB" (ByVal handle As Integer) As Integer
    Private Declare Function KMViaVB_ListLocations Lib "KMViaVB" (ByVal handle As Integer, ByRef nlocations As Integer, ByRef List As Integer) As Integer
	
	
    Public Delegate Function EventHandler(ByVal parm As String) As Short
    Dim handler As New EventHandler(AddressOf ConsoleCallback)
    Private Declare Function KMViaVB_SetConsoleCallback Lib "KMViaVB" (ByVal handle As Integer, ByVal pFunc As EventHandler) As Integer
    Public ConsoleMessage As String

    Private Sub MarshalPre(ByRef s As String)
        s = Space(255)
    End Sub
    Private Sub MarshalPost(ByRef s As String)
        Dim p As Short
        p = InStr(s, Chr(0))
        If p Then s = Left(s, p - 1)
    End Sub

    Public Function KM_Open() As Integer
        Dim lng As Integer
        Debug.Print(Hex(KMViaVB_Test1))
        lng = 0
        Debug.Print(VB6.TabLayout(Hex(KMViaVB_Test2(lng)), Hex(lng)))
        lng = 10
        Debug.Print(VB6.TabLayout(Hex(KMViaVB_Test2(lng)), Hex(lng)))
        Debug.Print(VB6.TabLayout(Hex(KMViaVB_Test3(lng)), Hex(lng)))
        Debug.Print(VB6.TabLayout(Hex(KMViaVB_Test3(lng)), Hex(lng)))

        If global_KM_Handle <> 0 Then
            KM_Open = 13
            Exit Function
        End If

        KMViaVB_New(global_KM_Handle, 0)
        Debug.Print(VB6.TabLayout(Hex(global_KM_Handle), Hex(KM_Open)))
    End Function
    Public Function KM_WriteLineReadLine(ByVal s As String, ByRef rtrn As String) As Integer
        If global_KM_Handle = 0 Then
            KM_Open()
            If global_KM_Handle = 0 Then
                KM_WriteLineReadLine = 13
                Exit Function
            End If
        End If
        MarshalPre(rtrn)
        KM_WriteLineReadLine = KMViaVB_WriteLineReadLine(global_KM_Handle, s, rtrn)
        MarshalPost(rtrn)
    End Function
    Public Function KM_WriteLine(ByVal s As String) As Integer
        If global_KM_Handle = 0 Then
            KM_Open()
            If global_KM_Handle = 0 Then
                KM_WriteLine = 13
                Exit Function
            End If
        End If
        KM_WriteLine = KMViaVB_WriteLine(global_KM_Handle, s)
    End Function
    Public Function KM_WriteLineWithEcho(ByVal s As String) As Integer
        If global_KM_Handle = 0 Then
            KM_Open()
            If global_KM_Handle = 0 Then
                KM_WriteLineWithEcho = 13
                Exit Function
            End If
        End If
        KM_WriteLineWithEcho = KMViaVB_WriteLineWithEcho(global_KM_Handle, s)
    End Function
    Public Function KM_ReadLineTimeOut(ByRef buf As String, ByVal TimeOutms As Integer) As Integer
        If global_KM_Handle = 0 Then
            KM_Open()
            If global_KM_Handle = 0 Then
                KM_ReadLineTimeOut = 13
                Exit Function
            End If
        End If
        MarshalPre(buf)
        KM_ReadLineTimeOut = KMViaVB_ReadLineTimeOut(global_KM_Handle, buf, TimeOutms)
        MarshalPost(buf)
    End Function
    Public Function KM_WaitToken(ByVal bd As Integer) As Integer
        If global_KM_Handle = 0 Then
            KM_Open()
            If global_KM_Handle = 0 Then
                KM_WaitToken = 13
                Exit Function
            End If
        End If
        KM_WaitToken = KMViaVB_WaitToken(global_KM_Handle)
    End Function
    Public Function KM_KMotionLock(ByVal bd As Integer) As Integer
        If global_KM_Handle = 0 Then
            KM_Open()
            If global_KM_Handle = 0 Then
                KM_KMotionLock = 13
                Exit Function
            End If
        End If
        KM_KMotionLock = KMViaVB_KMotionLock(global_KM_Handle)
    End Function
    Public Function KM_USBLocation(ByVal bd As Integer) As Integer
        If global_KM_Handle = 0 Then
            KM_Open()
            If global_KM_Handle = 0 Then
                KM_USBLocation = -1
                Exit Function
            End If
        End If
        KM_USBLocation = KMViaVB_USBLocation(global_KM_Handle)
    End Function
    Public Sub KM_ReleaseToken(ByVal bd As Integer)
        If global_KM_Handle = 0 Then
            KM_Open()
            If global_KM_Handle = 0 Then
                'KM_ReleaseToken = 13
                Exit Sub
            End If
        End If
        KMViaVB_ReleaseToken(global_KM_Handle)
    End Sub
    Public Function KM_Failed(ByVal bd As Integer) As Integer
        If global_KM_Handle = 0 Then
            KM_Open()
            If global_KM_Handle = 0 Then
                KM_Failed = 13
                Exit Function
            End If
        End If
        KM_Failed = KMViaVB_Failed(global_KM_Handle)
    End Function
    Public Function KM_CheckForReady(ByVal bd As Integer) As Integer
        If global_KM_Handle = 0 Then
            KM_Open()
            If global_KM_Handle = 0 Then
                KM_CheckForReady = 13
                Exit Function
            End If
        End If
        KM_CheckForReady = KMViaVB_CheckForReady(global_KM_Handle)
    End Function
    Public Function KM_ServiceConsole(ByVal bd As Integer) As Integer
        If global_KM_Handle = 0 Then
            KM_Open()
            If global_KM_Handle = 0 Then
                KM_ServiceConsole = 13
                Exit Function
            End If
        End If
        KM_ServiceConsole = KMViaVB_ServiceConsole(global_KM_Handle)
    End Function
    Public Function KM_ListLocations(ByRef nList As Integer, ByRef List() As Integer) As Integer
        If global_KM_Handle = 0 Then
            KM_Open()
            If global_KM_Handle = 0 Then
                KM_ListLocations = 13
                Exit Function
            End If
        End If
        KM_ListLocations = KMViaVB_ListLocations(global_KM_Handle, nList, List(0))
    End Function
    Public Function KM_LoadCoff(ByVal Thread As Integer, ByVal Name As String) As Integer
        If global_KM_Handle = 0 Then
            KM_Open()
            If global_KM_Handle = 0 Then
                KM_LoadCoff = 13
                Exit Function
            End If
        End If
        KM_LoadCoff = KMViaVB_LoadCoff(global_KM_Handle, Thread, Name)
    End Function
    Public Function KM_CompileAndLoadCoff(ByVal Thread As Integer, ByVal Name As String, ByRef Err As String, ByVal MaxErrLen As Integer) As Integer
        If global_KM_Handle = 0 Then
            KM_Open()
            If global_KM_Handle = 0 Then
                KM_CompileAndLoadCoff = 13
                Exit Function
            End If
        End If
        MarshalPre(Err)
        KM_CompileAndLoadCoff = KMViaVB_CompileAndLoadCoff(global_KM_Handle, Thread, Name, Err, MaxErrLen)
        MarshalPost(Err)
    End Function

    Public Function KM_Close__() As Integer
        Dim lng As Integer
        Debug.Print(Hex(KMViaVB_Test1))
        lng = 0
        Debug.Print(VB6.TabLayout(Hex(KMViaVB_Test2(lng)), Hex(lng)))
        lng = 10
        Debug.Print(VB6.TabLayout(Hex(KMViaVB_Test2(lng)), Hex(lng)))
        Debug.Print(VB6.TabLayout(Hex(KMViaVB_Test3(lng)), Hex(lng)))
        Debug.Print(VB6.TabLayout(Hex(KMViaVB_Test3(lng)), Hex(lng)))

        ' quick exit
        KM_Close__ = 13
        Exit Function


        If global_KM_Handle = 0 Then
            KM_Close__ = 13
            Exit Function
        End If
        System.Diagnostics.Debug.Write(VB6.TabLayout(Hex(global_KM_Handle), TAB))

        KMViaVB_Free(global_KM_Handle)

        If global_KM_Handle <> 0 Then
            KM_Close__ = 13
            Exit Function
        End If

        Debug.Print(Hex(global_KM_Handle))

    End Function

    Public Function KM_SetConsoleCallback(ByVal bd As Integer) As Integer
        If global_KM_Handle = 0 Then
            KM_Open()
            If global_KM_Handle = 0 Then
                KM_SetConsoleCallback = 13
                Exit Function
            End If
        End If
        KM_SetConsoleCallback = KMViaVB_SetConsoleCallback(global_KM_Handle, AddressOf ConsoleCallback)
        handler = AddressOf ConsoleCallback
    End Function


    Function ConsoleCallback(ByVal Msg As String) As Integer
        ConsoleMessage = ConsoleMessage & Msg
    End Function


End Module