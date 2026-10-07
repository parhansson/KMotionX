Imports System.IO

Public Class Class_Symbols

    '#Region "enums"

    '    Public Enum enum_Properties
    '        Name
    '        Type
    '        Value
    '        Locked
    '        Read_Only
    '        No_Edit

    '        Down_Handler
    '        Up_Handler

    '        Text
    '        Up_Image_File
    '        Down_Image_File
    '        Parent
    '        Top
    '        Left
    '        Width
    '        Height
    '        Back_Color
    '        Text_Color
    '        Text_Align
    '        Font
    '        Enabled
    '        Visible
    '        Comment

    '        Whole
    '        Real
    '        Round_0
    '        Round_1
    '        Round_2
    '        Round_3
    '        Round_4
    '        Fraction

    '        Last
    '    End Enum

    '    Public Enum enum_Symbol_Type
    '        Whole
    '        Real
    '        Logical
    '        Text
    '        Color
    '        Image
    '        Font

    '        Label
    '        Button
    '        Toggle
    '        Text_Box
    '        Check_Box
    '        Radio_Button

    '        GCode
    '        Macro
    '        Command
    '        Evnt
    '        Hot_Key
    '        Tool

    '        Process
    '        Process_GCode
    '        Process_Drawing

    '        TinyG_Setting

    '        Dialog_Box
    '        Dialog_GCode
    '        Dialog_Drawing
    '    End Enum

    '    Public Enum enum_Value_Data_Type
    '        Whole
    '        Real
    '        Round_0
    '        Round_1
    '        Round_2
    '        Round_3
    '        Round_4
    '        Logical
    '        Text
    '    End Enum

    '    'Public Enum enum_Event_Type
    '    '    Form_Control
    '    '    Internal
    '    'End Enum

    '#End Region

    '#Region "Classes"

    '    Public Class class_Symbol
    '        Public Key As String
    '        Public Name As String
    '        Public Type As enum_Symbol_Type
    '        Public No_Edit As Boolean
    '        Public Locked As Boolean
    '        Public Ctrl As Control
    '        Public Down_Handler As String
    '        Public Up_Handler As String
    '        Public Role As String
    '        Public Comment As String
    '        Public Tag As Object

    '        Private Symbol_Value As Object
    '        Private Value_Data_Type As enum_Value_Data_Type

    '        Public Down_Image_File As String
    '        Public Up_Image_File As String

    '        Public Tree_Node As TreeNode
    '        Public Symbol_Table_Row_Index As Integer

    '        Public Property Value() As Object
    '            Get
    '                Return Symbol_Value
    '            End Get
    '            Set(V As Object)
    '                Symbol_Value = V
    '            End Set
    '        End Property

    '        Public Property State() As Boolean

    '            Get
    '                Select Case Type
    '                    Case enum_Symbol_Type.Check_Box
    '                        Dim C As CheckBox = Ctrl
    '                        Symbol_Value = C.Checked
    '                        Return C.Checked
    '                    Case enum_Symbol_Type.Radio_Button
    '                        Dim C As RadioButton = Ctrl
    '                        Symbol_Value = C.Checked
    '                        Return C.Checked
    '                End Select

    '                Return CBool(Symbol_Value)

    '            End Get

    '            Set(V As Boolean)
    '                Symbol_Value = CBool(V)
    '                Select Case Type
    '                    Case enum_Symbol_Type.Check_Box
    '                        Dim C As CheckBox = Ctrl
    '                        C.Checked = V
    '                    Case enum_Symbol_Type.Radio_Button
    '                        Dim C As RadioButton = Ctrl
    '                        C.Checked = V
    '                End Select
    '            End Set

    '        End Property

    '        Public Property Text() As String
    '            Get
    '                If IsNothing(Ctrl) Then
    '                    Return CStr(Symbol_Value)
    '                Else
    '                    Return Ctrl.Text
    '                End If
    '            End Get
    '            Set(V As String)
    '                If Type = enum_Symbol_Type.Text_Box Or Type = enum_Symbol_Type.Text Then
    '                    Symbol_Value = V
    '                End If
    '                If Not IsNothing(Ctrl) Then
    '                    Ctrl.Text = V
    '                End If
    '            End Set
    '        End Property

    '        Public Property Parent() As String
    '            Get
    '                If IsNothing(Ctrl) Then
    '                    Return Nothing
    '                Else
    '                    Return Ctrl.Parent.Name
    '                End If
    '            End Get
    '            Set(V As String)
    '                If Not IsNothing(Ctrl) Then
    '                    Ctrl.Parent.Name = V
    '                End If
    '            End Set
    '        End Property

    '        Public Sub Change_Image(Image_Name As String)
    '            Dim Idx As Integer = Symbol.Image_Table_Keys.IndexOf(Image_Name)
    '            If Idx < 0 Then 'Not in image table, try to load it
    '                Load_Image_File(Image_Name)
    '            Else
    '                Ctrl.BackgroundImage = Symbol.Image_Table(Idx)
    '            End If
    '        End Sub

    '        Public Sub Load_Image_File(File_Name As String)
    '            If File_Name = "" Then Exit Sub

    '            Dim Idx As Integer = Symbol.Image_Table_Keys.IndexOf(File_Name)

    '            If Idx < 0 Then 'Image not yet loaded, add to image table
    '                If File.Exists(Chip_Images_Folder & File_Name) Then
    '                    Dim Bm As New Bitmap(Chip_Images_Folder & File_Name)
    '                    Bm.MakeTransparent(Color.Magenta)
    '                    Symbol.Image_Table.Add(Bm)
    '                    Symbol.Image_Table_Keys.Add(File_Name)
    '                    If Not IsNothing(Ctrl) Then Ctrl.BackgroundImage = Bm
    '                Else
    '                    Show_Error("Image file not found : " & File_Name)
    '                    Exit Sub
    '                End If
    '            Else
    '                If Not IsNothing(Ctrl) Then Ctrl.BackgroundImage = Symbol.Image_Table(Idx)
    '            End If


    '        End Sub

    '        Public Property Number() As Integer
    '            Get
    '                Return CInt(Symbol_Value)
    '            End Get
    '            Set(N As Integer)
    '                Symbol_Value = CInt(N)
    '            End Set
    '        End Property

    '        Public Property Real(Optional Digits As Integer = 3) As Single
    '            Get
    '                Return FormatNumber(Symbol_Value, Digits)
    '            End Get
    '            Set(R As Single)
    '                Symbol_Value = CSng(R)
    '            End Set
    '        End Property

    '        Public Property Top() As Integer
    '            Get
    '                If IsNothing(Ctrl) Then
    '                    Return 0
    '                Else
    '                    Return Ctrl.Top
    '                End If
    '            End Get
    '            Set(V As Integer)
    '                If Not IsNothing(Ctrl) Then
    '                    If V > 0 Then
    '                        Ctrl.Top = V
    '                    Else
    '                        Nop()
    '                    End If
    '                End If
    '            End Set
    '        End Property

    '        Public Property Left() As Integer
    '            Get
    '                If IsNothing(Ctrl) Then
    '                    Return 0
    '                Else
    '                    Return Ctrl.Left
    '                End If
    '            End Get
    '            Set(V As Integer)
    '                If Not IsNothing(Ctrl) Then
    '                    Ctrl.Left = V
    '                End If
    '            End Set
    '        End Property

    '        Public Property Height() As Integer
    '            Get
    '                If IsNothing(Ctrl) Then
    '                    Return 0
    '                Else
    '                    Return Ctrl.Height
    '                End If
    '            End Get
    '            Set(V As Integer)
    '                If Not IsNothing(Ctrl) Then
    '                    Ctrl.Height = V
    '                End If
    '            End Set
    '        End Property

    '        Public Property Width() As Integer
    '            Get
    '                If IsNothing(Ctrl) Then
    '                    Return 0
    '                Else
    '                    Return Ctrl.Width
    '                End If
    '            End Get
    '            Set(V As Integer)
    '                If Not IsNothing(Ctrl) Then
    '                    Ctrl.Width = V
    '                End If
    '            End Set
    '        End Property

    '        Public Overloads Sub Change_Back_Color(Color_Name As String)
    '            Ctrl.BackColor = Color.FromName(Color_Name)
    '        End Sub

    '        Public Overloads Sub Change_Back_Color(Clr As Color)
    '            Ctrl.BackColor = Clr
    '        End Sub

    '        Public Overloads Sub Change_Back_Color(Clr As Integer)
    '            Ctrl.BackColor = Color.FromKnownColor(Clr)
    '        End Sub

    '        Public ReadOnly Property Back_Color() As String
    '            Get
    '                If IsNothing(Ctrl) Then
    '                    Return Color.White.Name
    '                Else
    '                    Return Ctrl.BackColor.Name
    '                End If
    '            End Get
    '        End Property

    '        Public Overloads Sub Change_Text_Color(Color_Name As String)
    '            Ctrl.ForeColor = Color.FromName(Color_Name)
    '        End Sub

    '        Public Overloads Sub Change_Text_Color(Clr As Color)
    '            Ctrl.ForeColor = Clr
    '        End Sub

    '        Public ReadOnly Property Text_Color() As String
    '            Get
    '                If IsNothing(Ctrl) Then
    '                    Return Color.White.Name
    '                Else
    '                    Return Ctrl.ForeColor.Name
    '                End If
    '            End Get
    '        End Property

    '        Public Property Text_Align() As Integer
    '            Get
    '                Select Case Type
    '                    Case enum_Symbol_Type.Text_Box
    '                        Dim C As TextBox = Ctrl
    '                        Return C.TextAlign
    '                    Case enum_Symbol_Type.Button, enum_Symbol_Type.Toggle, enum_Symbol_Type.Label, _
    '                        enum_Symbol_Type.Check_Box, enum_Symbol_Type.Radio_Button
    '                        Dim C As Button = Ctrl
    '                        Return C.TextAlign
    '                    Case Else
    '                        Return 0
    '                End Select
    '            End Get
    '            Set(V As Integer)
    '                Select Case Type
    '                    Case enum_Symbol_Type.Text_Box
    '                        Dim C As TextBox = Ctrl
    '                        Try
    '                            C.TextAlign = V
    '                        Catch ex As Exception
    '                            C.TextAlign = HorizontalAlignment.Left
    '                        End Try
    '                    Case enum_Symbol_Type.Button, enum_Symbol_Type.Toggle, enum_Symbol_Type.Label, _
    '                        enum_Symbol_Type.Check_Box, enum_Symbol_Type.Radio_Button
    '                        Dim C As Button = Ctrl
    '                        Try
    '                            C.TextAlign = V
    '                        Catch ex As Exception
    '                            C.TextAlign = ContentAlignment.MiddleCenter
    '                        End Try
    '                End Select
    '            End Set

    '        End Property

    '        Public Property Font() As String
    '            Get
    '                Return Ctrl.Font.Name & ", " & Ctrl.Font.Size & "," & Ctrl.Font.Style.ToString
    '            End Get
    '            Set(Name As String)
    '                Dim S() As String = Name.Split(",")

    '                Select Case S(2)
    '                    Case "Regular"
    '                        Ctrl.Font = New Font(S(0), CSng(S(1)), FontStyle.Regular)
    '                    Case "Bold"
    '                        Ctrl.Font = New Font(S(0), CSng(S(1)), FontStyle.Bold)
    '                    Case "Italic"
    '                        Ctrl.Font = New Font(S(0), CSng(S(1)), FontStyle.Italic)
    '                    Case "Strikeout"
    '                        Ctrl.Font = New Font(S(0), CSng(S(1)), FontStyle.Strikeout)
    '                    Case "Underline"
    '                        Ctrl.Font = New Font(S(0), CSng(S(1)), FontStyle.Underline)
    '                End Select
    '                'End If
    '            End Set
    '        End Property

    '        Public Property Read_Only As Boolean
    '            Get
    '                If IsNothing(Ctrl) Then
    '                    Return False
    '                Else
    '                    Dim C As TextBox = Ctrl
    '                    Return C.ReadOnly
    '                End If
    '            End Get
    '            Set(Value As Boolean)
    '                If Not IsNothing(Ctrl) Then
    '                    Dim C As TextBox = Ctrl
    '                    C.ReadOnly = Value
    '                End If
    '            End Set
    '        End Property

    '        Public Property Enabled() As Boolean
    '            Get
    '                If IsNothing(Ctrl) Then
    '                    Return False
    '                Else
    '                    Return Ctrl.Enabled
    '                End If
    '            End Get

    '            Set(V As Boolean)
    '                If Not IsNothing(Ctrl) Then
    '                    Ctrl.Enabled = V
    '                End If
    '            End Set

    '        End Property

    '        Public Property Visible() As Boolean
    '            Get
    '                If IsNothing(Ctrl) Then
    '                    Return False
    '                Else
    '                    Return Ctrl.Visible
    '                End If
    '            End Get
    '            Set(V As Boolean)
    '                If Not IsNothing(Ctrl) Then
    '                    Ctrl.Visible = V
    '                End If
    '            End Set
    '        End Property

    '    End Class

    '#End Region

    '#Region "Declarations"

    '    Public Symbol_Table As New List(Of class_Symbol)
    '    Public Symbol_Table_Keys As New List(Of String)

    '    Public Image_Table As New List(Of Bitmap)
    '    Public Image_Table_Keys As New List(Of String)

    '    Private Setting_Index As Integer = 0
    '    Private First_Setting_Index As Integer = 0

    '    Private Top_Panel As Panel = Main_Form.Controls("Panel_Top")
    '    Private Bottom_Panel As Panel = Main_Form.Controls("Panel_Bottom")
    '    Private Main_Tab As TabControl = Bottom_Panel.Controls("Tab_Main")

    '#End Region

    '#Region "Initialize"

    '    Public Sub Initialize()
    '        Init_Property_List()
    '    End Sub

    '    Public Property_List As New List(Of String)

    '    Private Sub Init_Property_List()
    '        Dim I As enum_Properties
    '        Property_List.Clear()
    '        For I = 0 To enum_Properties.Last - 1
    '            Property_List.Add(UCase(I.ToString))
    '        Next
    '    End Sub

    '    Public Function Property_Index(Prop As String) As Integer
    '        Return Property_List.IndexOf(UCase(Prop))
    '    End Function


    '    Public Function Get_Control_From_Main_Form(Name As String) As Control

    '        Dim Ctrl As Control = Nothing

    '        Name = Name.Replace(".", "_")

    '        If Top_Panel.Controls.ContainsKey(Name) Then
    '            Ctrl = Top_Panel.Controls.Item(Name)
    '        Else
    '            For I = 0 To Main_Tab.TabPages.Count - 1
    '                If Main_Tab.TabPages(I).Controls.ContainsKey(Name) Then
    '                    Ctrl = Main_Tab.TabPages(I).Controls.Item(Name)
    '                    Exit For
    '                End If
    '            Next
    '        End If

    '        If IsNothing(Ctrl) Then Show_Error("Control not found : " & Name)

    '        Return Ctrl

    '    End Function

    '#End Region

    '#Region "Get_Type, Get_Value, Set_Value, Get_Symbol"

    '    Public Function Get_Type(Type As String) As enum_Symbol_Type
    '        Dim T As New enum_Symbol_Type

    '        Select Case Type
    '            Case "Whole"
    '                T = enum_Symbol_Type.Whole
    '            Case "Real"
    '                T = enum_Symbol_Type.Real
    '            Case "Logical"
    '                T = enum_Symbol_Type.Logical
    '            Case "Text"
    '                T = enum_Symbol_Type.Text
    '            Case "Color"
    '                T = enum_Symbol_Type.Color
    '            Case "Image"
    '                T = enum_Symbol_Type.Image
    '            Case "Font"
    '                T = enum_Symbol_Type.Font
    '            Case "Label"
    '                T = enum_Symbol_Type.Label
    '            Case "Button"
    '                T = enum_Symbol_Type.Button
    '            Case "Toggle"
    '                T = enum_Symbol_Type.Toggle
    '            Case "Text_Box"
    '                T = enum_Symbol_Type.Text_Box
    '            Case "Check_Box"
    '                T = enum_Symbol_Type.Check_Box
    '            Case "Radio_Button"
    '                T = enum_Symbol_Type.Radio_Button
    '            Case "GCode"
    '                T = enum_Symbol_Type.GCode
    '            Case "Macro"
    '                T = enum_Symbol_Type.Macro
    '            Case "Command"
    '                T = enum_Symbol_Type.Command
    '            Case "Evnt"
    '                T = enum_Symbol_Type.Evnt
    '            Case "Hot_Key"
    '                T = enum_Symbol_Type.Hot_Key
    '            Case "Tool"
    '                T = enum_Symbol_Type.Tool
    '            Case "Process"
    '                T = enum_Symbol_Type.Process
    '            Case "Process_GCode"
    '                T = enum_Symbol_Type.Process_GCode
    '            Case "Process_Drawing"
    '                T = enum_Symbol_Type.Process_Drawing
    '            Case "Dialog_Box"
    '                T = enum_Symbol_Type.Dialog_Box
    '            Case "Dialog_GCode"
    '                T = enum_Symbol_Type.Dialog_GCode
    '            Case "Dialog_Drawing"
    '                T = enum_Symbol_Type.Dialog_Drawing
    '            Case "TinyG_Setting"
    '                T = enum_Symbol_Type.TinyG_Setting
    '        End Select

    '        Return T

    '    End Function

    '    Public Function Get_Property(Source_Name As String, Optional Complain As Boolean = True) As Object
    '        Dim Idx As Integer
    '        Dim Root_Name As String = ""
    '        Dim Property_Name As String = ""
    '        Dim Source_Sym As class_Symbol

    '        Dim D() As String = Source_Name.Split(".")
    '        Property_Name = D(D.Length - 1)

    '        If D.Length = 1 Then
    '            Show_Error("Unknown property :" & Source_Name)
    '            Return Nothing
    '        End If

    '        Idx = Property_Index(Property_Name)
    '        If Idx < 0 Then
    '            Show_Error("Unknown property :" & Source_Name)
    '            Return Nothing
    '        End If

    '        Root_Name = Source_Name.Replace("." & Property_Name, "")

    '        Source_Sym = Get_Symbol(Root_Name)
    '        If IsNothing(Source_Sym) Then
    '            If IsNothing(Source_Sym) Then
    '                If Complain Then Show_Error("Symbol not found : " & Source_Name)
    '                Return Nothing
    '            End If
    '        End If

    '        Select Case Idx
    '            Case enum_Properties.Name
    '                Return Source_Sym.Name
    '            Case enum_Properties.Type
    '                Return Source_Sym.Type
    '            Case enum_Properties.Value
    '                Return Source_Sym.Value

    '            Case enum_Properties.Whole
    '                If IsNumeric(Source_Sym.Value) Then
    '                    Return CInt(Source_Sym.Value)
    '                Else
    '                    Return 0
    '                End If

    '            Case enum_Properties.Real
    '                If IsNumeric(Source_Sym.Value) Then
    '                    Return CSng(Source_Sym.Value)
    '                Else
    '                    Return 0
    '                End If

    '            Case enum_Properties.Round_0, enum_Properties.Round_1, enum_Properties.Round_2, enum_Properties.Round_3, enum_Properties.Round_4
    '                Dim Digits As Integer = Idx - enum_Properties.Round_0

    '                Dim T As String = Source_Sym.Value.GetType.ToString
    '                If T = "System.String" Then
    '                    If IsNumeric(Source_Sym.Value) Then
    '                        Return CSng(Math.Round(CSng(Source_Sym.Value), Digits))
    '                    Else
    '                        Return CSng(0)
    '                    End If
    '                Else
    '                    Return CSng(Math.Round(CSng(Source_Sym.Value), Digits))
    '                End If

    '            Case enum_Properties.Read_Only
    '                Return Source_Sym.Read_Only
    '            Case enum_Properties.No_Edit
    '                Return Source_Sym.No_Edit
    '            Case enum_Properties.Locked
    '                Return Source_Sym.Locked
    '            Case enum_Properties.Down_Handler
    '                Return Source_Sym.Down_Handler
    '            Case enum_Properties.Up_Handler
    '                Return Source_Sym.Up_Handler
    '            Case enum_Properties.Text
    '                Return Source_Sym.Text
    '            Case enum_Properties.Down_Image_File
    '                Return Source_Sym.Down_Image_File
    '            Case enum_Properties.Up_Image_File
    '                Return Source_Sym.Up_Image_File
    '            Case enum_Properties.Parent
    '                Return Source_Sym.Parent
    '            Case enum_Properties.Top
    '                Return Source_Sym.Top
    '            Case enum_Properties.Left
    '                Return Source_Sym.Left
    '            Case enum_Properties.Height
    '                Return Source_Sym.Height
    '            Case enum_Properties.Width
    '                Return Source_Sym.Width
    '            Case enum_Properties.Back_Color
    '                Return Source_Sym.Back_Color
    '            Case enum_Properties.Text_Color
    '                Return Source_Sym.Text_Color
    '            Case enum_Properties.Text_Align
    '                Return Source_Sym.Text_Align
    '            Case enum_Properties.Font
    '                Return Source_Sym.Font
    '            Case enum_Properties.Enabled
    '                Return Source_Sym.Enabled
    '            Case enum_Properties.Visible
    '                Return Source_Sym.Visible
    '            Case enum_Properties.Comment
    '                Return Source_Sym.Comment
    '            Case Else
    '                Return Nothing
    '        End Select

    '    End Function

    '    Public Sub Set_Property(Dest_Name As String, Value As Object)
    '        Dim Idx As Integer
    '        Dim Property_Name As String = ""
    '        Dim Root_Name As String = ""
    '        Dim Dest_Sym As class_Symbol
    '        Dim D() As String = Dest_Name.Split(".")

    '        Property_Name = D(D.Length - 1)
    '        Idx = Property_Index(Property_Name)

    '        If Idx < 0 Then
    '            Show_Error("Unknown property :" & Property_Name)
    '            Exit Sub
    '        End If

    '        Root_Name = Dest_Name.Replace("." & Property_Name, "")

    '        Dest_Sym = Get_Symbol(Root_Name)
    '        If IsNothing(Dest_Sym) Then
    '            If IsNothing(Dest_Sym) Then
    '                Show_Error("Symbol not found : " & Dest_Name)
    '                Exit Sub
    '            End If
    '        End If

    '        Select Case Idx

    '            Case enum_Properties.Value
    '                If Dest_Sym.Type = enum_Symbol_Type.Text_Box Then Dest_Sym.Ctrl.Text = Value
    '                Dest_Sym.Value = Value

    '            Case enum_Properties.Whole
    '                Select Case Dest_Sym.Type
    '                    Case enum_Symbol_Type.Text
    '                        Dest_Sym.Value = Math.Truncate(CSng(Value))
    '                    Case enum_Symbol_Type.Text_Box
    '                        Dest_Sym.Value = Math.Truncate(CSng(Value))
    '                        Dest_Sym.Ctrl.Text = Dest_Sym.Value
    '                    Case Else
    '                        Dest_Sym.Value = Math.Truncate(CSng(Value))
    '                End Select

    '            Case enum_Properties.Real
    '                Dest_Sym.Value = CStr(Value)
    '                Select Case Dest_Sym.Type
    '                    Case enum_Symbol_Type.Text
    '                        Dest_Sym.Value = FormatNumber(CSng(Value), 3)
    '                    Case enum_Symbol_Type.Text_Box
    '                        Dest_Sym.Value = FormatNumber(CSng(Value), 3)
    '                        Dest_Sym.Ctrl.Text = Dest_Sym.Value
    '                    Case Else
    '                        Dest_Sym.Value = Math.Round(CSng(Value), 3)
    '                End Select

    '            Case enum_Properties.Round_0
    '                Select Case Dest_Sym.Type
    '                    Case enum_Symbol_Type.Text
    '                        Dest_Sym.Value = FormatNumber(CSng(Value), 0)
    '                    Case enum_Symbol_Type.Text_Box
    '                        Dest_Sym.Value = FormatNumber(CSng(Value), 0)
    '                        Dest_Sym.Ctrl.Text = Dest_Sym.Value
    '                    Case Else
    '                        Dest_Sym.Value = Math.Round(CSng(Value), 0)
    '                End Select

    '            Case enum_Properties.Round_1
    '                Select Case Dest_Sym.Type
    '                    Case enum_Symbol_Type.Text
    '                        Dest_Sym.Value = FormatNumber(CSng(Value), 1)
    '                    Case enum_Symbol_Type.Text_Box
    '                        Dest_Sym.Value = FormatNumber(CSng(Value), 1)
    '                        Dest_Sym.Ctrl.Text = Dest_Sym.Value
    '                    Case Else
    '                        Dest_Sym.Value = Math.Round(CSng(Value), 1)
    '                End Select

    '            Case enum_Properties.Round_2
    '                Select Case Dest_Sym.Type
    '                    Case enum_Symbol_Type.Text
    '                        Dest_Sym.Value = FormatNumber(CSng(Value), 2)
    '                    Case enum_Symbol_Type.Text_Box
    '                        Dest_Sym.Value = FormatNumber(CSng(Value), 2)
    '                        Dest_Sym.Ctrl.Text = Dest_Sym.Value
    '                    Case Else
    '                        Dest_Sym.Value = Math.Round(CSng(Value), 2)
    '                End Select

    '            Case enum_Properties.Round_3
    '                Select Case Dest_Sym.Type
    '                    Case enum_Symbol_Type.Text
    '                        Dest_Sym.Value = FormatNumber(CSng(Value), 3)
    '                    Case enum_Symbol_Type.Text_Box
    '                        Dest_Sym.Value = FormatNumber(CSng(Value), 3)
    '                        Dest_Sym.Ctrl.Text = Dest_Sym.Value
    '                    Case Else
    '                        Dest_Sym.Value = Math.Round(CSng(Value), 3)
    '                End Select

    '            Case enum_Properties.Round_4
    '                Select Case Dest_Sym.Type
    '                    Case enum_Symbol_Type.Text
    '                        Dest_Sym.Value = FormatNumber(CSng(Value), 1)
    '                    Case enum_Symbol_Type.Text_Box
    '                        Dest_Sym.Value = FormatNumber(CSng(Value), 1)
    '                        Dest_Sym.Ctrl.Text = Dest_Sym.Value
    '                    Case Else
    '                        Dest_Sym.Value = Math.Round(CSng(Value), 1)
    '                End Select

    '            Case enum_Properties.Fraction
    '                'Select Case Dest_Sym.Type
    '                '    Case enum_Symbol_Type.Text
    '                '        Dest_Sym.Value = FormatNumber(csng(Value), 1)
    '                '    Case enum_Symbol_Type.Text_Box
    '                '        Dest_Sym.Value = FormatNumber(csng(Value), 1)
    '                '        Dest_Sym.Ctrl.Text = Dest_Sym.Value
    '                '    Case Else
    '                '        Dest_Sym.Value = Math.Round(csng(Value), 1)
    '                'End Select

    '            Case enum_Properties.Read_Only
    '                Dest_Sym.Read_Only = Value
    '            Case enum_Properties.No_Edit
    '                Dest_Sym.No_Edit = Value
    '            Case enum_Properties.Locked
    '                Dest_Sym.Locked = Value
    '            Case enum_Properties.Down_Handler
    '                Dest_Sym.Down_Handler = Value
    '            Case enum_Properties.Up_Handler
    '                Dest_Sym.Up_Handler = Value
    '            Case enum_Properties.Text
    '                If Dest_Sym.Type = enum_Symbol_Type.Text_Box Then Dest_Sym.Ctrl.Text = Value
    '                Dest_Sym.Text = Value
    '            Case enum_Properties.Down_Image_File
    '                Dest_Sym.Down_Image_File = Value
    '            Case enum_Properties.Up_Image_File
    '                Dest_Sym.Up_Image_File = Value
    '            Case enum_Properties.Parent
    '                Dest_Sym.Parent = Value
    '            Case enum_Properties.Top
    '                Dest_Sym.Top = Value
    '            Case enum_Properties.Left
    '                Dest_Sym.Left = Value
    '            Case enum_Properties.Height
    '                Dest_Sym.Height = Value
    '            Case enum_Properties.Width
    '                Dest_Sym.Width = Value
    '            Case enum_Properties.Back_Color
    '                Dest_Sym.Change_Back_Color(Value)
    '            Case enum_Properties.Text_Color
    '                Dest_Sym.Change_Text_Color(Value)
    '            Case enum_Properties.Text_Align
    '                Dest_Sym.Text_Align = Value
    '            Case enum_Properties.Font
    '                Dest_Sym.Font = Value
    '            Case enum_Properties.Enabled
    '                Dest_Sym.Enabled = Value
    '            Case enum_Properties.Visible
    '                Dest_Sym.Visible = Value
    '            Case enum_Properties.Comment
    '                Dest_Sym.Comment = Value
    '        End Select

    '    End Sub

    '    Public Function Get_Value(Source_Name As String) As Object
    '        Dim Source_Sym As class_Symbol = Get_Symbol(Source_Name)

    '        If IsNothing(Source_Sym) Then
    '            Return Get_Property(Source_Name)
    '        Else
    '            Select Case Source_Sym.Type
    '                Case enum_Symbol_Type.Whole
    '                    Return CInt(Source_Sym.Value)

    '                Case enum_Symbol_Type.Real
    '                    Return CSng(Source_Sym.Value)

    '                Case enum_Symbol_Type.Logical
    '                    Return CBool(Source_Sym.Value)

    '                Case enum_Symbol_Type.Text
    '                    Return CStr(Source_Sym.Value)

    '                Case Else
    '                    Return Source_Sym.Value
    '            End Select
    '        End If

    '    End Function

    '    Public Sub Set_Value(Dest_Name As String, Value As Object)
    '        Dim Dest_Sym As class_Symbol = Get_Symbol(Dest_Name)

    '        If IsNothing(Dest_Sym) Then
    '            Set_Property(Dest_Name, Value)
    '        Else
    '            If Dest_Sym.Type = enum_Symbol_Type.Text_Box Or Dest_Sym.Type = enum_Symbol_Type.Text Then
    '                If Not IsNothing(Dest_Sym.Ctrl) Then
    '                    Dest_Sym.Ctrl.Text = Value
    '                End If
    '            End If
    '            Dest_Sym.Value = Value
    '        End If

    '    End Sub

    '    Public Sub Set_Equal(Dest_Name As String, Source_Name As String)
    '        Dim Source_Value As Object = Get_Value(Source_Name)
    '        Dim Dest_Value As Object = Get_Value(Dest_Name)

    '        If IsNothing(Source_Value) Or IsNothing(Dest_Value) Then
    '            Exit Sub
    '        Else
    '            Set_Value(Dest_Name, Source_Value)
    '        End If

    '    End Sub

    '    Public Function Get_Symbol(Name As String, Optional Show_Error_Message As Boolean = False) As class_Symbol
    '        Dim Index As Integer = Symbol_Table_Keys.IndexOf(UCase(Name))
    '        If Index >= 0 And Index < Symbol_Table.Count Then
    '            Return Symbol_Table(Index)
    '        Else
    '            If Show_Error_Message Then Show_Error("Symbol does not exist : " & Name)
    '            Return Nothing
    '        End If
    '    End Function

    '    Public Sub Change_Image(Dest_Name As String, File_Name As String)
    '        Dim Dest_Sym As class_Symbol = Get_Symbol(Dest_Name)
    '        If IsNothing(Dest_Sym) Then Exit Sub
    '        Dest_Sym.Change_Image(File_Name)
    '    End Sub

    '#End Region

    '#Region "TinyG Download"

    '    Public Function Get_First_Setting() As String
    '        Setting_Index = 0

    '        For I = 0 To Symbol.Symbol_Table.Count - 1
    '            If Symbol.Symbol_Table(I).Type = enum_Symbol_Type.TinyG_Setting Then
    '                Setting_Index = I
    '                First_Setting_Index = I
    '                Return Symbol.Symbol_Table(I).Value
    '            End If
    '        Next

    '        Return Nothing

    '    End Function

    '    Public Function Get_Next_Setting() As String
    '        'Dim Params() As String

    '        Setting_Index += 1

    '        If Setting_Index > Symbol.Symbol_Table.Count - 1 Then
    '            Return ""
    '        Else
    '            Return Symbol.Symbol_Table(Setting_Index).Value
    '        End If

    '    End Function

    '    Public Function Get_Setting_Value(Name As String) As String
    '        Dim S() As String

    '        For I = First_Setting_Index To Symbol.Symbol_Table.Count - 1
    '            If Symbol.Symbol_Table(I).Type = enum_Symbol_Type.TinyG_Setting Then
    '                S = Symbol.Symbol_Table(I).Value.Split(":")
    '                S(0) = S(0).Replace("{", "")
    '                S(1) = S(1).Replace("}", "")
    '                If S(0) = Name Then
    '                    Return S(1)
    '                End If
    '            End If
    '        Next

    '        Return ""

    '    End Function

    '#End Region

    '#Region "Conversion routines"

    '    Private Function Convert_To_Integer(Value As Object) As Integer
    '        If IsNothing(Value) Then Return 0
    '        Dim X As Integer
    '        Try
    '            X = CInt(Value)
    '        Catch ex As Exception
    '            Show_Error("Cannot convert symbol to integer : " & Value)
    '            Return 0
    '        End Try
    '        Return X
    '    End Function

    '    Private Function Convert_To_Single(Value As Object) As Single
    '        If IsNothing(Value) Then Return 0
    '        Dim X As Single
    '        Try
    '            X = Calc.Round(CSng(Value))
    '        Catch ex As Exception
    '            Show_Error("Cannot convert symbol to single : " & Value)
    '            Return 0
    '        End Try
    '        Return X
    '    End Function

    '    Private Function Convert_To_String(Value As Object) As String
    '        If IsNothing(Value) Then Return ""
    '        Dim S As String
    '        Try
    '            S = Value
    '        Catch ex As Exception
    '            Show_Error("Cannot convert symbol to string : " & Value)
    '            Return ""
    '        End Try
    '        Return S
    '    End Function

    '    Private Function Convert_To_Boolean(Value As Object) As Boolean
    '        If IsNothing(Value) Then Return False
    '        Dim B As Boolean
    '        Try
    '            B = Value
    '        Catch ex As Exception
    '            Show_Error("Cannot convert symbol to boolean : " & Value)
    '            Return False
    '        End Try
    '        Return B
    '    End Function

    '    Private Function Convert_To_Color(Value As Object) As Color
    '        If IsNothing(Value) Then Return Color.White
    '        Dim C As Color
    '        Try
    '            C = Color.FromKnownColor(Value)
    '        Catch ex As Exception
    '            Show_Error("Cannot convert symbol to solor : " & Value)
    '            Return Color.White
    '        End Try
    '        Return C
    '    End Function

    '#End Region

    '#Region "Symbol Table functions (add, delete, insert etc.)"

    '    Public Function Symbol_In_Table(Name As String, Optional Message As String = "") As Boolean
    '        If Symbol_Table_Keys.Contains(UCase(Name)) Then
    '            If Message <> "" Then Show_Error("Symbol already exists : " & Name & ", " & Message)
    '            Return True
    '        End If
    '        Return False
    '    End Function

    '    Public Sub Add_Symbol(Sym As class_Symbol)
    '        Sym.Key = UCase(Sym.Name)
    '        Symbol_Table_Keys.Add(Sym.Key)
    '        Symbol_Table.Add(Sym)
    '    End Sub

    '    Public Sub Insert_Symbol(Sym As class_Symbol, Index As Integer)
    '        Sym.Key = UCase(Sym.Name)
    '        Symbol_Table_Keys.Insert(Index, Sym.Key)
    '        Symbol_Table.Insert(Index, Sym)
    '    End Sub

    '    Public Overloads Sub Delete_Symbol(Sym As class_Symbol)
    '        Symbol_Table.Remove(Sym)
    '    End Sub

    '    Public Overloads Sub Delete_Symbol(Index As Integer)
    '        Symbol_Table.RemoveAt(Index)
    '    End Sub

    '    Public Overloads Sub Delete_Symbol(Name As String)
    '        Dim Sym As class_Symbol
    '        Sym = Get_Symbol(Name, False)
    '        Symbol_Table.Remove(Sym)
    '    End Sub

    '    Public Function Create_Symbol(Name As String, Value As Object) As class_Symbol
    '        Dim Sym As New class_Symbol
    '        Sym = New class_Symbol
    '        Sym.Name = Name

    '        Sym.Value = Value
    '        Sym.Symbol_Table_Row_Index = Symbol_Table.Count
    '        Add_Symbol(Sym)
    '        Return Sym

    '    End Function

    '    Private Sub Add_Symbol_To_Table(Buffer As String)
    '        Dim Params() As String = Buffer.Split("~")
    '        Dim Sym As class_Symbol
    '        Dim Tool_Tip As New ToolTip
    '        Dim Dont_Change_Controls_Properties As Boolean = True

    '        If Params(0).Contains("*") Then
    '            Params(0) = Params(0).Replace("*", "")
    '            Dont_Change_Controls_Properties = True
    '        End If

    '        Tool_Tip.InitialDelay = 100
    '        Tool_Tip.AutoPopDelay = 5000
    '        Tool_Tip.ReshowDelay = 100

    '        Sym = Create_Symbol(Params(enum_Properties.Name), Params(enum_Properties.Name))
    '        Sym.Type = Get_Type(Params(enum_Properties.Type))

    '        Select Case Sym.Type
    '            Case enum_Symbol_Type.Whole
    '                Sym.Value = Convert_To_Integer(Params(enum_Properties.Value))
    '            Case enum_Symbol_Type.Real
    '                Sym.Value = Convert_To_Single(Params(enum_Properties.Value))
    '            Case enum_Symbol_Type.Logical
    '                Sym.Value = Convert_To_Boolean(Params(enum_Properties.Value))
    '            Case enum_Symbol_Type.Text
    '                Sym.Value = Convert_To_String(Params(enum_Properties.Value))
    '            Case enum_Symbol_Type.Color
    '                Sym.Value = Convert_To_String(Params(enum_Properties.Value))
    '            Case enum_Symbol_Type.Image
    '                Sym.Value = Convert_To_String(Params(enum_Properties.Value))
    '                Sym.Load_Image_File(Sym.Up_Image_File)
    '            Case enum_Symbol_Type.Font
    '                Sym.Value = Convert_To_String(Params(enum_Properties.Value))
    '            Case enum_Symbol_Type.Button, enum_Symbol_Type.Toggle, enum_Symbol_Type.Label, _
    '                 enum_Symbol_Type.Check_Box, enum_Symbol_Type.Radio_Button
    '                Sym.Value = Convert_To_Boolean(Params(enum_Properties.Value))
    '            Case enum_Symbol_Type.Text_Box
    '                Sym.Value = Convert_To_String(Params(enum_Properties.Value))
    '            Case enum_Symbol_Type.GCode
    '                Sym.Value = Convert_To_String(Params(enum_Properties.Value))
    '            Case enum_Symbol_Type.Macro
    '                Sym.Value = Convert_To_String(Params(enum_Properties.Value))
    '            Case enum_Symbol_Type.Command
    '                Sym.Value = Convert_To_String(Params(enum_Properties.Value))
    '            Case enum_Symbol_Type.Evnt
    '                Sym.Value = Convert_To_String(Params(enum_Properties.Value))
    '            Case enum_Symbol_Type.Hot_Key
    '                Sym.Value = Convert_To_String(Params(enum_Properties.Value))
    '            Case enum_Symbol_Type.Tool
    '                Sym.Value = Convert_To_String(Params(enum_Properties.Value))
    '            Case enum_Symbol_Type.Process
    '                Sym.Value = Convert_To_String(Params(enum_Properties.Value))
    '            Case enum_Symbol_Type.Process_GCode
    '                Sym.Value = Convert_To_String(Params(enum_Properties.Value))
    '            Case enum_Symbol_Type.Process_Drawing
    '                Sym.Value = Convert_To_String(Params(enum_Properties.Value))

    '            Case enum_Symbol_Type.Dialog_Box
    '                Sym.Value = Convert_To_String(Params(enum_Properties.Value))
    '            Case enum_Symbol_Type.Dialog_GCode
    '                Sym.Value = Convert_To_String(Params(enum_Properties.Value))
    '            Case enum_Symbol_Type.TinyG_Setting
    '                Sym.Value = Convert_To_String(Params(enum_Properties.Value))
    '        End Select

    '        Sym.Locked = Convert_To_Boolean(Params(enum_Properties.Locked))
    '        If Sym.Type = enum_Symbol_Type.Text_Box Then
    '            Sym.Read_Only = Convert_To_Boolean(Params(enum_Properties.Read_Only))
    '        Else
    '            Sym.Read_Only = False
    '        End If
    '        Sym.No_Edit = Convert_To_Boolean(Params(enum_Properties.No_Edit))

    '        Select Case Sym.Type
    '            Case enum_Symbol_Type.Button, enum_Symbol_Type.Toggle, enum_Symbol_Type.Label, _
    '                 enum_Symbol_Type.Check_Box, enum_Symbol_Type.Radio_Button, enum_Symbol_Type.Text_Box

    '                Sym.Ctrl = Get_Control_From_Main_Form(Sym.Name)
    '                Sym.Down_Image_File = Convert_To_String(Params(enum_Properties.Down_Image_File))
    '                Sym.Up_Image_File = Convert_To_String(Params(enum_Properties.Up_Image_File))
    '                Sym.Down_Handler = Convert_To_String(Params(enum_Properties.Down_Handler))
    '                Sym.Up_Handler = Convert_To_String(Params(enum_Properties.Up_Handler))

    '                If Dont_Change_Controls_Properties Then
    '                    Sym.Parent = Sym.Ctrl.Parent.Name
    '                    Sym.Top = Sym.Ctrl.Top
    '                    Sym.Left = Sym.Ctrl.Left
    '                    Sym.Height = Sym.Ctrl.Height
    '                    Sym.Width = Sym.Ctrl.Width
    '                    Sym.Font = Sym.Ctrl.Font.ToString
    '                Else
    '                    Sym.Parent = Convert_To_String(Params(enum_Properties.Parent))
    '                    Sym.Top = Convert_To_Integer(Params(enum_Properties.Top))
    '                    Sym.Left = Convert_To_Integer(Params(enum_Properties.Left))
    '                    Sym.Height = Convert_To_Integer(Params(enum_Properties.Height))
    '                    Sym.Width = Convert_To_Integer(Params(enum_Properties.Width))
    '                    Sym.Text_Align = Convert_To_String(Params(enum_Properties.Text_Align))
    '                End If

    '                Sym.Text = Convert_To_String(Params(enum_Properties.Text))

    '                If Sym.Type = enum_Symbol_Type.Text_Box Then
    '                    If Params(enum_Properties.Back_Color) = "Window" Then Params(enum_Properties.Back_Color) = "White"
    '                    If Params(enum_Properties.Text_Color) = "WindowText" Then Params(enum_Properties.Text_Color) = "Black"
    '                    Sym.Change_Back_Color(Convert_To_String(Params(enum_Properties.Back_Color)))
    '                    Sym.Change_Text_Color(Convert_To_String(Params(enum_Properties.Text_Color)))
    '                End If

    '                Sym.Enabled = Convert_To_Boolean(Params(enum_Properties.Enabled))

    '                If Sym.Type = enum_Symbol_Type.Text_Box Then
    '                    Sym.Read_Only = Convert_To_Boolean(Params(enum_Properties.Read_Only))
    '                End If
    '                'Sym.Visible = Convert_To_Boolean(Params(enum_Properties.Visible))

    '                Sym.Comment = Convert_To_String(Params(enum_Properties.Comment))
    '                'Display.Init_Control(Sym)

    '                Dim Mess As String
    '                If Sym.Type = enum_Symbol_Type.Text_Box Then
    '                    Mess = "Name : " & Sym.Name
    '                    If Sym.Down_Handler <> "" Then Mess &= vbCrLf & "Click Handler : " & Sym.Down_Handler
    '                    If Sym.Up_Handler <> "" Then Mess &= vbCrLf & "Edit Handler : " & Sym.Down_Handler
    '                    If Sym.Read_Only Or Sym.No_Edit Or Sym.Read_Only Then Mess &= vbCrLf
    '                    If Sym.Read_Only Then Mess &= "Read_Only "
    '                    If Sym.No_Edit Then Mess &= "No Edit "
    '                    If Sym.Read_Only Then Mess &= "Locked "
    '                    If (Not Sym.Read_Only) And Not (Sym.No_Edit) Then
    '                        Mess &= vbCrLf & "Click on box to edit. "
    '                    End If
    '                Else
    '                    Mess = "Name : " & Sym.Name
    '                    If Sym.Down_Handler <> "" Then Mess &= vbCrLf & "Down Handler : " & Sym.Down_Handler
    '                    If Sym.Up_Handler <> "" Then Mess &= vbCrLf & "Up Handler : " & Sym.Down_Handler
    '                End If

    '                If Sym.Comment <> "" Then Mess &= vbCrLf & Sym.Comment
    '                Tool_Tip.InitialDelay = 100
    '                Tool_Tip.SetToolTip(Sym.Ctrl, Mess)

    '            Case enum_Symbol_Type.Hot_Key
    '                Sym.Down_Handler = Convert_To_String(Params(enum_Properties.Down_Handler))
    '                Sym.Up_Handler = Convert_To_String(Params(enum_Properties.Up_Handler))

    '            Case enum_Symbol_Type.Tool
    '                Dim Lst As New List(Of String)
    '                For Tl = 6 To Params.Count - 2
    '                    Lst.Add(Params(Tl))
    '                Next
    '                Sym.Tag = Lst

    '            Case enum_Symbol_Type.Process
    '                Sym.Ctrl = Get_Control_From_Main_Form(Sym.Name)
    '                Sym.Role = Convert_To_String(Params(enum_Properties.Value))

    '            Case enum_Symbol_Type.Process_GCode
    '                Dim ctrl As New ctrl_Gcode
    '                Sym.Type = enum_Symbol_Type.Process_GCode
    '                Sym.Role = Convert_To_String(Params(enum_Properties.Value))
    '                Sym.Ctrl = ctrl
    '                'Display.Init_Control(Sym)

    '            Case enum_Symbol_Type.Process_Drawing
    '                Dim ctrl As New ctrl_Drawing
    '                Sym.Type = enum_Symbol_Type.Process_Drawing
    '                Sym.Ctrl = ctrl
    '                Sym.Role = Convert_To_String(Params(enum_Properties.Value))
    '                'Display.Init_Control(Sym)

    '            Case enum_Symbol_Type.Dialog_Box
    '                Sym.Role = Convert_To_String(Params(enum_Properties.Value))
    '                Sym.Type = enum_Symbol_Type.Dialog_Box
    '                Sym.Role = Convert_To_String(Params(enum_Properties.Value))

    '            Case enum_Symbol_Type.Dialog_GCode
    '                Dim ctrl As New ctrl_Gcode
    '                Sym.Role = Convert_To_String(Params(enum_Properties.Value))
    '                Sym.Type = enum_Symbol_Type.Dialog_GCode
    '                Sym.Ctrl = ctrl
    '                Sym.Role = Convert_To_String(Params(enum_Properties.Value))
    '                'Display.Init_Control(Sym)

    '            Case enum_Symbol_Type.TinyG_Setting

    '        End Select

    '        Sym.Comment = Convert_To_String(Params(Params.Length - 1))

    '    End Sub

    '#End Region

    '    Public Function Get_Bitmap(File_Name As String) As Bitmap
    '        Dim Bm As New Bitmap(Chip_Images_Folder & File_Name)
    '        Bm.MakeTransparent(Color.Magenta)
    '        Return Bm
    '    End Function

    '    Public Function Get_Properties(Properties As String) As List(Of String)
    '        Dim S() As String
    '        Dim Props As New List(Of String)
    '        S = Properties.Split("~")
    '        For I = 0 To S.Count - 1
    '            Props.Add(S(I))
    '        Next
    '        Return Props
    '    End Function

    '    Public Function Pack_Properties(Properties As List(Of String)) As String
    '        Dim Props As String = "|"
    '        For I = 0 To Properties.Count - 1
    '            If I > 0 Then Props &= "~"
    '            Props &= Properties(I)
    '        Next
    '        Props &= "|"
    '        Return Props
    '    End Function

    '#Region "Load, Save"

    '    Public Sub Load_Symbol_Table(File_Name As String)
    '        Dim Buff() As String
    '        Symbol_Table.Clear()

    '        If File.Exists(Chip_Symbols_File) Then
    '            Buff = File.ReadAllLines(File_Name)

    '            For R = 0 To Buff.Count - 1
    '                If Buff(R) <> "" Then
    '                    Add_Symbol_To_Table(Buff(R))
    '                End If
    '            Next
    '        Else
    '            Show_Error("Symbol table file not found : " & File_Name)
    '        End If

    '    End Sub

    '    Public Sub Save_Symbol_Table(File_Name As String)
    '        Exit Sub

    '        '        Dim Buff As New List(Of String)
    '        '        Dim Line As String

    '        '        Dim Sym As Class_Symbols.class_Symbol
    '        '        Dim DF As New Class_Data_File
    '        '        DF.Backup_File(File_Name, True)


    '        '        For I = 0 To Symbol_Table.Count - 1
    '        '            Sym = Symbol_Table(I)

    '        '            Line = Sym.Name
    '        '            Line &= "~" & Sym.Type.ToString

    '        '            Select Case Sym.Type
    '        '                Case enum_Symbol_Type.Whole
    '        '                    Line &= "~" & Convert_To_Integer(Sym.Value)
    '        '                Case enum_Symbol_Type.Real
    '        '                    Line &= "~" & Convert_To_Single(Sym.Value)
    '        '                Case enum_Symbol_Type.Logical
    '        '                    Line &= "~" & Convert_To_Boolean(Sym.Value)
    '        '                Case enum_Symbol_Type.Text
    '        '                    Line &= "~" & Convert_To_String(Sym.Value)
    '        '                Case enum_Symbol_Type.Button, enum_Symbol_Type.Toggle, enum_Symbol_Type.Label, _
    '        '                     enum_Symbol_Type.Check_Box, enum_Symbol_Type.Radio_Button
    '        '                    Line &= "~" & Convert_To_Boolean(Sym.Value)
    '        '                Case enum_Symbol_Type.Text_Box
    '        '                    Line &= "~" & Convert_To_String(Sym.Value)
    '        '                Case enum_Symbol_Type.Process
    '        '                    'Line &= "~" & Convert_To_String(Sym.Value)
    '        '                    Line &= "~" & Convert_To_String(Sym.Role)
    '        '                Case enum_Symbol_Type.Process_Drawing
    '        '                    'Line &= "~" & Convert_To_String(Sym.Value)
    '        '                    Line &= "~" & Convert_To_String(Sym.Role)
    '        '                Case enum_Symbol_Type.Process_GCode
    '        '                    'Line &= "~" & Convert_To_String(Sym.Value)
    '        '                    Line &= "~" & Convert_To_String(Sym.Role)
    '        '                Case enum_Symbol_Type.Hot_Key
    '        '                    Line &= "~" & Convert_To_String(Sym.Value)
    '        '                Case enum_Symbol_Type.Tool
    '        '                    Line &= "~" & Convert_To_String(Sym.Value)
    '        '                Case Else
    '        '                    Line &= "~" & Convert_To_String(Sym.Value)
    '        '            End Select

    '        '            Line &= "~" & Sym.Locked
    '        '            If Sym.Type = enum_Symbol_Type.Text_Box Then
    '        '                Line &= "~" & Sym.Read_Only
    '        '            Else
    '        '                Line &= "~False"
    '        '            End If
    '        '            Line &= "~" & Sym.No_Edit

    '        '            Select Case Sym.Type
    '        '                Case enum_Symbol_Type.Button, enum_Symbol_Type.Toggle, enum_Symbol_Type.Label, _
    '        '                     enum_Symbol_Type.Check_Box, enum_Symbol_Type.Radio_Button, enum_Symbol_Type.Text_Box

    '        '                    Line &= "~" & Sym.Down_Handler
    '        '                    Line &= "~" & Sym.Up_Handler
    '        '                    Line &= "~" & Sym.Text
    '        '                    Line &= "~" & Sym.Up_Image_File
    '        '                    Line &= "~" & Sym.Down_Image_File
    '        '                    Line &= "~" & Sym.Ctrl.Parent.Name
    '        '                    Line &= "~" & Sym.Top
    '        '                    Line &= "~" & Sym.Left
    '        '                    Line &= "~" & Sym.Width
    '        '                    Line &= "~" & Sym.Height
    '        '                    Line &= "~" & Sym.Back_Color
    '        '                    Line &= "~" & Sym.Text_Color
    '        '                    Line &= "~" & Sym.Text_Align
    '        '                    Line &= "~" & Sym.Font
    '        '                    Line &= "~" & Sym.Enabled
    '        '                    Line &= "~" & Sym.Visible

    '        '                Case enum_Symbol_Type.Hot_Key
    '        '                    Line &= "~" & Sym.Down_Handler
    '        '                    Line &= "~" & Sym.Up_Handler

    '        '                Case enum_Symbol_Type.Tool
    '        '                    If Not IsNothing(Sym.Tag) Then
    '        '                        Dim Tg As List(Of String)
    '        '                        Tg = Sym.Tag

    '        '                        For Tl = 0 To Tg.Count - 1
    '        '                            Line &= "~" & Tg(Tl)
    '        '                        Next
    '        '                    End If

    '        '            End Select

    '        '            Line &= "~" & Sym.Comment

    '        '            Buff.Add(Line)

    '        'Skip:
    '        '        Next

    '        '        File.WriteAllLines(File_Name, Buff)

    '    End Sub

    '#End Region

    '#Region "Delegate example"

    '    Public Delegate Function MathOperator(ByVal x As Single, ByVal y As Single) As Single

    '    Public Function AddNumbers(ByVal x As Single, ByVal y As Single) As Single
    '        Return x + y
    '    End Function

    '    Public Function SubtractNumbers(ByVal x As Single, ByVal y As Single) As Single
    '        Return x - y
    '    End Function

    '    Public Sub DelegateTest(ByVal x As Single, ByVal op As MathOperator, ByVal y As Single)
    '        Dim ret As Single
    '        ret = op.Invoke(x, y) ' Call the method.
    '        MsgBox(ret)
    '    End Sub

    '    Public Sub Test()
    '        DelegateTest(5, AddressOf AddNumbers, 3)
    '        DelegateTest(9, AddressOf SubtractNumbers, 3)
    '    End Sub

    '#End Region

End Class
