//********************************************
// Edge3d.cpp
//********************************************
// class CEdge3d
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 10/12/97
// Modified : 16/12/97
//********************************************

#include "..\stdafx.h"

//////////////////////////////////////////////
//////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// Constructor
//********************************************
CEdge3d::CEdge3d()
{
	m_pVertex1 = NULL;
	m_pVertex2 = NULL;
	m_pFace1 = NULL;
	m_pFace2 = NULL;
	m_Flag = 0;
}


//********************************************
// Constructor
//********************************************
CEdge3d::CEdge3d(CVertex3d *pVertex1,
								 CVertex3d *pVertex2)
{
	Set(pVertex1,pVertex2);
	m_pFace1 = NULL;
	m_pFace2 = NULL;
	m_Flag = 0;
}

//********************************************
// Constructor
//********************************************
CEdge3d::CEdge3d(CVertex3d *pVertex1,
								 CVertex3d *pVertex2,
								 CFace3d *pFace1,
								 CFace3d *pFace2)
{
	Set(pVertex1,pVertex2);
	Set(pFace1,pFace2);
	m_Flag = 0;
}


//********************************************
// Constructor
//********************************************
CEdge3d::CEdge3d(CEdge3d *pEdge)
{
	Set(pEdge);
	m_Flag = 0;
}



//////////////////////////////////////////////
//////////////////////////////////////////////
// DATAS
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// GetType
//********************************************
int CEdge3d::GetType()
{
	return TYPE_EDGE3D;
}

//********************************************
// Set
//********************************************
void CEdge3d::Set(CVertex3d *pVertex1,
									CVertex3d *pVertex2)
{
	m_pVertex1 = pVertex1;
	m_pVertex2 = pVertex2;
}

//********************************************
// Set
//********************************************
void CEdge3d::Set(CFace3d *pFace1,
									CFace3d *pFace2)
{
	m_pFace1 = pFace1;
	m_pFace2 = pFace2;
}


//********************************************
// Set
//********************************************
void CEdge3d::Set(CEdge3d *pEdge)
{
	Set(pEdge->v1(),pEdge->v2());
	Set(pEdge->f1(),pEdge->f2());
}

//////////////////////////////////////////////
//////////////////////////////////////////////
// PROCESSING
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// sharp
// threshold : sin(angle)
//********************************************
int CEdge3d::sharp(float threshold)
{
	// Check
	if(m_pFace1 == NULL || 
	   m_pFace2 == NULL)
		return 0;

	return (SinAngle(m_pFace1,m_pFace2) >= threshold);
}

//********************************************
// Equal (order independant)
//********************************************
int CEdge3d::Equal(CEdge3d *pEdge)
{
	int success = 0;
	success += m_pVertex1 == pEdge->v1();
	success += m_pVertex1 == pEdge->v2();
	success += m_pVertex2 == pEdge->v1();
	success += m_pVertex2 == pEdge->v2();
	return (success >= 2);
}


//********************************************
// GetVertexDiff
//********************************************
CVertex3d *CEdge3d::GetVertexDiff(CVertex3d *pVertexDiff)
{
	if(m_pVertex1 != pVertexDiff)
		return m_pVertex1;
	else
	{
		ASSERT(m_pVertex2 != pVertexDiff);
		return m_pVertex2;
	}
}

//////////////////////////////////////////////
//////////////////////////////////////////////
// OPENGL
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// glDrawHighlight
// Highlights edge
//********************************************
void CEdge3d::glDrawHighlight(unsigned char *color)
{
	// Color
	glColor3ub(color[0],color[1],color[2]);

	::glBegin(GL_LINES);
		::glVertex3f(m_pVertex1->x(),m_pVertex1->y(),m_pVertex1->z());
		::glVertex3f(m_pVertex2->x(),m_pVertex2->y(),m_pVertex2->z());
	::glEnd();
}

//********************************************
// glDrawHighlight
// Highlights edge and neighbors
//********************************************
void CEdge3d::glDrawHighlight(unsigned char *color,
															unsigned char *ColorNeighbor)
{
	// Color
	glColor3ub(color[0],color[1],color[2]);

	::glBegin(GL_LINES);
		::glVertex3f(m_pVertex1->x(),m_pVertex1->y(),m_pVertex1->z());
		::glVertex3f(m_pVertex2->x(),m_pVertex2->y(),m_pVertex2->z());
	::glEnd();

	m_pFace1->glDraw(ColorNeighbor);
	m_pFace2->glDraw(ColorNeighbor);
}




// ** EOF **



