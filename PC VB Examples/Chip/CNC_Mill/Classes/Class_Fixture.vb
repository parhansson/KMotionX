
Public Class Class_Fixture

    Public Structure struct_Part
        Dim Name As String

        Dim X_Fixture_Offset As Single
        Dim Y_Fixture_Offset As Single
        Dim Z_Fixture_Offset As Single

        Dim X_Min As Single
        Dim Y_Min As Single
        Dim Z_Min As Single

        Dim X_Max As Single
        Dim Y_Max As Single
        Dim Z_Max As Single

        Dim Material_X As Single
        Dim Material_Y As Single
        Dim Material_Z As Single

        Dim Viewport_Center As Point
        Dim Viewport_Scale As Single
        Dim Viewport_Rotation As Single

    End Structure

    Public Parts As New List(Of struct_Part)

    Public Sub Add_Part_To_Fixture(Material_X As Single, Material_Y As Single, Material_Z As Single)
        Dim P As New struct_Part
        P.Material_X = Material_X
        P.Material_Y = Material_Y
        P.Material_Z = Material_Z

        P.Viewport_Center = New Point(0, 0)
        P.Viewport_Scale = 0
        P.Viewport_Rotation = 0

        Parts.Add(P)
    End Sub



End Class
