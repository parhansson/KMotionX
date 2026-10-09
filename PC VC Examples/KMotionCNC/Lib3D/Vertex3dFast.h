//********************************************
// Vertex3dFast.h
// (Part of 3d Toolbox)
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 10/12/97
// Modified : 29/04/98
//********************************************

#ifndef _VERTEX_3D_FAST_
#define _VERTEX_3D_FAST_

#include "Object3d.h"
#include "Vector3d.h"
#include "Color.h"
#include "Array3d.h"


class CVertex3dFast : public CObject3d
{

private :

	float     m_Coord[3];	 // Geometry
	CColor    m_Color;     // Color
	int		 m_sequence_number;
	int		 m_ID;


public :

	// Constructors
	CVertex3dFast() {Set(0.0f,0.0f,0.0f); }
	CVertex3dFast(CVertex3dFast &vertex);
	CVertex3dFast(CVertex3dFast *pVertex);
	CVertex3dFast(CVertex3dFast *pVertex,CVector3d *pVector);
	CVertex3dFast(const float x,const float y,const float z); 
	CVertex3dFast(const float x,const float y,const float z, CColor &Color); 
	CVertex3dFast(const float x,const float y,const float z, CColor &Color, int sequence_number, int ID); 

	// Destructor (CArray3d will do the work...)
	virtual ~CVertex3dFast() {}

	// Data setting
	void Set(const float x,const float y,const float z);
	void Set(CVertex3dFast *pVertex);
	void Set(CVertex3dFast &vertex);

	// Moving
	void Move(CVector3d &vector,const float ratio);
	void Move(CVector3d *pVector,const float ratio = 1.0f);
	void Move(const float dx,const float dy,const float dz);

	// Per coordinate
	void Set(const unsigned int index,const float value) { m_Coord[index] = value;}
	void x(const float x) {	m_Coord[0] = x; }
	void y(const float y) {	m_Coord[1] = y; }
	void z(const float z) {	m_Coord[2] = z; }

	// Data access 
	float Get(const unsigned int index) { return m_Coord[index];}
	float x(void) { return m_Coord[0]; }
	float y(void) { return m_Coord[1]; }
	float z(void) { return m_Coord[2]; }

	// Type
	virtual int GetType();

	// Color
	CColor *GetColor(void) { return &m_Color; }
	void SetColor(CColor &color) { m_Color.Set(color); }

	int Get_sequence_number(void) { return m_sequence_number; }
	void Set_sequence_number(const int sequence_number) { m_sequence_number = sequence_number; }
	int GetID(void) { return m_ID; }
	void SetID(const int ID) { m_ID = ID; }

	// Operators
	CVertex3dFast operator=(CVertex3dFast& vertex);
	int Diff(CVertex3dFast *pVertex);
	int Equal(CVertex3dFast *pVertex);

	virtual int glDraw();
};



// Constructor
inline CVertex3dFast::CVertex3dFast(CVertex3dFast &vertex)
{
	m_Coord[0] = vertex.x();
	m_Coord[1] = vertex.y();
	m_Coord[2] = vertex.z();
	m_sequence_number=-1;
	m_ID=0;
}

// Constructor
inline CVertex3dFast::CVertex3dFast(CVertex3dFast *pVertex)
{
	m_Coord[0] = pVertex->x();
	m_Coord[1] = pVertex->y();
	m_Coord[2] = pVertex->z();
	m_sequence_number=-1;
	m_ID=0;
}

// Constructor
inline CVertex3dFast::CVertex3dFast(CVertex3dFast *pVertex,CVector3d *pVector)
{
	m_Coord[0] = pVertex->x() + pVector->x();
	m_Coord[1] = pVertex->y() + pVector->y();
	m_Coord[2] = pVertex->z() + pVector->z();
	m_sequence_number=-1;
	m_ID=0;
}

// Constructor
inline CVertex3dFast::CVertex3dFast(const float x,const float y,const float z) 
{
	m_Coord[0] = x; 
	m_Coord[1] = y; 
	m_Coord[2] = z; 
	m_sequence_number=-1;
	m_ID=0;
}

// Constructor
inline CVertex3dFast::CVertex3dFast(const float x,const float y,const float z, CColor &color) 
{
	m_Coord[0] = x; 
	m_Coord[1] = y; 
	m_Coord[2] = z; 

	m_Color.Set(color);

	m_sequence_number=-1;
	m_ID=0;
}
// Constructor

inline CVertex3dFast::CVertex3dFast(const float x,const float y,const float z, CColor &color, int sequence_number, int ID) 
{
	m_Coord[0] = x; 
	m_Coord[1] = y; 
	m_Coord[2] = z; 

	m_Color.Set(color);

	m_sequence_number = sequence_number;
	m_ID=ID;
}

// Setting
inline void CVertex3dFast::Set(const float x,const float y,const float z)
{
	m_Coord[0] = x;
	m_Coord[1] = y;
	m_Coord[2] = z;
}

// Setting
inline void CVertex3dFast::Set(CVertex3dFast *pVertex)
{
	m_Coord[0] = pVertex->x();
	m_Coord[1] = pVertex->y();
	m_Coord[2] = pVertex->z();
}

// Setting
inline void CVertex3dFast::Set(CVertex3dFast &vertex)
{
	m_Coord[0] = vertex.x();
	m_Coord[1] = vertex.y();
	m_Coord[2] = vertex.z();
}

// Moving
inline void CVertex3dFast::Move(const float dx,
														const float dy,
														const float dz)
{
	m_Coord[0] += dx;
	m_Coord[1] += dy;
	m_Coord[2] += dz;
}

// Moving
inline void CVertex3dFast::Move(CVector3d *pVector,
														float ratio)
{
	m_Coord[0] += ratio * pVector->x();
	m_Coord[1] += ratio * pVector->y();
	m_Coord[2] += ratio * pVector->z();
}

// Moving
inline void CVertex3dFast::Move(CVector3d &vector,
														float ratio)
{
	m_Coord[0] += ratio * vector.x();
	m_Coord[1] += ratio * vector.y();
	m_Coord[2] += ratio * vector.z();
}


#endif // _VERTEX_3D_
