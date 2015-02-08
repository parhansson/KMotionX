%module(directors="1") kmotion


#define __attribute__(x)

// from /usr/local/share/swig/3.0.4/python ...
//%include "pyopers.swg"
%include "typemaps.i"  
%include "cstring.i"  
%include "std_vector.i"  


%{
#include "SocketWrapper.h"
#include "CMutex.h"
#include "CString.h"
#include "KMotionDLL.h"

%}

%template(BoolVector) std::vector<bool>;
%template(IntVector) std::vector<int>;
%template(FloatVector) std::vector<double>;

%cstring_bounded_output(char *response, MAX_LINE);
%cstring_bounded_output(char *buf, MAX_LINE);
%cstring_output_maxsize(char *Err, int MaxErrLen);

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

// Make the class names nicer (DLL?)
%rename(KMotion) CKMotionDLL;
%rename(KStatus) MAIN_STATUS;

// Allow extension to this in Python
%feature("director") CKMotionDLL;

//%include "PC-DSP.h"

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

