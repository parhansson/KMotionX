Imports KMotion_dotNet
Imports System.IO

Public Class Class_KFLOP

#Region "Enums"

    Public Enum enum_KFlop_Threads
        Initialize = 1
        Home_Goto_Probe = 2
        Spindle_Control = 3
        Probe_Toolsetter_Saftety = 4
    End Enum

    Public Enum enum_Axis
        X
        Y
        Z
    End Enum

    Public Enum enum_Run_Mode
        Single_Step
        Continuous
    End Enum

    Public Enum enum_Coorinate_System
        G54 = 1
        G55 = 2
        G56 = 3
        G57 = 4
        G58 = 5
        G59 = 6
    End Enum

#End Region

#Region "Sructures"

    Structure struct_Offset
        Public G_Code As String
        Public X As Double
        Public Y As Double
        Public Z As Double
        Public Mount_X As Double
        Public Mount_Y As Double
        Public Mount_Delta_Z As Double
        Public Part_X As Double
        Public Part_Y As Double
        Public Part_Thickness As Double
    End Structure

#End Region

#Region "Declarations"

    Private Kflop_Version As String


    Public C_Program_Folder As String

    Public Initialiaze_C_Program = "Initialize KFlop TK.c"
    Public Home_Goto_Probe_C_Program = "Home_Goto_Probe_Routines TK.c"
    Public Spindle_C_Program As String = "Spindle_Control.c"
    Public Probe_Toolsetter_Saftey_C_Program As String = "Probe_Toolsetter_Saftey TK.c"

    Public Controller As KMotion_dotNet.KM_Controller
    Private X_Axis As KMotion_dotNet.KM_Axis
    Private Y_Axis As KMotion_dotNet.KM_Axis
    Private Z_Axis As KMotion_dotNet.KM_Axis
    Private X_Axis_Slave As KMotion_dotNet.KM_Axis
    Private Y_Axis_Slave As KMotion_dotNet.KM_Axis

    Private Master_Motor_Enable As KMotion_dotNet.KM_IO
    Private Kflop_Led_0 As KMotion_dotNet.KM_IO
    Private Kflop_Led_1 As KMotion_dotNet.KM_IO

    Public Metric_To_Inch As Integer = 25.4

    'Motor has 200 steps per rev, but with microstepping a step is divided by 16 so total counts for one rev is
    '200 * 16 = 3200
    '
    'For a ball screw with a 5mm lead, 1 rev or 3,200 counts = 5mm, so one count = 0.0015625‬ mm or 0.000061515748 in
    ' 1/0.00006151748 = 16,256 counts per inch

    'For a ball screw with a 10mm lead, 1 rev or 3,200 counts = 10mm, so one count = 0.003125‬ ‬mm or 0.000123031496 in
    ' 1/0.000123031496 = 8,128 counts per inch

    '10 mm = 0.3936996 inch so revs per inch = 2.54

    Public X_Counts_Per_Inch As Double = 16000   'Counts per motor rev times lead screw pitch. ie 5 turns = 1 inch = 16,000 counts
    Public Y_Counts_Per_Inch As Double = 16000    'Counts per motor rev times lead screw pitch. ie 5 turns = 1 inch = 16,000 counts
    Public Z_Counts_Per_Inch As Double = 16256   '5mm ball screw = 16256   10mm ball screw = 8128

    Public X_One_Inch_Per_Minute As Double = X_Counts_Per_Inch / 60
    Public Y_One_Inch_Per_Minute As Double = Y_Counts_Per_Inch / 60
    Public Z_One_Inch_Per_Minute As Double = Z_Counts_Per_Inch / 60

    Public G54_Offsets As struct_Offset
    Public G55_Offsets As struct_Offset

    Public KFlop_G55_Offsets As struct_Offset

    Private Selected_Offsets As enum_Coorinate_System

    Private Send_Request_Synclock As New Object
    Private Get_Response_Synclock As New Object
    Private Main_Status_Synclock As New Object
    Private Abort_Synclock As New Object

    Private Length_Units As KMotion_dotNet.CANON_UNITS = CANON_UNITS.CANON_UNITS_INCHES

    Public Main_Status As KMotion_dotNet.KM_MainStatus

    'Private Axis_Group_Array As KMotion_dotNet.KM_Axis()
    'Private Axis_Group As KMotion_dotNet.KM_AxisGroup

    Public Start_Here_Line As Integer = -1

    Public Current_Sent_Line As Integer

    Public GCode_Program_Done As Boolean = False
    Public GCode_Program_Executing As Boolean = False
    Public GCode_Single_Step As Boolean = False
    Public GCode_Program_Aborted As Boolean = False

    Public Interpreter_Lock As Boolean = False

    Public Test_Status As String

    Public MessageMutex As New Threading.Mutex()

    Public Run_Mode As enum_Run_Mode = enum_Run_Mode.Continuous

    Public Settings As New List(Of String)


    Public WithEvents Cycle_Stop_Timer As New Timer
    Public Cycle_Inhibit As Boolean = False



#End Region

#Region "Initialize"


    Public Function Ping()


        'Create Controller object
        Try
            Controller = New KMotion_dotNet.KM_Controller()
        Catch ex As Exception
            Show_Error("Unable to load KMotion_dotNet Libraries.  Check Windows PATH or .exe location\r\r" + ex.Message)
            End
        End Try

        'Ping Kflop
        Dim Response As String = ""
        Try
            Response = Controller.WriteLineReadLine("Version")
        Catch ex As KMotion_dotNet.DMException
            Return False
        End Try

        Kflop_Version = Get_Info("Version")
        Return True

    End Function

    Public Sub Initialize()

        'Try
        '    Controller = New KMotion_dotNet.KM_Controller()
        'Catch ex As Exception
        '    Show_Error("Unable to load KMotion_dotNet Libraries.  Check Windows PATH or .exe location\r\r" + ex.Message)
        '    End
        'End Try

        'Ping()

        C_Program_Folder = Chip_Root_Path & "\Kflop_C_Programs\"


        X_Axis = New KMotion_dotNet.KM_Axis(Controller, 0, "X")
        Y_Axis = New KMotion_dotNet.KM_Axis(Controller, 1, "Y")
        Z_Axis = New KMotion_dotNet.KM_Axis(Controller, 2, "Z")

        X_Axis_Slave = New KMotion_dotNet.KM_Axis(Controller, 3, "XS")
        Y_Axis_Slave = New KMotion_dotNet.KM_Axis(Controller, 4, "YS")

        Master_Motor_Enable = New KMotion_dotNet.KM_IO(Controller, 45, "Master", KMotion_dotNet.IO_TYPE.DIGITAL_OUT)
        Kflop_Led_0 = New KMotion_dotNet.KM_IO(Controller, 46, "Led_0", KMotion_dotNet.IO_TYPE.DIGITAL_OUT)
        Kflop_Led_1 = New KMotion_dotNet.KM_IO(Controller, 47, "Led_1", KMotion_dotNet.IO_TYPE.DIGITAL_OUT)


        'CoordMotion Params
        Controller.CoordMotion.MotionParams.BreakAngle = 40
        Controller.CoordMotion.MotionParams.CollinearTolerance = 0.001
        Controller.CoordMotion.MotionParams.CornerTolerance = 0.001
        Controller.CoordMotion.MotionParams.FacetAngle = 1.0
        Controller.CoordMotion.MotionParams.MaxLinearLength = 100
        Controller.CoordMotion.MotionParams.TPLookahead = 3

        Controller.CoordMotion.MotionParams.MaxAccelX = 10
        Controller.CoordMotion.MotionParams.MaxAccelY = 10
        Controller.CoordMotion.MotionParams.MaxAccelZ = 10

        Controller.CoordMotion.MotionParams.CountsPerInchX = X_Counts_Per_Inch
        Controller.CoordMotion.MotionParams.CountsPerInchY = Y_Counts_Per_Inch
        Controller.CoordMotion.MotionParams.CountsPerInchZ = Z_Counts_Per_Inch

        Controller.CoordMotion.MotionParams.DegreesA = False
        Controller.CoordMotion.MotionParams.DegreesB = False
        Controller.CoordMotion.MotionParams.DegreesC = False

        Controller.CoordMotion.Interpreter.SetMcodeAction(3, MCODE_TYPE.M_Action_Setbit, 0, 1, 0, 0, 0, "M3") 'Spindle On Clockwise, output number 0
        Controller.CoordMotion.Interpreter.SetMcodeAction(5, MCODE_TYPE.M_Action_Setbit, 0, 0, 0, 0, 0, "M5") 'Spindle Off
        Controller.CoordMotion.Interpreter.SetMcodeAction(8, MCODE_TYPE.M_Action_Setbit, 1, 1, 0, 0, 0, "M8") 'Coolant On, output number 1
        Controller.CoordMotion.Interpreter.SetMcodeAction(9, MCODE_TYPE.M_Action_Setbit, 1, 0, 0, 0, 0, "M9") 'Coolant Off

        'Spindle_Speed Kflop parameter location is 5, Kflop interpreter updates this to Gcode S parameter (Spindle RPM)
        Controller.CoordMotion.Interpreter.SetMcodeAction(10, MCODE_TYPE.M_Action_Program, 3, 5, 5, 5, 5, C_Program_Folder & Spindle_C_Program)

        AddHandler Controller.CoordMotion.CoordMotionStraightTraverse, AddressOf On_Coord_Motion_Straight_Tranverse
        AddHandler Controller.CoordMotion.CoordMotionStraightFeed, AddressOf On_Coord_Motion_Straight_Feed
        AddHandler Controller.CoordMotion.CoordMotionArcFeed, AddressOf On_Coord_Motion_Arc_Feed

        AddHandler Controller.CoordMotion.AsyncTraverseCompleted, AddressOf On_Async_Traverse_Completed

        AddHandler Controller.CoordMotion.Interpreter.InterpreterStatusUpdated, AddressOf Interpreter_Status_Updated
        AddHandler Controller.CoordMotion.Interpreter.InterpreterCompleted, AddressOf Interpreter_Complete

        AddHandler Controller.MessageReceived, AddressOf Console_Message_Received
        AddHandler Controller.ErrorReceived, AddressOf Console_Error_Received

        '        Enable_Soft_Limits()

        X_Enable = True
        Y_Enable = True
        Z_Enable = True

        'Wait.Delay_Seconds(0.2)
        Initialize_Interpreter()

    End Sub

    Public Sub Finish()
        Kflop.Axis_Master_Enable = False
    End Sub

#End Region

#Region "C Program Interface"

    Public Function Get_User_Data_Integer(Address) As Integer
        Return Controller.GetUserData(Address)
    End Function

    Public Function Get_User_Data_Float(Address As Integer) As Single
        Return Controller.GetUserDataFloat(Address)
    End Function

    Public Function Get_User_Data_Double(Address As Integer) As Double
        Return Controller.GetUserDataDouble(Address)
    End Function

    Public Sub Set_User_Data_Integer(Address As Integer, Data As Integer)
        Controller.SetUserData(Address, Data)
    End Sub

    Public Sub Set_User_Data_Float(Address As Integer, Data As Single)
        Controller.SetUserDataFloat(Address, Data)
    End Sub

    Public Sub Set_User_Data_Double(Address As Integer, Data As Double)
        Controller.SetUserDataDouble(Address, Data)
    End Sub

    Public Sub Load_C_Program(File_Name As String, Thread As Integer)
        If File.Exists(File_Name) Then
            Controller.CompileAndLoadCoff(Thread, File_Name, False)
        Else
            Show_Error("File Not Found : " & vbCrLf & File_Name)
        End If
    End Sub

    Public Sub Run_C_Program(File_Name As String, Thread As Integer)
        If File.Exists(File_Name) Then
            Controller.ExecuteProgram(Thread, File_Name, True)
        Else
            Show_Error("File Not Found : " & vbCrLf & File_Name)
        End If
    End Sub

    Public Sub Run_C_Program(Thread As Integer)
        Controller.ExecuteProgram(Thread)
    End Sub

    Public Sub Kill_C_Program(Thread As Integer)
        Dim T(0) As Integer
        T(0) = Thread
        Controller.KillProgramThreads(T)
        Controller.Feedhold()
        Controller.ResumeFeedhold()
    End Sub

    Public Sub Kill_All_C_Programs()
        Dim T(5) As Integer
        For I = 0 To 2
            T(I) = I + 1
        Next
        Controller.KillProgramThreads(T)
    End Sub

    Public Function Status_Pending() As Boolean
        If Get_User_Data_Integer(Chip_C_Interface.enum_User_Data_Address.Status_Pending_Flag) <> 0 Then
            Return True
        Else
            Return False
        End If
    End Function

    Public Sub Clear_Status_Pending()
        Set_User_Data_Integer(Chip_C_Interface.enum_User_Data_Address.Status_Pending_Flag, 0)
    End Sub

    Public Property C_Program_Command As Chip_C_Interface.enum_Commands
        Get
            Return Get_User_Data_Integer(Chip_C_Interface.enum_User_Data_Address.Command)
        End Get
        Set(ByVal value As Chip_C_Interface.enum_Commands)
            Set_User_Data_Integer(Chip_C_Interface.enum_User_Data_Address.Command, value)
        End Set
    End Property

    Public ReadOnly Property C_Program_Status As Chip_C_Interface.enum_Status
        Get
            Return Get_User_Data_Integer(Chip_C_Interface.enum_User_Data_Address.Status)
        End Get
    End Property


#End Region

#Region "Event Handlers (CoordMotion & Interpreter)"

    Public Sub On_Coord_Motion_Straight_Tranverse(ByVal x As Double, ByVal y As Double, ByVal z As Double, ByVal sequence_number As Integer)
        MessageMutex.WaitOne()
        Trace_Control.Log_Trace_Message(String.Format("[Motion] Straight Traverse : X={0}  Y={1}  Z={2}  Seq= {3}", x, y, z, sequence_number) & vbCrLf)
        MessageMutex.ReleaseMutex()
    End Sub

    Public Sub On_Coord_Motion_Straight_Feed(ByVal DesiredFeedRate_in_per_sec As Double, ByVal x As Double, ByVal y As Double, ByVal z As Double, ByVal sequence_number As Integer, ByVal ID As Integer)
        MessageMutex.WaitOne()
        Trace_Control.Log_Trace_Message(String.Format("[Motion] Straight Feed : X={0}  Y={1}  Z={2}  Seq= {3}  ID={4}", x, y, z, sequence_number, ID) & vbCrLf)
        MessageMutex.ReleaseMutex()
    End Sub

    Public Sub On_Coord_Motion_Arc_Feed(ByVal ZeroLenAsFullCircles As Boolean, ByVal DesiredFeedRate_in_per_sec As Double,
                                    ByVal plane As Integer, ByVal first_end As Double, ByVal second_end As Double,
                                    ByVal first_axis As Double, ByVal second_axis As Double, ByVal rotation As Integer,
                                    ByVal axis_end_point As Double, ByVal first_start As Double, ByVal second_start As Double,
                                    ByVal axis_start_point As Double, ByVal sequence_number As Integer, ByVal ID As Integer)
        MessageMutex.WaitOne()
        Trace_Control.Log_Trace_Message(String.Format("[Motion] Arc Feed : EX={0}  EY={1}  EZ={2}", first_end, second_end, axis_end_point) & vbCrLf)
        MessageMutex.ReleaseMutex()
    End Sub

    Public Sub On_Async_Traverse_Completed()
        MessageMutex.WaitOne()
        Trace_Control.Log_Trace_Message(String.Format("[Motion] Straight Traverse Complete" & vbCrLf))
        MessageMutex.ReleaseMutex()
    End Sub

#End Region

#Region "Cycle_Start, Cycle_Stop, Execute_Program"

    Public Function Cycle_Start() As Boolean
        If Cycle_Inhibit Then
            Flash_Message("Cycle start inhibited for 5 seconds.")
            Return False
        End If

        If Main_Form.ctl_GCode.Start_Execution() Then
            Main_Form.ctl_Chip_Drawing.Part_Drawing.Start_Drawing()
            Run_GCode(Interpreter_File_Name)
            Return True
        Else
            Return False
        End If
    End Function

    Public Sub Cycle_Stop()
        Controller.CoordMotion.Interpreter.Halt()
        Cycle_Stop_Timer.Interval = 5000
        Cycle_Inhibit = True
        Cycle_Stop_Timer.Start()
        Interpreter_Lock = False
    End Sub

    Public Sub Cycle_Stop_Timer_Tick() Handles Cycle_Stop_Timer.Tick
        Cycle_Inhibit = False
        Cycle_Stop_Timer.Stop()
    End Sub

    Public Sub Execute_Program(Thread As Integer, Program As String, Bypass_Error As Boolean)
        Controller.ExecuteProgram(Thread, Program, Bypass_Error)
    End Sub

#End Region

#Region "Kflop communication"

    Private Function Console_Message_Received(Message As String) As Integer
        Message = Message.Replace("~", "")

        Select Case Mid(Message, 1, 1)
            Case "@" 'Status or debug message
                Message = Message.Replace("@", "")
                Trace_Control.Log_Trace_Message("[Consol] " & Message)

            Case "*" 'Error message
                Message = Message.Replace("*", "")
                Trace_Control.Log_Trace_Message("[Consol Error] " & Message)
                Main_Form.Post_Error(Message)

            Case "!" 'Error message
                Message = Message.Replace("!", "")
                Show_Error(Message, "")
                Trace_Control.Log_Trace_Message("[Consol Error] " & Message)

            Case Else 'Message from KFlop system
                Trace_Control.Log_Trace_Message("[Kflop] " & Message)
                If Message.Contains("Soft Limit FeedHold") Then
                    Message = Message.Replace(":0", ": X Axis")
                    Message = Message.Replace(":1", ": Y Axis")
                    Message = Message.Replace(":2", ": Z Axis")
                End If
                Main_Form.Post_Error(Message)
        End Select


        Return 1
    End Function

    Private Function Console_Error_Received(Message As String) As Integer
        Message = Message.Replace("~", "")
        Trace_Control.Log_Trace_Message("[KFlop Err] " & Message)
        Show_Error(Message)
        Return 1
    End Function

    Public Sub Send_Command(ByVal Command As String)
        SyncLock Send_Request_Synclock
            Try
                Controller.WriteLine(Command)
            Catch ex As KMotion_dotNet.DMException
                Show_Error(ex.Message)
            End Try
        End SyncLock
    End Sub

    Public Function Get_Info(ByVal Command As String) As String
        SyncLock Get_Response_Synclock
            Dim Response As String = ""
            Try
                Response = Controller.WriteLineReadLine(Command)
            Catch ex As KMotion_dotNet.DMException
                Response = ""
                Show_Error(ex.Message)
            End Try
            Return Response
        End SyncLock
    End Function

    Public Function Get_Message_Log() As String
        Return Controller.MessageLog
    End Function

#End Region

#Region "Conversion routines"

    Public Function X_Counts(Inches As Double) As Double
        Return Inches * X_Counts_Per_Inch
    End Function

    Public Function Y_Counts(Inches As Double) As Double
        Return Inches * Y_Counts_Per_Inch
    End Function

    Public Function Z_Counts(Inches As Double) As Double
        Return Inches * Z_Counts_Per_Inch
    End Function

#End Region

#Region "Digital Inputs"

    Public Function Input_Bit(ID As Integer) As Boolean
        If Main_Status.TimeStamp = 0.0 Then Return False ' check if ever read

        If (ID >= 1024 And ID < 1024 + 32) Then
            If Main_Status.GetVirtualBitsEx0(ID - 1024) Then Return True Else Return False
        End If
        If ID < 48 Then
            If Main_Status.GetKFlopBitState(ID) > 0 Then Return True Else Return False
        End If
        If Main_Status.GetVirtualBits(ID - 152) > 0 Then Return True Else Return False
    End Function

#End Region

#Region "KFlop Leds"

    Public Property LED_0() As Boolean
        Get
            Return Kflop_Led_0.GetDigitalValue
        End Get
        Set(ByVal value As Boolean)
            Kflop_Led_0.SetDigitalValue(value)
        End Set
    End Property

    Public Property LED_1() As Boolean
        Get
            Return Kflop_Led_1.GetDigitalValue
        End Get
        Set(ByVal value As Boolean)
            Kflop_Led_1.SetDigitalValue(value)
        End Set
    End Property

#End Region

#Region "Axis Positions"

    Public Sub Get_Axis_Positions()
        Dim A As Double
        Dim B As Double
        Dim C As Double
        Controller.CoordMotion.UpdateCurrentPositionsABS(_X_Abs, _Y_Abs, _Z_Abs, A, B, C, False)
        'Set last parameter to TRUE to snap to exact previously commanded postion (if close) to avoid rounding errors

    End Sub

    Private _X_Abs As Double
    Public Property X_Abs() As Double
        Get
            Return _X_Abs
        End Get
        Set(ByVal value As Double)
            _X_Abs = value
            X_Axis.SetCommandedDest(value * Controller.CoordMotion.MotionParams.CountsPerInchX)
        End Set
    End Property

    Private _Y_Abs As Double
    Public Property Y_Abs() As Double
        Get
            Return _Y_Abs
        End Get
        Set(ByVal value As Double)
            _Y_Abs = value
            Y_Axis.SetCommandedDest(value * Controller.CoordMotion.MotionParams.CountsPerInchY)
        End Set
    End Property

    Private _Z_Abs As Double
    Public Property Z_Abs() As Double
        Get
            Return _Z_Abs
        End Get
        Set(ByVal value As Double)
            _Z_Abs = value
            Z_Axis.SetCommandedDest(value * Controller.CoordMotion.MotionParams.CountsPerInchZ)
        End Set
    End Property

    'Public Property X_Abs() As Double
    '    Get
    '        Return X_Axis.GetCommandedPosition / Controller.CoordMotion.MotionParams.CountsPerInchX
    '    End Get
    '    Set(ByVal value As Double)
    '        X_Axis.SetCommandedDest(value * Controller.CoordMotion.MotionParams.CountsPerInchX)
    '    End Set
    'End Property

    'Public Property Y_Abs() As Double
    '    Get
    '        Return Y_Axis.GetCommandedPosition / Controller.CoordMotion.MotionParams.CountsPerInchY
    '    End Get
    '    Set(ByVal value As Double)
    '        Y_Axis.SetCommandedDest(value * Controller.CoordMotion.MotionParams.CountsPerInchY)
    '    End Set
    'End Property

    'Public Property Z_Abs() As Double
    '    Get
    '        Return Z_Axis.GetCommandedPosition / Controller.CoordMotion.MotionParams.CountsPerInchZ
    '    End Get
    '    Set(ByVal value As Double)
    '        Z_Axis.SetCommandedDest(value * Controller.CoordMotion.MotionParams.CountsPerInchZ)
    '    End Set
    'End Property

    Public ReadOnly Property X_Work() As Double
        Get
            Return (X_Axis.GetCommandedPosition / X_Counts_Per_Inch) - G55_Offsets.X
        End Get
    End Property

    Public ReadOnly Property Y_Work() As Double
        Get
            Return (Y_Axis.GetCommandedPosition / Y_Counts_Per_Inch) - G55_Offsets.Y
        End Get
    End Property

    Public ReadOnly Property Z_Work() As Double
        Get
            Return (Z_Axis.GetCommandedPosition / Z_Counts_Per_Inch) - G55_Offsets.Z
        End Get
    End Property

    Public ReadOnly Property X_Work_Offset() As Double
        Get
            Return G55_Offsets.X
        End Get
    End Property

    Public ReadOnly Property Y_Work_Offset() As Double
        Get
            Return G55_Offsets.Y
        End Get
    End Property

    Public ReadOnly Property Z_Work_Offset As Double
        Get
            Return G55_Offsets.Z
        End Get
    End Property

#End Region

#Region "Status"

    Public ReadOnly Property Active_G_Codes() As String
        Get
            Dim G As New List(Of String)
            Dim S As String = ""
            G = Controller.CoordMotion.Interpreter.SetupParams.Active_GCodes
            For I = 0 To G.Count - 1
                S &= " " & G(I)
            Next
            Return S
        End Get
    End Property

    Public ReadOnly Property Active_M_Codes() As String
        Get
            Dim M As New List(Of String)
            Dim S As String = ""
            M = Controller.CoordMotion.Interpreter.SetupParams.Active_MCodes
            For I = 0 To M.Count - 1
                S &= " " & M(I)
            Next
            Return S
        End Get
    End Property

    Public Function If_Is_Active_M_Code(M_Code As Integer) As Boolean
        Dim M As New List(Of String)
        Dim S As String = "M" & Trim(Str(M_Code))
        M = Controller.CoordMotion.Interpreter.SetupParams.Active_MCodes
        For I = 0 To M.Count - 1
            If M(I) = S Then
                Return True
            End If
        Next
        Return False
    End Function

#End Region

#Region "Offsets"

    Public Sub Initialize_Offsets()
        Set_Coordinate_System(enum_Coorinate_System.G54)
    End Sub

    Public Sub Set_Fixture_Offsets(Coord_Sys As enum_Coorinate_System, X As Double, Y As Double, Z As Double)
        If Coord_Sys = enum_Coorinate_System.G54 Then
            G54_Offsets.X = X
            G54_Offsets.Y = Y
            G54_Offsets.Z = Z
        Else
            G55_Offsets.X = X
            G55_Offsets.Y = Y
            G55_Offsets.Z = Z
        End If
        KFlop.Controller.CoordMotion.Interpreter.SetOrigin(Coord_Sys, X, Y, Z, 0, 0, 0)
    End Sub

    Public Function Get_Coordinate_System_G_Code(Coord_System As enum_Coorinate_System) As String
        Dim S As String
        S = Coord_System.ToString
        Return S
    End Function

    Public Sub Download_Offsets()
        Kflop.Controller.CoordMotion.Interpreter.SetOrigin(enum_Coorinate_System.G54, G54_Offsets.X, G54_Offsets.Y, G54_Offsets.Z, 0, 0, 0)
        Kflop.Controller.CoordMotion.Interpreter.SetOrigin(enum_Coorinate_System.G55, G55_Offsets.X, G55_Offsets.Y, G55_Offsets.Z, 0, 0, 0)
    End Sub

    Public Function Upload_Offsets(Coord_System As enum_Coorinate_System) As struct_Offset
        Dim Offsets As struct_Offset = Nothing
        Controller.CoordMotion.Interpreter.GetOrigin(Coord_System, Offsets.X, Offsets.Y, Offsets.Z, -1, -1, -1)
        Return Offsets
    End Function

    Public Sub Set_Coordinate_System(Coord_System As enum_Coorinate_System)
        Selected_Offsets = Coord_System
        Controller.CoordMotion.Interpreter.ChangeFixtureNumber(Coord_System)  'Load offsets For fixture
    End Sub

    Public Function Get_Coordinate_System() As enum_Coorinate_System
        Selected_Offsets = Controller.CoordMotion.Interpreter.SetupParams.OriginIndex
        Return Selected_Offsets
    End Function

    Public Function Get_Current_Coordinate_System_GCode() As String
        Selected_Offsets = Controller.CoordMotion.Interpreter.SetupParams.OriginIndex
        Return Selected_Offsets.ToString
    End Function

#End Region

#Region "Estop, Feed_Hold"

    Private _E_Stop As Boolean
    Public Property E_Stop() As Boolean
        Get
            Return _E_Stop
        End Get
        Set(ByVal value As Boolean)
            _E_Stop = value
        End Set
    End Property

    Private _Feed_Hold As Boolean
    Public Property Feed_Hold() As Boolean
        Get
            Return _Feed_Hold
        End Get
        Set(ByVal value As Boolean)
            _Feed_Hold = value
            If _Feed_Hold Then
                Controller.Feedhold()
            Else
                Controller.ResumeFeedhold()
            End If
        End Set
    End Property

#End Region

#Region "Axis Enables"

    Private _X_Enable As Boolean
    Public Property X_Enable() As Boolean
        Get
            If X_Axis.CheckEnabled And X_Axis_Slave.CheckEnabled Then
                Return True
            Else
                Return False
            End If
        End Get

        Set(ByVal value As Boolean)
            If value = True Then
                Dim Pos As Double
                Pos = Kflop.X_Abs
                X_Axis.Enable()
                X_Axis_Slave.Enable()
                Kflop.X_Abs = Pos
            Else
                X_Axis.Disable()
                X_Axis_Slave.Disable()
            End If
        End Set
    End Property

    Private _Y_Enable As Boolean
    Public Property Y_Enable() As Boolean
        Get
            If Y_Axis.CheckEnabled And Y_Axis_Slave.CheckEnabled Then
                Return True
            Else
                Return False
            End If
        End Get
        Set(ByVal value As Boolean)
            If value = True Then
                Dim Pos As Double
                Pos = Kflop.Y_Abs
                Y_Axis.Enable()
                Y_Axis_Slave.Enable()
                Kflop.Y_Abs = Pos
            Else
                Y_Axis.Disable()
                Y_Axis_Slave.Disable()
            End If
        End Set
    End Property

    Private _Z_Enable As Boolean
    Public Property Z_Enable() As Boolean
        Get
            Return Z_Axis.CheckEnabled
        End Get
        Set(ByVal value As Boolean)
            If value = True Then
                Dim Pos As Double
                Pos = Kflop.Z_Abs
                Z_Axis.Enable()
                Kflop.Z_Abs = Pos
            Else
                Z_Axis.Disable()
            End If
        End Set
    End Property

    Private _X_Slave_Enable As Boolean
    Public Property X_Slave_Enable() As Boolean
        Get
            Return X_Axis_Slave.CheckEnabled
        End Get
        Set(ByVal value As Boolean)
            If value = True Then X_Axis_Slave.Enable() Else X_Axis_Slave.Disable()
        End Set
    End Property

    Private _Y_Slave_Enable As Boolean
    Public Property Y_Slave_Enable() As Boolean
        Get
            Return Y_Axis_Slave.CheckEnabled
        End Get
        Set(ByVal value As Boolean)
            If value = True Then Y_Axis_Slave.Enable() Else Y_Axis_Slave.Disable()
        End Set
    End Property

    Public Property Axis_Master_Enable() As Boolean
        Get
            Return Master_Motor_Enable.GetDigitalValue
        End Get
        Set(ByVal value As Boolean)
            Master_Motor_Enable.SetDigitalValue(value)
        End Set
    End Property

#End Region

#Region "Status"

    Public Function Is_M_Code_Active(M_Code As Integer) As Boolean

        Return False
    End Function

    Public Function Coolant_Status() As Boolean
        Return Controller.CoordMotion.Interpreter.SetupParams.MistCoolantOn

    End Function

    Public Property Feed_Rate() As Double
        Get
            Return Controller.CoordMotion.Interpreter.SetupParams.FeedRate
        End Get
        Set(ByVal value As Double)
            Controller.CoordMotion.Interpreter.SetupParams.FeedRate = value
        End Set
    End Property

    Public Function Spindle_Speed() As Double
        Return Controller.CoordMotion.Interpreter.SetupParams.SpindleSpeed
    End Function

#End Region

#Region "Jogging"

    Public Jog_Feedrate As Double = 30


    Public Sub X_Jog(Feed_Rate As Double)
        If X_Axis.CheckEnabled And Axis_Master_Enable Then
            X_Axis.Jog(Feed_Rate * X_One_Inch_Per_Minute)
        Else
            If Feed_Rate <> 0 Then Flash_Message("X Axis disabled")
        End If
    End Sub

    Public Sub Y_Jog(Feed_Rate As Double)
        If Y_Axis.CheckEnabled And Axis_Master_Enable Then
            Y_Axis.Jog(Feed_Rate * Y_One_Inch_Per_Minute)
        Else
            If Feed_Rate <> 0 Then Flash_Message("Y Axis disabled")
        End If
    End Sub

    Public Sub Z_Jog(Feed_Rate As Double)
        If Z_Axis.CheckEnabled And Axis_Master_Enable Then
            Z_Axis.Jog(Feed_Rate * Z_One_Inch_Per_Minute)
        Else
            If Feed_Rate <> 0 Then Flash_Message("Z Axis disabled")
        End If
    End Sub


#End Region

#Region "Moving"

    Public Sub X_Stop()
        X_Axis.Stop()
    End Sub

    Public Sub Y_Stop()
        Y_Axis.Stop()
    End Sub

    Public Sub Z_Stop()
        Z_Axis.Stop()
    End Sub

    Public Sub X_Move_Relative(Increment As Double, Feedrate As Double)
        If X_Axis.CheckEnabled And Axis_Master_Enable Then
            Dim Save As Double = X_Axis.Velocity
            Dim N As Double = Kflop.X_Counts(Increment)
            Dim F As Double = Feedrate * X_One_Inch_Per_Minute
            X_Axis.Velocity = F
            X_Axis.StartRelativeMoveTo(N)
            X_Axis.Velocity = Save
        Else
            Flash_Message("Axis disabled")
        End If
    End Sub

    Public Sub Y_Move_Relative(Increment As Double, Feedrate As Double)
        If Y_Axis.CheckEnabled And Axis_Master_Enable Then
            Dim Save As Double = Y_Axis.Velocity
            Dim N As Double = Kflop.Y_Counts(Increment)
            Dim F As Double = Feedrate * Y_One_Inch_Per_Minute
            Y_Axis.Velocity = F
            Y_Axis.StartRelativeMoveTo(N)
            Y_Axis.Velocity = Save
        Else
            Flash_Message("Axis disabled")
        End If
    End Sub


    Public Sub Z_Move_Relative(Increment As Double, Feedrate As Double)
        If Z_Axis.CheckEnabled And Axis_Master_Enable Then
            Dim Save As Double = Z_Axis.Velocity
            Dim N As Double = Kflop.Z_Counts(Increment)
            Dim F As Double = Feedrate * Z_One_Inch_Per_Minute
            Z_Axis.Velocity = F
            Z_Axis.StartRelativeMoveTo(N)
            Z_Axis.Velocity = Save
        Else
            Flash_Message("Axis disabled")
        End If
    End Sub

    Public Sub Z_Move_Absolute(Z As Double, Feedrate As Double)
        If Z_Axis.CheckEnabled And Axis_Master_Enable Then
            Dim Save As Double = Z_Axis.Velocity
            Dim N As Double = KFlop.Z_Counts(Z)
            Dim F As Double = Feedrate * Z_One_Inch_Per_Minute
            Z_Axis.Velocity = F
            Z_Axis.StartMoveTo(N)
            Z_Axis.Velocity = Save
        Else
            Flash_Message("Axis disabled")
        End If
    End Sub

    Public Sub Stop_Move()
        Send_Command("StopImmediate0")
    End Sub

#End Region

#Region "Soft_Limits"

    Public Sub Get_Soft_Limits(ByRef X_Pos As Single, ByRef X_Neg As Single,
                               ByRef X_Slave_Pos As Single, ByRef X_Slave_Neg As Single,
                               ByRef Y_Pos As Single, ByRef Y_Neg As Double,
                               ByRef Y_Slave_Pos As Single, ByRef Y_Slave_Neg As Single,
                               ByRef Z_Pos As Single, Counts As Boolean)
        Dim S As String = ""

        If Counts Then

            S = Kflop.Get_Info("SoftLimitPos0")
            X_Pos = CDbl(S)
            S = Kflop.Get_Info("SoftLimitNeg0")
            X_Neg = CDbl(S)

            S = Kflop.Get_Info("SoftLimitPos3")
            X_Slave_Pos = CDbl(S)
            S = Kflop.Get_Info("SoftLimitNeg3")
            X_Slave_Neg = CDbl(S)

            S = Kflop.Get_Info("SoftLimitPos1")
            Y_Pos = CDbl(S)
            S = Kflop.Get_Info("SoftLimitNeg1")
            Y_Neg = CDbl(S)

            S = Kflop.Get_Info("SoftLimitPos4")
            Y_Slave_Pos = CSng(S)
            S = Kflop.Get_Info("SoftLimitNeg4")
            Y_Slave_Neg = CSng(S)

            S = Kflop.Get_Info("SoftLimitPos2")
            Z_Pos = CDbl(S)

        Else
            X_Pos = Kflop.Get_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.X_Plus_Soft_Limit)
            X_Neg = Kflop.Get_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.X_Minus_Soft_Limit)
            X_Slave_Pos = X_Pos
            X_Slave_Neg = X_Neg

            Y_Pos = Kflop.Get_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.Y_Plus_Soft_Limit)
            Y_Neg = Kflop.Get_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.Y_Minus_Soft_Limit)
            Y_Slave_Pos = Y_Pos
            Y_Slave_Neg = Y_Neg

            Z_Pos = Kflop.Get_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.Z_Plus_Soft_Limit)

        End If

    End Sub

    Public Sub Enable_Soft_Limits()
        Kflop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.X_Plus_Soft_Limit, Display.X_Pos_Soft_Limit)
        Kflop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.X_Minus_Soft_Limit, Display.X_Neg_Soft_Limit)
        Kflop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.Y_Plus_Soft_Limit, Display.Y_Pos_Soft_Limit)
        Kflop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.Y_Minus_Soft_Limit, Display.Y_Neg_Soft_Limit)
        Kflop.Set_User_Data_Float(Chip_C_Interface.enum_User_Data_Address.Z_Plus_Soft_Limit, Display.Z_Pos_Soft_Limit)

        Main_Form.ctl_C_Interface.Run_C_Program(Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe, Chip_C_Interface.enum_Commands.Enable_Soft_Limits)
        Main_Form.btn_Soft_Limit_Enable.State = "Off"

    End Sub

    Public Sub Disable_Soft_Limits()

        Main_Form.ctl_C_Interface.Run_C_Program(Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe, Chip_C_Interface.enum_Commands.Disable_Soft_Limits)
        Main_Form.btn_Soft_Limit_Enable.State = "On"
    End Sub

#End Region

#Region "CoordMotion"

    Private Delegate Sub DoTraverseDelegate(ByVal x As Double, ByVal y As Double, ByVal z As Double)
    Public Sub DoTraverse(ByVal x As Double, ByVal y As Double, ByVal z As Double)
        Dim runner As New DoTraverseDelegate(AddressOf OnDoTraverse)
        runner.BeginInvoke(x, y, z, Nothing, Nothing)
    End Sub
    Private Sub OnDoTraverse(ByVal x As Double, ByVal y As Double, ByVal z As Double)
        Controller.CoordMotion.StraightTraverse(x, y, z, 0, 0, 0, False)
    End Sub

    Private Delegate Sub DoLinearDelegate(ByVal x As Double, ByVal y As Double, ByVal z As Double)
    Public Sub DoLinear(ByVal x As Double, ByVal y As Double, ByVal z As Double)
        Dim runner As New DoLinearDelegate(AddressOf OnDoLinear)
        runner.BeginInvoke(x, y, z, Nothing, Nothing)
    End Sub

    Private Sub OnDoLinear(ByVal x As Double, ByVal y As Double, ByVal z As Double)
        Controller.CoordMotion.StraightFeed(Feed_Rate, x, y, z, 0, 0, 0, 0, 0)
    End Sub

    Private Delegate Sub DoArcDelegate(ByVal ex As Double, ByVal ey As Double, ByVal ez As Double, ByVal cx As Double, ByVal cy As Double)
    Public Sub DoArc(ByVal ex As Double, ByVal ey As Double, ByVal ez As Double, ByVal cx As Double, ByVal cy As Double)

        Dim runner As New DoArcDelegate(AddressOf OnDoArc)
        runner.BeginInvoke(ex, ey, ez, cx, cy, Nothing, Nothing)
    End Sub
    Public Sub OnDoArc(ByVal ex As Double, ByVal ey As Double, ByVal ez As Double, ByVal cx As Double, ByVal cy As Double)

        Dim plane As Integer = KMotion_dotNet.CANON_PLANE.CANON_PLANE_XY 'This should be a property in a real application 
        Dim DirIsCCW As Integer = 0 'This should be a property in a real application 
        Controller.CoordMotion.ArcFeed(Feed_Rate, plane, ex, ey, cx, cy, DirIsCCW, ez, 0, 0, 0, 0, 0)
    End Sub

    Private Delegate Sub DoSyncPositionsDelegate()
    Public Sub DoSyncPositions()
        Dim runner As New DoSyncPositionsDelegate(AddressOf OnDoSyncPositions)
        runner.BeginInvoke(Nothing, Nothing)
    End Sub
    Public Sub OnDoSyncPositions()
        Dim x As Double
        Dim y As Double
        Dim z As Double
        Dim a As Double
        Dim b As Double
        Dim c As Double
        Controller.CoordMotion.ReadAndSyncCurPositions(x, y, z, a, b, c)
    End Sub

    Private Delegate Sub DoFlushExecuteDelegate()
    Public Sub DoFlushExecute()
        Dim runner As New DoFlushExecuteDelegate(AddressOf OnDoFlushExecute)
        runner.BeginInvoke(Nothing, Nothing)
    End Sub
    Public Sub OnDoFlushExecute()
        Controller.CoordMotion.FlushSegments()
    End Sub

#End Region

#Region "Interpreter"

    Public Sub Initialize_Interpreter()
        Controller.CoordMotion.Interpreter.InitializeInterpreter()
        Controller.CoordMotion.Interpreter.SetupParams.LengthUnits = Length_Units
    End Sub

    Private Sub Interpreter_Status_Updated(ByVal lineno As Integer, ByVal msg As String)
        MessageMutex.WaitOne()

        Current_Sent_Line = lineno
        GCode_Program_Executing = True

        msg = msg.Replace(vbCr & vbCrLf, "")
        Dim S() As String = msg.Split(vbCrLf)
        For I = 0 To S.Length - 1
            Trace_Control.Log_Trace_Message(String.Format("[Interp]  Line = {0} Data = {1} Block {2}",
                                            lineno, msg, Controller.CoordMotion.Interpreter.SetupParams.CurrentLine) & vbCrLf)
        Next
        MessageMutex.ReleaseMutex()

    End Sub

    Private Sub Interpreter_Complete(ByVal Status As Integer, ByVal Line_Number As Integer, ByVal Sequence_Number As Integer, ByVal Error_Message As String)

        MessageMutex.WaitOne()

        Trace_Control.Log_Trace_Message(String.Format("[Interp] Complete : Line = {0} Status = {1} Sequence = {2}", Line_Number, Status, Sequence_Number) & vbCrLf)

        If Error_Message <> "" Then
            If Status <> 1005 Then
                Trace_Control.Log_Trace_Message("[Interp] **** ERROR : " & vbCrLf)

                Show_Error("Interpreter Error : " & Error_Message & vbCrLf &
                       "Line Number : " & Line_Number & vbCrLf &
                       "Sequence Number : " & Sequence_Number & vbCrLf &
                       "Status Number : " & Status)
            End If
        End If

        Controller.CoordMotion.ClearAbort()
        Controller.CoordMotion.ClearHalt()

        GCode_Program_Done = True
        GCode_Program_Executing = False

        If Run_Mode = enum_Run_Mode.Continuous Then
            Current_Sent_Line = 0
        Else
            GCode_Single_Step = True
        End If

        Interpreter_Lock = False

        Set_Coordinate_System(enum_Coorinate_System.G54)

        MessageMutex.ReleaseMutex()

    End Sub

    Private _TraverseSpeed As Double
    Public Property TraverseSpeed() As Double
        Get
            Return _TraverseSpeed
        End Get
        Set(ByVal value As Double)
            _TraverseSpeed = value
        End Set
    End Property

    Public ReadOnly Property RealtimeLine() As Integer
        Get
            Return Controller.CoordMotion.Interpreter.SetupParams.CurrentLine
        End Get
    End Property

    Private _SimulateInterpreter As Boolean
    Public Property SimulateInterpreter() As Boolean
        Get
            Return _SimulateInterpreter
        End Get
        Set(ByVal value As Boolean)
            _SimulateInterpreter = value
            SetSimulation()
        End Set
    End Property

    Private _Interpreter_File_Name As String = ""
    Public Property Interpreter_File_Name() As String
        Get
            Return _Interpreter_File_Name
        End Get
        Set(ByVal value As String)
            _Interpreter_File_Name = value
        End Set
    End Property

    Public Sub Run_GCode(File_Name As String, Optional Start_Line As Integer = 0, Optional End_Line As Integer = Integer.MaxValue)
        'If Interpreter_Lock Then Exit Sub
        Interpreter_Lock = True

        'Check if we were previously stopped (halted), if so for this example just clear the condition
        If (Controller.CoordMotion.IsPreviouslyStopped <> KMotion_dotNet.PREV_STOP_TYPE.Prev_Stopped_None) Then
            Controller.CoordMotion.IsPreviouslyStopped = KMotion_dotNet.PREV_STOP_TYPE.Prev_Stopped_None
        End If


        If System.IO.File.Exists(File_Name) Then

            Controller.CoordMotion.Abort()
            Controller.CoordMotion.ClearAbort()
            Controller.CoordMotion.Interpreter.Abort()

            Set_Coordinate_System(enum_Coorinate_System.G55)

            'Do not remove, interpreter chokes without this delay
            Wait.Delay_Seconds(0.25) ' Might need to be longer eg. 1 second

            If Run_Mode = enum_Run_Mode.Single_Step Then
                Dim L As Integer
                If Start_Here_Line >= 0 Then
                    L = Start_Here_Line
                Else
                    L = Controller.CoordMotion.Interpreter.SetupParams.CurrentLine
                End If
                Show_Error("Single step mode out for test")
                'Controller.CoordMotion.Interpreter.Interpret(File_Name, L, L, 0)
            Else
                If Start_Here_Line >= 0 Then
                    Start_Line = Start_Here_Line
                End If

                If Start_Line = 0 And End_Line = Integer.MaxValue Then

                    Dim S_X, S_Y, S_Z, A_X, A_Y, A_Z As String
                    S_X = Save_X
                    S_Y = Save_Y
                    S_Z = Save_Z
                    A_X = Math.Round(KFlop.X_Abs, 4)
                    A_Y = Math.Round(KFlop.Y_Abs, 4)
                    A_Z = Math.Round(KFlop.Z_Abs, 4)


                    'If (S_X <> A_X) Or (S_Y <> A_Y) Or (S_Z <> A_Z) Then
                    '    Show_Error("Absolute positions do not match." & vbCrLf & vbCrLf &
                    '               "X : " & S_X & "   " & A_X & vbCrLf &
                    '               "Y : " & S_Y & "   " & A_Y & vbCrLf &
                    '               "Z : " & S_Z & "   " & A_Z)
                    'End If


                    If Main_Form.chk_Test.Checked Then
                        Dim dlg As New dlg_Message_Box
                        dlg.ShowDialog("Kill KFLOP programs?", "", MessageBoxButtons.YesNo)
                        If dlg.DialogResult = DialogResult.Yes Then
                            Dim T(5) As Integer
                            For I = 0 To 4
                                T(I) = I + 1
                            Next
                            Controller.KillProgramThreads(T)
                        End If

                        dlg.ShowDialog("Run interpreter?", "", MessageBoxButtons.YesNo)
                        If dlg.DialogResult = DialogResult.Yes Then
                            Controller.CoordMotion.Interpreter.Interpret(File_Name)
                            dlg.ShowDialog("Continue", "", MessageBoxButtons.YesNo)
                        End If

                    Else
                            Controller.CoordMotion.Interpreter.Interpret(File_Name)
                        End If

                    Else
                        Show_Error("Start at line mode out for test")
                    'Controller.CoordMotion.Interpreter.Interpret(File_Name, Start_Line, End_Line, 0)
                End If

            End If

            Start_Here_Line = -1

        End If

    End Sub

    Public Sub Rewind()
        Trace_Control.Clear_Trace_Log()

        'Check if we were previously stopped (halted), if so for this example just clear the condition
        If (Controller.CoordMotion.IsPreviouslyStopped <> KMotion_dotNet.PREV_STOP_TYPE.Prev_Stopped_None) Then
            Controller.CoordMotion.IsPreviouslyStopped = KMotion_dotNet.PREV_STOP_TYPE.Prev_Stopped_None
        End If

        If System.IO.File.Exists(Interpreter_File_Name) Then
            Controller.CoordMotion.ClearAbort()
            Controller.CoordMotion.Interpreter.Interpret(Interpreter_File_Name, 0, -1, -1)
        End If

    End Sub

    Private Sub SetSimulation()
        Controller.CoordMotion.IsSimulation = _SimulateInterpreter
    End Sub

#End Region

End Class
