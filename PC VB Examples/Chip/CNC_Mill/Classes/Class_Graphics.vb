Imports System.Drawing
Imports System.Drawing.Drawing2D

Public Class Class_Viewport
    Public Center As Point
    Public Height As Integer
    Public Width As Integer

    Private WithEvents ctl As Panel

    Public Sub New(Viewport_Control As Panel)
        ctl = Viewport_Control
        Width = ctl.Width
        Height = ctl.Height
        Center.X = ctl.Width / 2
        Center.Y = ctl.Height / 2
    End Sub

    Public Function Canvas_Location_At_Viewpoint_Center(Canvas As Class_Canvas, X As Integer, Y As Integer) As Point
        Dim Pt As Point
        Pt.X = Canvas.Left + ctl.Width / 2
        Pt.Y = Canvas.Bottom + ctl.Height / 2
        Return Pt
    End Function

    Private Sub Resize(sender As Object, e As System.EventArgs) Handles ctl.Resize
        Width = ctl.Width
        Height = ctl.Height
        Center.X = ctl.Width / 2
        Center.Y = ctl.Height / 2
    End Sub

End Class

Public Class Class_Canvas

#Region "Structures"

    Public Structure struct_Inch_Point
        Public X As Single
        Public Y As Single
    End Structure

#End Region

#Region "Declarations"

    Public Top As Integer
    Public Left As Integer
    Public Height As Integer
    Public Width As Integer

    Public X_Offset As Integer
    Public Y_Offset As Integer

    Public Height_In As Single
    Public Width_In As Single
    Public X_Offset_In As Integer
    Public Y_Offset_In As Integer

    Private Scale As Single
    Private Rotation As Integer
    Private Pen_Scale As Single

    Private X_Scale As Single = 1
    Private Y_Scale As Single = 1

    Private X_Dpi As Single
    Private Y_Dpi As Single

    Private X_Pen As Pen
    Private Y_Pen As Pen

    Public Viewport As Class_Viewport

    Private WithEvents ctl As PictureBox
    Private Viewport_Ctl As Panel

    Private Parent As Form
    Private Bit_Map As Bitmap
    Private g As Graphics

#End Region

#Region "Properties"

    Public Property Drawing_Scale As Single
        Get
            Return Scale
        End Get

        Set(ByVal value As Single)
            If value > 0.1 Then
                Scale = value
                X_Scale = value * X_Dpi
                Y_Scale = value * Y_Dpi
            End If

        End Set

    End Property

    Public Property Drawing_Rotation As Single

        Get
            Return Rotation
        End Get

        Set(ByVal value As Single)
            Rotation = value
        End Set

    End Property

    Public Property Bottom As Integer
        Get
            Return ctl.Top + ctl.Height - Viewport.Height
        End Get

        Set(ByVal value As Integer)
            ctl.Top = Viewport.Height - ctl.Height - value
        End Set

    End Property

#End Region

#Region "Events"

    Public Event Redraw()

#End Region

#Region "New"

    Public Sub New(Parent_Form As Form, Canvas_Control As PictureBox, View_Port_Control As Panel, _
                   Width_Inches As Single, Height_Inches As Single)

        ctl = Canvas_Control
        Parent = Parent_Form
        Viewport = New Class_Viewport(View_Port_Control)

        Scale = 1
        Width_In = Width_Inches
        Height_In = Height_Inches

        g = ctl.CreateGraphics()
        X_Dpi = g.DpiX
        Y_Dpi = g.DpiY

        Width = Width_In * X_Dpi * Scale
        Height = Height_In * Y_Dpi * Scale
        ctl.Width = Width
        ctl.Height = Height

        Bit_Map = New Bitmap(Width, Height, g)
        g.Dispose()

        Drawing_Scale = 1
        Pen_Scale = (1 / X_Dpi) / Scale

        RemoveHandler Parent.MouseWheel, AddressOf Parent_Mouse_Wheel_Handler
        AddHandler Parent.MouseWheel, AddressOf Parent_Mouse_Wheel_Handler

        RemoveHandler ctl.MouseDown, AddressOf Mouse_Down_Handler
        AddHandler ctl.MouseDown, AddressOf Mouse_Down_Handler

        RemoveHandler ctl.MouseMove, AddressOf Mouse_Move_Handler
        AddHandler ctl.MouseMove, AddressOf Mouse_Move_Handler

        RemoveHandler ctl.MouseUp, AddressOf Mouse_Up_Handler
        AddHandler ctl.MouseUp, AddressOf Mouse_Up_Handler
    End Sub

#End Region

    Public Function Convert_Y(Y) As Integer
        Return ctl.Height - Y
    End Function

    Public Sub Reset_Drawing()
        Drawing_Scale = 1
        Drawing_Rotation = 0
        Move_Canvas_Origin_To(0, 0)
    End Sub

    Public Overloads Sub Fit()
        Scale = 1
        Rotation = 0
        RaiseEvent Redraw()
        Move_Canvas_Origin_To(0, 0)
    End Sub

    Public Sub Rotate(Right As Boolean)
        Dim Center_Pt As New Point

        Center_Pt = Canvas_Location_At_Viewpoint_Center()

        If Right Then
            Drawing_Rotation += 90
            Bit_Map.RotateFlip(RotateFlipType.Rotate90FlipNone)
        Else
            Drawing_Rotation -= 90
            Bit_Map.RotateFlip(RotateFlipType.Rotate270FlipNone)
        End If

        If Drawing_Rotation >= 360 Or Drawing_Rotation <= -360 Then
            Drawing_Rotation = 0
        End If

        Move_Canvas_Pt_To_Viewport_Center(Center_Pt.Y, Center_Pt.X)

        ctl.Image = Bit_Map
    End Sub

#Region "Location Routines"

    Public Function Canvas_Location_At_Viewpoint_Center() As Point
        Dim Pt As Point
        Pt.X = Left + Viewport.Center.X
        Pt.Y = Bottom + Viewport.Center.Y
        Return Pt
    End Function

    Private Sub Location_Changed(sender As Object, e As System.EventArgs) Handles ctl.LocationChanged
        Left = ctl.Left
    End Sub

    Public Sub Move_Canvas_Origin_To(X As Integer, Y As Integer)
        Bottom = Y
        ctl.Left = X
    End Sub

    Public Sub Move_Canvas_Center_To(X As Integer, Y As Integer)
        ctl.Top = Y - ctl.Height + Viewport.Height
        ctl.Left = X
    End Sub

    Public Sub Move_Canvas_Pt_To_Viewport_Center(X As Integer, Y As Integer)
        Left = X - Viewport.Center.X
        Bottom = Y - Viewport.Center.Y
        Move_Canvas_Origin_To(Left, Bottom)
    End Sub


    Public Function Location_Inches(Pt As Point) As struct_Inch_Point
        Dim P As struct_Inch_Point
        P.X = Pt.X / Scale / X_Dpi
        P.Y = Pt.Y / Scale / Y_Dpi
        Return P
    End Function

#End Region

#Region "Drawing Routines"

    Public Sub Start_Drawing()
        g = Graphics.FromImage(Bit_Map)
    End Sub

    Public Sub Finish_Drawing()
        Bit_Map.RotateFlip(RotateFlipType.RotateNoneFlipY)
        ctl.Image = Bit_Map
        g.Dispose()
    End Sub

    Public Sub Clear_Drawing()
        g.Clear(ctl.BackColor)
    End Sub

    Public Sub Draw_Line(Pen_Color As Pen, X1 As Single, Y1 As Single, X2 As Single, Y2 As Single)
        g.DrawLine(Pen_Color, CInt(X1 * X_Scale), CInt(Y1 * Y_Scale), CInt(X2 * X_Scale), CInt(Y2 * X_Scale))
    End Sub

    Public Sub Draw_Arc(Pen_Color As Pen, X_Start As Single, Y_Start As Single, Width As Single, Height As Single, Deg_Start As Single, Deg_End As Single)
        Dim Deg_Sweep As Single = Deg_End - Deg_Start
        g.DrawArc(Pen_Color, -X_Start, Y_Start, Width, Height, Deg_Start, Deg_End)
    End Sub

    Public Sub Draw_String(Text As String, X As Single, Y As Single)
        g.DrawString(Text, New Font("Arial", 10), Brushes.Red, New PointF(X, Y))
    End Sub

#End Region

#Region "Mouse Handlers"

    Private Dragging As Boolean = False
    Private Windowing As Boolean = False
    Private Canvas_Rectangle As New Rectangle
    Private Start_Point As Point
    Private Last_Delta As Point
    Private Last_Move_Pt As Point

    Private Sub Parent_Mouse_Wheel_Handler(sender As Object, e As MouseEventArgs)
        If e.Delta > 0 Then
            Scale += 0.1
        Else
            Scale -= 0.1
        End If
        If Scale <= 0 Then Scale = 0.1

        X_Scale = Scale * X_Dpi
        Y_Scale = Scale * Y_Dpi

        RaiseEvent Redraw()
    End Sub

    Private Sub Mouse_Down_Handler(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
        If (e.Button = MouseButtons.Left) Then Dragging = True
        If (e.Button = MouseButtons.Right) Then Windowing = True

        If Dragging And Windowing Then
            Dragging = False
            Windowing = False
            Exit Sub
        End If

        Dim control As Control = CType(sender, Control)
        Start_Point = control.PointToScreen(New Point(e.X, e.Y))

        If Dragging Then
            Last_Move_Pt.X = e.X
            Last_Move_Pt.Y = e.Y
            Last_Delta.X = 0
            Last_Delta.Y = 0
        End If

    End Sub

    Private Sub Mouse_Move_Handler(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)

        If (Dragging) Then
            Last_Delta.X = e.X - Last_Move_Pt.X
            Last_Delta.Y = e.Y - Last_Move_Pt.Y

            ctl.Left += Last_Delta.X
            ctl.Top += Last_Delta.Y

            Last_Move_Pt.X = e.X - Last_Delta.X
            Last_Move_Pt.Y = e.Y - Last_Delta.Y
        End If

        If Windowing Then
            ' Hide the previous rectangle by calling the DrawReversibleFrame  
            ' method with the same parameters.
            ControlPaint.DrawReversibleFrame(Canvas_Rectangle, ctl.BackColor, FrameStyle.Dashed)

            ' Calculate the endpoint and dimensions for the new rectangle,  
            ' again using the PointToScreen method. 
            Dim endPoint As Point = CType(sender, Control).PointToScreen(New Point(e.X, e.Y))
            Dim width As Integer = endPoint.X - Start_Point.X
            Dim height As Integer = endPoint.Y - Start_Point.Y
            Canvas_Rectangle = New Rectangle(Start_Point.X, Start_Point.Y, width, height)

            ' Draw the new rectangle by calling DrawReversibleFrame again.  
            ControlPaint.DrawReversibleFrame(Canvas_Rectangle, ctl.BackColor, FrameStyle.Dashed)
        End If

    End Sub

    Private Sub Mouse_Up_Handler(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)

        If (e.Button = MouseButtons.Left) Then
            Dragging = False
        End If

        If (e.Button = MouseButtons.Right) Then
            ControlPaint.DrawReversibleFrame(Canvas_Rectangle, ctl.BackColor, FrameStyle.Dashed)
            Windowing = False
        End If

        ' Draw the rectangle to be evaluated. Set a dashed frame style  
        ' using the FrameStyle enumeration.

        ' Find out which controls intersect the rectangle and change their color. 
        ' The method uses the RectangleToScreen method to convert the  
        ' Control's client coordinates to screen coordinates. 
        'Dim i As Integer
        'Dim controlRectangle As Rectangle
        'For i = 0 To Controls.Count - 1
        '    controlRectangle = Controls(i).RectangleToScreen(Controls(i).ClientRectangle)
        '    If controlRectangle.IntersectsWith(theRectangle) Then
        '        Controls(i).BackColor = Color.BurlyWood
        '    End If
        'Next

        ' Reset the rectangle.
        Canvas_Rectangle = New Rectangle(0, 0, 0, 0)

    End Sub

#End Region

End Class

