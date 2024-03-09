
Public Class Class_Equation_Parser

    'Private m_Primatives As Hashtable

    Private Enum Precedence
        None = 11
        Unary = 10      ' Not actually used.
        Power = 9
        Times = 8
        Div = 7
        IntDiv = 6
        Modulus = 5
        Plus = 4
        Logical = 3
    End Enum

    Private Enum Conditional_Precedence
        NONE = 11
        _NOT = 10      ' Not actually used.
        _AND = 9
        _OR = 8
        _EQ = 7
        _GT = 6
        _LT = 5
        _GE = 4
        _LE = 3
    End Enum

    Public Error_Messages As New List(Of String)

    Public Function Check_Conditional_Expression(Expr As String) As Boolean
        Dim expr_len As Integer = Expr.Length

        Dim Open_Cnt As Integer = 0
        Dim Close_Cnt As Integer = 0

        For pos As Integer = 0 To expr_len - 1
            If Mid(Expr, 1, 1) = "(" Then Open_Cnt += 1
            If Mid(Expr, 1, 1) = ")" Then Close_Cnt += 1
        Next

        If Open_Cnt <> Close_Cnt Then Return False

        Return True

    End Function

    Public Function Initialize_Conditional_Expression(Expression As String) As String
        Dim expr As String

        expr = Trim(Expression.ToLower)

        'If (Mid(expr, 1, 1) <> "(") Or (Mid(expr, Expression.Length, 1) <> ")") Then
        '    expr = "(" & expr & ")"
        'End If

        expr = expr.Replace("and", " & ")
        expr = expr.Replace("or", " | ")
        expr = expr.Replace("not", "~")
        expr = expr.Replace(">=", "@")
        expr = expr.Replace("<=", "$")
        expr = expr.Replace("<>", "?")
        expr = expr.Replace(" ", "")

        Return expr
    End Function

    Public Function Evaluate_Conditional_Expression(ByVal expression As String) As Boolean
        Dim expr As String
        Dim is_unary As Boolean
        Dim next_unary As Boolean
        Dim parens As Integer
        Dim expr_len As Integer
        Dim ch As String
        Dim lexpr As String
        Dim rexpr As String
        Dim best_pos As Integer
        Dim best_prec As Precedence

        Error_Messages.Clear()

        expr = expression.ToLower
        expr_len = Len(expr)
        If expr_len = 0 Then Return 0

        ' If we find + or - now, it is a unary operator.
        is_unary = True

        ' So far we have nothing.
        best_prec = Precedence.None

        ' Find the operator with the lowest precedence.
        ' Look for places where there are no open
        ' parentheses.
        For pos As Integer = 0 To expr_len - 1
            ' Examine the next character.
            ch = expr.Substring(pos, 1)

            ' Assume we will not find an operator. In
            ' that case, the next operator will not
            ' be unary.
            next_unary = False

            If ch = " " Then
                ' Just skip spaces. We keep them here
                ' to make the error messages easier to
            ElseIf ch = "(" Then
                ' Increase the open parentheses count.
                parens += 1

                ' A ~ after "(" is unary.
                next_unary = True
            ElseIf ch = ")" Then
                ' Decrease the open parentheses count.
                parens -= 1

                ' An operator after ")" is not unary.
                next_unary = False

                ' If parens < 0, too many ')'s.
                If parens < 0 Then
                    Error_Messages.Add("Too many close parentheses in '" & expression & "'")
                End If
            ElseIf parens = 0 Then
                ' See if this is an operator.
                If ch = "&" Or ch = "|" Or _
                   ch = "~" Or ch = "=" Or _
                   ch = "@" Or ch = "$" Or _
                   ch = ">" Or ch = "<" _
                Then
                    ' An operator after an operator
                    ' is unary.
                    next_unary = True

                    ' See if this operator has higher
                    ' precedence than the current one.
                    Select Case ch
                        Case "=", "@", "$", "<", ">"
                            If best_prec >= Conditional_Precedence._EQ Then
                                best_prec = Conditional_Precedence._EQ
                                best_pos = pos
                            End If

                        Case "&", "|"
                            If best_prec >= Conditional_Precedence._AND Then
                                best_prec = Conditional_Precedence._AND
                                best_pos = pos
                            End If

                        Case "~"
                            ' Ignore unary operators
                            ' for now.
                            If (Not is_unary) And _
                                best_prec >= Conditional_Precedence._NOT _
                            Then
                                best_prec = Conditional_Precedence._NOT
                                best_pos = pos
                            End If
                    End Select
                End If
            End If
            is_unary = next_unary
        Next pos

        ' If the parentheses count is not zero,
        ' there's a ')' missing.
        If parens <> 0 Then
            Error_Messages.Add("Missing close parenthesis in '" & expression & "'")
        End If

        ' Hopefully we have the operator.
        If best_prec < Precedence.None Then
            lexpr = expr.Substring(0, best_pos)
            rexpr = expr.Substring(best_pos + 1)
            Select Case expr.Substring(best_pos, 1)

                Case "&"
                    If Evaluate_Expression(lexpr) And Evaluate_Expression(rexpr) Then
                        Return True
                    Else
                        Return False
                    End If

                Case "$"
                    If Evaluate_Expression(lexpr) Or Evaluate_Expression(rexpr) Then
                        Return True
                    Else
                        Return False
                    End If


                Case "="
                    If Evaluate_Expression(lexpr) = Evaluate_Expression(rexpr) Then
                        Return True
                    Else
                        Return False
                    End If

                Case "@"
                    If Evaluate_Expression(lexpr) >= Evaluate_Expression(rexpr) Then
                        Return True
                    Else
                        Return False
                    End If

                Case "$"
                    If Evaluate_Expression(lexpr) <= Evaluate_Expression(rexpr) Then
                        Return True
                    Else
                        Return False
                    End If

                Case "<"
                    If Evaluate_Expression(lexpr) < Evaluate_Expression(rexpr) Then
                        Return True
                    Else
                        Return False
                    End If

                Case ">"
                    If Evaluate_Expression(lexpr) > Evaluate_Expression(rexpr) Then
                        Return True
                    Else
                        Return False
                    End If
            End Select
        End If

        ' If we do not yet have an operator, there
        ' are several possibilities:
        '
        ' 1. expr is (expr2) for some expr2.
        ' 2. expr is -expr2 or +expr2 for some expr2.
        ' 4. expr is a primitive.
        ' 5. It's a literal like "3.14159".

        ' Look for (expr2).
        If expr.StartsWith("(") And expr.EndsWith(")") Then
            ' Remove the parentheses.
            Return Evaluate_Expression(expr.Substring(1, expr_len - 2))
            Exit Function
        End If

        ' Look for -expr2.
        If expr.StartsWith("~") Then
            Return Not Evaluate_Expression(expr.Substring(1))
        End If

        Try
            ' Try to convert the expression into a Single.
            If IsNumeric(expr) Then
                Dim value As Single = Single.Parse(expr)
                Return value
            Else
                Return 0
            End If
        Catch ex As Exception
            Error_Messages.Add("Error evaluating '" & expression & "' as a constant.")
        End Try

        Return 0

    End Function

    'Public Function Evaluate_Conditional_Expression(Expr As String) As Boolean
    '    Dim expr_len As Integer
    '    Dim Open_Pos As Integer = 0
    '    Dim Close_Pos As Integer = 0
    '    Dim Term As String = ""
    '    Dim S() As String
    '    Dim Left_Value as Single
    '    Dim Right_Value as Single
    '    Dim Term_Value as Single

    '    expr_len = expr.Length

    '    '((x > (Z + 5)) and not (y<2))
    '    '
    '    'Find inner set of ()
    '    For pos As Integer = 1 To expr_len
    '        If Mid(Expr, pos, 1) = "(" Then Open_Pos = pos
    '        If Mid(Expr, pos, 1) = ")" Then
    '            Close_Pos = pos

    '            Term = Mid(Expr, Open_Pos + 1, Close_Pos - Open_Pos - 1)

    '            If Term.Contains("&") Then 'And
    '                S = Term.Split("&")
    '                Left_Value = Evaluate_Conditional_Expression(S(0))
    '                Right_Value = Evaluate_Conditional_Expression(S(1))
    '                If Left_Value And Right_Value Then Return True

    '            ElseIf Term.Contains("|") Then 'Or
    '                S = Term.Split("|")
    '                Left_Value = Evaluate_Conditional_Expression(S(0))
    '                Right_Value = Evaluate_Conditional_Expression(S(1))
    '                If Left_Value And Right_Value Then Return True

    '            ElseIf Term.Contains("@") Then '>=
    '                S = Term.Split("@")
    '                Left_Value = Evaluate_Expression(S(0))
    '                Right_Value = Evaluate_Expression(S(1))
    '                If Left_Value >= Right_Value Then Return True

    '            ElseIf Term.Contains("$") Then '<=
    '                S = Term.Split("$")
    '                Left_Value = Evaluate_Expression(S(0))
    '                Right_Value = Evaluate_Expression(S(1))
    '                If Left_Value <= Right_Value Then Return True

    '            ElseIf Term.Contains(">") Then
    '                S = Term.Split(">")
    '                Left_Value = Evaluate_Expression(S(0))
    '                Right_Value = Evaluate_Expression(S(1))
    '                If Left_Value > Right_Value Then Return True

    '            ElseIf Term.Contains("<") Then
    '                S = Term.Split("<")
    '                Left_Value = Evaluate_Expression(S(0))
    '                Right_Value = Evaluate_Expression(S(1))
    '                If Left_Value < Right_Value Then Return True

    '            ElseIf Term.Contains("=") Then
    '                S = Term.Split("=")
    '                Left_Value = Evaluate_Expression(S(0))
    '                Right_Value = Evaluate_Expression(S(1))
    '                If Left_Value = Right_Value Then Return True
    '            Else
    '                Term_Value = Evaluate_Expression(Term)
    '                Expr = Expr.Replace("(" & Term & ")", Term_Value)
    '                Return Evaluate_Conditional_Expression(Expr)
    '            End If

    '        End If
    '    Next

    '    Return False

    'End Function

    'Public Function Evaluate_Conditional(Left_Value As String, Op As String, Right_Value As String) As Boolean

    '    If IsNumeric(Left_Value) And IsNumeric(Right_Value) Then
    '        Dim CV As Single = Left_Value
    '        Dim FV As Single = Right_Value

    '        Select Case Op
    '            Case "", "="
    '                If CV = FV Then Return True
    '            Case "<>"
    '                If CV <> FV Then Return True
    '            Case ">"
    '                If CV > FV Then Return True
    '            Case ">="
    '                If CV >= FV Then Return True
    '            Case "<"
    '                If CV < FV Then Return True
    '            Case "<="
    '                If CV <= FV Then Return True
    '        End Select

    '    Else
    '        Select Case Op
    '            Case "", "="
    '                If Left_Value = Right_Value Then Return True
    '            Case "<>"
    '                If Left_Value <> Right_Value Then Return True
    '        End Select
    '    End If

    '    Return False

    'End Function

    ' Evaluate the expression.
    'Beware this is a recursive routine (calls itself)

    Public Function Evaluate_Expression(ByVal expression As String) As Single
        Dim expr As String
        Dim is_unary As Boolean
        Dim next_unary As Boolean
        Dim parens As Integer
        Dim expr_len As Integer
        Dim ch As String
        Dim lexpr As String
        Dim rexpr As String
        Dim best_pos As Integer
        Dim best_prec As Precedence

        Error_Messages.Clear()

        expr = expression.ToLower
        expr = expr.Replace("and", " & ")
        expr = expr.Replace("or", " | ")
        expr = expr.Replace("not", "~")
        expr = expr.Replace(">=", "@")
        expr = expr.Replace("<=", "$")
        expr = expr.Replace("<>", "?")
        expr = expr.Replace(" ", "")

        expr_len = Len(expr)
        If expr_len = 0 Then Return 0

        ' If we find + or - now, it is a unary operator.
        is_unary = True

        ' So far we have nothing.
        best_prec = Precedence.None

        ' Find the operator with the lowest precedence.
        ' Look for places where there are no open
        ' parentheses.
        For pos As Integer = 0 To expr_len - 1
            ' Examine the next character.
            ch = expr.Substring(pos, 1)

            ' Assume we will not find an operator. In
            ' that case, the next operator will not
            ' be unary.
            next_unary = False

            If ch = " " Then
                ' Just skip spaces. We keep them here
                ' to make the error messages easier to
            ElseIf ch = "(" Then
                ' Increase the open parentheses count.
                parens += 1

                ' A + or - after "(" is unary.
                next_unary = True
            ElseIf ch = ")" Then
                ' Decrease the open parentheses count.
                parens -= 1

                ' An operator after ")" is not unary.
                next_unary = False

                ' If parens < 0, too many ')'s.
                If parens < 0 Then
                    Error_Messages.Add("Too many close parentheses in '" & expression & "'")
                End If
            ElseIf parens = 0 Then
                ' See if this is an operator.
                If ch = "^" Or ch = "*" Or _
                   ch = "/" Or ch = "\" Or _
                   ch = "%" Or ch = "+" Or _
                   ch = "-" Or ch = "&" Or _
                   ch = "|" Or ch = "=" Or _
                   ch = ">" Or ch = "<" Or _
                   ch = "@" Or ch = "$" Or _
                   ch = "~" Or ch = "?" _
                Then
                    ' An operator after an operator
                    ' is unary.
                    next_unary = True

                    ' See if this operator has higher
                    ' precedence than the current one.
                    Select Case ch
                        Case "^"
                            If best_prec >= Precedence.Power Then
                                best_prec = Precedence.Power
                                best_pos = pos
                            End If

                        Case "*", "/", "&", "|"
                            If best_prec >= Precedence.Times Then
                                best_prec = Precedence.Times
                                best_pos = pos
                            End If

                        Case "\"
                            If best_prec >= Precedence.IntDiv Then
                                best_prec = Precedence.IntDiv
                                best_pos = pos
                            End If

                        Case "%"
                            If best_prec >= Precedence.Modulus Then
                                best_prec = Precedence.Modulus
                                best_pos = pos
                            End If

                        Case "+", "-", "~"
                            ' Ignore unary operators
                            ' for now.
                            If (Not is_unary) And _
                                best_prec >= Precedence.Plus _
                            Then
                                best_prec = Precedence.Plus
                                best_pos = pos
                            End If

                        Case "=", ">", "<", "@", "$", "?"
                            If best_prec >= Precedence.Logical Then
                                best_prec = Precedence.Logical
                                best_pos = pos
                            End If

                    End Select
                End If
            End If
            is_unary = next_unary
        Next pos

        ' If the parentheses count is not zero,
        ' there's a ')' missing.
        If parens <> 0 Then
            Error_Messages.Add("Missing close parenthesis in '" & expression & "'")
        End If

        ' Hopefully we have the operator.
        If best_prec < Precedence.None Then
            lexpr = expr.Substring(0, best_pos)
            rexpr = expr.Substring(best_pos + 1)
            Select Case expr.Substring(best_pos, 1)
                Case "^"
                    Return _
                        Evaluate_Expression(lexpr) ^ _
                        Evaluate_Expression(rexpr)
                Case "*"
                    Return _
                        Evaluate_Expression(lexpr) * _
                        Evaluate_Expression(rexpr)
                Case "/"
                    Return _
                        Evaluate_Expression(lexpr) / _
                        Evaluate_Expression(rexpr)
                Case "\"
                    Return _
                        CLng(Evaluate_Expression(lexpr)) \ _
                        CLng(Evaluate_Expression(rexpr))
                Case "%"
                    Return _
                        Evaluate_Expression(lexpr) Mod _
                        Evaluate_Expression(rexpr)
                Case "+"
                    Return _
                        Evaluate_Expression(lexpr) + _
                        Evaluate_Expression(rexpr)
                Case "-"
                    Return _
                        Evaluate_Expression(lexpr) - _
                        Evaluate_Expression(rexpr)
                Case "&"
                    Dim L As Boolean = Evaluate_Expression(lexpr)
                    Dim R As Boolean = Evaluate_Expression(rexpr)

                    Return _
                       Evaluate_Expression(lexpr) And _
                       Evaluate_Expression(rexpr)
                Case "|"
                    Return _
                       Evaluate_Expression(lexpr) Or _
                       Evaluate_Expression(rexpr)

                    'Logical operators
                Case "="
                    If Evaluate_Expression(lexpr) = Evaluate_Expression(rexpr) Then
                        Return True
                    Else
                        Return False
                    End If

                Case "?"
                    If Evaluate_Expression(lexpr) <> Evaluate_Expression(rexpr) Then
                        Return True
                    Else
                        Return False
                    End If

                Case "@" '>=
                    If Evaluate_Expression(lexpr) >= Evaluate_Expression(rexpr) Then
                        Return True
                    Else
                        Return False
                    End If

                Case "$" '<=
                    If Evaluate_Expression(lexpr) <= Evaluate_Expression(rexpr) Then
                        Return True
                    Else
                        Return False
                    End If

                Case ">"
                    If Evaluate_Expression(lexpr) > Evaluate_Expression(rexpr) Then
                        Return True
                    Else
                        Return False
                    End If

                Case "<"
                    If Evaluate_Expression(lexpr) < Evaluate_Expression(rexpr) Then
                        Return True
                    Else
                        Return False
                    End If

            End Select
        End If

        ' If we do not yet have an operator, there
        ' are several possibilities:
        '
        ' 1. expr is (expr2) for some expr2.
        ' 2. expr is -expr2 or +expr2 for some expr2.
        ' 3. expr is Fun(expr2) for a function Fun.
        ' 4. expr is a primitive.
        ' 5. It's a literal like "3.14159".

        ' Look for (expr2).
        If expr.StartsWith("(") And expr.EndsWith(")") Then
            ' Remove the parentheses.
            Return Evaluate_Expression(expr.Substring(1, expr_len - 2))
            Exit Function
        End If

        ' Look for -expr2.
        If expr.StartsWith("-") Then
            Return -Evaluate_Expression(expr.Substring(1))
        End If

        ' Look for +expr2.
        If expr.StartsWith("+") Then
            Return Evaluate_Expression(expr.Substring(2))
        End If

        If expr.StartsWith("~") Then
            Return Not Evaluate_Expression(expr.Substring(2))
        End If

        ' Look for Fun(expr2).
        If expr_len > 5 And expr.EndsWith(")") Then
            ' Find the first (.
            Dim paren_pos As Integer = expr.IndexOf("(")
            If paren_pos > 0 Then
                ' See what the function is.
                lexpr = expr.Substring(0, paren_pos)
                rexpr = expr.Substring(paren_pos + 1, expr_len - paren_pos - 2)
                Select Case lexpr.ToLower
                    Case "deg"
                        Return (180 * Evaluate_Expression(rexpr) / Math.PI)
                    Case "rad"
                        Return (Math.PI * Evaluate_Expression(rexpr) / 180)
                    Case "sin"
                        Return Math.Sin(Evaluate_Expression(rexpr))
                    Case "cos"
                        Return Math.Cos(Evaluate_Expression(rexpr))
                    Case "tan"
                        Return Math.Tan(Evaluate_Expression(rexpr))
                    Case "asin"
                        Return Math.Asin(Evaluate_Expression(rexpr))
                    Case "acos"
                        Return Math.Acos(Evaluate_Expression(rexpr))
                    Case "atan"
                        Return Math.Atan(Evaluate_Expression(rexpr))
                    Case "sinh"
                        Return Math.Sinh(Evaluate_Expression(rexpr))
                    Case "cosh"
                        Return Math.Cosh(Evaluate_Expression(rexpr))
                    Case "tanh"
                        Return Math.Tanh(Evaluate_Expression(rexpr))
                    Case "sqrt"
                        Return Math.Sqrt(Evaluate_Expression(rexpr))
                    Case "fact"
                        Return Factorial(Evaluate_Expression(rexpr))
                    Case "abs"
                        Return Math.Abs(Evaluate_Expression(rexpr))
                    Case "log"
                        Return Math.Log(Evaluate_Expression(rexpr))
                    Case "log10"
                        Return Math.Log10(Evaluate_Expression(rexpr))
                    Case "round"
                        Return Calc.Round(Evaluate_Expression(rexpr))
                    Case "celing"
                        Return Math.Ceiling(Evaluate_Expression(rexpr))
                    Case "floor"
                        Return Math.Floor(Evaluate_Expression(rexpr))

                        ' Add other functions (including
                        ' program-defined functions) here.
                End Select
            End If
        End If

        Try
            ' Try to convert the expression into a Single.
            If IsNumeric(expr) Then
                Dim value As Single = Single.Parse(expr)
                Return value
            Else
                Return 0
            End If
        Catch ex As Exception
            Error_Messages.Add("Error evaluating '" & expression & "' as a constant.")
        End Try

        Return 0

    End Function

    ' Return the factorial of the expression.
    Private Function Factorial(ByVal value As Single) As Single
        Dim result As Single

        ' Make sure the value is an integer.
        If CLng(value) <> value Then
            Error_Messages.Add("Parameter to Factorial function must be an integer in Factorial(" & Format(value) & ")")
            Return 0
        End If

        result = 1
        Do While value > 1
            result = result * value
            value = value - 1
        Loop
        Return result

    End Function


End Class
