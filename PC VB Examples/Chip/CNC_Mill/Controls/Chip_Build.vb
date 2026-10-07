Imports System.IO

Public Class Chip_Build

#Region "Enums"

    Private Enum enum_Fixture_Columns
        Name
    End Enum

    Private Enum enum_Tool_Columns
        Name
        Tool_Seat_Z
        Tool_Measure_X
        Tool_Measure_Y
        Tool_Measure_Z
    End Enum

    Private Enum enum_Mount_Columns
        Name
        X_Offset
        Y_Offset
        Z_Delta
    End Enum

    Private Enum enum_Operation_Columns
        Name
        Time
        Tool
        Offset_Type
        Tool_Measurement_Type
        Seat_Z
        Measure_Z
        Measure_X
        Measure_Y
        Directions
    End Enum

    Private Enum enum_Task_Columns
        Enable
        Mount
        Time
        Program
        Part_Offset_X
        Part_Offset_Y
        Part_Offset_Z
        Material_Thickness
        Material_Length
        Material_Width
        Material_Origin
        Folder
    End Enum

    Private Enum enum_Image_Types
        Directory
        File
        CAD_Drawing
        DXF_Drawing
        CAM_Session
        G_Code
        Route
    End Enum



#End Region

#Region "Declarations"

    Public Current_Directory As String
    Public GCode_Control As Chip_Gcode
    Public Fixture_Name As String
    Public Fixture_Offsets_Control As Chip_Fixture_Offsets
    Public Fixture_Offset_Type As String

    Private Expanded_Nodes As New List(Of String)
    Private Root_Node As TreeNode
    Private Directory_Info As DirectoryInfo
    Private Image_List As New ImageList()

    Private Persist_Data As List(Of String)

    Private Tool_Changes_Made As Boolean = False
    Private Fixture_Changes_Made As Boolean = False
    Private Operation_Changes_Made As Boolean = False

    Private Ignore_Row_Enter As Boolean = False
    Private Ignore_Cell_Validating As Boolean = False

    Private Initialized As Boolean = False

    Private Tool_Grid_List As Class_Grid_List
    Private Fixture_Grid_List As Class_Grid_List
    Private Mount_Grid_Lists As List(Of Class_Grid_List)

    Private Operation_Grid_List As Class_Grid_List
    Private Task_Grid_Lists As List(Of Class_Grid_List)
    Private Operation_Grid_Header As New List(Of String)

    Private Chip_Build_Root As String = ""
    Private Chip_Tool_File As String = "" ' "C:\Old_Man_New\Tooling and Fixtures\Tool_Table.tol"
    Private Chip_Fixture_File As String = "" '"C:\Old_Man_New\Tooling and Fixtures\Fixtures.fix"

    Public Route_File As String

    Public GCode_Execution_Time As String
    Public GCode_Execution_Seconds As String

    Public Z_Part_Offset As Double


#End Region

#Region "Properties"

    Public Property File_Display_Width As Integer
        Get
            Return tree_Container.Width
        End Get
        Set(ByVal value As Integer)
            If value < 50 Then
                tree_Container.Width = 50
            Else
                tree_Container.Width = value
            End If
        End Set
    End Property

    Public Property Directory_Tree_Width As Integer
        Get
            Return tree_Container.SplitterDistance
        End Get
        Set(ByVal value As Integer)
            If value < 50 Then
                tree_Container.SplitterDistance = 50
            Else
                tree_Container.SplitterDistance = value
            End If
        End Set
    End Property

    Public Property Operations_Grid_Height As Integer
        Get
            Return grd_Operations.Height
        End Get
        Set(ByVal value As Integer)
            If value < 50 Then
                grd_Operations.Height = 50
            Else
                grd_Operations.Height = value
            End If
        End Set
    End Property

#End Region

#Region "Load, Initialize, Setup, Persistant_Data, Finish"

    Private Sub Chip_Build_Load(sender As Object, e As EventArgs) Handles Me.Load
        Dim dcs As New DataGridViewCellStyle
        Dim F As New Font("Arial", 10)

        dcs.Font = F
        dcs.WrapMode = DataGridViewTriState.True

        grd_Tools.DefaultCellStyle = dcs
        grd_Tools.ColumnHeadersDefaultCellStyle = dcs

        grd_Fixtures.DefaultCellStyle = dcs
        grd_Fixtures.ColumnHeadersDefaultCellStyle = dcs

        grd_Mounts.DefaultCellStyle = dcs
        grd_Mounts.ColumnHeadersDefaultCellStyle = dcs

        grd_Operations.DefaultCellStyle = dcs
        grd_Operations.ColumnHeadersDefaultCellStyle = dcs

        grd_Tasks.DefaultCellStyle = dcs
        grd_Tasks.ColumnHeadersDefaultCellStyle = dcs

    End Sub

    Public Sub Initialize()
        Image_List.Images.Add(Image.FromFile(Chip_Icons_Folder & "Folder.bmp"), Color.Magenta)
        Image_List.Images.Add(Image.FromFile(Chip_Icons_Folder & "File.bmp"), Color.Magenta)
        Image_List.Images.Add(Image.FromFile(Chip_Icons_Folder & "CAD.bmp"), Color.Magenta)
        Image_List.Images.Add(Image.FromFile(Chip_Icons_Folder & "DXF.bmp"), Color.Magenta)
        Image_List.Images.Add(Image.FromFile(Chip_Icons_Folder & "gear_32.bmp"), Color.Magenta)
        Image_List.Images.Add(Image.FromFile(Chip_Icons_Folder & "G_Codebmp.bmp"), Color.Magenta)
        Image_List.Images.Add(Image.FromFile(Chip_Icons_Folder & "Route.bmp"), Color.Magenta)
        tree_Files.ImageList = Image_List
        lst_Files.SmallImageList = Image_List

        Main_Form.ctl_Fixture_Offsets.Tool_Measured = False
        Main_Form.btn_Measure_Tool.State = "Not Measured"

        Tool_Grid_List = New Class_Grid_List(grd_Tools)
        Fixture_Grid_List = New Class_Grid_List(grd_Fixtures)
        Mount_Grid_Lists = New List(Of Class_Grid_List)
        Operation_Grid_List = New Class_Grid_List(grd_Operations)
        Task_Grid_Lists = New List(Of Class_Grid_List)

        Initialized = True
    End Sub

    Public Sub Setup(GCode_Ctl As Chip_Gcode, Fixture_Offsets_Ctl As Chip_Fixture_Offsets)
        GCode_Control = GCode_Ctl
        Fixture_Offsets_Control = Fixture_Offsets_Ctl

        Chip_Build_Root = Main_Form.txt_Build_Root_Directory.Text
        Chip_Tool_File = Chip_Build_Root & "\Tooling and Fixtures\Tool_Table.tol"
        Chip_Fixture_File = Chip_Build_Root & "\Tooling and Fixtures\Fixtures.fix"

        Load_Grid_List(Chip_Tool_File, Tool_Grid_List, "Tool")
        Load_Grid_List(Chip_Fixture_File, Fixture_Grid_List, "Fixture", Mount_Grid_Lists, "Mount", grd_Mounts)
        Operation_Grid_Header.Clear()
        If Load_Grid_List(Route_File, Operation_Grid_List, "Operation", Task_Grid_Lists, "Task", grd_Tasks, Operation_Grid_Header) Then
            Fixture_Name = Get_Header_Info(Operation_Grid_Header, "Fixture")
            lab_Fixture_Name.Text = Fixture_Name
            lab_Route_Name.Text = Route_File
        End If

        Fill_Tree()

        Expand_Tree(Persist_Data)

    End Sub

    Private Sub Expand_Tree(Node_Names As List(Of String))
        tree_Files.Nodes(0).Expand()
        Dim Last_Node As TreeNode = Nothing
        For I = 0 To Node_Names.Count - 1
            Dim Nodes() As TreeNode = tree_Files.Nodes.Find(Node_Names(I), True)
            For N = 0 To Nodes.Count - 1
                Nodes(N).Expand()
                If I = Node_Names.Count - 1 Then
                    If N = Nodes.Count - 1 Then
                        Last_Node = Nodes(N)
                    End If
                End If
            Next
        Next

        If Not IsNothing(Last_Node) Then
            Fill_Tree_Files_List(Last_Node)
        End If

    End Sub

    Public Function Finish() As Boolean
        If Not Finish_Tools() Then Return False
        If Not Finish_Mounts() Then Return False
        If Not Finish_Fixtures() Then Return False
        If Not Finish_Operations() Then Return False
        Return True
    End Function

    Public Sub Get_Persistant_Node_Data(ByRef Data As List(Of String))
        Expanded_Nodes.Clear()
        Find_Expanded_Nodes(tree_Files.Nodes(0))
        Expanded_Nodes.Add(Current_Directory)
        Data = Expanded_Nodes
    End Sub

    Public Sub Set_Persistant_Data(ByRef Data As List(Of String))
        Persist_Data = Data
    End Sub

    Private Sub Me_Leave(sender As Object, e As System.EventArgs) Handles Me.Leave
        grd_Tools.EndEdit()

        Finish_Tools()
        Finish_Mounts()
        Finish_Fixtures()
        Finish_Operations()


        'grd_Fixtures.EndEdit()
        'If Fixture_Changes_Made Then Finish_Fixtures()
    End Sub

#End Region

#Region "Interface to G_Code control"

    Public Function Get_Next_Program(ByRef File_Name As String, ByRef Index As Integer, ByRef Mode As String) As Boolean
        For I = 0 To grd_Tasks.Rows.Count - 1
            Mode = grd_Tasks.Rows(I).Cells(enum_Task_Columns.Enable).Value
            If Mode <> "" Then
                Dim Program As String = grd_Tasks.Rows(I).Cells(enum_Task_Columns.Program).Value
                Dim Folder As String = grd_Tasks.Rows(I).Cells(enum_Task_Columns.Folder).Value
                Index = I
                File_Name = Folder & Program
                Select Case Mode
                    Case "Run"
                        Dim dlg As New dlg_Message_Box
                        dlg.ShowDialog("Load program : " & Program, "Run Program", MessageBoxButtons.YesNo)
                        If dlg.DialogResult = DialogResult.Yes Then
                            Return True
                        Else
                            Return False
                        End If
                    Case "Auto"
                        Flash_Message("Loading program : " & Program)
                        Return True
                End Select
            End If
        Next

        Return False
    End Function

    Public Sub Program_Done(Index As Integer)
        grd_Tasks.Rows(Index).Cells(enum_Task_Columns.Enable).Value = ""
    End Sub

#End Region

#Region "Grid Utilities"

    Public Function Load_Grid_List(File_Name As String, Grid_List As Class_Grid_List, Key As String,
                              Optional Child_Lists As List(Of Class_Grid_List) = Nothing,
                              Optional Child_Key As String = "", Optional Child_Grid As DataGridView = Nothing,
                              Optional ByRef Header As List(Of String) = Nothing) As Boolean
        Dim Buff() As String
        Dim S() As String
        Dim Child_List As Class_Grid_List = Nothing
        Dim Err_Flag As Boolean = False

        Grid_List.Clear()
        If Not IsNothing(Child_Lists) Then Child_Lists.Clear()

        If File.Exists(File_Name) Then
            Buff = File.ReadAllLines(File_Name)
            For R = 0 To Buff.Count - 1
                If Buff(R) <> "" Then
                    S = Buff(R).Split("~")

                    Select Case S(0)
                        Case "Header"
                            If Not IsNothing(Header) Then
                                Header.Add(Buff(R).Replace("Header~", ""))
                            End If
                        Case Key
                            Grid_List.Add_List_Row(S, 1)
                            If Not IsNothing(Child_Lists) Then
                                Child_List = New Class_Grid_List(Child_Grid)
                                Child_Lists.Add(Child_List)
                            End If
                        Case Child_Key
                            If Key = "Operation" And Child_Key = "Task" Then
                                If S.Count <> 13 Then
                                    Err_Flag = True
                                Else
                                    ' if relative sysmbol then replace with installation folder
                                    S(12) = S(12).Replace("$$DEFAULT_BUILD_ROOT$$", Chip_Root_Path & "\Chip_Build")
                                End If
                            End If

                            Child_List.Add_List_Row(S, 1)
                    End Select
                End If
            Next

            Grid_List.Load_Grid()
            If Not IsNothing(Child_Lists) Then
                If Child_Lists.Count > 0 Then
                    Child_Lists(0).Load_Grid()
                End If
            End If

            If Err_Flag Then
                Show_Error("Error detected in route file")


            End If

            Return True
        Else
            Show_Error("File not found : " & File_Name)
            Return False
        End If


    End Function

    Public Sub Save_Grid_List(File_Name As String, Grid_List As Class_Grid_List, Parent_Key As String,
                              Optional Child_Lists As List(Of Class_Grid_List) = Nothing, Optional Child_Key As String = "",
                              Optional Header As List(Of String) = Nothing)

        Dim Buff As New List(Of String)
        Dim S As String
        Dim Idx As Integer = -1

        If Not IsNothing(Header) Then
            For I = 0 To Header.Count - 1
                Buff.Add("Header~" & Header(I))
            Next
        End If

        For I = 0 To Grid_List.List_Rows_Count - 1
            S = Parent_Key & "~" & Grid_List.Get_Packed_List_Row(I)
            Buff.Add(S)
            Idx += 1
            If Not IsNothing(Child_Lists) Then
                For R = 0 To Child_Lists(Idx).List_Rows_Count - 1
                    S = Child_Key & "~" & Child_Lists(Idx).Get_Packed_List_Row(R)
                    If (R = 12) Then ' if folder is within installation folder replace with relative symbol
                        S = S.Replace(Chip_Root_Path & "\Chip_Build", "$$DEFAULT_BUILD_ROOT$$")
                    End If

                    Buff.Add(S)
                Next
            End If
        Next
        File.WriteAllLines(File_Name, Buff)
    End Sub

    Public Sub Insert_Child_List(Grid_Lists As List(Of Class_Grid_List), Index As Integer, Child_Grid As DataGridView)
        Ignore_Row_Enter = True
        Dim New_List As New Class_Grid_List(Child_Grid)
        Grid_Lists.Insert(Index, New_List)
        Ignore_Row_Enter = False
    End Sub

    Public Function Get_Header_Info(Header As List(Of String), Key As String) As String
        Dim S() As String
        For I = 0 To Header.Count - 1
            S = Header(I).Split("~")
            If S(0) = Key Then
                Return S(1)
            End If
        Next
        Return ""
    End Function

#End Region

#Region "Tool Grid"

    Private Sub Check_For_Tool_Edit()
        If IsNothing(grd_Tools.CurrentRow) Then Exit Sub
        If IsNothing(grd_Tools.CurrentCell) Then Exit Sub
        Dim Current_Row As Integer = grd_Tools.CurrentRow.Index
        Dim Current_Cell As Integer = grd_Tools.CurrentCell.ColumnIndex

        If grd_Tools.CurrentCell.FormattedValue <> Tool_Grid_List.Get_Cell_Value(Current_Row, Current_Cell) Then
            Tool_Grid_List.Set_Cell_Value(Current_Row, Current_Cell, grd_Tools.CurrentCell.FormattedValue)
            Tool_Changes_Made = True
        End If
    End Sub

    Private Sub ts_Tools_ItemClicked(sender As Object, e As ToolStripItemClickedEventArgs) Handles ts_Tools.ItemClicked
        Dim Current_Row As Integer
        grd_Tools.EndEdit()
        Check_For_Tool_Edit()

        If e.ClickedItem.Text = "Insert" Then 'Handle empty grid
            If grd_Tools.Rows.Count = 0 Then
                Current_Row = -1
            Else
                Current_Row = grd_Tools.CurrentRow.Index
            End If
        Else
            If IsNothing(grd_Tools.CurrentRow) Then Exit Sub
            Current_Row = grd_Tools.CurrentRow.Index
        End If

        Select Case e.ClickedItem.Text
            Case "Insert"
                Tool_Grid_List.Insert_Row(Current_Row + 1)
                Tool_Changes_Made = True
            Case "Delete"
                Tool_Grid_List.Delete_Row(Current_Row)
                Tool_Changes_Made = True
            Case "Copy"
                Tool_Grid_List.Copy_Row(Current_Row)
                Tool_Changes_Made = True
            Case "Move Up"
                Tool_Grid_List.Move_Row_Up(Current_Row, Tool_Changes_Made)
                Tool_Changes_Made = True
            Case "Move Down"
                Tool_Grid_List.Move_Row_Down(Current_Row, Tool_Changes_Made)
                Tool_Changes_Made = True
        End Select

    End Sub

    Private Sub grd_Tools_CellValidating(sender As Object, e As System.Windows.Forms.DataGridViewCellValidatingEventArgs) Handles grd_Tools.CellValidating
        If Ignore_Row_Enter Then Exit Sub
        If e.FormattedValue <> Tool_Grid_List.Get_Cell_Value(e.RowIndex, e.ColumnIndex) Then
            Tool_Grid_List.Set_Cell_Value(e.RowIndex, e.ColumnIndex, e.FormattedValue)
            Tool_Changes_Made = True
        End If
    End Sub

    Private Function Finish_Tools() As Boolean

        If Tool_Changes_Made Then
            Dim Count As Integer

            For I = 0 To grd_Tools.RowCount - 1
                Count = 0
                For J = 0 To grd_Tools.RowCount - 1
                    If grd_Tools.Rows(I).Cells(enum_Tool_Columns.Name).Value = grd_Tools.Rows(J).Cells(enum_Tool_Columns.Name).Value Then
                        Count += 1
                        If Count > 1 Then
                            Show_Error("Tool names must be unique." & vbCrLf & grd_Tools.Rows(I).Cells(enum_Tool_Columns.Name).Value)
                            Return False
                        End If
                    End If
                Next
            Next

            Save_Grid_List(Chip_Tool_File, Tool_Grid_List, "Tool")
            Tool_Changes_Made = False
            Flash_Message("Tool Table Saved")
            Tool_Changes_Made = False
        End If

        Return True
    End Function

#End Region

#Region "Fixture Grid"

    Private Sub Check_For_Fixture_Edit()
        If IsNothing(grd_Fixtures.CurrentRow) Then Exit Sub
        If IsNothing(grd_Fixtures.CurrentCell) Then Exit Sub
        Dim Current_Row As Integer = grd_Fixtures.CurrentRow.Index
        Dim Current_Cell As Integer = grd_Fixtures.CurrentCell.ColumnIndex

        If grd_Fixtures.CurrentCell.FormattedValue <> Fixture_Grid_List.Get_Cell_Value(Current_Row, Current_Cell) Then
            Fixture_Grid_List.Set_Cell_Value(Current_Row, Current_Cell, grd_Fixtures.CurrentCell.FormattedValue)
            Fixture_Changes_Made = True
        End If
    End Sub

    Private Sub ts_Fixtures_ItemClicked(sender As Object, e As ToolStripItemClickedEventArgs) Handles ts_Fixtures.ItemClicked
        Dim Current_Row As Integer
        grd_Fixtures.EndEdit()
        grd_Mounts.EndEdit()
        Check_For_Fixture_Edit()

        If e.ClickedItem.Text = "Insert" Then 'Handle empty grid
            If grd_Fixtures.Rows.Count = 0 Then
                Current_Row = -1
            Else
                Current_Row = grd_Fixtures.CurrentRow.Index
            End If
        Else
            If IsNothing(grd_Fixtures.CurrentRow) Then Exit Sub
            Current_Row = grd_Fixtures.CurrentRow.Index
        End If

        Select Case e.ClickedItem.Text
            Case "Insert"
                Fixture_Grid_List.Insert_Row(Current_Row + 1)
                Insert_Child_List(Mount_Grid_Lists, Current_Row + 1, grd_Mounts)
                Mount_Grid_Lists(Current_Row + 1).Load_Grid()
                Fixture_Changes_Made = True
            Case "Delete"
                Fixture_Grid_List.Delete_Row(Current_Row)
                Mount_Grid_Lists.RemoveAt(Current_Row)

                'tktk if deleted last element set current to previous
                If (Current_Row = Fixture_Grid_List.List_Rows_Count) Then
                    Current_Row = Current_Row - 1
                End If

                Mount_Grid_Lists(Current_Row).Load_Grid()
                Fixture_Changes_Made = True
            Case "Copy"
                Dim New_List As Class_Grid_List
                New_List = Class_Grid_List.Copy_List(Mount_Grid_Lists(Current_Row), grd_Mounts)
                Mount_Grid_Lists.Insert(Current_Row + 1, New_List)
                Fixture_Grid_List.Copy_Row(Current_Row)
                Mount_Grid_Lists(Current_Row).Load_Grid()
                Fixture_Changes_Made = True
            Case "Move Up"
                Fixture_Grid_List.Move_Row_Up(Current_Row, Fixture_Changes_Made)
                Class_Grid_List.Move_List_Up(Mount_Grid_Lists, Current_Row)
                Mount_Grid_Lists(Current_Row - 1).Load_Grid()
            Case "Move Down"
                Fixture_Grid_List.Move_Row_Down(Current_Row, Fixture_Changes_Made)
                Class_Grid_List.Move_List_Down(Mount_Grid_Lists, Current_Row)
                Mount_Grid_Lists(Current_Row + 1).Load_Grid()
        End Select

    End Sub

    Private Sub grd_Fixtures_CellValidating(sender As Object, e As System.Windows.Forms.DataGridViewCellValidatingEventArgs) Handles grd_Fixtures.CellValidating
        If e.FormattedValue <> Fixture_Grid_List.Get_Cell_Value(e.RowIndex, e.ColumnIndex) Then
            Fixture_Grid_List.Set_Cell_Value(e.RowIndex, e.ColumnIndex, e.FormattedValue)
            Fixture_Changes_Made = True
        End If
    End Sub

    Private Sub grd_Fixtures_RowEnter(sender As Object, e As System.Windows.Forms.DataGridViewCellEventArgs) Handles grd_Fixtures.RowEnter
        If e.RowIndex < 0 Then Exit Sub
        grd_Mounts.EndEdit()
        ' tktk make within row range
        If (e.RowIndex >= Mount_Grid_Lists.Count) Then
            Mount_Grid_Lists(Mount_Grid_Lists.Count - 1).Load_Grid()
        Else
            Mount_Grid_Lists(e.RowIndex).Load_Grid()
        End If
    End Sub

    Private Function Finish_Fixtures() As Boolean

        If Fixture_Changes_Made Then
            Dim Count As Integer

            For I = 0 To grd_Fixtures.RowCount - 1
                Count = 0
                For J = 0 To grd_Fixtures.RowCount - 1
                    If grd_Fixtures.Rows(I).Cells(enum_Fixture_Columns.Name).Value = grd_Fixtures.Rows(J).Cells(enum_Fixture_Columns.Name).Value Then
                        Count += 1
                        If Count > 1 Then
                            Show_Error("Fixture names must be unique." & vbCrLf & grd_Fixtures.Rows(I).Cells(enum_Fixture_Columns.Name).Value)
                            Return False
                        End If
                    End If
                Next
            Next

            Save_Grid_List(Chip_Fixture_File, Fixture_Grid_List, "Fixture", Mount_Grid_Lists, "Mount")
            Fixture_Changes_Made = False
            Flash_Message("Fixtures Table Saved")

        End If

        Return True
    End Function

#End Region

#Region "Mount Grid"

    Private Function Get_Mount_Offsets(Fixture As String, Mount As String,
                                       ByRef X_Offset As Single, ByRef Y_Offset As Single,
                                       ByRef Z_Delta As Single) As Boolean

        For F = 0 To Fixture_Grid_List.List_Rows_Count - 1
            If Fixture_Grid_List.Get_Cell_Value(F, enum_Fixture_Columns.Name) = Fixture Then
                For M = 0 To Mount_Grid_Lists(F).List_Rows_Count - 1
                    If Mount_Grid_Lists(F).Get_Cell_Value(M, enum_Mount_Columns.Name) = Mount Then
                        X_Offset = Mount_Grid_Lists(F).Get_Cell_Value(M, enum_Mount_Columns.X_Offset)
                        Y_Offset = Mount_Grid_Lists(F).Get_Cell_Value(M, enum_Mount_Columns.Y_Offset)
                        Z_Delta = Mount_Grid_Lists(F).Get_Cell_Value(M, enum_Mount_Columns.Z_Delta)
                        Return True
                    End If
                Next
            End If
        Next
        Return False
    End Function

    Private Sub Check_For_Mount_Edit()
        If IsNothing(grd_Mounts.CurrentRow) Then Exit Sub
        If IsNothing(grd_Mounts.CurrentCell) Then Exit Sub
        Dim Current_Row As Integer = grd_Mounts.CurrentRow.Index
        Dim Current_Cell As Integer = grd_Mounts.CurrentCell.ColumnIndex
        Dim Current_Fixture As Integer = grd_Fixtures.CurrentRow.Index

        If grd_Mounts.CurrentCell.FormattedValue <> Mount_Grid_Lists(Current_Fixture).Get_Cell_Value(Current_Row, Current_Cell) Then
            Mount_Grid_Lists(Current_Fixture).Set_Cell_Value(Current_Row, Current_Cell, grd_Mounts.CurrentCell.FormattedValue)
            Fixture_Changes_Made = True
        End If

    End Sub

    Private Sub ts_Mounts_ItemClicked(sender As Object, e As ToolStripItemClickedEventArgs) Handles ts_Mounts.ItemClicked
        Dim Current_Fixture As Integer = grd_Fixtures.CurrentRow.Index
        If Current_Fixture < 0 Then Exit Sub
        Dim Current_Row As Integer
        grd_Mounts.EndEdit()

        Check_For_Mount_Edit()

        If e.ClickedItem.Text = "Insert" Then 'Handle empty grid
            If grd_Mounts.Rows.Count = 0 Then
                Current_Row = -1
            Else
                Current_Row = grd_Mounts.CurrentRow.Index
            End If
        Else
            If IsNothing(grd_Mounts.CurrentRow) Then Exit Sub
            Current_Row = grd_Mounts.CurrentRow.Index
        End If

        Select Case e.ClickedItem.Text
            Case "Insert"
                Mount_Grid_Lists(Current_Fixture).Insert_Row(Current_Row + 1)
                Fixture_Changes_Made = True
            Case "Delete"
                Mount_Grid_Lists(Current_Fixture).Delete_Row(Current_Row)
                Fixture_Changes_Made = True
            Case "Copy"
                Mount_Grid_Lists(Current_Fixture).Copy_Row(Current_Row)
                Fixture_Changes_Made = True
            Case "Move Up"
                Mount_Grid_Lists(Current_Fixture).Move_Row_Up(Current_Row, Fixture_Changes_Made)
            Case "Move Down"
                Mount_Grid_Lists(Current_Fixture).Move_Row_Down(Current_Row, Fixture_Changes_Made)
        End Select

    End Sub

    Private Sub grd_Mounts_CellValidating(sender As Object, e As System.Windows.Forms.DataGridViewCellValidatingEventArgs) Handles grd_Mounts.CellValidating
        If IsNothing(grd_Fixtures.CurrentRow) Then Exit Sub
        Dim Current_Fixture As Integer = grd_Fixtures.CurrentRow.Index
        If Current_Fixture < 0 Then Exit Sub

        If e.FormattedValue <> Mount_Grid_Lists(Current_Fixture).Get_Cell_Value(e.RowIndex, e.ColumnIndex) Then
            Mount_Grid_Lists(Current_Fixture).Set_Cell_Value(e.RowIndex, e.ColumnIndex, e.FormattedValue)
            Fixture_Changes_Made = True
        End If
    End Sub


    Private Sub grd_Mounts_CellClick(sender As Object, e As System.Windows.Forms.DataGridViewCellEventArgs) Handles grd_Mounts.CellClick
        If e.RowIndex < 0 Then Exit Sub
        Dim Mount As Integer = grd_Mounts.CurrentRow.Index
        Dim S As String = ""

        Select Case e.ColumnIndex

            Case enum_Mount_Columns.X_Offset
                Dim Num As Single = grd_Mounts.Rows(e.RowIndex).Cells(enum_Mount_Columns.X_Offset).Value
                Num = Get_User_Input(Num, 0, FormatNumber(Kflop.X_Abs, 4), Main_Form.txt_Surface_X.Text, "",
                                     "The Surface button returns the 'Surface X'" & vbCrLf &
                                     "measurement from the Jog/Probe Tab.")
                grd_Mounts.Rows(e.RowIndex).Cells(enum_Mount_Columns.X_Offset).Value = Num

            Case enum_Mount_Columns.Y_Offset
                Dim Num As Single = grd_Mounts.Rows(e.RowIndex).Cells(enum_Mount_Columns.Y_Offset).Value
                Num = Get_User_Input(Num, 0, FormatNumber(Kflop.Y_Abs, 4), Main_Form.txt_Surface_Y.Text, "",
                                     "The Surface button returns the 'Surface Y'" & vbCrLf &
                                     "measurement from the Jog/Probe Tab.")
                grd_Mounts.Rows(e.RowIndex).Cells(enum_Mount_Columns.Y_Offset).Value = Num

            Case enum_Mount_Columns.Z_Delta
                Dim Num As Single = grd_Mounts.Rows(e.RowIndex).Cells(enum_Mount_Columns.Z_Delta).Value
                Num = Get_User_Input(Num, 0, Kflop.Z_Abs, Main_Form.txt_Tool_Setter_Mount_Z_Delta.Text, "",
                                     "The Surface button returns the 'Mount Z Delta'" & vbCrLf &
                                     "measurement from the Toolsetter Tab.")
                grd_Mounts.Rows(e.RowIndex).Cells(enum_Mount_Columns.Z_Delta).Value = Num

        End Select

        Move_To_Cell(grd_Mounts, e.RowIndex, e.ColumnIndex - 1)
        Move_To_Cell(grd_Mounts, e.RowIndex, e.ColumnIndex)



        Dim dlg As New dlg_Message_Box
        dlg.ShowDialog("Do you want to reload this fixture?" & vbCrLf & vbCrLf &
                       "NOTE : G Code program must be reloaded for" & vbCrLf & vbCrLf &
                       "these changes to take effect.", MessageBoxButtons.YesNo)
        If dlg.DialogResult <> DialogResult.Yes Then
            Fixture_Name = grd_Fixtures.CurrentRow.Cells(enum_Fixture_Columns.Name).Value
            lab_Fixture_Name.Text = Fixture_Name
        End If

        Ignore_Cell_Validating = False
        Fixture_Changes_Made = True

    End Sub



    Private Function Finish_Mounts() As Boolean

        If Fixture_Changes_Made Then
            Dim Count As Integer

            For I = 0 To grd_Mounts.RowCount - 1
                Count = 0
                For J = 0 To grd_Mounts.RowCount - 1
                    If grd_Mounts.Rows(I).Cells(enum_Mount_Columns.Name).Value = grd_Mounts.Rows(J).Cells(enum_Mount_Columns.Name).Value Then
                        Count += 1
                        If Count > 1 Then
                            Show_Error("Mount names must be unique." & vbCrLf & grd_Mounts.Rows(I).Cells(enum_Mount_Columns.Name).Value)
                            Return False
                        End If
                    End If
                Next
            Next
        End If

        Return True
    End Function

#End Region

    Private Sub Load_Route(File_Name As String)
        Route_File = File_Name
        Ignore_Cell_Validating = True
        Operation_Grid_Header.Clear()
        If Load_Grid_List(Route_File, Operation_Grid_List, "Operation", Task_Grid_Lists, "Task", grd_Tasks, Operation_Grid_Header) Then
            lab_Route_Name.Text = Route_File
            Fixture_Name = Get_Header_Info(Operation_Grid_Header, "Fixture")
            lab_Fixture_Name.Text = Fixture_Name
        End If
        Ignore_Cell_Validating = False
    End Sub

#Region "Route Toolstrip"

    Private Sub ts_Route_ItemClicked(sender As Object, e As ToolStripItemClickedEventArgs) Handles ts_Route.ItemClicked

        Select Case e.ClickedItem.Text
            Case "Load Route"
                Dim Df As New Class_Data_File
                Dim File_Name As String = Df.Select_Input_File(Route_File, "Route Files (*.rte)|*.rte|Route Files (*.rte)|*.rte|All files (*.*)|*.*")
                If File_Name <> "" Then
                    Load_Route(File_Name)
                End If

            Case "Load Fixture"
                Dim dlg As New dlg_Fixture
                For I = 0 To Fixture_Grid_List.List_Rows_Count - 1
                    dlg.Add_Fixture(Fixture_Grid_List.Get_Cell_Value(I, 0))
                Next
                dlg.ShowDialog()
                If dlg.DialogResult = DialogResult.OK Then
                    Fixture_Name = dlg.Fixture
                    lab_Fixture_Name.Text = Fixture_Name
                    Operation_Changes_Made = True
                End If

            Case "New Route"
                Dim DF As New Class_Data_File
                Dim Folder As String = Path.GetDirectoryName(lab_Route_Name.Text)
                Dim Fn As String = ""
                Dim Buff As String = "New"
                Dim dlg As New FolderBrowserDialog()

                Finish_Operations()

                dlg.ShowNewFolderButton = True
                dlg.SelectedPath = Folder

                If dlg.ShowDialog() = DialogResult.OK Then
                    Folder = dlg.SelectedPath
                Else
                    Exit Sub
                End If

                Input_Dialog.Input_Value = ""
                Input_Dialog.Default_Value = ""
                Input_Dialog.Current_Position = ""
                Input_Dialog.Surface = ""
                Input_Dialog.Note = ""

                Input_Dialog.Prompt = "Enter File Name"
                Input_Dialog.ShowDialog()
                If Input_Dialog.DialogResult <> DialogResult.OK Then
                    Exit Sub
                End If

                Fn = Input_Dialog.Input_Value

                If Not Is_Valid_FileName(Folder & "\" & Fn) Then
                    Show_Error("Invalid File Name : " & Fn)
                    Exit Sub
                End If

                lab_Route_Name.Text = Folder & "\" & Fn
                Route_File = Folder & "\" & Fn & ".rte"
                Current_Directory = Folder

                grd_Operations.Rows.Clear()
                grd_Tasks.Rows.Clear()
                Operation_Grid_List.Clear()
                Task_Grid_Lists.Clear()
                Operation_Changes_Made = True

            Case "Fix Route"
                Fix_Route()
        End Select

    End Sub

    Function Is_Valid_FileName(ByVal Name As String) As Boolean
        If Name Is Nothing Then
            Return False
        End If
        For Each badChar As Char In System.IO.Path.GetInvalidPathChars
            If InStr(Name, badChar) > 0 Then
                Return False
            End If
        Next
        Return True
    End Function

#End Region


    Private Sub Fix_Route()
        Dim Df As New Class_Data_File
        Dim File_Name As String = Route_File
        Dim Buff() As String
        Dim S() As String
        Dim T(13) As String
        Dim Line As String

        If File.Exists(File_Name) Then
            Buff = File.ReadAllLines(File_Name)
            For R = 0 To Buff.Count - 1
                If Buff(R) <> "" Then
                    S = Buff(R).Split("~")

                    If S(0) = "Task" Then

                        If S.Count = 13 Then
                            Show_Error("File may have already been fixed")
                            Exit Sub
                        End If

                        For I = 0 To 12
                            Select Case I
                                Case < 7
                                    T(I) = S(I)
                                Case 7
                                    T(I) = "0"
                                Case > 7
                                    T(I) = S(I - 1)
                            End Select
                        Next

                        Line = ""

                        For I = 0 To T.Count - 1
                            Line &= T(I)
                            If I < T.Count - 2 Then Line &= "~"
                        Next
                        Buff(R) = Line
                    End If

                End If
            Next

            File.WriteAllLines(File_Name, Buff)

            Load_Route(File_Name)
        End If
    End Sub

#Region "Operations Grid"

    Private Sub Check_For_Operation_Edit()
        If IsNothing(grd_Operations.CurrentRow) Then Exit Sub
        If IsNothing(grd_Operations.CurrentCell) Then Exit Sub
        Dim Current_Row As Integer = grd_Operations.CurrentRow.Index
        Dim Current_Cell As Integer = grd_Operations.CurrentCell.ColumnIndex

        If grd_Operations.CurrentCell.FormattedValue <> Operation_Grid_List.Get_Cell_Value(Current_Row, Current_Cell) Then
            Operation_Grid_List.Set_Cell_Value(Current_Row, Current_Cell, grd_Operations.CurrentCell.FormattedValue)
            Operation_Changes_Made = True
        End If
    End Sub

    Private Sub ts_Operations_ItemClicked(sender As Object, e As ToolStripItemClickedEventArgs) Handles ts_Operations.ItemClicked
        Dim Current_Row As Integer
        grd_Operations.EndEdit()
        grd_Tasks.EndEdit()
        'Check_For_Operation_Edit()

        Select Case e.ClickedItem.Text
            Case "Insert"
                If grd_Operations.Rows.Count = 0 Then
                    Current_Row = -1
                Else
                    Current_Row = grd_Operations.CurrentRow.Index
                End If
            Case Else
                If IsNothing(grd_Operations.CurrentRow) Then Exit Sub
                Current_Row = grd_Operations.CurrentRow.Index
        End Select

        Select Case e.ClickedItem.Text
            Case "Insert"
                Operation_Grid_List.Insert_Row(Current_Row + 1)
                Insert_Child_List(Task_Grid_Lists, Current_Row + 1, grd_Tasks)
                Task_Grid_Lists(Current_Row + 1).Load_Grid()
                Operation_Changes_Made = True
            Case "Delete"
                Task_Grid_Lists.RemoveAt(Current_Row)
                Operation_Grid_List.Delete_Row(Current_Row)
                If grd_Operations.Rows.Count > 0 Then
                    Operation_Grid_List.Load_Grid()
                Else
                    grd_Tasks.Rows.Clear()
                End If
                Operation_Changes_Made = True
            Case "Copy"
                Dim New_List As Class_Grid_List
                New_List = Class_Grid_List.Copy_List(Task_Grid_Lists(Current_Row), grd_Tasks)
                Task_Grid_Lists.Insert(Current_Row + 1, New_List)
                Operation_Grid_List.Copy_Row(Current_Row)
                Task_Grid_Lists(Current_Row).Load_Grid()
                Operation_Changes_Made = True
            Case "Move Up"
                Operation_Grid_List.Move_Row_Up(Current_Row, Operation_Changes_Made)
                Class_Grid_List.Move_List_Up(Task_Grid_Lists, Current_Row)
                Task_Grid_Lists(Current_Row - 1).Load_Grid()
            Case "Move Down"
                Operation_Grid_List.Move_Row_Down(Current_Row, Operation_Changes_Made)
                Class_Grid_List.Move_List_Down(Task_Grid_Lists, Current_Row)
                Task_Grid_Lists(Current_Row + 1).Load_Grid()

            Case "Tool Defaults"
                Message_Box.ShowDialog("This function will set all tool parameters to defaults. Continue?", "", vbYesNo)
                If Message_Box.DialogResult <> DialogResult.OK Then
                    For I = 0 To grd_Operations.Rows.Count - 1
                        For J = 0 To grd_Tools.Rows.Count - 1
                            If grd_Tools.Rows(J).Cells(enum_Tool_Columns.Name).Value = grd_Operations.Rows(I).Cells(enum_Operation_Columns.Tool).Value Then
                                grd_Operations.Rows(I).Cells(enum_Operation_Columns.Seat_Z).Value = grd_Tools.Rows(J).Cells(enum_Tool_Columns.Tool_Seat_Z).Value
                                grd_Operations.Rows(I).Cells(enum_Operation_Columns.Measure_X).Value = grd_Tools.Rows(J).Cells(enum_Tool_Columns.Tool_Measure_X).Value
                                grd_Operations.Rows(I).Cells(enum_Operation_Columns.Measure_Y).Value = grd_Tools.Rows(J).Cells(enum_Tool_Columns.Tool_Measure_Y).Value
                                grd_Operations.Rows(I).Cells(enum_Operation_Columns.Measure_Z).Value = grd_Tools.Rows(J).Cells(enum_Tool_Columns.Tool_Measure_Z).Value
                            End If
                        Next
                    Next
                    Tool_Changes_Made = True
                End If
        End Select

    End Sub

    Private Sub grd_Operations_CellValidating(sender As Object, e As System.Windows.Forms.DataGridViewCellValidatingEventArgs) Handles grd_Operations.CellValidating
        If Ignore_Cell_Validating Then Exit Sub
        If e.FormattedValue <> Operation_Grid_List.Get_Cell_Value(e.RowIndex, e.ColumnIndex) Then
            Operation_Grid_List.Set_Cell_Value(e.RowIndex, e.ColumnIndex, e.FormattedValue)
            Operation_Changes_Made = True
        End If
    End Sub

    Private Sub grd_Operations_RowEnter(sender As Object, e As System.Windows.Forms.DataGridViewCellEventArgs) Handles grd_Operations.RowEnter
        If e.RowIndex < 0 Then Exit Sub
        grd_Tasks.EndEdit()
        If e.RowIndex < Task_Grid_Lists.Count Then
            Task_Grid_Lists(e.RowIndex).Load_Grid()
        End If
    End Sub

    Private Sub grd_Operations_CellClick(sender As Object, e As System.Windows.Forms.DataGridViewCellEventArgs) Handles grd_Operations.CellClick
        If e.RowIndex < 0 Then Exit Sub

        Select Case e.ColumnIndex
            Case enum_Operation_Columns.Tool

                Dim dlg As New dlg_Tools
                For I = 0 To Tool_Grid_List.List_Rows_Count - 1
                    dlg.Add_Tool(Tool_Grid_List.Get_List_Row(I))
                Next

                dlg.ShowDialog()
                If dlg.DialogResult = DialogResult.OK Then

                    Operation_Grid_List.Set_Cell_Value(e.RowIndex, enum_Operation_Columns.Tool, dlg.Tool_Name)
                    grd_Operations.Rows(e.RowIndex).Cells(enum_Operation_Columns.Tool).Value = dlg.Tool_Name

                    Operation_Grid_List.Set_Cell_Value(e.RowIndex, enum_Operation_Columns.Seat_Z, dlg.Seat_Z)
                    grd_Operations.Rows(e.RowIndex).Cells(enum_Operation_Columns.Seat_Z).Value = dlg.Seat_Z

                    Operation_Grid_List.Set_Cell_Value(e.RowIndex, enum_Operation_Columns.Measure_Z, dlg.Measure_Z)
                    grd_Operations.Rows(e.RowIndex).Cells(enum_Operation_Columns.Measure_Z).Value = dlg.Measure_Z

                    Operation_Grid_List.Set_Cell_Value(e.RowIndex, enum_Operation_Columns.Measure_X, dlg.Measure_X)
                    grd_Operations.Rows(e.RowIndex).Cells(enum_Operation_Columns.Measure_X).Value = dlg.Measure_X

                    Operation_Grid_List.Set_Cell_Value(e.RowIndex, enum_Operation_Columns.Measure_Y, dlg.Measure_Y)
                    grd_Operations.Rows(e.RowIndex).Cells(enum_Operation_Columns.Measure_Y).Value = dlg.Measure_Y

                    Move_To_Cell(grd_Operations, e.RowIndex, e.ColumnIndex - 1)
                    Move_To_Cell(grd_Operations, e.RowIndex, e.ColumnIndex)

                    Main_Form.ctl_C_Interface.Set_Parameter_Z_2("Go to Tool Seat", dlg.Seat_Z)
                    Main_Form.ctl_C_Interface.Set_Parameter_Position("Go to Tool Measure", dlg.Measure_X, dlg.Measure_Y, Display.Maximum_Z, dlg.Measure_Z)
                    Main_Form.ctl_C_Interface.Set_Parameter_Z_2("Go to Tool Measure", dlg.Measure_Z)

                    Operation_Changes_Made = True
                End If


            Case enum_Operation_Columns.Offset_Type
                Dim S As String = ""

                Select Case grd_Operations.Rows(e.RowIndex).Cells(e.ColumnIndex).Value
                    Case "Fixture"
                        S = "Current XY"
                    Case "Current XY"
                        S = "Fixture"
                    Case Else
                        S = "Fixture"
                End Select

                Operation_Grid_List.Set_Cell_Value(e.RowIndex, e.ColumnIndex, S)
                grd_Operations.Rows(e.RowIndex).Cells(e.ColumnIndex).Value = S
                Move_To_Cell(grd_Operations, e.RowIndex, e.ColumnIndex - 1)
                Move_To_Cell(grd_Operations, e.RowIndex, e.ColumnIndex)

            Case enum_Operation_Columns.Tool_Measurement_Type
                Dim S As String = ""

                Select Case grd_Operations.Rows(e.RowIndex).Cells(e.ColumnIndex).Value
                    Case "Fixed"
                        S = "Movable"
                    Case "Movable"
                        S = "Surface"
                    Case "Surface"
                        S = "Fixed"
                    Case Else
                        S = "Movable"
                End Select

                Operation_Grid_List.Set_Cell_Value(e.RowIndex, e.ColumnIndex, S)
                grd_Operations.Rows(e.RowIndex).Cells(e.ColumnIndex).Value = S
                Move_To_Cell(grd_Operations, e.RowIndex, e.ColumnIndex - 1)
                Move_To_Cell(grd_Operations, e.RowIndex, e.ColumnIndex)

            Case enum_Operation_Columns.Seat_Z, enum_Operation_Columns.Measure_Z
                Dim Num As Single = grd_Operations.Rows(e.RowIndex).Cells(e.ColumnIndex).Value
                Num = Get_User_Input(Num, 6, FormatNumber(KFlop.Z_Abs, 4))
                grd_Operations.Rows(e.RowIndex).Cells(e.ColumnIndex).Value = Num
                Move_To_Cell(grd_Operations, e.RowIndex, e.ColumnIndex - 1)
                Move_To_Cell(grd_Operations, e.RowIndex, e.ColumnIndex)

                Select Case e.ColumnIndex
                    Case enum_Operation_Columns.Seat_Z
                        Main_Form.ctl_C_Interface.Set_Parameter_Z_2("Go to Tool Seat", Num)
                    Case enum_Operation_Columns.Measure_Z
                        Main_Form.ctl_C_Interface.Set_Parameter_Z_2("Go to Tool Measure", Num)
                End Select

            Case enum_Operation_Columns.Measure_X
                Dim Num As Single
                Try
                    If grd_Operations.Rows(e.RowIndex).Cells(e.ColumnIndex).Value = "" Then
                        Num = 0
                    Else
                        Num = CSng(grd_Operations.Rows(e.RowIndex).Cells(e.ColumnIndex).Value)
                    End If
                Catch ex As Exception
                    Num = 0
                End Try
                Num = Get_User_Input(Num, 0, FormatNumber(KFlop.X_Abs, 4))
                grd_Operations.Rows(e.RowIndex).Cells(e.ColumnIndex).Value = Num
                Move_To_Cell(grd_Operations, e.RowIndex, e.ColumnIndex - 1)
                Move_To_Cell(grd_Operations, e.RowIndex, e.ColumnIndex)

                Main_Form.ctl_C_Interface.Set_Parameter_X_1("Go to Tool Measure", Num)

            Case enum_Operation_Columns.Measure_Y
                Dim Num As Single
                Try
                    Num = CSng(grd_Operations.Rows(e.RowIndex).Cells(e.ColumnIndex).Value)
                Catch ex As Exception
                    Num = 0
                End Try

                Num = Get_User_Input(Num, 0, FormatNumber(KFlop.Y_Abs, 4))
                grd_Operations.Rows(e.RowIndex).Cells(e.ColumnIndex).Value = Num
                Move_To_Cell(grd_Operations, e.RowIndex, e.ColumnIndex - 1)
                Move_To_Cell(grd_Operations, e.RowIndex, e.ColumnIndex)

                Main_Form.ctl_C_Interface.Set_Parameter_Y_1("Go to Tool Measure", Num)

        End Select

    End Sub

    Private Function Finish_Operations() As Boolean

        If Operation_Changes_Made Then
            Dim Count As Integer

            For I = 0 To grd_Operations.RowCount - 1
                Count = 0
                For J = 0 To grd_Operations.RowCount - 1
                    If grd_Operations.Rows(I).Cells(enum_Operation_Columns.Name).Value = grd_Operations.Rows(J).Cells(enum_Operation_Columns.Name).Value Then
                        Count += 1
                        If Count > 1 Then
                            Show_Error("Operation names must be unique." & vbCrLf & grd_Operations.Rows(I).Cells(enum_Operation_Columns.Name).Value)
                            Return False
                        End If
                    End If
                Next
            Next

            Operation_Grid_Header.Clear()
            Operation_Grid_Header.Add("Fixture~" & Fixture_Name)
            Save_Grid_List(Route_File, Operation_Grid_List, "Operation", Task_Grid_Lists, "Task", Operation_Grid_Header)
            Operation_Changes_Made = False
            Flash_Message("Operations Saved")

        End If

        Return True
    End Function

    Public Function Get_Toolsetter_Type() As String
        If IsNothing(grd_Operations.CurrentRow) Then Return ""
        If grd_Operations.Rows.Count = 0 Then Return ""
        Dim Current_Operation As Integer = grd_Operations.CurrentRow.Index
        Return grd_Operations.CurrentRow.Cells(enum_Operation_Columns.Tool_Measurement_Type).Value
    End Function

    Public Function Get_Tool_Name() As String
        If IsNothing(grd_Operations.CurrentRow) Then Return ""
        If grd_Operations.Rows.Count = 0 Then Return ""
        Return grd_Operations.CurrentRow.Cells(enum_Operation_Columns.Tool).Value
    End Function

    Public Function Get_Directions() As String
        If IsNothing(grd_Operations.CurrentRow) Then Return ""
        If grd_Operations.Rows.Count = 0 Then Return ""
        Return grd_Operations.CurrentRow.Cells(enum_Operation_Columns.Directions).Value
    End Function

    Public Sub Get_Goto_Positions(ByRef Tool_Seat_Z As Single, ByRef Tool_Measure_Z As Single, ByRef Tool_Measure_X As Single, ByRef Tool_Measure_Y As Single)
        Tool_Seat_Z = Get_Number(grd_Operations.CurrentRow.Cells(enum_Operation_Columns.Seat_Z).Value)
        Tool_Measure_Z = Get_Number(grd_Operations.CurrentRow.Cells(enum_Operation_Columns.Measure_Z).Value)
        Tool_Measure_X = Get_Number(grd_Operations.CurrentRow.Cells(enum_Operation_Columns.Measure_X).Value)
        Tool_Measure_Y = Get_Number(grd_Operations.CurrentRow.Cells(enum_Operation_Columns.Measure_Y).Value)
    End Sub


#End Region

#Region "Tasks Grid"

    Private Sub Get_Part_Offsets(ByRef Part_Offset_X As Single, ByRef Part_Offset_Y As Single, ByRef Part_Offset_Z As Single,
                                 ByRef Material_Thickness As Single, ByRef Material_Width As Single,
                                 ByRef Material_Length As Single, ByRef Material_origin As Single)
        Dim Current_Operation As Integer = grd_Operations.CurrentRow.Index
        Dim Current_Task = grd_Tasks.CurrentRow.Index
        Part_Offset_X = Get_Number(Task_Grid_Lists(Current_Operation).Get_Cell_Value(Current_Task, enum_Task_Columns.Part_Offset_X))
        Part_Offset_Y = Get_Number(Task_Grid_Lists(Current_Operation).Get_Cell_Value(Current_Task, enum_Task_Columns.Part_Offset_Y))
        Part_Offset_Z = Get_Number(Task_Grid_Lists(Current_Operation).Get_Cell_Value(Current_Task, enum_Task_Columns.Part_Offset_Z))

        Material_Thickness = Get_Number(Task_Grid_Lists(Current_Operation).Get_Cell_Value(Current_Task, enum_Task_Columns.Material_Thickness))
        Material_Width = Get_Number(Task_Grid_Lists(Current_Operation).Get_Cell_Value(Current_Task, enum_Task_Columns.Material_Width))
        Material_Length = Get_Number(Task_Grid_Lists(Current_Operation).Get_Cell_Value(Current_Task, enum_Task_Columns.Material_Length))
        Dim S As String = Task_Grid_Lists(Current_Operation).Get_Cell_Value(Current_Task, enum_Task_Columns.Material_Origin)

        Select Case S
            Case "Top Left"
                Material_origin = Chip_Drawing.Class_Canvas.enum_Material_Origin.Top_Left
            Case "Top Right"
                Material_origin = Chip_Drawing.Class_Canvas.enum_Material_Origin.Top_Right
            Case "Bottom Left"
                Material_origin = Chip_Drawing.Class_Canvas.enum_Material_Origin.Bottom_Left
            Case "Bottom Right"
                Material_origin = Chip_Drawing.Class_Canvas.enum_Material_Origin.Bottom_Right
        End Select

    End Sub

    Private Sub Load_GCode_and_Offsets(ByVal File_Name As String, ByVal Fixture As String, ByVal Mount As String, Offset_Type As String)

        If IsNothing(grd_Tasks.CurrentRow) Then Exit Sub

        Dim Mount_Offset_X As Single = 0
        Dim Mount_Offset_Y As Single = 0

        Dim Mount_Delta_Z As Single = 0
        Dim Part_Offset_X, Part_Offset_Y, Part_Offset_Z, Material_Thickness, Material_Width, Material_Length As Single
        Dim Material_Origin As Chip_Drawing.Class_Canvas.enum_Material_Origin = Chip_Drawing.Class_Canvas.enum_Material_Origin.Bottom_Left

        Dim Tool_Setter_Type As String

        Main_Form.ctl_Fixture_Offsets.Tool_Measured = False
        Main_Form.btn_Measure_Tool.State = "Not Measured"
        Fixture_Offset_Type = Offset_Type

        Dim Seat_Z As Single
        Dim Measure_Z As Single
        Dim Measure_X As Single
        Dim Measure_Y As Single

        Get_Goto_Positions(Seat_Z, Measure_Z, Measure_X, Measure_Y)
        Main_Form.ctl_C_Interface.Set_Parameter_Z_2("Go to Tool Seat", Seat_Z)
        Main_Form.ctl_C_Interface.Set_Parameter_Position("Go to Tool Measure", Measure_X, Measure_Y, Display.Maximum_Z, Measure_Z)
        Main_Form.ctl_C_Interface.Set_Parameter_Z_2("Go to Tool Measure", Measure_Z)

        If Offset_Type = "Current XY" Then
            Get_Part_Offsets(Part_Offset_X, Part_Offset_Y, Part_Offset_Z, Material_Thickness, Material_Width, Material_Length, Material_Origin)
            Z_Part_Offset = Part_Offset_Z

            Tool_Setter_Type = Get_Toolsetter_Type()

            If (Part_Offset_X <> 0) Or (Part_Offset_Y <> 0) Then
                Dim dlg As New dlg_Message_Box
                dlg.ShowDialog("Program X Offset = " & Part_Offset_X & vbCrLf &
                               "Program Y Offset = " & Part_Offset_Y & vbCrLf &
                               "Normally for an Operation offset type of 'Current XY', the above are set to zero." & vbCrLf &
                               "Continue anyway?", "Warning", MessageBoxButtons.YesNo)
                If dlg.DialogResult <> DialogResult.Yes Then
                    Exit Sub
                End If
            End If
            Main_Form.txt_Offset_Type.Text = "Current XY"

            Mount_Offset_X = 0
            Mount_Offset_Y = 0
            Part_Offset_X = Part_Offset_X + KFlop.X_Abs
            Part_Offset_Y = Part_Offset_Y + KFlop.Y_Abs
            Part_Offset_Z = Part_Offset_Z + KFlop.Z_Abs

            Fixture_Offsets_Control.Set_Build_Offsets(
                    Fixture, Mount, File_Name,
                    Mount_Offset_X, Part_Offset_X, Mount_Offset_Y, Part_Offset_Y,
                    Mount_Delta_Z, Part_Offset_Z, Material_Thickness, Tool_Setter_Type)

            Main_Form.ctl_Chip_Drawing.Part_Drawing.Set_Fixture_Part_Locations(
                    Mount_Offset_X, Mount_Offset_Y, Part_Offset_X, Part_Offset_Y,
                   Material_Thickness, Material_Width, Material_Length, Material_Origin)

            GCode_Execution_Time = Task_Grid_Lists(grd_Operations.CurrentRow.Index).Get_Cell_Value(grd_Tasks.CurrentRow.Index, enum_Task_Columns.Time)
            GCode_Execution_Seconds = Get_Seconds(GCode_Execution_Time)

            GCode_Control.Load_G_Code(File_Name)

            Main_Form.Tab_Main.SelectedIndex = 0
        Else
            If Get_Mount_Offsets(Fixture, Mount, Mount_Offset_X, Mount_Offset_Y, Mount_Delta_Z) Then
                Get_Part_Offsets(Part_Offset_X, Part_Offset_Y, Part_Offset_Z, Material_Thickness, Material_Width, Material_Length, Material_Origin)

                Z_Part_Offset = Part_Offset_Z

                Tool_Setter_Type = Get_Toolsetter_Type()

                Main_Form.txt_Offset_Type.Text = "Fixture"

                Fixture_Offsets_Control.Set_Build_Offsets(
                    Fixture, Mount, File_Name,
                    Mount_Offset_X, Part_Offset_X, Mount_Offset_Y, Part_Offset_Y,
                    Mount_Delta_Z, Part_Offset_Z, Material_Thickness, Tool_Setter_Type)
                Main_Form.ctl_Chip_Drawing.Part_Drawing.Set_Fixture_Part_Locations(
                    Mount_Offset_X, Mount_Offset_Y, Part_Offset_X, Part_Offset_Y,
                   Material_Thickness, Material_Width, Material_Length, Material_Origin)


                GCode_Execution_Time = Task_Grid_Lists(grd_Operations.CurrentRow.Index).Get_Cell_Value(grd_Tasks.CurrentRow.Index, enum_Task_Columns.Time)
                GCode_Execution_Seconds = Get_Seconds(GCode_Execution_Time)

                GCode_Control.Load_G_Code(File_Name)

                Main_Form.Tab_Main.SelectedIndex = 0
            Else
                Show_Error("Fixture or mount not found." & vbCrLf & vbCrLf &
                       "Fixture : " & Fixture & vbCrLf &
                       "Mount : " & Mount)
                Exit Sub
            End If

        End If

    End Sub

    Private Sub Check_For_Task_Edit()
        If IsNothing(grd_Tasks.CurrentRow) Then Exit Sub
        If IsNothing(grd_Tasks.CurrentCell) Then Exit Sub
        Dim Current_Row As Integer = grd_Tasks.CurrentRow.Index
        Dim Current_Cell As Integer = grd_Tasks.CurrentCell.ColumnIndex
        Dim Current_Operation As Integer = grd_Operations.CurrentRow.Index

        If Current_Cell > 0 Then
            If grd_Tasks.CurrentCell.FormattedValue <> Task_Grid_Lists(Current_Operation).Get_Cell_Value(Current_Row, Current_Cell) Then
                Task_Grid_Lists(Current_Operation).Set_Cell_Value(Current_Row, Current_Cell, grd_Tasks.CurrentCell.FormattedValue)
                Operation_Changes_Made = True
            End If
        End If

    End Sub

    Private Sub ts_Tasks_ItemClicked(sender As Object, e As ToolStripItemClickedEventArgs) Handles ts_Tasks.ItemClicked
        Dim Current_Operation As Integer = grd_Operations.CurrentRow.Index
        If Current_Operation < 0 Then Exit Sub
        Dim Current_Row As Integer
        grd_Tasks.EndEdit()

        Check_For_Task_Edit()

        If e.ClickedItem.Text = "Insert" Then 'Handle empty grid
            If grd_Tasks.Rows.Count = 0 Then
                Current_Row = -1
            Else
                Current_Row = grd_Tasks.CurrentRow.Index
            End If
        Else
            If IsNothing(grd_Tasks.CurrentRow) Then Exit Sub
            Current_Row = grd_Tasks.CurrentRow.Index
        End If

        Select Case e.ClickedItem.Text
            Case "Run GCode"
                If IsNothing(grd_Tasks.CurrentRow) Then Exit Sub

                Dim File_Path As String = grd_Tasks.CurrentRow.Cells(enum_Task_Columns.Folder).Value & "\" &
                                          grd_Tasks.CurrentRow.Cells(enum_Task_Columns.Program).Value

                If Not File.Exists(File_Path) Then
                    Show_Error("File does not exist : " & File_Path)
                    Exit Sub
                End If

                Dim Mount_Name As String = grd_Tasks.CurrentRow.Cells(enum_Task_Columns.Mount).Value
                Dim Offset_Type As String = grd_Operations.CurrentRow.Cells(enum_Operation_Columns.Offset_Type).Value
                Load_GCode_and_Offsets(File_Path, Fixture_Name, Mount_Name, Offset_Type)

            Case "Run All"
                Ignore_Cell_Validating = True
                For I = 0 To Task_Grid_Lists(Current_Operation).List_Rows_Count - 1
                    Task_Grid_Lists(Current_Operation).Set_Cell_Value(I, enum_Task_Columns.Enable, "Run")
                    grd_Tasks.Rows(I).Cells(enum_Task_Columns.Enable).Value = "Run"
                Next
                Ignore_Cell_Validating = False
                Operation_Changes_Made = True
            Case "Insert"
                Task_Grid_Lists(Current_Operation).Insert_Row(Current_Row + 1)
                Operation_Changes_Made = True
            Case "Delete"
                Task_Grid_Lists(Current_Operation).Delete_Row(Current_Row)
                Operation_Changes_Made = True
            Case "Copy"
                Task_Grid_Lists(Current_Operation).Copy_Row(Current_Row)
                Operation_Changes_Made = True
            Case "Move Up"
                Task_Grid_Lists(Current_Operation).Move_Row_Up(Current_Row, Operation_Changes_Made)
                Operation_Changes_Made = True
            Case "Move Down"
                Task_Grid_Lists(Current_Operation).Move_Row_Down(Current_Row, Operation_Changes_Made)
                Operation_Changes_Made = True
            Case "Set Offsets"
                Dim dlg As New dlg_Task_Parameters

                dlg.Offset_X = Task_Grid_Lists(Current_Operation).Get_Cell_Value(Current_Row, enum_Task_Columns.Part_Offset_X)
                dlg.Offset_Y = Task_Grid_Lists(Current_Operation).Get_Cell_Value(Current_Row, enum_Task_Columns.Part_Offset_Y)
                dlg.Offset_Z = Task_Grid_Lists(Current_Operation).Get_Cell_Value(Current_Row, enum_Task_Columns.Part_Offset_Z)
                dlg.Material_Thickness = Task_Grid_Lists(Current_Operation).Get_Cell_Value(Current_Row, enum_Task_Columns.Material_Thickness)
                dlg.Material_Length = Task_Grid_Lists(Current_Operation).Get_Cell_Value(Current_Row, enum_Task_Columns.Material_Length)
                dlg.Material_Width = Task_Grid_Lists(Current_Operation).Get_Cell_Value(Current_Row, enum_Task_Columns.Material_Width)
                dlg.ShowDialog()

                If dlg.DialogResult = DialogResult.OK Then
                    Dim First As Integer
                    Dim Last As Integer

                    If dlg.All_Operations Then
                        First = 0
                        Last = grd_Operations.Rows.Count - 1
                    Else
                        First = grd_Operations.CurrentRow.Index
                        Last = grd_Operations.CurrentRow.Index
                    End If

                    For Op = First To Last
                        For I = 0 To Task_Grid_Lists(Op).List_Rows_Count - 1
                            Task_Grid_Lists(Op).Set_Cell_Value(I, enum_Task_Columns.Part_Offset_X, FormatNumber(dlg.Offset_X, 3))
                            Task_Grid_Lists(Op).Set_Cell_Value(I, enum_Task_Columns.Part_Offset_Y, FormatNumber(dlg.Offset_Y, 3))
                            Task_Grid_Lists(Op).Set_Cell_Value(I, enum_Task_Columns.Part_Offset_Z, FormatNumber(dlg.Offset_Z, 3))
                            Task_Grid_Lists(Op).Set_Cell_Value(I, enum_Task_Columns.Material_Thickness, FormatNumber(dlg.Material_Thickness, 3))
                            Task_Grid_Lists(Op).Set_Cell_Value(I, enum_Task_Columns.Material_Length, FormatNumber(dlg.Material_Length, 3))
                            Task_Grid_Lists(Op).Set_Cell_Value(I, enum_Task_Columns.Material_Width, FormatNumber(dlg.Material_Width, 3))
                            Task_Grid_Lists(grd_Operations.CurrentRow.Index).Load_Grid()
                        Next
                    Next

                    Operation_Changes_Made = True
                End If

        End Select

    End Sub

    Private Sub grd_Tasks_CellValidating(sender As Object, e As System.Windows.Forms.DataGridViewCellValidatingEventArgs) Handles grd_Tasks.CellValidating
        If Ignore_Cell_Validating Then Exit Sub
        If IsNothing(grd_Operations.CurrentRow) Then Exit Sub
        Dim Current_Operation As Integer = grd_Operations.CurrentRow.Index
        If Current_Operation < 0 Then Exit Sub

        If e.FormattedValue <> Task_Grid_Lists(Current_Operation).Get_Cell_Value(e.RowIndex, e.ColumnIndex) Then
            Task_Grid_Lists(Current_Operation).Set_Cell_Value(e.RowIndex, e.ColumnIndex, e.FormattedValue)
            Operation_Changes_Made = True
        End If
    End Sub

    Private Sub grd_Tasks_CellClick(sender As Object, e As System.Windows.Forms.DataGridViewCellEventArgs) Handles grd_Tasks.CellClick
        If e.RowIndex < 0 Then Exit Sub
        Dim Current_Operation As Integer = grd_Operations.CurrentRow.Index
        If Current_Operation < 0 Then Exit Sub
        Dim S As String = ""

        Select Case e.ColumnIndex
            Case < 0
                Dim Full_Path As String = grd_Tasks.Rows(e.RowIndex).Cells(enum_Task_Columns.Folder).Value & "\" &
                                          grd_Tasks.Rows(e.RowIndex).Cells(enum_Task_Columns.Program).Value

                If Not File.Exists(Full_Path) Then
                    Show_Error("File does not exist : " & vbCrLf & Full_Path)
                    Exit Sub
                End If

                Full_Path = Full_Path.Replace("\\", "~~")

                Dim P() As String = Full_Path.Split("\")
                Dim L As New List(Of String)
                For I = 0 To P.Count - 2
                    If I = 0 Then
                        L.Add(P(0))
                    Else
                        L.Add(L(I - 1) & "\" & P(I))
                    End If
                Next

                Expand_Tree(L)

                Dim Nodes() As TreeNode = tree_Files.Nodes.Find(grd_Tasks.Rows(e.RowIndex).Cells(enum_Task_Columns.Folder).Value, True)
                If Nodes.Count < 1 Then Exit Sub

                For I = 0 To lst_Files.Items.Count - 1
                    lst_Files.Items(I).Checked = False
                    lst_Files.Items(I).BackColor = Color.White
                Next

                Nodes(0).Checked = True
                Nodes(0).BackColor = Color.DodgerBlue

                For I = 0 To lst_Files.Items.Count - 1
                    If lst_Files.Items(I).Text = grd_Tasks.Rows(e.RowIndex).Cells(enum_Task_Columns.Program).Value Then
                        lst_Files.Focus()
                        lst_Files.Items(I).Selected = True
                        Exit For
                    End If
                Next

            Case enum_Task_Columns.Mount
                Dim Mounts As New List(Of String)

                For I = 0 To Fixture_Grid_List.List_Rows_Count - 1
                    Dim st As String = Fixture_Grid_List.Get_Cell_Value(I, 0)
                    If Fixture_Grid_List.Get_Cell_Value(I, 0) = Fixture_Name Then
                        For M = 0 To Mount_Grid_Lists(I).List_Rows_Count - 1
                            Mounts.Add(Mount_Grid_Lists(I).Get_Cell_Value(M, 0))
                        Next
                        Dim dlg As New dlg_Mounts
                        dlg.Fixture = Fixture_Name
                        dlg.Mounts = Mounts
                        dlg.ShowDialog()
                        If dlg.DialogResult = DialogResult.OK Then
                            Ignore_Cell_Validating = True
                            Dim X, Y, Z_Delta As Single
                            Get_Mount_Offsets(Fixture_Name, dlg.Mount, X, Y, Z_Delta)
                            grd_Tasks.Rows(e.RowIndex).Cells(enum_Task_Columns.Mount).Value = dlg.Mount
                            Task_Grid_Lists(Current_Operation).Set_Cell_Value(e.RowIndex, enum_Task_Columns.Mount, dlg.Mount)

                            Move_To_Cell(grd_Tasks, e.RowIndex, enum_Task_Columns.Enable)
                            Ignore_Cell_Validating = False
                            Operation_Changes_Made = True
                        End If
                        Exit Sub
                    End If
                Next
                Show_Error("Fixture not found : " & Fixture_Name)

            Case enum_Task_Columns.Enable
                Select Case grd_Tasks.Rows(e.RowIndex).Cells(e.ColumnIndex).Value
                    Case "Run"
                        S = "Auto"
                    Case "Aut0"
                        S = ""
                    Case ""
                        S = "Run"
                    Case Else
                        S = ""
                End Select

                Ignore_Cell_Validating = True
                Task_Grid_Lists(Current_Operation).Set_Cell_Value(e.RowIndex, e.ColumnIndex, S)
                grd_Tasks.Rows(e.RowIndex).Cells(e.ColumnIndex).Value = S
                Move_To_Cell(grd_Tasks, e.RowIndex, e.ColumnIndex + 1)
                Move_To_Cell(grd_Tasks, e.RowIndex, e.ColumnIndex)
                Ignore_Cell_Validating = False
                Operation_Changes_Made = True

            Case enum_Task_Columns.Program
                Dim File_Path As String = grd_Tasks.CurrentRow.Cells(enum_Task_Columns.Folder).Value & "\" &
                                          grd_Tasks.CurrentRow.Cells(enum_Task_Columns.Program).Value
                Dim Df As New Class_Data_File
                Dim File_Name As String = Path.GetFileName(File_Path)
                Dim Folder As String = Path.GetDirectoryName(File_Path)

                File_Path = Df.Select_Input_File(File_Path, "Gcode Files (*.chp)|*.chp|Route Files (*.chp)|*.chp|All files (*.*)|*.*")
                If File_Path = "" Then Exit Sub

                Ignore_Cell_Validating = True

                File_Name = Path.GetFileName(File_Path)
                Folder = Path.GetDirectoryName(File_Path)

                Task_Grid_Lists(Current_Operation).Set_Cell_Value(grd_Tasks.CurrentRow.Index, enum_Task_Columns.Program, File_Name)
                grd_Tasks.CurrentRow.Cells(enum_Task_Columns.Program).Value = Path.GetFileName(File_Path)

                Task_Grid_Lists(Current_Operation).Set_Cell_Value(grd_Tasks.CurrentRow.Index, enum_Task_Columns.Folder, Folder)
                grd_Tasks.CurrentRow.Cells(enum_Task_Columns.Folder).Value = Path.GetDirectoryName(File_Path)

                Ignore_Cell_Validating = False

                Move_To_Cell(grd_Tasks, e.RowIndex, e.ColumnIndex + 1)
                Move_To_Cell(grd_Tasks, e.RowIndex, e.ColumnIndex)
                Operation_Changes_Made = True

            Case enum_Task_Columns.Material_Origin
                Dim dlg As New dlg_Material_Origin

                dlg.Material_Origin = grd_Tasks.Rows(e.RowIndex).Cells(enum_Task_Columns.Material_Origin).Value
                dlg.ShowDialog()
                grd_Tasks.Rows(e.RowIndex).Cells(enum_Task_Columns.Material_Origin).Value = dlg.Material_Origin

                Move_To_Cell(grd_Tasks, e.RowIndex, e.ColumnIndex - 1)
                Move_To_Cell(grd_Tasks, e.RowIndex, e.ColumnIndex)
                Ignore_Cell_Validating = False
                Operation_Changes_Made = True

        End Select


    End Sub

    Public Sub Set_Execution_Time(Elap As TimeSpan)
        Dim ts As TimeSpan = Elap
        Dim T_Elapsed As String = ts.ToString("hh\:mm\:ss")
        If IsNothing(grd_Operations.CurrentRow) Then Exit Sub
        If IsNothing(grd_Operations.CurrentCell) Then Exit Sub
        grd_Tasks.CurrentRow.Cells(enum_Task_Columns.Time).Value = T_Elapsed
        Task_Grid_Lists(grd_Operations.CurrentRow.Index).Set_Cell_Value(grd_Tasks.CurrentRow.Index, enum_Task_Columns.Time, T_Elapsed)
        grd_Tasks.CurrentRow.Cells(enum_Task_Columns.Time).Value = T_Elapsed

        Operation_Changes_Made = True
    End Sub

    Public Sub Get_Execution_Time(ByRef Execution_Time As String, ByRef Execution_Seconds As String)
        If IsNothing(grd_Operations.CurrentRow) Then Exit Sub
        If IsNothing(grd_Operations.CurrentCell) Then Exit Sub
        GCode_Execution_Time = Task_Grid_Lists(grd_Operations.CurrentRow.Index).Get_Cell_Value(grd_Tasks.CurrentRow.Index, enum_Task_Columns.Time)
        GCode_Execution_Seconds = Get_Seconds(GCode_Execution_Time)
        Execution_Time = GCode_Execution_Time
        Execution_Seconds = GCode_Execution_Seconds
    End Sub

#End Region


#Region "Tree Operations"

    Private Sub Fill_Tree()
        tree_Files.Nodes.Clear()

        If Not Directory.Exists(Chip_Build_Root) Then
            Show_Error("Directory not found : " & Chip_Build_Root)
            Exit Sub
        End If
        Directory_Info = New DirectoryInfo(Chip_Build_Root)

        If Directory_Info.Exists Then
            Root_Node = New TreeNode(Directory_Info.Name, enum_Image_Types.Directory, 0)
            Root_Node.Name = Directory_Info.FullName
            Root_Node.Tag = Directory_Info
            GetDirectories(Directory_Info.GetDirectories(), Root_Node)
            tree_Files.Nodes.Add(Root_Node)
        Else
            Show_Error("File Tree Error" & vbCrLf &
                       "Directory Not found : " & Chip_Build_Root)
        End If

    End Sub

    Private Sub Find_Expanded_Nodes(Target_Node As TreeNode)
        For I = 0 To Target_Node.Nodes.Count - 1
            If Target_Node.Nodes(I).IsExpanded Then
                Expanded_Nodes.Add(Target_Node.Nodes(I).Name)
                Find_Expanded_Nodes(Target_Node.Nodes(I))
            End If
        Next
    End Sub

    Private Sub GetDirectories(ByVal subDirs() As DirectoryInfo, ByVal nodeToAddTo As TreeNode)

        Dim aNode As TreeNode
        Dim subSubDirs() As DirectoryInfo
        Dim subDir As DirectoryInfo
        For Each subDir In subDirs
            aNode = New TreeNode(subDir.Name, 0, 0)
            aNode.Name = subDir.FullName
            aNode.Tag = subDir
            aNode.ImageKey = "folder"
            subSubDirs = subDir.GetDirectories()
            If subSubDirs.Length <> 0 Then
                GetDirectories(subSubDirs, aNode)
            End If
            nodeToAddTo.Nodes.Add(aNode)
        Next subDir

    End Sub

#End Region

#Region "Tree Events"

    Private Sub tree_Files_AfterCollapse(sender As Object, e As System.Windows.Forms.TreeViewEventArgs) Handles tree_Files.AfterCollapse
        If e.Node.Index = 0 Then
            Fill_Tree()
        End If
    End Sub

    Private Sub tree_Files_NodeMouseClick(ByVal sender As Object, ByVal e As TreeNodeMouseClickEventArgs) Handles tree_Files.NodeMouseClick
        Fill_Tree_Files_List(e.Node)
    End Sub

    Private Sub Fill_Tree_Files_List(Selected_Node As TreeNode)
        lst_Files.Items.Clear()
        Dim nodeDirInfo As DirectoryInfo =
        CType(Selected_Node.Tag, DirectoryInfo)
        Dim subItems() As ListViewItem.ListViewSubItem
        Dim item As ListViewItem = Nothing
        Current_Directory = nodeDirInfo.FullName

        Dim dir As DirectoryInfo
        For Each dir In nodeDirInfo.GetDirectories()
            item = New ListViewItem(dir.Name, 0)
            item.Tag = dir.FullName
            subItems = New ListViewItem.ListViewSubItem() _
                {New ListViewItem.ListViewSubItem(item, "Directory"),
                New ListViewItem.ListViewSubItem(item,
                dir.LastAccessTime.ToShortDateString())}
            item.SubItems.AddRange(subItems)
            lst_Files.Items.Add(item)
        Next dir

        Dim file As FileInfo
        Dim Ext As String
        Dim Idx As Integer
        Dim File_Type As String

        For Each file In nodeDirInfo.GetFiles()
            Ext = Path.GetExtension(file.Name)
            Idx = 1
            File_Type = "File"

            Select Case UCase(Ext)
                Case ".TCW"
                    Idx = enum_Image_Types.CAD_Drawing
                    File_Type = "CAD"
                Case ".DXF"
                    Idx = enum_Image_Types.DXF_Drawing
                    File_Type = "DXF"
                Case ".CRV"
                    Idx = enum_Image_Types.CAM_Session
                    File_Type = "CAM"
                Case ".CHP"
                    File_Type = "G Code"
                    Idx = enum_Image_Types.G_Code
                Case ".RTE"
                    File_Type = "Route"
                    Idx = enum_Image_Types.Route
            End Select

            item = New ListViewItem(file.Name & "  ", Idx)
            subItems = New ListViewItem.ListViewSubItem() _
                {New ListViewItem.ListViewSubItem(item, File_Type),
                New ListViewItem.ListViewSubItem(item, file.LastAccessTime.ToShortDateString()),
                New ListViewItem.ListViewSubItem(item, Current_Directory)}

            item.SubItems.AddRange(subItems)
            lst_Files.Items.Add(item)
        Next file

        lst_Files.AutoResizeColumns(ColumnHeaderAutoResizeStyle.HeaderSize)
    End Sub

    Private Sub lst_Tree_Mouse_Double_Click(sender As Object, e As System.Windows.Forms.MouseEventArgs) Handles lst_Files.MouseDoubleClick
        Dim hti As ListViewHitTestInfo = lst_Files.HitTest(e.X, e.Y)
        Dim It As System.Windows.Forms.ListViewItem
        If lst_Files.SelectedItems.Count > 0 Then
            It = lst_Files.SelectedItems(0)
            If It.SubItems(1).Text = "Directory" Then Exit Sub

            If hti.Item.Text.Contains(".rte") Then
                Load_Route(Current_Directory & "\" & hti.Item.Text)
            Else
                Open_File(hti.Item.Text)
            End If
        Else
            Exit Sub
        End If

    End Sub


#End Region

#Region "Utility Routines"

    Private Sub Open_File(File_Name As String)
        Dim PDF_Proc = New System.Diagnostics.Process
        PDF_Proc.StartInfo.FileName = Current_Directory & "\" & File_Name
        PDF_Proc.StartInfo.Verb = "Open"
        PDF_Proc.Start()
    End Sub

    Private Sub Move_To_Row(grid As DataGridView, Row As Integer)
        If Row < 0 Then Exit Sub
        If Row > grd_Tools.RowCount - 1 Then Exit Sub
        Move_To_Cell(grid, Row, 1)
    End Sub

    Public Sub Move_To_Cell(grid As DataGridView, Row As Integer, Cell As Integer)
        If grid.RowCount = 0 Then Exit Sub
        If Cell < 0 Then Exit Sub
        If grid.RowCount = 1 Then Row = 0
        If Row >= grid.RowCount Then Row = grid.RowCount - 1
        grid.CurrentCell = grid.Rows(Row).Cells(Cell)
    End Sub


#End Region

End Class
