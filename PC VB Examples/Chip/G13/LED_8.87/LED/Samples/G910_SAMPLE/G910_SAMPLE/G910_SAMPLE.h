
// G910_SAMPLE.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CG910_SAMPLEApp:
// See G910_SAMPLE.cpp for the implementation of this class
//

class CG910_SAMPLEApp : public CWinApp
{
public:
	CG910_SAMPLEApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CG910_SAMPLEApp theApp;