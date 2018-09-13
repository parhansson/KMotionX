/*
	Unicode Button
	©2005 Robbert E. Peters

	History:
				Version 1.0 Date: 25/09/2005

	Copyright:
				You may use this code anyway you like.
*/

#pragma once


// CUniButton

class CUniButton : public CButton
{
//	DECLARE_DYNAMIC(CUniButton)

public:
	enum _AlignHorz {
	H_LEFT,
	H_CENTER,
	H_RIGHT
	};
	enum _AlignVert {
	V_TOP,
	V_CENTER,
	V_BOTTOM
	};

	CUniButton();
	virtual ~CUniButton();
	void SetHilightColors (COLORREF hilightBkgnd,COLORREF hilightText)
	{
		m_clrHilightBkgnd = hilightBkgnd;
		m_clrHilightText = hilightText;
		DetermineOwnerDraw();
	};
	void SetNormalColors (COLORREF clrBkgnd,COLORREF clrText)
	{
		m_clrNormalText = clrText;
		m_clrBkgnd = clrBkgnd;
		DetermineOwnerDraw();
	};
	void SetText(WCHAR *szText)
	{
		mTextDefined = szText[0] != 0;
		wcscpy(m_szText,szText);
		DetermineOwnerDraw();
	};
	void SetRTL(int bRTL)
	{
		m_bUseRTL=bRTL;
	};
	void SetDefaultSSAFlags(DWORD dwFlags)
	{
		m_dwDefaultSSAflags=dwFlags;
	};
	void SetFont(const char *szFaceName, int height, bool Bold, bool Italic);
	void SetDefaultTextColors();


	void SetHorizontalAlignment(_AlignHorz hAlign)
	{
		m_HorzAlign=hAlign;
	};
	void SetVerticalAlignment(_AlignVert vAlign)
	{
		m_VertAlign=vAlign;
	};

	virtual bool DetermineOwnerDraw(void);

	int Style;
	bool ToggleType;
	bool Toggled;

	COLORREF m_clrHilightBkgnd;
	COLORREF m_clrNormalText;
	COLORREF m_clrHilightText;
	COLORREF m_clrBkgnd;


protected:
	int			m_bUseRTL;
	DWORD		m_dwDefaultSSAflags;
	_AlignHorz m_HorzAlign;
	_AlignVert m_VertAlign;
	bool mFontDefined, mTextDefined;
	WCHAR m_szText[200];
	SIZE CalcTXTUnicode(HDC hDC, WCHAR *Sstr);
	void PlotTXTUnicode(
		HDC      hDC,               // In   Device
		int		xpos,
		int		ypos,
		RECT    *prc,               // In   Clipping rectange
		WCHAR	*szText,
		int      iLen);
	CFont m_font;
	int MomentaryResult;
	bool MomentaryCommandedDown;
	bool ForceDisableFocus;


// Overrides
     // ClassWizard generated virtual function overrides
     //{{AFX_VIRTUAL(CUniButton)
public:
	bool DrawPushed;
	int HandleButtonDown();
	 CStringW ToolTipText;
	 CStringW Script;
	 double Value;
	 int m_HotKey;
	 bool m_MouseOver;
	 int m_OriginalStyle;
	 bool m_OriginalStyleValid;
     virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	 int Var;
	 void Reset();


     //}}AFX_VIRTUAL
   // Generated message map functions
protected:
   //{{AFX_MSG(CUniButton)
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg LRESULT CUniButton::OnMouseLeave(WPARAM wp, LPARAM lp);
   afx_msg LRESULT CUniButton::OnMouseHover(WPARAM wp, LPARAM lp);
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};


