/*

This is a set of dummy definitions for the canonical machining
functions given in canon.hh. These functions just print. On each
output line is printed:
1. an output line number (sequential, starting with 1).
2. an input line number read from the input (or ... if not provided).
3. a printed representation of the function call which was made.

If an interpreter which makes these calls is compiled with this set of
definitions, it can be used as a translator by redirecting output from
stdout to a file.

*/

/*
  Modification history:

   1-Nov-2000 WPS added unused variables a,b,c to ARC_FEED,STRAIHT_FEED, etc.
  24-Feb-2000  FMP added CANON_UPDATE_POSITION()
*/
#define _CRT_SECURE_NO_DEPRECATE 1

#include "stdafx.h"
#include "rs274ngc_return.h"

#define AND              &&
#define IS               ==
#define ISNT             !=
#define MAX(x, y)        ((x) > (y) ? (x) : (y))
#define NOT              !
#define OR               ||
#define SET_TO           =



CString Output;
CString ErrorOutput;
CGCodeInterpreter *GC;


void print_nc_line_number()
{
	char s[256];

//  int line_number SET_TO _interpreter_block.line_number;
  int line_number SET_TO 0;

	
if (line_number IS -1)
    sprintf(s," N ... ");
  else if (line_number < 10)
    sprintf(s,"    N%d ", line_number);
  else if (line_number < 100)
    sprintf(s,"   N%d ", line_number);
  else if (line_number < 1000)
    sprintf(s,"  N%d ", line_number);
  else if (line_number < 10000)
    sprintf(s," N%d ", line_number);
  else
    sprintf(s,"N%d ", line_number);

//  Output += s;
  Output += "-"; // this looks a little nicer in the KMotion app
}

#define PRINT0(control) if (1)               \
          {										\
		   char _s[256];						 \
		   sprintf(_s,"%5d ", line_number++);    \
		   Output += _s;                      \
           print_nc_line_number();           \
           sprintf(_s,control);                  \
		   if ('\n'==_s[strlen(_s)-1]) _s[strlen(_s)-1]=0; \
		   Output += _s;                      \
		   sprintf(_s,"@%.3f\r\n",((double)GetTickCount())/1000.0); \
		   Output += _s;                      \
          } else
#define PRINT1(control, arg1) if (1)         \
          {										\
		   char _s[256];						 \
		   sprintf(_s,"%5d ", line_number++);    \
		   Output += _s;                      \
           print_nc_line_number();           \
           sprintf(_s,control, arg1);            \
		   if ('\n'==_s[strlen(_s)-1]) _s[strlen(_s)-1]=0; \
		   Output += _s;                      \
		   sprintf(_s,"@%.3f\r\n",((double)GetTickCount())/1000.0); \
		   Output += _s;                      \
          } else
#define PRINT2(control, arg1, arg2) if (1)   \
          {										\
		   char _s[256];						 \
		   sprintf(_s,"%5d ", line_number++);    \
		   Output += _s;                      \
           print_nc_line_number();           \
           sprintf(_s,control, arg1, arg2);      \
		   if ('\n'==_s[strlen(_s)-1]) _s[strlen(_s)-1]=0; \
		   Output += _s;                      \
		   sprintf(_s,"@%.3f\r\n",((double)GetTickCount())/1000.0); \
		   Output += _s;                      \
          } else
#define PRINT3(control, arg1, arg2, arg3) if (1)         \
          {										\
		   char _s[256];						 \
		   sprintf(_s,"%5d ", line_number++);    \
		   Output += _s;                      \
           print_nc_line_number();                       \
           sprintf(_s,control, arg1, arg2, arg3);            \
		   if ('\n'==_s[strlen(_s)-1]) _s[strlen(_s)-1]=0; \
		   Output += _s;                      \
		   sprintf(_s,"@%.3f\r\n",((double)GetTickCount())/1000.0); \
		   Output += _s;                      \
          } else
#define PRINT4(control, arg1, arg2, arg3, arg4) if (1)   \
          {										\
		   char _s[256];						 \
		   sprintf(_s,"%5d ", line_number++);    \
		   Output += _s;                      \
           print_nc_line_number();                       \
           sprintf(_s,control, arg1, arg2, arg3, arg4);      \
		   if ('\n'==_s[strlen(_s)-1]) _s[strlen(_s)-1]=0; \
		   Output += _s;                      \
		   sprintf(_s,"@%.3f\r\n",((double)GetTickCount())/1000.0); \
		   Output += _s;                      \
          } else
#define PRINT6(control, arg1, arg2, arg3, arg4, arg5, arg6) if (1) \
          {										\
		   char _s[256];						 \
		   sprintf(_s,"%5d ", line_number++);    \
		   Output += _s;                      \
           print_nc_line_number();                                 \
           sprintf(_s,control, arg1, arg2, arg3, arg4, arg5, arg6);    \
		   if ('\n'==_s[strlen(_s)-1]) _s[strlen(_s)-1]=0; \
		   Output += _s;                      \
		   sprintf(_s,"@%.3f\r\n",((double)GetTickCount())/1000.0); \
		   Output += _s;                      \
          } else

		  
int line_number SET_TO 1;
static CANON_VECTOR program_origin;
static CANON_UNITS length_units = CANON_UNITS_MM;
static CANON_PLANE active_plane = CANON_PLANE_XY;

/* Representation */

void SET_ORIGIN_OFFSETS(double x, double y, double z,
			double a, double b, double c)
{PRINT3("SET_ORIGIN_OFFSETS(%.4f, %.4f, %.4f)\n", x, y, z);
  program_origin.x SET_TO x;
  program_origin.y SET_TO y;
  program_origin.z SET_TO z;
}

void USE_LENGTH_UNITS(CANON_UNITS in_unit)
{PRINT1("USE_LENGTH_UNITS(%s)\n",
        (in_unit IS CANON_UNITS_INCHES) ? "CANON_UNITS_INCHES" :
        (in_unit IS CANON_UNITS_MM)     ? "CANON_UNITS_MM" : "UNKNOWN");
}

/* Free Space Motion */
void SET_TRAVERSE_RATE(double rate)
{PRINT1("SET_TRAVERSE_RATE(%.4f)\n", rate);}

void STRAIGHT_TRAVERSE (double x, double y, double z,
                        double a, double b , double c)
{
	PRINT4("STRAIGHT_TRAVERSE(%.4f, %.4f, %.4f, %.4f)\n", x, y, z, a);

	GC->CoordMotion->StraightTraverse(GC->UserUnitsToInches(x+_setup.axis_offset_x+_setup.origin_offset_x),
						 GC->UserUnitsToInches(y+_setup.axis_offset_y+_setup.origin_offset_y),
						 GC->UserUnitsToInches(z+_setup.axis_offset_z+_setup.origin_offset_z+_setup.tool_length_offset),
						 GC->UserUnitsToInchesOrDegA(a+_setup.AA_axis_offset+_setup.AA_origin_offset),
						 GC->UserUnitsToInchesOrDegB(b+_setup.BB_axis_offset+_setup.BB_origin_offset),
						 GC->UserUnitsToInchesOrDegC(c+_setup.CC_axis_offset+_setup.CC_origin_offset),
						 false,_setup.sequence_number,0);

}

/* Machining Attributes */
void SET_FEED_RATE(double rate)
{
	PRINT1("SET_FEED_RATE(%.4f)\n", rate);
}

void SET_FEED_REFERENCE(CANON_FEED_REFERENCE reference)
{PRINT1("SET_FEED_REFERENCE(%s)\n",
        (reference IS CANON_WORKPIECE) ? "CANON_WORKPIECE" : "CANON_XYZ");}

void SET_MOTION_CONTROL_MODE(CANON_MOTION_MODE mode)
{PRINT1("SET_MOTION_CONTROL_MODE(%s)\n",
        (mode IS CANON_EXACT_PATH) ? "CANON_EXACT_PATH" :
        (mode IS CANON_EXACT_STOP) ? "CANON_EXACT_STOP" : "CANON_CONTINUOUS");}

void SET_SPINDLE_MODE(CANON_SPINDLE_MODE mode)
{PRINT1("SET_MOTION_CONTROL_MODE(%s)\n",
        (mode IS CANON_SPINDLE_NORMAL) ? "CANON_SPINDLE_NORMAL" :
		(mode IS CANON_SPINDLE_CSS) ? "CANON_SPINDLE_CSS" : "CANON_SPINDLE_INVALID");

	GC->SetCSS(mode);
}

void SELECT_PLANE(CANON_PLANE in_plane)
{PRINT1("SELECT_PLANE(%s)\n",
        (in_plane IS CANON_PLANE_XY) ? "CANON_PLANE_XY" :
        (in_plane IS CANON_PLANE_YZ) ? "CANON_PLANE_YZ" :
        (in_plane IS CANON_PLANE_XZ) ? "CANON_PLANE_XZ" : "UNKNOWN");
}

void SET_CUTTER_RADIUS_COMPENSATION(double radius)
{PRINT1("SET_CUTTER_RADIUS_COMPENSATION(%.4f)\n", radius);}

void START_CUTTER_RADIUS_COMPENSATION(int side)
{PRINT1("START_CUTTER_RADIUS_COMPENSATION(%s)\n",
        (side IS LEFT)  ? "LEFT"  :
        (side IS RIGHT) ? "RIGHT" : "UNKNOWN");
}

void STOP_CUTTER_RADIUS_COMPENSATION()
{PRINT0 ("STOP_CUTTER_RADIUS_COMPENSATION()\n");}

void START_SPEED_FEED_SYNCH()
{PRINT0 ("START_SPEED_FEED_SYNCH()\n");}

void STOP_SPEED_FEED_SYNCH()
{PRINT0 ("STOP_SPEED_FEED_SYNCH()\n");}

void SELECT_MOTION_MODE(CANON_MOTION_MODE mode)
{PRINT1("SELECT_MOTION_MODE(%s)\n",
        (mode IS CANON_EXACT_STOP) ? "CANON_EXACT_STOP" :
        (mode IS CANON_EXACT_PATH) ? "CANON_EXACT_PATH" :
        (mode IS CANON_CONTINUOUS) ? "CANON_CONTINUOUS" :
                                          "UNKNOWN");
}
void SELECT_SPINDLE_MODE(CANON_SPINDLE_MODE mode)
{PRINT1("SELECT_SPINDLE_MODE(%s)\n",
        (mode IS CANON_SPINDLE_NORMAL) ? "CANON_SPINDLE_NORMAL" :
        (mode IS CANON_SPINDLE_CSS) ? "CANON_SPINDLE_CSS" :
                                          "UNKNOWN");
}

/* Machining Functions */
void ARC_FEED(double first_end, double second_end,
              double first_axis, double second_axis, int rotation,
              double axis_end_point,
			  double a, double b, double c, int ID)
{
	PRINT6("ARC_FEED(%.4f, %.4f, %.4f, %.4f, %d, %.4f)\n",
        first_end, second_end, first_axis, second_axis, rotation,
        axis_end_point);

	if (_setup.plane == CANON_PLANE_XY)
	{
		GC->CoordMotion->ArcFeed(GC->UserUnitsToInches(_setup.feed_rate)/60.0, _setup.plane,
					GC->UserUnitsToInches(first_end+_setup.axis_offset_x+_setup.origin_offset_x), 
					GC->UserUnitsToInches(second_end+_setup.axis_offset_y+_setup.origin_offset_y), 
					GC->UserUnitsToInches(first_axis+_setup.axis_offset_x+_setup.origin_offset_x), 
					GC->UserUnitsToInches(second_axis+_setup.axis_offset_y+_setup.origin_offset_y), 
					(rotation==1 ? 1 : 0),
					GC->UserUnitsToInches(axis_end_point+_setup.axis_offset_z+_setup.origin_offset_z+_setup.tool_length_offset),
					GC->UserUnitsToInchesOrDegA(a + _setup.AA_axis_offset + _setup.AA_origin_offset),
					GC->UserUnitsToInchesOrDegB(b + _setup.BB_axis_offset + _setup.BB_origin_offset),
					GC->UserUnitsToInchesOrDegC(c + _setup.CC_axis_offset + _setup.CC_origin_offset),
					_setup.sequence_number, ID);
	}
	else if (_setup.plane == CANON_PLANE_XZ)
	{
		// Actually more like ZX plane
		GC->CoordMotion->ArcFeed(GC->UserUnitsToInches(_setup.feed_rate)/60.0, _setup.plane,
					GC->UserUnitsToInches(first_end+_setup.axis_offset_z+_setup.origin_offset_z+_setup.tool_length_offset), 
					GC->UserUnitsToInches(second_end+_setup.axis_offset_x+_setup.origin_offset_x), 
					GC->UserUnitsToInches(first_axis+_setup.axis_offset_z+_setup.origin_offset_z+_setup.tool_length_offset), 
					GC->UserUnitsToInches(second_axis+_setup.axis_offset_x+_setup.origin_offset_x), 
					(rotation==1 ? 1 : 0),
					GC->UserUnitsToInches(axis_end_point+_setup.axis_offset_y+_setup.origin_offset_y),
					GC->UserUnitsToInchesOrDegA(a + _setup.AA_axis_offset + _setup.AA_origin_offset),
					GC->UserUnitsToInchesOrDegB(b + _setup.BB_axis_offset + _setup.BB_origin_offset),
					GC->UserUnitsToInchesOrDegC(c + _setup.CC_axis_offset + _setup.CC_origin_offset),
					_setup.sequence_number, ID);
	}
	else
	{
		GC->CoordMotion->ArcFeed(GC->UserUnitsToInches(_setup.feed_rate)/60.0, _setup.plane,
					GC->UserUnitsToInches(first_end+_setup.axis_offset_y+_setup.origin_offset_y), 
					GC->UserUnitsToInches(second_end+_setup.axis_offset_z+_setup.origin_offset_z+_setup.tool_length_offset), 
					GC->UserUnitsToInches(first_axis+_setup.axis_offset_y+_setup.origin_offset_y), 
					GC->UserUnitsToInches(second_axis+_setup.axis_offset_z+_setup.origin_offset_z+_setup.tool_length_offset), 
					(rotation==1 ? 1 : 0),
					GC->UserUnitsToInches(axis_end_point+_setup.axis_offset_x+_setup.origin_offset_x),
					GC->UserUnitsToInchesOrDegA(a + _setup.AA_axis_offset + _setup.AA_origin_offset),
					GC->UserUnitsToInchesOrDegB(b + _setup.BB_axis_offset + _setup.BB_origin_offset),
					GC->UserUnitsToInchesOrDegC(c + _setup.CC_axis_offset + _setup.CC_origin_offset),
					_setup.sequence_number, ID);
	}
}

void STRAIGHT_FEED (double x, double y, double z,
		            double a, double b, double c, int ID)
{
	PRINT4("STRAIGHT_FEED(%.4f, %.4f, %.4f, %.4f)\n", x, y, z, a);

	GC->CoordMotion->StraightFeed(GC->UserUnitsToInches(_setup.feed_rate)/60.0,
					 GC->UserUnitsToInches(x+_setup.axis_offset_x+_setup.origin_offset_x),
					 GC->UserUnitsToInches(y+_setup.axis_offset_y+_setup.origin_offset_y),
					 GC->UserUnitsToInches(z+_setup.axis_offset_z+_setup.origin_offset_z+_setup.tool_length_offset),
					 GC->UserUnitsToInchesOrDegA(a+_setup.AA_axis_offset+_setup.AA_origin_offset),
					 GC->UserUnitsToInchesOrDegB(b+_setup.BB_axis_offset+_setup.CC_origin_offset),
					 GC->UserUnitsToInchesOrDegC(c+_setup.CC_axis_offset+_setup.CC_origin_offset),
					 _setup.sequence_number,ID);
}

void STRAIGHT_PROBE (double x, double y, double z,
		     double a, double b, double c)
{PRINT3("STRAIGHT_PROBE(%.4f, %.4f, %.4f)\n", x, y, z);}


/*
void PARAMETRIC_2D_CURVE_FEED(FunctionPtr f1, FunctionPtr f2,
                              double start_parameter_value,
                              double end_parameter_value) {}

void PARAMETRIC_3D_CURVE_FEED(FunctionPtr xfcn, FunctionPtr yfcn,
        FunctionPtr zfcn, double start_parameter_value,
                              double end_parameter_value) {}
*/

void DWELL(double seconds)
{
	PRINT1("DWELL(%.4f)\n", seconds);

	GC->CoordMotion->Dwell(seconds,_setup.sequence_number);
}

/* Spindle Functions */
void SPINDLE_RETRACT_TRAVERSE()
{PRINT0("SPINDLE_RETRACT_TRAVERSE()\n");}

void START_SPINDLE_CLOCKWISE()
{
	PRINT0("START_SPINDLE_CLOCKWISE()\n");
	GC->InvokeAction(3);  // do the defined action for M Code
}

void START_SPINDLE_COUNTERCLOCKWISE()
{
	PRINT0("START_SPINDLE_COUNTERCLOCKWISE()\n");
	GC->InvokeAction(4);  // do the defined action for M Code
}

void SET_SPINDLE_SPEED(double r)
{
	PRINT1("SET_SPINDLE_SPEED(%.4f)\n", r);
	GC->InvokeAction(10);  // do the defined action for S Speed
}

void STOP_SPINDLE_TURNING()
{
	PRINT0("STOP_SPINDLE_TURNING()\n");
	GC->InvokeAction(5);  // do the defined action for M Code
}

void SPINDLE_RETRACT()
{PRINT0("SPINDLE_RETRACT()\n");}

void ORIENT_SPINDLE(double orientation, CANON_DIRECTION direction)
{PRINT2("ORIENT_SPINDLE(%.4f, %s)\n", orientation,
        (direction IS CANON_CLOCKWISE) ? "CANON_CLOCKWISE" :
                                         "CANON_COUNTERCLOCKWISE");
}

void USE_NO_SPINDLE_FORCE()
{PRINT0("USE_NO_SPINDLE_FORCE()\n");}

/* Tool Functions */

void USE_TOOL_LENGTH_OFFSET(double length)
{PRINT1("USE_TOOL_LENGTH_OFFSET(%.4f)\n", length);}

void CHANGE_TOOL(int slot)
{
	PRINT1("CHANGE_TOOL(%d)\n", slot);
	GC->InvokeAction(6);  // do the defined action for M Code
}

int M100(int mcode)	/* User M code  */
{
	PRINT1("MCODE(%d)\n", mcode);
	return GC->InvokeAction(mcode);  // do the defined action for M Code
}


void SELECT_TOOL(int slot)
{PRINT1("SELECT_TOOL(%d)\n", slot);}


/* Misc Functions */

void CLAMP_AXIS(CANON_AXIS axis)
{PRINT1("CLAMP_AXIS(%s)\n",
        (axis IS CANON_AXIS_X) ? "CANON_AXIS_X" :
        (axis IS CANON_AXIS_Y) ? "CANON_AXIS_Y" :
        (axis IS CANON_AXIS_Z) ? "CANON_AXIS_Z" : "UNKNOWN");}


// check for CMD,xxxxxx  any case and after removing whitespace 


int CheckForPassThroughCommand(char *comment)
{				/* string with comment */
    int m;
    int item;

    for (m = 0; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if ((item != 'C') && (item != 'c')) return 0;
    for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if ((item != 'M') && (item != 'm')) return 0;
    for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if ((item != 'D') && (item != 'd')) return 0;
    for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if (item != ',') return 0;
	
	PRINT1("COMMAND(\"%s\")\n", comment + m + 1);
	GC->CoordMotion->DoKMotionCmd(comment + m + 1, TRUE);
    return 1; 
}


int CheckForBufferedCommand(char *comment)
{				/* string with comment */
    int m;
    int item;

    for (m = 0; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if ((item != 'B') && (item != 'b')) return 0;
    for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if ((item != 'U') && (item != 'u')) return 0;
    for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if ((item != 'F') && (item != 'f')) return 0;
    for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if (item != ',') return 0;
	
	PRINT1("BUFFER(\"%s\")\n", comment + m + 1);
	GC->CoordMotion->DoKMotionBufCmd(comment + m + 1);
    return 1; 
}

int CheckForUserCallback(char *comment)
{				/* string with comment */
    int m;
    int item;

    for (m = 0; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if ((item != 'U') && (item != 'u')) return 0;
    for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if ((item != 'S') && (item != 's')) return 0;
    for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if ((item != 'R') && (item != 'r')) return 0;
    for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if (item != ',') return 0;
	
	PRINT1("USR(\"%s\")\n", comment + m + 1);
	if (GC->m_UserFn)
	{
		if (GC->CoordMotion->FlushSegments()) {GC->CoordMotion->SetAbort(); return 1;}  
		if (GC->CoordMotion->WaitForSegmentsFinished(TRUE)) {GC->CoordMotion->SetAbort(); return 1;}
		if (GC->m_UserFn(comment + m + 1)){GC->CoordMotion->SetAbort(); return 1;}

		// don't sample positions until everything is stopped 
		if (GC->CoordMotion->WaitForSegmentsFinished()) return 1;
		if (GC->CoordMotion->WaitForMoveXYZABCFinished()) return 1;

		if (!GC->CoordMotion->m_Simulate && GC->ReadAndSyncCurPositions(&_setup.current_x,&_setup.current_y,&_setup.current_z,
			                                               &_setup.AA_current,&_setup.BB_current,&_setup.CC_current))
			return 1;
	}
    return 1; 
}


void COMMENT(char *s)
{
	if (CheckForBufferedCommand(s)) return;	
	if (CheckForPassThroughCommand(s)) return;	
	if (CheckForUserCallback(s)) return;	
	PRINT1("COMMENT(\"%s\")\n", s);
}



void DISABLE_FEED_OVERRIDE()
{PRINT0("DISABLE_FEED_OVERRIDE()\n");}

void DISABLE_SPEED_OVERRIDE()
{PRINT0("DISABLE_SPEED_OVERRIDE()\n");}

void ENABLE_FEED_OVERRIDE()
{PRINT0("ENABLE_FEED_OVERRIDE()\n");}

void ENABLE_SPEED_OVERRIDE()
{PRINT0("ENABLE_SPEED_OVERRIDE()\n");}

void FLOOD_OFF()
{
	PRINT0("FLOOD_OFF()\n");
}

void FLOOD_ON()
{
	PRINT0("FLOOD_ON()\n");
	GC->InvokeAction(8);  // do the defined action for M Code
}

void MESSAGE(char *s)
{
	if (GC->CoordMotion->FlushSegments()) {GC->CoordMotion->SetAbort(); return;}  
	if (GC->CoordMotion->WaitForSegmentsFinished(TRUE)) {GC->CoordMotion->SetAbort(); return;}

	PRINT1("MESSAGE(\"%s\")\n", s);
	if (AfxMessageBox(s,MB_OKCANCEL|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL)==IDCANCEL)
	{
		GC->Abort();
	}
}

void MIST_OFF()
{
	PRINT0("MIST_OFF()\n");
	GC->InvokeAction(9);  // do the defined action for M Code
}

void MIST_ON()
{
	PRINT0("MIST_ON()\n");
	GC->InvokeAction(7);  // do the defined action for M Code
}

void PALLET_SHUTTLE()
{PRINT0("PALLET_SHUTTLE()\n");}

void TURN_PROBE_OFF()
{PRINT0("TURN_PROBE_OFF()\n");}

void TURN_PROBE_ON()
{PRINT0("TURN_PROBE_ON()\n");}

void UNCLAMP_AXIS(CANON_AXIS axis)
{PRINT1("UNCLAMP_AXIS(%s)\n",
        (axis IS CANON_AXIS_X) ? "CANON_AXIS_X" :
        (axis IS CANON_AXIS_Y) ? "CANON_AXIS_Y" :
        (axis IS CANON_AXIS_Z) ? "CANON_AXIS_Z" : "UNKNOWN");}

/* Program Functions */

void PROGRAM_STOP()
{
	PRINT0("PROGRAM_STOP()\n");
//	GC->m_Halt=true;
	GC->m_end=0;  // force line number to be passed
}

void OPTIONAL_PROGRAM_STOP()
{PRINT0("OPTIONAL_PROGRAM_STOP()\n");}

void PROGRAM_END()
{
	PRINT0("PROGRAM_END()\n");

	GC->CoordMotion->FlushSegments();
}


/* returns the current x, y, z origin offsets */
CANON_VECTOR GET_PROGRAM_ORIGIN()
{
  return program_origin;
}

/* returns the current active units */
CANON_UNITS GET_LENGTH_UNITS()
{
  return length_units;
}

CANON_PLANE GET_PLANE()
{
  return active_plane;
}

/*********************************************************************/

/*

The purpose of these GET_XXX (and other) functions is to provide for
local emulation of the world modeling done by the EMC system.

/*********************************************************************/

/* GET_EXTERNAL_FEED_RATE

called by: rs274ngc_synch

This is a stub.

*/

double GET_EXTERNAL_FEED_RATE()
{
  return _setup.feed_rate;
}

/*********************************************************************/

int GET_EXTERNAL_FLOOD()
{
  return OFF;
}

/*********************************************************************/

int GET_EXTERNAL_MIST()
{
  return OFF;
}

/*********************************************************************/

int GET_EXTERNAL_POCKET()
{
  return 1;
}

/*********************************************************************/

CANON_POSITION GET_EXTERNAL_POSITION()
{
  return CANON_POSITION(_setup.current_x,
                        _setup.current_y,
                        _setup.current_z,
                        0.0,
                        0.0,
                        0.0);
}

/*********************************************************************/

CANON_POSITION GET_EXTERNAL_PROBE_POSITION()
{
  return CANON_POSITION(_setup.current_x,
                        _setup.current_y,
                        _setup.current_z,
                        0.0,
                        0.0,
                        0.0);
}

/*********************************************************************/

double GET_EXTERNAL_PROBE_VALUE()
{
  return 1.0;
}

/*********************************************************************/

double GET_EXTERNAL_SPEED()
{
  // speed is in RPMs everywhere
  return 1000;
}

/*********************************************************************/

CANON_DIRECTION GET_EXTERNAL_SPINDLE()
{
  return CANON_STOPPED;
}

/*********************************************************************/

int GET_EXTERNAL_TOOL()
{
  return 1;
}

/*********************************************************************/

int GET_EXTERNAL_TOOL_MAX()
{
  return CANON_TOOL_MAX;
}

/*********************************************************************/

CANON_TOOL_TABLE GET_EXTERNAL_TOOL_TABLE(int pocket)
{
  CANON_TOOL_TABLE retval;

  if (pocket == 0)
    {
      retval.id = 1;
      retval.length = 2.0;
      retval.diameter = 1.0;
    }
  else if (pocket == 1)
    {
      retval.id = 1;
      retval.length = 2.0;
      retval.diameter = 1.0;
    }
  else if (pocket == 2)
    {
      retval.id = 2;
      retval.length = 1.0;
      retval.diameter = 2.0;
    }
  else
    {
      retval.id = 0;
      retval.length = 0.0;
      retval.diameter = 0.0;
    }

  return retval;
}


/*********************************************************************/

double GET_EXTERNAL_TRAVERSE_RATE()
{
  return 100.0;
}

/*********************************************************************/

CANON_MOTION_MODE GET_MOTION_CONTROL_MODE()
{
  return CANON_CONTINUOUS;
//  return CANON_EXACT_PATH;

}

/*********************************************************************/

CANON_SPINDLE_MODE GET_SPINDLE_MODE()
{
  return CANON_SPINDLE_NORMAL;
}

/*********************************************************************/

/* INIT_CANON()

called by: rs274ngc_init

This is a stub.

*/

void INIT_CANON()
{}

/*********************************************************************/

/*

IS_EXTERNAL_QUEUE_EMPTY emulates the EMC external to the
interpreter. It just returns 1, meaning the queue is empty (any
non-zero means the queue is empty).

*/

int IS_EXTERNAL_QUEUE_EMPTY()
{
  return 1;
}

/*********************************************************************/

/*

CANON_UPDATE_POSITION tells the canonical interface that it should
update the end position it may have been saving, due to an abort other
other external event that may have made the end position different.
Here it does nothing since the standalone interpreter doesn't keep track
of the end position.

*/

void CANON_UPDATE_POSITION()
{
  return;
}

/*********************************************************************/
CANON_MOTION_MODE MotionMode = CANON_EXACT_STOP;
// Returns the current motion control mode
CANON_MOTION_MODE GET_EXTERNAL_MOTION_CONTROL_MODE()
{
	return MotionMode;
}




CANON_SPINDLE_MODE SpindleMode = CANON_SPINDLE_NORMAL;
// Returns the current motion control mode
CANON_SPINDLE_MODE GET_EXTERNAL_SPINDLE_MODE()
{
	return SpindleMode;
}


// returns nothing but copies the name of the parameter file into
// the filename array, stopping at max_size if the name is longer
// An empty string may be placed in filename.
void GET_EXTERNAL_PARAMETER_FILE_NAME(char *filename, int max_size)
{
	CString Name;

	if (GC->VarsFile[0] == 0)
		Name=(CString)(GC->CoordMotion->MainPathRoot) + "\\GCode Programs\\emc.var";
	else
		Name=GC->VarsFile;

	strcpy(filename,Name.GetBuffer(0));
}

// returns the currently active plane
CANON_PLANE GET_EXTERNAL_PLANE()  { return CANON_PLANE_XY; };

// returns the current a-axis position
double GET_EXTERNAL_POSITION_A()  { return 0.0; };

// returns the current b-axis position
double GET_EXTERNAL_POSITION_B()  { return 0.0; };

// returns the current c-axis position
double GET_EXTERNAL_POSITION_C()  { return 0.0; };

// returns the current x-axis position
double GET_EXTERNAL_POSITION_X()  { return 0.0; };

// returns the current y-axis position
double GET_EXTERNAL_POSITION_Y()  { return 0.0; };

// returns the current z-axis position
double GET_EXTERNAL_POSITION_Z()  { return 0.0; };

// Returns the machine A-axis position at the last probe trip.
double GET_EXTERNAL_PROBE_POSITION_A()  { return 0.0; };

// Returns the machine B-axis position at the last probe trip.
double GET_EXTERNAL_PROBE_POSITION_B()  { return 0.0; };

// Returns the machine C-axis position at the last probe trip.
double GET_EXTERNAL_PROBE_POSITION_C()  { return 0.0; };

// Returns the machine X-axis position at the last probe trip.
double GET_EXTERNAL_PROBE_POSITION_X()  { return 0.0; };

// Returns the machine Y-axis position at the last probe trip.
double GET_EXTERNAL_PROBE_POSITION_Y()  { return 0.0; };

// Returns the machine Z-axis position at the last probe trip.
double GET_EXTERNAL_PROBE_POSITION_Z()  { return 0.0; };


// Returns zero if queue is not empty, non-zero if the queue is empty
// This always returns a valid value
int GET_EXTERNAL_QUEUE_EMPTY()  { return 1; };

// returns current tool length offset
double GET_EXTERNAL_TOOL_LENGTH_OFFSET()  { return 0.0; };

// Returns the system length unit type
CANON_UNITS GET_EXTERNAL_LENGTH_UNIT_TYPE() {return CANON_UNITS_MM; };

// Returns the system value for the carousel slot in which the tool
// currently in the spindle belongs. Return value zero means there is no
// tool in the spindle.
int GET_EXTERNAL_TOOL_SLOT() { return 0; };

void SET_CANON_DEVICE(int *device)
{
	CGCodeInterpreter *GC=(CGCodeInterpreter *)device; 
}
int CHECK_INIT_ON_EXE()
{
	if (GC->m_InitializeOnExecute)
	{
	   return GC->InitializeInterp();
	}
	else
	{
       return 0;
	}
}
int CHECK_PREVIOUS_STOP(double mx, double my, int log)
{
	if (!GC->CoordMotion->m_PreviouslyStopped || GC->CoordMotion->m_PreviouslyStoppedID==1)
	{ 
       return 1;
	}
	else 
	{
		if(log)
		{
		  GC->CoordMotion->m_StoppedMidx = mx;
		  GC->CoordMotion->m_StoppedMidy = my;
		} 
		return 0;
	}
}