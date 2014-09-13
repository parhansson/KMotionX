//********************************************
// ColorRamp.cpp
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 19/05/98
// Modified : 19/05/98
//********************************************

#include "..\stdafx.h"

//********************************************
// Constructor
//********************************************
CColorRamp::CColorRamp()
{
	BuildDefault();
}

//********************************************
// Destructor
//********************************************
CColorRamp::~CColorRamp()
{
}

//********************************************
// Build
//********************************************
int CColorRamp::Build()
{
 int x1,y1,x2,y2;
 float a,b;
 ASSERT(m_NbNode >= 2);
 for(unsigned char k=0;k<3;k++)
	 for(int i=0;i<m_NbNode-1;i++)
   {
    x1 = (int)m_Node[i];
    x2 = (int)m_Node[i+1];

    ASSERT(x1<x2);
    y1 = m_Color[k][x1];
    y2 = m_Color[k][x2];

    a = (float)(y2-y1) / (float)(x2-x1);
    b = (float)y1 - a*(float)x1;

    for(int j=x1;j<x2;j++)
      m_Color[k][j]=(unsigned char)(a*(float)j+b);
   }
	return 1;
}

//********************************************
// Build
//********************************************
int CColorRamp::BuildNodes()
{
	// Check first and last are set
	m_Color[3][0] = 1;
	m_Color[3][m_Size-1] = 1;
	// Count nodes
	m_NbNode = 0;
	for(int i=0;i<m_Size;i++)
		if(m_Color[3][i]==1)
		{
			m_Node[m_NbNode]=(unsigned char)i;
			m_NbNode++;
		}
	TRACE("NbNode : %d\n",m_NbNode);
	ASSERT(m_NbNode>=2);
	return 1;
}

//********************************************
// ResetNodes
// Just first and last node
//********************************************
void CColorRamp::ResetNodes()
{
	for(int i=0;i<m_Size;i++)
		m_Color[3][i] = 0;
	m_Color[3][0] = 1;
	m_Color[3][m_Size-1] = 1;
	BuildNodes();
}

//********************************************
// BuildDefault
// 256 grey levels
//********************************************
void CColorRamp::BuildDefault()
{
	m_Size = 256;
	ResetNodes();

	// Grey scales
	for(unsigned char i=0;i<3;i++)
	{
		m_Color[i][0]=0;
		m_Color[i][255]=255;
	}
	Build();
}

//********************************************
// BuildDefault
// 256 grey levels
//********************************************
void CColorRamp::BuildRainbow()
{
	m_Size = 256;
	ResetNodes();

	// Rainbow
	m_Color[3][0] = 1;m_Color[0][0] = 0;m_Color[1][0] = 0;m_Color[2][0] = 255;
	m_Color[3][48] = 1;m_Color[0][48] = 0;m_Color[1][48] = 254;m_Color[2][48] = 255;
	m_Color[3][96] = 1;m_Color[0][96] = 0;m_Color[1][96] = 254;m_Color[2][96] = 0;
	m_Color[3][144] = 1;m_Color[0][144] = 255;m_Color[1][144] = 255;m_Color[2][144] = 0;
	m_Color[3][192] = 1;m_Color[0][192] = 255;m_Color[1][192] = 126;m_Color[2][192] = 0;
	m_Color[3][240] = 1;m_Color[0][240] = 255;m_Color[1][240] = 0;m_Color[2][240] = 0;
	m_Color[3][255] = 1;m_Color[0][255] = 255;m_Color[1][255] = 255;m_Color[2][255] = 255;

	BuildNodes();
	Build();
}

//********************************************
// Trace
//********************************************
void CColorRamp::Trace()
{
	TRACE("Ramp (%d elts)\n",m_Size);
	for(int i=0;i<m_Size;i++)
	{
		TRACE("  %3d %3d %3d\n",m_Color[0][i],m_Color[1][i],m_Color[2][i]);
	}
}

//** EOF **
