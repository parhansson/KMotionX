

Public Class DynoMotionVBnetProvider

    Private _Controller As New KMotion_dotNet.KM_Controller()

    Public MessageMutex As New Threading.Mutex()

    Private JogSpeed As Double = 1000



    Private Sub WriteLineException(ByVal s As String)
        Try
            _Controller.WriteLine(s)
        Catch ex As KMotion_dotNet.DMException
            MessageBox.Show(ex.InnerException.Message(), "Dynomotion")
        End Try
    End Sub




#Region "Data Members"

    Private _XPosition As Double
    Public Property XPosition() As Double
        Get
            Return _XPosition
        End Get
        Set(ByVal value As Double)
            _XPosition = value
        End Set
    End Property

    Private _YPosition As Double
    Public Property YPosition() As Double
        Get
            Return _YPosition
        End Get
        Set(ByVal value As Double)
            _YPosition = value
        End Set
    End Property

    Private _ZPosition As Double
    Public Property ZPosition() As Double
        Get
            Return _ZPosition
        End Get
        Set(ByVal value As Double)
            _ZPosition = value
        End Set
    End Property





    Private _XCommanded As Double
    Public Property XCommanded() As Double
        Get
            Return _XCommanded
        End Get
        Set(ByVal value As Double)
            _XCommanded = value
        End Set
    End Property

    Private _YCommanded As Double
    Public Property YCommanded() As Double
        Get
            Return _YCommanded
        End Get
        Set(ByVal value As Double)
            _YCommanded = value
        End Set
    End Property

    Private _ZCommanded As Double
    Public Property ZCommanded() As Double
        Get
            Return _ZCommanded
        End Get
        Set(ByVal value As Double)
            _ZCommanded = value
        End Set
    End Property





    Private _XEnabled As Boolean
    Public Property XEnabled() As Boolean
        Get
            Return _XEnabled
        End Get
        Set(ByVal value As Boolean)
            _XEnabled = value
            SetXEnabled(value)
        End Set
    End Property

    Private _YEnabled As Boolean
    Public Property YEnabled() As Boolean
        Get
            Return _YEnabled
        End Get
        Set(ByVal value As Boolean)
            _YEnabled = value
            SetYEnabled(value)
        End Set
    End Property

    Private _ZEnabled As Boolean
    Public Property ZEnabled() As Boolean
        Get
            Return _ZEnabled
        End Get
        Set(ByVal value As Boolean)
            _ZEnabled = value
            SetZEnabled(value)
        End Set
    End Property



    Private _XJogValue As Double
    Public Property XJogValue() As Double
        Get
            Return _XJogValue
        End Get
        Set(ByVal value As Double)
            _XJogValue = value
            JogX()
        End Set
    End Property

    Private _YJogValue As Double
    Public Property YJogValue() As Double
        Get
            Return _YJogValue
        End Get
        Set(ByVal value As Double)
            _YJogValue = value
            JogY()
        End Set
    End Property

    Private _ZJogValue As Double
    Public Property ZJogValue() As Double
        Get
            Return _ZJogValue
        End Get
        Set(ByVal value As Double)
            _ZJogValue = value
            JogZ()
        End Set
    End Property





    Private _LED_0_On As Boolean
    Public Property LED_0_On() As Boolean
        Get
            Return _LED_0_On
        End Get
        Set(ByVal value As Boolean)
            _LED_0_On = value
            SetLED_0(value)
        End Set
    End Property

    Private _LED_1_On As Boolean
    Public Property LED_1_On() As Boolean
        Get
            Return _LED_1_On
        End Get
        Set(ByVal value As Boolean)
            _LED_1_On = value
            SetLED_1(value)
        End Set
    End Property






















    Private _FeedRate As Double = 50
    Public Property FeedRate() As Double
        Get
            Return _FeedRate
        End Get
        Set(ByVal value As Double)
            _FeedRate = value
        End Set
    End Property

    Private _TraverseSpeed As Double
    Public Property TraverseSpeed() As Double
        Get
            Return _TraverseSpeed
        End Get
        Set(ByVal value As Double)
            _TraverseSpeed = value
        End Set
    End Property
 
    Private _CoordMotionStatus As String = ""
    Public Property CoordMotionStatus() As String
        Get
            Return _CoordMotionStatus
        End Get
        Set(ByVal value As String)
            _CoordMotionStatus = value
        End Set
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

    Private _InterpreterFileName As String = ""
    Public Property InterpreterFileName() As String
        Get
            Return _InterpreterFileName
        End Get
        Set(ByVal value As String)
            _InterpreterFileName = value
        End Set
    End Property


    Private _InterpreterStatus As String = ""
    Public Property InterpreterStatus() As String
        Get
            Return _InterpreterStatus
        End Get
        Set(ByVal value As String)
            _InterpreterStatus = value
        End Set
    End Property
#End Region



    Public Sub New()

        ' check if the board is available
        If (_Controller.KMotionLock() = KMotion_dotNet.KMOTION_TOKEN.KMOTION_LOCKED) Then
            _Controller.ReleaseToken()

            Try

                _Controller.WriteLine(String.Format("Accel{0}={1}", 0, 500))
                _Controller.WriteLine(String.Format("Accel{0}={1}", 1, 500))
                _Controller.WriteLine(String.Format("Accel{0}={1}", 2, 500))

                _Controller.WriteLine(String.Format("Vel{0}={1}", 0, 100))
                _Controller.WriteLine(String.Format("Vel{0}={1}", 1, 100))
                _Controller.WriteLine(String.Format("Vel{0}={1}", 2, 50))

                _Controller.WriteLine(String.Format("Jerk{0}={1}", 0, 5000))
                _Controller.WriteLine(String.Format("Jerk{0}={1}", 1, 5000))
                _Controller.WriteLine(String.Format("Jerk{0}={1}", 2, 5000))

                '  Coordinate System
                _Controller.WriteLine(String.Format("DefineCS ={0} {1} {2} {3} {4} {5}", 0, 1, 2, -1, -1, -1))

            Catch ex As KMotion_dotNet.DMException
                MessageBox.Show(ex.InnerException.Message(), "Dynomotion")
            End Try

        End If

        '  CoordMotion Params
        _Controller.CoordMotion.MotionParams.BreakAngle = 40
        _Controller.CoordMotion.MotionParams.CollinearTolerance = 0.001
        _Controller.CoordMotion.MotionParams.CornerTolerance = 0.001
        _Controller.CoordMotion.MotionParams.FacetAngle = 1.0
        _Controller.CoordMotion.MotionParams.MaxLinearLength = 100
        _Controller.CoordMotion.MotionParams.TPLookahead = 10.0

        _Controller.CoordMotion.MotionParams.MaxAccelX = 500
        _Controller.CoordMotion.MotionParams.MaxAccelY = 500
        _Controller.CoordMotion.MotionParams.MaxAccelZ = 500

        _Controller.CoordMotion.MotionParams.MaxVelX = 100
        _Controller.CoordMotion.MotionParams.MaxVelY = 100
        _Controller.CoordMotion.MotionParams.MaxVelZ = 100

        _Controller.CoordMotion.MotionParams.CountsPerInchX = 1000
        _Controller.CoordMotion.MotionParams.CountsPerInchY = 1000
        _Controller.CoordMotion.MotionParams.CountsPerInchZ = 1000
        AddHandler _Controller.CoordMotion.CoordMotionStraightTraverse, AddressOf OnCoordMotionStraightTranverse
        AddHandler _Controller.CoordMotion.CoordMotionStraightFeed, AddressOf OnCoordMotionStraightFeed
        AddHandler _Controller.CoordMotion.CoordMotionArcFeed, AddressOf OnCoordMotionArcFeed


        '   Interpreter Params
        AddHandler _Controller.CoordMotion.Interpreter.InterpreterStatusUpdated, AddressOf InterpreterStatusUpdated
        AddHandler _Controller.CoordMotion.Interpreter.InterpreterCompleted, AddressOf InterpreterComplete
    End Sub


 
#Region "Jogging"
    Private Sub JogX()
        If _XEnabled Then
            WriteLineException(String.Format("Jog{0}={1}", 0, XJogValue * JogSpeed))
        End If
    End Sub
    Private Sub JogY()
        If _YEnabled Then
            WriteLineException(String.Format("Jog{0}={1}", 1, YJogValue * JogSpeed))
        End If
    End Sub
    Private Sub JogZ()
        If _ZEnabled Then
            WriteLineException(String.Format("Jog{0}={1}", 2, ZJogValue * JogSpeed))
        End If
    End Sub
#End Region

#Region "Moving"
    Public Sub MoveX()
        If _XEnabled Then
            WriteLineException(String.Format("Move{0}={1}", 0, XCommanded))
        End If
    End Sub
    Public Sub MoveY()
        If _YEnabled Then
            WriteLineException(String.Format("Move{0}={1}", 1, YCommanded))
        End If
    End Sub
    Public Sub MoveZ()
        If _ZEnabled Then
            WriteLineException(String.Format("Move{0}={1}", 2, ZCommanded))
        End If
    End Sub
#End Region

#Region "Enabling"
    Private Sub SetXEnabled(ByVal value As Boolean)
        If value = True Then
            WriteLineException("EnableAxis0")
        Else
            WriteLineException("DisableAxis0")
        End If
    End Sub

    Private Sub SetYEnabled(ByVal value As Boolean)
        If value = True Then
            WriteLineException("EnableAxis1")
        Else
            WriteLineException("DisableAxis1")
        End If
    End Sub

    Private Sub SetZEnabled(ByVal value As Boolean)
        If value = True Then
            WriteLineException("EnableAxis2")
        Else
            WriteLineException("DisableAxis2")
        End If
    End Sub
#End Region

#Region "IO "

    Private Sub SetLED_0(ByVal value As Boolean)
        If value = True Then
            WriteLineException("SetBit46")
        Else
            WriteLineException("ClearBit46")
        End If
    End Sub

    Private Sub SetLED_1(ByVal value As Boolean)
        If value = True Then
            WriteLineException("SetBit47")
        Else
            WriteLineException("ClearBit47")
        End If
    End Sub
#End Region

#Region "CoordMotion"
    Private Delegate Sub DoTraverseDelegate(ByVal x As Double, ByVal y As Double, ByVal z As Double)
    Public Sub DoTraverse(ByVal x As Double, ByVal y As Double, ByVal z As Double)
        Dim runner As New DoTraverseDelegate(AddressOf OnDoTraverse)
        runner.BeginInvoke(x, y, z, Nothing, Nothing)
    End Sub
    Private Sub OnDoTraverse(ByVal x As Double, ByVal y As Double, ByVal z As Double)
        _Controller.CoordMotion.StraightTraverse(x, y, z, 0, 0, 0, False)
    End Sub

    Private Delegate Sub DoLinearDelegate(ByVal x As Double, ByVal y As Double, ByVal z As Double)
    Public Sub DoLinear(ByVal x As Double, ByVal y As Double, ByVal z As Double)
        Dim runner As New DoLinearDelegate(AddressOf OnDoLinear)
        runner.BeginInvoke(x, y, z, Nothing, Nothing)
    End Sub
    Private Sub OnDoLinear(ByVal x As Double, ByVal y As Double, ByVal z As Double)
        _Controller.CoordMotion.StraightFeed(_FeedRate, x, y, z, 0, 0, 0, 0, 0)
    End Sub

    Private Delegate Sub DoArcDelegate(ByVal ex As Double, ByVal ey As Double, ByVal ez As Double, ByVal cx As Double, ByVal cy As Double)
    Public Sub DoArc(ByVal ex As Double, ByVal ey As Double, ByVal ez As Double, ByVal cx As Double, ByVal cy As Double)

        Dim runner As New DoArcDelegate(AddressOf OnDoArc)
        runner.BeginInvoke(ex, ey, ez, cx, cy, Nothing, Nothing)
    End Sub
    Public Sub OnDoArc(ByVal ex As Double, ByVal ey As Double, ByVal ez As Double, ByVal cx As Double, ByVal cy As Double)

        Dim plane As Integer = KMotion_dotNet.CANON_PLANE.CANON_PLANE_XY 'This should be a property in a real application 
        Dim DirIsCCW As Integer = 0 'This should be a property in a real application 
        _Controller.CoordMotion.ArcFeed(_FeedRate, plane, ex, ey, cx, cy, DirIsCCW, ez, 0, 0, 0, 0, 0)
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
        _Controller.CoordMotion.UpdateCurrentPositionsABS(x, y, z, a, b, c, False)
    End Sub
    Private Delegate Sub DoFlushExecuteDelegate()
    Public Sub DoFlushExecute()

        Dim runner As New DoFlushExecuteDelegate(AddressOf OnDoFlushExecute)
        runner.BeginInvoke(Nothing, Nothing)
    End Sub
    Public Sub OnDoFlushExecute()
        _Controller.CoordMotion.FlushSegments()
    End Sub
    Public Sub OnCoordMotionStraightTranverse(ByVal x As Double, ByVal y As Double, ByVal z As Double, ByVal sequence_number As Integer)
        MessageMutex.WaitOne()
        _CoordMotionStatus &= vbCrLf
        _CoordMotionStatus &= String.Format("[Straight Traverse] X={0}  Y={1}  Z={2}", x, y, z)
        MessageMutex.ReleaseMutex()
    End Sub

    Public Sub OnCoordMotionStraightFeed(ByVal DesiredFeedRate_in_per_sec As Double, ByVal x As Double, ByVal y As Double, ByVal z As Double, ByVal sequence_number As Integer, ByVal ID As Integer)
        MessageMutex.WaitOne()
        _CoordMotionStatus &= vbCrLf
        _CoordMotionStatus &= String.Format("[Straight Feed] X={0}  Y={1}  Z={2}", x, y, z)
        MessageMutex.ReleaseMutex()
    End Sub

    Public Sub OnCoordMotionArcFeed(ByVal ZeroLenAsFullCircles As Boolean, ByVal DesiredFeedRate_in_per_sec As Double, ByVal plane As Integer, ByVal first_end As Double, ByVal second_end As Double, ByVal first_axis As Double, ByVal second_axis As Double, ByVal rotation As Integer, ByVal axis_end_point As Double, ByVal first_start As Double, ByVal second_start As Double, ByVal axis_start_point As Double, ByVal sequence_number As Integer, ByVal ID As Integer)
        MessageMutex.WaitOne()
        _CoordMotionStatus &= vbCrLf
        _CoordMotionStatus &= String.Format("[Arc Feed] EX={0}  EY={1}  EZ={2}", first_end, second_end, axis_end_point)
        MessageMutex.ReleaseMutex()
    End Sub
#End Region

#Region "Interpreter"
    Public Sub Run()
        _InterpreterStatus = ""

        'Check if we were previously stopped (halted), if so for this example just clear the condition
        If (_Controller.CoordMotion.IsPreviouslyStopped <> KMotion_dotNet.PREV_STOP_TYPE.Prev_Stopped_None) Then
            _Controller.CoordMotion.IsPreviouslyStopped = KMotion_dotNet.PREV_STOP_TYPE.Prev_Stopped_None
        End If

        If System.IO.File.Exists(_InterpreterFileName) = True Then
            _Controller.CoordMotion.Interpreter.Interpret(_InterpreterFileName)
        End If
    End Sub

    Public Sub Pause()
        Try
            Dim response As String = _Controller.WriteLineReadLine("GetStopState")

            If (response = "0") Then
                _Controller.WriteLine("StopImmediate0")
            Else
                _Controller.WriteLine("StopImmediate1")
            End If

        Catch ex As KMotion_dotNet.DMException
            MessageBox.Show(ex.InnerException.Message(), "Dynomotion")
        End Try

    End Sub

    Public Sub Halt()
        _Controller.CoordMotion.Interpreter.Halt()
    End Sub

    Private Sub SetSimulation()
        _Controller.CoordMotion.IsSimulation = _SimulateInterpreter
    End Sub

    Private Sub InterpreterStatusUpdated(ByVal lineno As Integer, ByVal msg As String)
        MessageMutex.WaitOne()
        _InterpreterStatus &= String.Format("[Interp Stat]  Line = {0} Data = {1}", lineno, msg)
        MessageMutex.ReleaseMutex()
    End Sub

    Private Sub InterpreterComplete(ByVal status As Integer, ByVal lineno As Integer, ByVal sequence_number As Integer, ByVal er As String)
        MessageMutex.WaitOne()
        _InterpreterStatus &= String.Format("[Interp Complete]  Line = {0} Status = {1} Sequence = {2} Error = {3}", lineno, status, sequence_number, er)
        _InterpreterStatus &= vbCrLf
        MessageMutex.ReleaseMutex()
        _Controller.CoordMotion.ClearAbort()
        _Controller.CoordMotion.ClearHalt()
    End Sub
#End Region



    Public Sub StopAll()
        Try
            _Controller.WriteLine("DisableAxis0")
            _Controller.WriteLine("DisableAxis1")
            _Controller.WriteLine("DisableAxis2")
        Catch ex As KMotion_dotNet.DMException
            MessageBox.Show(ex.InnerException.Message(), "Dynomotion")
        End Try
    End Sub

    Public Sub UpdateVaules()

        ' check if the board is available
        If (_Controller.WaitToken(100) = KMotion_dotNet.KMOTION_TOKEN.KMOTION_LOCKED) Then
            _Controller.ReleaseToken()

            Try
                _XPosition = Convert.ToDouble(_Controller.WriteLineReadLine("Dest0") / _Controller.CoordMotion.MotionParams.CountsPerInchX)
                _YPosition = Convert.ToDouble(_Controller.WriteLineReadLine("Dest1") / _Controller.CoordMotion.MotionParams.CountsPerInchY)
                _ZPosition = Convert.ToDouble(_Controller.WriteLineReadLine("Dest2") / _Controller.CoordMotion.MotionParams.CountsPerInchZ)

                _XEnabled = Convert.ToDouble(_Controller.WriteLineReadLine("Enabled0"))
                _YEnabled = Convert.ToDouble(_Controller.WriteLineReadLine("Enabled1"))
                _ZEnabled = Convert.ToDouble(_Controller.WriteLineReadLine("Enabled2"))

                _LED_0_On = Convert.ToDouble(_Controller.WriteLineReadLine("ReadBit46"))
                _LED_1_On = Convert.ToDouble(_Controller.WriteLineReadLine("ReadBit47"))
            Catch ex As KMotion_dotNet.DMException
                MessageBox.Show(ex.InnerException.Message(), "Dynomotion")
            End Try


            _SimulateInterpreter = _Controller.CoordMotion.IsSimulation
        End If

    End Sub
End Class
