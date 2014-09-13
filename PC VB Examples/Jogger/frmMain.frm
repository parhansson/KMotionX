VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.1#0"; "mscomctl.ocx"
Begin VB.Form frmMain 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Jogger"
   ClientHeight    =   5430
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   6165
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5430
   ScaleWidth      =   6165
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Execute 
      Caption         =   "Execute"
      Height          =   375
      Left            =   2760
      TabIndex        =   23
      Top             =   4200
      Width           =   2055
   End
   Begin VB.CommandButton C 
      Caption         =   "CompileLoadCoff"
      Height          =   375
      Left            =   240
      TabIndex        =   21
      Top             =   4200
      Width           =   2175
   End
   Begin VB.TextBox CoffFile 
      Height          =   375
      Left            =   360
      TabIndex        =   20
      Text            =   "Text1"
      Top             =   3720
      Width           =   5535
   End
   Begin VB.Timer Timer1 
      Interval        =   300
      Left            =   120
      Top             =   1560
   End
   Begin MSComctlLib.Slider sldSpeedX 
      Height          =   255
      Left            =   120
      TabIndex        =   8
      Top             =   2760
      Width           =   1215
      _ExtentX        =   2143
      _ExtentY        =   450
      _Version        =   393216
      Max             =   100
      SelStart        =   25
      TickFrequency   =   10
      Value           =   25
   End
   Begin VB.CommandButton cmdANeg 
      Caption         =   "Neg"
      Height          =   495
      Index           =   6
      Left            =   4920
      TabIndex        =   7
      Top             =   1560
      Width           =   615
   End
   Begin VB.CommandButton cmdAPos 
      Caption         =   "Pos"
      Height          =   495
      Index           =   5
      Left            =   4920
      TabIndex        =   6
      Top             =   120
      Width           =   615
   End
   Begin VB.CommandButton cmdZDown 
      Caption         =   "Down"
      Height          =   495
      Index           =   4
      Left            =   3600
      TabIndex        =   5
      Top             =   1560
      Width           =   615
   End
   Begin VB.CommandButton cmdZUp 
      Caption         =   "Up"
      Height          =   495
      Index           =   3
      Left            =   3600
      TabIndex        =   4
      Top             =   120
      Width           =   615
   End
   Begin VB.CommandButton cmdYFront 
      Caption         =   "Front"
      Height          =   495
      Index           =   2
      Left            =   1320
      TabIndex        =   3
      Top             =   1560
      Width           =   615
   End
   Begin VB.CommandButton cmdXRight 
      Caption         =   "Right"
      Height          =   495
      Index           =   1
      Left            =   2280
      TabIndex        =   2
      Top             =   840
      Width           =   615
   End
   Begin VB.CommandButton cmdYBack 
      Caption         =   "Back"
      Height          =   495
      Left            =   1320
      TabIndex        =   1
      Top             =   120
      Width           =   615
   End
   Begin VB.CommandButton cmdXLeft 
      Caption         =   "Left"
      Height          =   495
      Index           =   0
      Left            =   360
      TabIndex        =   0
      Top             =   840
      Width           =   615
   End
   Begin MSComctlLib.Slider sldSpeedY 
      Height          =   255
      Left            =   1920
      TabIndex        =   9
      Top             =   2760
      Width           =   1215
      _ExtentX        =   2143
      _ExtentY        =   450
      _Version        =   393216
      Max             =   100
      SelStart        =   25
      TickFrequency   =   10
      Value           =   25
   End
   Begin MSComctlLib.Slider sldSpeedZ 
      Height          =   255
      Left            =   3360
      TabIndex        =   10
      Top             =   2760
      Width           =   1215
      _ExtentX        =   2143
      _ExtentY        =   450
      _Version        =   393216
      Max             =   100
      SelStart        =   25
      TickFrequency   =   10
      Value           =   25
   End
   Begin MSComctlLib.Slider sldSpeedA 
      Height          =   255
      Left            =   4680
      TabIndex        =   11
      Top             =   2760
      Width           =   1215
      _ExtentX        =   2143
      _ExtentY        =   450
      _Version        =   393216
      Max             =   100
      SelStart        =   25
      TickFrequency   =   10
      Value           =   25
   End
   Begin VB.Label CompileMsg 
      Height          =   255
      Left            =   240
      TabIndex        =   22
      Top             =   4800
      Width           =   5655
   End
   Begin VB.Label USBLocation 
      Caption         =   "USB Location"
      Height          =   255
      Left            =   1680
      TabIndex        =   19
      Top             =   3240
      Width           =   2535
   End
   Begin VB.Label txtAPos 
      Caption         =   "-01.234"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   13.5
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   4800
      TabIndex        =   18
      Top             =   2280
      Width           =   975
   End
   Begin VB.Label txtZPos 
      Caption         =   "-01.234"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   13.5
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   3480
      TabIndex        =   17
      Top             =   2280
      Width           =   975
   End
   Begin VB.Label txtYPos 
      Caption         =   "-01.234"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   13.5
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   2040
      TabIndex        =   16
      Top             =   2280
      Width           =   975
   End
   Begin VB.Label txtXPos 
      Caption         =   "-01.234"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   13.5
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   240
      TabIndex        =   15
      Top             =   2280
      Width           =   975
   End
   Begin VB.Label Label3 
      Alignment       =   2  'Center
      Caption         =   "A"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   13.5
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   5040
      TabIndex        =   14
      Top             =   960
      Width           =   375
   End
   Begin VB.Label Label2 
      Alignment       =   2  'Center
      Caption         =   "Z"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   13.5
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   3720
      TabIndex        =   13
      Top             =   960
      Width           =   375
   End
   Begin VB.Label Label1 
      Caption         =   "X && Y"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   13.5
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   360
      TabIndex        =   12
      Top             =   240
      Width           =   855
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Const MaxSpeedX As Single = 16000
Const MaxSpeedY As Single = 16000
Const MaxSpeedZ As Single = 16000
Const MaxSpeedA As Single = 16000
Const cpiX As Single = 16000
Const cpiY As Single = 16000
Const cpiZ As Single = 16000
Const cpiA As Single = 5729.6
    Dim board As Integer


Private Sub myerror()
    'Text1.Text = Format(Now, "hh:mm:ss mm/dd/yy") + ": myerror"
    'Text2.Text = Format(StartTime - Now, "hhh:mm:ss mm/dd/yy") + " Elapsed"
    'displayTime "MyError"
    MsgBox "MYERROR CALLED"
    'stopflag = 1
End Sub

Private Sub C_Click()
    Dim Error As String
    Dim result As Integer
    
    result = KM_CompileAndLoadCoff(1, CoffFile.Text, Error, 100)
    
    If (result = 0) Then
        CompileMsg = " No Errors"
    Else
        CompileMsg = Error
    End If
End Sub

Private Sub cmdANeg_MouseDown(Index As Integer, Button As Integer, Shift As Integer, X As Single, Y As Single)
    'go
    Debug.Print "jog A Neg"
    If KM_WriteLine("EnableAxis3") Then myerror
    If KM_WriteLine("Jog3=" + Trim(Str(-1 * sldSpeedA.Value / sldSpeedA.Max * MaxSpeedA))) Then myerror
End Sub

Private Sub cmdANeg_MouseUp(Index As Integer, Button As Integer, Shift As Integer, X As Single, Y As Single)
    'stop
    If KM_WriteLine("Jog3=0") Then myerror
End Sub

Private Sub cmdAPos_MouseDown(Index As Integer, Button As Integer, Shift As Integer, X As Single, Y As Single)
    'go
    Debug.Print "jog A Pos"
    If KM_WriteLine("EnableAxis3") Then myerror
    If KM_WriteLine("Jog3=" + Trim(Str(sldSpeedA.Value / sldSpeedA.Max * MaxSpeedA))) Then myerror
End Sub

Private Sub cmdAPos_MouseUp(Index As Integer, Button As Integer, Shift As Integer, X As Single, Y As Single)
    'stop
    If KM_WriteLine("Jog3=0") Then myerror
End Sub

Private Sub cmdXLeft_MouseDown(Index As Integer, Button As Integer, Shift As Integer, X As Single, Y As Single)
    'go
    Debug.Print "jog x left"
    'If KM_WriteLine("EnableAxis0") Then myerror
    If KM_WriteLine("Jog0=" + Trim(Str(-1 * sldSpeedX.Value / sldSpeedX.Max * MaxSpeedX))) Then myerror
End Sub

Private Sub cmdXLeft_MouseUp(Index As Integer, Button As Integer, Shift As Integer, X As Single, Y As Single)
    'stop
    If KM_WriteLine("Jog0=0") Then myerror
End Sub

Private Sub cmdXRight_MouseDown(Index As Integer, Button As Integer, Shift As Integer, X As Single, Y As Single)
    'go
    Debug.Print "jog x Right"
    'If KM_WriteLine("EnableAxis0") Then myerror
    If KM_WriteLine("Jog0=" + Trim(Str(sldSpeedX.Value / sldSpeedX.Max * MaxSpeedX))) Then myerror
End Sub

Private Sub cmdXRight_MouseUp(Index As Integer, Button As Integer, Shift As Integer, X As Single, Y As Single)
    'stop
    If KM_WriteLine("Jog0=0") Then myerror
End Sub

Private Sub cmdYBack_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    'go
    Debug.Print "jog y back"
    'If KM_WriteLine("EnableAxis1=1") Then myerror
    If KM_WriteLine("Jog1=" + Trim(Str(-1 * sldSpeedY.Value / sldSpeedY.Max * MaxSpeedY))) Then myerror

End Sub

Private Sub cmdYBack_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    'stop
    Debug.Print "jog y stop"
    If KM_WriteLine("Jog1=0") Then myerror
End Sub

Private Sub cmdYFront_MouseDown(Index As Integer, Button As Integer, Shift As Integer, X As Single, Y As Single)
    'go
    Debug.Print "jog y back"
    If KM_WriteLine("EnableAxis1=1") Then myerror
    If KM_WriteLine("Jog1=" + Trim(Str(sldSpeedY.Value / sldSpeedY.Max * MaxSpeedY))) Then myerror
End Sub

Private Sub cmdYFront_MouseUp(Index As Integer, Button As Integer, Shift As Integer, X As Single, Y As Single)
    'stop
    Debug.Print "jog y stop"
    If KM_WriteLine("Jog1=0") Then myerror
End Sub

Private Sub cmdZDown_MouseDown(Index As Integer, Button As Integer, Shift As Integer, X As Single, Y As Single)
    'go
    Debug.Print "jog Z Down"
    If KM_WriteLine("EnableAxis2") Then myerror
    If KM_WriteLine("Jog2=" + Trim(Str(-1 * sldSpeedZ.Value / sldSpeedZ.Max * MaxSpeedZ))) Then myerror
End Sub

Private Sub cmdZDown_MouseUp(Index As Integer, Button As Integer, Shift As Integer, X As Single, Y As Single)
    'stop
    If KM_WriteLine("Jog2=0") Then myerror
End Sub

Private Sub cmdZUp_MouseDown(Index As Integer, Button As Integer, Shift As Integer, X As Single, Y As Single)
    'go
    Debug.Print "jog Z up"
    If KM_WriteLine("EnableAxis2") Then myerror
    If KM_WriteLine("Jog2=" + Trim(Str(sldSpeedZ.Value / sldSpeedZ.Max * MaxSpeedZ))) Then myerror
End Sub

Private Sub cmdZUp_MouseUp(Index As Integer, Button As Integer, Shift As Integer, X As Single, Y As Single)
    'stop
    If KM_WriteLine("Jog2=0") Then myerror
End Sub

Private Sub Execute_Click()
    If KM_WriteLine("Execute1") <> 0 Then
        myerror
    End If
End Sub

Private Sub Form_Load()
    KM_Open
    
    Dim s As String
    Dim i As Integer
    
    s = App.Path
    
    i = InStr(s, "PC VB Examples")
    If (i > 0) Then
        s = Left$(s, i - 2)
        s = s & "\C Programs\KStep\InitKStep3Axis.c"
    End If
    
    i = InStr(s, "KMotion\Release")
    If (i > 0) Then
        s = Left$(s, i - 2)
        s = s & "\C Programs\KStep\InitKStep3Axis.c"
    End If
    
    i = InStr(s, "KMotion\Debug")
    If (i > 0) Then
        s = Left$(s, i - 2)
        s = s & "\C Programs\KStep\InitKStep3Axis.c"
    End If
    
    CoffFile.Text = s
    
End Sub

Private Sub Timer1_Timer()
    Dim rtrnstr As String
    Dim USBPort As Integer
    Dim result As Integer
    Dim KMOTION_LOCKED, KMOTION_IN_USE, KMOTION_NOT_CONNECTED As Integer
    Dim List(256) As Long
    Dim nList As Long
    
    result = KM_ListLocations(nList, List) 'See which boards are present
    
    If (nList > 0) Then     ' if we have at least one pick the first port
        board = List(0)
    Else
        board = 0
    End If
    
        
    KMOTION_LOCKED = 0          ' (and token is locked) if KMotion is available for use
    KMOTION_IN_USE = 1          ' if already in use
    KMOTION_NOT_CONNECTED = 2   ' if error or not able to connect
    
    result = KM_KMotionLock(board) 'See if we can lock the board
    
    If (result = KMOTION_LOCKED) Then
        KM_ReleaseToken (board)  ' Yes, we locked it so release it
    End If
    
    If (result = KMOTION_NOT_CONNECTED) Then
        USBLocation.Caption = "USB Port Location = Not Present"
    Else
        USBPort = KM_USBLocation(board)
    
        USBLocation.Caption = "USB Port Location = " + Format(USBPort)
 
        'report positions in inches or degrees
        If KM_WriteLineReadLine("dest0", rtrnstr) > 0 Then
            myerror
        Else
            txtXPos.Caption = Format(Val(rtrnstr) / cpiX, "0.000")
            If KM_WriteLineReadLine("dest1", rtrnstr) > 0 Then
                myerror
            Else
                txtYPos.Caption = Format(Val(rtrnstr) / cpiY, "0.000")
                If KM_WriteLineReadLine("dest2", rtrnstr) > 0 Then
                    myerror
                Else
                    txtZPos.Caption = Format(Val(rtrnstr) / cpiZ, "0.000")
                    If KM_WriteLineReadLine("dest3", rtrnstr) > 0 Then
                        myerror
                    Else
                        txtAPos.Caption = Format(Val(rtrnstr) / cpiA, "0.000")
                    End If
                End If
            End If
        End If
    End If
End Sub
