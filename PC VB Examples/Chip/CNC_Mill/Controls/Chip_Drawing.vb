Imports System.Drawing
Imports System.Drawing.Drawing2D

Public Class Chip_Drawing

    Public Structure struct_Persistant_Setting
        Public Key As String
        Public Value As String
    End Structure


#Region "Declarations"

    Private Const Line_Width As Integer = 1

    Public WithEvents Part_Drawing As New Class_Canvas

    Private Initialized As Boolean = False
    Private Setup_Done As Boolean = False

    Public Persistant_Settings As New List(Of String)


#End Region

#Region "Initialize, Setup, Reset, Close"

    Public Sub Initialize(X_Maximum As Single, Y_Maximum As Single, Z_Maximum As Single)

        Part_Drawing = New Class_Canvas

        Part_Drawing.Initialize(Main_Form, pnl_Canvas, pnl_Viewport, txt_Scale, txt_Mouse, X_Maximum, Y_Maximum)

        ud_X_Angle.Left = ts_Drawing.Left + ts_Drawing.Width + 25
        ud_Y_Angle.Left = ud_X_Angle.Left + 75
        ud_Z_Angle.Left = ud_Y_Angle.Left + 75
        txt_Scale.Left = ud_Z_Angle.Left + 75

        lab_X.Left = ud_X_Angle.Left + (ud_X_Angle.Width / 2) - (lab_X.Width / 2)
        lab_Y.Left = ud_Y_Angle.Left + (ud_Y_Angle.Width / 2) - (lab_Y.Width / 2)
        lab_Z.Left = ud_Z_Angle.Left + (ud_Z_Angle.Width / 2) - (lab_Z.Width / 2)

        txt_Scale.Width = 50
        txt_Scale.TextAlign = HorizontalAlignment.Right
        lab_Scale.Left = txt_Scale.Left + (txt_Scale.Width / 2) - (lab_Scale.Width / 2)
        txt_Scale.Top = ud_X_Angle.Top
        txt_Scale.Text = "1"

        ud_Z_Scale.Left = txt_Scale.Left + txt_Scale.Width + 20
        lab_Z_Scale.Left = ud_Z_Scale.Left + (ud_Z_Scale.Width / 2) - (lab_Z_Scale.Width / 2)

        btn_Max.Left = ud_Z_Scale.Left + ud_Z_Scale.Width + 10
        btn_Min.Left = ud_Z_Scale.Left + ud_Z_Scale.Width + 10

        txt_Mouse.Left = btn_Max.Left + btn_Max.Width + 20

        Initialized = True

    End Sub

    Public Sub Setup(G_Code_Control As Chip_Gcode)
        Setup_Done = False

        Part_Drawing.Z_Scale = 1
        ud_Z_Scale.Text = 1

        Part_Drawing.Show_Rapid = True
        ts_Show_Rapid.Text = "Hide Rapid"
        Part_Drawing.Show_Z_Plus = True
        ts_Show_Z_Plus.Text = "Hide Z+"
        Part_Drawing.Use_Offsets = True
        Part_Drawing.Show_Part = True
        ts_Offsets_On.Visible = False
        ts_Offsets_Off.Visible = True

        Part_Drawing.Setup()

        Part_Drawing.Load_Drawing_Points(G_Code_Control)

        Part_Drawing.Repaint()
        Setup_Done = True

        Reset(True)

    End Sub

    Public Sub Apply_Settings()

        'Dim S() As String
        'Dim P() As String

        'For I = 0 To Settings.Count - 1
        '    S = Settings(I).Split("=")
        '    P = S(0).Split(".")
        '    Select Case P(1)
        '        Case "Scale"
        '            Part_Drawing.Scale = S(1)
        '        Case "Z_Scale"
        '            Part_Drawing.Z_Scale = S(1)
        '        Case "Canvas_Left"
        '            Part_Drawing.Canvas_Ctl.Left = S(1)
        '        Case "Canvas_Top"
        '            Part_Drawing.Canvas_Ctl.Top = S(1)
        '    End Select
        'Next

        'Repaint()

    End Sub

    Public Sub Finish()

    End Sub

    Public Sub Repaint()
        Part_Drawing.Repaint()
    End Sub

    Public Sub Reset(Optional Controls As Boolean = False)
        If Controls Then
            Part_Drawing.Reset_View_Controls()
        End If
        Part_Drawing.Repaint()
    End Sub

#End Region

#Region "Rescale, Angles"

    Public Sub Change_Scale_Text(Scale As Single)
        txt_Scale.Text = FormatNumber(Scale, 2)
    End Sub

    Sub Part_Drawing_Reset_Rotation() Handles Part_Drawing.Reset_Rotation
        Part_Drawing.X_Angle = 0
        ud_X_Angle.Text = 0
        Part_Drawing.Y_Angle = 0
        ud_Y_Angle.Text = 0
        Part_Drawing.Z_Angle = 0
        ud_Z_Angle.Text = 0
        Part_Drawing.Z_Scale = 25
        ud_Z_Scale.Text = 25
        Part_Drawing.Repaint()
    End Sub

    Private Sub ud_X_Angle_ValueChanged() Handles ud_X_Angle.ValueChanged
        If Not Initialized Then Exit Sub
        Part_Drawing.X_Angle = ud_X_Angle.Value
        Part_Drawing.Repaint()
    End Sub

    Private Sub ud_Y_Angle_ValueChanged() Handles ud_Y_Angle.ValueChanged
        If Not Initialized Then Exit Sub
        Part_Drawing.Y_Angle = ud_Y_Angle.Value
        Part_Drawing.Repaint()
    End Sub

    Private Sub ud_Z_Angle_ValueChanged() Handles ud_Z_Angle.ValueChanged
        If Not Initialized Then Exit Sub
        Part_Drawing.Z_Angle = ud_Z_Angle.Value
        Part_Drawing.Repaint()
    End Sub

    Private Sub ud_Z_Scale_ValueChanged(sender As Object, e As EventArgs) Handles ud_Z_Scale.ValueChanged
        If Not Initialized Then Exit Sub
        If Not Setup_Done Then Exit Sub
        Part_Drawing.Z_Scale = ud_Z_Scale.Value
        Part_Drawing.Repaint()
    End Sub

#End Region

#Region "Tool strip"

    Private Sub ts_Drawing_ItemClicked(sender As Object, e As ToolStripItemClickedEventArgs) Handles ts_Drawing.ItemClicked
        Static Dim Mess As String = ""

        Select Case e.ClickedItem.Text
            Case "Test"
                Show_Error("Left : " & Part_Drawing.Canvas_Ctl.Left & vbCr &
                           "Top : " & Part_Drawing.Canvas_Ctl.Top)

                'If Mess = "" Then Mess = "Vw=" & Str(Part_Drawing.Viewport.Viewport_Ctl.Width).PadLeft(5) & "  Vh=" & Str(Part_Drawing.Viewport.Viewport_Ctl.Height).PadLeft(5) & vbCrLf
                'Mess &= "Cw=" & Str(Part_Drawing.Canvas_Ctl.Width).PadLeft(5) & "  Ch=" & Str(Part_Drawing.Canvas_Ctl.Height).PadLeft(5) & vbCrLf &
                '        "CL=" & Str(Part_Drawing.Canvas_Ctl.Left).PadLeft(5) & "  CT=" & Str(Part_Drawing.Canvas_Ctl.Top).PadLeft(5) & "  CB=" & Str(Part_Drawing.Canvas_Ctl.Bottom).PadLeft(5) & vbCrLf &
                '        "Mx=" & Str(Part_Drawing.Mouse_Pos.X).PadLeft(5) & "  My=" & Str(Part_Drawing.Mouse_Pos.Y).PadLeft(5) & vbCrLf &
                '        "Sx=" & Str(Part_Drawing.Mouse_Scaled_Pos.X).PadLeft(5) & "  Sy=" & Str(Part_Drawing.Mouse_Scaled_Pos.Y).PadLeft(5) & vbCrLf &
                '                        "--------------------------" & vbCrLf

                'Show_Error(Mess, "Test")
                                '                Part_Drawing.Test_Mode = Not Part_Drawing.Test_Mode

            Case "Redraw"
                Part_Drawing.Repaint()

            Case "Reset"
                Reset(True)
                Mess = ""

            Case "Show Rapid"
                ts_Show_Rapid.Text = "Hide Rapid"
                Part_Drawing.Show_Rapid = True
                Part_Drawing.Repaint()

            Case "Hide Rapid"
                ts_Show_Rapid.Text = "Show Rapid"
                Part_Drawing.Show_Rapid = False
                Part_Drawing.Repaint()

            Case "Show Z+"
                ts_Show_Z_Plus.Text = "Hide Z+"
                Part_Drawing.Show_Z_Plus = True
                Part_Drawing.Repaint()

            Case "Hide Z+"
                ts_Show_Z_Plus.Text = "Show Z+"
                Part_Drawing.Show_Z_Plus = False
                Part_Drawing.Repaint()

            Case "Offsets On"
                Part_Drawing.Use_Offsets = True
                ts_Offsets_Off.Visible = True
                ts_Offsets_On.Visible = False
                Part_Drawing.Translate_Points()
                Part_Drawing.Repaint()

            Case "Offsets Off"
                Part_Drawing.Use_Offsets = False
                ts_Offsets_On.Visible = True
                ts_Offsets_Off.Visible = False
                Part_Drawing.Translate_Points()
                Part_Drawing.Repaint()

            Case "Show Material"
                Part_Drawing.Show_Part = True
                ts_Show_Part.Text = "Hide Material"
                Part_Drawing.Repaint()

            Case "Hide Material"
                Part_Drawing.Show_Part = False
                ts_Show_Part.Text = "Show Material"
                Part_Drawing.Repaint()

        End Select

    End Sub

    Private Sub btn_Max_Click(sender As Object, e As EventArgs) Handles btn_Max.Click
        ud_Z_Scale.Value = ud_Z_Scale.Maximum
    End Sub

    Private Sub btn_Min_Click(sender As Object, e As EventArgs) Handles btn_Min.Click
        ud_Z_Scale.Value = ud_Z_Scale.Minimum
    End Sub

#End Region

#Region "Class Canvas"

    Public Class Class_Canvas

#Region "Enums"

        Public Enum enum_Block_Type
            Grid
            Grid_Hash
            Grid_Tick
            Non_Move
            Line
            Arc_CW
            Arc_CCW
            Rapid
            Z_Move
            Z_Rapid
        End Enum

        Public Enum enum_Block_Status
            Not_Cut
            Next_Cut
            Cut
        End Enum

        Public Enum enum_Material_Origin
            None
            Top_Left
            Top_Right
            Bottom_Left
            Bottom_Right
        End Enum


#End Region

#Region "Structures"

        Public Structure struct_Inch_Point
            Public X As Single
            Public Y As Single
        End Structure

        Public Structure struct_Block
            Public Type As enum_Block_Type
            Public Status As enum_Block_Status
            Public Code As String
            Public X As Single
            Public Y As Single
            Public Z As Single
            Public I As Single
            Public J As Single
            Public Rapid As Boolean
        End Structure

        Public Structure struct_Draw_Point
            Public Type As enum_Block_Type
            Public Block_Number As Integer
            Public Xo As Single 'GCode point
            Public Yo As Single
            Public Zo As Single
            Public Xt As Single 'Translated into 3D
            Public Yt As Single
            Public Zt As Single
        End Structure

#End Region

#Region "Declarations"
        Public Test_Mode As Boolean = False

        Private Drawing_Granularity As Single = 0.001

        Private Grid_Color As New Pen(Color.Gray, 1)
        Private Grid_Hash_Color As New Pen(Color.LightGray, 2)
        Public Grid_Tick_Color As New Pen(Color.LightGray, 1)

        Public Fixture_Color As New Pen(Color.Tomato, 2)
        Public Offset_Color As New Pen(Color.GreenYellow, 2)
        Public Part_Color As New Pen(Color.LightSalmon, 2)


        Public Next_Move_Color As New Pen(Color.Red, Line_Width)
        Public Highlight_Color As New Pen(Color.Fuchsia, Line_Width * 4)

        Public Rapid_Path_Color As New Pen(Color.Red, Line_Width)
        Public Rapid_Cutting_Color As New Pen(Color.Yellow, Line_Width + 2)
        Public Rapid_Cut_Color As New Pen(Color.Firebrick, Line_Width + 1)

        Public Path_Color As New Pen(Color.DarkTurquoise, Line_Width)
        Public Path_Cutting_Color As New Pen(Color.Magenta, Line_Width + 2)
        Public Path_Cut_Color As New Pen(Color.GreenYellow, Line_Width + 1)

        Public Z_Path_Color As New Pen(Color.Fuchsia, Line_Width)
        Public Z_Cutting_Color As New Pen(Color.Yellow, Line_Width + 2)
        Public Z_Cut_Color As New Pen(Color.Orchid, Line_Width + 1)

        Private Position_Brush As New System.Drawing.SolidBrush(System.Drawing.Color.Yellow)
        Public Position_Block As Integer

        Public Show_Rapid As Boolean = False
        Public Show_Z_Plus As Boolean = False

        Public Grid_Points As New List(Of struct_Draw_Point)
        Public Draw_Points As New List(Of struct_Draw_Point)
        Public Block_Points As New List(Of struct_Draw_Point)
        Public Fixture_Points As New List(Of struct_Draw_Point)

        Public Drawing_Blocks As New List(Of struct_Block)

        Public Height_Inch As Single
        Public Width_Inch As Single

        Private _Drawing_Scale As Single
        Private X_Scale As Single '= 4 * X_Dpi
        Private Y_Scale As Single '= 4 * Y_Dpi
        Private X_Dpi As Single
        Private Y_Dpi As Single

        Public Bit_Map_Scale As Single = 2

        Private Pen_Scale As Single

        Public X_Angle As Single = 0
        Public Y_Angle As Single = 0
        Public Z_Angle As Single = 0
        Public Z_Scale As Single = 0
        Public Offsets As Boolean = False

        Private Offset_X As Single = 0
        Private Offset_Y As Single = 0
        Private Offset_Z As Single = 0
        Public Use_Offsets As Boolean
        Public Show_Part As Boolean

        Private Cutting_Start_X_Shadow As Double
        Private Cutting_Start_Y_Shadow As Double
        Private Cutting_Start_Z_Shadow As Double

        Private Rad_X As Single
        Private Rad_Y As Single
        Private Rad_Z As Single

        Private C_X As Single
        Private S_X As Single
        Private C_Y As Single
        Private S_Y As Single
        Private C_Z As Single
        Private S_Z As Single

        Public Viewport As Class_Viewport

        Public WithEvents Canvas_Ctl As PictureBox

        Private Parent As Form

        Private Scale_Text_Box As TextBox
        Private Mouse_Text_Box As TextBox

        Public Viewpoint_Center As Point
        Public Viewpoint_Center_Screen As Point
        Public Mouse_Inch As struct_Inch_Point
        Public Mouse_Scaled_Pos As Point
        Public Mouse_Pos As Point

        Private Last_Cut_Block As Integer = 0

        Private Fixture_Mount_Origin As struct_Draw_Point
        Private Part_Program_Offset As struct_Draw_Point
        Private Material_Height As Single
        Private Material_Width As Single
        Private Material_Length As Single
        Private Material_Origin As enum_Material_Origin

#End Region

#Region "Properties"

        Public Property Scale As Single

            Get
                Return _Drawing_Scale
            End Get

            Set(ByVal value As Single)
                If value > 0.1 Then
                    _Drawing_Scale = value
                    X_Scale = value * X_Dpi
                    Y_Scale = value * Y_Dpi
                    Scale_Text_Box.Text = _Drawing_Scale
                End If

            End Set

        End Property


#End Region

#Region "Events"

        Public Event Reset_Rotation()

#End Region

#Region "Initialize, Setup"

        Public Sub Initialize(Parent_Form As Form, Canvas_Control As PictureBox, View_Port_Control As Panel,
                              Scale_txt As TextBox, Mouse_txt As TextBox, Width_Inches As Single, Height_Inches As Single)

            Canvas_Ctl = Canvas_Control
            Parent = Parent_Form
            Scale_Text_Box = Scale_txt
            Mouse_Text_Box = Mouse_txt

            Viewport = New Class_Viewport(View_Port_Control)

            Scale = 1
            Width_Inch = Width_Inches
            Height_Inch = Height_Inches

            Dim temp_g As Graphics
            temp_g = Canvas_Ctl.CreateGraphics()
            X_Dpi = temp_g.DpiX ' / 16
            Y_Dpi = temp_g.DpiY '/ 16
            temp_g.Dispose()

            Canvas_Ctl.Height = Height_Inch * Y_Dpi * Scale
            Canvas_Ctl.Width = Width_Inch * X_Dpi * Scale

            Pen_Scale = (1 / X_Dpi) / Scale

            RemoveHandler Canvas_Ctl.MouseWheel, AddressOf Mouse_Wheel_Handler
            AddHandler Canvas_Ctl.MouseWheel, AddressOf Mouse_Wheel_Handler

            RemoveHandler Canvas_Ctl.MouseDown, AddressOf Mouse_Down_Handler
            AddHandler Canvas_Ctl.MouseDown, AddressOf Mouse_Down_Handler

            RemoveHandler Canvas_Ctl.MouseMove, AddressOf Mouse_Move_Handler
            AddHandler Canvas_Ctl.MouseMove, AddressOf Mouse_Move_Handler

            RemoveHandler Canvas_Ctl.MouseUp, AddressOf Mouse_Up_Handler
            AddHandler Canvas_Ctl.MouseUp, AddressOf Mouse_Up_Handler

        End Sub

        Public Sub Setup()
            Scale = 1
            Canvas_Ctl.Width = Width_Inch * X_Dpi * Scale
            Canvas_Ctl.Height = Height_Inch * Y_Dpi * Scale
            Canvas_Ctl.Left = 0
            Canvas_Ctl.Top = 0

        End Sub

#End Region

#Region "Rescale, repaint routines"

        Public Sub Set_Fixture_Part_Locations(Mount_X As Single, Mount_Y As Single,
                                              Part_Offset_X As Single, Part_Offset_Y As Single,
                                              Height_Material As Single, Width_Material As Single,
                                              Length_Material As Single, Origin_Material As enum_Material_Origin)
            Fixture_Mount_Origin.Xo = Mount_X
            Fixture_Mount_Origin.Yo = Mount_Y

            Part_Program_Offset.Xo = Part_Offset_X
            Part_Program_Offset.Yo = Part_Offset_Y

            Material_Height = Height_Material
            Material_Width = Width_Material
            Material_Length = Length_Material
            Material_Origin = Origin_Material

        End Sub

        Public Sub Rescale(Delta As Integer, Mouse_X As Integer, Mouse_Y As Integer)
            Dim Old_Scale As Single = Scale

            If Delta > 0 Then
                Scale += 0.25
            Else
                Scale -= 0.25
            End If

            If Scale > 8 Then Scale = 8

            Scale_Text_Box.Text = FormatNumber(Scale, 2)

            Canvas_Ctl.Top = 0
            Canvas_Ctl.Left = 0

            Viewpoint_Center.X = (Viewport.Viewport_Ctl.Width / 2)
            Viewpoint_Center.Y = (Viewport.Viewport_Ctl.Height / 2)
            Viewpoint_Center_Screen = Viewport.Viewport_Ctl.PointToScreen(Viewpoint_Center)

            Mouse_Pos.X = Mouse_X
            Mouse_Pos.Y = Mouse_Y

            Mouse_Inch.X = (Mouse_X / X_Dpi) / Old_Scale
            Mouse_Inch.Y = (Canvas_Ctl.Height - Mouse_Y) / Y_Dpi / Old_Scale

            Canvas_Ctl.Width = Width_Inch * X_Dpi * Scale
            Canvas_Ctl.Height = Height_Inch * Y_Dpi * Scale

            Mouse_Scaled_Pos.X = Mouse_Inch.X * X_Dpi * Scale
            Mouse_Scaled_Pos.Y = Canvas_Ctl.Height - (Mouse_Inch.Y * Y_Dpi * Scale)

            Canvas_Ctl.Left = Viewpoint_Center.X - Mouse_Scaled_Pos.X
            Canvas_Ctl.Top = Viewpoint_Center.Y - Mouse_Scaled_Pos.Y

            'Put cursor in middle of view port
            Cursor.Position = Viewpoint_Center_Screen

Skip:

            Repaint()

        End Sub

        Public Sub Repaint()
            Canvas_Ctl.Invalidate()
        End Sub

        Public Sub Update_Drawing(Current_Block As Integer)
            Dim gr As Graphics = Canvas_Ctl.CreateGraphics
            Dim Repaint As Boolean = False

            Canvas_Ctl.SuspendLayout()
            Draw_Cutting(gr)

            If Kflop.GCode_Program_Executing Then
                Show_Next_Move(gr, Current_Block)
                For I = Last_Cut_Block To Current_Block - 1
                    Mark_As_Cut(gr, I)
                    Repaint = True
                Next
                Last_Cut_Block = Current_Block

            End If

            Canvas_Ctl.ResumeLayout()

            gr.Dispose()

            If Repaint Then Canvas_Ctl.Invalidate()

        End Sub

        Private Sub Canvas_Ctl_Paint(sender As Object, e As System.Windows.Forms.PaintEventArgs) Handles Canvas_Ctl.Paint
            Draw_Cutter_Path(e.Graphics)
        End Sub

        Public Sub Reset_View_Controls()
            Scale = 0.25
            Scale_Text_Box.Text = FormatNumber(Scale, 2)

            X_Angle = 0
            Y_Angle = 0
            Z_Angle = 0
            Z_Scale = 25

            Canvas_Ctl.Width = Width_Inch * X_Dpi * Scale
            Canvas_Ctl.Height = Height_Inch * Y_Dpi * Scale

            Move_Canvas_Origin_To(0, 0)

            RaiseEvent Reset_Rotation()

        End Sub

#End Region

#Region "Location Routines"

        Public Sub Move_Canvas_Origin_To(X As Integer, Y As Integer)
            Canvas_Ctl.Top = Viewport.Height - Canvas_Ctl.Height - Y
            Canvas_Ctl.Left = X
        End Sub

#End Region

#Region "Drawing Routines"

        Public Sub Clear_Drawing()
            Drawing_Blocks.Clear()
        End Sub

        Public Sub Start_Drawing()
            Dim B As struct_Block
            For I = 0 To Drawing_Blocks.Count - 1
                B = Drawing_Blocks(I)
                B.Status = enum_Block_Status.Not_Cut
                Drawing_Blocks(I) = B
            Next

            Last_Cut_Block = 0

            Canvas_Ctl.Invalidate()
        End Sub




        Public Sub Draw_Cutter_Path(g As Graphics, Optional Reset As Boolean = False)
            Dim G_Mode As String = ""
            Dim Draw_Pen As Pen = Pens.Black

            Translate_Points()

            Canvas_Ctl.SuspendLayout()

            g.Clear(Canvas_Ctl.BackColor)

            Dim P As Integer = 1

            Do While P < Grid_Points.Count - 1
                Select Case Grid_Points(P).Type
                    Case enum_Block_Type.Grid
                        Draw_Line(g, Grid_Color, Grid_Points(P).Xt, Grid_Points(P).Yt, Grid_Points(P + 1).Xt, Grid_Points(P + 1).Yt)
                    Case enum_Block_Type.Grid_Hash
                        Draw_Line(g, Grid_Hash_Color, Grid_Points(P).Xt, Grid_Points(P).Yt, Grid_Points(P + 1).Xt, Grid_Points(P + 1).Yt)
                    Case enum_Block_Type.Grid_Tick
                        Draw_Line(g, Grid_Tick_Color, Grid_Points(P).Xt, Grid_Points(P).Yt, Grid_Points(P + 1).Xt, Grid_Points(P + 1).Yt)
                End Select
                P += 2
            Loop

            If Use_Offsets Then

                'Part Offset
                Draw_Cross(g, 4, Offset_Color)

                'Part Extents
                If Show_Part Then
                    'Mount Origin
                    Draw_Cross(g, 0, Fixture_Color)
                    Draw_Cube(g, 8, Part_Color)
                End If
            End If

            For I = 0 To Draw_Points.Count - 1
                Draw_Pen = Path_Color

                Select Case Drawing_Blocks(Draw_Points(I).Block_Number).Status

                    Case enum_Block_Status.Not_Cut
                        Select Case Drawing_Blocks(Draw_Points(I).Block_Number).Type

                            Case enum_Block_Type.Grid
                                Draw_Pen = Grid_Color

                            Case enum_Block_Type.Line, enum_Block_Type.Arc_CW, enum_Block_Type.Arc_CCW
                                Draw_Pen = Path_Color

                            Case enum_Block_Type.Rapid, enum_Block_Type.Z_Rapid
                                Draw_Pen = Rapid_Path_Color

                            Case enum_Block_Type.Z_Move
                                Draw_Pen = Z_Path_Color

                        End Select

                    Case enum_Block_Status.Next_Cut
                        Draw_Pen = Next_Move_Color

                    Case enum_Block_Status.Cut
                        Select Case Drawing_Blocks(Draw_Points(I).Block_Number).Type

                            Case enum_Block_Type.Grid
                                Draw_Pen = Grid_Color

                            Case enum_Block_Type.Line, enum_Block_Type.Arc_CW, enum_Block_Type.Arc_CCW
                                Draw_Pen = Path_Cut_Color

                            Case enum_Block_Type.Rapid
                                Draw_Pen = Rapid_Cut_Color

                            Case enum_Block_Type.Z_Move, enum_Block_Type.Z_Rapid
                                Draw_Pen = Z_Cut_Color

                        End Select
                End Select

                If Test_Mode Then
                    If I > 0 Then

                        Message_Box.ShowDialog("Point Index:" & I & vbCrLf &
                                               "I : " & I & vbCrLf &
                                               "Block : " & Draw_Points(I - 1).Block_Number & " ~ " & Draw_Points(I).Block_Number & vbCrLf &
                                               "Xo " & Draw_Points(I - 1).Xo & ", " & "Yo " & Draw_Points(I - 1).Yo & " to " &
                                               "Xo " & Draw_Points(I).Xo & ", " & "Yo " & Draw_Points(I).Yo & vbCrLf &
                                               "Xt " & Draw_Points(I - 1).Xt & ", " & "Yt " & Draw_Points(I - 1).Yt & " to " &
                                               "Xt " & Draw_Points(I).Xt & ", " & "Yt " & Draw_Points(I).Yt & vbCrLf, "", MessageBoxButtons.OKCancel)

                        If Message_Box.DialogResult <> DialogResult.OK Then
                            Test_Mode = False
                        End If

                    End If

                End If

                Select Case Drawing_Blocks(Draw_Points(I).Block_Number).Type

                    Case enum_Block_Type.Arc_CW, enum_Block_Type.Arc_CCW

                        Draw_Line(g, Draw_Pen, Draw_Points(I - 1).Xt, Draw_Points(I - 1).Yt, Draw_Points(I).Xt, Draw_Points(I).Yt)

                    Case enum_Block_Type.Rapid, enum_Block_Type.Z_Rapid
                        If Show_Rapid Then
                            If I > 0 Then
                                Draw_Line(g, Draw_Pen, Draw_Points(I - 1).Xt, Draw_Points(I - 1).Yt, Draw_Points(I).Xt, Draw_Points(I).Yt)
                            End If
                        End If

                    Case enum_Block_Type.Z_Move
                        If I > 0 Then
                            If Show_Z_Plus Then
                                Draw_Line(g, Draw_Pen, Draw_Points(I - 1).Xt, Draw_Points(I - 1).Yt, Draw_Points(I).Xt, Draw_Points(I).Yt)
                            Else
                                If Draw_Points(I - 1).Zo < 0 And Draw_Points(I).Zo < 0 Then
                                    Draw_Line(g, Draw_Pen, Draw_Points(I - 1).Xt, Draw_Points(I - 1).Yt, Draw_Points(I).Xt, Draw_Points(I).Yt)
                                End If
                            End If
                        End If

                    Case Else
                        If I > 0 Then
                            Draw_Line(g, Draw_Pen, Draw_Points(I - 1).Xt, Draw_Points(I - 1).Yt, Draw_Points(I).Xt, Draw_Points(I).Yt)
                        End If
                End Select

            Next

            If Position_Block > 0 Then
                Dim Pt As struct_Draw_Point
                Pt = Block_Points(Position_Block - 1)
                Dim rect As New Rectangle(Pt.Xt - 5, Pt.Yt - 5, 10, 10)
                g.FillEllipse(Position_Brush, rect)
                Position_Block = -1
            End If

Skip:
            Canvas_Ctl.ResumeLayout()

        End Sub

        Public Sub Reset_Draw_Cutting()
            If Use_Offsets Then
                Cutting_Start_X_Shadow = Kflop.X_Abs
                Cutting_Start_Y_Shadow = Kflop.Y_Abs
                Cutting_Start_Z_Shadow = (Kflop.Z_Abs - Offset_Z) / Z_Scale
            Else
                Cutting_Start_X_Shadow = Kflop.X_Work
                Cutting_Start_Y_Shadow = Kflop.Y_Work
                Cutting_Start_Z_Shadow = Kflop.Z_Work / Z_Scale
            End If
        End Sub

        Public Sub Draw_Cutting(Dwg_Gr As Graphics)
            Dim Start_Point As struct_Draw_Point
            Dim End_Point As struct_Draw_Point

            Start_Point.Xt = Cutting_Start_X_Shadow
            Start_Point.Yt = Cutting_Start_Y_Shadow
            Start_Point.Zt = Cutting_Start_Z_Shadow

            If Use_Offsets Then
                End_Point.Xt = Kflop.X_Abs
                End_Point.Yt = Kflop.Y_Abs
                End_Point.Zt = (Kflop.Z_Abs - Offset_Z + Material_Height) / Z_Scale
            Else
                End_Point.Xt = Kflop.X_Work
                End_Point.Yt = Kflop.Y_Work
                End_Point.Zt = Kflop.Z_Work / Z_Scale
            End If

            Cutting_Start_X_Shadow = End_Point.Xt
            Cutting_Start_Y_Shadow = End_Point.Yt
            Cutting_Start_Z_Shadow = End_Point.Zt

            Dim Rad_X As Single = X_Angle * Math.PI / 180
            Dim Rad_Y As Single = Y_Angle * Math.PI / 180
            Dim Rad_Z As Single = Z_Angle * Math.PI / 180

            Dim C_X = Math.Cos(Rad_X)
            Dim S_X = Math.Sin(Rad_X)
            Dim C_Y = Math.Cos(Rad_Y)
            Dim S_Y = Math.Sin(Rad_Y)
            Dim C_Z = Math.Cos(Rad_Z)
            Dim S_Z = Math.Sin(Rad_Z)

            Dim D As Single

            'Rotate around X
            Start_Point.Zt = (Start_Point.Zt * C_X) - (Start_Point.Yt * S_X)
            Start_Point.Yt = (Start_Point.Zt * S_X) + (Start_Point.Yt * C_X)

            'Rotate around Y
            Start_Point.Zt = (Start_Point.Zt * C_Y) - (Start_Point.Xt * S_Y)
            Start_Point.Xt = (Start_Point.Zt * S_Y) + (Start_Point.Xt * C_Y)

            'Rotate around Z
            Start_Point.Xt = (Start_Point.Xt * C_Z) - (Start_Point.Yt * S_Z)
            Start_Point.Yt = (Start_Point.Xt * S_Z) + (Start_Point.Yt * C_Z)

            D = Math.Abs(Math.Pow(Start_Point.Zt + sgn(Start_Point.Zt), sgn(Start_Point.Zt)))
            Start_Point.Xt = (Start_Point.Xt * D)
            Start_Point.Yt = (Start_Point.Yt * D)

            Start_Point.Xt = (Start_Point.Xt * X_Scale)
            Start_Point.Yt = Canvas_Ctl.Height - (Start_Point.Yt * Y_Scale)

            'Rotate around X
            End_Point.Zt = (End_Point.Zt * C_X) - (End_Point.Yt * S_X)
            End_Point.Yt = (End_Point.Zt * S_X) + (End_Point.Yt * C_X)

            'Rotate around Y
            End_Point.Zt = (End_Point.Zt * C_Y) - (End_Point.Xt * S_Y)
            End_Point.Xt = (End_Point.Zt * S_Y) + (End_Point.Xt * C_Y)

            'Rotate around Z
            End_Point.Xt = (End_Point.Xt * C_Z) - (End_Point.Yt * S_Z)
            End_Point.Yt = (End_Point.Xt * S_Z) + (End_Point.Yt * C_Z)

            D = Math.Abs(Math.Pow(End_Point.Zt + sgn(End_Point.Zt), sgn(End_Point.Zt)))
            End_Point.Xt = (End_Point.Xt * D)
            End_Point.Yt = (End_Point.Yt * D)

            End_Point.Xt = (End_Point.Xt * X_Scale)
            End_Point.Yt = Canvas_Ctl.Height - (End_Point.Yt * Y_Scale) - 1

            Draw_Cut_Line(Dwg_Gr, Path_Cutting_Color, Start_Point.Xt, Start_Point.Yt, End_Point.Xt, End_Point.Yt)

        End Sub

        Public Sub Show_Next_Move(Dwg_Gr As Graphics, Block_Number As Integer, Optional Highlight As Boolean = False)
            If Block_Number = 0 Then Exit Sub

            If (Block_Number > 1) And (Block_Number < Block_Points.Count - 2) Then

                Dim Pt As struct_Draw_Point
                Dim Next_Pt As struct_Draw_Point
                Dim C As Pen

                If Highlight Then
                    C = Highlight_Color
                Else
                    C = Next_Move_Color
                End If

                Pt = Block_Points(Block_Number - 1)
                Next_Pt = Block_Points(Block_Number)

                Dim Blk As struct_Block = Drawing_Blocks(Block_Number - 1)
                Blk.Status = enum_Block_Status.Next_Cut

                Select Case Drawing_Blocks(Block_Number).Type
                    Case enum_Block_Type.Rapid
                        If Show_Rapid Then
                            Draw_Cut_Line(Dwg_Gr, C, Pt.Xt, Pt.Yt, Next_Pt.Xt, Next_Pt.Yt)
                        End If
                    Case enum_Block_Type.Z_Rapid
                        If Show_Rapid Then
                            Draw_Cut_Line(Dwg_Gr, C, Pt.Xt, Pt.Yt, Next_Pt.Xt, Next_Pt.Yt)
                        End If
                    Case Else
                        Draw_Cut_Line(Dwg_Gr, C, Pt.Xt, Pt.Yt, Next_Pt.Xt, Next_Pt.Yt)
                End Select

            End If

        End Sub

        Public Sub Mark_As_Cut(Dwg_Gr As Graphics, Block_Number As Integer)
            If Block_Number <= 0 Then Exit Sub
            If Block_Number > Block_Points.Count - 1 Then Exit Sub

            Dim Blk As struct_Block = Drawing_Blocks(Block_Number)
            Dim Pt As struct_Draw_Point = Block_Points(Block_Number)
            Dim Prev_Pt As struct_Draw_Point = Block_Points(Block_Number - 1)
            Dim Prev_Blk As struct_Block = Drawing_Blocks(Block_Number - 1)

            'Dim temp_g As Graphics
            'temp_g = Canvas_Ctl.CreateGraphics()

            Select Case Drawing_Blocks(Block_Number).Type

                Case enum_Block_Type.Rapid
                    If Show_Rapid Then
                        Draw_Line(Dwg_Gr, Rapid_Cut_Color, Prev_Pt.Xt, Prev_Pt.Yt, Pt.Xt, Pt.Yt, True)
                    End If
                    Blk.Status = enum_Block_Status.Cut

                Case enum_Block_Type.Line
                    Draw_Line(Dwg_Gr, Path_Cut_Color, Prev_Pt.Xt, Prev_Pt.Yt, Pt.Xt, Pt.Yt, True)
                    Blk.Status = enum_Block_Status.Cut

                Case enum_Block_Type.Arc_CW, enum_Block_Type.Arc_CCW
                    Blk.Status = enum_Block_Status.Cut

                Case enum_Block_Type.Z_Move
                    Draw_Line(Dwg_Gr, Z_Cut_Color, Prev_Pt.Xt, Prev_Pt.Yt, Pt.Xt, Pt.Yt, True)
                    Blk.Status = enum_Block_Status.Cut

                Case enum_Block_Type.Z_Rapid
                    If Show_Rapid Then
                        Draw_Line(Dwg_Gr, Z_Cut_Color, Prev_Pt.Xt, Prev_Pt.Yt, Pt.Xt, Pt.Yt, True)
                    End If
                    Blk.Status = enum_Block_Status.Cut

            End Select


            'temp_g.Dispose()

            Drawing_Blocks(Block_Number) = Blk

        End Sub

        Public Function PointOnCircle(Radius As Single, AngleInDegrees As Single, Origin As PointF) As PointF
            ' Convert from degrees to radians via multiplication by PI/180        
            Dim Pt As PointF
            Pt.X = CSng(Radius * Math.Cos(AngleInDegrees * Math.PI / 180.0F)) + Origin.X
            Pt.Y = CSng(Radius * Math.Sin(AngleInDegrees * Math.PI / 180.0F)) + Origin.Y
            Return Pt
        End Function

        Private Function sgn(x As Single) As Single
            If x = 0 Then Return 0
            Return Math.Abs(x) / x
        End Function

        Public Sub Translate_Angles()
            Rad_X = X_Angle * Math.PI / 180
            Rad_Y = Y_Angle * Math.PI / 180
            Rad_Z = Z_Angle * Math.PI / 180

            C_X = Math.Cos(Rad_X)
            S_X = Math.Sin(Rad_X)
            C_Y = Math.Cos(Rad_Y)
            S_Y = Math.Sin(Rad_Y)
            C_Z = Math.Cos(Rad_Z)
            S_Z = Math.Sin(Rad_Z)
        End Sub

        Public Sub Translate_Point(ByRef Pt As struct_Draw_Point, Show_Offsets As Boolean)
            Dim D As Single

            Offset_X = Kflop.X_Work_Offset
            Offset_Y = Kflop.Y_Work_Offset
            Offset_Z = Kflop.Z_Work_Offset

            If Z_Scale = 0 Then Z_Scale = 1

            If Show_Offsets And Use_Offsets Then
                Pt.Xt = Pt.Xo + Offset_X
                Pt.Yt = Pt.Yo + Offset_Y
                Pt.Zt = Pt.Zo / Z_Scale
            Else
                Pt.Xt = Pt.Xo
                Pt.Yt = Pt.Yo
                Pt.Zt = Pt.Zo / Z_Scale
            End If

            'Rotate around X
            Pt.Zt = (Pt.Zt * C_X) - (Pt.Yt * S_X)
            Pt.Yt = (Pt.Zt * S_X) + (Pt.Yt * C_X)

            'Rotate around Y
            Pt.Zt = (Pt.Zt * C_Y) - (Pt.Xt * S_Y)
            Pt.Xt = (Pt.Zt * S_Y) + (Pt.Xt * C_Y)

            'Rotate around Z
            Pt.Xt = (Pt.Xt * C_Z) - (Pt.Yt * S_Z)
            Pt.Yt = (Pt.Xt * S_Z) + (Pt.Yt * C_Z)

            'Pt.Z = Pt.Zo * 2 'Lens 'zm=point[i].z/(float)lens;
            'Add perspective
            D = Math.Abs(Math.Pow(Pt.Zt + sgn(Pt.Zt), sgn(Pt.Zt)))

            Pt.Xt = (Pt.Xt * D)
            Pt.Yt = (Pt.Yt * D)
            Pt.Xt = (Pt.Xt * X_Scale)
            Pt.Yt = Canvas_Ctl.Height - (Pt.Yt * Y_Scale) - 1 'If 1 pixel not removed, bottom of drawing along Y0 is not shown

        End Sub

        Public Sub Translate_Points()
            Translate_Angles()

            Dim Pt As struct_Draw_Point

            For I = 0 To Grid_Points.Count - 1
                Pt = Grid_Points(I)
                Translate_Point(Pt, False)
                Grid_Points(I) = Pt
            Next

            For I = 0 To Fixture_Points.Count - 1
                Pt = Fixture_Points(I)
                Translate_Point(Pt, False)
                Fixture_Points(I) = Pt
            Next

            For I = 0 To Block_Points.Count - 1
                Pt = Block_Points(I)
                Translate_Point(Pt, True)
                Block_Points(I) = Pt
            Next

            For I = 0 To Draw_Points.Count - 1
                Pt = Draw_Points(I)
                Translate_Point(Pt, True)
                Draw_Points(I) = Pt
            Next

        End Sub

        Private Sub Add_Cross(ByRef Fixture_Points As List(Of struct_Draw_Point), ByVal Center As struct_Draw_Point, Length As Single)
            Dim Pt As struct_Draw_Point = Nothing
            Dim L As Single = (Length / 2)

            Pt.Xo = Center.Xo - L
            Pt.Yo = Center.Yo
            Pt.Zo = 0
            Fixture_Points.Add(Pt)

            Pt.Xo = Center.Xo + L
            Pt.Yo = Center.Yo
            Pt.Zo = 0
            Fixture_Points.Add(Pt)

            Pt.Xo = Center.Xo
            Pt.Yo = Center.Yo - L
            Pt.Zo = 0
            Fixture_Points.Add(Pt)

            Pt.Xo = Center.Xo
            Pt.Yo = Center.Yo + L
            Pt.Zo = 0
            Fixture_Points.Add(Pt)

        End Sub

        Private Sub Add_Cube(ByRef Fixture_Points As List(Of struct_Draw_Point),
                             ByVal Offset As struct_Draw_Point,
                             ByVal Dimensions As struct_Draw_Point)
            Dim Pt As struct_Draw_Point = Nothing
            Dim Z As Single

            'Verticals at corners
            Pt.Xo = Offset.Xo
            Pt.Yo = Offset.Yo
            Pt.Zo = 0
            Fixture_Points.Add(Pt)

            Pt.Xo = Offset.Xo
            Pt.Yo = Offset.Yo
            Pt.Zo = Dimensions.Zo
            Fixture_Points.Add(Pt)

            Pt.Xo = Offset.Xo + Dimensions.Xo
            Pt.Yo = Offset.Yo
            Pt.Zo = 0
            Fixture_Points.Add(Pt)

            Pt.Xo = Offset.Xo + Dimensions.Xo
            Pt.Yo = Offset.Yo
            Pt.Zo = Dimensions.Zo

            Fixture_Points.Add(Pt)
            Pt.Xo = Offset.Xo
            Pt.Yo = Offset.Yo + Dimensions.Yo
            Pt.Zo = 0
            Fixture_Points.Add(Pt)

            Pt.Xo = Offset.Xo
            Pt.Yo = Offset.Yo + Dimensions.Yo
            Pt.Zo = Dimensions.Zo
            Fixture_Points.Add(Pt)

            Pt.Xo = Offset.Xo + Dimensions.Xo
            Pt.Yo = Offset.Yo + Dimensions.Yo
            Pt.Zo = 0
            Fixture_Points.Add(Pt)

            Pt.Xo = Offset.Xo + Dimensions.Xo
            Pt.Yo = Offset.Yo + Dimensions.Yo
            Pt.Zo = Dimensions.Zo
            Fixture_Points.Add(Pt)

            Z = 0
            For I = 0 To 1
                If I = 1 Then Z = Dimensions.Zo

                'Bottom rectangle
                'Front
                Pt.Xo = Offset.Xo
                Pt.Yo = Offset.Yo
                Pt.Zo = Z
                Fixture_Points.Add(Pt)

                Pt.Xo = Offset.Xo + Dimensions.Xo
                Pt.Yo = Offset.Yo
                Pt.Zo = Z
                Fixture_Points.Add(Pt)

                'Right Side
                Pt.Xo = Offset.Xo + Dimensions.Xo
                Pt.Yo = Offset.Yo
                Pt.Zo = Z
                Fixture_Points.Add(Pt)

                Pt.Xo = Offset.Xo + Dimensions.Xo
                Pt.Yo = Offset.Yo + Dimensions.Yo
                Pt.Zo = Z
                Fixture_Points.Add(Pt)

                'Top
                Pt.Xo = Offset.Xo + Dimensions.Xo
                Pt.Yo = Offset.Yo + Dimensions.Yo
                Pt.Zo = Z
                Fixture_Points.Add(Pt)

                Pt.Xo = Offset.Xo
                Pt.Yo = Offset.Yo + Dimensions.Yo
                Pt.Zo = Z
                Fixture_Points.Add(Pt)

                'Left Side
                Pt.Xo = Offset.Xo
                Pt.Yo = Offset.Yo + Dimensions.Yo
                Pt.Zo = Z
                Fixture_Points.Add(Pt)

                Pt.Xo = Offset.Xo
                Pt.Yo = Offset.Yo
                Pt.Zo = Z
                Fixture_Points.Add(Pt)

            Next

        End Sub

        Public Sub Load_Drawing_Points(G_Code_Control As Chip_Gcode)
            Dim S() As String
            Dim G_Mode As String = ""
            Dim Pt As struct_Draw_Point = Nothing
            Dim Part_Offset_Center As struct_Draw_Point = Nothing
            Dim Part_Extents As struct_Draw_Point = Nothing
            Dim Part_Dimensions As struct_Draw_Point = Nothing
            Dim Last_X As Single = 0
            Dim Last_Y As Single = 0
            Dim Last_Z As Single = 0
            Dim Drawing_Block As struct_Block
            Static Dim Acccumulator As Single = 0

            Drawing_Blocks.Clear()

            For I = 0 To G_Code_Control.grid.RowCount - 1
                Drawing_Block = New struct_Block
                Drawing_Block.Code = G_Code_Control.grid.Rows(I).Cells(Chip_Gcode.enum_Grid_Colums.Block).Value
                Drawing_Blocks.Add(Drawing_Block)
            Next

            G_Code_Control.Cursor = Cursors.Hand

            Offset_X = 0
            Offset_Y = 0
            Offset_Z = 0

            Grid_Points.Clear()
            Block_Points.Clear()
            Draw_Points.Clear()
            Fixture_Points.Clear()

            'Mount origin
            Add_Cross(Fixture_Points, Fixture_Mount_Origin, 1)

            'Part Offset
            Part_Offset_Center.Xo = Fixture_Mount_Origin.Xo + Part_Program_Offset.Xo
            Part_Offset_Center.Yo = Fixture_Mount_Origin.Yo + Part_Program_Offset.Yo
            Add_Cross(Fixture_Points, Part_Offset_Center, 0.5)

            'Part extents
            Select Case Material_Origin
                Case enum_Material_Origin.None
                Case enum_Material_Origin.Top_Left
                    Part_Extents.Xo = Fixture_Mount_Origin.Xo
                    Part_Extents.Yo = Fixture_Mount_Origin.Yo - Material_Width
                Case enum_Material_Origin.Top_Right
                    Part_Extents.Xo = Fixture_Mount_Origin.Xo - Material_Length
                    Part_Extents.Yo = Fixture_Mount_Origin.Yo - Material_Width
                Case enum_Material_Origin.Bottom_Left
                    Part_Extents.Xo = Fixture_Mount_Origin.Xo
                    Part_Extents.Yo = Fixture_Mount_Origin.Yo
                Case enum_Material_Origin.Bottom_Right
                    Part_Extents.Xo = Fixture_Mount_Origin.Xo - Material_Length
                    Part_Extents.Yo = Fixture_Mount_Origin.Yo
            End Select

            Part_Dimensions.Xo = Math.Abs(Material_Length)
            Part_Dimensions.Yo = Math.Abs(Material_Width)
            Part_Dimensions.Zo = Math.Abs(Material_Height)
            Add_Cube(Fixture_Points, Part_Extents, Part_Dimensions)

            'Grid
            Pt.Xo = 0
            Pt.Yo = 0
            Pt.Zo = 0
            Grid_Points.Add(Pt)

            Pt.Block_Number = 0

            For X = 0 To Width_Inch

                If X Mod 10 = 0 Or X = Width_Inch Then
                    Pt.Type = enum_Block_Type.Grid_Hash
                Else
                    If X Mod 5 = 0 Then
                        Pt.Type = enum_Block_Type.Grid_Tick
                    Else
                        Pt.Type = enum_Block_Type.Grid
                    End If
                End If

                Pt.Xo = X
                Pt.Yo = 0
                Grid_Points.Add(Pt)

                Pt.Xo = X
                Pt.Yo = Height_Inch
                Grid_Points.Add(Pt)
            Next

            For Y = 0 To Height_Inch
                If Y Mod 10 = 0 Or Y = Height_Inch Then
                    Pt.Type = enum_Block_Type.Grid_Hash
                Else
                    If Y Mod 5 = 0 Then
                        Pt.Type = enum_Block_Type.Grid_Tick
                    Else
                        Pt.Type = enum_Block_Type.Grid
                    End If
                End If

                Pt.Xo = 0
                Pt.Yo = Y
                Grid_Points.Add(Pt)

                Pt.Xo = Width_Inch
                Pt.Yo = Y
                Grid_Points.Add(Pt)
            Next

            Pt.Xo = 0
            Pt.Yo = 0
            Pt.Zo = 0
            Grid_Points.Add(Pt)

            G_Mode = ""

            For I = 0 To Drawing_Blocks.Count - 1
                Pt.Block_Number = I

                Drawing_Block = Drawing_Blocks(I)

                If (Not IsNothing(Drawing_Block.Code)) And (Mid(Drawing_Block.Code, 1, 1) <> "(") And (Mid(Drawing_Block.Code, 1, 1) <> ";") Then
                    Drawing_Block.Type = enum_Block_Type.Non_Move
                    Drawing_Block.Code = Drawing_Block.Code.Replace("S", " S")
                    S = Drawing_Block.Code.Split(" ")

                    For J = 0 To S.Count - 1
                        Select Case Mid(S(J), 1, 1)
                            Case "X"
                                Drawing_Block.X = Mid(S(J), 2)
                                Pt.Xo = Drawing_Block.X

                            Case "Y"
                                Drawing_Block.Y = Mid(S(J), 2)
                                Pt.Yo = Drawing_Block.Y
                            Case "Z"
                                Drawing_Block.Z = Mid(S(J), 2)
                                If Show_Part Then
                                    Pt.Zo = Drawing_Block.Z + Material_Height
                                Else
                                    Pt.Zo = Drawing_Block.Z
                                End If


                            Case "I"
                                Drawing_Block.I = Mid(S(J), 2)
                            Case "J"
                                Drawing_Block.J = Mid(S(J), 2)
                        End Select

                        Select Case S(J)
                            Case "G00", "G01", "G02", "G03"
                                G_Mode = S(J)
                        End Select
                    Next

                    Select Case G_Mode
                        Case "G00"
                            Drawing_Block.Type = enum_Block_Type.Rapid
                            Pt.Type = enum_Block_Type.Rapid
                        Case "G01"
                            Drawing_Block.Type = enum_Block_Type.Line
                            Pt.Type = enum_Block_Type.Line
                        Case "G02"
                            Drawing_Block.Type = enum_Block_Type.Arc_CW
                            Pt.Type = enum_Block_Type.Arc_CW
                        Case "G03"
                            Drawing_Block.Type = enum_Block_Type.Arc_CCW
                            Pt.Type = enum_Block_Type.Arc_CCW
                    End Select

                    If Pt.Zo <> Last_Z Then
                        If Drawing_Block.Type = enum_Block_Type.Rapid Then
                            Drawing_Block.Type = enum_Block_Type.Z_Rapid
                        Else
                            Drawing_Block.Type = enum_Block_Type.Z_Move
                        End If
                    End If

                    Select Case Drawing_Block.Type
                        Case enum_Block_Type.Arc_CW, enum_Block_Type.Arc_CCW
                            'Create points on the circle

                            Dim Coor As List(Of Class_Calc.struct_Coordinate)
                            Coor = Calc.Interpolate_Arc(Drawing_Block.Code, Last_X, Last_Y, 0.1, Last_Z)
                            If IsNothing(Coor) Then
                                Exit Sub
                            End If

                            Block_Points.Add(Pt)

                            For C = 0 To Coor.Count - 1
                                Dim Cir_Pt As struct_Draw_Point = Nothing
                                Cir_Pt.Xo = Coor(C).X
                                Cir_Pt.Yo = Coor(C).Y
                                Cir_Pt.Zo = Coor(C).Z
                                Cir_Pt.Block_Number = I
                                Cir_Pt.Type = Drawing_Block.Type
                                Draw_Points.Add(Cir_Pt)
                            Next

                            Last_X = Pt.Xo
                            Last_Y = Pt.Yo
                            Last_Z = Pt.Zo

                        Case Else

                            Acccumulator += Math.Abs(Pt.Xo - Last_X) + Math.Abs(Pt.Yo - Last_Y)
                            If (Acccumulator > Drawing_Granularity) Or (Pt.Zo <> Last_Z) Or (I < 2) Then
                                Acccumulator = 0
                                G_Code_Control.Set_Grid_Point_Cell(I, Draw_Points.Count)
                                Draw_Points.Add(Pt)
                            End If

                            Last_X = Pt.Xo
                            Last_Y = Pt.Yo
                            Last_Z = Pt.Zo

                            'If (Math.Abs(Pt.Xo - Last_X) > Drawing_Granularity) Or (Math.Abs(Pt.Yo - Last_Y) > Drawing_Granularity) Or (Pt.Zo <> Last_Z) Then
                            '    GCode_Control.Set_Grid_Point_Cell(I, Draw_Points.Count)
                            '    Draw_Points.Add(Pt)
                            '    Last_X = Pt.Xo
                            '    Last_Y = Pt.Yo
                            '    Last_Z = Pt.Zo
                            'End If

                            Block_Points.Add(Pt)
                    End Select


                    Drawing_Blocks(I) = Drawing_Block

                End If 'If Not IsNothing(Drawing_Block)
            Next

            G_Code_Control.Cursor = Cursors.Arrow

        End Sub


#End Region

#Region "Drawing Routines"

        Public Sub Draw_Line(g As Graphics, Pen_Color As Pen, X1 As Single, Y1 As Single, X2 As Single, Y2 As Single, Optional Refresh As Boolean = False)
            'One has to be added to X and subtracted from Y so lines show up at Zero
            g.DrawLine(Pen_Color, X1 + 1, Y1 - 1, X2 + 1, Y2 - 1)
            '            If Refresh Then Canvas_Ctl.Image = Bit_Map
        End Sub

        Public Sub Draw_Cut_Line(g As Graphics, Pen_Color As Pen, X1 As Single, Y1 As Single, X2 As Single, Y2 As Single)
            'One has to be added to X and subtracted from Y so lines show up at Zero
            g.DrawLine(Pen_Color, X1 + 1, Y1 - 1, X2 + 1, Y2 - 1)
            '           Canvas_Ctl.Image = Bit_Map
        End Sub

        Public Sub Draw_Dashed_Line(g As Graphics, Pen_Color As Pen, X1 As Single, Y1 As Single, X2 As Single, Y2 As Single)
            Dim dashValues As Single() = {5, 10} ', 5, 10}
            Dim blackPen As New Pen(Color.Black, 2)
            blackPen.DashPattern = dashValues
            g.DrawLine(blackPen, CInt(X1 * X_Scale), CInt(Y1 * Y_Scale), CInt(X2 * X_Scale), CInt(Y2 * X_Scale))
        End Sub

        Public Sub Draw_Arc(g As Graphics, Pen_Color As Pen, X_Start As Single, Y_Start As Single, Width As Single, Height As Single, Deg_Start As Single, Deg_End As Single)
            Dim Deg_Sweep As Single = Deg_End - Deg_Start
            'g.DrawArc(Pen_Color, -X_Start, Y_Start, Width, Height, Deg_Start, Deg_End)

            'g.DrawCurve(Pen_Color, pts)

        End Sub

        Public Sub Draw_String(g As Graphics, Text As String, X As Single, Y As Single)
            g.DrawString(Text, New Font("Arial", 10), Brushes.Red, New PointF(X, Y))
        End Sub

        Public Sub Draw_Cross(g As Graphics, Index As Integer, Col As Pen)
            For I = Index To Index + 3 Step 2
                Draw_Line(g, Col, Fixture_Points(I).Xt, Fixture_Points(I).Yt, Fixture_Points(I + 1).Xt, Fixture_Points(I + 1).Yt)
            Next
        End Sub

        Public Sub Draw_Cube(g As Graphics, Index As Integer, Col As Pen)
            For I = Index To Index + 23 Step 2
                Draw_Line(g, Col, Fixture_Points(I).Xt, Fixture_Points(I).Yt, Fixture_Points(I + 1).Xt, Fixture_Points(I + 1).Yt)
            Next
        End Sub

#End Region

#Region "Mouse Handlers"

        Private Canvas_Rectangle As New Rectangle
        Private Start_Point As Point
        Private Last_Delta As Point
        Private Last_Move_Pt As Point
        Private Dragging As Boolean


        Private Sub Mouse_Wheel_Handler(sender As Object, e As MouseEventArgs)
            Rescale(e.Delta, e.X, e.Y)
        End Sub

        Private Sub Mouse_Down_Handler(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)

            Select Case e.Button
                Case Windows.Forms.MouseButtons.Left
                    Dim control As Control = CType(sender, Control)
                    Start_Point = control.PointToScreen(New Point(e.X, e.Y))
                    Last_Move_Pt.X = e.X
                    Last_Move_Pt.Y = e.Y
                    Last_Delta.X = 0
                    Last_Delta.Y = 0
                    Dragging = True
                Case Windows.Forms.MouseButtons.Right
                    Dragging = False

                Case Windows.Forms.MouseButtons.Middle

            End Select

        End Sub

        Private Sub Mouse_Move_Handler(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
            Static Dim cnt As Integer = 0

            Mouse_Text_Box.Text = FormatNumber(e.X / X_Dpi / Scale, 3) & ",  " & FormatNumber((Canvas_Ctl.Height - e.Y) / Y_Dpi / Scale, 3)

            Select Case e.Button

                Case Windows.Forms.MouseButtons.Middle
                    cnt += 1
                    Last_Delta.X = e.X - Last_Move_Pt.X
                    Last_Delta.Y = e.Y - Last_Move_Pt.Y

                    Last_Move_Pt.X = e.X - Last_Delta.X
                    Last_Move_Pt.Y = e.Y - Last_Delta.Y

                    If Last_Delta.X > 0 Then
                        X_Angle -= 1
                    ElseIf Last_Delta.X < 0 Then
                        X_Angle += 1
                    End If

                    If Last_Delta.Y > 0 Then
                        Y_Angle -= 1
                    ElseIf Last_Delta.Y < 0 Then
                        Y_Angle += 1
                    End If

                    Exit Sub
            End Select

            If Dragging Then
                Last_Delta.X = e.X - Last_Move_Pt.X
                Last_Delta.Y = e.Y - Last_Move_Pt.Y

                Last_Move_Pt.X = e.X - Last_Delta.X
                Last_Move_Pt.Y = e.Y - Last_Delta.Y

                Canvas_Ctl.Left += Last_Delta.X
                Canvas_Ctl.Top += Last_Delta.Y

            End If

        End Sub

        Private Sub Mouse_Up_Handler(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
            If (e.Button = MouseButtons.Right) Then
                ControlPaint.DrawReversibleFrame(Canvas_Rectangle, Canvas_Ctl.BackColor, FrameStyle.Dashed)
            End If
            Dragging = False
        End Sub

#End Region

    End Class

#End Region

#Region "Class Viewport"

    Public Class Class_Viewport
        Public Center As Point
        Public Height As Integer
        Public Width As Integer

        Public WithEvents Viewport_Ctl As Panel

        Public Sub New(Viewport_Control As Panel)
            Viewport_Ctl = Viewport_Control
            Width = Viewport_Ctl.Width
            Height = Viewport_Ctl.Height
            Center.X = Viewport_Ctl.Width / 2
            Center.Y = Viewport_Ctl.Height / 2
        End Sub

        Private Sub Resize(sender As Object, e As System.EventArgs) Handles Viewport_Ctl.Resize
            Width = Viewport_Ctl.Width
            Height = Viewport_Ctl.Height
            Center.X = Viewport_Ctl.Width / 2
            Center.Y = Viewport_Ctl.Height / 2
        End Sub


    End Class


#End Region

End Class
