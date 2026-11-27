//********************************************
// Object3d.h
//********************************************
// class CObject3d, the base 3d object
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 17/12/97
// Modified : 19/12/97
//********************************************

#ifndef _OBJECT3D_
#define _OBJECT3D_

class CVector3d;

class CObject3d
{
private :

public :
	CObject3d() {}
	virtual ~CObject3d() {}

	// Datas
	virtual int GetType();
	virtual int glBuildList();
	virtual int glDraw();
	virtual void SetModified() { };
	virtual void InvalidateDisplayList() { };

	bool invertMatrix(const float* m, float* invOut);
	void createTranslationMatrix(float* matrix, CVector3d* translation);
	void createScaleMatrix(float* matrix, CVector3d* scale);
	void createRotationMatrix(float* matrix, CVector3d* axis, float angle);
	void createRotationMatrixX(float* matrix, float angle);
	void createRotationMatrixY(float* matrix, float angle);
	void createRotationMatrixZ(float* matrix, float angle);
	void copyMatrix(float* dest, const float* src);
	void multiplyMatrices(float* result, const float* a, const float* b);
	void multiplyMatrixVector(const float* matrix, const float* vector, float* result);

	// Function to compile shader
	GLuint compileShader(const char* filePath, GLenum shaderType);

	GLuint BuildShader(const char* vertexShaderSource, const char* fragmentShaderSource);
};

#endif // _OBJECT3D_
