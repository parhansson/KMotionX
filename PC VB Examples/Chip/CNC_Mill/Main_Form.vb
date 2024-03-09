Imports System.IO
Imports System.Collections
Imports System.ComponentModel
Imports KMotion_dotNet

Public Class Main_Form

#Region "Declarations"

    Private Chip_Version As String

    Private WithEvents Startup_Timer As New Timer
    Private WithEvents KFlop_Timer As New System.Windows.Threading.DispatcherTimer

    Private WithEvents Keyboard_Hook As New Class_Keyboard_Hook

    Private Hidden_Tab_Pages = New List(Of TabPage)

    Private KFlop_Timer_Sync As Boolean = True

    Private Ignore_Key_Down As Boolean = False
    Public Initialized As Boolean = False
    Public Ignore_Event As Boolean = False

#End Region

#Region "Shown, Closing"

    '**************************************** IMPORTANT
    'If you get errors when opening this form in the forms editor, there is something wrong in the Main_Shown handler
    'This runs when you open the form in the forms editor
    'Comment out all the code, rebuild and see if the error goes away.
    'If so comment out portions of the code, rebuild and reopen in the forms editor to narrow down where the error is.

    Private Sub Main_Shown(sender As Object, e As EventArgs) Handles Me.Shown
        ''Hot key test form
        'Dim dlg As New dlg_Key_Test
        'dlg.ShowDialog()
        'End

        'If (My.Computer.Name = "RICK-HP") Or (My.Computer.Name = "DESKTOP-OEQGC6V") Then
        '    TEST_STAND = True
        'Else
        '    TEST_STAND = False
        'End If

        If Not Kflop.Ping() Then
            Flash_Message("Kflop not connected or not powered up.")
            SIMULATE = True
        Else
            SIMULATE = False
        End If

        If SIMULATE Then
            Initialize_Paths()
            Display.Load_Settings_File()

            ctl_Fixture_Offsets.Initialize()
            Display.Load_Persistant_Settings()

            Display.Initialize_Controls(Me)

            ctl_Gcode.Setup(ctl_Chip_Drawing)
            ctl_Chip_Drawing.Setup(ctl_Gcode)
            ctl_Fixture_Offsets.Setup()
            ctl_Build.Setup(ctl_Gcode, ctl_Fixture_Offsets)
            Exit Sub
        End If

        Initialize_Paths()
        Initialize_Main_Form_Settings()

        Trace_Control = ctl_Trace
        ctl_Trace.Initialize()

        Display.Load_Settings_File()
        Display.Load_Persistant_Settings()

        Kflop.Initialize()
        Kflop.Initialize_Offsets()

        Display.Initialize_Controls(Me)
        Display.Spindle_Handler(Class_Display_Handlers.enum_Event_Type.Initialize)

        track_Feed_Rate_Override.Value = 100

        ctl_Gcode.Setup(ctl_Chip_Drawing)
        ctl_Chip_Drawing.Setup(ctl_Gcode)
        ctl_Fixture_Offsets.Setup()
        ctl_Build.Setup(ctl_Gcode, ctl_Fixture_Offsets)

        Display.Reset_Handlers()

        Display.Set_Axis_Persistant_Positions()

        Kflop.Controller.ReleaseToken()

        txt_Spindle_Test_Rpm.Text = "0"
        Post_Error("")

        Logitech_G13.Init()

        Initialized = True

        Kflop_Timer.Interval = TimeSpan.FromMilliseconds(100)

        Startup_Timer.Interval = 100
        Startup_Timer.Enabled = True

    End Sub

    Private Sub Main_FormClosing(sender As Object, e As FormClosingEventArgs) Handles Me.FormClosing
        led_Spindle_Enable.Set_Boolean_Output(False, "Off")
        ctl_C_Interface.Set_Spindle_Speed(0)
        ctl_C_Interface.Set_Spindle_Override(100)
        led_Coolant_Enable.Set_Boolean_Output(False, "Off")

        If Not ctl_Build.Finish() Then
            e.Cancel = True
        End If

        Display.Finish(Me)
        Kflop.Finish()

    End Sub

    Private Sub Startup_Timer_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Startup_Timer.Tick
        ctl_Chip_Drawing.Reset(True)
        Startup_Timer.Enabled = False
        Kflop_Timer.IsEnabled = True
        Kflop_Timer.Start()
    End Sub

#End Region

#Region "Timers"

    Private Sub LimitLines(ByRef MyBox As TextBox, n As Integer)
        If MyBox.Lines.Count > n Then
            Dim newList As List(Of String) = MyBox.Lines.ToList
            newList.RemoveRange(0, MyBox.Lines.Count - n)
            MyBox.Lines = newList.ToArray
        End If
    End Sub

    'Private Trace_Count As Integer
    Private RTL As Integer = 0
    Private Current_Block As Integer

    Private Sub KFLOP_Timer_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Kflop_Timer.Tick

        If (Kflop.Controller.WaitToken(100) = KMOTION_TOKEN.KMOTION_LOCKED) Then

            Try
                Kflop.Main_Status = Kflop.Controller.GetStatus(False)
                Kflop.Controller.ReleaseToken()
                Kflop.Get_Axis_Positions()

                RTL = KFlop.RealtimeLine
                Current_Block = RTL

                Trace_Control.Update_Trace()

                If Kflop.GCode_Program_Executing Or Kflop.GCode_Single_Step Then
                    ctl_Gcode.Update_Sent_Line_Number(Kflop.Current_Sent_Line)
                    ctl_Gcode.Update_Line_Number(Current_Block)
                    ctl_Chip_Drawing.Part_Drawing.Update_Drawing(Current_Block)
                    Kflop.GCode_Single_Step = False
                End If

                Display.Update_Display()
                KFlop_Timer_Sync = True

            Catch ex As Exception
                Kflop.Controller.ReleaseToken()
            End Try

        End If

    End Sub

#End Region

#Region "Tool Strips"

    Private Sub ts_Main_ItemClicked(sender As Object, e As ToolStripItemClickedEventArgs) Handles ts_Main.ItemClicked
        If Not (Initialized) Then Exit Sub

        Select Case e.ClickedItem.Text
            Case "Exit"
                Close()

            Case "Save"
                Display.Finish(Me)
                ctl_Build.Finish()

                Flash_Message("Settings Saved")

            Case "Reset"
                    'Macros.Reset_Controller()

            Case "Setup"


            Case "Help"
                Dim Mess As String = Kflop.Get_Info("Version")
                Message_Box.ShowDialog(Mess)

        End Select

    End Sub

    Private Sub tab_Main_SelectedIndexChanged(sender As Object, e As System.EventArgs) Handles Tab_Main.SelectedIndexChanged
        'If Tab_Main.SelectedTab.Text = "Trace" Then
        '    Show_Trace_Information()
        'End If
        'btn_Connected.Focus()
    End Sub


#End Region

    Private Sub Hot_Key_Event(ByVal Command As String) Handles Keyboard_Hook.Hot_Key_Event
        Display.Hot_Key_Handler(Command)
    End Sub

#Region "Settings Tab"

    Private Sub Initialize_Main_Form_Settings()
        Dim P(1) As String
        For I = 0 To Tab_Main.TabPages.Count - 1
            P(0) = "Visible"
            P(1) = Tab_Main.TabPages(I).Text
            grd_Tab_Enables.Rows.Add(P)
            grd_Tab_Enables.Rows(I).Tag = Tab_Main.TabPages(I)
        Next
    End Sub

    Private Sub txt_Build_Root_Directory_Mouse_Click(sender As Object, e As System.Windows.Forms.MouseEventArgs) Handles txt_Build_Root_Directory.MouseClick
        Dim dlg As New FolderBrowserDialog()
        dlg.ShowDialog()
        If dlg.SelectedPath <> "" Then
            txt_Build_Root_Directory.Text = dlg.SelectedPath
        End If
    End Sub

    Private Sub Enable_Tab_Page(Page As TabPage, Enable As Boolean)
        If Enable Then
            Tab_Main.TabPages.Add(Page)
        Else
            Tab_Main.TabPages.Remove(Page)
        End If
    End Sub

    Private Sub grd_Tab_Enables_Cell_Click(sender As Object, e As System.Windows.Forms.DataGridViewCellEventArgs) Handles grd_Tab_Enables.CellClick
        If e.ColumnIndex > 0 Then Exit Sub

        If grd_Tab_Enables.Rows(e.RowIndex).Cells(1).Value = "Settings" Then
            Flash_Message("Cannot hide settings tab.")
            Exit Sub
        End If

        If grd_Tab_Enables.Rows(e.RowIndex).Cells(0).Value = "Visible" Then
            grd_Tab_Enables.Rows(e.RowIndex).Cells(0).Value = "Hidden"
            Tab_Main.TabPages.Remove(grd_Tab_Enables.Rows(e.RowIndex).Tag)
        Else
            grd_Tab_Enables.Rows(e.RowIndex).Cells(0).Value = "Visible"
            If e.RowIndex > Tab_Main.TabCount - 1 Then
                Tab_Main.TabPages.Insert(Tab_Main.TabCount - 1, grd_Tab_Enables.Rows(e.RowIndex).Tag)
            Else
                Tab_Main.TabPages.Insert(e.RowIndex, grd_Tab_Enables.Rows(e.RowIndex).Tag)
            End If
        End If
        Move_To_Cell(grd_Tab_Enables, e.RowIndex, 1)
        Move_To_Cell(grd_Tab_Enables, e.RowIndex, 0)

    End Sub

    Public Sub Move_To_Cell(grid As DataGridView, Row As Integer, Cell As Integer)
        If grid.RowCount = 0 Then Exit Sub
        If grid.RowCount = 1 Then Row = 0
        If Row >= grid.RowCount Then Row = grid.RowCount - 1
        grid.CurrentCell = grid.Rows(Row).Cells(Cell)
    End Sub

#End Region


    Public Sub Post_Error(Message As String)
        If Message = "" Then
            txt_Error.Text = ""
            txt_Error.Visible = False
        Else
            txt_Error.Text &= Message
            txt_Error.Visible = True
        End If
    End Sub

    Private Sub btn_Get_Soft_Limits_Click(sender As Object, e As EventArgs) Handles btn_Get_Soft_Limits.Click
        Dim X_Pos, X_Neg, X_Slave_Pos, X_Slave_Neg As Double
        Dim Y_Pos, Y_Neg, Y_Slave_Pos, Y_Slave_Neg As Double
        Dim Z_Pos As Double

        Kflop.Get_Soft_Limits(X_Pos, X_Neg, X_Slave_Pos, X_Slave_Neg, Y_Pos, Y_Neg, Y_Slave_Pos, Y_Slave_Neg, Z_Pos, False)

        txt_X_Minus_Soft_Limit.Text = FormatNumber(X_Neg, 2).PadLeft(6)
        txt_X_Plus_Soft_Limit.Text = FormatNumber(X_Pos, 2).PadLeft(6)
        txt_Y_Minus_Soft_Limit.Text = FormatNumber(Y_Neg, 2).PadLeft(6)
        txt_Y_Plus_Soft_Limit.Text = FormatNumber(Y_Pos, 2).PadLeft(6)
        txt_Z_Plus_Soft_Limit.Text = FormatNumber(Z_Pos, 2).PadLeft(6)

        Kflop.Get_Soft_Limits(X_Pos, X_Neg, X_Slave_Pos, X_Slave_Neg, Y_Pos, Y_Neg, Y_Slave_Pos, Y_Slave_Neg, Z_Pos, True)

        txt_KFlop_X_Minus_Soft_Limit.Text = FormatNumber(X_Neg / Kflop.X_Counts_Per_Inch, 2).PadLeft(6) & " (" & X_Neg & ")"
        txt_KFlop_X_Plus_Soft_Limit.Text = FormatNumber(X_Pos / Kflop.X_Counts_Per_Inch, 2).PadLeft(6) & " (" & X_Pos & ")"
        txt_KFlop_Y_Minus_Soft_Limit.Text = FormatNumber(Y_Neg / Kflop.Y_Counts_Per_Inch, 2).PadLeft(6) & " (" & Y_Neg & ")"
        txt_KFlop_Y_Plus_Soft_Limit.Text = FormatNumber(Y_Pos / Kflop.Y_Counts_Per_Inch, 2).PadLeft(6) & " (" & Y_Pos & ")"
        txt_KFlop_Z_Plus_Soft_Limit.Text = FormatNumber(Z_Pos / Kflop.Z_Counts_Per_Inch, 2).PadLeft(6) & " (" & Z_Pos & ")"

        txt_KFlop_X_Slave_Minus_Soft_Limit.Text = FormatNumber(X_Slave_Neg / Kflop.X_Counts_Per_Inch, 2).PadLeft(6) & " (" & X_Slave_Neg & ")"
        txt_KFlop_X_Slave_Plus_Soft_Limit.Text = FormatNumber(X_Slave_Pos / Kflop.X_Counts_Per_Inch, 2).PadLeft(6) & " (" & X_Slave_Pos & ")"
        txt_KFlop_Y_Slave_Minus_Soft_Limit.Text = FormatNumber(Y_Slave_Neg / Kflop.Y_Counts_Per_Inch, 2).PadLeft(6) & " (" & Y_Slave_Neg & ")"
        txt_KFlop_Y_Slave_Plus_Soft_Limit.Text = FormatNumber(Y_Slave_Pos / Kflop.Y_Counts_Per_Inch, 2).PadLeft(6) & " (" & Y_Slave_Pos & ")"

    End Sub

    Private Sub track_Feed_Rate_Value_Changed(sender As Object, e As EventArgs) Handles track_Feed_Rate_Override.ValueChanged
        If Ignore_Event Then Exit Sub
        If Not Initialized Then Exit Sub
        If Not KFlop_Timer_Sync Then Exit Sub

        Dim FR As Integer
        Dim FRO As Single = sender.value / 100

        txt_Feed_Rate_Override.Text = sender.value
        FR = sender.value

        If (KFlop.Feed_Rate * FR / 100) > Display.Max_Feedrate Then
            Ignore_Event = True
            FRO = Display.Max_Feedrate / KFlop.Feed_Rate
            txt_Feed_Rate_Override.Text = FRO * 100
            track_Feed_Rate_Override.Value = FRO * 100
            Ignore_Event = False
        End If

        Dim Cmd As String = "SetFRO " & FormatNumber(FRO, 2)
        KFlop.Send_Command(Cmd)

        KFlop_Timer_Sync = False
    End Sub

    Private Sub btn_Feed_Rate_Reset_Click(sender As Object, e As EventArgs) Handles btn_Feed_Rate_Reset.Click
        track_Feed_Rate_Override.Value = 100
    End Sub

    Private Sub btn_Feed_Rate_Max_Click(sender As Object, e As EventArgs) Handles btn_Feed_Rate_Max.Click
        Dim Pct As Integer
        Dim FRO As Single = Display.Max_Feedrate / KFlop.Feed_Rate
        Pct = FRO * 100
        If Pct > track_Feed_Rate_Override.Maximum Then
            track_Feed_Rate_Override.Value = track_Feed_Rate_Override.Maximum
        Else
            track_Feed_Rate_Override.Value = Pct
        End If
    End Sub

    Private Sub track_Spindle_Speed_Override_ValueChanged(sender As Object, e As EventArgs) Handles track_Spindle_Speed_Override.ValueChanged
        If Not Initialized Then Exit Sub
        If sender.value > Display.Max_Spindle_RPM Then

        End If

        txt_Spindle_Speed_Override.Text = sender.value
        ctl_C_Interface.Set_Spindle_Override(txt_Spindle_Speed_Override.Text)
    End Sub

    Private Sub btn_Spindle_RPM_Reset_Click(sender As Object, e As EventArgs) Handles btn_Spindle_RPM_Reset.Click
        track_Spindle_Speed_Override.Value = 100
    End Sub

    Private Sub btn_Spindle_RPM_Max_Click(sender As Object, e As EventArgs) Handles btn_Spindle_RPM_Max.Click
        If Not Initialized Then Exit Sub
        Dim RPM As Single = KFlop.Spindle_Speed
        track_Spindle_Speed_Override.Value = (Display.Max_Spindle_RPM / RPM) * 100
    End Sub

    Private Sub track_Spindle_Value_Changed(sender As Object, e As EventArgs) Handles track_Spindle_Speed.ValueChanged
        If Not Initialized Then Exit Sub
        If Not KFlop_Timer_Sync Then Exit Sub

        If btn_Spindle_Test.State = "Off" Then
            Flash_Message("Spindle Test Not Enabled")
            Exit Sub
        End If

        If sender.value = 0 Then
            txt_Spindle_Test_Rpm.Text = "0"
        Else
            txt_Spindle_Test_Rpm.Text = FormatNumber(sender.value, 0, TriState.False, TriState.False, TriState.True)
        End If

        If led_Spindle_Enable.State = "On" Then
            ctl_C_Interface.Set_Spindle_Speed(track_Spindle_Speed.Value)
        End If

        KFlop_Timer_Sync = False

    End Sub


    Private Sub txt_Error_Mouse_Click(sender As Object, e As System.Windows.Forms.MouseEventArgs) Handles txt_Error.MouseClick
        Post_Error("")
    End Sub



    Private Sub btn_Test_Click(sender As Object, e As EventArgs) Handles btn_Test.Click


        'KFlop.Z_Move_Absolute(Display.Maximum_Z, Display.Max_Feedrate)


        'Dim X As Integer = Me.Left + Me.Panel_Top.Left + btn_Cycle_Start.Left + (btn_Cycle_Start.Width / 2)
        'Dim Y As Integer = Me.Top + Me.Panel_Top.Top + btn_Cycle_Start.Top + (btn_Cycle_Start.Height / 2)
        'Mouse_Control.Move_Cursor(X, Y)
        'KFlop.Controller.CoordMotion.Interpreter.Interpret("")

        'ctl_C_Interface.Run_C_Program(Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe, Chip_C_Interface.enum_Commands.Test)
        '
        'txt_Test_2.Text = "SetFRO 1.0"
        'KFlop.Send_Command("SetFRO 1.0")

    End Sub

    Private Sub btn_Test_2_Click(sender As Object, e As EventArgs) Handles btn_Test_2.Click

        'txt_Test_2.Text = "SetFRO 0.5"
        'KFlop.Send_Command("SetFRO 0.5")

    End Sub


End Class

