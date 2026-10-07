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


int CScreen::ScriptReset()
{
	DeleteDlgControls();
	if (ReadResourceIDs()) return 1;  // read resource ID symbolic names/IDs

	CImageButton::ErrorDisplayed = false; // display first error message

	if (TheFrame->GCodeDlg.m_DialogFaceInUse != CUSTOM_DLG_FACE)
	{
		TheFrame->GCodeDlg.m_DialogFaceInUse = CUSTOM_DLG_FACE;
		TheFrame->GCodeDlg.DestroyWindow();
		TheFrame->GCodeDlg.Create(IDD_KMOTIONCNC_0_ORIGINAL + CUSTOM_DLG_FACE);  // put up the real main window
		TheFrame->GCodeDlg.m_LastFixtureDisplayed = TheFrame->GCodeDlg.m_LastToolDisplayed = -1;
	}

	ResetAllControls();
	return 0;
}


int CScreen::ProcessScript(CString file, int OffX, int OffY)
{
	bool SubScript = true;
	static int NestingLevel = 0;
	DLG_CONTROL *Dlg;
	CStringW sw;

	if (file == "") return 0;  // nothing specified do nothing

	AddRelPaths(file);  // if not absolute path find in default directories

	wchar_t wcsString[4001];

	if (CreateListOfAllControlsInScriptFile(file)) return 1;;


	// Open the file with the specified encoding
	// Some editors like NotePad++ don't put BOM Byte Order Mark so better to let Windows decide
	FILE *fStream;
	errno_t e = _tfopen_s(&fStream, file, _T("rt,ccs=UNICODE"));
	if (e != 0)  // failed..CString sRead;
	{
		MessageBoxW(NULL, TheFrame->KMotionDLL->Translate("Unable to open Screen Script file:\r\r") + (CStringW)file, L"KMotionCNC", MB_ICONSTOP | MB_OK);
		return 1;
	}


	CStringW sRead;

	Dlg = new DLG_CONTROL();

	bool bReadData, NewControl, FirstLine=true;

	do
	{
		bReadData = (NULL != fgetws(wcsString, 4000, fStream));

		if (bReadData)
		{
			sw = wcsString;

			if (FirstLine)
			{
				FirstLine = false;
				NestingLevel++;
				CStringW swl = sw;
				{
					if (swl.MakeLower().Find(L"subscript") == 0)
					{
						continue;
					}
					else
					{
						if (NestingLevel == 1)
						{
							SubScript = false;
							LastLoadedScreen = file;
							if (ScriptReset())
							{
								NestingLevel--;
								fclose(fStream);
								return 1;
							}
						}
					}
				}
			}

			int result = Execute(sw, Dlg, &NewControl, OffX, OffY);

			if (result)
			{
				fclose(fStream);
				if (result == 2)
					MessageBoxW(NULL, L"Screen Script file:\r\r" + (CStringW)file + /*TRAN*/TheFrame->KMotionDLL->Translate("\r\rUnable to find ID:\r\r") + (CStringW)Dlg->Name,
						L"KMotionCNC", MB_ICONSTOP | MB_OK);
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

	fclose(fStream);


	// After processing Screen check if any Labels have a Screen 
	// Script Specified if so execute the Screen Script Command

	for (POSITION p = DlgControls.GetHeadPosition(); p != NULL; )
	{
		DLG_CONTROL *Dlg = DlgControls.GetNext(p);
		if (Dlg->Style == Label)
		{
			bool NewControl = false;
			if (Dlg->Script != "")
				if (Execute(Dlg->Script, Dlg, &NewControl, OffX, OffY))
				{
					CString Script = Dlg->Script;
					MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Error Screen Load Label Action/Script/WinMsg.  ID:") + (CStringW) Dlg->Name + L"\r\r" + (CStringW)Script, 
						L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
				}
		}
	}


	if (--NestingLevel == 0 && !SubScript)
		TheFrame->GCodeDlg.Invalidate();


	return 0;
}


int CScreen::CreateListOfAllControlsInScriptFile(CString file)
{
	bool SubScript = true;
	static int NestingLevel = 0;
	wchar_t wcsString[4001];
	CStringW sw;
	DLG_CONTROL Dlg;

	// Open the file with the specified encoding
	// Some editors like NotePad++ don't put BOM Byte Order Mark so better to let Windows decide
	FILE* fStream;
	errno_t e = _tfopen_s(&fStream, file, _T("rt,ccs=UNICODE"));
	if (e != 0)  // failed..CString sRead;
	{
		MessageBoxW(NULL, TheFrame->KMotionDLL->Translate("Unable to open Screen Script file:\r\r") + (CStringW)file, L"KMotionCNC", MB_ICONSTOP | MB_OK);
		return 1;
	}


	CStringW sRead;

	bool bReadData, NewControl, FirstLine = true;

	DlgControlIDsFound.RemoveAll();

	do
	{
		bReadData = (NULL != fgetws(wcsString, 4000, fStream));

		if (bReadData)
		{
			sw = wcsString;

			if (FirstLine)
			{
				FirstLine = false;
				NestingLevel++;
				CStringW swl = sw;
				{
					if (swl.MakeLower().Find(L"subscript") == 0)
					{
						return 0;
					}
					else
					{
						if (NestingLevel == 1)
						{
							SubScript = false;
						}
					}
				}
			}

			int result = Execute(sw, &Dlg, &NewControl, 0, 0);

			if (result)
			{
				fclose(fStream);
				if (result == 2)
					MessageBoxW(NULL, L"Screen Script file:\r\r" + (CStringW)file + /*TRAN*/TheFrame->KMotionDLL->Translate("\r\rUnable to find ID:\r\r") + (CStringW)Dlg.Name,
						L"KMotionCNC", MB_ICONSTOP | MB_OK);
				--NestingLevel;
				return 1;
			}

			if (NewControl)
			{
				DlgControlIDsFound.AddTail(Dlg.ID);
			}
		}
	} while (bReadData);


	fclose(fStream);
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

int CScreen::Execute(CStringW s, DLG_CONTROL *Dlg, bool *NewControl, int OffX, int OffY)
{
	*NewControl = false;

	if (s != "")
	{
		if (s.Find(L"ID:") == 0)
		{
			return DoControlID(s, Dlg, NewControl, OffX, OffY);
		}
		else if (s.Find(L"Main:") == 0)
		{
			return DoMainDlg(s);
		}
		else if (s.Find(L"SScript:") == 0)
		{
			return DoSScript(s, OffX, OffY);
		}
		else if (s.Find(L"Action:") == 0)
		{
			return DoAction(s);
		}
		else if (s.Find(L"WinMsg:") == 0)
		{
			return DoWinMsg(s);
		}
		else if (s.Find(L"ScriptName:") == 0)
		{
			return DoScriptName(s);
		}
		else if (s.Find(L"#") == 0) // comment?  ignore
		{
			return 0;
		}
	}
	return 0;
}

int CScreen::DoSScript(CStringW s, int OffX, int OffY)
{
	CString FileName;
	int NX, NY;

	if (ParseString(s, "SScript:", FileName)) return 1;
	if (s.GetLength() > 2)  // XY is optional
	{
		if (ParseInt(s, "X:", NX)) return 1;
		if (ParseInt(s, "Y:", NY)) return 1;
		OffX += NX;
		OffY += NY;
	}

	return ProcessScript(FileName, OffX, OffY);
}

int CScreen::DoWinMsg(CStringW s)
{
	CString Msg,DlgName;

	if (ParseString(s, "WinMsg:", Msg)) return 1;


	HWND Foreground = GetForegroundWindow();
	HWND hh = TheFrame->GCodeDlg.m_hWnd;

	if (Foreground != hh)
	{
		::SetForegroundWindow(hh);
	}

		
	CString type = Part(0, Msg);
	if (type == "DlgName")
	{
		CString name = Part(1, Msg);
		int ID = FindResourceIDs(name);
		if (ID == -1)
		{
			MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Invalid WinMsg ID Name: ") + (CStringW) name, L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
			return 1;
		}

		int Message, wParam;

		CString MessageCode = Part(2, Msg);
		if (MessageCode == "SET_FOCUS")
		{
			Message = -1;  // treated special
			wParam = 0;
		}
		else if (MessageCode == "BM_CLICK")
		{
			Message = BM_CLICK;
			wParam = 0;
		}
		else if (MessageCode == "BM_SETCHECK")
		{
			Message = BM_SETCHECK;
			CString Param = Part(3, Msg);
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
				MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Invalid WinMsg Message Param : ") + (CStringW) Param, L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
				return 1;
			}
		}
		else
		{
			MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Invalid WinMsg Message Code : ") + (CStringW) MessageCode, L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
			return 1;
		}

		HWND hDlg = GetDlgItem(TheFrame->GCodeDlg.m_hWnd, ID);
		CWnd* pWnd = CWnd::FromHandle(hDlg);

		if (Message == -1)  // Set_FOCUS treated special
		{
			TheFrame->GCodeDlg.GotoDlgCtrl(pWnd);  // set the focus

			if (pWnd == (CWnd*)(&TheFrame->GCodeDlg.m_Editor)) // special case for editor window restore the last selection
			{
				TheFrame->GCodeDlg.m_Editor.SetFirstVisibleLine(TheFrame->GCodeDlg.EditorCurrentLine[TheFrame->GCodeDlg.m_Thread]);
				TheFrame->GCodeDlg.m_Editor.SetSelectionStart(TheFrame->GCodeDlg.SelectionStart[TheFrame->GCodeDlg.m_Thread]);
				TheFrame->GCodeDlg.m_Editor.SetSelectionEnd(TheFrame->GCodeDlg.SelectionEnd[TheFrame->GCodeDlg.m_Thread]);
			}
		}
		else
		{
			SendMessage(hDlg, Message, wParam, 0);
			TheFrame->GCodeDlg.UpdateData();
		}
	}
	else if (type == "Keyboard")
	{
		int nParam;
		WORD Key;
		CString MessageCode = Part(2, Msg);
		CString Param = Part(3, Msg);
		sscanf(Param, "%d", &nParam);

		bool ShiftWasDown = (GetKeyState(VK_SHIFT) & (1<<31)) != 0;
		bool ControlWasDown = (GetKeyState(VK_CONTROL) & (1<<31)) != 0;
		bool Shift = (nParam & 0x10000) != 0;
		bool Control = (nParam & 0x20000) != 0;
		
		Key = nParam & 0xffff;

		if (Shift) // shift?
		{
			if (!ShiftWasDown) KeyDown(VK_SHIFT);
		}
		else
		{
			if (ShiftWasDown) KeyUp(VK_SHIFT);
		}


		if (Control) // ctrl?
		{
			if (!ControlWasDown) KeyDown(VK_CONTROL);
		}
		else
		{
			if (ControlWasDown) KeyUp(VK_CONTROL);
		}

		if (MessageCode == "KeyPress")
		{
			KeyPress(Key);
		}
		else if (MessageCode == "KeyDown")
		{
			KeyDown(Key);
		}
		else if (MessageCode == "KeyUp")
		{
			KeyUp(Key);
		}
		else
		{
			MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Invalid WinMsg Message Code : ") + (CStringW) MessageCode, L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
			return 1;
		}

		if (ShiftWasDown) // shift?
		{
			if (!Shift) KeyDown(VK_SHIFT);
		}
		else
		{
			if (Shift) KeyUp(VK_SHIFT);
		}


		if (ControlWasDown) // ctrl?
		{
			if (!Control) KeyDown(VK_CONTROL);
		}
		else
		{
			if (Control) KeyUp(VK_CONTROL);
		}
	}
	else
	{
		MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Invalid WinMsg Type: ") + (CStringW) type, L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
		return 1;
	}


	return 0;
}

int CScreen::KeyPress(WORD vkey)
{
	KeyDown(vkey);
	return KeyUp(vkey);
}

int CScreen::KeyDown(WORD vkey)
{
	INPUT input;
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MAKELONG(vkey, MapVirtualKey(vkey, 0));
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
	SendInput(1, &input, sizeof(INPUT));
	return 0;
}

int CScreen::KeyUp(WORD vkey)
{
	INPUT input;
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MAKELONG(vkey, MapVirtualKey(vkey, 0));
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = KEYEVENTF_KEYUP; // 
	SendInput(1, &input, sizeof(INPUT));
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
		MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Invalid Action Code"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
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
				MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Invalid Action Parameter"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
				return 1;
			}
		}
	}
	CString Name = Part(6, Action);

	AddRelPaths(Name);

	strncpy(MCodeAction.String, Name, sizeof(MCodeAction.String));

	return TheFrame->GCodeDlg.Interpreter->InvokeAction(-1, false, &MCodeAction);
}

int CScreen::DoMainDlg(CStringW s)
{
	bool CaptionSpecified = false;

	if (ParseInt(s, "Main:CX:", MainCX)) return 1;
	if (ParseInt(s, "CY:", MainCY)) return 1;
	if (ParseIntHex(s, "BackColor:", MainBackColor)) return 1;

	// Check for backward compatibilty
	if (s.Find(L"Caption:") == 0 && ParseInt(s, "Caption:", Caption, true) == 0)
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
	AddRelPaths(BGFile);

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
				MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Can't open Background image:") + (CStringW) BGFile, L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
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

int CScreen::DoScriptName(CStringW s)
{
	if (ParseString(s, "ScriptName:", LastLoadedScreen, true)) return 1;
	return 0;
}

int CScreen::DoControlID(CStringW s, DLG_CONTROL *Dlg, bool *NewControl, int OffX, int OffY)
{
	int ishow, iBold, iItalic, ID;
	CString Name;
// ID:IDC_ZeroAll,Type:PUSHBUTTON,X:11,Y:211,CX:54,CY:23,Show:1,Var:0,BitmapFile:,Colors:,Font:,FontSize:,HotKey:,Text:zero all
	if (ParseString(s, "ID:", Name)) return 1;
	ID = FindResourceIDs(Name);
	if (Dlg->ID == -1) return 2;

	DLG_CONTROL *DlgOrig = FindDlgControl(ID);
	if (DlgOrig != NULL) // Already Exist?
	{
		*NewControl = false;  // modify existing
		Dlg = DlgOrig;
	}
	else
	{
		*NewControl = true;  // adding new
		Dlg->ID = ID;
		Dlg->Name = Name;
	}

	ishow = Dlg->show ? 1 : 0;
	iBold = Dlg->Bold ? 1 : 0;
	iItalic = Dlg->Italic ? 1 : 0;

	int PrevLength;
	bool XSpecified = false, YSpecified = false;
	do  // loop to allow parameters in any order
	{
		PrevLength = s.GetLength();

		if (ParseString(s, "Type:", Dlg->Type, true) == 1) return 1;
		if (s.Find(L"X:") == 0) XSpecified = true;
		if (ParseInt(s, "X:", Dlg->x, true) == 1) return 1;
		if (s.Find(L"Y:") == 0) YSpecified = true;
		if (ParseInt(s, "Y:", Dlg->y, true) == 1) return 1;
		if (ParseInt(s, "CX:", Dlg->width, true) == 1) return 1;
		if (ParseInt(s, "CY:", Dlg->height, true) == 1) return 1;
		if (ParseInt(s, "Show:", ishow, true) == 1) return 1;
		Dlg->show = ishow == 1;
		if (ParseInt(s, "Var:", Dlg->Var, true) == 1) return 1;
		if (ParseInt(s, "Style:", Dlg->Style, true) == 1) return 1;
		if (ParseString(s, /*TRAN*/"BitmapFile:", Dlg->BitmapFile, true) == 1) return 1;
		if (ParseString(s, "Colors:", Dlg->Colors, true) == 1) return 1;
		if (ParseString(s, "Font:", Dlg->FontName, true) == 1) return 1;
		if (ParseInt(s, "FontSize:", Dlg->FontSize, true) == 1) return 1;
		if (ParseInt(s, "HotKey:", Dlg->HotKey, true) == 1) return 1;
		if (ParseInt(s, "Bold:", iBold, true) == 1) return 1;
		Dlg->Bold = iBold == 1;
		if (ParseInt(s, "Italic:", iItalic, true) == 1) return 1;
		Dlg->Italic = iItalic == 1;
		if (ParseString(s, "Text:", Dlg->Text, true) == 1) return 1;
		if (ParseString(s, "ToolTipText:", Dlg->ToolTipText, true) == 1) return 1;
		if (ParseString(s, "Script:", Dlg->Script, true) == 1) return 1;
	} while (s.GetLength() != PrevLength);

	if (XSpecified) Dlg->x += OffX;  // Add in Offsets if they were specified, otherwise leave original
	if (YSpecified) Dlg->y += OffY;


	if (s.GetLength() != 0)
	{
		MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Invalid Screen Script Format. \r\r") + s, L"KMotionCNC", MB_ICONSTOP | MB_OK);
		return 1;
	}


	DoCompatibility(Dlg);
	
	CWnd *W = TheFrame->GCodeDlg.GetDlgItem(Dlg->ID);

	if (!W)
	{
		MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Error Screen Script File.  Screen Control not found.\r\r") + (CStringW)Dlg->Name,
			L"KMotionCNC", MB_ICONSTOP | MB_OK);
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

		AddRelPaths(Part0);
		AddRelPaths(Part1);

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
		if (I && I->Style == DROLabel && Dlg->Text == "$KEEP$")
		{
			I->SetText(I->m_szText);
		}
		else 
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
		else if (C)  // Combo Box Dropdown list
		{
			C->SetTextAndDropDown(Dlg->Text);
		}
		else
		{
			SetDlgItemTextW(TheFrame->GCodeDlg.m_hWnd, Dlg->ID, Dlg->Text);
		}
	}


	return 0;
}

void CScreen::AddRelPaths(CString &FileName)
{
	if (FileName != "" && FileName.Find("\\\\") == -1 && FileName.Find(':') == -1)
	{
		// Determine from the Previous FileName (if any), the default subdirectory path, and a default name
		// return the full path to be used to divert the Open Dialog to the right place
		//
		// if no path specified try:
		// #1 the directory of the current Screen Script file
		// #2 all subfolders of the current Screen Script file
		// #3 MainPath\\KMotionCNC\\Screens\\
		// #4 MainPath\\KMotionCNC\\res\\
        // #5 MainPath\\C Programs\\

		CString ScreenPath, Name;

		int LastBackslash = LastLoadedScreen.ReverseFind('\\');

		if (LastBackslash != -1)
		{
			ScreenPath = LastLoadedScreen.Left(LastBackslash+1);

//			Name = ScreenPath + FileName; // yes, add in the default path
//			if (CheckIfFileExists(Name))
//			{
//				FileName = Name;
//				return;
//			}

			CString Path = Recurse(ScreenPath, FileName);
			if (Path != "")
			{
				FileName = Path + FileName;
				return;
			}
		}

		Name = TheFrame->MainPathRoot + SCREEN_SCRIPTS_DIR + FileName;
		if (CheckIfFileExists(Name))
		{
			FileName = Name;
			return;
		}

		Name = TheFrame->MainPathRoot + SCREEN_BITMAPS_DIR + FileName;
		if (CheckIfFileExists(Name))
		{
			FileName = Name;
			return;
		}

		Name = TheFrame->MainPathRoot + C_PROGRAMS_DIR + FileName;
		if (CheckIfFileExists(Name))
		{
			FileName = Name;
			return;
		}
	}
}

// search for File in directory pstr and all subdirectories

CString CScreen::Recurse(CString pstr, CString File)
{
	CFileFind finder;

	if (pstr == "") return "";

	if (CheckIfFileExists(pstr + File))
		return pstr;

	// build a string with wildcards
	CString strWildcard(pstr);
	strWildcard += _T("*.*");  // don't add slash

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);
	CString result;

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
		// recur infinitely!

		if (finder.IsDots())
			continue;

		// if it's a directory, recursively search it

		if (finder.IsDirectory())
		{
			CString str = finder.GetFilePath() + '\\';
			result = Recurse(str, File);
			if (result != "")
				break;
		}
	}

	finder.Close();
	return result;
}

bool CScreen::CheckIfFileExists(CString Name)
{
	FILE *fp = fopen(Name, "rb");
	if (fp)
	{
		fclose(fp);
		return true;
	}
	return false;
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
//	POSITION position = CImageButton::ImageButtons.GetHeadPosition();
//	for (int i = 0; i<CImageButton::ImageButtons.GetCount(); i++)
//		CImageButton::ImageButtons.GetNext(position)->Reset(true);


	int k;
	POSITION position = CImageButton::ImageButtons.GetHeadPosition();

	for (int i = 0; i < CImageButton::ImageButtons.GetCount(); i++)
	{
		CImageButton* I = CImageButton::ImageButtons.GetNext(position);
		if (::IsWindow(I->m_hWnd))
		{
			int ID = I->GetDlgCtrlID();

			POSITION positionFound = DlgControlIDsFound.GetHeadPosition();
			for (k = 0; k < DlgControlIDsFound.GetCount(); k++)
				if (ID == DlgControlIDsFound.GetNext(positionFound)) break;

			// was it found?
			if (k < DlgControlIDsFound.GetCount())
				I->Reset(true);			// Yes, Reset
		}
	}

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

CComboBoxScreen* CScreen::FindComboBoxScreenFromHandle(HWND w)
{
	POSITION position = CComboBoxScreen::ComboBoxScreens.GetHeadPosition();
	LPCComboBoxScreen p;
	for (int i = 0; i < CComboBoxScreen::ComboBoxScreens.GetCount(); i++)
	{
		p = CComboBoxScreen::ComboBoxScreens.GetNext(position);

		// Found?

		if (p->m_hWnd)
		{
			CWnd *wndc = p->GetWindow(GW_CHILD);
			if (wndc && wndc->m_hWnd == w) return p;

			wndc = p->GetEditCtrl();
			if (wndc && wndc->m_hWnd == w) return p;
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
	for (int i=0; i<CImageButton::ImageButtons.GetCount(); i++)
	{
		LPCImageButton p = CImageButton::ImageButtons.GetNext(position);

		if (p->Var != -1 && p->m_hWnd != NULL)
		{
			if (p->Style == ToggleButton)
			{
				bool state = GetStatusBit(p->Var);
				
				if (p->Toggled != state)
				{
					p->Toggled = state;
					p->Invalidate(FALSE);
				}
			}
			else if (p->Style == VertBar)
			{
					int value;
				int result = TheFrame->GCodeDlg.GetVar(p->Var, &value);

				if (result)
				{
					MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Error Reading Persist Variable for Vert Bar Graph"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
					return 1;
				}

				if (p->Value != *(float*)&value)
				{
					p->Value = *(float*)&value;
					p->Invalidate(FALSE);
				}
			}
			else if (p->Style == DROLabel)
			{
				int value;
				int result = TheFrame->GCodeDlg.GetVar(p->Var, &value);

				if (result)
				{
					MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Error Reading Persist Variable for DRO"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
					return 1;
				}
				
				if (value != 0)
				{
					CString s;
					result = TheFrame->GCodeDlg.GetStringFromGather(value, &s, 50);
					if (result)
					{
						MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Error Reading String for DRO"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
						return 1;
					}
					if (TheFrame->GCodeDlg.SetVar(p->Var, 0)) return 1;
					CStringW sw = ConvertANSIToWide(s);
					p->SetText(sw.GetBuffer());
					p->Invalidate(TRUE);
				}

			}
			else if (p->Style == Label)
			{
				// Toggle the button based on the specified Bit
				bool state = GetStatusBit(p->Var);
				if (p->DrawPushed != state)
				{
					p->DrawPushed = state;
					p->Invalidate(FALSE);
				}
			}
		}
	}

	// Edit Control text can be written to by placing a negative gather buffer offset into the persist variable
	position = CEditScreen::EditScreens.GetHeadPosition();
	for (int i = 0; i<CEditScreen::EditScreens.GetCount(); i++)
	{
		LPCEditScreen p = CEditScreen::EditScreens.GetNext(position);

		if (p->Var != -1 && p->m_hWnd != NULL)
		{
			int value;
			int result = TheFrame->GCodeDlg.GetVar(p->Var, &value);

			if (result)
			{
				MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Error Reading Persist Variable for Edit Control"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
				return 1;
			}

			if (value < 0)
			{
				CString s;
				result = TheFrame->GCodeDlg.GetStringFromGather(-value, &s, 50);
				if (result)
				{
					MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Error Reading String for Edit Control"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
					return 1;
				}
				if (TheFrame->GCodeDlg.SetVar(p->Var, 0)) return 1;
				CStringW sw = ConvertANSIToWide(s);
				p->SetWText(sw.GetBuffer());
				p->Invalidate(TRUE);
			}
		}
	}

	// Combo Box Control text can be written to by placing a negative gather buffer offset into the persist variable
	position = CComboBoxScreen::ComboBoxScreens.GetHeadPosition();
	for (int i = 0; i<CComboBoxScreen::ComboBoxScreens.GetCount(); i++)
	{
		LPCComboBoxScreen p = CComboBoxScreen::ComboBoxScreens.GetNext(position);

		if (p->Var != -1 && p->m_hWnd != NULL)
		{
			int value;
			int result = TheFrame->GCodeDlg.GetVar(p->Var, &value);

			if (result)
			{
				MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Error Reading Persist Variable for Edit Control"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
				return 1;
			}

			if (value < 0)
			{
				CString s;
				result = TheFrame->GCodeDlg.GetStringFromGather(-value, &s, 50);
				if (result)
				{
					MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Error Reading String for Edit Control"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
					return 1;
				}
				if (TheFrame->GCodeDlg.SetVar(p->Var, 0)) return 1;
				CStringW sw = ConvertANSIToWide(s);
				p->SetWText(sw.GetBuffer());
				p->Invalidate(TRUE);
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
		return (MS->VirtualBits & (1 << ((bit - 168 + 16) & 31))) != 0;

	if (bit < 290) // Kogna Inputs/Outputs
		return (MS->BitsState200[(bit-200)>>5] & (1 << ((bit - 200) & 31))) != 0;

	if (bit < 1056) // KStep Inputs
		return (MS->VirtualBitsEx0 & (1<<((bit-1024)&31)))!=0;

	// If main frame couldn't connect then don't bother
	if (!TheFrame->GCodeDlg.m_ConnectedForStatus) return false;

	if (bit >= 1024 && bit < 2047) ReadBitCached(bit);

	return false;
}

#define CACHE_BIT_TIME 0.1  // read bits if more than this time elapsed
bool CScreen::ReadBitCached(int bit)
{
	static int VirtualBitsEx[N_VIRTUAL_BITS_EX / 32];	// 1024 Expanded Virtual Bits 
	static CHiResTimer Timer;
	CString ch;
	char  r[MAX_LINE];

	// check if its been a while since we read them
	if (Timer.nSplit == 0 || Timer.Elapsed_Seconds() > CACHE_BIT_TIME)
	{
		ch.Format("GETVIRTUALBITS%d %d", 0, N_VIRTUAL_BITS_EX / 32);  // read all virtual extended bits

		if (TheFrame->KMotionDLL->WriteLineReadLine(ch, r)) return false;
		Timer.Start();  // start timer
		char* s = r;
		for (int i = 0; i < N_VIRTUAL_BITS_EX / 32; i++)
		{
			if (sscanf(s, "%x", VirtualBitsEx + i) != 1) return false;
			if (i != N_VIRTUAL_BITS_EX / 32)
				s = strchr(s, ' ') + 1;
		}
	}
	// get the desired bit

	return ((VirtualBitsEx[bit / 32] >> (bit % 32)) & 1) == 1;
}
// return 0 sucess with data
// return 1 not found with error
// return 2 not found error suppressed

int CScreen::ParseString(CStringW &s, const CString label0, CString &r, bool NoErrors)
{
	CStringW wr;
	int result = ParseString(s, label0, wr, NoErrors);

	if (!result)
		r = wr;

	if (NoErrors)
		return 0;
	else
		return result;
}

// return 0 sucess with data
// return 1 not found with error
// return 2 not found error suppressed

int CScreen::ParseString(CStringW &s, const CString label0, CStringW &r, bool NoErrors)
{
	CStringW label = label0;

	if (s.Find(label) != 0)
	{
		if (!NoErrors)
			MessageBoxW(NULL, TheFrame->KMotionDLL->Translate("Error Screen Script File.  Expecting:") + label + "\r\rFound:" + s, L"KMotionCNC", MB_ICONSTOP | MB_OK);
		return NoErrors ? 2 : 1;
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

	int result = ParseString(s, label, v, NoErrors);   // must find if error then don't convert

	if (result && !NoErrors) return 1;

	if (v.IsEmpty()) return NoErrors ? 0 : 1;

	CString va = v;
	if (sscanf(va, "%d", &r) != 1)
	{
		MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Invalid Screen Script integer for ") + (CStringW)label, 
			L"KMotionCNC", MB_ICONSTOP | MB_OK);
		return 1;
	}

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
		MessageBoxW(NULL,/*TRAN*/TheFrame->KMotionDLL->Translate("Unable to open Screen resource.h file"), L"KMotionCNC",MB_ICONSTOP|MB_OK);
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

// Find a Control with the associated Var number
// if found, return current text

int CScreen::GetEditScreenVar(int Var, CString *s)
{
	POSITION position = CEditScreen::EditScreens.GetHeadPosition();
	for (int i = 0; i<CEditScreen::EditScreens.GetCount(); i++)
	{
		LPCEditScreen p = CEditScreen::EditScreens.GetNext(position);

		// Found?
		if (p->Var == Var)
		{
			*s = ConvertWideToANSI(p->GetWText());  
			return 0;
		}
	}

	position = CComboBoxScreen::ComboBoxScreens.GetHeadPosition();
	for (int i = 0; i<CComboBoxScreen::ComboBoxScreens.GetCount(); i++)
	{
		LPCComboBoxScreen p = CComboBoxScreen::ComboBoxScreens.GetNext(position);

		// Found?
		if (p->Var == Var)
		{
			*s = ConvertWideToANSI(p->GetWText()); 
			return 0;
		}
	}
	return 1;
}

// Attempt to convert wide string to ANSI string if not possible include hex value instead 
CString CScreen::ConvertWideToANSI(CStringW sw)
{
	CString s, Result;
	CStringW w;

	for (int i = 0; i < sw.GetLength(); i++)
	{
		s = sw[i];  // attempt to convert a character

		if (s[0] == sw[i])  // is it the same as before?
		{
			// yes, copy over the character
			Result += s;
		}
		else
		{
			// no, convert to hex

			wchar_t wc = sw[i];
			unsigned char *p = (unsigned char*)&wc;

			Result += "\\x";
			for (int i = sizeof(wc) - 1; i >= 0; i--)  // do little endian
			{
				s.Format("%02x", p[i]);
				Result += s;
			}
		}
	}
	return Result;
}

// Attempt to convert ANSI string to wide string looking for embedded hex wide characters 
CStringW CScreen::ConvertANSIToWide(CString s)
{
	CStringW Result;
	
	int i = 0;
	while (s[i] != 0)
	{
		// must have at least 6 characters and start with "\x"
		if (i + 6 <= s.GetLength() && s[i] == '\\' && s[i+1] == 'x')
		{
			int k;
			unsigned int v;
			wchar_t wc;
			unsigned char *p = (unsigned char*)&wc;

			for (k = 0; k < sizeof(wc); k++) 
			{
				CString t = s.Mid(i + 2 + k*2, 2);  // two hex digits per byte
				sscanf(t, "%x", &v);
				p[sizeof(wc) - 1 - k] = (unsigned char)v; // little endian put the bytes in reverse order
			}
			
			Result += wc;  // put the wide character
			i += 2 + k * 2;
		}
		else
		{
			// simply convert/copy character
			Result += s[i++];
		}
	}
	return Result;
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



CStringW CScreen::GetPersistText(CString IDNameToFind)
{
	static bool FirstMessage = true;  // only display first error

	// Open the file with the specified encoding
	FILE *fStream;
	CString file = TheFrame->MainPathRoot + EDIT_CONTROL_PERSIST_FILE;
	errno_t e = _tfopen_s(&fStream, file, _T("rt,ccs=UNICODE"));
	if (e != 0)  // failed..CString sRead;
	{
		if (FirstMessage)
		{
			FirstMessage = false;
			MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Unable to open Screen Script file:\r\r") + (CStringW)file, 
				L"KMotionCNC", MB_ICONSTOP | MB_OK);
		}
		return "";
	}

	CStringW s;
	while (!feof(fStream))
	{
		CString IDName;

		fgetws(s.GetBufferSetLength(1001), 1000, fStream);
		s.ReleaseBuffer();
		int k = s.Find(':');
		if (k == -1) k = s.GetLength();
		IDName = s.Mid(0, k);

		if (IDName == IDNameToFind)
		{
			s.Delete(0, k);
			if (s[0] == ':') s.Delete(0, 1);
			s.Remove('\r');
			s.Remove('\n');
			fclose(fStream);
			return s;
		}
	}
	fclose(fStream);
	return "";  // not found return blank
}

