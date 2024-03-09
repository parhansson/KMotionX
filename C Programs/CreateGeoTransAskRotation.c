#include "KMotionDef.h"
#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

// Create KMotionCNC GeoCorrection File based on simple Transformation

double offsetx = 0.0;  // Transform values
double offsety = 0.0;
double theta = 0.0;
double scale = 1.0;

void WriteGrid(int row, int col, FILE *f);
double TransformX(double x, double y);
double TransformY(double x, double y);

main()
{
    float value;
    
    // Ask Operator desired rotation angle
    if (!InputBox("Enter Angle in degrees",&value))  
    {
        theta = value * PI/180.0;  // convert to radians
        
        // Create Simple 4 point Geo File
        FILE *f=fopen("C:\\Temp\\MapFile.txt","wt");
        fprintf(f,"2,2\n");        // rows, cols
        fprintf(f,"1,1\n");        // gridx gridy
        fprintf(f,"-0.5,-0.5\n");  // centerX centerY

        WriteGrid(0, 0, f);  // write the 4 transformed points
        WriteGrid(0, 1, f);
        WriteGrid(1, 0, f);
        WriteGrid(1, 1, f);
        fclose(f);
        DoPC(PC_COMM_RELOAD_GEOCORRECTION);  // tell KMotionCNC to reload the file
    }
}

// write a transformed point to Geo File
void WriteGrid(int row, int col, FILE *f)
{
    double Centerx = 0.5;
    double Centery = 0.5;
    double x = col - Centerx;
    double y = row - Centery;
    fprintf(f,"%d,%d,%.11f,%.11f,0\n", row, col, TransformX(x, y), TransformY(x, y));
}

// Desired X Transformaion
double TransformX(double x, double y)
{
    return (scale * (x * cos(theta) - y * sin(theta))) + offsetx;
}

// Desired Y Transformaion
double TransformY(double x, double y)
{
    return (scale * (x * sin(theta) + y * cos(theta))) + offsety;
}
