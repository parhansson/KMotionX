Public Class ctrl_Form

    Private Enum enum_Panels
        None
        Top
        Bottom
        Left
        Right
        Middle
        Toolbar
        Statusbar
    End Enum

    Private Structure struct_Selected_Control
        Public Ctrl As Control
        Public Parent As ctrl_Basic
        Public Back_Color As Color
    End Structure

    Public Design_Mode As Boolean = False

    Private Current_Panel As enum_Panels

    Private Current_Parent As Control

    Private Shared Selected_Controls As New List(Of struct_Selected_Control)

    Public Event Property_Changed(Symbol As class_Symbol_Table_OLD.Class_Symbol)

    Private WithEvents Tab_Pages As New List(Of TabPage)
    Private Ctrl_Rectangle As New Rectangle

    Private Form_Design_Changed As Boolean = False

    Public Form_Symbol As class_Symbol_Table_OLD.Class_Symbol
    Public WithEvents Parent_Form As Form

    Private WithEvents lst_Menu As New ListBox

    Private Sub ctrl_Form_Load(sender As Object, e As EventArgs) Handles Me.Load

        'Menu for inserting controls
        lst_Menu.Visible = False
        Me.Controls.Add(lst_Menu)
        lst_Menu.BringToFront()

        Dim S() As String = Nothing
        Dim T As String
        Dim Tab As TabPage

        tab_Top.Visible = False
        tab_Bottom.Visible = False
        tab_Left.Visible = False
        tab_Right.Visible = False
        tab_Middle.Visible = False

        split_Top.Visible = False
        split_Bottom.Visible = False
        split_Left.Visible = False
        split_Right.Visible = False

        For I = 0 To Symbols.Symbol_Table.Count - 1

            Select Case Symbols.Symbol_Table(I).Symbol_Type
                Case class_Symbol_Table_OLD.enum_Symbol_Type.Form

                    For j = 0 To Symbols.Symbol_Table(I).Parent_Form_Tab_Pages.Count - 1
                        T = Symbols.Symbol_Table(I).Parent_Form_Tab_Pages(j)
                        S = T.Split("~")
                        Tab = New TabPage
                        Tab.Text = S(1)
                        Tab.Name = S(1)
                        AddHandler Tab.MouseDown, AddressOf Mouse_Down_Handler

                        Select Case S(0)
                            Case "Top"
                                tab_Top.TabPages.Add(Tab)
                                tab_Top.Visible = True

                            Case "Bottom"
                                tab_Bottom.TabPages.Add(Tab)
                                tab_Bottom.Visible = True

                            Case "Left"
                                tab_Left.TabPages.Add(Tab)
                                tab_Left.Visible = True

                            Case "Right"
                                tab_Right.TabPages.Add(Tab)
                                tab_Right.Visible = True

                            Case "Middle"
                                tab_Middle.TabPages.Add(Tab)
                                tab_Middle.Visible = True
                        End Select

                    Next

                Case class_Symbol_Table_OLD.enum_Symbol_Type.Tab
                    S = Symbols.Symbol_Table(I).Name.Split(".")
                    Nop()
                Case class_Symbol_Table_OLD.enum_Symbol_Type.Control
                    S = Symbols.Symbol_Table(I).Name.Split(".")
                    S = Symbols.Symbol_Table(I).Control_Location.Split(".")

                    AddHandler Symbols.Symbol_Table(I).Control.Changed, AddressOf Control_Change_Event_Handler

                    If Parent_Form.Name = "Forms." & S(1) Then

                        Select Case S(2)
                            Case "Top"
                                Add_Control_To_Tab_Page(tab_Top, S(3), Symbols.Symbol_Table(I))
                            Case "Bottom"
                                Add_Control_To_Tab_Page(tab_Bottom, S(3), Symbols.Symbol_Table(I))
                            Case "Left"
                                Add_Control_To_Tab_Page(tab_Left, S(3), Symbols.Symbol_Table(I))
                            Case "Right"
                                Add_Control_To_Tab_Page(tab_Right, S(3), Symbols.Symbol_Table(I))
                            Case "Middle"
                                Add_Control_To_Tab_Page(tab_Middle, S(3), Symbols.Symbol_Table(I))
                        End Select

                    End If

            End Select

            If tab_Left.Visible And tab_Middle.Visible Then
                split_Left.Visible = True
            End If

            If tab_Right.Visible And tab_Middle.Visible Then
                split_Right.Visible = True
            End If

            If tab_Top.Visible And tab_Middle.Visible Then
                split_Top.Visible = True
            End If

            If tab_Bottom.Visible And tab_Middle.Visible Then
                split_Bottom.Visible = True
            End If

        Next

        Me.Dock = DockStyle.Fill

    End Sub

    Private Sub Add_Control_To_Tab_Page(Tab_Control As TabControl, Page_Name As String, Sym As class_Symbol_Table_OLD.Class_Symbol)
        For I = 0 To Tab_Control.TabPages.Count - 1
            If Tab_Control.TabPages(I).Name = Page_Name Then
                Sym.Control.Initialize(Sym, Me, Tab_Control, False)
                Tab_Control.TabPages(I).Controls.Add(Sym.Control)
            End If
        Next
    End Sub

    Public Sub Add_Selected_Control(Ctrl As Control)

        For I = 0 To Selected_Controls.Count - 1
            If Selected_Controls(I).Ctrl Is Ctrl Then
                Selected_Controls(I).Ctrl.BackColor = Selected_Controls(I).Back_Color
                Selected_Controls(I).Ctrl.Refresh()
                Selected_Controls.Remove(Selected_Controls(I))
                Exit Sub
            End If
        Next

        Dim Sel As New struct_Selected_Control
        Sel.Parent = Ctrl.Parent
        Sel.Ctrl = Ctrl
        Sel.Back_Color = Sel.Ctrl.BackColor
        Ctrl.BackColor = Color.Magenta
        Selected_Controls.Add(Sel)
    End Sub

    Private Sub Parent_Form_KeyDown(sender As Object, e As System.Windows.Forms.KeyEventArgs) Handles Parent_Form.KeyDown

        If Design_Mode = True Then
            Select Case e.KeyCode
                Case Keys.Left
                Case Keys.Right
                Case Keys.Up
                Case Keys.Down

            End Select
        End If

    End Sub

    Private Sub ts_Main_ItemClicked(sender As Object, e As System.Windows.Forms.ToolStripItemClickedEventArgs) Handles ts_Main.ItemClicked

        Select Case e.ClickedItem.Text
            Case "Exit"
                If Form_Design_Changed Then
                    Message_Box.ShowDialog("The form design has been changed but not saved." & vbCrLf & _
                                           "Save the form before exiting?", "Warning", MessageBoxButtons.YesNo)

                    If Message_Box.DialogResult = DialogResult.Yes Then
                        Symbols.Save_Symbol_Table(Symbol_File_Name)
                        Form_Design_Changed = False
                    End If
                End If

                Dim frm As Form = Me.Parent
                frm.Close()

            Case "Run"
                Dim BM As Bitmap = Nothing
                Design_Mode = False
                BM = New Bitmap(CNC_Icons & "Design.bmp")
                ts_Design.Image = BM
                ts_Design.Text = "Design"

                'If Form_Design_Changed Then
                Symbols.Save_Symbol_Table(Symbol_File_Name)
                Form_Design_Changed = False
                'End If


            Case "Design"
                Dim BM As Bitmap = Nothing
                BM = New Bitmap(CNC_Icons & "Running_Man.bmp")
                ts_Design.Image = BM
                ts_Design.Text = "Run"
                Design_Mode = True

            Case "Symbols"
                Dim dlg As New dlg_Symbols
                dlg.ShowDialog()

            Case Else

        End Select

    End Sub

    Private Sub Lst_SelectedIndexChanged(sender As Object, e As System.EventArgs) Handles lst_Menu.SelectedIndexChanged
        If Not lst_Menu.Visible Then Exit Sub

        Dim I As Integer = lst_Menu.SelectedIndex
        Dim Ctrl_Type_Text As String = lst_Menu.SelectedItem
        Dim Ctrl_Type As class_Symbol_Table_OLD.enum_Ctrl_Type

        Ctrl_Type_Text = Ctrl_Type_Text.Replace("Insert ", "")

        Dim Symbol As New class_Symbol_Table_OLD.Class_Symbol

        Select Case Ctrl_Type_Text
            Case "Label"
                Ctrl_Type = class_Symbol_Table_OLD.enum_Ctrl_Type.Label
            Case "Button"
                Ctrl_Type = class_Symbol_Table_OLD.enum_Ctrl_Type.Button
            Case "Text Box"
                Ctrl_Type = class_Symbol_Table_OLD.enum_Ctrl_Type.Text_Box
            Case "Menu"
                Ctrl_Type = class_Symbol_Table_OLD.enum_Ctrl_Type.Menu
            Case "Check Box"
                Ctrl_Type = class_Symbol_Table_OLD.enum_Ctrl_Type.Check_Box
            Case "Track Bar"
                Ctrl_Type = class_Symbol_Table_OLD.enum_Ctrl_Type.Track_Bar
            Case "Image Box"
                Ctrl_Type = class_Symbol_Table_OLD.enum_Ctrl_Type.Image_Box
        End Select

        Create_Symbol_Control(Symbol, Ctrl_Type)

        Dim dlg As New dlg_Symbol_Properties
        dlg.Symbol = Symbol
        dlg.ShowDialog()

        If dlg.DialogResult = Windows.Forms.DialogResult.OK Then
            Cursor = Cursors.WaitCursor
            Insert_Control_Symbol(Symbol)
            Symbol.Control.Set_Properties()

            Dim Parent_Type As String = Current_Parent.GetType.ToString
            Parent_Type = Parent_Type.Replace("System.Windows.Forms.", "")

            Select Case Parent_Type
                Case "TabPage"
                    Current_Parent.Controls.Add(Symbol.Control)

                Case "ToolStrip"
                    Select Case Symbol.Control.Control_Type
                        Case class_Symbol_Table_OLD.enum_Ctrl_Type.Label
                            Dim ts_Lab As New ToolStripLabel
                            ts_Main.Items.Add(ts_Lab)

                        Case class_Symbol_Table_OLD.enum_Ctrl_Type.Button
                            Dim ts_Button As New ToolStripButton
                            ts_Main.Items.Add(ts_Button)
                        Case class_Symbol_Table_OLD.enum_Ctrl_Type.Text_Box
                            Dim ts_Text As New ToolStripButton
                            ts_Main.Items.Add(ts_Text)
                    End Select

                Case "StatusStrip"
                    Select Case Symbol.Control.Control_Type
                        Case class_Symbol_Table_OLD.enum_Ctrl_Type.Label
                            Dim ts_Lab As New ToolStripLabel
                            stat_Main.Items.Add(ts_Lab)

                        Case class_Symbol_Table_OLD.enum_Ctrl_Type.Text_Box
                            Dim ts_Text As New ToolStripTextBox
                            stat_Main.Items.Add(ts_Text)
                    End Select
            End Select

            Cursor = Cursors.Arrow
        End If

        Form_Design_Changed = True


        lst_Menu.Visible = False
    End Sub

    Private Sub pnl_Mouse_Down(sender As Object, e As System.Windows.Forms.MouseEventArgs) Handles ts_Main.MouseDown, stat_Main.MouseDown
        Mouse_Down_Handler(sender, e)
    End Sub

    Private Sub Lst_Menu_Mouse_Leave() Handles lst_Menu.MouseLeave
        lst_Menu.Visible = False
    End Sub

    Private Sub Mouse_Down_Handler(sender As Object, e As System.Windows.Forms.MouseEventArgs)
        Current_Parent = sender

        Dim Pt As Point = sender.PointToClient(System.Windows.Forms.Control.MousePosition)
        'stat_Main_Mouse.Text = Pt.X & " " & Pt.Y

        Dim My_Pt As Point = Me.PointToClient(System.Windows.Forms.Control.MousePosition)

        Dim Target_Control As Control = Me.GetChildAtPoint(My_Pt)
        If IsNothing(Target_Control) Then
            Exit Sub
        End If

        Dim Name As String = Target_Control.Name

        Dim Tp As String = Target_Control.GetType.ToString

        stat_Main_Stat.Text = Name

        Dim Ts As Rectangle = ts_Main.Bounds()
        Dim Tb As Rectangle = tab_Middle.Bounds()

        Dim Left_Top_Corner As Point = Me.Parent.PointToScreen(New Point(Target_Control.Left, Target_Control.Top))

        Ctrl_Rectangle.X = Left_Top_Corner.X
        Ctrl_Rectangle.Y = Left_Top_Corner.Y

        Ctrl_Rectangle.Width = Target_Control.Width
        Ctrl_Rectangle.Height = Target_Control.Height

        Dim Type As String = sender.GetType.ToString

        Dim Top As Integer = 0
        Dim Left As Integer = 0

        Select Case e.Button

            Case Windows.Forms.MouseButtons.Left
                lst_Menu.Visible = False


            Case Windows.Forms.MouseButtons.Right
                If Not Design_Mode Then Exit Sub

                Select Case Type
                    Case "System.Windows.Forms.TabPage"
                        Top = sender.Parent.Top + sender.Top
                        Left = sender.Parent.Left + sender.Left
                    Case "System.Windows.Forms.ToolStrip", "System.Windows.Forms.StatusStrip"
                        Top = sender.Top
                        Left = sender.Left
                    Case Else
                        lst_Menu.Visible = False
                        Exit Sub
                End Select

                lst_Menu.Items.Clear()

                Select Case Type
                    Case "System.Windows.Forms.TabPage"
                        lst_Menu.Items.Add("Insert Label")
                        lst_Menu.Items.Add("Insert Button")
                        lst_Menu.Items.Add("Insert Text Box")
                        lst_Menu.Items.Add("Insert Menu")
                        lst_Menu.Items.Add("Insert Check Box")
                        lst_Menu.Items.Add("Insert Track Bar")
                        lst_Menu.Items.Add("Insert Image")

                    Case "System.Windows.Forms.ToolStrip"
                        lst_Menu.Items.Add("Insert Label")
                        lst_Menu.Items.Add("Insert Button")
                        lst_Menu.Items.Add("Insert Text Box")
                        lst_Menu.Items.Add("Insert Menu")

                    Case "System.Windows.Forms.StatusStrip"
                        lst_Menu.Items.Add("Insert Label")
                        lst_Menu.Items.Add("Insert Button")
                        lst_Menu.Items.Add("Insert Text Box")
                        lst_Menu.Items.Add("Insert Menu")
                    Case Else
                        lst_Menu.Visible = False
                        Exit Sub
                End Select

                lst_Menu.Top = Top + Pt.Y - (lst_Menu.ItemHeight)
                lst_Menu.Left = Left + Pt.X - (lst_Menu.Width / 4)

                lst_Menu.Height = lst_Menu.ItemHeight * (lst_Menu.Items.Count + 1)

                'See if list extends below form, if so move up
                If lst_Menu.Top + lst_Menu.Height > Me.Height Then
                    lst_Menu.Top = Me.Height - lst_Menu.Height - (2 * lst_Menu.ItemHeight)
                End If

                'See if list extends to right of form, if so move left
                If lst_Menu.Left + lst_Menu.Width > Me.Width Then
                    lst_Menu.Left = lst_Menu.Left - (lst_Menu.Width / 2) ' + (Lst_Menu.Width / 4)
                End If

                lst_Menu.Visible = True

        End Select
    End Sub

    Public Sub Insert_Control_Symbol(Symbol As class_Symbol_Table_OLD.Class_Symbol)
        Dim First_Control As Integer = 0
        Dim Inserted As Boolean = False

        Symbol.Tree_Key = "Controls"
        Symbol.Header = "Controls"
        Symbol.Parent_Form_Control = Me
        Symbol.Parent_Form = Me.Parent

        Dim I As Integer = Symbols.Symbol_Table.Count - 1

        Do While I >= 0
            If Symbols.Symbol_Table(I).Symbol_Type = class_Symbol_Table_OLD.enum_Symbol_Type.Control Then
                First_Control = I
                If Symbol.Name > Symbols.Symbol_Table(I).Name Then
                    Symbols.Symbol_Table.Insert(I + 1, Symbol)
                    Inserted = True
                    Exit Do
                End If
            End If
            I -= 1
        Loop

        If Not Inserted Then
            Symbols.Symbol_Table.Insert(First_Control, Symbol)
        End If

    End Sub

    Public Sub Create_Symbol_Control(Symbol As class_Symbol_Table_OLD.Class_Symbol, Type As class_Symbol_Table_OLD.enum_Ctrl_Type)
        Dim ctrl As New ctrl_Basic

        Symbol.Symbol_Type = class_Symbol_Table_OLD.enum_Symbol_Type.Control
        Symbol.Control = ctrl
        Symbol.Control.Control_Type = Type
        Symbol.Control_Location = Me.Name & "." & Current_Parent.Name
        ctrl.Initialize(Symbol, Me, Current_Parent, True)

    End Sub

    Private Sub ToolStripButton1_Click(sender As Object, e As EventArgs) Handles ToolStripButton1.Click
        'Dim dlg As New dlg_Test
        'dlg.ShowDialog()

    End Sub

    Private Sub Control_Change_Event_Handler(Sym As class_Symbol_Table_OLD.Class_Symbol)

    End Sub

End Class
