//********************************************
// Path3d.cpp
//********************************************
// class CPath3d
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 15/01/98
// Modified : 15/01/98
// Modified : 07/16/06 - Tom Kerekes
//********************************************

#include "..\stdafx.h"

//////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////

//********************************************
// Constructor
//********************************************
CPath3d::CPath3d()
{
	m_ListDone = 0;
	m_Modified = 1;
	m_Name = _T("Path");
	m_Show = 1;
	m_ListOpenGL=-1;
}

//********************************************
// Destructor
//********************************************
CPath3d::~CPath3d()
{
	Free();
}

//********************************************
// Free
//********************************************
void CPath3d::Free()
{
	//TRACE("Cleanup mesh %x\n",this);
	m_ArrayVertex.Free();
}


//////////////////////////////////////////////
// OPENGL
//////////////////////////////////////////////

//********************************************
// BuildList
//********************************************
int CPath3d::glBuildList()
{
	//TRACE(" Start building list ...\n");

	// Check for valid Path
	if(m_ArrayVertex.GetSize() == 0)
	{
		return 0;
	}

	if(!m_Modified && m_ListDone)
		return 0;

	// Erase last list
	if (m_ListOpenGL!=-1)
		::glDeleteLists(m_ListOpenGL,1);

	// Search for a new list
	m_ListOpenGL = ::glGenLists(1);
	if(m_ListOpenGL == 0)
	{
		TRACE("CPath3d::BuildList : unable to build DrawList\n");
		return 0;
	}

	// Start list
	unsigned int NbVertex = (unsigned int)m_ArrayVertex.GetSize();

	if(!NbVertex)
		return 0;

	CVertex3d *pPrev,*pNew;

	::glNewList(m_ListOpenGL,GL_COMPILE);

	// Material

	::glPushMatrix();

	// Position / translation / scaling
	glTranslatef(m_Transform.GetTranslation()->x(),
			     m_Transform.GetTranslation()->y(),
			     m_Transform.GetTranslation()->z());

	glScalef(m_Transform.GetScale()->x(),
			     m_Transform.GetScale()->y(),
			     m_Transform.GetScale()->z());

	if (m_Transform.GetValueRotation()!=0.0f)
		glRotatef(m_Transform.GetValueRotation(),
			      m_Transform.GetRotation()->x(),
			      m_Transform.GetRotation()->y(),
			      m_Transform.GetRotation()->z());

	if (m_Transform.GetValueRotationXY()!=0.0f) glRotatef(m_Transform.GetValueRotationXY(),0.0f,0.0f,1.0f);
	if (m_Transform.GetValueRotationYZ()!=0.0f) glRotatef(m_Transform.GetValueRotationYZ(),1.0f,0.0f,0.0f);
	if (m_Transform.GetValueRotationZX()!=0.0f) glRotatef(m_Transform.GetValueRotationZX(),0.0f,1.0f,0.0f);


	// path
	pPrev = m_ArrayVertex[0];
	::glBegin(GL_LINE_STRIP);
	::glColor3ub(pPrev->GetColor()->r(),pPrev->GetColor()->g(),pPrev->GetColor()->b());
	::glVertex3f(pPrev->x(),pPrev->y(),pPrev->z());
	
	for(unsigned int i=1;i<NbVertex;i++)
	{
		pNew = m_ArrayVertex[i];
	
		if (!(*pPrev->GetColor() == *pNew->GetColor()))
		{
			::glBegin(GL_LINE_STRIP);
			::glColor3ub(pNew->GetColor()->r(),pNew->GetColor()->g(),pNew->GetColor()->b());
			::glVertex3f(pPrev->x(),pPrev->y(),pPrev->z());
		}	
			
		::glVertex3f(pNew->x(),pNew->y(),pNew->z());

		pPrev=pNew;
	}

	::glEnd();
	::glPopMatrix();

	::glEndList();

	// List is done now
	m_ListDone = 1;
	m_Modified = 0;

	return 1;
}

//********************************************
// Draw
//********************************************
int CPath3d::glDraw()
{
	if(!m_Show)
		return 0;

	if(m_ArrayVertex.GetSize() == 0)
		return 0;

	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	// Build list at first
	if(!m_ListDone || m_Modified)
		glBuildList();

	// Search for a new list
	if(::glIsList(m_ListOpenGL)==GL_TRUE)
	{
		::glDisable(GL_LIGHTING);  // paths have lighting disabled
	    ::glCallList(m_ListOpenGL);
		return 1;
	}
	else
	{
		return 0;
	}
}


//////////////////////////////////////////////
// RANGE
//////////////////////////////////////////////

//********************************************
// Range
//********************************************
void CPath3d::Range(int coord, 
										float *min,
										float *max)
{
	ASSERT(coord >= 0 && coord <= 2);
	int NbVertex = m_ArrayVertex.GetSize();

	if (NbVertex==0) return;

	float Min = m_ArrayVertex[0]->Get(coord);
	float Max = Min;
	for(int i=1;i<NbVertex;i++)
	{
		float value = m_ArrayVertex[i]->Get(coord);
		if(value < Min)
			Min = value;
		if(value > Max)
			Max = value;
	}
	*min = Min;
	*max = Max;
}

//********************************************
// Range (apply)
//********************************************
void CPath3d::Range(int coord, 
										float min,
										float max)
{
	float Min,Max;
	Range(coord,&Min,&Max);
	Offset(coord,-Min);
	Scale(coord,(max-min)/(Max-Min));
	Offset(coord,min);
}

//********************************************
// Scale
//********************************************
void CPath3d::Scale(int coord,
										float scale)
{
	int NbVertex = m_ArrayVertex.GetSize();
	for(int i=0;i<NbVertex;i++)
		m_ArrayVertex[i]->Set(coord,m_ArrayVertex[i]->Get(coord) * scale);
	m_Modified = 1;
}

//********************************************
// Offset
//********************************************
void CPath3d::Offset(int coord,
										 float offset)
{
	int NbVertex = m_ArrayVertex.GetSize();
	for(int i=0;i<NbVertex;i++)
		m_ArrayVertex[i]->Set(coord,m_ArrayVertex[i]->Get(coord) + offset);
	m_Modified = 1;
}



//////////////////////////////////////////////
// DATAS
//////////////////////////////////////////////

//********************************************
// Copy
//********************************************
void CPath3d::Copy(CPath3d *pPath)
{
	// Vertices
	int NbVertex = pPath->NbVertex();
	m_ArrayVertex.SetSize(NbVertex);
	for(int i=0;i<NbVertex;i++)
		m_ArrayVertex.SetAt(i,new CVertex3d(pPath->GetVertex(i)));

	// Transform
	m_Transform.Copy(pPath->GetTransform());
}


//********************************************
// GetType
//********************************************
int CPath3d::GetType()
{
	return TYPE_PATH3D;
}



//********************************************
// DeleteVertex
//********************************************
int CPath3d::DeleteVertex(CVertex3d *pVertex)
{
	int size = m_ArrayVertex.GetSize();
	for(int i=0;i<size;i++)
	{
		CVertex3d *pV = m_ArrayVertex[i];
		if(pV == pVertex)
		{
			m_ArrayVertex.RemoveAt(i);
			delete pVertex;
		  return 1;
		}
	}
	return 0;
}

//********************************************
// DeleteVertex
//********************************************
int CPath3d::DeleteVertex(int index)
{
	if(index < m_ArrayVertex.GetSize())
	{
		CVertex3d *pVertex = (CVertex3d *)m_ArrayVertex[index];
		m_ArrayVertex.RemoveAt(index);
		delete pVertex;
		return 1;
	}
	return 0;
}


//
// area=(1/2) x Base x Height. Where the height is an altitude drawn from the base to the opposite angle. 
// This formula makes for a relatively easy calculation of the area of a triangle but it is rather difficult 
// to naturally find a triangle that is given in terms of at least one side (the base) and a height. 
// We typically can determine or are given the sides of a triangle when a triangle is present. 
// formula does exist that can calculate the area of a triangle when all three sides are known.
// This formula is attributed to Heron of Alexandria but can be traced back to Archimedes.
//
// This formula is represented by
// Area=SQRT(s(s-a)(s-b)(s-c)),
// where s=(a+b+c)/2 or perimeter/2. 
//
// Finds distance from a point to a line segment which is either:
// #1 perpendiculat distance if point is "within" the endpoints
// #2 distance to closest endpoint if outside
//
// s0 - s2 is the line
// s1 is the point

double CPath3d::FindDistPointToSegment(CVertex3d *s0, CVertex3d *s1, CVertex3d *s2)
{
	double dx = s1->x() - s0->x();
	double dy = s1->y() - s0->y();
	double dz = s1->z() - s0->z();

	double a = sqrt(dx*dx+dy*dy+dz*dz);

	dx = s2->x() - s1->x();
	dy = s2->y() - s1->y();
	dz = s2->z() - s1->z();

	double b = sqrt(dx*dx+dy*dy+dz*dz);

	dx = s2->x() - s0->x();
	dy = s2->y() - s0->y();
	dz = s2->z() - s0->z();

	double c = sqrt(dx*dx+dy*dy+dz*dz);

	double s = s=(a+b+c)/2;

	// if dist from beg to end is tiny, then 
	//    if both other sides tiny treat as collinear

	if (c <= 0.0)		
	{
		if (a<b) return a;
		else     return b;
	}

	double h,v = s*(s-a)*(s-b)*(s-c);

	if (v > 0.0)
		h = 2.0 * sqrt(v)/c;
	else
		h=0.0;

	if (a*a - h*h > c*c || b*b - h*h > c*c)
	{
		// point is outside the edge
		// return distance to closest endpoint

		if (a<b) return a;
		else     return b;
	}
	return h;
}





//********************************************
// DeleteVertex at the end that are flagged with
// sequence_number >= sequence_number
//********************************************
int CPath3d::RemovePathEnd(int sequence_number, int ID, double x, double y, double z)
{
	int n = m_ArrayVertex.GetSize()-1;

	// first verify that the line number is there
	while (n>=0)
	{
		int SeqNum = GetVertex(n)->Get_sequence_number();
		if (SeqNum == sequence_number) break;
		n--;
	}

	if (n<0) return 1;


	// now delete until we find the sequence_number
	n = m_ArrayVertex.GetSize()-1;
	while (n>=0)
	{
		if (GetVertex(n)->Get_sequence_number() == sequence_number) break;
		DeleteVertex(n);
		n--;
	}

	if (n<0) return 1;


	// find which segment we nearly pass through by
	// finding the minimum distance to all of them

	double mindist=1e99;
	int mini=0;
	n = m_ArrayVertex.GetSize()-2;
	CVertex3d Point(x,y,z);

	while (n>=0)
	{
		double dist = FindDistPointToSegment(GetVertex(n),&Point,GetVertex(n+1));
		
		if (dist < mindist)
		{
			mindist = dist;
			mini = n;
		}
			
		if (GetVertex(n)->Get_sequence_number() != sequence_number ||
			GetVertex(n)->GetID() == ID) break;
		
		n--;
	}



	// now delete all of the line number
	// or until we find the ID

	n = m_ArrayVertex.GetSize()-1;
	while (n>=0)
	{
		if (GetVertex(n)->Get_sequence_number() != sequence_number ||
			GetVertex(n)->GetID() == ID) break;

		if (n == mini+1)
		{
			// instead of deleting the last segment
			// that goes through the point
			// modify it to go to the new point
			GetVertex(n)->Set(x,y,z);
			break;
		}

		DeleteVertex(n);
		n--;
	}

	if (n<0) return 1;
	return 0;
}



