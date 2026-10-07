

Public Class Chip_Trace

    Private Trace_Messages As New List(Of String)
    Private New_Trace_Messages As New List(Of String)

    Private Trace_Log_Count As Integer = 0

    Private Trace_Sync_Lock As New Object
    Private Display_Sync_Lock As New Object
    Private Filter_Sync_Lock As New Object
    Private Initialized As Boolean = False

    Private _Trace_Log_Limit As Integer = 50
    Public Property Trace_Log_Limit As Integer
        Get
            Return _Trace_Log_Limit
        End Get
        Set(ByVal value As Integer)
            _Trace_Log_Limit = value
            txt_Trace_Log_Limit.Text = value
        End Set
    End Property

    Public Sub Initialize()
        Trace_Log_Limit = 50
        Initialized = True
    End Sub

    Private Sub Limit_Lines(ByRef txt_Box As TextBox, Limit As Integer)
        If txt_Box.Lines.Count > Limit Then
            Dim newList As List(Of String) = txt_Box.Lines.ToList
            newList.RemoveRange(0, txt_Box.Lines.Count - Limit)
            txt_Box.Lines = newList.ToArray
        End If
    End Sub


    Public Sub Update_Trace()

        SyncLock Display_Sync_Lock

            If New_Trace_Messages.Count > 0 Then

                For I = 0 To New_Trace_Messages.Count - 1
                    Display_Trace_Line(New_Trace_Messages(I))
                Next
                New_Trace_Messages.Clear()

                Limit_Lines(txt_Trace_Messages, Trace_Log_Limit)

                While Trace_Messages.Count > Trace_Log_Limit
                    Trace_Messages.RemoveAt(0)
                End While

                txt_Trace_Messages.Focus()
                txt_Trace_Messages.Select(txt_Trace_Messages.TextLength, 1)
                txt_Trace_Messages.ScrollToCaret()
                txt_Trace_Messages.Refresh()

            End If

        End SyncLock

    End Sub

    Public Sub Log_Trace_Message(Message As String)

        If chk_Enable_Tracing.Checked Then
            SyncLock Trace_Sync_Lock

                If Trace_Messages.Count > Trace_Log_Limit Then
                    Trace_Messages.RemoveAt(0)
                End If

                Trace_Log_Count += 1

                Trace_Messages.Add(Trace_Log_Count.ToString.PadLeft(6) & "  " & Message)
                New_Trace_Messages.Add(Trace_Log_Count.ToString.PadLeft(6) & "  " & Message)

            End SyncLock
        End If

    End Sub

    Public Sub Clear_Trace_Log()
        Trace_Log_Count = 0
        Trace_Messages.Clear()
        New_Trace_Messages.Clear()
        txt_Trace_Messages.Text = ""
    End Sub

    Private Sub btn_Display_Console_Message_Click(sender As Object, e As EventArgs) Handles _
                   chk_Trace_Motion.CheckedChanged, chk_Trace_Interpreter.CheckedChanged, chk_Trace_Console.CheckedChanged,
                   chk_Trace_Block.CheckedChanged, chk_Trace_Handler.CheckedChanged
        If Not Initialized Then Exit Sub
        SyncLock Filter_Sync_Lock
            txt_Trace_Messages.Text = ""
            For I = 0 To Trace_Messages.Count - 1
                Display_Trace_Line(Trace_Messages(I))
            Next
        End SyncLock
    End Sub

    Private Overloads Sub Display_Trace_Line(Message As String)
        Dim Idx As Integer = Message.IndexOf("[")

        Select Case Mid(Message, Idx + 2, 1)
            Case "I"
                If chk_Trace_Interpreter.Checked Then
                    txt_Trace_Messages.Text &= Message
                End If
            Case "M"
                If chk_Trace_Motion.Checked Then
                    txt_Trace_Messages.Text &= Message
                End If
            Case "C"
                If chk_Trace_Console.Checked Then
                    txt_Trace_Messages.Text &= Message
                End If
            Case "B"
                If chk_Trace_Block.Checked Then
                    txt_Trace_Messages.Text &= Message
                End If
            Case "H"
                If chk_Trace_Handler.Checked Then
                    txt_Trace_Messages.Text &= Message
                End If
        End Select

    End Sub

    Private Sub txt_Trace_Log_Count_KeyDown(sender As Object, e As System.Windows.Forms.KeyEventArgs) Handles txt_Trace_Log_Limit.KeyDown
        If e.KeyCode = Keys.Enter Then btn_Clear_Console_Messages.Focus()
    End Sub

    Private Sub txt_Trace_Log_Count_Validated(sender As Object, e As EventArgs) Handles txt_Trace_Log_Limit.Validated
        Trace_Log_Limit = txt_Trace_Log_Limit.Text
    End Sub

    Private Sub btn_Clear_Console_Messages_Click(sender As Object, e As EventArgs) Handles btn_Clear_Console_Messages.Click
        Clear_Trace_Log()
    End Sub

    Private Sub chk_Enable_Tracing_CheckedChanged(sender As Object, e As EventArgs) Handles chk_Enable_Tracing.CheckedChanged

    End Sub
End Class
