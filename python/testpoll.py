import kmotion
import time

"""
Low-level test for polling callbacks, from KMotionDLL.
"""

# Derive a class which handles ChangedPos events.
class KMotion(kmotion.KMotion):
    def __init__(self, dev, hostname = None, port = kmotion.KMOTION_PORT, with_console=False):
        if hostname is None:
            super(KMotion, self).__init__(dev)
        else:
            super(KMotion, self).__init__(dev, port, hostname)
        if with_console:
            self.SetConsoleCallback()
        self.SetPollCallback(kmotion.POLL_POS|kmotion.POLL_DONE|kmotion.POLL_DEST)
        self.axis_done = 0
    # Overrides of virtual methods in superclass...
    def Console(self, line):
        print "Console:", line,
    def ErrMsg(self, line):
        print "Err:", line
    def ChangedPos(self, pos):
        print "Position:"
        for axis, p in zip("XYZ", pos):
            print axis,p,'  ',
        print
    def ChangedDest(self, pos):
        print "Dest:"
        for axis, p in zip("XYZ", pos):
            print axis,p,'  ',
        print
    def ChangedDone(self, axis_done):
        print "Done:", axis_done
        self.axis_done = axis_done
    
    # Extra methods added for Python...
    def axisinit(self):
        for i in range(6):
            self.WriteLine("enableaxis%d" % (i,))
            self.WriteLine("zero%d" % (i,))
    def move(self, x, y):
        self.WriteLine("movexyzabc %g %g 0 0 0 0" % (x, y))
        self.axis_done = 0
    def alldone(self):
        return self.axis_done == 0x3F
    

k = KMotion(0, with_console=True)

k.axisinit()
k.Poll()
k.move(20000., 30000.)
if True:
    while not k.alldone():
        k.Poll()
        time.sleep(0.01)
print "...that's it folks..."



