Imports System.ComponentModel

Public Class Chip_Text_Box


#Region "Declarations"

    Private Text_Before_Edit As String

    Public Persistant_Settings As New List(Of String)

#End Region


#Region "Properties"

    Private _State As String
    Public Property State As String
        Get
            Return _State
        End Get
        Set(ByVal value As String)
            _State = value
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

    '<EditorBrowsable(EditorBrowsableState.Always)>
    '<Browsable(True)>
    '<DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)>
    '<Bindable(True)>

    <EditorBrowsable(EditorBrowsableState.Always)>
    <Browsable(True)>
    <Bindable(True)>
    Public Overrides Property Text As String
        Get
            Return txt_Chip.Text
        End Get
        Set(ByVal value As String)
            txt_Chip.Text = value
            Me.Refresh()
        End Set
    End Property

    Private _Default_Value As String
    Public Property Default_Value As String
        Get
            Return _Default_Value
        End Get
        Set(ByVal value As String)
            _Default_Value = value
        End Set
    End Property

    Public Property Read_Only As Boolean
        Get
            Return txt_Chip.ReadOnly
        End Get
        Set(ByVal value As Boolean)
            txt_Chip.ReadOnly = value
        End Set
    End Property

    Public Property Back_Color As Color
        Get
            Return txt_Chip.BackColor
        End Get
        Set(ByVal value As Color)
            txt_Chip.BackColor = value
        End Set
    End Property

    Public Property Fore_Color As Color
        Get
            Return txt_Chip.ForeColor
        End Get
        Set(ByVal value As Color)
            txt_Chip.ForeColor = value
        End Set
    End Property

    'Public Property Font As Font
    '    Get
    '        Return txt_Chip.Font
    '    End Get
    '    Set(ByVal value As Font)
    '        txt_Chip.Font = value
    '        Me.Width = txt_Chip.Width + 1
    '        Me.Height = txt_Chip.Height + 1
    '    End Set
    'End Property

    Public Property Chip_Text_Align As HorizontalAlignment
        Get
            Return txt_Chip.TextAlign
        End Get
        Set(ByVal value As HorizontalAlignment)
            txt_Chip.TextAlign = value
        End Set
    End Property

#End Region

#Region "Methods"

    Public Sub Initialize()
        Fire_Event(Class_Display_Handlers.enum_Event_Type.Initialize)
    End Sub

    Public Sub Finish()
        Fire_Event(Class_Display_Handlers.enum_Event_Type.Finish)
    End Sub

    Public Sub Update_State()
        Fire_Event(Class_Display_Handlers.enum_Event_Type.Update)
    End Sub

    Public Sub Fire_Event(ByVal Evnt As Class_Display_Handlers.enum_Event_Type)
        Display.Run_Handler(Me, Evnt, True)
    End Sub

    Public Function Number(Optional Digits As Integer = 4) As Double
        If IsNumeric(txt_Chip.Text) Then
            Return FormatNumber(txt_Chip.Text, Digits)
        Else
            Return 0
        End If
    End Function

#End Region

#Region "Events"

    Private Sub txt_Chip_FontChanged(sender As Object, e As System.EventArgs) Handles txt_Chip.FontChanged
        Me.Width = txt_Chip.Width + 1
        Me.Height = txt_Chip.Height + 1
    End Sub


    Private Sub btn_Chip_MouseDown(sender As Object, e As System.Windows.Forms.MouseEventArgs) Handles txt_Chip.MouseDown
        Text_Before_Edit = txt_Chip.Text
        Select Case e.Button
            Case MouseButtons.Left
                Display.Run_Handler(Me, Class_Display_Handlers.enum_Event_Type.Left_Mouse_Button_Down, True)
            Case MouseButtons.Right
                Display.Run_Handler(Me, Class_Display_Handlers.enum_Event_Type.Right_Mouse_Button_Down, True)
            Case MouseButtons.Middle
                Display.Run_Handler(Me, Class_Display_Handlers.enum_Event_Type.Middle_Mouse_Button_Down, True)
        End Select
    End Sub

    Private Sub txt_Chip_Validating(sender As Object, e As System.ComponentModel.CancelEventArgs) Handles txt_Chip.Validating
        If Not txt_Chip.Modified Then Exit Sub

        If Display.Run_Handler(Me, Class_Display_Handlers.enum_Event_Type.Validate, True) Then
            Text = txt_Chip.Text
        Else
            txt_Chip.Text = Text_Before_Edit
            e.Cancel = True
        End If
    End Sub

#End Region

End Class
