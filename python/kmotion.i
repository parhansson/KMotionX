%module(directors="1") kmotion


#define __attribute__(x)

// from /usr/local/share/swig/3.0.4/python ...
//%include "pyopers.swg"
%include "typemaps.i"  
%include "cstring.i"  
%include "std_vector.i"  


%{
typedef unsigned BOOL;
#include "KMotionX.h"
#include "SocketWrapper.h"
#include "CMutex.h"
#include "CString.h"
#include "KMotionDLL.h"
#include "rs274ngc_return.h"
#include "rs274ngc.h"
#include "canon.h"
#include "TrajectoryPlanner.h"
#include "Kinematics.h"	// Added by ClassView
#include "CoordMotion.h"
#include "SetupTracker.h"
#include "GCodeInterpreter.h"
#undef check
#include "swig_extras.cpp"
%}

typedef unsigned BOOL;

%template(BoolVector) std::vector<bool>;
%template(IntVector) std::vector<int>;
%template(FloatVector) std::vector<double>;

%cstring_bounded_output(char *response, MAX_LINE);
%cstring_bounded_output(char *buf, MAX_LINE);
%cstring_output_maxsize(char *Err, int MaxErrLen);
%cstring_output_maxsize(char *OutFile, int MaxLength);

%apply int * OUTPUT { int *size_text, int *size_bss, int *size_data, int *size_total };

// We provide alternative for this, using std:vector<int>
%ignore CKMotionDLL::ListLocations(int *nlocations, int *list);
// Always use the overload wih error return
%ignore CKMotionDLL::CompileAndLoadCoff(const char *Name, int Thread);
// Don't support C callbacks.  Derive from KMotionCallbacks class instead and use SetCallbacks() method.
%ignore CKMotionDLL::SetConsoleCallback(CONSOLE_HANDLER *ch);
%ignore CKMotionDLL::SetErrMsgCallback(ERRMSG_HANDLER *eh);
// We handle status a bit differently.  Use virtual methods called on status change instead.
%ignore CKMotionDLL::GetStatus(MAIN_STATUS& status, bool lock);

// The following are dangerous to use directly.  Make them "hidden" to Python by renaming with underscore
%rename(_WaitToken) CKMotionDLL::WaitToken;
%rename(_ReleaseToken) CKMotionDLL::ReleaseToken;

// Make the class names a bit more Pythonic
%rename(KMotion) CKMotionDLL;
%rename(MotionParams) MOTION_PARAMS;
%rename(KStatus) MAIN_STATUS;
%rename(Kinematics) CKinematics;
%rename(CoordMotion) CCoordMotion;

// Allow extension to this in Python
%feature("director") CKMotionDLL;
%feature("director") GCodeInterpreter;

%import "SocketWrapper.h"

%include "KMotionDLL.h"

%extend CKMotionDLL {
    intvec ListLocations() {
        int locs[MAX_LINE/sizeof(int)];
        int nlocs;
        intvec iv;
        int rc = $self->ListLocations(&nlocs, locs);
        if (!rc) {
            for (int i = 0; i < nlocs; ++i)
                iv.push_back(locs[i]);
        }
        return iv;
    }
    
    void SetConsoleCallback() {
        // This used just to notify server
        $self->SetConsoleCallback(NULL);
    }
    

}


%rename($ignore, %$isfunction, %$not %$ismember) ""; // Ignore global functions
%rename($ignore, %$isvariable, %$not %$ismember) ""; // Ignore global variables
%ignore CGCodeInterpreter::SetUserMCodeCallback;
%ignore CGCodeInterpreter::SetUserCallback;
%ignore CGCodeInterpreter::m_UserFn;
%ignore CGCodeInterpreter::m_UserFnMCode;
%ignore CGCodeInterpreter::McodeActions; 
%ignore CGCodeInterpreter::Interpret(int board_type,
		          const char *fname,
			      int start, int end,
				  int restart,
                  G_STATUS_CALLBACK *StatusFn,
				  G_COMPLETE_CALLBACK *CompleteFn);
%ignore CGCodeInterpreter::SetOrigin(int index, double x, double y, double z, double a, double b, double c); 
%ignore CGCodeInterpreter::GetOrigin(int index, double *x, double *y, double *z, double *a, double *b, double *c);
%ignore CGCodeInterpreter::ConvertAbsoluteToInterpreterCoord(double x,double y,double z,double a,double b,double c, 
										double *gx,double *gy,double *gz,double *ga,double *gb,double *gc);
%ignore CGCodeInterpreter:: ConvertAbsoluteToMachine(double x,double y,double z,double a,double b,double c, 
									double *gx,double *gy,double *gz,double *ga,double *gb,double *gc);
	
%ignore CGCodeInterpreter:: ReadAndSyncCurPositions(double *x, double *y, double *z, double *a, double *b, double *c);

%ignore TP_COEFF;
%ignore SEGMENT;
%ignore SPECIAL_CMD;

%include "PC-DSP.h"
%include "TrajectoryPlanner.h"
%include "Kinematics.h"
%include "CoordMotion.h"
%include "GCodeInterpreter.h"
%include "swig_extras.cpp"


