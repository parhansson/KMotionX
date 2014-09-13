//********************************************
// Color.cpp
//********************************************
// class CColor
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 10/12/97
// Modified : 09/02/98
//********************************************

#include "..\stdafx.h"

//////////////////////////////////////////////
// DATAS
//////////////////////////////////////////////

//********************************************
// Set
//********************************************
void CColor::Set(unsigned char red,
								 unsigned char green,
								 unsigned char blue,
								 unsigned char alpha)
{
	m_Red = red;
	m_Green = green;
	m_Blue = blue;
	m_Alpha = alpha;
}

//********************************************
// Set
//********************************************
void CColor::Set(unsigned char red,
								 unsigned char green,
								 unsigned char blue)
{
	m_Red = red;
	m_Green = green;
	m_Blue = blue;
	m_Alpha = 255;
}

//********************************************
// Set
//********************************************
void CColor::Set(unsigned char grey)
{
	m_Red = grey;
	m_Green = grey;
	m_Blue = grey;
	m_Alpha = 255;
}


//********************************************
// Set
//********************************************
void CColor::Set(CColor &c)
{
	m_Red = c.r();
	m_Green = c.g();
	m_Blue = c.b();
	m_Alpha = c.a();
}






// ** EOF **
