
Public Class Class_Process

    Public Enum enum_Show_Window
        SW_SHOWNORMAL = 1
        SW_SHOWMAXIMIZED = 3
        SW_SHOW = 5
        SW_RESTORE = 9
        SW_SHOWDEFAULT = 10
    End Enum

    Public Procs As Process()
    Public Proc As Process

    Public Proc_ID As Integer
    Public Part_Numbers As List(Of String)
    Public Part_Number_Index As Integer
    Public Exit_Process As Boolean = False

    Private WithEvents Delay_Timer As New Timer
    Private Watch As New Stopwatch

    Private Declare Function ShowWindow Lib "user32" (ByVal handle As IntPtr, ByVal nCmdShow As Integer) As Integer

    Public Sub Open_With_Start_Info(Proc_Name As String, Arguments As String)
        Dim startInfo As New ProcessStartInfo(Proc_Name)
        startInfo.Arguments = Arguments
        Process.Start(startInfo)
    End Sub

    Public Function Get_Running_Procs() As Process()
        Dim Procs() As Process
        Procs = Process.GetProcesses
        Return Procs
    End Function

    Sub ShowAppWindow(Proc_Name As String, Type As enum_Show_Window)
        Try
            Dim localByName As Process() = Process.GetProcessesByName(Proc_Name)
            For Each p As Process In localByName
                ShowWindow(p.MainWindowHandle, Type)
            Next
        Catch ex As Exception
            ' do something
        End Try
    End Sub

    Public Overloads Function Connect_to_Process(ID As Integer, Delay As Single) As Boolean
        Proc = Process.GetProcessById(ID)
        Application.DoEvents()

        Try
            ShowWindow(Proc.MainWindowHandle, 9)
            AppActivate(ID)
        Catch ex As Exception
            'Direction_Box.ShowDialog("Could not activate to process : " & CNC_USB_Process_Name & vbCrLf & ex.Message)
            Return False
        End Try

        Delay_Seconds(Delay)

        Return True

    End Function

    Public Overloads Function Connect_to_Process(Process_Name As String, Delay As Single, Optional ByRef Window_Title As String = "") As Boolean
        Dim Proc_Name As String

        Dim localByName As Process() = Process.GetProcesses

        Procs = Process.GetProcessesByName(Process_Name)
        Application.DoEvents()

        If Procs.Length = 0 Then
            'Direction_Box.ShowDialog("Could not connect to process : " & CNC_USB_Process_Name)
            Procs = Nothing
            Return False
        End If

        Try

            Proc_ID = Procs(0).Id
            AppActivate(Proc_ID)
            Proc_Name = Procs(0).ProcessName

            ShowAppWindow(Proc_Name, enum_Show_Window.SW_SHOW) 'was SW_RESTORE

            Window_Title = Procs(0).MainWindowTitle

        Catch ex As Exception
            'Direction_Box.ShowDialog("Could not activate to process : " & CNC_USB_Process_Name & vbCrLf & ex.Message)
            Return False
        End Try

        Delay_Seconds(Delay)

        Return True

    End Function

    Public Sub Send_Keys_To_Process(Process_Name As String, ByVal Keys As String, Optional ByVal Enter As Boolean = True, Optional Tab As Boolean = False)

        Connect_to_Process(Process_Name, 0)
        If IsNothing(Procs) Then Exit Sub

        Proc_ID = Procs(0).Id
        AppActivate(Proc_ID)

        Procs(0).WaitForInputIdle()

        If Keys <> "" Then My.Computer.Keyboard.SendKeys(Keys, True)
        If Enter Then My.Computer.Keyboard.SendKeys("{ENTER}", True)
        If Tab Then My.Computer.Keyboard.SendKeys("{TAB}", True)

    End Sub

    Public Sub Send_Key_String(ByVal Key_String As String, Delay As Single)

        Try
            AppActivate(Proc_ID)
        Catch ex As Exception
            Exit Sub
        End Try

        Procs(0).WaitForInputIdle()
        My.Computer.Keyboard.SendKeys(Key_String, True)
        'Procs(0).WaitForInputIdle()

        If Delay > 0 Then
            Delay_Seconds(Delay)
        End If

    End Sub

    Public Function Wait_For_Printer(ByVal Seconds As Single) As Boolean
        Dim S As Integer = Seconds * 1000
        If S = 0 Then Return True

        Delay_Timer.Interval = S
        Delay_Timer.Enabled = True
        Do
            Application.DoEvents()
            'If Cancel_Printing Then
            '    Delay_Timer.Enabled = False
            '    Return False
            'End If
        Loop Until Delay_Timer.Enabled = False

        Return True
    End Function

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

    Public Sub Delay_Ticks(ByVal Seconds As Single)
        Dim Freq As Long = Stopwatch.Frequency
        'Dim Period As Single = 1 / Freq

        'Watch.Start()

        'Do While Watch.ElapsedMilliseconds < Ticks
        '    Application.DoEvents()
        'Loop

    End Sub

    Public Sub Send_Key_String_To_Process(Process_Name As String, Key_String As List(Of String))
        Dim K() As String
        Dim P() As String
        Dim Proc_ID As Integer

        Connect_to_Process(Process_Name, 0)
        Proc_ID = Procs(0).Id
        AppActivate(Proc_ID)

        For I = 0 To Key_String.Count - 1
            If Exit_Process Then
                Exit Sub
            End If

            P = Key_String(I).Split("|")

            Application.DoEvents()
            Procs(0).WaitForInputIdle()

            For J = 0 To P.Count - 1

                K = P(J).Split("~")

                Select Case K(0)

                    Case "{Delay}"
                        Send_Keys.Delay_Seconds(K(1))

                    Case "{Pause}"
                        Exit Sub

                    Case Else
                        My.Computer.Keyboard.SendKeys(K(0), True)
                        If K(1) <> 0 Then Send_Keys.Delay_Seconds(K(1))

                End Select

            Next

        Next 'I = 0 To P.Count - 1


    End Sub

    Public Sub Send_Keys_To_Acrobat(Delay As Integer)
        Dim Proc_Name As String
        Dim Proc_Index As Integer

        Delay_Seconds(2)

        Dim localByName As Process() = Process.GetProcesses

        Procs = Process.GetProcessesByName("AcroRd32")
        Application.DoEvents()

        If Procs.Length = 0 Then
            Procs = Nothing
            Exit Sub
        End If

        Try
            For I = 0 To Procs.Count - 1
                If Procs(I).MainWindowTitle <> "" Then
                    Proc_Index = I
                    Proc_ID = Procs(I).Id
                    Exit For
                End If
            Next

            Proc_Name = Procs(Proc_Index).ProcessName
            AppActivate(Proc_ID)
            ShowAppWindow(Proc_Name, enum_Show_Window.SW_SHOW) 'was SW_RESTORE

            'If Not Cancel_Printing Then
            '    Procs(Proc_Index).WaitForInputIdle()
            '    My.Computer.Keyboard.SendKeys("^p", True)

            '    Delay_Seconds(2)
            '    Procs(Proc_Index).WaitForInputIdle()
            '    My.Computer.Keyboard.SendKeys("{ENTER}", True)

            '    Procs(Proc_Index).WaitForInputIdle()
            '    Wait_For_Printer(Delay)
            'End If

            Procs(Proc_Index).WaitForInputIdle()
            AppActivate(Proc_ID)
            ShowAppWindow(Proc_Name, enum_Show_Window.SW_SHOW) 'was SW_RESTORE

            Delay_Seconds(2)
            Procs(Proc_Index).WaitForInputIdle()
            My.Computer.Keyboard.SendKeys("^q", True)
            Delay_Seconds(2)
            '        Procs(Proc_Index).CloseMainWindow()
            '        Procs(Proc_Index).Close()

        Catch ex As Exception
            Exit Sub
        End Try

Early_Exit:


    End Sub

    Public Sub View_File(File_Name As String)
        Dim PDF_Proc = New System.Diagnostics.Process
        PDF_Proc.StartInfo.FileName = File_Name
        PDF_Proc.StartInfo.Verb = "Open"
        PDF_Proc.Start()
    End Sub


End Class
