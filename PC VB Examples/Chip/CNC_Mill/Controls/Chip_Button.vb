Imports System.Xml.Serialization
Imports System.ComponentModel


Public Class Chip_Button

#Region "Declarations"

    Public Persistant_Settings As New List(Of String)

    Public Cycle_Step As Integer = 0

    Private Initialized_State As String = ""
    Private Initialized As Boolean = False
    Private _State As String = ""

#End Region

#Region "Load, Initialize"

    Private Sub ctrl_Button_Load(sender As Object, e As EventArgs) Handles Me.Load
        If Not Initialized Then

            Dim _ctl As Button = btn_Chip
            Dim Bm As Bitmap
            _ctl.BackgroundImageLayout = ImageLayout.Stretch
            _ctl.BackgroundImageLayout = ImageLayout.Stretch
            _ctl.FlatStyle = FlatStyle.Flat
            _ctl.FlatAppearance.BorderSize = 0
            _ctl.TabStop = False
            _ctl.FlatAppearance.BorderColor = btn_Chip.Parent.BackColor
            _ctl.FlatAppearance.MouseOverBackColor = btn_Chip.Parent.BackColor
            _ctl.FlatAppearance.MouseDownBackColor = btn_Chip.Parent.BackColor
            _ctl.BackColor = btn_Chip.Parent.BackColor
            Bm = _ctl.BackgroundImage
            If Not IsNothing(Bm) Then Bm.MakeTransparent(Color.Magenta)

            If Not IsNothing(States) Then
                If States.Count > 0 Then
                    If Initialized_State <> "" Then
                        _State = Initialized_State
                    Else
                        If Not IsNothing(States(0).State_Name) Then
                            _State = States(0).State_Name
                        End If
                    End If

                    For I = 0 To States.Count - 1
                        If _State = States(I).State_Name Then
                            If Not IsNothing(States(I).State_Image) Then
                                btn_Chip.BackgroundImage = States(I).State_Image

                                'Dim bm As Bitmap
                                Bm = btn_Chip.BackgroundImage
                                '                                If Not IsNothing(Bm) Then Bm.MakeTransparent(Color.Magenta)
                                If Not IsNothing(Bm) Then Bm.MakeTransparent(Color.Magenta)

                                btn_Chip.ForeColor = States(I).State_Fore_Color
                                btn_Chip.Text = States(I).State_Text
                                btn_Chip.TextAlign = States(I).State_Text_Align
                                btn_Chip.Font = States(I).State_Font
                                Initialized_State = ""
                            End If
                        End If
                    Next
                    Me.Refresh()

                End If
            End If
        End If

    End Sub

    Public Sub Initialize()
        Dim _ctl As Button = btn_Chip
        Dim Bm As Bitmap
        _ctl.BackgroundImageLayout = ImageLayout.Stretch
        _ctl.BackgroundImageLayout = ImageLayout.Stretch
        _ctl.FlatStyle = FlatStyle.Flat
        _ctl.FlatAppearance.BorderSize = 0
        _ctl.TabStop = False
        _ctl.FlatAppearance.BorderColor = btn_Chip.Parent.BackColor
        _ctl.FlatAppearance.MouseOverBackColor = btn_Chip.Parent.BackColor
        _ctl.FlatAppearance.MouseDownBackColor = btn_Chip.Parent.BackColor
        _ctl.BackColor = btn_Chip.Parent.BackColor
        Bm = _ctl.BackgroundImage
        If Not IsNothing(Bm) Then Bm.MakeTransparent(Color.Magenta)

        'State = State 'Force image to be updated
        For I = 0 To States.Count - 1
            If _State = States(I).State_Name Then
                If Not IsNothing(States(I).State_Image) Then
                    btn_Chip.BackgroundImage = States(I).State_Image

                    Bm = btn_Chip.BackgroundImage
                    If Not IsNothing(Bm) Then Bm.MakeTransparent(Color.Magenta)

                    btn_Chip.ForeColor = States(I).State_Fore_Color
                    btn_Chip.Text = States(I).State_Text
                    btn_Chip.TextAlign = States(I).State_Text_Align
                    btn_Chip.Font = States(I).State_Font
                End If
            End If
        Next
        Me.Refresh()

        Fire_Event(Class_Display_Handlers.enum_Event_Type.Initialize)
        Initialized_State = State
        Initialized = True
    End Sub

#End Region

#Region "Properties"

    Public Property State As String
        Get
            Return _State
        End Get
        Set(ByVal value As String)
            If _State <> value Then
                _State = value
                For I = 0 To States.Count - 1
                    If _State = States(I).State_Name Then
                        If Not IsNothing(States(I).State_Image) Then

                            btn_Chip.BackgroundImage = States(I).State_Image
                            Dim bm As Bitmap
                            bm = btn_Chip.BackgroundImage
                            If Not IsNothing(bm) Then bm.MakeTransparent(Color.Magenta)

                            btn_Chip.ForeColor = States(I).State_Fore_Color
                            btn_Chip.Text = States(I).State_Text
                            btn_Chip.TextAlign = States(I).State_Text_Align
                            btn_Chip.Font = States(I).State_Font
                            Initialized_State = ""
                        End If
                    End If
                Next
                Me.Refresh()
            End If
        End Set
    End Property

    Public Property Handler As String

    <EditorBrowsable(EditorBrowsableState.Always)>
    <Browsable(True)>
    <Bindable(True)>
    Public Overrides Property Text As String
        Get
            Return btn_Chip.Text
        End Get
        Set(ByVal value As String)
            btn_Chip.Text = value
        End Set
    End Property

    Public Property Fore_Color As Color
        Get
            Return btn_Chip.ForeColor
        End Get
        Set(ByVal value As Color)
            btn_Chip.ForeColor = value
        End Set
    End Property

    Public Property Text_Align As ContentAlignment
        Get
            Return btn_Chip.TextAlign
        End Get
        Set(ByVal value As ContentAlignment)
            btn_Chip.TextAlign = value
        End Set
    End Property

    <DesignerSerializationVisibility(DesignerSerializationVisibility.Content),
    Browsable(True)>
    Public Property States() As New List(Of Button_State)

#End Region

#Region "Methods"

    Public Sub Fire_Event(ByVal Evnt As Class_Display_Handlers.enum_Event_Type)
        Display.Run_Handler(Me, Evnt, True)
    End Sub


    Public Sub Finish()

    End Sub

#End Region

#Region "Class Button_State"

    Public Class Button_State

        Public Property State_Name() As String
        Public Property State_Image() As Image
        Public Property State_Text() As String
        Public Property State_Font As Font
        Public Property State_Fore_Color As Color
        Public Property State_Text_Align As ContentAlignment

    End Class

#End Region

#Region "Events"

    Private Sub btn_Chip_MouseDown(sender As Object, e As System.Windows.Forms.MouseEventArgs) Handles btn_Chip.MouseDown
        Select Case e.Button
            Case MouseButtons.Left
                Display.Run_Handler(Me, Class_Display_Handlers.enum_Event_Type.Left_Mouse_Button_Down, True)
            Case MouseButtons.Right
                Display.Run_Handler(Me, Class_Display_Handlers.enum_Event_Type.Right_Mouse_Button_Down, True)
            Case MouseButtons.Middle
                Display.Run_Handler(Me, Class_Display_Handlers.enum_Event_Type.Middle_Mouse_Button_Down, True)
        End Select
    End Sub

    Private Sub btn_Chip_MouseUp(sender As Object, e As System.Windows.Forms.MouseEventArgs) Handles btn_Chip.MouseUp
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
