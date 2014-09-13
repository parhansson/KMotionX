//********************************************
// Vertex3d.cpp
//********************************************
// class CVertex3d
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 10/12/97
// Modified : 14/01/98
//********************************************

#include "..\stdafx.h"

//////////////////////////////////////////////
// DATAS
//////////////////////////////////////////////

//********************************************
// GetType
//********************************************
int CVertex3d::GetType()
{
	return TYPE_VERTEX3D;
}



//////////////////////////////////////////////
// OPERATORS
//////////////////////////////////////////////

//********************************************
// Operator = 
//********************************************
CVertex3d CVertex3d::operator=(CVertex3d& vertex)
{
	CVertex3d v;
	v.Set(vertex);
	return v;
}


//////////////////////////////////////////////
// VERTEX NEIGHBORING
//////////////////////////////////////////////

//********************************************
// AddNeighbor
//********************************************
void CVertex3d::AddNeighbor(CVertex3d *pVertex)
{
	// Duplicate vertices are removed
	if(pVertex == this)
		return;

	if(!HasNeighbor(pVertex))
		m_ArrayVertexNeighbor.Add(pVertex);
}

//********************************************
// AddNeighborNoCheck
//********************************************
void CVertex3d::AddNeighborNoCheck(CVertex3d *pVertex)
{
	// Add a neighboring vertex
	m_ArrayVertexNeighbor.Add(pVertex);
}


//********************************************
// NbNeighbor
//********************************************
int CVertex3d::NbVertexNeighbor(void)
{
	return m_ArrayVertexNeighbor.GetSize();
}

//********************************************
// RemoveAllNeighbor
//********************************************
void CVertex3d::RemoveAllVertexNeighbor(void)
{
	m_ArrayVertexNeighbor.RemoveAll();
}

//********************************************
// RemoveVertexNeighbor
//********************************************
int CVertex3d::RemoveNeighbor(CVertex3d *pVertex)
{
	int size = m_ArrayVertexNeighbor.GetSize();
	for(int i=0;i<size;i++)
		if(m_ArrayVertexNeighbor[i]==pVertex)
		{
			m_ArrayVertexNeighbor.RemoveAt(i);
			return 1;
		}
	return 0;
}

//********************************************
// UpdateNeighbor
//********************************************
int CVertex3d::UpdateNeighbor(CVertex3d *pVertexOld,
															CVertex3d *pVertexNew)
{
	int success = 0;
	for(int j=0;j<m_ArrayVertexNeighbor.GetSize();j++)
		if(m_ArrayVertexNeighbor[j] == pVertexOld)
		{
			m_ArrayVertexNeighbor.RemoveAt(j);
			this->AddNeighbor(pVertexNew);
			pVertexNew->AddNeighbor(this);
			success = 1;
		}
	return success;
}

//********************************************
// UpdateNeighbor
//********************************************
void CVertex3d::UpdateNeighborRecursive(CVertex3d *pVertexOld,
																				CVertex3d *pVertexNew)
{
	for(int i=0;i<m_ArrayVertexNeighbor.GetSize();i++)
		if(m_ArrayVertexNeighbor[i]!=NULL)
			m_ArrayVertexNeighbor[i]->UpdateNeighbor(pVertexOld,pVertexNew);
}

//********************************************
// FindFaceAroundContainVertex
//********************************************
int CVertex3d::FindFaceAroundContainVertex(CVertex3d *pVertex,
																					 CArray3d<CFace3d> &ArrayFace)
{
	int success = 0;
	for(int i=0;i<m_ArrayFaceNeighbor.GetSize();i++)
		if(m_ArrayFaceNeighbor[i] != NULL)
			if(m_ArrayFaceNeighbor[i]->HasVertex(pVertex))
			{
				ArrayFace.Add(m_ArrayFaceNeighbor[i]);
				success = 1;
			}
	return success;
}


//********************************************
// GetMeanAreaAround
//********************************************
double CVertex3d::GetMeanAreaAround()
{
	return ::GetMeanArea(&m_ArrayFaceNeighbor);
}

//********************************************
// GetMinAreaAround
//********************************************
double CVertex3d::GetMinAreaAround()
{
	return ::GetMinArea(&m_ArrayFaceNeighbor);
}

//////////////////////////////////////////////
// FACE NEIGHBORING
//////////////////////////////////////////////

//********************************************
// AddFaceNeighbor
//********************************************
void CVertex3d::AddNeighbor(CFace3d *pFace)
{
	// Add a neighboring face only when it
	// is not included in the array
	if(!HasNeighbor(pFace))
		m_ArrayFaceNeighbor.Add(pFace);
}


//********************************************
// NbFaceNeighbor
//********************************************
int CVertex3d::NbFaceNeighbor(void)
{
	return m_ArrayFaceNeighbor.GetSize();
}

//********************************************
// RemoveAllFaceNeighbor
//********************************************
void CVertex3d::RemoveAllFaceNeighbor(void)
{
	m_ArrayFaceNeighbor.RemoveAll();
}

//********************************************
// RemoveFaceNeighbor
//********************************************
int CVertex3d::RemoveNeighbor(CFace3d *pFace)
{
	int size = m_ArrayFaceNeighbor.GetSize();
	for(int i=0;i<size;i++)
		if(m_ArrayFaceNeighbor[i]==pFace)
		{
			m_ArrayFaceNeighbor.RemoveAt(i);
			return 1;
		}
	return 0;
}

//********************************************
// Find each faces sharing between pVertex
// and "this"
// We assume adjacency is built
//********************************************
int CVertex3d::FindSharingFaces(CVertex3d *pVertex,
															  CArray3d<CFace3d> &array)
{
	int added = 0;
	int NbFaceNeighbor = pVertex->NbFaceNeighbor();
	for(int i=0;i<NbFaceNeighbor;i++)
	{
		if(m_ArrayFaceNeighbor.Has(pVertex->GetFaceNeighbor(i)))
		{
			array.Add(pVertex->GetFaceNeighbor(i));
			added++;
		}
	}
	return added;
}


//////////////////////////////////////////////
// BOUNDARIES
//////////////////////////////////////////////


//********************************************
// IsOnBoundary
//********************************************
int CVertex3d::IsOnBoundary()
{
	// ** DEBUG **
	int size = m_ArrayFaceNeighbor.GetSize();
	for(int i=0;i<size;i++)
		for(int j=0;j<3;j++)
			if(m_ArrayFaceNeighbor[i]->f(j) == NULL && 
				 (m_ArrayFaceNeighbor[i]->v(j) == this || 
				  m_ArrayFaceNeighbor[i]->v((j+1)%3) == this))
				return 1;
	return 0;
}

//////////////////////////////////////////////
// FLAGS
//////////////////////////////////////////////

int CVertex3d::HasNeighborWithFlag(int flag)
{
	int size = NbVertexNeighbor();
	for(int i=0;i<size;i++)
	{
		CVertex3d *pVertex = m_ArrayVertexNeighbor[i];
		if(pVertex->GetFlag() == flag)
			return 1;
	}
	return 0;
}

int CVertex3d::HasNeighborWithFlagButDiff(int flag,
																					CVertex3d *pVertexDiff)
{
	int size = NbVertexNeighbor();
	for(int i=0;i<size;i++)
	{
		CVertex3d *pVertex = m_ArrayVertexNeighbor[i];
		if(pVertex != pVertexDiff)
			if(pVertex->GetFlag() == flag)
				return 1;
	}
	return 0;
}



CVertex3d *CVertex3d::GetFirstVertexNeighborWithFlag(int flag)
{
	int size = NbVertexNeighbor();
	for(int i=0;i<size;i++)
	{
		CVertex3d *pVertex = m_ArrayVertexNeighbor[i];
		if(pVertex->GetFlag() == flag)
			return pVertex;
	}
	return NULL;
}

CVertex3d *CVertex3d::GetNearestVertexNeighborWithFlag(int flag)
{
	double distance = 1e38;
	CVertex3d *pVertexNearest = NULL;
	int size = NbVertexNeighbor();
	for(int i=0;i<size;i++)
	{
		CVertex3d *pVertex = m_ArrayVertexNeighbor[i];
		if(pVertex->GetFlag() == flag)
		{
			double tmp = ::DistanceSquare(this,pVertex);
			if(tmp < distance)
			{
				pVertexNearest = pVertex;
				distance = tmp;
			}
		}

	}
	return pVertexNearest;
}


int CVertex3d::DiffFlagsOnNeighboringFaces()
{
	int size = NbFaceNeighbor();
	if(size <= 1)
		return 0;

	int flag = m_ArrayFaceNeighbor[0]->GetFlag(); // first flag
	for(int i=1;i<size;i++)
	{
		CFace3d *pFace = m_ArrayFaceNeighbor[i];
		int tmp = pFace->GetFlag();
		if(tmp != flag)
			return 1;
	}
	return 0;
}


int CVertex3d::IsFlagSmallestOnNeighboringFaces(int flag)
{
	int size = NbFaceNeighbor();
	for(int i=0;i<size;i++)
	{
		CFace3d *pFace = m_ArrayFaceNeighbor[i];
		int tmp = pFace->GetFlag();
		if(tmp < flag)
			return 0;
	}
	return 1;
}


CVertex3d *CVertex3d::GetFirstVertexNeighborWithFlagButDiff(int flag,
																														CVertex3d *pVertexDiff)
{
	int size = NbVertexNeighbor();
	for(int i=0;i<size;i++)
	{
		CVertex3d *pVertex = m_ArrayVertexNeighbor[i];
		if(pVertex != pVertexDiff)
			if(pVertex->GetFlag() == flag)
				return pVertex;
	}
	return NULL;
}


int CVertex3d::NbVertexNeighborWithFlag(int flag)
{
	int nb = 0;
	int size = NbVertexNeighbor();
	for(int i=0;i<size;i++)
		if(m_ArrayVertexNeighbor[i]->GetFlag() == flag)
			nb++;
	return nb;
}

int CVertex3d::FindVertexNeighborsWhichFlagIsGreater(int flag,
																										 CArray3d<CVertex3d> &array)
{
	int added = 0;
	int size = NbVertexNeighbor();
	for(int i=0;i<size;i++)
	{
		CVertex3d *pVertex = m_ArrayVertexNeighbor[i];
		if(pVertex->GetFlag() > flag)
		{
			array.Add(pVertex);
			added++;
		}
	}
	return added;
}

int CVertex3d::FindVertexNeighborsWhichFlagIsSmaller(int flag,
																										 CArray3d<CVertex3d> &array)
{
	int added = 0;
	int size = NbVertexNeighbor();
	for(int i=0;i<size;i++)
	{
		CVertex3d *pVertex = m_ArrayVertexNeighbor[i];
		if(pVertex->GetFlag() < flag)
		{
			array.Add(pVertex);
			added++;
		}
	}
	return added;
}

//********************************************
// 	GetFirstNeighborWithFlagAndSharingFaceWithFlag
//********************************************
CVertex3d *CVertex3d::GetFirstNeighborWithFlagAndSharingFaceWithFlag(int FlagVertex,
																																		 int FlagFace)
{
	int size = NbVertexNeighbor();
	for(int i=0;i<size;i++)
	{
		CVertex3d *pVertex = m_ArrayVertexNeighbor[i];
		if(pVertex->GetFlag() == FlagVertex)
		{
			CArray3d<CFace3d> array;
			if(FindSharingFaces(pVertex,array))
			{
				int NbFace = array.GetSize();
				for(int j=0;j<NbFace;j++)
				{
					CFace3d *pFace = array[j];
					if(pFace->GetFlag() == FlagFace)
						return pVertex;
				}
			}
		}
	}
	return NULL;
}




//////////////////////////////////////////////
// SHARP EDGES
//////////////////////////////////////////////

//********************************************
// NbSharpEdge
//********************************************
int CVertex3d::NbSharpEdge(const double threshold)
{
	// 3 sharp edges on boundaries
	if(IsOnBoundary())
		return 3;
	int NbSharp = ::NbSharpEdge(m_ArrayFaceNeighbor,threshold);
	//TRACE("NbSharp : %d\n",NbSharp);
	return NbSharp;
}

//********************************************
// 	NormalSum
//********************************************
int CVertex3d::NormalSum(double *pSum)
{
	return ::NormalSum(m_ArrayFaceNeighbor,pSum);
}

//********************************************
// 	NormalMax
//********************************************
int CVertex3d::NormalMax(double *pMax)
{
	return ::NormalMax(m_ArrayFaceNeighbor,pMax);
}


//////////////////////////////////////////////
//////////////////////////////////////////////
// CURVENESS
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// GetMeanCurveAround
//********************************************
double CVertex3d::GetMeanCurveAround()
{
	int size = m_ArrayFaceNeighbor.GetSize();
	if(size == 0)
		return 0.0f;
	double sum;
	::NormalSum(m_ArrayFaceNeighbor,&sum);
	return sum/size;
}

//********************************************
// GetSumCurveAround
//********************************************
double CVertex3d::GetSumCurveAround()
{
	double sum;
	if(::NormalSum(m_ArrayFaceNeighbor,&sum))
		return sum;
	else
		return 0.0;
}

//********************************************
// GetMaxCurveAround
//********************************************
double CVertex3d::GetMaxCurveAround()
{
	double max;
	if(::NormalMax(m_ArrayFaceNeighbor,&max))
		return max;
	else
	{
		TRACE("** CVertex3d::GetMaxCurveAround\n");
		return 0.0;
	}
}

double CVertex3d::GetMaxAngleAround()
{
	double max;
	if(::MaxAngleBetweenFaces(m_ArrayFaceNeighbor,&max))
		return max;
	else
		return 0.0;
}


//********************************************
// GetMaxCurveAroundAndNeighbors
//********************************************
double CVertex3d::GetMaxCurveAroundAndNeighbors()
{
	double max = 0.0;
	::NormalMax(m_ArrayFaceNeighbor,&max);
	int size = m_ArrayVertexNeighbor.GetSize();
	for(int i=0;i<size;i++)
		max = max(max,m_ArrayVertexNeighbor[i]->GetMaxCurveAround());
	return max;
}




//////////////////////////////////////////////
// MISC
//////////////////////////////////////////////

//********************************************
// Diff
//********************************************
int CVertex3d::Diff(CVertex3d *pVertex)
{
	return ( (m_Coord[0] != pVertex->x()) || 
		       (m_Coord[1] != pVertex->y()) || 
					 (m_Coord[2] != pVertex->z()));
}

//********************************************
// Equal
//********************************************
int CVertex3d::Equal(CVertex3d *pVertex)
{
	return ( (m_Coord[0] == pVertex->x()) && 
		       (m_Coord[1] == pVertex->y()) && 
					 (m_Coord[2] == pVertex->z()));
}


//////////////////////////////////////////////
// PRECISION
//////////////////////////////////////////////

//********************************************
// ReplaceOnGrid
//********************************************
void CVertex3d::ReplaceOnGrid(float threshold)
{
	for(int i=0;i<3;i++)
		m_Coord[i] = (m_Coord[i] < threshold) ? 0.0f : m_Coord[i];
}


//********************************************
// GetMeanLengthEdgeAround
//********************************************
double CVertex3d::GetMeanLengthEdgeAround()
{
	double sum = 0.0f;
	int size = m_ArrayVertexNeighbor.GetSize();
	if(size == 0)
		return 0.0f;

	for(int i=0;i<size;i++)
		sum += ::Distance(m_ArrayVertexNeighbor[i],this);

	return (sum/size);
}




//////////////////////////////////////////////
// DEBUG
//////////////////////////////////////////////

//********************************************
// Trace
//********************************************
void CVertex3d::Trace()
{
	TRACE("\n");
	TRACE("** Vertex **\n");
	TRACE("Adress      : %x\n",this);
	TRACE("Coordinates : (%g %g %g)\n",m_Coord[0],m_Coord[1],m_Coord[2]);
	TRACE("Normal      : (%g %g %g)\n",m_Normal.x(),m_Normal.y(),m_Normal.z());
	TRACE("Neighbors   : %d\n",m_ArrayVertexNeighbor.GetSize());
	int size = m_ArrayFaceNeighbor.GetSize();
	for(int i=0;i<size;i++)
	{
		m_ArrayFaceNeighbor[i]->Trace();
		ASSERT(m_ArrayFaceNeighbor[i]->IsValid());
	}
}


//////////////////////////////////////////////
// OPENGL
//////////////////////////////////////////////

//********************************************
// glDrawHighlight
// Highlights vertex and its neighbors
//********************************************
void CVertex3d::glDrawHighlight(const float radius,
																const float RadiusNeighbor,
																unsigned char *ColorVertex,
																CMesh3d *pMesh /* = NULL */,
																unsigned char *ColorNeightbor /* = NULL */)
{
	GLUquadricObj* pQuadric = gluNewQuadric();


	// Transform
	if(pMesh != NULL)
		{
			CTransform *pTransform = pMesh->GetTransform();
		::glPushMatrix();

			// Position / translation / scaling
			glTranslatef(pTransform->GetTranslation()->x(),
						 pTransform->GetTranslation()->y(),
						 pTransform->GetTranslation()->z());

			glScalef(pTransform->GetScale()->x(),
					 pTransform->GetScale()->y(),
					 pTransform->GetScale()->z());

			if (pTransform->GetValueRotation()!=0.0f)
				glRotatef(pTransform->GetValueRotation(),
						  pTransform->GetRotation()->x(),
						  pTransform->GetRotation()->y(),
						  pTransform->GetRotation()->z());

			if (pTransform->GetValueRotationXY()!=0.0f) glRotatef(pTransform->GetValueRotationXY(),0.0f,0.0f,1.0f);
			if (pTransform->GetValueRotationYZ()!=0.0f) glRotatef(pTransform->GetValueRotationYZ(),1.0f,0.0f,0.0f);
			if (pTransform->GetValueRotationZX()!=0.0f) glRotatef(pTransform->GetValueRotationZX(),0.0f,1.0f,0.0f);

		}

	// Main vertex
	glColor3ub(ColorVertex[0],ColorVertex[1],ColorVertex[2]);
	glPushMatrix();
	glTranslated(m_Coord[0],m_Coord[1],m_Coord[2]);
	gluSphere(pQuadric,radius,8,8); 
	glPopMatrix();

	// Neighbors
	if(ColorNeightbor != NULL)
		{
		int size = NbVertexNeighbor();
		if(size != 0)
			{
			glColor3ub(ColorNeightbor[0],ColorNeightbor[1],ColorNeightbor[2]);
			for(int i=0;i<size;i++)
				{
				CVertex3d *pVertex = (CVertex3d *)m_ArrayVertexNeighbor[i];
				glPushMatrix();
				glTranslated(pVertex->x(),pVertex->y(),pVertex->z());
				gluSphere(pQuadric,RadiusNeighbor,8,8); 
				glPopMatrix();
				}
			}
		}

	// Face neighbors
	int size = NbFaceNeighbor();
	if(size != 0)
		{
		glColor3ub(0,0,255);
		for(int i=0;i<size;i++)
			{
			CFace3d *pFace = m_ArrayFaceNeighbor[i];
			::glBegin(GL_TRIANGLES);
				::glVertex3f(pFace->v1()->x(),pFace->v1()->y(),pFace->v1()->z());
				::glVertex3f(pFace->v2()->x(),pFace->v2()->y(),pFace->v2()->z());
				::glVertex3f(pFace->v3()->x(),pFace->v3()->y(),pFace->v3()->z());
			::glEnd();
			}
		}

	// Restore
	if(pMesh != NULL)
		::glPopMatrix();

}

//********************************************
// glDraw
//********************************************
int CVertex3d::glDraw()
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

//********************************************
// glDraw
//********************************************
void CVertex3d::glDraw(const float radius,
											 unsigned char *ColorVertex,
											 CMesh3d *pMesh /* = NULL */)
{
	GLUquadricObj* pQuadric = gluNewQuadric();

	// Transform
	if(pMesh != NULL)
		{
			CTransform *pTransform = pMesh->GetTransform();
		::glPushMatrix();

			// Position / translation / scaling
			glTranslatef(pTransform->GetTranslation()->x(),
						 pTransform->GetTranslation()->y(),
						 pTransform->GetTranslation()->z());

			glScalef(pTransform->GetScale()->x(),
					 pTransform->GetScale()->y(),
					 pTransform->GetScale()->z());

			if (pTransform->GetValueRotation()!=0.0f)
				glRotatef(pTransform->GetValueRotation(),
						  pTransform->GetRotation()->x(),
						  pTransform->GetRotation()->y(),
						  pTransform->GetRotation()->z());

			if (pTransform->GetValueRotationXY()!=0.0f) glRotatef(pTransform->GetValueRotationXY(),0.0f,0.0f,1.0f);
			if (pTransform->GetValueRotationYZ()!=0.0f) glRotatef(pTransform->GetValueRotationYZ(),1.0f,0.0f,0.0f);
			if (pTransform->GetValueRotationZX()!=0.0f) glRotatef(pTransform->GetValueRotationZX(),0.0f,1.0f,0.0f);

		}

	// Main vertex
	glColor3ub(ColorVertex[0],ColorVertex[1],ColorVertex[2]);
	glPushMatrix();
	glTranslated(m_Coord[0],m_Coord[1],m_Coord[2]);
	gluSphere(pQuadric,radius,8,8); 
	gluDeleteQuadric(pQuadric);
	glPopMatrix();

	// Restore
	if(pMesh != NULL)
		::glPopMatrix();
}




// ** EOF **

