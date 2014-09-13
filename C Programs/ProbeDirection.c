#include "KMotionDef.h"

// For basic KMotionCNC Probing configure KMotionCNC to call this C Program
// with an MCode and 3 parameters P Q R passed in variables 50,51,52 to 
// specify the X Y Z Probe directions speeds
//
// Modify this program for the input bit you are using for your probe.  As
// written the program keeps moving until the bit goes high.  If the polarity
// of your probe is opposit this then remove the NOT Symbol '!' applied to the
// ReadBit result. 
//
// For Example:
//
// M100 = Exec/Wat/Sync  Thread 2 Var 50 <>\C programs\ProbeDirection.c
//
// Then to probe in the X direction at 100 counts/sec use GCode as:
//
// M100 P100 Q0 R0
//
// Then to probe in the Y direction at 50 counts/sec use GCode as:
//
// M100 P0 Q50 R0
//
// 

main()
{
	Jog(0,*(float *)&persist.UserData[50]);	// move in the velocites specified
	Jog(1,*(float *)&persist.UserData[51]);
	Jog(2,*(float *)&persist.UserData[52]);

//	printf("X Y Z Speeds = %f %f %f\n",
//		*(float *)&persist.UserData[50],
//		*(float *)&persist.UserData[51],
//		*(float *)&persist.UserData[52]);

	while (!ReadBit(46)) ;   		// wait for switch to go high

	Jog(0,0); 						//Stop
	Jog(1,0);
	Jog(2,0);
}
