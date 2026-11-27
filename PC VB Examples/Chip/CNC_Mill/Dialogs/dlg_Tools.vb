Imports System.IO

Public Class dlg_Tools

    Public Tool_Name As String = ""
    Public Seat_Z As String = ""
    Public Measure_Z As String = ""
    Public Measure_X As String = ""
    Public Measure_Y As String = ""

    Public Sub Add_Tool(Tool_Info As List(Of String))
        Dim S() As String
        S = Tool_Info.ToArray
        grd_Tools.Rows.Add(S)
    End Sub

    Private Sub btn_OK_Click(sender As Object, e As EventArgs) Handles btn_OK.Click
        Finish()
    End Sub

    Private Sub btn_Cancel_Click(sender As Object, e As EventArgs) Handles btn_Cancel.Click
        DialogResult = DialogResult.Cancel
        Close()
    End Sub

    Private Sub grd_Tools_Cell_Double_Click(sender As Object, e As System.Windows.Forms.DataGridViewCellEventArgs) Handles grd_Tools.CellDoubleClick
        Finish()
    End Sub

    Private Sub dlg_Key_Down(sender As Object, e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyDown
        Select Case e.KeyCode
            Case Keys.Enter
                Finish()
            Case Keys.Escape
                DialogResult = DialogResult.Cancel
                Close()
        End Select
    End Sub

    Private Sub Finish()
        Tool_Name = grd_Tools.CurrentRow.Cells(0).Value
        Seat_Z = grd_Tools.CurrentRow.Cells(1).Value
        Measure_Z = grd_Tools.CurrentRow.Cells(2).Value
        Measure_X = grd_Tools.CurrentRow.Cells(3).Value
        Measure_Y = grd_Tools.CurrentRow.Cells(4).Value

        DialogResult = DialogResult.OK
        Close()
    End Sub


End Class