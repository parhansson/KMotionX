//********************************************
// Vector3d.cpp
//********************************************
// class CVector3d
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 10/12/97
// Modified : 20/01/98
//********************************************

#include "..\stdafx.h"


//////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////


//********************************************
// Destructor
//********************************************
CVector3d::~CVector3d()
{
}

//********************************************
// Constructor
//********************************************
CVector3d::CVector3d(CVector3d &vector)
{
	Set(&vector);
}

//********************************************
// Constructor
//********************************************
CVector3d::CVector3d(CVector3d *pVector)
{
	Set(pVector);
}

//********************************************
// Constructor
//********************************************
CVector3d::CVector3d(const float x,
										 const float y,
										 const float z)
{
	Set(x,y,z);
}

//********************************************
// Constructor
//********************************************
CVector3d::CVector3d(CVertex3d *pVertex1,
										 CVertex3d *pVertex2)
{
	m_x = pVertex2->x()-pVertex1->x();
	m_y = pVertex2->y()-pVertex1->y();
	m_z = pVertex2->z()-pVertex1->z();
}
	
//********************************************
// Constructor
//********************************************
CVector3d::CVector3d(CVertex3d &vertex1,
										 CVertex3d &vertex2)
{
	m_x = vertex2.x()-vertex1.x();
	m_y = vertex2.y()-vertex1.y();
	m_z = vertex2.z()-vertex1.z();
}


//////////////////////////////////////////////
// DATAS
//////////////////////////////////////////////

//********************************************
// Copy
//********************************************
void CVector3d::Copy(CVector3d &vector)
{
	Set(&vector);
}

//********************************************
// Copy
//********************************************
void CVector3d::Copy(CVector3d *pVector)
{
	Set(pVector);
}


//********************************************
// GetType
//********************************************
int CVector3d::GetType()
{
	return TYPE_VECTOR3D;
}

//********************************************
// Set
//********************************************
void CVector3d::Clear(void)
{
	Set(0.0f,0.0f,0.0f);
}

//********************************************
// Set
//********************************************
void CVector3d::Set(const float x,
										const float y,
										const float z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}

//********************************************
// Set
//********************************************
void CVector3d::Set(CVector3d *pVector)
{
	Set(pVector->x(),pVector->y(),pVector->z());
}

//********************************************
// Set
//********************************************
void CVector3d::Set(CVector3d &vector)
{
	Set(vector.x(),vector.y(),vector.z());
}


//********************************************
// Set
//********************************************
void CVector3d::Set(CVertex3d *pVertex1,
										 CVertex3d *pVertex2)
{
	m_x = pVertex2->x()-pVertex1->x();
	m_y = pVertex2->y()-pVertex1->y();
	m_z = pVertex2->z()-pVertex1->z();
}


//********************************************
// Trace
//********************************************
void CVector3d::Trace()
{
	TRACE("\n");
	TRACE("** Vector **\n");
	TRACE("Adress      : %x\n",this);
	TRACE("Coordinates : (%g %g %g)\n",m_x,m_y,m_z);
}

//////////////////////////////////////////////
// OPERATORS
//////////////////////////////////////////////

//********************************************
// Operator = 
//********************************************
CVector3d CVector3d::operator=(CVector3d& vector)
{
	CVector3d v;
	v.Set(vector);
	return v;
}


//********************************************
// Operator + 
//********************************************
void CVector3d::operator+=(CVector3d* pVector)
{
	m_x += pVector->x();
	m_y += pVector->y();
	m_z += pVector->z();
}

//********************************************
// Operator /=
//********************************************
void CVector3d::operator/=(float factor)
{
	m_x /= factor;
	m_y /= factor;
	m_z /= factor;
}

//********************************************
// Operator *=
//********************************************
void CVector3d::operator*=(float factor)
{
	m_x *= factor;
	m_y *= factor;
	m_z *= factor;
}


//********************************************
// Inner
//********************************************
void CVector3d::Inner(CVector3d& vector)
{
	double x = (double)m_y*(double)vector.z()-(double)m_z*(double)vector.y();
	double y = (double)m_z*(double)vector.x()-(double)m_x*(double)vector.z();
	double z = (double)m_x*(double)vector.y()-(double)m_y*(double)vector.x();
	Set((float)x,(float)y,(float)z);
}


//////////////////////////////////////////////
//////////////////////////////////////////////
// PROCESSING
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// Normalize
//********************************************
void CVector3d::NormalizeL2(void)
{
	double norm = GetNormL2();
	if(norm != 0.0f)
	{
		m_x = (float)(m_x / norm);
		m_y = (float)(m_y / norm);
		m_z = (float)(m_z / norm);
	}
	//TRACE("norm : %g\n",GetNormL2());
}

//********************************************
// Normalize
//********************************************
void CVector3d::NormalizeL2(float value)
{
	double norm = GetNormL2();
	if(norm != 0.0f)
	{
		m_x *= (float)(value/norm);
		m_y *= (float)(value/norm);
		m_z *= (float)(value/norm);
	}
	//TRACE("norm : %g (wanted : %g)\n",GetNormL2(),value);
}
	
//********************************************
// GetNormSquare
//********************************************
double CVector3d::GetNormL2Square(void)
{
	return ((double)m_x*(double)m_x + 
		      (double)m_y*(double)m_y + 
					(double)m_z*(double)m_z);
}
	
//********************************************
// GetNorm
//********************************************
double CVector3d::GetNormL2(void)
{
	return sqrt((double)m_x*(double)m_x + 
		          (double)m_y*(double)m_y + 
							(double)m_z*(double)m_z);
}
	
//********************************************
// Collinear
//********************************************
int CVector3d::Collinear(CVector3d *pVector)
{
	float x = pVector->x() / m_x;
	float y = pVector->y() / m_y;
	float z = pVector->z() / m_z;
	return ((x == y) && (y == z));
}

//********************************************
// Collinear
//********************************************
int CVector3d::Collinear(CVector3d &vector)
{
	float x = vector.x() / m_x;
	float y = vector.y() / m_y;
	float z = vector.z() / m_z;
	return ((x == y) && (y == z));
}


//////////////////////////////////////////////
//////////////////////////////////////////////
// EXTERNAL OPERATORS
//////////////////////////////////////////////
//////////////////////////////////////////////


//********************************************
// Operator + 
//********************************************
CVector3d operator+(CVector3d& u,
                    CVector3d& v)
{
	CVector3d w;
	w.Set(u.x()+v.x(),u.y()+v.y(),u.z()+v.z());
	return w;
}

//********************************************
// Operator -
//********************************************
CVector3d operator-(CVector3d& u,
                    CVector3d& v)
{
	CVector3d w;
	w.Set(u.x()-v.x(),u.y()-v.y(),u.z()-v.z());
	return w;
}

//********************************************
// Operator ^
//********************************************
CVector3d operator^(CVector3d& u,
                    CVector3d& v)
{
	// w = u ^ v
	CVector3d w;
	w.Set(u.y()*v.z()-u.z()*v.y(),
		    u.z()*v.x()-u.x()*v.z(),
		    u.x()*v.y()-u.y()*v.x());
	return w;
}

//********************************************
// Inner
//********************************************
CVector3d CVector3d::Inner(CVector3d* u,
                           CVector3d* v)
{
	// w = u ^ v
	CVector3d w;
	w.Set(u->y()*v->z()-u->z()*v->y(),
		    u->z()*v->x()-u->x()*v->z(),
		    u->x()*v->y()-u->y()*v->x());
	return w;
}


void CVector3d::RotateXY(float alpha)
{
	float x = (float)(m_x*cos(alpha)-m_y*sin(alpha));
	float y = (float)(m_x*sin(alpha)+m_y*cos(alpha));
	m_x = x;
	m_y = y;
}
void CVector3d::RotateZX(float alpha)
{
	float z = (float)(m_z*cos(alpha)-m_x*sin(alpha));
	float x = (float)(m_z*sin(alpha)+m_x*cos(alpha));
	m_z = z;
	m_x = x;
}
void CVector3d::RotateYZ(float alpha)
{
	float y = (float)(m_y*cos(alpha)-m_z*sin(alpha));
	float z = (float)(m_y*sin(alpha)+m_z*cos(alpha));
	m_y = y;
	m_z = z;
}

// ** EOF **



