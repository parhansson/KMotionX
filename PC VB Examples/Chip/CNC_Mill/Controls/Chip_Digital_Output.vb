Imports System.ComponentModel

Public Class Chip_Digital_Output

#Region "Declarations"

    Public Persistant_Settings As New List(Of String)
    Public KFlop_Digital_Output As KMotion_dotNet.KM_IO

#End Region

#Region "Load"

    Private Sub ctrl_Button_Load(sender As Object, e As EventArgs) Handles Me.Load
            Dim _ctl As Button = led_Chip
            Dim Bm As Bitmap
            _ctl.BackgroundImageLayout = ImageLayout.Stretch
            _ctl.BackgroundImageLayout = ImageLayout.Stretch
            _ctl.FlatStyle = FlatStyle.Flat
            _ctl.FlatAppearance.BorderSize = 0
            _ctl.TabStop = False
            _ctl.FlatAppearance.BorderColor = led_Chip.Parent.BackColor
            _ctl.FlatAppearance.MouseOverBackColor = led_Chip.Parent.BackColor
            _ctl.FlatAppearance.MouseDownBackColor = led_Chip.Parent.BackColor
            _ctl.BackColor = led_Chip.Parent.BackColor
            Bm = _ctl.BackgroundImage
            If Not IsNothing(Bm) Then Bm.MakeTransparent(Color.Magenta)
            If Not IsNothing(States) Then
                If States.Count > 0 Then
                    If Not IsNothing(States(0).State_Name) Then
                        State = States(0).State_Name
                    End If
                End If
            End If

    End Sub

#End Region

#Region "Properties"

    Private _Output_Number As Integer
    Public Property Output_Number As Integer
        Get
            Return _Output_Number
        End Get
        Set(ByVal value As Integer)
            _Output_Number = value
        End Set
    End Property

    Private _State As String
    Public Property State As String
        Get
            Return _State
        End Get
        Set(ByVal value As String)
            _State = value
            For I = 0 To States.Count - 1
                If _State = States(I).State_Name Then
                    If Not IsNothing(States(I).State_Image) Then
                        led_Chip.BackgroundImage = States(I).State_Image
                        Dim bm As Bitmap
                        bm = led_Chip.BackgroundImage
                        If Not IsNothing(bm) Then bm.MakeTransparent(Color.Magenta)
                        led_Chip.ForeColor = States(I).State_Fore_Color
                        led_Chip.Text = States(I).State_Text
                        led_Chip.TextAlign = States(I).State_Text_Align
                    End If
                End If
            Next
            Me.Refresh()
        End Set
    End Property

    Private _Handler As String
    Public Property Handler As String
            Get
                Return _Handler
            End Get
        Set(ByVal value As String)
            _Handler = value
        End Set
    End Property

    <EditorBrowsable(EditorBrowsableState.Always)>
    <Browsable(True)>
    <DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)>
    <Bindable(True)>
    Public Overrides Property Text As String
        Get
            Return led_Chip.Text
        End Get
        Set(ByVal value As String)
            led_Chip.Text = value
        End Set
    End Property

    Public Property Fore_Color As Color
            Get
                Return led_Chip.ForeColor
            End Get
            Set(ByVal value As Color)
                led_Chip.ForeColor = value
            End Set
        End Property

    Public Property Text_Align As ContentAlignment
        Get
            Return led_Chip.TextAlign
        End Get
        Set(ByVal value As ContentAlignment)
            led_Chip.TextAlign = value
        End Set
    End Property

    Private _States As New List(Of Digital_Output_State)
    <DesignerSerializationVisibility(DesignerSerializationVisibility.Content),
        Description("A list of state names and images."),
        Browsable(True)>
    Public Property States() As List(Of Digital_Output_State)
        Get
            Return _States
        End Get
        Set(ByVal value As List(Of Digital_Output_State))
            _States = value
        End Set
    End Property

#End Region

#Region "Methods"

    Public Sub Fire_Event(ByVal Evnt As Class_Display_Handlers.enum_Event_Type)
        Display.Run_Handler(Me, Evnt, True)
    End Sub

    Public Sub Initialize()
        KFlop_Digital_Output = New KMotion_dotNet.KM_IO(Kflop.Controller, Output_Number, Name, KMotion_dotNet.IO_TYPE.DIGITAL_OUT)
        Fire_Event(Class_Display_Handlers.enum_Event_Type.Initialize)
    End Sub

    Public Sub Finish()

    End Sub

    Public Sub Set_Boolean_Output(Set_On As Boolean, New_State As String)
        KFlop_Digital_Output.SetDigitalValue(Set_On)
        State = New_State
    End Sub

    Public Function Get_Boolean_Output(True_State As String, False_State As String) As String
        If KFlop_Digital_Output.GetDigitalValue Then
            Return True_State
        Else
            Return False_State
        End If
    End Function

#End Region

#Region "Digital_Output_State"

    Public Class Digital_Output_State

        Private _State_Name As String = ""
        Private _State_Image As Image = Nothing
        Private _State_Text As String = ""
        Private _State_Fore_Color As Color = Color.Black
        Private _State_Text_Align As ContentAlignment = ContentAlignment.MiddleCenter

        Public Property State_Name() As String
            Get
                Return _State_Name
            End Get
            Set(ByVal value As String)
                _State_Name = value
            End Set
        End Property

        Public Property State_Image() As Image
            Get
                Return _State_Image
            End Get
            Set(ByVal value As Image)
                _State_Image = value
            End Set
        End Property

        Public Property State_Text() As String
            Get
                Return _State_Text
            End Get
            Set(ByVal value As String)
                _State_Text = value
            End Set
        End Property

        Public Property State_Fore_Color As Color
            Get
                Return _State_Fore_Color
            End Get
            Set(ByVal value As Color)
                _State_Fore_Color = value
            End Set
        End Property

        Public Property State_Text_Align As ContentAlignment
            Get
                Return _State_Text_Align
            End Get
            Set(ByVal value As ContentAlignment)
                _State_Text_Align = value
            End Set
        End Property

    End Class

#End Region

#Region "Events"

    Private Sub btn_Chip_MouseDown(sender As Object, e As System.Windows.Forms.MouseEventArgs) Handles led_Chip.MouseDown
        Select Case e.Button
            Case MouseButtons.Left
                Display.Run_Handler(Me, Class_Display_Handlers.enum_Event_Type.Left_Mouse_Button_Down, True)
            Case MouseButtons.Right
                Display.Run_Handler(Me, Class_Display_Handlers.enum_Event_Type.Right_Mouse_Button_Down, True)
            Case MouseButtons.Middle
                Display.Run_Handler(Me, Class_Display_Handlers.enum_Event_Type.Middle_Mouse_Button_Down, True)
        End Select
    End Sub

    Private Sub btn_Chip_MouseUp(sender As Object, e As System.Windows.Forms.MouseEventArgs) Handles led_Chip.MouseUp
        Try
            Select Case e.Button
                Case MouseButtons.Left
                    Display.Run_Handler(Me, Class_Display_Handlers.enum_Event_Type.Left_Mouse_Button_Up, False)
                Case MouseButtons.Right
                    Display.Run_Handler(Me, Class_Display_Handlers.enum_Event_Type.Right_Mouse_Button_Up, False)
                Case MouseButtons.Middle
                    Display.Run_Handler(Me, Class_Display_Handlers.enum_Event_Type.Middle_Mouse_Button_Up, False)
            End Select
        Catch ex As Exception
        End Try
    End Sub


#End Region

End Class
