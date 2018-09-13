// Screen.h : header file
/*********************************************************************/
/*         Copyright (c) 2003-20015  DynoMotion Incorporated          */
/*********************************************************************/


#if !defined(AFX_Screen_H__C433BC64_687C_4410_A89F_14C89F9A4EE6__INCLUDED_)
#define AFX_Screen_H__C433BC64_687C_4410_A89F_14C89F9A4EE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MotionButton.h"
#include "CEditScreen.h"
#include "CComboBoxScreen.h"

typedef struct 
{
    CString Name;
    CString Type;
    int ID;
    DWORD hWnd;
    int x, y;
    int width, height;
	CString BitmapFile;
	CString Colors;
	CString FontName;
	int FontSize;
	int HotKey;
	CStringW Text;
	CStringW ToolTipText;
	CStringW Script;
	bool selected;
	bool show;
	bool Bold;
	bool Italic;
	int Var;
	int Style;
} DLG_CONTROL;

typedef DLG_CONTROL *PDLG_CONTROL;

/////////////////////////////////////////////////////////////////////////////
// CScreen Processor

class CScreen 
{
// Construction
public:
    CScreen();

	void DeleteDlgControls(void);

	CList<PDLG_CONTROL,PDLG_CONTROL> DlgControls;
	CList<CString,CString> Defines;

// Implementation
public:
    virtual ~CScreen();

	int ProcessScript(CString file);
	BOOL SetWindowPosDPI(CWnd * W, const CWnd * pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags);
	int ReadResourceIDs();
	int FindResourceIDs(CString s);
	CString FindResourceName(int ID);
	DLG_CONTROL *FindDlgControl(int ID);
	int ParseString(CStringW &s, CString label, CStringW &r, bool NoErrors = false);
	int ParseString(CStringW &s, CString label, CString &r, bool NoErrors = false);
	int ParseInt(CStringW &s, CString label, int &r, bool NoErrors = false);
	int ParseIntHex(CStringW &s, CString label, int &r);
	CString Part(int n, CString p);
	int PartHex(int n, CString p);
	int SwapRGB(int n);
	bool CheckForScreenEditorToolTip(UINT id, LPWSTR Tip);
	int GetEditScreenVar(int Var, CString *s);
	bool CheckForScreenEditorHotKey(int VirtualKey, UINT *ID, CImageButton **I);
	bool Find3MotionButtonsSameAxisDir(int axis, int dir, CMotionButton **B, CMotionButton **B2, CMotionButton **Bstep);
	int Execute(CStringW s, DLG_CONTROL *Dlg, bool *NewControl);
	int DoControlID(CStringW s, DLG_CONTROL *Dlg);
	int DoCompatibility(DLG_CONTROL *Dlg);
	int DoMainDlg(CStringW s);
	int DoSScript(CStringW s);
	int DoAction(CStringW s);
	int DoWinMsg(CStringW s);
	CImageButton* FindImageButton(int ID);
	CMotionButton* FindMotionButton(int ID);
	CImageButton * FindImageButtonHotKey(int VirtualKey);
	CEditScreen* FindEditScreen(int ID);
	CComboBoxScreen* FindComboBoxScreen(int ID);
	CDisplay* FindDisplay(int ID);
	CStringW CreateScript(DLG_CONTROL *DlgCtrl);
	int ServiceImageButtons();
	bool GetStatusBit(int bit);
	void HandleRadioButton(CDialog *Dlg, int nIDC);
	void RemoveCommasW(CStringW &s);
	void Convert24to32(CImage *img);
	void ResetAllControls();
	int EditScreenChangesCount;
	bool CheckIfOKtoChangeText(int ID);




	CString LastLoadedScreen;

	int MainCX,MainCY,MainBackColor;
	int Caption, MaxBox, MinBox, ResizeFrame, Maximize;
	CString BackBitmap;

	CString BGFile_loaded;
	CImage BGimg;
	CBitmap BitmapFile;				// bitmap we are drawing
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_Screen_H__C433BC64_687C_4410_A89F_14C89F9A4EE6__INCLUDED_)
