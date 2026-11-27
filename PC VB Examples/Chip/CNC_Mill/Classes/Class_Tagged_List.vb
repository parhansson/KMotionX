
Class Class_List_Of_Tagged_List

    Private Tagged_List As New List(Of Class_Tagged_List)
    Private Keys As New List(Of String)

    Public Function Count() As Integer
        Return Keys.Count
    End Function

    Public Sub Clear()
        Keys.Clear()
        Tagged_List.Clear()
    End Sub

    Public Overloads Function Child_Count(Index As Integer) As Integer
        If Index > Tagged_List.Count - 1 Then Return Nothing
        Return Tagged_List(Index).Count
    End Function

    Public Overloads Function Child_Count(Key As String) As Integer
        Dim Idx As Integer = Keys.IndexOf(Key)
        If Idx < 0 Then Return Nothing Else Return Idx
        Return Tagged_List(Idx).Count
    End Function

    Public Function Get_Index(Key As String) As Integer
        Dim Idx As Integer = Keys.IndexOf(Key)
        If Idx < 0 Then Return Nothing Else Return Idx
    End Function

    Public Overloads Function Get_Key(Index As Integer) As String
        If Index > Keys.Count - 1 Then Return Nothing
        Return Keys(Index)
    End Function

    Public Overloads Function Get_Key(Index As Integer, Child_Index As Integer) As String
        Return Tagged_List(Index).Get_Key(Child_Index)
    End Function


    Public Overloads Function Get_Value(Key As String, Child_Key As String) As Object
        Dim Idx As Integer = Keys.IndexOf(Key)
        If Idx < 0 Then Return Nothing

        Dim Child_Idx As Integer = Tagged_List(Idx).Get_Index(Tagged_List(Idx).Get_Index(Child_Key))
        If Child_Idx < 0 Then Return Nothing

        Return Tagged_List(Idx).Get_Value(Child_Key)

    End Function

    Public Overloads Function Get_Value(Index As Integer, Child_Index As Integer) As Object
        Return Tagged_List(Index).Get_Value(Child_Index)
    End Function


    Public Overloads Sub Set_Value(Key As String, Value As Object)
        Dim Idx As Integer = Keys.IndexOf(Key)
        If Idx < 0 Then 'Setting does not exist, create
            Tagged_List.Add(Value)
            Keys.Add(Key)
        End If

    End Sub

    Public Overloads Sub Set_Value(Key As String, Child_Key As String, Value As Object)
        Dim Idx As Integer = Keys.IndexOf(Key)

        If Idx < 0 Then 'Setting does not exist, create
            Dim TL As New Class_Tagged_List
            TL.Set_Value(Child_Key, Value)
            Tagged_List.Add(TL)
            Keys.Add(Key)
        End If

        Idx = Keys.IndexOf(Key)
        Dim Child_Idx As Integer = Tagged_List(Idx).Get_Index(Tagged_List(Idx).Get_Index(Child_Key))
        If Child_Idx < 0 Then Exit Sub

        Tagged_List(Idx).Set_Value(Child_Key, Value)

    End Sub

End Class

Public Class Class_Tagged_List

    'This class implements a tagged list which is a list whose values can be retrieved or set by using keys.
    'To access the olist use either the Get_Value or Put_Value routines and provide a key.
    'The keys are strings and are typically a name. The Values are objects so any datatype can be used

    Private Values As New List(Of Object)
    Public Keys As New List(Of String)

    Public Function Count() As Integer
        Return Keys.Count
    End Function

    Public Sub Clear()
        Keys.Clear()
        Values.Clear()
    End Sub

    Public Function Get_Index(Key As String) As Integer
        Dim Idx As Integer = Keys.IndexOf(Key)
        If Idx < 0 Then Return Nothing Else Return Idx
    End Function

    Public Overloads Function Get_Value(Index As Integer) As Object
        If Index > Values.Count - 1 Then Return Nothing
        Return Values(Index)
    End Function

    Public Overloads Function Get_Value(Key As String) As Object
        Dim Idx As Integer = Keys.IndexOf(Key)
        If Idx < 0 Then
            Return Nothing
        Else
            Return Values(Idx)
        End If
    End Function

    Public Function Get_Key(Index As Integer) As String
        Return Keys(Index)
    End Function

    Public Overloads Sub Set_Value(Index As Integer, Value As Object)
        If Index > Values.Count - 1 Then Exit Sub
        Values(Index) = Value
    End Sub

    Public Overloads Sub Set_Value(Key As String, Value As Object)
        Dim Idx As Integer = Keys.IndexOf(Key)
        If Idx < 0 Then 'Setting does not exist, create
            Keys.Add(Key)
            Values.Add(Value)
        Else
            Values(Idx) = Value
        End If
    End Sub

End Class

