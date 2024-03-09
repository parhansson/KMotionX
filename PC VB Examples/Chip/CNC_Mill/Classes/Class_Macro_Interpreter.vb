Imports System.IO

'This class interprets and executes macros
'Macros are G code programming language extended to provide looping, commands to the controller and math expressions
'
'This is built for speed when the macro is executed. 
'Progam lines are distilled down to a Reverse Polish Notation stack that can be executed quickly at run time.
'
'The parsing is broken up into steps so that the code is easier to understand.
'  1. The programs lines are broken up into 'Components'
'     a component is either a keyword, local or global variable, operator or function
'     These components are each assigned a type. see enum_Component_Type
'  2. The program lines and components are checked for errors.
'  3. The level of each program line is established. eg. statements within a FOR loop are a higher level than the FOR
'  4. The expession are put into reverse Polish notation stacks (RPN), eg. expressions in FOR, WHILE, UNTIL and single line expressions
'  5. The straight line, and branch destinations are are found. ie. lines to go to when a conditional expression is True or False.
'  6. The links to local and global variables are connected. Links or pointers are used for speed. For example int the
'     statement X = I + 5  the pointer to X is stored as the destination.
'     I, + and 5 are all stored in the RPN stack (I 5 +). When the RPN stack is executed, the pointer to I is used to get its value,
'     5 is a constant, and + is the operator.

'Macros are executed by sequencing through the RPN stacks in each statement and if there is a branch, then branching to the proper 
'next statement to be executed. Some statements (like LABEL and MACRO) do not have an RPN stack, most others have only one RPN stack
'but the FOR has two because there may be two expressions eg. FOR I = J+5 TO K+6, the for expression is J+5 and the TO K+6

Public Class Class_Macro_Interpreter

    Public Shared Valid_Symbol_Characters As String = ".#_%$?!ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
    Public Shared Illegal_Characters As String = "~`|@;,"

#Region "Help string constants"

    Public Enum enum_Error_Help_Messages
        Macro_Help
        IF_Help
        FOR_Help
        WHILE_Help
        DO_Help
        Local_WHOLE_Variable_Help
        Local_FLOAT_Variable_Help
        Local_STRING_Variable_Help
        Local_WHOLE_LIST_Variable_Help
        Local_REAL_LIST_Variable_Help
        Local_STRING_LIST_Variable_Help
        String_Constant_Help
        Symbol_Help
        Logical_EXPRESSION_Help
        Math_EXPRESSION_Help
        String_Functions_Help
        Math_Function_Help
        LABEL_Help
        BREAKPOINT_Help
        GOTO_Help
        CALL_Help
        Unpaired_Help
        CYCLE_Help
        SPINDLE_Help
        COOLANT_Help
        FEED_OVERRIDE_Help
        PAUSE_Help
        MESSAGE_Help
        BROWSE_Help
        READ_Help
        WRITE_Help
        System_Functions_Help
    End Enum

    Private Help_MACRO As String = "Correct MACRO statement format is : " & vbCrLf & vbCrLf & _
                                   "MACRO name" & vbCrLf & _
                                   "  statements" & vbCrLf & _
                                   "  .." & vbCrLf & _
                                   "MACRO_END" & vbCrLf & _
                                   "where 'name' is the unique name of the macro"


    Private Help_Local_INTEGER_Variable As String = "Correct local WHOLE variable format is : " & vbCrLf & vbCrLf & _
                                                    "WHOLE name=n" & vbCrLf & _
                                                    "where 'name' is the unique name of the local variable" & vbCrLf & _
                                                    "'n' is an integer initialization value" & vbCrLf & _
                                                    "'name' cannot be a number eg. #1"

    Private Help_Local_FLOAT_Variable As String = "Correct local FLOAT variable format is : " & vbCrLf & vbCrLf & _
                                                  "REAL name=n" & vbCrLf & _
                                                  "where 'name' is the unique name of the local variable" & vbCrLf & _
                                                  "'n' is a floating point initialization value" & vbCrLf & _
                                                  "'name' cannot be a number eg. #1"

    Private Help_Local_STRING_Variable As String = "Correct local string variable format is :" & vbCrLf & vbCrLf & _
                                                   "STRING name= ""s""" & vbCrLf & _
                                                   "where 'name' is the unique name of the local variable" & vbCrLf & _
                                                   "'s' is a string initialization value"

    Private Help_Local_INTEGER_LIST_Variable As String = "Correct local INTEGER_LIST variable format is : " & vbCrLf & vbCrLf & _
                                                      "WHOLE[] name=n" & vbCrLf & _
                                                      "where 'name' is the unique name of the local variable" & vbCrLf & _
                                                      "'n' is an integer initialization value" & vbCrLf & _
                                                      "'name' cannot be a number eg. #1"

    Private Help_Local_FLOAT_LIST_Variable As String = "Correct local FLOAT_LIST variable format is : " & vbCrLf & vbCrLf & _
                                                       "REAL[] name=n" & vbCrLf & _
                                                       "where 'name' is the unique name of the local variable" & vbCrLf & _
                                                       "'n' is a floating point initialization value" & vbCrLf & _
                                                       "'name' cannot be a number eg. #1"

    Private Help_Local_STRING_LIST_Variable As String = "Correct local string variable format is :" & vbCrLf & vbCrLf & _
                                                        "STRING[] name= ""s""" & vbCrLf & _
                                                        "where 'name' is the unique name of the local variable" & vbCrLf & _
                                                        "'s' is a string initialization value"

    Private Help_String_Constants As String = "Correct string constnat format is :" & vbCrLf & vbCrLf & _
                                              """s""" & vbCrLf & _
                                              " where s is a constant string, must be enclosed in quote characters" & vbCrLf & vbCrLf & _
                                              "System String Constants : " & vbCrLf & vbCrLf & _
                                              "  ""    Empty string" & vbCrLf & _
                                              "  CR    Carraige retrun character" & vbCrLf & _
                                              "  LF    Line feed character" & vbCrLf & _
                                              "  CRLF  Carraige retrun amd line feed character"


    Private Help_IF As String = "Correct IF statement format is : " & vbCrLf & vbCrLf & _
                             "IF exp THEN" & vbCrLf & _
                             "  statements" & vbCrLf & _
                             "  .." & vbCrLf & _
                             "ELSE" & vbCrLf & _
                             "  .." & vbCrLf & _
                             "IF_END" & vbCrLf & vbCrLf & _
                             "where 'exp' is a boolean conditional expression" & vbCrLf & _
                             "'ELSE' and 'IF_END' must be on lines all by themselves"


    Private Help_FOR As String = "Correct FOR statement format is : " & vbCrLf & vbCrLf & _
                                 "FOR n = start to finish" & vbCrLf & _
                                 "  statements" & vbCrLf & _
                                 "  .." & vbCrLf & _
                                 "NEXT" & vbCrLf & vbCrLf & _
                                 "'n' is the loop counter, " & vbCrLf & _
                                 "'start' is the loop counter starting value, " & vbCrLf & _
                                 "'finish' is the loop counter end value" & vbCrLf & _
                                 "'NEXT' must be on line all by itself" & vbCrLf

    Private Help_WHILE As String = "Correct WHILE statement tormat is : " & vbCrLf & vbCrLf & _
                                   "WHILE exp" & vbCrLf & _
                                   "  statements" & vbCrLf & _
                                   "  .." & vbCrLf & _
                                   "WHILE_END" & vbCrLf & vbCrLf & _
                                   "where 'exp' is a boolean conditional expression" & vbCrLf & _
                                   "'WHILE_END' must be on line all by itself" & vbCrLf

    Private Help_DO As String = "Correct DO statement format is : & vbCrLf " & vbCrLf & _
                                "DO" & vbCrLf & _
                                "  statements" & vbCrLf & _
                                "  .." & vbCrLf & _
                                "UNTIL" & vbCrLf & vbCrLf & _
                                "where 'exp' is a boolean conditional expression" & vbCrLf & _
                                "'DO' must be on line all by itself" & vbCrLf

    Private Help_SYMBOL As String = "The symbol was not defined as " & vbCrLf & _
                                    "a local or a global variable " & vbCrLf & _
                                    "ie. it is not in the Global symbol table" & vbCrLf & vbCrLf & _
                                    "Symbol cannot start with a numeric character eg. 2Count " & vbCrLf & _
                                    "and must be composed of the following valid characters : " & vbCrLf & _
                                    Valid_Symbol_Characters


    Private Help_Logical_EXPRESSION As String = "Correct logical expression format is :" & vbCrLf & vbCrLf & _
                                                "exp1 op exp2" & vbCrLf & _
                                                "'exp1' and 'exp2' are expressions" & vbCrLf & _
                                                "eg. (#I + #J) * 2" & vbCrLf & _
                                                "and 'op' is +, -, =, <>, >=, <=, >, <, " & vbCrLf & _
                                                "AND, OR, NOT  eg. #I < 5"

    Private Help_Math_EXPRESSION As String = "Correct mathematical expression format is :" & vbCrLf & vbCrLf & _
                                              "Var = exp, or  Var = exp1 op exp2" & vbCrLf & _
                                              "'exp', 'exp1' and 'exp2' are expressions eg. #I = (#J + #K) * 2" & vbCrLf & _
                                              "'op' is an operation :" & vbCrLf & _
                                              " +   Add" & vbCrLf & _
                                              " - Subtract" & vbCrLf & _
                                              " *  Multiply" & vbCrLf & _
                                              " /  Divide" & vbCrLf & _
                                              " ^  Power eg #I ^ 2  is I squared" & vbCrLf & vbCrLf

    Private Help_LABEL As String = "Correct LABEL statement format is : " & vbCrLf & vbCrLf & _
                                   ":name" & vbCrLf & _
                                   "where 'name' is the unique name of the label (within the macro)" & vbCrLf & _
                                   "a lable must be on line all by itself"

    Private Help_BREAKPOINT As String = "Correct BREAKPOINT statement format is : " & vbCrLf & vbCrLf & _
                                        "BREAKPOINT" & vbCrLf & _
                                        "BREAKPOINT must be on line all by itself"

    Private Help_GOTO As String = "Correct GOTO statement format is : " & vbCrLf & vbCrLf & _
                                  "GOTO name:" & vbCrLf & _
                                  "where 'name' is the the label to branch to"

    Private Help_CALL As String = "Correct CALL statement format is : " & vbCrLf & vbCrLf & _
                                  "CALL name(param1, ..., param_n)" & vbCrLf & _
                                  "where 'name' is the unique name of the subroutine, " & vbCrLf & _
                                  "params are list of parameters passed to/from subroutine"

    Private Help_Unpaired As String = "MACRO, IF, FOR, WHILE an DO statements" & vbCrLf &
                                      "must have a matching end statement" & vbCrLf & _
                                      "One or more end statements are missing"

    Private Help_Math_Functions As String = "Math functions : eg. X = SIN(A)" & vbCrLf & vbCrLf & _
                                            "RAD(Angle in degrees)  RAD is angle in radians" & vbCrLf & _
                                            "                       (pi * angle in degrees) / 180" & vbCrLf & _
                                            "DEG(Angle in radians)  DEG is angle in degrees" & vbCrLf & _
                                            "                       (180 * angle in degrees) / pi" & vbCrLf & _
                                            "SIN(Angle in radians)  Sine of the specified angle" & vbCrLf & _
                                            "COS(Angle in radians)  Cosine of the specified angle" & vbCrLf & _
                                            "TAN(Angle in radians)  Tangent of the specifed angle" & vbCrLf & _
                                            "ASIN(n)                Returns angle (in radians) whose SIN is n" & vbCrLf & _
                                            "ACOS(n)                Returns angle (in radians) whose COS is n" & vbCrLf & _
                                            "ATAN(n)                Returns angle (in radians) whose TAN is n" & vbCrLf & _
                                            "SINH(Angle in radians) Hyperbolic sine of the specified angle" & vbCrLf & _
                                            "COSH(Angle in radians) Hyperbolic cosine of the specified angle" & vbCrLf & _
                                            "TANH(Angle in radians) Hyperbolic tangent of the specified angle" & vbCrLf & _
                                            "SQRT(expression)       Aquare root of expression" & vbCrLf & _
                                            "ABS(expression)        Absolute value of expression" & vbCrLf & _
                                            "LOG(expression)        Logarithm of expression" & vbCrLf & _
                                            "LOG10(expression)      Base 10 logarithm of expression" & vbCrLf & _
                                            "ROUND(expression)      Rounds a FLOAT to the nearest INTEGER value" & vbCrLf & _
                                            "CELING(expression)     Smallest INTEGER value >= expression" & vbCrLf & _
                                            "FLOOR(expression)      Largest INTEGER value <= expression"

    Private Help_String_Functions As String = "Correct string function format is : " & vbCrLf & vbCrLf & _
                                              "LEN(expr)         Length of string expressions" & vbCrLf & _
                                              "FORMAT(FLOAT, n)  Formats a FLOAT, n is siginifiant digits after decimal"

    Private Help_Cycle As String = "Correct CYCLE format is :" & vbCrLf & vbCrLf & _
                                   "CYCLE(c) where c = START, STOP, HOLD, or STEP" & vbCrLf & _
                                   "c is missing or not one of the system internal constants above"

    Private Help_Spindle As String = "Correct SPINDLE format is :" & vbCrLf & vbCrLf & _
                                   "SPINDLE(c) where c = OFF, CW, CCW" & vbCrLf & _
                                   "c is missing or not one of the system internal constants above"

    Private Help_Coolant As String = "Correct COOLANT format is :" & vbCrLf & vbCrLf & _
                                     "COOLANT(c) where c = ON or OFF" & vbCrLf & _
                                     "c is missing or not one of the system internal constants above"

    Private Help_Feed_Overide As String = "Correct FEED_OVERRIDE format is :" & vbCrLf & vbCrLf & _
                                          "FEED_OVERRIDE(p) where p is a percentage" & vbCrLf & _
                                          "p is missing or not in the range of 0 to 200 (percent)"

    Private Help_Pause As String = "Correct PAUSE formats are :" & vbCrLf & vbCrLf & _
                                    "PAUSE or PAUSE(msg, ctl, b)" & vbCrLf & _
                                    "msg, ctl or b parameters are illegal or missing"

    Private Help_Message As String = "Correct MESSAGE format id :" & vbCrLf & vbCrLf & _
                                     "MESSAGE(msg, ctl, b)" & vbCrLf & _
                                     "msg, ctl or b parameters are illegal or missing"

    Private Help_Browse As String = "Correct BROWSE format is :" & vbCrLf & vbCrLf & _
                                    "BROWSE(c, dir, filter)" & vbCrLf & _
                                    "c, dir, filter missing or illegal characters in dir or" & vbCrLf & _
                                    "c is not LOAD or SAVE"

    Private Help_READ As String = "Correct READ format is :" & vbCrLf & vbCrLf & _
                                  "READ(dir, filter)" & vbCrLf & _
                                  "either dir, filter or both are missing or " & vbCrLf & _
                                   "illegal characters in dir or filter"

    Private Help_WRITE As String = "Correct WRITE format is :" & vbCrLf & vbCrLf & _
                                   "WRITE(Filename, Data)" & vbCrLf & _
                                   "either Filename or Data or both are missing or " & vbCrLf & _
                                   "illegal characters in filename"

    Private Help_System_Functions As String = "Correct systen function format is : " & vbCrLf & vbCrLf & _
                                              "CYCLE(c)             Controls execution of macro c = system constant START or STOP" & vbCrLf & _
                                              "SPINDLE(c)           c is system internal constant is CW, CCW or OFF" & vbCrLf & _
                                              "COOLANT(const)       c is system internal constant ON or OFF" & vbCrLf & _
                                              "FEED_OVERRIDE(p)     Set feedrate overide percent 0 to 200, 100 is no overide" & vbCrLf & _
                                              "                     c = START start at current macro line" & vbCrLf & _
                                              "                     c = STOP stop motion and macro execution" & vbCrLf & _
                                              "                         does not wait for current line to complete" & vbCrLf & _
                                              "                         uesd for E-Stop and jog functions" & vbCrLf & _
                                              "                     c = HOLD stops motion, CYCLE(START) to continue" & vbCrLf & _
                                              "                     c = STEP executes next macro line" & vbCrLf & _
                                              "PAUSE                Pauses macro execution, use CYCLE_START to continue" & vbCrLf & _
                                              "                     msg is put into ctl text property" & vbCrLf & _
                                              "                     eg. PAUSE(""Macro paused, Cycle Start to continue"",txt_User_Info)" & vbCrLf & _
                                              "                         PAUSE(""Macro paused, Cycle Start to continue"",txt_User_Info)" & vbCrLf & _
                                              "PAUSE(msg, ctl, b)   Pauses macro execution" & vbCrLf & _
                                              "                     ctl = form control" & vbCrLf & _
                                              "                           msg is putinto ctl.Text property" & vbCrLf & _
                                              "                           b is specified is and error" & vbCrLf & _
                                              "                     ctl = ystem internal constant FLASH_BOX" & vbCrLf & _
                                              "                           msg displayed in pop up box" & vbCrLf & _
                                              "                           pop up boc closes after b seconds" & vbCrLf & _
                                              "                     ctl = ystem internal constant POP_UP_BOX then form control, msg is putinto ctl.Text property" & vbCrLf & _
                                              "                           msg displayed in pop up box" & vbCrLf & _
                                              "                           box closes when user clicks a button" & vbCrLf & _
                                              "                           b is a string that defines buttons" & vbCrLf & _
                                              "                           O=OK, Y=Yes, N=No, A=Abort and C=Cancel" & vbCrLf & _
                                              "                           eg. ""YN"" dialog has Yes and No button" & vbCrLf & _
                                              "                           returns button clicked eg. ""Y""" & vbCrLf & _
                                              "                     eg. IF PAUSE(""Continue?"",txt_Pause,""YN"") = ""Y"" THEN" & vbCrLf & _
                                              "                           CYCLE(START)" & vbCrLf & _
                                              "                         ELSE" & vbCrLf & _
                                              "                           GOTO Abort_Program" & vbCrLf & _
                                              "                         IF_END" & vbCrLf & _
                                              "MESSAGE(msg, ctl, b) Same as pause but : " & vbCrLf & _
                                              "                     DOES NOT CAUSE MOTION OR MACRO EXECUION TO PAUSE" & vbCrLf & _
                                              "BROWSE(c, d, f)      Returns file name from pop up browse dialog" & vbCrLf & _
                                              "                     c = LOAD Open File pop up dialog" & vbCrLf & _
                                              "                     c = SAVE Save File pop up dialog, warns about overwriting file" & vbCrLf & _
                                              "                     d = Initial directory eg. ""C:\Programs\""" & vbCrLf & _
                                              "                     f = Filter is file filter eg. *.txt" & vbCrLf & _
                                              "                     If no file name returned then user did not pick a file" & vbCrLf & _
                                              "                     BROWSE is usually followed by a READ or WRITE command" & vbCrLf & _
                                              "                     eg. My_File = BROWSE(""C:\Programs\"",""*.txt"")" & vbCrLf & _
                                              "                        IF My_File <> """" THEN 'User selected a file" & vbCrLf & _
                                              "                          G_Code_Editor.Text = READ(My_File) 'Put text into edit control" & vbCrLf & _
                                              "                        IF_END" & vbCrLf & _
                                              "READ(Fn)             Reads file (Fn) into string variable" & vbCrLf & _
                                              "                     eg. T = READ(My_File) 'Put text into variable T" & vbCrLf & _
                                              "                     eg. G_Code_Editor.Text = READ(My_File) 'Put text into edit control" & vbCrLf & _
                                              "WRITE(Fn, Data)      Saves text to a file, no popup, will overwrite existing file" & vbCrLf & _
                                              "                      Data us a string variable or control text property" & vbCrLf & _
                                              "                      both code snipits below do the same thing" & vbCrLf & _
                                              "                      eg. WRITE(Fn,G_Code_Edit.Text) " & vbCrLf & _
                                              "                      eg. T = G_Code_Edit.Text " & vbCrLf & _
                                              "                          WRITE(Fn, T)"
#End Region

#Region "Enumerations"

    Private Enum enum_Grid_Columns
        Line
        Block
        Comment
        Error_Message
    End Enum

    Public Enum enum_Run_Commands
        Initialize
        Check
        Rewind
        Continuous
        Single_Step
        Halt
        Start_At
        Abort
    End Enum

    Public Enum enum_Run_Status
        OK
        Halted
        End_Of_Program
        Error_Encountered
    End Enum

    Public Enum enum_Component_Type
        _START
        _TOKEN
        _MACRO_NAME
        _UNKNOWN
        _EXPRESSION
        _COMMENT

        'Keywords
        _KEYWORD
        _KEYWORD_CONTINUATION
        _KEYWORD_MACRO
        _KEYWORD_MACRO_END
        _KEYWORD_WHOLE
        _KEYWORD_REAL
        _KEYWORD_STRING
        _KEYWORD_WHOLE_LIST
        _KEYWORD_REAL_LIST
        _KEYWORD_STRING_LIST
        _KEYWORD_LABEL
        _KEYWORD_IF
        _KEYWORD_THEN
        _KEYWORD_ELSE
        _KEYWORD_IF_END
        _KEYWORD_FOR
        _KEYWORD_TO
        _KEYWORD_NEXT
        _KEYWORD_WHILE
        _KEYWORD_WHILE_END
        _KEYWORD_DO
        _KEYWORD_UNTIL
        _KEYWORD_SELECT
        _KEYWORD_CASE
        _KEYWORD_SELECT_END
        _KEYWORD_GOTO
        _KEYWORD_CALL
        _KEYWORD_BREAKPOINT
        _KEYWORD_CYCLE
        _KEYWORD_SPINDLE
        _KEYWORD_COOLANT
        _KEYWORD_FEED_OVERRIDE
        _KEYWORD_PAUSE
        _KEYWORD_MESSAGE
        _KEYWORD_BROWSE
        _KEYWORD_READ
        _KEYWORD_WRITE
        _KEYWORD_G_CODE

        'Constants
        _CONSTANT
        _CONSTANT_NUMBER
        _CONSTANT_INTEGER
        _CONSTANT_FLOAT
        _CONSTANT_STRING
        _CONSTANT_CR
        _CONSTANT_LF
        _CONSTANT_CRLF

        'Variables
        _LOCAL
        _LOCAL_WHOLE
        _LOCAL_REAL
        _LOCAL_STRING
        _LOCAL_WHOLE_LIST
        _LOCAL_REAL_LIST
        _LOCAL_STRING_LIST

        _GLOBAL
        _GLOBAL_WHOLE
        _GLOBAL_REAL
        _GLOBAL_STRING
        _GLOBAL_VARIABLES

        'Separators
        _SEPARATOR
        _SEPARATOR_SPACE
        _SEPARATOR_OPEN_PAREN
        _SEPARATOR_CLOSE_PAREN

        'Math operators
        _MATH
        _MATH_EQUAL
        _MATH_ADD
        _MATH_SUBTRACT
        _MATH_MULTIPLY
        _MATH_DIVIDE
        _MATH_POWER
        _MATH_FUNCTION
        _MATH_MODULUS

        'Logical operators
        _LOGICAL
        _LOGICAL_NOT_EQ
        _LOGICAL_EQ
        _LOGICAL_GT
        _LOGICAL_LT
        _LOGICAL_GE
        _LOGICAL_LE
        _LOGICAL_NOT
        _LOGICAL_AND
        _LOGICAL_OR

        'Math functions
        _FUNCTION
        _FUNCTION_NEG
        _FUNCTION_DEG
        _FUNCTION_RAD
        _FUNCTION_SIN
        _FUNCTION_COS
        _FUNCTION_TAN
        _FUNCTION_ASIN
        _FUNCTION_ACOS
        _FUNCTION_ATAN
        _FUNCTION_SINH
        _FUNCTION_COSH
        _FUNCTION_TANH
        _FUNCTION_SQRT
        _FUNCTION_ABS
        _FUNCTION_LOG
        _FUNCTION_LOG10
        _FUNCTION_ROUND
        _FUNCTION_CEILING
        _FUNCTION_FLOOR

        _FUNCTION_NEGATE
        'String Functions
        _FUNCTION_TO_FLOAT
        _FUNCTION_TO_INTEGER
        _FUNCTION_FORMAT_NUMBER
        _FUNCTION_LEN

    End Enum

#End Region

#Region "Classes"

    Public Class class_Variable
        Public Type As enum_Component_Type
        Public Name As String
        Public Value As Object
    End Class

    'Public Class class_List
    '    Public Type As enum_Component_Type
    '    Public Name As String
    '    Public Values As New List(Of Object)

    '    Public Sub Clear()
    '        Values.Clear()
    '    End Sub

    '    Public Sub Add(Value As Object)
    '        Values.Add(Value)
    '    End Sub

    '    Public Sub Remove_At(Index As Integer)
    '        Values.RemoveAt(Index)
    '    End Sub

    'End Class

    Public Class class_Label
        Public Name As String
        Public Program_Line As class_Program_Line
    End Class

    Public Class class_Component
        Public Category As enum_Component_Type
        Public Type As enum_Component_Type
        Public Token As String 'Token Text  eg. Count
        Public Variable_Index As Integer
        Public Function_Parameters As List(Of Object)

        Public Sub New()
            Category = enum_Component_Type._UNKNOWN
            Type = enum_Component_Type._UNKNOWN
        End Sub

    End Class

    Public Class class_Expression
        Public Destination_Symbol As String 'eg. I = J + K,  Destination_Symbol = 'I'"
        Public Destination_Type As enum_Component_Type = enum_Component_Type._UNKNOWN
        Public Destination_Index As Integer 'Index to local or global variable
        'Public Destination_Pointer As Object  'Pointer to memory location of destination symbol
        Public RPN_Stack As New List(Of class_Component)
    End Class

    Public Class class_Program_Line
        Public Type As enum_Component_Type = enum_Component_Type._UNKNOWN
        Public Line_Number As Integer = -1
        Public Level As Integer = 0

        Public Raw_Text As String = ""
        Public Statement As String = ""
        Public Comment As String = ""
        Public Error_Messages As New List(Of String)
        Public Help_Messages As New List(Of enum_Error_Help_Messages)

        'Pointers to statement first line, last line and IF statement ELSE line
        Public Begin_Line_Pointer As class_Program_Line = Nothing
        Public End_Line_Pointer As class_Program_Line = Nothing
        Public Else_Line_Pointer As class_Program_Line = Nothing

        'Pointer when conditional in statement is true
        Public True_Line_Number As Integer = Nothing

        'Pointer when conditional in statement is false
        Public False_Line_Number As Integer = Nothing

        'Next executible statement after end of statement
        Public Next_Line_Number As Integer = Nothing

        Public Components As New List(Of class_Component)

        Public Expressions As New List(Of class_Expression)

        Public Expression_Stack As New List(Of class_Component)
        Public FOR_Expression_Stack As New List(Of class_Component)

    End Class

    Public Class class_Macro
        Public Name As String
        Public Index As Integer

        Public Start_Found As Boolean = False
        Public End_Found As Boolean = False

        Public Error_Messages As New List(Of String)
        Public Has_Errors As Boolean = False

        Public Local_Variable_Names As New List(Of String)
        Public Local_Variables As New List(Of class_Variable) 'Also includes line Labels

        'Public Local_List_Names As List(Of String)
        'Public Local_Lists As New List(Of class_List)
        Public Labels As New List(Of class_Label)

        Public Program As New List(Of class_Program_Line)
        Public Program_Counter As Integer

        Public Run_Command As enum_Run_Commands
        Public Halt As Boolean = False

        '        Public Highest_Level As Integer

        Public Call_Stack As New Stack

        Public Sub Clear_Program()
            Program.Clear()
            Local_Variables.Clear()
        End Sub

        Public Sub Delete_Line(Line_No As Integer)
            Program.RemoveAt(Line_No)
        End Sub

        Public Sub Insert_Line(Line_No As Integer, Optional Text As String = "")
            Dim Line As New class_Program_Line
            If Text <> "" Then
                Line.Raw_Text = Text
            Else
                Text = ""
            End If
            Program.Insert(Line_No, Line)
        End Sub

        Public Sub Move_Line_Up(Index As Integer)
            Dim Line_1 As class_Program_Line
            Dim Line_2 As class_Program_Line
            Dim Ln_1 As Integer
            Dim Ln_2 As Integer

            Line_1 = G_Code.Macro.Program(Index - 1)
            Line_2 = G_Code.Macro.Program(Index)
            Ln_1 = Line_1.Line_Number
            Ln_2 = Line_2.Line_Number
            Ln_1 = Line_2.Line_Number
            Ln_2 = Line_1.Line_Number

            G_Code.Macro.Program(Index - 1) = Line_2
            G_Code.Macro.Program(Index) = Line_1

        End Sub

        Public Sub Move_Line_Down(Index As Integer)
            Dim Line_1 As class_Program_Line
            Dim Line_2 As class_Program_Line
            Dim Ln_1 As Integer
            Dim Ln_2 As Integer

            Line_1 = G_Code.Macro.Program(Index + 1)
            Line_2 = G_Code.Macro.Program(Index)
            Ln_1 = Line_1.Line_Number
            Ln_2 = Line_2.Line_Number
            Ln_1 = Line_2.Line_Number
            Ln_2 = Line_1.Line_Number

            G_Code.Macro.Program(Index + 1) = Line_2
            G_Code.Macro.Program(Index) = Line_1

        End Sub

    End Class

#End Region

#Region "Definitions"

    Public Dump_Reports As Boolean = False

    Private Const Not_Equal As String = Chr(1)
    Private Const Greater_Than_Or_Equal As String = Chr(2)
    Private Const Less_Than_Or_Equal As String = Chr(3)
    Private Help_Box As TextBox

    Private Separators() As String = {"~", " ", "(", ")", """", "=", "@", "+", "-", "*", "/", "^", "<", ">", Not_Equal, Greater_Than_Or_Equal, Less_Than_Or_Equal}
    Public Macro As New class_Macro

#End Region

#Region "Load_G_Code, Save_G_Code"

    Public Sub Load_G_Code_From_String(Text As String)
        If IsNothing(Text) Then Exit Sub

        Dim S() As String
        S = Text.Split("|")
        Dim Cnt As Integer = 0

        Macro.Program.Clear()

        If Text = "" Then Exit Sub

        For I = 0 To S.Count - 1
            Dim Program_Line As New class_Program_Line
            Program_Line.Raw_Text = Trim(S(I))
            Program_Line.Line_Number = Cnt
            Macro.Program.Add(Program_Line)
            Cnt += 1
        Next

    End Sub

    Public Sub Put_G_Code_Into_String(ByRef Text As String)
        Text = ""
        For I = 0 To Macro.Program.Count - 1
            If I = 0 Then
                Text &= Macro.Program(I).Raw_Text
            Else
                Text &= "|" & Macro.Program(I).Raw_Text
            End If
        Next

    End Sub

    Public Function Load_G_Code(File_Name As String) As Boolean
        Dim Df As New Class_Data_File
        Dim Line As String = ""
        Dim Cnt As Integer = 0

        If Not File.Exists(File_Name) Then
            Message_Box.ShowDialog("File does not exist" & vbCrLf & File_Name)
            Return False
        End If

        Macro.Program.Clear()

        'Read macro file into macro line list
        Df.Open_Input_File(File_Name)
        While Df.Read_Line(Line)
            Dim Program_Line As New class_Program_Line
            Program_Line.Raw_Text = Trim(Line)
            Program_Line.Line_Number = Cnt
            Macro.Program.Add(Program_Line)
            Cnt += 1
        End While
        Df.Close_Input_File()

        Return True

    End Function

    Public Sub Save_G_Code(File_Name As String)
        Dim Df As New Class_Data_File

        Df.Open_Output_File(File_Name)

        For I = 0 To Macro.Program.Count - 1
            Df.Write_Line(Macro.Program(I).Raw_Text)
        Next

        Df.Close_Output_File()

    End Sub

#End Region

#Region "Load_Grid, Highlight_Grid"

    Public Sub Load_Grid(Grid As DataGridView, Optional ByRef Help_Text_Box As TextBox = Nothing, Optional Refresh As Boolean = False)
        Dim S(3) As String
        Help_Box = Help_Text_Box

        If Not Refresh Then Grid.Rows.Clear()

        For L = 0 To Macro.Program.Count - 1

            S(0) = Trim(Str(Macro.Program(L).Line_Number))
            If Macro.Program(L).Type = enum_Component_Type._COMMENT Then
                S(1) = Macro.Program(L).Comment
                S(2) = ""
            Else
                S(1) = Trim(Macro.Program(L).Statement)
                S(2) = Macro.Program(L).Comment
            End If

            If Macro.Program(L).Error_Messages.Count > 0 Then
                S(3) = Macro.Program(L).Error_Messages(0)
            Else
                S(3) = ""
            End If

            If Refresh Then
                For C = 0 To Grid.Columns.Count - 1
                    Grid.Rows(L).Cells(C).Style.BackColor = Color.White
                Next

                For I = 0 To S.Count - 1
                    Grid.Rows(L).Cells(I).Value = S(I)
                Next
            Else
                Grid.Rows.Add(S)
            End If

            If Macro.Program(L).Level >= 0 Then
                If Macro.Program(L).Type = enum_Component_Type._KEYWORD_LABEL Then
                    Grid.Rows(L).Cells(enum_Grid_Columns.Block).Value = Grid.Rows(L).Cells(enum_Grid_Columns.Block).Value
                Else
                    Grid.Rows(L).Cells(enum_Grid_Columns.Block).Value = Space(Macro.Program(L).Level * 2) & Trim(Grid.Rows(L).Cells(enum_Grid_Columns.Block).Value)
                End If
            End If

        Next

        Highlight_Grid(Grid)

    End Sub

    Public Sub Highlight_Grid(ByRef Grid As DataGridView)

        If IsNothing(Grid.CurrentRow) Then
            If Not IsNothing(Help_Box) Then
                Help_Box.Text = ""
            End If
            Exit Sub
        End If

        Dim Level As Integer = -10
        Dim Type As enum_Component_Type

        Dim Row As class_Program_Line = G_Code.Macro.Program(Grid.CurrentRow.Index)
        Dim Start_Row As class_Program_Line = Row.Begin_Line_Pointer
        Dim End_Row As class_Program_Line = Start_Row.End_Line_Pointer

        'Display help
        If Not IsNothing(Help_Box) Then
            Help_Box.Text = G_Code.Get_Help(Grid.CurrentRow.Index)
        End If

        'Get level and type of current statement
        Level = Start_Row.Level
        If Level < 0 Then Level = -10
        Type = Start_Row.Type


        'Highlight Start and End of multi-line statements, eg. IF, FOR etc.
        For R = 0 To Grid.Rows.Count - 1
            Grid.Rows(R).Cells(enum_Grid_Columns.Line).Style.BackColor = Color.White

            If Macro.Program(R).Statement = "" And Macro.Program(R).Comment <> "" Then
                Grid.Rows(R).Cells(enum_Grid_Columns.Block).Style.BackColor = Color.OldLace
            Else
                If Macro.Program(R).Comment <> "" Then
                    Grid.Rows(R).Cells(enum_Grid_Columns.Comment).Style.BackColor = Color.OldLace
                End If
            End If

            If Not IsNothing(Start_Row) Then
                If Not IsNothing(End_Row) Then
                    If R >= Start_Row.Line_Number And R <= End_Row.Line_Number Then
                        If (G_Code.Macro.Program(R).Level = Level) Then
                            Grid.Rows(R).Cells(enum_Grid_Columns.Line).Style.BackColor = Color.PaleGoldenrod
                        Else
                            Grid.Rows(R).Cells(enum_Grid_Columns.Line).Style.BackColor = Color.AntiqueWhite
                        End If
                    End If
                End If
            End If

            If Macro.Program(R).Error_Messages.Count > 0 Then
                Grid.Rows(R).Cells(enum_Grid_Columns.Block).Style.BackColor = Color.MistyRose
            End If

        Next

    End Sub

#End Region

#Region "Parsing routines"

    Private Shared Function Is_Uniary(Component As class_Component) As Boolean

        'FLOAT X = -12.34  'IF Previous is '=' and next numeric then constant
        If Component.Type = enum_Component_Type._MATH_EQUAL Then
            Return True
        End If

        'X = (I + J) * -(I + K)   'IF Previous an operator then uniary
        'x = +-K
        If Component.Category = enum_Component_Type._MATH Then
            Return True
        End If

        'If Not (SQRT(K) < -2) Then 'If previous a logical then constant
        'If X = -2 Then          'If previouls = 
        If Component.Category = enum_Component_Type._LOGICAL Then
            Return True
        End If

        'J = J + 1 + (-J)   'IF previous ( then uniary
        If Component.Type = enum_Component_Type._SEPARATOR_OPEN_PAREN Then
            Return True
        End If

        Return False

    End Function

    Public Function Get_Local_Variable(Name As String) As Object

        If Macro.Local_Variable_Names.Count = 0 Then Return Nothing

        Dim Idx As Integer = Macro.Local_Variable_Names.IndexOf(Name)

        If Idx >= 0 Then
            Return Macro.Local_Variables(Idx)
        End If

        Return Nothing

    End Function

    Public Function Get_Global_Variable(Name As String) As Object
        'Dim Idx As Integer = Symbols_OLD.Index_Of(Name)
        'If Idx < 0 Then Return Nothing
        'Return Symbols_OLD.Symbol_Table(Idx)


        Return Nothing
    End Function

    Public Sub Parse_Types()
        Dim Comp As class_Component = Nothing
        Dim Line As class_Program_Line
        Dim T As String
        Dim S() As String
        Dim Words As New List(Of String)
        Dim Add_Negate_Function As Boolean = False
        Dim String_Constants As New List(Of String)
        Dim LS As String
        Dim Constant As Boolean
        Dim String_Const_Count As Integer = 0
        Dim Statement As String
        Dim Variable As Object

        Macro.Has_Errors = False


        Macro.Local_Variable_Names.Clear()
        Macro.Local_Variables.Clear()

        For L = 0 To Macro.Program.Count - 1
            Macro.Program(L).Line_Number = L

            Line = Macro.Program(L)

            Line.Expressions.Clear()
            Line.Components.Clear()
            Line.Error_Messages.Clear()
            Line.Help_Messages.Clear()
            Line.Type = enum_Component_Type._UNKNOWN

            T = Trim(Line.Raw_Text)

            'Separate out comments
            If Mid(Trim(T), 1, 1) = "'" Then 'Whole line comment
                Line.Type = enum_Component_Type._COMMENT
                Line.Comment = Line.Raw_Text
                GoTo Skip
            End If

            If Trim(T) = "" Then 'Blank line
                Line.Type = enum_Component_Type._COMMENT
                Line.Comment = ""
                GoTo Skip
            End If

            If Line.Type <> enum_Component_Type._UNKNOWN Then GoTo Skip

            'Line with trailing comment
            S = T.Split("'")
            T = Trim(S(0))

            If S.Count > 1 Then
                Line.Comment = "'" & Trim(S(1))
                Line.Statement = Trim(Line.Raw_Text.Replace(Line.Comment, ""))
            Else
                Line.Statement = Trim(Line.Raw_Text)
            End If

            Statement = Line.Statement

            If Statement.Contains("""") Then 'Contains String constants
                LS = ""
                T = ""
                Constant = False

                For I = 0 To Statement.Length - 1

                    If Constant Then
                        T &= Statement(I)
                        If Statement(I) = """" Then
                            String_Constants.Add(T)
                            LS &= "`"
                            T = ""
                            Constant = False
                        End If
                    Else
                        If Statement(I) = """" Then
                            T = """"
                            Constant = True
                        Else
                            LS &= Statement(I)
                        End If
                    End If

                Next

                If Constant Then 'Missmatched quotes
                    Add_Error_Message(Line, "Missing quote in string constant")
                    Add_Help_Message(Line, enum_Error_Help_Messages.String_Constant_Help)
                End If
                Statement = LS
            End If


            Statement = Statement.Replace(" ", "~")
            Statement = Statement.Replace("<>", "~" & Not_Equal & "~")
            Statement = Statement.Replace(">=", "~" & Greater_Than_Or_Equal & "~")
            Statement = Statement.Replace("<=", "~" & Less_Than_Or_Equal & "~")
            Statement = Statement.Replace("(", "~(~")
            Statement = Statement.Replace(")", "~)~")
            Statement = Statement.Replace("=", "~=~")
            Statement = Statement.Replace("+", "~+~")
            Statement = Statement.Replace("-", "~-~")
            Statement = Statement.Replace("*", "~*~")
            Statement = Statement.Replace("/", "~/~")
            Statement = Statement.Replace("^", "~^~")
            Statement = Statement.Replace(">", "~>~")
            Statement = Statement.Replace("<", "~<~")
            Statement = Statement.Replace("`", "~`~")
            Statement = Statement.Replace(":", "~:~")
            Statement = Statement.Replace(",", "~,~")

            Statement = UCase(Statement)

            While Statement.Contains("~~")
                Statement = Statement.Replace("~~", "~")
            End While
            S = Statement.Split("~")
            Words.Clear()

            For I = 0 To S.Count - 1
                If S(I) <> "" Then
                    Words.Add(S(I))
                End If
            Next

            'Create component list
            For W = 0 To Words.Count - 1
                Add_Negate_Function = False

                Comp = New class_Component
                Comp.Category = enum_Component_Type._UNKNOWN
                Comp.Type = enum_Component_Type._UNKNOWN
                Comp.Token = Words(W)

                Select Case UCase(Words(W))

                    Case "`"
                        If Words(W) <> "" Then
                            Comp.Category = enum_Component_Type._CONSTANT
                            Comp.Type = enum_Component_Type._CONSTANT_STRING
                            Comp.Token = String_Constants(String_Const_Count)
                            String_Const_Count += 1
                        End If

                    Case "("
                        Comp.Category = enum_Component_Type._SEPARATOR
                        Comp.Type = enum_Component_Type._SEPARATOR_OPEN_PAREN

                        If Line.Components.Count > 1 Then
                            If Line.Components(Line.Components.Count - 1).Type = enum_Component_Type._MATH_SUBTRACT Then
                                If Is_Uniary(Line.Components(Line.Components.Count - 1)) Then 'Uniary - found, replace with Negate function
                                    Line.Components.RemoveAt(Line.Components.Count - 1)
                                    Add_Negate_Function = True
                                End If

                            End If
                        End If

                    Case ")"
                        Comp.Category = enum_Component_Type._SEPARATOR
                        Comp.Type = enum_Component_Type._SEPARATOR_CLOSE_PAREN

                    Case "="
                        Comp.Category = enum_Component_Type._MATH
                        Comp.Type = enum_Component_Type._MATH_EQUAL

                    Case "+"
                        Comp.Category = enum_Component_Type._MATH
                        Comp.Type = enum_Component_Type._MATH_ADD

                    Case "-"
                        Comp.Category = enum_Component_Type._MATH
                        Comp.Type = enum_Component_Type._MATH_SUBTRACT

                    Case "*"
                        Comp.Category = enum_Component_Type._MATH
                        Comp.Type = enum_Component_Type._MATH_MULTIPLY

                    Case "/"
                        Comp.Category = enum_Component_Type._MATH
                        Comp.Type = enum_Component_Type._MATH_DIVIDE

                    Case "^"
                        Comp.Category = enum_Component_Type._MATH
                        Comp.Type = enum_Component_Type._MATH_POWER

                    Case "MOD"
                        Comp.Category = enum_Component_Type._MATH
                        Comp.Type = enum_Component_Type._MATH_MODULUS

                    Case ">"
                        Comp.Category = enum_Component_Type._LOGICAL
                        Comp.Type = enum_Component_Type._LOGICAL_GT

                    Case "<"
                        Comp.Category = enum_Component_Type._LOGICAL
                        Comp.Type = enum_Component_Type._LOGICAL_LT

                    Case Not_Equal
                        Comp.Category = enum_Component_Type._LOGICAL
                        Comp.Type = enum_Component_Type._LOGICAL_NOT_EQ

                    Case Greater_Than_Or_Equal
                        Comp.Category = enum_Component_Type._LOGICAL
                        Comp.Type = enum_Component_Type._LOGICAL_GE

                    Case Less_Than_Or_Equal
                        Comp.Category = enum_Component_Type._LOGICAL
                        Comp.Type = enum_Component_Type._LOGICAL_LE

                    Case "CR"
                        Comp.Category = enum_Component_Type._CONSTANT
                        Comp.Type = enum_Component_Type._CONSTANT_CR
                        Comp.Function_Parameters.Add(vbCr)
                    Case "LF"
                        Comp.Category = enum_Component_Type._CONSTANT
                        Comp.Type = enum_Component_Type._CONSTANT_LF
                        Comp.Function_Parameters.Add(vbLf)

                    Case "CRLF"
                        Comp.Category = enum_Component_Type._CONSTANT
                        Comp.Type = enum_Component_Type._CONSTANT_CRLF
                        Comp.Function_Parameters.Add(vbCrLf)

                    Case "AND"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._LOGICAL_AND

                    Case "OR"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._LOGICAL_OR

                    Case "NOT"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._LOGICAL_NOT

                    Case "DEG"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_DEG

                    Case "RAD"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_RAD

                    Case "SIN"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_SIN

                    Case "COS"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_COS

                    Case "TAN"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_TAN

                    Case "ASIN"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_ASIN

                    Case "ACOS"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_ACOS

                    Case "ATAN"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_ATAN

                    Case "SINH"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_SINH

                    Case "COSH"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_COSH

                    Case "TANH"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_TANH

                    Case "SQRT"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_SQRT

                    Case "ABS"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_ABS

                    Case "LOG"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_LOG

                    Case "LOG10"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_LOG10

                    Case "ROUND"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_ROUND

                    Case "CEILING"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_CEILING

                    Case "FLOOR"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_FLOOR

                    Case "TO_FLOAT"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_TO_FLOAT

                    Case "TO_INTEGER"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_TO_INTEGER

                    Case "FORMAT"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_FORMAT_NUMBER
                        Comp.Function_Parameters = New List(Of Object)

                        If Words.Count < 7 Then
                            Add_Error_Message(Line, "FORMAT number missing parameter")
                            Add_Help_Message(Line, enum_Error_Help_Messages.String_Functions_Help)
                        Else
                            If IsNumeric(Words(6)) Then
                                Comp.Function_Parameters.Add(Words(6))
                            Else
                                Variable = Get_Local_Variable(Words(6))
                                If Not IsNothing(Variable) Then
                                    Comp.Function_Parameters.Add(Words(6))
                                Else
                                    Variable = Get_Global_Variable(Words(6))
                                    If Not IsNothing(Variable) Then
                                        Comp.Function_Parameters.Add(Words(6))
                                    Else
                                        Add_Error_Message(Line, "FORMAT number not a constant, local or global variable")
                                        Add_Help_Message(Line, enum_Error_Help_Messages.String_Functions_Help)
                                    End If
                                End If
                            End If

                        End If

                    Case "LEN"
                        Comp.Category = enum_Component_Type._FUNCTION
                        Comp.Type = enum_Component_Type._FUNCTION_LEN

                    Case Else

                        If IsNumeric(Words(W)) Then
                            Comp.Category = enum_Component_Type._CONSTANT
                            Comp.Type = enum_Component_Type._CONSTANT_NUMBER

                            'Handle uniary - sign
                            If Line.Components(Line.Components.Count - 1).Type = enum_Component_Type._MATH_SUBTRACT Then
                                If Is_Uniary(Line.Components(Line.Components.Count - 2)) Then
                                    Line.Components.RemoveAt(Line.Components.Count - 1) 'Remove component with - sign
                                    Comp.Token = "-" & Comp.Token 'Add negated token
                                End If
                            End If

                        Else

                            'Handle uniary - sign
                            If Line.Components.Count > 1 Then
                                If Line.Components(Line.Components.Count - 1).Type = enum_Component_Type._MATH_SUBTRACT Then
                                    If Is_Uniary(Line.Components(Line.Components.Count - 2)) Then 'Uniary - found, replace with Negate function
                                        Line.Components.RemoveAt(Line.Components.Count - 1)
                                        Add_Negate_Function = True
                                    End If

                                End If

                            End If

                            Variable = Get_Local_Variable(Words(W))

                            If Not IsNothing(Variable) Then
                                Comp.Category = enum_Component_Type._LOCAL
                                Comp.Type = Variable.Type
                                Comp.Variable_Index = Macro.Local_Variable_Names.IndexOf(Comp.Token)
                                'Else
                                '    Add_Error_Message(Line, "Unknown Symbol")
                            End If

                        End If 'IsNumeric(Words(W)) 

                End Select


                If Words(W) <> "" Then
                    If Add_Negate_Function Then
                        Dim Fun As New class_Component
                        Fun.Token = "NEG"
                        Fun.Category = enum_Component_Type._FUNCTION
                        Fun.Type = enum_Component_Type._FUNCTION_NEG
                        Line.Components.Add(Fun)
                    End If

                    Line.Components.Add(Comp)
                End If

            Next

            Select Case Line.Components(0).Token

                Case "MACRO"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_MACRO
                    Line.Type = Line.Components(0).Type

                    If Line.Components.Count > 1 Then
                        Line.Components(1).Category = enum_Component_Type._KEYWORD
                        Line.Components(1).Type = enum_Component_Type._MACRO_NAME
                    End If

                Case "MACRO_END"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_MACRO_END
                    Line.Type = Line.Components(0).Type

                Case "WHOLE"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_WHOLE
                    Line.Type = Line.Components(0).Type

                    If Line.Components.Count > 1 Then
                        Line.Components(1).Category = enum_Component_Type._LOCAL
                        Line.Components(1).Type = enum_Component_Type._KEYWORD_WHOLE

                        'Check for unique name
                        Variable = Get_Local_Variable(Line.Components(1).Token)
                        If IsNothing(Variable) Then
                            Dim Var As New class_Variable
                            Var.Name = Line.Components(1).Token
                            Var.Type = enum_Component_Type._LOCAL_WHOLE

                            If Line.Components.Count > 2 Then
                                If IsNumeric(Line.Components(3).Token) Then
                                    Var.Value = CInt(Line.Components(3).Token)
                                End If
                            End If

                            Macro.Local_Variable_Names.Add(Var.Name)
                            Macro.Local_Variables.Add(Var)
                        Else
                            Add_Error_Message(Line, "Local WHOLE variable '" & Line.Components(1).Token & "' already defined")
                            Add_Help_Message(Line, enum_Error_Help_Messages.Local_WHOLE_Variable_Help)
                        End If

                    End If

                Case "REAL"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_REAL
                    Line.Type = Line.Components(0).Type

                    If Line.Components.Count > 1 Then
                        Line.Components(1).Category = enum_Component_Type._LOCAL
                        Line.Components(1).Type = enum_Component_Type._KEYWORD_REAL

                        'Check for unique name           
                        Variable = Get_Local_Variable(Line.Components(1).Token)
                        If IsNothing(Variable) Then
                            Dim Var As New class_Variable
                            Var.Name = Line.Components(1).Token
                            Var.Type = enum_Component_Type._LOCAL_REAL

                            If Line.Components.Count > 2 Then
                                If IsNumeric(Line.Components(3).Token) Then
                                    Var.Value = CSng(Line.Components(3).Token)
                                End If
                            End If

                            Macro.Local_Variable_Names.Add(Var.Name)
                            Macro.Local_Variables.Add(Var)
                        Else
                            Add_Error_Message(Line, "Local FLOAT variable '" & Line.Components(1).Token & "' already defined")
                            Add_Help_Message(Line, enum_Error_Help_Messages.Local_FLOAT_Variable_Help)
                        End If
                    End If

                Case "STRING"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_STRING
                    Line.Type = Line.Components(0).Type

                    If Line.Components.Count > 1 Then
                        Line.Components(1).Category = enum_Component_Type._LOCAL
                        Line.Components(1).Type = enum_Component_Type._KEYWORD_STRING


                        'Check for unique name
                        Variable = Get_Local_Variable(Line.Components(1).Token)
                        If IsNothing(Variable) Then
                            Dim Var As New class_Variable
                            Var.Name = Line.Components(1).Token
                            Var.Type = enum_Component_Type._LOCAL_STRING

                            If Line.Components.Count > 2 Then
                                Var.Value = Line.Components(3).Token
                            End If

                            Macro.Local_Variable_Names.Add(Var.Name)
                            Macro.Local_Variables.Add(Var)
                        Else
                            Add_Error_Message(Line, "Local STRING variable '" & Line.Components(1).Token & "' already defined")
                            Add_Help_Message(Line, enum_Error_Help_Messages.Local_STRING_Variable_Help)
                        End If
                    End If

                Case "WHOLE[]"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_WHOLE_LIST
                    Line.Type = Line.Components(0).Type

                    If Line.Components.Count > 1 Then
                        Line.Components(1).Category = enum_Component_Type._LOCAL
                        Line.Components(1).Type = enum_Component_Type._LOCAL_WHOLE_LIST

                        'Check for unique name
                        Variable = Get_Local_Variable(Line.Components(1).Token)
                        If IsNothing(Get_Local_Variable(Line.Components(1).Token)) Then
                            Dim Var As New class_Variable
                            Var.Name = Line.Components(1).Token
                            Var.Type = enum_Component_Type._LOCAL_WHOLE_LIST
                            Var.Value = New List(Of Integer)
                            Macro.Local_Variable_Names.Add(Var.Name)
                            Macro.Local_Variables.Add(Var)
                        Else
                            Add_Error_Message(Line, "Local INTEGER[] variable '" & Line.Components(1).Token & "' already defined")
                            Add_Help_Message(Line, enum_Error_Help_Messages.Local_WHOLE_LIST_Variable_Help)
                        End If

                    End If

                Case "REAL[]"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_REAL_LIST
                    Line.Type = Line.Components(0).Type

                    If Line.Components.Count > 1 Then
                        Line.Components(1).Category = enum_Component_Type._LOCAL
                        Line.Components(1).Type = enum_Component_Type._LOCAL_REAL_LIST

                        'Check for unique name
                        Variable = Get_Local_Variable(Line.Components(1).Token)
                        If IsNothing(Get_Local_Variable(Line.Components(1).Token)) Then
                            Dim Var As New class_Variable
                            Var.Name = Line.Components(1).Token
                            Var.Type = enum_Component_Type._LOCAL_REAL_LIST
                            Var.Value = New List(Of Single)
                            Macro.Local_Variable_Names.Add(Var.Name)
                            Macro.Local_Variables.Add(Var)
                        Else
                            Add_Error_Message(Line, "Local FLOAT[] variable '" & Line.Components(1).Token & "' already defined")
                            Add_Help_Message(Line, enum_Error_Help_Messages.Local_REAL_LIST_Variable_Help)
                        End If

                    End If


                Case "STRING[]"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_STRING_LIST
                    Line.Type = Line.Components(0).Type

                    If Line.Components.Count > 1 Then
                        Line.Components(1).Category = enum_Component_Type._LOCAL
                        Line.Components(1).Type = enum_Component_Type._LOCAL_STRING_LIST

                        'Check for unique name
                        Variable = Get_Local_Variable(Line.Components(1).Token)
                        If IsNothing(Get_Local_Variable(Line.Components(1).Token)) Then
                            Dim Var As New class_Variable
                            Var.Name = Line.Components(1).Token
                            Var.Type = enum_Component_Type._LOCAL_STRING_LIST
                            Var.Value = New List(Of String)
                            Macro.Local_Variable_Names.Add(Var.Name)
                            Macro.Local_Variables.Add(Var)
                        Else
                            Add_Error_Message(Line, "Local STRING[] variable '" & Line.Components(1).Token & "' already defined")
                            Add_Help_Message(Line, enum_Error_Help_Messages.Local_STRING_LIST_Variable_Help)
                        End If

                    End If

                Case ":" 'Label
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_LABEL
                    Line.Type = Line.Components(0).Type

                    If Line.Components.Count < 2 Then
                        Add_Error_Message(Line, "Label name missing")
                        Add_Help_Message(Line, enum_Error_Help_Messages.LABEL_Help)
                    End If

                    If Line.Components.Count > 1 Then
                        For I = 0 To Macro.Labels.Count - 1
                            If Macro.Labels(I).Name = Line.Components(1).Token Then
                                Add_Error_Message(Line, "Label already defined")
                                Add_Help_Message(Line, enum_Error_Help_Messages.LABEL_Help)
                                Exit For
                            End If
                        Next
                    Else
                        Dim lab As New class_Label
                        lab.Name = Line.Components(1).Token
                        lab.Program_Line = Line
                        Macro.Labels.Add(lab)
                    End If


                Case "IF"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_IF
                    Line.Type = Line.Components(0).Type

                Case "ELSE"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_ELSE
                    Line.Type = Line.Components(0).Type

                Case "IF_END"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_IF_END
                    Line.Type = Line.Components(0).Type

                Case "FOR"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_FOR
                    Line.Type = Line.Components(0).Type

                Case "NEXT"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_NEXT
                    Line.Type = Line.Components(0).Type

                Case "WHILE"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_WHILE
                    Line.Type = Line.Components(0).Type

                Case "WHILE_END"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_WHILE_END
                    Line.Type = Line.Components(0).Type

                Case "DO"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_DO
                    Line.Type = Line.Components(0).Type

                Case "UNTIL"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_UNTIL
                    Line.Type = Line.Components(0).Type

                Case "GOTO"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_GOTO
                    Line.Type = Line.Components(0).Type

                Case "CALL"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_CALL
                    Line.Type = Line.Components(0).Type

                Case "BREAKPOINT"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_BREAKPOINT
                    Line.Type = Line.Components(0).Type

                Case "CYCLE"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_CYCLE
                    Line.Type = Line.Components(0).Type

                Case "SPINDLE"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_SPINDLE
                    Line.Type = Line.Components(0).Type

                Case "COOLANT"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_COOLANT
                    Line.Type = Line.Components(0).Type

                Case "FEED_OVERRIDE"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_FEED_OVERRIDE
                    Line.Type = Line.Components(0).Type

                Case "PAUSE"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_PAUSE
                    Line.Type = Line.Components(0).Type

                Case "MESSAGE"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_MESSAGE
                    Line.Type = Line.Components(0).Type

                Case "BROWSE"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_BROWSE
                    Line.Type = Line.Components(0).Type

                Case "READ"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_READ
                    Line.Type = Line.Components(0).Type

                Case "WRITE"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_WRITE
                    Line.Type = Line.Components(0).Type

                Case "G_CODE"
                    Line.Components(0).Category = enum_Component_Type._KEYWORD
                    Line.Components(0).Type = enum_Component_Type._KEYWORD_G_CODE
                    Line.Type = Line.Components(0).Type

                Case Else
                    If (Line.Components(0).Category = enum_Component_Type._LOCAL) Or _
                       (Line.Components(0).Category = enum_Component_Type._GLOBAL) Then
                        Line.Type = enum_Component_Type._EXPRESSION
                    End If

            End Select

            'These types have logical statements, replace Math_Equal with Logical_Eq
            If (Line.Components(0).Type = enum_Component_Type._KEYWORD_IF) Or _
                (Line.Components(0).Type = enum_Component_Type._KEYWORD_WHILE) Or _
                (Line.Components(0).Type = enum_Component_Type._KEYWORD_UNTIL) Or _
                (Line.Components(0).Type = enum_Component_Type._KEYWORD_FOR) Then
                For I = 1 To Line.Components.Count - 1
                    If Line.Components(I).Type = enum_Component_Type._MATH_EQUAL Then
                        Line.Components(I).Type = enum_Component_Type._LOGICAL_EQ
                    End If
                Next
            End If
Skip:

        Next

    End Sub

    Public Sub Check_For_Errors()
        Dim Line As class_Program_Line
        Dim Found As Boolean
        Dim Open_Paren_Count As Integer
        Dim Close_Paren_Count As Integer
        Dim Quote_Count As Integer

        For L = 0 To Macro.Program.Count - 1
            Line = Macro.Program(L)

            Open_Paren_Count = 0
            Close_Paren_Count = 0
            Quote_Count = 0

            For I = 0 To Line.Statement.Length - 1
                Select Case Line.Statement(I)
                    Case "("
                        Open_Paren_Count += 1
                    Case ")"
                        Close_Paren_Count += 1
                    Case """"
                        Quote_Count += 1
                End Select
            Next

            If Open_Paren_Count <> Close_Paren_Count Then
                Add_Error_Message(Line, "Unbalanced parens in expression")
                Add_Help_Message(Line, enum_Error_Help_Messages.Logical_EXPRESSION_Help)
                Add_Help_Message(Line, enum_Error_Help_Messages.Math_EXPRESSION_Help)
            End If

            If Quote_Count Mod 2 <> 0 Then
                Add_Error_Message(Line, "Unbalanced quotes in string variable")
                Add_Help_Message(Line, enum_Error_Help_Messages.Symbol_Help)
            End If

            Select Case Line.Type
                Case enum_Component_Type._COMMENT

                Case enum_Component_Type._KEYWORD_MACRO
                    Macro.Start_Found = True

                    If Line.Components.Count < 2 Then
                        Add_Error_Message(Line, "Macro name missing or incorrect format")
                        Add_Help_Message(Line, enum_Error_Help_Messages.Macro_Help)
                    Else
                        'Check for unique name
                        Macro.Name = Line.Components(1).Token
                        'If Symbols_OLD.Exists(Macro.Name) Then
                        '    Add_Error_Message(Line, "Macro already defined in symbol table")
                        '    Add_Help_Message(Line, enum_Error_Help_Messages.Macro_Help)
                        'End If
                    End If

                Case enum_Component_Type._KEYWORD_MACRO_END
                    Macro.End_Found = True

                    If Line.Components.Count <> 1 Then
                        Add_Error_Message(Line, "END_MACRO statement format error")
                        Add_Help_Message(Line, enum_Error_Help_Messages.Macro_Help)
                    End If

                Case enum_Component_Type._KEYWORD_WHOLE

                    If Line.Components.Count < 2 Then
                        Add_Error_Message(Line, "Local INTEGER variable name missing or incorrect format")
                        Add_Help_Message(Line, enum_Error_Help_Messages.Local_WHOLE_Variable_Help)
                    Else
                        If Line.Components(2).Token <> "=" Then
                            Add_Error_Message(Line, "Local INTEGER statement missing '='")
                            Add_Help_Message(Line, enum_Error_Help_Messages.Local_WHOLE_Variable_Help)
                        End If

                        If Line.Components.Count < 4 Then
                            Add_Error_Message(Line, "Initializetion value missing")
                            Add_Help_Message(Line, enum_Error_Help_Messages.Local_WHOLE_Variable_Help)
                        End If

                        If Not IsNumeric(Line.Components(3).Token) Then
                            Add_Error_Message(Line, "Initializetion value must be a constant eg. 12")
                            Add_Help_Message(Line, enum_Error_Help_Messages.Local_WHOLE_Variable_Help)
                        End If
                    End If

                Case enum_Component_Type._KEYWORD_REAL

                    If Line.Components.Count < 2 Then
                        Add_Error_Message(Line, "Local FLOAT variable name missing or incorrect format")
                        Add_Help_Message(Line, enum_Error_Help_Messages.Local_FLOAT_Variable_Help)
                    Else
                        If Line.Components(2).Token <> "=" Then
                            Add_Error_Message(Line, "Local FLOAT statement missing '='")
                            Add_Help_Message(Line, enum_Error_Help_Messages.Local_WHOLE_Variable_Help)
                        End If

                        If Line.Components.Count < 4 Then
                            Add_Error_Message(Line, "Initializetion value missing")
                            Add_Help_Message(Line, enum_Error_Help_Messages.Local_WHOLE_Variable_Help)
                        End If

                        If Not IsNumeric(Line.Components(3).Token) Then
                            Add_Error_Message(Line, "Initializetion value must be a constant eg. 12.3")
                            Add_Help_Message(Line, enum_Error_Help_Messages.Local_WHOLE_Variable_Help)
                        End If
                    End If

                Case enum_Component_Type._KEYWORD_STRING

                    If Line.Components.Count < 2 Then
                        Add_Error_Message(Line, "Local STRING variable name missing or incorrect format")
                        Add_Help_Message(Line, enum_Error_Help_Messages.Local_WHOLE_Variable_Help)
                    Else
                        If Line.Components(2).Token <> "=" Then
                            Add_Error_Message(Line, "Local STRING statement missing '='")
                            Add_Help_Message(Line, enum_Error_Help_Messages.Local_WHOLE_Variable_Help)
                        End If

                        If Line.Components.Count < 4 Then
                            Add_Error_Message(Line, "Initializetion value missing")
                            Add_Help_Message(Line, enum_Error_Help_Messages.Local_WHOLE_Variable_Help)
                        End If

                        If (Mid(Line.Components(3).Token, 1, 1) <> """") Or (Mid(Line.Components(3).Token, Line.Components(3).Token.Length, 1) <> """") Then
                            Add_Error_Message(Line, "Initializetion value must be a string constant eg. ""Test"" ")
                            Add_Help_Message(Line, enum_Error_Help_Messages.Local_WHOLE_Variable_Help)
                        End If
                    End If

                Case enum_Component_Type._KEYWORD_WHOLE_LIST
                    If Line.Components.Count < 2 Then
                        Add_Error_Message(Line, "Local INTEGER[] variable name missing or incorrect format")
                        Add_Help_Message(Line, enum_Error_Help_Messages.Local_WHOLE_Variable_Help)
                    End If

                Case enum_Component_Type._KEYWORD_REAL_LIST
                    If Line.Components.Count < 2 Then
                        Add_Error_Message(Line, "Local FLOAT[] variable name missing or incorrect format")
                        Add_Help_Message(Line, enum_Error_Help_Messages.Local_WHOLE_Variable_Help)
                    End If

                Case enum_Component_Type._KEYWORD_STRING_LIST
                    If Line.Components.Count < 2 Then
                        Add_Error_Message(Line, "Local STRING[] variable name missing or incorrect format")
                        Add_Help_Message(Line, enum_Error_Help_Messages.Local_WHOLE_Variable_Help)
                    End If

                Case enum_Component_Type._KEYWORD_LABEL
                    Dim lab As New class_Label

                Case enum_Component_Type._KEYWORD_IF

                    If Line.Components(Line.Components.Count - 1).Token <> "THEN" Then
                        Add_Error_Message(Line, "THEN missing in IF statement")
                        Add_Help_Message(Line, enum_Error_Help_Messages.IF_Help)
                    Else
                        If Line.Components.Count < 3 Then
                            Add_Error_Message(Line, "IF statement has no conditional expression")
                            Add_Help_Message(Line, enum_Error_Help_Messages.IF_Help)
                        End If
                    End If

                Case enum_Component_Type._KEYWORD_ELSE
                    If Line.Components.Count > 1 Then
                        Add_Error_Message(Line, "IF_END statement format error")
                        Add_Help_Message(Line, enum_Error_Help_Messages.IF_Help)
                    End If

                Case enum_Component_Type._KEYWORD_IF_END

                    If Line.Components.Count > 1 Then
                        Add_Error_Message(Line, "IF_END statement format error")
                        Add_Help_Message(Line, enum_Error_Help_Messages.IF_Help)
                    End If

                Case enum_Component_Type._KEYWORD_FOR

                    If Line.Components.Count < 2 Then
                        Add_Error_Message(Line, "FOR statement format error")
                        Add_Help_Message(Line, enum_Error_Help_Messages.FOR_Help)
                    Else
                        Found = False
                        For I = 0 To Line.Components.Count - 1
                            If Line.Components(I).Token = "TO" Then
                                Found = True
                            End If
                        Next

                        If Not Found Then
                            Add_Error_Message(Line, "Missing TO in FOR statement")
                            Add_Help_Message(Line, enum_Error_Help_Messages.FOR_Help)
                        End If

                        If Line.Components.Count < 4 Then
                            Add_Error_Message(Line, "FOR missing conditional expression")
                            Add_Help_Message(Line, enum_Error_Help_Messages.FOR_Help)
                        End If

                    End If

                Case enum_Component_Type._KEYWORD_NEXT

                    If Line.Components.Count > 1 Then
                        Add_Error_Message(Line, "NEXT statement format error")
                        Add_Help_Message(Line, enum_Error_Help_Messages.FOR_Help)
                    End If


                Case enum_Component_Type._KEYWORD_WHILE

                    If Line.Components.Count < 2 Then
                        Add_Error_Message(Line, "WHILE missing conditional expression")
                        Add_Help_Message(Line, enum_Error_Help_Messages.WHILE_Help)
                    End If

                Case enum_Component_Type._KEYWORD_WHILE_END

                    If Line.Components.Count > 1 Then
                        Add_Error_Message(Line, "WHILE_END statement format error")
                        Add_Help_Message(Line, enum_Error_Help_Messages.WHILE_Help)
                    End If

                Case enum_Component_Type._KEYWORD_DO

                    If Line.Components.Count > 1 Then
                        Add_Error_Message(Line, "DO statement format error")
                        Add_Help_Message(Line, enum_Error_Help_Messages.DO_Help)
                    End If

                Case enum_Component_Type._KEYWORD_UNTIL

                    If Line.Components.Count < 2 Then
                        Add_Error_Message(Line, "UNTIL missing conditional expression")
                        Add_Help_Message(Line, enum_Error_Help_Messages.DO_Help)
                    End If

                Case enum_Component_Type._KEYWORD_GOTO

                    If Line.Components.Count < 2 Then
                        Add_Error_Message(Line, "GOTO missing destination label")
                        Add_Help_Message(Line, enum_Error_Help_Messages.WHILE_Help)
                    End If
                    If Line.Components.Count > 2 Then
                        Add_Error_Message(Line, "GOTO format error")
                        Add_Help_Message(Line, enum_Error_Help_Messages.WHILE_Help)
                    End If

                Case enum_Component_Type._KEYWORD_CALL

                Case enum_Component_Type._KEYWORD_BREAKPOINT

                    If Line.Components.Count > 1 Then
                        Add_Error_Message(Line, "BREAKPOINT format error")
                        Add_Help_Message(Line, enum_Error_Help_Messages.LABEL_Help)
                    End If

                Case enum_Component_Type._KEYWORD_CYCLE

                Case enum_Component_Type._KEYWORD_SPINDLE

                Case enum_Component_Type._KEYWORD_COOLANT

                Case enum_Component_Type._KEYWORD_FEED_OVERRIDE

                Case enum_Component_Type._KEYWORD_PAUSE

                Case enum_Component_Type._KEYWORD_MESSAGE

                Case enum_Component_Type._KEYWORD_BROWSE

                Case enum_Component_Type._KEYWORD_READ

                Case enum_Component_Type._KEYWORD_WRITE

                Case enum_Component_Type._KEYWORD_G_CODE

                Case enum_Component_Type._EXPRESSION

                    If Line.Components.Count < 3 Then
                        Add_Error_Message(Line, "Expression format error, missing operator, source or destination")
                        Add_Help_Message(Line, enum_Error_Help_Messages.Math_EXPRESSION_Help)
                        Add_Help_Message(Line, enum_Error_Help_Messages.Logical_EXPRESSION_Help)
                    Else
                        If Line.Components(1).Token <> "=" Then
                            Add_Error_Message(Line, "Expression format error, missing = sign")
                            Add_Help_Message(Line, enum_Error_Help_Messages.Math_EXPRESSION_Help)
                        End If
                    End If


                    For I = 0 To Line.Components.Count - 1
                        If Line.Components(I).Type = enum_Component_Type._UNKNOWN Then
                            Add_Error_Message(Line, "Unknown symbol " & Line.Components(I).Token & "")
                            Add_Help_Message(Line, enum_Error_Help_Messages.Symbol_Help)
                        End If
                    Next

                Case Else

                    For I = 0 To Line.Components.Count - 1
                        If Line.Components(I).Type = enum_Component_Type._UNKNOWN Then
                            Add_Error_Message(Line, "Unknown symbol " & Line.Components(I).Token & "")
                            Add_Help_Message(Line, enum_Error_Help_Messages.Symbol_Help)
                        End If
                    Next

            End Select

        Next

        If Not Macro.Start_Found Then
            Macro.Error_Messages.Add("MACRO Statement missing")
            Macro.Has_Errors = True
        End If

        If Not Macro.End_Found Then
            Macro.Error_Messages.Add("MACRO_END Statement missing")
            Macro.Has_Errors = True
        End If

    End Sub

#End Region

#Region "Compile routines"

    Public Function Compile_Macro(Optional ByRef Error_Box As ListBox = Nothing)
        If Not IsNothing(Error_Box) Then
            Error_Box.Items.Clear()
        End If

        Parse_Types()
        Check_For_Errors()
        Find_Statement_Levels()

        Program_Dump()

        If Not Macro.Has_Errors Then
            Create_Expression_Stacks()

            If Dump_Reports Then Stack_Dump()

            Find_Next_Executable_Statements()
            If Dump_Reports Then Sequence_Dump()
        End If

        If IsNothing(Error_Box) Then
            If Macro.Has_Errors Then
                Return False
            Else
                Return True
            End If
        End If

        Error_Box.Text = ""

        'Format lines for display
        For M = 0 To Macro.Error_Messages.Count - 1
            Error_Box.Items.Add("****:" & Macro.Error_Messages(M))
        Next

        For L = 0 To Macro.Program.Count - 1
            If Macro.Program(L).Error_Messages.Count > 0 Then
                For M = 0 To Macro.Program(L).Error_Messages.Count - 1
                    Error_Box.Items.Add(Trim(Str(L)).PadLeft(4) & ":" & Macro.Program(L).Statement)
                    Error_Box.Items.Add(Trim(Str(L)).PadLeft(4) & ":" & "   ->" & Macro.Program(L).Error_Messages(M))
                Next
                Error_Box.Items.Add("")
            End If
        Next

        Error_Box.Refresh()

        If Macro.Has_Errors Then
            Return False
        Else
            Return True
        End If

        Return True

    End Function

    Public Function Find_Next(Line_Number As Integer) As class_Program_Line
        Dim Stat As class_Program_Line = Macro.Program(Line_Number)


        If Line_Number > Macro.Program.Count - 2 Then
            Return Macro.Program(Line_Number)
        End If

        Line_Number += 1

        While Macro.Program(Line_Number).Type = enum_Component_Type._COMMENT Or _
            Macro.Program(Line_Number).Type = enum_Component_Type._KEYWORD_MACRO

            Line_Number += 1

            If Line_Number = Macro.Program.Count - 1 Then
                Return Macro.Program(Line_Number)
            End If

        End While

        Return Macro.Program(Line_Number)

    End Function

    Public Sub Find_Next_Executable_Statements()
        Dim Statement As class_Program_Line

        For L = 0 To Macro.Program.Count - 1

            Statement = Find_Next(L)
            Macro.Program(L).Next_Line_Number = Statement.Line_Number

            Select Case Macro.Program(L).Type

                Case enum_Component_Type._KEYWORD_MACRO, enum_Component_Type._COMMENT, _
                     enum_Component_Type._KEYWORD_WHOLE, enum_Component_Type._KEYWORD_REAL, _
                     enum_Component_Type._KEYWORD_STRING, enum_Component_Type._KEYWORD_WHOLE_LIST, _
                     enum_Component_Type._KEYWORD_REAL_LIST, enum_Component_Type._KEYWORD_STRING_LIST

                Case enum_Component_Type._KEYWORD_IF
                    Macro.Program(L).True_Line_Number = Statement.Line_Number

                Case enum_Component_Type._KEYWORD_ELSE
                    Macro.Program(L).Begin_Line_Pointer.False_Line_Number = Statement.Line_Number 'Set false pointer in IF statement

                Case enum_Component_Type._KEYWORD_IF_END
                    Macro.Program(L).Begin_Line_Pointer.Next_Line_Number = Statement.Line_Number 'Set next line pointer in IF statement
                    If IsNothing(Macro.Program(L).Begin_Line_Pointer.Else_Line_Pointer) Then 'No else found
                        Macro.Program(L).Begin_Line_Pointer.False_Line_Number = Statement.Line_Number 'Set false line pointer in IF statement
                    End If

                Case enum_Component_Type._KEYWORD_FOR
                    'If FOR conditional is true go to statement after FOR statement
                    Macro.Program(L).True_Line_Number = Statement.Line_Number

                Case enum_Component_Type._KEYWORD_NEXT
                    'If FOR conditional is false go to statement after NEXT statement
                    Macro.Program(L).Begin_Line_Pointer.False_Line_Number = Statement.Line_Number
                    'Set next line pointer in FOR statement
                    Macro.Program(L).Begin_Line_Pointer.Next_Line_Number = Statement.Line_Number

                Case enum_Component_Type._KEYWORD_WHILE
                    'If WHILE conditional is true go to statement after WHILE statement
                    Macro.Program(L).True_Line_Number = Statement.Line_Number

                Case enum_Component_Type._KEYWORD_WHILE_END
                    'If WHILE contitional is false, go to next statement after WHILE_END
                    Macro.Program(L).Begin_Line_Pointer.False_Line_Number = Statement.Line_Number
                    'Set next line pointer in WHILE statement
                    Macro.Program(L).Begin_Line_Pointer.Next_Line_Number = Statement.Line_Number

                Case enum_Component_Type._KEYWORD_DO

                Case enum_Component_Type._KEYWORD_UNTIL
                    'If UNTIL contitional is true, go to next statement after DO statement
                    Macro.Program(L).True_Line_Number = Macro.Program(L).Begin_Line_Pointer.Next_Line_Number
                    'If UNTIL contitional is false, go to next statement after UNTIL statement
                    Macro.Program(L).False_Line_Number = Statement.Line_Number

            End Select

        Next

    End Sub

    Public Sub Find_Statement_Levels()
        Dim Level As Integer = 0
        Dim Found As Boolean = False
        Dim Stk As New Stack
        Dim Statement As class_Program_Line = Nothing

        For L = 0 To Macro.Program.Count - 1

            Macro.Program(L).Begin_Line_Pointer = Macro.Program(L)

            Select Case Macro.Program(L).Type

                Case enum_Component_Type._COMMENT
                    Macro.Program(L).Type = enum_Component_Type._COMMENT
                    Macro.Program(L).Level = -1

                Case enum_Component_Type._KEYWORD_WHOLE, enum_Component_Type._KEYWORD_REAL, enum_Component_Type._KEYWORD_STRING, _
                     enum_Component_Type._KEYWORD_WHOLE_LIST, enum_Component_Type._KEYWORD_REAL_LIST, enum_Component_Type._KEYWORD_STRING_LIST
                    Macro.Program(L).Level = Level

                Case enum_Component_Type._KEYWORD_MACRO
                    Macro.Program(L).Level = Level
                    Stk.Push(Macro.Program(L))
                    Level += 1

                Case enum_Component_Type._KEYWORD_MACRO_END
                    Macro.Program(L).Level = Level - 1

                    For I = 0 To Stk.Count - 1
                        Statement = Stk(I)
                        Select Case Statement.Type
                            Case enum_Component_Type._KEYWORD_IF
                                Add_Error_Message(Statement, "IF statement missing IF_END")
                                Add_Help_Message(Statement, enum_Error_Help_Messages.IF_Help)
                            Case enum_Component_Type._KEYWORD_FOR
                                Add_Error_Message(Statement, "FOR statement missing NEXT")
                                Add_Help_Message(Statement, enum_Error_Help_Messages.FOR_Help)
                            Case enum_Component_Type._KEYWORD_WHILE
                                Add_Error_Message(Statement, "WHILE statement missing WHILE_END")
                                Add_Help_Message(Statement, enum_Error_Help_Messages.WHILE_Help)
                            Case enum_Component_Type._KEYWORD_DO
                                Add_Error_Message(Statement, "DO statement missing UNTIL")
                                Add_Help_Message(Statement, enum_Error_Help_Messages.DO_Help)
                            Case Else
                        End Select
                    Next

                    Pop_Stack(Stk, Level - 1, Macro.Program(L), enum_Component_Type._KEYWORD_MACRO)
                    Level -= 1

                Case enum_Component_Type._KEYWORD_IF
                    Macro.Program(L).Level = Level
                    Stk.Push(Macro.Program(L))
                    Level += 1

                Case enum_Component_Type._KEYWORD_ELSE
                    Macro.Program(L).Level = Level - 1
                    Statement = Peek_Stack(Stk)
                    Macro.Program(L).Begin_Line_Pointer = Statement 'Set pointer to starting IF statement

                    If IsNothing(Statement) Or (Statement.Type <> enum_Component_Type._KEYWORD_IF) Then
                        Add_Error_Message(Macro.Program(L), "ELSE unexpectedly encountered")
                        Add_Help_Message(Macro.Program(L), enum_Error_Help_Messages.IF_Help)
                        Add_Help_Message(Macro.Program(L), enum_Error_Help_Messages.Unpaired_Help)
                    Else
                        Statement.Else_Line_Pointer = Macro.Program(L)
                    End If

                Case enum_Component_Type._KEYWORD_IF_END
                    Found = True
                    Macro.Program(L).Level = Level - 1
                    Statement = Pop_Stack(Stk, Level - 1, Macro.Program(L), enum_Component_Type._KEYWORD_IF)

                    If IsNothing(Statement) Then
                        Found = False
                    Else
                        If Statement.Type <> enum_Component_Type._KEYWORD_IF Then
                            Found = False
                        End If
                    End If

                    If Found Then
                        Macro.Program(L).Begin_Line_Pointer = Statement 'Set pointer to starting IF statement
                        Statement.End_Line_Pointer = Macro.Program(L) 'Set end pointer in starting IF statement
                        If Not IsNothing(Statement.Else_Line_Pointer) Then
                            If Not IsNothing(Statement.Else_Line_Pointer.End_Line_Pointer) Then
                                Statement.Else_Line_Pointer.End_Line_Pointer = Macro.Program(L) 'Set end pointer in ELSE statement
                            End If
                        End If
                    Else
                        Add_Error_Message(Macro.Program(L), "IF_END unexpectedly encountered")
                        Add_Help_Message(Macro.Program(L), enum_Error_Help_Messages.IF_Help)
                        Add_Help_Message(Macro.Program(L), enum_Error_Help_Messages.Unpaired_Help)
                    End If

                    Level -= 1

                Case enum_Component_Type._KEYWORD_FOR
                    Macro.Program(L).Level = Level
                    Stk.Push(Macro.Program(L))
                    Level += 1

                Case enum_Component_Type._KEYWORD_NEXT
                    Macro.Program(L).Level = Level - 1
                    Statement = Pop_Stack(Stk, Level - 1, Macro.Program(L), enum_Component_Type._KEYWORD_FOR)

                    If IsNothing(Statement) Or (Statement.Type <> enum_Component_Type._KEYWORD_FOR) Then
                        Add_Error_Message(Macro.Program(L), "NEXT unexpectedly encountered")
                        Add_Help_Message(Macro.Program(L), enum_Error_Help_Messages.FOR_Help)
                        Add_Help_Message(Macro.Program(L), enum_Error_Help_Messages.Unpaired_Help)
                    Else
                        Macro.Program(L).Begin_Line_Pointer = Statement
                        Statement.End_Line_Pointer = Macro.Program(L)
                    End If

                    Level -= 1

                Case enum_Component_Type._KEYWORD_WHILE
                    Macro.Program(L).Level = Level
                    Stk.Push(Macro.Program(L))
                    Level += 1

                Case enum_Component_Type._KEYWORD_WHILE_END
                    Macro.Program(L).Level = Level - 1
                    Statement = Pop_Stack(Stk, Level - 1, Macro.Program(L), enum_Component_Type._KEYWORD_WHILE)

                    If IsNothing(Statement) Or (Statement.Type <> enum_Component_Type._KEYWORD_WHILE) Then
                        Add_Error_Message(Macro.Program(L), "WHILE_END unexpectedly encountered")
                        Add_Help_Message(Macro.Program(L), enum_Error_Help_Messages.WHILE_Help)
                        Add_Help_Message(Macro.Program(L), enum_Error_Help_Messages.Unpaired_Help)
                    Else
                        Macro.Program(L).Begin_Line_Pointer = Statement
                        Statement.End_Line_Pointer = Macro.Program(L)
                    End If

                    Level -= 1

                Case enum_Component_Type._KEYWORD_DO
                    Macro.Program(L).Level = Level
                    Macro.Program(L).Begin_Line_Pointer = Macro.Program(L)
                    Stk.Push(Macro.Program(L))
                    Level += 1

                Case enum_Component_Type._KEYWORD_UNTIL
                    Macro.Program(L).Level = Level - 1
                    Statement = Pop_Stack(Stk, Level - 1, Macro.Program(L), enum_Component_Type._KEYWORD_DO)

                    If IsNothing(Statement) Or (Statement.Type <> enum_Component_Type._KEYWORD_DO) Then
                        Add_Error_Message(Macro.Program(L), "UNTIL unexpectedly encountered")
                        Add_Help_Message(Macro.Program(L), enum_Error_Help_Messages.DO_Help)
                        Add_Help_Message(Macro.Program(L), enum_Error_Help_Messages.Unpaired_Help)
                    Else
                        Macro.Program(L).Begin_Line_Pointer = Statement
                        Statement.End_Line_Pointer = Macro.Program(L)
                    End If

                    Level -= 1

                Case enum_Component_Type._KEYWORD_LABEL
                    Macro.Program(L).Level = Level

                Case enum_Component_Type._KEYWORD_GOTO
                    Macro.Program(L).Level = Level

                Case enum_Component_Type._KEYWORD_CALL
                    Macro.Program(L).Level = Level

                Case enum_Component_Type._KEYWORD_BREAKPOINT
                    Macro.Program(L).Level = Level

                Case enum_Component_Type._EXPRESSION
                    Macro.Program(L).Level = Level

                Case enum_Component_Type._KEYWORD_CYCLE
                    Macro.Program(L).Level = Level

                Case enum_Component_Type._KEYWORD_SPINDLE
                    Macro.Program(L).Level = Level

                Case enum_Component_Type._KEYWORD_FEED_OVERRIDE
                    Macro.Program(L).Level = Level

                Case enum_Component_Type._KEYWORD_MESSAGE
                    Macro.Program(L).Level = Level

                Case enum_Component_Type._KEYWORD_BROWSE
                    Macro.Program(L).Level = Level

                Case enum_Component_Type._KEYWORD_READ
                    Macro.Program(L).Level = Level

                Case enum_Component_Type._KEYWORD_WRITE
                    Macro.Program(L).Level = Level

            End Select

        Next

    End Sub

    Public Function Check_Types(N1 As Object, n2 As Object)
        If Object.ReferenceEquals(N1.GetType, n2.GetType) Then
            Return True
        Else
            Return False
        End If
    End Function

    Private Function Get_G_Code_Block(Expression As String) As String
        Dim S() As String = Expression.Split(" ")
        If (Mid(Expression, 1, 1) = "G" Or Mid(Expression, 1, 1) = "M") And IsNumeric(Mid(Expression, 2, 1)) Then
            Return Expression
        Else
            Return ""
        End If
    End Function

    Public Function Peek_Stack(Stk As Stack) As class_Program_Line
        Dim Stat As class_Program_Line = Nothing

        If Stk.Count > 0 Then
            Stat = Stk.Peek
        End If

        Return Stat

    End Function

    Public Function Pop_Stack(Stk As Stack, Level As Integer, Statement As class_Program_Line, Begin_Type As enum_Component_Type) As class_Program_Line
        Dim Stat As class_Program_Line = Nothing
        Dim Error_Found As Boolean = False

        If Stk.Count > 0 Then
            Stat = Stk.Pop
            If (Level < 0) Or (Stat.Level <> Level) Or (Stat.Type <> Begin_Type) Then
                Error_Found = True
            End If
        Else
            Error_Found = True
        End If

        If Error_Found Then
            Statement.Error_Messages.Add(Statement.Components(0).Token & " statement missing or out of place")
            Statement.Help_Messages.Add(enum_Error_Help_Messages.Unpaired_Help)
        End If

        Return Stat

    End Function

#End Region

#Region "Reverse Polish Notation Stacks"

    Public Sub Create_Expression_Stacks()
        Dim First As Integer = -1
        Dim Count As Integer = -1

        For L = 0 To Macro.Program.Count - 1

            Select Case Macro.Program(L).Type

                Case enum_Component_Type._KEYWORD_WHOLE, enum_Component_Type._KEYWORD_REAL, enum_Component_Type._KEYWORD_STRING
                    Dim Exp As New class_Expression
                    Macro.Program(L).Expressions.Add(Exp)
                    Exp.Destination_Symbol = Macro.Program(L).Components(1).Token
                    Exp.Destination_Type = Macro.Program(L).Components(1).Type
                    Exp.Destination_Index = Macro.Local_Variable_Names.IndexOf(Exp.Destination_Symbol)

                    First = 3
                    Count = Macro.Program(L).Components.Count
                    Create_RPN_Stack(Macro.Program(L).Components, Exp.RPN_Stack, First, Count)

                Case enum_Component_Type._KEYWORD_IF, enum_Component_Type._KEYWORD_WHILE, enum_Component_Type._KEYWORD_UNTIL
                    Dim Exp As New class_Expression
                    Macro.Program(L).Expressions.Add(Exp)
                    Exp.Destination_Symbol = Macro.Program(L).Components(0).Token
                    Exp.Destination_Type = Macro.Program(L).Components(0).Type
                    First = 1
                    Count = Macro.Program(L).Components.Count - 1
                    Create_RPN_Stack(Macro.Program(L).Components, Exp.RPN_Stack, First, Count)

                Case enum_Component_Type._KEYWORD_FOR
                    Dim FOR_Exp As New class_Expression
                    Macro.Program(L).Expressions.Add(FOR_Exp)
                    FOR_Exp.Destination_Symbol = Macro.Program(L).Components(0).Token
                    FOR_Exp.Destination_Type = Macro.Program(L).Components(0).Type
                    First = 1
                    For I = 1 To Macro.Program(L).Components.Count - 1
                        If Macro.Program(L).Components(I).Type = enum_Component_Type._KEYWORD_TO Then
                            Count = I - 1
                            Exit For
                        End If
                    Next
                    Create_RPN_Stack(Macro.Program(L).Components, FOR_Exp.RPN_Stack, First, Count)

                    Dim TO_Exp As New class_Expression
                    Macro.Program(L).Expressions.Add(TO_Exp)
                    TO_Exp.Destination_Symbol = Macro.Program(L).Components(Count + 1).Token
                    TO_Exp.Destination_Type = Macro.Program(L).Components(Count + 1).Type
                    First = Count + 2
                    Count = Macro.Program(L).Components.Count - 1
                    Create_RPN_Stack(Macro.Program(L).Components, TO_Exp.RPN_Stack, First, Count)

                Case enum_Component_Type._EXPRESSION
                    Dim Exp As New class_Expression
                    Macro.Program(L).Expressions.Add(Exp)
                    Exp.Destination_Symbol = Macro.Program(L).Components(0).Token
                    Exp.Destination_Type = Macro.Program(L).Components(0).Type
                    Exp.Destination_Index = Macro.Local_Variable_Names.IndexOf(Exp.Destination_Symbol)

                    First = 2
                    Count = Macro.Program(L).Components.Count
                    Create_RPN_Stack(Macro.Program(L).Components, Exp.RPN_Stack, First, Count)

            End Select

        Next


    End Sub

    Public Function Create_RPN_Stack(ByRef Comps As List(Of class_Component), ByRef RPN As List(Of class_Component), First As Integer, Count As Integer) As String
        Dim Comp As class_Component = Nothing
        Dim Peek_Comp As class_Component = Nothing
        Dim Pop_Comp As class_Component = Nothing
        Dim Pop_Queue As class_Component = Nothing
        Dim op1 As class_Component = Nothing
        Dim op2 As class_Component = Nothing
        Dim Comp_Stack As New Stack
        Dim Queue As New Stack
        Dim Done As Boolean = False

        Dim Q As String = ""
        Dim C As String = ""

        'Loop through components
        For I = First To Count - 1

            Comp = Comps(I)

            Select Case Comp.Category

                Case enum_Component_Type._SEPARATOR

                    Select Case Comp.Type
                        Case enum_Component_Type._SEPARATOR_OPEN_PAREN
                            Comp_Stack.Push(Comp)

                        Case enum_Component_Type._SEPARATOR_CLOSE_PAREN
                            If Comp_Stack.Count > 0 Then
                                Peek_Comp = Comp_Stack.Peek
                                While Peek_Comp.Type <> enum_Component_Type._SEPARATOR_OPEN_PAREN
                                    Pop_Comp = Comp_Stack.Pop
                                    Queue.Push(Pop_Comp)
                                    Peek_Comp = Comp_Stack.Peek
                                End While
                                Pop_Comp = Comp_Stack.Pop() 'discard "("
                            End If
                    End Select

                Case enum_Component_Type._CONSTANT
                    Queue.Push(Comp)

                Case enum_Component_Type._LOCAL, enum_Component_Type._GLOBAL
                    Queue.Push(Comp)

                Case enum_Component_Type._MATH, enum_Component_Type._LOGICAL, enum_Component_Type._FUNCTION
                    op1 = Comp

                    Done = False
                    Do While Not Done

                        If Comp_Stack.Count > 0 Then
                            Peek_Comp = Comp_Stack.Peek

                            If Peek_Comp.Category = enum_Component_Type._MATH Or _
                                Peek_Comp.Category = enum_Component_Type._LOGICAL Or _
                                Peek_Comp.Category = enum_Component_Type._FUNCTION Then

                                op2 = Comp_Stack.Peek

                                If (op2.Type <> enum_Component_Type._MATH_POWER) And (Precedence(op1) = Precedence(op2)) Then
                                    '"^" is the only right-associative operator
                                    Pop_Comp = Comp_Stack.Pop
                                    Queue.Push(Pop_Comp)
                                ElseIf Precedence(op1) < Precedence(op2) Then
                                    Pop_Comp = Comp_Stack.Pop
                                    Queue.Push(Pop_Comp)
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

                    Comp_Stack.Push(op1)

                Case Else

            End Select

            Q = ""
            For J = 0 To Queue.Count - 1
                Q &= Queue(J).Token & " "
            Next

            C = ""
            For J = 0 To Comp_Stack.Count - 1
                C &= Comp_Stack(J).Token & " "
            Next

        Next 'I

        'Dim Q As String = ""
        'For I = 0 To Queue.Count - 1
        '    Pop_Queue = Queue(I)
        '    Q &= Pop_Queue.Token
        'Next

        'Dim C As String = ""
        'For I = 0 To Comp_Stack.Count - 1
        '    Pop_Comp = Comp_Stack(I)
        '    C &= Pop_Comp.Token
        'Next


        While Queue.Count > 0
            Pop_Queue = Queue.Pop
            Comp_Stack.Push(Pop_Queue)
        End While

        Dim S As String = ""
        While Comp_Stack.Count > 0
            Pop_Comp = Comp_Stack.Pop
            S &= Pop_Comp.Token & " "
            RPN.Add(Pop_Comp)
        End While

        'Expression : ((3+4)*2)/(((1-5)^2)^3)
        'Input      : 3 + 4 * 2 / ( 1 - 5 ) ^ 2 ^ 3
        'Output     : 3 4 2 * 1 5 - 2 3 ^ ^ / +

        Return S

    End Function

    Public Sub Evaluate_Expression(Expression As class_Expression)
        Dim Comp_Stack As New Stack
        Dim Comp As class_Component = Nothing
        Dim Op_1 As Object = Nothing
        Dim Op_2 As Object = Nothing
        Dim Value As Object = Nothing

        For I = 0 To Expression.RPN_Stack.Count - 1
            Value = Nothing

            Select Case Expression.RPN_Stack(I).Category

                Case enum_Component_Type._MATH, enum_Component_Type._LOGICAL
                    Op_1 = Comp_Stack.Pop
                    Op_2 = Comp_Stack.Pop

                Case enum_Component_Type._FUNCTION
                    Op_1 = Comp_Stack.Pop

            End Select


            Select Case Expression.RPN_Stack(I).Type

                Case enum_Component_Type._LOCAL_WHOLE, enum_Component_Type._LOCAL_REAL, enum_Component_Type._LOCAL_STRING
                    Value = Macro.Local_Variables(Expression.RPN_Stack(I).Variable_Index).Value

                Case enum_Component_Type._CONSTANT_NUMBER
                    Value = CSng(Expression.RPN_Stack(I).Token)

                Case enum_Component_Type._CONSTANT_STRING

                    Select Case Expression.Destination_Type

                        Case enum_Component_Type._KEYWORD_STRING, enum_Component_Type._LOCAL_STRING
                            Value = Expression.RPN_Stack(I).Token.ToString

                    End Select

                Case enum_Component_Type._MATH_ADD
                    Value = Op_1 + Op_2
                Case enum_Component_Type._MATH_SUBTRACT
                    Value = Value = Op_1 - Op_2
                Case enum_Component_Type._MATH_MULTIPLY
                    Value = Op_1 * Op_2
                Case enum_Component_Type._MATH_DIVIDE
                    Value = Op_1 / Op_2
                Case enum_Component_Type._MATH_POWER
                    Value = Op_2 ^ Op_1
                Case enum_Component_Type._MATH_MODULUS
                    Value = Op_1 Mod Op_2
                Case enum_Component_Type._LOGICAL_EQ
                    Value = Op_1 = Op_2
                Case enum_Component_Type._LOGICAL_NOT_EQ
                    Value = Op_1 <> Op_2
                Case enum_Component_Type._LOGICAL_GT
                    Value = Op_1 > Op_2
                Case enum_Component_Type._LOGICAL_LT
                    Value = Op_1 < Op_2
                Case enum_Component_Type._LOGICAL_GE
                    Value = Op_1 >= Op_2
                Case enum_Component_Type._LOGICAL_LE
                    Value = Op_1 <= Op_2
                Case enum_Component_Type._LOGICAL_NOT
                    Value = Not Op_1
                Case enum_Component_Type._LOGICAL_AND
                    Value = Op_1 And Op_2
                Case enum_Component_Type._LOGICAL_OR
                    Value = Op_1 Or Op_2

                Case enum_Component_Type._FUNCTION_NEG
                    Value = -Op_1
                Case enum_Component_Type._FUNCTION_DEG
                    Value = (180 * Op_1 / Math.PI)
                Case enum_Component_Type._FUNCTION_RAD
                    Value = (Math.PI * Op_1 / 180)
                Case enum_Component_Type._FUNCTION_SIN
                    Value = Math.Sin(Op_1)
                Case enum_Component_Type._FUNCTION_COS
                    Value = Math.Cos(Op_1)
                Case enum_Component_Type._FUNCTION_TAN
                    Value = Math.Tan(Op_1)
                Case enum_Component_Type._FUNCTION_ASIN
                    Value = Math.Asin(Op_1)
                Case enum_Component_Type._FUNCTION_ACOS
                    Value = Math.Acos(Op_1)
                Case enum_Component_Type._FUNCTION_ATAN
                    Value = Math.Atan(Op_1)
                Case enum_Component_Type._FUNCTION_SINH
                    Value = Math.Sinh(Op_1)
                Case enum_Component_Type._FUNCTION_COSH
                    Value = Math.Cosh(Op_1)
                Case enum_Component_Type._FUNCTION_TANH
                    Value = Math.Tanh(Op_1)
                Case enum_Component_Type._FUNCTION_SQRT
                    Value = Math.Sqrt(Op_1)
                Case enum_Component_Type._FUNCTION_ABS
                    Value = Math.Abs(Op_1)
                Case enum_Component_Type._FUNCTION_LOG
                    Value = Math.Log(Op_1)
                Case enum_Component_Type._FUNCTION_LOG10
                    Value = Math.Log10(Op_1)
                Case enum_Component_Type._FUNCTION_ROUND
                    Value = Calc.Round(Op_1)
                Case enum_Component_Type._FUNCTION_CEILING
                    Value = Math.Ceiling(Op_1)
                Case enum_Component_Type._FUNCTION_FLOOR
                    Value = Math.Floor(Op_1)
                Case enum_Component_Type._FUNCTION_NEGATE
                    Value = -Op_1

                    'String Functions
                Case enum_Component_Type._FUNCTION_TO_FLOAT
                    Value = CSng(Op_1)
                Case enum_Component_Type._FUNCTION_TO_INTEGER
                    Value = CInt(Op_1)
                Case enum_Component_Type._FUNCTION_FORMAT_NUMBER
                    Value = FormatNumber(Op_1, Expression.RPN_Stack(I).Function_Parameters(0))
            End Select

            If Not IsNothing(Value) Then
                Comp_Stack.Push(Value)
            End If
        Next

        Select Case Expression.Destination_Type
            Case enum_Component_Type._KEYWORD_IF
                Expression.Destination_Index = Value

            Case enum_Component_Type._LOCAL_WHOLE, enum_Component_Type._KEYWORD_WHOLE
                Macro.Local_Variables(Expression.Destination_Index).Value = CInt(Value)

            Case enum_Component_Type._LOCAL_REAL, enum_Component_Type._KEYWORD_REAL
                Macro.Local_Variables(Expression.Destination_Index).Value = CSng(Value)

            Case enum_Component_Type._LOCAL_STRING, enum_Component_Type._KEYWORD_STRING
                If IsNothing(Value) Then
                    Macro.Local_Variables(Expression.Destination_Index).Value = ""
                Else
                    Macro.Local_Variables(Expression.Destination_Index).Value = Value.ToString
                End If

        End Select

    End Sub

    Function Precedence(op As class_Component) As Integer

        If op.Category = enum_Component_Type._FUNCTION Then
            Return 4
        End If

        Select Case op.Type
            Case enum_Component_Type._MATH_ADD, enum_Component_Type._MATH_SUBTRACT
                Return 1
            Case enum_Component_Type._MATH_MULTIPLY, enum_Component_Type._MATH_DIVIDE
                Return 2
            Case enum_Component_Type._MATH_POWER, enum_Component_Type._MATH_FUNCTION
                Return 3
        End Select

        Return 0

    End Function

#End Region

#Region "Execution routines"

    Public Function Execute_Macro(Command As enum_Run_Commands, Optional Line_Number As Integer = -1) As enum_Run_Status

        Macro.Run_Command = Command

        Select Case Command
            Case enum_Run_Commands.Initialize

            Case enum_Run_Commands.Check

            Case enum_Run_Commands.Rewind

            Case enum_Run_Commands.Start_At
                If Line_Number >= 0 Then
                    Macro.Program_Counter = Line_Number
                Else
                    Line_Number = 0
                End If


                Return enum_Run_Status.OK

            Case enum_Run_Commands.Continuous
                While True
                    Application.DoEvents()
                    If Macro.Halt Then
                        Return enum_Run_Status.Halted
                    End If
                    Execute_Macro_Statement()
                End While

            Case enum_Run_Commands.Single_Step
                Execute_Macro_Statement()


        End Select

        Return enum_Run_Status.OK

    End Function

    Public Function Execute_Macro_Statement() As enum_Run_Status

        Select Case Macro.Program(Macro.Program_Counter).Type

            Case enum_Component_Type._KEYWORD_WHOLE, enum_Component_Type._KEYWORD_REAL, enum_Component_Type._KEYWORD_STRING
                Evaluate_Expression(Macro.Program(Macro.Program_Counter).Expressions(0))
                Macro.Program_Counter = Macro.Program(Macro.Program_Counter).Next_Line_Number

            Case enum_Component_Type._EXPRESSION
                Evaluate_Expression(Macro.Program(Macro.Program_Counter).Expressions(0))
                Macro.Program_Counter = Macro.Program(Macro.Program_Counter).Next_Line_Number

            Case enum_Component_Type._KEYWORD_IF
                Evaluate_Expression(Macro.Program(Macro.Program_Counter).Expressions(0))
                If Macro.Program(Macro.Program_Counter).Expressions(0).Destination_Index Then
                    Macro.Program_Counter = Macro.Program(Macro.Program_Counter).True_Line_Number
                Else
                    Macro.Program_Counter = Macro.Program(Macro.Program_Counter).False_Line_Number
                End If

            Case enum_Component_Type._KEYWORD_FOR
                Return enum_Run_Status.OK

            Case enum_Component_Type._KEYWORD_WHILE
                Return enum_Run_Status.OK

            Case enum_Component_Type._KEYWORD_UNTIL
                Return enum_Run_Status.OK

            Case enum_Component_Type._KEYWORD_GOTO
                Return enum_Run_Status.OK

            Case enum_Component_Type._KEYWORD_MACRO_END
                Return enum_Run_Status.End_Of_Program

            Case Else
                Macro.Program_Counter = Macro.Program(Macro.Program_Counter).Next_Line_Number
        End Select


        Return enum_Run_Status.Error_Encountered

    End Function

#End Region

#Region "Error and help"

    Public Sub Add_Error_Message(Statement As class_Program_Line, Message As String, Optional Insert_At As Integer = -1)
        Macro.Has_Errors = True
        If Not Statement.Error_Messages.Contains(Message) Then
            If Insert_At >= 0 Then
                Statement.Error_Messages.Insert(Insert_At, Message)
            Else
                Statement.Error_Messages.Add(Message)
            End If
        End If
    End Sub

    Public Sub Add_Help_Message(Statement As class_Program_Line, Help As enum_Error_Help_Messages)
        If Not Statement.Help_Messages.Contains(Help) Then
            Statement.Help_Messages.Add(Help)
        End If
    End Sub

    Public Function Get_Help(Line_Number As Integer) As String
        Dim Mess As String = ""

        If Macro.Program(Line_Number).Help_Messages.Count = 0 Then

            Select Case Macro.Program(Line_Number).Type
                Case enum_Component_Type._KEYWORD_MACRO
                    Mess &= Help_MACRO

                Case enum_Component_Type._KEYWORD_IF, enum_Component_Type._KEYWORD_ELSE, enum_Component_Type._KEYWORD_IF_END
                    Mess &= Help_IF & vbCrLf & vbCrLf
                    Mess &= Help_Logical_EXPRESSION

                Case enum_Component_Type._KEYWORD_FOR, enum_Component_Type._KEYWORD_NEXT
                    Mess &= Help_FOR

                Case enum_Component_Type._KEYWORD_WHILE, enum_Component_Type._KEYWORD_WHILE_END
                    Mess &= Help_WHILE & vbCrLf & vbCrLf
                    Mess &= Help_Logical_EXPRESSION

                Case enum_Component_Type._KEYWORD_DO, enum_Component_Type._KEYWORD_UNTIL
                    Mess &= Help_DO & vbCrLf & vbCrLf
                    Mess &= Help_Logical_EXPRESSION

                Case enum_Component_Type._KEYWORD_WHOLE
                    Mess &= Help_Local_INTEGER_Variable

                Case enum_Component_Type._KEYWORD_REAL
                    Mess &= Help_Local_FLOAT_Variable

                Case enum_Component_Type._KEYWORD_STRING
                    Mess &= Help_Local_STRING_Variable & vbCrLf
                    Mess &= Help_String_Constants

                Case enum_Component_Type._EXPRESSION

                    For I = 0 To Macro.Program(Line_Number).Components.Count - 1
                        Select Case Macro.Program(Line_Number).Components(I).Token

                            Case "FORMAT", "LEN"
                                Mess &= Help_String_Functions

                        End Select
                    Next
                    If Mess = "" Then
                        Mess &= Help_Math_EXPRESSION
                        Mess &= Help_Math_Functions
                    End If

                Case enum_Component_Type._KEYWORD_LABEL
                    Mess &= Help_LABEL

                Case enum_Component_Type._KEYWORD_BREAKPOINT
                    Mess &= Help_BREAKPOINT

                Case enum_Component_Type._KEYWORD_GOTO
                    Mess &= Help_GOTO

                Case enum_Component_Type._KEYWORD_CALL
                    Mess &= Help_CALL

                Case enum_Error_Help_Messages.CYCLE_Help
                    Mess &= Help_Cycle

                Case enum_Error_Help_Messages.SPINDLE_Help
                    Mess &= Help_Spindle

                Case enum_Error_Help_Messages.COOLANT_Help
                    Mess &= Help_Coolant

                Case enum_Error_Help_Messages.FEED_OVERRIDE_Help
                    Mess &= Help_Feed_Overide

                Case enum_Error_Help_Messages.PAUSE_Help
                    Mess &= Help_Pause

                Case enum_Error_Help_Messages.MESSAGE_Help
                    Mess &= Help_Message

                Case enum_Error_Help_Messages.BROWSE_Help
                    Mess &= Help_Browse
                Case enum_Error_Help_Messages.READ_Help
                    Mess &= Help_READ
                Case enum_Error_Help_Messages.WRITE_Help
                    Mess &= Help_WRITE
                Case enum_Error_Help_Messages.System_Functions_Help
                    Mess &= Help_System_Functions

            End Select

            Mess &= vbCrLf

        Else
            For I = 0 To Macro.Program(Line_Number).Help_Messages.Count - 1

                Select Case Macro.Program(Line_Number).Help_Messages(I)

                    Case enum_Error_Help_Messages.Macro_Help
                        Mess &= Help_MACRO

                    Case enum_Error_Help_Messages.IF_Help
                        Mess &= Help_IF

                    Case enum_Error_Help_Messages.FOR_Help

                        Mess &= Help_FOR
                    Case enum_Error_Help_Messages.WHILE_Help
                        Mess &= Help_WHILE

                    Case enum_Error_Help_Messages.DO_Help
                        Mess &= Help_DO

                    Case enum_Error_Help_Messages.Local_WHOLE_Variable_Help
                        Mess &= Help_Local_INTEGER_Variable

                    Case enum_Error_Help_Messages.Local_FLOAT_Variable_Help
                        Mess &= Help_Local_FLOAT_Variable

                    Case enum_Error_Help_Messages.Local_STRING_Variable_Help
                        Mess &= Help_Local_STRING_Variable

                    Case enum_Error_Help_Messages.Local_WHOLE_LIST_Variable_Help
                        Mess &= Help_Local_INTEGER_LIST_Variable

                    Case enum_Error_Help_Messages.Local_REAL_LIST_Variable_Help
                        Mess &= Help_Local_FLOAT_LIST_Variable

                    Case enum_Error_Help_Messages.Local_STRING_LIST_Variable_Help
                        Mess &= Help_Local_STRING_LIST_Variable

                    Case enum_Error_Help_Messages.Symbol_Help
                        Mess &= Help_SYMBOL

                    Case enum_Error_Help_Messages.String_Functions_Help
                        Mess &= Help_String_Functions

                    Case enum_Error_Help_Messages.Logical_EXPRESSION_Help
                        Mess &= Help_Logical_EXPRESSION

                    Case enum_Error_Help_Messages.Math_EXPRESSION_Help
                        Mess &= Help_Math_EXPRESSION

                    Case enum_Error_Help_Messages.LABEL_Help
                        Mess &= Help_LABEL

                    Case enum_Error_Help_Messages.BREAKPOINT_Help
                        Mess &= Help_BREAKPOINT

                    Case enum_Error_Help_Messages.GOTO_Help
                        Mess &= Help_GOTO

                    Case enum_Error_Help_Messages.CALL_Help
                        Mess &= Help_CALL

                    Case enum_Error_Help_Messages.Unpaired_Help
                        Mess &= Help_Unpaired

                    Case enum_Error_Help_Messages.String_Constant_Help
                        Mess &= Help_String_Constants

                    Case enum_Error_Help_Messages.CYCLE_Help
                        Mess &= Help_Cycle

                    Case enum_Error_Help_Messages.SPINDLE_Help
                        Mess &= Help_Spindle

                    Case enum_Error_Help_Messages.COOLANT_Help
                        Mess &= Help_Coolant

                    Case enum_Error_Help_Messages.FEED_OVERRIDE_Help
                        Mess &= Help_Feed_Overide

                    Case enum_Error_Help_Messages.PAUSE_Help
                        Mess &= Help_Pause

                    Case enum_Error_Help_Messages.MESSAGE_Help
                        Mess &= Help_Message

                    Case enum_Error_Help_Messages.BROWSE_Help
                        Mess &= Help_Browse
                    Case enum_Error_Help_Messages.READ_Help
                        Mess &= Help_READ
                    Case enum_Error_Help_Messages.WRITE_Help
                        Mess &= Help_WRITE
                    Case enum_Error_Help_Messages.System_Functions_Help
                        Mess &= Help_System_Functions

                End Select

                Mess &= vbCrLf
            Next
        End If

        Return Mess

    End Function

#End Region

#Region "Debug Dump Routines"

    Public Sub Program_Dump()
        If Not Dump_Reports Then Exit Sub

        Dim Line As String
        Dim DF As New Class_Data_File

        If Not DF.Open_Output_File(Chip_Macro_Program_Folder & "Program_Dump.txt") Then Exit Sub


        For L = 0 To Macro.Program.Count - 1

            Line = Str(Macro.Program(L).Line_Number).PadLeft(4)
            If Macro.Program(L).Error_Messages.Count = 0 Then
                Line &= ":"
            Else
                Line &= "*"
            End If
            Line &= Macro.Program(L).Raw_Text.PadRight(50) & Macro.Program(L).Type.ToString
            DF.Write_Line(Line)

            For C = 0 To Macro.Program(L).Components.Count - 1
                Line = Space(5) & Macro.Program(L).Components(C).Token.ToString.PadRight(15)
                Line &= Macro.Program(L).Components(C).Category.ToString.PadRight(15)
                Line &= Macro.Program(L).Components(C).Type.ToString
                If Line.Contains(Not_Equal) Then Line = Line.Replace(Not_Equal, "<>")
                If Line.Contains(Greater_Than_Or_Equal) Then Line = Line.Replace(Greater_Than_Or_Equal, ">=")
                If Line.Contains(Less_Than_Or_Equal) Then Line = Line.Replace(Less_Than_Or_Equal, "<=")
                DF.Write_Line(Line)
            Next

            'If Macro.Program(L).Expression.Components.Count > 0 Then
            '    DF.Write_Line("     Expression : " & Macro.Program(L).Expression.Destination_Symbol & " " & _
            '                                         Macro.Program(L).Expression.Destination_Type.ToString)

            '    For C = 0 To Macro.Program(L).Expression.Components.Count - 1
            '        Line = Space(5) & Macro.Program(L).Expression.Components(C).Token.ToString.PadRight(15)
            '        Line &= Macro.Program(L).Expression.Components(C).Category.ToString.PadRight(15)
            '        Line &= Macro.Program(L).Expression.Components(C).Type.ToString
            '        If Line.Contains(Not_Equal) Then Line = Line.Replace(Not_Equal, "<>")
            '        If Line.Contains(Greater_Than_Or_Equal) Then Line = Line.Replace(Greater_Than_Or_Equal, ">=")
            '        If Line.Contains(Less_Than_Or_Equal) Then Line = Line.Replace(Less_Than_Or_Equal, "<=")
            '        DF.Write_Line(Line)
            '    Next
            'End If

            DF.Write_Line("")
        Next

        DF.Close_Output_File()

    End Sub

    Public Sub Stack_Dump()
        If Not Dump_Reports Then Exit Sub

        Dim Line As String
        Dim DF As New Class_Data_File

        DF.Open_Output_File(Chip_Macro_Program_Folder & "Stack_Dump.txt")

        For L = 0 To Macro.Program.Count - 1
            Line = Str(Macro.Program(L).Line_Number).PadLeft(4)
            If Macro.Program(L).Error_Messages.Count = 0 Then
                Line &= ":"
            Else
                Line &= "*"
            End If
            Line &= Macro.Program(L).Raw_Text.PadRight(50) & Macro.Program(L).Type.ToString
            DF.Write_Line(Line)

            For Exp = 0 To Macro.Program(L).Expressions.Count - 1
                Line = Space(12) & "--->" & Macro.Program(L).Expressions(Exp).Destination_Symbol.PadRight(5)
                Line &= Macro.Program(L).Expressions(Exp).Destination_Type.ToString
                DF.Write_Line(Line)
                For R = 0 To Macro.Program(L).Expressions(Exp).RPN_Stack.Count - 1
                    Line = Space(16) & Macro.Program(L).Expressions(Exp).RPN_Stack(R).Token.PadRight(5)
                    Line &= Macro.Program(L).Expressions(Exp).RPN_Stack(R).Type.ToString
                    If Line.Contains(Not_Equal) Then Line = Line.Replace(Not_Equal, "<>")
                    If Line.Contains(Greater_Than_Or_Equal) Then Line = Line.Replace(Greater_Than_Or_Equal, ">=")
                    If Line.Contains(Less_Than_Or_Equal) Then Line = Line.Replace(Less_Than_Or_Equal, "<=")
                    DF.Write_Line(Line)
                Next
            Next

            DF.Write_Line("")
        Next

        DF.Close_Output_File()

    End Sub

    Public Sub Sequence_Dump()
        If Not Dump_Reports Then Exit Sub

        Dim Line As String
        Dim DF As New Class_Data_File

        DF.Open_Output_File(Chip_Macro_Program_Folder & "Sequence_Dump.txt")

        For L = 0 To Macro.Program.Count - 1

            Line = Str(Macro.Program(L).Line_Number).PadLeft(4) & ":" & Macro.Program(L).Raw_Text.PadRight(50) & Macro.Program(L).Type.ToString
            DF.Write_Line(Line)

            Line = "     Begin   ELSE    End   True  False   Next"
            DF.Write_Line(Line)

            Line = Space(5) & Str(Macro.Program(L).Begin_Line_Pointer.Line_Number).PadLeft(5)

            If IsNothing(Macro.Program(L).Else_Line_Pointer) Then
                Line &= "       "
            Else
                Line &= Str(Macro.Program(L).Else_Line_Pointer.Line_Number).PadLeft(7)
            End If

            If IsNothing(Macro.Program(L).End_Line_Pointer) Then
                Line &= "       "
            Else
                Line &= Str(Macro.Program(L).End_Line_Pointer.Line_Number).PadLeft(7)
            End If

            If IsNothing(Macro.Program(L).True_Line_Number) Then
                Line &= "       "
            Else
                Line &= Str(Macro.Program(L).True_Line_Number).PadLeft(7)
            End If

            If IsNothing(Macro.Program(L).False_Line_Number) Then
                Line &= "       "
            Else
                Line &= Str(Macro.Program(L).False_Line_Number).PadLeft(7)
            End If

            If IsNothing(Macro.Program(L).Next_Line_Number) Then
                Line &= "   ****"
            Else
                Line &= Str(Macro.Program(L).Next_Line_Number).PadLeft(7)
            End If


            DF.Write_Line(Line)
            DF.Write_Line("")

        Next

        DF.Close_Output_File()

    End Sub

#End Region

End Class
