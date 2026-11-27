Imports System.IO

Public Class Chip_C_Interface

#Region "Enums"

    Public Enum enum_Commands
        '***********************************************************
        'Note that the C Program must also define these the same way
        'so both the C program and PC are in sync with the commands
        'of the user data.
        '***********************************************************
        Idle = 0
        Set_Spindle_RPM = 1
        Go_To = 2
        Home_X = 3
        Home_Y = 4
        Home_Z = 5
        Home_All = 6
        Probe_X_Plus = 7
        Probe_X_Minus = 8
        Probe_Y_Plus = 9
        Probe_Y_Minus = 10
        Probe_Z_Minus = 11
        Tool_Setter_Fixed = 12
        Tool_Setter_Movable = 13
        Disable_Limit_Switches = 14
        Enable_Limit_Switches = 15
        Disable_Soft_Limits = 16
        Enable_Soft_Limits = 17
        Move_Y_Master = 18
        Test = 19
        Last = 20
    End Enum

    Public Enum enum_Status
        Idle = 0

        Move_Status = 10

        Home_X_Status = 20
        Home_Y_Status = 30
        Home_Z_Status = 40

        Probe_X_Plus_Status = 50
        Probe_X_Minus_Status = 60
        Probe_Y_Plus_Status = 70
        Probe_Y_Minus_Status = 80
        Probe_Z_Minus_Status = 90

        Tool_Setter_Fixed_Status = 100
        Tool_Setter_Movable_Status = 110

        Spindle_Status = 120

    End Enum

    Public Enum enum_Moving_To_Position_Status_Offsets
        Moving_To_Position_1 = 1
        Moving_To_Position_2 = 2
        Moving_To_Position_3 = 3
        Moving_To_Position_4 = 4
        Move_Done = 5
        Move_Error = 6
    End Enum

    Public Enum enum_Moving_To_Switch_Status_Offsets
        Moving_To_Switch = 1
        Moving_To_Latch = 2
        Moving_Off_Switch = 3
        Moving_To_Backoff = 4
        Homed = 5
    End Enum

    Public Enum enum_Probe_Status_Offsets
        Probe_Extend = 1
        Probe_Retract = 2
        Probe_Touch = 3
        Probe_Release = 4
        Probe_Done = 5
        Probe_Overtravel = 6
        Probe_In_Contact = 7
    End Enum

    Public Enum enum_Spindle_Status_Offsets
        RPM_Change = 1
        RPM_Too_High = 2
    End Enum

    Public Enum enum_User_Data_Address
        '***********************************************************
        'Note that the C Program must also define these the same way
        'so both the C program and PC are in sync with the addresses
        'of the user data.
        '***********************************************************

        'Integers
        Command = 0
        Status = 1
        Status_Pending_Flag = 2 'Used to sync multiple steps. Kflop sets Flag, PC clears it
        Saftey_Error_Code = 3

        'Single
        Spindle_Speed = 5      'Used to set spindle speed from PC application
        Spindle_Override = 6   'Used to set spindle speed override from PC application

        'Double
        X_Params = 10   'Blocks of data for an axis see enum_User_Data_Offsets below
        Y_Params = 15
        Z_Params = 20


        Pos_1 = 30      'Position data sent by KFlop to Chip app, for probe and toolsetter touch positions
        Pos_2 = 31
        Spindle_RPM = 32  'Used by KFlop to show actual spindle RPM after applying override

        'Soft limits
        X_Plus_Soft_Limit = 40
        X_Minus_Soft_Limit = 41
        Y_Plus_Soft_Limit = 42
        Y_Minus_Soft_Limit = 43
        Z_Plus_Soft_Limit = 44
        Z_Minus_Soft_Limit = 45

    End Enum

    Public Enum enum_User_Data_Offsets
        Dest_1 = 0
        Dest_2 = 1
        Feed_1 = 2
        Feed_2 = 3
    End Enum

    Public Enum enum_grid_Parameter_Columns
        Order
        Set_Parameters
        Name
        Command
        Command_Value
        X_1
        X_2
        Y_1
        Y_2
        Z_1
        Z_2
        Feed_X_1
        Feed_X_2
        Feed_Y_1
        Feed_Y_2
        Feed_Z_1
        Feed_Z_2
    End Enum

#End Region

#Region "Declarations"

    Public C_Program_Parameters_Request As Integer = -1
    Public Run_C_Program_Request As Integer = -1

#End Region

#Region "Properties"

    Private _Handler As String
    Public Property Handler As String
        Get
            Return _Handler
        End Get
        Set(ByVal value As String)
            _Handler = value
        End Set
    End Property

#End Region

#Region "Methods"

    Public Sub Initialize()

        grid_Theads.Rows.Clear()
        For I = 0 To 6
            grid_Theads.Rows.Add()
            grid_Theads.Rows(I).Cells(1).Value = I + 1
            grid_Theads.Rows(I).Cells(0).Value = "Run"
        Next

        grid_User_Data.Rows.Clear()
        Add_User_Data_Row("Command", enum_User_Data_Address.Command)
        Add_User_Data_Row("Status", enum_User_Data_Address.Status)
        Add_User_Data_Row("Status Pending", enum_User_Data_Address.Status_Pending_Flag)
        Add_User_Data_Row("Saftey Error Code", enum_User_Data_Address.Saftey_Error_Code)

        Add_User_Data_Row("Spindle Speed", enum_User_Data_Address.Spindle_Speed)

        Add_User_Data_Row("X Destination 1", enum_User_Data_Address.X_Params + enum_User_Data_Offsets.Dest_1)
        Add_User_Data_Row("X Destination 2", enum_User_Data_Address.X_Params + enum_User_Data_Offsets.Dest_2)
        Add_User_Data_Row("X Feedrate 1", enum_User_Data_Address.X_Params + enum_User_Data_Offsets.Feed_1)
        Add_User_Data_Row("X Feedrate 2", enum_User_Data_Address.X_Params + enum_User_Data_Offsets.Feed_2)

        Add_User_Data_Row("Y Destination 1 ", enum_User_Data_Address.Y_Params + enum_User_Data_Offsets.Dest_1)
        Add_User_Data_Row("Y Desinationt 2 ", enum_User_Data_Address.Y_Params + enum_User_Data_Offsets.Dest_2)
        Add_User_Data_Row("Y Feedrate 1", enum_User_Data_Address.Y_Params + enum_User_Data_Offsets.Feed_1)
        Add_User_Data_Row("Y Feedrate 2", enum_User_Data_Address.Y_Params + enum_User_Data_Offsets.Feed_2)

        Add_User_Data_Row("Z Desinationt 1 ", enum_User_Data_Address.Z_Params + enum_User_Data_Offsets.Dest_1)
        Add_User_Data_Row("Z Desinationt 2 ", enum_User_Data_Address.Z_Params + enum_User_Data_Offsets.Dest_2)
        Add_User_Data_Row("Z Feedrate 1", enum_User_Data_Address.Z_Params + enum_User_Data_Offsets.Feed_1)
        Add_User_Data_Row("Z Feedrate 2", enum_User_Data_Address.Z_Params + enum_User_Data_Offsets.Feed_2)

        Add_User_Data_Row("Position 1", enum_User_Data_Address.Pos_1)
        Add_User_Data_Row("Position 2", enum_User_Data_Address.Pos_2)

        If Not SIMULATE Then
            Get_All_User_Data()
        End If

        Fire_Event(Class_Display_Handlers.enum_Event_Type.Initialize)

    End Sub

    Public Sub Get_Persistant_Data(ByRef Data As List(Of String))
        Dim Line As String
        Data.Clear()
        For R = 0 To grid_Parameters.Rows.Count - 1
            Line = ""
            For C = 0 To grid_Parameters.Columns.Count - 1
                Line &= grid_Parameters.Rows(R).Cells(C).Value
                If C < grid_Parameters.Columns.Count - 1 Then
                    Line &= "~"
                End If
            Next
            Data.Add(Line)
        Next
    End Sub

    Public Sub Set_Persistant_Data(ByRef Data As List(Of String))
        Dim P() As String
        grid_Parameters.Rows.Clear()
        For I = 0 To Data.Count - 1
            P = Data(I).Split("~")
            P(1) = "Set"
            grid_Parameters.Rows.Add(P)
        Next
        Reorder()
    End Sub

    Public Sub Set_Feed_Rate(Name As String, Axis As String, Feed_Rate As Double, Optional Feed_Rate_2 As Double = Double.MinValue)

        For I = 0 To grid_Parameters.Rows.Count - 1
            If grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Name).Value = Name Then
                Move_To_Row(I, False)
                Select Case Axis
                    Case "All"
                        grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Feed_X_1).Value = Feed_Rate
                        grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Feed_Y_1).Value = Feed_Rate
                        grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Feed_Z_1).Value = Feed_Rate
                        If Feed_Rate_2 > Double.MinValue Then
                            grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Feed_X_2).Value = Feed_Rate_2
                            grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Feed_Y_2).Value = Feed_Rate_2
                            grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Feed_Z_2).Value = Feed_Rate_2
                        End If
                    Case "X"
                        grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Feed_X_1).Value = Feed_Rate
                        If Feed_Rate_2 > Double.MinValue Then grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Feed_X_2).Value = Feed_Rate_2
                    Case "Y"
                        grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Feed_Y_1).Value = Feed_Rate
                        If Feed_Rate_2 > Double.MinValue Then grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Feed_Y_2).Value = Feed_Rate_2
                    Case "Z"
                        grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Feed_Z_1).Value = Feed_Rate
                        If Feed_Rate_2 > Double.MinValue Then grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Feed_Z_2).Value = Feed_Rate_2
                End Select
            End If
        Next

    End Sub

    Public Sub Set_Parameter_Position(Name As String, X As Double, Y As Double, Z_1 As Double, Z_2 As Double)
        For I = 0 To grid_Parameters.Rows.Count - 1
            If grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Name).Value = Name Then
                Move_To_Row(I, False)
                grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.X_1).Value = Math.Round(X, 4)
                grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Y_1).Value = Math.Round(Y, 4)
                grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Z_1).Value = Math.Round(Z_1, 4)
                grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Z_2).Value = Math.Round(Z_2, 4)
            End If
        Next
    End Sub



    Public Sub Set_Parameter_Z_1(Name As String, Z As Double)
        For I = 0 To grid_Parameters.Rows.Count - 1
            If grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Name).Value = Name Then
                Move_To_Row(I, False)
                grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Z_1).Value = Math.Round(Z, 4)
            End If
        Next
    End Sub

    Public Sub Set_Parameter_X_1(Name As String, X As Double)
        For I = 0 To grid_Parameters.Rows.Count - 1
            If grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Name).Value = Name Then
                Move_To_Row(I, False)
                grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.X_1).Value = Math.Round(X, 4)
            End If
        Next
    End Sub


    Public Sub Set_Parameter_Y_1(Name As String, Y As Double)
        For I = 0 To grid_Parameters.Rows.Count - 1
            If grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Name).Value = Name Then
                Move_To_Row(I, False)
                grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Y_1).Value = Math.Round(Y, 4)
            End If
        Next
    End Sub

    Public Sub Set_Parameter_Z_2(Name As String, Z As Double)
        For I = 0 To grid_Parameters.Rows.Count - 1
            If grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Name).Value = Name Then
                Move_To_Row(I, False)
                grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Z_2).Value = Math.Round(Z, 4)
            End If
        Next
    End Sub

    Public Sub Set_Parameter_X_2(Name As String, X As Double)
        For I = 0 To grid_Parameters.Rows.Count - 1
            If grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Name).Value = Name Then
                Move_To_Row(I, False)
                grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.X_2).Value = Math.Round(X, 4)
            End If
        Next
    End Sub


    Public Sub Set_Parameter_Y_2(Name As String, Y As Double)
        For I = 0 To grid_Parameters.Rows.Count - 1
            If grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Name).Value = Name Then
                Move_To_Row(I, False)
                grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Y_2).Value = Math.Round(Y, 4)
            End If
        Next
    End Sub

    Public Sub Fire_Event(ByVal Evnt As Class_Display_Handlers.enum_Event_Type)
        Display.Run_Handler(Me, Evnt, True)
    End Sub

    Public Sub Load_Parameters_and_Run_C_Program(Parameter_Name As String, Thread As Integer, Optional Command As enum_Commands = enum_Commands.Idle)
        For I = 0 To grid_Parameters.Rows.Count - 1
            If grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Name).Value = Parameter_Name Then
                Move_To_Row(I, False)
                C_Program_Parameters_Request = I
                Update_Parameter_Changes()
                Run_C_Program(Thread, Command)
                Exit Sub
            End If
        Next
    End Sub

    Public Sub Load_Parameters(Name As String)
        For I = 0 To grid_Parameters.Rows.Count - 1
            If grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Name).Value = Name Then
                Move_To_Row(I, False)
                C_Program_Parameters_Request = I
            End If
        Next
    End Sub

    Public Sub Load_C_Program(File_Name As String, Thread As Integer)
        If Thread < 1 Or Thread > 7 Then
            Show_Error("Load_C_Program, Illegal thread : " & Thread)
            Exit Sub
        End If
        grid_Theads.Rows(Thread - 1).Cells(2).Value = File_Name
        KFlop.Load_C_Program(KFlop.C_Program_Folder & File_Name, Thread)
    End Sub

    Public Sub Run_C_Program(Thread As Integer, Optional Command As enum_Commands = enum_Commands.Idle)
        If Command > enum_Commands.Idle Then KFlop.Set_User_Data_Integer(enum_User_Data_Address.Command, Command)
        KFlop.Run_C_Program(Thread)
        If grid_Theads.RowCount > 0 Then
            grid_Theads.Rows(Thread - 1).Cells(0).Value = "Kill"
        End If
    End Sub

    Public Sub Kill_C_Program(Thread As Integer)
        KFlop.Kill_C_Program(Thread)
        If grid_Theads.RowCount > 0 Then
            grid_Theads.Rows(Thread - 1).Cells(0).Value = "Run"
        End If
    End Sub


    Public Function C_Program_Running(Thread As Integer) As Boolean
        Return KFlop.Controller.ThreadExecuting(Thread)
    End Function

    Public Sub Set_Spindle_Speed(Spindle_RPM As Single)
        KFlop.Set_User_Data_Integer(enum_User_Data_Address.Command, enum_Commands.Set_Spindle_RPM)
        KFlop.Set_User_Data_Float(enum_User_Data_Address.Spindle_Speed, CSng(Spindle_RPM))
        Get_All_User_Data()
        Run_C_Program(Class_KFLOP.enum_KFlop_Threads.Spindle_Control)
    End Sub

    Public Sub Set_Spindle_Override(Overide_Percent As Single)
        KFlop.Set_User_Data_Integer(enum_User_Data_Address.Command, enum_Commands.Set_Spindle_RPM)
        KFlop.Set_User_Data_Float(enum_User_Data_Address.Spindle_Override, CSng(Overide_Percent))
        Get_All_User_Data()
        Run_C_Program(Class_KFLOP.enum_KFlop_Threads.Spindle_Control)
    End Sub

#End Region

#Region "grid_Threads"

    Private Sub grid_Theads_Cell_Click(sender As Object, e As System.Windows.Forms.DataGridViewCellEventArgs) Handles grid_Theads.CellClick
        If e.RowIndex < 0 Then Exit Sub

        Select Case e.ColumnIndex
            Case 0
                If grid_Theads.Rows(e.RowIndex).Cells(2).Value <> "" Then
                    If grid_Theads.Rows(e.RowIndex).Cells(0).Value = "Kill" Then
                        KFlop.Kill_C_Program(e.RowIndex + 1)
                        KFlop.Enable_Soft_Limits() 'Resets soft limits
                    Else
                        grid_Theads.Rows(e.RowIndex).Cells(0).Value = "Kill"
                        KFlop.Run_C_Program(KFlop.C_Program_Folder & grid_Theads.Rows(e.RowIndex).Cells(2).Value, e.RowIndex + 1)
                        KFlop.Enable_Soft_Limits() 'Resets soft limits
                    End If
                End If

            Case 2
                Dim dlg As New OpenFileDialog
                Dim Result As DialogResult
                dlg.InitialDirectory = KFlop.C_Program_Folder
                dlg.FileName = grid_Theads.Rows(e.RowIndex).Cells(2).Value
                dlg.Filter = "C Files (*.c)|*.c|All files (*.*)|*.*"

                Result = dlg.ShowDialog()
                If Result = Windows.Forms.DialogResult.OK Then
                    grid_Theads.Rows(e.RowIndex).Cells(2).Value = Path.GetFileName(dlg.FileName)
                    Load_C_Program(Path.GetFileName(dlg.FileName), e.RowIndex + 1)
                End If

            Case Else
                Exit Sub
        End Select

    End Sub

    Public Sub Update_Status()
        Static Dim Active_Thread As Integer = 0

        If C_Program_Parameters_Request >= 0 Then Update_Parameter_Changes()

        For I = 0 To grid_Theads.Rows.Count - 1

            If KFlop.Controller.ThreadExecuting(I + 1) Then
                grid_Theads.Rows(I).Cells(0).Value = "Kill"
            Else
                grid_Theads.Rows(I).Cells(0).Value = "Run"
            End If

            '            If grid_Theads.Rows(I).Cells(0).Value = "Kill" Then
            '            If Not KFlop.Controller.ThreadExecuting(I + 1) Then
            '            grid_Theads.Rows(I).Cells(0).Value = "Run"
            '            End If
            '            End If

            If KFlop.Controller.ThreadExecuting(I + 1) Then
                Active_Thread = 2
            End If
        Next

        If Active_Thread > 0 Then
            Get_All_User_Data()
            Active_Thread -= 1
        End If

    End Sub

#End Region

#Region "grid_User_Data"

    Public Sub Get_All_User_Data()
        Dim Address As enum_User_Data_Address
        Dim N As Integer
        Dim D As Double
        Dim F As Single

        For I = 0 To grid_User_Data.Rows.Count - 1
            Address = grid_User_Data.Rows(I).Cells(1).Value

            Select Case Address
                Case enum_User_Data_Address.Command, enum_User_Data_Address.Status, enum_User_Data_Address.Status_Pending_Flag, enum_User_Data_Address.Saftey_Error_Code
                    N = KFlop.Get_User_Data_Integer(Address)
                    grid_User_Data.Rows(I).Cells(2).Value = N

                Case enum_User_Data_Address.Spindle_Speed
                    F = KFlop.Get_User_Data_Float(Address)
                    grid_User_Data.Rows(I).Cells(2).Value = FormatNumber(F, 0)

                Case Else
                    D = KFlop.Get_User_Data_Float(Address)
                    grid_User_Data.Rows(I).Cells(2).Value = FormatNumber(D, 4)
            End Select

        Next
    End Sub

    Private Sub Add_User_Data_Row(Name As String, Address As enum_User_Data_Address)
        Dim Addr As Integer = Address
        grid_User_Data.Rows.Add(Name, Addr, "")
    End Sub

    Private Sub grid_User_Data_CellClick(sender As Object, e As System.Windows.Forms.DataGridViewCellEventArgs) Handles grid_User_Data.CellClick
        If e.RowIndex < 0 Then Exit Sub

        Dim Addr As Integer = grid_User_Data.Rows(e.RowIndex).Cells(1).Value
        grid_User_Data.Rows(e.RowIndex).Cells(2).Value = Get_User_Input(grid_User_Data.Rows(e.RowIndex).Cells(2).Value)


        Select Case Addr
            Case enum_User_Data_Address.Command, enum_User_Data_Address.Status, enum_User_Data_Address.Status_Pending_Flag
                KFlop.Set_User_Data_Integer(Addr, grid_User_Data.Rows(e.RowIndex).Cells(2).Value)

            Case enum_User_Data_Address.Spindle_Speed
                KFlop.Set_User_Data_Float(Addr, grid_User_Data.Rows(e.RowIndex).Cells(2).Value)

            Case Else
                KFlop.Set_User_Data_Float(Addr, grid_User_Data.Rows(e.RowIndex).Cells(2).Value)

        End Select

    End Sub

    Private Sub ts_User_Data_Item_Clicked(sender As Object, e As ToolStripItemClickedEventArgs) Handles ts_User_Data.ItemClicked

        Select Case e.ClickedItem.Text

            Case "Refresh"
                Get_All_User_Data()

        End Select

    End Sub

#End Region

#Region "grid_Parameters"

    Private Sub ts_Go_To_Item_Clicked(sender As Object, e As ToolStripItemClickedEventArgs) Handles ts_Go_To.ItemClicked

        Select Case e.ClickedItem.Text
            Case "Insert"
                If grid_Parameters.Rows.Count = 0 Then
                    grid_Parameters.Rows.Insert(0, 1)
                    Edit_Parameters(0)
                Else
                    grid_Parameters.Rows.Insert(grid_Parameters.CurrentRow.Index, 1)
                    Reorder()
                    Move_To_Row(grid_Parameters.CurrentRow.Index - 1, True)
                    Edit_Parameters(grid_Parameters.CurrentRow.Index)
                End If

            Case "Delete"
                If grid_Parameters.Rows.Count = 0 Then Exit Sub

                Dim Idx As Integer = grid_Parameters.CurrentRow.Index
                grid_Parameters.Rows.RemoveAt(grid_Parameters.CurrentRow.Index)
                Reorder()
                If Idx <= grid_Parameters.RowCount - 1 Then
                    Move_To_Row(grid_Parameters.CurrentRow.Index, True)
                End If

            Case "Copy"
                Dim P(enum_grid_Parameter_Columns.Feed_Z_2) As String
                For I = 0 To enum_grid_Parameter_Columns.Feed_Z_2
                    P(I) = grid_Parameters.CurrentRow.Cells(I).Value
                Next
                grid_Parameters.Rows.Insert(grid_Parameters.CurrentRow.Index + 1, P)
                Reorder()
                grid_Parameters.ClearSelection()
                Move_To_Row(grid_Parameters.CurrentRow.Index + 1, True)


            Case "Move Up"
                Dim P1(enum_grid_Parameter_Columns.Feed_Z_2) As String
                Dim P2(enum_grid_Parameter_Columns.Feed_Z_2) As String

                If grid_Parameters.CurrentRow.Index < 1 Then Exit Sub

                For I = 0 To enum_grid_Parameter_Columns.Feed_Z_2
                    P1(I) = grid_Parameters.CurrentRow.Cells(I).Value
                Next

                For I = 0 To enum_grid_Parameter_Columns.Feed_Z_2
                    P2(I) = grid_Parameters.Rows(grid_Parameters.CurrentRow.Index - 1).Cells(I).Value
                Next

                For I = 0 To enum_grid_Parameter_Columns.Feed_Z_2
                    grid_Parameters.CurrentRow.Cells(I).Value = P2(I)
                Next

                For I = 0 To enum_grid_Parameter_Columns.Feed_Z_2
                    grid_Parameters.Rows(grid_Parameters.CurrentRow.Index - 1).Cells(I).Value = P1(I)
                Next

                Reorder()
                Move_To_Row(grid_Parameters.CurrentRow.Index - 1, False)

            Case "Move Down"
                Dim P1(enum_grid_Parameter_Columns.Feed_Z_2) As String
                Dim P2(enum_grid_Parameter_Columns.Feed_Z_2) As String

                If grid_Parameters.CurrentRow.Index >= grid_Parameters.RowCount - 1 Then Exit Sub

                For I = 0 To enum_grid_Parameter_Columns.Feed_Z_2
                    P1(I) = grid_Parameters.CurrentRow.Cells(I).Value
                Next

                For I = 0 To enum_grid_Parameter_Columns.Feed_Z_2
                    P2(I) = grid_Parameters.Rows(grid_Parameters.CurrentRow.Index + 1).Cells(I).Value
                Next

                For I = 0 To enum_grid_Parameter_Columns.Feed_Z_2
                    grid_Parameters.CurrentRow.Cells(I).Value = P2(I)
                Next

                For I = 0 To enum_grid_Parameter_Columns.Feed_Z_2
                    grid_Parameters.Rows(grid_Parameters.CurrentRow.Index + 1).Cells(I).Value = P1(I)
                Next

                Reorder()
                Move_To_Row(grid_Parameters.CurrentRow.Index + 1, False)

            Case "Load C Programs"
                Load_C_Program(KFlop.Home_Goto_Probe_C_Program, Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)
                Load_C_Program(KFlop.Spindle_C_Program, Class_KFLOP.enum_KFlop_Threads.Spindle_Control)
                Load_C_Program(KFlop.Probe_Toolsetter_Saftey_C_Program, Class_KFLOP.enum_KFlop_Threads.Probe_Toolsetter_Saftety)

        End Select

    End Sub

    Private Sub grid_Parameters_Cell_Mouse_Click(sender As Object, e As System.Windows.Forms.DataGridViewCellMouseEventArgs) Handles grid_Parameters.CellMouseClick
        Select Case e.ColumnIndex
            Case < 0
            Case enum_grid_Parameter_Columns.Set_Parameters
                C_Program_Parameters_Request = e.RowIndex
            Case Else
                Edit_Parameters(e.RowIndex)
        End Select
    End Sub

    Public Sub Update_Parameter_Changes()
        If C_Program_Parameters_Request < 0 Then Exit Sub
        Dim Row As Integer = C_Program_Parameters_Request
        KFlop.Set_User_Data_Integer(enum_User_Data_Address.Command, CInt(grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Command_Value).Value))
        KFlop.Set_User_Data_Integer(enum_User_Data_Address.Status_Pending_Flag, 0)

        KFlop.Set_User_Data_Float(enum_User_Data_Address.X_Params + enum_User_Data_Offsets.Dest_1, CSng(grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.X_1).Value))
        KFlop.Set_User_Data_Float(enum_User_Data_Address.X_Params + enum_User_Data_Offsets.Dest_2, CSng(grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.X_2).Value))
        KFlop.Set_User_Data_Float(enum_User_Data_Address.X_Params + enum_User_Data_Offsets.Feed_1, CSng(grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Feed_X_1).Value))
        KFlop.Set_User_Data_Float(enum_User_Data_Address.X_Params + enum_User_Data_Offsets.Feed_2, CSng(grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Feed_X_2).Value))
        KFlop.Set_User_Data_Float(enum_User_Data_Address.Y_Params + enum_User_Data_Offsets.Dest_1, CSng(grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Y_1).Value))
        KFlop.Set_User_Data_Float(enum_User_Data_Address.Y_Params + enum_User_Data_Offsets.Dest_2, CSng(grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Y_2).Value))
        KFlop.Set_User_Data_Float(enum_User_Data_Address.Y_Params + enum_User_Data_Offsets.Feed_1, CSng(grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Feed_Y_1).Value))
        KFlop.Set_User_Data_Float(enum_User_Data_Address.Y_Params + enum_User_Data_Offsets.Feed_2, CSng(grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Feed_Y_2).Value))
        KFlop.Set_User_Data_Float(enum_User_Data_Address.Z_Params + enum_User_Data_Offsets.Dest_1, CSng(grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Z_1).Value))
        KFlop.Set_User_Data_Float(enum_User_Data_Address.Z_Params + enum_User_Data_Offsets.Dest_2, CSng(grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Z_2).Value))
        KFlop.Set_User_Data_Float(enum_User_Data_Address.Z_Params + enum_User_Data_Offsets.Feed_1, CSng(grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Feed_Z_1).Value))
        KFlop.Set_User_Data_Float(enum_User_Data_Address.Z_Params + enum_User_Data_Offsets.Feed_2, CSng(grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Feed_Z_2).Value))
        Get_All_User_Data()
        C_Program_Parameters_Request = -1
    End Sub

    Private Sub Edit_Parameters(Row)
        Dim dlg As New dlg_Go_To

        dlg.Parameter_Name = grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Name).Value
        dlg.Command = grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Command).Value
        dlg.Command_Value = grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Command_Value).Value
        dlg.X_1 = grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.X_1).Value
        dlg.X_2 = grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.X_2).Value
        dlg.Y_1 = grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Y_1).Value
        dlg.Y_2 = grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Y_2).Value
        dlg.Z_1 = grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Z_1).Value
        dlg.Z_2 = grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Z_2).Value
        dlg.Feed_X_1 = grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Feed_X_1).Value
        dlg.Feed_X_2 = grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Feed_X_2).Value
        dlg.Feed_Y_1 = grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Feed_Y_1).Value
        dlg.Feed_Y_2 = grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Feed_Y_2).Value
        dlg.Feed_Z_1 = grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Feed_Z_1).Value
        dlg.Feed_Z_2 = grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Feed_Z_2).Value

        dlg.ShowDialog()
        If dlg.DialogResult = DialogResult.Cancel Then Exit Sub

        grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Name).Value = dlg.Parameter_Name
        grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Command).Value = dlg.Command
        grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Command_Value).Value = dlg.Command_Value
        grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.X_1).Value = dlg.X_1
        grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.X_2).Value = dlg.X_2
        grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Y_1).Value = dlg.Y_1
        grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Y_2).Value = dlg.Y_2
        grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Z_1).Value = dlg.Z_1
        grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Z_2).Value = dlg.Z_2

        grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Feed_X_1).Value = dlg.Feed_X_1
        grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Feed_X_2).Value = dlg.Feed_X_2
        grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Feed_Y_1).Value = dlg.Feed_Y_1
        grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Feed_Y_2).Value = dlg.Feed_Y_2
        grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Feed_Z_1).Value = dlg.Feed_Z_1
        grid_Parameters.Rows(Row).Cells(enum_grid_Parameter_Columns.Feed_Z_2).Value = dlg.Feed_Z_2

    End Sub

    Private Sub Reorder()
        For I = 0 To grid_Parameters.RowCount - 1
            grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Order).Value = I + 1
            grid_Parameters.Rows(I).Cells(enum_grid_Parameter_Columns.Set_Parameters).Value = "Set"
        Next
    End Sub

    Private Sub Move_To_Row(Row As Integer, Clear_Selection As Boolean)
        If Row < 0 Then Exit Sub
        If Row > grid_Parameters.RowCount - 1 Then Exit Sub

        If Clear_Selection Then
            If grid_Parameters.RowCount > 0 Then
                Move_To_Cell(grid_Parameters, 0, 1)
            End If
        End If

        Move_To_Cell(grid_Parameters, Row, 1)

    End Sub

    Public Sub Move_To_Cell(Grid As DataGridView, ByVal Row As Integer, Optional ByVal Cell As Integer = 0)
        If Row > Grid.RowCount - 1 Or Row < 0 Then Exit Sub
        If Cell > Grid.ColumnCount - 1 Then Exit Sub

        Grid.EndEdit()

        If IsNothing(Grid.Rows(Row)) Then Exit Sub
        If IsNothing(Grid.Rows(Row).Cells(Cell)) Then Exit Sub

        Try
            If Cell >= 0 Then
                If Grid.Rows(Row).Cells(Cell).Visible Then
                    Grid.CurrentCell = Grid.Rows(Row).Cells(Cell)
                End If
            Else
                'Move to first visible cell
                Dim C As Integer
                For C = 0 To Grid.Columns.Count - 1
                    If Grid.Columns(C).Visible Then
                        Grid.CurrentCell = Grid.Rows(Row).Cells(C)
                        Exit For
                    End If
                Next
            End If
        Catch ex As Exception
            'Ignore faults
        End Try

    End Sub

#End Region

End Class
