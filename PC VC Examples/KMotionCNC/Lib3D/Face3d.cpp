//********************************************
// Face3d.cpp
//********************************************
// class CFace3d
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 10/12/97
// Modified : 09/02/98
//********************************************

#include "..\stdafx.h"

//////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////

//********************************************
// Constructor
//********************************************
CFace3d::CFace3d()
{
	int i;
	for(i=0;i<3;i++)
	  m_pFace[i] = NULL;
	for(i=0;i<6;i++)
	  m_pVertex[i] = NULL;
	m_Flag = 0;
}

//********************************************
// Constructor
//********************************************
CFace3d::CFace3d(CVertex3d *pVertex1,
								 CVertex3d *pVertex2,
								 CVertex3d *pVertex3)
{
	int i;

	// Face
	for(i=0;i<3;i++)
	  m_pFace[i] = NULL;
	// Vertices
	Set(pVertex1,pVertex2,pVertex3);
	for(i=3;i<6;i++)
	  m_pVertex[i] = NULL;

	// Normal
	m_Normal.Set(0.0f,0.0f,0.0f);

	m_Flag = 0;
}


//********************************************
// Constructor
//********************************************
CFace3d::CFace3d(CFace3d *pFace)
{
	Set(pFace);
	m_Flag = 0;
}


//////////////////////////////////////////////
// DATAS
//////////////////////////////////////////////

//********************************************
// Set
//********************************************
inline void CFace3d::Clear()
{
	int i;
	for(i=0;i<3;i++)
	  m_pFace[i] = NULL;
	for(i=0;i<6;i++)
	  m_pVertex[i] = NULL;
	m_Flag = 0;
}

//********************************************
// Equal
//********************************************
int CFace3d::Equal(CFace3d *pFace)
{
	return (HasVertex(pFace->v1()) && 
		      HasVertex(pFace->v2()) &&
					HasVertex(pFace->v3()));
}

//********************************************
// IsValid
//********************************************
int CFace3d::IsValid()
{
	int success = 1;

	success &= (m_pFace[0] != this);
	success &= (m_pFace[1] != this);
	success &= (m_pFace[2] != this);

	// Different neighbors, if there are
	success &= ((m_pFace[0] != m_pFace[1]) || (m_pFace[0] == NULL));
	success &= ((m_pFace[0] != m_pFace[2]) || (m_pFace[0] == NULL));
	success &= ((m_pFace[1] != m_pFace[2]) || (m_pFace[1] == NULL));

#ifdef _DEBUG
	if(!success)
		{
		TRACE("Face [%x] has the same neighbors\n",this);
		}
#endif

	// Different vertices
	success &= (m_pVertex[0] != m_pVertex[1]);
	success &= (m_pVertex[1] != m_pVertex[2]);
	success &= (m_pVertex[0] != m_pVertex[2]);

/*
#ifdef _DEBUG
	if(NbFaceNeighbor() != 3)
		{
		TRACE("Face [%x] has %d neighbors\n",NbFaceNeighbor());
		}
#endif
*/

#ifdef _DEBUG
	if(!success)
		{
		TRACE("Face [%x] has the same vertices \n",this);
		}
#endif

	// Reciproc. neighboring
	for(int i=0;i<3;i++)
	{
		CFace3d *pFace = f(i);
		if(pFace != NULL)
		{
			if(!pFace->HasNeighbor(this))
			{
			TRACE("Face [%x] has invalid reciproc. neighboring \n",this);
			success = 0;
			}
		}
	}

	return success;
}

//********************************************
// Set
//********************************************
inline void CFace3d::Set(CVertex3d *pVertex1,
												 CVertex3d *pVertex2,
												 CVertex3d *pVertex3)
{
	m_pVertex[0] = pVertex1;
	m_pVertex[1] = pVertex2;
	m_pVertex[2] = pVertex3;
}

//********************************************
// Set
//********************************************
inline void CFace3d::Set(CFace3d *pFace1,
												 CFace3d *pFace2,
												 CFace3d *pFace3)
{
	m_pFace[0] = pFace1;
	m_pFace[1] = pFace2;
	m_pFace[2] = pFace3;
}

//********************************************
// Set
//********************************************
inline void CFace3d::Set(CVertex3d *pVertex1,
												 CVertex3d *pVertex2,
												 CVertex3d *pVertex3,
												 CFace3d *pFace1,
												 CFace3d *pFace2,
												 CFace3d *pFace3)
{
	m_pVertex[0] = pVertex1;
	m_pVertex[1] = pVertex2;
	m_pVertex[2] = pVertex3;
	m_pFace[0] = pFace1;
	m_pFace[1] = pFace2;
	m_pFace[2] = pFace3;
}

//********************************************
// Set
//********************************************
inline void CFace3d::Set(CFace3d *pFace)
{
	Set(pFace->v1(),pFace->v2(),pFace->v3());
	Set(pFace->f1(),pFace->f2(),pFace->f3());
}

//********************************************
// SetFlagOnVerticesIfDiff
//********************************************
void CFace3d::SetFlagOnVerticesIfDiff(int FlagDiff,
																			int flag)
{
	for(int i=0;i<3;i++)
	{
		CVertex3d *pVertex = v(i);
		if(pVertex->GetFlag() != FlagDiff)
			pVertex->SetFlag(flag);
	}
}


//********************************************
// IndexFrom
//********************************************
int CFace3d::IndexFrom(CVertex3d *pVertex)
{
	ASSERT(HasVertex(pVertex));
	for(int i=0;i<3;i++)
		if(m_pVertex[i]==pVertex)
			return i;
	return 0;
}


//********************************************
// GetCenter
// Allocate on the heap
//********************************************
CVertex3d* CFace3d::GetCenter(void)
{
	CVertex3d* pVertex = new CVertex3d;
	pVertex->x((m_pVertex[0]->x()+m_pVertex[1]->x()+m_pVertex[2]->x())/3.0f);
	pVertex->y((m_pVertex[0]->y()+m_pVertex[1]->y()+m_pVertex[2]->y())/3.0f);
	pVertex->z((m_pVertex[0]->z()+m_pVertex[1]->z()+m_pVertex[2]->z())/3.0f);
	return pVertex;
}

//********************************************
// FindNearestVertex
//********************************************
CVertex3d *CFace3d::FindNearestVertex(CVertex3d *pVertex)
{
	CVertex3d *pV = v(0);
	double MinDistance = DistanceSquare(pVertex,v(0));
	for(int i=1;i<3;i++)
	{
		double tmp = DistanceSquare(pVertex,v(i));
		if(tmp < MinDistance)
		{
			MinDistance = tmp;
			pV = v(i);
		}
	}
	return pV;
}


//********************************************
// ColorSharpEdge
//********************************************
void CFace3d::ColorSharpEdge(double threshold,
														 CColor &color)
{
	for(int i=0;i<3;i++)
		if(SinAngle(this,f(i)) >= threshold)
		{
			v(i)->SetColor(color);
			v((i+1)%3)->SetColor(color);
		}
}

//********************************************
// ColorSharpEdge
//********************************************
int CFace3d::GetSharpEdge(double threshold,
													int *SharpEdge)
{
	int success = 0;
	for(int i=0;i<3;i++)
		if(f(i) != NULL)
			if(SinAngle(this,f(i)) >= threshold)
			{
				SharpEdge[i]=1;			
				success = 1;
			}
	return success;
}

//********************************************
// HasSharpEdge
//********************************************
int CFace3d::HasSharpEdge(double threshold)
{
	for(int i=0;i<3;i++)
	{
		double sinus = SinAngle(this,f(i));
		//TRACE("SinAngle : %g\n",sinus);
		if(sinus >= threshold)
			return 1;
	}
	return 0;
}


//////////////////////////////////////////////
// DATA ACCESS
//////////////////////////////////////////////

//********************************************
// GetType
//********************************************
int CFace3d::GetType()
{
	return TYPE_FACE3D;
}

//********************************************
// NbVertex
//********************************************
int CFace3d::NbVertex()
{
	int NbVertex = 0;
	for(int i=0;i<6;i++)
	  NbVertex += (m_pVertex[i] != NULL);
	return NbVertex;
}

//********************************************
// NbFaceNeighbor
//********************************************
int CFace3d::NbFaceNeighbor()
{
	int NbFace = 0;
	NbFace += (m_pFace[0] != NULL);
	NbFace += (m_pFace[1] != NULL);
	NbFace += (m_pFace[2] != NULL);
	return NbFace;
}


//////////////////////////////////////////////
// PROCESSING
//////////////////////////////////////////////


//********************************************
// CalculateNormal
//********************************************
void CFace3d::CalculateNormal()
{
	CVector3d u(m_pVertex[0],m_pVertex[1]);
	CVector3d v(m_pVertex[0],m_pVertex[2]);
	u.Inner(v);
	m_Normal.Set(u);
	m_Normal.NormalizeL2();
}





//////////////////////////////////////////////
// MISC
//////////////////////////////////////////////

//********************************************
// HasVertex
//********************************************
int CFace3d::HasVertex(CVertex3d *pVertex)
{
	return (m_pVertex[0] == pVertex ||
		      m_pVertex[1] == pVertex ||
		      m_pVertex[2] == pVertex);
}

//********************************************
// HasVertexWithFlag
//********************************************
int CFace3d::HasVertexWithFlag(int flag)
{
	return (m_pVertex[0]->GetFlag() == flag ||
		      m_pVertex[1]->GetFlag() == flag ||
		      m_pVertex[2]->GetFlag() == flag);
}

//********************************************
// HasVertex
//********************************************
int CFace3d::HasVertex(CVertex3d *pVertex,
											 int *index)
{
	for(int i=0;i<3;i++)
		if(m_pVertex[i] == pVertex)
			{
			*index = i;
			return 1;
			}
	return 0;
}

//********************************************
// HasVertex
//********************************************
int CFace3d::HasNeighbor(CFace3d *pFace)
{
	return (m_pFace[0] == pFace ||
		      m_pFace[1] == pFace ||
		      m_pFace[2] == pFace);
}


//********************************************
// HasVertex
//********************************************
int CFace3d::HasNeighbor(CFace3d *pFace,
												 int *index)
{
	for(int i=0;i<3;i++)
		if(m_pFace[i] == pFace)
			{
			*index = i;
			return 1;
			}
	return 0;
}


//********************************************
// GetNeighborExclusive
// Get neighboring face wich has pVertexHas
// and has not pVertexHasNot
//********************************************
CFace3d *CFace3d::GetNeighborExclusive(CVertex3d *pVertexHas,
																			 CVertex3d *pVertexHasNot)
{
	for(int i=0;i<3;i++)
	{
		CFace3d *pFace = f(i);
		if(pFace != NULL)
				if(pFace->HasVertex(pVertexHas) && 
					!pFace->HasVertex(pVertexHasNot))
					return pFace;
	}	
	return NULL;
}

//********************************************
// GetVertexExclusive
//********************************************
CVertex3d *CFace3d::GetVertexExclusive(CVertex3d *pV0,
																			 CVertex3d *pV1)
{
	for(int i=0;i<3;i++)
		if(v(i) != pV0 && v(i) != pV1)
			return v(i);
	return NULL;
}


//********************************************
// GetVertexExclusive
// Return vertex common to this and pFace,
// but pV
//********************************************
CVertex3d *CFace3d::GetVertexExclusive(CVertex3d *pV,
																			 CFace3d *pFace)
{
	for(int i=0;i<3;i++)
		if(v(i) != pV && pFace->HasVertex(v(i)))
			return v(i);
	return NULL;
}


//********************************************
// GetFaceNeighborExclusive
//********************************************
CFace3d *CFace3d::GetFaceNeighborExclusive(CFace3d *pF0,
																					 CFace3d *pF1)
{
	for(int i=0;i<3;i++)
		if(f(i) != pF0 && f(i) != pF1)
			return f(i);
	return NULL;
}


//********************************************
// GetVertexFaceNeighborExclusive
// Get vertex on neighboring face, which is not 
// on "this".
// index : neighboring face
//********************************************
CVertex3d *CFace3d::GetVertexFaceNeighborExclusive(unsigned int index)
{
	CFace3d *pFace = m_pFace[index%3];
	for(int i=0;i<3;i++)
		if(!this->HasVertex(pFace->v(i)))
			return pFace->v(i);
	return NULL;
}


//********************************************
// JointNeighbor
//********************************************
int CFace3d::JointNeighbor(CFace3d **pFace0,
													 CFace3d **pFace1)
{
	ASSERT(NbFaceNeighbor()==2);

	if(NbFaceNeighbor()!=2)
		return 0;

	// Find 2 neighbors
	CFace3d *pFaceNeighbor[2];
	int k=0;
	for(int i=0;i<3;i++)
	{
		if(m_pFace[i] != NULL)
			pFaceNeighbor[k++] = m_pFace[i];
	}
	ASSERT(k==2);
	if(k!=2)
		return 0;

	ASSERT(pFaceNeighbor[0]->HasNeighbor(this));
	ASSERT(pFaceNeighbor[1]->HasNeighbor(this));
	ASSERT(pFaceNeighbor[0] != this);
	ASSERT(pFaceNeighbor[1] != this);

	*pFace0 = pFaceNeighbor[0];
	*pFace1 = pFaceNeighbor[1];

	pFaceNeighbor[0]->UpdateNeighbor(this,pFaceNeighbor[1]);
	pFaceNeighbor[1]->UpdateNeighbor(this,pFaceNeighbor[0]);
	
	return 1;
}



//********************************************
// UpdateVertex
//********************************************
int CFace3d::UpdateVertex(CVertex3d *pOld,
													CVertex3d *pNew)
{
	int index;
	if(HasVertex(pOld,&index))
		{
		v(index,pNew);
		CalculateNormal(); // Update normal
		return 1;
		}
	return 0;
}

//********************************************
// UpdateNeighbor
//********************************************
int CFace3d::UpdateNeighbor(CFace3d *pOld,
													  CFace3d *pNew)
{
	int index;
	if(HasNeighbor(pOld,&index))
	{
		f(index,pNew);
		return 1;
	}
	return 0;
}

//********************************************
// Share2Vertex (exactly)
//********************************************
int CFace3d::Share2Vertex(CFace3d *pFace)
{
	if(pFace == NULL)
		return 0;
	int NbSharedVertex = 0;
	for(int i=0;i<3;i++)
		for(int j=0;j<3;j++)
			NbSharedVertex += (pFace->v(i) == m_pVertex[j]);
	return (NbSharedVertex == 2);
}

//********************************************
// Share2Vertex (exactly)
// Get sharing edge index info
//********************************************
int CFace3d::Share2Vertex(CFace3d *pFace,
													 int *IndexEdgeThis,
													 int *IndexEdgeOther)
{
	if(!Share2Vertex(pFace))
		return 0;

	int IndexThis[3] = {0,0,0};
	int IndexOther[3] = {0,0,0};

	for(int i=0;i<3;i++)
		for(int j=0;j<3;j++)
			if(pFace->v(i) == m_pVertex[j]) 
			{ 
				IndexThis[j] = 1;
				IndexOther[i] = 1; 
			}

	// Set IndexEdges
	*IndexEdgeThis = (IndexThis[0] && IndexThis[1]) ? 0 : (IndexThis[1] && IndexThis[2]) ? 1 : 2;
	*IndexEdgeOther = (IndexOther[0] && IndexOther[1]) ? 0 : (IndexOther[1] && IndexOther[2]) ? 1 : 2;

	// ** DEBUG **
	if(*IndexEdgeThis == 2)
		{
		ASSERT(IndexThis[0] && IndexThis[2]);
		}
	if(*IndexEdgeOther == 2)
		{
		ASSERT(IndexOther[0] && IndexOther[2]);
		}

	return 1;
}

//********************************************
// Share2Vertex (exactly)
// Get sharing edge index info
//********************************************
int CFace3d::Share2Vertex(CFace3d *pFace,
													 CVertex3d **pSharedV1,
													 CVertex3d **pSharedV2)
{
	int EdgeThis;
	int EdgeOther;
	if(!Share2Vertex(pFace))
		{
		*pSharedV1 = NULL;
		*pSharedV2 = NULL;
		return 0;
		}
	Share2Vertex(pFace,&EdgeThis,&EdgeOther);
	*pSharedV1 = v(EdgeThis);
	*pSharedV2 = v((EdgeThis+1)%3);
	return 1;
}

//********************************************
// Share1Vertex (exactly)
//********************************************
int CFace3d::Share1Vertex(CFace3d *pFace)
{
	int NbSharedVertex = 0;
	for(int i=0;i<3;i++)
		for(int j=0;j<3;j++)
			NbSharedVertex += (pFace->v(i) == m_pVertex[j]);
	return (NbSharedVertex == 1);
}


//********************************************
// UpdateVertexRecursive
//********************************************
int CFace3d::UpdateVertexRecursive(CVertex3d *pVertexOld,
																	 CVertex3d *pVertexNew)
{
	if(pVertexOld == pVertexNew)
		return 0;
	//TRACE("Update vertex %x in face %x\n",pVertexOld,this);
	this->UpdateVertex(pVertexOld,pVertexNew);
	for(int i=0;i<3;i++)
	{
		CFace3d *pFace = f(i);
		if(pFace != NULL)
			if(pFace->HasVertex(pVertexOld))
				pFace->UpdateVertexRecursive(pVertexOld,pVertexNew);
	}
	return 1;
}

//********************************************
// BuildArrayNeighbor
//********************************************
int CFace3d::BuildArrayNeighbor(CArray3d<CFace3d> *pArrayFace,
																int depth)
{
	// Remove all
	pArrayFace->SetSize(0);
	pArrayFace->Add(this);

	for(int i=0;i<3;i++)
	{
		CVertex3d *pVertex = v(i);
		int NbFaceNeighbor = pVertex->NbFaceNeighbor();
		for(int j=0;j<NbFaceNeighbor;j++)
		{
			CFace3d *pFace = pVertex->GetFaceNeighbor(j);
			if(pFace != this) // it's done
				if(!pArrayFace->Has(pFace))
					pArrayFace->Add(pFace);
		}
	}

	return 1;
}





//////////////////////////////////////////////
// DEBUG
//////////////////////////////////////////////

//********************************************
// Trace
//********************************************
void CFace3d::Trace()
{
	TRACE("\n");
	TRACE("Face %x\n",this);
	TRACE("Vertex   : %d\n",NbVertex());
	TRACE("Face (n) : %d\n",NbFaceNeighbor());
	TRACE("Normal   : %s\n",(m_Normal.GetNormL2Square()==0) ? "no" : "yes");
	TRACE("Vertices : (%g,%g,%g) (%g,%g,%g) (%g,%g,%g) \n",
		m_pVertex[0]->x(),m_pVertex[0]->y(),m_pVertex[0]->z(),
		m_pVertex[1]->x(),m_pVertex[1]->y(),m_pVertex[1]->z(),
		m_pVertex[2]->x(),m_pVertex[2]->y(),m_pVertex[2]->z());
	TRACE("Normal   : (%g,%g,%g)\n",m_Normal.x(),m_Normal.y(),m_Normal.z());

	// ** DEBUG **
	/*
	for(int i=0;i<3;i++)
		m_pVertex[i]->Trace();
		*/
}


//////////////////////////////////////////////
// OPENGL
//////////////////////////////////////////////

//********************************************
// glDraw
// Highlights face and its neighbors
//********************************************
void CFace3d::glDraw(unsigned char *ColorFace,
										 CMesh3d *pMesh /* = NULL */,
										 unsigned char *ColorNeightbor /* = NULL */)
{
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

	// Neighbor
	if(ColorNeightbor != NULL)
	{
		glColor3ub(ColorNeightbor[0],ColorNeightbor[1],ColorNeightbor[2]);
		for(int k=0;k<3;k++)
		{
			CFace3d *pFace = f(k);
			if(pFace != NULL)
				{
				::glBegin(GL_POLYGON);
					::glVertex3f(pFace->v1()->x(),pFace->v1()->y(),pFace->v1()->z());
					::glVertex3f(pFace->v2()->x(),pFace->v2()->y(),pFace->v2()->z());
					::glVertex3f(pFace->v3()->x(),pFace->v3()->y(),pFace->v3()->z());
				::glEnd();
				}
		}
	}

	// Main face
	glColor3ub(ColorFace[0],ColorFace[1],ColorFace[2]);

	::glBegin(GL_POLYGON);
		for(int i=0;i<3;i++)
			::glVertex3f(m_pVertex[i]->x(),m_pVertex[i]->y(),m_pVertex[i]->z());
	::glEnd();

	if(pMesh != NULL)
	{
		::glPopMatrix();
	}
}


//********************************************
// Area
//********************************************
double CFace3d::Area()
{
	return ::Area(m_pVertex[0],m_pVertex[1],m_pVertex[2]);
}

//********************************************
// Perimeter
//********************************************
double CFace3d::Perimeter()
{
	return (::Distance(m_pVertex[0],m_pVertex[1]) + 
		      ::Distance(m_pVertex[1],m_pVertex[2]) +
					::Distance(m_pVertex[2],m_pVertex[0]));
}

//********************************************
// Compacity
//********************************************
double CFace3d::Compacity()
{
	double perimeter = Perimeter();
	if(perimeter == 0.0f)
	{
		TRACE("CFace3d::Compacity : null triangle\n");
		return 0.0;
	}
	return (4.0*PI*Area()/(perimeter*perimeter));
}


// ** EOF **





