// SimpleCoordMotion.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include "CoordMotion.h"

// Global Variables

CKMotionDLL *KM;
CCoordMotion *CM;

int count=0;  // just count the callbacks

void StraightTraverseCallback(double x, double y, double z, int sequence_number)
{
	count++;
}
	
void StraightFeedCallback(double DesiredFeedRate_in_per_sec,
							   double x, double y, double z, int sequence_number, int ID)

{
	count++;
}

void ArcFeedCallback(bool ZeroLenAsFullCircles, double DesiredFeedRate_in_per_sec, 
			    CANON_PLANE plane,
				double first_end, double second_end, 
		        double first_axis, double second_axis, int rotation,
				double axis_end_point,
				double first_start, double second_start, double axis_start_point, int sequence_number, int ID)
{
	count++;
}


int main(int argc, char* argv[])
{
	KM = new CKMotionDLL(0);  // create as board 0
	CM = new CCoordMotion(KM);


	MOTION_PARAMS *p=CM->GetMotionParams();

	p->BreakAngle = 30;
	p->MaxAccelX = 1;
	p->MaxAccelY = 1;
	p->MaxAccelZ = 1;
	p->MaxAccelA = 1;
	p->MaxAccelB = 1;
	p->MaxAccelC = 1;
	p->MaxVelX = 1;
	p->MaxVelY = 1;
	p->MaxVelZ = 1;
	p->MaxVelA = 1;
	p->MaxVelB = 1;
	p->MaxVelC = 1;
	p->CountsPerInchX = 100;
	p->CountsPerInchY = 100;
	p->CountsPerInchZ = 100;
	p->CountsPerInchA = 100;
	p->CountsPerInchB = 100;
	p->CountsPerInchC = 100;

	p->DegreesA = p->DegreesB = p->DegreesC = FALSE; 
	p->ArcsToSegs = true;


	CM->SetTPParams();  // Apply motion parameters to the Trajectory Planner

	
	CM->SetAbort();
	CM->ClearAbort();

	CM->SetStraightTraverseCallback(StraightTraverseCallback);
	CM->SetStraightFeedCallback(StraightFeedCallback);
	CM->SetArcFeedCallback(ArcFeedCallback);

	
	double Speed = 0.5; //inch/sec

	CM->StraightTraverse(0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000);  // jump back to zero
    
	// set a bit, move in a circle, clear a bit

	CM->DoKMotionBufCmd("SetBitBuf0");
    CM->ArcFeed(Speed,CANON_PLANE_XY,0.0000, 0.5000, 0.0000, 0.2500, DIR_CCW, 0.0000, 0.0000, 0.0000, 0.0000, 0, 0);
    CM->ArcFeed(Speed,CANON_PLANE_XY,0.0000, 0.0000, 0.0000, 0.2500, DIR_CCW, 0.0000, 0.0000, 0.0000, 0.0000, 0, 0);
	CM->DoKMotionBufCmd("ClearBitBuf0");
	
	// move in an xy square (0,0) -> (0,0.5) -> (0.5,0.5) -> (0.5,0) -> (0,0)

	CM->StraightFeed(Speed, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0, 0);
	CM->StraightFeed(Speed, 0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 0, 0);
	CM->StraightFeed(Speed, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0);
	CM->StraightFeed(Speed, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0);

	// turn on a bit, wait 2 seconds, turn the bit off

	CM->DoKMotionBufCmd("SetBitBuf0");
	CM->Dwell(2,0);
	CM->DoKMotionBufCmd("ClearBitBuf0");
	CM->FlushSegments();
	return 0;
}

