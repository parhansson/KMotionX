"""
Extra utilities for kmotion.py
"""

import os, time, threading, sys, traceback

if sys.platform == 'win32':
    from . import _menigcnc_root, _menigcnc_version
    from . import kmotion
else:
    #FIXME: modularize this!
    _menigcnc_root = os.path.abspath(os.path.join(os.environ['KMOTION_BIN'], ".."))
    _menigcnc_version = "v2"
    import kmotion




class Message(object):
    def __init__(self, lineno, msg, strip=True):
        self.lineno = lineno
        self.msg = msg.strip() if strip else msg
    def get_type(self):
        return "status"
    def get_log_prefix(self):
        return "E"
    def get_severity(self):
        return "I"
    def is_fatal(self):
        return False
    def is_popup(self):
        return False
    def get_line(self):
        return self.lineno
    def show_line(self):
        return False
    def show_on_status(self):
        return False
    def get_msg_color(self):
        return "black"
    def is_valid_line(self):
        return self.lineno > 0
    def get_msg(self):
        return self.msg
    def get_code(self):
        return 0
    def get_filename(self):
        return None
    def get_kflop_thread(self):
        return None
    def __str__(self):
        if self.is_valid_line():
            return self.get_type()+":"+str(self.get_line())+": "+self.get_msg()
        return self.get_type()+": "+self.get_msg()
        
class ConsoleMessage(Message):
    def __init__(self, msg):
        super(ConsoleMessage, self).__init__(-1, msg, strip=False)
    def get_type(self):
        return "console"
    def get_log_prefix(self):
        return "k"
    def get_severity(self):
        return "I"

class StatusMessage(Message):   # from pc cmd 50
    clrmap = { '!' : "red", '$' : "black", '#' : "#AA6600", '?' : "#99bb33" }
    def __init__(self, msg, kflop_thread=None):
        super(StatusMessage, self).__init__(-1, msg, strip=False)
        self.kflop_thread = kflop_thread
    def get_type(self):
        return "status"
    def get_log_prefix(self):
        return "s"
    def get_severity(self):
        return "I"
    def show_on_status(self):
        return True
    def get_msg_color(self):
        try:
            return self.clrmap[self.msg[0]]
        except:
            return "black"
    def get_kflop_thread(self):
        """Return message-originating thread on kflop (1-7), or 0 if unknown (old protocol)
        or None if locally generated.
        """
        return self.kflop_thread
    def get_msg(self):
        return self.msg[1:]

class StatusClearMessage(StatusMessage):
    def __init__(self, kflop_thread=None):
        super(StatusClearMessage, self).__init__('')
    def get_type(self):
        return "status clear"

class DebugMessage(Message):
    def __init__(self, msg):
        super(DebugMessage, self).__init__(-1, msg)
    def get_type(self):
        return "debug"
    def get_log_prefix(self):
        return "d"
    def get_severity(self):
        return "D"

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
    def get_log_prefix(self):
        return "F"
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
    def get_msg_color(self):
        return "red"

class PopupMessage(Message):
    def __init__(self, msg, title, opts, kflop_thread=None, result_var=None):
        super(PopupMessage, self).__init__(-1, msg)
        self.title = title
        self.opts = opts
        self.kflop_thread = kflop_thread
        self.result_var = result_var
    def get_type(self):
        return "popup"
    def get_log_prefix(self):
        return "d"
    def is_popup(self):
        return True
    def get_title(self):
        return self.title
    def get_opts(self):
        return self.opts
    def get_kflop_thread(self):
        """Return message-originating thread on kflop (1-7), or 0 if unknown (old protocol)
        or None if locally generated.
        """
        return self.kflop_thread
    def get_result_var(self):
        """Return final result persist var number (1-199) or None if n/a.
        """
        return self.result_var


class TaskStatusMessage(Message):
    DONE = 0        # Task completed successfully
    PROGRESS = 1    # Task progress report
    ERROR = 2       # Task failed
    def __init__(self, status=DONE, result=0, from_task=None):
        """Message sent from Task object to GUI indicating the task status.
        status is one of the above constants
        result indicates the result code or other detail.
        from_task is originating Task object
        """
        super(TaskStatusMessage, self).__init__(-1, (status,result,from_task), strip=False)
    def get_type(self):
        return "task status"
    def get_log_prefix(self):
        return "t"
    def get_status(self):
        return self.msg[0]
    def get_result(self):
        return self.msg[1]
    def get_task(self):
        return self.msg[2]


class Task(threading.Thread):
    def __init__(self, msgqueue, group=None, target=None, name=None, args=(), kwargs={}):
        """Task class for threads that can notify the parent via a message queue.
        msgqueue -- object with an append(msg) method.  msg will be a TaskStatusMessage object.
        gen -- optional generator which will be passed as arg in the TaskStatusMessage.
             When the parent thread receives the message, it will call next(gen) to continue
             a sequence of steps.
        """
        def wrapper(*args, **kwargs):
            """Decorate given target callable so that it sends DONE or ERROR message at end.
            This is not strictly necessary, but is convenient for parent thread since it knows it
            can join() without a long timeout.
            Also, the target function is passed self (this Task) as first parameter.  Thus,
            target functions should at least have a signature like tgt(task).
            """
            try:
                rc = target(self, *args, **kwargs)
                print "task", self.name, "ran ok, rc=", rc
                self.result = rc
                self.done = True
                self.msgqueue.append(TaskStatusMessage(TaskStatusMessage.DONE, rc, self))
            except Exception as e:
                print "task", self.name, "abend"
                self.result = traceback.format_exc(32)
                self.err_result = True
                self.done = True
                self.msgqueue.append(TaskStatusMessage(TaskStatusMessage.ERROR, e, self))
        super(Task, self).__init__(group=group, target=wrapper, name=name, args=args, kwargs=kwargs)
        self.msgqueue = msgqueue
        self.gen = None
        self.progress = None
        self.result = None
        self.err_result = False
        self.done = False
        self.can_start = True
    def set_gen(self, gen):
        self.gen = gen
    def send_progress(self, data):
        self.progress = data
        self.msgqueue.append(TaskStatusMessage(TaskStatusMessage.PROGRESS, data, self))
    def start(self):
        if self.can_start:
            self.can_start = False
            super(Task,self).start()
        


class Tool(object):
    """Class to represent tools in tool table
    """
    UNITS_UNSPECIFIED = 'none'   # Traditional: units are whatever the interpreter mode is at time of use (g20/21).  This is classic
                            #   RS485 NGC, but is not recommended except to maintain bug-for-bug compatibility.
    UNITS_INCH = 'inch'          # Fixed imperial or metric: interpreter tool table is recalculated when switching between g20/21.
    UNITS_MM = 'mm'            #   Note that all dimensions (length, diam, etc.) in the Tool entry are assumed to have the same units.
    column_names = ('Slot', 'ID', 'Length', 'Diam', 'X-offs', 'Y-offs', 'Units', 'Comment', 'Image', 'Shank', 'Tip-rad')
    column_types = (int, int, float, float, float, float, str, str, str, float, float)
    def __init__(self, slot=0, id=0, length=0., diam=0., xoffset=0., yoffset=0., units='none', comment="", image="", shank_diam=0., tip_radius=0.):
        self.set_slot(slot)
        self.set_id(id)
        self.set_length(length)
        self.set_diam(diam)
        self.set_xoffset(xoffset)
        self.set_yoffset(yoffset)
        self.set_units(units)
        self.set_comment(comment)
        self.set_image(image)
        self.set_shank_diam(shank_diam)
        self.set_tip_radius(tip_radius) # This allows controlled point to be adjusted to within (+) or beyond (-) the tool tip as measured by the THS.
                                        # When the THS is measured on the machine, the returned length offset (stored in length field) will be adjusted
                                        # according to this value.  It is thus used only by the machine, not the interpreter.  The machine needs to
                                        # explicitly query the tip_radius value, otherwise it is not used.
    def get_as_row(self, metric=None):
        if metric is None:
            return [self.slot, self.id, self.length, self.diam, self.xoffset, self.yoffset, \
                    self.units, self.comment, self.image, self.shank_diam, self.tip_radius]
        elif metric:
            return self.get_as_row_metric()
        else:
            return self.get_as_row_imperial()
    def get_as_row_metric(self):
        # Note that we omit the 'units' field...  This makes it compatible with legacy file format.
        conv = 25.4 if self.units=='inch' else 1.
        return [self.slot, self.id, self.length*conv, self.diam*conv, self.xoffset*conv, self.yoffset*conv, \
                self.comment, self.image, self.shank_diam*conv, self.tip_radius*conv]
    def get_as_row_imperial(self):
        conv = 1./25.4 if self.units=='mm' else 1.
        return [self.slot, self.id, self.length*conv, self.diam*conv, self.xoffset*conv, self.yoffset*conv, \
                self.comment, self.image, self.shank_diam*conv, self.tip_radius*conv]
    def set_slot(self, s):
        v = int(s)
        if v < 0 or v > 99:
            raise ValueError("Tool slot must be in range 0..99")
        self.slot = v
    def set_id(self, s):
        v = int(s)
        if v < 0 or v > 9999:
            raise ValueError("Tool id must be in range 0..9999")
        self.id = v
    def set_length(self, s):
        if isinstance(s, str):
            v = float(s)
        else: v = s
        self.length = v
    def set_diam(self, s):
        if isinstance(s, str):
            v = float(s)
        else: v = s
        self.diam = v
    def set_xoffset(self, s):
        if isinstance(s, str):
            v = float(s)
        else: v = s
        self.xoffset = v
    def set_yoffset(self, s):
        if isinstance(s, str):
            v = float(s)
        else: v = s
        self.yoffset = v
    def set_units(self, s):
        s = s.lower()
        if s not in ('none','inch','mm','','in'):
            raise ValueError("Tool units invalid")
        if s == '':
            s = 'none'
        elif s == 'in':
            s = 'inch'
        self.units = s
    def set_comment(self, s):
        self.comment = s
    def set_image(self, s):
        self.image = s
    def set_shank_diam(self, s):
        if isinstance(s, str):
            v = float(s)
        else: v = s
        self.shank_diam = v
    def set_tip_radius(self, s):
        if isinstance(s, str):
            v = float(s)
        else: v = s
        self.tip_radius = v
    def set_column(self, col, s):
        f = (self.set_slot, self.set_id, self.set_length, self.set_diam, self.set_xoffset, self.set_yoffset, \
             self.set_units, self.set_comment, self.set_image, self.set_shank_diam, self.set_tip_radius)[col]
        f(s)
        
def new_tool_from_row(row):
    return Tool(*tuple(row))
    
    
class KMotionX_NoKFlop(object):
    """This is a dummy KMotion class for use without actually connecting to a kflop.
    Used just for the thread manager, when precompiling C source files etc.
    Helps build .out file distros.
    
    Aditionally, this defines the minimum set of methods required for the 'k'
    parameter when creating a ThreadManager.
    """
    def __init__(self, **tmgr_args):
        self.tmgr = ThreadManager(self, **tmgr_args)
    def get_tmgr(self):
        # Not really part of the interface, but convenient for getting the ThreadManager.
        return self.tmgr
        
    def ConvertToOut(self, thread, filename, n):
        return filename[:-2] + '(' + str(thread) + ').out'
    def add_message(self, s):
        print s
    def ExtractCoffVersionString(self, filename):
        #FIXME: need to implement this without the DLL.  When standalone compiling,
        # its use can be avoided by explicitly specifying a target firmware version
        # and firmware .out file.
        raise NotImplementedError
    def LoadCoff(self, thread, outfile):
        # Can avoid calling this by not specifying load or exec to ThreadManager.run()
        raise NotImplementedError
    def kill(self, thread):
        # Can avoid calling this by not specifying load or exec to ThreadManager.run()
        raise NotImplementedError
    def execute(self, thread):
        # Can avoid calling this by not specifying load or exec to ThreadManager.run()
        raise NotImplementedError
    def SetCustomCompiler(self, comp, opts):
        # Can avoid by using only cl6x for compilation.  tcc67 uses the kmotion DLL.
        raise NotImplementedError
    def Compile(self, filename, outfile, boardtype, thread, maxmsg):
        # Can avoid by using only cl6x for compilation.  tcc67 uses the kmotion DLL.
        raise NotImplementedError


class ThreadManager(object):
    """Class to manage kflop thread execution, including compile, link and load.  
    Manages kflop firmware versions.
    
    Firmware versions are specified by a string key:
    '' - unknown version e.g. because using old style dspkflop.out file name.  This
             is treated as the 'oldest' version.
    '433m' - version extracted from .out file name e.g. dspkflop-433m.out
             or can be parsed from Version string returned from kflop, or
             by using kmotion.ExtractCoffVersionString() on a firmware file.
    None - not known.  This will often cause exceptions if the version needs to be known.
    """
    # Options for compile and/or load and/or run (in that sequence)...
    AUTO = 0        # Automatically perform operation if required
    ONLY = 1        # Only perform this operation (skips any following ops)
    SKIP = 2        # Skip this operation
    FORCE = 3       # Always do this operation (unless previous op was 'ONLY')
    
    COMPILED = 1
    LOADED = 2
    EXECUTED = 3
    
    def __init__(self, k, srcdir=None, cc="tcc67", ccbindir=None, kflopdir=None, kflopname=None, verbose=False, target_firmware_version=None):
        self.verbose = verbose
        self.k = k; # KMotionX object (see below) or derivative.  May contain special
                    # compilation methods: ccompile_xxx(), gen_depend_xxx().  
                    # where xxx is the compiler name (cl6x or tcc67 etc.)  The method in
                    # k is used if it exists, otherwise the method in this
                    # class is used.
        self.thd2obj = {}  # Map thread number to (object code path, source code path)
        self.kflopname = None
        self.set_kflop(kflopdir, kflopname, target_firmware_version, False)
        self.set_cc(cc, ccbindir)
        self.set_srcdir(os.path.expanduser(srcdir or "~"))
        self.defines = []
        self.opts = None
    def invalidate_thread_cache(self):
        """Forget the thread code which is currently loaded in the kflop.  This should
        be called if the kflop threads become invalidated because of a reboot or
        reflashing the firmware.
        """
        self.thd2obj = {}
    def parse_firmware_version(self, version_str):
        """version_str is string from "version" script command.  Something like:
          KFLOP 4.33m Build 16:29:35 Jun 26 2015
        Extract the 4.33m, lowercase, and remove any punctuation e.g. 433m.  We then assume
        a lexicographically increasing sequence for successive versions.
        Also extract the build date e.g. "16:29:35 Jun 26 2015" and reorder
        so it will sort properly too.
        Return tuple e.g. ("433m", "2015/06/26 16:29:35").
        It's wise to wrap this in a try/except just in case the kflop comms or firmware
        file are garbled.
        """
        x = version_str.split(' ')
        ver = x[1].lower().translate(None, '.,():[]{}')
        t = x[3].split(':')
        t = "%02d:%02d:%02d" % (int(t[0]),int(t[1]),int(t[2]),)
        mm = {'jan':1,'feb':2,'mar':3,'apr':4,'may':5,'jun':6,'jul':7,'aug':8,'sep':9,'oct':10,'nov':11,'dec':12,}[x[4].lower()]
        dd = int(x[5])
        yyyy = int(x[6])
        d = "%04d/%02d/%02d" % (yyyy, mm, dd)
        return (ver, d+' '+t)
    def _fv_from_outfilename(self, filename):
        """Return firmware version from a firmware file.
        If filename is absolute, then the file may be opened to read the version symbol.
        Filename's basename must start with 'dspkflop' and end with '.out' (case insensitive).
        If the filename is relative, then the version is guessed by looking at the name
        for a version identifier e.g. dspkflop-433M.out will be '433m'.  If the name is
        just dspkflop.out then a zero length string will be returned, so ideally absolute
        paths should be provided so that the version can be definitively otained.
        """
        nx = os.path.splitext(filename)
        if nx[1].lower() != ".out":
            return None # not a .out file
        name = os.path.basename(nx[0]).lower()
        if not name.startswith('dspkflop'):
            return None
        if os.path.isabs(filename):
            rc, fvs = self.k.ExtractCoffVersionString(filename)
            if rc==0:
                try:
                    fv, datecode = self.parse_firmware_version(fvs)
                    return fv
                except:
                    pass
            print "Warning: trouble extracting version info from", filename
        if name == 'dspkflop':
            return ''   # unversioned
        elif name.startswith('dspkflop-'):
            return name[9:].lower() # should be e.g. '433m'
        return None # not a dspkflop file
    def _get_firmware_files(self, lst, dirname, fnames):
        for filename in fnames:
            absfile = os.path.join(dirname, filename)
            fv = self._fv_from_outfilename(absfile)
            if fv is not None:
                lst.append((fv, absfile))
        fnames[:] = []  # No recursion
    def _find_fw_list(self, dirname):
        lst = []
        os.path.walk(dirname, self._get_firmware_files, lst)
        return lst
    def get_latest_kflop_version(self, kfdir=None):
        lst = self._find_fw_list(kfdir or self.kflopdir)
        try:
            return max(lst)
        except ValueError:
            return (None, None)
    def get_kflop_version(self, kfdir, fv):
        lst = self._find_fw_list(kfdir or self.kflopdir)
        #print lst
        for l in lst:
            if l[0] == fv:
                return l
        return (None, None)
    def is_latest_kflop_version(self, kfdir, fv):
        lv, latest = self.get_latest_kflop_version(kfdir)
        if latest is None:
            return True
        return lv <= self.target_firmware_version
    def get_firmware_filename(self):
        return self.kflopoutfile
    def set_kflop(self, kflopdir=None, kflopname=None, fv=None, extract_syms=True):
        """Set kflop firmware parameters:
        kflopdir -- firmware directory (default <_menigcnc_root>/DSP_KFLOP)
        kflopname -- specific .out file name e.g. DSPKFLOP-433m.out, or
                    defaults to latest firmware version available in
                    kflopdir if fv not specified, or else specific
                    version if fv specified.
        fv -- target firmware version.  Default is latest in kflopdir,
              otherwise looks for specific version.  Ignored if
              kflopname is specified.
        extract_syms -- if true (default), then extract symbols for special
              link process.  This should only be false if the caller will
              immediately call set_cc() with the cl6x compiler next.
              
        This should be called when a connection is made, and the attached kflop's
        version becomes known.  This will ensure compiled code matches the
        target version (else may crash!)
        
        If being used without a kflop attached, then fv should be set to the
        expected target version.  Note that the target version is stored in
        the .dc file so that the .out files can be checked that they are
        compiled correctly.  The .dc and .out files should be kept together
        in the same directory.
        FIXME: requiring the .c.dc file is error-prone.  Intention is to change
        the naming convention for .out files to include the target version
        e.g. foomatic(1-433p).out i.e. in the parens, 1st digit is the target
        thread, then all after the dash is the condensed version id.
        """
        self.kflopdir = os.path.expanduser(kflopdir or os.path.join(_menigcnc_root, "DSP_KFLOP"))
        if kflopname is None:
            if fv is None:
                # automatically find latest firmware if fv is None
                _, latest = self.get_latest_kflop_version(self.kflopdir)
                if latest is not None:
                    kflopname = os.path.basename(latest)
            else:
                # else find specific version, to match a connected kflop
                _, specific = self.get_kflop_version(self.kflopdir, fv)
                #print "specific=", specific
                if specific is not None:
                    kflopname = os.path.basename(specific)
                #print "kflopname=", kflopname
        if kflopname is None:
            if self.verbose:
                print "kflopdir=", self.kflopdir
                print "target firmware version=", fv
            raise ValueError("Failed to locate a firmware binary")
        if kflopname != self.kflopname:
            self.invalidate_thread_cache()
        self.kflopname = kflopname
        self.kflopoutfile = os.path.join(self.kflopdir, self.kflopname)
        self.target_firmware_version = self.get_firmware_version_from_filename(self.kflopoutfile)
        if self.verbose: print "Set target firmware:", self.kflopoutfile
        if extract_syms:
            # use nm to extract symbols...
            self.set_cc(self.cc, self.ccbindir)
    def set_target_firmware_version(self, fv):
        """Called when attached kflop version is discovered.
        This delegates to set_kflop() without changing the firmware directory.  It will
        try to locate a matching .out file.  If none can be found, raises ValueError.
        """
        self.set_kflop(self.kflopdir, None, fv)
    def get_source_and_thread_from_outfilename(self, outfilename):
        """Given out file name (e.g. 'foo(6).out') return the source file name (e.g. 'foo.c')
        and thread number (e.g. 6).  May be absolute or relative path.
        Returns tuple (sourcefilename, thread), or None if filename does not match *(n).out
        where n is 1..7.
        """
        if len(outfilename) < 7: return None
        if not outfilename.lower().endswith(').out') or outfilename[-7] != '(' or \
            outfilename[-6] not in ['1','2','3','4','5','6','7']:
            return None
        return (outfilename[:-7]+'.c', int(outfilename[-6]))
    def get_firmware_version_from_filename(self, filename):
        """Compute target version based on the file name (if it is a dspkflop firmware file)
        or look in the .dc file for target version of compiled source code.
        filename -- for firmware, may be a relative name since it only looks at the name,
                    however a better result is obtained if an absolute path is provided
                    since then it can check for sure.
                    For source code, an absolute path should be given since it needs to
                    look inside the matching .dc file.
                    Should be a xxx.c or a xxx(n).out file for source files.
        """
        fv = self._fv_from_outfilename(filename)
        if fv is not None:
            return fv
        else:
            # Presumably a source file.  In this case, look for ccfile (.dc extension) in same place,
            # and pull the last target firmware out of that.  Since filename may be an out file,
            # turn it into the original .c file name
            sfo = self.get_source_and_thread_from_outfilename(filename)
            if sfo is not None:
                filename = sfo[0]
            try:
                ccfile = self.get_ccfile_name(filename)
                with open(ccfile, "r") as f:
                    x = f.read().split('\n')
                    cccmd = x[0]
                    fv = x[1] if len(x)>1 else ''
                return fv
            except:
                pass
        return ''
    def get_ccfile_name_from_outfilename(self, outfilename):
        sfo = self.get_source_and_thread_from_outfilename(outfilename)
        if sfo is not None:
            filename = sfo[0]
        else:
            # allow it to be a .c file as well
            filename = outfilename
        try:
            return self.get_ccfile_name(filename)
        except:
            return None
        
        
    def _set_ccbindir(self, ccbindir, cc):
        if not ccbindir:
            if cc == "cl6x":
                # Linux development only...
                homedir = os.path.expanduser("~")
                self.ccbindir = os.path.join(homedir, "c6000_7.4.14/bin")
            else:
                self.ccbindir = os.path.join(_menigcnc_root, "bin")
        else:
            self.ccbindir = ccbindir
    def add_define(self, name, value):
        self.defines.append((name, value))
    def set_defines(self, name_value_dict):
        self.defines = [(n, v) for n, v in name_value_dict.items()]
    def add_incldir(self, dirname):
        self.incldirs.append(dirname)
    def add_incldirs(self, dirnames):
        self.incldirs.extend(dirnames)
    def set_incldirs(self, dirnames):
        self.incldirs = [self.kflopdir, self.srcdir]
        self.incldirs.extend(dirnames)
    def set_opts(self, opts):
        self.opts = opts
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
            self.kflopexportsyms = self.kflopoutfile + ".nm"
            self.get_kflopsyms(self.kflopoutfile, self.kflopexportsyms)
        elif cc == 'tcc67' or cc == 'tcc67wine':
            self.tcc67 = os.path.join(self.ccbindir, cc)
        self.ccompile = getattr(self.k, ccmethname, None) or getattr(self, ccmethname)
        self.gen_depend = getattr(self.k, gdmethname, None) or getattr(self, gdmethname)
        
            
    def set_srcdir(self, srcdir):
        self.srcdir = srcdir
        self.incldirs = [self.kflopdir, self.srcdir]
        
    def run(self, filename, thread, compile_opts=AUTO, load_opts=AUTO, run_opts=AUTO, actions_done=[]):
        """Compile, load and run a C program in specified thread.
        By default, all steps are done automatically if required, and is the normal mode since it
        skips unnecessary steps, like a makefile.  Otherwise, one or more of the steps compile, load,
        and execute may be bypassed or forced.
        
        Note that it is permitted to have only a .out file with no source.  In this case, any
        compilation is skipped (since it can't be rebuilt).  Useful for 'object code only'
        such as the supervisor.
        
        actions_done is a list (mutable) which, on return, will contain zero or more of integers:
          1 (ThreadManager.COMPILED): .c successfully compiled into .out
          2 (ThreadManager.LOADED): .out successfully loaded to kflop
          3 (ThreadManager.EXECUTED): thread executed on kflop
        The value will not be set if the action was skipped because it was not necessary, or if the
        action failed.
        """
        actions_done[:] = []
        if filename.lower().endswith(".out"):
            # If outfile specified, cannot compile, and must force load.
            outfile = filename
            compile_opts = self.SKIP
            load_opts = self.FORCE
        else:
            outfile = self.k.ConvertToOut(thread, filename, 1000)
        if compile_opts != self.SKIP:
            #print "test compile:"
            #print "outfile=", outfile, os.path.exists(outfile)
            #print "filename=", filename, os.path.exists(filename)
            if os.path.exists(outfile) and not os.path.exists(filename):
                fv = self.get_firmware_version_from_filename(filename)
                tfv = self.target_firmware_version
                if fv and fv != tfv:
                    err = "Binary file version %s not compatible with target firmware version %s:\n  %s" \
                            % (fv, tfv if tfv else '<unversioned>', filename)
                    self.k.add_message(CompilerErrorMessage(err, -1, filename))
                rc = 0
                if self.verbose: print "Skipping compile, object only"
            else:
                do_compile = compile_opts == self.FORCE or self.need_compile(outfile, filename, thread)
                if do_compile:
                    rc, err = self.gen_depend(filename)
                    if not rc:
                        rc, err = self.ccompile(outfile, filename, thread)
                        if not rc:
                            actions_done.append(self.COMPILED)
                            self.write_ccfile(filename)
                    else:
                        print "gen_depend failure, rc=",rc,":"
                        print err
                else:
                    rc = 0
            if compile_opts == self.ONLY or rc:
                if rc:
                    self.k.add_message(CompilerErrorMessage(err, rc, filename))
                return not rc
        do_load = False
        if load_opts != self.SKIP:
            do_load = load_opts == self.FORCE or self.need_load(outfile, filename, thread)
            if do_load:
                #print "Loading", outfile
                rc = self.k.LoadCoff(thread, outfile)
                if rc:
                    if thread in self.thd2obj:
                        del self.thd2obj[thread]
                    err = "Could not load object file %s, rc=%d" % (outfile, rc)
                    self.k.add_message(DLLErrorMessage(err))
                    return False
                actions_done.append(self.LOADED)
                self.thd2obj[thread] = (outfile, filename, os.path.getmtime(outfile))
            #else:
                #print "Load skipped"
            if load_opts == self.ONLY:
                return True
        if run_opts != self.SKIP:
            if self.verbose: print "Executing thread", thread
            if not do_load:
                self.k.kill(thread)
            self.k.execute(thread);
            actions_done.append(self.EXECUTED)
        return True
        
    def need_compile(self, outfile, filename, thread):
        """Return True if need to compile filename to run as thread.
        """
        if self.verbose: print "Need compile?...", filename, "->", outfile
        try:
            mtold = os.path.getmtime(outfile)
            if mtold <= os.path.getmtime(filename):
                # outfile is older, definitely need recompile
                # Also, if either does not exist, will raise exception.
                if self.verbose: print "...yes"
                return True
        except:
            if self.verbose: print "...yes, out file non-existent"
            return True
        try:
            cccmd, deplist, fv = self.get_deplist(filename)
        except:
            if self.verbose: print "...yes, no dependency file"
            return True
        if cccmd != self.cc:
            if self.verbose: print "...yes, compiler changed", cccmd, '->', self.cc
            return True
        if fv != self.target_firmware_version:
            if self.verbose: print "...yes, target firmware changed", fv, '->', self.target_firmware_version
            return True
        for dfile in deplist:
            if mtold <= os.path.getmtime(dfile):
                if self.verbose: print "...yes,", dfile, "is more recent"
                return True
        if self.verbose: print "...no"
        return False
    def get_ccfile_name(self, sourcefile_name):
        return sourcefile_name + ".dc"
    def get_depfile_name(self, sourcefile_name):
        return sourcefile_name + ".d"
    def get_deplist(self, filename):
        """Return tuple (cc, list of file names which 'filename' (source file) depends on, target firmware version).
        
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
            x = f.read().split('\n')
            cccmd = x[0]
            fv = x[1] if len(x)>1 else ''
        with open(depfile, "r") as f:
            return (cccmd, [s.strip() for s in f.readlines()], fv)
    def need_load(self, outfile, filename, thread):
        """Return true if need to load outfile to run as thread.
        False only returned if file name is same and it has the same mtime as the currently loaded obj file.
        """
        if self.verbose: print "Need load?...", thread, ":"
        try:
            if self.thd2obj[thread][0] != outfile:
                if self.verbose: print "...yes, new code for thread"
                return True
            if self.thd2obj[thread][2] != os.path.getmtime(outfile):
                if self.verbose: print "...yes, recompiled"
                return True
        except KeyError:
            if self.verbose: print "...yes, current unknown"
            return True
        if self.verbose: print "...no"
        return False
        
    def runcmd(self, cmd):
        if self.verbose: print "Running command", cmd
        try:
            rc = os.system(cmd)
            return (rc, cmd)
        except:
            return (1, "cmd abend")
            
    def write_ccfile(self, filename):
        with open(self.get_ccfile_name(filename), "w") as f:
            f.write(self.cc)
            f.write('\n')
            f.write(self.target_firmware_version)
            
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
            f.write('\n')
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
        self.k.SetCustomCompiler(None, self.opts)
        return self.k.Compile(filename, outfile, kmotion.BOARD_TYPE_KFLOP, thread, 1000)
        
    def ccompile_tcc67wine(self, outfile, filename, thread):
        """Invoke the Windows version TCC67 compiler under wine (via the DLL).
        """
        self.k.SetCustomCompiler("tcc67wine", self.opts)
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
    def __init__(self, dev, hostname = None, port = kmotion.KMOTION_PORT, with_console=False, dirlist=[], \
                srcdir=None, cc="tcc67", ccbindir=None, kflopdir=None, kflopname=None, verbose_tmgr=False, \
                target_firmware_version=None):
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
        if kflopdir is None:
            # Default firmware directory: this is compatible with legacy CNC apps.  New apps should
            # provide an explicit directory.
            kflopdir = os.path.join(_menigcnc_root, "DSP_KFLOP")
        self.SetMainPathRoot(_menigcnc_root)
        self.tmgr = ThreadManager(self, srcdir=srcdir, cc=cc, ccbindir=ccbindir, kflopdir=kflopdir, kflopname=kflopname, \
                                    verbose=verbose_tmgr, target_firmware_version=target_firmware_version)
        self.set_comm_result(-2)
        self.msgresp_poll_func = self.return_cancel
        self.firmware_version = None
        self.firmware_build = None
        self.version_str = ''
        self.fw_dirlist = dirlist
        self.read_firmware_version()
    def return_cancel(self, kthread):
        return kmotion.IDCANCEL
    def set_msgresp_poll(self, msgresp_poll_func):
        self.msgresp_poll_func = msgresp_poll_func
    # Overrides of virtual methods in superclass.  These may be called from
    # different thread (e.g. g-code interpreter).
    def HandleConsole(self, line):
        #print "Console:", line,
        self.add_message(ConsoleMessage(line))
    def HandleErrMsg(self, line):
        #print "Err:", line
        self.add_message(DLLErrorMessage(line))
    def HandleMsgBox(self, title, msg, opts):
        """This is a callback for handling error conditions encountered by the DLL.
        Since this may be invoked in another thread, pass message to UI instead of
        trying to multithread with GTK.
        """
        print "HandleMsgBox"
        self.add_message(PopupMessage(msg, title, opts, kflop_thread="DLL"))
        # Wait 1 sec for ui to process message
        time.sleep(1.)
        # Now poll main ui for response
        while True:
            rc = self.msgresp_poll_func("DLL")
            if rc != 0:
                break
            time.sleep(0.5)
        print "HMB response:", rc
        return rc
        

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
            if idle_func and not idle_func(thread):
                return
            time.sleep(sleep_time)
    def thread_done(self, thread):
        rc, r = self.WriteLineReadLine("CheckThread%d" % thread)
        if not rc:
            return r == "0"
        return -1
       
    def read_firmware_version(self):
        rc, self.version_str = self.WriteLineReadLine("version")
        if not rc:
            try:
                self.firmware_version, self.firmware_build = self.tmgr.parse_firmware_version(self.version_str)
                self.tmgr.set_target_firmware_version(self.get_firmware_version())
            except ValueError:
                #FIXME: for now, fall back to unqualified version (dspkflop.out) and hope for the best
                self.add_message(ErrorMessage(-1, "Falling back to unversioned firmware binary - no match for version %s" % self.get_firmware_version(), 0))
                self.tmgr.set_target_firmware_version('')
            except:
                self.add_message(ErrorMessage(-1, "Could not parse version string", 0))
                self.firmware_version, self.firmware_build = None, None
                
        self.find_matching_firmware()
        return rc
    def get_firmware_version(self):
        return self.firmware_version if self.firmware_version else ''
    def get_firmware_version_str(self):
        return self.version_str
    def get_firmware_build(self):
        return self.firmware_build if self.firmware_build else ''
    def set_firmware_dir_list(self, dirlist):
        """Set list of directories to search when locating a firmware (DSPKFLOP*.out) file.
        Also try to locate matching firmware so that we keep consistent state.
        """
        self.fw_dirlist = dirlist
        self.find_matching_firmware()
    def find_matching_firmware(self):
        """Scan firmware directory list to find firmware .out file which matches the
        current Kflop version.  Sets this as the firmware to use when compiling etc.
        If none found, does not change anything, but future compilation may fail.
        Returns filename if found, else whatever is currently set.
        NOTE: does not depend on naming convention.  Looks inside .out file to extract symbol
        and match to full version string.
        """
        filename = None
        for d in self.fw_dirlist:
            filename = self.find_matching_firmware_by_str(d, self.get_firmware_version_str())
            if filename: break
        if filename:
            self.SetDSPKFLOP(filename)
            print "Found matching firmware:", filename
        elif self.fw_dirlist:
            print "No firmware matching", self.get_firmware_version_str()
            print "  located in", self.fw_dirlist
        return self.GetDSPKFLOP()
    def _find_match_fw(self, version_str, dirname, fnames):
        for filename in fnames:
            #print "candidate", os.path.join(dirname, filename)
            f = filename.lower()
            if f.startswith("dspkflop") and f.endswith(".out"):
                filename = os.path.join(dirname, filename)
                rc, fvs = self.ExtractCoffVersionString(filename)
                #print "...", rc, fvs
                if rc == 0 and fvs.strip() == version_str:
                    self._found_fw = filename
                    break
        fnames[:] = []  # No recursion
    def find_matching_firmware_by_str(self, d, version_str):
        """Search directory d looking for a DSPKFLOP*.out file with matching version_s.
        version_str must be the full version string returned by the Kflop 'version' command.
        Does not depend on firmware file name to determine version; actually looks inside it.
        """
        self._found_fw = None
        os.path.walk(d, self._find_match_fw, version_str)
        return self._found_fw
        



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
          handle_pc_other(intvec) -- handle non-standard kflop to pc command.  intvec is 8 ints.
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
                mname = "m%d" % (acode - kmotion.MAX_MCODE_ACTIONS_M1+kmotion.MAX_MCODE_ACTIONS_BUTTONS + 80)
            cb = getattr(self, mname, None) or getattr(self.ui, mname, None)
            #print mname
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
    def HandleMCode(self, mcode):
        print "Handle mcode", mcode
        acode = mcode if mcode < 100 else mcode-79
        try:
            return self.hdict.get(acode, self.no_op)() or 0
        except Exception as e:
            traceback.print_exc(32)
            return 1
    def no_op(self):
        return 0

    def PC_SetToolLength(self):
        pc_comm = self.GetPCComm()
        index = pc_comm[1]
        where = pc_comm[2]
        length = self.GetOnePersistDouble(where*2)
        return self.ui.update_tool_entry(length=length, index=index)
    def PC_GetToolLength(self):
        pc_comm = self.GetPCComm()
        index = pc_comm[1]
        where = pc_comm[2]
        lengthv = self.ui.get_tool_entry(field='length', index=index, units=self.UserIsMetric())
        if length is None:
            return -1
        self.SetOnePersistDouble(where*2, length)
        return 0
    def PC_UpdateFixture(self):
        return 1
    def PC_GetToolOffsetX(self):
        return -1
    def PC_SetToolOffsetX(self):
        return -1
    def PC_GetToolOffsetY(self):
        return -1
    def PC_SetToolOffsetY(self):
        return -1
    def PC_SlotToIndex(self):
        pc_comm = self.GetPCComm()
        where = pc_comm[1]
        slot = pc_comm[2]
        index = self.ui.get_tool_table_index(slot=slot)
        if index is None:
            return -1
        self.SetPersistOneInt(where, index)
        return 0
        
    def PC_StatusMsg(self):
        pc_comm = self.GetPCComm()
        kthread = self.GetKFlopThread()
        self.k.add_message(StatusMessage(self.GetGatherString(pc_comm[1], 1000), kthread))
        return 0
    def PC_StatusClear(self):
        kthread = self.GetKFlopThread()
        self.k.add_message(StatusClearMessage(kthread))
        return 0
    
    # Old-style message box (deprecated) - assumes kflop unknown thread.
    def PC_MsgBox(self):
        msg = self.GetLastGatherString()
        opts = self.GetPCOptions()
        kthread = self.GetKFlopThread()
        try:
            self.ui.popup_remote(opts, msg, kthread)
            return 0
        except:
            return -1
    def PC_Running_MsgBox(self):
        try:
            rc = self.ui.done_remote(0)
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
            self.ui.cancel_remote(0)
        except:
            pass
            
    # New-style message box: accepts thread and result variable parameter so that several
    # messages may be outstanding.
    def PC_NBMsgBox(self):
        msg = self.GetLastGatherString()
        opts = self.GetPCOptions()
        kthread = self.GetKFlopThread()
        #self.k.add_message(PopupMessage(msg, title, opts, kthread, self.GetResultPersist()))
        try:
            self.ui.popup_remote(opts, msg, kthread, self.GetResultPersist())
            return 0
        except:
            return -1
        
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
    def run_mdi(self, text):
        tempfile = "/tmp/mdi.ngc"
        with open(tempfile, "w") as f:
            f.write(text + '\n')
        self.finished = False
        self.ui.handle_running(True)
        self.Interpret(kmotion.BOARD_TYPE_KFLOP, tempfile, 0, 0, False)
        


