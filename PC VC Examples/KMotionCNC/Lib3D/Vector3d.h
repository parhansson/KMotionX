//********************************************
// Vector3d.h
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 10/12/97
// Modified : 19/01/98
//********************************************

#ifndef _VECTOR_3D_
#define _VECTOR_3D_

#include "Object3d.h"

class CVertex3d;

class CVector3d : public CObject3d
{

private :

	// Geometry
	float m_x;
	float m_y;
	float m_z;

public :

	CVector3d() { m_x = m_y = m_z = 0.0f; } 
	virtual ~CVector3d();
	
	// Constructors
	CVector3d(const float x,const float y,const float z);
	CVector3d(CVector3d &vector);
	CVector3d(CVector3d *pVector);
	CVector3d(CVertex3d *pVertex1,CVertex3d *pVertex2);
	CVector3d(CVertex3d &vertex1,CVertex3d &vertex2);

	// Debug
	void Trace();

	// Data setting
	virtual int GetType();
	void Clear(void);
	void Set(CVector3d *pVector);
	void Set(CVector3d &vector);
	void Set(CVertex3d *pVertex1,CVertex3d *pVertex2);
	void Set(const float x,const float y,const float z);
	void Copy(CVector3d &vector);
	void Copy(CVector3d *pVector);

	// Per coordinate (explicit inline functions)
	void x(float x) {	m_x = x; }
	void y(float y) {	m_y = y; }
	void z(float z) {	m_z = z; } 

	// Data access (explicit inline functions)
	float x(void) { return m_x; }
	float y(void) { return m_y; }
	float z(void) { return m_z; }

	// Operators
	CVector3d operator=(CVector3d& vector); // Setting
	void operator+=(CVector3d* pVector);
	void operator/=(float factor);
	void operator*=(float factor);

	void Inner(CVector3d& vector);
	static CVector3d Inner(CVector3d* u,CVector3d* v); // Stupid c++

	// Misc
	void NormalizeL2(void);
	void NormalizeL2(float value);
	double GetNormL2(void);
	double GetNormL2Square(void);
	int Collinear(CVector3d *pVector);
	int Collinear(CVector3d &vector);

	void RotateXY(float alpha);
	void RotateYZ(float alpha);
	void RotateZX(float alpha);
};

// External operators
CVector3d operator+(CVector3d& u,CVector3d& v); // Add
CVector3d operator-(CVector3d& u,CVector3d& v); // Subtract
CVector3d operator^(CVector3d& u,CVector3d& v); // Inner

#endif // _VECTOR_3D_
