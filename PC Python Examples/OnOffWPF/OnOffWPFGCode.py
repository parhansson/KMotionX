import wpf,sys,clr,System,os.path
import ctypes

from System.Windows import Application, Window, Diagnostics
from os.path import dirname

sys.path.append(dirname(dirname(dirname(dirname(os.path.abspath(__file__))))) + r"\KMotion\Debug")
clr.AddReference("KMotion_dotNet")
from KMotion_dotNet import *

KM = KM_Controller()
CoordMotion = KM_CoordMotion(KM)
Interpeter = KM_Interpreter(CoordMotion)

try:

    KM.ExecuteProgram(1, r"C:\KMotion435e\C Programs\KStep\InitKStep3Axis.c", False);

    KM.CoordMotion.MotionParams.BreakAngle = 30;
    KM.CoordMotion.MotionParams.MaxAccelX = 1;
    KM.CoordMotion.MotionParams.MaxAccelY = 1;
    KM.CoordMotion.MotionParams.MaxAccelZ = 1;
    KM.CoordMotion.MotionParams.MaxAccelA = 1;
    KM.CoordMotion.MotionParams.MaxAccelB = 1;
    KM.CoordMotion.MotionParams.MaxAccelC = 1;
    KM.CoordMotion.MotionParams.MaxVelX = 1;
    KM.CoordMotion.MotionParams.MaxVelY = 1;
    KM.CoordMotion.MotionParams.MaxVelZ = 1;
    KM.CoordMotion.MotionParams.MaxVelA = 1;
    KM.CoordMotion.MotionParams.MaxVelB = 1;
    KM.CoordMotion.MotionParams.MaxVelC = 1;
    KM.CoordMotion.MotionParams.CountsPerInchX = 100;
    KM.CoordMotion.MotionParams.CountsPerInchY = 100;
    KM.CoordMotion.MotionParams.CountsPerInchZ = 100;
    KM.CoordMotion.MotionParams.CountsPerInchA = 100;
    KM.CoordMotion.MotionParams.CountsPerInchB = 100;
    KM.CoordMotion.MotionParams.CountsPerInchC = 100;
    KM.CoordMotion.MotionParams.DegreesA = False;
    KM.CoordMotion.MotionParams.DegreesB = False;
    KM.CoordMotion.MotionParams.DegreesC = False;

    KM.CoordMotion.Abort();
    KM.CoordMotion.ClearAbort();
    KM.CoordMotion.Interpreter.InitializeInterpreter();

    print(Interpeter.Interpret(r"C:\KMotion435e\GCode Programs\box10.ngc"))


except DMException as error:
    error_string = str(error.InnerException)
    MessageBox = ctypes.windll.user32.MessageBoxW
    MessageBox(None, error_string, 'KMotion', 0)


class MyWindow(Window):
    def __init__(self):
        wpf.LoadComponent(self, 'OnOffWPF.xaml')
    
    def ONButton_Click(self, sender, e):
        KM.WriteLine("SetBit46")
       
    def OFFButton_Click(self, sender, e):
        KM.WriteLine("ClearBit46")

if __name__ == '__main__':
    Application().Run(MyWindow())
