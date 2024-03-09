Imports System.IO

Module Mod_Global

    Public Save_X As Single
    Public Save_Y As Single
    Public Save_Z As Single

    Public TEST_STAND As Boolean = False
    Public SIMULATE As Boolean = False

    Public Break_Point As Boolean = False

    Public KFlop As New Class_KFLOP

    Public Firmware_Build As String = ""
    Public FirmwareVersion As String = ""
    Public Hardware_Version As String = ""
    Public TinyG_ID As String = ""

    Public Test_Cnt As Integer = 0

    Public Enum enum_Program_Mode
        Simulate
        Real_Machine
    End Enum

    Public mm_to_inch As Single = 1 / 25.4

    Public Trace_Control As Chip_Trace

    Public Logitech_G13 As New Class_Logitech_G13

    Public Calc As New Class_Calc

    Public Program_Mode As enum_Program_Mode
    Private Status_Shadow As String

    Public Chip_Root_Path As String = "C:\Chip"
    Public Chip_Maintenance_File_Name As String
    Public Chip_Icons_Folder As String
    Public Chip_Images_Folder As String
    Public Chip_Settings_File As String
    Public Chip_Help_Path As String = "Help\"

    Public Grid_Font = New Font("Microsoft Sans Serif", 9)
    Public GCode_Grid_Font = New Font("Microsoft Sans Serif", 11, FontStyle.Regular)

    Public Display As New Class_Display_Handlers

    Public Wait As New Class_Delay

    Public Send_Keys As New Class_Process
    Public Wizards As New Class_Wizards

    Public Message_Box As New dlg_Message_Box
    Private Message_Flash As New dlg_Flash_Message
    Private Err_Dialog As New dlg_Message_Box
    Public Input_Dialog As New dlg_Input
    Public Download_Dialog As New dlg_Modal_Message

    Public Debug_Environment As Boolean = False

    Public Fixture_Name As String
    Public Fixture_Mount As String

    Public Mouse_Control As New Class_Mouse_User32_DLL

    Public Sub Initialize_Paths()
        Dim MainPath As String


        'Determine the installation root directory
        MainPath = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly.Location)
        'If in normal Dynomotion location set path appropriately for Dynomotion installation
        If (MainPath.EndsWith("\KMotion\Release") Or MainPath.EndsWith("\KMotion\Debug") Or
            MainPath.EndsWith("\KMotion\Release64") Or MainPath.EndsWith("\KMotion\Debug64")) Then
            MainPath = Path.GetDirectoryName(MainPath)
            MainPath = Path.GetDirectoryName(MainPath)
            My.Settings.Chip_Root_Path = MainPath & "\PC VB Examples\Chip"
        End If

        If My.Settings.Chip_Root_Path = "" Then
            Chip_Root_Path = "C:\Chip"
            My.Settings.Chip_Root_Path = Chip_Root_Path
            My.Settings.Save()
        Else
            Chip_Root_Path = My.Settings.Chip_Root_Path
        End If

        If Not Directory.Exists(Chip_Root_Path) Then
            Show_Error("Chip root folder not found : " & Chip_Root_Path & vbCrLf & vbCrLf &
                       "This folder holds all the Chip setup information. And must exist." & vbCrLf &
                       "Close this message box, and a folder browser will appear." & vbCrLf &
                       "Navigate to, and select the folder that holds Chip setup information.")
            Dim dlg As New FolderBrowserDialog()
            dlg.ShowDialog()
            If dlg.SelectedPath <> "" Then
                Chip_Root_Path = dlg.SelectedPath
                If Not File.Exists(Chip_Root_Path & "\Settings\Persistant_Control_Settings.txt") Then
                    Show_Error("Settings files do not exist in this directory. this application will close." & vbCrLf & vbCrLf &
                               "Find Chip settup files and install.")
                    End
                Else
                    My.Settings.Chip_Root_Path = Chip_Root_Path
                    My.Settings.Save()
                End If
            Else
                End
            End If
        End If

        Nop()

        Chip_Images_Folder &= Chip_Root_Path & "\Images\"
        Chip_Icons_Folder &= Chip_Root_Path & "\Icons\"
        Chip_Settings_File = Chip_Root_Path & "\Settings\Persistant_Control_Settings.txt"
        Chip_Maintenance_File_Name = Chip_Root_Path & "\Maint\Chip_Maintenance.txt"

    End Sub

    Public Sub Flash_Message(ByVal Message As String, Optional Seconds As Single = 1.5)
        Message_Flash.ShowMessage(Message, Main_Form, Seconds)
    End Sub

    Public Function Show_Error(ByVal Message As String, Optional Prompt As String = "") As DialogResult
        If Message_Box.Visible Then Return DialogResult.Cancel
        If Prompt = "" Then Prompt = "Error"
        Message_Box.ShowDialog(Message, Prompt)
        Return Message_Box.DialogResult
    End Function

    Public Function Get_User_Input(ByVal Value As String,
                                   Optional Default_Value As String = "",
                                   Optional Current_Position As String = "",
                                   Optional Surface As String = "",
                                   Optional Prompt As String = "",
                                   Optional Note As String = "") As String
        Input_Dialog.Input_Value = Value
        Input_Dialog.Default_Value = Default_Value
        Input_Dialog.Current_Position = Current_Position
        Input_Dialog.Surface = Surface
        Input_Dialog.Prompt = Prompt
        Input_Dialog.Note = Note

        Input_Dialog.ShowDialog()
        If Input_Dialog.DialogResult = DialogResult.OK Then
            Return Input_Dialog.Input_Value
        Else
            Return Value
        End If
    End Function

    Public Function Browse_Folder(Optional Folder As String = "") As String

        If Folder <> "" Then
            If Not My.Computer.FileSystem.DirectoryExists(Folder) Then
                Show_Error("Folder does not exist : " & Folder)
                Folder = ""
            End If
        End If

        Dim dlg As New FolderBrowserDialog
        Dim Res As DialogResult
        dlg.SelectedPath = Folder
        Res = dlg.ShowDialog()
        If Res = Windows.Forms.DialogResult.OK Then
            Return dlg.SelectedPath
        Else
            Return ""
        End If

        Return ""

    End Function

    Public Sub Browse_File(ByRef Folder_Name As String, ByRef File_Name As String)

        If My.Computer.FileSystem.DirectoryExists(Folder_Name) Then
            Dim dlg As New OpenFileDialog
            Dim Result As DialogResult
            dlg.InitialDirectory = Folder_Name
            dlg.FileName = File_Name
            Result = dlg.ShowDialog()
            If Result = Windows.Forms.DialogResult.OK Then
                Folder_Name = Path.GetDirectoryName(dlg.FileName)
                File_Name = Path.GetFileName(dlg.FileName)
            End If
        End If

    End Sub

    Public Function C_Int(N As String) As Integer
        If IsNumeric(N) Then
            Return CInt(N)
        Else
            Return 0
        End If
    End Function

    Public Function C_dbl(N As String) As Integer
        If IsNumeric(N) Then
            Return CSng(N)
        Else
            Return 0
        End If
    End Function

    Public Function Get_Number(S As String) As String
        If IsNumeric(S) Then
            Return S
        Else
            Return "0"
        End If
    End Function

    Public Function Put_Number(S As String, Digits As Integer) As String
        If Not IsNumeric(S) Then
            Show_Error("Invalid number format.")
            Return S
        End If
        Return FormatNumber(S, Digits)
    End Function

    Public Function Get_Seconds(Time_String As String) As Integer
        Dim S() As String

        If Time_String = "" Then Return 0

        S = Time_String.Split(":")
        Select Case S.Count
            Case 1
                Return S(0)
            Case 2
                Return S(0) * 60 + S(1)
            Case 3
                Return (S(0) * 60) + (S(1) * 60) + S(2)
            Case Else
                Return 0
        End Select

    End Function

    Public Sub Nop()
    End Sub

End Module
