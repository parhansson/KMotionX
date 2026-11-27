//********************************************
// ColorRamp.h
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 19/05/98
// Modified : 19/05/98
//********************************************

#ifndef _COLOR_RAMP_
#define _COLOR_RAMP_


// Datas : 
// Red Green Blue IsNode (0/1)

class CColorRamp
{
private :
	unsigned char m_Color[4][256];
	unsigned char m_Node[256];
	int  m_Size;
	int  m_NbNode;

public :

	// Constructor
	CColorRamp();
	~CColorRamp();

	// Datas
	int GetSize() { return m_Size; }
	unsigned char Red(unsigned char index) { return m_Color[0][index]; }
	unsigned char Green(unsigned char index) { return m_Color[1][index]; }
	unsigned char Blue(unsigned char index) { return m_Color[2][index]; }

	// Misc
	int Build();
	void BuildDefault();
	int BuildNodes();
	void ResetNodes();
	void Trace();

	void BuildRainbow();

};

#endif // _COLOR_RAMP_
