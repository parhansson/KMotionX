//********************************************
// Transform.h
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 23/02/98
// Modified : 23/02/98
//********************************************

#ifndef _TRANSFORM_
#define _TRANSFORM_

#include "Vector3d.h"

class CTransform
{

private :

	// Datas
	CVector3d m_Scale;
	CVector3d m_Rotation;
	float m_ValueRotation;
	CVector3d m_Translation;
	float m_ValueRotationXY;
	float m_ValueRotationYZ;
	float m_ValueRotationZX;
public :

	// Constructor
	CTransform(); 
	~CTransform() {}

	// Data access
	void Clear(void);
	CVector3d *GetScale() { return &m_Scale; }
	CVector3d *GetRotation() { return &m_Rotation; }
	CVector3d *GetTranslation() { return &m_Translation; }
	float GetValueRotation() { return m_ValueRotation; }
	float GetValueRotationXY() { return m_ValueRotationXY; }
	float GetValueRotationYZ() { return m_ValueRotationYZ; }
	float GetValueRotationZX() { return m_ValueRotationZX; }

	// Data setting
	void SetScale(CVector3d &vector) { m_Scale.Copy(vector); }
	void SetRotation(CVector3d &vector) { m_Rotation.Copy(vector); }
	void SetTranslation(CVector3d &vector) { m_Translation.Copy(vector); }
	void SetValueRotation(float value) { m_ValueRotation = value; }
	void SetValueRotationXY(float value) { m_ValueRotationXY = value; }
	void SetValueRotationYZ(float value) { m_ValueRotationYZ = value; }
	void SetValueRotationZX(float value) { m_ValueRotationZX = value; }

	void SetScale(CVector3d *pVector) { m_Scale.Copy(pVector); }
	void SetRotation(CVector3d *pVector) { m_Rotation.Copy(pVector); }
	void SetTranslation(CVector3d *pVector) { m_Translation.Copy(pVector); }

	void Copy(CTransform &transform);
	void Copy(CTransform *pTransform);
};

#endif // _TRANSFORM_

