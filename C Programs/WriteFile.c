#include "KMotionDef.h"

main()
{
	FILE *f=fopen("C:\\temp\\kflopdata.txt","wt");
	fprintf(f,"BBB\n");  // write some data
	fprintf(f,"CCC\n");  // write some data
	fprintf(f,"%f %f %f\n",4.5,4.5,4.5);  // write some data
	printf("%f %f %f\n",4.5,4.5,4.5);  // write some data
	fclose(f);
}


