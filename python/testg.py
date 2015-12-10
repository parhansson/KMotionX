
"""
Test GCode interpreter
"""
import kmotion
import time, sys


class KMotion(kmotion.KMotion):
    def __init__(self, dev, hostname = None, port = kmotion.KMOTION_PORT, with_console=False):
        if hostname is None:
            super(KMotion, self).__init__(dev)
        else:
            super(KMotion, self).__init__(dev, port, hostname)
        if with_console:
            self.SetConsoleCallback()
        self.SetPollCallback(kmotion.POLL_THREAD)
        self.Poll()
    # Overrides of virtual methods in superclass...
    def Console(self, line):
        print "Console:", line,
    def ErrMsg(self, line):
        print "Err:", line
    def ChangedThread(self, threads_active):
        self.threads_active = threads_active
    


class Interpreter(kmotion.GCodeInterpreter):
    def __init__(self, k, cm=None):
        if cm is None:
            self.cm = kmotion.CoordMotion(k)
        else:
            self.cm = cm
        super(Interpreter, self).__init__(self.cm)
        self.finished = False
        self.CurrentLineNo = 0
        self.ErrorLineNo = 0
        self.exitcode = 0
    # Overrides of virtual methods in superclass...
    def HandleStatus(self, lineno, msg):
	    print "Status -- line: %d message:\n%s" % (lineno,msg)
	    self.CurrentLineNo=lineno;
    def HandleComplete(self, status, lineno, sequence_number, err):
	    print "Complete -- status: %d line: %d  error: %s" % (status,lineno,err)
	    self.ErrorLineNo=lineno
	    self.ErrorMsg=err
	    self.exitcode=status
	    self.finished=True;
    def HandleUser(self, msg):
        print "User callback:", msg
        return 0
    def HandleMCode(self, mcode):
        print "MCode callback:", mcode
        return 0

BoardType = kmotion.BOARD_TYPE_KFLOP;
setup_thread = 1;
m4_thread = 3;

setup_cfile = "../KMotionX/examples/ExecuteGCode/Stepper3Axis.c"
m4_cfile = "../C Programs/BlinkKFLOP.c"
InFile = "../GCode Programs/SimpleCircle.ngc"

k = KMotion(0, with_console=True)

print "Loading file %s to thread %d" % (setup_cfile, setup_thread)
rc, err = k.CompileAndLoadCoff(setup_cfile, setup_thread, 1000)
if rc:
    print "Loading file failed:", err
    sys.exit(1)
else:
    k.WriteLine("Execute%d" % setup_thread);

terp = Interpreter(k)

mp = terp.GetMotionParams()
mp.BreakAngle = 30;
mp.MaxAccelX = 1;
mp.MaxAccelY = 1;
mp.MaxAccelZ = 1;
mp.MaxAccelA = 1;
mp.MaxAccelB = 1;
mp.MaxAccelC = 1;
mp.MaxVelX = 1;
mp.MaxVelY = 1;
mp.MaxVelZ = 1;
mp.MaxVelA = 1;
mp.MaxVelB = 1;
mp.MaxVelC = 1;
mp.CountsPerInchX = 100;
mp.CountsPerInchY = 100;
mp.CountsPerInchZ = 100;
mp.CountsPerInchA = 100;
mp.CountsPerInchB = 100;
mp.CountsPerInchC = 100;
terp.SetMotionParams(mp)

terp.ToolFile = ""
terp.SetupFile = ""

terp.SetActionCallback(3);
terp.SetActionCallback(kmotion.MCODE_ACTIONS_M100_OFFSET+5)
terp.SetActionProgramWaitSync(4, m4_cfile, m4_thread)

print "Interpreting file", InFile
terp.Interpret(BoardType, InFile, 0, -1, True);

# Display Current Line Number while executing

DisplayedLineNo = 0
while not terp.finished:
	if terp.CurrentLineNo > DisplayedLineNo:
		DisplayedLineNo = terp.CurrentLineNo;
		#printf "Current Line", terp.CurrentLineNo
	time.sleep(0.01)


# Check Interpreter's exit code

if terp.exitcode:
	print "Error in line", terp.ErrorLineNo
	print terp.ErrorMsg

