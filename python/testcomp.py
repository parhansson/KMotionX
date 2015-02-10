import kmotion
import time


class KMotion(kmotion.KMotion):
    def __init__(self, dev, hostname = None, port = kmotion.KMOTION_PORT, with_console=False):
        if hostname is None:
            super(KMotion, self).__init__(dev)
        else:
            super(KMotion, self).__init__(dev, port, hostname)
        if with_console:
            self.SetConsoleCallback()
        self.SetPollCallback(kmotion.POLL_THREAD)
        self.threads_active = 0
        self.Poll()
    # Overrides of virtual methods in superclass...
    def Console(self, line):
        print "Console:", line,
    def ErrMsg(self, line):
        print "Err:", line
    def ChangedThread(self, threads_active):
        #print "Threads active now 0x%02X" % threads_active
        self.threads_active = threads_active
    
    # Extra methods added for Python...
    def compile(self, basename, thread=1, load_it=True):
        rc, err = self.Compile(basename+".c", basename+".out", kmotion.BOARD_TYPE_KFLOP, thread, 1000)
        if rc:
            raise RuntimeError("Compile %s.c failed, err=%d/%s" % (basename, rc, err))
        if load_it:
            if self.LoadCoff(thread, basename+".out"):
                raise RuntimeError("LoadCoff %s.c failed, err=%d" % (basename, rc))
    def compile_string(self, code, thread=1, load_it=True):
        basename = "/tmp/testkmotion"
        with open(basename+".c", "w") as f:
            f.write(code)
        self.compile(basename, thread, load_it)
    def execute(self, thread=1):
        k.WriteLine("execute%d" % thread)
        self.Poll()
    def wait_thread_done(self, thread=1):
        threadbit = 1<<thread
        while self.threads_active & threadbit:
            self.Poll()
            
    def run(self, basename):
        """Run basename.c on thread 1, and wait for the thread to terminate.
        """
        self.compile(basename)
        self.execute()
        self.wait_thread_done()
    def run_string(self, code):
        """Run basename.c on thread 1, and wait for the thread to terminate.
        """
        self.compile_string(code)
        self.execute()
        self.wait_thread_done()

k = KMotion(0, with_console=True)

print "Compile and run file..."
k.run("test")

code = r"""
#include "KMotionDef.h"
int fac(int n)
{
    return n ? n*fac(n-1) : 1;
}
void main()
{
    int i = %d;
    
    printf("This is a very roundabout way of computing that %%d! = %%d\n", i, fac(i));
    printf("Python string!\n");

}
""" % (9,)


print "Compile and run string..."
k.run_string(code)



