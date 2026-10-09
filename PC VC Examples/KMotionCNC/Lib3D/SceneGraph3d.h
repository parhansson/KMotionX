//********************************************
// SceneGraph.h
// class CSceneGraph
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 15/01/98
// Modified : 26/05/98
//********************************************

#ifndef _SCENEGRAPH_3D_
#define _SCENEGRAPH_3D_

#include "Object3d.h"
#include "Texture.h"
#include "Material.h"
#include "Array3d.h"

enum {
	RED_COLOR_INDEX,
	GREEN_COLOR_INDEX,
	BLUE_COLOR_INDEX,
	BLACK_COLOR_INDEX,
	GRAY_COLOR_INDEX,
	WHITE_COLOR_INDEX,
	UNCUT_FEED_COLOR_INDEX,		// G-code viewer preview: feed not yet cut
	UNCUT_RAPID_COLOR_INDEX,	// G-code viewer preview: rapid not yet made
	CUT_RAPID_COLOR_INDEX,		// G-code viewer preview: rapid made (cut feed = WHITE)
	N_PATH_COLORS				// size of CPath3d::colorlookup and the shader's colors[]
};


class CSceneGraph3d
{

private :

	// Datas
	CArray3d<CObject3d> m_ArrayObject3d;
	CArray3d<CTexture>  m_ArrayTexture;
	CArray3d<CMaterial> m_ArrayMaterial;
	GLuint *m_pIndexTextureBinding;
	unsigned int m_ListDone;




public :

	// Constructor
	CSceneGraph3d();
	~CSceneGraph3d();


	// Datas
	void Add(CObject3d *pObject3d);
	void RemoveAt(int index);
	CObject3d *GetAt(int index);
	CObject3d *operator[](int index);
	int NbObject(void) { return m_ArrayObject3d.GetSize(); }
	void Free(void);

	// I/O
	int SaveFile(wchar_t *name);
	int SaveFileRaw(wchar_t *name);
	int WriteHeader(CStdioFile &file,wchar_t *name);

	// Misc
	int BuildAdjacency();
	int CalculateNormalPerVertex();
	int CalculateNormalPerFace();

	// Binding
	void SetNormalBinding(int type);
	void SetColorBinding(int type);

	// OpenGL
	virtual int glBuildList();
	void glDraw(void);
	void glDraw(int type);

	// Textures
	CArray3d<CTexture> *GetArrayTexture() { return &m_ArrayTexture; }
	int HasTexture(wchar_t *name,int *index);
	int NbTexture() { return m_ArrayTexture.GetSize(); }
	int AddTexture(CTexture *pTexture) { return m_ArrayTexture.Add(pTexture); }
	void RemoveAllTexture() { m_ArrayTexture.RemoveAll(); }
	void RemoveTexture(int index) { m_ArrayTexture.RemoveAt(index); }
	CTexture *GetTexture(int index) { return m_ArrayTexture[index]; }
};

#endif // _SCENEGRAPH_3D_
