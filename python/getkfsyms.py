import kmotion
import time, sys, os


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
        self.cnbuf = ""
        self.Poll()
    # Overrides of virtual methods in superclass...
    def Console(self, line):
        #print "Console:", line,
        self.cnbuf += line
    def ErrMsg(self, line):
        print "Err:", line
    def ChangedThread(self, threads_active):
        #print "Threads active now 0x%02X" % threads_active
        self.threads_active = threads_active
    
    # Extra methods added for Python...
    def compile(self, basename, thread=1, load_it=True):
        rc, err = self.Compile(basename+".c", basename+".out", kmotion.BOARD_TYPE_KFLOP, thread, 1000)
        if rc:
            raise RuntimeError("Compile %s.c failed:\n  %s" % (basename, err))
        if load_it:
            if self.LoadCoff(thread, basename+".out"):
                raise RuntimeError("LoadCoff %s.out failed" % (basename, ))
    def compile_string(self, code, thread=1, load_it=True):
        basename = "/tmp/testkmotion"
        with open(basename+".c", "w") as f:
            f.write(code)
        self.compile(basename, thread, load_it)
    def load(self, basename, thread=1):
        if self.LoadCoff(thread, basename+".out"):
            raise RuntimeError("LoadCoff %s.out failed" % (basename, ))
        
    def execute(self, thread=1):
        k.WriteLine("execute%d" % thread)
        self.Poll()
    def wait_thread_done(self, thread=1):
        threadbit = 1<<thread
        while self.threads_active & threadbit:
            self.Poll()
            
    def write2file(self, filename):
        with open(filename, "w") as f:
            f.write(self.cnbuf)
            
    def run(self, basename, thread=1):
        """Run basename.c on thread 1, and wait for the thread to terminate.
        """
        try:
            self.compile(basename, thread)
        except RuntimeError as e:
            print e
            return
        self.execute(thread)
        self.wait_thread_done(thread)
    def run_string(self, code, thread=1):
        """Run C code string on thread 1, and wait for the thread to terminate.
        """
        try:
            self.compile_string(code, thread)
        except RuntimeError as e:
            print e
            return
        self.execute(thread)
        self.wait_thread_done(thread)
        
def runcmd(cmd):
    print
    print "Running command:"
    print cmd
    print
    try:
        return os.system(cmd)
    except:
        return -1

def keep(sym):
    if len(sym) < 3 or sym[:2] in ('__','_i'): return False
    if sym[1:] not in ('gather_buffer', 'persist', 'ch0', 'EnableAxis'): return False
    return True

k = KMotion(0, with_console=True)

#print "Compile and run file..."
#k.run("test")

compilerbin =  "tcc67wine"
k.SetCustomCompiler(compilerbin)
kflopdir = "/home/steve/KMotionX/DSP_KFLOP"
kflophdrs = ["/home/steve/KMotionX/DSP_KFLOP/KMotionDef.h"]
kflopout = os.path.join(kflopdir, "DSPKFLOP.out")
c6xloc = "/home/steve/c6000_7.4.14/bin"
nm6x = os.path.join(c6xloc, "nm6x")

kffilename = "/tmp/%s.nm" % (os.path.basename(kflopout),)
cmd = '%s -g "%s" > "%s"' % (nm6x, kflopout, kffilename)
runcmd(cmd)
usyms = []
nsyms = 0
with open(kffilename,"r") as f:
    for s in f.readlines():
        ss = s.split(' ')
        while ss.pop(0) == '': pass # skip blanks and first field (line number)
        if ss[0] in set(['T','B','D']): # select global text, bss and data symbols
            sym = ss[1].strip()
            if sym.startswith("_"): # skip if not underscored (mangled C cymbol)
                if keep(sym):
                    usyms.append(sym[1:])
                    nsyms += 1
                    #if nsyms > 20: break
                
exts = [r'extern int %s;' % (s,) for s in usyms if s != 'printf']
exts = '\n'.join(exts)
syms = [r'psym("%s",&%s);' % (s,s) for s in usyms]
syms = '\n'.join(syms)

codeexts = r"""
//#include "KMotionDef.h"
extern void printf(const char * fmt, ...);
%s
void psym(const char * s, const void * v)
{ printf("_%%s = 0x%%p;\n", s, v); }
main() {
%s
}
"""

code = r"""
#include "KMotionDef.h"
void psym(const char * s, const void * v)
{ printf("_%%s = 0x%%p;\n", s, v); }
main() {
%s
}
"""




print "Compile and run string..."
#k.run_string(codeexts % (exts, syms,))
k.run_string(code % (syms,))
#k.write2file("kflopout"+".syms")
#print code % (exts, syms,)
print
print k.cnbuf


