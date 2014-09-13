Public Class Form1
    Dim device As New DynoMotionVBnetProvider
    Dim updating As Boolean = False

    Private Sub Form1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Timer1.Enabled = True
    End Sub

    Private Sub Timer1_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Timer1.Tick
 
        updating = True

 
        device.UpdateVaules()
        txtXAxisPos.Text = FormatNumber(device.XPosition, 3)
        txtYAxisPos.Text = FormatNumber(device.YPosition, 3)
        txtZAxisPos.Text = FormatNumber(device.ZPosition, 3)

        chkEnableX.Checked = device.XEnabled
        chkEnableY.Checked = device.YEnabled
        chkEnableZ.Checked = device.ZEnabled

        chkToggleLED_0.Checked = device.LED_0_On
        chkToggleLED_1.Checked = device.LED_1_On

        chkSimInterpreter.Checked = device.SimulateInterpreter

        device.MessageMutex.WaitOne() ' be thread safe as callbacks are from diff threads

        If device.InterpreterStatus <> "" Then
            lblInterpreterFeedBack.Text &= device.InterpreterStatus
            Dim pos As Integer = lblInterpreterFeedBack.Text.LastIndexOf(vbCrLf)
            lblInterpreterFeedBack.Select(pos + 2, 0)
            lblInterpreterFeedBack.ScrollToCaret()
            device.InterpreterStatus = ""
        End If

        If device.CoordMotionStatus <> "" Then
            lblCoordStatus.Text &= device.CoordMotionStatus
            Dim pos As Integer = lblCoordStatus.Text.LastIndexOf(vbCrLf)
            lblCoordStatus.Select(pos + 2, 0)
            lblCoordStatus.ScrollToCaret()
            device.CoordMotionStatus = ""
        End If

        device.MessageMutex.ReleaseMutex()

        updating = False

    End Sub



    'Jogging  
    Private Sub sldJogX_Scroll(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles sldJogX.Scroll
        If updating = False Then
            device.XJogValue = sldJogX.Value / 100.0
            lblXJogPercent.Text = String.Format("{0} %", sldJogX.Value) 
        End If 
    End Sub

    Private Sub sldJogY_Scroll(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles sldJogY.Scroll
        If updating = False Then
            device.YJogValue = sldJogY.Value / 100.0
            lblYJogPercent.Text = String.Format("{0} %", sldJogY.Value)
        End If
    End Sub

    Private Sub sldJogZ_Scroll(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles sldJogZ.Scroll
        If updating = False Then
            device.ZJogValue = sldJogZ.Value / 100.0
            lblZJogPercent.Text = String.Format("{0} %", sldJogZ.Value)
        End If
    End Sub

    Private Sub sldJogX_MouseUp(ByVal sender As System.Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles sldJogX.MouseUp
        If updating = False Then
            sldJogX.Value = 0
            device.XJogValue = 0.0
            lblXJogPercent.Text = String.Format("{0} %", sldJogX.Value)
        End If
    End Sub

    Private Sub sldJogY_MouseUp(ByVal sender As System.Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles sldJogY.MouseUp
        If updating = False Then
            sldJogY.Value = 0
            device.YJogValue = 0.0
            lblYJogPercent.Text = String.Format("{0} %", sldJogY.Value)
        End If
    End Sub

    Private Sub sldJogZ_MouseUp(ByVal sender As System.Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles sldJogZ.MouseUp
        If updating = False Then
            sldJogZ.Value = 0
            device.ZJogValue = 0.0
            lblZJogPercent.Text = String.Format("{0} %", sldJogZ.Value)
        End If
    End Sub



    'Manual Positioning
    Private Sub btnStopAll_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnStopAll.Click
        device.StopAll()
    End Sub

    Private Sub chkEnableX_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkEnableX.CheckedChanged
        If updating = False Then
            device.XEnabled = chkEnableX.Checked
        End If
    End Sub

    Private Sub chkEnableY_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkEnableY.CheckedChanged
        If updating = False Then
            device.YEnabled = chkEnableY.Checked
        End If
    End Sub

    Private Sub chkEnableZ_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkEnableZ.CheckedChanged
        If updating = False Then
            device.ZEnabled = chkEnableZ.Checked
        End If
    End Sub

    Private Sub btnMoveX_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnMoveX.Click
        If updating = False Then
            device.MoveX()
        End If
    End Sub

    Private Sub btnMoveY_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnMoveY.Click
        If updating = False Then
            device.MoveY()
        End If
    End Sub

    Private Sub btnMoveZ_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnMoveZ.Click
        If updating = False Then
            device.MoveZ()
        End If
    End Sub

    Private Sub txtXAxisCommanded_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtXAxisCommanded.TextChanged
        If IsNumeric(txtXAxisCommanded.Text.ToString()) Then
            device.XCommanded = Convert.ToDouble(txtXAxisCommanded.Text)
        End If
    End Sub

    Private Sub txtYAxisCommanded_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtYAxisCommanded.TextChanged
        If IsNumeric(txtYAxisCommanded.Text.ToString()) Then
            device.YCommanded = Convert.ToDouble(txtYAxisCommanded.Text)
        End If
    End Sub

    Private Sub txtZAxisCommanded_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtZAxisCommanded.TextChanged
        If IsNumeric(txtZAxisCommanded.Text.ToString()) Then
            device.ZCommanded = Convert.ToDouble(txtZAxisCommanded.Text)
        End If
    End Sub



    'IO
    Private Sub chkToggleLED_0_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkToggleLED_0.CheckedChanged
        If updating = False Then
            device.LED_0_On = chkToggleLED_0.Checked
        End If
    End Sub

    Private Sub chkToggleLED_1_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkToggleLED_1.CheckedChanged
        If updating = False Then
            device.LED_1_On = chkToggleLED_1.Checked
        End If
    End Sub


 
    Private Sub chkSimInterpreter_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkSimInterpreter.CheckedChanged
        If updating = False Then
            device.SimulateInterpreter = chkSimInterpreter.Checked
        End If
    End Sub

    Private Sub btnRunInterpreter_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRunInterpreter.Click
        device.Run()
    End Sub

    Private Sub btnPauseInterpreter_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnPauseInterpreter.Click
        device.Pause()
    End Sub

    Private Sub btnHaltInterpreter_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnHaltInterpreter.Click
        device.Halt()
    End Sub

    Private Sub btnGetInterpreterFileName_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnGetInterpreterFileName.Click
        Dim dlg As New OpenFileDialog()

        If dlg.ShowDialog() = Windows.Forms.DialogResult.OK Then
            txtInterpreterFileName.Text = dlg.FileName
        End If
    End Sub

    Private Sub txtInterpreterFileName_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtInterpreterFileName.TextChanged
        device.InterpreterFileName = txtInterpreterFileName.Text
    End Sub

    Private Sub btnExecuteTraverse_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExecuteTraverse.Click
        Dim x As Double = 1
        Dim y As Double = 1
        Dim z As Double = 1

        If IsNumeric(txtTraverseX.Text.ToString()) Then
            x = Convert.ToDouble(txtTraverseX.Text)
        End If

        If IsNumeric(txtTraverseY.Text.ToString()) Then
            y = Convert.ToDouble(txtTraverseY.Text)
        End If

        If IsNumeric(txtTraverseZ.Text.ToString()) Then
            z = Convert.ToDouble(txtTraverseZ.Text)
        End If
        device.DoTraverse(x, y, z)
    End Sub

    Private Sub btnExecuteStraight_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExecuteStraight.Click
        Dim x As Double = 1
        Dim y As Double = 1
        Dim z As Double = 1

        If IsNumeric(txtStraightX.Text.ToString()) Then
            x = Convert.ToDouble(txtStraightX.Text)
        End If

        If IsNumeric(txtStraightY.Text.ToString()) Then
            y = Convert.ToDouble(txtStraightY.Text)
        End If

        If IsNumeric(txtStraightZ.Text.ToString()) Then
            z = Convert.ToDouble(txtStraightZ.Text)
        End If

        device.DoLinear(x, y, z)
    End Sub

    Private Sub btnExecuteArc_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExecuteArc.Click
        Dim ex As Double = 1
        Dim ey As Double = 1
        Dim ez As Double = 1

        Dim cx As Double = 3
        Dim cy As Double = 3

        If IsNumeric(txtArcEX.Text.ToString()) Then
            ex = Convert.ToDouble(txtArcEX.Text)
        End If

        If IsNumeric(txtArcEY.Text.ToString()) Then
            ey = Convert.ToDouble(txtArcEY.Text)
        End If

        If IsNumeric(txtArcEZ.Text.ToString()) Then
            ez = Convert.ToDouble(txtArcEZ.Text)
        End If

        If IsNumeric(txtArcCY.Text.ToString()) Then
            cx = Convert.ToDouble(txtArcCX.Text)
        End If

        If IsNumeric(txtArcCX.Text.ToString()) Then
            cy = Convert.ToDouble(txtArcCY.Text)
        End If

        device.DoArc(ex, ey, ez, cx, cy) 
    End Sub

    Private Sub btnClearCoordLog_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnClearCoordLog.Click

        device.CoordMotionStatus = ""
        lblCoordStatus.Text = ""

    End Sub

    
    Private Sub txtFeedRate_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtFeedRate.TextChanged
        If IsNumeric(txtFeedRate.Text.ToString()) Then
            device.FeedRate = Convert.ToDouble(txtFeedRate.Text)
        End If
    End Sub

    Private Sub SyncPositions_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles SyncPositions.Click
        device.DoSyncPositions()
    End Sub

    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click
        device.DoFlushExecute()
    End Sub
End Class
