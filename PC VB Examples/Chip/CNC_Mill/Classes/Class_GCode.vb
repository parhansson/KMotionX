Public Class Class_GCode

    Private Header As String = ""
    Private Idx As Integer
    Private Pos As Integer
    Private Characters As String = "+-.0123456789"

    Public N As String = ""
    Public G As New List(Of String)
    Public M As New List(Of String)
    Public X As String = ""
    Public Y As String = ""
    Public Z As String = ""
    Public I As String = ""
    Public J As String = ""
    Public K As String = ""
    Public L As String = ""
    Public P As String = ""
    Public F As String = ""
    Public S As String = ""
    Public Comment As String = ""
    Public Original_Block As String
    Public Formatted_Block As String

    Public X_Range As Single
    Public Y_Range As Single
    Public X_Min As Single
    Public Y_Min As Single


    Public Property Block As String
        Get
            Return Formatted_Block
        End Get

        Set(ByVal Value As String)
            Original_Block = UCase(Value)
            If Mid(Value, 1, 1) = "'" Then 'Header
                Header = Value
                Formatted_Block = ""
            Else
                N = Get_String("N")
                G = Get_Strings("G")
                X = Get_Value("X")
                Y = Get_Value("Y")
                Z = Get_Value("Z")
                I = Get_Value("I")
                J = Get_Value("J")
                K = Get_Value("K")
                L = Get_Value("L")
                P = Get_Value("P")
                F = Get_String("F")
                S = Get_String("S")
                M = Get_Strings("M")
                Get_Comment(Original_Block)
                Formatted_Block = Format_Block(Original_Block)
            End If
        End Set

    End Property

    Public Function Format_Block(Block As String) As String
        Dim Blk As String = ""

        If N <> "" Then Blk = "N" & N
        For Codes = 0 To G.Count - 1
            Blk &= " G" & G(Codes)
        Next
        If L <> "" Then Blk &= " L" & L
        If P <> "" Then Blk &= " P" & P
        If X <> "" Then Blk &= " X" & X
        If Y <> "" Then Blk &= " Y" & Y
        If Z <> "" Then Blk &= " Z" & Z
        If I <> "" Then Blk &= " I" & I
        If J <> "" Then Blk &= " J" & J
        If K <> "" Then Blk &= " K" & K
        If F <> "" Then Blk &= " F" & F
        If S <> "" Then Blk &= " S" & S
        For Codes = 0 To M.Count - 1
            Blk &= " M" & M(Codes)
        Next

        'If Comment <> "" Then Blk &= " ;" & Comment

        Return Blk

    End Function

    Public Function G_Contains(Code As Integer) As Boolean
        If G.Count < 1 Then Return False
        For Cnt = 0 To G.Count - 1
            If CInt(G(Cnt)) = Code Then Return True
        Next
        Return False

    End Function

    Public Function M_Contains(Code As Integer) As Boolean
        If M.Count < 1 Then Return False
        For Cnt = 0 To M.Count - 1
            If CInt(M(Cnt)) = Code Then Return True
        Next
        Return False

    End Function

    Private Sub Get_Comment(Block As String)
        Dim T() As String = Nothing

        Block = Block.Replace("(", ";")
        Block = Block.Replace(")", "")

        If Block.Contains(";") Then 'Comment
            If Mid(Block, 1, 1) = ";" Then 'whole line comment
                Comment = Block.Replace(";", "")
                Block = ""
            Else
                T = Block.Split(";")
                Block = T(0)
                Comment = T(1)
            End If
        End If

    End Sub

    Private Function Get_String(Code As String, Optional Index As Integer = 0) As String
        Dim Value As String = ""

        Idx = Original_Block.IndexOf(Code, Index) + 2
        While (Idx > 0) And (Idx <= Original_Block.Length)
            If Characters.Contains(Mid(Original_Block, Idx, 1)) Then
                Value &= Mid(Original_Block, Idx, 1)
                Idx += 1
            Else
                Idx = -1
            End If
        End While

        Return Value
    End Function

    Public Function Get_Value(Code As String, Optional Index As Integer = 0) As String
        Dim Value As String = Get_String(Code, Index)
        Dim V As Single

        If Value <> "" Then
            V = CSng(Value)
            Return FormatNumber(V, 4)
        End If

        Return Value

    End Function

    Private Function Get_Strings(Code As String) As List(Of String)
        Dim Values As New List(Of String)
        Dim Value As String = ""
        Dim Idx As Integer
        Dim Int As Integer

        Idx = Original_Block.IndexOf(Code)
        While Idx >= 0
            Value = Get_Value(Code, Idx)
            If Value <> "" Then
                Int = Value
                Value = Int
                Values.Add(Value.PadLeft(2, "0"))
            End If
            Idx = Original_Block.IndexOf(Code, Idx + 1)
        End While

        Return Values
    End Function



End Class




