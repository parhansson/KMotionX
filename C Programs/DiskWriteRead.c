#include "KMotionDef.h"

main()
{
	FILE *f;
	char s[256];
	double a=123.456,b=999.999,c=0.001;
	double x=0,y=0,z=0;
	int result;
	
	// write 3 comma separated values to a disk file
	f=fopen("c:\\Temp\\KFlopData.txt","wt");
	fprintf(f,"%f,%f,%f\n",a,b,c);
	fclose(f);
	
	// read them back in
	f=fopen("c:\\Temp\\KFlopData.txt","rt");
	if (!f)
	{
		printf("Unable to open file\n");
		return;
	}
	
	// read a line and convert 3 doubles
	result=fscanf(f,"%lf,%lf,%lf",&x,&y,&z);
	fclose(f);
	
	printf("# values converted = %d, x=%f, y=%f, z=%f\n",result,x,y,z);
}
