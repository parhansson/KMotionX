// KMotionApp.cpp : Defines the initialization routines for the DLL.
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CKMotionApp

BEGIN_MESSAGE_MAP(CKMotionApp, CWinApp)
	//{{AFX_MSG_MAP(CKMotionApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKMotionApp construction


CString MainPathDLL;
CString MainPath;
CString MainPathRoot;

CKMotionApp::CKMotionApp()
{
	
	GetModuleFileName(GetModuleHandle("KMotionDLL.dll"),MainPath.GetBuffer(MAX_PATH),MAX_PATH);
	MainPath.ReleaseBuffer();
	
	// Save for everybody what directory we are installed in


	MainPath.Replace("\"","");  // remove quotes
	MainPath.TrimRight();
	MainPath.TrimLeft();

	int LastSlash=MainPath.ReverseFind('\\');
	MainPathDLL=MainPath=MainPath.Left(LastSlash);


	// Check if we are running from the debug directory
	// if we are, then strip it off

	if (MainPath.Right(6).CompareNoCase("\\debug") == 0)
	{
		MainPath = MainPath.Left(MainPath.GetLength()-6);
	}

	// Check if we are running from the release directory
	// if we are, then strip it off

	if (MainPath.Right(8).CompareNoCase("\\release") == 0)
	{
		MainPath = MainPath.Left(MainPath.GetLength()-8);
	}

	// Now set the root install directory

	if (MainPath.Right(8).CompareNoCase("\\KMotion") == 0)
	{
		MainPathRoot = MainPath.Left(MainPath.GetLength()-8);
	}
	else
	{
		MainPathRoot = MainPath;
	}
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CKMotionApp object

CKMotionApp theApp;

BOOL CKMotionApp::InitApplication() 
{
	char s[MAX_PATH];

	GetModuleFileName(NULL,s,MAX_PATH);
	return CWinApp::InitApplication();
}

BOOL CKMotionApp::InitInstance() 
{
	share++;   // keep track of how many instances	
	
	return CWinApp::InitInstance();
}

int CKMotionApp::ExitInstance() 
{
	share--;   // keep track of how many instances	
	
	return CWinApp::ExitInstance();
}
