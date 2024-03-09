#include "KMotionDef.h"

#define TAP_VAR 50 // start of where parameters should be placed

#define CANON_UNITS_INCHES 1  

main()
{
	// Call MCode M119 to do Rigid Tapping
	// 
	// Var+0 - (int) 119 (MCode number)
	// Var+1 - (float) Bottom Z in User Units
	// Var+2 - (float) Retract Position in User Units
	// Var+3 - (float) Distance per Peck in User Units (0 if unspecified)
	// Var+4 - (float) Feed Rate F Number (User Units/Rev)
	// Var+5 - (float) Spindle RPM to Tap
	// Var+6 - (int) Units #define CANON_UNITS_INCHES 1  #define CANON_UNITS_MM 2
	// Var+7 - (int) KFLOP axis to move (derived from active GCode Plane and Coord Motion System defs)
	// Var+8 - (float) Axis Resolution Counts/inch

	int MCode = persist.UserData[TAP_VAR+0];
	float Bottom = *(float *)&persist.UserData[TAP_VAR+1];
	float Retract = *(float *)&persist.UserData[TAP_VAR+2];
	float Peck = *(float *)&persist.UserData[TAP_VAR+3];
	float Rate = *(float *)&persist.UserData[TAP_VAR+4];
	float RPM = *(float *)&persist.UserData[TAP_VAR+5];
	int Units = persist.UserData[TAP_VAR+6];
	int Axis = persist.UserData[TAP_VAR+7];
	float AxisRes = *(float *)&persist.UserData[TAP_VAR+8];

	printf("MCode = %d\n",MCode); 
	printf("Bottom = %f\n",Bottom); 
	printf("Retract = %f\n",Retract); 
	printf("Peck = %f\n",Peck); 
	printf("Rate = %f\n",Rate); 
	printf("RPM = %f\n",RPM); 
	if (Units == CANON_UNITS_INCHES)
		printf("Units = Inches\n"); 
	else
		printf("Units = mm\n"); 
	printf("Axis = %d\n",Axis); 
	printf("AxisRes = %f\n",AxisRes); 

}
