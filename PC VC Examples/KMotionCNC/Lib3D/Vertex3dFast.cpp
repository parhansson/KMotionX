//********************************************
// Vertex3dFast.cpp
//********************************************
// class CVertex3dFast
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 10/12/97
// Modified : 14/01/98
//********************************************

#include "stdafx.h"

//////////////////////////////////////////////
// DATAS
//////////////////////////////////////////////

//********************************************
// GetType
//********************************************
int CVertex3dFast::GetType()
{
	return TYPE_VERTEX3DFAST;
}



//////////////////////////////////////////////
// OPERATORS
//////////////////////////////////////////////

//********************************************
// Operator = 
//********************************************
CVertex3dFast CVertex3dFast::operator=(CVertex3dFast& vertex)
{
	CVertex3dFast v;
	v.Set(vertex);
	return v;
}




//////////////////////////////////////////////
// MISC
//////////////////////////////////////////////

//********************************************
// Diff
//********************************************
int CVertex3dFast::Diff(CVertex3dFast *pVertex)
{
	return ( (m_Coord[0] != pVertex->x()) || 
		       (m_Coord[1] != pVertex->y()) || 
					 (m_Coord[2] != pVertex->z()));
}

//********************************************
// Equal
//********************************************
int CVertex3dFast::Equal(CVertex3dFast *pVertex)
{
	return ( (m_Coord[0] == pVertex->x()) && 
		       (m_Coord[1] == pVertex->y()) && 
					 (m_Coord[2] == pVertex->z()));
}


//////////////////////////////////////////////
// OPENGL
//////////////////////////////////////////////


//********************************************
// glDraw
//********************************************
int CVertex3dFast::glDraw()
{
	GLUquadricObj* pQuadric = gluNewQuadric();

	// Main vertex
	glColor3ub(m_Color.r(),m_Color.g(),m_Color.b());
	glPushMatrix();
	glTranslated(m_Coord[0],m_Coord[1],m_Coord[2]);
	gluSphere(pQuadric,0.01,8,8); 
	glPopMatrix();
	return 1;
}


// ** EOF **

