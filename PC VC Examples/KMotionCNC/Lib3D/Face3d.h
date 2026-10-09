//********************************************
// Face3d.h
// class CFace3d
//********************************************
// A triangular face :
// + three vertex ref (subdivision added)
// + three faces ref
// + normal vector
// + color
// + material ref
//********************************************
// m_pFace : shares edges
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 15/12/97
// Modified : 18/02/98
//********************************************

#ifndef _FACE_3D_
#define _FACE_3D_

#include "Object3d.h"
#include "Vector3d.h"
#include "Color.h"

class CVertex3d;
class CVectorSet3d;
class CMesh3d;
template<class T>
class CArray3d;

class CFace3d : public CObject3d
{

private :

	CVertex3d *m_pVertex[6];      // Vertices (subdivision included)
	CFace3d *m_pFace[3];          // Neighbors
	CVector3d m_Normal;           // Normal
	CColor m_Color;               // Color
	unsigned int m_IndexMaterial; // Material
	int m_Flag;                   // Flag (processings)

public :

	// Constructors
	CFace3d();
	CFace3d(CVertex3d *pVertex1,CVertex3d *pVertex2,CVertex3d *pVertex3);
	CFace3d(CFace3d *pFace);
	virtual ~CFace3d() {}

	// General
	int IsValid();
	void Clear();
	int Equal(CFace3d *pFace);
	void Set(CVertex3d *pVertex1,CVertex3d *pVertex2,CVertex3d *pVertex3);
	void Set(CFace3d *pFace1,CFace3d *pFace2,CFace3d *pFace3);
	void Set(CVertex3d *pVertex1,CVertex3d *pVertex2,CVertex3d *pVertex3,
		       CFace3d *pFace1,CFace3d *pFace2,CFace3d *pFace3);
	void Set(CFace3d *pFace);
	CVertex3d* GetCenter(void);
	CVertex3d *FindNearestVertex(CVertex3d *pVertex);

	// Neighboring
	int BuildArrayNeighbor(CArray3d<CFace3d> *pArrayFace,int depth = 1);

	// Color
	CColor *GetColor(void) { return &m_Color; }
	void SetColor(CColor &color) { m_Color.Set(color); }

	// Flag
	int GetFlag(void) { return m_Flag; }
	void SetFlag(int flag) { m_Flag = flag; }
	void SetFlagOnVerticesIfDiff(int FlagDiff,int flag);

	// Per vertex
	void v1(CVertex3d *pVertex) { m_pVertex[0] = pVertex; }
	void v2(CVertex3d *pVertex) { m_pVertex[1] = pVertex; }
	void v3(CVertex3d *pVertex) { m_pVertex[2] = pVertex; }
	void v(int index,CVertex3d *pVertex) { m_pVertex[index%6] = pVertex; }
	int IndexFrom(CVertex3d *pVertex);

	// Per neighboring face
	void f1(CFace3d *pFace) { m_pFace[0] = pFace; }
	void f2(CFace3d *pFace) { m_pFace[1] = pFace; }
	void f3(CFace3d *pFace) { m_pFace[2] = pFace; }
	void f(int index,CFace3d *pFace) { m_pFace[index%3] = pFace; }

	// Normal
	void SetNormal(float x,float y,float z) { m_Normal.Set(x,y,z); }
	CVector3d * GetNormal() { return &m_Normal; }
	int HasNormal() { return (m_Normal.GetNormL2Square() != 0);}
	void CalculateNormal();

	// Sharp edges
	void ColorSharpEdge(double threshold,CColor &color);
	int GetSharpEdge(double threshold,int *SharpEdge);
	int HasSharpEdge(double threshold);

	virtual int GetType();

	// Numbers
	int NbVertex();
	int NbFaceNeighbor();

	// Per vertex
	CVertex3d *v1(void) { return m_pVertex[0]; }
	CVertex3d *v2(void) { return m_pVertex[1]; }
	CVertex3d *v3(void) { return m_pVertex[2]; }
	CVertex3d *v(int index) { return m_pVertex[index%6]; }

	// Per face
	CFace3d *f1(void) { return m_pFace[0]; }
	CFace3d *f2(void) { return m_pFace[1]; }
	CFace3d *f3(void) { return m_pFace[2]; }
	CFace3d *f(int index) { return m_pFace[index%3]; }

	// Find whenever face contain pVertex
	int HasVertex(CVertex3d *pVertex);
	int HasVertex(CVertex3d *pVertex,int *index);
	int HasVertexWithFlag(int flag);

	// Find whenever face contain pFace
	int HasNeighbor(CFace3d *pFace);
	int HasNeighbor(CFace3d *pFace,int *index);
	CFace3d *GetFaceNeighborExclusive(CFace3d *pF0,CFace3d *pF1);
	CFace3d *GetNeighborExclusive(CVertex3d *pVertexHas,CVertex3d *pVertexHasNot);
	CVertex3d *GetVertexExclusive(CVertex3d *pV0,CVertex3d *pV1);
	CVertex3d *GetVertexExclusive(CVertex3d *pV,CFace3d *pFace);
	CVertex3d *GetVertexFaceNeighborExclusive(unsigned int index);
	CVertex3d *GetVertexFaceNeighborExclusive(int index);
	int JointNeighbor(CFace3d **pFace0,CFace3d **pFace1);

	// Vertex sharing
	int Share1Vertex(CFace3d *pFace);
	int Share2Vertex(CFace3d *pFace);
	int Share2Vertex(CFace3d *pFace,int *IndexEdgeThis,int *IndexEdgeOther);
	int Share2Vertex(CFace3d *pFace,CVertex3d **pSharedV1,CVertex3d **pSharedV2);

	// Find whenever face contain pOld, and update it to pNew
	int UpdateVertex(CVertex3d *pOld,CVertex3d *pNew);
	int UpdateNeighbor(CFace3d *pOld,CFace3d *pNew);
	int UpdateVertexRecursive(CVertex3d *pVertexOld,CVertex3d *pVertexNew);

	// OpenGL
	void glDraw(unsigned char *ColorFace,CMesh3d *pMesh = NULL,unsigned char *ColorNeightbor = NULL);

	// Debug
	void Trace();

	// Geometric values
	double Area(); 
	double Perimeter();
	double Compacity();

};

#endif // _FACE_3D_
