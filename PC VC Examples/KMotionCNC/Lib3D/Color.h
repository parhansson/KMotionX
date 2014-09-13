//********************************************
// Color.h
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 10/12/97
// Modified : 09/02/98
//********************************************

#ifndef _COLOR_
#define _COLOR_

class CColor
{

private :

	// Color
	unsigned char m_Red;
	unsigned char m_Green;
	unsigned char m_Blue;
	unsigned char m_Alpha;

public :

	// Constructor
	CColor() 
	{ 
		m_Red = m_Green = m_Blue = m_Alpha = 0;
	}

	CColor(CColor &color) 
	{ 
		m_Red = color.r();
		m_Green = color.g();
		m_Blue = color.b();
		m_Alpha = color.a();
	}

	~CColor() {}


	// Constructor
	CColor(unsigned char grey) { Set(grey); }
	CColor(unsigned char red,unsigned char green,unsigned char blue) { Set(red,green,blue); }
	CColor(unsigned char red,unsigned char green,unsigned char blue,unsigned char alpha) { Set(red,green,blue,alpha); }

	// Data access
	unsigned char r(void) { return m_Red; } 
	unsigned char g(void) { return m_Green; }  
	unsigned char b(void) { return m_Blue; }  
	unsigned char a(void) { return m_Alpha; }  

	// Data setting
	void r(unsigned char red)   { m_Red = red; } 
	void g(unsigned char green) { m_Green = green; }  
	void b(unsigned char blue)  { m_Blue = blue; }  
	void a(unsigned char alpha) { m_Alpha = alpha; }  

	// Data setting
	void Set(CColor &c);
	void Set(unsigned char grey); 
	void Set(unsigned char red,unsigned char green,unsigned char blue); 
	void Set(unsigned char red,unsigned char green,unsigned char blue,unsigned char alpha); 

	int operator==(CColor &color) { return (color.r() == m_Red) && (color.g() == m_Green) && (color.b() == m_Blue); }
	int operator==(CColor *pColor) { return (pColor->r() == m_Red) && (pColor->g() == m_Green) && (pColor->b() == m_Blue); }

};

#endif // _COLOR_

