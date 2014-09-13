//********************************************
// ParserVrml.cpp
// class CParserVrml
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 02/04/98
// Modified : 02/04/98
//********************************************

#include "..\stdafx.h"

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
int CParserVrml::Run(char *filename,
										 CSceneGraph3d *pSceneGraph)
{
	TRACE("\n");
	TRACE("Start vrml parser\n");
	TRACE("  file : %s\n",filename);

	// Free
	TRACE("  free...");
	Free();
	TRACE("ok\n");

	// ReadFile
	if(!ReadFile(filename))
		return 0;

	// CheckVersion
	if(!CheckVersion())
		return 0;

	CountDef();
	CountMesh();

	while(OffsetToStringBeginLine("DEF"))
		ReadMesh(pSceneGraph);

	TRACE("End vrml parser\n");
	TRACE("\n");
	return 1;
}


//********************************************
// ReadFile
//********************************************
int CParserVrml::ReadFile(char *filename)
{
	m_FileName = filename;

	CFile file;
	CFileException ex;

	// Opening
	TRACE("  opening...");
	if(!file.Open(filename, CFile::modeRead | CFile::typeBinary,&ex))
	{
		#ifdef _DEBUG
		  afxDump << "File could not be opened " << ex.m_cause << "\n";
		#endif
		TRACE("unable to open file for reading\n");
		return 0;
	}
	TRACE("ok\n");

	// Size file
	m_SizeFile = file.GetLength();
	TRACE("  length : %d bytes\n",m_SizeFile);

	// Alloc
	TRACE("  alloc...");
	m_pBuffer = new char[m_SizeFile];
	if(m_pBuffer == NULL)
	{
		TRACE("insuffisant memory\n");
		return 0;
	}
	TRACE("ok\n");

	// Reading
	TRACE("  reading...");
	TRY
	{
		file.Read(m_pBuffer,m_SizeFile);
	}
	CATCH(CFileException, e)
	{
		#ifdef _DEBUG
				afxDump << "Error during reading " << e->m_cause << "\n";
		#endif
		TRACE("error during reading\n");
		file.Close();
		return 0;
	}
	END_CATCH
	TRACE("ok\n");

	// Closing
	file.Close();

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

	//TRACE("  line : %s\n",m_pBufferLine);

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

	//TRACE("  word : %s\n",m_pBufferWord);

	return 1;
}




//********************************************
// CheckVersion
//********************************************
int CParserVrml::CheckVersion()
{
	ReadLine();
	TRACE("  check version (vrml 2.0)...");
	if(strstr(m_pBufferLine,"#VRML V2.0") != NULL)
	{
		TRACE("ok\n");
		return 1;
	}
	TRACE("invalid\n");
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
		if(strncmp(m_pBufferLine,"DEF",3) == 0)
			nb++;
	}
	TRACE("  %d objects\n",nb);
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
		if(strstr(m_pBufferLine,"IndexedFaceSet") != NULL)
			nb++;
	}
	TRACE("  %d meshes\n",nb);
	m_IndexBuffer = tmp;
	return nb;
}

//********************************************
// OffsetToStringBeginLine
//********************************************
int CParserVrml::OffsetToStringBeginLine(char *string)
{
	while(m_IndexBuffer < m_SizeFile)
	{
		ReadLine();
		if(strncmp(m_pBufferLine,string,strlen(string)) == 0)
		{
			m_IndexBuffer -= strlen(m_pBufferLine)+1;
			/*
			TRACE("  begin line : %c%c%c%c%c...\n",m_pBuffer[m_IndexBuffer],
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
int CParserVrml::OffsetToString(char *string)
{
	while(m_IndexBuffer < m_SizeFile)
	{
		ReadLine();
		char *adr = strstr(m_pBufferLine,string);
		if(strstr(m_pBufferLine,string) != NULL)
		{
			m_IndexBuffer = m_IndexBuffer - strlen(m_pBufferLine) - 1 + (adr-m_pBufferLine);
			ASSERT(m_IndexBuffer >= 0);
			/*
			TRACE("  offset to string : %c%c%c%c%c... IndexBuffer : %d\n",m_pBuffer[m_IndexBuffer],
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
int CParserVrml::OffsetToStringBefore(char *string,
																			char *before)
{
	while(m_IndexBuffer < m_SizeFile)
	{
		ReadLine();
		char *adr = strstr(m_pBufferLine,string);
		if(strstr(m_pBufferLine,before) != NULL)
			return 0;
		if(strstr(m_pBufferLine,string) != NULL)
		{
			m_IndexBuffer = m_IndexBuffer - strlen(m_pBufferLine) - 1 + (adr-m_pBufferLine);
			ASSERT(m_IndexBuffer >= 0);
			/*
			TRACE("  offset to string : %c%c%c%c%c... IndexBuffer : %d\n",m_pBuffer[m_IndexBuffer],
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
	TRACE("  check mesh...");
	OffsetToStringBeginLine("DEF");
	int tmp = m_IndexBuffer;
	ReadLine();
	if(strstr(m_pBufferLine,"DEF") != NULL && 
	   strstr(m_pBufferLine,"Transform") != NULL)
	{
		m_IndexBuffer = tmp;
		if(OffsetToString("Transform") &&
		   OffsetToString("Material") &&
		   OffsetToString("IndexedFaceSet"))
		{
			m_IndexBuffer = tmp;
			TRACE("ok\n");
			return 1;
		}
	}
	//m_IndexBuffer = tmp;
	TRACE("not a mesh\n");
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
	ASSERT(strstr(m_pBufferLine,"DEF") != NULL);
	ASSERT(strstr(m_pBufferLine,"Transform") != NULL);
	// DEF [name] Transform {
	if(sscanf(m_pBufferLine,"DEF %s Transform",m_pBufferWord) != 1)
	{
		TRACE("  invalid syntax (BufferLine : %s BufferWord : %s\n",m_pBufferLine,m_pBufferWord);
		return 0;
	}

	TRACE("  start reading mesh %s\n",m_pBufferWord);

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

	OffsetToString("Transform");
	ReadLine(); // Transform
	CTransform transform;
	transform.Clear();

	// Translation
	tmp = m_IndexBuffer;
	ReadLine();
	if(strstr(m_pBufferLine,"translation") != NULL)
	{
		// Come back
		m_IndexBuffer = tmp;
		// Jump after "translation"
		ReadWord(); 

		float x,y,z;
		ReadWord();
		int success = sscanf(m_pBufferWord,"%f",&x);
		ReadWord();
		success &= sscanf(m_pBufferWord,"%f",&y);
		ReadWord();
		success &= sscanf(m_pBufferWord,"%f",&z);
		if(success)
		{
			transform.SetTranslation(CVector3d(x,y,z));
			TRACE("    translation : %g %g %g\n",x,y,z);
		}
		ReadLine();
		tmp = m_IndexBuffer;
		ReadLine();
	}

	// Rotation
	if(strstr(m_pBufferLine,"rotation") != NULL)
	{
		// Come back
		m_IndexBuffer = tmp;
		// Jump after "rotation"
		ReadWord(); 

		float x,y,z,value;
		ReadWord();
		int success = sscanf(m_pBufferWord,"%f",&x);
		ReadWord();
		success &= sscanf(m_pBufferWord,"%f",&y);
		ReadWord();
		success &= sscanf(m_pBufferWord,"%f",&z);
		ReadWord();
		success &= sscanf(m_pBufferWord,"%f",&value);
		if(success)
		{
			transform.SetRotation(CVector3d(x,y,z));
			transform.SetValueRotation(value/3.1415926f*180.0f);
			TRACE("    rotation : %g %g %g %g\n",x,y,z,value);
		}
		ReadLine();
		tmp = m_IndexBuffer;
		ReadLine();
	}

	// Scale
	if(strstr(m_pBufferLine,"scale") != NULL)
	{
		// Come back
		m_IndexBuffer = tmp;
		// Jump after "scale"
		ReadWord(); 

		float x,y,z;
		ReadWord();
		int success = sscanf(m_pBufferWord,"%f",&x);
		ReadWord();
		success &= sscanf(m_pBufferWord,"%f",&y);
		ReadWord();
		success &= sscanf(m_pBufferWord,"%f",&z);
		if(success)
		{
			transform.SetScale(CVector3d(x,y,z));
			TRACE("    scale : %g %g %g\n",x,y,z);
		}
		ReadLine();
		tmp = m_IndexBuffer;
		ReadLine();
	}

	// ScaleOrientation
	if(strstr(m_pBufferLine,"scaleOrientation") != NULL)
	{
		// Come back
		m_IndexBuffer = tmp;
		// Jump after "scaleOrientation"
		ReadWord(); 

		float x,y,z,value;
		ReadWord();
		int success = sscanf(m_pBufferWord,"%f",&x);
		ReadWord();
		success &= sscanf(m_pBufferWord,"%f",&y);
		ReadWord();
		success &= sscanf(m_pBufferWord,"%f",&z);
		ReadWord();
		success &= sscanf(m_pBufferWord,"%f",&value);
		if(success)
		{
			//transform.SetScale(CVector3d(x,y,z));
			TRACE("    scaleOrientation : %g %g %g %g\n",x,y,z,value);
		}
		ReadLine();
	}

	// Material
	//********************************************
	// appearance Appearance {
  // material Material {
  // diffuseColor 0.5686 0.1098 0.6941
	CMaterial material;
	if(OffsetToString("Material"))
	{
		ReadLine();

		tmp = m_IndexBuffer;

		// Diffuse color
		ReadLine(); 
		if(strstr(m_pBufferLine,"diffuseColor") != NULL)
		{
			// Come back
			m_IndexBuffer = tmp;

			// Jump
			ReadWord();

			float r,g,b;
			ReadWord();
			int success = sscanf(m_pBufferWord,"%f",&r);
			ReadWord();
			success &= sscanf(m_pBufferWord,"%f",&g);
			ReadWord();
			success &= sscanf(m_pBufferWord,"%f",&b);
			if(success)
			{
				material.SetDiffuse(r,g,b,1.0f);
				TRACE("    diffuseColor : %g %g %g\n",r,g,b);
			}
		}
	}

	// Texture
	//********************************************
	int texture = 0;
	if(OffsetToStringBefore("texture ImageTexture","geometry"))
	{
		texture = 1;
		ReadLine();
		tmp = m_IndexBuffer;

		ReadLine(); 
		if(strstr(m_pBufferLine,"url") != NULL)
		{
			// Come back
			m_IndexBuffer = tmp;

			// Jump
			ReadWord();
			char string[MAX_PATH];
			ReadWord();
			int success = sscanf(m_pBufferWord,"%s",string);

			// Remove ""
			CString TextureName = string;
			TextureName = TextureName.Mid(1,TextureName.GetLength()-2);
			TRACE("    texture : %s\n",TextureName);

			// Ask SceneGraph to add texture, if needed
			char *name = TextureName.GetBuffer(MAX_PATH);
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

	TRACE("  end reading mesh\n");

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
	TRACE("    size mesh...");
	int tmp = m_IndexBuffer;

	ASSERT(pNbVertex != NULL);
	ASSERT(pNbFace != NULL);

	if(!OffsetToString("IndexedFaceSet"))
	{
		TRACE("invalid mesh\n");
		return 0;
	}

	// Count points
	//***********************************************
	if(!OffsetToString("Coordinate { point ["))
	{
		TRACE("invalid mesh\n");
		return 0;
	}

	m_IndexBuffer += strlen("Coordinate { point [") + 1;

	// Cur : x y z,
	// End : x y z]
	int NbVertex = 0;
	int success;
	do
	{
		float x,y,z;
		ReadWord();
		success = sscanf(m_pBufferWord,"%f",&x);
		ReadWord();
		success &= sscanf(m_pBufferWord,"%f",&y);
		ReadWord();
		success &= sscanf(m_pBufferWord,"%f",&z);
		NbVertex += success;
		//TRACE("\n (%g %g %g) ",x,y,z);
	}
	while(success);
	TRACE(" %d points,",NbVertex);

	if(NbVertex <= 0)
		return 0;


	// Count texture coordinates, if needed 
	//***********************************************
	int NbTextureCoordinate = 0;
	if(HasTexture)
	{
		if(!OffsetToString("TextureCoordinate { point ["))
		{
			TRACE("invalid texture coordinates\n");
			return 0;
		}

		m_IndexBuffer += strlen("TextureCoordinate { point [") + 1;

		// Cur : x y,
		// End : x y]
		int success;
		do
		{
			float x,y;
			ReadWord();
			success = sscanf(m_pBufferWord,"%f",&x);
			ReadWord();
			success &= sscanf(m_pBufferWord,"%f",&y);
			NbTextureCoordinate += success;
			//TRACE("\n (%g %g %g) ",x,y,z);
		}
		while(success);
		TRACE(" %d texture coordinates,",NbTextureCoordinate);

		if(NbTextureCoordinate <= 0)
			return 0;
	}


	// Count faces, accept only triangles
	//***********************************************
	m_IndexBuffer = tmp;
	if(!OffsetToString("coordIndex ["))
	{
		TRACE("invalid mesh\n");
		return 0;
	}
	m_IndexBuffer += strlen("coordIndex [") + 1;

	// Cur : int, int, int, -1,
	// End : int, int, int, -1]
	int NbFace = 0;
	do
	{
		int v1,v2,v3;
		ReadWord();
		success  = sscanf(m_pBufferWord,"%d,",&v1);
		ReadWord();
		success &= sscanf(m_pBufferWord,"%d,",&v2);
		ReadWord();
		success &= sscanf(m_pBufferWord,"%d,",&v3);
		NbFace += success;

		ASSERT(v1 >= 0);
		ASSERT(v2 >= 0);
		ASSERT(v3 >= 0);

		int test;
		ReadWord();
		sscanf(m_pBufferWord,"%d",&test);
		if(strstr(m_pBufferWord,"]") != NULL)
			success = 0;
	}
	while(success);
	TRACE(" %d faces,",NbFace);

	if(NbFace <= 0)
		return 0;

	// Count texture coordinate index 
	//***********************************************
	if(HasTexture)
	{
		m_IndexBuffer = tmp;
		if(!OffsetToString("texCoordIndex ["))
		{
			TRACE("invalid texture coordinate index\n");
			return 0;
		}
		m_IndexBuffer += strlen("texCoordIndex [") + 1;

		// Cur : int, int, int, -1,
		// End : int, int, int, -1]
		int NbCoordIndex = 0;
		do
		{
			int v1,v2,v3;
			ReadWord();
			success  = sscanf(m_pBufferWord,"%d,",&v1);
			ReadWord();
			success &= sscanf(m_pBufferWord,"%d,",&v2);
			ReadWord();
			success &= sscanf(m_pBufferWord,"%d,",&v3);
			NbCoordIndex += success;

			ASSERT(v1 >= 0);
			ASSERT(v2 >= 0);
			ASSERT(v3 >= 0);

			int test;
			ReadWord();
			sscanf(m_pBufferWord,"%d",&test);
			if(strstr(m_pBufferWord,"]") != NULL)
				success = 0;
		}
		while(success);
		TRACE(" %d coordinate index\n",NbCoordIndex);

		if(NbFace != NbCoordIndex)
		{
			TRACE(" different values for coord index and faces\n");
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
	TRACE("    store mesh...");
	int tmp = m_IndexBuffer;

	if(!OffsetToString("IndexedFaceSet"))
	{
		TRACE("invalid mesh\n");
		return 0;
	}

	// Store vertices
	//***********************************************
	if(!OffsetToString("Coordinate { point ["))
	{
		TRACE("invalid mesh\n");
		return 0;
	}
	m_IndexBuffer += strlen("Coordinate { point [") + 1;
	// Cur : x y z,
	// End : x y z]
	int success;
	int NbVertex = 0;
	do
	{
		float x,y,z;
		ReadWord();
		success = sscanf(m_pBufferWord,"%f",&x);
		ReadWord();
		success &= sscanf(m_pBufferWord,"%f",&y);
		ReadWord();
		success &= sscanf(m_pBufferWord,"%f",&z);
		if(success)
			pArrayVertex->SetAt(NbVertex++,new CVertex3d(x,y,z));
		//TRACE("\n (%g %g %g) ",x,y,z);
	}
	while(success);
	TRACE(" added %d vertices,",NbVertex);

	// Store texture coordinates (if needed)
	//***********************************************
	if(HasTexture)
	{
		if(!OffsetToString("TextureCoordinate { point ["))
		{
			TRACE("invalid texture coordinate\n");
			return 0;
		}
		m_IndexBuffer += strlen("TextureCoordinate { point [") + 1;
		// Cur : x y,
		// End : x y
		int success;
		int NbTextureCoordinate = 0;
		do
		{
			float x,y;
			ReadWord();
			success = sscanf(m_pBufferWord,"%f",&x);
			ReadWord();
			success &= sscanf(m_pBufferWord,"%f",&y);
			if(success)
			{
				pTextureCoordinate[2*NbTextureCoordinate] = x;
				pTextureCoordinate[2*NbTextureCoordinate+1] = y;
				NbTextureCoordinate++;
			}
			//TRACE("\n (%g %g) ",x,y);
		}
		while(success);
		TRACE(" added %d texture coordinates,",NbTextureCoordinate);
	}

	

	// Store faces, accept only triangles
	//***********************************************
	m_IndexBuffer = tmp;
	if(!OffsetToString("coordIndex ["))
	{
		TRACE("invalid mesh\n");
		return 0;
	}
	m_IndexBuffer += strlen("coordIndex [") + 1;

	// Cur : int, int, int, -1,
	// End : int, int, int, -1]
	int NbFace = 0;
	do
	{
		int v1,v2,v3;
		ReadWord();
		success  = sscanf(m_pBufferWord,"%d,",&v1);
		ReadWord();
		success &= sscanf(m_pBufferWord,"%d,",&v2);
		ReadWord();
		success &= sscanf(m_pBufferWord,"%d,",&v3);

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
		sscanf(m_pBufferWord,"%d",&test);
		if(strstr(m_pBufferWord,"]") != NULL)
			success = 0;

	}
	while(success);
	TRACE(" added %d faces\n",NbFace);

	// Store texture coord index
	//***********************************************
	if(HasTexture)
	{
		m_IndexBuffer = tmp;
		if(!OffsetToString("texCoordIndex ["))
		{
			TRACE("invalid mesh\n");
			return 0;
		}
		m_IndexBuffer += strlen("texCoordIndex [") + 1;

		// Cur : int, int, int, -1,
		// End : int, int, int, -1]
		int NbTexCoordIndex = 0;
		do
		{
			int v1,v2,v3;
			ReadWord();
			success  = sscanf(m_pBufferWord,"%d,",&v1);
			ReadWord();
			success &= sscanf(m_pBufferWord,"%d,",&v2);
			ReadWord();
			success &= sscanf(m_pBufferWord,"%d,",&v3);

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
			sscanf(m_pBufferWord,"%d",&test);
			if(strstr(m_pBufferWord,"]") != NULL)
				success = 0;

		}
		while(success);
		TRACE(" added %d texture coordinate index\n",NbTexCoordIndex);
	}

	return 1;
}

// ** EOF **





