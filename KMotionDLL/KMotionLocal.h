// KMotionLocal.h: interface for the CKMotionLocal class.
//
// Maintains the internal local connections to all the KMotion Boards
//
// There is one KMotionIO object for each board in the system
//
// As multiple Apps attach to the DLL connections may or may not
//
// need to be opened 
//
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#if !defined(AFX_KMOTIONLOCAL_H__00C03845_F2F4_4CE3_A014_1FA7DC1E4026__INCLUDED_)
#define AFX_KMOTIONLOCAL_H__00C03845_F2F4_4CE3_A014_1FA7DC1E4026__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CKMotionLocal;

extern CKMotionLocal KMotionLocal;  // declare one global instance 

extern volatile int share;

class CKMotionLocal  
{
public:
	CKMotionLocal();
	virtual ~CKMotionLocal();

	CKMotionIO KMotionIO[MAX_BOARDS];
};

#endif // !defined(AFX_KMOTIONLOCAL_H__00C03845_F2F4_4CE3_A014_1FA7DC1E4026__INCLUDED_)
