// PWM->Analog Correction
//
// assume very non-linear for first few count and linear thereafter 
//
// Measure output ratio for first 0-7 counts then at 240
 

float V[]=
{
0.001,  // count = 0
0.031,  // count = 1
0.044,  // count = 2
0.054,  // count = 3
0.062,  // count = 4
0.069,  // count = 5
0.074,  // count = 6
0.079   // count = 7
};

float V240=0.970;

int CorrectAnalog(float v)
{
	int r;
	float v2=2.0f*v;
	// compare with half way points to determine closest count
	if (v2 < V[1]+V[0]) return 0;
	if (v2 < V[2]+V[1]) return 1;
	if (v2 < V[3]+V[2]) return 2;
	if (v2 < V[4]+V[3]) return 3;
	if (v2 < V[5]+V[4]) return 4;
	if (v2 < V[6]+V[5]) return 5;
	if (v2 < V[7]+V[6]) return 6;
	
	// must be 7 or higher do linear interpolation
	
	r = (int)(7.5 + (v-V[7])/(V240-V[7])*(240.0f-7.0f));
	if (r>255) r=255;
	return r;
}
