// Screen.cpp : implementation file
/*********************************************************************/
/*         Copyright (c) 2003-20015  DynoMotion Incorporated          */
/*********************************************************************/

#include "stdafx.h"
#include "Screen.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScreen

CScreen::CScreen()
{
	EditScreenChangesCount = 0;
}

CScreen::~CScreen()
{
	DeleteDlgControls();
}

void CScreen::DeleteDlgControls(void)
{
	for (POSITION p = DlgControls.GetHeadPosition(); p != NULL; )
	{
		DLG_CONTROL *Dlg = DlgControls.GetNext(p);
		delete Dlg;
	}

	DlgControls.RemoveAll();
}

int DontChangeTextCtrls[] = { IDC_Editor, IDC_Command, IDC_FeedRateEdit, IDC_SpindleRateEdit,
IDC_Step0, IDC_Step1, IDC_Step2, IDC_Step3, IDC_Step4, IDC_Step0, IDC_tool, IDC_fixture,
IDC_PosX, IDC_PosY, IDC_PosZ, IDC_PosA, IDC_PosB, IDC_PosC };

int CScreen::ProcessScript(CString file)
{
	static int NestingLevel = 0;
	DLG_CONTROL *Dlg;
	CStringW sw;

	if (file == "") return 0;  // nothing specified do nothing

	if (NestingLevel++ == 0)
	{
		DeleteDlgControls();
		if (ReadResourceIDs()) return 1;  // read resource ID symbolic names/IDs

		LastLoadedScreen = file;
		CImageButton::ErrorDisplayed = false; // display first error message
			
		if (TheFrame->GCodeDlg.m_DialogFaceInUse != CUSTOM_DLG_FACE)
		{
			TheFrame->GCodeDlg.m_DialogFaceInUse = CUSTOM_DLG_FACE;
			TheFrame->GCodeDlg.DestroyWindow();
			TheFrame->GCodeDlg.Create(IDD_KMOTIONCNC_0_ORIGINAL + CUSTOM_DLG_FACE);  // put up the real main window
			TheFrame->GCodeDlg.m_LastFixtureDisplayed = TheFrame->GCodeDlg.m_LastToolDisplayed = -1;
		}

		ResetAllControls();
	}

	// check if there is no path specified, then add in default
	if (file.Find("\\\\") == -1 && file.Find(':') == -1)
	{
		file = TheFrame->MainPathRoot + SCREEN_SCRIPTS_DIR + file;
	}


	wchar_t wcsString[1000];


	FILE *f = fopen(file,"r");
	if (!f)
	{
		MessageBox(NULL,"Unable to open Screen Script file:\r\r"+file,"KMotionCNC",MB_ICONSTOP|MB_OK);
		--NestingLevel;
		return 1;
	}

	short unsigned int Buf;
	int nw = fread(&Buf, 2, 1, f);  // Read the Byte Order mark to see if it is proper Unicode
	fclose(f);

	if (nw != 1)
	{
		MessageBox(NULL, "Unable to open Screen Script file:\r\r" + file, "KMotionCNC", MB_ICONSTOP | MB_OK);
		--NestingLevel;
		return 1;
	}

	if (Buf != 0xbbef && Buf != 0xfeff)  //UTF-8 or Unicode little endian
	{
		MessageBox(NULL, "Screen Script file not Little Endian Unicode or UTF-8 Encoding:\r\r" + file, "KMotionCNC", MB_ICONSTOP | MB_OK);
		--NestingLevel;
		return 1;
	}


	// Open the file with the specified encoding
	FILE *fStream;
	errno_t e = _tfopen_s(&fStream, file, _T("rt,ccs=UNICODE"));
	if (e != 0)  // failed..CString sRead;
	{
		MessageBox(NULL,"Unable to open Screen Script file:\r\r"+file,"KMotionCNC",MB_ICONSTOP|MB_OK);
		--NestingLevel;
		return 1;
	}

	CStdioFile g(fStream);  // open the file from this stream
	CStringW sRead;

	Dlg = new DLG_CONTROL();

	bool bReadData, NewControl;

	do
	{
		bReadData = (NULL != fgetws(wcsString, 900, g.m_pStream));

		if (bReadData)
		{
			sw = wcsString;

			int result = Execute(sw, Dlg, &NewControl);

			if (result)
			{
				g.Close();
				if (result == 2)
					MessageBox(NULL, "Screen Script file:\r\r" + file + "\r\rUnable to find ID:\r\r" + Dlg->Name, "KMotionCNC", MB_ICONSTOP | MB_OK);
				--NestingLevel;
				return 1;
			}

			if (NewControl)
			{
				DlgControls.AddTail(Dlg);
				Dlg = new DLG_CONTROL();
			}
		}
	} while (bReadData);

	delete Dlg;

	g.Close();

	// After processing Screen check if any Labels have a Screen 
	// Script Specified if so execute the Screen Script Command

	for (POSITION p = DlgControls.GetHeadPosition(); p != NULL; )
	{
		DLG_CONTROL *Dlg = DlgControls.GetNext(p);
		if (Dlg->Style == Label)
		{
			bool NewControl = false;
			if (Dlg->Script != "")
				if (Execute(Dlg->Script, Dlg, &NewControl))
				{
					CString Script = Dlg->Script;
					AfxMessageBox("Error Screen Load Label Action/Script/WinMsg.  ID:" + Dlg->Name + "\r\r" + Script);
				}
		}
	}


	if (--NestingLevel == 0)
		TheFrame->GCodeDlg.Invalidate();


	return 0;
}

BOOL CScreen::SetWindowPosDPI(CWnd *W, const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags)
{
	HDC screen = GetDC(0);

	int dpiX = GetDeviceCaps(screen, LOGPIXELSX);
	int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
	int dpi_standard = 96;

	return W->SetWindowPos(0, x * dpi_standard / dpiX, y * dpi_standard / dpiX, cx * dpi_standard / dpiX, cy * dpi_standard / dpiX, nFlags);
}

// Execute a line of Screen Script
//
// ID: defines a Dialog Controll
// Main: defines the Main Dialog Settings
// 

int CScreen::Execute(CStringW s, DLG_CONTROL *Dlg, bool *NewControl)
{
	*NewControl = false;

	if (s != "")
	{
		if (s.Find(L"ID:") == 0)
		{
			*NewControl = true;
			return DoControlID(s, Dlg);
		}
		else if (s.Find(L"Main:") == 0)
		{
			return DoMainDlg(s);
		}
		else if (s.Find(L"SScript:") == 0)
		{
			return DoSScript(s);
		}
		else if (s.Find(L"Action:") == 0)
		{
			return DoAction(s);
		}
		else if (s.Find(L"WinMsg:") == 0)
		{
			return DoWinMsg(s);
		}
	}
	return 0;
}

int CScreen::DoSScript(CStringW s)
{
	CString FileName;

	if (ParseString(s, "SScript:", FileName)) return 1;

	if (FileName.Find("\\\\") == -1 && FileName.Find(':') == -1)
	{
		CString DefaultDir = TheFrame->MainPathRoot + SCREEN_SCRIPTS_DIR;
		// no directory so add the default
		FileName = DefaultDir + FileName;
	}

	return ProcessScript(FileName);
}

int CScreen::DoWinMsg(CStringW s)
{
	CString Msg,DlgName;

	if (ParseString(s, "WinMsg:", Msg)) return 1;

	CString type = Part(0, Msg);
	if (type !=  "DlgName")
	{
		AfxMessageBox("Invalid WinMsg Type: "+type);
		return 1;
	}

	CString name = Part(1, Msg);
	int ID = FindResourceIDs(name);
	if (ID == -1)
	{
		AfxMessageBox("Invalid WinMsg ID Name: "+name);
		return 1;
	}

	CString MessageCode = Part(2, Msg);
	if (MessageCode == "BM_SETCHECK")
	{
		int Message = BM_SETCHECK;
		CString Param = Part(3, Msg);
		int wParam;
		if (Param == "BST_CHECKED")
		{
			wParam = BST_CHECKED;
		}
		else if (Param == "BST_UNCHECKED")
		{
			wParam = BST_UNCHECKED;
		}
		else
		{
			AfxMessageBox("Invalid WinMsg Message Param : " + Param);
			return 1;
		}
		HWND hDlg = GetDlgItem(TheFrame->GCodeDlg.m_hWnd, ID);
		SendMessage(hDlg, Message, wParam, 0);
		TheFrame->GCodeDlg.UpdateData();
	}
	else
	{
		AfxMessageBox("Invalid WinMsg Message Code : " + MessageCode);
		return 1;
	}
	return 0;
}


int CScreen::DoAction(CStringW s)
{
	int result;
	CString Action;
	MCODE_ACTION MCodeAction;

	if (ParseString(s, "Action:", Action)) return 1;

	result = sscanf(Action, "%d", &MCodeAction.Action);
	if (result != 1 || MCodeAction.Action<0 || MCodeAction.Action>M_Action_Waitbit)
	{
		AfxMessageBox("Invalid Action Code");
		return 1;
	}

	for (int i = 0; i < 5; i++)
	{
		CString s = Part(i + 1, Action);
		if (s == "")
		{
			MCodeAction.dParams[i] = 0.0;
		}
		else
		{
			result = sscanf(s, "%lf", &MCodeAction.dParams[i]);
			if (result != 1)
			{
				AfxMessageBox("Invalid Action Parameter");
				return 1;
			}
		}
	}
	strncpy(MCodeAction.String,Part(6, Action),sizeof(MCodeAction.String));

	return TheFrame->GCodeDlg.Interpreter->InvokeAction(-1, false, &MCodeAction);
}

int CScreen::DoMainDlg(CStringW s)
{
	bool CaptionSpecified = false;

	if (ParseInt(s, "Main:CX:", MainCX)) return 1;
	if (ParseInt(s, "CY:", MainCY)) return 1;
	if (ParseIntHex(s, "BackColor:", MainBackColor)) return 1;

	if (ParseInt(s, "Caption:", Caption, true) == 0)
	{
		CaptionSpecified = true;
		if (ParseInt(s, "MaxBox:", MaxBox)) return 1;
		if (ParseInt(s, "MinBox:", MinBox)) return 1;
		if (ParseInt(s, "ResizeFrame:", ResizeFrame)) return 1;
		if (ParseInt(s, "Maximize:", Maximize)) return 1;
	}

	BackBitmap = ""; // default to none
	ParseString(s, "BackBitmap:", BackBitmap, true);

	MainBackColor = SwapRGB(MainBackColor);
	TheFrame->GCodeDlg.m_DlgBackgroundColor = MainBackColor;
	TheFrame->GCodeDlg.m_DlgBackgroundBrush.DeleteObject();
	TheFrame->GCodeDlg.m_DlgBackgroundBrush.Detach();
	TheFrame->GCodeDlg.m_DlgBackgroundBrush.CreateSolidBrush(MainBackColor); // color white brush


	// handle main dialog background image
	CString BGFile = BackBitmap;

	// check if there is no path specified, then add in default
	if (BGFile != "" && BGFile.Find(':') == -1 && BGFile.Find("\\\\") == -1)
	{
		BGFile = TheFrame->MainPathRoot + SCREEN_BITMAPS_DIR + BGFile;
	}

	if (BGFile_loaded != BGFile)
	{
		HBITMAP hBmp = BGimg; // CImage is still the data owner
		if (hBmp != NULL)
		{
			BGimg.Detach(); // CImage is "empty" now
			BitmapFile.Detach(); // CBitmap is empty now
			::DeleteObject(hBmp);
		}

		BGFile_loaded = BGFile;
		if (BackBitmap != "")
		{
			HRESULT result = BGimg.Load(_T(BGFile));
			if (result == E_FAIL)
			{
				AfxMessageBox("Can't open Background image:" + BGFile);
				return 1;
			}
			Convert24to32(&BGimg);
			BitmapFile.Attach(BGimg);
			TheFrame->GCodeDlg.SetBackgroundImage(BitmapFile);
		}
		else
		{
			TheFrame->GCodeDlg.SetBackgroundImage((HBITMAP)NULL);
		}
	}



	int CurStyle = TheFrame->GCodeDlg.GetStyle();

	if (CaptionSpecified)
	{
		if (Caption)
			TheFrame->GCodeDlg.ModifyStyle(0, WS_CAPTION); // to show
		else
			TheFrame->GCodeDlg.ModifyStyle(WS_CAPTION, 0); // to hide
														   
		if (MaxBox)
			TheFrame->GCodeDlg.ModifyStyle(0, WS_MAXIMIZEBOX); // to show
		else
			TheFrame->GCodeDlg.ModifyStyle(WS_MAXIMIZEBOX, 0); // to hide

		if (MinBox)
			TheFrame->GCodeDlg.ModifyStyle(0, WS_MINIMIZEBOX); // to show
		else
			TheFrame->GCodeDlg.ModifyStyle(WS_MINIMIZEBOX, 0); // to hide

		if (ResizeFrame)
			TheFrame->GCodeDlg.ModifyStyle(0, WS_THICKFRAME); // to show
		else
			TheFrame->GCodeDlg.ModifyStyle(WS_THICKFRAME, 0); // to hide

		if (Maximize)
		{
			TheFrame->GCodeDlg.ShowWindow(SW_MAXIMIZE);
			TheFrame->GCodeDlg.ModifyStyle(0, WS_MAXIMIZE); // to show
		}
		else
		{
			TheFrame->GCodeDlg.ModifyStyle(WS_MAXIMIZE, 0); // off
			TheFrame->GCodeDlg.SetWindowPos(0, 0, 0, MainCX, MainCY, SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
		}
	
	}
	else
	{
		TheFrame->GCodeDlg.ModifyStyle(WS_MAXIMIZE, 0); // off
		TheFrame->GCodeDlg.SetWindowPos(0, 0, 0, MainCX, MainCY, SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
	}
	return 0;
}


int CScreen::DoControlID(CStringW s, DLG_CONTROL *Dlg)
{
	int ishow, iBold, iItalic;
// ID:IDC_ZeroAll,Type:PUSHBUTTON,X:11,Y:211,CX:54,CY:23,Show:1,Var:0,BitmapFile:,Colors:,Font:,FontSize:,HotKey:,Text:zero all
	if (ParseString(s, "ID:", Dlg->Name)) return 1;
	Dlg->ID = FindResourceIDs(Dlg->Name);
	if (Dlg->ID == -1) return 2;
	if (ParseString(s,"Type:",Dlg->Type)) return 1;
	if (ParseInt(s,"X:",Dlg->x)) return 1;
	if (ParseInt(s,"Y:",Dlg->y)) return 1;
	if (ParseInt(s,"CX:",Dlg->width)) return 1;
	if (ParseInt(s,"CY:",Dlg->height)) return 1;
	if (ParseInt(s,"Show:",ishow)) return 1;
	Dlg->show = ishow==1;
	if (ParseInt(s, "Var:", Dlg->Var)) return 1;
	if (ParseInt(s, "Style:", Dlg->Style)) return 1;
	if (ParseString(s, "BitmapFile:", Dlg->BitmapFile)) return 1;
	if (ParseString(s, "Colors:", Dlg->Colors)) return 1;
	if (ParseString(s, "Font:", Dlg->FontName)) return 1;
	if (ParseInt(s, "FontSize:", Dlg->FontSize)) return 1;
	if (ParseInt(s, "HotKey:", Dlg->HotKey)) return 1;
	if (ParseInt(s, "Bold:", iBold)) return 1;
	Dlg->Bold = iBold == 1;
	if (ParseInt(s, "Italic:", iItalic)) return 1;
	Dlg->Italic = iItalic == 1;
	if (ParseString(s, "Text:", Dlg->Text)) return 1;
	if (ParseString(s, "ToolTipText:", Dlg->ToolTipText)) return 1;
	if (ParseString(s, "Script:", Dlg->Script)) return 1;

	DoCompatibility(Dlg);
	
	CWnd *W = TheFrame->GCodeDlg.GetDlgItem(Dlg->ID);

	if (!W)
	{
		MessageBox(NULL, "Error Screen Script File.  Screen Control not found.\r\r" + Dlg->Name, "KMotionCNC", MB_ICONSTOP | MB_OK);
		return 1;
	}

	if (Dlg->ID == IDC_Editor)
	{
		if (Dlg->FontName != "")
			TheFrame->GCodeDlg.m_Editor.SetupForGCode(Dlg->FontSize, Dlg->FontName);
		else
			TheFrame->GCodeDlg.m_Editor.SetupForGCode(10, "Courier New");
		TheFrame->GCodeDlg.m_Editor.ReadWriteVar = Dlg->Var;

		if (Dlg->Var == 0 || Dlg->Var == 2)
			TheFrame->GCodeDlg.m_Editor.SetReadOnly(TRUE);
		else
			if (Dlg->Var == 1)
				TheFrame->GCodeDlg.m_Editor.SetReadOnly(FALSE);
	}

	CEditScreen *E=NULL;
	CComboBoxScreen *C=NULL;
	CDisplay *D=NULL;

	CImageButton *I;
	I=FindImageButton(Dlg->ID);
	if (I)
	{
		CString Part0=Part(0,Dlg->BitmapFile);
		CString Part1=Part(1,Dlg->BitmapFile);

		if (Part0 != "" && Part0.Find("\\\\") == -1 && Part0.Find(':') == -1)
		{
			CString DefaultDir = TheFrame->MainPathRoot + SCREEN_BITMAPS_DIR;
			// no directory so add the default
			Part0 = DefaultDir + Part0;
		}

		if (Part1 != "" && Part1.Find("\\\\") == -1 && Part1.Find(':') == -1)
		{
			CString DefaultDir = TheFrame->MainPathRoot + SCREEN_BITMAPS_DIR;
			// no directory so add the default
			Part1 = DefaultDir + Part1;
		}
		I->LoadBitmaps(Part0, Part1, "");

		if (Dlg->Colors != ";;;" && Dlg->Colors != "")
		{
			int Color0 = SwapRGB(PartHex(0, Dlg->Colors));
			int Color1 = SwapRGB(PartHex(1, Dlg->Colors));
			int Color2 = SwapRGB(PartHex(2, Dlg->Colors));
			int Color3 = SwapRGB(PartHex(3, Dlg->Colors));

			if (Color0 != 0 || Color1 != 0 || Color2 != 0 || Color3 != 0)
			{
				I->SetNormalColors(Color1, Color0);
				I->SetHilightColors(Color3, Color2);
			}
		}
		else
		{
			I->SetDefaultTextColors();
		}

		I->SetFont(Dlg->FontName, Dlg->FontSize, Dlg->Bold, Dlg->Italic);

		I->m_HotKey = Dlg->HotKey;
		I->Var = Dlg->Var;

		if (Dlg->Style != Undefined) // for compatibility with old scripts only set if defined in the script
			I->Style = Dlg->Style;
		
		I->Script = Dlg->Script;
		I->ToolTipText = Dlg->ToolTipText;
	}
	else
	{
		E = FindEditScreen(Dlg->ID);
		if (E)
		{
			E->SetFont(Dlg->FontName, Dlg->FontSize, Dlg->Bold, Dlg->Italic);
			E->ToolTipText = Dlg->ToolTipText;
			E->Var = Dlg->Var;
		}

		C = FindComboBoxScreen(Dlg->ID);
		if (C)
		{
			C->SetFont(Dlg->FontName, Dlg->FontSize, Dlg->Bold, Dlg->Italic);
			C->ToolTipText = Dlg->ToolTipText;
			C->Var = Dlg->Var;
		}

		D = FindDisplay(Dlg->ID);
		if (D)
		{
			if (Dlg->FontName != "")
				D->SetFont(Dlg->FontName, Dlg->FontSize, Dlg->Bold, Dlg->Italic);
			else
				D->SetFont("Courier New", 36, true, false);

			int Color1 = 0; // default to black background
			if (Dlg->Colors != ";;;" && Dlg->Colors != "")
				Color1 = SwapRGB(PartHex(1, Dlg->Colors));
			D->SetBackColor(Color1);
		}
	}

	if (Dlg->show)
		W->SetWindowPos(0, Dlg->x, Dlg->y, Dlg->width, Dlg->height, SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
	else
		W->SetWindowPos(0, Dlg->x, Dlg->y, Dlg->width, Dlg->height, SWP_NOACTIVATE | SWP_NOZORDER | SWP_HIDEWINDOW);

	// only change text of controls not dynamically updated
	if (CheckIfOKtoChangeText(Dlg->ID))
	{
		if (I && I->Style != Radio && I->Style != CheckBox)
		{
			I->SetText(Dlg->Text.GetBuffer());
		}
		else if (E)  // for Edit Controls only set the text (to default) if there is no text
		{
			WCHAR w[256];
			GetDlgItemTextW(TheFrame->GCodeDlg.m_hWnd, Dlg->ID, w, 255);
			if (w[0]==0)
				SetDlgItemTextW(TheFrame->GCodeDlg.m_hWnd, Dlg->ID, Dlg->Text);
		}
		else
		{
			SetDlgItemTextW(TheFrame->GCodeDlg.m_hWnd, Dlg->ID, Dlg->Text);
		}
	}


	return 0;
}

// only change text of controls not in this list as they are dynamically updated
bool CScreen::CheckIfOKtoChangeText(int ID)
{
	int i, n_no_text = sizeof(DontChangeTextCtrls) / sizeof(int);
	for (i = 0; i < n_no_text; i++)
		if (ID == DontChangeTextCtrls[i])
			return false;
	return true;
}

void CScreen::ResetAllControls()
{
	POSITION position = CImageButton::ImageButtons.GetHeadPosition();
	for (int i = 0; i<CImageButton::ImageButtons.GetCount(); i++)
		CImageButton::ImageButtons.GetNext(position)->Reset();

	position = CDisplay::Displays.GetHeadPosition();
	for (int i = 0; i<CDisplay::Displays.GetCount(); i++)
		CDisplay::Displays.GetNext(position)->Reset();


	position = CEditScreen::EditScreens.GetHeadPosition();
	for (int i = 0; i<CEditScreen::EditScreens.GetCount(); i++)
		CEditScreen::EditScreens.GetNext(position)->Reset();


	position = CComboBoxScreen::ComboBoxScreens.GetHeadPosition();
	for (int i = 0; i<CComboBoxScreen::ComboBoxScreens.GetCount(); i++)
		CComboBoxScreen::ComboBoxScreens.GetNext(position)->Reset();
}

// Patch old Screen Scripts to be compatible with new functionality
int CScreen::DoCompatibility(DLG_CONTROL *Dlg)
{
	if (Dlg->ID == IDC_Cust3)
	{
		if (Dlg->Style != DualLabel)
		{
			Dlg->Style = DualLabel;
			Dlg->BitmapFile = "FR.png;FR32.png";
		}
	}
	else if (Dlg->ID == IDC_Cust2)
	{
		if (Dlg->Style != DualLabel)
		{
			Dlg->Style = DualLabel;
			Dlg->BitmapFile = "SR.png;SRCSS.png";
		}
	}
	return 0;
}


// Find a Control based on ID return NULL if not found

DLG_CONTROL *CScreen::FindDlgControl(int ID)
{
	for (POSITION p = DlgControls.GetHeadPosition(); p != NULL; )
	{
		DLG_CONTROL *Dlg = DlgControls.GetNext(p);
		if (Dlg->ID == ID)
		{
			return Dlg;
		}
	}
	return NULL;
}

CStringW CScreen::CreateScript(DLG_CONTROL *DlgCtrl)
{
	CStringW s;
	CStringW wName = DlgCtrl->Name;			RemoveCommasW(wName);
	CStringW wType = DlgCtrl->Type;				RemoveCommasW(wType);
	CStringW wBitmapFile = DlgCtrl->BitmapFile;	RemoveCommasW(wBitmapFile);
	CStringW wColors = DlgCtrl->Colors;			RemoveCommasW(wColors);
	CStringW wFontName = DlgCtrl->FontName;		RemoveCommasW(wFontName);
	CStringW wToolTipText = DlgCtrl->ToolTipText;			RemoveCommasW(wToolTipText);
	CStringW wText = DlgCtrl->Text;				RemoveCommasW(wText);

	s.GetBufferSetLength(2000);
	
	s.Format(L"ID:%s,Type:%s,X:%d,Y:%d,CX:%d,CY:%d,Show:%d,Var:%d,Style:%d,BitmapFile:%s,Colors:%s,Font:%s,FontSize:%d,HotKey:%d,Bold:%d,Italic:%d,Text:%s,ToolTipText:%s,Script:%s",
		(LPCWSTR)wName, (LPCWSTR)wType, DlgCtrl->x, DlgCtrl->y, DlgCtrl->width, DlgCtrl->height, DlgCtrl->show ? 1 : 0, DlgCtrl->Var, DlgCtrl->Style,
		(LPCWSTR)wBitmapFile,(LPCWSTR)wColors, wFontName, DlgCtrl->FontSize, DlgCtrl->HotKey, DlgCtrl->Bold ? 1 : 0, DlgCtrl->Italic ? 1 : 0, (LPCWSTR)wText,
		(LPCWSTR)wToolTipText, (LPCWSTR)DlgCtrl->Script);

	return s;
}

void CScreen::RemoveCommasW(CStringW &s)
{
	s.Replace(L",", L" ");
}

CImageButton* CScreen::FindImageButton(int ID)
{
	POSITION position = CImageButton::ImageButtons.GetHeadPosition();
	LPCImageButton p;
	for (int i = 0; i<CImageButton::ImageButtons.GetCount(); i++)
	{
		p = CImageButton::ImageButtons.GetNext(position);

		// Found?
		if (p->m_hWnd && p->GetDlgCtrlID() == ID)
		{
			return p;
		}
	}
	return NULL;
}

CDisplay* CScreen::FindDisplay(int ID)
{
	POSITION position = CDisplay::Displays.GetHeadPosition();
	LPCDisplay p;
	for (int i = 0; i<CDisplay::Displays.GetCount(); i++)
	{
		p = CDisplay::Displays.GetNext(position);

		// Found?
		if (p->m_hWnd && p->GetDlgCtrlID() == ID)
		{
			return p;
		}
	}
	return NULL;
}

CMotionButton* CScreen::FindMotionButton(int ID)
{
	POSITION position = CMotionButton::AxisGroup.GetHeadPosition();
	LPCMotionButton p;
	for (int i = 0; i<CMotionButton::AxisGroup.GetCount(); i++)
	{
		p = CMotionButton::AxisGroup.GetNext(position);

		// Found?
		if (p->m_hWnd && p->GetDlgCtrlID() == ID)
		{
			return p;
		}
	}
	return NULL;
}


CEditScreen* CScreen::FindEditScreen(int ID)
{
	POSITION position = CEditScreen::EditScreens.GetHeadPosition();
	LPCEditScreen p;
	for (int i = 0; i<CEditScreen::EditScreens.GetCount(); i++)
	{
		p = CEditScreen::EditScreens.GetNext(position);

		// Found?
		if (p->m_hWnd && p->GetID() == ID)
		{
			return p;
		}
	}
	return NULL;
}

CComboBoxScreen* CScreen::FindComboBoxScreen(int ID)
{
	POSITION position = CComboBoxScreen::ComboBoxScreens.GetHeadPosition();
	LPCComboBoxScreen p;
	for (int i = 0; i<CComboBoxScreen::ComboBoxScreens.GetCount(); i++)
	{
		p = CComboBoxScreen::ComboBoxScreens.GetNext(position);

		// Found?
		if (p->m_hWnd && p->GetDlgCtrlID() == ID)
		{
			return p;
		}
	}
	return NULL;
}

CImageButton* CScreen::FindImageButtonHotKey(int VirtualKey)
{
	POSITION position = CImageButton::ImageButtons.GetHeadPosition();
	LPCImageButton p;
	for (int i = 0; i<CImageButton::ImageButtons.GetCount(); i++)
	{
		p = CImageButton::ImageButtons.GetNext(position);

		// Found?
		if (p->m_HotKey == VirtualKey) return p;
	}
	return NULL;
}

int CScreen::ServiceImageButtons()
{
	POSITION position=CImageButton::ImageButtons.GetHeadPosition();
	LPCImageButton p;
	for (int i=0; i<CImageButton::ImageButtons.GetCount(); i++)
	{
		p=CImageButton::ImageButtons.GetNext(position);

		if (p->Var != -1 && p->m_hWnd != NULL)
		{
			if (p->Style == ToggleButton)
			{
				p->Toggled = GetStatusBit(p->Var);
				p->Invalidate(FALSE);
			}
			else if (p->Style == VertBar)
			{
					int value;
				int result = TheFrame->GCodeDlg.GetVar(p->Var, &value);

				if (result)
				{
					AfxMessageBox("Error Reading Persist Variable for Vert Bar Graph");
					return 1;
				}

				p->Value = *(float *)&value;
				p->Invalidate(FALSE);

			}
			else if (p->Style == DROLabel)
			{
				int value;
				int result = TheFrame->GCodeDlg.GetVar(p->Var, &value);

				if (result)
				{
					AfxMessageBox("Error Reading Persist Variable for DRO");
					return 1;
				}
				
				if (value != 0)
				{
					CString s;
					result = TheFrame->GCodeDlg.GetStringFromGather(value, &s, 50);
					if (result)
					{
						AfxMessageBox("Error Reading String for DRO");
						return 1;
					}
					if (TheFrame->GCodeDlg.SetVar(p->Var, 0)) return 1;
					CStringW sw = s;
					p->SetText(sw.GetBuffer());
					p->Invalidate(TRUE);
				}

			}
			else if (p->Style == Label)
			{
				// Toggle the button based on the specified Bit
				p->DrawPushed = GetStatusBit(p->Var);
				p->Invalidate(FALSE);
			}
		}
	}
	return 0;
}


bool CScreen::GetStatusBit(int bit)
{
	MAIN_STATUS *MS = &TheFrame->GCodeDlg.MainStatus;

	if (bit < 48)  // KFLOP
		return (MS->BitsState[bit/32] & (1<<(bit&31)))!=0;

	if (bit < 64) // Virtual
		return (MS->VirtualBits & (1<<((bit-48)&31)))!=0;

	if (bit < 94) // Snap0
		return (MS->SnapBitsState0 & (1<<((bit-64)&31)))!=0;

	if (bit < 126) // Snap1
		return (MS->SnapBitsState0 & (1<<((bit-96)&31)))!=0;

	if (bit < 144) // Kanalog Inputs
		return (MS->KanalogBitsStateInputs & (1<<((bit-128)&31)))!=0;

	if (bit < 167) // Kanalog Outputs
		return (MS->KanalogBitsStateOutputs & (1<<((bit-144)&31)))!=0;

	if (bit < 184) // KStep Inputs
		return (MS->VirtualBits & (1<<((bit-168+16)&31)))!=0;

	if (bit < 1056) // KStep Inputs
		return (MS->VirtualBitsEx0 & (1<<((bit-1024)&31)))!=0;

	// If main frame couldn't connect then don't bother
	if (!TheFrame->GCodeDlg.m_ConnectedForStatus) return false;

	CString s,r;

	if (bit >= 1024 && bit < 2047)
	{
		s.Format("ReadBit%d",bit);
		if(TheFrame->KMotionDLL->WriteLineReadLine(s, r.GetBufferSetLength(100))) return false;
		r.ReleaseBuffer();

		return r[0] == '1';
	}
	return false;
}


int CScreen::ParseString(CStringW &s, const CString label0, CString &r, bool NoErrors)
{
	CStringW wr;
	int result = ParseString(s, label0, wr, NoErrors);

	if (!result)
		r = wr;

	return result;
}

int CScreen::ParseString(CStringW &s, const CString label0, CStringW &r, bool NoErrors)
{
	CStringW label = label0;

	if (s.Find(label) != 0)
	{
		if (!NoErrors)
			MessageBoxW(NULL, "Error Screen Script File.  Expecting:" + label + "\r\rFound:" + s, L"KMotionCNC", MB_ICONSTOP | MB_OK);
		return 1;
	}
	s.Delete(0, label.GetLength());

	r = s;

	int k = s.Find(',');
	if (k == -1) k = s.GetLength();
	r = s.Mid(0, k);
	s.Delete(0, k);
	if (s[0] == ',')	s.Delete(0, 1);

	r.Remove('\r');
	r.Remove('\n');

	return 0;
}



int CScreen::ParseInt(CStringW &s, CString label, int &r, bool NoErrors)
{
	CStringW v;

	if (ParseString(s, label, v, NoErrors)) return 1;

	CString va = v;
	sscanf(va, "%d", &r);
	return 0;
}

int CScreen::ParseIntHex(CStringW &s, CString label, int &r)
{
	CStringW v;

	if (ParseString(s, label, v)) return 1;

	CString va = v;
	sscanf(va, "%x", &r);
	return 0;
}

int CScreen::ReadResourceIDs()
{
	if (Defines.GetCount() > 0) return 0; // check if already read Resource IDs

	CString s,Resourceh = TheFrame->MainPathRoot + "\\PC VC Examples\\KMotionCNC\\resource.h";

	FILE *f = fopen(Resourceh,"rt");

	if (!f)
	{
		MessageBox(NULL,"Unable to open Screen resource.h file","KMotionCNC",MB_ICONSTOP|MB_OK);
		return 1;
	}

	Defines.RemoveAll();

	while(!feof(f))
	{
		fgets(s.GetBufferSetLength(1000),999,f);
		s.ReleaseBuffer();
		if (s.Find("#define")==0)
		{
			s.Delete(0,7);
			s=s.TrimLeft(' ');
			s=s.TrimLeft('\t');
			Defines.AddTail(s);
		}
	}
	Defines.AddTail("IDC_TOOLBAR 59392");  // note Windows forces this ID for Toolbars
	fclose(f);

	return 0;
}


CString CScreen::FindResourceName(int ID)
{
	int i, j, D_ID;
	CString s, r;

	if (ReadResourceIDs()) return "";  // read resource ID symbolic names/IDs (if not already)
	POSITION p = Defines.GetHeadPosition();
	for (i = 0; i<Defines.GetSize(); i++)
	{
		s = Defines.GetNext(p);
		for (j = 0; j < s.GetLength(); j++) if (s[j] == ' ' || s[j] == '\t') break; // find next space

		sscanf((const char *)s + j + 1, "%d", &D_ID);
		if (D_ID == ID)
		{
			return s.Mid(0, j);
		}
	}
	return "";
}

int CScreen::FindResourceIDs(CString Name)
{
	int i, j, ID;
	CString s, r;

	if (ReadResourceIDs()) return -1;  // read resource ID symbolic names/IDs (if not already)
	POSITION p = Defines.GetHeadPosition();
	for (i = 0; i<Defines.GetSize(); i++)
	{
		s = Defines.GetNext(p);
		for (j = 0; j < s.GetLength(); j++) if (s[j] == ' ' || s[j] == '\t') break; // find next space

		if (s.Mid(0, j) == Name)
		{
			sscanf((const char *)s + j + 1, "%d", &ID);
			return ID;
		}
	}
	return -1;
}

// extract a string into parts separated by semicolons
CString CScreen::Part(int n, CString p)
{
	CString s = "";
	for (int k = 0; k <= n; k++)
	{
		int i = p.Find(';');
		if (i < 0) i = p.GetLength();

		s = p.Mid(0, i);

		if (k<n && p.GetLength() > 0) p = p.Mid(i + 1, p.GetLength() - i - 1);
	}
	return s;
}

// extract a string into parts separated by semicolons
int CScreen::PartHex(int n, CString p)
{
	int i;	
	CString s = Part(n, p);
	if (sscanf(s.GetBuffer(), "%x", &i) != 1) i = 0;;
	return i;
}

// Change RGB to BGR 
int CScreen::SwapRGB(int n)
{
	return ((n & 0xff) << 16) | (n & 0xff00) | ((n & 0xff0000) >> 16);
}

bool CScreen::CheckForScreenEditorToolTip(UINT id, LPWSTR Tip)
{
	CImageButton *I;
	I = FindImageButton(id);
	if (I && I->ToolTipText != "")
	{
		wcscpy(Tip,I->ToolTipText);
		return true;
	}

	CEditScreen *E;
	E = FindEditScreen(id);
	if (E && E->ToolTipText != "")
	{
		wcscpy(Tip, E->ToolTipText);
		return true;
	}
	return false;
}

bool CScreen::CheckForScreenEditorHotKey(int VirtualKey, UINT *id, CImageButton **I)
{
	*I = FindImageButtonHotKey(VirtualKey);
	if (*I)
	{
		*id = (*I)->GetDlgCtrlID();

		return true;
	}

	return false;
}

// Find 3 Motion Buttons with the same HotKey B=slow B2=fast Bstep=step

bool CScreen::Find3MotionButtonsSameAxisDir(int axis, int dir, CMotionButton **B, CMotionButton **B2, CMotionButton **Bstep)
{
	POSITION position = CImageButton::ImageButtons.GetHeadPosition();
	LPCImageButton p;
	*B = *B2 = *Bstep = NULL;
	for (int i = 0; i < CImageButton::ImageButtons.GetCount(); i++)
	{
		p = CImageButton::ImageButtons.GetNext(position);

		// Found?
		if (p->Style == MotionButton)
		{
			CMotionButton *b = (CMotionButton*)p;
			if (b->m_axis == axis && b->m_dir == dir)
			{
				if (b->m_StepMode)
				{
					*Bstep = b;
				}
				else if (!*B)
				{
					*B = b;
				}
				else if (!*B2)
				{
					*B2 = b;
					if (*(*B)->m_mag > *(*B2)->m_mag)
					{
						CMotionButton *temp = *B;
						*B = *B2;
						*B2 = temp;
					}
				}
				if (*B && *B2 && *Bstep && (*B)->m_HotKey == (*B2)->m_HotKey && (*Bstep)->m_HotKey == (*B)->m_HotKey) return true;
			}
		}
	}

	return false;
}

int CScreen::GetEditScreenVar(int Var, CString *s)
{
	POSITION position = CEditScreen::EditScreens.GetHeadPosition();
	LPCEditScreen p;
	for (int i = 0; i<CEditScreen::EditScreens.GetCount(); i++)
	{
		p = CEditScreen::EditScreens.GetNext(position);

		// Found?
		if (p->Var == Var)
		{
			p->GetWindowTextA(*s);
			return 0;
		}
	}
	return 1;
}


// This seems to be the only way to simulate clicking on a Radio Button in a group
void CScreen::HandleRadioButton(CDialog *Dlg, int nIDC)
{
	HWND hWndCtrl,hWndCtrlSel;
	Dlg->GetDlgItem(nIDC, &hWndCtrl);
	hWndCtrlSel = hWndCtrl;

	// find first in group
	while ((::GetWindowLong(hWndCtrl, GWL_STYLE) & WS_GROUP) == 0)
	{
		hWndCtrl = ::GetWindow(hWndCtrl, GW_HWNDPREV);
	}

	// do all in group
	do
	{
		// clear all radio buttons in group except selected
		if (::SendMessage(hWndCtrl, WM_GETDLGCODE, 0, 0L) & DLGC_RADIOBUTTON)
			::SendMessage(hWndCtrl, BM_SETCHECK, (hWndCtrlSel == hWndCtrl), 0L);
		
		hWndCtrl = ::GetWindow(hWndCtrl, GW_HWNDNEXT);
	} while ((::GetWindowLong(hWndCtrl, GWL_STYLE) & WS_GROUP) == 0);
	Dlg->UpdateData();
}

void CScreen::Convert24to32(CImage *img)
{
	if (img->GetBPP() == 24)
	{
		CImage img32;
		img32.CreateEx(img->GetWidth(), img->GetHeight(), 32, BI_RGB, NULL, CImage::createAlphaChannel);

		// Copy the pixels:
		HDC const hdc = img32.GetDC();
		img->AlphaBlend(hdc, 0, 0, 0xff, AC_SRC_OVER);
		img32.ReleaseDC();
		img->Detach();
		img->Attach(img32);
		img32.Detach();

		// we loaded a 24bpp image set the alph channel to 0xFF
		BYTE *bits32 = (BYTE *)img->GetBits();
		for (int r = 0; r < img->GetHeight(); r++)
		{
			for (int c = 0; c < img->GetWidth(); c++)
			{
				int i = r*img->GetPitch() + c * 4;
				bits32[i + 3] = 0xFF;
			}
		}
	}
}
