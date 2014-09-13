#define SPINDLEAXIS 6		// Axis Channel to Jog to rotate Spindle
#define FACTOR (1000/60.0)  // to convert RPM to counts/sec (counts/rev / 60.0sec)
#define SPINDLECW_BIT 154   // bit to activate to cause CW rotation
#define SPINDLECCW_BIT 155	// bit to activate to cause CCW rotation
#define SPEEDVAR 99			// global persistant variable to store latest speed
#define STATEVAR 98			// global persistant variable to store latest state (-1=CCW,0=off,1=CW)
#define KMVAR PC_COMM_CSS_S // variable KMotionCNC will pass speed parameter (113)

