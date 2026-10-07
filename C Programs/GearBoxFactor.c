#include "KMotionDef.h"

#define GIN1 136 // Gear Sensor Input 1
#define GIN2 137 // Gear Sensor Input 2
#define GIN3 138 // Gear Sensor Input 3

// Table of required ratios to increase commanded speed based on Gear
float SpeedRatioTable[8]={1.0f,  		// 0=Gear A 
					2500.0f/1500.0f, 	// 1=Gear B
					0.0f, 				// 2=Invalid Sensor combo
					2500.0f/900.0f, 	// 3=Gear C
					2500.0f/325.0f,		// 4=Gear E
					0.0f, 				// 5=Invalid Sensor combo
					2500.0f/540.0f, 	// 6=Gear D
					0.0f}; 				// 7=Invalid Sensor combo


main()
{
	int Gear;
	float SpeedRatio;

	Gear = ReadBit(GIN1) + ReadBit(GIN2)*2 + ReadBit(GIN3)*4;
	SpeedRatio = SpeedRatioTable[Gear];
	
	printf("Gear Number=%d Speed Ratio=%f\n",Gear,SpeedRatio); 
}
