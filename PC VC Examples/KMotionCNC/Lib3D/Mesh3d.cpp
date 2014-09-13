//********************************************
// Mesh3d.cpp
//********************************************
// class CMesh3d
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 15/01/98
// Modified : 15/01/98
//********************************************

#include "..\stdafx.h"

//////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////

//********************************************
// Constructor
//********************************************
CMesh3d::CMesh3d()
{
	m_ListDone = 0;
	m_Modified = 1;
	m_NormalBinding = NORMAL_PER_VERTEX;
	m_ColorBinding = COLOR_PER_VERTEX;
	m_Name = _T("Mesh");
	// Texture
	m_IndexTexture = -1;
	m_pTextureCoordinate = NULL;
	m_pTextureCoordinateIndex = NULL;
	m_ListOpenGL=-1;
	m_Show = 1;
}

//********************************************
// Destructor
//********************************************
CMesh3d::~CMesh3d()
{
	Free();
}

//********************************************
// Free
//********************************************
void CMesh3d::Free()
{
	//TRACE("Cleanup mesh %x\n",this);
	m_ArrayVertex.Free();
	m_ArrayFace.Free();
	if(m_pTextureCoordinate != NULL)
		delete [] m_pTextureCoordinate;
	if(m_pTextureCoordinateIndex != NULL)
		delete [] m_pTextureCoordinateIndex;
}


//////////////////////////////////////////////
// OPENGL
//////////////////////////////////////////////

//********************************************
// BuildList
//********************************************
int CMesh3d::glBuildList()
{
	//TRACE(" Start building list ...\n");

	// Check for valid mesh
	if(m_ArrayVertex.GetSize() == 0 ||
		 m_ArrayFace.GetSize() == 0)
	{
		TRACE("CMesh3d::BuildList : empty mesh\n");
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
		TRACE("CMesh3d::BuildList : unable to build DrawList\n");
		return 0;
	}

	// Start list
	unsigned int NbVertex = (unsigned int)m_ArrayVertex.GetSize();
	unsigned int NbFace = (unsigned int)m_ArrayFace.GetSize();

	if(!NbVertex)
		return 0;
	if(!NbFace)
		return 0;

	CFace3d *pFace;
	CVector3d *pVector;
	CColor *pColorPrevious;

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

	// Init color
	pFace = m_ArrayFace[0];
	pColorPrevious = pFace->GetColor();
	::glColor3ub(pFace->GetColor()->r(),pFace->GetColor()->g(),pFace->GetColor()->b());

	// Triangles
	::glBegin(GL_TRIANGLES);
	for(unsigned int i=0;i<NbFace;i++)
	{
		{
			pFace = m_ArrayFace[i];
			ASSERT(pFace != NULL);

			// Normal (per face)
			if(m_NormalBinding == NORMAL_PER_FACE)
			{
				pVector = pFace->GetNormal();
				::glNormal3f(pVector->x(),pVector->y(),pVector->z());
			}

/*

			// Color (per face)
			if(m_ColorBinding == COLOR_PER_FACE && 
				 pColorPrevious != pFace->GetColor())
			{
				::glColor3ub(pFace->GetColor()->r(),pFace->GetColor()->g(),pFace->GetColor()->b());
				pColorPrevious = pFace->GetColor();
			}
*/

			for(int j=0;j<3;j++)
			{
				// Normal
				if(m_NormalBinding == NORMAL_PER_VERTEX)
				{
					pVector = pFace->v(j)->GetNormal();
					::glNormal3f(pVector->x(),pVector->y(),pVector->z());
				}
/*
				// Color (per vertex)
				if(m_ColorBinding == COLOR_PER_VERTEX && 
					 pColorPrevious != pFace->v(j)->GetColor())
				{
					::glColor3ub(pFace->v(j)->GetColor()->r(),pFace->v(j)->GetColor()->g(),pFace->v(j)->GetColor()->b());
					pColorPrevious = pFace->v(j)->GetColor();
				}

				// Texture coordinate (if needed)
				if(m_IndexTexture != -1)
				{
					glTexCoord2f(m_pTextureCoordinate[2*m_pTextureCoordinateIndex[3*i+j]],
						           m_pTextureCoordinate[2*m_pTextureCoordinateIndex[3*i+j]+1]);	
				}
*/
				// Vertex
				::glColor3ub(pFace->GetColor()->r(),pFace->GetColor()->g(),pFace->GetColor()->b());

				::glVertex3f(pFace->v(j)->x(),pFace->v(j)->y(),pFace->v(j)->z());
			}
		}
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
int CMesh3d::glDraw()
{
	if(!m_Show)
		return 0;

	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	
	// Build list at first
	if(!m_ListDone || m_Modified)
		glBuildList();

	::glEnable(GL_LIGHTING);

	// Search for a new list
	if(::glIsList(m_ListOpenGL)==GL_TRUE)
	{
	  ::glCallList(m_ListOpenGL);
		return 1;
	}
	else
	{
		return 0;
	}
}





//////////////////////////////////////////////
// DATAS
//////////////////////////////////////////////

//********************************************
// Copy
//********************************************
void CMesh3d::Copy(CMesh3d *pMesh)
{
	// Vertices
	int i,NbVertex = pMesh->NbVertex();
	m_ArrayVertex.SetSize(NbVertex);
	for(i=0;i<NbVertex;i++)
		m_ArrayVertex.SetAt(i,new CVertex3d(pMesh->GetVertex(i)));

	// Faces
	int NbFace = pMesh->NbFace();
	m_ArrayFace.SetSize(NbFace);
	for(i=0;i<NbFace;i++)
	{
		CFace3d *pFace = pMesh->GetFace(i);
		m_ArrayFace.SetAt(i,new CFace3d(
			m_ArrayVertex[pMesh->IndexFrom(pFace->v1())],
			m_ArrayVertex[pMesh->IndexFrom(pFace->v2())],
			m_ArrayVertex[pMesh->IndexFrom(pFace->v3())]));
	}

	// Transform
	m_Transform.Copy(pMesh->GetTransform());
}


//********************************************
// GetType
//********************************************
int CMesh3d::GetType()
{
	return TYPE_MESH3D;
}

//********************************************
// IsValid
//********************************************
int CMesh3d::IsValid()
{
	int NbFace = m_ArrayFace.GetSize();
	for(int i=0;i<NbFace;i++)
		if(!m_ArrayFace[i]->IsValid())
			return 0;
	return 1;
}


//********************************************
// DeleteVertex
//********************************************
int CMesh3d::DeleteVertex(CVertex3d *pVertex)
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
int CMesh3d::DeleteVertex(int index)
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

//********************************************
// DeleteFace
//********************************************
int CMesh3d::DeleteFaceNbNeighbors(int NbNeighbor)
{
	TRACE("Delete faces which has %d neighbors...",NbNeighbor);
	int deleted = 0;
	for(int i=0;i<m_ArrayFace.GetSize();i++)
	{
		CFace3d *pFace = m_ArrayFace[i];
		if(pFace->NbFaceNeighbor() == NbNeighbor)
		{
			m_ArrayFace.RemoveAt(i);
			delete pFace;
			i--;
			deleted++;
		}
	}
	TRACE("%d face(s) deleted\n",deleted);
	return deleted;
}


//////////////////////////////////////////////
// RANGE
//////////////////////////////////////////////

//********************************************
// Range
//********************************************
void CMesh3d::Range(int coord, 
										float *min,
										float *max)
{
	ASSERT(coord >= 0 && coord <= 2);
	int NbVertex = m_ArrayVertex.GetSize();
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
void CMesh3d::Range(int coord, 
										float min,
										float max)
{
	TRACE("Range mesh...");
	float Min,Max;
	Range(coord,&Min,&Max);
	TRACE("old : (%g,%g) -> (%g %g)",Min,Max,min,max);
	Offset(coord,-Min);
	Scale(coord,(max-min)/(Max-Min));
	Offset(coord,min);
	TRACE("ok\n");
}

//********************************************
// Scale
//********************************************
void CMesh3d::Scale(int coord,
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
void CMesh3d::Offset(int coord,
										 float offset)
{
	int NbVertex = m_ArrayVertex.GetSize();
	for(int i=0;i<NbVertex;i++)
		m_ArrayVertex[i]->Set(coord,m_ArrayVertex[i]->Get(coord) + offset);
	m_Modified = 1;
}


//////////////////////////////////////////////
// PROCESSING
//////////////////////////////////////////////

//********************************************
// BuildAdjacency
// From VertexArray and FaceArray, build
// neighboring vertices and faces, using
// edge and vertex sharing
//********************************************
int CMesh3d::BuildAdjacency()
{

	CMesh3d *pMesh = this;
	
	// Check for valid sizes
	int i,NbVertex = m_ArrayVertex.GetSize();
	int NbFace = m_ArrayFace.GetSize();
	TRACE("Build adjacency (%d faces, %d vertices)",NbFace,NbVertex);
	if(NbVertex < 3 || NbFace <= 0)
	{
		TRACE("  invalid array sizes\n");
		return 0;
	}

	// At first, clear all neighbors for each face
	//TRACE("  clear face neighbors\n");
	TRACE(".");
	for(i=0;i<NbFace;i++)
		for(int k=0;k<6;k++)
			m_ArrayFace[i]->f(k,NULL);

	// At first, clear all neighbors for each vertex
	//TRACE("  clear vertex neighbors\n");
	TRACE(".");
	for(i=0;i<NbVertex;i++)
	{
		m_ArrayVertex[i]->RemoveAllFaceNeighbor();
		m_ArrayVertex[i]->RemoveAllVertexNeighbor();
	}

	//*********************************************
	// For each face, set face neighboring vertices 
	//*********************************************
	//TRACE("  set face neighboring vertices\n");
	TRACE(".");
	for(i=0;i<NbFace;i++)
	{
		CFace3d *pFaceCurrent = m_ArrayFace[i];
		for(int j=0;j<3;j++)
			pFaceCurrent->v(j)->AddNeighbor(pFaceCurrent);
	}

	//*********************************************
	// For each vertex, set vertex neighboring,
  // just look on neighboring faces 
	//*********************************************
	//TRACE("  set vertices neighboring vertices\n");
	TRACE(".");
	for(i=0;i<NbVertex;i++)
	{
		CVertex3d *pVertexCurrent = m_ArrayVertex[i];
		int NbFaceNeighbor = pVertexCurrent->NbFaceNeighbor();
		for(int j=0;j<NbFaceNeighbor;j++)
		{
			CFace3d *pFace = pVertexCurrent->GetFaceNeighbor(j);
			for(int k=0;k<3;k++)
				pVertexCurrent->AddNeighbor(pFace->v(k));
		}
	}

	//*********************************************
	// For each face, set face neighboring,
  // just look on faces neighboring vertices
	//*********************************************
	//TRACE("  set faces neighboring faces\n");
	TRACE(".");
	for(i=0;i<NbFace;i++)
	{
		CFace3d *pFaceCurrent = m_ArrayFace[i];
		// For each edge
		for(int j=0;j<3;j++)
		{
			CVertex3d *pVertex = pFaceCurrent->v(j);
			CVertex3d *pNextVertex = pFaceCurrent->v((j+1)%3);
			int NbFaceNeighbor = pVertex->NbFaceNeighbor();
			for(int k=0;k<NbFaceNeighbor;k++)
			{
				// This face contain pVertex
				CFace3d *pFace = pVertex->GetFaceNeighbor(k);
				if(pFace != pFaceCurrent)
				  if(pFaceCurrent->f(j) == NULL)
						if(pFace->HasVertex(pVertex))
							if(pFace->HasVertex(pNextVertex))
								pFaceCurrent->f(j,pFace);
			}
		}
	}

	/*
	// Check
	for(i=0;i<NbFace;i++)
	{
		ASSERT(m_ArrayFace[i]->IsValid());
	}
	*/


	TRACE("ok\n");

	return TRUE;
}

//********************************************
// Rebuild
//********************************************
void CMesh3d::Rebuild()
{
	BuildAdjacency();
	CalculateNormalPerFace();
	CalculateNormalPerVertex();
	SetModified();
}

/////////////////////////////////////////////
// DEBUG
//////////////////////////////////////////////

//********************************************
// Trace
//********************************************
void CMesh3d::Trace()
{
	int NbVertex = m_ArrayVertex.GetSize();
	int NbFace = m_ArrayFace.GetSize();

	TRACE("\n");
	TRACE(" Mesh \n");
	TRACE("Vertex : %d\n",NbVertex);
	TRACE("Face   : %d\n",NbFace);

	for(int i=0;i<NbVertex;i++)
		((CVertex3d *)m_ArrayVertex[i])->Trace();

	for(int j=0;j<NbFace;j++)
		((CFace3d *)m_ArrayFace[j])->Trace();
}





//********************************************
// IndexFrom
// Return -1 if failed
//********************************************
int CMesh3d::IndexFrom(CFace3d *pFace)
{
	ASSERT(pFace != NULL);
	int NbFace = m_ArrayFace.GetSize();
	for(int i=0;i<NbFace;i++)
		if(m_ArrayFace[i] == pFace)
			return i;
	return -1;
}

//********************************************
// IndexFrom
// Return -1 if failed
//********************************************
int CMesh3d::IndexFrom(CVertex3d *pVertex)
{
	int NbVertex = m_ArrayVertex.GetSize();
	for(int i=0;i<NbVertex;i++)
		if(m_ArrayVertex[i] == pVertex)
			return i;
	return -1;
}

//********************************************
// Move
//********************************************
void CMesh3d::Move(float dx,float dy,float dz)
{
	int NbVertex = m_ArrayVertex.GetSize();
	for(int i=0;i<NbVertex;i++)
		m_ArrayVertex[i]->Move(dx,dy,dz);
	m_Modified = 1;
}


//********************************************
// FindVertexInFaces
//********************************************
int CMesh3d::FindVertex(CVertex3d *pVertex)
{
	int j,find = 0;

	int NbFace = m_ArrayFace.GetSize();
	for(j=0;j<NbFace;j++)
		if(m_ArrayFace[j]->HasVertex(pVertex))
		{
			find = 1;
			TRACE("Find vertex [%x] in face %d\n",pVertex,j);
		}

	int NbVertex = m_ArrayVertex.GetSize();
	for(j=0;j<NbVertex;j++)
		if(m_ArrayVertex[j] == pVertex)
		{
			find = 1;
			TRACE("Find vertex [%x] at position %d\n",pVertex,j);
		}

	for(j=0;j<NbVertex;j++)
	{
		CVertex3d *pV = m_ArrayVertex[j];
		if(pV->HasNeighbor(pVertex))
		{
			find = 1;
			TRACE("Find vertex [%x] in neighbors of vertex %d\n",pVertex,j);
		}
	}

	return find;
}

//********************************************
// FindFace
//********************************************
int CMesh3d::FindFace(CFace3d *pFace)
{
	int find = 0;

	int NbFace = m_ArrayFace.GetSize();
	for(int j=0;j<NbFace;j++)
	{
		CFace3d *pF = m_ArrayFace[j];
		if(pF == pFace)
		{
			find = 1;
			TRACE("Find face [%x] in mesh (index : %d)\n",pFace,j);
		}

		for(int i=0;i<3;i++)
		if(pF->f(i) == pFace)
		{
			find = 1;
			TRACE("Find face [%x] in neighbors %d of face %d\n",pFace,i,j);
		}
	}

	return find;
}



//////////////////////////////////////////////
// NORMALS
//////////////////////////////////////////////


//********************************************
// CalculateNormalPerVertex
//********************************************
int CMesh3d::CalculateNormalPerVertex(void)
{

	int NbVertex = m_ArrayVertex.GetSize();
	int NbFace = m_ArrayFace.GetSize();
	TRACE("Calculate normal per vertex (%d faces, %d vertices)...",NbFace,NbVertex);
	for(int i=0;i<NbVertex;i++)
	{
		CVertex3d *pVertex = m_ArrayVertex[i];
		int NbNormal = 0;
		CVector3d vector;
		int NbFaceNeighbor = pVertex->NbFaceNeighbor();
		for(int j=0;j<NbFaceNeighbor;j++)
		{
			CFace3d *pFace = pVertex->GetFaceNeighbor(j);
			NbNormal += 1;
			vector += pFace->GetNormal();
		}
		if(NbNormal>=1)
		{
			vector.NormalizeL2();
			pVertex->SetNormal(vector);
		}
	}
	m_Modified = 1;
	TRACE("ok\n");

	return 1;
}

//********************************************
// CalculateNormalPerFace
//********************************************
int CMesh3d::CalculateNormalPerFace(void)
{
	int size = m_ArrayFace.GetSize();
	TRACE("Calculate normal per face (%d faces)...",size);
	for(int i=0;i<size;i++)
		{
			CFace3d *pFace = m_ArrayFace[i];
			if(pFace->IsValid())
				pFace->CalculateNormal();
		}
	m_Modified = 1;
	TRACE("ok\n");
	return 1;
}




//********************************************
// ColorSharpEdges
//********************************************
int CMesh3d::ColorSharpEdge(float threshold,
														CColor &color)
{
	int NbFace = m_ArrayFace.GetSize();

	TRACE(" Start ColorSharpEdges\n");
	TRACE("   Faces : %d\n",NbFace);
	for(int i=0;i<NbFace;i++)
		m_ArrayFace[i]->ColorSharpEdge(threshold,color);
	TRACE(" End ColorSharpEdges\n");

	return 1;
}


//********************************************
// SetNormalBinding
//********************************************
void CMesh3d::SetNormalBinding(int type)
{
	m_NormalBinding = type;
	m_Modified = 1;
}

//********************************************
// GetNormalBinding
//********************************************
int CMesh3d::GetNormalBinding(void)
{
	return m_NormalBinding;
}

//********************************************
// SetColorBinding
//********************************************
void CMesh3d::SetColorBinding(int type)
{
	m_ColorBinding = type;
	m_Modified = 1;
}

//********************************************
// SetColorBinding
//********************************************
void CMesh3d::SetColor(CColor &color)
{
	int size = m_ArrayFace.GetSize();
	for(int i=0;i<size;i++)
		m_ArrayFace[i]->SetColor(color);
	m_Modified = 1;
}

//********************************************
// GetColorBinding
//********************************************
int CMesh3d::GetColorBinding(void)
{
	return m_ColorBinding;
}

//********************************************
// SetFlagOnFaces
//********************************************
void CMesh3d::SetFlagOnFaces(int flag)
{
	int size = m_ArrayFace.GetSize();
	for(int i=0;i<size;i++)
		m_ArrayFace[i]->SetFlag(flag);
}

//********************************************
// GetMaxFlagOnFaces
//********************************************
int CMesh3d::GetMaxFlagOnFaces()
{
	int size = m_ArrayFace.GetSize();
	int max = 0;
	for(int i=0;i<size;i++)
	{
		int tmp = m_ArrayFace[i]->GetFlag();
		max = (tmp > max) ? tmp : max;
	}
	return max;
}

//********************************************
// SetFlagOnVertices
//********************************************
void CMesh3d::SetFlagOnVertices(int flag)
{
	int size = m_ArrayVertex.GetSize();
	for(int i=0;i<size;i++)
		m_ArrayVertex[i]->SetFlag(flag);
}


//********************************************
// GetFirstVertexWithFlag
//********************************************
float CMesh3d::GetMeanLengthEdge()
{
	int size = m_ArrayVertex.GetSize();
	double sum = 0;
	for(int i=0;i<size;i++)
		sum += m_ArrayVertex[i]->GetMeanLengthEdgeAround();
	if(size)
		return (float)sum/size;
	else
		return 0.0f;
}





//////////////////////////////////////////////
// SUBDIVISION
//////////////////////////////////////////////

//********************************************
// Alpha
// From Piecewise smooth reconstruction (Hoppe)
//********************************************
float CMesh3d::Alpha(int n)
{
	float tmp = 3.0f + 2.0f * (float)cos(2.0f * 3.14159265359f/(float)n);
	float a = 5.0f/8.0f - (tmp*tmp)/64.0f;
	return (n*(1-a)/a);
}


//********************************************
// SubdivisionLoop
// From Piecewise smooth reconstruction (Hoppe)
// Charles Loop (SIGGRAPH 94)
//********************************************
int CMesh3d::SubdivisionLoop(int MoveOnBundary /* = 1 */)
{
	// We assume adjacency is built
	int i,NbVertex = m_ArrayVertex.GetSize();

	// Create subdivision info (edge vertices)
	int NbFace = m_ArrayFace.GetSize();
	TRACE("Start loop's subdivision (%d faces, %d vertices)\n",NbFace,NbVertex);

	TRACE("  subdivision info (%d vertices)\n",m_ArrayVertex.GetSize());

	// For each face
	for(i=0;i<NbFace;i++)
	{
		CFace3d *pFace = m_ArrayFace[i];

		// Check face has at least 1 neighbor
		if(pFace->NbFaceNeighbor() == 0)
			continue;

		// Check valid neighboring
		if(!pFace->IsValid())
			continue;

		// On each edge
		for(int IndexEdge=0;IndexEdge<3;IndexEdge++)
		{
			// Get IndexEdge on neighbor
			int IndexCurrent,IndexNeighbor;
			CFace3d *pFaceNeighbor = pFace->f(IndexEdge);

			// No neighbor on this edge, go to next
			if(pFaceNeighbor == NULL)
				continue;

			VERIFY(pFace->Share2Vertex(pFaceNeighbor,&IndexCurrent,&IndexNeighbor));
			ASSERT(IndexCurrent == IndexEdge);

			// If neighboring face has been treated, then get vertex
			// and go to next step
			if(pFaceNeighbor->GetFlag())
			{
				CVertex3d *pVertex = pFaceNeighbor->v(3+(IndexNeighbor%3));
				pFace->v(3+(IndexEdge%3),pVertex);
				continue;
			}

			// Vertex weighting
			// 0 & 1 : weight : 3, 2 & 3 : weight : 1
			CVertex3d *pVertex[4];

			// Weight : 3
			pVertex[0] = pFace->v(IndexEdge);
			pVertex[1] = pFace->v((IndexEdge+1)%3);
			// Weight : 1
			pVertex[2] = pFace->v((IndexEdge+2)%3);
			pVertex[3] = pFaceNeighbor->v((IndexNeighbor+2)%3);

			ASSERT(pVertex[0] != NULL &&  pVertex[1] != NULL &&  pVertex[2] != NULL &&  pVertex[3] != NULL);

			// For each composant
			float coord[3];
			for(int k=0;k<3;k++)
				coord[k] = (3.0f * (pVertex[0]->Get(k) + pVertex[1]->Get(k)) + 
					                  pVertex[2]->Get(k) + pVertex[3]->Get(k)) / 8.0f;

			// Add vertex to global mesh array, and set face's vertex
			CVertex3d *pNewVertex = new CVertex3d(coord[0],coord[1],coord[2]);
			m_ArrayVertex.Add(pNewVertex);
			
			pFace->v(3+IndexCurrent,pNewVertex);
		}

		// Set flag
		pFace->SetFlag(1); 
	}

	//*****************************
	// Create faces
	//*****************************
	TRACE("  creating faces (%d faces)\n",NbFace);
	// For each valid face
	for(i=0;i<NbFace;i++)
	{
		CFace3d *pFace = m_ArrayFace[i];
		pFace->SetFlag(0);

		// Valid face
		int NbVertex = pFace->NbVertex();
		CFace3d *pNewFace;

		switch(NbVertex)
		{
		case 4:
			// Create one face

			// On edge 0
			if(pFace->v(3) != NULL)
			{
				pNewFace = new CFace3d(pFace->v(3),pFace->v(1),pFace->v(2));
				m_ArrayFace.Add(pNewFace);

				// Move current face
				pFace->v(1,pFace->v(3));
			}
			else
				// On edge 1
				if(pFace->v(4) != NULL)
				{
					pNewFace = new CFace3d(pFace->v(0),pFace->v(4),pFace->v(2));
					m_ArrayFace.Add(pNewFace);

					// Move current face
					pFace->v(2,pFace->v(4));
				}
				else
					// On edge 2
					if(pFace->v(5) != NULL)
					{
						pNewFace = new CFace3d(pFace->v(5),pFace->v(1),pFace->v(2));
						m_ArrayFace.Add(pNewFace);

						// Move current face
						pFace->v(2,pFace->v(5));
					}
			break;
		case 5:
			// Create two faces

			// On edge 0 & 2
			if(pFace->v(3) != NULL && 
				 pFace->v(5) != NULL)
			{
				pNewFace = new CFace3d(pFace->v(0),pFace->v(3),pFace->v(5));
				m_ArrayFace.Add(pNewFace);
				pNewFace = new CFace3d(pFace->v(5),pFace->v(3),pFace->v(2));
				m_ArrayFace.Add(pNewFace);

				// Move current face
				pFace->v(0,pFace->v(3));
			}
			else
				// On edge 0 & 1
				if(pFace->v(3) != NULL && 
					 pFace->v(4) != NULL)
				{
					pNewFace = new CFace3d(pFace->v(3),pFace->v(1),pFace->v(4));
					m_ArrayFace.Add(pNewFace);
					pNewFace = new CFace3d(pFace->v(0),pFace->v(3),pFace->v(4));
					m_ArrayFace.Add(pNewFace);

					// Move current face
					pFace->v(1,pFace->v(4));
				}
				else
					// On edge 1 & 2
					if(pFace->v(4) != NULL && 
						 pFace->v(5) != NULL)
					{
						pNewFace = new CFace3d(pFace->v(1),pFace->v(4),pFace->v(5));
						m_ArrayFace.Add(pNewFace);
						pNewFace = new CFace3d(pFace->v(4),pFace->v(2),pFace->v(5));
						m_ArrayFace.Add(pNewFace);

						// Move current face
						pFace->v(2,pFace->v(5));
					}
			break;
		case 6:
			// Create three faces

			// First (v3,v1,v4)
			pNewFace = new CFace3d(pFace->v(3),pFace->v(1),pFace->v(4));
			m_ArrayFace.Add(pNewFace);
			// Second (v3,v4,v5)
			pNewFace = new CFace3d(pFace->v(3),pFace->v(4),pFace->v(5));
			m_ArrayFace.Add(pNewFace);
			// Third (v5,v4,v2)
			pNewFace = new CFace3d(pFace->v(5),pFace->v(4),pFace->v(2));
			m_ArrayFace.Add(pNewFace);
			
			// Move current face
			pFace->v(1,pFace->v(3));
			pFace->v(2,pFace->v(5));

			break;
		}
		// Remove subdivision info
		for(int k=3;k<6;k++)
			pFace->v(k,NULL);

	}
	TRACE("  end creating faces (%d faces)\n",m_ArrayFace.GetSize());


	//*****************************
	// Move original vertices
	//*****************************

	// Copy
	TRACE("  copy\n");
	CArray3d<CVertex3d> ArrayVertex;
	ArrayVertex.SetSize(NbVertex);
	for(i=0;i<NbVertex;i++)
		ArrayVertex.SetAt(i,new CVertex3d);

	// For each vertex (at least 3 neighbors)
	for(i=0;i<NbVertex;i++)
	{

		CVertex3d *pVertex = m_ArrayVertex[i];

		// Do we move boundaries ??
		if(!MoveOnBundary)
			if(pVertex->IsOnBoundary())
			{
				ArrayVertex[i]->Set(pVertex);	
				continue;
			}

		int n = pVertex->NbVertexNeighbor();
		float alpha = Alpha(n);
		float tmp = alpha + (float)n;

		// For each composant
		for(unsigned int j=0;j<3;j++)
		{
			float value = alpha * pVertex->Get(j);
			for(int k=0;k<n;k++)
				value += pVertex->GetVertexNeighbor(k)->Get(j);
			value /= tmp;
			ArrayVertex[i]->Set(j,value);
		}
	}

	// Restore
	TRACE("  restore\n");
	for(i=0;i<NbVertex;i++)
		for(unsigned int j=0;j<3;j++)
			m_ArrayVertex[i]->Set(j,ArrayVertex[i]->Get(j));

	ArrayVertex.Free();

	// Rebuild adjacency and normals
	BuildAdjacency();
	CalculateNormalPerFace();
	CalculateNormalPerVertex();
	m_Modified = 1;

	TRACE("End loop's subdivision (%d faces, %d vertices)\n",
		m_ArrayFace.GetSize(),m_ArrayVertex.GetSize());

	return 1;
}


//********************************************
// Subdivision
// Simple : 1->4
//********************************************
int CMesh3d::Subdivision(void)
{
	// We assume adjacency is built
	int i,NbVertex = m_ArrayVertex.GetSize();

	// Create subdivision info (edge vertices)
	int NbFace = m_ArrayFace.GetSize();
	TRACE("Start subdivision (%d faces, %d vertices)\n",NbFace,NbVertex);

	TRACE("  subdivision info (%d vertices)\n",m_ArrayVertex.GetSize());

	// For each face
	for(i=0;i<NbFace;i++)
	{
		CFace3d *pFace = m_ArrayFace[i];

		// On each edge
		for(int IndexEdge=0;IndexEdge<3;IndexEdge++)
		{
			// Get IndexEdge on neighbor
			int IndexCurrent,IndexNeighbor;
			CFace3d *pFaceNeighbor = pFace->f(IndexEdge);

			// No neighbor on this edge, go to next
			if(pFaceNeighbor != NULL)
			{

				VERIFY(pFace->Share2Vertex(pFaceNeighbor,&IndexCurrent,&IndexNeighbor));
				ASSERT(IndexCurrent == IndexEdge);

				// If neighboring face has been treated, then get vertex
				// and go to next step
				if(pFaceNeighbor->GetFlag())
				{
					CVertex3d *pVertex = pFaceNeighbor->v(3+(IndexNeighbor%3));
					pFace->v(3+(IndexEdge%3),pVertex);
					continue;
				}
			}

			// Vertex weighting
			// 0 & 1 : weight : 1
			CVertex3d *pVertex[2];

			// Weight : 1
			pVertex[0] = pFace->v(IndexEdge);
			pVertex[1] = pFace->v((IndexEdge+1)%3);

			ASSERT(pVertex[0] != NULL &&  pVertex[1] != NULL);

			// For each composant
			float coord[3];
			for(int k=0;k<3;k++)
				coord[k] = (pVertex[0]->Get(k) + pVertex[1]->Get(k)) / 2.0f;

			// Add vertex to global mesh array, and set face's vertex
			CVertex3d *pNewVertex = new CVertex3d(coord[0],coord[1],coord[2]);
			m_ArrayVertex.Add(pNewVertex);
			
			pFace->v(3+IndexEdge,pNewVertex);
		}

		// Set flag
		pFace->SetFlag(1); 
	}

	//*****************************
	// Create faces
	//*****************************
	TRACE("  creating faces (%d faces)\n",NbFace);
	// For each valid face
	for(i=0;i<NbFace;i++)
	{
		CFace3d *pFace = m_ArrayFace[i];
		pFace->SetFlag(0);

		// Valid face
		int NbVertex = pFace->NbVertex();
		CFace3d *pNewFace;

		switch(NbVertex)
		{
		case 4:
			// Create one face

			// On edge 0
			if(pFace->v(3) != NULL)
			{
				pNewFace = new CFace3d(pFace->v(3),pFace->v(1),pFace->v(2));
				m_ArrayFace.Add(pNewFace);

				// Move current face
				pFace->v(1,pFace->v(3));
			}
			else
				// On edge 1
				if(pFace->v(4) != NULL)
				{
					pNewFace = new CFace3d(pFace->v(0),pFace->v(4),pFace->v(2));
					m_ArrayFace.Add(pNewFace);

					// Move current face
					pFace->v(2,pFace->v(4));
				}
				else
					// On edge 2
					if(pFace->v(5) != NULL)
					{
						pNewFace = new CFace3d(pFace->v(5),pFace->v(1),pFace->v(2));
						m_ArrayFace.Add(pNewFace);

						// Move current face
						pFace->v(2,pFace->v(5));
					}
			break;
		case 5:
			// Create two faces

			// On edge 0 & 2
			if(pFace->v(3) != NULL && 
				 pFace->v(5) != NULL)
			{
				pNewFace = new CFace3d(pFace->v(0),pFace->v(3),pFace->v(5));
				m_ArrayFace.Add(pNewFace);
				pNewFace = new CFace3d(pFace->v(5),pFace->v(3),pFace->v(2));
				m_ArrayFace.Add(pNewFace);

				// Move current face
				pFace->v(0,pFace->v(3));
			}
			else
				// On edge 0 & 1
				if(pFace->v(3) != NULL && 
					 pFace->v(4) != NULL)
				{
					pNewFace = new CFace3d(pFace->v(3),pFace->v(1),pFace->v(4));
					m_ArrayFace.Add(pNewFace);
					pNewFace = new CFace3d(pFace->v(0),pFace->v(3),pFace->v(4));
					m_ArrayFace.Add(pNewFace);

					// Move current face
					pFace->v(1,pFace->v(4));
				}
				else
					// On edge 1 & 2
					if(pFace->v(4) != NULL && 
						 pFace->v(5) != NULL)
					{
						pNewFace = new CFace3d(pFace->v(1),pFace->v(4),pFace->v(5));
						m_ArrayFace.Add(pNewFace);
						pNewFace = new CFace3d(pFace->v(4),pFace->v(2),pFace->v(5));
						m_ArrayFace.Add(pNewFace);

						// Move current face
						pFace->v(2,pFace->v(5));
					}
			break;
		case 6:
			// Create three faces

			// First (v3,v1,v4)
			pNewFace = new CFace3d(pFace->v(3),pFace->v(1),pFace->v(4));
			m_ArrayFace.Add(pNewFace);
			// Second (v3,v4,v5)
			pNewFace = new CFace3d(pFace->v(3),pFace->v(4),pFace->v(5));
			m_ArrayFace.Add(pNewFace);
			// Third (v5,v4,v2)
			pNewFace = new CFace3d(pFace->v(5),pFace->v(4),pFace->v(2));
			m_ArrayFace.Add(pNewFace);
			
			// Move current face
			pFace->v(1,pFace->v(3));
			pFace->v(2,pFace->v(5));

			break;
		}
		// Remove subdivision info
		for(int k=3;k<6;k++)
			pFace->v(k,NULL);

	}
	TRACE("  end creating faces (%d faces)\n",m_ArrayFace.GetSize());

	// Rebuild adjacency and normals
	BuildAdjacency();
	CalculateNormalPerFace();
	CalculateNormalPerVertex();
	m_Modified = 1;

	TRACE("End subdivision (%d faces, %d vertices)\n",
		m_ArrayFace.GetSize(),m_ArrayVertex.GetSize());

	return 1;
}

//********************************************
// Smooth
// 30/09/98
//********************************************
int CMesh3d::Smooth(int MoveOnBundary /* = 1 */)
{
	// We assume adjacency is built
	int i,NbVertex = m_ArrayVertex.GetSize();

	// Create subdivision info (edge vertices)
	int NbFace = m_ArrayFace.GetSize();
	TRACE("Start smoothing (%d faces, %d vertices)",NbFace,NbVertex);

	//*****************************
	// Move original vertices
	//*****************************

	// Copy
	TRACE(".");
	CArray3d<CVertex3d> ArrayVertex;
	ArrayVertex.SetSize(NbVertex);
	for(i=0;i<NbVertex;i++)
		ArrayVertex.SetAt(i,new CVertex3d);

	// For each vertex (at least 3 neighbors)
	for(i=0;i<NbVertex;i++)
	{
		CVertex3d *pVertex = m_ArrayVertex[i];

		if(!MoveOnBundary)
			if(pVertex->IsOnBoundary())
			{
				ArrayVertex[i]->Set(pVertex);	
				continue;
			}

		int n = pVertex->NbVertexNeighbor();
		float alpha = Alpha(n);
		float tmp = alpha + (float)n;
		// For each composant
		for(unsigned int j=0;j<3;j++)
		{
			float value = alpha * pVertex->Get(j);
			for(int k=0;k<n;k++)
				value += pVertex->GetVertexNeighbor(k)->Get(j);
			value /= tmp;
			ArrayVertex[i]->Set(j,value);
		}
	}

	// Restore
	TRACE(".");
	for(i=0;i<NbVertex;i++)
		for(unsigned int j=0;j<3;j++)
			m_ArrayVertex[i]->Set(j,ArrayVertex[i]->Get(j));

	// Cleanup
	TRACE(".");
	ArrayVertex.Free();

	TRACE("ok\n");

	// Rebuild adjacency and normals
	BuildAdjacency();
	CalculateNormalPerFace();
	CalculateNormalPerVertex();
	m_Modified = 1;

	return 1;
}






//********************************************
// ColorCurvature
// Each face is colored, function of mean curvature
//********************************************
void CMesh3d::ColorCurvature(CColorRamp *pRamp)
{
	TRACE("Start coloring mesh (curvature)\n");
	int i,NbVertex = m_ArrayVertex.GetSize();
	TRACE("  %d vertices\n",NbVertex);

	double *pMax = new double[NbVertex];

	// Store curvatures
	for(i=0;i<NbVertex;i++)
		pMax[i] = m_ArrayVertex[i]->GetMaxAngleAround();

	// Process extremas
	double min = MAX_DOUBLE;
	double max = 0.0f;
	for(i=0;i<NbVertex;i++)
	{
		min = (pMax[i] < min) ? pMax[i] : min;
		max = (pMax[i] > max) ? pMax[i] : max;
	}

	min = (min < 0.0f) ? 0.0f : min;
	double amplitude = max-min;//max-min;
	TRACE("  min : %g\n",min);
	TRACE("  max : %g\n",max);
	TRACE("  amplitude : %g\n",amplitude);
	for(i=0;i<NbVertex;i++)
	{
		int _grey = (int)((pMax[i]-min)/amplitude * 255.0f);
		unsigned char grey = _grey > 255 ? 255 : _grey;
		m_ArrayVertex[i]->SetColor(CColor(pRamp->Red(grey),pRamp->Green(grey),pRamp->Blue(grey)));
	}

	SetModified();

	TRACE("End coloring mesh (curvature)\n");
}

//********************************************
// ColorSpaceNormal
// Each vertex is colored, function of 
// normal space (sum of angles between
// adjacent faces)
//********************************************
void CMesh3d::ColorNormalSpace(CColorRamp *pRamp)
{
	TRACE("Start coloring mesh (space of normals)\n");
	int i,NbVertex = m_ArrayVertex.GetSize();
	TRACE("  %d vertices\n",NbVertex);
	
	double *pSum = new double[NbVertex];

	// Store curvatures
	for(i=0;i<NbVertex;i++)
		m_ArrayVertex[i]->NormalMax(&pSum[i]);

	// Process extremas
	double min = MAX_DOUBLE;
	double max = 0.0f;
	for(i=0;i<NbVertex;i++)
	{
		min = (pSum[i] < min) ? pSum[i] : min;
		max = (pSum[i] > max) ? pSum[i] : max;
	}

	min = (min < 0.0f) ? 0.0f : min;
	double amplitude = max-min;//max-min;
	TRACE("  min : %g\n",min);
	TRACE("  max : %g\n",max);
	TRACE("  amplitude : %g\n",amplitude);
	for(i=0;i<NbVertex;i++)
	{
		int _grey = (int)((pSum[i]-min)/amplitude * 255.0f);
		unsigned char grey = _grey > 255 ? 255 : _grey;
		//unsigned char grey = (unsigned char)((pCurvature[i]-min)/amplitude * 255.0f);
		m_ArrayVertex[i]->SetColor(CColor(pRamp->Red(grey),pRamp->Green(grey),pRamp->Blue(grey)));
	}

	SetModified();
	delete [] pSum;

	TRACE("End coloring mesh (space of normals)\n");
}


//********************************************
// ColorCompacity
// Each face is colored, function of face
// compacity
//********************************************
void CMesh3d::ColorCompacity(CColorRamp *pRamp)
{
	TRACE("Start coloring mesh (compacity)\n");
	int i,NbFace = m_ArrayFace.GetSize();
	TRACE("  %d face(s)\n",NbFace);
	
	double *pCompacity = new double[NbFace];

	// Store compacity
	for(i=0;i<NbFace;i++)
		pCompacity[i] = m_ArrayFace[i]->Compacity();

	// Process extremas
	double min = MAX_DOUBLE;
	double max = 0.0f;
	for(i=0;i<NbFace;i++)
	{
		min = (pCompacity[i] < min) ? pCompacity[i] : min;
		max = (pCompacity[i] > max) ? pCompacity[i] : max;
	}

	double amplitude = max-min;//max-min;
	TRACE("  min : %g\n",min);
	TRACE("  max : %g\n",max);
	TRACE("  amplitude : %g\n",amplitude);
	for(i=0;i<NbFace;i++)
	{
		int _grey = (int)((pCompacity[i]-min)/amplitude * 255.0f);
		unsigned char grey = _grey > 255 ? 255 : _grey;
		//unsigned char grey = (unsigned char)((pCurvature[i]-min)/amplitude * 255.0f);
		m_ArrayFace[i]->SetColor(CColor(pRamp->Red(grey),pRamp->Green(grey),pRamp->Blue(grey)));
	}

	SetModified();
	delete [] pCompacity;

	TRACE("End coloring mesh (compacity)\n");
}

//********************************************
// ColorHeight
// Each vertex is colored, function of height
//********************************************
void CMesh3d::ColorHeight(CColorRamp *pRamp)
{
	TRACE("Start coloring mesh (height)\n");
	int i,NbVertex = m_ArrayVertex.GetSize();
	TRACE("  %d vertices\n",NbVertex);

	double *pMax = new double[NbVertex];

	// Process extremas
	double min = MAX_DOUBLE;
	double max = 0.0f;
	for(i=0;i<NbVertex;i++)
	{
		float height = m_ArrayVertex[i]->y();
		min = (height < min) ? height : min;
		max = (height > max) ? height : max;
	}

	double amplitude = max-min;//max-min;
	TRACE("  min : %g\n",min);
	TRACE("  max : %g\n",max);
	TRACE("  amplitude : %g\n",amplitude);
	for(i=0;i<NbVertex;i++)
	{
		float height = m_ArrayVertex[i]->y();
		int _grey = (int)((height-min)/amplitude * 255.0f);
		unsigned char grey = _grey > 255 ? 255 : _grey;
		m_ArrayVertex[i]->SetColor(CColor(pRamp->Red(grey),pRamp->Green(grey),pRamp->Blue(grey)));
	}

	delete [] pMax;
	SetModified();

	TRACE("End coloring mesh (height)\n");
}



//////////////////////////////////////////////
// PREDEFINED
//////////////////////////////////////////////

//********************************************
// GenerateBox
//********************************************
int CMesh3d::GenerateBox(float dx,
												 float dy,
												 float dz)
{
	TRACE("Generate box...");
	CVertex3d *pVertex;

	pVertex = new CVertex3d(-dx/2,-dy/2,-dz/2);
	m_ArrayVertex.Add(pVertex);

	pVertex = new CVertex3d(-dx/2,+dy/2,-dz/2);
	m_ArrayVertex.Add(pVertex);

	pVertex = new CVertex3d(+dx/2,+dy/2,-dz/2);
	m_ArrayVertex.Add(pVertex);

	pVertex = new CVertex3d(+dx/2,-dy/2,-dz/2);
	m_ArrayVertex.Add(pVertex);

	pVertex = new CVertex3d(-dx/2,-dy/2,+dz/2);
	m_ArrayVertex.Add(pVertex);

	pVertex = new CVertex3d(-dx/2,+dy/2,+dz/2);
	m_ArrayVertex.Add(pVertex);

	pVertex = new CVertex3d(+dx/2,+dy/2,+dz/2);
	m_ArrayVertex.Add(pVertex);

	pVertex = new CVertex3d(+dx/2,-dy/2,+dz/2);
	m_ArrayVertex.Add(pVertex);

	CFace3d *pFace;
	pFace = new CFace3d(m_ArrayVertex[0],
		                  m_ArrayVertex[1],
		                  m_ArrayVertex[3]);
	pFace->SetNormal(0.0f,0.0f,-1.0f);
	m_ArrayFace.Add(pFace);

	pFace = new CFace3d(m_ArrayVertex[3],
		                  m_ArrayVertex[1],
		                  m_ArrayVertex[2]);
	pFace->SetNormal(0.0f,0.0f,-1.0f);
	m_ArrayFace.Add(pFace);

	pFace = new CFace3d(m_ArrayVertex[0],
		                  m_ArrayVertex[4],
		                  m_ArrayVertex[1]);
	pFace->SetNormal(-1.0f,0.0f,0.0f);
	m_ArrayFace.Add(pFace);

	pFace = new CFace3d(m_ArrayVertex[1],
		                  m_ArrayVertex[4],
		                  m_ArrayVertex[5]);
	pFace->SetNormal(-1.0f,0.0f,0.0f);
	m_ArrayFace.Add(pFace);

	pFace = new CFace3d(m_ArrayVertex[3],
		                  m_ArrayVertex[2],
		                  m_ArrayVertex[7]);
	m_ArrayFace.Add(pFace);
	pFace->SetNormal(1.0f,0.0f,0.0f);

	pFace = new CFace3d(m_ArrayVertex[7],
		                  m_ArrayVertex[2],
		                  m_ArrayVertex[6]);
	m_ArrayFace.Add(pFace);
	pFace->SetNormal(1.0f,0.0f,0.0f);

	pFace = new CFace3d(m_ArrayVertex[4],
		                  m_ArrayVertex[0],
		                  m_ArrayVertex[3]);
	m_ArrayFace.Add(pFace);
	pFace->SetNormal(0.0f,-1.0f,0.0f);

	pFace = new CFace3d(m_ArrayVertex[7],
		                  m_ArrayVertex[4],
		                  m_ArrayVertex[3]);
	m_ArrayFace.Add(pFace);
	pFace->SetNormal(0.0f,-1.0f,0.0f);

	pFace = new CFace3d(m_ArrayVertex[6],
		                  m_ArrayVertex[4],
		                  m_ArrayVertex[7]);
	m_ArrayFace.Add(pFace);
	pFace->SetNormal(0.0f,0.0f,1.0f);

	pFace = new CFace3d(m_ArrayVertex[6],
		                  m_ArrayVertex[5],
		                  m_ArrayVertex[4]);
	m_ArrayFace.Add(pFace);
	pFace->SetNormal(0.0f,0.0f,1.0f);

	pFace = new CFace3d(m_ArrayVertex[1],
		                  m_ArrayVertex[5],
		                  m_ArrayVertex[6]);
	m_ArrayFace.Add(pFace);
	pFace->SetNormal(0.0f,1.0f,0.0f);

	pFace = new CFace3d(m_ArrayVertex[2],
		                  m_ArrayVertex[1],
		                  m_ArrayVertex[6]);
	m_ArrayFace.Add(pFace);
	pFace->SetNormal(0.0f,1.0f,0.0f);

	TRACE("ok\n");

	return 1;
}

//********************************************
// GenerateMap
//********************************************
int CMesh3d::GenerateMap(int line,
												 int col,
												 float min,
												 float max)
{
	TRACE("Generate map...");
	float x,y,z;
	int i,j;

	// Set vertices
	for(i=0;i<col;i++)
		for(j=0;j<line;j++)
			{
			x = min + ((float)i/(float)line)*(max-min);
			z = min + ((float)j/(float)line)*(max-min);
			y = (float)(cos(x)*cos(z));
			m_ArrayVertex.Add(new CVertex3d(x,y,z));
			}

	// Set faces
	for(i=0;i<col-1;i++)
		for(j=0;j<line-1;j++)
			{
			CVertex3d *pVertex1 = m_ArrayVertex[line*i+j];
			CVertex3d *pVertex2 = m_ArrayVertex[line*i+j+1];
			CVertex3d *pVertex3 = m_ArrayVertex[line*(i+1)+j+1];
			CVertex3d *pVertex4 = m_ArrayVertex[line*(i+1)+j];

			m_ArrayFace.Add(new CFace3d(pVertex1,pVertex2,pVertex3));
			m_ArrayFace.Add(new CFace3d(pVertex1,pVertex3,pVertex4));
			}

	TRACE("ok\n");
	return 1;
}


//********************************************
// GenerateMapFromImage
//********************************************
int CMesh3d::GenerateMap(CTexture *pTexture,
												 int width,
												 int height,
												 int FlagColor /* = 1 */)
{
	// Cleanup
	Free();

	int WidthImage = pTexture->GetWidth();
	int HeightImage = pTexture->GetHeight();
	unsigned char red,green,blue;

	// Vertices
	int j,k=0;
	for(j=0;j<height;j++)
		for(int i=0;i<width;i++)
		{
			int xImage = (int)((float)i/(float)width*(float)WidthImage);
			int yImage = (int)((float)j/(float)height*(float)HeightImage);
			int index = m_ArrayVertex.Add(new CVertex3d((float)i,(float)pTexture->Grey(xImage,yImage),(float)j));
			if(FlagColor)
			{
				pTexture->Color(xImage,yImage,&red,&green,&blue);
				m_ArrayVertex[index]->SetColor(CColor(red,green,blue));
			}
		}

	// Faces
	for(j=0;j<height-1;j++)
		for(int i=0;i<width-1;i++)
		{
			int index = m_ArrayFace.Add(new CFace3d(m_ArrayVertex[j*width+i+1],
				                                      m_ArrayVertex[j*width+i],
																							m_ArrayVertex[(j+1)*width+i+1]));
			m_ArrayFace.Add(new CFace3d(m_ArrayVertex[(j+1)*width+i+1],
				                          m_ArrayVertex[j*width+i],
																	m_ArrayVertex[(j+1)*width+i]));
			if(FlagColor)
			{
				m_ArrayFace[index]->SetColor(*m_ArrayVertex[j*width+i]->GetColor());
				m_ArrayFace[index+1]->SetColor(*m_ArrayVertex[j*width+i+1]->GetColor());
			}
		}

	// Rebuild
	BuildAdjacency();
	CalculateNormalPerFace();
	CalculateNormalPerVertex();

	return 1;
}

/////////////////////////////////////////////
// INTERSECTION
/////////////////////////////////////////////

//********************************************
// NearestIntersectionWithLine
// Non-optimized
// Nearest -> distance from pV0 to pVertexResult
//********************************************
int CMesh3d::NearestIntersectionWithLine(CVertex3d *pV0,
																				 CVertex3d *pV1,
																				 CVertex3d *pVertexResult,
																				 CFace3d **ppFaceResult,
																				 int *pNbFaceVisited)
{
	return ::NearestIntersectionWithLine(&m_ArrayFace,pV0,pV1,pVertexResult,ppFaceResult,pNbFaceVisited);
}

/////////////////////////////////////////////
// I/O
/////////////////////////////////////////////

//********************************************
// WriteFile
//********************************************
int CMesh3d::WriteFile(CStdioFile &file)
{
	CString string;
	TRY
	{
		// Comment
		string.Format("# Mesh : %d vertices, %d faces\n",NbVertex(),NbFace());
		file.WriteString(string);

		// First line
		file.WriteString("DEF Mesh-ROOT Transform {\n");

		// Transform
		string.Format("  translation %g %g %g\n",m_Transform.GetTranslation()->x(),
		                                         m_Transform.GetTranslation()->y(),
								m_Transform.GetTranslation()->z());
		file.WriteString(string);
		string.Format("  rotation %g %g %g %g\n",m_Transform.GetRotation()->x(),
                                             m_Transform.GetRotation()->y(),
		                                         m_Transform.GetRotation()->z(),
								m_Transform.GetValueRotation()/360.0f*2*3.14159265359f);
		file.WriteString(string);
		string.Format("  scale %g %g %g\n",m_Transform.GetScale()->x(),
			                                 m_Transform.GetScale()->y(),
			                                 m_Transform.GetScale()->z());
		file.WriteString(string);

		// Material
		file.WriteString("  children [\n");
		file.WriteString("    Shape {\n");
		file.WriteString("      appearance Appearance {\n");
		file.WriteString("        material Material {\n");
		file.WriteString("          diffuseColor 0 0 0\n"); // todo
		file.WriteString("        }\n");
		file.WriteString("      }\n");

		// Geometry
		file.WriteString("      geometry DEF Mesh-FACES IndexedFaceSet {\n");
		file.WriteString("        ccw TRUE\n");
		file.WriteString("        solid TRUE\n");

		// Vertices
		file.WriteString("        coord DEF Mesh-COORD Coordinate { point [\n");
		int i,NbVertex = m_ArrayVertex.GetSize();
		for(i=0;i<NbVertex;i++)
		{
			string.Format("          %g %g %g",m_ArrayVertex[i]->x(),
		                                     m_ArrayVertex[i]->y(),
																	       m_ArrayVertex[i]->z());
			file.WriteString(string);
			if(i!=(NbVertex-1))
				file.WriteString(",\n");
			else
				file.WriteString("]\n");
		}
		file.WriteString("        }\n");

		// Faces
		file.WriteString("        coordIndex [\n");
		int NbFace = m_ArrayFace.GetSize();
		for(i=0;i<NbFace;i++)
		{
			string.Format("          %d, %d, %d, -1",m_ArrayVertex.IndexFrom(m_ArrayFace[i]->v(0)),
			                                         m_ArrayVertex.IndexFrom(m_ArrayFace[i]->v(1)),
																						   m_ArrayVertex.IndexFrom(m_ArrayFace[i]->v(2)));
			file.WriteString(string);
			if(i!=(NbFace-1))
				file.WriteString(",\n");
			else
				file.WriteString("]\n");
		}

		// End
		file.WriteString("        }\n");
		file.WriteString("      }\n");
		file.WriteString("    ]\n");
		file.WriteString("  }\n\n");

	}
	CATCH(CFileException, e)
	{
		#ifdef _DEBUG
				afxDump << "Error during writing transform" << e->m_cause << "\n";
		#endif
		AfxMessageBox("Error during writing transform");
		return 0;
	}
	END_CATCH

	return 1;
}

//********************************************
// WriteFileRaw (binary raw mode)
//********************************************
int CMesh3d::WriteFileRaw(CFile &file)
{
	// A mesh : 
	//*******************************************
	// Transform  : 10 * float 32 bits
	// NbVertices : UINT 32 bits
	// NbFaces    : UINT 32 bits
	// Vertices   : x y z    : 3 x float 32 bits
	// Faces      : v1 v2 v3 : 3 x UINT 32 bits
	//*******************************************
	// Cost : 40 + 8 + 12*(v+f) bytes

	CString string;
	TRY
	{

		// Transform

		// Translation (xyz)
		float x,y,z;
		x = m_Transform.GetTranslation()->x();
		y = m_Transform.GetTranslation()->y();
		z = m_Transform.GetTranslation()->z();
		file.Write(&x,sizeof(float));
		file.Write(&y,sizeof(float));
		file.Write(&z,sizeof(float));

		// Rotation (xyz)
		x = m_Transform.GetRotation()->x();
		y = m_Transform.GetRotation()->y();
		z = m_Transform.GetRotation()->z();
		float v = m_Transform.GetValueRotation();
		file.Write(&x,sizeof(float));
		file.Write(&y,sizeof(float));
		file.Write(&z,sizeof(float));
		file.Write(&v,sizeof(float));

		// Scale (xyz)
		x = m_Transform.GetScale()->x();
		y = m_Transform.GetScale()->y();
		z = m_Transform.GetScale()->z();
		file.Write(&x,sizeof(float));
		file.Write(&y,sizeof(float));
		file.Write(&z,sizeof(float));

		// Geometry 
		// NbVertices
		// NbFaces
		
		unsigned int NbVertex = m_ArrayVertex.GetSize();
		unsigned int NbFace = m_ArrayFace.GetSize();
		file.Write(&NbVertex,sizeof(unsigned int));
		file.Write(&NbFace,sizeof(unsigned int));

		// Vertices
		unsigned int i;
		for(i=0;i<NbVertex;i++)
		{
			x = m_ArrayVertex[i]->x();
			y = m_ArrayVertex[i]->y();
			z = m_ArrayVertex[i]->z();
			file.Write(&x,sizeof(float));
			file.Write(&y,sizeof(float));
			file.Write(&z,sizeof(float));
		}

		// Faces
		unsigned int v1,v2,v3;
		for(i=0;i<NbFace;i++)
		{
			v1 = m_ArrayVertex.IndexFrom(m_ArrayFace[i]->v1());
			v2 = m_ArrayVertex.IndexFrom(m_ArrayFace[i]->v2());
			v3 = m_ArrayVertex.IndexFrom(m_ArrayFace[i]->v3());
			file.Write(&v1,sizeof(unsigned int));
			file.Write(&v2,sizeof(unsigned int));
			file.Write(&v3,sizeof(unsigned int));
		}

	}
	CATCH(CFileException, e)
	{
		#ifdef _DEBUG
				afxDump << "Error during writing " << e->m_cause << "\n";
		#endif
		AfxMessageBox("Error during writing");
		return 0;
	}
	END_CATCH

	return 1;
}


//********************************************
// GetMinArea
//********************************************
double CMesh3d::GetMinArea(CFace3d **ppFace /* NULL */)
{
	double min = MAX_DOUBLE;
	int size = m_ArrayFace.GetSize();
	for(int i=0;i<size;i++)
	{
		double area = m_ArrayFace[i]->Area();
		if(area < min)
		{
			min = area;
			if(ppFace != NULL)
				*ppFace = m_ArrayFace[i];
		}
	}
	return min;
}

//********************************************
// GetMeanArea
//********************************************
double CMesh3d::GetMeanArea()
{
	return ::GetMeanArea(&m_ArrayFace);
}



// ** EOF **




