
Public Class Class_Reverse_Polish_Notation

    Private Operators As String = "+-*/^%"

    Function Precedence(op As String) As Integer
        Select Case op
            Case "+", "-"
                Return 1
            Case "*", "/"
                Return 2
            Case "^", "%"
                Return 3
        End Select

        Return 0

    End Function

    Public Function Create(Expression As String) As String
        Dim i As Long
        Dim Token As String
        Dim Tokens() As String
        Dim Token_Stack As New Stack
        Dim Queue As New Stack
        Dim op1 As String, op2 As String
        Dim Done As Boolean = False

        Dim Q As String
        Dim C As String

        'Input : 3 + 4 * 2 / ( 1 - 5 ) ^ 2 ^ 3
        'Output : 3 4 2 * 1 5 - 2 3 ^ ^ / +

        'Expression = "( ( 3 + 4 ) * 2 ) / ( ( ( 1 - 5 ) ^ 2 ) ^ 3 )"


        Expression = "( 1 + 2 ) * ( 3 + 4 ) + F % ( 5 + ( 3 * 2 ) ) "

        '1 2 + 3 4 + * + 5 6 + %  

        'Expression = " 1 +  % ( 4 ) "

        ' Get tokens
        Tokens = Split(Expression)

        ' Loop over tokens
        For i = 0 To Tokens.Count - 1
            Token = Tokens(i)

            If Token = "(" Then
                Token_Stack.Push(Token)
            ElseIf Token = ")" Then
                If Token_Stack.Count > 0 Then
                    While Token_Stack.Peek <> "("
                        Queue.Push(Token_Stack.Pop)
                    End While
                    Token_Stack.Pop() 'discard "("
                End If
            ElseIf Operators.Contains(Token) Then
                op1 = Token

                Done = False
                Do While Not Done
                    If Token_Stack.Count > 0 Then
                        If Operators.Contains(Token_Stack.Peek) Then
                            op2 = Token_Stack.Peek
                            If op2 <> "^" And Precedence(op1) = Precedence(op2) Then
                                '"^" is the only right-associative operator
                                Queue.Push(Token_Stack.Pop)
                            ElseIf Precedence(op1) < Precedence(op2) Then
                                Queue.Push(Token_Stack.Pop)
                            Else
                                Exit Do
                            End If
                        Else
                            Done = True
                        End If
                    Else
                        Done = True
                    End If
                Loop

                Token_Stack.Push(op1)
            Else   'number
                'actually, wrong operator could end up here, like say %
                'If the token is a number, then add it to the output queue.

                If IsNumeric(Token) Or (Token = "F") Then
                    Queue.Push(Token)
                End If

            End If

            Q = ""
            For J = 0 To Queue.Count - 1
                Q &= Queue(J) & " "
            Next

            C = ""
            For J = 0 To Token_Stack.Count - 1
                C &= Token_Stack(J) & " "
            Next

        Next

        Dim S As String = ""

        While Queue.Count > 0
            Token_Stack.Push(Queue.Pop)
        End While

        While Token_Stack.Count > 0
            S &= Token_Stack.Pop & " "
        End While

        'Output : 3 4 2 * 1 5 - 2 3 ^ ^ / +

        Return S

    End Function

    Public Function Evaluate(Expression As String)
        Dim Exp() As String = Trim(Expression).Split(" ")
        Dim Token As String
        Dim Token_Stack As New Stack
        Dim Value As Single = 0
        Dim Debug As String = ""
        Dim X As Single
        Dim Fn As String

        'Original : 3 + 4 * 2 / ( 1 - 5 ) ^ 2 ^ 3

        'Input : 3 4 2 * 1 5 - 2 3 ^ ^ / +
        'Value : 3.00012

        For I = 0 To Exp.Count - 1
            Token = Exp(I)
            If Token <> "" Then

                Select Case Token
                    Case "+"
                        Debug &= Token_Stack(0) & " + " & Token_Stack(1)
                        Token_Stack.Push(CSng(Token_Stack.Pop) + CSng(Token_Stack.Pop))
                    Case "-"
                        Debug &= Token_Stack(1) & " - " & Token_Stack(0)
                        Token_Stack.Push(-CSng(Token_Stack.Pop) + CSng(Token_Stack.Pop))
                    Case "*"
                        Debug &= Token_Stack(0) & " * " & Token_Stack(1)
                        Token_Stack.Push(CSng(Token_Stack.Pop) * CSng(Token_Stack.Pop))
                    Case "/"
                        Debug &= Token_Stack(0) & " / " & Token_Stack(1)
                        Token_Stack.Push(CSng(Token_Stack.Pop) / CSng(Token_Stack.Pop))
                    Case "^"
                        Debug &= Token_Stack(0) & " ^ " & Token_Stack(1)
                        Token_Stack.Push(CSng(Token_Stack.Pop) ^ CSng(Token_Stack.Pop))

                    Case "%"
                        Debug &= Token_Stack(0) & " % " & Token_Stack(1)

                        X = CSng(Token_Stack.Pop)
                        Fn = Token_Stack.Pop
                        Select Case Fn
                            Case "F"
                                X = Math.Sqrt(X)
                        End Select

                        Token_Stack.Push(X)

                    Case "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "F"
                        Token_Stack.Push(Token)
                End Select

                If Operators.Contains(Token) Then
                    Debug &= " = " & Token_Stack.Peek & vbCrLf
                End If

            End If

        Next

        Debug = Expression & vbCrLf & Debug

        '"1 2 + 3 4 + * F + 5 6 + %  "

        '"1 2 + 3 4 + * F 5 6 + % +  "

        Debug &= "Answer : " & Token_Stack.Peek

        Return Token_Stack.Pop

    End Function

End Class
