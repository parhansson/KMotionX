Imports System.IO

Public Class dlg_Program_Parameters

#Region "Declarations"

    Private Initialized As Boolean = False
    Private Changes_Made As Boolean = False

#End Region

#Region "Shown, Closing"

    Private Sub dlg_Shown(sender As Object, e As EventArgs) Handles Me.Shown
        Dim T As String
        Dim Cnt As String

        Changes_Made = False
        txt_Folder.Text = Program_Parameters.File_Folder
        txt_File_Name.Text = Program_Parameters.File_Name
        cmb_Tool_Name.Text = Program_Parameters.Tool_Name
        cmb_Tool_Speed.Text = Program_Parameters.Tool_Speed
        txt_Tool_Seat_X.Text = Program_Parameters.Tool_Seat_X
        txt_Tool_Seat_Y.Text = Program_Parameters.Tool_Seat_Y
        txt_Tool_Seat_Z.Text = Program_Parameters.Tool_Seat_Z
        txt_Tool_Measure_X.Text = Program_Parameters.Tool_Measure_X
        txt_Tool_Measure_Y.Text = Program_Parameters.Tool_Measure_Y
        txt_Tool_Measure_Z.Text = Program_Parameters.Tool_Measure_Z
        txt_Park_X.Text = Program_Parameters.Park_X
        txt_Park_Y.Text = Program_Parameters.Park_Y
        txt_Park_Z.Text = Program_Parameters.Park_Z
        txt_Time.Text = Program_Parameters.Machining_Time
        If Program_Parameters.Tool_Measure_From = "Fixed" Then
            rad_Fixed.Checked = True
            rad_Surface.Checked = False
        Else
            rad_Fixed.Checked = False
            rad_Surface.Checked = True
        End If

        cmb_Tool_Name.Items.Clear()
        Cnt = 0
        Do
            Cnt += 1
            T = "Tool.Name." & Trim(Cnt.ToString) & ".Text"
            T = Symbol.Get_Property(T, False)
            If T <> "" Then cmb_Tool_Name.Items.Add(T)
        Loop Until T = ""

        If Program_Parameters.Tool_Speed = "" Then
            cmb_Tool_Speed.Text = "Full"
        Else
            cmb_Tool_Speed.Text = Program_Parameters.Tool_Speed
        End If

        Validate_Parameters()
        Initialized = True

    End Sub

    Private Sub dlg_Program_Parameters_FormClosing(sender As Object, e As FormClosingEventArgs) Handles Me.FormClosing
        If Not Program_Parameters.Parameter_Check() Then
            e.Cancel = True
        End If
    End Sub

#End Region

#Region "Toolbar"

    Private Sub ts_Exit_Click(sender As Object, e As EventArgs) Handles ts_Exit.Click
        If Changes_Made Then
            Message_Box.ShowDialog("Changes have been made, save?", "Warning", MessageBoxButtons.YesNo)
            If Message_Box.DialogResult = Windows.Forms.DialogResult.Yes Then
                Save_Parameters()
            End If
        End If
        Close()
    End Sub

    Private Sub ts_Cancel_Click(sender As Object, e As EventArgs) Handles ts_Cancel.Click
        DialogResult = Windows.Forms.DialogResult.Cancel
        Close()
    End Sub

    Private Sub ts_Copy_From_Click(sender As Object, e As EventArgs) Handles ts_Copy_From.Click
        Program_Parameters.Load_Parameter_File(True)
    End Sub

    Private Sub ts_Save_Click(sender As Object, e As EventArgs) Handles ts_Save.Click
        Save_Parameters()
    End Sub

#End Region

    Private Sub Save_Parameters()
        Program_Parameters.Tool_Name = cmb_Tool_Name.Text
        Program_Parameters.Tool_Speed = cmb_Tool_Speed.Text
        Program_Parameters.Tool_Seat_X = Get_Single(txt_Tool_Seat_X.Text)
        Program_Parameters.Tool_Seat_Y = Get_Single(txt_Tool_Seat_Y.Text)
        Program_Parameters.Tool_Seat_Z = Get_Single(txt_Tool_Seat_Z.Text)
        Program_Parameters.Tool_Measure_X = Get_Single(txt_Tool_Measure_X.Text)
        Program_Parameters.Tool_Measure_Y = Get_Single(txt_Tool_Measure_Y.Text)
        Program_Parameters.Tool_Measure_Z = Get_Single(txt_Tool_Measure_Z.Text)
        Program_Parameters.Park_X = Get_Single(txt_Park_X.Text)
        Program_Parameters.Park_Y = Get_Single(txt_Park_Y.Text)
        Program_Parameters.Park_Z = Get_Single(txt_Park_Z.Text)
        Program_Parameters.Machining_Time = txt_Time.Text
        If rad_Fixed.Checked Then
            Program_Parameters.Tool_Measure_From = "Fixed"
        Else
            Program_Parameters.Tool_Measure_From = "Surface"
        End If

        Program_Parameters.Save_Parameter_File()
        Changes_Made = False

        Macros.Program_Parameters_Changed()

    End Sub

#Region "Control Events"

    Private Sub txt_Tool_Seat_X_Click(sender As Object, e As EventArgs) Handles txt_Tool_Seat_X.Click
        txt_Tool_Seat_X.Text = Get_User_Input(txt_Tool_Seat_X.Text, "Enter Tool Seat X Position", Symbol.Get_Value("Sys.Abs_X"), "Current X", True, _
                                              Symbol.Get_Value("Text_Box.Tool.Setter.X"), "Tool Setter X")
        Validate_Parameters()
        Changes_Made = True
    End Sub

    Private Sub txt_Tool_Seat_Y_Click(sender As Object, e As EventArgs) Handles txt_Tool_Seat_Y.Click
        txt_Tool_Seat_Y.Text = Get_User_Input(txt_Tool_Seat_Y.Text, "Enter Tool CSeat Y Position", Symbol.Get_Value("Sys.Abs_Y"), "Current Y", True, _
                                              Symbol.Get_Value("Text_Box.Tool.Setter.Y"), "Tool Setter Y")
        Validate_Parameters()
        Changes_Made = True
    End Sub

    Private Sub txt_Tool_Seat_Z_Click(sender As Object, e As EventArgs) Handles txt_Tool_Seat_Z.Click
        txt_Tool_Seat_Z.Text = Get_User_Input(txt_Tool_Seat_Z.Text, "Enter Tool Seat Z Position", Symbol.Get_Value("Sys.Abs_Z"), "Current Z", True)
        Validate_Parameters()
        Changes_Made = True
    End Sub

    Private Sub txt_Tool_Measure_X_Click(sender As Object, e As EventArgs) Handles txt_Tool_Measure_X.Click
        txt_Tool_Measure_X.Text = Get_User_Input(txt_Tool_Measure_X.Text, "Enter Tool Measure X Position", Symbol.Get_Value("Sys.Abs_X"), "Current X", True, _
                                                 Symbol.Get_Value("Text_Box.Tool.Setter.X"), "Tool Setter X")
        Validate_Parameters()
        Changes_Made = True
    End Sub

    Private Sub txt_Tool_Measure_Y_Click(sender As Object, e As EventArgs) Handles txt_Tool_Measure_Y.Click
        txt_Tool_Measure_Y.Text = Get_User_Input(txt_Tool_Measure_Y.Text, "Enter Tool Measure Y Position", Symbol.Get_Value("Sys.Abs_Y"), "Current Y", True, _
                                                 Symbol.Get_Value("Text_Box.Tool.Setter.Y"), "Tool Setter Y")
        Validate_Parameters()
        Changes_Made = True
    End Sub

    Private Sub txt_Tool_Measure_Z_Click(sender As Object, e As EventArgs) Handles txt_Tool_Measure_Z.Click
        txt_Tool_Measure_Z.Text = Get_User_Input(txt_Tool_Measure_Z.Text, "Enter Tool Measure Z Position", Symbol.Get_Value("Sys.Abs_Z"), "Current Z", True)
        Validate_Parameters()
        Changes_Made = True
    End Sub

    Private Sub txt_Park_X_Click(sender As Object, e As EventArgs) Handles txt_Park_X.Click
        txt_Park_X.Text = Get_User_Input(txt_Park_X.Text, "Enter Park X Position", Symbol.Get_Value("Sys.Abs_X"), "Current X", True)
        Validate_Parameters()
        Changes_Made = True
    End Sub

    Private Sub txt_Park_Y_Click(sender As Object, e As EventArgs) Handles txt_Park_Y.Click
        txt_Park_Y.Text = Get_User_Input(txt_Park_Y.Text, "Enter Park Y Position", Symbol.Get_Value("Sys.Abs_Y"), "Current Y", True)
        Validate_Parameters()
        Changes_Made = True
    End Sub

    Private Sub txt_Park_Z_Click(sender As Object, e As EventArgs) Handles txt_Park_Z.Click
        txt_Park_Z.Text = Get_User_Input(txt_Park_Z.Text, "Enter Park Z Position", Symbol.Get_Value("Sys.Abs_Z"), "Current Z", True)
        Validate_Parameters()
        Changes_Made = True
    End Sub

    Private Sub cmb_Tool_Name_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cmb_Tool_Name.SelectedIndexChanged
        If Initialized Then
            Validate_Parameters()
            Changes_Made = True
        End If
    End Sub

    Private Sub cmb_Tool_Speed_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cmb_Tool_Speed.SelectedIndexChanged
        If Initialized Then
            Validate_Parameters()
            Changes_Made = True
        End If
    End Sub

    Private Sub rad_Fixed_CheckedChanged(sender As Object, e As EventArgs) Handles rad_Fixed.CheckedChanged
        If Initialized Then
            Changes_Made = True
        End If
    End Sub

    Private Sub rad_Surface_CheckedChanged(sender As Object, e As EventArgs) Handles rad_Surface.CheckedChanged
        If Initialized Then
            Changes_Made = True
        End If
    End Sub

#End Region

    Private Function Get_Single(Number As String) As String
        If IsNumeric(Number) Then Return Number Else Return ""
    End Function

    Private Sub Validate_Parameters()
        Validate_Entry(txt_Tool_Measure_X, True)
        Validate_Entry(txt_Tool_Measure_Y, True)
        Validate_Entry(txt_Tool_Measure_Z, True)
        Validate_Entry(txt_Park_X, True)
        Validate_Entry(txt_Park_Y, True)
        Validate_Entry(txt_Park_Z, True)
        Validate_Entry(cmb_Tool_Name, False)
        Validate_Entry(cmb_Tool_Speed, False)
    End Sub

    Private Sub Validate_Entry(ctrl As Control, Number As Boolean)
        If Number Then
            If Not IsNumeric(Number) Then
                ctrl.Text = ""
            End If
        End If
        If ctrl.Text = "" Then ctrl.BackColor = Color.MistyRose Else ctrl.BackColor = Color.White
    End Sub

End Class