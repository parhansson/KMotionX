Imports System.IO

Public Class Class_Settings

    Private Settings As New Class_List_Of_Tagged_List

    Private In_Buffer() As String
    Private Out_Buffer As New List(Of String)

    Public Function Count() As Integer
        Return Settings.Count
    End Function

    Public Function Get_Setting(Key As String, Child_Key As String, Default_Value As Object,
                                Optional Data_Type As String = "String", Optional Add_Tag As Boolean = False) As String

        If Add_Tag Then Key &= "." & Child_Key

        Dim Value As String = CStr(Settings.Get_Value(Key, Child_Key))
        If IsNothing(Value) Then
            Return Default_Value
        Else
            Select Case Data_Type
                Case ""
                    Return Value
                Case "Integer", "Int16", "Int32", "Int64"
                    Return CInt(Value)
                Case "Single"
                    Return CSng(Value)
                Case "Boolean"
                    Return CBool(Value)
                Case "String"
                    Return CStr(Value)
                Case Else
                    Return Nothing
            End Select

        End If
    End Function

    Public Sub Put_Setting(Key As String, Child_Key As String, Value As String, Optional Add_Tag As Boolean = False)
        If Add_Tag Then Key &= "." & Child_Key
        Settings.Set_Value(Key, Child_Key, Value)
    End Sub

    Public Sub Set_Form_Size_and_Location(Frm As Form)
        Frm.Top = Get_Setting(Frm.Name, "Top", 50)
        Frm.Left = Get_Setting(Frm.Name, "Left", 50)
        Frm.Width = Get_Setting(Frm.Name, "Width", 500)
        Frm.Height = Get_Setting(Frm.Name, "Height", 500)
    End Sub

    Public Sub Save_Form_Size_and_Location(Frm As Form)
        Put_Setting(Frm.Name, "Top", Frm.Top)
        Put_Setting(Frm.Name, "Left", Frm.Left)
        Put_Setting(Frm.Name, "Width", Frm.Width)
        Put_Setting(Frm.Name, "Height", Frm.Height)
    End Sub

    Public Function Get_Control_Setting(Ctrl As Control, Child_Key As String, Optional Default_Value As Object = Nothing,
                                        Optional Data_Type As String = "") As Object
        Dim Key As String = ""
        Dim S As Object

        Key = Ctrl.Name
        S = Get_Setting(Key, Child_Key, Default_Value)

        Select Case Data_Type
            Case ""
                Return S
            Case "Integer", "Int16", "Int32", "Int64"
                Return CInt(S)
            Case "Single"
                Return CSng(S)
            Case "Boolean"
                Return CBool(S)
            Case "String"
                Return CStr(S)
            Case Else
                Return Nothing
        End Select


    End Function

    Public Sub Put_Control_Setting(Ctrl As Control, Child_Key As String, Value As Object)
        Dim Key As String = Ctrl.Name
        Put_Setting(Key, Child_Key, Value)
    End Sub

#Region "Load/Save settings"

    Public Sub Load(File_Name As String)
        Dim S() As String = Nothing
        Dim T() As String = Nothing

        If Not File.Exists(File_Name) Then Exit Sub

        Settings.Clear()

        In_Buffer = File.ReadAllLines(File_Name)

        For I = 0 To In_Buffer.Count - 1
            If In_Buffer(I) <> "" Then
                Dim Sets = New Class_Tagged_List
                S = In_Buffer(I).Split("~")
                For J = 1 To S.Count - 1
                    T = S(J).Split("=")
                    If T.Count > 1 Then
                        Sets.Set_Value(T(0), T(1))
                    End If
                Next
                Settings.Set_Value(S(0), Sets)
            End If
        Next

    End Sub

    Public Sub Save(File_Name As String)
        Dim DF As New Class_Data_File
        Dim Directory As String = Path.GetDirectoryName(File_Name)
        Dim Fn As String = Path.GetFileName(File_Name)
        DF.Backup_File(File_Name, True)

        Dim Line As String
        Out_Buffer.Clear()

        For I = 0 To Settings.Count - 1
            Line = ""
            For J = 0 To Settings.Child_Count(I) - 1
                Line &= "~" & Settings.Get_Key(I, J) & "=" & CStr(Settings.Get_Value(I, J))
            Next
            Line = Settings.Get_Key(I) & Line
            Out_Buffer.Add(Line)
        Next
        File.WriteAllLines(File_Name, Out_Buffer)

    End Sub

    Public Sub Load_Control_Values(Contrl As Control, Values As Class_Settings, Update_Symbols As Boolean)

        Load_Control(Contrl, Values, Update_Symbols)

        Dim ctrl As Control

        For C = 0 To Contrl.Controls.Count - 1
            ctrl = Contrl.Controls(C)
            Load_Control_Value(ctrl, Values, Update_Symbols)
        Next
    End Sub

    Private Sub Load_Control_Value(Contrl As Control, Values As Class_Settings, Update_Symbols As Boolean)

        If Contrl.Controls.Count = 0 Then
            Load_Control(Contrl, Values, Update_Symbols)
            Exit Sub
        Else
            For C = 0 To Contrl.Controls.Count - 1
                Dim n As String = Contrl.Controls(C).GetType.Name

                Select Case Contrl.Controls(C).GetType.Name
                    Case "TextBox", "ComboBox", "RadioButton", "CheckBox", "NumericUpDown", "ctrl_GCode", "ctrl_Drawing"
                        Load_Control(Contrl.Controls(C), Values, Update_Symbols)
                    Case Else
                        Load_Control_Value(Contrl.Controls(C), Values, Update_Symbols) 'Recursive call
                End Select
            Next
        End If

    End Sub


    Private Sub Load_Control(Contrl As Control, Values As Class_Settings, Update_Symbol As Boolean)
        Nop()
        'Dim txt As TextBox = Nothing
        'Dim cmb As ComboBox = Nothing
        'Dim chk As CheckBox = Nothing
        'Dim rad As RadioButton = Nothing
        'Dim Num As NumericUpDown = Nothing
        'Dim gcd As ctrl_Gcode = Nothing
        'Dim dwg As ctrl_Drawing = Nothing
        'Dim tab As TabControl = Nothing
        'Dim Tag As Object = Contrl.Tag
        'Dim Sym As Object = Nothing

        'If Update_Symbol Then
        '    If Not IsNothing(Tag) Then Sym = Tag
        'End If

        'Select Case Contrl.GetType.Name

        '    Case "TabControl"
        '        tab = Contrl
        '        tab.SelectedIndex = Values.Get_Control_Setting(Contrl, "Page", "", "Integer")

        '    Case "ctl_GCode"
        '        gcd = Contrl
        '        gcd.Width = Values.Get_Control_Setting(Contrl, "Width", "", "Integer")

        '    Case "ctrl_Drawing"
        '        dwg = Contrl

        '    Case "TextBox"
        '        txt = Contrl
        '        txt.Text = Values.Get_Control_Setting(Contrl, "TextBox", "", "String")
        '        If Update_Symbol Then Sym.Value = txt.Text

        '    Case "ComboBox"
        '        cmb = Contrl
        '        cmb.Text = Values.Get_Control_Setting(Contrl, "ComboBox", "", "String")
        '        If Update_Symbol Then Sym.Value = cmb.Text

        '    Case "RadioButton"
        '        rad = Contrl
        '        rad.Checked = Values.Get_Control_Setting(Contrl, "RadioButton", False, "Boolean")
        '        If Update_Symbol Then Sym.Value = rad.Checked

        '    Case "CheckBox"
        '        chk = Contrl
        '        chk.Checked = Values.Get_Control_Setting(Contrl, "CheckBox", False, "Boolean")
        '        If Update_Symbol Then Sym.Value = chk.Checked

        '    Case "NumericUpDown"
        '        Num = Contrl
        '        Num.Value = Values.Get_Control_Setting(Contrl, "NumericUpDown", 0, "String")
        '        If Update_Symbol Then Sym.Value = Num.Value

        'End Select
    End Sub


    Public Sub Save_Control_Values(Contrl As Control, Values As Class_Settings)
        Dim ctrl As Control

        Save_Control(Contrl, Values)

        For C = 0 To Contrl.Controls.Count - 1
            ctrl = Contrl.Controls(C)
            Save_Control_Value(ctrl, Values)
        Next
    End Sub

    Private Sub Save_Control_Value(Contrl As Control, Values As Class_Settings)


        If Contrl.Controls.Count = 0 Then
            Save_Control(Contrl, Values)
            Exit Sub
        Else
            For C = 0 To Contrl.Controls.Count - 1

                Dim s As String = Contrl.Controls(C).GetType.Name

                Select Case Contrl.Controls(C).GetType.Name
                    Case "TextBox", "ComboBox", "RadioButton", "CheckBox", "NumericUpDown", "ctrl_GCode", "ctrl_Drawing"
                        Save_Control(Contrl.Controls(C), Values)
                    Case Else
                        Save_Control_Value(Contrl.Controls(C), Values) 'Recursive call
                End Select
            Next
        End If

    End Sub

    Private Sub Save_Control(Contrl As Control, Values As Class_Settings)
        'Dim txt As TextBox = Nothing
        'Dim cmb As ComboBox = Nothing
        'Dim chk As CheckBox = Nothing
        'Dim rad As RadioButton = Nothing
        'Dim Num As NumericUpDown = Nothing
        'Dim gcode As ctrl_Gcode = Nothing
        'Dim dwg As ctrl_Drawing = Nothing
        'Dim tab As TabControl = Nothing

        'Select Case Contrl.GetType.Name
        '    Case "TabControl"
        '        tab = Contrl
        '        Values.Put_Control_Setting(tab, "Page", tab.SelectedIndex)

        '    Case "ctrl_GCode"
        '        gcode = Contrl
        '        Values.Put_Control_Setting(gcode, "Width", gcode.Width)

        '    Case "ctrl_Drawing"
        '        dwg = Contrl

        '    Case "TextBox"
        '        txt = Contrl
        '        Values.Put_Control_Setting(txt, "TextBox", txt.Text)
        '    Case "ComboBox"
        '        cmb = Contrl
        '        Values.Put_Control_Setting(cmb, "ComboBox", cmb.Text)
        '    Case "RadioButton"
        '        rad = Contrl
        '        Values.Put_Control_Setting(rad, "RadioButton", rad.Checked)
        '    Case "CheckBox"
        '        chk = Contrl
        '        Values.Put_Control_Setting(chk, "CheckBox", chk.Checked)
        '    Case "NumericUpDown"
        '        Num = Contrl
        '        Values.Put_Control_Setting(Num, "NumericUpDown", Num.Value)
        'End Select

    End Sub

#End Region

End Class

