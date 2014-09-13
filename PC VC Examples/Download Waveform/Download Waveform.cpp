// Download Waveform.cpp 

// simple example to download some data to the gather buffer


#include "stdafx.h"
#include "stdlib.h"
#include <string.h>
#include "..\..\KMotionDll\KMotionDll.h"  // KMOtion DLL Header

CKMotionDLL *KM;           // one instance of the interface class

void MyError();

#define N 1000  // no of words to send

int main(int argc, char* argv[])
{
	int i,board=0;
	char s[MAX_LINE],s2[16];
	int data[N];

	KM = new CKMotionDLL(0);  // create as board 0

	// fill simple buffer with a ramp

	for (i=0;i<N; i++) data[i]=i;



	// first get the token for the board to allow uninterrupted access

	if (KM->WaitToken()!=KMOTION_LOCKED) MyError();

	// tell the board we will send N (32 bit ) words at offset 0

	sprintf(s,"SetGatherHex 0 %d", N);
	if (KM->WriteLine(s))  MyError();


	// send the data (simple ramp)  (8 hex words per line)

	s[0]=0;
	for (i=0; i<N; i++)
	{
		sprintf(s2,"%X", data[i]);

		strncat(s,s2,10);  // append the hex string

		if (((i%8) == 7) || i==N-1)  // every 8 or on the last send it
		{
			if (KM->WriteLine(s))  MyError();
			s[0]=0;
		}
		else
		{
			strncat(s," ",1);  // otherwise insert a space
		}

	} 

	// release our access to the board

	KM->ReleaseToken();


	return 0;
}

// Display Error and terminate

void MyError()
{
	printf("Unable to communicate with KMotion board");
	exit(1);
}

