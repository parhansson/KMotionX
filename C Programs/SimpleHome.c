#include "KMotionDef.h"

main()
{
	Jog(0,-100);          // start moving
	while (!ReadBit(8)) ; // wait for switch (input #8) to change
	Jog(0,0);		      // stop
}
