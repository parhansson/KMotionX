// SimpleCoffload.cpp 

#include "stdafx.h"
#include "..\..\KMotionDll\KMotionDll.h"  // KMotion DLL Header

CKMotionDLL *KM;                // one instance of the interface class

void MyError();


int main(int argc, char* argv[])
{
	CKMotionDLL *KM = new CKMotionDLL(0);

	// Compile and Download a C Program into Thread #1

	if (KM->CompileAndLoadCoff("..\\..\\C Programs\\StepMotor0.c",1))	MyError();

	// Execute Thread 1
	
	if (KM->WriteLine( "Execute1")) MyError(); 
	
	return 0;
}

// Display Error and terminate

void MyError()
{
	printf("Unable to communicate with KMotion board");
	exit(1);
}

