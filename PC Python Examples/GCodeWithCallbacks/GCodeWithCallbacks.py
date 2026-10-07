import sys,clr,os.path
import ctypes
import time

PathRoot = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

sys.path.append(PathRoot + r"\KMotion\Debug64")
clr.AddReference("KMotion_dotNet")
from KMotion_dotNet import *

KM = KM_Controller()
CM = KM.CoordMotion
GC = CM.Interpreter

GCodeOutput = ""
ErrorLineNo = 0
CurrentLineNo = 0
ErrorMsg = ""
exitcode = 0
Finished = False

def CompleteCallback(status, line_no, sequence_number, err):
    global ErrorLineNo, ErrorMsg, exitcode, Finished
    ErrorLineNo=line_no
    ErrorMsg=err
    exitcode=status
    Finished=True
    print("Execution complete with exit code: ", exitcode)
    if(exitcode != 0):
        print("Error line number: ", ErrorLineNo)
        print("Error message: ", ErrorMsg)
    
def StatusCallback(line_no, msg):
    global CurrentLineNo, GCodeOutput
    CurrentLineNo = line_no
    GCodeOutput += msg
    print("CurrentLineNo: ", CurrentLineNo)
    print("Message: ", msg)
    
def reset():
    global ErrorLineNo, ErrorMsg, exitcode, Finished, CurrentLineNo, GCodeOutput
    GCodeOutput = ""
    ErrorLineNo = 0
    CurrentLineNo = 0
    ErrorMsg = ""
    exitcode = 0
    Finished = False
    print("reset")
    
def executeFile(file_name):
    global Finished
    print(GC.Interpret(file_name))
    while(not(Finished)):
        time.sleep(1)
    
try:
    KM.ExecuteProgram(1, PathRoot + r"\C Programs\KStep\InitKStep3Axis.c", False)
    
    MP = CM.MotionParams
    MP.BreakAngle = 30
    MP.MaxAccelX = 1
    MP.MaxAccelY = 1
    MP.MaxAccelZ = 1
    MP.MaxAccelA = 1
    MP.MaxAccelB = 1
    MP.MaxAccelC = 1
    MP.MaxVelX = 1
    MP.MaxVelY = 1
    MP.MaxVelZ = 1
    MP.MaxVelA = 1
    MP.MaxVelB = 1
    MP.MaxVelC = 1
    MP.CountsPerInchX = 100
    MP.CountsPerInchY = 100
    MP.CountsPerInchZ = 100
    MP.CountsPerInchA = 100
    MP.CountsPerInchB = 100
    MP.CountsPerInchC = 100
    MP.DegreesA = False
    MP.DegreesB = False
    MP.DegreesC = False

    CM.Abort()
    CM.ClearAbort()
    GC.InterpreterCompleted += CompleteCallback
    GC.InterpreterStatusUpdated += StatusCallback
    GC.InitializeInterpreter()

    #call to new gcode file
    executeFile(PathRoot + r"\GCode Programs\box10.ngc")
    reset()
    
    GC.InitializeInterpreter()
    GC.InitializeOnExecute = False
    GC.SetupParams.SetParameter(100,123.4)
    GC.SetupParams.SetParameter(101,50.0)
    Var = GC.SetupParams.GetParameter(100)
    print (Var)
    executeFile(PathRoot + r"\GCode Programs\AddVars100_101.ngc")
    reset()
    Var = GC.SetupParams.GetParameter(102)
    print (Var)
    time.sleep(3)

except DMException as error:
    error_string = str(error.InnerException)
    MessageBox = ctypes.windll.user32.MessageBoxW
    MessageBox(None, error_string, 'KMotion', 0)


