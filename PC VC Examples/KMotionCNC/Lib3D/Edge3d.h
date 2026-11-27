//********************************************
// Edge3d.h
// class CEdge3d
//********************************************
// An edge :
// + Two vertex ref
// + Two neighbors ref
// + A flag
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 15/12/97
// Modified : 18/02/98
//********************************************

#ifndef _EDGE_3D_
#define _EDGE_3D_

#include "Object3d.h"

class CVertex3d;
class CFace3d;

class CEdge3d : public CObject3d
{

private :

	// Geometry
	CVertex3d *m_pVertex1;
	CVertex3d *m_pVertex2;

	CFace3d *m_pFace1;
	CFace3d *m_pFace2;

	// Flag
	char m_Flag;

public :

	CEdge3d();
	virtual ~CEdge3d() {}
	
	// Constructors
	CEdge3d(CVertex3d *pVertex1,CVertex3d *pVertex2);
	CEdge3d(CVertex3d *pVertex1,CVertex3d *pVertex2,CFace3d *pFace1,CFace3d *pFace2);
	CEdge3d(CEdge3d *pEdge);

	// Data setting
	virtual int GetType();

	// Copy Edge
	void Set(CEdge3d *pEdge);

	// Vertices
	void Set(CVertex3d *pVertex1,CVertex3d *pVertex2);
	void v1(CVertex3d *pVertex) { m_pVertex1 = pVertex; }
	void v2(CVertex3d *pVertex) { m_pVertex2 = pVertex; }

	void v(int index,CVertex3d *pVertex)
	{
		if(index == 0)
			m_pVertex1 = pVertex;
		else
			m_pVertex2 = pVertex;
	}

	// Faces
	void Set(CFace3d *pFace1,CFace3d *pFace2);
	void f1(CFace3d *pFace) { m_pFace1 = pFace; }
	void f2(CFace3d *pFace) { m_pFace2 = pFace; }
	void f(int index,CFace3d *pFace)
	{
		if(index == 0)
			m_pFace1 = pFace;
		else
			m_pFace2 = pFace;
	}

	// Vertices
	CVertex3d *v1(void) { return m_pVertex1; }
	CVertex3d *v2(void) { return m_pVertex2; }
	CVertex3d *v(int index) { return ((index == 0) ? m_pVertex1 : m_pVertex2); }
	CVertex3d *GetVertexDiff(CVertex3d *pVertexDiff);

	// Faces
	CFace3d *f1(void) { return m_pFace1; }
	CFace3d *f2(void) { return m_pFace2; }
	CFace3d *f(int index) { return ((index == 0) ? m_pFace1 : m_pFace2); }

	// Flag
	void SetFlag(char flag) { m_Flag = flag; }
	char GetFlag() { return m_Flag; }

	// Processing
	int Equal(CEdge3d *pEdge);
	int sharp(float threshold); // threshold : sin(angle)

	// OpenGL
	void glDrawHighlight(unsigned char *color);
	void glDrawHighlight(unsigned char *color,unsigned char *ColorNeighbor);

};

#endif // _EDGE_3D_
