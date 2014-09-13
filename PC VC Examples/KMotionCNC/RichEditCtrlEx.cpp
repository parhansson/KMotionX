// RichEditCtrlEx.cpp : implementation file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/

#include "stdafx.h"
#include "RichEditCtrlEx.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define tabsize 4


const char cKeyWords[] = 
  "break "
  "case char const continue "
  "default do double else enum extern float for "
  "goto if inline int long "
  "register return short signed sizeof static struct switch "
  "typedef union unsigned "
  "void volatile while ";

const char GKeyWords[] = 
  "G0 G2 G3 M2 F ";

	
static UINT WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);

/////////////////////////////////////////////////////////////////////////////
// CRichEditCtrlEx

CRichEditCtrlEx::CRichEditCtrlEx()
{
	m_pFindReplaceDlg = NULL;
	m_bFindOnly = TRUE;
	m_bCase = FALSE;
	m_bNext = TRUE;
	m_bWord = FALSE;
	m_bFirstSearch = TRUE;
	m_bRegularExpression = FALSE;
	m_SingleLineOnly=FALSE;
	m_esc_pushed=FALSE;
	CtrlIsDown=ShiftIsDown=FALSE;
	ModeCode=-1;
}

CRichEditCtrlEx::~CRichEditCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CRichEditCtrlEx, CScintillaCtrl)
	//{{AFX_MSG_MAP(CRichEditCtrlEx)
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_COMMAND(ID_Undo, OnUndo)
	ON_COMMAND(ID_Redo, OnRedo)
	ON_COMMAND(ID_Cut, OnCut)
	ON_COMMAND(ID_Copy, OnCopy)
	ON_COMMAND(ID_Paste, OnPaste)
	ON_COMMAND(ID_Delete, OnDelete)
	ON_COMMAND(ID_SelectAll, OnSelectAll)
	ON_COMMAND(ID_Find, OnFind)
	ON_COMMAND(ID_Replace, OnReplace)
	ON_COMMAND(ID_SpecialContext, OnSpecialContext)
	ON_COMMAND(ID_TransformSel, OnTransformSel)
	ON_COMMAND(ID_ShowLineNumbers, OnShowLineNumbers)
    ON_REGISTERED_MESSAGE( WM_FINDREPLACE, OnFindReplaceCmd )
	//}}AFX_MSG_MAP
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRichEditCtrlEx message handlers


CString StructList="ChanNumber Enable InputMode OutputMode LimitSwitchOptions "
"LimitSwitchNegBit LimitSwitchPosBit "
"MaxFollowingError LastFollowingError t Dest DestOffset Vel Accel Jerk FFAccel "
"SoftLimitNeg SoftLimitPos "
"FFVel Position invDistPerCycle StepperAmplitude Output Lead last_dest prev_dest "
"last_vel OutputChan0 OutputChan1 InputChan0 InputChan1 InputOffset0 InputGain0 "
"OutputGain OutputOffset "
"InputOffset1 InputGain1 CommutationOffset last_theta last_enc MaxI MaxErr "
"BacklashAmount BacklashRate BacklashMode "
"integrator MaxOutput DeadBandRange DeadBandGain MasterAxis SlaveGain ";

CString GlobalList="ENCODER_MODE ADC_MODE RESOLVER_MODE USER_INPUT_MODE BACKLASH_OFF BACKLASH_LINEAR "
"MICROSTEP_MODE DC_SERVO_MODE BRUSHLESS_3PH_MODE BRUSHLESS_4PH_MODE DAC_SERVO_MODE "
"DAC ADC WritePWMR WritePWMC WritePWM Write3PH Write4PH Time_sec WaitUntil Delay_sec "
"WaitNextTimeSlice ServoTick chan ch0-> ch1-> ch2-> ch3-> ch4-> ch5-> ch6-> ch7-> "
"DisableAxis EnableAxisDest EnableAxis ResetFilters Zero Move MoveExp MoveAtVel MoveRel MoveRelAtVel "
"Jog CheckDone MoveXYZABC CheckDoneXYZABC CheckDoneBuf CheckDoneGather "
"StartMove DefineCoordSystem DefineCoordSystem6 StopMotion SetBitDirection GetBitDirection "
"SetBit ClearBit SetStateBit ReadBit printf sprintf InitAux AddKonnect AddKonnect_Aux0 "
"StartThread PauseThread ResumeThread ThreadDone MutexLock MutexUnlock "
"VersionAndBuildTime WriteSnapAmp ReadSnapAmp StopCoordinatedMotion "
"ResumeCoordinatedMotion ClearStopImmediately StatusRequestCounter ";


void CRichEditCtrlEx::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	int nItems;
	bool ListFound=false;
	char linebuf[1000];
	CString List,s;

	if (nChar == VK_SHIFT)
		ShiftIsDown=false;

	if (nChar == VK_CONTROL)
		CtrlIsDown=false;

	if (nChar == VK_ESCAPE)
	{
		m_esc_pushed=true;
		m_t0_esc_time=timeGetTime();
	}

	int  current  =  GetCurLine(sizeof(linebuf),linebuf);
	int  pos  =  GetCurrentPos();

	CScintillaCtrl::OnKeyUp(nChar, nRepCnt, nFlags);


	if  (ModeCode==MODE_C && nChar == VK_OEM_6)  // check for '}'  
	{
		char  linebuf[1000];
		int  curLine  =  LineFromPosition(pos);
		int  lineLength  =  LineLength(curLine);
		
		if  (lineLength  <  sizeof(linebuf))  
		{
			GetLine(curLine, linebuf);
			linebuf[lineLength]  =  '\0';
			
			// The user just typed a '}'
			// Search through the line until we find the bracket
			// then remove a previous tab if present

			for  (int i  =  1;  linebuf[i];  i++)  
			{
				if  (linebuf[i-1]  ==  '\t' &&
					 linebuf[i]    ==  '}'  &&
					 linebuf[i+1]  ==  '\r' &&
					 linebuf[i+2]  ==  '\n' &&
					 linebuf[i+3]  ==  '\0')
				{
					linebuf[0]   =  '}';
					linebuf[1]   =  '\0';
					SetSel(pos-2,pos);
					ReplaceSel(linebuf);
					break;
				}
			}
		}
	}

	if  (ModeCode==MODE_C && nChar == VK_RETURN)  
	{
		char  linebuf[1000];
		int  curLine  =  LineFromPosition(pos);
		int  lineLength  =  LineLength(curLine);
		
		if  (curLine  >  0  &&  lineLength  <=  2)  
		{
			int  prevLineLength  =  LineLength(curLine  -  1);

			if  (prevLineLength  <  sizeof(linebuf))  
			{
				WORD  buflen  =  sizeof(linebuf);
				
				memcpy(linebuf,  &buflen,  sizeof(buflen));
				
				GetLine(curLine -1, linebuf);

				linebuf[prevLineLength]  =  '\0';
				
				// go through the copied line until we detect
				// a non white space character.  Also if we
				// find an open curly bracket replace it 
				// with a tab and terminate after it

				for  (int  i  =  0;  linebuf[i];  i++)  
				{
					if  (linebuf[i]  ==  '{')
					{
						linebuf[i]   =  '\t';
						linebuf[i+1] =  '\0';
						break;
					}
					if  (linebuf[i]  !=  ' '  &&  linebuf[i]  !=  '\t')
					{
						linebuf[i]  =  '\0';
						break;
					}
				}
				
				ReplaceSel(linebuf);
			}
		}
	}



/*
	if (ModeCode==MODE_CMD)
	{
		bool FoundExactMatch=false;

		// See if we know the command he just typed
		
		int  startword  =  current;

		// skip backwards over identifier

		while  (startword  >  0  &&  isalpha_numeric(linebuf[startword  -  1]))
			startword--;
    
		linebuf[current]  =  '\0';

		char*  word  =  linebuf  +  startword;
		int word_length = current-startword;

		POSITION listpos = GlobalFuncsNames.GetHeadPosition();

		int index=0;
		while (listpos)
		{
			if (GlobalFuncsNames.GetNext(listpos) == word)
			{
				// found a word, display tip
				
//				CallTipShow(pos,GlobalFuncsDefs.GetAt(GlobalFuncsDefs.FindIndex(index)));
				FoundExactMatch=true;
				break;
			}
			index++;
		}


//		if (!CallTipActive())
		{
			nItems=DoAutoComplete(word,List,CMDList);

			if (nItems>0)
			{
				List.TrimRight();

				if (nItems>1 || List!=word)  // don't show if only one choice exatly matches
				{
					AutoCSetIgnoreCase(true);
					AutoCShow(word_length,List);
					ListFound=true;
				}
			}

			if (!ListFound)
			{
				AutoCCancel();
			}
		}
	}
	else 
*/
	
	if (ModeCode==MODE_C && nChar == '9' && ShiftIsDown)
	{
		// See if we know the global function he just typed

		
		int  startword  =  current;

		// skip backwards over '('

		while  (startword  >  0  &&  linebuf[startword  -  1] == '(')
			startword--;
    
		// skip backwards over white space

		while  (startword  >  0 && iswhitespace(linebuf[startword  -  1]))
			startword--;

		current = startword;

		// skip backwards over identifier

		while  (startword  >  0  &&  isalpha_numeric(linebuf[startword  -  1]))
			startword--;
    
		linebuf[current]  =  '\0';

		char*  word  =  linebuf  +  startword;

		POSITION listpos = GlobalFuncsNames.GetHeadPosition();

		int index=0;
		while (listpos)
		{
			if (GlobalFuncsNames.GetNext(listpos) == word)
			{
				// found a word, display tip

				CallTipShow(pos,GlobalFuncsDefs.GetAt(GlobalFuncsDefs.FindIndex(index)));
				break;
			}
			index++;
		}
	}
	else if (ModeCode==MODE_C && !arrow_key(nChar)  && !escape_time() && (isalpha_numeric(nChar) || !AutoCActive()))
	{
		
		int  startword  =  current;
    
		while  (startword  >  0  &&  isalpha_numeric(linebuf[startword  -  1]))
			startword--;
    
		linebuf[current]  =  '\0';



		char*  word  =  linebuf  +  startword;
		int word_length = current-startword;
	
		char prev_char,prev_prev_char;
    
		// skip backwards over white space

		while  (startword  >  0 && iswhitespace(linebuf[startword  -  1]))
			startword--;
		
		if (startword>1)
		{
			prev_char     =linebuf[startword  -  1];
			prev_prev_char=linebuf[startword  -  2];
		}
		else if (startword>0)
		{
			prev_char     =linebuf[startword  -  1];
			prev_prev_char=0;
		}
		else
		{
			prev_char=0;
			prev_prev_char=0;
		}

		if (prev_char == '.' || 
			(prev_char == '>' && prev_prev_char == '-'))
		{
			nItems=DoAutoComplete(word,List,StructList);
		}
		else
		{
			nItems=DoAutoComplete(word,List,GlobalList);
		}

		if (nItems>0)
		{
			List.TrimRight();

			if (nItems>1 || List!=word)  // don't show if only one choice exatly matches
			{
				AutoCSetIgnoreCase(true);
				AutoCShow(word_length,List);
				ListFound=true;
			}
		}

		if (!ListFound)
		{
			AutoCCancel();
		}
	}
}



bool CRichEditCtrlEx::isalpha_numeric(char s)
{
	if (s>='a' && s<='z') return true;
	if (s>='A' && s<='Z') return true;
	if (s>='0' && s<='9') return true;
	if (s=='_') return true;

	return false;
}

bool CRichEditCtrlEx::arrow_key(char s)
{
	if (s == VK_LEFT) return true;
	if (s == VK_RIGHT) return true;
	if (s == VK_UP) return true;
	if (s == VK_DOWN) return true;

	return false;
}

#define ESC_TIME 20000


bool CRichEditCtrlEx::escape_time()
{
	DWORD t1=timeGetTime();

	if (m_esc_pushed)
	{
		if (t1 - m_t0_esc_time > ESC_TIME)
		{
			m_esc_pushed=false;
			return false;
		}
		else
		{
			return true;
		}
	}
	return false;
}




bool CRichEditCtrlEx::iswhitespace(char s)
{
	if (s==' ') return true;
	if (s=='\t') return true;

	return false;
}

void CRichEditCtrlEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CString s;

	if (nChar == VK_SHIFT)
		ShiftIsDown=true;

	if (nChar == VK_CONTROL)
		CtrlIsDown=true;

	if (CtrlIsDown && nChar=='F')
	{
		OnFind();
	}

	if (m_SingleLineOnly && nChar == VK_RETURN)
	{
		GetWindowText(s);
		s.Remove('\n');
		s.Remove('\r');
		SetWindowText(s);
		GotoLine(0);
	}

	CScintillaCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

int CRichEditCtrlEx::DoAutoComplete(CString word, CString &List, CString WordList)
{
	int n=0,i=0,k;

	if (word.GetLength() >= 2)
	{
		word.MakeLower();

		for (;;)
		{
			k=WordList.Find(" ",i);
			if (k==-1) break;
			n+=CheckMatch(word,WordList.Mid(i,k-i),List);
			i=k+1;
		}
	}

	return n;
}


int CRichEditCtrlEx::CheckMatch(CString word, CString v, CString &List)
{
	CString v0=v;

	if (word.GetLength() <= v.GetLength())
	{
		v.MakeLower();

		if (word == v.Left(word.GetLength()))
		{
			List += v0 + " ";
			return 1;
		}
	}
	return 0;
}

void CRichEditCtrlEx::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu Menu;
	RECT rect;

	pWnd->GetWindowRect(&rect);

	ContextPoint.x=point.x-rect.left;
	ContextPoint.y=point.y-rect.top;

	Menu.CreatePopupMenu();
	
	if (CanUndo())
		Menu.AppendMenu(0,ID_Undo,"Undo");
	else
		Menu.AppendMenu(MF_GRAYED,ID_Undo,"Undo");


	if (CanRedo())
		Menu.AppendMenu(0,ID_Redo,"Redo");
	else
		Menu.AppendMenu(MF_GRAYED,ID_Redo,"Redo");


	Menu.AppendMenu(MF_SEPARATOR);
	
	if (GetSelectionEnd() > GetSelectionStart())
	{
		Menu.AppendMenu(0,ID_Cut,"Cut");
		Menu.AppendMenu(0,ID_Copy,"Copy");
	}
	else
	{
		Menu.AppendMenu(MF_GRAYED,ID_Cut,"Cut");
		Menu.AppendMenu(MF_GRAYED,ID_Copy,"Copy");
	}
	
	if (CanPaste())
		Menu.AppendMenu(0,ID_Paste,"Paste");
	else
		Menu.AppendMenu(MF_GRAYED,ID_Paste,"Paste");
	

	Menu.AppendMenu(MF_SEPARATOR);
	Menu.AppendMenu(0,ID_SelectAll,"Select All");
	Menu.AppendMenu(MF_SEPARATOR);
	Menu.AppendMenu(0,ID_Find,"Find...");
	Menu.AppendMenu(0,ID_Replace,"Replace...");

	if (!m_SpecialContext.IsEmpty())
	{
		Menu.AppendMenu(MF_SEPARATOR);
		Menu.AppendMenu(0,ID_SpecialContext,m_SpecialContext);
	}

	if (ModeCode==MODE_G)
	{
		Menu.AppendMenu(0,ID_TransformSel,"Transform Sel");
		if (TheFrame->GCodeDlg.m_ShowLineNumbers)
		{
			Menu.AppendMenu(0,ID_ShowLineNumbers,"Hide Line Numbers");
		}
		else
		{
			Menu.AppendMenu(0,ID_ShowLineNumbers,"Show Line Numbers");
		}
	}

#ifndef KMOTION_CNC
	if (ModeCode==MODE_C)
	{
		if (TheFrame->ProgramDlg.m_ShowLineNumbers)
		{
		Menu.AppendMenu(0,ID_ShowLineNumbers,"Hide Line Numbers");
	}
	else
	{
		Menu.AppendMenu(0,ID_ShowLineNumbers,"Show Line Numbers");
	}
	}
#endif


	Menu.TrackPopupMenu(TPM_LEFTALIGN + TPM_LEFTBUTTON, point.x, point.y, this, NULL);	
}


void CRichEditCtrlEx::SetAStyle(int style, COLORREF fore, COLORREF back, int size, const char* face) 
{
	StyleSetFore(style, fore);
	StyleSetBack(style, back);
	
	if (size >= 1)
		StyleSetSize(style, size);
	if (face) 
		StyleSetFont(style, face);
}

void CRichEditCtrlEx::SetupForGCode() 
{
	ModeCode=MODE_G;

	m_SpecialContext = "Set Next Statement";
	
	SetupDirectAccess();

	SetLexer(SCLEX_GCODE);
	SetKeyWords(0, GKeyWords);

	//Setup styles
	SetAStyle(STYLE_DEFAULT, RGB(0, 0, 0), RGB(0xff, 0xff, 0xff), 10, "Courier New");
	StyleClearAll();

		
	SetAStyle(SCE_GCODE_IDENTIFIER, RGB(0, 0, 0));
	StyleSetBold(SCE_GCODE_IDENTIFIER, 1);
	SetAStyle(SCE_GCODE_DEFAULT, RGB(0, 0, 0));
	SetAStyle(SCE_GCODE_COMMENT, RGB(0, 0x80, 0));
	SetAStyle(SCE_GCODE_NUMBER, RGB(0, 0, 0x80));
	StyleSetBold(SCE_GCODE_NUMBER, 1);
	SetAStyle(SCE_GCODE_PARAM, RGB(0, 0x80,0x80));
	StyleSetBold(SCE_GCODE_PARAM, 1);
	SetAStyle(SCE_GCODE_BUF, RGB(0xc0, 0x0,0x0));
	SetAStyle(SCE_GCODE_CMD, RGB(0x00, 0x00,0xff));
	SetAStyle(SCE_GCODE_MSG, RGB(0xff, 0x7f,0x00));
	
	
	
	SetTabWidth(tabsize);	
	AssignCmdKey((SCMOD_CTRL<<16)|('F'), SCI_NULL);

	if (ModeCode==MODE_G && TheFrame->GCodeDlg.m_ShowLineNumbers)
	{
		SetMarginTypeN(0,SC_MARGIN_NUMBER);
		SetMarginWidthN(0,60);
	}
	else
	{
		SetMarginTypeN(0,SC_MARGIN_NUMBER);
		SetMarginWidthN(0,0);
	}
}



void CRichEditCtrlEx::SetupForCCode() 
{
	ModeCode=MODE_C;

	SetupDirectAccess();

	SetLexer(SCLEX_CPP);
	SetKeyWords(0, cKeyWords);

	//Setup styles
	SetAStyle(STYLE_DEFAULT, RGB(0, 0, 0), RGB(0xff, 0xff, 0xff), 10, "Courier New");
	StyleClearAll();
	SetAStyle(SCE_C_DEFAULT, RGB(0, 0, 0));
	SetAStyle(SCE_C_COMMENT, RGB(0, 0x80, 0));
	SetAStyle(SCE_C_COMMENTLINE, RGB(0, 0x80, 0));
	SetAStyle(SCE_C_COMMENTDOC, RGB(0, 0x80, 0));
	SetAStyle(SCE_C_COMMENTLINEDOC, RGB(0, 0x80, 0));
	SetAStyle(SCE_C_COMMENTDOCKEYWORD, RGB(0, 0x80, 0));
	SetAStyle(SCE_C_COMMENTDOCKEYWORDERROR, RGB(0, 0x80, 0));
	SetAStyle(SCE_C_NUMBER, RGB(0, 0x80, 0x80));
	SetAStyle(SCE_C_WORD, RGB(0, 0, 0x80));
//	StyleSetBold(SCE_C_WORD, 1);
	SetAStyle(SCE_C_STRING, RGB(0x80, 0, 0x80));
	SetAStyle(SCE_C_IDENTIFIER, RGB(0, 0, 0));
	SetAStyle(SCE_C_PREPROCESSOR, RGB(0x80, 0, 0));
	SetAStyle(SCE_C_OPERATOR, RGB(0x80, 0x80, 0));
	SetTabWidth(tabsize);	

	AssignCmdKey((SCMOD_CTRL<<16)|('F'), SCI_NULL);

#ifndef KMOTION_CNC

	if (ModeCode==MODE_C && TheFrame->ProgramDlg.m_ShowLineNumbers)
	{
		SetMarginTypeN(0,SC_MARGIN_NUMBER);
		SetMarginWidthN(0,60);
	}
	else
	{
		SetMarginTypeN(0,SC_MARGIN_NUMBER);
		SetMarginWidthN(0,0);
	}

	CreateGlobalFunctionDefinitions(TheFrame->MainPathRoot + "\\DSP_KFLOP\\KMotionDef.h");
#endif
}


void CRichEditCtrlEx::SetupForCMD() 
{
	ModeCode=MODE_CMD;
//	m_SingleLineOnly=true;

	SetupDirectAccess();

	SetLexer(SCLEX_CPP);

	//Setup styles
	SetAStyle(STYLE_DEFAULT, RGB(0, 0, 0), RGB(0xff, 0xff, 0xff), 10, "Courier New");
	StyleClearAll();
	SetAStyle(SCE_C_DEFAULT, RGB(0, 0, 0));
	SetAStyle(SCE_C_COMMENT, RGB(0, 0x80, 0));
	SetAStyle(SCE_C_COMMENTLINE, RGB(0, 0x80, 0));
	SetAStyle(SCE_C_COMMENTDOC, RGB(0, 0x80, 0));
	SetAStyle(SCE_C_COMMENTLINEDOC, RGB(0, 0x80, 0));
	SetAStyle(SCE_C_COMMENTDOCKEYWORD, RGB(0, 0x80, 0));
	SetAStyle(SCE_C_COMMENTDOCKEYWORDERROR, RGB(0, 0x80, 0));
	SetAStyle(SCE_C_NUMBER, RGB(0, 0x80, 0x80));
	SetAStyle(SCE_C_WORD, RGB(0, 0, 0x80));
//	StyleSetBold(SCE_C_WORD, 1);
	SetAStyle(SCE_C_STRING, RGB(0x80, 0, 0x80));
	SetAStyle(SCE_C_IDENTIFIER, RGB(0, 0, 0));
	SetAStyle(SCE_C_PREPROCESSOR, RGB(0x80, 0, 0));
	SetAStyle(SCE_C_OPERATOR, RGB(0x80, 0x80, 0));
	SetTabWidth(tabsize);	
}



void CRichEditCtrlEx::CreateGlobalFunctionDefinitions(CString fname) 
{
	CString s;
	FILE *f=fopen(fname,"rb");

	if (!f) return;
	
	// global functions of interest are defined as lines
	// with these characters in sequence "(", ")", ";" "/","/"

	GlobalFuncsNames.RemoveAll();
	GlobalFuncsDefs.RemoveAll();

	while (!feof(f))
	{
		fgets(s.GetBuffer(1000),1000,f);
		s.ReleaseBuffer();

		if (CheckForChars(s,"();//") != -1)
		{
			s=WhitespaceToSpace(s);

			s=s.Left(s.GetLength()-2);

			int r = s.Find("(");

			CString t = FindTokenBackwards(s,r-1);

			GlobalFuncsNames.AddTail(t);

			if (s.GetLength() >0 && s.GetAt(0) == ' ') s.Delete(0);

			if (s.Find("// ") != -1) s.Replace("// ","\n");
			if (s.Find("//") != -1) s.Replace("// ","\n");

			GlobalFuncsDefs.AddTail(s);
		}
	}
	fclose(f);
}

// finds a token immediately preceeding the specified location

CString CRichEditCtrlEx::FindTokenBackwards(CString s, int r)
{
	int i=r;

	if (s.GetAt(i) == ' ') i--;

	while (i>=0 && isalpha_numeric(s.GetAt(i))) i--;

	return s.Mid(i+1,r-i);
}



// converts consecutive whitespace chars to single space

CString CRichEditCtrlEx::WhitespaceToSpace(CString s) 
{
	CString r;
	bool in_white_space=false; 

	for (int i=0; i<s.GetLength(); i++)
	{
		char c = s.GetAt(i);

		if (in_white_space)
		{
			if (c!=' ' && c!='\t')
			{
				r+=c;
				in_white_space=false;
			}
		}
		else
		{
			if (c!=' ' && c!='\t')
			{
				r+=c;
			}
			else
			{
				r+=' ';
				in_white_space=true;
			}
		}
	}
	return r;
}


// check for sequence of characters (not necessarily consecutive)
//
// returns -1 if not all are present
// otherwise returns location of last required char

int CRichEditCtrlEx::CheckForChars(CString s, CString chs) 
{
	int r=0;

	for (int i=0; i<chs.GetLength(); i++)
	{
		r = s.Find(chs.GetAt(i),r);

		if (r==-1) return -1;

		r++;
	}

	return r;
}



void CRichEditCtrlEx::OnPaint() 
{
	CScintillaCtrl::OnPaint();
}

void CRichEditCtrlEx::OnUndo() 
{
	Undo();
}

void CRichEditCtrlEx::OnRedo() 
{
	Redo();
}

void CRichEditCtrlEx::OnCut() 
{
	Cut();
}

void CRichEditCtrlEx::OnCopy() 
{
	Copy();
}

void CRichEditCtrlEx::OnPaste() 
{
	Paste();

	if (m_SingleLineOnly)
	{
		CString s;
		GetWindowText(s);
		s.Remove('\n');
		s.Remove('\r');
		SetWindowText(s);
		GotoLine(0);
	}
}

void CRichEditCtrlEx::OnDelete() 
{
	Clear();
}

void CRichEditCtrlEx::OnSelectAll() 
{
	SetSel(0,-1);
}

void CRichEditCtrlEx::OnFind() 
{
	OnEditFindReplace(TRUE);
}


void CRichEditCtrlEx::OnReplace() 
{
	OnEditFindReplace(FALSE);
}

void CRichEditCtrlEx::OnSpecialContext() 
{
	int pos = PositionFromPoint(ContextPoint.x, ContextPoint.y);

	int  current  =  LineFromPosition(pos);

	TheFrame->GCodeDlg.SetExecutionPoint(current);
}

void CRichEditCtrlEx::OnShowLineNumbers() 
{
	if (ModeCode==MODE_G)
	{
		if (TheFrame->GCodeDlg.m_ShowLineNumbers)
		{
			TheFrame->GCodeDlg.m_ShowLineNumbers=0;
			SetMarginWidthN(0,0);
		}
		else
		{
			TheFrame->GCodeDlg.m_ShowLineNumbers=1;
			SetMarginWidthN(0,60);
		}
	}
#ifndef KMOTION_CNC
	if (ModeCode==MODE_C)
	{
		if (TheFrame->ProgramDlg.m_ShowLineNumbers)
		{
			TheFrame->ProgramDlg.m_ShowLineNumbers=0;
			SetMarginWidthN(0,0);
		}
		else
		{
			TheFrame->ProgramDlg.m_ShowLineNumbers=1;
			SetMarginWidthN(0,60);
		}
	}	
#endif
}

void CRichEditCtrlEx::OnTransformSel() 
{
	CString s;

	int length = GetSelectionEnd()-GetSelectionStart();
	
	if (length == 0)
	{
		AfxMessageBox("Nothing Selected to Transform");
		return;
	}

	GetSelText(s.GetBufferSetLength(length+1));
	s.ReleaseBuffer();

	if (TransformDlg.DoModal() == IDOK)
	{
		if (!TransformDlg.DoTransform(s))
		{
			ReplaceSel(s);
		}
	}
}

void CRichEditCtrlEx::AdjustFindDialogPosition()
{
	ASSERT(m_pFindReplaceDlg);
	int nStart = GetSelectionStart();
	CPoint point;
	point.x = PointXFromPosition(nStart);
	point.y = PointYFromPosition(nStart);
	ClientToScreen(&point);
	CRect rectDlg;
	m_pFindReplaceDlg->GetWindowRect(&rectDlg);
	if (rectDlg.PtInRect(point))
	{
		if (point.y > rectDlg.Height())
			rectDlg.OffsetRect(0, point.y - rectDlg.bottom - 20);
		else
		{
			int nVertExt = GetSystemMetrics(SM_CYSCREEN);
			if (point.y + rectDlg.Height() < nVertExt)
				rectDlg.OffsetRect(0, 40 + point.y - rectDlg.top);
		}
		m_pFindReplaceDlg->MoveWindow(&rectDlg);
	}
}

void CRichEditCtrlEx::OnEditFindReplace(BOOL bFindOnly)
{
	ASSERT_VALID(this);
	
	m_bFirstSearch = TRUE;
	if (m_pFindReplaceDlg != NULL)
	{
		if (m_bFindOnly == bFindOnly)
		{
			m_pFindReplaceDlg->SetActiveWindow();
			m_pFindReplaceDlg->ShowWindow(SW_SHOW);
			return;
		}
		else
		{
			ASSERT(m_bFindOnly != bFindOnly);
			m_pFindReplaceDlg->SendMessage(WM_CLOSE);
			ASSERT(m_pFindReplaceDlg == NULL);
			ASSERT_VALID(this);
		}
	}
	CString strFind = GetSelText();
	//if selection is empty or spans multiple lines use old find text
	if (strFind.IsEmpty() || (strFind.FindOneOf(_T("\n\r")) != -1))
		strFind = m_strFind;
	
	CString strReplace = m_strReplace;
	m_pFindReplaceDlg = new CScintillaFindReplaceDlg;
	ASSERT(m_pFindReplaceDlg != NULL);
	DWORD dwFlags = NULL;
	if (m_bNext)
		dwFlags |= FR_DOWN;
	if (m_bCase)
		dwFlags |= FR_MATCHCASE;
	if (m_bWord)
		dwFlags |= FR_WHOLEWORD;
	if (m_bRegularExpression)
		m_pFindReplaceDlg->SetRegularExpression(TRUE);
	
	if (!m_pFindReplaceDlg->Create(bFindOnly, strFind, strReplace, dwFlags, this))
	{
		m_pFindReplaceDlg = NULL;
		ASSERT_VALID(this);
		return;
	}
	ASSERT(m_pFindReplaceDlg != NULL);
	m_bFindOnly = bFindOnly;
	m_pFindReplaceDlg->SetActiveWindow();
	m_pFindReplaceDlg->ShowWindow(SW_SHOW);
	ASSERT_VALID(this);
}

void CRichEditCtrlEx::OnFindNext(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression)
{
	ASSERT_VALID(this);
	
	m_strFind = lpszFind;
	m_bCase = bCase;
	m_bWord = bWord;
	m_bNext = bNext;
	m_bRegularExpression = bRegularExpression;
	
	if (!FindTextView(m_strFind, m_bNext, m_bCase, m_bWord, m_bRegularExpression))
		TextNotFound();
	else
		AdjustFindDialogPosition();
	ASSERT_VALID(this);
}

void CRichEditCtrlEx::OnReplaceSel(LPCTSTR lpszFind, BOOL bNext, BOOL bCase,	BOOL bWord, BOOL bRegularExpression, LPCTSTR lpszReplace)
{
	ASSERT_VALID(this);
	
	m_strFind = lpszFind;
	m_strReplace = lpszReplace;
	m_bCase = bCase;
	m_bWord = bWord;
	m_bNext = bNext;
	m_bRegularExpression = bRegularExpression;
	
	if (!SameAsSelected(m_strFind, m_bCase, m_bWord, m_bRegularExpression))
	{
		if (!FindTextView(m_strFind, m_bNext, m_bCase, m_bWord, m_bRegularExpression))
			TextNotFound();
		else
			AdjustFindDialogPosition();
		return;
	}
	
	TargetFromSelection();

	if (bRegularExpression)
		ReplaceTargetRE(m_strReplace.GetLength(),m_strReplace);
	else
		ReplaceTarget(m_strReplace.GetLength(),m_strReplace);

	// move selection to the end so that if we replaced with
	// something that contains the original search pattern 
	// we don't find it again
	int cpMin = GetSelectionStart()+m_strReplace.GetLength();
	int cpMax = GetSelectionEnd()+m_strReplace.GetLength();

	SetSel(cpMin, cpMax);

	if (!FindTextView(m_strFind, m_bNext, m_bCase, m_bWord, m_bRegularExpression))
		TextNotFound();
	else
		AdjustFindDialogPosition();

	ASSERT_VALID(this);
}

void CRichEditCtrlEx::OnReplaceAll(LPCTSTR lpszFind, LPCTSTR lpszReplace, BOOL bCase, BOOL bWord, BOOL bRegularExpression)
{
	ASSERT_VALID(this);
	
	m_strFind = lpszFind;
	m_strReplace = lpszReplace;
	m_bCase = bCase;
	m_bWord = bWord;
	m_bNext = TRUE;
	m_bRegularExpression = bRegularExpression;

	int NReplaced=0;
	
	CWaitCursor wait;
	// no selection or different than what looking for
	if (!SameAsSelected(m_strFind, m_bCase, m_bWord, bRegularExpression))
	{
		if (!FindTextView(m_strFind, m_bNext, m_bCase, m_bWord, m_bRegularExpression))
		{
			TextNotFound();
			return;
		}
	}
	
	HideSelection(TRUE, FALSE);
	do
	{
		TargetFromSelection();
		if (bRegularExpression)
			ReplaceTargetRE(m_strReplace.GetLength(),m_strReplace);
		else
			ReplaceTarget(m_strReplace.GetLength(),m_strReplace);

		// move selection to the end so that if we replaced with
		// something that contains the original search pattern 
		// we don't find it again
		int cpMin = GetSelectionStart()+m_strReplace.GetLength();
		int cpMax = GetSelectionEnd()+m_strReplace.GetLength();

		SetSel(cpMin, cpMax);

		NReplaced++;
	} 
	while (FindTextSimple(m_strFind, m_bNext, m_bCase, m_bWord, m_bRegularExpression));
	
	TextNotFound();
	HideSelection(FALSE, FALSE);

	if (NReplaced>0)
	{
		CString s;
		s.Format("%d occurances replaced",NReplaced);
		MessageBox(s);
	}
}

LRESULT CRichEditCtrlEx::OnFindReplaceCmd(WPARAM /*wParam*/, LPARAM lParam)
{
	ASSERT_VALID(this);
	
	CScintillaFindReplaceDlg* pDialog = (CScintillaFindReplaceDlg*) CFindReplaceDialog::GetNotifier(lParam);
	ASSERT(pDialog != NULL);
	ASSERT(pDialog == m_pFindReplaceDlg);
	
	if (pDialog->IsTerminating())
		m_pFindReplaceDlg = NULL;
	else if (pDialog->FindNext())
		OnFindNext(pDialog->GetFindString(), pDialog->SearchDown(), pDialog->MatchCase(), pDialog->MatchWholeWord(), pDialog->GetRegularExpression());
	else if (pDialog->ReplaceCurrent())
	{
		ASSERT(!m_bFindOnly);
		OnReplaceSel(pDialog->GetFindString(), pDialog->SearchDown(), pDialog->MatchCase(), pDialog->MatchWholeWord(), pDialog->GetRegularExpression(), pDialog->GetReplaceString());
	}
	else if (pDialog->ReplaceAll())
	{
		ASSERT(!m_bFindOnly);
		OnReplaceAll(pDialog->GetFindString(), pDialog->GetReplaceString(), pDialog->MatchCase(), pDialog->MatchWholeWord(), pDialog->GetRegularExpression());
	}
	ASSERT_VALID(this);
	
	return 0;
}

BOOL CRichEditCtrlEx::SameAsSelected(LPCTSTR lpszCompare, BOOL bCase, BOOL /*bWord*/, BOOL bRegularExpression)
{
	// check length first
	size_t nLen = lstrlen(lpszCompare);
	int nStartChar = GetSelectionStart();
	int nEndChar = GetSelectionEnd();

	//if we are doing a regular expression Find / Replace, and the selection is non-zero then assume they match!!
	if (bRegularExpression  && (size_t)(nEndChar - nStartChar) > 0)
		return TRUE;
	
	if (nLen != (size_t)(nEndChar - nStartChar))
		return FALSE;
	
	// length is the same, check contents
	CString strSelect = GetSelText();
	return (bCase && lstrcmp(lpszCompare, strSelect) == 0) || (!bCase && lstrcmpi(lpszCompare, strSelect) == 0);
}

BOOL CRichEditCtrlEx::FindTextView(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression)
{
	ASSERT_VALID(this);
	CWaitCursor wait;
	return FindTextSimple(lpszFind, bNext, bCase, bWord, bRegularExpression);
}

BOOL CRichEditCtrlEx::FindTextSimple(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression)
{
	USES_CONVERSION;
	

	ASSERT(lpszFind != NULL);
	TextToFind ft;
	ft.chrg.cpMin = GetSelectionStart();
	ft.chrg.cpMax = GetSelectionEnd();
	if (m_bFirstSearch)
		m_bFirstSearch = FALSE;
	
	ft.lpstrText = T2A((LPTSTR)lpszFind);
	if (ft.chrg.cpMin != ft.chrg.cpMax) // i.e. there is a selection
	{
#ifndef _UNICODE
		// If byte at beginning of selection is a DBCS lead byte,
		// increment by one extra byte.
		TEXTRANGE textRange;
		TCHAR ch[2];
		textRange.chrg.cpMin = ft.chrg.cpMin;
		textRange.chrg.cpMax = ft.chrg.cpMin + 1;
		textRange.lpstrText = ch;
		SendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&textRange);
		if (_istlead(ch[0]))
		{
			ASSERT(ft.chrg.cpMax - ft.chrg.cpMin >= 2);
			
			if (bNext)
				ft.chrg.cpMin++;
			else
				ft.chrg.cpMax = ft.chrg.cpMin - 1;
		}
#endif
		
		if (bNext)  
			ft.chrg.cpMin++;
		else
			ft.chrg.cpMax = ft.chrg.cpMin - 1;
	}
	
	int nLength = GetLength();
	if (bNext)
		ft.chrg.cpMax = nLength;
	else
		ft.chrg.cpMin = 0;
	
	DWORD dwFlags = bCase ? SCFIND_MATCHCASE : 0;
	dwFlags |= bWord ? SCFIND_WHOLEWORD : 0;
	dwFlags |= bRegularExpression ? SCFIND_REGEXP : 0;
	
	if (!bNext)
	{
		//Swap the start and end positions which Scintilla uses to flag backward searches
		int ncpMinTemp = ft.chrg.cpMin;
		ft.chrg.cpMin = ft.chrg.cpMax;
		ft.chrg.cpMax = ncpMinTemp;
	}
	
	// if we find the text return TRUE
	return (FindAndSelect(dwFlags, ft) != -1);
}

long CRichEditCtrlEx::FindAndSelect(DWORD dwFlags, TextToFind& ft)
{
	long index = FindText(dwFlags, &ft);
	if (index != -1) // i.e. we found something
		SetSel(ft.chrgText.cpMin, ft.chrgText.cpMax);
	return index;
}

void CRichEditCtrlEx::TextNotFound()
{
	ASSERT_VALID(this);
	m_bFirstSearch = TRUE;
}




void CRichEditCtrlEx::OnSetFocus(CWnd* pOldWnd)
{
	CScintillaCtrl::OnSetFocus(pOldWnd);

#ifdef KMOTION_CNC
	if (pOldWnd!=NULL) // are we gaining focus ?
		TheFrame->GCodeDlg.DisableKeyJog();
#endif
}
