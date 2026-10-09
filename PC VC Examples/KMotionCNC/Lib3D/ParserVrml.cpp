//********************************************
// ParserVrml.cpp
// class CParserVrml
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 02/04/98
// Modified : 02/04/98
//********************************************

#include "stdafx.h"

//********************************************
// Constructor
//********************************************
CParserVrml::CParserVrml()
{
	m_pBuffer = NULL;
	m_pBufferWord[0] = '\0';
	m_pBufferLine[0] = '\0';
	m_IndexBuffer = 0;
}

//********************************************
// Destructor
//********************************************
CParserVrml::~CParserVrml()
{
	Free();
}

//********************************************
// Destructor
//********************************************
void CParserVrml::Free(void)
{
	if(m_pBuffer != NULL)
		delete [] m_pBuffer;
	m_IndexBuffer = 0;
}

//********************************************
// Run
//********************************************
int CParserVrml::Run(wchar_t *filename,
										 CSceneGraph3d *pSceneGraph)
{
	TRACE(L"\n");
	TRACE(L"Start vrml parser\n");
	TRACE(L"  file : %ls\n",filename);

	// Free
	TRACE(L"  free...");
	Free();
	TRACE(L"ok\n");

	// ReadFile
	if(!ReadFile(filename))
		return 0;

	// CheckVersion
	if(!CheckVersion())
		return 0;

	CountDef();
	CountMesh();

	while(OffsetToStringBeginLine(L"DEF"))
		ReadMesh(pSceneGraph);

	TRACE(L"End vrml parser\n");
	TRACE(L"\n");
	return 1;
}


//********************************************
// ReadFile
//********************************************
int CParserVrml::ReadFile(wchar_t *filename)
{
	m_FileName = filename;

	CString s, data;

	CFileException ex;

	// Opening
	TRACE(L"  opening...");

	FILE* f;
	_tfopen_s(&f, filename, _T("rt,ccs=UTF-8"));


	if(!f)
	{
		#ifdef _DEBUG
		  afxDump << "File could not be opened " << ex.m_cause << "\n";
		#endif
		TRACE(L"unable to open file for reading\n");
		swprintf(s.GetBufferSetLength(201), L"Unable to open VRML File %ls", filename);
		MessageBox(NULL, s, L"KMotionCNC", MB_ICONSTOP | MB_OK | MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL);
		return 0;
	}
	TRACE(L"ok\n");

	while (!feof(f))
	{
		fgetws(s.GetBufferSetLength(301), 300, f);
		if (!feof(f))
		{
			data += s;
		}
	}
	fclose(f);

	m_SizeFile = data.GetLength();

	// Alloc
	TRACE(L"  alloc...");
	m_pBuffer = new wchar_t[m_SizeFile + 1];
	if(m_pBuffer == NULL)
	{
		TRACE(L"Insufficent memory\n");
		return 0;
	}
	TRACE(L"ok\n");

	wcscpy(m_pBuffer, data);
		
	return 1;
}

//********************************************
// ReadLine
// eol : '\n'
// eos : '\0'
//********************************************
int CParserVrml::ReadLine()
{
	m_pBufferLine[0] = '\0';
	int i=0;
	do
		m_pBufferLine[i++] = m_pBuffer[m_IndexBuffer++];
	while(m_pBuffer[m_IndexBuffer-1] != '\n' && 
		    i < MAX_LINE_VRML &&
				m_IndexBuffer < m_SizeFile);

	m_pBufferLine[i-1] = '\0';

	//TRACE(L"  line : %ls\n",m_pBufferLine);

	return 1;
}


//********************************************
// ReadLine
// eol : '\n'
// eos : '\0'
//********************************************
int CParserVrml::ReadWord()
{
	m_pBufferWord[0] = '\0';
	int i=0;

	// Jump to next valid character
	while((m_pBuffer[m_IndexBuffer] == '\n' || 
		     m_pBuffer[m_IndexBuffer] == '\t' || 
		     m_pBuffer[m_IndexBuffer] == '\r' || 
		     m_pBuffer[m_IndexBuffer] == ' ') &&
				 m_IndexBuffer < m_SizeFile)
		m_IndexBuffer++;

	// Check eof
	if(m_IndexBuffer >= m_SizeFile)
		return 0;

	do
		m_pBufferWord[i++] = m_pBuffer[m_IndexBuffer++];
	while(m_pBuffer[m_IndexBuffer-1] != '\n' && 
		    m_pBuffer[m_IndexBuffer-1] != '\t' && 
		    m_pBuffer[m_IndexBuffer-1] != '\r' && 
		    m_pBuffer[m_IndexBuffer-1] != ' ' && 
		    i < MAX_WORD_VRML &&
				m_IndexBuffer < m_SizeFile);

	m_pBufferWord[i-1] = '\0';

	//TRACE(L"  word : %ls\n",m_pBufferWord);

	return 1;
}




//********************************************
// CheckVersion
//********************************************
int CParserVrml::CheckVersion()
{
	ReadLine();
	TRACE(L"  check version (vrml 2.0)...");
	if(wcsstr(m_pBufferLine,L"#VRML V2.0") != NULL)
	{
		TRACE(L"ok\n");
		return 1;
	}
	TRACE(L"invalid\n");
	return 0;
}



//********************************************
// CountDef
//********************************************
void CParserVrml::CountDef(void)
{
	int tmp = m_IndexBuffer;
	int nb = 0;
	while(m_IndexBuffer < m_SizeFile)
	{
		ReadLine();
		if(wcsncmp(m_pBufferLine, L"DEF",3) == 0)
			nb++;
	}
	TRACE(L"  %d objects\n",nb);
	m_IndexBuffer = tmp;
}

//********************************************
// CountMesh
//********************************************
int CParserVrml::CountMesh(void)
{
	int tmp = m_IndexBuffer;
	int nb = 0;
	while(m_IndexBuffer < m_SizeFile)
	{
		ReadLine();
		if(wcsstr(m_pBufferLine, L"IndexedFaceSet") != NULL)
			nb++;
	}
	TRACE(L"  %d meshes\n",nb);
	m_IndexBuffer = tmp;
	return nb;
}

//********************************************
// OffsetToStringBeginLine
//********************************************
int CParserVrml::OffsetToStringBeginLine(wchar_t *string)
{
	while(m_IndexBuffer < m_SizeFile)
	{
		ReadLine();
		if(wcsncmp(m_pBufferLine,string,(int)wcslen(string)) == 0)
		{
			m_IndexBuffer -= (int)wcslen(m_pBufferLine)+1;
			/*
			TRACE(L"  begin line : %c%c%c%c%c...\n",m_pBuffer[m_IndexBuffer],
				                                     m_pBuffer[m_IndexBuffer+1],
				                                     m_pBuffer[m_IndexBuffer+2],
				                                     m_pBuffer[m_IndexBuffer+3],
																				     m_pBuffer[m_IndexBuffer+4]);*/
			return 1;
		}
	}
	return 0;
}

//********************************************
// OffsetToString
//********************************************
int CParserVrml::OffsetToString(wchar_t *string)
{
	while(m_IndexBuffer < m_SizeFile)
	{
		ReadLine();
		wchar_t *adr = wcsstr(m_pBufferLine,string);
		if(wcsstr(m_pBufferLine,string) != NULL)
		{
			m_IndexBuffer = m_IndexBuffer - (int)wcslen(m_pBufferLine) - 1 + (adr-m_pBufferLine);
			ASSERT(m_IndexBuffer >= 0);
			/*
			TRACE(L"  offset to string : %c%c%c%c%c... IndexBuffer : %d\n",m_pBuffer[m_IndexBuffer],
 				                                           m_pBuffer[m_IndexBuffer+1], 
				                                           m_pBuffer[m_IndexBuffer+2],
				                                           m_pBuffer[m_IndexBuffer+3],
																				           m_pBuffer[m_IndexBuffer+4],m_IndexBuffer);*/

			return 1;
		}
	}
	return 0;
}

//********************************************
// OffsetToString
//********************************************
int CParserVrml::OffsetToStringBefore(wchar_t *string,
																			wchar_t *before)
{
	while(m_IndexBuffer < m_SizeFile)
	{
		ReadLine();
		wchar_t *adr = wcsstr(m_pBufferLine,string);
		if(wcsstr(m_pBufferLine,before) != NULL)
			return 0;
		if(wcsstr(m_pBufferLine,string) != NULL)
		{
			m_IndexBuffer = m_IndexBuffer - (int)wcslen(m_pBufferLine) - 1 + (adr-m_pBufferLine);
			ASSERT(m_IndexBuffer >= 0);
			/*
			TRACE(L"  offset to string : %c%c%c%c%c... IndexBuffer : %d\n",m_pBuffer[m_IndexBuffer],
 				                                           m_pBuffer[m_IndexBuffer+1], 
				                                           m_pBuffer[m_IndexBuffer+2],
				                                           m_pBuffer[m_IndexBuffer+3],
																				           m_pBuffer[m_IndexBuffer+4],m_IndexBuffer);*/

			return 1;
		}
	}
	return 0;
}

//********************************************
// CheckMesh
//********************************************
int CParserVrml::CheckMesh()
{
	// Find a possible mesh
	TRACE(L"  check mesh...");
	OffsetToStringBeginLine(L"DEF");
	int tmp = m_IndexBuffer;
	ReadLine();
	if(wcsstr(m_pBufferLine, L"DEF") != NULL && 
	   wcsstr(m_pBufferLine, L"Transform") != NULL)
	{
		m_IndexBuffer = tmp;
		if(OffsetToString(L"Transform") &&
		   OffsetToString(L"Material") &&
		   OffsetToString(L"IndexedFaceSet"))
		{
			m_IndexBuffer = tmp;
			TRACE(L"ok\n");
			return 1;
		}
	}
	//m_IndexBuffer = tmp;
	TRACE(L"not a mesh\n");
	return 0;
}

//********************************************
// ReadMesh
//********************************************
int CParserVrml::ReadMesh(CSceneGraph3d *pSceneGraph)
{
	// Check
	if(!CheckMesh())
		return 0;

	int tmp = m_IndexBuffer;
	ReadLine();
	ASSERT(wcsstr(m_pBufferLine, L"DEF") != NULL);
	ASSERT(wcsstr(m_pBufferLine, L"Transform") != NULL);
	// DEF [name] Transform {
	if(swscanf(m_pBufferLine, L"DEF %ls Transform",m_pBufferWord) != 1)
	{
		TRACE(L"  invalid syntax (BufferLine : %ls BufferWord : %ls\n",m_pBufferLine,m_pBufferWord);
		return 0;
	}

	TRACE(L"  start reading mesh %ls\n",m_pBufferWord);

	m_IndexBuffer = tmp;

	int IndexTexture = -1;

	// Transform
	//********************************************
	// Syntax :
	// Transform {
  // translation -360.7 1370 3471
  // rotation 0.3236 -0.3236 -0.8891 -1.688
  // scale -49.36 -49.36 -49.36
  // scaleOrientation -0.689 0.4766 -0.546 -0.6007

	OffsetToString(L"Transform");
	ReadLine(); // Transform
	CTransform transform;
	transform.Clear();

	// Translation
	tmp = m_IndexBuffer;
	ReadLine();
	if(wcsstr(m_pBufferLine, L"translation") != NULL)
	{
		// Come back
		m_IndexBuffer = tmp;
		// Jump after "translation"
		ReadWord(); 

		float x,y,z;
		ReadWord();
		bool success = swscanf(m_pBufferWord, L"%f",&x) == 1;
		ReadWord();
		success = success && swscanf(m_pBufferWord, L"%f",&y) == 1;
		ReadWord();
		success = success && swscanf(m_pBufferWord, L"%f",&z) == 1;
		if(success)
		{
			transform.SetTranslation(CVector3d(x,y,z));
			TRACE(L"    translation : %g %g %g\n",x,y,z);
		}
		ReadLine();
		tmp = m_IndexBuffer;
		ReadLine();
	}

	// Rotation
	if(wcsstr(m_pBufferLine, L"rotation") != NULL)
	{
		// Come back
		m_IndexBuffer = tmp;
		// Jump after "rotation"
		ReadWord(); 

		float x,y,z,value;
		ReadWord();
		bool success = swscanf(m_pBufferWord, L"%f",&x) == 1;
		ReadWord();
		success = success && swscanf(m_pBufferWord, L"%f",&y) == 1;
		ReadWord();
		success = success && swscanf(m_pBufferWord, L"%f",&z) == 1;
		ReadWord();
		success = success && swscanf(m_pBufferWord, L"%f",&value) == 1;
		if(success)
		{
			transform.SetRotation(CVector3d(x,y,z));
			transform.SetValueRotation(value/3.1415926f*180.0f);
			TRACE(L"    rotation : %g %g %g %g\n",x,y,z,value);
		}
		ReadLine();
		tmp = m_IndexBuffer;
		ReadLine();
	}

	// Scale
	if(wcsstr(m_pBufferLine, L"scale") != NULL)
	{
		// Come back
		m_IndexBuffer = tmp;
		// Jump after "scale"
		ReadWord(); 

		float x,y,z;
		ReadWord();
		bool success = swscanf(m_pBufferWord, L"%f",&x) == 1;
		ReadWord();
		success = success && swscanf(m_pBufferWord, L"%f",&y) == 1;
		ReadWord();
		success = success && swscanf(m_pBufferWord, L"%f",&z) == 1;
		if(success)
		{
			transform.SetScale(CVector3d(x,y,z));
			TRACE(L"    scale : %g %g %g\n",x,y,z);
		}
		ReadLine();
		tmp = m_IndexBuffer;
		ReadLine();
	}

	// ScaleOrientation
	if(wcsstr(m_pBufferLine, L"scaleOrientation") != NULL)
	{
		// Come back
		m_IndexBuffer = tmp;
		// Jump after "scaleOrientation"
		ReadWord(); 

		float x,y,z,value;
		ReadWord();
		bool success = swscanf(m_pBufferWord, L"%f",&x) == 1;
		ReadWord();
		success = success && swscanf(m_pBufferWord, L"%f",&y) == 1;
		ReadWord();
		success = success && swscanf(m_pBufferWord, L"%f",&z) == 1;
		ReadWord();
		success = success && swscanf(m_pBufferWord, L"%f",&value) == 1;
		if(success)
		{
			//transform.SetScale(CVector3d(x,y,z));
			TRACE(L"    scaleOrientation : %g %g %g %g\n",x,y,z,value);
		}
		ReadLine();
	}

	// Material
	//********************************************
	// appearance Appearance {
  // material Material {
  // diffuseColor 0.5686 0.1098 0.6941
	CMaterial material;
	if(OffsetToString(L"Material"))
	{
		ReadLine();

		tmp = m_IndexBuffer;

		// Diffuse color
		ReadLine(); 
		if(wcsstr(m_pBufferLine, L"diffuseColor") != NULL)
		{
			// Come back
			m_IndexBuffer = tmp;

			// Jump
			ReadWord();

			float r,g,b;
			ReadWord();
			bool success = swscanf(m_pBufferWord, L"%f",&r) == 1;
			ReadWord();
			success = success && swscanf(m_pBufferWord, L"%f",&g) == 1;
			ReadWord();
			success = success && swscanf(m_pBufferWord, L"%f",&b) == 1;
			if(success)
			{
				material.SetDiffuse(r,g,b,1.0f);
				TRACE(L"    diffuseColor : %g %g %g\n",r,g,b);
			}
		}
	}

	// Texture
	//********************************************
	int texture = 0;
	if(OffsetToStringBefore(L"texture ImageTexture", L"geometry"))
	{
		texture = 1;
		ReadLine();
		tmp = m_IndexBuffer;

		ReadLine(); 
		if(wcsstr(m_pBufferLine, L"url") != NULL)
		{
			// Come back
			m_IndexBuffer = tmp;

			// Jump
			ReadWord();
			wchar_t string[MAX_PATH];
			ReadWord();
			bool success = swscanf(m_pBufferWord, L"%ls",string) == 1;

			// Remove ""
			CString TextureName = string;
			TextureName = TextureName.Mid(1,TextureName.GetLength()-2);
			TRACE(L"    texture : %ls\n",TextureName);

			// Ask SceneGraph to add texture, if needed
			wchar_t *name = TextureName.GetBuffer(MAX_PATH);
			if(!pSceneGraph->HasTexture(name,&IndexTexture))
			{
				CTexture *pTexture = new CTexture;
				pTexture->ReadFile(name);
				IndexTexture = pSceneGraph->AddTexture(pTexture);
			}
			TextureName.ReleaseBuffer();
		}
	}
	else // come back
		m_IndexBuffer = tmp;

	// Mesh
	//********************************************
	int NbVertex,NbFace,NbTextureCoordinate;
	// Count size (do not offset in file)
	if(!SizeMesh(&NbVertex,&NbFace,texture,&NbTextureCoordinate))
		return 0;

	// Add mesh
	CMesh3d *pMesh = new CMesh3d;
	pSceneGraph->Add(pMesh);
	// Set Size (faster)
	pMesh->m_ArrayVertex.SetSize(NbVertex);
	pMesh->m_ArrayFace.SetSize(NbFace);
	if(texture)
	{
		pMesh->m_pTextureCoordinate = new float[NbTextureCoordinate*2]; // x y 
		pMesh->m_pTextureCoordinateIndex = new int[NbFace*3];           // triangular faces
		pMesh->m_IndexTexture = IndexTexture;
	}
	// Store mesh (offset in file)
	StoreMesh(&pMesh->m_ArrayVertex,&pMesh->m_ArrayFace,texture,
		pMesh->m_pTextureCoordinate,pMesh->m_pTextureCoordinateIndex);

	// Transform & material
	pMesh->SetTransform(transform);
	pMesh->SetMaterial(&material);

	TRACE(L"  end reading mesh\n");

	return 1;
}

//********************************************
// SizeMesh
//********************************************
int CParserVrml::SizeMesh(int *pNbVertex,
													int *pNbFace,
													int HasTexture,
													int *pNbTextureCoordinate /* = NULL */)
{
	TRACE(L"    size mesh...");
	int tmp = m_IndexBuffer;

	ASSERT(pNbVertex != NULL);
	ASSERT(pNbFace != NULL);

	if(!OffsetToString(L"IndexedFaceSet"))
	{
		TRACE(L"invalid mesh\n");
		return 0;
	}

	// Count points
	//***********************************************
	if(!OffsetToString(L"Coordinate { point ["))
	{
		TRACE(L"invalid mesh\n");
		return 0;
	}

	m_IndexBuffer += (int)wcslen(L"Coordinate { point [") + 1;

	// Cur : x y z,
	// End : x y z]
	int NbVertex = 0;
	bool success;
	do
	{
		float x,y,z;
		ReadWord();
		success = swscanf(m_pBufferWord, L"%f",&x) == 1;
		ReadWord();
		success = success && swscanf(m_pBufferWord, L"%f",&y) == 1;
		ReadWord();
		success = success && swscanf(m_pBufferWord, L"%f",&z) == 1;
		NbVertex += success;
		//TRACE(L"\n (%g %g %g) ",x,y,z);
	}
	while(success);
	TRACE(L" %d points, L",NbVertex);

	if(NbVertex <= 0)
		return 0;


	// Count texture coordinates, if needed 
	//***********************************************
	int NbTextureCoordinate = 0;
	if(HasTexture)
	{
		if(!OffsetToString(L"TextureCoordinate { point ["))
		{
			TRACE(L"invalid texture coordinates\n");
			return 0;
		}

		m_IndexBuffer += (int)wcslen(L"TextureCoordinate { point [") + 1;

		// Cur : x y,
		// End : x y]
		bool success;
		do
		{
			float x,y;
			ReadWord();
			success = swscanf(m_pBufferWord, L"%f",&x) == 1;
			ReadWord();
			success = success && swscanf(m_pBufferWord, L"%f",&y) == 1;
			NbTextureCoordinate += success;
			//TRACE(L"\n (%g %g %g) ",x,y,z);
		}
		while(success);
		TRACE(L" %d texture coordinates,",NbTextureCoordinate);

		if(NbTextureCoordinate <= 0)
			return 0;
	}


	// Count faces, accept only triangles
	//***********************************************
	m_IndexBuffer = tmp;
	if(!OffsetToString(L"coordIndex ["))
	{
		TRACE(L"invalid mesh\n");
		return 0;
	}
	m_IndexBuffer += (int)wcslen(L"coordIndex [") + 1;

	// Cur : int, int, int, -1,
	// End : int, int, int, -1]
	int NbFace = 0;
	do
	{
		int v1,v2,v3;
		ReadWord();
		success  = swscanf(m_pBufferWord, L"%d,",&v1) == 1;
		ReadWord();
		success = success && swscanf(m_pBufferWord, L"%d,",&v2) == 1;
		ReadWord();
		success = success && swscanf(m_pBufferWord, L"%d,",&v3) == 1;
		NbFace += success;

		ASSERT(v1 >= 0);
		ASSERT(v2 >= 0);
		ASSERT(v3 >= 0);

		int test;
		ReadWord();
		swscanf(m_pBufferWord, L"%d",&test);
		if(wcsstr(m_pBufferWord, L"]") != NULL)
			success = 0;
	}
	while(success);
	TRACE(L" %d faces,",NbFace);

	if(NbFace <= 0)
		return 0;

	// Count texture coordinate index 
	//***********************************************
	if(HasTexture)
	{
		m_IndexBuffer = tmp;
		if(!OffsetToString(L"texCoordIndex ["))
		{
			TRACE(L"invalid texture coordinate index\n");
			return 0;
		}
		m_IndexBuffer += (int)wcslen(L"texCoordIndex [") + 1;

		// Cur : int, int, int, -1,
		// End : int, int, int, -1]
		int NbCoordIndex = 0;
		do
		{
			int v1,v2,v3;
			ReadWord();
			success  = swscanf(m_pBufferWord, L"%d,",&v1) == 1;
			ReadWord();
			success = success && swscanf(m_pBufferWord, L"%d,",&v2) == 1;
			ReadWord();
			success = success && swscanf(m_pBufferWord, L"%d,",&v3) == 1;
			NbCoordIndex += success;

			ASSERT(v1 >= 0);
			ASSERT(v2 >= 0);
			ASSERT(v3 >= 0);

			int test;
			ReadWord();
			swscanf(m_pBufferWord, L"%d",&test);
			if(wcsstr(m_pBufferWord, L"]") != NULL)
				success = 0;
		}
		while(success);
		TRACE(L" %d coordinate index\n",NbCoordIndex);

		if(NbFace != NbCoordIndex)
		{
			TRACE(L" different values for coord index and faces\n");
			return 0;
		}
	}

	// Store result
	*pNbVertex = NbVertex;
	*pNbFace = NbFace;
	if(HasTexture)
		*pNbTextureCoordinate = NbTextureCoordinate;

	m_IndexBuffer = tmp;

	return 1;
}


//********************************************
// StoreMesh
//********************************************
int CParserVrml::StoreMesh(CArray3d<CVertex3d> *pArrayVertex,
													 CArray3d<CFace3d> *pArrayFace,
													 int HasTexture,
													 float *pTextureCoordinate,
													 int *pTextureCoordinateIndex)
{
	TRACE(L"    store mesh...");
	int tmp = m_IndexBuffer;

	if(!OffsetToString(L"IndexedFaceSet"))
	{
		TRACE(L"invalid mesh\n");
		return 0;
	}

	// Store vertices
	//***********************************************
	if(!OffsetToString(L"Coordinate { point ["))
	{
		TRACE(L"invalid mesh\n");
		return 0;
	}
	m_IndexBuffer += (int)wcslen(L"Coordinate { point [") + 1;
	// Cur : x y z,
	// End : x y z]
	bool success;
	int NbVertex = 0;
	do
	{
		float x,y,z;
		ReadWord();
		success = swscanf(m_pBufferWord, L"%f",&x) == 1;
		ReadWord();
		success = success && swscanf(m_pBufferWord, L"%f",&y) == 1;
		ReadWord();
		success = success && swscanf(m_pBufferWord, L"%f",&z) == 1;
		if(success)
			pArrayVertex->SetAt(NbVertex++,new CVertex3d(x,y,z));
		//TRACE(L"\n (%g %g %g) ",x,y,z);
	}
	while(success);
	TRACE(L" added %d vertices,",NbVertex);

	// Store texture coordinates (if needed)
	//***********************************************
	if(HasTexture)
	{
		if(!OffsetToString(L"TextureCoordinate { point ["))
		{
			TRACE(L"invalid texture coordinate\n");
			return 0;
		}
		m_IndexBuffer += (int)wcslen(L"TextureCoordinate { point [") + 1;
		// Cur : x y,
		// End : x y
		bool success;
		int NbTextureCoordinate = 0;
		do
		{
			float x,y;
			ReadWord();
			success = swscanf(m_pBufferWord, L"%f",&x) == 1;
			ReadWord();
			success = success && swscanf(m_pBufferWord, L"%f",&y) == 1;
			if(success)
			{
				pTextureCoordinate[2*NbTextureCoordinate] = x;
				pTextureCoordinate[2*NbTextureCoordinate+1] = y;
				NbTextureCoordinate++;
			}
			//TRACE(L"\n (%g %g) ",x,y);
		}
		while(success);
		TRACE(L" added %d texture coordinates,",NbTextureCoordinate);
	}

	

	// Store faces, accept only triangles
	//***********************************************
	m_IndexBuffer = tmp;
	if(!OffsetToString(L"coordIndex ["))
	{
		TRACE(L"invalid mesh\n");
		return 0;
	}
	m_IndexBuffer += (int)wcslen(L"coordIndex [") + 1;

	// Cur : int, int, int, -1,
	// End : int, int, int, -1]
	int NbFace = 0;
	do
	{
		int v1,v2,v3;
		ReadWord();
		success  = swscanf(m_pBufferWord, L"%d,",&v1) == 1;
		ReadWord();
		success = success && swscanf(m_pBufferWord, L"%d,",&v2) == 1;
		ReadWord();
		success = success && swscanf(m_pBufferWord, L"%d,",&v3) == 1;

		ASSERT(v1 >= 0);
		ASSERT(v2 >= 0);
		ASSERT(v3 >= 0);


		if(success && v1 >= 0 && v2 >= 0 && v3 >= 0)
		{
			CFace3d *pFace = new CFace3d(pArrayVertex->GetAt(v1),
				                           pArrayVertex->GetAt(v2),
																	 pArrayVertex->GetAt(v3));
			pArrayFace->SetAt(NbFace++,pFace);
		}

		int test;
		ReadWord();
		swscanf(m_pBufferWord, L"%d",&test);
		if(wcsstr(m_pBufferWord, L"]") != NULL)
			success = 0;

	}
	while(success);
	TRACE(L" added %d faces\n",NbFace);

	// Store texture coord index
	//***********************************************
	if(HasTexture)
	{
		m_IndexBuffer = tmp;
		if(!OffsetToString(L"texCoordIndex ["))
		{
			TRACE(L"invalid mesh\n");
			return 0;
		}
		m_IndexBuffer += (int)wcslen(L"texCoordIndex [") + 1;

		// Cur : int, int, int, -1,
		// End : int, int, int, -1]
		int NbTexCoordIndex = 0;
		do
		{
			int v1,v2,v3;
			ReadWord();
			success  = swscanf(m_pBufferWord, L"%d,",&v1) == 1;
			ReadWord();
			success = success && swscanf(m_pBufferWord, L"%d,",&v2) == 1;
			ReadWord();
			success = success && swscanf(m_pBufferWord, L"%d,",&v3) == 1;

			ASSERT(v1 >= 0);
			ASSERT(v2 >= 0);
			ASSERT(v3 >= 0);

			if(success && v1 >= 0 && v2 >= 0 && v3 >= 0)
			{
				pTextureCoordinateIndex[3*NbTexCoordIndex] = v1;
				pTextureCoordinateIndex[3*NbTexCoordIndex+1] = v2;
				pTextureCoordinateIndex[3*NbTexCoordIndex+2] = v3;
				NbTexCoordIndex++;
			}

			int test;
			ReadWord();
			swscanf(m_pBufferWord, L"%d",&test);
			if(wcsstr(m_pBufferWord, L"]") != NULL)
				success = 0;

		}
		while(success);
		TRACE(L" added %d texture coordinate index\n",NbTexCoordIndex);
	}

	return 1;
}

// ** EOF **





