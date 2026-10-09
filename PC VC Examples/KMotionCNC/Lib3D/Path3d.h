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
#include "SceneGraph3D.h"

typedef struct
{
	GLuint Array;
	GLuint VertexBuf;
	GLuint ColorBuf;
	int NbVertex;
} Array2Bufs;

class CPath3d : public CObject3d
{

private :

	// Datas
	CArray3d<CVertex3dFast> m_ArrayVertex;

	CArray3d<Array2Bufs> m_ListArray;

	CTransform m_Transform;
	CString m_Name;
	
	// OpenGL-specific
	unsigned int m_nListsOpenGL;
	unsigned int m_nPointsPerList;
	unsigned int m_nPointsInList;
	unsigned int m_ListDone;
	int m_Modified;
	int m_Show;
	int m_RecolorFirst, m_RecolorLast;	// pending recolour range (empty when Last < First)

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

	GLuint createShaderProgram();

	GLfloat ColorToFloat(unsigned char c);

	unsigned char GetColorIndexCached(CColor* c);

	// OpenGL
	virtual int glBuildList();
	virtual int glDraw();
	void Show(int flag) { m_Show = flag; }
	static GLuint shaderProgram[N_GL_CONTEXT];  // save the shader program for each context
	static HGLRC  shaderProgram_gl_context[N_GL_CONTEXT];

	// Set the uniform array of colors (index order must match the COLOR_INDEX enum
	// in SceneGraph3d.h; the path shader declares colors[N_PATH_COLORS]).  A vertex colour that
	// is not in this table is drawn with WHITE_COLOR_INDEX (GetColorIndexCached).
	// Entries are written as <byte>/255 so they compare equal to ColorToFloat().
	const GLfloat colorlookup[N_PATH_COLORS][3] = {
	   {1.0f, 0.0f, 0.0f}, // Red
	   {0.0f, 1.0f, 0.0f}, // Green
	   {0.0f, 0.0f, 1.0f}, // Blue
	   {0.0f, 0.0f, 0.0f}, // Black
	   {0.5f, 0.5f, 0.5f}, // Gray
	   {1.0f, 1.0f, 1.0f}, // White
	   {150.0f / 255.0f, 150.0f / 255.0f, 150.0f / 255.0f}, // Uncut feed  - medium gray
	   {200.0f / 255.0f,  80.0f / 255.0f,  80.0f / 255.0f}, // Uncut rapid - muted red
	   {255.0f / 255.0f, 175.0f / 255.0f, 175.0f / 255.0f}  // Cut rapid   - light red
	};

	// Recolouring of vertices already uploaded to the GPU.  glBuildList() only
	// ever (re)builds the last, partially filled list, so a colour change on an
	// already-listed vertex needs its list's colour buffer re-uploaded.  Callers
	// change vertex colours, then MarkRecolor(first,last) the index range; the
	// next glBuildList() (GL context current) re-uploads those lists' colours.
	// Caller must hold the view's OpenGLMutex around the colour changes + mark.
	void MarkRecolor(int first, int last)
	{
		if (first < m_RecolorFirst) m_RecolorFirst = first;
		if (last  > m_RecolorLast)  m_RecolorLast  = last;
	}
	bool RecolorPending() const { return m_RecolorLast >= m_RecolorFirst; }



	// Modif
	void SetModified() { m_Modified=1; }
	
	void InvalidateDisplayList() 
	{
		// delete all display lists
		int nlists = m_ListArray.GetSize();

		while (nlists)
		{
			// Erase last list
			nlists--;
			Array2Bufs* p = m_ListArray.GetAt(nlists);
			glBindVertexArray(0); // unbound any vertex array
			glDeleteVertexArrays(1, &p->Array);
			glDeleteBuffers(1, &p->VertexBuf);
			glDeleteBuffers(1, &p->ColorBuf);
			m_ListArray.RemoveAt(nlists);
			delete p;
		}

		m_nPointsInList = 0;
	};

	int GetModified() { return m_Modified; }


	// Vertex removal
	int VertexRemoval(CVertex3dFast *pV);
	int VertexRemoval();

	bool m_ToolOffsetValid;
};


#endif // _Path_3D_
