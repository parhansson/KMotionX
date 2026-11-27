//********************************************
// Transform.cpp
//********************************************
// class CTransform
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 23/02/98
// Modified : 23/02/98
//********************************************

#include "stdafx.h"


//********************************************
// Constructor
//********************************************
CTransform::CTransform()
{
	SetScale(CVector3d(1.0f,1.0f,1.0f));
	SetTranslation(CVector3d(0.0f,0.0f,0.0f));
	SetValueRotation(0.0f);
	SetValueRotationXY(0.0f);
	SetValueRotationYZ(0.0f);
	SetValueRotationZX(0.0f);
}

//********************************************
// Constructor
//********************************************
void CTransform::Clear(void)
{
	SetScale(CVector3d(1.0f,1.0f,1.0f));
	SetTranslation(CVector3d(0.0f,0.0f,0.0f));
	SetValueRotation(0.0f);
	SetValueRotationXY(0.0f);
	SetValueRotationYZ(0.0f);
	SetValueRotationZX(0.0f);
}

//********************************************
// Copy
//********************************************
void CTransform::Copy(CTransform &transform)
{
	SetScale(transform.GetScale());
	SetTranslation(transform.GetTranslation());
	SetRotation(transform.GetRotation());
	SetValueRotation(transform.GetValueRotation());
	SetValueRotationXY(transform.GetValueRotationXY());
	SetValueRotationYZ(transform.GetValueRotationYZ());
	SetValueRotationZX(transform.GetValueRotationZX());
}

//********************************************
// Copy
//********************************************
void CTransform::Copy(CTransform *pTransform)
{
	SetScale(pTransform->GetScale());
	SetTranslation(pTransform->GetTranslation());
	SetRotation(pTransform->GetRotation());
	SetValueRotation(pTransform->GetValueRotation());
	SetValueRotationXY(pTransform->GetValueRotationXY());
	SetValueRotationYZ(pTransform->GetValueRotationYZ());
	SetValueRotationZX(pTransform->GetValueRotationZX());
}

// opengl type  of matrix transformation
// Vector b * Matrix a = Vector c

void CTransform::matrix_mul_vector(float *c,float *a,float *b)
{
	c[0]=(a[ 0]*b[0])+(a[ 4]*b[1])+(a[ 8]*b[2])+(a[12]);
	c[1]=(a[ 1]*b[0])+(a[ 5]*b[1])+(a[ 9]*b[2])+(a[13]);
	c[2]=(a[ 2]*b[0])+(a[ 6]*b[1])+(a[10]*b[2])+(a[14]);
	c[3]=(a[ 3]*b[0])+(a[ 7]*b[1])+(a[11]*b[2])+(a[15]);
}


void CTransform::make_matrix(float *a)
{
	glMatrixMode(GL_MODELVIEW);
	::glPushMatrix();
	glLoadIdentity();
	::glGetFloatv(GL_MODELVIEW_MATRIX,a);


	// Position / translation / scaling
	glTranslatef(GetTranslation()->x(),
				 GetTranslation()->y(),
				 GetTranslation()->z());

	glScalef(GetScale()->x(),
				 GetScale()->y(),
				 GetScale()->z());

	if (GetValueRotation()!=0.0f)
		glRotatef(GetValueRotation(),
				  GetRotation()->x(),
				  GetRotation()->y(),
				  GetRotation()->z());

	if (GetValueRotationXY() != 0.0f) glRotatef(GetValueRotationXY(), 0.0f, 0.0f, 1.0f);
	if (GetValueRotationZX() != 0.0f) glRotatef(GetValueRotationZX(), 0.0f, 1.0f, 0.0f);
	if (GetValueRotationYZ() != 0.0f) glRotatef(GetValueRotationYZ(), 1.0f, 0.0f, 0.0f);

	::glGetFloatv(GL_MODELVIEW_MATRIX,a);
	::glPopMatrix();
}


//////////////////////////////////////////////
// DATAS
//////////////////////////////////////////////



// ** EOF **
