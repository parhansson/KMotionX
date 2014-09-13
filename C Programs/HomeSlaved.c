	
#include "KMotionDef.h"

// Home  (X0 and X1 are slaved)
//

#define X0_AXIS 0
#define X1_AXIS 3

#define X0_HOME_BIT 16
#define X1_HOME_BIT 17

#define X_SLOW_SPEED 500
#define X_FAST_SPEED 1500

#define X0_DIST 5000  // amount to move inside the limits
#define X1_DIST 5000  // different distances can be used to square the axis


void SquareX(float speed);

void main() 
{
	int SaveLimitX0,SaveLimitX1;

	SaveLimitX0 = chan[X0_AXIS].LimitSwitchOptions;  // save and disable limits
	SaveLimitX1 = chan[X1_AXIS].LimitSwitchOptions;
	chan[X0_AXIS].LimitSwitchOptions = 0;
	chan[X1_AXIS].LimitSwitchOptions = 0;

	// Square up the x axis

	chan[X0_AXIS].MasterAxis = -1;  //  De-Slave both
	chan[X1_AXIS].MasterAxis = -1;

	// check if we already are in the square/limits
	// if so, move away from the switch and re-square
	if (ReadBit(X0_HOME_BIT) || ReadBit(X1_HOME_BIT))
	{
		MoveRel(X0_AXIS,X_FAST_SPEED);  // move inside the limits a small amount
		MoveRel(X1_AXIS,X_FAST_SPEED);

		while (!CheckDone(0) || !CheckDone(1))  ;  // wait till 
	}

	SquareX(X_FAST_SPEED); // square up fast 
	SquareX(X_SLOW_SPEED);   // square up slow

	chan[X0_AXIS].LimitSwitchOptions = SaveLimitX0;  // restore limits
	chan[X1_AXIS].LimitSwitchOptions = SaveLimitX1;


	chan[X1_AXIS].MasterAxis = X0_AXIS;  //  Slave X1 to X0
	chan[X1_AXIS].SlaveGain = 1;
	chan[X0_AXIS].MasterAxis = -1; // do not slave X0

   
	printf("Home Complete\n");
}



void SquareX(float speed)
{
	int found0,found1;

	Jog(X0_AXIS,-speed);  // start moving both axis toward swiches 
	Jog(X1_AXIS,-speed);

	found0 = found1 = 0;

	while (!found0 || !found1)
	{
		if (!found0 && ReadBit(X0_HOME_BIT))
		{
			Jog(X0_AXIS,0.0); // StopMotion
			found0=1;
		}
		if (!found1 && ReadBit(X1_HOME_BIT))
		{
			Jog(X1_AXIS,0.0); // StopMotion
			found1=1;
		}
	}

	while (!CheckDone(X0_AXIS) || !CheckDone(X1_AXIS))  ;  // wait till done

	// we are now all square
	// zero both x axis

	Zero(X0_AXIS);
	Zero(X1_AXIS);
	
	printf("X before move in %f %f\n",chan[X0_AXIS].Position,chan[X1_AXIS].Position);

	MoveAtVel(X0_AXIS,X0_DIST,speed);  // move inside the limits a small amount
	MoveAtVel(X1_AXIS,X1_DIST,speed);  // different distances can be used to square the axis

	while (!CheckDone(X0_AXIS) || !CheckDone(X1_AXIS))  ;  // wait till done

	printf("X After move in %f %f\n",chan[X0_AXIS].Position,chan[X1_AXIS].Position);
	printf("Switch Status X0_HOME_BIT=%d,X1_HOME_BIT=%d\n",ReadBit(X0_HOME_BIT),ReadBit(X1_HOME_BIT));

	Zero(X0_AXIS);
	Zero(X1_AXIS);
}


