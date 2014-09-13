#include "KMotionDef.h"

//Plugin calls for Mach3 Home (actually Purge) Commands
//Called from Mach3 "REF" command
//in this case just Zero the measured position (encoder)
//and set the commanded destination to zero

#define X 0
#define Y 1
#define Z 2

main()
{
	int flags = persist.UserData[5];  // Mach3 flags bit0=X, bit1=Y, Bit2=Z, etc...

	printf("Mach3 Home Call, flags = %d\n",flags); 
	
	if (flags & 1)
	{
		DisableAxis(X);
		Zero(X);
		EnableAxisDest(X,0.0);
	}
	if (flags & 2)
	{
		DisableAxis(Y);
		Zero(Y);
		EnableAxisDest(Y,0.0);
	}
	if (flags & 4)
	{
		DisableAxis(Z);
		Zero(Z);
		EnableAxisDest(Z,0.0);

		Jog(Z,10000);          	// start moving
		while (ReadBit(15)) ; 	// wait for switch (input #15) to change
		Jog(Z,0);		      	// StopMotion
		
		while(!CheckDone(Z)) ;

		Jog(Z,-3000);          	// start moving
		while (!ReadBit(15)) ; 	// wait for switch (input #15) to change
		Jog(Z,0);		      	// StopMotion

		while(!CheckDone(Z)) ;
		Delay_sec(.25);
		
		DisableAxis(Z);
		Zero(Z);
		EnableAxisDest(Z,0.0);
	}
}
