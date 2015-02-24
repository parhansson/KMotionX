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
    
    The following macros should be defined as appropriate for each binding language
*/
#ifdef SWIGPYTHON
    #define _ENTER_BINDING  PyGILState_STATE gstate = PyGILState_Ensure()
    #define _EXIT_BINDING  PyGILState_Release(gstate)
    #define _INIT_THREADS PyEval_InitThreads()
#else
    #error "You'll need to define _ENTER_BINDING / _EXIT_BINDING / _INIT_THREADS appropriately for this language."
#endif


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


int GCodeInterpreter::Poll()
{
    int rc = PollKMotion();
    if (rc)
        return rc;
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


#endif





