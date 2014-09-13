//********************************************
// Material.cpp
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 24/02/98
// Modified : 24/02/98
//********************************************

#include "..\stdafx.h"

//********************************************
// Constructor
//********************************************
CMaterial::CMaterial()
{
	// Default
	SetAmbient(0.0f,0.33f,0.5f,1.0f);
	SetDiffuse(0.5f,0.5f,0.5f,1.0f);
	SetSpecular(1.0f,1.0f,1.0f,1.0f);
	SetShininess(84.0f);
	SetEmission(0.0f,0.0f,0.0f,1.0f);
}

//////////////////////////////////////////////
// DATA SETTINGS
//////////////////////////////////////////////

//********************************************
// SetAmbient
//********************************************
void CMaterial::SetAmbient(float r,
													 float g,
													 float b,
													 float a)
{
	m_pAmbient[0] = r;
	m_pAmbient[1] = g;
	m_pAmbient[2] = b;
	m_pAmbient[3] = a;
}

//********************************************
// SetDiffuse
//********************************************
void CMaterial::SetDiffuse(float r,
													 float g,
													 float b,
													 float a)
{
	m_pDiffuse[0] = r;
	m_pDiffuse[1] = g;
	m_pDiffuse[2] = b;
	m_pDiffuse[3] = a;
}

//********************************************
// SetSpecular
//********************************************
void CMaterial::SetSpecular(float r,
														float g,
														float b,
														float a)
{
	m_pSpecular[0] = r;
	m_pSpecular[1] = g;
	m_pSpecular[2] = b;
	m_pSpecular[3] = a;
}

//********************************************
// SetEmission
//********************************************
void CMaterial::SetEmission(float r,
														float g,
														float b,
														float a)
{
	m_pEmission[0] = r;
	m_pEmission[1] = g;
	m_pEmission[2] = b;
	m_pEmission[3] = a;
}


//********************************************
// SetShininess
//********************************************
void CMaterial::SetShininess(float value)
{
	m_pShininess[0] = value;
}

//********************************************
// Copy
//********************************************
void CMaterial::Copy(CMaterial *pMaterial)
{
	SetSpecular(pMaterial->GetSpecular()[0],pMaterial->GetSpecular()[1],pMaterial->GetSpecular()[2],pMaterial->GetSpecular()[3]);
	SetAmbient(pMaterial->GetAmbient()[0],pMaterial->GetAmbient()[1],pMaterial->GetAmbient()[2],pMaterial->GetAmbient()[3]);
	SetDiffuse(pMaterial->GetDiffuse()[0],pMaterial->GetDiffuse()[1],pMaterial->GetDiffuse()[2],pMaterial->GetDiffuse()[3]);
	SetEmission(pMaterial->GetEmission()[0],pMaterial->GetEmission()[1],pMaterial->GetEmission()[2],pMaterial->GetEmission()[3]);
	SetShininess(pMaterial->GetShininess()[0]);
}


// ** EOF **



