/*
    Derived classes for use with swig bindings.
    
    Mainly to provide virtual members in order to provide a cleaner, more object-oriented style.
    
    This is only to be used with swig, so it includes both the definition and implementation of
    classes to be included directly in the generated wrapper.
    
    All necessary headers are assumed to have been included by the .i file.
    
    Unfortunately, we need to do a bit of a hack to handle callback functions.  The standard callbacks
    have no "this" parameter, so we have to provide it in a global.  The global is set by the
    ctor, so there can obviously be only one instance of GCodeInterpreter.
*/


// Wrapper class for main DLL, mainly to add interpreter lock for DoErrMsg() and Console().
class KMotion;
static KMotion * _theKMotion;   // an unfortunate hack to get message box callback to work
static int _msgbox_bouncer(const char *title, const char *msg, int options);

class KMotion : public CKMotionDLL
{
public:
    // Ctor overrides 
	KMotion(int boardid) : CKMotionDLL(boardid) { _theKMotion = this; mb_callback = _msgbox_bouncer; }
	KMotion(int boardid, unsigned int dfltport, const char * url = NULL) : CKMotionDLL(boardid, dfltport, url) { }
    virtual void Console(const char *buf);
    virtual void ErrMsg(const char *buf);
    int MsgBox(const char *title, const char *msg, int options);   // Bounce into binding
    virtual void HandleConsole(const char *buf) {}  // Default does nothing for binding
    virtual void HandleErrMsg(const char *buf) {}  // Default does nothing for binding
    virtual int HandleMsgBox(const char *title, const char *msg, int options) {return -1;}
    
};

void KMotion::ErrMsg(const char *buf)
{
    _ENTER_BINDING;
	HandleErrMsg(buf);    // This expected to be implemented as a binding override function
    _EXIT_BINDING;
}

void KMotion::Console(const char *buf)
{
    _ENTER_BINDING;
	HandleConsole(buf);    // This expected to be implemented as a binding override function
    _EXIT_BINDING;
}

static int _msgbox_bouncer(const char *title, const char *msg, int options)
{
    if (_theKMotion) {
        return _theKMotion->MsgBox(title, msg, options);
    }
    return -1;
}

int KMotion::MsgBox(const char *title, const char *msg, int options)
{
    int rc;
    _ENTER_BINDING;
	rc = HandleMsgBox(title, msg, options);    // This expected to be implemented as a binding override function
    _EXIT_BINDING;
    return rc;
}


// Following macros copied out of KMotionDef.h, so that binding can handle message boxes.
/*
 * MessageBox() Flags thes can be passed to the PC to invoke MessageBoxes
 *              for some applications such as KMotionCNC which monitor upload
 *              status and present message boxes when requested.  See the pc-dsp.h
 *              header for more information
 */
#define MB_OK                       0x00000000L
#define MB_OKCANCEL                 0x00000001L
#define MB_ABORTRETRYIGNORE         0x00000002L
#define MB_YESNOCANCEL              0x00000003L
#define MB_YESNO                    0x00000004L
#define MB_RETRYCANCEL              0x00000005L
#define MB_CANCELTRYCONTINUE        0x00000006L
#define MB_ICONHAND                 0x00000010L
#define MB_ICONQUESTION             0x00000020L
#define MB_ICONEXCLAMATION          0x00000030L
#define MB_ICONASTERISK             0x00000040L
#define MB_APPLMODAL                0x00000000L
#define MB_SYSTEMMODAL              0x00001000L
#define MB_TASKMODAL                0x00002000L
#define MB_NOFOCUS                  0x00008000L
#define MB_SETFOREGROUND            0x00010000L
#define MB_DEFAULT_DESKTOP_ONLY     0x00020000L
#define MB_TOPMOST                  0x00040000L
#define MB_RIGHT                    0x00080000L

/*
 * Dialog Box Command IDs
 */
#undef IDCANCEL
#undef IDYES
#undef IDNO
#define IDOK                1
#define IDCANCEL            2
#define IDABORT             3
#define IDRETRY             4
#define IDIGNORE            5
#define IDYES               6
#define IDNO                7



/*
typedef void G_COMPLETE_CALLBACK(int status, int lineno, int sequence_number, const char *err);
typedef void G_STATUS_CALLBACK(int line_no, const char *msg);
typedef int G_USER_CALLBACK(const char *msg);
typedef int G_M_USER_CALLBACK(int mCode);
*/

class GCodeInterpreter;
struct Action;
struct CHD;

GCodeInterpreter * _theInterpreter;

#define GEV_GEN_ENUM
#include "swig_extras_events.cpp"
#undef GEV_GEN_ENUM

// Additional event classes for GCodeInterpreter::EnableEventByClasses()
enum {
    POLL_G_M_CODES =        0x00010000, // G- and M- code activation
    POLL_OFFSETS =          0x00020000, //
    POLL_MISC_MODES =       0x00040000, //
    POLL_TOOL =             0x00080000, //
    POLL_CUTTER_COMP =      0x00100000, //
    POLL_CANNED_CYCLE =     0x00200000, //
    POLL_FEED =             0x00400000, //
    POLL_SPINDLE =          0x00800000, //
    POLL_UNITS =            0x01000000, //
    POLL_COOLANT =          0x02000000, //
    POLL_PLANE =            0x04000000, //
    POLL_LINE =             0x08000000, //
    POLL_x =          0x10000000, //
    POLL_y =          0x20000000, //
    POLL_z =          0x40000000, //
    POLL_a =          0x80000000, //
};


static void _gci_complete(int status, int lineno, int sequence_number, const char *err);


class GCodeInterpreter: public CGCodeInterpreter
{
public:
    GCodeInterpreter(CCoordMotion *CM);
	virtual ~GCodeInterpreter(void) {_theInterpreter = NULL;}
	
	// Virtual functions for "callbacks" from Interpret().  These are invoked from thread other than
	// caller of Interpret().
	virtual void HandleStatus(int line_no, const char *msg) {}
	virtual void HandleComplete(int status, int lineno, int sequence_number, const char *err) {}
	virtual int HandleUser(const char *msg) {return 0;}
	virtual int HandleMCode(int mCode) {return 0;}
	int Interpret(
				  int board_type,
		          const char *fname,
			      int start, int end,
				  int restart);
    bool Running() const { return is_running; } // Whether running g-code.
				  
	// Callbacks for event handling.  Invoked from Poll(), which looks for state changes and
	// calls these virtuals.  
	virtual void HandleBoolEvt(int evt_code, bool data) {}
	virtual void HandleIntEvt(int evt_code, int data, int prevdata) {}
	virtual void HandleFloatEvt(int evt_code, double data, double prevdata) {}
	// Non-virtual overloads to direct to the above (this shouldn't be required for c++, but makes it easier to
	// bind via swig while using templates here).
	void HandleEvt(int evt_code, bool data) { HandleBoolEvt(evt_code, data); }
	void HandleEvt(int evt_code, bool data, bool) { HandleBoolEvt(evt_code, data); }
	void HandleEvt(int evt_code, int data, int prevdata) { HandleIntEvt(evt_code, data, prevdata); }
	void HandleEvt(int evt_code, double data, double prevdata) { HandleFloatEvt(evt_code, data, prevdata); }
	int Poll();
	int PollKMotion();
	int PollGCodeInterpreter();
	// If known is false, then event will be generated immediately on next poll, otherwise only when value actually changes.
	void EnableEvent(int evt_code, bool enable=true, bool known=false);
	void EnableEventByClasses(unsigned clsbits, bool enable=true, bool known=false); // clsbits is OR of POLL_* enum value(s) (see KMotionDLL.h, plus the above)


    // Interface to action setting.  Return 0 if OK, 1 if bad action number.
    int SetActionNone(int action_num);
    int SetActionSetBit(int action_num, int bit, int state);
    int SetActionSetTwoBits(int action_num, int bit, int state, int bit2, int state2);
    int SetActionDAC(int action_num, int dacnum, double scale, double offset, int minval, int maxval);
    int SetActionProgram(int action_num, const char * name, int thread, int ipersist = -1);
    int SetActionProgramWait(int action_num, const char * name, int thread, int ipersist = -1);
    int SetActionProgramWaitSync(int action_num, const char * name, int thread, int ipersist = -1);
    int SetActionProgramPC(int action_num, const char * name);
    int SetActionCallback(int action_num);
    int SetActionWaitbit(int action_num, int bit, int state);
    
    // Motion parameter setting.  For language bindings, it is easier to have separate get/set rather than
    // directly manipulating a *ptr struct.  Use idiom (in e.g. Python) like:
    // mp = my_gcode_interp.GetMotionParams()
    // mp.BreakAngle = 0.02
    // my_gcode_interp.SetMotionParams(mp)
    void SetMotionParams(const MOTION_PARAMS & m) { CoordMotion->Kinematics->m_MotionParams = m; }
    
    // Some helper functions for coordinate transforms
	int SetOrigin(int index, const floatvec & vec);
	floatvec GetOrigin(int index);
    floatvec ConvertAbsoluteToInterpreterCoord(const floatvec & vec);
    floatvec ConvertAbsoluteToMachine(const floatvec & vec);
    floatvec ReadAndSyncCurPositions();
	floatvec TransformCADtoActuators(const floatvec & vec);
	floatvec TransformActuatorstoCAD(const floatvec & vec);
	
	// Convert between g20/g21 units in effect, and "fixed" metric or imperial.
	floatvec FixedToUser(const floatvec & vec, bool metric_in);
	floatvec UserToFixed(const floatvec & vec, bool metric_out);
	bool UserIsMetric() { return p_setup->length_units!=CANON_UNITS_INCHES; }
	
	// Simplified interfaces to CoordinatedMotion object...
	
	// Returns true if actual spindle speed obtained.  Always returns desired speed.
	bool GetSpindleRPM(double * actual, double * desired);
	bool GetSpindleRPMWithOverride(double * actual, double * desired);
	
	// Fixed methods for handling PC comms.
    void ServiceKFLOPCommands();    // Called automatically from Poll(), but app can call too.
    int GetPendingPC() const { return pc_pending; }
    const char * GetLastGatherString() const { return pc_string; }
    int GetPCOptions() const { return pc_options; }
    void SetPCResult(int r) { pc_result = r; }
	
	// Virtual methods to handle PC comms from the kflop.  All take no args, and return 0 if OK, -1 if error, 1 to
	// call the default handler (in this base class).
	// This group returns their result to the kflop immediately, thus do not require the cancel/running protocol.
    virtual int PC_Estop();
    virtual int PC_Halt();
    virtual int PC_Execute();
    virtual int PC_SingleStep();
    virtual int PC_Restart();
    virtual int PC_SetToolLength();
    virtual int PC_GetToolLength();
    virtual int PC_SetFRO();
    virtual int PC_SetSSO();
    virtual int PC_SetFROInc();
    virtual int PC_SetSSOInc();
    virtual int PC_UpdateFixture();
    virtual int PC_UserButton();
    virtual int PC_MCode();
    virtual int PC_Other();     // Catch-all for unknown PC commands
    // This group is long-running, so there are extra virtuals for cancellation and checking for continuation,
    // for each PC command code.  Long-running commands save the current command code in pc_pending field.
    // PC_xxx is called once, initially, for a new command of that type.
    // PC_Running_xxx is called whenever the above command is pending (i.e. kflop waiting for response), and
    // should return 1 to keep going, or 0/-1 if finished/error.
    // PC_Cancel_xxx is called if the kflop issued a new command without waiting for the current one to finish.
    virtual int PC_MsgBox();
    virtual void PC_Cancel_MsgBox();
    virtual int PC_Running_MsgBox();
    virtual int PC_MDI();
    virtual void PC_Cancel_MDI();
    virtual int PC_Running_MDI();

    // Helper functions for sending data to the kflop.
    int SendPQR(int persist, int mcode);// Send g-code P,Q,R parameters in the standard manner (used with most M-code handlers)
    int SendToolSlot(int persist);      // Send tool slot number (used with M6)
    int SyncPosition();                 // Sync interpreter pos from kflop

protected:
    int setAction(int action_num, int type, double p1 = 0., double p2 = 0., double p3 = 0., double p4 = 0., double p5 = 0., const char * str = NULL);

    MAIN_STATUS pm;  // Previous KMotion state
    setup ps;        // Previous interpreter state
    MAIN_STATUS nm;  // New KMotion state
                     // New interpreter state is in *p_setup
    int first_km_event;
    int last_km_event;
    int first_int_event;
    int last_int_event;
    std::vector<CHD *> t;   // Table of change detectors
    
    template <class T, class U> void setup_tm(T & val, U cbsel);   // set up t for a MAIN_STATUS field (val must be field of pm)
    void setup_tmbit(int & val, int fldsize, int idx);
    template <class T, class U> void setup_ts(T & val, U cbsel);   // set up t for an interpreter field (val must be field of ps)
    void setup_t();
    
    bool is_running;
    friend void ::_gci_complete(int status, int lineno, int sequence_number, const char *err);
    
    int pc_pending; // PC command which is pending completion (or 0 if none)
    int pc_options;         // MsgBox options from kflop
    int pc_result;          // Result to send to kflop (for MsgBox)
    char pc_string[1024];   // String pulled out of gather buffer for MsgBox or MDI
    
    int ReadInterpPos(double *pos);
    int SendCoordinates(int n, bool MachineCoords);
    int SendOneDouble(int i, double d);
    int GetVar(int Var, int *value);
    int GetStringFromGather(int WordOffset, char *msg, int nWords);
    int SetKFLOPCommandResult(int r);

};

#ifndef SWIG    // SWIG doesn't need the implementation details...

struct CHD
{
    int evt_code;
    bool enabled;
    bool known;
    CHD(int ec): evt_code(ec), enabled(false), known(false) {}
    virtual ~CHD() {}
    virtual bool is_changed() const = 0;
    virtual void handled() const = 0;
    virtual void handle(GCodeInterpreter *)  = 0;
};

template <class T, class U>
struct CHT: public CHD
{
    T * newval;
    T * oldval;
    CHT(int ec, T & nv, T & ov): CHD(ec), newval(&nv), oldval(&ov) {}
    virtual bool is_changed() const { return !known || *newval != *oldval; }
    virtual void handled() const { *oldval = *newval; }
    virtual void handle(GCodeInterpreter * i)  {
        if (enabled && is_changed()) {
            i->HandleEvt(evt_code, (U)*newval, (U)*oldval);
            known = true;
            handled();
        }
    }
};

struct CHTB: public CHT<int, int>
{
    unsigned mask;
    unsigned shift;
    int fldsize;
    CHTB(int ec, int & nv, int & ov, int _fldsize, int idx): CHT<int,int>(ec, nv, ov), fldsize(_fldsize) {
        while ((size_t)(idx * fldsize) >= sizeof(int)*8) {
            ++newval;
            ++oldval;
            idx -= sizeof(int)*8/fldsize;
        }
        shift = idx * fldsize;
        mask = ((1u<<fldsize)-1) << shift;
    }
    int getfld_new() const { return (*newval&mask)>>shift; }
    int getfld_old() const { return (*oldval&mask)>>shift; }
    virtual bool is_changed() const { return !known || (*newval&mask) != (*oldval&mask); }
    virtual void handled() const { *oldval &= ~mask; *oldval |= *newval&mask; }
    virtual void handle(GCodeInterpreter * i)  {
        if (enabled && is_changed()) {
            if (fldsize == 1)
                i->HandleEvt(evt_code, (bool)getfld_new());
            else
                i->HandleEvt(evt_code, getfld_new(), getfld_old());
            known = true;
            handled();
        }
    }
};


template <class T, class U>
void GCodeInterpreter::setup_tm(T & val, U cbsel)
{
    int ec = t.size();
    t.push_back(new CHT<T,U>(ec, ((T *)&nm)[&val - (T *)&pm], val));
}

// bitfields must currently have 'int' type (or int array, if more than 32 bits)
//FIXME: some horrible assumptions here!
void GCodeInterpreter::setup_tmbit(int & val, int fldsize, int bit)
{
    int ec = t.size();
    t.push_back(new CHTB(ec, ((int *)&nm)[&val - (int *)&pm], val, fldsize, bit));
}

template <class T, class U>
void GCodeInterpreter::setup_ts(T & val, U cbsel)
{
    int ec = t.size();
    t.push_back(new CHT<T,U>(ec, ((T *)p_setup)[&val - (T *)&ps], val));
}




void GCodeInterpreter::setup_t()
{
    int i;
    // These are basically used to select the appropriate setup member template parameter 'U'
    bool cb_bool = false;
    int cb_int = 0;
    double cb_float = 0.;
    
    first_km_event = 0;

#define GEV(tag,fld,cbsel) setup_tm(pm.fld,cbsel);
#define GEVX(tag, n,fld,cbsel) for (i = 0; i < n; ++i) setup_tm(pm.fld[i],cbsel);
#define GEVXB(tag, n, bits,fld) for (i = 0; i < n; ++i) setup_tmbit(pm.fld, bits, i);
#define GEV_BLOCK_KMOTION
#include "swig_extras_events.cpp"

    last_km_event = first_int_event = t.size();
    
#define GEV(tag,fld,cbsel) setup_ts(ps.fld,cbsel);
#define GEVX(tag, n,fld,cbsel) for (i = 0; i < n; ++i) setup_ts(ps.fld[i],cbsel);
#define GEVXX(tag, n,fld,typ,cbsel) for (i = 0; i < n; ++i) setup_ts(((typ *)&ps.fld)[i],cbsel);
#define GEV_BLOCK_INTERPETER
#include "swig_extras_events.cpp"

    last_int_event = t.size();
}




int GCodeInterpreter::setAction(int action_num, int type, double p1, double p2, double p3, double p4, double p5, const char * str)
{
    if ((unsigned)action_num >= MAX_MCODE_ACTIONS)
        return 1;
    MCODE_ACTION * m = McodeActions + action_num;
    m->Action = type;
    m->dParams[0] = p1;
    m->dParams[1] = p2;
    m->dParams[2] = p3;
    m->dParams[3] = p4;
    m->dParams[4] = p5;
    if (str) {
        strncpy(m->String, str, sizeof(m->String)-1);
        m->String[sizeof(m->String)-1] = 0;
    }
    else
        m->String[0] = 0;
    return 0;
}

int GCodeInterpreter::SetActionNone(int action_num)
{
    return setAction(action_num, M_Action_None);
}

int GCodeInterpreter::SetActionSetBit(int action_num, int bit, int state)
{
    return setAction(action_num, M_Action_Setbit, bit, state);
}

int GCodeInterpreter::SetActionSetTwoBits(int action_num, int bit, int state, int bit2, int state2)
{
    return setAction(action_num, M_Action_Setbit, bit, state, bit2, state2);
}

int GCodeInterpreter::SetActionDAC(int action_num, int dacnum, double scale, double offset, int minval, int maxval)
{
    return setAction(action_num, M_Action_DAC, dacnum, scale, offset, minval, maxval);
}

int GCodeInterpreter::SetActionProgram(int action_num, const char * name, int thread, int ipersist)
{
    return setAction(action_num, M_Action_Program, thread, ipersist, 0,0,0, name);
}

int GCodeInterpreter::SetActionProgramWait(int action_num, const char * name, int thread, int ipersist)
{
    return setAction(action_num, M_Action_Program_wait, thread, ipersist, 0,0,0, name);
}

int GCodeInterpreter::SetActionProgramWaitSync(int action_num, const char * name, int thread, int ipersist)
{
    return setAction(action_num, M_Action_Program_wait_sync, thread, ipersist, 0,0,0, name);
}

int GCodeInterpreter::SetActionProgramPC(int action_num, const char * name)
{
    return setAction(action_num, M_Action_Program_PC, 0,0,0,0,0, name);
}

int GCodeInterpreter::SetActionCallback(int action_num)
{
    return setAction(action_num, M_Action_Callback);
}

int GCodeInterpreter::SetActionWaitbit(int action_num, int bit, int state)
{
    return setAction(action_num, M_Action_Waitbit, bit, state);
}


/*
    Since the GCode interpreter runs as a separate thread, the following _gci_* callbacks
    will be invoked from a thread created outside the binding language.
    E.g. for Python, the GIL must be handled correctly.
    
    Use _ENTER_BINDING, _EXIT_BINDING around call to the virtual method.
*/


static void _gci_status(int line_no, const char *msg)
{
    //printf("-status- %d %s\n", line_no, msg);
    if (_theInterpreter) {
        _ENTER_BINDING;
        _theInterpreter->HandleStatus(line_no, msg);
        _EXIT_BINDING;
    }
}

static void _gci_complete(int status, int lineno, int sequence_number, const char *err)
{
    //printf("-compleat- %d %d %d %s\n", status, lineno, sequence_number, err);
    if (_theInterpreter) {
        _ENTER_BINDING;
        _theInterpreter->is_running = false;
        _theInterpreter->HandleComplete(status, lineno, sequence_number, err);
        _EXIT_BINDING;
    }
}

static int _gci_user(const char *msg)
{
    int rc = 0;
    //printf("-user- %s\n", msg);
    if (_theInterpreter) {
        _ENTER_BINDING;
        rc = _theInterpreter->HandleUser(msg);
        _EXIT_BINDING;
    }
    return rc;
}

static int _gci_mcode(int mCode)
{
    int rc = 0;
    //printf("-mcode- %d\n", mCode);
    if (_theInterpreter) {
        _ENTER_BINDING;
        rc = _theInterpreter->HandleMCode(mCode);
        _EXIT_BINDING;
    }
    return rc;
}

GCodeInterpreter::GCodeInterpreter(CCoordMotion *CM): CGCodeInterpreter(CM) 
{
    _theInterpreter = this;
    is_running = false;
    pc_pending = 0;
    pc_string[0] = 0;
    pc_result = -1;
    pc_options = 0;
    CGCodeInterpreter::SetUserCallback(_gci_user);
    CGCodeInterpreter::SetUserMCodeCallback(_gci_mcode);
    setup_t();

    _INIT_THREADS;
}

int GCodeInterpreter::Interpret(
				  int board_type,
		          const char *fname,
			      int start, int end,
				  int restart)
{
    is_running = true;
    return CGCodeInterpreter::Interpret(board_type, fname, start, end, restart, _gci_status, _gci_complete);
}



int GCodeInterpreter::SetOrigin(int index, const floatvec & vec)
{
    size_t s = vec.size();
    return CGCodeInterpreter::SetOrigin(index, s>0?vec[0]:0., s>1?vec[1]:0., s>2?vec[2]:0., s>3?vec[3]:0., s>4?vec[4]:0., s>5?vec[5]:0.);
}


struct fv6: public floatvec
{
    fv6(const double * v): floatvec(6) { for (int i = 0; i < 6; ++i) (*this)[i] = v[i]; }  
};

floatvec GCodeInterpreter::GetOrigin(int index)
{
    double v[6];
    int rc = CGCodeInterpreter::GetOrigin(index, v+0, v+1, v+2, v+3, v+4, v+5);
    if (rc)
        return floatvec(0); // empty vector if error
    return fv6(v);
}

floatvec GCodeInterpreter::ConvertAbsoluteToInterpreterCoord(const floatvec & vec)
{
    double v[6];
    size_t s = vec.size();
    CGCodeInterpreter::ConvertAbsoluteToInterpreterCoord(s>0?vec[0]:0., s>1?vec[1]:0., s>2?vec[2]:0., s>3?vec[3]:0., s>4?vec[4]:0., s>5?vec[5]:0., v+0, v+1, v+2, v+3, v+4, v+5);
    return fv6(v);
}

floatvec GCodeInterpreter::ConvertAbsoluteToMachine(const floatvec & vec)
{
    double v[6];
    size_t s = vec.size();
    CGCodeInterpreter::ConvertAbsoluteToMachine(s>0?vec[0]:0., s>1?vec[1]:0., s>2?vec[2]:0., s>3?vec[3]:0., s>4?vec[4]:0., s>5?vec[5]:0., v+0, v+1, v+2, v+3, v+4, v+5);
    return fv6(v);
}

floatvec GCodeInterpreter::ReadAndSyncCurPositions()
{
    double v[6];
    int rc = CGCodeInterpreter::ReadAndSyncCurPositions(v+0, v+1, v+2, v+3, v+4, v+5);
    if (rc)
        return floatvec(0); // empty vector if error
    return fv6(v);
}

floatvec GCodeInterpreter::TransformActuatorstoCAD(const floatvec & vec)
{
    double v[6];
    double a[6];
    size_t s = vec.size();
    for (size_t i = 0; i < 6; ++i)
        a[i] = s>i?vec[i]:0.;
    int rc = CoordMotion->Kinematics->TransformActuatorstoCAD(a ,v+0, v+1, v+2, v+3, v+4, v+5);
    if (rc)
        return floatvec(0); // empty vector if error
    return fv6(v);
}

floatvec GCodeInterpreter::TransformCADtoActuators(const floatvec & vec)
{
    double a[6];
    size_t s = vec.size();
    int rc = CoordMotion->Kinematics->TransformCADtoActuators(s>0?vec[0]:0., s>1?vec[1]:0., s>2?vec[2]:0., s>3?vec[3]:0., s>4?vec[4]:0., s>5?vec[5]:0., a);
    if (rc)
        return floatvec(0); // empty vector if error
    return fv6(a);
}


floatvec GCodeInterpreter::FixedToUser(const floatvec & vec, bool metric_in)
{
    double v[6];
    size_t s = vec.size();
    double fac = p_setup->length_units==CANON_UNITS_INCHES ? (metric_in ? 1./25.4 : 1.) : (metric_in ? 1. : 25.4);
    for (size_t i = 0; i < 6; ++i) {
        v[i] = s>i?vec[i]:0.;
        if (i < 3)
            v[i] *= fac;
        else if (i==3)
            v[i] *= CoordMotion->Kinematics->m_MotionParams.DegreesA ? 1. : fac;
        else if (i==4)
            v[i] *= CoordMotion->Kinematics->m_MotionParams.DegreesB ? 1. : fac;
        else
            v[i] *= CoordMotion->Kinematics->m_MotionParams.DegreesC ? 1. : fac;
    }
    return fv6(v);
}

floatvec GCodeInterpreter::UserToFixed(const floatvec & vec, bool metric_out)
{
    double v[6];
    size_t s = vec.size();
    double fac = p_setup->length_units==CANON_UNITS_INCHES ? (metric_out ? 25.4 : 1.) : (metric_out ? 1. : 1./25.4);
    for (size_t i = 0; i < 6; ++i) {
        v[i] = s>i?vec[i]:0.;
        if (i < 3)
            v[i] *= fac;
        else if (i==3)
            v[i] *= CoordMotion->Kinematics->m_MotionParams.DegreesA ? 1. : fac;
        else if (i==4)
            v[i] *= CoordMotion->Kinematics->m_MotionParams.DegreesB ? 1. : fac;
        else
            v[i] *= CoordMotion->Kinematics->m_MotionParams.DegreesC ? 1. : fac;
    }
    return fv6(v);
}


bool GCodeInterpreter::GetSpindleRPM(double * actual, double * desired)
{ 
    float rps = 0.; 
    bool rc = 0 == CoordMotion->GetSpindleRPS(rps);
    *actual = rps * 60.;
    *desired = p_setup->speed;
    return rc;
}

bool GCodeInterpreter::GetSpindleRPMWithOverride(double * actual, double * desired)
{ 
    float rps = 0.; 
    bool rc = 0 == CoordMotion->GetSpindleRPS(rps);
    *actual = rps * 60.;
    *desired = p_setup->speed * CoordMotion->GetSpindleRateOverride();
    return rc;
}


int GCodeInterpreter::Poll()
{
    int rc = PollKMotion();
    if (rc)
        return rc;
    ServiceKFLOPCommands();
    return PollGCodeInterpreter();
}

int GCodeInterpreter::PollKMotion()
{
    // Similar to KMotionDLL::Poll(), except fine-grained events.
    if (CoordMotion->KMotionDLL->GetStatus(nm, true))
        return 1;
    for (int i = first_km_event; i < last_km_event; ++i)
        t[i]->handle(this);
    return 0;
}

int GCodeInterpreter::PollGCodeInterpreter()
{
    for (int i = first_int_event; i < last_int_event; ++i)
        t[i]->handle(this);
    return 0;
}

void GCodeInterpreter::EnableEvent(int evt_code, bool enable, bool known)
{
    if (evt_code < 0 || evt_code >= LAST_GEVTCODE)
        return;
    t[evt_code]->enabled = enable;
    t[evt_code]->known = known;
    if (known) {
        t[evt_code]->handled();
    }
}


void GCodeInterpreter::EnableEventByClasses(unsigned clsbits, bool enable, bool known)
{
}




/*****************************************
    Handler for commands from Kflop.
    
    ServiceKFLOPCommands() is the main routine called e.g. from polling.  It dispatches
    to the appropriate virtual method in the Interpreter class.  The default v.m.
    matches the actions in KMotionCNC, however the derived interpreter needs to
    override some of these which don't have a suitable default.
    
    Return code from default v.m. is 0 if OK or -1 if error.
    Return from derived class v.m. is same as above, but can also be 1 to call the
    default implementation.
    
    Some PC commands are long-running (such as message boxes and g-code execution).
    These are executed in several phases, with appropriate virtual methods:
    
      int PC_xxx()         - initial - return -1 if error, 0 if OK, 1 to call
                             default v.m.
      void PC_Cancel_xxx() - called if kflop changed the request code while this
                             command was in progress.
      int PC_Running_xxx() - called to determine if command is still in progress.
                           - return 1 if still running, 0 if completed OK,
                             -1 if completed in error.
    
    
******************************************/
#define CALL_BINDING(vmname) do { \
        int rc; \
        _ENTER_BINDING; \
        rc = vmname(); \
        _EXIT_BINDING; \
        if (rc == 1) rc = GCodeInterpreter::vmname(); \
        SetKFLOPCommandResult(rc ? -1 : 0); \
    } while (0)
    
#define CALL_LONG_BINDING(vmname) do { \
        int rc; \
        pc_result = -1; \
        _ENTER_BINDING; \
        rc = vmname(); \
        _EXIT_BINDING; \
        if (rc == 1) rc = GCodeInterpreter::vmname(); \
        if (rc < 0) \
            SetKFLOPCommandResult(-1); \
        else \
            pc_pending = nm.PC_comm[0]; \
    } while (0)
    
#define CALL_BINDING_RUNNING(vmname, pres) do { \
        int rc; \
        _ENTER_BINDING; \
        rc = vmname(); \
        _EXIT_BINDING; \
        if (rc < 1) { \
			sprintf(s, "SetPersistDec%d %d",PC_COMM_PERSIST+pres,pc_result); \
			if (CoordMotion->KMotionDLL->WriteLine(s)) rc = -1; \
            SetKFLOPCommandResult(rc ? -1 : 0); \
        } \
    } while (0)
    
    
int GCodeInterpreter::PC_Estop()
{
    char s[80];
    int i;
    
    for (i=1;i<N_USER_THREADS;i++)   // kill all user programs except Thread #1
    {
	    sprintf(s, "Kill %d", i+1);
	    if (CoordMotion->KMotionDLL->WriteLine(s)) return -1;
    }

    Halt();

    Abort();

    for (i=0;i<N_CHANNELS;i++)             // disable all servo channels
    {
	    sprintf(s, "DISABLEAXIS%d",i);
	    if (CoordMotion->KMotionDLL->WriteLine(s)) return -1;
    }
    return 0;
}

int GCodeInterpreter::PC_Halt()
{
    Halt();
    return 0;
}

int GCodeInterpreter::PC_Execute()
{
    return -1;  // No default implementation
}

int GCodeInterpreter::PC_SingleStep()
{
    return -1;  // No default implementation
}

int GCodeInterpreter::PC_Restart()
{
    return -1;  // No default implementation
}

int GCodeInterpreter::PC_SetToolLength()
{
    return -1;  // No default implementation
}

int GCodeInterpreter::PC_GetToolLength()
{
    return -1;  // No default implementation
}

int GCodeInterpreter::PC_SetFRO()
{
	float NewValue = *(float *)&nm.PC_comm[1];
	if (NewValue > 0.001 && NewValue < 1000.0)   // ignore if crazy number
		CoordMotion->SetFeedRateOverride(NewValue);
	else
	    return -1;
    return 0;
}

int GCodeInterpreter::PC_SetSSO()
{
	float NewValue = *(float *)&nm.PC_comm[1];
	if (NewValue > 0.001 && NewValue < 1000.0)   // ignore if crazy number
		CoordMotion->SetSpindleRateOverride(NewValue);
	else
	    return -1;
    return 0;
}

int GCodeInterpreter::PC_SetFROInc()
{
	float NewValue = *(float *)&nm.PC_comm[1] * CoordMotion->GetFeedRateOverride();
	if (NewValue > 0.001 && NewValue < 1000.0)   // ignore if crazy number
		CoordMotion->SetFeedRateOverride(NewValue);
	else
	    return -1;
    return 0;
}

int GCodeInterpreter::PC_SetSSOInc()
{
	float NewValue = *(float *)&nm.PC_comm[1] * CoordMotion->GetSpindleRateOverride();
	if (NewValue > 0.001 && NewValue < 1000.0)   // ignore if crazy number
		CoordMotion->SetSpindleRateOverride(NewValue);
	else
	    return -1;
    return 0;
}

int GCodeInterpreter::PC_UpdateFixture()
{
	setup_pointer p = p_setup;
	int Fix = p->origin_index-1;
	double *Vars = p->parameters;	

	if (p->length_units==CANON_UNITS_INCHES)
	{
		// for inches round to 6 digits
		for (int axis=0; axis<6; axis++)
			Vars[5221+Fix*20+axis] = floor(Vars[5221+Fix*20+axis] * 1e6 + 0.5) / 1e6;
	}
	else if (p->length_units==CANON_UNITS_MM)
	{
		// for mm round to 4 digits
		for (int axis=0; axis<6; axis++)
			Vars[5221+Fix*20+axis] = floor(Vars[5221+Fix*20+axis] * 1e4 + 0.5) / 1e4;
	}
    return 0;
}


int GCodeInterpreter::PC_UserButton()
{
	if (!Running()) {
		InvokeAction(11+nm.PC_comm[1],FALSE);
		return 0;
	}
	else
	    return -1;
}

int GCodeInterpreter::PC_MCode()
{
	if (!Running()) {
		InvokeAction(nm.PC_comm[1],FALSE);
		return 0;
	}
	else
	    return -1;
}

int GCodeInterpreter::PC_MsgBox()
{
    return -1;  // No default implementation
}
void GCodeInterpreter::PC_Cancel_MsgBox()
{
}
int GCodeInterpreter::PC_Running_MsgBox()
{
    return 0;
}

int GCodeInterpreter::PC_MDI()
{
    return -1;  // No default implementation
}

void GCodeInterpreter::PC_Cancel_MDI()
{
}
int GCodeInterpreter::PC_Running_MDI()
{
    return 0;
}


int GCodeInterpreter::PC_Other()
{
    return -1;  // Unknown code, error.
}





void GCodeInterpreter::ServiceKFLOPCommands()
{
	double pos[6],*dp;
	float F,NewValue;
	char s[80];
	int i;
	MOTION_PARAMS *MP;
	CCoordMotion *CM = CoordMotion;

    if (pc_pending > 0 && nm.PC_comm[0] != pc_pending) {
        // Outstanding long-running PC action in progress, but kflop changed the PC request code,
        // so invoke cancellation callbacks.
        _ENTER_BINDING;
        switch (pc_pending) {
        case PC_COMM_MSG:
            PC_Cancel_MsgBox();
            break;
        case PC_COMM_MDI:
            PC_Cancel_MDI();
            break;
        }
        _EXIT_BINDING;
        pc_pending = 0;
    }

	if (nm.PC_comm[0] <= 0)  // don't do anything if error or zero
	    return;
	    
	switch (nm.PC_comm[0])
	{
	case PC_COMM_ESTOP:
	    CALL_BINDING(PC_Estop);
		break;

	case PC_COMM_HALT:
	    CALL_BINDING(PC_Halt);
		break;

	case PC_COMM_EXECUTE:
	    CALL_BINDING(PC_Execute);
		break;

	case PC_COMM_SINGLE_STEP:
	    CALL_BINDING(PC_SingleStep);
		break;

	case PC_COMM_SET_FRO:
	    CALL_BINDING(PC_SetFRO);
		break;

	case PC_COMM_SET_SSO:
	    CALL_BINDING(PC_SetSSO);
		break;

	case PC_COMM_SET_FRO_INC:
	    CALL_BINDING(PC_SetFROInc);
		break;

	case PC_COMM_SET_SSO_INC:
	    CALL_BINDING(PC_SetSSOInc);
		break;

	case PC_COMM_SET_X:     // No callback for these
	case PC_COMM_SET_Y:
	case PC_COMM_SET_Z:
	case PC_COMM_SET_A:
	case PC_COMM_SET_B:
	case PC_COMM_SET_C:
		NewValue = *(float *)&nm.PC_comm[1];
		i = nm.PC_comm[0] - PC_COMM_SET_X;
		dp = &p_setup->axis_offset_x;
		if (!Running() && !ReadInterpPos(pos))
		{
			dp[i] += pos[i] - NewValue;
			p_setup->parameters[5211+i]=dp[i];
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;


	case PC_COMM_USER_BUTTON:
	    CALL_BINDING(PC_UserButton);
		break;

	case PC_COMM_MCODE:
	    CALL_BINDING(PC_MCode);
		break;

	case PC_COMM_MSG:
	    if (pc_pending)
	        CALL_BINDING_RUNNING(PC_Running_MsgBox, 3);
	    else {
	        if (GetStringFromGather(nm.PC_comm[1],pc_string,sizeof(pc_string)/4)) {
	            SetKFLOPCommandResult(-1);
                break;
            }
	        pc_options = nm.PC_comm[2];
	        CALL_LONG_BINDING(PC_MsgBox);
	    }
	    /* Message box protocol...
		if (!MsgDisplayed)
		{
			if (GetStringFromGather(nm.PC_comm[1],&s,50)) break;
			MsgDisplayed=true;
			nm.PC_comm[0]=0;  // clear the command now that it has been executed
			result = AfxMessageBox(s,nm.PC_comm[2]);
			MsgDisplayed=false;
			s.Format("SetPersistDec%d %d",PC_COMM_PERSIST+3,result);
			if (CM->KMotionDLL->WriteLine(s)) break;
			SetKFLOPCommandResult(0);
		}
		*/
		break;

	case PC_COMM_GET_VARS:  // no callback
		for (i=0; i<nm.PC_comm[2]; i++)  // loop through how many to transfer
		{
			double d = p_setup->parameters[nm.PC_comm[1]+i];
			if (SendOneDouble(nm.PC_comm[3]+i, d)) break;
		}
		if (i==nm.PC_comm[2])
			SetKFLOPCommandResult(0);
		else
			SetKFLOPCommandResult(-1);

		break;

	case PC_COMM_SET_VARS:  // no callback
		for (i=0; i<nm.PC_comm[2]; i++)  // loop through how many to transfer
		{
			double *d = &p_setup->parameters[nm.PC_comm[1]+i];

			if (GetVar((nm.PC_comm[3]+i)*2,    (int*)d   )) break;
			if (GetVar((nm.PC_comm[3]+i)*2+1, ((int*)d)+1)) break;
		}
		SetKFLOPCommandResult(0);
		break;

	case PC_COMM_MDI:
	    if (pc_pending)
	        CALL_BINDING_RUNNING(PC_Running_MDI, 2);
	    else {
	        if (GetStringFromGather(nm.PC_comm[1],pc_string,sizeof(pc_string)/4)) {
	            SetKFLOPCommandResult(-1);
                break;
            }
	        CALL_LONG_BINDING(PC_MDI);
	    }
	    /*
		if (!GcodeInProgress)
		{
			if (!Running())
			{
				if (GetStringFromGather(nm.PC_comm[1],&s,50)) break;
				if (DoGCodeLine(s))
					SetKFLOPCommandResult(-1);  // failed tell KFLOP
				else
					GcodeInProgress=true;  // GCode is successfully Launched!
			}
			else
			{
				SetKFLOPCommandResult(-1);  // not possible to launch GCode
			}
		}
		else
		{
			if (!Running())
			{
				// tell KFLOP the final exit code
				s.Format("SetPersistDec%d %d",PC_COMM_PERSIST+2,m_exitcode);
				if (TheFrame->KMotionDLL->WriteLine(s)) break;
				SetKFLOPCommandResult(0);  // tell KFLOP Gcode is done
				GcodeInProgress=false;
			}
		}
		*/
		break;

	case PC_COMM_GETAXISRES:    // no callback
		MP=&CM->Kinematics->m_MotionParams;
		F=MP->CountsPerInchX;
		sprintf(s, "SetPersistHex%d %x",(nm.PC_comm[1]+0),*(int*)&F);
		if (CM->KMotionDLL->WriteLine(s)) break;
		F=MP->CountsPerInchY;
		sprintf(s, "SetPersistHex%d %x",(nm.PC_comm[1]+1),*(int*)&F);
		if (CM->KMotionDLL->WriteLine(s)) break;
		F=MP->CountsPerInchZ;
		sprintf(s, "SetPersistHex%d %x",(nm.PC_comm[1]+2),*(int*)&F);
		if (CM->KMotionDLL->WriteLine(s)) break;
		F=MP->CountsPerInchA;
		sprintf(s, "SetPersistHex%d %x",(nm.PC_comm[1]+3),*(int*)&F);
		if (CM->KMotionDLL->WriteLine(s)) break;
		F=MP->CountsPerInchB;
		sprintf(s, "SetPersistHex%d %x",(nm.PC_comm[1]+4),*(int*)&F);
		if (CM->KMotionDLL->WriteLine(s)) break;
		F=MP->CountsPerInchC;
		sprintf(s, "SetPersistHex%d %x",(nm.PC_comm[1]+5),*(int*)&F);
		if (CM->KMotionDLL->WriteLine(s)) break;
		SetKFLOPCommandResult(0);
		break;

	case PC_COMM_RESTART:
	    CALL_BINDING(PC_Restart);
		break;

	case PC_COMM_SET_TOOLTABLE_LENGTH:
	    CALL_BINDING(PC_SetToolLength);
	    /*
		{
			CEditToolFile EditToolFile;
			// first read the Tool Table File
			if (EditToolFile.LoadFile(m_ToolFile))
			{
				SetKFLOPCommandResult(1);
				break;
			}

			index=nm.PC_comm[1];
			if (index>=0 && index <128)
			{
				dp = &Interpreter->p_setup->tool_table[index+1].length;
				if (GetVar((nm.PC_comm[2])*2,    (int*)dp   )) break;
				if (GetVar((nm.PC_comm[2])*2+1, ((int*)dp)+1)) break;

				EditToolFile.ChangeLength(index,*dp);

				EditToolFile.SaveFile(m_ToolFile);
				TheFrame->GCodeDlg.Interpreter->m_ReadToolFile=true;

				// check if this tool is selected and in use.  If so,
				// update the tool length in use
				setup_pointer p=TheFrame->GCodeDlg.Interpreter->p_setup;
				if (p->active_g_codes[9]==G_43 && index == p->length_offset_index)  //active and
				{
					p->current_z = (p->current_z + p->tool_length_offset - *dp);
					p->tool_length_offset = *dp;
				}

				SetKFLOPCommandResult(0);
			}
			else
			{
				SetKFLOPCommandResult(2);
			}
		}
		*/
		break;

	case PC_COMM_GET_TOOLTABLE_LENGTH:
	    CALL_BINDING(PC_GetToolLength);
	    /*
		{
			CEditToolFile EditToolFile;
			// first read the Tool Table File
			if (EditToolFile.LoadFile(m_ToolFile))
			{
				SetKFLOPCommandResult(1);
				break;
			}
			index=nm.PC_comm[1];
			if (index>=0 && index <20)
			{
				double d = Interpreter->p_setup->tool_table[index+1].length;
				if (SendOneDouble(nm.PC_comm[2], d)) break;
				SetKFLOPCommandResult(0);
			}
			else
			{
				SetKFLOPCommandResult(2);
			}
		}
		*/
		break;


	case PC_COMM_GET_MISC_SETTINGS:  // Units, T, H, D indexes - no callback
		sprintf(s, "SetPersistHex%d %x",nm.PC_comm[1],p_setup->length_units);
		if (CM->KMotionDLL->WriteLine(s)) break;
		sprintf(s, "SetPersistHex%d %x",nm.PC_comm[1]+1,p_setup->selected_tool_slot);
		if (CM->KMotionDLL->WriteLine(s)) break;
		sprintf(s, "SetPersistHex%d %x",nm.PC_comm[1]+2,p_setup->length_offset_index); // H number
		if (CM->KMotionDLL->WriteLine(s)) break;
		sprintf(s, "SetPersistHex%d %x",nm.PC_comm[1]+3,p_setup->tool_table_index); // D number
		if (CM->KMotionDLL->WriteLine(s)) break;
		SetKFLOPCommandResult(0);
		break;

	case PC_COMM_GET_MACHINE_COORDS:
		SetKFLOPCommandResult(SendCoordinates(nm.PC_comm[1], true) ? -1 : 0);
		break;

	case PC_COMM_GET_DROS:
		SetKFLOPCommandResult(SendCoordinates(nm.PC_comm[1], false) ? -1 : 0);
		break;

	case PC_COMM_UPDATE_FIXTURE:
	    CALL_BINDING(PC_UpdateFixture);
		break;

	default:
	    CALL_BINDING(PC_Other);
		break;
	}

}


int GCodeInterpreter::ReadInterpPos(double *pos) 
{
	if (CoordMotion->ReadCurAbsPosition(pos+0,pos+1,pos+2,pos+3,pos+4,pos+5)) return 1;
	CGCodeInterpreter::ConvertAbsoluteToInterpreterCoord(pos[0],pos[1],pos[2],pos[3],pos[4],pos[5],pos+0,pos+1,pos+2,pos+3,pos+4,pos+5);

	return 0;
}


int GCodeInterpreter::SendCoordinates(int n, bool MachineCoords)
{
	double ActsDest[MAX_ACTUATORS],x,y,z,a,b,c;
	CCoordMotion *CM = CoordMotion;

	for (int i=0; i<MAX_ACTUATORS; i++) ActsDest[i]=0.0;

	if (CM->x_axis >=0)	ActsDest[CM->x_axis] = nm.Dest[CM->x_axis];
	if (CM->y_axis >=0)	ActsDest[CM->y_axis] = nm.Dest[CM->y_axis];
	if (CM->z_axis >=0)	ActsDest[CM->z_axis] = nm.Dest[CM->z_axis];
	if (CM->a_axis >=0)	ActsDest[CM->a_axis] = nm.Dest[CM->a_axis];
	if (CM->b_axis >=0)	ActsDest[CM->b_axis] = nm.Dest[CM->b_axis];
	if (CM->c_axis >=0)	ActsDest[CM->c_axis] = nm.Dest[CM->c_axis];

	CM->Kinematics->TransformActuatorstoCAD(ActsDest,&x,&y,&z,&a,&b,&c);

	if (MachineCoords)
		CGCodeInterpreter::ConvertAbsoluteToMachine(x,y,z,a,b,c,&x,&y,&z,&a,&b,&c);
	else
		CGCodeInterpreter::ConvertAbsoluteToInterpreterCoord(x,y,z,a,b,c,&x,&y,&z,&a,&b,&c);

	if (SendOneDouble(n,  x)) return 1;
	if (SendOneDouble(n+1,y)) return 1;
	if (SendOneDouble(n+2,z)) return 1;
	if (SendOneDouble(n+3,a)) return 1;
	if (SendOneDouble(n+4,b)) return 1;
	if (SendOneDouble(n+5,c)) return 1;
	return 0;
}


int GCodeInterpreter::SendOneDouble(int i, double d)
{
	char s[80];

	sprintf(s, "SetPersistHex%d %x",i*2,((int*)&d)[0]);
	if (CoordMotion->KMotionDLL->WriteLine(s)) return 1;
	sprintf(s, "SetPersistHex%d %x",i*2+1,((int*)&d)[1]);
	if (CoordMotion->KMotionDLL->WriteLine(s)) return 1;
	return 0;
}

int GCodeInterpreter::GetVar(int Var, int *value)
{
	char s[80], r[80];

	if (Var >= 0 && Var < 200)
	{
		sprintf(s, "GetPersistHex %d",Var);
		if(CoordMotion->KMotionDLL->WriteLineReadLine(s, r)) return 1;
		sscanf(r,"%x",value);
	}
	return 0;
}


// msg must point to at least max(4,nWords*4) bytes.
// Reads in chunks of 32 words (128 bytes), until a null terminator is found (or nWords exceeded).
int GCodeInterpreter::GetStringFromGather(int WordOffset, char *msg, int nWords)
{
	char s[MAX_LINE+1];
	int j, w, w2r;
	int * out = (int *)msg;
	
	if (nWords < 1)
	    nWords = 1;

	CoordMotion->KMotionDLL->WaitToken();
	
	for (w = 0; w < nWords; w += 32, out += 32, WordOffset += 32) {
	    w2r = nWords - w;
	    if (w2r > 32)
	        w2r = 32;
	    sprintf(s, "GetGatherHex %d %d",WordOffset,w2r);  
	    CoordMotion->KMotionDLL->WriteLine(s);

	    for (int i=0;i<w2r;++i)  // convert hex to 32 bit words
	    {
		    if (!(i&7)) {  // every 8 hex get a new line
			    if (CoordMotion->KMotionDLL->ReadLineTimeOut(s, 1000)) return 1;  
			    j = 0;
	        }

		    int result = sscanf(s + j,"%8X",out+i);
		    j += 9;

		    if (result!=1)
		    {
			    CoordMotion->KMotionDLL->ReleaseToken();
			    return 1;
		    }

	    }
	    for (j = 0; j < w2r*4; ++j)
	        if (!((char *)out)[j])
	            goto _done;  // Found null in this chunk, so done.
	}
_done:
	CoordMotion->KMotionDLL->ReleaseToken();

	msg[nWords*4-1] = 0;  // make sure it is terminated
	//printf("GSFG: %p %s\n", msg, msg);
	return 0;
}



int GCodeInterpreter::SetKFLOPCommandResult(int r)
{
	char s[80];

	nm.PC_comm[0]=0;    // clear the command now that it has been executed
	pc_pending = 0;     // No more pending command
	sprintf(s, "SetPersistDec%d %d",PC_COMM_PERSIST, r);
	if (CoordMotion->KMotionDLL->WriteLine(s)) return 1;
	return 0;
}


int GCodeInterpreter::SendPQR(int persist, int mcode)
{
    // Send g-code P,Q,R parameters in the standard manner.
	int count=0;
	char s[80];
	
	// For most M Codes, check if P Q or R words are present on the same line.
	// If so, download them into successive persist vars as floats.

	if (p_setup->block1.p_flag)
	{
		float p = (float)p_setup->block1.p_number;
		sprintf(s, "SetPersistHex %d %x", persist, *(int *)&p);
		if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
		persist++;
		count++;
	}

	if (p_setup->block1.q_flag)
	{
		float q = (float)p_setup->block1.q_number;
		sprintf(s, "SetPersistHex %d %x", persist, *(int *)&q);
		if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
		persist++;
		count++;
	}

	if (p_setup->block1.r_flag)
	{
		float r = (float)p_setup->block1.r_number;
		sprintf(s, "SetPersistHex %d %x", persist, *(int *)&r);
		if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
		persist++;
		count++;
	}

	if (count==0)  // if no parameters just set the MCode number
	{
		sprintf(s, "SetPersistHex %d %x", persist, mcode);
		if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
	}
	return 0;
}

int GCodeInterpreter::SendToolSlot(int persist)
{
    // Send tool slot number (used with M6)
    char s[80];
	sprintf(s, "SetPersistHex %d %x", persist, p_setup->selected_tool_slot);
	if (CoordMotion->KMotionDLL->WriteLine(s)) {
	    CoordMotion->SetAbort(); 
	    return 1;
	}
	return 0;
}

int GCodeInterpreter::SyncPosition()
{
    // Sync interpreter pos from kflop
	// don't sample positions until everything is stopped 
	if (CoordMotion->WaitForSegmentsFinished()) return 1;
	if (CoordMotion->WaitForMoveXYZABCFinished()) return 1;

	if (!CoordMotion->m_Simulate && 
	    CGCodeInterpreter::ReadAndSyncCurPositions(&_setup.current_x,&_setup.current_y,&_setup.current_z,
		                                           &_setup.AA_current,&_setup.BB_current,&_setup.CC_current))
	{
		if (CoordMotion->m_AxisDisabled)
			return 2;
		return 1;
	}
	return 0;
}

#endif





