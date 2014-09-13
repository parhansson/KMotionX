//********************************************
// Object3d.h
//********************************************
// class CObject3d, the base 3d object
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 17/12/97
// Modified : 19/12/97
//********************************************

#ifndef _OBJECT3D_
#define _OBJECT3D_

class CObject3d
{
private :

public :
	CObject3d() {}
	virtual ~CObject3d() {}

	// Datas
	virtual int GetType();
	virtual int glBuildList();
	virtual int glDraw();
	virtual void SetModified() { };
	virtual void InvalidateDisplayList() { };
};

#endif // _OBJECT3D_
