//********************************************
// Material.h
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 24/02/98
// Modified : 24/02/98
//********************************************

#ifndef _MATERIAL_
#define _MATERIAL_

class CMaterial
{

private :

	float m_pAmbient[4];
	float m_pDiffuse[4];
	float m_pSpecular[4];
	float m_pShininess[1];
	float m_pEmission[4];

public :

	// Constructor
	CMaterial(); 
	~CMaterial() {}

	// Data access
	float *GetAmbient() { return m_pAmbient; }
	float *GetDiffuse() { return m_pDiffuse; }
	float *GetSpecular() { return m_pSpecular; }
	float *GetShininess() { return m_pShininess; }
	float *GetEmission() { return m_pEmission; }

	// Data setting
	void SetAmbient(float r,float g,float b,float a);
	void SetDiffuse(float r,float g,float b,float a);
	void SetSpecular(float r,float g,float b,float a);
	void SetShininess(float value);
	void SetEmission(float r,float g,float b,float a);

	// Datas
	void Copy(CMaterial *pMaterial);


};

#endif // _MATERIAL_

