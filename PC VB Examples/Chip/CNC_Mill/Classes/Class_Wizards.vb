Imports System.IO

Public Class Class_Wizards
    Public Fixture_Offsets_Control As Chip_Fixture_Offsets
    Public GCode_Control As Chip_Gcode

    Public Sub Get_GCode_File_Path(ByRef File_Path As String)
        Dim Df As New Class_Data_File
        File_Path = Df.Select_Input_File(File_Path, "Gcode Files (*.chp)|*.chp|Route Files (*.chp)|*.chp|All files (*.*)|*.*")
        If File_Path = "" Then Exit Sub
    End Sub


End Class
