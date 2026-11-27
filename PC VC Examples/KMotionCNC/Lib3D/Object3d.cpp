//********************************************
// Object3d.cpp
//********************************************
// class CObject3d, the base 3d object
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 17/12/97
// Modified : 15/01/98
//********************************************

#include "stdafx.h"



//********************************************
// GetType
//********************************************
int CObject3d::GetType()
{
	return TYPE_OBJECT3D;
}

//********************************************
// glBuildList
//********************************************
int CObject3d::glBuildList()
{
	AfxMessageBox(L"CObject3d::BuildList : virtual function");
	return 1;
}

//********************************************
// glDraw
//********************************************
int CObject3d::glDraw()
{
	AfxMessageBox(L"CObject3d::BuildList : virtual function");
	return 1;
}

// Function to invert a 4x4 matrix
bool CObject3d::invertMatrix(const float* m, float* invOut) {
	float inv[16], det;
	int i;

	inv[0] = m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

	inv[4] = -m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];

	inv[8] = m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	inv[12] = -m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];

	inv[1] = -m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];

	inv[5] = m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];

	inv[9] = -m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];

	inv[13] = m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];

	inv[2] = m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	inv[6] = -m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	inv[10] = m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	inv[14] = -m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det == 0)
		return false;

	det = 1.0f / det;

	for (i = 0; i < 16; i++)
		invOut[i] = inv[i] * det;

	return true;
}
// Function to create a translation matrix
void CObject3d::createTranslationMatrix(float* matrix, CVector3d* t)
{
	matrix[0] = 1.0f; matrix[4] = 0.0f; matrix[8] = 0.0f; matrix[12] = t->x();
	matrix[1] = 0.0f; matrix[5] = 1.0f; matrix[9] = 0.0f; matrix[13] = t->y();
	matrix[2] = 0.0f; matrix[6] = 0.0f; matrix[10] = 1.0f; matrix[14] = t->z();
	matrix[3] = 0.0f; matrix[7] = 0.0f; matrix[11] = 0.0f; matrix[15] = 1.0f;
}
// Function to create a scale matrix
void CObject3d::createScaleMatrix(float* matrix, CVector3d* scale) {
	matrix[0] = scale->x(); matrix[4] = 0.0f;       matrix[8] = 0.0f;        matrix[12] = 0.0f;
	matrix[1] = 0.0f;       matrix[5] = scale->y(); matrix[9] = 0.0f;        matrix[13] = 0.0f;
	matrix[2] = 0.0f;       matrix[6] = 0.0f;       matrix[10] = scale->z(); matrix[14] = 0.0f;
	matrix[3] = 0.0f;       matrix[7] = 0.0f;       matrix[11] = 0.0f;       matrix[15] = 1.0f;
}


// Function to create a rotation matrix around an arbitrary vector (XYZ)
void CObject3d::createRotationMatrix(float* matrix, CVector3d* axis, float angle_deg) {
	float angle = angle_deg * PI_F / 180.0f;
	float c = cosf(angle);
	float s = sinf(angle);
	float t = 1.0f - c;

	// Normalize the axis vector
	CVector3d nAxis;
	nAxis.Set(axis);
	nAxis.NormalizeL2();

	float x = nAxis.x();
	float y = nAxis.y();
	float z = nAxis.z();

	matrix[0] = t * x * x + c;     matrix[4] = t * x * y - s * z; matrix[8] = t * x * z + s * y; matrix[12] = 0.0f;
	matrix[1] = t * x * y + s * z; matrix[5] = t * y * y + c;     matrix[9] = t * y * z - s * x; matrix[13] = 0.0f;
	matrix[2] = t * x * z - s * y; matrix[6] = t * y * z + s * x; matrix[10] = t * z * z + c;    matrix[14] = 0.0f;
	matrix[3] = 0.0f;              matrix[7] = 0.0f;              matrix[11] = 0.0f;             matrix[15] = 1.0f;
}



// Function to create a rotation matrix around the X axis
void CObject3d::createRotationMatrixX(float* matrix, float angle_deg) {
	float angle = angle_deg * PI_F / 180.0f;
	float c = cosf(angle);
	float s = sinf(angle);
	matrix[0] = 1.0f; matrix[4] = 0.0f; matrix[8] = 0.0f; matrix[12] = 0.0f;
	matrix[1] = 0.0f; matrix[5] = c;    matrix[9] = -s;   matrix[13] = 0.0f;
	matrix[2] = 0.0f; matrix[6] = s;    matrix[10] = c;    matrix[14] = 0.0f;
	matrix[3] = 0.0f; matrix[7] = 0.0f; matrix[11] = 0.0f; matrix[15] = 1.0f;
}

// Function to create a rotation matrix around the Y axis
void CObject3d::createRotationMatrixY(float* matrix, float angle_deg) {
	float angle = angle_deg * PI_F / 180.0f;
	float c = cosf(angle);
	float s = sinf(angle);
	matrix[0] = c;    matrix[4] = 0.0f; matrix[8] = s;    matrix[12] = 0.0f;
	matrix[1] = 0.0f; matrix[5] = 1.0f; matrix[9] = 0.0f; matrix[13] = 0.0f;
	matrix[2] = -s;   matrix[6] = 0.0f; matrix[10] = c;    matrix[14] = 0.0f;
	matrix[3] = 0.0f; matrix[7] = 0.0f; matrix[11] = 0.0f; matrix[15] = 1.0f;
}

// Function to create a rotation matrix around the Z axis
void CObject3d::createRotationMatrixZ(float* matrix, float angle_deg) {
	float angle = angle_deg * PI_F / 180.0f;
	float c = cosf(angle);
	float s = sinf(angle);
	matrix[0] = c;    matrix[4] = -s;   matrix[8] = 0.0f; matrix[12] = 0.0f;
	matrix[1] = s;    matrix[5] = c;    matrix[9] = 0.0f; matrix[13] = 0.0f;
	matrix[2] = 0.0f; matrix[6] = 0.0f; matrix[10] = 1.0f; matrix[14] = 0.0f;
	matrix[3] = 0.0f; matrix[7] = 0.0f; matrix[11] = 0.0f; matrix[15] = 1.0f;
}

// Function to copy a 4x4 matrix
void CObject3d::copyMatrix(float* dest, const float* src) {
	for (int i = 0; i < 16; ++i) {
		dest[i] = src[i];
	}
}



// Function to multiply two 4x4 matrices
void CObject3d::multiplyMatrices(float* result, const float* a0, const float* b0) {
	float a[16], b[16];
	copyMatrix(a, a0);
	copyMatrix(b, b0);

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result[i * 4 + j] = a[i * 4 + 0] * b[0 * 4 + j] +
				a[i * 4 + 1] * b[1 * 4 + j] +
				a[i * 4 + 2] * b[2 * 4 + j] +
				a[i * 4 + 3] * b[3 * 4 + j];
		}
	}
}


// Function to multiply a 4x4 matrix with a 4x1 vector
void CObject3d::multiplyMatrixVector(const float* matrix, const float* vector, float* result) {
	for (int r = 0; r < 4; ++r) {
		result[r] = matrix[r + 0 * 4] * vector[0] +
			matrix[r + 1 * 4] * vector[1] +
			matrix[r + 2 * 4] * vector[2] +
			matrix[r + 3 * 4] * vector[3];
	}
}

// Function to compile shader
GLuint CObject3d::compileShader(const char* shaderCode, GLenum shaderType) {
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderCode, NULL);
	glCompileShader(shader);

	// Check for compilation errors
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		AfxMessageBox(L"ERROR::SHADER::COMPILATION_FAILED\n" + (CString)infoLog);
	}

	return shader;
}




GLuint CObject3d::BuildShader(const char *vertexShaderSource, const char *fragmentShaderSource)
{
	GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);


	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Check for linking errors
	GLint success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		AfxMessageBox(L"ERROR::SHADER::PROGRAM::LINKING_FAILED\n" + (CString)infoLog);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}


// ** EOF **



