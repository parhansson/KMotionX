"""
Extra utilities for kmotion.py
"""

import kmotion
import os, time


class Message(object):
    def __init__(self, lineno, msg):
        self.lineno = lineno
        self.msg = msg.strip()
    def get_type(self):
        return "status"
    def get_severity(self):
        return "I"
    def is_fatal(self):
        return False
    def get_line(self):
        return self.lineno
    def show_line(self):
        return False
    def show_on_status(self):
        return False
    def is_valid_line(self):
        return self.lineno > 0
    def get_msg(self):
        return self.msg
    def get_code(self):
        return 0
    def get_filename(self):
        return None
    def __str__(self):
        if self.is_valid_line():
            return self.get_type()+":"+str(self.get_line())+": "+self.get_msg()
        return self.get_type()+": "+self.get_msg()
        
class ConsoleMessage(Message):
    def __init__(self, msg):
        super(ConsoleMessage, self).__init__(-1, msg)
    def get_type(self):
        return "console"
    def get_severity(self):
        return "I"
    def is_valid_line(self):
        return False

class DebugMessage(Message):
    def __init__(self, msg):
        super(DebugMessage, self).__init__(-1, msg)
    def get_type(self):
        return "debug"
    def get_severity(self):
        return "D"
    def is_valid_line(self):
        return False

class ErrorMessage(Message):
    def __init__(self, lineno, msg, code):
        super(ErrorMessage, self).__init__(lineno, msg)
        self.code = code
    def get_type(self):
        return "ERROR"
    def get_severity(self):
        return "E"
    def get_code(self):
        return self.code
    def show_line(self):
        return self.is_valid_line()
    def __str__(self):
        s = self.get_type()
        if self.is_valid_line():
            s += ":"+str(self.get_line())
        s += ": "+self.get_msg()
        if self.code:
            s += " (code "+str(self.code)+")"
        return s

class DLLErrorMessage(ErrorMessage):
    def __init__(self, msg):
        super(DLLErrorMessage, self).__init__(-1, msg, 0)
    def get_type(self):
        return "DLL ERROR"
    def get_severity(self):
        return "F"
    def is_fatal(self):
        return True
    def is_valid_line(self):
        return False

class CompilerErrorMessage(ErrorMessage):
    def __init__(self, msg, code, filename):
        super(CompilerErrorMessage, self).__init__(-1, msg, code)
        self.filename = filename
    def get_type(self):
        return "C ERROR"
    def get_filename(self):
        return self.filename
    def is_valid_line(self):
        return False

class ConfigErrorMessage(ErrorMessage):
    def __init__(self, msg):
        super(ConfigErrorMessage, self).__init__(-1, msg, 0)
    def get_type(self):
        return "CONFIG ERROR"
    def is_valid_line(self):
        return False

class InternalErrorMessage(ErrorMessage):
    def __init__(self, msg):
        super(InternalErrorMessage, self).__init__(-1, msg, 0)
    def get_type(self):
        return "INTERNAL ERROR"
    def is_valid_line(self):
        return False
    def show_on_status(self):
        return True




class ThreadManager(object):
    """Class to manage kflop thread execution.
  
    """
    # Options for compile and/or load and/or run (in that sequence)...
    AUTO = 0        # Automatically perform operation if required
    ONLY = 1        # Only perform this operation (skips any following ops)
    SKIP = 2        # Skip this operation
    FORCE = 3       # Always do this operation (unless previous op was 'ONLY')
    
    def __init__(self, k, srcdir=None, cc="tcc67wine", ccbindir=None, kflopdir=None, kflopname="DSPKFLOP.out"):
        self.k = k; # KMotionX object (see below) or derivative.  May contain special
                    # compilation methods: ccompile_xxx(), gen_depend_xxx().  
                    # where xxx is the compiler name (cl6x or tcc67 etc.)  The method in
                    # k is used if it exists, otherwise the method in this
                    # class is used.
        self.thd2obj = {}  # Map thread number to (object code path, source code path)
        self.kflopname = kflopname
        self.kflopdir = os.path.expanduser(kflopdir or "~/KMotionX/DSP_KFLOP")
        self.kflopoutfile = os.path.join(self.kflopdir, self.kflopname)
        self.set_cc(cc, ccbindir)
        self.set_srcdir(os.path.expanduser(srcdir or "~"))
        self.defines = []
        
    def _set_ccbindir(self, ccbindir, cc):
        if not ccbindir:
            homedir = os.path.expanduser("~")
            if cc == "cl6x":
                self.ccbindir = os.path.join(homedir, "c6000_7.4.14/bin")
            else:
                self.ccbindir = os.path.join(homedir, "KMotionX/bin")
        else:
            self.ccbindir = ccbindir
    def add_define(self, name, value):
        self.defines.append((name, value))
    def set_cc(self, cc, ccbindir=None):
        """Set the C compiler (and, implicitly, linker and other related tools).
        We support one of 'cl6x' (TI compiler), 'tcc67' (native TCC67) or 'tcc67wine'
        (original Dynomotion compiler run under wine).
        """
        self._set_ccbindir(ccbindir, cc)
        self.cc = cc
        ccmethname = 'ccompile_' + cc
        gdmethname = 'gen_depend_' + cc
        if cc == 'cl6x':
            self.cl6x = os.path.join(self.ccbindir, "cl6x")
            self.nm6x = os.path.join(self.ccbindir, "nm6x") # Needed for special linking
            self.kflopexportsyms = os.path.join(self.kflopdir, self.kflopname + ".nm")
            self.get_kflopsyms(self.kflopoutfile, self.kflopexportsyms)
        elif cc == 'tcc67' or cc == 'tcc67wine':
            self.tcc67 = os.path.join(self.ccbindir, cc)
        self.ccompile = getattr(self.k, ccmethname, None) or getattr(self, ccmethname)
        self.gen_depend = getattr(self.k, gdmethname, None) or getattr(self, gdmethname)
        
            
    def set_srcdir(self, srcdir):
        self.srcdir = srcdir
        self.incldirs = [self.kflopdir, self.srcdir]
        
    def run(self, filename, thread, compile_opts=AUTO, load_opts=AUTO, run_opts=AUTO):
        """Compile, load and run a C program in specified thread.
        By default, all steps are done automatically if required, and is the normal mode since it
        skips unnecessary steps, like a makefile.  Otherwise, one or more of the steps compile, load,
        and execute may be bypassed or forced.
        """
        outfile = self.k.ConvertToOut(thread, filename, 1000)
        if compile_opts != self.SKIP:
            try:
                do_compile = compile_opts == self.FORCE or self.need_compile(outfile, filename, thread)
            except Exception as e:
                print e
                do_compile = True
            if do_compile:
                rc, err = self.gen_depend(filename)
                if not rc:
                    rc, err = self.ccompile(outfile, filename, thread)
                self.write_ccfile(filename)
            else:
                rc = 0
            if compile_opts == self.ONLY or rc:
                if rc:
                    self.k.add_message(CompilerErrorMessage(err, rc, filename))
                return not rc
        do_load = False
        if load_opts != self.SKIP:
            try:
                do_load = load_opts == self.FORCE or self.need_load(outfile, filename, thread)
            except:
                do_load = True
            if do_load:
                #print "Loading", outfile
                rc = self.k.LoadCoff(thread, outfile)
                if rc:
                    if thread in self.thd2obj:
                        del self.thd2obj[thread]
                    err = "Could not load object file %s, rc=%d" % (outfile, rc)
                    self.k.add_message(DLLErrorMessage(err))
                    return False
                self.thd2obj[thread] = (outfile, filename, os.path.getmtime(outfile))
            #else:
                #print "Load skipped"
            if load_opts == self.ONLY:
                return True
        if run_opts != self.SKIP:
            print "Executing thread", thread
            if not do_load:
                self.k.kill(thread)
            self.k.execute(thread);
        return True
        
    def need_compile(self, outfile, filename, thread):
        """Return True or raise exception if need to compile filename to run as thread.
        """
        print "Need compile?...", filename, "->", outfile
        mtold = os.path.getmtime(outfile)
        if mtold <= os.path.getmtime(filename):
            # outfile is older, definitely need recompile
            # Also, if either does not exist, will raise exception.
            print "...yes"
            return True
        cccmd, deplist = self.get_deplist(filename)
        if cccmd != self.cc:
            print "...yes, compiler changed", self.cc, "->", cccmd
            return True         # Using a different compiler
        for dfile in deplist:
            if mtold <= os.path.getmtime(dfile):
                print "...yes,", dfile, "is more recent"
                return True     # some dependency is more recent
        print "...no"
        return False
    def get_ccfile_name(self, sourcefile_name):
        return sourcefile_name + ".dc"
    def get_depfile_name(self, sourcefile_name):
        return sourcefile_name + ".d"
    def get_deplist(self, filename):
        """Return tuple (cc, list of file names which 'filename' (source file) depends on).
        
        If there is a file of same name as filename, with a .dc extension then it
        is assumed to contain the compiler command line used last time.
        
        If there is a file of same name as filename, with a .d extension, then it 
        is assumed to contain a list of dependency filenames, so that file is
        read and the list returned.
        
        Otherwise, raise an exception, which should make the caller recompile
        the file (and generate its depfile).
        """
        ccfile = self.get_ccfile_name(filename)
        depfile = self.get_depfile_name(filename)
        with open(ccfile, "r") as f:
            cccmd = f.read()
        with open(depfile, "r") as f:
            return (cccmd, [s.strip() for s in f.readlines()])
    def need_load(self, outfile, filename, thread):
        """Return true if need to load outfile to run as thread.
        False only returned if file name is same and it has the same mtime as the currently loaded obj file.
        """
        print "Need load?...", thread, ":", self.thd2obj[thread]
        return self.thd2obj[thread][0] != outfile or self.thd2obj[thread][2] != os.path.getmtime(outfile)
        
    def runcmd(self, cmd):
        print "Running command", cmd
        try:
            rc = os.system(cmd)
            return (rc, cmd)
        except:
            return (1, "cmd abend")
            
    def write_ccfile(self, filename):
        with open(self.get_ccfile_name(filename), "w") as f:
            f.write(self.cc)
            
    def gen_depend_cl6x(self, filename):
        depfilename = self.get_depfile_name(filename)
        inclopts = ' '.join(['-i"'+x+'"' for x in self.incldirs])
        cmd = r'''%s "%s" %s --preproc_includes --output_file="%s"''' % (self.cl6x, filename, inclopts, depfilename)
        rc, err = self.runcmd(cmd)
        if rc:
            return (rc, err)
        return (0, '')

    def gen_depend_tcc67(self, filename):
        # TCC67 itself does not have options to generate dependencies, so just use gcc (it should be there!)
        depfilename = self.get_depfile_name(filename)
        inclopts = ' '.join(['-I"'+x+'"' for x in self.incldirs])
        cmd = r'''gcc -nostdinc -M %s -o "%s" "%s"''' % (inclopts, depfilename, filename)
        rc, err = self.runcmd(cmd)
        if rc:
            return (rc, err)
        # Unfortunately, gcc generates makefile dependency rules, but we just want a simple file list.
        # So munge output file and rewite appropriately.  Split file by whitespace, then discard first
        # entry (the rule target), and any backslashes (which are added to split up the long dependency
        # line).  FIXME: this will probably break if filenames include spaces, but you get what you
        # deserve if you do that.
        with open(depfilename, "r") as f:
            s = f.read()
        s = '\n'.join([x for x in s.split()[1:] if x != '\\'])
        with open(depfilename, "w") as f:
            f.write(s) 
        return (0, '')
        
    def gen_depend_tcc67wine(self, filename):
        return self.gen_depend_tcc67(filename)

    def get_kflopsyms(self, objfile, symfile):
        """Run from set_cc() to preload the dict of kflop.out exported symbols if using cl6x compiler.
        """
        cmd = '%s "%s" > "%s"' % (self.nm6x, objfile, symfile)
        try:
            if os.path.getmtime(symfile) <= os.path.getmtime(objfile):
                self.runcmd(cmd)
        except:
            self.runcmd(cmd)
        with open(symfile,"r") as f:
            self.kflopsyms = {s.split(' ')[2].strip() : s[:8] for s in f.readlines()}

    def ccompile_cl6x(self, outfile, filename, thread):
        """Invoke the TI compiler (cl6x) and link
        """
        objfilename = "/tmp/%s.o" % (os.path.basename(filename),)
        #opts = "-mv6710 -mu -ml3 -O0 --symdebug:dwarf"
        #opts = "-mv6710 -ml3 -mu -O2 --opt_for_space --entry_hook --exit_hook --entry_parm=name --exit_parm=name"
        opts = "-mv6710 -ml3 -mu -O2 --opt_for_space"
        #opts = "-mv6710 -ml3 -O2"
        inclopts = ' '.join(['-i"'+x+'"' for x in self.incldirs])
        defopts = ' '.join(['-D'+name+"="+value for name, value in self.defines])
        cmd = r'''%s "%s" %s %s --output_file="%s" %s''' % (self.cl6x, filename, inclopts, defopts, objfilename, opts)
        rc, err = self.runcmd(cmd)
        if rc:
            return (rc, err)
        
        nmfilename = "/tmp/%s.nm" % (os.path.basename(filename),)
        cmd = '%s "%s" > "%s"' % (self.nm6x, objfilename, nmfilename)
        self.runcmd(cmd)
        usyms = {}
        with open(nmfilename,"r") as f:
            for s in f.readlines():
                if s.startswith("00000000 U _"):
                    xsym = s.split(' ')[2].strip()
                    usyms[xsym] = self.kflopsyms[xsym]
        
        xsyms = '\n'.join([sym+' = 0x'+loc+';' for sym,loc in usyms.items()])
        mtype = 'THREAD_MEM'    # Or IRAM for small programs
        linkcmds = r'''
--entry_point _main
--output_file=%s
--map_file=%s.map
--ram_model
"%s"
%s
MEMORY {
IRAM: o = 0x1001c000, l = 0x00004000
THREAD_MEM: o = 0x%08X, l = 0x%08X
SDRAM: o = 0x80100000, l = 0x00f00000
}
SECTIONS {
.placeholder: palign(8), fill = 0xaaaaaaaa {. += 4;} > THREAD_MEM
.text: > %s
.far: > %s
.const: > %s
.cinit: > SDRAM
.switch: > THREAD_MEM
}
''' % (outfile, outfile, objfilename, xsyms, 0x80040000 + thread*0x10000, 0x50000 if thread==7 else 0x10000, mtype, mtype, mtype)
        linkcmdsfile = "/tmp/%s.cmd" % (os.path.basename(outfile))
        with open(linkcmdsfile, "w") as f:
            f.write(linkcmds)

        cmd = '%s --run_linker "%s"' % (self.cl6x, linkcmdsfile)
        rc, err = self.runcmd(cmd)
        if rc:
            return (rc, err)
        return (0, '')
        
    def ccompile_tcc67(self, outfile, filename, thread):
        """Invoke the native TCC67 compiler (via the DLL).  This is a bit less
        flexible than providing our own command line, but it works.
        """
        self.k.SetCustomCompiler(None)
        return self.k.Compile(filename, outfile, kmotion.BOARD_TYPE_KFLOP, thread, 1000)
        
    def ccompile_tcc67wine(self, outfile, filename, thread):
        """Invoke the Windows version TCC67 compiler under wine (via the DLL).
        """
        self.k.SetCustomCompiler("tcc67wine")
        return self.k.Compile(filename, outfile, kmotion.BOARD_TYPE_KFLOP, thread, 1000)
        

class KMotionX(kmotion.KMotion):
    """
    Wrapper class for 'raw' KMotion.
    
    Adds thread manager for more intelligent handling of threads, different compilers etc.

    Naming convention:
      UI options provide
         incldirs : -I header location directory list
         objdir : object, executable and temporary file directory
         kmotionobjpath : KMotion .out file path
      xxx.c ->[compile] xxx.o
    """
    def __init__(self, dev, hostname = None, port = kmotion.KMOTION_PORT, with_console=False):
        if hostname is None:
            super(KMotionX, self).__init__(dev)
        else:
            super(KMotionX, self).__init__(dev, port, hostname)
        self.firstpoll = 0xFFFFFFFF
        self.pccmd_handlers = {
            kmotion.PC_COMM_ESTOP: self.handle_estop,
            kmotion.PC_COMM_MSG: self.handle_msgbox,
            }
        # Initially assume 1:1 mapping.  axis_map[0] is the kflop axis for X etc.
        # Note that this class is responsible for mapping incoming "raw" axis data to
        # interpreter-relevant axes.
        self.axis_map = list(range(8))
        self.clear_messages()
        if with_console:
            self.SetConsoleCallback()
        self.tmgr = ThreadManager(self, cc="tcc67wine", srcdir="~/DM6/DM6-SCL-Rev01")
        self.set_comm_result(-2)
    # Overrides of virtual methods in superclass.  These may be called from
    # different thread (e.g. g-code interpreter).
    def HandleConsole(self, line):
        #print "Console:", line,
        self.add_message(ConsoleMessage(line))
    def HandleErrMsg(self, line):
        #print "Err:", line
        self.add_message(DLLErrorMessage(line))
        

    # Since we are not polling, this will not be called from self.Poll(),
    # but instead is used from the ui object to delegate its poll results.
    def ChangedPCComm(self, commvec):
        cmd = commvec[0]
        if self.firstpoll & kmotion.POLL_PCCOMM:
            self.firstpoll &= ~kmotion.POLL_PCCOMM
            # Error if kflop has pending command first time
            if cmd:
                self.set_comm_result(-2)
        elif cmd > 0:
            handler = self.pccmd_handlers.get(cmd)
            if handler:
                self.set_comm_result(handler(commvec) or 0)
                
    # PC command handlers (should return None or 0 if OK, -1 for error)
    def set_comm_result(self, result):
        self.WriteLine("SetPersistDec%d %d" % (kmotion.PC_COMM_PERSIST, result))
    def handle_estop(self, cmdvec):
        self.add_message(DebugMessage("kflop sent estop"))
    def handle_msgbox(self, cmdvec):
        self.add_message(DebugMessage("kflop sent msgbox"+str(cmdvec)))
            
    # App functionality
    def clear_messages(self):
        self.msgqueue = []  # List of Message objects from DLL or interpreter
    def get_message(self):
        return self.msgqueue.pop(0) if len(self.msgqueue) else None
    def add_message(self, m):
        self.msgqueue.append(m)
        
    def update_axis_map(self):
        # Should be called after reset etc., when the kflop has run DefineCS.
        rc, r = self.WriteLineReadLine("definecs")
        if not rc:
            try:
                self.axis_map = [int(x) for x in r.split(' ')]
            except:
                self.add_message(InternalErrorMessage("Bad DefineCS result '%s'" % r))
                return 1
        return rc
    
    def kill(self, thread):
        self.WriteLine("Kill%d" % thread);
    def execute(self, thread):
        self.WriteLine("Execute%d" % thread);
    def run_c(self, filename, thread):
        return self.tmgr.run(filename, thread)
    def wait_c(self, thread, idle_func=None, sleep_time=0.05):
        while not self.thread_done(thread):
            if idle_func and idle_func(thread):
                return
            time.sleep(sleep_time)
    def thread_done(self, thread):
        rc, r = self.WriteLineReadLine("CheckThread%d" % thread)
        if not rc:
            return r == "0"
        return -1
       
        


class Interpreter(kmotion.GCodeInterpreter):
    def __init__(self, k, ui, cm=None):
        """Create G-code interpreter.
        Parameters:
          k -- kmotion instance
          ui -- user interface object (see below for required interface)
          cm -- CoordinatedMotion instance; by default one will be created here.
        ui interface ('self' parameter of these methods omitted for clarity):
          handle_event(evt_code, parameter) - general interpreter/kmotion event handler.
          handle_running(bool) - handle run start (True) or complete (False).
          m<digits>() -- M-code handler for M2..9, M100..119.
          set_spindle_speed() -- S word handler
          user<digits>() -- user button handler for buttons 1..10.  These probably won't be
            called, since ui handles all user interface itself.
          popup_remote(opts, msg) -- display remote action dialog (delegated from PC_MsgBox)
          done_remote() -- check whether remote action dialog completed (>0 = response code)
          cancel_remote() -- pop down remote action dialog
        """
        cm = kmotion.CoordMotion(k) if cm is None else cm
        super(Interpreter, self).__init__(cm)
        self.ui = ui
        self.cm = cm
        self.k = k
        self.finished = True
        self.CurrentLineNo = 0
        self.ErrorLineNo = 0
        self.exitcode = 0
        self.setup_handlers()
        
    def setup_handlers(self):
        # Handle M-codes (and other codes) with callback.  The int "acode" number passed from interpreter is mapped as follows:
        # 0-9 = M0..M9 = 0..kmotion.MAX_MCODE_ACTIONS_M1-2
        # 10 = S (spindle speed) = kmotion.MAX_MCODE_ACTIONS_M1-1
        # 11-20 = User buttons 1..10 = kmotion.MAX_MCODE_ACTIONS_M1..kmotion.MAX_MCODE_ACTIONS_M1+kmotion.MAX_MCODE_ACTIONS_BUTTONS-1
        # 21-40 = M100..M119 = kmotion.MAX_MCODE_ACTIONS_M1+kmotion.MAX_MCODE_ACTIONS_BUTTONS.. *+MAX_MCODE_ACTIONS_M100-1
        self.hdict = {}
        for acode in range(0,kmotion.MAX_MCODE_ACTIONS):
            if acode < kmotion.MAX_MCODE_ACTIONS_M1-1:
                mname = "m%d" % acode
            elif acode == kmotion.MAX_MCODE_ACTIONS_M1-1:
                mname = "set_spindle_speed"
            elif acode < kmotion.MAX_MCODE_ACTIONS_M1+kmotion.MAX_MCODE_ACTIONS_BUTTONS:
                mname = "user%d" % (acode - kmotion.MAX_MCODE_ACTIONS_M1 + 1)
            else:
                mname = "m%d" % (acode - kmotion.MAX_MCODE_ACTIONS_M1+kmotion.MAX_MCODE_ACTIONS_BUTTONS + 100)
            cb = getattr(self, mname, None) or getattr(self.ui, mname, None)
            if not cb:
                continue
            #print "Setting acode", acode, "callback to", mname
            self.SetActionCallback(acode);
            self.hdict[acode] = cb

    # Overrides of virtual methods in superclass...
    # Note that the Handle* (except HandleEvent) methods are called from a separate thread.  This is OK for
    # Python, but be cautious about calling external API that may not be thread-safe.
    def HandleStatus(self, lineno, msg):
	    self.CurrentLineNo=lineno;
	    if msg:
	        self.k.add_message(Message(lineno, msg))
    def HandleComplete(self, status, lineno, sequence_number, err):
	    self.ErrorLineNo=lineno
	    self.ErrorMsg=err
	    self.exitcode=status
	    self.finished=True;
	    self.ui.handle_running(False)
	    if status:
	        self.k.add_message(ErrorMessage(lineno, err, status))
	    else:
	        self.k.add_message(Message(lineno, "complete"))
	    #print "-k-", self.cm.WaitForMoveXYZABCFinished()
    def HandleUser(self, msg):
        print "User callback:", msg
        return 0
    def HandleMCode(self, acode):
        print "Handle acode", acode
        try:
            return self.hdict.get(acode, self.no_op)() or 0
        except Exception as e:
            print e
            return 1
    def no_op(self):
        return 0
        
    def PC_MsgBox(self):
        msg = self.GetLastGatherString()
        opts = self.GetPCOptions()
        try:
            self.ui.popup_remote(opts, msg)
            return 0
        except:
            return -1
    def PC_Running_MsgBox(self):
        try:
            rc = self.ui.done_remote()
            if rc > 0:
                self.SetPCResult(rc);
                return 0
            elif rc < 0:
                return -1
            return 1
        except:
            return -1
    def PC_Cancel_MsgBox(self):
        print "MsgBox: cancelled by kflop"
        try:
            self.ui.cancel_remote()
        except:
            pass
        
    def HandleFloatEvt(self, evt_code, newval, oldval):
        """Invoked when Poll() is called (e.g. from main app idle processing).
        This runs in same thread as main app.  It is called for each event
        for which interest was registered using EnableEvent().
        Note: we have a callback like this for bool, int and float because
        of the difficulty getting swig to create polymorphic parameters when
        calling from C/C++.
        Note: for this app, we ignore the old values.  New value events are
        delegated to the ui.
        """
        self.ui.handle_event(evt_code, newval)
    def HandleIntEvt(self, evt_code, newval, oldval):
        self.ui.handle_event(evt_code, newval)
    def HandleBoolEvt(self, evt_code, newval):
        self.ui.handle_event(evt_code, newval)

    # Additional interpreter API for our app...
    def get_line(self):
        return self.CurrentLineNo
    def is_finished(self):
        return self.finished
    def get_error_line(self):
        return self.ErrorLineNo
    def get_exit_code(self):
        return self.exitcode

    def ready_start(self):
        self.CurrentLineNo = 0
        self.ErrorLineNo = 0
        self.exitcode = 0
        #self.k.clear_messages()
    def ready_run(self):
        self.finished = False
        self.ui.handle_running(True)
        #self.CoordMotion.ClearAbort()
        #self.CoordMotion.ClearHalt() <- Interpret() does these always
    def rewind(self):
        self.ready_start()
    def run_from_start(self, filename):
        self.ready_start()
        self.ready_run()
        self.Interpret(kmotion.BOARD_TYPE_KFLOP, filename, 0, -1, True)
    def run_continue(self, filename):
        self.ready_run()
        self.Interpret(kmotion.BOARD_TYPE_KFLOP, filename, self.CurrentLineNo, -1, False)
    def run_step(self, filename):
        self.ready_run()
        self.Interpret(kmotion.BOARD_TYPE_KFLOP, filename, self.CurrentLineNo, self.CurrentLineNo, False)



