//********************************************
// ParserVrml.h
// class CParserVrml
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 02/04/98
// Modified : 02/04/98
//********************************************

#ifndef _PARSER_VRML_
#define _PARSER_VRML_


#define MAX_LINE_VRML 10000
#define MAX_WORD_VRML 1000

class CParserVrml
{
private :

	int m_SizeFile;
	wchar_t *m_pBuffer;
	int m_IndexBuffer;
	CString m_FileName;
	wchar_t m_pBufferLine[MAX_LINE_VRML];
	wchar_t m_pBufferWord[MAX_WORD_VRML];

public :

	// Construtor / destructor
	CParserVrml();
	~CParserVrml();

	// Running
	void Free(void);
	int Run(wchar_t *filename,CSceneGraph3d *pSceneGraph);

	// Word processing
	int ReadFile(wchar_t *filename);
	int ReadLine();
	int ReadWord();
	int OffsetToString(wchar_t *string);
	int OffsetToStringBefore(wchar_t *string,wchar_t *before);
	int OffsetToStringBeginLine(wchar_t *string);


	int CheckVersion();
	void CountDef(void);
	int CountMesh(void);
	int CheckMesh();
	int ReadMesh(CSceneGraph3d *pSceneGraph);
	int StoreMesh(CArray3d<CVertex3d> *pArrayVertex,CArray3d<CFace3d> *pArrayFace,int HasTexture,float *pTextureCoordinate,int *pTextureCoordinateIndex);
	int SizeMesh(int *pNbVertex,int *pNbFace,int HasTexture,int *pNbTextureCoordinate = NULL);
};

#endif // _PARSER_VRML_

