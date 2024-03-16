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
#define NO_WARN_MBCS_MFC_DEPRECATION

#include "StdAfx.h"
#include "rs274ngc_return.h"

#define AND              &&
#define IS               ==
#define ISNT             !=
#define MAX(x, y)        ((x) > (y) ? (x) : (y))
#define NOT              !
#define OR               ||
#define SET_TO           =

int HandleThreading(double *FeedRate);
int CheckIfThreadingInProgress(void);

std::string Output;
std::string ErrorOutput;
CGCodeInterpreter *GC;
CCoordMotion *CM;;


void print_nc_line_number()
{
	wchar_t s[256];

//  int line_number SET_TO _interpreter_block.line_number;
  int line_number SET_TO 0;

	
if (line_number IS -1)
    swprintf(s, 255, L" N ... ");
  else if (line_number < 10)
    swprintf(s, 255, L"    N%d ", line_number);
  else if (line_number < 100)
    swprintf(s, 255, L"   N%d ", line_number);
  else if (line_number < 1000)
    swprintf(s, 255, L"  N%d ", line_number);
  else if (line_number < 10000)
    swprintf(s, 255, L" N%d ", line_number);
  else
    swprintf(s, 255, L"N%d ", line_number);

//  Output += s;
  Output += "-"; // this looks a little nicer in the KMotion app
}

#define PRINT0(control) if (1)               \
          {										\
		   wchar_t _s[256];						 \
		   swprintf(_s, 255, L"%5d ", line_number++);    \
		   Output += kmx::wstrtostr(_s);                      \
           print_nc_line_number();           \
           swprintf(_s, 255, control);                  \
		   if ('\n'==_s[wcslen(_s)-1]) _s[wcslen(_s)-1]=0; \
		   Output += kmx::wstrtostr(_s);                      \
		   swprintf(_s, 255, L"@%.3f\r\n",((double)GetTickCount())/1000.0); \
		   Output += kmx::wstrtostr(_s);                      \
          } else
#define PRINT1(control, arg1) if (1)         \
          {										\
		   wchar_t _s[256];						 \
		   swprintf(_s, 255, L"%5d ", line_number++);    \
		   Output += kmx::wstrtostr(_s);                      \
           print_nc_line_number();           \
           swprintf(_s, 255, (control), (arg1));            \
		   if ('\n'==_s[wcslen(_s)-1]) _s[wcslen(_s)-1]=0; \
		   Output += kmx::wstrtostr(_s);                      \
		   swprintf(_s, 255, L"@%.3f\r\n",((double)GetTickCount())/1000.0); \
		   Output += kmx::wstrtostr(_s);                      \
          } else
#define PRINT2(control, arg1, arg2) if (1)   \
          {										\
		   wchar_t _s[256];						 \
		   swprintf(_s, 255, L"%5d ", line_number++);    \
		   Output += kmx::wstrtostr(_s);                      \
           print_nc_line_number();           \
           swprintf(_s, 255, control, arg1, arg2);      \
		   if ('\n'==_s[wcslen(_s)-1]) _s[wcslen(_s)-1]=0; \
		   Output += kmx::wstrtostr(_s);                      \
		   swprintf(_s, 255, L"@%.3f\r\n",((double)GetTickCount())/1000.0); \
		   Output += kmx::wstrtostr(_s);                      \
          } else
#define PRINT3(control, arg1, arg2, arg3) if (1)         \
          {										\
		   wchar_t _s[256];						 \
		   swprintf(_s, 255, L"%5d ", line_number++);    \
		   Output += kmx::wstrtostr(_s);                      \
           print_nc_line_number();                       \
           swprintf(_s, 255, control, arg1, arg2, arg3);            \
		   if ('\n'==_s[wcslen(_s)-1]) _s[wcslen(_s)-1]=0; \
		   Output += kmx::wstrtostr(_s);                      \
		   swprintf(_s, 255, L"@%.3f\r\n",((double)GetTickCount())/1000.0); \
		   Output += kmx::wstrtostr(_s);                      \
          } else
#define PRINT4(control, arg1, arg2, arg3, arg4) if (1)   \
          {										\
		   wchar_t _s[256];						 \
		   swprintf(_s, 255, L"%5d ", line_number++);    \
		   Output += kmx::wstrtostr(_s);                      \
           print_nc_line_number();                       \
           swprintf(_s, 255, control, arg1, arg2, arg3, arg4);      \
		   if ('\n'==_s[wcslen(_s)-1]) _s[wcslen(_s)-1]=0; \
		   Output += kmx::wstrtostr(_s);                      \
		   swprintf(_s, 255, L"@%.3f\r\n",((double)GetTickCount())/1000.0); \
		   Output += kmx::wstrtostr(_s);                      \
          } else
#define PRINT6(control, arg1, arg2, arg3, arg4, arg5, arg6) if (1) \
          {										\
		   wchar_t _s[256];						 \
		   swprintf(_s, 255, L"%5d ", line_number++);    \
		   Output += kmx::wstrtostr(_s);                      \
           print_nc_line_number();                                 \
           swprintf(_s, 255, control, arg1, arg2, arg3, arg4, arg5, arg6);    \
		   if ('\n'==_s[wcslen(_s)-1]) _s[wcslen(_s)-1]=0; \
		   Output += kmx::wstrtostr(_s);                      \
		   swprintf(_s, 255, L"@%.3f\r\n",((double)GetTickCount())/1000.0); \
		   Output += kmx::wstrtostr(_s);                      \
          } else

		  
int line_number SET_TO 1;
static CANON_VECTOR program_origin;
static CANON_UNITS length_units = CANON_UNITS_MM;
static CANON_PLANE active_plane = CANON_PLANE_XY;

/* Representation */
//ExcludeTranslate
void SET_ORIGIN_OFFSETS(double x, double y, double z,
			double a, double b, double c, double u, double v)
{PRINT3(L"SET_ORIGIN_OFFSETS(%.4f, %.4f, %.4f)\n", x, y, z);
  program_origin.x SET_TO x;
  program_origin.y SET_TO y;
  program_origin.z SET_TO z;
}

void USE_LENGTH_UNITS(CANON_UNITS in_unit)
{PRINT1(L"USE_LENGTH_UNITS(%s)\n",
        (in_unit IS CANON_UNITS_INCHES) ? L"CANON_UNITS_INCHES" :
        (in_unit IS CANON_UNITS_MM)     ? L"CANON_UNITS_MM" : L"UNKNOWN");
}

/* Free Space Motion */
void SET_TRAVERSE_RATE(double rate)
{PRINT1(L"SET_TRAVERSE_RATE(%.4f)\n", rate);}

void STRAIGHT_TRAVERSE (double x, double y, double z,
                        double a, double b, double c, double u, double v)
{
	PRINT4(L"STRAIGHT_TRAVERSE(%.4f, %.4f, %.4f, %.4f)\n", x, y, z, a);

	if (CheckIfThreadingInProgress()) return;

	GC->SaveStateOnceOnly();  // save the state here before creating any motion segments

	CM->StraightTraverse(GC->UserUnitsToInchesX(x+_setup.axis_offset_x+_setup.origin_offset_x+_setup.tool_xoffset),
						GC->UserUnitsToInches(y+_setup.axis_offset_y+_setup.origin_offset_y+_setup.tool_yoffset),
						GC->UserUnitsToInches(z+_setup.axis_offset_z+_setup.origin_offset_z+_setup.tool_length_offset),
						GC->UserUnitsToInchesOrDegA(a+_setup.AA_axis_offset+_setup.AA_origin_offset),
						GC->UserUnitsToInchesOrDegB(b + _setup.BB_axis_offset + _setup.BB_origin_offset),
						GC->UserUnitsToInchesOrDegC(c + _setup.CC_axis_offset + _setup.CC_origin_offset),
						GC->UserUnitsToInches(u + _setup.UU_axis_offset + _setup.UU_origin_offset),
						GC->UserUnitsToInches(v + _setup.VV_axis_offset + _setup.VV_origin_offset),
		false,_setup.sequence_number,0);
}

/* Machining Attributes */
void SET_FEED_RATE(double rate)
{
	PRINT1(L"SET_FEED_RATE(%.4f)\n", rate);
}

void SET_FEED_REFERENCE(CANON_FEED_REFERENCE reference)
{PRINT1(L"SET_FEED_REFERENCE(%s)\n",
        (reference IS CANON_WORKPIECE) ? L"CANON_WORKPIECE" : L"CANON_XYZ");}

void SET_MOTION_CONTROL_MODE(CANON_MOTION_MODE mode)
{PRINT1(L"SET_MOTION_CONTROL_MODE(%s)\n",
        (mode IS CANON_EXACT_PATH) ? L"CANON_EXACT_PATH" :
        (mode IS CANON_EXACT_STOP) ? L"CANON_EXACT_STOP" : L"CANON_CONTINUOUS");}

void SET_SPINDLE_MODE(CANON_SPINDLE_MODE mode)
{PRINT1(L"SET_MOTION_CONTROL_MODE(%s)\n",
        (mode IS CANON_SPINDLE_NORMAL) ? L"CANON_SPINDLE_NORMAL" :
		(mode IS CANON_SPINDLE_CSS) ? L"CANON_SPINDLE_CSS" : L"CANON_SPINDLE_INVALID");

	GC->SetCSS(mode);
}

void SELECT_PLANE(CANON_PLANE in_plane)
{PRINT1(L"SELECT_PLANE(%s)\n",
        (in_plane IS CANON_PLANE_XY) ? L"CANON_PLANE_XY" :
        (in_plane IS CANON_PLANE_YZ) ? L"CANON_PLANE_YZ" :
        (in_plane IS CANON_PLANE_XZ) ? L"CANON_PLANE_XZ" : L"UNKNOWN");
}

void SET_CUTTER_RADIUS_COMPENSATION(double radius)
{PRINT1(L"SET_CUTTER_RADIUS_COMPENSATION(%.4f)\n", radius);}

void START_CUTTER_RADIUS_COMPENSATION(int side)
{PRINT1(L"START_CUTTER_RADIUS_COMPENSATION(%s)\n",
        (side IS LEFT)  ? L"LEFT"  :
        (side IS RIGHT) ? L"RIGHT" : L"UNKNOWN");
}

void STOP_CUTTER_RADIUS_COMPENSATION()
{PRINT0(L"STOP_CUTTER_RADIUS_COMPENSATION()\n");}

void START_SPEED_FEED_SYNCH()
{PRINT0(L"START_SPEED_FEED_SYNCH()\n");}

void STOP_SPEED_FEED_SYNCH()
{PRINT0(L"STOP_SPEED_FEED_SYNCH()\n");}

void SELECT_MOTION_MODE(CANON_MOTION_MODE mode)
{PRINT1(L"SELECT_MOTION_MODE(%s)\n",
        (mode IS CANON_EXACT_STOP) ? L"CANON_EXACT_STOP" :
        (mode IS CANON_EXACT_PATH) ? L"CANON_EXACT_PATH" :
        (mode IS CANON_CONTINUOUS) ? L"CANON_CONTINUOUS" :
                                          L"UNKNOWN");
}
void SELECT_SPINDLE_MODE(CANON_SPINDLE_MODE mode)
{PRINT1(L"SELECT_SPINDLE_MODE(%s)\n",
        (mode IS CANON_SPINDLE_NORMAL) ? L"CANON_SPINDLE_NORMAL" :
        (mode IS CANON_SPINDLE_CSS) ? L"CANON_SPINDLE_CSS" :
                                          L"UNKNOWN");
}

/* Machining Functions */
void ARC_FEED(double first_end, double second_end,
              double first_axis, double second_axis, int rotation,
              double axis_end_point,
			  double a, double b, double c, double u, double v, int ID)
{
	double FeedRate;

	PRINT6(L"ARC_FEED(%.4f, %.4f, %.4f, %.4f, %d, %.4f)\n",
        first_end, second_end, first_axis, second_axis, rotation,
        axis_end_point);

	if (HandleThreading(&FeedRate)) return;

	if (!CM->m_ThreadingMode)  // Threading?
	{
		FeedRate = GC->UserUnitsToInches(_setup.feed_rate) / 60.0;
	}


	GC->SaveStateOnceOnly();  // save the state here before creating any motion segments

	if (_setup.plane == CANON_PLANE_XY)
	{
		CM->ArcFeed(FeedRate, _setup.plane,
					GC->UserUnitsToInchesX(first_end+_setup.axis_offset_x+_setup.origin_offset_x+_setup.tool_xoffset), 
					GC->UserUnitsToInches(second_end+_setup.axis_offset_y+_setup.origin_offset_y+_setup.tool_yoffset), 
					GC->UserUnitsToInchesX(first_axis+_setup.axis_offset_x+_setup.origin_offset_x+_setup.tool_xoffset), 
					GC->UserUnitsToInches(second_axis+_setup.axis_offset_y+_setup.origin_offset_y+_setup.tool_yoffset), 
					(rotation==1 ? 1 : 0),
					GC->UserUnitsToInches(axis_end_point+_setup.axis_offset_z+_setup.origin_offset_z+_setup.tool_length_offset),
					GC->UserUnitsToInchesOrDegA(a + _setup.AA_axis_offset + _setup.AA_origin_offset),
					GC->UserUnitsToInchesOrDegB(b + _setup.BB_axis_offset + _setup.BB_origin_offset),
					GC->UserUnitsToInchesOrDegC(c + _setup.CC_axis_offset + _setup.CC_origin_offset),
					GC->UserUnitsToInches(u + _setup.UU_axis_offset + _setup.UU_origin_offset),
					GC->UserUnitsToInches(v + _setup.VV_axis_offset + _setup.VV_origin_offset),
					_setup.sequence_number, ID);
	}
	else if (_setup.plane == CANON_PLANE_XZ)
	{
		// Actually more like ZX plane
		CM->ArcFeed(FeedRate, _setup.plane,
					GC->UserUnitsToInches(first_end+_setup.axis_offset_z+_setup.origin_offset_z+_setup.tool_length_offset), 
					GC->UserUnitsToInchesX(second_end+_setup.axis_offset_x+_setup.origin_offset_x+_setup.tool_xoffset), 
					GC->UserUnitsToInches(first_axis+_setup.axis_offset_z+_setup.origin_offset_z+_setup.tool_length_offset), 
					GC->UserUnitsToInchesX(second_axis+_setup.axis_offset_x+_setup.origin_offset_x+_setup.tool_xoffset), 
					(rotation==1 ? 1 : 0),
					GC->UserUnitsToInches(axis_end_point+_setup.axis_offset_y+_setup.origin_offset_y+_setup.tool_yoffset),
					GC->UserUnitsToInchesOrDegA(a + _setup.AA_axis_offset + _setup.AA_origin_offset),
					GC->UserUnitsToInchesOrDegB(b + _setup.BB_axis_offset + _setup.BB_origin_offset),
					GC->UserUnitsToInchesOrDegC(c + _setup.CC_axis_offset + _setup.CC_origin_offset),
					GC->UserUnitsToInches(u + _setup.UU_axis_offset + _setup.UU_origin_offset),
					GC->UserUnitsToInches(v + _setup.VV_axis_offset + _setup.VV_origin_offset),
					_setup.sequence_number, ID);
	}
	else
	{
		CM->ArcFeed(FeedRate, _setup.plane,
					GC->UserUnitsToInches(first_end+_setup.axis_offset_y+_setup.origin_offset_y+_setup.tool_yoffset), 
					GC->UserUnitsToInches(second_end+_setup.axis_offset_z+_setup.origin_offset_z+_setup.tool_length_offset), 
					GC->UserUnitsToInches(first_axis+_setup.axis_offset_y+_setup.origin_offset_y+_setup.tool_yoffset), 
					GC->UserUnitsToInches(second_axis+_setup.axis_offset_z+_setup.origin_offset_z+_setup.tool_length_offset), 
					(rotation==1 ? 1 : 0),
					GC->UserUnitsToInchesX(axis_end_point+_setup.axis_offset_x+_setup.origin_offset_x+_setup.tool_xoffset),
					GC->UserUnitsToInchesOrDegA(a + _setup.AA_axis_offset + _setup.AA_origin_offset),
					GC->UserUnitsToInchesOrDegB(b + _setup.BB_axis_offset + _setup.BB_origin_offset),
					GC->UserUnitsToInchesOrDegC(c + _setup.CC_axis_offset + _setup.CC_origin_offset),
					GC->UserUnitsToInches(u + _setup.UU_axis_offset + _setup.UU_origin_offset),
					GC->UserUnitsToInches(v + _setup.VV_axis_offset + _setup.VV_origin_offset),
					_setup.sequence_number, ID);
	}
}
//ResumeTranslate

void STRAIGHT_FEED (double x, double y, double z,
		            double a, double b, double c, double u, double v, int ID)
{
	PRINT4(L"STRAIGHT_FEED(%.4f, %.4f, %.4f, %.4f)\n", x, y, z, a);

	double FeedRate;

	if (HandleThreading(&FeedRate)) return;

	if (!CM->m_ThreadingMode)  // Threading?
	{
		// must do this to determine if feed is a pure angle
		// if so feedrate is in degrees/min so don't convert from mm to inches
		double dx = x - _setup.current_x;
		double dy = y - _setup.current_y;
		double dz = z - _setup.current_z;
		double da = a - _setup.AA_current;
		double db = b - _setup.BB_current;
		double dc = c - _setup.CC_current;
		double du = u - _setup.UU_current;
		double dv = v - _setup.VV_current;

		BOOL pure_angle;

		CM->FeedRateDistance(dx, dy, dz, da, db, dc, du, dv, &pure_angle);

		if (pure_angle)
			FeedRate = _setup.feed_rate / 60.0;  // convert to degrees/sec
		else
			FeedRate = GC->UserUnitsToInches(_setup.feed_rate) / 60.0; // convert to inches/sec
	}

	GC->SaveStateOnceOnly();  // save the state here before creating any motion segments

	CM->StraightFeed(FeedRate,
					 GC->UserUnitsToInchesX(x+_setup.axis_offset_x+_setup.origin_offset_x+_setup.tool_xoffset),
					 GC->UserUnitsToInches(y+_setup.axis_offset_y+_setup.origin_offset_y+_setup.tool_yoffset),
					 GC->UserUnitsToInches(z+_setup.axis_offset_z+_setup.origin_offset_z+_setup.tool_length_offset),
					 GC->UserUnitsToInchesOrDegA(a+_setup.AA_axis_offset+_setup.AA_origin_offset),
					 GC->UserUnitsToInchesOrDegB(b + _setup.BB_axis_offset + _setup.BB_origin_offset),
					 GC->UserUnitsToInchesOrDegC(c + _setup.CC_axis_offset + _setup.CC_origin_offset),
				 	 GC->UserUnitsToInches(u + _setup.UU_axis_offset + _setup.UU_origin_offset),
					 GC->UserUnitsToInches(v + _setup.VV_axis_offset + _setup.VV_origin_offset),
					 _setup.sequence_number,ID);
}

// determine if we are to be in Threading mode based on Feed and Motion Modes
// if Threading return calculated feedrate
// if transitioning to Threading mode Flush any previous motion

int HandleThreading(double *FeedRate)
{
	if (_setup.motion_mode == G_32 ||
		((_setup.feed_mode == UNITS_PER_REV) 
			&& (_setup.motion_mode == G_1 || _setup.motion_mode == G_2 || _setup.motion_mode == G_3)))  // Threading?
	{
		double InchesPerRev = GC->UserUnitsToInches(_setup.feed_rate);
		double RevsPerSec = _setup.speed / 60.0;

		*FeedRate = InchesPerRev * RevsPerSec;

		// if not already in threading mode, then wait for anything that
		// may already be in progress to complete so when we re-launch motion
		// we will be in sychronized mode
		if (!CM->m_ThreadingMode)
		{
			if (CM->FlushSegments()) { CM->SetAbort(); return 1; }
			if (CM->WaitForSegmentsFinished(TRUE)) { CM->SetAbort(); return 1; }
		}

		if (GC->p_setup->spindle_turning == CANON_COUNTERCLOCKWISE)
			CM->m_ThreadingBaseSpeedRPS = -RevsPerSec;
		else
			CM->m_ThreadingBaseSpeedRPS = RevsPerSec;
		
		CM->m_ThreadingMode = true;
	}
	else
	{
		// if won't be doing Threading, flush any in progress
		if (CheckIfThreadingInProgress()) { CM->SetAbort(); return 1; }
	}
	return 0;
}




// whenever a new motion is to be initiated that is not spindle synchronized
// and spindle synchronized mode had been in progress then make sure it finishes
// so a new launch of coordinated motion (without spindle sync) will be generated

int CheckIfThreadingInProgress(void)
{
	if (CM->m_ThreadingMode)  
	{
		if (CM->FlushSegments()) {CM->SetAbort(); return 1;}  
		if (CM->WaitForSegmentsFinished(TRUE)) {CM->SetAbort(); return 1;}
	}
	return 0;
}




void STRAIGHT_PROBE (double x, double y, double z,
		     double a, double b, double c, double u, double v)
{PRINT3(L"STRAIGHT_PROBE(%.4f, %.4f, %.4f)\n", x, y, z);}


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
	PRINT1(L"DWELL(%.4f)\n", seconds);

	if (CheckIfThreadingInProgress()) return;

	GC->SaveStateOnceOnly();  // save the state here before creating any motion segments

	CM->Dwell(seconds,_setup.sequence_number);
}

/* Spindle Functions */
void SPINDLE_RETRACT_TRAVERSE()
{PRINT0(L"SPINDLE_RETRACT_TRAVERSE()\n");}

void START_SPINDLE_CLOCKWISE()
{
	PRINT0(L"START_SPINDLE_CLOCKWISE()\n");
	GC->InvokeAction(3);  // do the defined action for M Code
}

void START_SPINDLE_COUNTERCLOCKWISE()
{
	PRINT0(L"START_SPINDLE_COUNTERCLOCKWISE()\n");
	GC->InvokeAction(4);  // do the defined action for M Code
}

void SET_SPINDLE_SPEED(double r)
{
	PRINT1(L"SET_SPINDLE_SPEED(%.4f)\n", r);
	GC->InvokeAction(10);  // do the defined action for S Speed
}

void STOP_SPINDLE_TURNING()
{
	PRINT0(L"STOP_SPINDLE_TURNING()\n");
	GC->InvokeAction(5);  // do the defined action for M Code
}

void SPINDLE_RETRACT()
{PRINT0(L"SPINDLE_RETRACT()\n");}

void ORIENT_SPINDLE(double orientation, CANON_DIRECTION direction)
{PRINT2(L"ORIENT_SPINDLE(%.4f, %s)\n", orientation,
        (direction IS CANON_CLOCKWISE) ? L"CANON_CLOCKWISE" :
                                         L"CANON_COUNTERCLOCKWISE");
}

void USE_NO_SPINDLE_FORCE()
{PRINT0(L"USE_NO_SPINDLE_FORCE()\n");}

/* Tool Functions */

void USE_TOOL_LENGTH_OFFSET(double length_units, double xoffset_units, double yoffset_units, bool TCP_Mode)
{
    double Acts[MAX_ACTUATORS];

    PRINT1(L"USE_TOOL_LENGTH_OFFSET(%.4f)\n", length_units);

    double xoffset = GC->UserUnitsToInchesX(xoffset_units);
    double yoffset = GC->UserUnitsToInches(yoffset_units);
    double length  = GC->UserUnitsToInches(length_units);

    // Things get complicated if Tool Center point changes on-the-fly
    // for non-linear Kineamtics.  So if tool offset changes Flush
    // KFLOP buffer before changes.  For optimization
    // only flush when some offset changed and TCP is used

    if (CM->m_TCP_affects_actuators && 
        (TCP_Mode != CM->GetMotionParams()->TCP_Active ||
		 CM->GetMotionParams()->TCP_X != xoffset ||
         CM->GetMotionParams()->TCP_Y != yoffset ||
         CM->GetMotionParams()->TCP_Z != length))
    {
		// Finish everything
        if (CM->FlushSegments()) { CM->SetAbort(); return; }
        if (CM->WaitForSegmentsFinished(TRUE)) { CM->SetAbort(); return; }
        
		// Determine where all actuators are
        CM->Kinematics->TransformCADtoActuators(CM->current_x, CM->current_y, CM->current_z, CM->current_a, CM->current_b, CM->current_c, CM->current_u, CM->current_v, Acts);
		
		// apply new mode and offsets
		CM->GetMotionParams()->TCP_Active = TCP_Mode;
        CM->GetMotionParams()->TCP_X = xoffset;
        CM->GetMotionParams()->TCP_Y = yoffset;
        CM->GetMotionParams()->TCP_Z = length;

		// Map actuators to CAD
        CM->Kinematics->TransformActuatorstoCAD(Acts, &CM->current_x, &CM->current_y, &CM->current_z, &CM->current_a, &CM->current_b, &CM->current_c, &CM->current_u, &CM->current_v);
        GC->ConvertAbsoluteToInterpreterCoord(CM->current_x, CM->current_y, CM->current_z, CM->current_a, CM->current_b, CM->current_c, CM->current_u, CM->current_v,
            &_setup.current_x, &_setup.current_y, &_setup.current_z, &_setup.AA_current, &_setup.BB_current, &_setup.CC_current, &_setup.UU_current, &_setup.VV_current, &_setup);
    }
}

void CHANGE_TOOL(int slot)
{
	PRINT1(L"CHANGE_TOOL(%d)\n", slot);
	GC->InvokeAction(6);  // do the defined action for M Code
}

int M100(int mcode)	/* User M code  */
{
	PRINT1(L"MCODE(%d)\n", mcode);
	return GC->InvokeAction(mcode);  // do the defined action for M Code
}


void SELECT_TOOL(int slot)
{PRINT1(L"SELECT_TOOL(%d)\n", slot);}


/* Misc Functions */

void CLAMP_AXIS(CANON_AXIS axis)
{PRINT1(L"CLAMP_AXIS(%s)\n",
        (axis IS CANON_AXIS_X) ? L"CANON_AXIS_X" :
        (axis IS CANON_AXIS_Y) ? L"CANON_AXIS_Y" :
        (axis IS CANON_AXIS_Z) ? L"CANON_AXIS_Z" : L"UNKNOWN");}


// check for CMD,xxxxxx  any case and after removing whitespace 


int CheckForPassThroughCommand(const wchar_t *comment)
{				/* string with comment */
    int m;
    int item;
	char acomment[256];

    for (m = 0; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if ((item != 'C') && (item != 'c')) return 0;
    for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if ((item != 'M') && (item != 'm')) return 0;
    for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if ((item != 'D') && (item != 'd')) return 0;
    for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if (item != ',') return 0;
	for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);

	PRINT1(L"COMMAND(\"%s\")\n", comment + m);
	wcstombs(acomment, comment, 256);
	CM->DoKMotionCmd(acomment + m, TRUE);
    return 1; 
}


int CheckForBufferedCommand(const wchar_t *comment)
{				/* string with comment */
    int m;
    int item;
	char acomment[256];

    for (m = 0; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if ((item != 'B') && (item != 'b')) return 0;
    for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if ((item != 'U') && (item != 'u')) return 0;
    for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if ((item != 'F') && (item != 'f')) return 0;
    for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if (item != ',') return 0;
	for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);

	PRINT1(L"BUFFER(\"%s\")\n", comment + m);
	wcstombs(acomment, comment, 256);
	CM->DoKMotionBufCmd(acomment + m,GC->p_setup->sequence_number);
    return 1; 
}

int CheckForUserCallback(const wchar_t *comment)
{				/* string with comment */
    int m;
    int item;
	char acomment[256];

    for (m = 0; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if ((item != 'U') && (item != 'u')) return 0;
    for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if ((item != 'S') && (item != 's')) return 0;
    for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if ((item != 'R') && (item != 'r')) return 0;
    for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);
    if (item != ',') return 0;
	for (m++; ((item = comment[m]) == ' ') || (item == '\t'); m++);

	PRINT1(L"USR(\"%ls\")\n", comment + m);
	if (GC->m_UserFn)
	{
		if (CM->FlushSegments()) {CM->SetAbort(); return 1;}  
		if (CM->WaitForSegmentsFinished(TRUE)) {CM->SetAbort(); return 1;}
		wcstombs(acomment, comment, 256);
		if (GC->m_UserFn(acomment + m)){CM->SetAbort(); return 1;}

		// don't sample positions until everything is stopped 
		if (CM->WaitForSegmentsFinished()) return 1;
		if (CM->WaitForMoveXYZABCFinished()) return 1;

		if (!CM->m_Simulate && GC->ReadAndSyncCurPositions(&_setup.current_x,&_setup.current_y,&_setup.current_z,
			                                               &_setup.AA_current, &_setup.BB_current, &_setup.CC_current, &_setup.UU_current, &_setup.VV_current))
			return 1;
	}
    return 1; 
}


void COMMENT(const wchar_t *s)
{
	if (CheckForBufferedCommand(s)) return;	
	if (CheckForPassThroughCommand(s)) return;	
	if (CheckForUserCallback(s)) return;	
	PRINT1(L"COMMENT(\"%ls\")\n", s);  /*TRAN*/
}


void DISABLE_FEED_OVERRIDE()
{PRINT0(L"DISABLE_FEED_OVERRIDE()\n");}

void DISABLE_SPEED_OVERRIDE()
{PRINT0(L"DISABLE_SPEED_OVERRIDE()\n");}

void ENABLE_FEED_OVERRIDE()
{PRINT0(L"ENABLE_FEED_OVERRIDE()\n");}

void ENABLE_SPEED_OVERRIDE()
{PRINT0(L"ENABLE_SPEED_OVERRIDE()\n");}

void FLOOD_OFF()
{
	PRINT0(L"FLOOD_OFF()\n");
}

void FLOOD_ON()
{
	PRINT0(L"FLOOD_ON()\n");
	GC->InvokeAction(8);  // do the defined action for M Code
}

void MESSAGE(char *s)
{
	if (CM->FlushSegments()) {CM->SetAbort(); return;}  
	if (CM->WaitForSegmentsFinished(TRUE)) {CM->SetAbort(); return;}

	PRINT1(L"MESSAGE(\"%s\")\n", s);
	if (AfxMessageBox(s,MB_OKCANCEL|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL)==IDCANCEL)
	{
		GC->Abort();
	}
}

void MIST_OFF()
{
	PRINT0(L"MIST_OFF()\n");
	GC->InvokeAction(9);  // do the defined action for M Code
}

void MIST_ON()
{
	PRINT0(L"MIST_ON()\n");
	GC->InvokeAction(7);  // do the defined action for M Code
}

void PALLET_SHUTTLE()
{PRINT0(L"PALLET_SHUTTLE()\n");}

void TURN_PROBE_OFF()
{PRINT0(L"TURN_PROBE_OFF()\n");}

void TURN_PROBE_ON()
{PRINT0(L"TURN_PROBE_ON()\n");}

void UNCLAMP_AXIS(CANON_AXIS axis)
{PRINT1(L"UNCLAMP_AXIS(%s)\n",
        (axis IS CANON_AXIS_X) ? L"CANON_AXIS_X" :
        (axis IS CANON_AXIS_Y) ? L"CANON_AXIS_Y" :
        (axis IS CANON_AXIS_Z) ? L"CANON_AXIS_Z" : L"UNKNOWN");}

/* Program Functions */

void PROGRAM_STOP()
{
	PRINT0(L"PROGRAM_STOP()\n");
//	GC->m_Halt=true;
	GC->InvokeAction(0,TRUE);
	GC->m_end=0;  // force line number to be passed
}

void OPTIONAL_PROGRAM_STOP()
{
	GC->InvokeAction(1,TRUE);
	PRINT0(L"OPTIONAL_PROGRAM_STOP()\n");
}

void PROGRAM_END(int MCode)
{
	PRINT0(L"PROGRAM_END()\n");

	CM->FlushSegments();
	if (MCode==30)
		GC->InvokeAction(24,TRUE);  // M30 Special Operation
	else if (MCode==2)
		GC->InvokeAction(2,TRUE);  // M2 Special Operation
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
	if (GC->VarsFile[0] == 0)
		sprintf(filename, "%s%cData%cemc.var",GC->CoordMotion->MainPathRoot, PATH_SEPARATOR, PATH_SEPARATOR);
	else
		strcpy(filename, GC->VarsFile);
}

// returns the currently active plane
CANON_PLANE GET_EXTERNAL_PLANE()  { return CANON_PLANE_XY; };

// returns the current a-axis position
double GET_EXTERNAL_POSITION_A()  { return 0.0; };

// returns the current b-axis position
double GET_EXTERNAL_POSITION_B() { return 0.0; };

// returns the current c-axis position
double GET_EXTERNAL_POSITION_C() { return 0.0; };

// returns the current u-axis position
double GET_EXTERNAL_POSITION_U() { return 0.0; };

// returns the current v-axis position
double GET_EXTERNAL_POSITION_V() { return 0.0; };

// returns the current x-axis position
double GET_EXTERNAL_POSITION_X()  { return 0.0; };

// returns the current y-axis position
double GET_EXTERNAL_POSITION_Y()  { return 0.0; };

// returns the current z-axis position
double GET_EXTERNAL_POSITION_Z()  { return 0.0; };

// Returns the machine A-axis position at the last probe trip.
double GET_EXTERNAL_PROBE_POSITION_A()  { return 0.0; };

// Returns the machine B-axis position at the last probe trip.
double GET_EXTERNAL_PROBE_POSITION_B() { return 0.0; };

// Returns the machine C-axis position at the last probe trip.
double GET_EXTERNAL_PROBE_POSITION_C() { return 0.0; };

// Returns the machine U-axis position at the last probe trip.
double GET_EXTERNAL_PROBE_POSITION_U() { return 0.0; };

// Returns the machine V-axis position at the last probe trip.
double GET_EXTERNAL_PROBE_POSITION_V() { return 0.0; };

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
	if (!CM->m_PreviouslyStopped || CM->m_PreviouslyStoppedID==1)
	{ 
       return 1;
	}
	else 
	{
		if(log)
		{
		  CM->m_StoppedMidx = mx;
		  CM->m_StoppedMidy = my;
		} 
		return 0;
	}
}
