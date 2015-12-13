//********************************************
// Path3d.h
// class CPath3d
//********************************************
// A mesh : simple path
// + Array of vertex
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 15/01/98
// Modified : 07/07/98
// Modified : 07/16/06 - Tom Kerekes
//********************************************

#ifndef _Path_3D_
#define _Path_3D_

#include "Array3d.h"
#include "Vertex3dFast.h"
#include "Face3d.h"
#include "Vector3d.h"
#include "Material.h"
#include "Transform.h"


class CPath3d : public CObject3d
{

private :

	// Datas
	CArray3d<CVertex3dFast> m_ArrayVertex;

	CArray3d<int> m_ListArray;

	CTransform m_Transform;
	CString m_Name;
	


	// OpenGL-specific
	unsigned int m_nListsOpenGL;
	unsigned int m_nPointsPerList;
	unsigned int m_nPointsInList;
	unsigned int m_ListDone;
	int m_Modified;
	int m_Show;

	float m_LastToolOffsetX,m_LastToolOffsetY,m_LastToolOffsetZ;
	float m_LastToolPositionX,m_LastToolPositionY,m_LastToolPositionZ;


public :

	// Constructor
	CPath3d();
	virtual ~CPath3d();

	// Datas
	void Free();
	virtual int GetType();
	int IsValid();
	void Copy(CPath3d *pPath);


	// Vertices
	int NbVertex() { return m_ArrayVertex.GetSize(); }
	void SetNbVertex(int NbVertex)  { m_ArrayVertex.SetSize(NbVertex); }
	CArray3d<CVertex3dFast> *GetArrayVertex() { return &m_ArrayVertex; }
	void AddVertex(CVertex3dFast *pVertex) { m_ArrayVertex.Add(pVertex); m_Modified=true; }
	void AddVertexTool(CVertex3dFast *pVertex);

	int DeleteVertex(CVertex3dFast *pVertex);
	int DeleteVertex(int index);
	CVertex3dFast *GetVertex(int index) {return m_ArrayVertex[index];}
	int Has(CVertex3dFast *pVertex) { return m_ArrayVertex.Has(pVertex); }

	int RemovePathEnd(int sequence_number,int ID, double x, double y, double z);
	double FindDistPointToSegment(CVertex3dFast *s0, CVertex3dFast *s1, CVertex3dFast *s2);

	// Transform
	void SetTransform(CTransform &transform) { m_Transform.Copy(transform); }
	CTransform *GetTransform(void) { return &m_Transform; }

	// Range
	void Range(int coord,float *min,float *max); 
	void Range(int coord,float min,float max); 
	void Offset(int coord,float offset);
	void Scale(int coord,float scale);
	void Move(float dx,float dy,float dz);

	// OpenGL
	virtual int glBuildList();
	virtual int glDraw();
	void Show(int flag) { m_Show = flag; }


	// Modif
	void SetModified() { m_Modified=1; }
	void InvalidateDisplayList() {};
	int GetModified() { return m_Modified; }


	// Vertex removal
	int VertexRemoval(CVertex3dFast *pV);
	int VertexRemoval();

	bool m_ToolOffsetValid;
};


#endif // _Path_3D_
