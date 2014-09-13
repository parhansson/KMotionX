//********************************************
// SceneGraph.cpp
//********************************************
// class CSceneGraph
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
CSceneGraph3d::CSceneGraph3d()
{
	m_pIndexTextureBinding = NULL;
	m_ListDone = 0;
}

//********************************************
// Destructor
//********************************************
CSceneGraph3d::~CSceneGraph3d()
{
	Free();
}

//********************************************
// Destructor
//********************************************
void CSceneGraph3d::Free(void)
{
	// Textures
	if(m_ArrayTexture.GetSize())
	{
		::glDeleteTextures(m_ArrayTexture.GetSize(),m_pIndexTextureBinding);
		if(m_pIndexTextureBinding != NULL)
		{
			delete [] m_pIndexTextureBinding;
			m_pIndexTextureBinding = NULL;
		}
	}
	m_ArrayTexture.Free();
	// Objects
	m_ArrayObject3d.Free();
}

//////////////////////////////////////////////
// DATAS
//////////////////////////////////////////////

//********************************************
// Add
//********************************************
void CSceneGraph3d::Add(CObject3d *pObject3d)
{
	m_ArrayObject3d.Add(pObject3d);
}

//********************************************
// Add
//********************************************
void CSceneGraph3d::RemoveAt(int index)
{
	m_ArrayObject3d.RemoveAt(index);
}

//********************************************
// GetAt
//********************************************
CObject3d *CSceneGraph3d::GetAt(int index)
{
	ASSERT(index < m_ArrayObject3d.GetSize());
	return m_ArrayObject3d[index];
}
	
//********************************************
// Operator []
//********************************************
CObject3d *CSceneGraph3d::operator[](int index)
{
	ASSERT(index < m_ArrayObject3d.GetSize());
	return m_ArrayObject3d[index];
}
	



//////////////////////////////////////////////
// OPENGL
//////////////////////////////////////////////

//********************************************
// BuildList
//********************************************
int CSceneGraph3d::glBuildList()
{
	// Meshes
	//***********************************
	unsigned int i,size = m_ArrayObject3d.GetSize();
	for(i=0; i<size; i++)
	{
		CObject3d *pObject3d = m_ArrayObject3d.GetAt(i);
		if(pObject3d != NULL)
		{
			pObject3d->glBuildList();
		}
	}

	// Textures
	//***********************************
	unsigned int NbTexture = m_ArrayTexture.GetSize();
	if(NbTexture)
	{
		TRACE("SceneGraph : texture binding...(%d texture(s))\n",NbTexture);

		// Cleanup
		if(m_pIndexTextureBinding != NULL)
		{
			::glDeleteTextures(NbTexture,m_pIndexTextureBinding);
			delete [] m_pIndexTextureBinding;
		}
		m_pIndexTextureBinding = new unsigned int[NbTexture];

		::glGenTextures(NbTexture,m_pIndexTextureBinding);
		int error = glGetError();
		ASSERT(error !=  GL_INVALID_VALUE);
		ASSERT(error !=  GL_INVALID_OPERATION);

		TRACE("Bind texture...\n");
		for(i=0;i<NbTexture;i++)
		{
			while (GL_NO_ERROR != glGetError() ) {}

			// Bind texture
			glBindTexture(GL_TEXTURE_2D,m_pIndexTextureBinding[i]);


			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			int error = glGetError();
			ASSERT(error !=  GL_INVALID_ENUM);
			ASSERT(error !=  GL_INVALID_OPERATION);

			// Read datas
			CTexture *pTexture = m_ArrayTexture[i];
			ASSERT(pTexture->GetData() != NULL);
			::glTexImage2D(GL_TEXTURE_2D,0,3,
										 pTexture->GetWidth(),pTexture->GetHeight(),0,			
										 GL_RGB,GL_UNSIGNED_BYTE,pTexture->GetData());
			error = glGetError();
			ASSERT(error !=  GL_INVALID_ENUM);
			ASSERT(error !=  GL_INVALID_OPERATION);
			ASSERT(error !=  GL_INVALID_VALUE);

		}

	}
	m_ListDone = 1;

	return 1;
}

//********************************************
// glDraw
//********************************************
void CSceneGraph3d::glDraw(void)
{
	if(!m_ListDone)
		glBuildList();

	unsigned int size = m_ArrayObject3d.GetSize();
	for(unsigned int i=0; i<size; i++)
	{
		CObject3d *pObject3d = m_ArrayObject3d[i];
		// Texture
		if(pObject3d->GetType() == TYPE_MESH3D)
		{
			CMesh3d *pMesh = (CMesh3d *)pObject3d;
			int IndexTexture = pMesh->GetTextureIndex();
			if(IndexTexture >= 0)
			{
				ASSERT(glIsTexture(m_pIndexTextureBinding[IndexTexture]));
				glBindTexture(GL_TEXTURE_2D,m_pIndexTextureBinding[IndexTexture]);
				TRACE("Texture : %d\n",m_pIndexTextureBinding[IndexTexture]);
			}
			// Drawing
			pObject3d->glDraw();
		}
		else
			pObject3d->glDraw();
	}
}

//********************************************
// glDraw
// draw only type
//********************************************
void CSceneGraph3d::glDraw(int type)
{
	if(!m_ListDone)
		glBuildList();

	unsigned int size = m_ArrayObject3d.GetSize();
	for(unsigned int i=0; i<size; i++)
	{
		CObject3d *pObject3d = m_ArrayObject3d[i];
		if(pObject3d->GetType() == type)
			pObject3d->glDraw();
	}
}


//////////////////////////////////////////////
// MISC
//////////////////////////////////////////////

//********************************************
// BuildAdjacency
// For each mesh
//********************************************
int CSceneGraph3d::BuildAdjacency()
{
	int size = m_ArrayObject3d.GetSize();
	if(size ==0)
		return 0;
	
	TRACE("Scene %x : Start BuildAdjacency...\n",this);
	TRACE("  NbObject : %d\n",size);
	for(int i=0;i<size;i++)
		{
		CObject3d *pObject3d = m_ArrayObject3d[i];
		if(pObject3d->GetType() != TYPE_MESH3D)
			continue;
		CMesh3d *pMesh = (CMesh3d *)m_ArrayObject3d[i];
		pMesh->BuildAdjacency();
		}
	TRACE("Scene %x : End BuildAdjacency...\n",this);

	return 1;
}

//********************************************
// CalculateNormalPerVertex
// For each mesh
//********************************************
int CSceneGraph3d::CalculateNormalPerVertex()
{
	int size = m_ArrayObject3d.GetSize();
	if(size ==0)
		return 0;

	for(int i=0;i<size;i++)
		{
		CObject3d *pObject3d = m_ArrayObject3d[i];
		if(pObject3d->GetType() != TYPE_MESH3D)
			continue;
		CMesh3d *pMesh = (CMesh3d *)m_ArrayObject3d[i];
		pMesh->CalculateNormalPerVertex();
		}

	return 1;
}

//********************************************
// CalculateNormalPerFace
// For each mesh
//********************************************
int CSceneGraph3d::CalculateNormalPerFace()
{
	int size = m_ArrayObject3d.GetSize();
	if(size ==0)
		return 0;

	for(int i=0;i<size;i++)
		{
		CObject3d *pObject3d = m_ArrayObject3d[i];
		if(pObject3d->GetType() != TYPE_MESH3D)
			continue;
		CMesh3d *pMesh = (CMesh3d *)m_ArrayObject3d[i];
		pMesh->CalculateNormalPerFace();
		}

	return 1;
}


//********************************************
// SetNormalBinding
//********************************************
void CSceneGraph3d::SetNormalBinding(int type)
{
	int size = m_ArrayObject3d.GetSize();
	for(int i=0;i<size;i++)
	{
		CObject3d *pObject3d = m_ArrayObject3d[i];
		if(pObject3d->GetType() != TYPE_MESH3D)
			continue;
		CMesh3d *pMesh = (CMesh3d *)m_ArrayObject3d[i];
		pMesh->SetNormalBinding(type);
	}
}

//********************************************
// SetColorBinding
//********************************************
void CSceneGraph3d::SetColorBinding(int type)
{
	int size = m_ArrayObject3d.GetSize();
	for(int i=0;i<size;i++)
	{
		CObject3d *pObject3d = m_ArrayObject3d[i];
		if(pObject3d->GetType() != TYPE_MESH3D)
			continue;
		CMesh3d *pMesh = (CMesh3d *)m_ArrayObject3d[i];
		pMesh->SetColorBinding(type);
	}
}


//////////////////////////////////////////////
// TEXTURES
//////////////////////////////////////////////

//********************************************
// HasTexture
//********************************************
int CSceneGraph3d::HasTexture(char *name,
															int *index)
{
	for(int i=0;i<m_ArrayTexture.GetSize();i++)
		if(m_ArrayTexture[i]->GetFileName() == name)
		{
			*index = i;
			return 1;
		}
	return 0;
}


//////////////////////////////////////////////
// I/O
//////////////////////////////////////////////

//********************************************
// SaveFile
//********************************************
int CSceneGraph3d::SaveFile(char *name)
{
	// Check
	if(NbObject() == 0)
	{
		AfxMessageBox("This scene does not contain meshes");
		return 0;
	}

	// Check for valid file
	CStdioFile file;
	CFileException ex;
	
	// Write header
	if(!WriteHeader(file,name))
	{
		AfxMessageBox("Error during writing header");
		return 0;
	}

	// Meshes
	for(int i=0;i<NbObject();i++)
	{
		CObject3d *pObject = m_ArrayObject3d[i];
		if(pObject->GetType() == TYPE_MESH3D)
			((CMesh3d *)pObject)->WriteFile(file);
	}

	// Close file
  file.Close();

	return 1;
}

//********************************************
// SaveFileRaw
//********************************************
int CSceneGraph3d::SaveFileRaw(char *name)
{
	// Check
	if(NbObject() == 0)
	{
		AfxMessageBox("This scene does not contain meshes");
		return 0;
	}

	// Check for valid file
	CFile file;
	CFileException ex;

	// Try to open file (text mode)
	if(!file.Open(name,CFile::modeCreate | CFile::modeWrite | CFile::typeBinary,&ex))
	{
		#ifdef _DEBUG
		  afxDump << "File could not be opened " << ex.m_cause << "\n";
		#endif
		AfxMessageBox("Unable to open file for writing");
		return 0;
	}

	
	// Meshes
	unsigned int NbMesh = NbObject();
	file.Write(&NbMesh,sizeof(unsigned int));
	for(unsigned int i=0;i<NbMesh;i++)
	{
		CObject3d *pObject = m_ArrayObject3d[i];
		if(pObject->GetType() == TYPE_MESH3D)
			((CMesh3d *)pObject)->WriteFileRaw(file);
	}

	// Close file
  file.Close();

	return 1;
}


//**********************************************
// WriteHeader
// Do not close file
//**********************************************
int CSceneGraph3d::WriteHeader(CStdioFile &file,
															 char *name)
{
	CFileException ex;
	
	// Try to open file (text mode)
	if(!file.Open(name,CFile::modeCreate | CFile::modeWrite | CFile::typeText,&ex))
	{
		#ifdef _DEBUG
		  afxDump << "File could not be opened " << ex.m_cause << "\n";
		#endif
		AfxMessageBox("Unable to open file for writing");
		return 0;
	}

	// ** Header *******************************
	TRACE("\nSave VRML 2.0 File...\n");
	TRACE("  name : %s\n",name);
	TRY
	{
		file.WriteString("#VRML V2.0 utf8\n\n");
		file.WriteString("# Produced by 3d Toolbox 1.0 (Pierre Alliez, CNET / DIH / HDM)\n\n");
	}
	CATCH(CFileException, e)
	{
		#ifdef _DEBUG
				afxDump << "Error during writing " << e->m_cause << "\n";
		#endif
		AfxMessageBox("Error during writing file header");
		file.Close();
		return 0;
	}
	END_CATCH

	// do not close file
	return 1;
}


// ** EOF **


