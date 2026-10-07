Option Strict On
Imports System.Runtime.InteropServices
Imports System

Public Class Class_Logitech_G13

    Public Declare Function SetDllDirectoryA Lib "kernel32" (ByVal lpPathName As String) As Boolean

    <DllImport("LogitechLcdEnginesWrapper", CharSet:=CharSet.Unicode, CallingConvention:=CallingConvention.Cdecl)>
    Shared Function LogiLcdInit(ByVal Name As String, ByVal Type As Integer) As Boolean
    End Function
    <DllImport("LogitechLcdEnginesWrapper", CharSet:=CharSet.Unicode, CallingConvention:=CallingConvention.Cdecl)>
    Shared Function LogiLcdMonoSetText(ByVal Line_Number As Integer, ByVal Text As String) As Boolean
    End Function

    <DllImport("LogitechLcdEnginesWrapper", CharSet:=CharSet.Unicode, CallingConvention:=CallingConvention.Cdecl)>
    Shared Function LogiLcdUpdate() As Boolean
    End Function

    <DllImport("LogitechLedEnginesWrapper", CharSet:=CharSet.Unicode, CallingConvention:=CallingConvention.Cdecl)>
    Shared Function LogiLedInit() As Boolean
    End Function

    <DllImport("LogitechLedEnginesWrapper", CharSet:=CharSet.Unicode, CallingConvention:=CallingConvention.Cdecl)>
    Shared Function LogiLedSetTargetDevice(ByVal Device As Integer) As Boolean
    End Function

    <DllImport("LogitechLedEnginesWrapper", CharSet:=CharSet.Unicode, CallingConvention:=CallingConvention.Cdecl)>
    Shared Function LogiLedSetLighting(ByVal RedPct As Integer, ByVal GreenPct As Integer, ByVal BluePct As Integer) As Boolean
    End Function

    Public Enabled As Boolean = False

    Public Sub Init()
        Dim Res As Boolean
        Try
            If (Environment.Is64BitProcess) Then
                Res = SetDllDirectoryA(Chip_Root_Path & "\Logitech_G13_Libraries\x64\")
                LogiLcdInit("Chip", 1)
            Else
                Res = SetDllDirectoryA(Chip_Root_Path & "\Logitech_G13_Libraries\x86\")
                LogiLcdInit("Chip", 1)
            End If

            Enabled = True
        Catch ex As Exception
            MessageBox.Show(ex.Message)
            Enabled = False
            Exit Sub
        End Try

        Wait.Delay_Seconds(0.25)
        LogiLedInit()
        LogiLedSetTargetDevice(7)
        Wait.Delay_Seconds(0.25)

    End Sub

    Public Sub Set_Color(Red_Pct As Integer, Green_Pct As Integer, Blue_Pct As Integer)
        If Not Enabled Then Exit Sub
        Try
            LogiLedSetLighting(Red_Pct, Green_Pct, Blue_Pct)
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try
    End Sub

    Public Sub Set_Text(Line_Number As Integer, ByRef Text As String)
        If Not Enabled Then Exit Sub
        LogiLcdMonoSetText(Line_Number, Text)
    End Sub

    Public Sub Display_Message(Line_0 As String, Optional Line_1 As String = "", Optional Line_2 As String = "", Optional Line_3 As String = "")
        If Not Enabled Then Exit Sub
        LogiLcdMonoSetText(0, Line_0)
        LogiLcdMonoSetText(1, Line_1)
        LogiLcdMonoSetText(2, Line_2)
        LogiLcdMonoSetText(3, Line_3)
        LogiLcdUpdate()
    End Sub

    Public Sub Update_Display()
        If Not Enabled Then Exit Sub
        LogiLcdUpdate()
    End Sub


End Class
