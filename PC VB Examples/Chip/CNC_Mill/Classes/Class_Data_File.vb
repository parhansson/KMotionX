Imports System.IO
Imports System.Text

Public Class Class_Data_File

#Region "Definitions"

    Private Output_File As System.IO.StreamWriter
    Private Input_File As System.IO.StreamReader

#End Region

#Region "Select, Rename"

    Public Sub Rename(Old_Path As String, New_Path As String)
        FileSystem.Rename(Old_Path, New_Path)
    End Sub

    Public Function Select_Input_File(File_Name As String, Filter As String) As String
        Dim dlg As New OpenFileDialog
        If File_Name <> "" Then
            dlg.InitialDirectory = Path.GetDirectoryName(File_Name)
        End If
        dlg.FileName = Path.GetFileName(File_Name)
        dlg.Filter = Filter
        If dlg.ShowDialog() = Windows.Forms.DialogResult.OK Then
            Return dlg.FileName
        Else
            Return ""
        End If
    End Function

    Public Sub Directory_Browser(ByRef Root_Directory As String)
        Dim dlg As New FolderBrowserDialog()
        dlg.SelectedPath = Root_Directory

        dlg.ShowDialog()
        Root_Directory = dlg.SelectedPath
    End Sub

#End Region

#Region "Read/Write All"

    Public Function Read_All_Text(File_Name As String) As String
        If Not File.Exists(File_Name) Then Return Nothing
        Dim Buffer As String
        Buffer = My.Computer.FileSystem.ReadAllText(File_Name)
        Return Buffer
    End Function

    Public Sub Write_All_Text(File_Name As String, Buffer As String, Optional Append As Boolean = False)
        My.Computer.FileSystem.WriteAllText(File_Name, Buffer, Append)
    End Sub

#End Region

#Region "Sequential Input File Operations"

    Public Function Open_Input_File(ByVal Path As String) As Boolean
        If File.Exists(Path) = False Then
            Message_Box.ShowDialog("File not found : " & Path)
            Return False
        End If
        Input_File = My.Computer.FileSystem.OpenTextFileReader(Path)
        Return (True)
    End Function

    Public Sub Close_Input_File()
        If Input_File IsNot Nothing Then Input_File.Close()
    End Sub

    Public Function Read_Line(ByRef Line As String) As Boolean
        If Input_File.EndOfStream Then Return False
        Line = Input_File.ReadLine
        Return True
    End Function

    Public Function End_Of_File() As Boolean
        If Input_File.EndOfStream Then
            Return True
        Else
            Return False
        End If
    End Function

    Public Function Parse_Line(ByRef Param() As String, ByVal Separator As String) As Boolean
        Dim L As String
        Dim I As Integer
        If Input_File.EndOfStream Then Return False
        L = Input_File.ReadLine
        Param = L.Split(Separator)
        For I = 0 To Param.Length - 1
            Param(I) = Param(I).Trim
        Next
        Return True
    End Function

#End Region

#Region "Sequential Output File Operations"

    Public Function Open_Output_File(ByVal Path As String, Optional ByVal Append As Boolean = False) As Boolean
        'If File.Exists(Path) = False Then
        '    Message_Box.ShowDialog("File not found : ", Path)
        '    Return False
        'End If
        Try
            Output_File = My.Computer.FileSystem.OpenTextFileWriter(Path, Append)
        Catch ex As Exception
            Message_Box.ShowDialog(ex.Message)
            Return False
        End Try

        Return True
    End Function

    Public Sub Close_Output_File()
        If Output_File IsNot Nothing Then
            Output_File.Flush()
            Output_File.Close()
        End If
    End Sub

    Public Sub Write(ByRef Line As String)
        Output_File.Write(Line)
    End Sub

    Public Sub Write_Line(ByRef Line As String)
        Output_File.WriteLine(Line)
    End Sub

#End Region

#Region "Table Operations"

    Public Sub Load_Table_From_Delimited_File(Table As DataTable, File_Path As String, Key As String, Create_Columns As Boolean)
        Dim P() As String = Nothing
        Dim Match As Boolean

        If Not Open_Input_File(File_Path) Then Exit Sub

        If Create_Columns Then
            'First row contains columns
            Table.Columns.Clear()
            Parse_Line(P, "~")
            For I = 0 To P.Length - 1
                Table.Columns.Add(P(I))
            Next
        End If

        While Parse_Line(P, "~")
            If Key = "" Then
                Match = True
            Else
                If Key = P(0) Then
                    Match = True
                Else
                    Match = False
                End If
            End If

            If Match Then
                If Not (P.Length = 1 And P(0) = "") Then
                    If P.Length < Table.Columns.Count Then
                        ReDim Preserve P(Table.Columns.Count - 1)
                        For I = 1 To P.Length - 1
                            If IsNothing(P(I)) Then P(I) = ""
                        Next
                    End If
                    Table.Rows.Add(P)
                End If
            End If

        End While

        Close_Input_File()

    End Sub

    Public Sub Save_Tables_To_Delimited_File(Tables As List(Of DataTable), File_Path As String, Create_Columns As Boolean)
        Dim Line As String = ""

        Dim P() As String = Nothing

        If Not Open_Output_File(File_Path) Then Exit Sub

        If Create_Columns Then
            Line = ""
            For C = 0 To Tables(0).Columns.Count - 1
                If C > 0 Then Line &= "~"
                Line &= Tables(0).Columns(C).ColumnName
            Next
            Write_Line(Line)
        End If

        For T = 0 To Tables.Count - 1
            For R = 0 To Tables(T).Rows.Count - 1
                Line = Tables(T).TableName
                For C = 1 To Tables(T).Columns.Count - 1
                    If C > 0 Then Line &= "~"
                    Line &= Tables(T).Rows(R).Item(C)
                Next
                Write_Line(Line)
            Next
        Next

        Close_Output_File()

    End Sub

    Public Sub Save_Table_To_Delimited_File(Table As DataTable, File_Path As String, Create_Columns As Boolean)
        Dim Line As String = ""

        Dim P() As String = Nothing

        If Not Open_Output_File(File_Path) Then Exit Sub

        If Create_Columns Then
            Line = ""
            For C = 0 To Table.Columns.Count - 1
                If C > 0 Then Line &= "~"
                Line &= Table.Columns(C).ColumnName
            Next
            Write_Line(Line)
        End If

        For R = 0 To Table.Rows.Count - 1
            Line = ""
            For C = 0 To Table.Columns.Count - 1
                If C > 0 Then Line &= "~"
                Line &= Table.Rows(R).Item(C)
            Next
            Write_Line(Line)
        Next

        Close_Output_File()

    End Sub

#End Region

#Region "Grid Operations"

    Public Sub Load_Grid_From_Table(Grid As DataGridView, Table As DataTable, Key As String)
        Dim P(Table.Columns.Count - 1) As String
        Dim Match As Boolean = True

        Grid.Rows.Clear()
        Grid.Columns.Clear()

        For C = 0 To Table.Columns.Count - 1
            Grid.Columns.Add(Table.Columns(C).ColumnName, Table.Columns(C).ColumnName)
            Grid.Columns(C).AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells
        Next

        For R = 0 To Table.Rows.Count - 1
            If Key <> "" Then
                If Table.Rows(R).Item(0) = Key Then
                    Match = True
                Else
                    Match = False
                End If
            End If
            If Match Then
                For C = 0 To Table.Columns.Count - 1
                    P(C) = Table.Rows(R).Item(C)
                Next
                Grid.Rows.Add(P)
            End If
        Next

    End Sub

    Public Sub Save_Grid_To_Table(Grid As DataGridView, Table As DataTable)
        Dim P(Grid.Columns.Count - 1) As String

        Grid.EndEdit()

        Table.Rows.Clear()
        Table.Columns.Clear()
        For I = 0 To Grid.Columns.Count - 1
            Table.Columns.Add(Grid.Columns(I).HeaderText)
        Next

        For R = 0 To Grid.Rows.Count - 1
            For C = 0 To Grid.Columns.Count - 1
                P(C) = Grid.Rows(R).Cells(C).Value
            Next
            Table.Rows.Add(P)
        Next

    End Sub

#End Region

#Region "Backup"

    Public Sub Backup_File(File_Name As String, Optional Use_Backup_Directory As Boolean = False)
        Dim F As String
        Dim Stamp As String
        Dim Day As String = Today.Day
        Dim Mo As String = Today.Month
        Dim Yr As String = Today.Year
        Dim Dr As String = Path.GetDirectoryName(File_Name) & "\"
        Dim Fn As String = Path.GetFileNameWithoutExtension(File_Name)
        Dim Ext As String = Path.GetExtension(File_Name)
        Dim Bdr As String = ""

        Mo = Mo.PadLeft(2, "0")
        Day = Day.PadLeft(2, "0")

        Stamp = Yr & "_" & Mo & "_" & Day

        If Use_Backup_Directory Then
            If Not Directory.Exists(Dr & "Backup") Then
                Directory.CreateDirectory(Dr & "Backup")
            End If
            Bdr = Dr & "Backup\"
        End If

        F = FileSystem.Dir(Bdr)
        Do While F <> ""
            If F.Contains(Fn) And F.Contains(Ext) And F.Contains(Stamp) Then
                FileSystem.Kill(Bdr & F)
            End If
            F = FileSystem.Dir()
        Loop

        Backup(Dr, Fn, Ext, Use_Backup_Directory)

    End Sub

    Private Sub Backup(Base_Path As String, Base_File_Name As String, Ext As String, Optional Use_Backup_Directory As Boolean = False)
        Dim Backup_File_Name As String = ""
        Dim Dr As String = ""
        Dim Bdr As String = ""
        Dim File_Names As New List(Of String)
        Dim File_Cnt As Integer = -1
        Dim Day As String
        Dim Mo As String
        Dim Hr As String
        Dim Min As String
        Dim Sec As String

        If Not File.Exists(Base_Path & Base_File_Name & Ext) Then Exit Sub

        Mo = Today.Month
        Day = Today.Day
        Hr = Now.Hour
        Min = Now.Minute
        Sec = Now.Second

        Mo = Mo.PadLeft(2, "0")
        Day = Day.PadLeft(2, "0")
        Hr = Hr.PadLeft(2, "0")
        Min = Min.PadLeft(2, "0")
        Sec = Sec.PadLeft(2, "0")

        Base_File_Name &= Ext
        Backup_File_Name = Path.GetFileNameWithoutExtension(Base_File_Name) &
            "_" & Today.Year & "_" & Mo & "_" & Day & "_" & Hr & "_" & Min & "_" & Sec & Ext

        If Use_Backup_Directory Then
            FileSystem.FileCopy(Base_Path & Base_File_Name, Base_Path & "Backup\" & Backup_File_Name)
            Bdr = FileSystem.Dir(Base_Path & "Backup\" & Path.GetFileNameWithoutExtension(Base_File_Name) & "*" & Path.GetExtension(Base_File_Name))
            Base_Path &= "Backup\"
        Else
            FileSystem.FileCopy(Base_Path & Base_File_Name, Base_Path & Backup_File_Name)
            Bdr = FileSystem.Dir(Base_Path & Path.GetFileNameWithoutExtension(Base_File_Name) & "*" & Path.GetExtension(Base_File_Name))
        End If

        Do While Bdr <> ""
            If Bdr <> Base_File_Name Then
                File_Cnt += 1
                File_Names.Add(Bdr)
            End If
            Bdr = FileSystem.Dir
        Loop

        File_Names.Sort()

        'System.Array.Sort(File_Names)

        For File_Cnt = 0 To File_Names.Count - 5
            FileSystem.Kill(Base_Path & File_Names(File_Cnt))
        Next

    End Sub

#End Region

End Class
