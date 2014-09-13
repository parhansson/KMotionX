// KMotionLocal.cpp: implementation of the CKMotionLocal class.
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#include "stdafx.h"

#pragma data_seg (".SHARED") // any name you like

int volatile share=0;
#pragma data_seg ()

#pragma comment(linker, "/section:.SHARED,RWS")

CKMotionLocal KMotionLocal;  // declare one global instance 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CKMotionLocal::CKMotionLocal()
{

}

CKMotionLocal::~CKMotionLocal()
{

}
