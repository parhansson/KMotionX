Imports System.IO

Public Class Chip_Fixture_Offsets

#Region "enums"

    Public Enum enum_Image_Index
        Bar_On
        Bar_Off
        Bar_Moving
        Adjust_Plus_X_Up
        Adjust_Plus_X_Down
        Adjust_Minus_X_Up
        Adjust_Munus_X_Down
        Adjust_Plus_Y_Up
        Adjust_Plus_Y_Down
        Adjust_Minus_Y_Up
        Adjust_Munus_Y_Down
        Adjust_Plus_Z_Up
        Adjust_Plus_Z_Down
        Adjust_Minus_Z_Up
        Adjust_Munus_Z_Down
    End Enum

#End Region

#Region "Declarations"

    Private Image_Table As New List(Of Bitmap)

    Public Persistant_Settings As New List(Of String)

    Private Ignore_Grid_Events As Boolean = False
    Private Offsets_Changed As Boolean = False

    Public Tool_Measured As Boolean = False
    Public Offsets_Have_Been_Set As Boolean = True 'tktk


#End Region


#Region "Initialize, Setup"

    Public Sub Initialize()

        Init_Button(btn_Reset_X)
        Init_Button(btn_Reset_Y)
        Init_Button(btn_Reset_Z)
        Init_Button(btn_Goto_Offset)

        Init_Button(btn_Adjust_Plus_X)
        Init_Button(btn_Adjust_Minus_X)
        Init_Button(btn_Adjust_Plus_Y)
        Init_Button(btn_Adjust_Minus_Y)
        Init_Button(btn_Adjust_Plus_Z)
        Init_Button(btn_Adjust_Minus_Z)

        Init_Button(btn_Show_KFlop_Offsets)

        Load_Image("Green_Bar_On.png")
        Load_Image("Green_Bar_Off.png")
        Load_Image("Yellow_Bar_On.png")

        Load_Image("Arrow_Right.png")
        Load_Image("Arrow_Right_Pressed.png")
        Load_Image("Arrow_Left.png")
        Load_Image("Arrow_Left_Pressed.png")

        Load_Image("Arrow_Up.png")
        Load_Image("Arrow_Up_Pressed.png")
        Load_Image("Arrow_Down.png")
        Load_Image("Arrow_Down_Pressed.png")

        Load_Image("Red_Arrow_Up.png")
        Load_Image("Red_Arrow_Up_Pressed.png")
        Load_Image("Red_Arrow_Down.png")
        Load_Image("Red_Arrow_Down_Pressed.png")

        txt_X_Offset.Text = "0"
        txt_Original_X.Text = "0"
        txt_Difference_X.Text = "0"
        txt_Y_Offset.Text = "0"
        txt_Original_Y.Text = "0"
        txt_Difference_Y.Text = "0"
        txt_Z_Offset.Text = "0"
        txt_Original_Z.Text = "0"
        txt_Difference_Z.Text = "0"
        txt_Adjust_Increment.Text = ".001"

    End Sub

    Public Sub Setup()
        Calculate_Offsets()
    End Sub

#End Region


#Region "Methods"

    Public Sub Set_Build_Offsets(Fixture As String, Mount As String, Part As String,
                                 Mount_Offset_X As Single, Part_Offset_X As Single,
                                 Mount_Offset_Y As Single, Part_Offset_Y As Single,
                                 Mount_Delta_Z As Single, Part_Offset_Z As Single,
                                 Part_Thickness As Single, Tool_Setter_Type As String)

        txt_Fixture.Text = Fixture
        txt_Mount.Text = Mount
        txt_Part.Text = Part

        txt_X_Mount_Offset.Text = FormatNumber(Mount_Offset_X, 4)
        txt_X_Part_Offset.Text = FormatNumber(Part_Offset_X, 4)

        txt_Y_Part_Offset.Text = FormatNumber(Part_Offset_Y, 4)
        txt_Y_Mount_Offset.Text = FormatNumber(Mount_Offset_Y, 4)

        txt_Z_Part_Offset.Text = FormatNumber(Part_Offset_Z, 4)
        txt_Mount_Delta_Z.Text = FormatNumber(Mount_Delta_Z, 4)

        txt_Part_Thickness.Text = FormatNumber(Part_Thickness, 4)

        txt_Difference_X.Text = 0
        txt_Difference_Y.Text = 0
        txt_Difference_Z.Text = 0

        Select Case Tool_Setter_Type
            Case "Fixed"
                rad_Fixed_Toolsetter.Checked = True
            Case "Movable"
                rad_Movable_Toolsetter.Checked = True
            Case "Surface"
                rad_Surface.Checked = True
            Case Else
                rad_Fixed_Toolsetter.Checked = False
                rad_Movable_Toolsetter.Checked = False
                rad_Surface.Checked = False
        End Select

        Calculate_Offsets()

        Offsets_Have_Been_Set = True
        Offsets_Changed = True

    End Sub

    Private Sub Calculate_Offsets()

        txt_Original_X.Text = FormatNumber(Get_Text_Number(txt_X_Mount_Offset.Text) + Get_Text_Number(txt_X_Part_Offset.Text), 4)
        txt_X_Offset.Text = FormatNumber(Get_Text_Number(txt_Original_X.Text) + Get_Text_Number(txt_Difference_X.Text), 4)

        txt_Original_Y.Text = FormatNumber(Get_Text_Number(txt_Y_Mount_Offset.Text) + Get_Text_Number(txt_Y_Part_Offset.Text), 4)
        txt_Y_Offset.Text = FormatNumber(Get_Text_Number(txt_Original_Y.Text) + Get_Text_Number(txt_Difference_Y.Text), 4)

        txt_Z_Fixed_Offset.Text = FormatNumber(Get_Text_Number(txt_Z_Tool_Measurement_Fixed.Text) - Get_Text_Number(txt_Fixed_Tool_Setter_Height.Text) +
                                  Get_Text_Number(txt_Mount_Delta_Z.Text) + Get_Text_Number(txt_Part_Thickness.Text), 4)

        txt_Z_Movable_Offset.Text = FormatNumber(Get_Text_Number(txt_Z_Tool_Measurement_Movable.Text) -
                                                 Get_Text_Number(txt_Movable_Tool_Setter_Height.Text), 4)

        txt_Original_Z.Text = 0
        'tktk        txt_Z_Offset.Text = Display.Maximum_Z
        txt_Z_Offset.Text = 0.0

        If rad_Fixed_Toolsetter.Checked Then
            txt_Z_Mount_Offset.Text = txt_Z_Fixed_Offset.Text
            txt_Original_Z.Text = FormatNumber(Get_Text_Number(txt_Z_Mount_Offset.Text) + Get_Text_Number(txt_Z_Part_Offset.Text), 4)
            txt_Z_Offset.Text = FormatNumber(Get_Text_Number(txt_Original_Z.Text) + Get_Text_Number(txt_Difference_Z.Text), 4)
        End If

        If rad_Movable_Toolsetter.Checked Then
            txt_Z_Mount_Offset.Text = txt_Z_Movable_Offset.Text
            txt_Original_Z.Text = FormatNumber(Get_Text_Number(txt_Z_Mount_Offset.Text) + Get_Text_Number(txt_Z_Part_Offset.Text), 4)
            txt_Z_Offset.Text = FormatNumber(Get_Text_Number(txt_Original_Z.Text) + Get_Text_Number(txt_Difference_Z.Text), 4)
        End If

        If rad_Surface.Checked Then
            txt_Z_Mount_Offset.Text = txt_Z_Surface_Offset.Text
            txt_Original_Z.Text = FormatNumber(Get_Text_Number(txt_Z_Mount_Offset.Text) + Get_Text_Number(txt_Z_Part_Offset.Text), 4)
            txt_Z_Offset.Text = FormatNumber(Get_Text_Number(txt_Original_Z.Text) + Get_Text_Number(txt_Difference_Z.Text), 4)
        End If

        KFlop.Set_Fixture_Offsets(Class_KFLOP.enum_Coorinate_System.G55,
                                 Get_Text_Number(txt_X_Offset.Text),
                                 Get_Text_Number(txt_Y_Offset.Text),
                                 Get_Text_Number(txt_Z_Offset.Text))
    End Sub

    Public Sub Set_Tool_Offset(Z_Measurement As Double, Type As String)
        Dim Off As Class_KFLOP.struct_Offset
        Dim Tool_Setter_Height As Double
        Dim Z_Delta As Double = Get_Text_Number(txt_Mount_Delta_Z.Text)
        Dim Part_Thickness As Double = Get_Text_Number(txt_Part_Thickness.Text)

        Select Case Type
            Case "Fixed"
                Tool_Setter_Height = Get_Text_Number(txt_Fixed_Tool_Setter_Height.Text)
                txt_Z_Tool_Measurement_Fixed.Text = Math.Round(Z_Measurement, 4)
                rad_Fixed_Toolsetter.Checked = True
                Main_Form.ctl_Fixture_Offsets.Tool_Measured = True

            Case "Movable"
                Tool_Setter_Height = Get_Text_Number(txt_Movable_Tool_Setter_Height.Text)
                txt_Z_Tool_Measurement_Movable.Text = Math.Round(Z_Measurement, 4)
                rad_Movable_Toolsetter.Checked = True
                Main_Form.ctl_Fixture_Offsets.Tool_Measured = True

            Case "Surface"
                txt_Z_Surface_Offset.Text = Math.Round(Z_Measurement, 4)
                rad_Surface.Checked = True
                Main_Form.ctl_Fixture_Offsets.Tool_Measured = True

            Case "None"
                Main_Form.ctl_Fixture_Offsets.Tool_Measured = False
        End Select

        Calculate_Offsets()

        Off.X = Get_Text_Number(txt_X_Offset.Text)
        Off.Y = Get_Text_Number(txt_Y_Offset.Text)
        Off.Z = Get_Text_Number(txt_Z_Offset.Text)

        Kflop.Set_Fixture_Offsets(Class_KFLOP.enum_Coorinate_System.G55, Off.X, Off.Y, Off.Z)

    End Sub

    Public Sub Set_Fixed_Tool_Setter_Height(H As Double)
        txt_Fixed_Tool_Setter_Height.Text = FormatNumber(H, 4)
        Calculate_Offsets()
    End Sub

    Public Sub Set_Movable_Tool_Setter_Height(H As Double)
        txt_Movable_Tool_Setter_Height.Text = FormatNumber(H, 4)
        Calculate_Offsets()
    End Sub

    Public Function Get_X_Offset() As Single
        Return Get_Text_Number(txt_Difference_X.Text)
    End Function

    Public Function Get_Y_Offset() As Single
        Return Get_Text_Number(txt_Difference_Y.Text)
    End Function

    Public Function Get_Z_Offset() As Single
        Return Get_Text_Number(txt_Difference_Z.Text)
    End Function

    Public Sub Reset_Offset_Adjustments()
        txt_Difference_X.Text = 0
        txt_X_Offset.Text = txt_Original_X.Text
        txt_Difference_Y.Text = 0
        txt_Y_Offset.Text = txt_Original_Y.Text
        txt_Difference_Z.Text = 0
        txt_Z_Offset.Text = txt_Original_Z.Text
        KFlop.Set_Fixture_Offsets(Class_KFLOP.enum_Coorinate_System.G55, CDbl(txt_X_Offset.Text), CDbl(txt_Y_Offset.Text), CDbl(txt_Z_Offset.Text))
    End Sub

    Public Function Offsets_OK() As Boolean

        If Not Main_Form.ctl_Fixture_Offsets.Offsets_Have_Been_Set Then
            Show_Error("Offsets have not been set." & vbCrLf & vbCrLf &
                       "Probably because there is no program loaded," & vbCrLf & vbCrLf &
                       "or the measurement type has been changed. (Fixed, Movable or Surface)")
            Return False
        End If

        If Not Main_Form.ctl_Fixture_Offsets.Tool_Measured Then
            If (Not Main_Form.ctl_Fixture_Offsets.rad_Fixed_Toolsetter.Checked) And
               (Not Main_Form.ctl_Fixture_Offsets.rad_Movable_Toolsetter.Checked) And
               (Not Main_Form.ctl_Fixture_Offsets.rad_Surface.Checked) Then
                '                Show_Error("Toolsetter type has not been set." & vbCrLf & vbCrLf &
                '                           "This needs to be set and the tool" & vbCrLf & vbCrLf &
                '                           "measured before Cycle Start can continue.")
                Dim dlg1 As New dlg_Message_Box
                dlg1.ShowDialog("Toolsetter type has not been set." & vbCrLf & vbCrLf &
                           "This needs to be set and the tool" & vbCrLf & vbCrLf &
                           "measured before Cycle Start can continue." & vbCrLf & vbCrLf &
                           "Continue anyway?", "Warning", MessageBoxButtons.YesNo)
                If dlg1.DialogResult <> DialogResult.Yes Then
                Return False
                Else
                    Main_Form.ctl_Fixture_Offsets.Tool_Measured = True
                    Main_Form.btn_Measure_Tool.State = "Up"
                    Return True
                End If
            End If

            Dim dlg As New dlg_Message_Box
            dlg.ShowDialog("Tool has not been measured." & vbCrLf & vbCrLf &
                           "Probably because program was just loaded." & vbCrLf & vbCrLf &
                           "Continue anyway?", "Warning", MessageBoxButtons.YesNo)
            If dlg.DialogResult <> DialogResult.Yes Then
                Return False
            Else
                Main_Form.ctl_Fixture_Offsets.Tool_Measured = True
                Main_Form.btn_Measure_Tool.State = "Up"
            End If

        End If

        Return True

    End Function

#End Region


#Region "Utility Routines"

    Private Sub Init_Button(Ctl As Button)
        Dim Bm As Bitmap
        Ctl.BackgroundImageLayout = ImageLayout.Stretch
        Ctl.BackgroundImageLayout = ImageLayout.Stretch
        Ctl.FlatStyle = FlatStyle.Flat
        Ctl.FlatAppearance.BorderSize = 0
        Ctl.TabStop = False
        Ctl.FlatAppearance.BorderColor = Ctl.Parent.BackColor
        Ctl.FlatAppearance.MouseOverBackColor = Ctl.Parent.BackColor
        Ctl.FlatAppearance.MouseDownBackColor = Ctl.Parent.BackColor
        Ctl.BackColor = Ctl.Parent.BackColor
        Bm = Ctl.BackgroundImage
        If Not IsNothing(Bm) Then Bm.MakeTransparent(Color.Magenta)
    End Sub

    Public Sub Load_Image(File_Name As String)
        If File.Exists(Chip_Images_Folder & File_Name) Then
            Dim Bm As New Bitmap(Chip_Images_Folder & File_Name)
            Bm.MakeTransparent(Color.Magenta)
            Image_Table.Add(Bm)
        Else
            Show_Error("Image file not found : " & File_Name)
            Exit Sub
        End If
    End Sub

    Public Function Get_Text_Number(S As String) As Double
        If S = "" Then Return 0
        If IsNumeric(S) Then
            Return CSng(S)
        Else
            Return 0
        End If
    End Function

#End Region


#Region "Kflop events"

    Public Sub Cycle_Stop()
        btn_Goto_Offset.BackgroundImage = Image_Table(enum_Image_Index.Bar_On)
        btn_Goto_Offset.Text = "Go To Offset"
    End Sub

    Public Sub Status_Update()

        'If Kflop.Controller.ThreadExecuting(Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe) Then
        Select Case Kflop.C_Program_Status
                Case Chip_C_Interface.enum_Status.Idle

                Case Chip_C_Interface.enum_Status.Move_Status + Chip_C_Interface.enum_Moving_To_Position_Status_Offsets.Move_Done
                    Main_Form.btn_Cycle_Start.State = "Cycle Start"
                    btn_Goto_Offset.BackgroundImage = Image_Table(enum_Image_Index.Bar_On)
                    btn_Goto_Offset.Text = "Go To Offset"
            End Select

        'End If

    End Sub

#End Region


#Region "Control Events"

    Private Sub btn_Goto_Offset_Mouse_Down(sender As Object, e As EventArgs) Handles btn_Goto_Offset.MouseDown
        Dim Btn As Button = sender
        Btn.BackgroundImage = Image_Table(enum_Image_Index.Bar_Moving)
        Btn.Text = "Moving to Offset"
        Main_Form.btn_Cycle_Start.State = "In Cycle"
        Main_Form.ctl_C_Interface.Set_Parameter_Position("Go to Offset", txt_X_Offset.Text, txt_Y_Offset.Text, Display.Maximum_Z, Display.Maximum_Z)
        Main_Form.ctl_C_Interface.Load_Parameters_and_Run_C_Program("Go to Offset", Class_KFLOP.enum_KFlop_Threads.Home_Goto_Probe)
    End Sub

    Private Sub btn_Offsets_Button_Down(sender As Object, e As EventArgs) Handles _
                                        btn_Reset_X.MouseDown, btn_Reset_Y.MouseDown, btn_Reset_Z.MouseDown, btn_Show_KFlop_Offsets.MouseDown

        Dim Btn As Button = sender
        Btn.BackgroundImage = Image_Table(enum_Image_Index.Bar_Off)

        Select Case Btn.Name
            Case "btn_Reset_X"
                txt_Difference_X.Text = 0
                txt_X_Offset.Text = txt_Original_X.Text
            Case "btn_Reset_Y"
                txt_Difference_Y.Text = 0
                txt_Y_Offset.Text = txt_Original_Y.Text
            Case "btn_Reset_Z"
                txt_Difference_Z.Text = 0
                txt_Z_Offset.Text = txt_Original_Z.Text
            Case "btn_Show_KFlop_Offsets"
                Exit Sub
        End Select

        Kflop.Set_Fixture_Offsets(Class_KFLOP.enum_Coorinate_System.G55, CDbl(txt_X_Offset.Text), CDbl(txt_Y_Offset.Text), CDbl(txt_Z_Offset.Text))

    End Sub

    Private Sub btn_Offsets_Button_Up(sender As Object, e As EventArgs) Handles btn_Reset_X.MouseUp, btn_Reset_Y.MouseUp, btn_Reset_Z.MouseUp, btn_Show_KFlop_Offsets.MouseUp
        Dim Btn As Button = sender
        Btn.BackgroundImage = Image_Table(enum_Image_Index.Bar_On)
    End Sub

    Private Sub btn_Adjust_MouseDown(sender As Object, e As EventArgs) Handles _
                btn_Adjust_Plus_X.MouseDown, btn_Adjust_Minus_X.MouseDown,
                btn_Adjust_Plus_Y.MouseDown, btn_Adjust_Minus_Y.MouseDown,
                btn_Adjust_Plus_Z.MouseDown, btn_Adjust_Minus_Z.MouseDown
        Dim Btn As Button = sender

        If Not IsNumeric(txt_Adjust_Increment.Text) Then
            Adjust_Button_Up(Btn)
            Show_Error("Value is not a correct number format.")
            Exit Sub
        End If

        Select Case Btn.Name
            Case "btn_Adjust_Plus_X"
                Btn.BackgroundImage = Image_Table(enum_Image_Index.Adjust_Plus_X_Down)
                txt_Difference_X.Text = FormatNumber(Get_Text_Number(txt_Difference_X.Text) + Get_Text_Number(txt_Adjust_Increment.Text), 4)

            Case "btn_Adjust_Minus_X"
                Btn.BackgroundImage = Image_Table(enum_Image_Index.Adjust_Munus_X_Down)
                txt_Difference_X.Text = FormatNumber(Get_Text_Number(txt_Difference_X.Text) - Get_Text_Number(txt_Adjust_Increment.Text), 4)

            Case "btn_Adjust_Plus_Y"
                Btn.BackgroundImage = Image_Table(enum_Image_Index.Adjust_Plus_Y_Down)
                txt_Difference_Y.Text = FormatNumber(Get_Text_Number(txt_Difference_Y.Text) + Get_Text_Number(txt_Adjust_Increment.Text), 4)

            Case "btn_Adjust_Minus_Y"
                Btn.BackgroundImage = Image_Table(enum_Image_Index.Adjust_Munus_Y_Down)
                txt_Difference_Y.Text = FormatNumber(Get_Text_Number(txt_Difference_Y.Text) - Get_Text_Number(txt_Adjust_Increment.Text), 4)

            Case "btn_Adjust_Plus_Z"
                Btn.BackgroundImage = Image_Table(enum_Image_Index.Adjust_Plus_Z_Down)
                txt_Difference_Z.Text = FormatNumber(Get_Text_Number(txt_Difference_Z.Text) + Get_Text_Number(txt_Adjust_Increment.Text), 4)

            Case "btn_Adjust_Minus_Z"
                Btn.BackgroundImage = Image_Table(enum_Image_Index.Adjust_Munus_Z_Down)
                txt_Difference_Z.Text = FormatNumber(Get_Text_Number(txt_Difference_Z.Text) - Get_Text_Number(txt_Adjust_Increment.Text), 4)

        End Select

        Calculate_Offsets()

        Kflop.Set_Fixture_Offsets(Class_KFLOP.enum_Coorinate_System.G55, CDbl(txt_X_Offset.Text), CDbl(txt_Y_Offset.Text), CDbl(txt_Z_Offset.Text))

    End Sub

    Private Sub btn_Adjust_MouseUp(sender As Object, e As EventArgs) Handles _
                btn_Adjust_Plus_X.MouseUp, btn_Adjust_Minus_X.MouseUp,
                btn_Adjust_Plus_Y.MouseUp, btn_Adjust_Minus_Y.MouseUp,
                btn_Adjust_Plus_Z.MouseUp, btn_Adjust_Minus_Z.MouseUp
        Dim Btn As Button = sender
        Adjust_Button_Up(Btn)
    End Sub

    Private Sub txt_Increment_Mouse_Click(sender As Object, e As System.Windows.Forms.MouseEventArgs) Handles txt_Adjust_Increment.MouseClick
        txt_Adjust_Increment.Text = Get_User_Input(txt_Adjust_Increment.Text, 0.01)
    End Sub

    Private Sub Adjust_Button_Up(Btn As Button)

        Select Case Btn.Name
            Case "btn_Adjust_Plus_X"
                Btn.BackgroundImage = Image_Table(enum_Image_Index.Adjust_Plus_X_Up)
            Case "btn_Adjust_Minus_X"
                Btn.BackgroundImage = Image_Table(enum_Image_Index.Adjust_Minus_X_Up)
            Case "btn_Adjust_Plus_Y"
                Btn.BackgroundImage = Image_Table(enum_Image_Index.Adjust_Plus_Y_Up)
            Case "btn_Adjust_Minus_Y"
                Btn.BackgroundImage = Image_Table(enum_Image_Index.Adjust_Minus_Y_Up)
            Case "btn_Adjust_Plus_Z"
                Btn.BackgroundImage = Image_Table(enum_Image_Index.Adjust_Plus_Z_Up)
            Case "btn_Adjust_Minus_Z"
                Btn.BackgroundImage = Image_Table(enum_Image_Index.Adjust_Minus_Z_Up)
        End Select
    End Sub

    Private Sub txt_Increment_Validated(sender As Object, e As EventArgs)
        txt_X_Offset.Focus()
    End Sub

    Private Sub rad_Movable_Toolsetter_CheckedChanged(sender As Object, e As EventArgs) Handles rad_Movable_Toolsetter.CheckedChanged
        Tool_Measured = False
        Calculate_Offsets()
    End Sub

    Private Sub rad_Surface_CheckedChanged(sender As Object, e As EventArgs) Handles rad_Surface.CheckedChanged
        Calculate_Offsets()
    End Sub

    Private Sub rad_Fixed_Toolsetter_CheckedChanged(sender As Object, e As EventArgs) Handles rad_Fixed_Toolsetter.CheckedChanged
        Calculate_Offsets()
    End Sub

    Private Sub btn_Show_KFlop_Offsets_Click(sender As Object, e As EventArgs) Handles btn_Show_KFlop_Offsets.Click
        Dim Offsets As Class_KFLOP.struct_Offset = KFlop.Upload_Offsets(Class_KFLOP.enum_Coorinate_System.G55)
        Show_Error("G55 Offsets" & vbCrLf &
                   "X : " & Offsets.X & vbCrLf &
                   "Y : " & Offsets.Y & vbCrLf &
                   "Z : " & Offsets.Z)
    End Sub

    Private Sub btn_Goto_Offset_Click(sender As Object, e As EventArgs) Handles btn_Goto_Offset.Click

    End Sub

#End Region

End Class
