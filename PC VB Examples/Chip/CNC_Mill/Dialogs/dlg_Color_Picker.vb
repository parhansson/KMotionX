
Public Class dlg_Color_Picker

    Private NameArray() As String = _
       {"White", "WhiteSmoke", "GhostWhite", "Snow", "Ivory", "FloralWhite", "SeaShell", "OldLace", _
       "Linen", "AntiqueWhite", "Gainsboro", "LightGray", "Silver", "DarkGray", "LightSlateGray", _
       "SlateGray", "Gray", "DimGray", "DarkSlateGray", "Black", "LightYellow", "Cornsilk", _
       "LightGoldenrodYellow", "LemonChiffon", "Beige", "LavenderBlush", "PapayaWhip", "BlanchedAlmond", _
       "MistyRose", "Bisque", "Moccasin", "NavajoWhite", "Wheat", "PaleGoldenrod", "Gold", "Yellow", _
       "", "", "", "", "Honeydew", "MintCream", "LightGreen", "PaleGreen", "SpringGreen", _
       "MediumSpringGreen", "Chartreuse", "GreenYellow", "LawnGreen", "Lime", "LimeGreen", _
       "YellowGreen", "OliveDrab", "SeaGreen", "ForestGreen", "Green", "DarkGreen", _
       "Olive", "DarkOliveGreen", "", "Azure", "LightCyan", "Cyan", "Aqua", "Aquamarine", _
       "PaleTurquoise", "PowderBlue", "Turquoise", "MediumTurquoise", "DarkTurquoise", _
       "MediumAquamarine", "LightSeaGreen", "MediumSeaGreen", "DarkSeaGreen", "DarkCyan", _
       "Teal", "", "", "", "", "AliceBlue", "LightBlue", "DeepSkyBlue", "LightSkyBlue", "SkyBlue", _
       "LightSteelBlue", "CornflowerBlue", "DodgerBlue", "Blue", "RoyalBlue", "MediumBlue", _
       "MediumSlateBlue", "CadetBlue", "SlateBlue", "SteelBlue", "Navy", "DarkSlateBlue", _
       "DarkBlue", "MidnightBlue", "", "Lavender", "Thistle", "Plum", "Violet", "Orchid", _
       "MediumOrchid", "MediumPurple", "BlueViolet", "DarkViolet", "DarkOrchid", "DarkMagenta", _
       "Purple", "Indigo", "", "", "", "", "", "", "", "PeachPuff", "Pink", "LightPink", "Fuchsia", _
       "Magenta", "HotPink", "DeepPink", "LightSalmon", "Salmon", "LightCoral", "DarkSalmon", _
       "PaleVioletRed", "MediumVioletRed", "", "", "", "", "", "", "", "Khaki", "Tan", "BurlyWood", _
       "RosyBrown", "DarkKhaki", "Goldenrod", "SandyBrown", "Chocolate", "Peru", "DarkGoldenrod", _
       "Sienna", "SaddleBrown", "Firebrick", "Brown", "", "", "", "", "", "", "Coral", "Orange", "DarkOrange", _
       "Tomato", "OrangeRed", "Red", "Crimson", "IndianRed", "DarkRed", "Maroon", "", "", "", "", "", "", _
       "", "", "", ""}

    Private Const LastRow As Integer = 19
    Private Const LastCol As Integer = 8

    Private TopRow As Integer = 0
    Private RowHeight As Integer = 0
    Private Const ColWidth As Integer = 110

    Private ColorArray(LastRow, LastCol) As TextBox

    Public Color_Name As String = "White"
    Public Color As Color

    Public Text_Original_Color As TextBox

    Private Sub dlgColorPicker_Shown(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Shown
        Dim I As Integer
        Dim R As Integer
        Dim C As Integer
        Dim Name As String
        Dim Fnt As New Font("Arial", 8)
        Dim x As New TextBox

        SuspendLayout()

        RowHeight = x.Height

        TopRow = 0

        Me.Width = ColWidth * (LastCol + 1)
        Me.Height = ts_Main.Height + TopRow + (RowHeight * (LastRow + 3))
        Me.Text = Color_Name

        Dim A As Integer
        Dim Red As Integer
        Dim Green As Integer
        Dim Blue As Integer

        I = 0
        For C = 0 To LastCol
            For R = 0 To LastRow
                Name = NameArray(I)

                Dim T As New TextBox
                If R = 0 And C = 0 Then
                    Text_Original_Color = T
                End If

                T.Font = Fnt
                T.BorderStyle = BorderStyle.Fixed3D

                T.Text = Name
                If Name = "" Then
                    T.BackColor = Drawing.Color.White
                Else
                    T.BackColor = Drawing.Color.FromName(Name)
                    If "Color [" & Name & "]" = Color_Name Then
                        Text_Original_Color = T
                    End If

                    If Name = Color_Name Then
                        Text_Original_Color = T
                    End If


                End If
                T.ReadOnly = True

                Red = T.BackColor.R
                Green = T.BackColor.G
                Blue = T.BackColor.B

                A = Red + Green + Blue
                If A < 382 Then
                    T.ForeColor = Drawing.Color.White
                End If

                T.Height = RowHeight
                T.Width = ColWidth
                T.Left = C * ColWidth
                T.Top = TopRow + (R * RowHeight)
                ColorArray(R, C) = T

                pnl_Colors.Controls.Add(T)
                AddHandler T.Click, AddressOf Click_Handler

                I = I + 1
            Next R
        Next C

        ResumeLayout()

        Text_Original_Color.Focus()

    End Sub

    Private Sub Click_Handler(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Color = sender.BackColor
        Color_Name = sender.BackColor.Name
        Me.DialogResult = Windows.Forms.DialogResult.OK
    End Sub

    Private Sub ts_Cancel_Click(sender As Object, e As EventArgs) Handles ts_Cancel.Click
        DialogResult = Windows.Forms.DialogResult.Cancel
        Close()
    End Sub

End Class