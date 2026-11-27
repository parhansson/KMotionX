Option Strict Off
Option Explicit On
#Disable Warning BC40000
Friend Class frmMain
	Inherits System.Windows.Forms.Form
	Const MaxSpeedX As Single = 16000
	Const MaxSpeedY As Single = 16000
    Const MaxSpeedZ As Single = 16000
    Const MaxSpeedA As Single = 16000
	Const cpiX As Single = 16000
	Const cpiY As Single = 16000
	Const cpiZ As Single = 16000
    Const cpiA As Single = 5729.6

    Dim board As Integer

    Dim MainPath As String

    Dim MessDisplayed As Boolean = False



    Private Sub myerror()
        'Text1.Text = Format(Now, "hh:mm:ss mm/dd/yy") + ": myerror"
        'Text2.Text = Format(StartTime - Now, "hhh:mm:ss mm/dd/yy") + " Elapsed"
        'displayTime "MyError"
        If (Not MessDisplayed) Then
            MessDisplayed = True
            MsgBox("MYERROR CALLED")
            MessDisplayed = False
        End If
        'stopflag = 1
    End Sub

    Private Sub cmdANeg_MouseDown(ByVal eventSender As System.Object, ByVal eventArgs As System.Windows.Forms.MouseEventArgs) Handles cmdANeg.MouseDown
        Dim Button As Short = eventArgs.Button \ &H100000
        Dim Shift As Short = System.Windows.Forms.Control.ModifierKeys \ &H10000
        Dim Index As Short = cmdANeg.GetIndex(eventSender)
        'go


        Debug.Print("jog A Neg")
        'If KM_WriteLine("EnableAxis3") Then myerror()
        If KM_WriteLine("Jog3=" & Trim(Str(-1 * TrackBarA.Value / TrackBarA.Maximum * MaxSpeedA))) Then myerror()
    End Sub
	
	Private Sub cmdANeg_MouseUp(ByVal eventSender As System.Object, ByVal eventArgs As System.Windows.Forms.MouseEventArgs) Handles cmdANeg.MouseUp
		Dim Button As Short = eventArgs.Button \ &H100000
		Dim Shift As Short = System.Windows.Forms.Control.ModifierKeys \ &H10000
        Dim Index As Short = cmdANeg.GetIndex(eventSender)
        'stop
        If KM_WriteLine("Jog3=0") Then myerror()
	End Sub
	
	Private Sub cmdAPos_MouseDown(ByVal eventSender As System.Object, ByVal eventArgs As System.Windows.Forms.MouseEventArgs) Handles cmdAPos.MouseDown
		Dim Button As Short = eventArgs.Button \ &H100000
		Dim Shift As Short = System.Windows.Forms.Control.ModifierKeys \ &H10000
        Dim Index As Short = cmdAPos.GetIndex(eventSender)
        'go
        Debug.Print("jog A Pos")
        'If KM_WriteLine("EnableAxis3") Then myerror()
        If KM_WriteLine("Jog3=" & Trim(Str(TrackBarA.Value / TrackBarA.Maximum * MaxSpeedA))) Then myerror()
	End Sub
	
	Private Sub cmdAPos_MouseUp(ByVal eventSender As System.Object, ByVal eventArgs As System.Windows.Forms.MouseEventArgs) Handles cmdAPos.MouseUp
		Dim Button As Short = eventArgs.Button \ &H100000
		Dim Shift As Short = System.Windows.Forms.Control.ModifierKeys \ &H10000
        Dim Index As Short = cmdAPos.GetIndex(eventSender)
        'stop
        If KM_WriteLine("Jog3=0") Then myerror()
	End Sub
	
	Private Sub cmdXLeft_MouseDown(ByVal eventSender As System.Object, ByVal eventArgs As System.Windows.Forms.MouseEventArgs) Handles cmdXLeft.MouseDown
		Dim Button As Short = eventArgs.Button \ &H100000
		Dim Shift As Short = System.Windows.Forms.Control.ModifierKeys \ &H10000
        Dim Index As Short = cmdXLeft.GetIndex(eventSender)
        'go
        Debug.Print("jog x left")
        'If KM_WriteLine("EnableAxis0") Then myerror()
        If KM_WriteLine("Jog0=" & Trim(Str(-1 * TrackBarX.Value / TrackBarX.Maximum * MaxSpeedX))) Then myerror()
	End Sub
	
	Private Sub cmdXLeft_MouseUp(ByVal eventSender As System.Object, ByVal eventArgs As System.Windows.Forms.MouseEventArgs) Handles cmdXLeft.MouseUp
		Dim Button As Short = eventArgs.Button \ &H100000
		Dim Shift As Short = System.Windows.Forms.Control.ModifierKeys \ &H10000
        Dim Index As Short = cmdXLeft.GetIndex(eventSender)
        'stop
        If KM_WriteLine("Jog0=0") Then myerror()
	End Sub
	
	Private Sub cmdXRight_MouseDown(ByVal eventSender As System.Object, ByVal eventArgs As System.Windows.Forms.MouseEventArgs) Handles cmdXRight.MouseDown
		Dim Button As Short = eventArgs.Button \ &H100000
		Dim Shift As Short = System.Windows.Forms.Control.ModifierKeys \ &H10000
        Dim Index As Short = cmdXRight.GetIndex(eventSender)
        'go
        Debug.Print("jog x Right")
        'If KM_WriteLine("EnableAxis0") Then myerror()
        If KM_WriteLine("Jog0=" & Trim(Str(TrackBarX.Value / TrackBarX.Maximum * MaxSpeedX))) Then myerror()
	End Sub
	
	Private Sub cmdXRight_MouseUp(ByVal eventSender As System.Object, ByVal eventArgs As System.Windows.Forms.MouseEventArgs) Handles cmdXRight.MouseUp
		Dim Button As Short = eventArgs.Button \ &H100000
		Dim Shift As Short = System.Windows.Forms.Control.ModifierKeys \ &H10000
        Dim Index As Short = cmdXRight.GetIndex(eventSender)
        'stop
        If KM_WriteLine("Jog0=0") Then myerror()
	End Sub
	
	Private Sub cmdYBack_MouseDown(ByVal eventSender As System.Object, ByVal eventArgs As System.Windows.Forms.MouseEventArgs) Handles cmdYBack.MouseDown
		Dim Button As Short = eventArgs.Button \ &H100000
		Dim Shift As Short = System.Windows.Forms.Control.ModifierKeys \ &H10000
        'go
        Debug.Print("jog y back")
        'If KM_WriteLine("EnableAxis1=1") Then myerror()
        If KM_WriteLine("Jog1=" & Trim(Str(-1 * TrackBarY.Value / TrackBarY.Maximum * MaxSpeedY))) Then myerror()
		
	End Sub
	
	Private Sub cmdYBack_MouseUp(ByVal eventSender As System.Object, ByVal eventArgs As System.Windows.Forms.MouseEventArgs) Handles cmdYBack.MouseUp
		Dim Button As Short = eventArgs.Button \ &H100000
		Dim Shift As Short = System.Windows.Forms.Control.ModifierKeys \ &H10000
        'stop
        Debug.Print("jog y stop")
        If KM_WriteLine("Jog1=0") Then myerror()
	End Sub
	
	Private Sub cmdYFront_MouseDown(ByVal eventSender As System.Object, ByVal eventArgs As System.Windows.Forms.MouseEventArgs) Handles cmdYFront.MouseDown
		Dim Button As Short = eventArgs.Button \ &H100000
		Dim Shift As Short = System.Windows.Forms.Control.ModifierKeys \ &H10000
        Dim Index As Short = cmdYFront.GetIndex(eventSender)
        'go
        Debug.Print("jog y back")
        'If KM_WriteLine("EnableAxis1=1") Then myerror()
        If KM_WriteLine("Jog1=" & Trim(Str(TrackBarY.Value / TrackBarY.Maximum * MaxSpeedY))) Then myerror()
	End Sub
	
	Private Sub cmdYFront_MouseUp(ByVal eventSender As System.Object, ByVal eventArgs As System.Windows.Forms.MouseEventArgs) Handles cmdYFront.MouseUp
		Dim Button As Short = eventArgs.Button \ &H100000
		Dim Shift As Short = System.Windows.Forms.Control.ModifierKeys \ &H10000
        Dim Index As Short = cmdYFront.GetIndex(eventSender)
        'stop
        Debug.Print("jog y stop")
        If KM_WriteLine("Jog1=0") Then myerror()
	End Sub
	
	Private Sub cmdZDown_MouseDown(ByVal eventSender As System.Object, ByVal eventArgs As System.Windows.Forms.MouseEventArgs) Handles cmdZDown.MouseDown
		Dim Button As Short = eventArgs.Button \ &H100000
		Dim Shift As Short = System.Windows.Forms.Control.ModifierKeys \ &H10000
        Dim Index As Short = cmdZDown.GetIndex(eventSender)
        'go
        Debug.Print("jog Z Down")
        'If KM_WriteLine("EnableAxis2") Then myerror()
        If KM_WriteLine("Jog2=" & Trim(Str(-1 * TrackBarZ.Value / TrackBarZ.Maximum * MaxSpeedZ))) Then myerror()
	End Sub
	
	Private Sub cmdZDown_MouseUp(ByVal eventSender As System.Object, ByVal eventArgs As System.Windows.Forms.MouseEventArgs) Handles cmdZDown.MouseUp
		Dim Button As Short = eventArgs.Button \ &H100000
		Dim Shift As Short = System.Windows.Forms.Control.ModifierKeys \ &H10000
        Dim Index As Short = cmdZDown.GetIndex(eventSender)
        'stop
        If KM_WriteLine("Jog2=0") Then myerror()
	End Sub
	
	Private Sub cmdZUp_MouseDown(ByVal eventSender As System.Object, ByVal eventArgs As System.Windows.Forms.MouseEventArgs) Handles cmdZUp.MouseDown
		Dim Button As Short = eventArgs.Button \ &H100000
		Dim Shift As Short = System.Windows.Forms.Control.ModifierKeys \ &H10000
        Dim Index As Short = cmdZUp.GetIndex(eventSender)
        'go
        Debug.Print("jog Z up")
        'If KM_WriteLine("EnableAxis2") Then myerror()
        If KM_WriteLine("Jog2=" & Trim(Str(TrackBarZ.Value / TrackBarZ.Maximum * MaxSpeedZ))) Then myerror()
	End Sub
	
	Private Sub cmdZUp_MouseUp(ByVal eventSender As System.Object, ByVal eventArgs As System.Windows.Forms.MouseEventArgs) Handles cmdZUp.MouseUp
		Dim Button As Short = eventArgs.Button \ &H100000
		Dim Shift As Short = System.Windows.Forms.Control.ModifierKeys \ &H10000
        Dim Index As Short = cmdZUp.GetIndex(eventSender)
        'stop
        If KM_WriteLine("Jog2=0") Then myerror()
	End Sub
	
	Private Sub frmMain_Load(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles MyBase.Load
        Dim args As String()
        Dim result As Integer
        Dim FirmwareFile As String

        KM_Open()
        'Determine the installation root directory
        MainPath = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly.Location)
        MainPath = System.IO.Path.GetDirectoryName(MainPath)
        MainPath = System.IO.Path.GetDirectoryName(MainPath)

        Me.CFile.Text = MainPath & "\C Programs\KStep\InitKStep3Axis.c"

        args = System.Environment.GetCommandLineArgs()  'check if we are to do a firmware update onlly

        If (args.Length > 1) Then
            If (args(1) = "UpdateFirmware") Then
                FirmwareFile = MainPath & "\DSP_KFLOP\DSPKFLOP.out"
                result = KM_LoadCoff(-1, FirmwareFile, 1)
                If (result = 0) Then
                    result = KM_WriteLineWithEcho("ProgFlashImage")
                End If
                Do
                    result = KM_CheckForReady(0)
                Loop While (result <> 2)

                result = KM_WriteLineWithEcho("Reboot!")
                Application.Exit()
            End If
        End If
    End Sub
	
	Private Sub Timer1_Tick(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles Timer1.Tick
        Dim rtrnstr As String = ""
        Dim USBPort As Integer
        Dim result As Short
		Dim KMOTION_LOCKED, KMOTION_IN_USE As Object
		Dim KMOTION_NOT_CONNECTED As Short
        Dim List(256) As Integer
        Dim nList As Integer

        result = KM_ListLocations(nList, List) 'See which boards are present
		
		If (nList > 0) Then ' if we have at least one pick the first port
			board = List(0)
        Else
            board = 0
        End If

        'Add in console messages
        If (ConsoleMessage <> "") Then
            ConsoleMessages.Text = ConsoleMessages.Text & ConsoleMessage
            ConsoleMessages.SelectionStart = ConsoleMessages.TextLength
            ConsoleMessages.ScrollToCaret()
            ConsoleMessage = ""
        End If


        KMOTION_LOCKED = 0 ' (and token is locked) if KMotion is available for use
        KMOTION_IN_USE = 1 ' if already in use
        KMOTION_NOT_CONNECTED = 2 ' if error or not able to connect

        result = KM_KMotionLock(board) 'See if we can lock the board

        If (result = KMOTION_LOCKED) Then
            KM_ReleaseToken((board)) ' Yes, we locked it so release it
        End If

        If (result <> KMOTION_LOCKED) Then
            USBLocation.Text = "USB Port Location = Not Present"
        Else
            USBPort = KM_USBLocation(board)

            USBLocation.Text = "Board ID = " & BoardString(USBPort)

            'report positions in inches or degrees
            result = KM_WriteLineReadLine("dest0", rtrnstr)
            If result = KMOTION_NOT_CONNECTED Then Return
            If result > 0 Then
                myerror()
            Else
                txtXPos.Text = VB6.Format(Val(rtrnstr) / cpiX, "0.000")
                result = KM_WriteLineReadLine("dest1", rtrnstr)
                If result = KMOTION_NOT_CONNECTED Then Return
                If result > 0 Then
                    myerror()
                Else
                    txtYPos.Text = VB6.Format(Val(rtrnstr) / cpiY, "0.000")
                    result = KM_WriteLineReadLine("dest2", rtrnstr)
                    If result = KMOTION_NOT_CONNECTED Then Return
                    If result > 0 Then
                        myerror()
                    Else
                        txtZPos.Text = VB6.Format(Val(rtrnstr) / cpiZ, "0.000")
                        result = KM_WriteLineReadLine("dest3", rtrnstr)
                        If result = KMOTION_NOT_CONNECTED Then Return
                        If result > 0 Then
                            myerror()
                        Else
                            txtAPos.Text = VB6.Format(Val(rtrnstr) / cpiA, "0.000")
                        End If
                    End If
                End If
            End If
        End If
    End Sub


    Private Function num2ip(ByVal lNum As Double) As String
        Dim b0 As Double
        Dim b1 As Double
        Dim b2 As Double
        Dim b3 As Double

        b0 = Int(lNum / 256 ^ 3)
        lNum = lNum - b0 * 256 ^ 3
        b1 = Int(lNum / 256 ^ 2)
        lNum = lNum - b1 * 256 ^ 2
        b2 = Int(lNum / 256)
        lNum = lNum - b2 * 256
        b3 = Int(lNum)

        num2ip = VB6.Format(b0) & "." & VB6.Format(b1) & "." & VB6.Format(b2) & "." & VB6.Format(b3)
    End Function

    Private Function BoardString(ByVal board As Integer) As String
        Dim uBoard As UInt32

        If (board < 0) Then
            uBoard = board + 256 ^ 4
        Else
            uBoard = board
        End If

        If (board <= 0 And board >= -15) Then
            BoardString = VB6.Format(board) ' Show As Decimal number
        ElseIf (uBoard < &HFFFFFF) Then ' USB?
            BoardString = "KFLOP" & Hex(uBoard) ' show As hex USB ID number
        Else ' assume IP Address
            BoardString = "Kogna" & num2ip(uBoard)
        End If
    End Function



    Private Sub _cmdZUp_3_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles _cmdZUp_3.Click

    End Sub

    Private Sub CompileLoadCoff_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles CompileLoadCoff.Click
        Dim Err As String = "Bad File"
        Dim result As Integer

        If CThread.Text = -1 Then
            result = KM_LoadCoff(CThread.Text, CFile.Text, 1)
            If (result = 0) Then
                result = KM_WriteLineWithEcho("ProgFlashImage")
            End If
            Do
                result = KM_CheckForReady(0)
            Loop While (result <> 2)
            result = 0
        Else
            result = KM_CompileAndLoadCoff(CThread.Text, CFile.Text, Err, 250)
        End If


        If (result = 0) Then
            CompilerOutput.Text = "No Errors"
        Else
            CompilerOutput.Text = Err
        End If
    End Sub
    Private Sub Execute_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Execute.Click
        KM_WriteLine("Execute" & CThread.Text)
    End Sub
    Private Sub SetConsoleCallback_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles SetConsoleCallback.Click
        KM_SetConsoleCallback(board)
    End Sub

    Private Sub ClearMessages_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ClearMessages.Click
        ConsoleMessages.Text = ""
    End Sub

    Private Sub OpenCFile_FileOk(ByVal sender As System.Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles OpenCFile.FileOk
        CFile.Text = OpenCFile.FileName
    End Sub

    Private Sub BrowseCFile_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles BrowseCFile.Click
        OpenCFile.InitialDirectory = IO.Path.GetDirectoryName(CFile.Text)
        OpenCFile.FileName = IO.Path.GetFileName(CFile.Text)
        OpenCFile.ShowDialog()
    End Sub

    Private Sub _cmdXLeft_0_Click(sender As Object, e As EventArgs) Handles _cmdXLeft_0.Click
        Dim i As Integer
        Dim k As Integer

        Dim s As String
        Dim t As String
        s = ""
        t = ""
        KM_WaitToken(0)
        KM_WriteLine("getgatherhex 0 400")
        KM_ReadLineTimeOut(s, 10000)
        KM_ReadLineTimeOut(t, 10000)
        KM_ReleaseToken(0)

        i = s.IndexOf(vbLf)
        k = t.Length

    End Sub
End Class