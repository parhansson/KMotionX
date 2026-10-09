// CKMotionCNCDlg.cpp : implementation file
/*********************************************************************/
/*         Copyright (c) 2003-2014  DynoMotion Incorporated          */
/*********************************************************************/


#include "stdafx.h"
#include <typeinfo>
#include "afxglobals.h"
#include "MainFrm.h"
#include "EditToolFile.h"
#include "EditFixtures.h"
#include "AllToolSetupSheet.h"
#include "SetValue.h"
#include "KMotionCNCDlg.h"
#include "ResumeDlg.h"
#include "OpenDlg.h"
#include "Frame.h"
#include "HiResTimer.h"
#include "..\GCodeInterpreter\rs274ngc_return.h"	// RS274NGC_EXIT / _ENDFILE (G-code viewer preview)


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FLAG_COLOR 0x808000

#define MAGIC_CHECKWORD 12345678

#define GCODE_FILES		/*TRAN*/"\\Data\\GFilesCNC.txt"

#define SLIDER_RANGE 100
#define OVERRIDE_MIN 0.10
#define OVERRIDE_MAX 2.0

#define STATUS_TIME 0.1 //sec

#define sqr(x) ((x)*(x))

#define ACTION_CYCLE_START 25
#define ACTION_HALT 26
#define ACTION_STOP 27
#define ACTION_FEEDHOLD 28
#define ACTION_RESUME 29
#define ACTION_PROG_START 30
#define ACTION_PROG_EXIT 31

// Axis indices
#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2
#define AXIS_A 3
#define AXIS_B 4
#define AXIS_C 5
#define AXIS_U 6
#define AXIS_V 7
#define NUM_AXES 8

// Parameter type indices
#define PT_VEL              0
#define PT_ACCEL            1
#define PT_COUNTS_PER_INCH  2
#define PT_JOG_VEL          3
#define NUM_PARAM_TYPES     4

// Note PT_JOG_VEL is not in this table (m_JogSpeed is an array member so no
// pointer-to-member is possible) - the GET/SET handlers treat it as a special
// case before indexing here.  Only the first 3 rows are initialized.
double CKMotionCNCDlg::* TP_ParamMember[NUM_PARAM_TYPES][NUM_AXES] =
{
	{
		&CKMotionCNCDlg::m_MaxVelX,
		&CKMotionCNCDlg::m_MaxVelY,
		&CKMotionCNCDlg::m_MaxVelZ,
		&CKMotionCNCDlg::m_MaxVelA,
		&CKMotionCNCDlg::m_MaxVelB,
		&CKMotionCNCDlg::m_MaxVelC,
		&CKMotionCNCDlg::m_MaxVelU,
		&CKMotionCNCDlg::m_MaxVelV
	},
	{
		&CKMotionCNCDlg::m_MaxAccelX,
		&CKMotionCNCDlg::m_MaxAccelY,
		&CKMotionCNCDlg::m_MaxAccelZ,
		&CKMotionCNCDlg::m_MaxAccelA,
		&CKMotionCNCDlg::m_MaxAccelB,
		&CKMotionCNCDlg::m_MaxAccelC,
		&CKMotionCNCDlg::m_MaxAccelU,
		&CKMotionCNCDlg::m_MaxAccelV
	},
	{
		&CKMotionCNCDlg::m_CountsPerInchX,
		&CKMotionCNCDlg::m_CountsPerInchY,
		&CKMotionCNCDlg::m_CountsPerInchZ,
		&CKMotionCNCDlg::m_CountsPerInchA,
		&CKMotionCNCDlg::m_CountsPerInchB,
		&CKMotionCNCDlg::m_CountsPerInchC,
		&CKMotionCNCDlg::m_CountsPerInchU,
		&CKMotionCNCDlg::m_CountsPerInchV
	}
};

//status callbacks

void StraightTraverseCallback(double x, double y, double z, double a, double b, double c, int sequence_number);

void ArcFeedCallback(bool ZeroLenAsFullCircles, double DesiredFeedRate_in_per_sec, 
			    CANON_PLANE plane,
				double first_end, double second_end, 
		        double first_axis, double second_axis, int rotation,
				double axis_end_point,double a, double b, double c,
				double first_start, double second_start, double axis_start_point, int sequence_number, int ID);

void StraightFeedCallback(double DesiredFeedRate_in_per_sec,
							   double x, double y, double z, double a, double b, double c, int sequence_number, int ID);


int DoProcessScript(const wchar_t *FileName);


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CKMotionCNCDlg::SaveOnExit(FILE * f)
{
	fwprintf(f,L"%d %d\n",m_Thread,m_Rapid);
	fwprintf(f,L"%d %d\n",m_Simulate, m_DoTime);
	fwprintf(f,L"%d\n",m_ShowMach);
	fwprintf(f,L"%d\n",m_StepSize);
	CDlgX::SaveOnExit(f);

	SaveFileNames();
	SaveConfig();
}

void CKMotionCNCDlg::RestoreOnStart(FILE * f)
{
	CString s;

	fgetws(s.GetBufferSetLength(100),99,f);
	s.ReleaseBuffer();

	if (s.IsEmpty() || s=="\n")  // skip over a blank line
	{
		fgetws(s.GetBufferSetLength(100),99,f);
		s.ReleaseBuffer();
	}
	int result = swscanf(s,L"%d%d",&m_Thread, &m_Rapid);
	if (result < 2) m_Rapid=1;


	fgetws(s.GetBufferSetLength(100), 99, f);
	s.ReleaseBuffer();

	int r = swscanf(s,L"%d %d",&m_Simulate, &m_DoTime);
	if (r < 2) m_DoTime = 0;
	fwscanf(f,L"%d",&m_ShowMach);
	fwscanf(f,L"%d",&m_StepSize);

	// also read in file names

	for (int i=0; i<N_USER_GCODE_FILES; i++)      // set to default names
	{
		s.Format(L"user%d.ngc",i+1);
		FileNames[i] = TheFrame->MainPath + s;
	}

	CString File = TheFrame->MainPath + GCODE_FILES;
	
	FILE *g;
	_tfopen_s(&g, File, _T("rt,ccs=UTF-8"));

	
	if (!g)
	{
		CString s;
		s.Format(/*TRAN*/TheFrame->KMotionDLL->Translate("Error opening file %ls\nPrevious G Code Filenames"), File.GetBuffer(MAX_PATH));

		MessageBox(s, L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
	}
	else
	{
		for (int i=0; i<N_USER_GCODE_FILES && !feof(g); i++)
		{
			fgetws(FileNames[i].GetBuffer(MAX_PATH),MAX_PATH,g);
			FileNames[i].ReleaseBuffer();
			FileNames[i].Remove('\n');

			// check if there is no path specified, then add in default

			if (FileNames[i].Find(L"\\\\") == -1 && FileNames[i].Find(':') == -1)
			{
				FileNames[i] = TheFrame->MainPathRoot + GCODE_SUB_DIR + FileNames[i];
			}
		}
		
		fclose(g);
	}

	LoadConfig();
	CDlgX::RestoreOnStart(f);
}


#define PERSIST_MCODE(x)			                            \
		INT(Interpreter->McodeActions[x].Action);			    \
		DOUBLE(Interpreter->McodeActions[x].dParams[0]);			\
		DOUBLE(Interpreter->McodeActions[x].dParams[1]);			\
		DOUBLE(Interpreter->McodeActions[x].dParams[2]);			\
		DOUBLE(Interpreter->McodeActions[x].dParams[3]);			\
		DOUBLE(Interpreter->McodeActions[x].dParams[4]);			\
		CHARS(Interpreter->McodeActions[x].String);		

	


int CKMotionCNCDlg::SaveLoadConfig(FILE *f, wchar_t *s, bool save)
{
	CSTRING_PATH(m_SetupFile, DATA_SUB_DIR);
	CSTRING_PATH(m_ToolFile, DATA_SUB_DIR);
	CSTRING_PATH(m_GeoFile, DATA_SUB_DIR);
	CSTRING(m_Button0);
	CSTRING(m_Button1);
	CSTRING(m_Button2);
	CSTRING(m_Button3);
	CSTRING(m_Button4);
	CSTRING(m_Button5);
	CSTRING(m_Button6);
	CSTRING(m_Button7);
	CSTRING(m_Button8);
	CSTRING(m_Button9);

	CSTRING(CommandHistory[0]);
	CSTRING(CommandHistory[1]);
	CSTRING(CommandHistory[2]);
	CSTRING(CommandHistory[3]);
	CSTRING(CommandHistory[4]);
	CSTRING(CommandHistory[5]);
	CSTRING(CommandHistory[6]);
	CSTRING(CommandHistory[7]);
	CSTRING(CommandHistory[8]);
	CSTRING(CommandHistory[9]);

	CSTRING(m_CommandString);

	DOUBLE(m_BreakAngle);
	DOUBLE(m_CollinearTol);
	DOUBLE(m_CornerTol);
	DOUBLE(m_FacetAngle);
	DOUBLE(m_JogSpeed[0]);
	DOUBLE(m_JogSpeed[1]);
	DOUBLE(m_JogSpeed[2]);
	DOUBLE(m_JogSpeed[3]);
	DOUBLE(m_JogSpeed[4]);
	DOUBLE(m_JogSpeed[5]);
	DOUBLE(m_JogSpeed[6]);
	DOUBLE(m_JogSpeed[7]);
	DOUBLE(m_JogSlowPercent);
	DOUBLE(m_HardwareFRORange);
	DOUBLE(m_MaxRapidFRO);
	DOUBLE(m_ArcRadiusTol);
	DOUBLE(m_ArcRSmallTol);
	DOUBLE(m_TPLookahead);
	DOUBLE(m_RadiusC);
	DOUBLE(m_RadiusB);
	DOUBLE(m_RadiusA);
	DOUBLE(m_MaxAccelC);
	DOUBLE(m_MaxAccelB);
	DOUBLE(m_MaxAccelA);
	DOUBLE(m_MaxAccelX);
	DOUBLE(m_MaxAccelY);
	DOUBLE(m_MaxAccelZ);
	DOUBLE(m_MaxAccelU);
	DOUBLE(m_MaxAccelV);
	DOUBLE(m_MaxVelC);
	DOUBLE(m_MaxVelB);
	DOUBLE(m_MaxVelA);
	DOUBLE(m_MaxVelX);
	DOUBLE(m_MaxVelY);
	DOUBLE(m_MaxVelZ);
	DOUBLE(m_MaxVelU);
	DOUBLE(m_MaxVelV);
	DOUBLE(m_CountsPerInchC);
	DOUBLE(m_CountsPerInchB);
	DOUBLE(m_CountsPerInchA);
	DOUBLE(m_CountsPerInchX);
	DOUBLE(m_CountsPerInchY);
	DOUBLE(m_CountsPerInchZ);
	DOUBLE(m_CountsPerInchU);
	DOUBLE(m_CountsPerInchV);

	// Constraints in Actuator Space (keys must be literal, so unrolled;
	// absent keys in an old config leave the defaults => mode off)
	INT(m_ActuatorLimits);
	DOUBLE(m_ActScale[0]);    DOUBLE(m_ActScale[1]);    DOUBLE(m_ActScale[2]);    DOUBLE(m_ActScale[3]);
	DOUBLE(m_ActScale[4]);    DOUBLE(m_ActScale[5]);    DOUBLE(m_ActScale[6]);    DOUBLE(m_ActScale[7]);
	DOUBLE(m_MaxActVel[0]);   DOUBLE(m_MaxActVel[1]);   DOUBLE(m_MaxActVel[2]);   DOUBLE(m_MaxActVel[3]);
	DOUBLE(m_MaxActVel[4]);   DOUBLE(m_MaxActVel[5]);   DOUBLE(m_MaxActVel[6]);   DOUBLE(m_MaxActVel[7]);
	DOUBLE(m_MaxActAccel[0]); DOUBLE(m_MaxActAccel[1]); DOUBLE(m_MaxActAccel[2]); DOUBLE(m_MaxActAccel[3]);
	DOUBLE(m_MaxActAccel[4]); DOUBLE(m_MaxActAccel[5]); DOUBLE(m_MaxActAccel[6]); DOUBLE(m_MaxActAccel[7]);
	DOUBLE(m_MaxActJerk[0]);  DOUBLE(m_MaxActJerk[1]);  DOUBLE(m_MaxActJerk[2]);  DOUBLE(m_MaxActJerk[3]);
	DOUBLE(m_MaxActJerk[4]);  DOUBLE(m_MaxActJerk[5]);  DOUBLE(m_MaxActJerk[6]);  DOUBLE(m_MaxActJerk[7]);
	INT(m_ActDegrees[0]);     INT(m_ActDegrees[1]);     INT(m_ActDegrees[2]);     INT(m_ActDegrees[3]);
	INT(m_ActDegrees[4]);     INT(m_ActDegrees[5]);     INT(m_ActDegrees[6]);     INT(m_ActDegrees[7]);

	INT(m_ThirdOrderTP);
	INT(m_TPLogSegs);
	INT(m_TPCubicKnots);
	DOUBLE(m_MaxJerkX);
	DOUBLE(m_MaxJerkY);
	DOUBLE(m_MaxJerkZ);
	DOUBLE(m_MaxJerkA);
	DOUBLE(m_MaxJerkB);
	DOUBLE(m_MaxJerkC);
	DOUBLE(m_MaxJerkU);
	DOUBLE(m_MaxJerkV);

	DOUBLE(m_Step0);
	DOUBLE(m_Step1);
	DOUBLE(m_Step2);
	DOUBLE(m_Step3);
	DOUBLE(m_Step4);
	DOUBLE(m_Step5);

	INT(m_ReverseRZ);
	INT(m_EnableGamePad);
	INT(m_ZeroUsingFixtures);
	INT(m_ToolLengthImmediately);
	INT(m_ToolTableDoM6);
	INT(m_ConfirmExit);
	INT(m_AllowConcaveCorners);
	INT(m_ArcsToSegs);
	INT(m_DisplayEncoder);
	INT(m_Lathe);
	INT(m_DoRapidsAsFeeds);
	INT(m_DiameterMode);
	INT(m_XPosFront);

	INT(m_SpindleType);
	INT(m_SpindleAxis);

	INT(m_SaveFixtureOnOK);

	DOUBLE(m_SpindleUpdateTime);
	DOUBLE(m_SpindleTau);
	DOUBLE(m_SpindleCntsPerRev);

	PERSIST_MCODE(0);
	PERSIST_MCODE(1);
	PERSIST_MCODE(2);
	PERSIST_MCODE(3);
	PERSIST_MCODE(4);
	PERSIST_MCODE(5);
	PERSIST_MCODE(6);
	PERSIST_MCODE(7);
	PERSIST_MCODE(8);
	PERSIST_MCODE(9);
	PERSIST_MCODE(10);
	PERSIST_MCODE(11);
	PERSIST_MCODE(12);
	PERSIST_MCODE(13);
	PERSIST_MCODE(14);
	PERSIST_MCODE(15);
	PERSIST_MCODE(16);
	PERSIST_MCODE(17);
	PERSIST_MCODE(18);
	PERSIST_MCODE(19);
	PERSIST_MCODE(20);
	PERSIST_MCODE(21);
	PERSIST_MCODE(22);
	PERSIST_MCODE(23);
	PERSIST_MCODE(24);
	PERSIST_MCODE(25);
	PERSIST_MCODE(26);
	PERSIST_MCODE(27);
	PERSIST_MCODE(28);
	PERSIST_MCODE(29);
	PERSIST_MCODE(30);
	PERSIST_MCODE(31);
	PERSIST_MCODE(32);
	PERSIST_MCODE(33);
	PERSIST_MCODE(34);
	PERSIST_MCODE(35);
	PERSIST_MCODE(36);
	PERSIST_MCODE(37);
	PERSIST_MCODE(38);
	PERSIST_MCODE(39);
	PERSIST_MCODE(40);
	PERSIST_MCODE(41);
	PERSIST_MCODE(42);
	PERSIST_MCODE(43);
	PERSIST_MCODE(44);
	PERSIST_MCODE(45);
	PERSIST_MCODE(46);
	PERSIST_MCODE(47);
	PERSIST_MCODE(48);

	INT(m_DialogFace);
	DOUBLE(m_SafeZ);
	INT(m_SafeRelAbs);
	INT(m_DegreesA);
	INT(m_DegreesB);
	INT(m_DegreesC);

	INT (m_UserButtonKeys[0]);
	INT (m_UserButtonKeys[1]);
	INT (m_UserButtonKeys[2]);
	INT (m_UserButtonKeys[3]);
	INT (m_UserButtonKeys[4]);
	INT (m_UserButtonKeys[5]);
	INT (m_UserButtonKeys[6]);
	INT (m_UserButtonKeys[7]);
	INT (m_UserButtonKeys[8]);
	INT (m_UserButtonKeys[9]);
	
	CSTRING_PATH(m_VarsFile, GCODE_SUB_DIR);
	CSTRING_PATH(m_ScreenScriptFile, SCREEN_SCRIPTS_DIR);

	INT (m_DisplayGViewer);

	INT(m_ConfigCheckWord);  // do this last as a checksum and indication all is well

	return 0;
}


int CKMotionCNCDlg::SaveConfig()
{
	wchar_t s[81];
	CString Name;
	FILE *f;

	for (int iTry=0; iTry<2; iTry++)
	{
		if (iTry == 0)
			Name = TheFrame->MainPathRoot + TheFrame->config_file;
		else
			Name = TheFrame->MainPathRoot + TheFrame->config_file_backup;

		_tfopen_s(&f, Name, _T("wt,ccs=UTF-8"));

		if (!f)
		{
			CString cs;
			cs.Format(/*TRAN*/TheFrame->KMotionDLL->Translate("Error Opening Configuration File %ls"), Name.GetBuffer(0));
			MessageBox( cs,/*TRAN*/TheFrame->KMotionDLL->Translate("Error"),MB_ICONSTOP|MB_OK);
		}
		else
		{
			// if GViewer window is open when saving configuration then 
			// set flag to restore it on next startup
			m_DisplayGViewer = TheFrame->GViewDlg.m_hWnd!=NULL;

			m_ConfigCheckWord=MAGIC_CHECKWORD;
			SaveLoadConfig(f, s, true);

			fclose(f);
		}
	}

	return 0;
}



int CKMotionCNCDlg::LoadConfig()
{
	wchar_t s[301];
	CString Name;
	FILE *f;
	int success=false;

	for (int iTry=0; iTry<2; iTry++)
	{
		if (iTry==0)
			Name = TheFrame->MainPathRoot + TheFrame->config_file;
		else
			Name = TheFrame->MainPathRoot + TheFrame->config_file_backup;

		m_ConfigCheckWord=0;

		_tfopen_s(&f, Name, _T("rt,ccs=UTF-8"));
		
		if (f)
		{
			while (!feof(f))
			{
				fgetws(s,300,f);
				if (!feof(f))
				{
					SaveLoadConfig(f, s, false);
				}
			}
			fclose(f);
		}


		if (!m_ConfigCheckWordVersion || m_ConfigCheckWord==MAGIC_CHECKWORD)
		{
			iTry=1;
			PersistRestored=true;
		}
		else
		{
			if (iTry==0)
			{
				if (MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate("Previous Configuration File:\r\r") +  Name +
							  /*TRAN*/TheFrame->KMotionDLL->Translate("\r\rcould not be read.  Attempt to recover settings from Backup file?\r"),
							  L"KMotion", MB_YESNO | MB_ICONSTOP)
							  != IDYES)
				{
					if (MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate("Previous Configuration File:\r\r") +  Name +
								  /*TRAN*/TheFrame->KMotionDLL->Translate("\r\rcould not be read.  Continuing will cause a loss of all\rsettings.  Are you sure you would like to continue?"),
								  L"KMotion", MB_YESNO|MB_ICONSTOP)
								  != IDYES)
						exit(0);
					
					iTry=1; // exit tries
				}
			}
			else
			{
				if (MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate("Backup Configuration File:\r\r") +  Name +
							  /*TRAN*/TheFrame->KMotionDLL->Translate("\r\rcould not be read.  Continuing will cause a loss of all\rsettings.  Are you sure you would like to continue?"),
							  L"KMotion", MB_YESNO|MB_ICONSTOP)
							  != IDYES)
					exit(0);
			}
		}
	}

	//initially set the Screen to be used to what is in the Configuration
	m_DialogFaceInUse = m_DialogFace;

	return 0;
}




void CKMotionCNCDlg::SaveFileNames()
{
	CString File = TheFrame->MainPath + GCODE_FILES;

	FILE *g;
	_tfopen_s(&g, File, _T("wt,ccs=UTF-8"));

	if (!g)
	{
		CString s;
		s.Format(/*TRAN*/TheFrame->KMotionDLL->Translate("Error opening file %ls\nTo store Previous GCode Filenames"),
				 File.GetBuffer(MAX_PATH));
		MessageBox(s, L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
	}
	else
	{
		for (int i=0; i<N_USER_GCODE_FILES && !feof(g); i++)
		{
			// if the path is the default, and there is no other path specified, strip it off
			fwprintf(g,L"%ls\n",StripPathMatch(FileNames[i], GCODE_SUB_DIR).GetBuffer());
		}

		fclose(g);
	}
}


void CKMotionCNCDlg::LoadFile(int thread,bool ResetPosition)
{
	CString s;
	CString DefaultPath = TheFrame->MainPathRoot + GCODE_SUB_DIR;
	char* UTF8;
	int BOM_offset = 0;  // offset if BOM found

	// A Thread whose file has since been deleted or moved (a remembered
	// name from a previous session, or the Thread about to receive another
	// file) silently becomes a blank, unnamed file - no error box, and the
	// previous Thread's text does not linger in the editor.
	if (!FileNames[thread].IsEmpty() &&
		GetFileAttributes(FileNames[thread]) == INVALID_FILE_ATTRIBUTES &&
		(GetLastError() == ERROR_FILE_NOT_FOUND || GetLastError() == ERROR_PATH_NOT_FOUND))
	{
		FileNames[thread] = "";
	}

	if (FileNames[thread].IsEmpty() || FileNames[thread] == DefaultPath)
	{
		// if filename for the thread is undefined
		// just set the editor empty

		UTF8 = new char[1];
		UTF8[0]=0;
	}
	else
	{
		CFileStatus filestatus;
		BOOL success = CFile::GetStatus(FileNames[thread], filestatus);

		FILE* f;
		_tfopen_s(&f, FileNames[thread], _T("rb,ccs=UTF-8"));

		if (!f || success != TRUE)
		{
			if (f) fclose(f);
			s.Format(/*TRAN*/TheFrame->KMotionDLL->Translate("Error opening GCode file %ls\nFrom Previous G Code Filenames"),
				FileNames[thread]);
			MessageBox(s, L"KMotion", MB_ICONSTOP | MB_OK | MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL);

			s = "";
			FileNames[thread] = "";

			// after the error the editor shows an empty, unnamed file rather
			// than whatever program was displayed before
			UTF8 = new char[1];
			UTF8[0] = 0;
		}
		else
		{
			UTF8 = new char[filestatus.m_size + 1];
			int result = (int)fread(UTF8, 1, filestatus.m_size, f);

			fclose(f);

			if (result != filestatus.m_size)
			{
				delete [] UTF8;

				s.Format(/*TRAN*/TheFrame->KMotionDLL->Translate("Error reading GCode file %ls"),
					FileNames[thread]);
				MessageBox(s, L"KMotion", MB_ICONSTOP | MB_OK | MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL);

				s = "";
				FileNames[thread] = "";

				UTF8 = new char[1];
				UTF8[0] = 0;
			}
			else
			{
				UTF8[filestatus.m_size] = 0;  // terminate

				// check for one or more BOM and remove them
				unsigned char BOM[3] = { 0xEF, 0xBB, 0xBF };
				while (filestatus.m_size >= 3 && memcmp(UTF8 + BOM_offset, BOM, 3) == 0)
				{
					BOM_offset += 3;
					filestatus.m_size -= 3;
				}
			}
		}
	}

	m_Editor.SetReadOnly(FALSE);
	m_Editor.SetText(UTF8 + BOM_offset);  // set text skipping over any BOMs
	delete [] UTF8;
	m_Editor.EmptyUndoBuffer();
	m_Editor.SetSavePoint();

	// For Read Only Settings reset to read only whenever a new file is loaded
	m_Editor.SetReadOnly(m_Editor.ReadWriteVar==0 || m_Editor.ReadWriteVar==2);

	DisplayedCurrentLine=-1;
	
	if (ResetPosition)
		OnRestart(); // reset to the beginning

	RefreshTitle();

	UpdateData(FALSE);

	RequestPreview(thread);	// G-code viewer: plot the whole file once the interpreter is idle
}


int CKMotionCNCDlg::SaveFile(int thread, bool ForceSave)
{
	char *UTF8;

	// if the user isn't explicitly saving and the 
	// user hasn't changed anything then don't save

	if (!ForceSave && !m_Editor.GetModify()) return 0;

	// save the edit window

	int nUTF8Length = m_Editor.GetTextLength();

	UTF8 = new char[nUTF8Length + 1];

	if (UTF8 == NULL)
	{
		MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate("unable to allocate memory"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
		return 1;
	}

	m_Editor.GetText(nUTF8Length + 1, UTF8);

	// if the file to save to is undefined and the editor is empty, forget it

	if (UTF8[0] == 0 && FileNames[thread].IsEmpty())
	{
		delete UTF8;
		return 0;
	}

	if (FileNames[thread].IsEmpty())
	{
		// if there is something to save, but the filename is undefined
		// ask user where to save it 

		delete UTF8;
		return DoSaveAs(thread);
	}

	FILE *f;
	_tfopen_s(&f, FileNames[thread], _T("wb,ccs=UTF-8"));

	if (!f)
	{
		// if we couldn't open the file and the editor is
		// empty, then just forget it

		if (UTF8[0] == 0)
		{
			delete UTF8;
			return 0;
		}

		delete UTF8;
		CString err;
		err.Format(/*TRAN*/TheFrame->KMotionDLL->Translate("Error saving GCode file %ls\nFrom Previous GCode Filenames\r\rUse SaveAs to save to different directory or filename"), FileNames[thread]);
		MessageBox(err, L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);

		return 1;
	}
	else
	{
		int length = (int)strlen(UTF8);
		unsigned char BOM[3] = { 0xEF, 0xBB, 0xBF };
		if (length > 0 &&
			(fwrite(BOM, 3, 1, f) != 1
			|| fwrite(UTF8, length, 1, f) != 1))
		{
			MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate("unable to write to file"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
			delete UTF8;
			return 1;
		}
		else
		{
			fclose(f);
		}
	}

	delete UTF8;
	m_Editor.SetSavePoint();  // clear the Modified Flag
	RefreshTitle();

	RequestPreview(thread);	// G-code viewer: the file changed on disk, re-plot the preview

	return 0;
}

void CKMotionCNCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDlgX::DoDataExchange(pDX);

	// if saving to variables save feed rate to old setting
	if (pDX->m_bSaveAndValidate)
	{
		if (m_Rapid==0)
		{
			DDX_Text(pDX, IDC_FeedRateEdit, m_FeedRateRapidValue);
			DDV_MinMaxDouble(pDX, m_FeedRateRapidValue, 0.001, 2.);
		}
		else
		{
			DDX_Text(pDX, IDC_FeedRateEdit, m_FeedRateValue);
			DDV_MinMaxDouble(pDX, m_FeedRateValue, 0.001, 2.);
		}
	}


	//{{AFX_DATA_MAP(CKMotionCNCDlg)
	DDX_Control(pDX, IDC_Command, m_Command);
	DDX_Control(pDX, IDC_tool, m_tool);
	DDX_Control(pDX, IDC_fixture, m_FixtureOffset);
	DDX_Control(pDX, IDC_Editor, m_Editor);
	DDX_Radio(pDX, IDC_Thread1, m_Thread);
	DDX_Radio(pDX, IDC_Rapid, m_Rapid);
	DDX_Check(pDX, IDC_Simulate, m_Simulate);
	DDX_Check(pDX, IDC_DoTime, m_DoTime);
	DDX_Check(pDX, IDC_BlockDelete, Interpreter->p_setup->block_delete);
	DDX_Check(pDX, IDC_ShowMach, m_ShowMach);
	DDX_Control(pDX, IDC_FeedRateEdit, m_FeedRateEdit);
	DDX_Control(pDX, IDC_SpindleRateEdit, m_SpindleRateEdit);
	DDX_Control(pDX, IDC_PosC, m_PosC);
	DDX_Control(pDX, IDC_PosB, m_PosB);
	DDX_Control(pDX, IDC_PosA, m_PosA);
	DDX_Control(pDX, IDC_PosZ, m_PosZ);
	DDX_Control(pDX, IDC_PosY, m_PosY);
	DDX_Control(pDX, IDC_PosX, m_PosX);
	DDX_Control(pDX, IDC_FeedSlider, m_FeedSlider);
	DDX_Control(pDX, IDC_SpindleSlider, m_SpindleSlider);
	DDX_Control(pDX, IDC_Zplus2, m_Zplus2);
	DDX_Control(pDX, IDC_Zplus, m_Zplus);
	DDX_Control(pDX, IDC_ZplusStep, m_ZplusStep);
	DDX_Control(pDX, IDC_Zminus2, m_Zminus2);
	DDX_Control(pDX, IDC_Zminus, m_Zminus);
	DDX_Control(pDX, IDC_ZminusStep, m_ZminusStep);

	DDX_Control(pDX, IDC_Aplus2, m_Aplus2);
	DDX_Control(pDX, IDC_Aplus, m_Aplus);
	DDX_Control(pDX, IDC_AplusStep, m_AplusStep);
	DDX_Control(pDX, IDC_Aminus2, m_Aminus2);
	DDX_Control(pDX, IDC_Aminus, m_Aminus);
	DDX_Control(pDX, IDC_AminusStep, m_AminusStep);

	DDX_Control(pDX, IDC_Bplus2, m_Bplus2);
	DDX_Control(pDX, IDC_Bplus, m_Bplus);
	DDX_Control(pDX, IDC_BplusStep, m_BplusStep);
	DDX_Control(pDX, IDC_Bminus2, m_Bminus2);
	DDX_Control(pDX, IDC_Bminus, m_Bminus);
	DDX_Control(pDX, IDC_BminusStep, m_BminusStep);

	DDX_Control(pDX, IDC_Cplus2, m_Cplus2);
	DDX_Control(pDX, IDC_Cplus, m_Cplus);
	DDX_Control(pDX, IDC_CplusStep, m_CplusStep);
	DDX_Control(pDX, IDC_Cminus2, m_Cminus2);
	DDX_Control(pDX, IDC_Cminus, m_Cminus);
	DDX_Control(pDX, IDC_CminusStep, m_CminusStep);

	DDX_Control(pDX, IDC_Right2, m_Right2);
	DDX_Control(pDX, IDC_Right, m_Right);
	DDX_Control(pDX, IDC_RightStep, m_RightStep);
	DDX_Control(pDX, IDC_StopStep, m_StopStep);
	DDX_Control(pDX, IDC_Down2, m_Down2);
	DDX_Control(pDX, IDC_Down, m_Down);
	DDX_Control(pDX, IDC_DownStep, m_DownStep);
	DDX_Control(pDX, IDC_Up2, m_Up2);
	DDX_Control(pDX, IDC_Up, m_Up);
	DDX_Control(pDX, IDC_UpStep, m_UpStep);
	DDX_Control(pDX, IDC_Left2, m_Left2);
	DDX_Control(pDX, IDC_Left, m_Left);
	DDX_Control(pDX, IDC_LeftStep, m_LeftStep);
	DDX_Text(pDX, IDC_SpindleRateEdit, m_SpindleRateValue);
	DDX_Text(pDX, IDC_SpindleRateEdit, m_SpindleRateValue);
	DDV_MinMaxDouble(pDX, m_SpindleRateValue, 0.1, 2.);
	DDX_CBString(pDX, IDC_Command, m_CommandString);
	DDV_MaxChars(pDX, m_CommandString, 256);
	DDX_Control(pDX, IDC_EmergencyStop, m_EmergencyStop);
	DDX_Control(pDX, IDC_GO, m_GO);
	DDX_Control(pDX, IDC_KeyJogMode, m_KeyJogMode);
	DDX_Control(pDX, IDC_FeedHold, m_FeedHold);
	DDX_Control(pDX, IDC_Forward, m_Forward);
	DDX_Control(pDX, IDC_Reverse, m_Reverse);
	DDX_Radio(pDX, IDC_Step0, m_StepSize);

	DDX_Control(pDX, IDC_Cust3, m_FR);
	DDX_Control(pDX, IDC_Cust2, m_SR);

	//}}AFX_DATA_MAP

	if (m_DialogFaceInUse == CUSTOM_DLG_FACE)
	{
		DDX_Control(pDX, IDC_PosV, m_PosV);
		DDX_Control(pDX, IDC_PosU, m_PosU);

		DDX_Control(pDX, IDC_Uplus2, m_Uplus2);
		DDX_Control(pDX, IDC_Uplus, m_Uplus);
		DDX_Control(pDX, IDC_UplusStep, m_UplusStep);
		DDX_Control(pDX, IDC_Uminus2, m_Uminus2);
		DDX_Control(pDX, IDC_Uminus, m_Uminus);
		DDX_Control(pDX, IDC_UminusStep, m_UminusStep);

		DDX_Control(pDX, IDC_Vplus2, m_Vplus2);
		DDX_Control(pDX, IDC_Vplus, m_Vplus);
		DDX_Control(pDX, IDC_VplusStep, m_VplusStep);
		DDX_Control(pDX, IDC_Vminus2, m_Vminus2);
		DDX_Control(pDX, IDC_Vminus, m_Vminus);
		DDX_Control(pDX, IDC_VminusStep, m_VminusStep);

		DDX_Control(pDX, IDC_ZeroU, m_ZeroU);
		DDX_Control(pDX, IDC_ZeroV, m_ZeroV);
		DDX_Control(pDX, IDC_SetU, m_SetU);
		DDX_Control(pDX, IDC_SetV, m_SetV);

		for (int i=0; i<MAX_USER_BUTTONS; i++)
			DDX_Control(pDX, IDC_But0+i, m_UserImageBut[i]);

		for (int i = 0; i<MAX_EDIT_CONTROLS; i++)
			DDX_Control(pDX, IDC_Edit0 + i, m_UserEditCtrls[i]);

		for (int i = 0; i<MAX_COMBO_CONTROLS; i++)
			DDX_Control(pDX, IDC_Combo0 + i, m_UserComboCtrls[i]);

		DDX_Control(pDX, IDC_STATICX, m_StaticLabelX);
		DDX_Control(pDX, IDC_STATICY, m_StaticLabelY);
		DDX_Control(pDX, IDC_STATICZ, m_StaticLabelZ);
		DDX_Control(pDX, IDC_STATICA, m_StaticLabelA);
		DDX_Control(pDX, IDC_STATICB, m_StaticLabelB);
		DDX_Control(pDX, IDC_STATICC, m_StaticLabelC);
		DDX_Control(pDX, IDC_STATICU, m_StaticLabelU);
		DDX_Control(pDX, IDC_STATICV, m_StaticLabelV);


		DDX_Control(pDX, IDC_FeedRateCmd, m_FeedRateCmd);
		DDX_Control(pDX, IDC_FeedRateLabel, m_FeedRateLabel);
		DDX_Control(pDX, IDC_SpindleRateCmd, m_SpindleRateCmd);
		DDX_Control(pDX, IDC_SpindleRateLabel, m_SpindleRateLabel);
		DDX_Control(pDX, IDC_STATIC10, m_STATIC10);
		DDX_Control(pDX, IDC_STATIC15, m_STATIC15);
		DDX_Control(pDX, IDC_STATIC20, m_STATIC20);
		DDX_Control(pDX, IDC_STATIC30, m_STATIC30);
		DDX_Control(pDX, IDC_STATIC40, m_STATIC40);
		DDX_Control(pDX, IDC_STATIC50, m_STATIC50);
		DDX_Control(pDX, IDC_STATIC75, m_STATIC75);
		DDX_Control(pDX, IDC_STATIC100, m_STATIC100);
		DDX_Control(pDX, IDC_STATIC150, m_STATIC150);
		DDX_Control(pDX, IDC_STATIC200, m_STATIC200);



		DDX_Control(pDX, IDC_STATICTOOL, m_StaticTool);
		DDX_Control(pDX, IDC_Thread, m_StaticThread);
		DDX_Control(pDX, IDC_SimulateStatic, m_SimulateStatic);
		DDX_Control(pDX, IDC_STATICUNITS, m_StaticUnits);
		DDX_Control(pDX, IDC_STATICFIXTURE_GROUP, m_StaticUnits);
		DDX_Control(pDX, IDC_STATICCOORD, m_StaticCoord);
		DDX_Control(pDX, IDC_STATICSTEP_SIZE, m_StaticStepSize);
		DDX_Control(pDX, IDC_BlockDeleteStatic, m_BlockDeleteStatic);
		DDX_Control(pDX, IDC_BlockDeleteStatic2, m_BlockDeleteStatic2);
		DDX_Control(pDX, IDC_DoTimeStatic, m_DoTimeStatic);
		DDX_Control(pDX, IDC_DoTimeStatic2, m_DoTimeStatic2);
		DDX_Control(pDX, IDC_STATICFIXTURE_GROUP, m_Static_Fixture);

		DDX_Control(pDX, IDC_Step0, m_RadioStep0);
		DDX_Control(pDX, IDC_Step1, m_RadioStep1);
		DDX_Control(pDX, IDC_Step2, m_RadioStep2);
		DDX_Control(pDX, IDC_Step3, m_RadioStep3);
		DDX_Control(pDX, IDC_Step4, m_RadioStep4);
		DDX_Control(pDX, IDC_Step5, m_RadioStep5);

		DDX_Control(pDX, IDC_Rapid, m_RapidRadio);
		DDX_Control(pDX, IDC_Feed, m_Feed);
		DDX_Control(pDX, IDC_mm, m_mm);
		DDX_Control(pDX, IDC_inch, m_inch);
		DDX_Control(pDX, IDC_Rel, m_Rel);
		DDX_Control(pDX, IDC_Abs, m_Abs);
		DDX_Control(pDX, IDC_Thread1, m_Thread1);
		DDX_Control(pDX, IDC_Thread2, m_Thread2);
		DDX_Control(pDX, IDC_Thread3, m_Thread3);
		DDX_Control(pDX, IDC_Thread4, m_Thread4);
		DDX_Control(pDX, IDC_Thread5, m_Thread5);
		DDX_Control(pDX, IDC_Thread6, m_Thread6);
		DDX_Control(pDX, IDC_Thread7, m_Thread7);

		DDX_Control(pDX, IDC_XY, m_GVXY);
		DDX_Control(pDX, IDC_YZ, m_GVYZ);
		DDX_Control(pDX, IDC_XZ, m_GVXZ);
		DDX_Control(pDX, IDC_RotXY, m_GVRotXY);
		DDX_Control(pDX, IDC_ClearPaths, m_GVClearPaths);
		DDX_Control(pDX, IDC_ShowAxis, m_GVShowAxis);
		DDX_Control(pDX, IDC_Box, m_GVBox);
		DDX_Control(pDX, IDC_ShowTool, m_GVShowTool);
		DDX_Control(pDX, IDC_Ortho, m_GVOrtho);
		DDX_Control(pDX, IDC_GViewerSetup, m_GVGViewerSetup);

		DDX_Control(pDX, IDC_New,m_GCNew);
		DDX_Control(pDX, IDC_OpenFile,m_GCOpenFile);
		DDX_Control(pDX, IDC_SaveFile,m_GCSaveFile);
		DDX_Control(pDX, IDC_SaveAs,m_GCSaveAs);
		DDX_Control(pDX, IDC_Restart,m_GCRestart);
		DDX_Control(pDX, IDC_SingleStep,m_GCSingleStep);
		DDX_Control(pDX, IDC_ToolSetup,m_GCToolSetup);
		DDX_Control(pDX, IDC_GView,m_GView);

		DDX_Control(pDX, IDC_viewctl, GViewControlParent.m_view);
	}
	else
	{
		for (int i = 0; i<NUSERBUTTONS; i++)
			DDX_Control(pDX, IDC_But0+i, m_UserImageBut[i]);
	}

	DDX_Control(pDX, IDC_ZeroX, m_ZeroX);
	DDX_Control(pDX, IDC_ZeroY, m_ZeroY);
	DDX_Control(pDX, IDC_ZeroZ, m_ZeroZ);
	DDX_Control(pDX, IDC_ZeroA, m_ZeroA);
	DDX_Control(pDX, IDC_ZeroB, m_ZeroB);
	DDX_Control(pDX, IDC_ZeroC, m_ZeroC);
	if (m_ZeroU.m_hWnd) DDX_Control(pDX, IDC_ZeroU, m_ZeroU);
	if (m_ZeroV.m_hWnd) DDX_Control(pDX, IDC_ZeroV, m_ZeroV);
	DDX_Control(pDX, IDC_SetX, m_SetX);
	DDX_Control(pDX, IDC_SetY, m_SetY);
	DDX_Control(pDX, IDC_SetZ, m_SetZ);
	DDX_Control(pDX, IDC_SetA, m_SetA);
	DDX_Control(pDX, IDC_SetB, m_SetB);
	DDX_Control(pDX, IDC_SetC, m_SetC);
	if (m_SetU.m_hWnd) DDX_Control(pDX, IDC_SetU, m_SetU);
	if (m_SetV.m_hWnd) DDX_Control(pDX, IDC_SetV, m_SetV);
	DDX_Control(pDX, IDC_EditToolFile, m_EditToolFile);
	DDX_Control(pDX, IDC_ZeroAll, m_ZeroAll);
	DDX_Control(pDX, IDC_Measure, m_Measure);
	DDX_Control(pDX, IDC_EditFixtures, m_EditFixtures);
	DDX_Control(pDX, IDC_SetFixture, m_SetFixture);
	DDX_Control(pDX, IDC_SpindleOnCW, m_SpindleOnCW);
	DDX_Control(pDX, IDC_SpindleOnCCW, m_SpindleOnCCW);
	DDX_Control(pDX, IDC_SpindleOff, m_SpindleOff);
	DDX_Control(pDX, IDC_FeedRateApply, m_FeedRateApply);
	DDX_Control(pDX, IDC_SpindleRateApply, m_SpindleRateApply);
	DDX_Control(pDX, IDC_RunSimulate, m_RunSimulate);
	DDX_Control(pDX, IDC_KMotion_HELP, m_KMotion_HELP);
	DDX_Control(pDX, IDC_Send, m_Send);

	// if restoring from variables get feed rate from new setting
	if (!pDX->m_bSaveAndValidate)
	{
		if (m_Rapid==0)
		{
			DDX_Text(pDX, IDC_FeedRateEdit, m_FeedRateRapidValue);
			DDV_MinMaxDouble(pDX, m_FeedRateRapidValue, 0.001, 2.);
		}
		else
		{
			DDX_Text(pDX, IDC_FeedRateEdit, m_FeedRateValue);
			DDV_MinMaxDouble(pDX, m_FeedRateValue, 0.001, 2.);
		}
	}
}

BEGIN_MESSAGE_MAP(CKMotionCNCDlg, CDlgX)
	//{{AFX_MSG_MAP(CKMotionCNCDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_SpindleRateApply, OnSpindleRateApply)
	ON_BN_CLICKED(IDC_FeedRateApply, OnFeedRateApply)
	ON_BN_CLICKED(IDC_GO, OnGO)
	ON_BN_CLICKED(IDC_EmergencyStop, OnEmergencyStop)
	ON_BN_CLICKED(IDC_ZeroAll, OnZeroAll)
	ON_BN_CLICKED(IDC_ZeroX, OnZeroX)
	ON_BN_CLICKED(IDC_ZeroY, OnZeroY)
	ON_BN_CLICKED(IDC_ZeroZ, OnZeroZ)
	ON_BN_CLICKED(IDC_ZeroA, OnZeroA)
	ON_BN_CLICKED(IDC_ZeroB, OnZeroB)
	ON_BN_CLICKED(IDC_ZeroC, OnZeroC)
	ON_BN_CLICKED(IDC_ZeroU, OnZeroU)
	ON_BN_CLICKED(IDC_ZeroV, OnZeroV)
	ON_BN_CLICKED(IDC_KMotion_HELP, OnIhelp)
	ON_BN_CLICKED(IDC_Rapid, OnRapid)
	ON_BN_CLICKED(IDC_Feed,  OnFeed)
	ON_BN_CLICKED(IDC_Thread1, OnThread1)
	ON_BN_CLICKED(IDC_Thread2, OnThread2)
	ON_BN_CLICKED(IDC_Thread3, OnThread3)
	ON_BN_CLICKED(IDC_Thread4, OnThread4)
	ON_BN_CLICKED(IDC_Thread5, OnThread5)
	ON_BN_CLICKED(IDC_Thread6, OnThread6)
	ON_BN_CLICKED(IDC_Thread7, OnThread7)
	ON_COMMAND(IDC_SaveFile, OnSaveFile)
	ON_COMMAND(IDC_New, OnNew)
	ON_COMMAND(IDC_OpenFile, OnOpenFile)
	ON_COMMAND(IDC_Execute, OnExecute)
	ON_COMMAND(IDC_SaveAs, OnSaveAs)
	ON_COMMAND(IDC_GCodeHalt, OnHalt)
	ON_COMMAND(IDC_ToolSetup, OnToolSetup)
	ON_BN_CLICKED(IDC_ExecuteComplete, OnExecuteComplete)
	ON_COMMAND(IDC_SingleStep, OnSingleStep)
	ON_COMMAND(IDC_Restart, OnRestart)
	ON_UPDATE_COMMAND_UI(IDC_Restart, OnUpdateRestart)
	ON_UPDATE_COMMAND_UI(IDC_SingleStep, OnUpdateSingleStep)
	ON_UPDATE_COMMAND_UI(IDC_New, OnUpdateNew)
	ON_UPDATE_COMMAND_UI(IDC_OpenFile, OnUpdateOpenFile)
	ON_COMMAND(IDC_GView, OnGView)
	ON_BN_CLICKED(IDC_Simulate, OnSimulate)
	ON_BN_CLICKED(IDC_DoTime, OnDoTime)
	ON_BN_CLICKED(IDC_BlockDelete, OnBlockDelete)
	ON_BN_CLICKED(IDC_ShowMach, OnShowMach)
	ON_CBN_DROPDOWN(IDC_fixture, OnDropdownfixture)
	ON_CBN_CLOSEUP(IDC_fixture, OnCloseupfixture)
	ON_CBN_CLOSEUP(IDC_tool, OnCloseuptool)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_Send, OnSend)
	ON_CBN_DROPDOWN(IDC_Command, OnDropdownCommand)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_But0, OnBut0)
	ON_BN_CLICKED(IDC_But1, OnBut1)
	ON_BN_CLICKED(IDC_But2, OnBut2)
	ON_BN_CLICKED(IDC_But3, OnBut3)
	ON_BN_CLICKED(IDC_But4, OnBut4)
	ON_BN_CLICKED(IDC_But5, OnBut5)
	ON_BN_CLICKED(IDC_But6, OnBut6)
	ON_BN_CLICKED(IDC_But7, OnBut7)
	ON_BN_CLICKED(IDC_But8, OnBut8)
	ON_BN_CLICKED(IDC_But9, OnBut9)
	ON_BN_CLICKED(IDC_Measure, OnMeasure)
	ON_BN_CLICKED(IDC_Step0, OnStep0)
	ON_BN_CLICKED(IDC_Step1, OnStep1)
	ON_BN_CLICKED(IDC_Step2, OnStep2)
	ON_BN_CLICKED(IDC_Step3, OnStep3)
	ON_BN_CLICKED(IDC_Step4, OnStep4)
	ON_BN_CLICKED(IDC_Step5, OnStep5)
	ON_BN_CLICKED(IDC_SetX, OnSetX)
	ON_BN_CLICKED(IDC_SetY, OnSetY)
	ON_BN_CLICKED(IDC_SetZ, OnSetZ)
	ON_BN_CLICKED(IDC_SetA, OnSetA)
	ON_BN_CLICKED(IDC_SetB, OnSetB)
	ON_BN_CLICKED(IDC_SetC, OnSetC)
	ON_BN_CLICKED(IDC_SetU, OnSetU)
	ON_BN_CLICKED(IDC_SetV, OnSetV)
	ON_BN_CLICKED(IDC_StopStep, OnStopStep)
	ON_BN_CLICKED(IDC_mm, Onmm)
	ON_BN_CLICKED(IDC_inch, Oninch)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_Abs, OnAbs)
	ON_BN_CLICKED(IDC_Rel, OnRel)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_FeedHold, &CKMotionCNCDlg::OnBnClickedFeedhold)
	ON_BN_CLICKED(IDC_EditToolFile, &CKMotionCNCDlg::OnBnClickedEdittoolfile)
	ON_BN_CLICKED(IDC_EditFixtures, &CKMotionCNCDlg::OnBnClickedEditfixtures)
	ON_EN_SETFOCUS(IDC_FeedRateEdit, &CKMotionCNCDlg::OnEnSetfocusFeedrateedit)
	ON_EN_SETFOCUS(IDC_SpindleRateEdit, &CKMotionCNCDlg::OnEnSetfocusSpindlerateedit)
	ON_BN_SETFOCUS(IDC_SetX, &CKMotionCNCDlg::OnBnSetfocusSetx)
	ON_BN_SETFOCUS(IDC_SetY, &CKMotionCNCDlg::OnBnSetfocusSety)
	ON_BN_SETFOCUS(IDC_SetZ, &CKMotionCNCDlg::OnBnSetfocusSetz)
	ON_BN_SETFOCUS(IDC_SetA, &CKMotionCNCDlg::OnBnSetfocusSeta)
	ON_BN_SETFOCUS(IDC_SetB, &CKMotionCNCDlg::OnBnSetfocusSetb)
	ON_BN_SETFOCUS(IDC_SetC, &CKMotionCNCDlg::OnBnSetfocusSetc)
	ON_CBN_SETFOCUS(IDC_Command, &CKMotionCNCDlg::OnCbnSetfocusCommand)
	ON_WM_SIZING()
	ON_BN_CLICKED(IDC_SetFixture, &CKMotionCNCDlg::OnBnClickedSetfixture)
	ON_BN_CLICKED(IDC_RunSimulate, &CKMotionCNCDlg::OnBnClickedRunsimulate)
	ON_BN_CLICKED(IDC_SpindleOnCW, &CKMotionCNCDlg::OnBnClickedSpindleoncw)
	ON_COMMAND(ID_F9, OnBnClickedSpindleoncw)	
	ON_BN_CLICKED(IDC_SpindleOnCCW, &CKMotionCNCDlg::OnBnClickedSpindleonccw)
	ON_COMMAND(ID_F10, OnBnClickedSpindleonccw)	
	ON_BN_CLICKED(IDC_SpindleOff, &CKMotionCNCDlg::OnBnClickedSpindleoff)
	ON_COMMAND(ID_F11, OnBnClickedSpindleoff)	
	ON_WM_NCACTIVATE()
	ON_COMMAND(ID_ReloadGeoCorrection, OnReloadGeoCorrection)
	ON_COMMAND(ID_ReloadGCodeFile, OnReloadGCodeFile)
	ON_COMMAND(ID_OpenGCodeFile, OnOpenGCodeFile)
	ON_COMMAND_EX(ID_ScreenScript, OnScreenScript)
	ON_COMMAND_EX(ID_DoJog, OnDoJog)
	ON_COMMAND_EX(ID_DoMoveExp, OnDoMoveExp)
	ON_COMMAND_EX(ID_DoMove, OnDoMove)
	ON_COMMAND_EX(ID_GetPositions, OnGetPositions)
	ON_COMMAND_EX(ID_GetPositionsNoGeo, OnGetPositionsNoGeo)
	ON_COMMAND_EX(ID_GetControlInfo, OnGetControlInfo)
	ON_COMMAND_EX(ID_MainDlgInfo, OnMainDlgInfo)
	ON_WM_COPYDATA()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	ON_MESSAGE(WM_NOTIFYFORMAT, OnNotifyFormat)
	ON_NOTIFY(TTN_SHOW, 0, OnToolTipTextAboutToShow)

	// GViewer functions
	ON_COMMAND(IDC_XY, OnXy)
	ON_COMMAND(IDC_XZ, OnXz)
	ON_COMMAND(IDC_YZ, OnYz)
	ON_COMMAND(IDC_ClearPaths, OnClearPaths)
	ON_COMMAND(IDC_ShowAxis, OnShowAxis)
	ON_COMMAND(IDC_Ortho, OnOrtho)
	ON_COMMAND(IDC_Box, OnBox)
	ON_COMMAND(IDC_RotXY, OnRotXY)
	ON_COMMAND(IDC_GViewerSetup, OnGViewerSetup)
	ON_COMMAND(IDC_ShowTool, OnShowTool)
	ON_WM_ERASEBKGND()
	ON_WM_NCMOUSEHOVER()
	ON_WM_NCMOUSELEAVE()
	ON_WM_NCMOUSEMOVE()
	END_MESSAGE_MAP()



BEGIN_EASYSIZE_MAP(CKMotionCNCDlg)

	if (m_DialogFaceInUse == 8)
	{

//                         left          top       right     bottom
	EASYSIZE(IDC_Thread, ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_Thread1,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_Thread2,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_Thread3,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_Thread4,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_Thread5,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_Thread6,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_Thread7,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)

	// maybe need to be in order of dependencies

    EASYSIZE(IDC_Send,				ES_KEEPSIZE, ES_KEEPSIZE,			ES_BORDER, ES_BORDER,		 0)
    EASYSIZE(IDC_KMotion_HELP,		ES_KEEPSIZE, IDC_Simulate,			ES_BORDER, IDC_Send,		 ES_VCENTER)
	EASYSIZE(IDC_Simulate,			ES_KEEPSIZE, IDC_Thread7,			ES_BORDER, IDC_KMotion_HELP, ES_VCENTER)	//centered between Thread7 and Help
	EASYSIZE(IDC_SimulateStatic,	ES_KEEPSIZE, ES_KEEPSIZE,			ES_BORDER, IDC_Simulate,	 0)				// Move relative to Simulate
	EASYSIZE(IDC_RunSimulate,		ES_KEEPSIZE, ES_KEEPSIZE,			ES_BORDER, IDC_Simulate,	 0)				// Moverelative to Simulate
	EASYSIZE(IDC_DoTimeStatic,		ES_KEEPSIZE, ES_KEEPSIZE,			ES_BORDER, IDC_RunSimulate,  0)
	EASYSIZE(IDC_DoTime,			ES_KEEPSIZE, ES_KEEPSIZE,			ES_BORDER, IDC_DoTimeStatic, 0)
	EASYSIZE(IDC_BlockDelete,		ES_KEEPSIZE, IDC_SimulateStatic,	ES_BORDER, IDC_DoTime,		 0)
    EASYSIZE(IDC_BlockDeleteStatic,	ES_KEEPSIZE, ES_KEEPSIZE,			ES_BORDER, IDC_BlockDelete,	 0)				// Move relative to Block Delete

//                         left        top       right     bottom
	EASYSIZE(IDC_Editor, ES_BORDER,ES_BORDER,  ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_Command,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)

    EASYSIZE(IDC_But0,          ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
    EASYSIZE(IDC_But1,          ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
    EASYSIZE(IDC_But2,          ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
    EASYSIZE(IDC_But3,          ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
    EASYSIZE(IDC_But4,          ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
    EASYSIZE(IDC_But5,          ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
    EASYSIZE(IDC_But6,          ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
    EASYSIZE(IDC_But7,          ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
    EASYSIZE(IDC_But8,          ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
    EASYSIZE(IDC_But9,          ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)

    EASYSIZE(IDC_Zminus,       ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)

//                                left          top    right     bottom
	EASYSIZE(IDC_FeedHold,      ES_BORDER,IDC_Zminus,ES_KEEPSIZE,IDC_But0,ES_VCENTER)
	EASYSIZE(IDC_Forward,		ES_KEEPSIZE,ES_KEEPSIZE,IDC_FeedHold,IDC_FeedHold,0)
	EASYSIZE(IDC_Reverse,		ES_KEEPSIZE,ES_KEEPSIZE,IDC_FeedHold,IDC_FeedHold,0)
	EASYSIZE(IDC_EmergencyStop, ES_BORDER,IDC_Zminus,ES_KEEPSIZE,IDC_But0,ES_VCENTER)
	
	}

END_EASYSIZE_MAP



/////////////////////////////////////////////////////////////////////////////
//  message handlers



void CKMotionCNCDlg::OnIhelp() 
{
	DisableKeyJog();
	TheFrame->HelpDlg.Show("KMotionCNC\\KMotionCNC.htm");
}

void CKMotionCNCDlg::OnRapid() 
{
	if (!UpdateData(TRUE)) return;  // Update integer
	UpdateData(FALSE);
	m_FeedSlider.SetPos(m_FeedRateRapidValue);
	Interpreter->CoordMotion->SetFeedRateRapidOverride(m_FeedRateRapidValue);
}

void CKMotionCNCDlg::OnFeed() 
{
	if (!UpdateData(TRUE)) return;  // Update integer
	UpdateData(FALSE);
	m_FeedSlider.SetPos(m_FeedRateValue);
	Interpreter->CoordMotion->SetFeedRateOverride(m_FeedRateValue);
}


void CKMotionCNCDlg::OnThread1() 
{
	ThreadChanged();
}

void CKMotionCNCDlg::OnThread2() 
{
	ThreadChanged();
}

void CKMotionCNCDlg::OnThread3() 
{
	ThreadChanged();
}

void CKMotionCNCDlg::OnThread4() 
{
	ThreadChanged();
}

void CKMotionCNCDlg::OnThread5() 
{
	ThreadChanged();
}

void CKMotionCNCDlg::OnThread6() 
{
	ThreadChanged();
}

void CKMotionCNCDlg::OnThread7() 
{
	ThreadChanged();
}

void CKMotionCNCDlg::ThreadChanged()
{
	int PrevThread=m_Thread;
	UpdateData(TRUE);  // save the editor box

	if (SaveFile(PrevThread,false)) // don't force the save
	{
		// for some reason can't save the previous file
		// switch back to this previous thread number

		m_Thread=PrevThread;
		UpdateData(false);
		return;
	}

	LoadFile(m_Thread,false);
}

void CKMotionCNCDlg::OnSaveFile() 
{
	SaveFile(m_Thread, true); // force the save
}

void CKMotionCNCDlg::OnNew() 
{
	CString File;
	
	File.Format(L"user%d.ngc",m_Thread+1);

	int answer = MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate("Overwrite File:") + File+" ?"
		, L"KMotion", MB_ICONSTOP|MB_YESNO|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);

	if (answer != IDYES) return;

	BOOL PrevState = m_Editor.GetReadOnly();
	m_Editor.SetReadOnly(FALSE);
	m_Editor.SetText("");
	m_Editor.SetReadOnly(PrevState);

	FileNames[m_Thread] = File;

	RefreshTitle();
	
	UpdateData(FALSE);

	SaveFile(m_Thread, true); // force the save

	// set focus to it
	GetDlgItem(IDC_Editor)->SetFocus();
}

void CKMotionCNCDlg::OnOpenFile() 
{
	CString s;
	
	DisableKeyJog();
	CPersistOpenDlg FileDlg (TRUE, L".ngc", 
		TheFrame->GCodeDlg.InitialFile(FileNames[m_Thread], GCODE_SUB_DIR, L"Dynomotion.ngc"),
				OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ENABLESIZING, 
		/*TRAN*/L"GCode Files (*.ngc *.nc *.ncc *.cnc *.tap *.txt)|*.ngc;*.nc;*.ncc;*.cnc;*.tap;*.txt|All Files (*.*)|*.*||");

	if (FileDlg.DoModal() == IDOK)
	{
		FileNames[m_Thread] = FileDlg.GetPathName();
		LoadFile(m_Thread,true);
		CurrentDirectory = ExtractDirectory(FileNames[m_Thread]);
	}
}


void CKMotionCNCDlg::OnSaveAs() 
{
	DoSaveAs(m_Thread);
}


int CKMotionCNCDlg::DoSaveAs(int thread) 
{
	CString NoFile;

	DisableKeyJog();
	CPersistOpenDlg FileDlg (FALSE, L".ngc",
		TheFrame->GCodeDlg.InitialFile(FileNames[thread], GCODE_SUB_DIR, L""),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING,
		/*TRAN*/L"GCode Files (*.ngc *.nc *.ncc *.cnc *.tap *.txt)|*.ngc;*.nc;*.ncc;*.cnc;*.tap;*.txt|All Files (*.*)|*.*||");

	// check if the last filename exists, if not
	// don't try to select it otherwise the file dialog fails

	FILE *f=NULL;
	
	if (!FileNames[thread].IsEmpty())
		_tfopen_s(&f, FileNames[thread], _T("rt,ccs=UTF-8"));


	if (f)
	{
		fclose(f);
	}
	else
	{
		FileDlg.m_ofn.lpstrFile=NoFile.GetBuffer(MAX_PATH);
	}
	
	if (FileDlg.DoModal() == IDOK)
	{
		FileNames[thread] = FileDlg.GetPathName();
		SaveFile(thread, true);  // force the save
	}
	else
	{
		return 1;
	}
	return 0;
}



void CKMotionCNCDlg::RefreshTitle() 
{
	// no need, timer does it all the time
}


void CKMotionCNCDlg::OnExecuteComplete()
{
	// G-code viewer: the quiet preview simulation finished.  The interpreter
	// thread still runs a little after the complete callback (tool-wear stats),
	// so state is restored by ServicePreview once that thread has exited.
	if (m_PreviewRunning)
	{
		m_PreviewFinishOk = (m_exitcode == 0 || m_exitcode == RS274NGC_ENDFILE || m_exitcode == RS274NGC_EXIT);
		m_PreviewFinishPending = true;
		ServicePreview();
		return;
	}

	// G-code viewer: a run over the preview stopped part way (Single Step, run to
	// a line, M0/M1, Halt).  Remember where, in the preview's block numbering, so
	// a run continuing from that line maps exactly - also inside loops and
	// subroutines, where one file line is drawn many times.  A halted run's
	// state is restored to the halted block, which the resumed run re-reads.
	if (m_PlaybackArmed || m_ProgressActive)
	{
		m_ChainValid = m_CompleteLine > 0 &&
			(m_exitcode == 0 || (m_exitcode == 1005 && m_CompleteStopped));
		m_ChainLine = m_CompleteLine;
		m_ChainSeq = m_CompleteSeq + m_ProgressSeqOffset - (m_exitcode == 0 ? 0 : 1);
	}

	// G-code viewer: Simulate over the preview finished -> play back what it executed
	if (m_PlaybackArmed)
	{
		m_PlaybackArmed = false;
		m_ProgressActive = false;
		int n = ActualGViewParent->m_Path->NbVertex();
		if (m_PreviewValid && m_PreviewTime.GetSize() == n && m_PreviewFileLine.GetSize() == n)
		{
			m_PlaybackEnd = PreviewIndexAfterRun();
			if (m_PlaybackEnd > m_ProgressIndex)
			{
				m_PlaybackTime = m_ProgressIndex > 0 ? m_PreviewTime[m_ProgressIndex - 1] : 0.0;
				m_PlaybackWall = ElapsedTimer.Elapsed_Seconds();
				m_PlaybackPaused = false;
				m_PlaybackRunning = true;
			}
		}
	}
	// G-code viewer: a job over the preview finished - what it executed is all cut
	else if (m_ProgressActive)
	{
		int e = PreviewIndexAfterRun();
		if (e > m_ProgressIndex)
		{
			ColorPreviewRange(m_ProgressIndex, e - 1, true);
			m_ProgressIndex = e;
		}
	}

	if (m_Simulate)
	{
		if (m_DoingSimulationRun)
		{
			m_Simulate = false;
			Interpreter->CoordMotion->m_Simulate = false;
			UpdateData(FALSE);
			m_DoingSimulationRun = false;
		}
		if (m_DoTime)
		{
			JobDoTimeSecs = Interpreter->CoordMotion->m_TotalDoTime;
			JobDoTimeValid = true;
		}
	}
	else
	{
		if (JobEndTimeValid && JobStartTimeValid)
			LogJobEndTime(JobEndTimeSecs-JobStartTimeSecs);
	}

	if (m_exitcode)
	{
		CWnd *Focus=GetFocus();
		if (Focus)
		{
			CWnd *Parent=Focus->GetParent();
			// Switch focus to Editor unless the MDI has Focus 
			if (Parent && Parent->GetDlgCtrlID() != IDC_Command)
				GetDlgItem(IDC_Editor)->SetFocus();
		}
		else
		{
			GetDlgItem(IDC_Editor)->SetFocus();
		}
	}
}



void CompleteCallback(int status, int lineno, int sequence_number, const wchar_t *err)
{
	CKMotionCNCDlg *p=&TheFrame->GCodeDlg;
	CCoordMotion *CM = p->Interpreter->CoordMotion;

	// G-code viewer preview simulation finished: no editor line, error pop-up,
	// job time or resume bookkeeping -- FinishPreview (GUI thread) restores state.
	if (p->m_PreviewRunning)
	{
		p->m_exitcode = status;
		p->m_PreviewSeq0 = p->Interpreter->p_setup->percent_flag == ON ? 1 : 0;	// before FinishPreview restores p_setup
		p->ThreadIsExecuting = false;
		if (p->m_hWnd)
			p->PostMessage(WM_COMMAND, IDC_ExecuteComplete, 0);
		return;
	}

	p->m_ErrorOutput=err;
	p->m_exitcode=status;
	// G-code viewer: where this run stopped, for playback / progress and the next run
	p->m_CompleteLine = lineno;
	p->m_CompleteSeq = sequence_number;
	p->m_CompleteStopped = CM->m_Stopping != STOPPED_NONE;

	if (p->m_ThreadThatWasLaunched>=0)
		p->GCodeThreadActive[p->m_ThreadThatWasLaunched]=false;

	if (status)
	{
		p->GCodeMutex->Lock();
		p->m_ErrorOutput=err;
		p->GCodeMutex->Unlock();
		
		// we know what line number
		// mark it in the edit cell

		if (p->m_ThreadThatWasLaunched>=0 &&
			(CM->m_Stopping == STOPPED_COORD ||
			 CM->m_Stopping == STOPPED_COORD_FINISHED ||
			 CM->m_Stopping == STOPPED_INDEP))

		{
			p->CurrentLine[p->m_ThreadThatWasLaunched]=lineno;

			// if we stopped on the first of two things
			//    Arc(ID=1) Arc(ID=2)
			// or Arc(ID=1) Linear(ID=3)
			// Then delete everything associated with the gcode line
			// otherwise only delete up to but not including ID1
			MOTION_PARAMS *MP=&TheFrame->GCodeDlg.Interpreter->CoordMotion->Kinematics->m_MotionParams;
			double x=CM->m_StoppedMachinex,y=CM->m_StoppedMachiney,z=CM->m_StoppedMachinez;

			// rotation about X axis changes Y and Z Axis as a function of A
			if (TheFrame->GViewDlg.m_IncludeA && MP->DegreesA && MP->RadiusA>0.0)
			{
				double Theta=CM->m_StoppedMachinea*PI/180.0;
				double y0 = y+MP->RadiusA;
				y = y0 * cos(Theta) - z * sin(Theta);
				z = y0 * sin(Theta) + z * cos(Theta);
			}

			// rotation about Y axis changes Z and X Axis as a function of B
			if (TheFrame->GViewDlg.m_IncludeB && MP->DegreesB && MP->RadiusB>0.0)
			{
				double Theta=CM->m_StoppedMachineb*PI/180.0;
				double z0 = z+MP->RadiusB;
				z = z0 * cos(Theta) - x * sin(Theta);
				x = z0 * sin(Theta) + x * cos(Theta);
			}

			// rotation about Z axis changes X and Y Axis as a function of C
			if (TheFrame->GViewDlg.m_IncludeC && MP->DegreesC && MP->RadiusC>0.0)
			{
				double Theta=CM->m_StoppedMachinec*PI/180.0;
				double x0 = x+MP->RadiusC;
				x = x0 * cos(Theta) - y * sin(Theta);
				y = x0 * sin(Theta) + y * cos(Theta);
			}

			// (not over a kept preview: its unexecuted part stays drawn, uncut)
			if (!p->m_ProgressActive)
			{
				if (CM->m_PreviouslyStoppedID==1 &&
					CM->m_Stopping == STOPPED_COORD)
					p->ActualGViewParent->m_Path->RemovePathEnd(sequence_number,-1,x,y,z);
				else
					p->ActualGViewParent->m_Path->RemovePathEnd(sequence_number,1,x,y,z);

				p->ActualGViewParent->m_Path->SetModified();
			}
		}

		if (p->m_ThreadThatWasLaunched>=0)
			p->ThreadHadError[p->m_ThreadThatWasLaunched]=true;

		if (!p->ShuttingDownApplication && 
			CM->m_Stopping == STOPPED_NONE &&
			status != 1005)
			MessageBox(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("G Code Error\r\r") +  p->m_ErrorOutput,
				L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
	}
	else
	{
		if (p->m_ThreadThatWasLaunched>=0)
		{
			p->ThreadHadError[p->m_ThreadThatWasLaunched]=false;
			p->CurrentLine[p->m_ThreadThatWasLaunched]=lineno;
		}
	}
	
	p->ThreadIsExecuting=false;

	// record time when we stopped
	if (p->m_ThreadThatWasLaunched != -1)
	{
		p->JobEndTimeSecs = p->ElapsedTimer.Elapsed_Seconds();
		p->JobEndTimeValid = true;
		p->JobDoTimeSecs = p->Interpreter->CoordMotion->m_TotalDoTime;
		if (p->m_Simulate && p->m_DoTime)
			p->JobDoTimeValid = true;
	}
	if (p->m_RestoreStoppedState)
	{
		CM->m_PreviouslyStopped = p->m_SaveStoppedState;
		p->m_RestoreStoppedState=false;
	}
	else
	{
		// keep track of which Thread was running when the Interpreter 
		// stopped so we don't try to resume those conditions in a different file
		p->m_ThreadThatWasOriginallyStopped=p->m_ThreadThatWasLaunched;
	}


	// post message so GUI thread updates screens

	if (p->m_hWnd)
		p->PostMessage(WM_COMMAND,IDC_ExecuteComplete,0);


	if (p->m_PerformPostHaltCommand)
	{
		p->m_PerformPostHaltCommand=false;
		if (!p->m_Simulate)
			p->Interpreter->InvokeAction(ACTION_HALT,FALSE);  // Do Special Action
	}
}

void StatusCallback(int line_no, const wchar_t *msg)
{
	CKMotionCNCDlg *p=&TheFrame->GCodeDlg;

	p->GCodeMutex->Lock();

	if (p->m_ThreadThatWasLaunched>=0 && !p->m_PreviewRunning)	// a preview must not move the editor's line
		p->CurrentLine[p->m_ThreadThatWasLaunched]=line_no;

	p->GCodeMutex->Unlock();
}

void CKMotionCNCDlg::OnGO()
{
	if (ThreadIsExecuting)
		OnHalt();
	else if (m_PlaybackRunning)
		StopPlayback();		// Simulate playback is only an animation: nothing to Halt
	else
		OnExecute();
}

void CKMotionCNCDlg::OnExecute()
{
	if (!ThreadIsExecuting)
	{
		Interpreter->CoordMotion->ClearAbort();
		Interpreter->CoordMotion->ClearHalt();
		Interpreter->m_Halt = false;
		if (!m_Simulate)
			Interpreter->InvokeAction(ACTION_CYCLE_START, FALSE);  // Do Special Action

		MSG msg;
		while (Interpreter->m_InvokeThreadID != -1 && GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (Interpreter->GetAbort() || Interpreter->GetHalt()) return;  // if any Start Action set Halt or Abort

		if (SaveFile(m_Thread,false)) return;  // don't force the save
		m_ThreadThatWasLaunched=m_Thread;
		m_RestoreStoppedState=false;

		JobStartTimeSecs=ElapsedTimer.Elapsed_Seconds();
		JobStartTimeValid=true;
		JobEndTimeValid=false;
		JobDoTimeValid=false;
		JobDoTimeSecs = 0.0;
		if (LaunchExecution(FileNames[m_Thread],CurrentLine[m_Thread],-1)) return;
		GCodeThreadActive[m_Thread]=1;
	}
}

void CKMotionCNCDlg::OnSingleStep() 
{
	if (!ThreadIsExecuting)
	{
		Interpreter->CoordMotion->ClearAbort();
		Interpreter->CoordMotion->ClearHalt();
		Interpreter->m_Halt = false;
		Interpreter->InvokeAction(ACTION_CYCLE_START, FALSE);  // Do Special Action

		MSG msg;
		while (Interpreter->m_InvokeThreadID != -1 && GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (Interpreter->GetAbort() || Interpreter->GetHalt()) return;  // if any Start Action set Halt or Abort
		if (SaveFile(m_Thread,false)) return;  // don't force the save
		m_ThreadThatWasLaunched=m_Thread;
		m_RestoreStoppedState=false;

		JobStartTimeSecs=ElapsedTimer.Elapsed_Seconds();
		JobStartTimeValid=true;
		JobEndTimeValid = false;
		JobDoTimeValid = false;
		JobDoTimeSecs = 0.0;

		if (LaunchExecution(FileNames[m_Thread],CurrentLine[m_Thread],CurrentLine[m_Thread])) return;
		GCodeThreadActive[m_Thread]=1;
	}
}

void CKMotionCNCDlg::OnRestart() 
{
	ThreadHadError[m_Thread]=false;
	CurrentLine[m_Thread]=0;
	Interpreter->CoordMotion->m_PreviouslyStopped = STOPPED_NONE;
	Interpreter->p_setup->ConcaveDefered = FALSE;
}


int CKMotionCNCDlg::LaunchExecution(CString InFile,int begin, int end)
{
	int BoardType=BOARD_TYPE_UNKNOWN;

	if (InFile.IsEmpty())
	{
		MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate("Invalid Filename Specified"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
		return 1;
	}

	// A quiet preview simulation is running; don't start anything else until it
	// finishes (a fraction of a second to a few seconds after loading a file).
	if (m_PreviewRunning && !m_PreviewLaunching) return 1;

	StopPlayback();		// a Simulate playback of the previous run is only a picture

	// The preview runs simulated WITHOUT touching the dialog's Simulate checkbox
	// (m_Simulate), so the DROs etc. keep showing the machine.
	bool Simulating = m_Simulate || m_PreviewLaunching;

	Interpreter->CoordMotion->SetStraightTraverseCallback(StraightTraverseCallback);
	Interpreter->CoordMotion->SetStraightFeedCallback(StraightFeedCallback);
	Interpreter->CoordMotion->SetArcFeedCallback(ArcFeedCallback);

	Interpreter->CoordMotion->m_Simulate = Simulating;
	Interpreter->CoordMotion->m_DoTime = (m_DoTime != 0) && !m_PreviewLaunching;
	Interpreter->CoordMotion->m_DisableSoftLimits = false;

	SetMotionParams();


	if (!Simulating)
		if (Interpreter->CoordMotion->KMotionDLL->CheckKMotionVersion( &BoardType)) return 1;

	Interpreter->CoordMotion->KMotionDLL->BoardID=board;
	Interpreter->CoordMotion->m_board_type=BoardType;
	Interpreter->CoordMotion->ClearAbort();
	Interpreter->CoordMotion->m_AxisDisabled=false;
	Interpreter->CoordMotion->ClearHalt();

	Interpreter->CoordMotion->SetFeedRateOverride(m_FeedRateValue);
	Interpreter->CoordMotion->SetFeedRateRapidOverride(m_FeedRateRapidValue);

	Interpreter->p_setup->AllowConcaveCorners = m_AllowConcaveCorners;


	if (!Simulating && CheckForResumeCircumstances()) return 1; // check if we should move back to where we stopped

	// G-code viewer: a real run over an up-to-date preview keeps the preview and
	// colours it as it is cut, instead of clearing and plotting live.
	// A Simulate run over an up-to-date preview also keeps it: the simulation
	// itself is near-instant, so the cut is played back afterwards (UpdatePlayback).
	if (m_PlaybackRunning) StopPlayback();	// e.g. the next Single Step before the last one finished playing
	if (!Simulating)
		ArmProgress(begin, InFile);
	else if (!m_PreviewLaunching)
	{
		ArmProgress(begin, InFile);
		m_PlaybackArmed = m_ProgressActive;
	}
	else
		m_ProgressActive = false;

	ThreadIsExecuting=true;
	EnableJogKeys=false;

	if (begin==0 && m_ThreadThatWasLaunched!=-1 && !m_ProgressActive)
	{
		m_PrevABCPlotValid = false;
		ActualGViewParent->ClearPaths();
	}

	if (m_ThreadThatWasLaunched!=-1)
		ThreadHadError[m_ThreadThatWasLaunched]=false;

	Interpreter->Interpret(BoardType,InFile,begin,end,false,StatusCallback,CompleteCallback);
	
	return 0;
}


// check if we should move back to where we stopped

int CKMotionCNCDlg::CheckForResumeCircumstances()
{
	double Machinex,Machiney,Machinez,Machinea,Machineb,Machinec;

	CCoordMotion *CM = Interpreter->CoordMotion;
	setup_pointer ps = Interpreter->p_setup;

	if (!CM->m_PreviouslyStopped || m_ThreadThatWasOriginallyStopped != m_ThreadThatWasLaunched)
	{
		return Interpreter->ReadAndSyncCurPositions(&ps->current_x,&ps->current_y,&ps->current_z,&ps->AA_current,&ps->BB_current,&ps->CC_current);
	}

	// read the current positions

	if (CM->ReadCurAbsPosition(&Machinex,&Machiney,&Machinez,&Machinea,&Machineb,&Machinec,true)) return 1;

	if (!CM->m_PreviouslyStopped) return 0;

	// "Moved" must be judged with a small tolerance, not exact equality:
	// on machines with a Kinematics class the Actuator->CAD direction is
	// an ITERATIVE inversion converging to ~1e-6, so two reads of the
	// SAME (unmoved) destinations land at slightly different points in
	// the convergence ball - exact equality only ever held for trivial
	// linear kinematics where both sides derive bit-identically from
	// counts.  1e-5 (0.25um on linear axes, negligible on degree axes)
	// is 10x the inversion tolerance and far below any real motion.
	#define RESUME_MOVED_TOL 1e-5
	if (((CM->x_axis < 0) || (fabs(Machinex - CM->m_StoppedMachinex) <= RESUME_MOVED_TOL)) &&
		((CM->y_axis < 0) || (fabs(Machiney - CM->m_StoppedMachiney) <= RESUME_MOVED_TOL)) &&
		((CM->z_axis < 0) || (fabs(Machinez - CM->m_StoppedMachinez) <= RESUME_MOVED_TOL)) &&
		((CM->a_axis < 0) || (fabs(Machinea - CM->m_StoppedMachinea) <= RESUME_MOVED_TOL)) &&
		((CM->b_axis < 0) || (fabs(Machineb - CM->m_StoppedMachineb) <= RESUME_MOVED_TOL)) &&
		((CM->c_axis < 0) || (fabs(Machinec - CM->m_StoppedMachinec) <= RESUME_MOVED_TOL))) return 0;


	CResumeDlg ResumeDlg;

	ResumeDlg.m_SafeZ = Interpreter->InchesToUserUnits(m_SafeZ);
	ResumeDlg.m_SafeRelAbs = m_SafeRelAbs;
	ResumeDlg.m_MoveToSafeZ = TRUE;
	ResumeDlg.m_TraverseXY = TRUE;
	ResumeDlg.m_Metric = Interpreter->p_setup->length_units == CANON_UNITS_MM;


	// DISPLAY the positions rounded to 0.1um (metric) / 1uinch so the
	// dialog is not cluttered with iterative-inversion noise digits -
	// but COMMAND the exact stop position: if the user leaves a field
	// at its displayed value the exact unrounded position is restored
	// below (some machines have nanometer-scale precision), while an
	// edited field is honored as typed.
	double PosQuantum = ResumeDlg.m_Metric ? 0.0001 : 0.000001;
	double ExactX = CM->m_Stoppedx, ExactY = CM->m_Stoppedy, ExactZ = CM->m_Stoppedz;
	double DispX = floor(ExactX / PosQuantum + 0.5) * PosQuantum;
	double DispY = floor(ExactY / PosQuantum + 0.5) * PosQuantum;
	double DispZ = floor(ExactZ / PosQuantum + 0.5) * PosQuantum;
	ResumeDlg.m_TraverseSafeX = DispX;
	ResumeDlg.m_TraverseSafeY = DispY;

	if (Interpreter->m_StoppedInterpState.spindle_turning == CANON_STOPPED)
	{
		ResumeDlg.m_SafeStartSpindle = FALSE;
		ResumeDlg.m_SafeSpindleCWCCW = 0;
	}
	else if (Interpreter->m_StoppedInterpState.spindle_turning == CANON_CLOCKWISE)
	{
		ResumeDlg.m_SafeStartSpindle = TRUE;
		ResumeDlg.m_SafeSpindleCWCCW = 0;
	}
	else
	{
		ResumeDlg.m_SafeStartSpindle = TRUE;
		ResumeDlg.m_SafeSpindleCWCCW = 1;
	}

	ResumeDlg.m_DoSafeFeedZ = TRUE;
	ResumeDlg.m_FeedSafeZ = DispZ;
	ResumeDlg.m_ResumeFeedRate = ResumeDlg.m_ZFeedRate = Interpreter->m_StoppedInterpState.feed_rate;
	ResumeDlg.m_RestoreFeedRate = TRUE;

	int result = ResumeDlg.DoModal();

	if (result == IDCANCEL)
	{
		return 2;
	}

	if (result == IDOK)
	{
		m_SafeZ = Interpreter->UserUnitsToInches(ResumeDlg.m_SafeZ);
		m_SafeRelAbs = ResumeDlg.m_SafeRelAbs;

		Interpreter->m_Resume=true;
		Interpreter->m_ResumeSafeZ=ResumeDlg.m_SafeZ;
		Interpreter->m_ResumeSafeRelAbs=ResumeDlg.m_SafeRelAbs;
		Interpreter->m_ResumeMoveToSafeZ=ResumeDlg.m_MoveToSafeZ;
		Interpreter->m_ResumeTraverseXY=ResumeDlg.m_TraverseXY;
		// a field returned at its displayed (rounded) value was not
		// edited: command the EXACT stop position instead.  Any
		// intentional edit differs by at least one display quantum.
		Interpreter->m_ResumeTraverseSafeX =
			(fabs(ResumeDlg.m_TraverseSafeX - DispX) < 0.5*PosQuantum) ? ExactX : ResumeDlg.m_TraverseSafeX;
		Interpreter->m_ResumeTraverseSafeY =
			(fabs(ResumeDlg.m_TraverseSafeY - DispY) < 0.5*PosQuantum) ? ExactY : ResumeDlg.m_TraverseSafeY;
		Interpreter->m_ResumeSafeStartSpindle=ResumeDlg.m_SafeStartSpindle;
		Interpreter->m_ResumeSafeSpindleCWCCW=ResumeDlg.m_SafeSpindleCWCCW;
		Interpreter->m_ResumeDoSafeFeedZ=ResumeDlg.m_DoSafeFeedZ;
		Interpreter->m_ResumeFeedSafeZ =
			(fabs(ResumeDlg.m_FeedSafeZ - DispZ) < 0.5*PosQuantum) ? ExactZ : ResumeDlg.m_FeedSafeZ;
		Interpreter->m_ResumeResumeFeedRate=ResumeDlg.m_ResumeFeedRate;
		Interpreter->m_ResumeZFeedRate=ResumeDlg.m_ZFeedRate;
		Interpreter->m_ResumeRestoreFeedRate=ResumeDlg.m_RestoreFeedRate;
	}
	return 0;
}


void CKMotionCNCDlg::SetMotionParams()
{
	MOTION_PARAMS *p=Interpreter->GetMotionParams();

	p->BreakAngle = m_BreakAngle;
	p->CollinearTol = m_CollinearTol;
	p->CornerTol = m_CornerTol;
	p->FacetAngle = m_FacetAngle;
	p->TPLookahead = m_TPLookahead;
	p->RadiusA = m_RadiusA;
	p->RadiusB = m_RadiusB;
	p->RadiusC = m_RadiusC;
	p->MaxAccelX = m_MaxAccelX;
	p->MaxAccelY = m_MaxAccelY;
	p->MaxAccelZ = m_MaxAccelZ;
	p->MaxAccelU = m_MaxAccelU;
	p->MaxAccelV = m_MaxAccelV;
	p->MaxAccelA = m_MaxAccelA;
	p->MaxAccelB = m_MaxAccelB;
	p->MaxAccelC = m_MaxAccelC;
	p->MaxVelX = m_MaxVelX;
	p->MaxVelY = m_MaxVelY;
	p->MaxVelZ = m_MaxVelZ;
	p->MaxVelU = m_MaxVelU;
	p->MaxVelV = m_MaxVelV;
	p->MaxVelA = m_MaxVelA;
	p->MaxVelB = m_MaxVelB;
	p->MaxVelC = m_MaxVelC;
	p->CountsPerInchX = m_CountsPerInchX;
	p->CountsPerInchY = m_CountsPerInchY;
	p->CountsPerInchZ = m_CountsPerInchZ;
	p->CountsPerInchU = m_CountsPerInchU;
	p->CountsPerInchV = m_CountsPerInchV;
	p->CountsPerInchA = m_CountsPerInchA;
	p->CountsPerInchB = m_CountsPerInchB;
	p->CountsPerInchC = m_CountsPerInchC;
	p->ActuatorLimits = m_ActuatorLimits != 0;
	for (int i = 0; i < MAX_TP_ACTUATORS; i++)
	{
		p->ActScale[i]    = m_ActScale[i];
		p->MaxActVel[i]   = m_MaxActVel[i];
		p->MaxActAccel[i] = m_MaxActAccel[i];
		p->MaxActJerk[i]  = m_MaxActJerk[i];
		p->ActDegrees[i]  = m_ActDegrees[i] != 0;
	}
	// Actuator Space Limits mode: the visible Scale column is the single
	// source of truth for counts scaling -- it overrides the (hidden) CAD
	// Counts/inch fields so the transform, download, and limit conversion
	// all use the same values and no cross-mode matching is ever needed
	if (m_ActuatorLimits)
	{
		p->CountsPerInchX = m_ActScale[0];
		p->CountsPerInchY = m_ActScale[1];
		p->CountsPerInchZ = m_ActScale[2];
		p->CountsPerInchA = m_ActScale[3];
		p->CountsPerInchB = m_ActScale[4];
		p->CountsPerInchC = m_ActScale[5];
		p->CountsPerInchU = m_ActScale[6];
		p->CountsPerInchV = m_ActScale[7];
	}

	p->ThirdOrderTP = m_ThirdOrderTP != 0;
	p->LogSegments = m_TPLogSegs != 0;
	p->CubicKnots = m_TPCubicKnots != 0;
	if (p->ThirdOrderTP)
	{
		// the 3rd Order planner always streams rapids as feeds and facets
		// arcs to segments (Collinear Tolerance governs the faceting)
		p->DoRapidsAsFeeds = true;
		p->ArcsToSegs = true;
	}
	p->MaxJerkX = m_MaxJerkX;
	p->MaxJerkY = m_MaxJerkY;
	p->MaxJerkZ = m_MaxJerkZ;
	p->MaxJerkA = m_MaxJerkA;
	p->MaxJerkB = m_MaxJerkB;
	p->MaxJerkC = m_MaxJerkC;
	p->MaxJerkU = m_MaxJerkU;
	p->MaxJerkV = m_MaxJerkV;
	wcscpy(Interpreter->ToolFile,m_ToolFile);
	wcscpy(Interpreter->SetupFile,m_SetupFile);
	wcscpy(Interpreter->GeoFile,m_GeoFile);
	wcscpy(Interpreter->VarsFile,m_VarsFile);
	p->DegreesA = m_DegreesA!=0;
	p->DegreesB = m_DegreesB!=0;
	p->DegreesC = m_DegreesC!=0;
	p->ArcsToSegs = m_ArcsToSegs!=0;
	p->MaxRapidFRO=m_MaxRapidFRO;
	p->DoRapidsAsFeeds=m_DoRapidsAsFeeds!=0;

	Interpreter->p_setup->arc_radius_tol=m_ArcRadiusTol;
	Interpreter->p_setup->arc_radius_small_tol=m_ArcRSmallTol;
	Interpreter->CoordMotion->SetTPParams();
}


int DoCheckShiftAndKey(int Key,WPARAM wParam, LPARAM lParam, CMotionButton &Button, CMotionButton &Button2, CMotionButton &ButtonStep)
{
	if (wParam == VK_SHIFT)
	{
		if (lParam & 0x80000000)
		{
			// shift went up
			if (Button2.DrawPushed)
			{
				Button.HandleButtonDown();
				Button2.HandleButtonUp();
			}
		}
		else
		{
			// shift went down
			if (Button.DrawPushed)
			{
				Button2.HandleButtonDown();
				Button.HandleButtonUp();
			}
		}
	}

	if (GetKeyState(VK_CONTROL)&0x80000000)
	{
		// control key kills any motion
		if (Button.DrawPushed)
		{
			Button.HandleButtonUp();
		}
		if (Button2.DrawPushed)
		{
			Button2.HandleButtonUp();
		}
	}
	else
	{
		// if control key not pressed then if step button is down, release it
		if (ButtonStep.DrawPushed)
		{
			ButtonStep.HandleButtonUp();
		}
	}

	if (Key == wParam)
	{
		if (GetKeyState(VK_CONTROL)&0x80000000)
		{
			if ((lParam & 0x80000000)==0)
			{
				// arrow key was pressed while control was down
				ButtonStep.HandleButtonDown();
			}
			else
			{
				// arrow key was released while control was down
				ButtonStep.HandleButtonUp();
		}
		}
		else if (GetKeyState(VK_SHIFT)&0x80000000)
		{
			if (lParam & 0x80000000)
			{
				if (Button2.DrawPushed)
				{
					Button2.HandleButtonUp();
				}
			}
			else
			{
				if (!Button2.DrawPushed)
				{
					Button2.HandleButtonDown();
				}
			}
		}
		else
		{
			if (lParam & 0x80000000)
			{
				if (Button.DrawPushed)
				{
					Button.HandleButtonUp();
				}
			}
			else
			{
				if (!Button.DrawPushed)
				{
					Button.HandleButtonDown();
				}
			}
		}
		return 1;
	}
	return 0;
}


int FormKeyCode(int k)
{
	if (GetKeyState(VK_SHIFT) & 0x80000000)
		k += 0x10000;
	if (GetKeyState(VK_CONTROL) & 0x80000000)
		k += 0x20000;
	if (GetKeyState(VK_MENU) & 0x80000000)
		k += 0x40000;

	return k;
}


HHOOK       g_hhk   =   NULL;


LRESULT CALLBACK KbdProc    (   int     nCode,  // hook code
                                WPARAM  wParam, // virtual-key code
                                LPARAM  lParam  // keystroke-message information
                            )
{
	UINT ID;
	CKMotionCNCDlg *p=&TheFrame->GCodeDlg;
	CImageButton *I;


	if (nCode == HC_NOREMOVE)
		return  (CallNextHookEx(g_hhk, nCode, wParam, lParam));


	bool Repeat = (lParam & 0x40000000) != 0;

	if (nCode==HC_ACTION && !Repeat && p->Screen.CheckForScreenEditorHotKey(FormKeyCode(wParam), &ID, &I) && 
		I->Style != MotionButton && I->Style != Momentary)
	{
		if ((lParam & (1 << 31)) == 0) // Pressed??  
		{
			I->DetermineOwnerDraw();  // make sure style has been determined

			if (I->Style == Radio)
				p->Screen.HandleRadioButton(p, ID);

			p->SendMessage(WM_COMMAND, MAKELONG(ID, BN_CLICKED), (LPARAM)(I->m_hWnd));
		}
		CallNextHookEx(g_hhk, nCode, wParam, lParam);
		return 1;
	}
	
	// Check Motion Buttons
	if (p->m_KeyJogMode.Toggled && (!p->ThreadIsExecuting || p->EnableJogKeys) && !p->ForceDisableJogKeys)	
	{
		CMotionButton *B, *B2, *Bstep;

		// check for Motion Button sets with same Axis, Directions, and HotKeys then handle as group
		for(int axis = 0; axis < MAX_ACTUATORS; axis++)
			for (int dir = -1; dir <=1; dir+=2)
				if (p->Screen.Find3MotionButtonsSameAxisDir(axis, dir, &B, &B2, &Bstep))
				{
					if (DoCheckShiftAndKey(B->m_HotKey, wParam, lParam, *B, *B2, *Bstep))
						return 1;
				}
	}

	if (p->Screen.CheckForScreenEditorHotKey(FormKeyCode(wParam), &ID, &I))
	{
		if (I->Style == Momentary)
		{
			if (lParam & 0x80000000)
			{
				if (I->DrawPushed)
					I->HandleButtonUp();
			}
			else
			{
				if (!I->DrawPushed)
					I->HandleButtonDown();
			}
			return 1; // found hot key so exit to avoid beep 
		}
	}

	return  (CallNextHookEx(g_hhk,nCode,wParam,lParam));
}

// Make Controls Wide Unicode from Ascii

void CKMotionCNCDlg::MakeUnicode(int ID, CImageButton &I)
{
	RECT rect;
	GetDlgItem(ID)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	CString txt;
	GetDlgItem(ID)->GetWindowText(txt);
	CString wtxt = txt;

	CFont *Font = GetDlgItem(ID)->GetFont();

	LOGFONT lf;
	Font->GetLogFont(&lf);

	DWORD Style = GetDlgItem(ID)->GetStyle();

	CString ClassName;
	GetClassName(GetDlgItem(ID)->m_hWnd, ClassName.GetBufferSetLength(200), 199);
	ClassName.ReleaseBuffer();
	CString wClassName = ClassName;

	GetDlgItem(ID)->DestroyWindow();


	CreateWindowW(wClassName,  // Predefined class; Unicode assumed 
		wtxt,		// Button text 
		Style,		// (must include BS_MULTILINE to display international chars properly for some reason)  
		rect.left,  // x position 
		rect.top,   // y position 
		rect.right - rect.left,        // Button width
		rect.bottom - rect.top,        // Button height
		m_hWnd,     // Parent window
		(HMENU)((HANDLE64)ID),       // No menu an integer ID for dialog controls
		(HINSTANCE)GetWindowLongPtr(m_hWnd, GWLP_HINSTANCE),
		NULL);

	GetDlgItem(ID)->SetFont(Font);

	//  associate the button object with the window
	I.m_hWnd = GetDlgItem(ID)->m_hWnd;

	::SetWindowTheme(I.m_hWnd, L"dummy", L"dummy");  // disable themes or you can't change text color
}








BOOL CKMotionCNCDlg::OnInitDialog()
{
	static double full = 100.0;  // 100%
	int result, i;

	// Determine the parent of the GCodeViewer Control - Main Screen or GCodeViewer Dialog
	if (m_DialogFaceInUse == CUSTOM_DLG_FACE)
	{
		ActualGViewParent = &GViewControlParent;
	}
	else
	{
		ActualGViewParent = &TheFrame->GViewDlg.GViewParent;
	}

	CDialogEx::OnInitDialog(); // bypass CDlgX Init Dialog for Modal Dialogs like this

	// Make Controls Wide Unicode
	MakeUnicode(IDC_ShowMach, m_ShowMachButton);
	MakeUnicode(IDC_Simulate, m_SimulateButton);
	MakeUnicode(IDC_BlockDelete, m_BlockDeleteButton);

	MakeUnicode(IDC_Step0, m_RadioStep0);
	MakeUnicode(IDC_Step1, m_RadioStep1);
	MakeUnicode(IDC_Step2, m_RadioStep2);
	MakeUnicode(IDC_Step3, m_RadioStep3);
	MakeUnicode(IDC_Step4, m_RadioStep4);
	MakeUnicode(IDC_Step5, m_RadioStep5);

	MakeUnicode(IDC_Thread1, m_Thread1);
	MakeUnicode(IDC_Thread2, m_Thread2);
	MakeUnicode(IDC_Thread3, m_Thread3);
	MakeUnicode(IDC_Thread4, m_Thread4);
	MakeUnicode(IDC_Thread5, m_Thread5);
	MakeUnicode(IDC_Thread6, m_Thread6);
	MakeUnicode(IDC_Thread7, m_Thread7);

	MakeUnicode(IDC_Rapid, m_RapidRadio);
	MakeUnicode(IDC_Feed, m_Feed);

	MakeUnicode(IDC_mm, m_mm);
	MakeUnicode(IDC_inch, m_inch);

	MakeUnicode(IDC_Rel, m_Rel);
	MakeUnicode(IDC_Abs, m_Abs);

	// after MakeUnicode: controls are only reset while attached to a window, so
	// resetting before it left the radio buttons, check boxes etc. with the
	// colors/fonts of a previously loaded custom screen when switching faces
	Screen.ResetAllControls();

	CDC *screen = GetDC();
	dpiX = screen->GetDeviceCaps(LOGPIXELSX);
	dpiY = screen->GetDeviceCaps(LOGPIXELSY);

	ReleaseDC(screen);
	m_dpi_scale = dpiX / (double)dpi_standard;

	m_DlgBackgroundColor = ::GetSysColor(COLOR_3DFACE);
	m_DlgBackgroundBrush.DeleteObject();
	m_DlgBackgroundBrush.CreateSolidBrush(m_DlgBackgroundColor); // Initialize to default

	m_RealTimeSetup = Interpreter->p_setup;  // set to something valid

	Interpreter->CoordMotion->m_Simulate = (m_Simulate != 0);  // make sure the libraries match
	Interpreter->CoordMotion->m_DoTime = (m_DoTime != 0);  // make sure the libraries match

	Interpreter->p_setup->DiameterMode = m_DiameterMode;

	m_Editor.SetupForGCode(10, L"Courier New");

	for (i = 0; i < N_USER_GCODE_FILES; i++)
	{
		DisplayedThreadStat[i] = -1;     // set all undefined
		GCodeThreadActive[i] = 0;
	}

	DisplayedCurrentLine = -1;

	m_ConfigSpindleDirty = true;

	if (!GreenBrush)
	{
		GreenBrush = new CBrush;
		GreenBrush->CreateSolidBrush(0xff00);
	}

	if (m_Thread < 0 || m_Thread >= N_USER_GCODE_FILES) m_Thread = 0;
	LoadFile(m_Thread, false);

	LastTitleText = "";
	m_prevFeedRateLabel = "";
	m_prevFeedRateLabelF = "";
	m_prevSpindleRateLabel = "";
	m_prevSpindleRateLabelF = "";

	RefreshTitle();

	if (CurrentDirectory.IsEmpty())
	{
		CurrentDirectory = TheFrame->MainPathRoot + GCODE_SUB_DIR;
	}



	m_EmergencyStop.LoadBitmaps(IDB_EmergencyStop, 0, 0);
	m_EmergencyStop.Style = Button;
	m_GO.LoadBitmaps(IDB_GO, IDB_Halt, 0);
	m_GO.Style = DualButton;
	m_KeyJogMode.LoadBitmaps(IDB_ToggleKeyModeUp, IDB_ToggleKeyModeDown, 0);
	m_KeyJogMode.ToggleType = true;
	m_KeyJogMode.Style = ToggleButton;
	m_Reverse.LoadBitmaps(IDB_Rew, 0, 0);
	m_Forward.LoadBitmaps(IDB_For, 0, 0);
	m_FeedHold.LoadBitmaps(IDB_FeedHold, IDB_FeedHoldPause, 0);
	m_FeedHold.Style = DualButton;

	m_FR.LoadBitmaps(L"FR.png", L"FR32.png", L"");
	m_FR.Style = DualLabel;

	m_SR.LoadBitmaps(L"SR.png", L"SRCSS.png", L"");
	m_SR.Style = DualLabel;

	m_LeftStep.LoadBitmaps(IDB_LeftStep, 0, 0);
	m_Left.LoadBitmaps(IDB_Left, 0, 0);
	m_Left2.LoadBitmaps(IDB_Left2, 0, 0);
	m_RightStep.LoadBitmaps(IDB_LeftStep, 0, 0);
	m_Right.LoadBitmaps(IDB_Right, 0, 0);
	m_Right2.LoadBitmaps(IDB_Right2, 0, 0);

	m_DownStep.LoadBitmaps(IDB_UpStep, 0, 0);
	m_Down.LoadBitmaps(IDB_Down, 0, 0);
	m_Down2.LoadBitmaps(IDB_Down2, 0, 0);
	m_UpStep.LoadBitmaps(IDB_UpStep, 0, 0);
	m_Up.LoadBitmaps(IDB_Up, 0, 0);
	m_Up2.LoadBitmaps(IDB_Up2, 0, 0);

	m_ZplusStep.LoadBitmaps(IDB_UpStep, 0, 0);
	m_Zplus.LoadBitmaps(IDB_Up, 0, 0);
	m_Zplus2.LoadBitmaps(IDB_Up2, 0, 0);
	m_ZminusStep.LoadBitmaps(IDB_UpStep, 0, 0);
	m_Zminus.LoadBitmaps(IDB_Down, 0, 0);
	m_Zminus2.LoadBitmaps(IDB_Down2, 0, 0);

	m_AminusStep.LoadBitmaps(IDB_LeftStep, 0, 0);
	m_Aminus.LoadBitmaps(IDB_Left, 0, 0);
	m_Aminus2.LoadBitmaps(IDB_Left2, 0, 0);
	m_AplusStep.LoadBitmaps(IDB_LeftStep, 0, 0);
	m_Aplus.LoadBitmaps(IDB_Right, 0, 0);
	m_Aplus2.LoadBitmaps(IDB_Right2, 0, 0);

	m_BminusStep.LoadBitmaps(IDB_LeftStep, 0, 0);
	m_Bminus.LoadBitmaps(IDB_Left, 0, 0);
	m_Bminus2.LoadBitmaps(IDB_Left2, 0, 0);
	m_BplusStep.LoadBitmaps(IDB_LeftStep, 0, 0);
	m_Bplus.LoadBitmaps(IDB_Right, 0, 0);
	m_Bplus2.LoadBitmaps(IDB_Right2, 0, 0);

	m_CminusStep.LoadBitmaps(IDB_LeftStep, 0, 0);
	m_Cminus.LoadBitmaps(IDB_Left, 0, 0);
	m_Cminus2.LoadBitmaps(IDB_Left2, 0, 0);
	m_CplusStep.LoadBitmaps(IDB_LeftStep, 0, 0);
	m_Cplus.LoadBitmaps(IDB_Right, 0, 0);
	m_Cplus2.LoadBitmaps(IDB_Right2, 0, 0);

	if (m_DialogFaceInUse == CUSTOM_DLG_FACE)
	{
		m_UminusStep.LoadBitmaps(IDB_LeftStep, 0, 0);
		m_Uminus.LoadBitmaps(IDB_Left, 0, 0);
		m_Uminus2.LoadBitmaps(IDB_Left2, 0, 0);
		m_UplusStep.LoadBitmaps(IDB_LeftStep, 0, 0);
		m_Uplus.LoadBitmaps(IDB_Right, 0, 0);
		m_Uplus2.LoadBitmaps(IDB_Right2, 0, 0);

		m_VminusStep.LoadBitmaps(IDB_LeftStep, 0, 0);
		m_Vminus.LoadBitmaps(IDB_Left, 0, 0);
		m_Vminus2.LoadBitmaps(IDB_Left2, 0, 0);
		m_VplusStep.LoadBitmaps(IDB_LeftStep, 0, 0);
		m_Vplus.LoadBitmaps(IDB_Right, 0, 0);
		m_Vplus2.LoadBitmaps(IDB_Right2, 0, 0);
	}

	m_StopStep.LoadBitmaps(IDB_StopStep, 0, 0);

	if (m_Lathe)
	{
		m_RightStep.Init(this, 2, 1, &m_JogSlowPercent, true, VK_RIGHT);
		m_Right.Init(this, 2, 1, &m_JogSlowPercent, false, VK_RIGHT);
		m_Right2.Init(this, 2, 1, &full, false, VK_RIGHT);
		m_LeftStep.Init(this, 2, -1, &m_JogSlowPercent, true, VK_LEFT);
		m_Left.Init(this, 2, -1, &m_JogSlowPercent, false, VK_LEFT);
		m_Left2.Init(this, 2, -1, &full, false, VK_LEFT);

		if (m_XPosFront)
		{
			m_ZplusStep.Init(this, 0, -1, &m_JogSlowPercent, true, VK_UP);
			m_Zplus.Init(this, 0, -1, &m_JogSlowPercent, false, VK_UP);
			m_Zplus2.Init(this, 0, -1, &full, false, VK_UP);
			m_ZminusStep.Init(this, 0, 1, &m_JogSlowPercent, true, VK_DOWN);
			m_Zminus.Init(this, 0, 1, &m_JogSlowPercent, false, VK_DOWN);
			m_Zminus2.Init(this, 0, 1, &full, false, VK_DOWN);
		}
		else
		{
			m_ZplusStep.Init(this, 0, 1, &m_JogSlowPercent, true, VK_UP);
			m_Zplus.Init(this, 0, 1, &m_JogSlowPercent, false, VK_UP);
			m_Zplus2.Init(this, 0, 1, &full, false, VK_UP);
			m_ZminusStep.Init(this, 0, -1, &m_JogSlowPercent, true, VK_DOWN);
			m_Zminus.Init(this, 0, -1, &m_JogSlowPercent, false, VK_DOWN);
			m_Zminus2.Init(this, 0, -1, &full, false, VK_DOWN);
		}
		m_UpStep.Init(this, 1, 1, &m_JogSlowPercent, true, VK_PRIOR);
		m_Up.Init(this, 1, 1, &m_JogSlowPercent, false, VK_PRIOR);
		m_Up2.Init(this, 1, 1, &full, false, VK_PRIOR);
		m_DownStep.Init(this, 1, -1, &m_JogSlowPercent, true, VK_NEXT);
		m_Down.Init(this, 1, -1, &m_JogSlowPercent, false, VK_NEXT);
		m_Down2.Init(this, 1, -1, &full, false, VK_NEXT);
	}
	else
	{
		m_RightStep.Init(this, 0, 1, &m_JogSlowPercent, true, VK_RIGHT);
		m_Right.Init(this, 0, 1, &m_JogSlowPercent, false, VK_RIGHT);
		m_Right2.Init(this, 0, 1, &full, false, VK_RIGHT);
		m_LeftStep.Init(this, 0, -1, &m_JogSlowPercent, true, VK_LEFT);
		m_Left.Init(this, 0, -1, &m_JogSlowPercent, false, VK_LEFT);
		m_Left2.Init(this, 0, -1, &full, false, VK_LEFT);

		m_ZplusStep.Init(this, 2, 1, &m_JogSlowPercent, true, VK_PRIOR);
		m_Zplus.Init(this, 2, 1, &m_JogSlowPercent, false, VK_PRIOR);
		m_Zplus2.Init(this, 2, 1, &full, false, VK_PRIOR);
		m_ZminusStep.Init(this, 2, -1, &m_JogSlowPercent, true, VK_NEXT);
		m_Zminus.Init(this, 2, -1, &m_JogSlowPercent, false, VK_NEXT);
		m_Zminus2.Init(this, 2, -1, &full, false, VK_NEXT);
		m_UpStep.Init(this, 1, 1, &m_JogSlowPercent, true, VK_UP);
		m_Up.Init(this, 1, 1, &m_JogSlowPercent, false, VK_UP);
		m_Up2.Init(this, 1, 1, &full, false, VK_UP);
		m_DownStep.Init(this, 1, -1, &m_JogSlowPercent, true, VK_DOWN);
		m_Down.Init(this, 1, -1, &m_JogSlowPercent, false, VK_DOWN);
		m_Down2.Init(this, 1, -1, &full, false, VK_DOWN);
	}


	m_AplusStep.Init(this, 3, 1, &m_JogSlowPercent, true, VK_ADD);
	m_Aplus.Init(this, 3, 1, &m_JogSlowPercent, false, VK_ADD);
	m_Aplus2.Init(this, 3, 1, &full, false, VK_ADD);
	m_AminusStep.Init(this, 3, -1, &m_JogSlowPercent, true, VK_SUBTRACT);
	m_Aminus.Init(this, 3, -1, &m_JogSlowPercent, false, VK_SUBTRACT);
	m_Aminus2.Init(this, 3, -1, &full, false, VK_SUBTRACT);

	m_BplusStep.Init(this, 4, 1, &m_JogSlowPercent, true, -1);
	m_Bplus.Init(this, 4, 1, &m_JogSlowPercent, false, -1);
	m_Bplus2.Init(this, 4, 1, &full, false, -1);
	m_BminusStep.Init(this, 4, -1, &m_JogSlowPercent, true, -1);
	m_Bminus.Init(this, 4, -1, &m_JogSlowPercent, false, -1);
	m_Bminus2.Init(this, 4, -1, &full, false, -1);

	m_CplusStep.Init(this, 5, 1, &m_JogSlowPercent, true, -1);
	m_Cplus.Init(this, 5, 1, &m_JogSlowPercent, false, -1);
	m_Cplus2.Init(this, 5, 1, &full, false, -1);
	m_CminusStep.Init(this, 5, -1, &m_JogSlowPercent, true, -1);
	m_Cminus.Init(this, 5, -1, &m_JogSlowPercent, false, -1);
	m_Cminus2.Init(this, 5, -1, &full, false, -1);

	m_UplusStep.Init(this, 6, 1, &m_JogSlowPercent, true, -1);
	m_Uplus.Init(this, 6, 1, &m_JogSlowPercent, false, -1);
	m_Uplus2.Init(this, 6, 1, &full, false, -1);
	m_UminusStep.Init(this, 6, -1, &m_JogSlowPercent, true, -1);
	m_Uminus.Init(this, 6, -1, &m_JogSlowPercent, false, -1);
	m_Uminus2.Init(this, 6, -1, &full, false, -1);

	m_VplusStep.Init(this, 7, 1, &m_JogSlowPercent, true, -1);
	m_Vplus.Init(this, 7, 1, &m_JogSlowPercent, false, -1);
	m_Vplus2.Init(this, 7, 1, &full, false, -1);
	m_VminusStep.Init(this, 7, -1, &m_JogSlowPercent, true, -1);
	m_Vminus.Init(this, 7, -1, &m_JogSlowPercent, false, -1);
	m_Vminus2.Init(this, 7, -1, &full, false, -1);

	m_Forward.Init(this, 1, 0.2);
	m_Reverse.Init(this, -1, 0.2);

	m_Forward.ShowWindow(SW_HIDE);
	m_Reverse.ShowWindow(SW_HIDE);

	SetDefaultHotKeys();  // set these until they are possibly overridden by the Screen Editor

	if (m_DialogFaceInUse == CUSTOM_DLG_FACE)
	{
		m_StaticLabelX.Style = Label;
		m_StaticLabelX.SetText(L"X");
		m_StaticLabelY.Style = Label;
		m_StaticLabelY.SetText(L"Y");
		m_StaticLabelZ.Style = Label;
		m_StaticLabelZ.SetText(L"Z");
		m_StaticLabelA.Style = Label;
		m_StaticLabelA.SetText(L"A");
		m_StaticLabelB.Style = Label;
		m_StaticLabelB.SetText(L"B");
		m_StaticLabelC.Style = Label;
		m_StaticLabelC.SetText(L"C");
		m_StaticLabelU.Style = Label;
		m_StaticLabelU.SetText(L"U");
		m_StaticLabelV.Style = Label;
		m_StaticLabelV.SetText(L"V");

		m_StaticTool.Style = Label;
		m_StaticTool.SetText(L"tool");
		m_StaticThread.Style = Label;
		m_StaticThread.SetText(/*TRAN*/TheFrame->KMotionDLL->Translate(L"file"));
		m_SimulateStatic.Style = Label;
		m_SimulateStatic.SetText(L"Simulate");
		m_StaticUnits.Style = Label;
		m_StaticUnits.SetText(L"units");

		m_StaticCoord.Style = Label;
		m_StaticCoord.SetText(L"coord");
		m_StaticStepSize.Style = Label;
		m_StaticStepSize.SetText(L"step size");
		m_BlockDeleteStatic.Style = Label;
		m_BlockDeleteStatic.SetText(L"Block");
		m_BlockDeleteStatic2.Style = Label;
		m_BlockDeleteStatic2.SetText(L"Delete");
		m_DoTimeStatic.Style = Label;
		m_DoTimeStatic.SetText(L"Do");
		m_DoTimeStatic2.Style = Label;
		m_DoTimeStatic2.SetText(L"Times");
		m_Static_Fixture.Style = Label;
		m_Static_Fixture.SetText(L"fixture offset");

		m_ShowMachButton.Style=CheckBox;
		m_SimulateButton.Style=CheckBox;
		m_BlockDeleteButton.Style=CheckBox;

		m_GVXY.LoadBitmaps(L"GV_XY.png", L"", L"");
		m_GVYZ.LoadBitmaps(L"GV_YZ.png", L"", L"");
		m_GVXZ.LoadBitmaps(L"GV_XZ.png", L"", L"");
		m_GVRotXY.LoadBitmaps(L"GV_RotXY.png", L"", L"");
		m_GVRotXY.Style = ToggleButton;
		m_GVClearPaths.LoadBitmaps(L"GV_ClearPaths.png", L"", L"");
		m_GVShowAxis.LoadBitmaps(L"GV_ShowAxis.png", L"", L"");
		m_GVShowAxis.Style = ToggleButton;
		m_GVBox.LoadBitmaps(L"GV_Box.png", L"", L"");
		m_GVBox.Style = ToggleButton;
		m_GVShowTool.LoadBitmaps(L"GV_ShowTool.png", L"", L"");
		m_GVShowTool.Style = ToggleButton;
		m_GVOrtho.LoadBitmaps(L"GV_Ortho.png", L"", L"");
		m_GVOrtho.Style = ToggleButton;
		m_GVGViewerSetup.LoadBitmaps(L"GV_GViewerSetup.png", L"", L"");

		m_GCNew.LoadBitmaps(L"GC_New.png", L"", L"");
		m_GCOpenFile.LoadBitmaps(/*TRAN*/"GC_OpenFile.png", L"", L"");
		m_GCSaveFile.LoadBitmaps(/*TRAN*/"GC_SaveFile.png", L"", L"");
		m_GCSaveAs.LoadBitmaps(L"GC_SaveAs.png", L"", L"");
		m_GCRestart.LoadBitmaps(L"GC_Restart.png", L"", L"");
		m_GCSingleStep.LoadBitmaps(L"GC_SingleStep.png", L"", L"");
		m_GCToolSetup.LoadBitmaps(L"GC_ToolSetup.png", L"", L"");
		m_GView.LoadBitmaps(L"GC_GView.png", L"", L"");

		// these newer controls should be hidden if not specified in legacy files
		if (m_ScreenScriptFile != "")
		{
			m_GVXY.ShowWindow(SW_HIDE);
			m_GVYZ.ShowWindow(SW_HIDE);
			m_GVXZ.ShowWindow(SW_HIDE);
			m_GVRotXY.ShowWindow(SW_HIDE);
			m_GVClearPaths.ShowWindow(SW_HIDE);
			m_GVShowAxis.ShowWindow(SW_HIDE);
			m_GVBox.ShowWindow(SW_HIDE);
			m_GVShowTool.ShowWindow(SW_HIDE);
			m_GVOrtho.ShowWindow(SW_HIDE);
			m_GVGViewerSetup.ShowWindow(SW_HIDE);
			m_GCNew.ShowWindow(SW_HIDE);
			m_GCOpenFile.ShowWindow(SW_HIDE);
			m_GCSaveFile.ShowWindow(SW_HIDE);
			m_GCSaveAs.ShowWindow(SW_HIDE);
			m_GCRestart.ShowWindow(SW_HIDE);
			m_GCSingleStep.ShowWindow(SW_HIDE);
			m_GCToolSetup.ShowWindow(SW_HIDE);
			m_GView.ShowWindow(SW_HIDE);
			ActualGViewParent->m_view.ShowWindow(SW_HIDE);
		}
	}



	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_TOOLSETUP, L"Tool Setup Screens");
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	m_hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAINFRAME));
	SendMessage(WM_SETICON, ICON_SMALL, (LPARAM)m_hIcon);
	m_hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAINFRAME));
	SendMessage(WM_SETICON, ICON_BIG, (LPARAM)m_hIcon);


	if (m_GCodeTools)
		delete m_GCodeTools;

	m_GCodeTools = new CDlgToolBar;

	// Create toolbar at the top of the dialog window
	if (m_GCodeTools->Create(this))
	{
	    result = m_GCodeTools->LoadToolBar(IDR_GCodeBar);
	}      
	
	m_GCodeTools->SetBarStyle(m_GCodeTools->GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	if (m_DialogFaceInUse == CUSTOM_DLG_FACE)  // hide this for custom screens and use buttons instead
		m_GCodeTools->ShowWindow(SW_HIDE);


	// And position the control bars
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 1);


	m_FeedSlider.SetRange(OVERRIDE_MIN,OVERRIDE_MAX,100,-6 * m_dpi_scale,RGB(0,128,128));
	m_FeedSlider.SetTicFreq(5);
	m_FeedSlider.SetPos(1.0);

	m_SpindleSlider.SetRange(OVERRIDE_MIN,OVERRIDE_MAX,100,-34 * m_dpi_scale,RGB(192,0,0));
	m_SpindleSlider.SetTicFreq(5);
	m_SpindleSlider.SetPos(1.0);


	CRect rcClientStart;
	CRect rcClientNow;
	GetClientRect(rcClientStart);
	
	// To reposition and resize the control bar
	
	CRect rcWindow;
	CRect rcWindowScint;
	m_GCodeTools->GetWindowRect(rcWindow);
	m_Editor.GetWindowRect(rcWindowScint);

	int width,height;
	height = rcWindow.bottom-rcWindow.top;
	width = (rcWindowScint.right-rcWindowScint.left);
	rcWindow.left = rcWindowScint.left;

	if (m_DialogFaceInUse == 8)
	{
		width+=85;
		rcWindow.left -= 40;
	}
	else
	{
		rcWindow.left += 10;
	}

	rcWindow.bottom = rcWindowScint.top;
	rcWindow.top = rcWindow.bottom-height;
	rcWindow.right = rcWindow.left+width;

	ScreenToClient(rcWindow);

	m_GCodeTools->MoveWindow(rcWindow, FALSE);	
	
	m_Editor.SetupForGCode(10, L"Courier New");

	FillComboWithTools(&m_tool);
	FillComboWithCountFixture(1,9,&m_FixtureOffset);

	SetUserButtons();

	SetMotionParams();

	Interpreter->CoordMotion->SetSpindleRateOverride(m_SpindleRateValue);
	Interpreter->CoordMotion->SetHardwareFRORange(m_HardwareFRORange);

	Interpreter->ReadSetupFile();

	EnableToolTips();

	Interpreter->CoordMotion->Kinematics->ReadGeoTable(m_GeoFile);

	m_RealTimeSetup=Interpreter->GetRealTimeState();

	SetStepSizes();

	CSS_BitmapValid=false;
	CSS_BitmapDisplayed=false;
	G32_BitmapValid=false;
	G32_BitmapDisplayed=false;

	if (!g_hhk)  // only Hook Keyboard once
		g_hhk = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)KbdProc, NULL, GetCurrentThreadId());

	Interpreter->SetScreenScriptCallback(DoProcessScript);

	INIT_EASYSIZE;

	if (m_DialogFaceInUse == CUSTOM_DLG_FACE)
	{
		ActualGViewParent->OnInitDialog();
		if (ActualGViewParent->m_FirstScreenDisplay)
		{
			ActualGViewParent->m_FirstScreenDisplay = false;
			if (TheFrame->GCodeDlg.m_Lathe)
				OnXz();
			else
				OnXy();
		}
	}

	// Restore GViewer Window if it was open when shut down
	if (FirstInitDlg && m_DisplayGViewer) OnGView();

	// for tool length immediately mode make sure Interpreter matches initial tool
	if (m_ToolLengthImmediately && m_ToolTableDoM6 && m_tool.GetCount() > 0)
	{
		int tool;
		CString s;
		m_tool.GetLBText(m_RealTimeSetup->selected_tool_slot,s);

		if (s.GetAt(0) == 'I')
			s.Delete(0,3);
		else
			s.Delete(0,5);

		swscanf(s,L"%d",&tool);

		s.Format(L"H%dG43.4",tool);
		DoGCodeLine(s);
	}

	Screen.ReadResourceIDs();  // Initialize the Screen processor

	SetBackgroundImage((HBITMAP)NULL);
	Screen.BGFile_loaded="";
	if (m_DialogFaceInUse == CUSTOM_DLG_FACE)
	{
		int result = Screen.ProcessScript(m_ScreenScriptFile);
		
		DLG_CONTROL *DlgView = Screen.FindDlgControl(IDC_viewctl);   // try and find GViewer Control 
		
		if (!result && m_ScreenScriptFile != "" && DlgView != NULL && DlgView->Var == 0)  // if Var is 0 use floating screen
		{
			ActualGViewParent = &TheFrame->GViewDlg.GViewParent;
			ActualGViewParent->OnInitDialog();
		}
	}

	SetTimer(0,(int)(STATUS_TIME*1000),NULL);

	FirstInitDlg = false;

	CreateToolTipForRect(m_hWnd);  // create tool tip for title bar

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CKMotionCNCDlg::CreateToolTipForRect(HWND hwndParent)
{
	HINSTANCE g_hInst = (HINSTANCE)GetClassLongPtr(m_hWnd, GCLP_HMODULE);

	// Create a tooltip.
	hwndTT = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		hwndParent, NULL, g_hInst, NULL);

	::SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	// Set up "tool" information. 

	ti = { 0 };
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = hwndParent;
	ti.hinst = g_hInst;
	ti.lpszText = L"KMotionCNC";

	::GetClientRect(hwndParent, &ti.rect);

	ti.rect.top = -20;  // up into title bar
	ti.rect.bottom = 10;
	ti.rect.right -= 150;  // reserve 150 pixels for min,max, restore buttons
	// Associate the tooltip with the "tool" window.
	::SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
}
	
#define ABCPlotAngle 1.0

void StraightTraverseCallback(double x, double y, double z, double a, double b, double c, int sequence_number)
{
	CKMotionCNCDlg *p = &TheFrame->GCodeDlg;
	MOTION_PARAMS *MP = &p->Interpreter->CoordMotion->Kinematics->m_MotionParams;
	double Xorig = x, Yorig = y, Zorig = z;

	if (!p->m_PrevABCPlotValid)
	{
		p->m_PrevABCPlotValid = true;
		p->m_PrevPlotX = x;
		p->m_PrevPlotY = y;
		p->m_PrevPlotZ = z;
		p->m_PrevPlotA = a;
		p->m_PrevPlotB = b;
		p->m_PrevPlotC = c;
	}

	// Determine if any angle needs to be subdivided that is in degrees mode with radius
	if ((TheFrame->GViewDlg.m_IncludeA && MP->DegreesA && MP->RadiusA > 0.0 && fabs(a - p->m_PrevPlotA) > ABCPlotAngle) ||
		(TheFrame->GViewDlg.m_IncludeB && MP->DegreesB && MP->RadiusB > 0.0 && fabs(b - p->m_PrevPlotB) > ABCPlotAngle) ||
		(TheFrame->GViewDlg.m_IncludeC && MP->DegreesC && MP->RadiusC > 0.0 && fabs(c - p->m_PrevPlotC) > ABCPlotAngle))
	{
		StraightTraverseCallback((x + p->m_PrevPlotX) / 2.0, (y + p->m_PrevPlotY) / 2.0, (z + p->m_PrevPlotZ) / 2.0, (a + p->m_PrevPlotA) / 2.0, (b + p->m_PrevPlotB) / 2.0, (c + p->m_PrevPlotC) / 2.0, sequence_number);
		StraightTraverseCallback(x, y, z, a, b, c, sequence_number);
		return;
	}


	// rotation about X axis changes Y and Z Axis as a function of A
	if (TheFrame->GViewDlg.m_IncludeA && MP->DegreesA && MP->RadiusA > 0.0)
	{
		double Theta = a * PI / 180.0;
		double y0 = y + MP->RadiusA;
		y = y0 * cos(Theta) - z * sin(Theta);
		z = y0 * sin(Theta) + z * cos(Theta);
	}

	// rotation about Y axis changes Z and X Axis as a function of B
	if (TheFrame->GViewDlg.m_IncludeB && MP->DegreesB && MP->RadiusB > 0.0)
	{
		double Theta = b * PI / 180.0;
		double z0 = z + MP->RadiusB;
		z = z0 * cos(Theta) - x * sin(Theta);
		x = z0 * sin(Theta) + x * cos(Theta);
	}

	// rotation about Z axis changes X and Y Axis as a function of C
	if (TheFrame->GViewDlg.m_IncludeC && MP->DegreesC && MP->RadiusC > 0.0)
	{
		double Theta = c * PI / 180.0;
		double x0 = x + MP->RadiusC;
		x = x0 * cos(Theta) - y * sin(Theta);
		y = x0 * sin(Theta) + y * cos(Theta);
	}

	p->PlotPathVertex(x, y, z, true, 0.0, sequence_number, 0);	// rapid
	p->ActualGViewParent->m_Path->SetModified();

	p->m_PrevPlotX = Xorig;
	p->m_PrevPlotY = Yorig;
	p->m_PrevPlotZ = Zorig;
	p->m_PrevPlotA = a;
	p->m_PrevPlotB = b;
	p->m_PrevPlotC = c;
}


void StraightFeedCallback(double DesiredFeedRate_in_per_sec,
	double x, double y, double z, double a, double b, double c, int sequence_number, int ID)

{
	CKMotionCNCDlg *p = &TheFrame->GCodeDlg;
	MOTION_PARAMS *MP = &p->Interpreter->CoordMotion->Kinematics->m_MotionParams;
	double Xorig = x, Yorig = y, Zorig = z;

	if (!p->m_PrevABCPlotValid)
	{
		p->m_PrevABCPlotValid = true;
		p->m_PrevPlotX = x;
		p->m_PrevPlotY = y;
		p->m_PrevPlotZ = z;
		p->m_PrevPlotA = a;
		p->m_PrevPlotB = b;
		p->m_PrevPlotC = c;
	}

	// rotation about X axis changes Y and Z Axis as a function of A
	if (TheFrame->GViewDlg.m_IncludeA && MP->DegreesA && MP->RadiusA > 0.0)
	{
		// subdivide to make small angles
		if (fabs(a - p->m_PrevPlotA) > ABCPlotAngle)
		{
			StraightFeedCallback(DesiredFeedRate_in_per_sec, (x + p->m_PrevPlotX) / 2.0, (y + p->m_PrevPlotY) / 2.0, (z + p->m_PrevPlotZ) / 2.0, (a + p->m_PrevPlotA) / 2.0, (b + p->m_PrevPlotB) / 2.0, (c + p->m_PrevPlotC) / 2.0, sequence_number, ID);
			StraightFeedCallback(DesiredFeedRate_in_per_sec, x, y, z, a, b, c, sequence_number, ID);
			return;
		}
		else
		{
			double Theta = a*PI / 180.0;
			double y0 = y + MP->RadiusA;
			y = y0 * cos(Theta) - z * sin(Theta);
			z = y0 * sin(Theta) + z * cos(Theta);
		}
	}

	// rotation about Y axis changes Z and X Axis as a function of B
	if (TheFrame->GViewDlg.m_IncludeB && MP->DegreesB && MP->RadiusB > 0.0)
	{
		// subdivide to make small angles
		if (fabs(b - p->m_PrevPlotB) > ABCPlotAngle)
		{
			StraightFeedCallback(DesiredFeedRate_in_per_sec, (x + p->m_PrevPlotX) / 2.0, (y + p->m_PrevPlotY) / 2.0, (z + p->m_PrevPlotZ) / 2.0, (a + p->m_PrevPlotA) / 2.0, (b + p->m_PrevPlotB) / 2.0, (c + p->m_PrevPlotC) / 2.0, sequence_number, ID);
			StraightFeedCallback(DesiredFeedRate_in_per_sec, x, y, z, a, b, c, sequence_number, ID);
			return;
		}
		else
		{
			double Theta = b*PI / 180.0;
			double z0 = z + MP->RadiusB;
			z = z0 * cos(Theta) - x * sin(Theta);
			x = z0 * sin(Theta) + x * cos(Theta);
		}
	}

	// rotation about Z axis changes X and Y Axis as a function of C
	if (TheFrame->GViewDlg.m_IncludeC && MP->DegreesC && MP->RadiusC > 0.0)
	{
		// subdivide to make small angles
		if (fabs(c - p->m_PrevPlotC) > ABCPlotAngle)
		{
			StraightFeedCallback(DesiredFeedRate_in_per_sec, (x + p->m_PrevPlotX) / 2.0, (y + p->m_PrevPlotY) / 2.0, (z + p->m_PrevPlotZ) / 2.0, (a + p->m_PrevPlotA) / 2.0, (b + p->m_PrevPlotB) / 2.0, (c + p->m_PrevPlotC) / 2.0, sequence_number, ID);
			StraightFeedCallback(DesiredFeedRate_in_per_sec, x, y, z, a, b, c, sequence_number, ID);
			return;
		}
		else
		{
			double Theta = c*PI / 180.0;
			double x0 = x + MP->RadiusC;
			x = x0 * cos(Theta) - y * sin(Theta);
			y = x0 * sin(Theta) + y * cos(Theta);
		}
	}

	p->PlotPathVertex(x, y, z, false, DesiredFeedRate_in_per_sec, sequence_number, ID);	// feed
	p->ActualGViewParent->m_Path->SetModified();

	p->m_PrevPlotX = Xorig;
	p->m_PrevPlotY = Yorig;
	p->m_PrevPlotZ = Zorig;
	p->m_PrevPlotA = a;
	p->m_PrevPlotB = b;
	p->m_PrevPlotC = c;
}

#define N_SEG 100.0

void ArcFeedCallback(bool ZeroLenAsFullCircles, double DesiredFeedRate_in_per_sec, 
			    CANON_PLANE plane,
				double first_end, double second_end, 
		        double first_axis, double second_axis, int rotation,
				double axis_end_point, double a, double b, double c,
				double first_start, double second_start, double axis_start_point, int sequence_number, int ID)
{
	double invd, radius, theta0, theta1, d_theta, dxy, theta;
	double dx = first_start-first_axis;
	double dy = second_start-second_axis;
	double dz = axis_end_point-axis_start_point;
	double sum2,x,y,z;
	int n_segs,i;
	CKMotionCNCDlg *p = &TheFrame->GCodeDlg;
	MOTION_PARAMS *MP=&p->Interpreter->CoordMotion->Kinematics->m_MotionParams;


	radius = sqrt(dx*dx + dy*dy); 

	theta0 = atan2(second_start-second_axis,first_start-first_axis);
	theta1 = atan2(second_end-second_axis,first_end-first_axis);  
						
	d_theta =  theta1 - theta0;  

	if (fabs(d_theta) < THETA_SIGMA) d_theta=0;  // force super small arcs to zero

	if (ZeroLenAsFullCircles)
	{
		if (rotation)
		{
			if (d_theta <= 0.0f) d_theta+=TWO_PI; // CCW delta should be +  
		}
		else
		{
			if (d_theta >= 0.0f) d_theta-=TWO_PI;  // CW delta should be -
		}
	}
	else
	{
		if (rotation)
		{
			if (d_theta < 0.0f) d_theta+=TWO_PI; // CCW delta should be +  
		}
		else
		{
			if (d_theta > 0.0f) d_theta-=TWO_PI;  // CW delta should be -
		}
	}

	dxy = d_theta * radius;  // path length along circle                                   
	
	sum2 = dxy*dxy + dz*dz;
						  
	if (sum2>0.0f)                      
		invd = 1.0f/sqrt(sum2); // inverse total length
	else
		sum2=0.0f;

	// compute number of line segments to achieve desired tolerance
	//
	//
	// given tolerance T and Radius R the largest angle of a segment is:
	//
	// max_theta = acos(1 - T/radius)
	// but actually for now just do N_SEG sided circles

	n_segs = (int)fabs(N_SEG * d_theta/TWO_PI) + 1;

	for (i=1; i<=n_segs; i++)
	{
		double xp,yp,zp;

		theta = theta0 + (double) i * d_theta/n_segs;

		xp = radius * cos(theta) + first_axis;
		yp = radius * sin(theta) + second_axis;
		zp = (double) i * dz/n_segs + axis_start_point;
		
		if (plane == CANON_PLANE_XY)
		{
			x=xp;
			y=yp;
			z=zp;
		}
		else if (plane == CANON_PLANE_XZ)
		{
			x=yp;
			y=zp;
			z=xp;
		}
		else // YZ
		{
			x=zp;
			y=xp;
			z=yp;
		}
		

		// rotation about X axis changes Y and Z Axis as a function of A
		if (TheFrame->GViewDlg.m_IncludeA && MP->DegreesA && MP->RadiusA>0.0)
		{
			double Theta=a*PI/180.0;
			double y0 = y+MP->RadiusA;
			y = y0 * cos(Theta) - z * sin(Theta);
			z = y0 * sin(Theta) + z * cos(Theta);
		}

		// rotation about Y axis changes Z and X Axis as a function of B
		if (TheFrame->GViewDlg.m_IncludeB && MP->DegreesB && MP->RadiusB>0.0)
		{
			double Theta=b*PI/180.0;
			double z0 = z+MP->RadiusB;
			z = z0 * cos(Theta) - x * sin(Theta);
			x = z0 * sin(Theta) + x * cos(Theta);
		}

		// rotation about Z axis changes X and Y Axis as a function of C
		if (TheFrame->GViewDlg.m_IncludeC && MP->DegreesC && MP->RadiusC>0.0)
		{
			double Theta=c*PI/180.0;
			double x0 = x+MP->RadiusC;
			x = x0 * cos(Theta) - y * sin(Theta);
			y = x0 * sin(Theta) + y * cos(Theta);
		}

		p->PlotPathVertex(x, y, z, false, DesiredFeedRate_in_per_sec, sequence_number, ID);	// arc feed
	}
	p->ActualGViewParent->m_Path->SetModified();
}


//***************************************************************************
// G-code viewer: full-path preview on load + cut progress while running.
// Design notes: kmotioncnc-src/DESIGN-gviewer-progress.md
//***************************************************************************

static bool SameColor(CColor *a, CColor &b)
{
	return a->r() == b.r() && a->g() == b.g() && a->b() == b.b();
}

// Preview moves are split into pieces of about this many seconds (at 100%
// feed) so the cut colouring can advance smoothly within a long move rather
// than a whole G-code block at a time.
#define PREVIEW_PIECE_TIME 0.05
#define PREVIEW_MAX_PIECES 500

// Every plotted path vertex goes through here (called from the interpreter's
// motion callbacks).  During a real run that is colouring a kept preview the
// path already holds every vertex, so nothing is added.  rate_in_per_sec is
// the programmed feed rate (0 for rapids).
void CKMotionCNCDlg::PlotPathVertex(double x, double y, double z, bool rapid, double rate_in_per_sec, int sequence_number, int ID)
{
	if (m_ProgressActive) return;

	if (!m_PreviewRunning)	// stock plotting
	{
		CColor &c = rapid ? m_ColorTraverse : m_ColorFeed;
		ActualGViewParent->m_Path->AddVertexTool(new CVertex3dFast((float)x, (float)y, (float)z, c, sequence_number, ID));
		return;
	}

	CColor &c = rapid ? m_ColorPreviewTraverse : m_ColorPreviewFeed;
	int n = (int)m_PreviewTime.GetSize();
	double t0 = n > 0 ? m_PreviewTime[n - 1] : 0.0;

	if (!m_PreviewPrevValid)
	{
		AddPreviewVertex(x, y, z, c, t0, sequence_number, ID);
	}
	else
	{
		// estimated time for the move: programmed feed (or rapid) rate, limited
		// by the axis maximum velocities, ignoring acceleration
		double dx = x - m_PreviewPrevX, dy = y - m_PreviewPrevY, dz = z - m_PreviewPrevZ;
		double d = sqrt(dx*dx + dy*dy + dz*dz);
		double t = 0.0;
		if (d > 0.0)
		{
			CKinematics *K = Interpreter->CoordMotion->Kinematics;
			double rate = 0.0, maxrate = 0.0;
			K->MaxRateInDirection(dx, dy, dz, 0, 0, 0, 0, 0, &maxrate);
			if (rapid)
			{
				// rapids run at KFLOP's rapid settings (3rd-order rapids) or the Tool
				// Setup max velocities; take the faster so the estimate stays optimistic
				if (K->MaxRapidRateInDirection(dx, dy, dz, 0, 0, 0, 0, 0, &rate) || rate < maxrate) rate = maxrate;
			}
			else
			{
				rate = rate_in_per_sec;
				if (maxrate > 0.0 && (rate <= 0.0 || rate > maxrate)) rate = maxrate;
			}
			if (rate > 0.0) t = d / rate;
		}

		int pieces = (int)ceil(t / PREVIEW_PIECE_TIME);
		if (pieces < 1) pieces = 1;
		if (pieces > PREVIEW_MAX_PIECES) pieces = PREVIEW_MAX_PIECES;

		for (int k = 1; k <= pieces; k++)
		{
			double f = (double)k / pieces;
			if (k == pieces)
				AddPreviewVertex(x, y, z, c, t0 + t, sequence_number, ID);
			else
				AddPreviewVertex(m_PreviewPrevX + f*dx, m_PreviewPrevY + f*dy, m_PreviewPrevZ + f*dz, c, t0 + f*t, sequence_number, ID);
		}
	}

	m_PreviewPrevX = x;
	m_PreviewPrevY = y;
	m_PreviewPrevZ = z;
	m_PreviewPrevValid = true;
}

// Add one preview vertex, recording its file line and time.  AddVertexTool may
// also insert a tool-change "jump" vertex, so record every vertex actually added.
void CKMotionCNCDlg::AddPreviewVertex(double x, double y, double z, CColor &c, double t, int sequence_number, int ID)
{
	CPath3d *path = ActualGViewParent->m_Path;
	int n0 = path->NbVertex();
	int n = (int)m_PreviewTime.GetSize();
	double tprev = n > 0 ? m_PreviewTime[n - 1] : 0.0;

	path->AddVertexTool(new CVertex3dFast((float)x, (float)y, (float)z, c, sequence_number, ID));

	int line = Interpreter->p_setup->current_line;
	int n1 = path->NbVertex();
	for (int i = n0; i < n1; i++)
	{
		m_PreviewFileLine.Add(line);
		m_PreviewTime.Add(i == n1 - 1 ? t : tprev);
	}
}

// Recolour vertices [first,last]: cut=true turns preview colours into cut
// colours, cut=false turns them back.  Other colours (e.g. tool-change jumps)
// are left alone.
void CKMotionCNCDlg::ColorPreviewRange(int first, int last, bool cut)
{
	CPath3d *path = ActualGViewParent->m_Path;
	int n = path->NbVertex();
	if (first < 0) first = 0;
	if (last > n - 1) last = n - 1;
	if (first > last) return;

	ActualGViewParent->m_view.OpenGLMutex->Lock();
	for (int i = first; i <= last; i++)
	{
		CVertex3dFast *v = path->GetVertex(i);
		CColor *c = v->GetColor();
		if (cut)
		{
			if (SameColor(c, m_ColorPreviewFeed))          v->SetColor(m_ColorCutFeed);
			else if (SameColor(c, m_ColorPreviewTraverse)) v->SetColor(m_ColorCutTraverse);
		}
		else
		{
			if (SameColor(c, m_ColorCutFeed))              v->SetColor(m_ColorPreviewFeed);
			else if (SameColor(c, m_ColorCutTraverse))     v->SetColor(m_ColorPreviewTraverse);
		}
	}
	path->MarkRecolor(first, last);
	ActualGViewParent->m_view.OpenGLMutex->Unlock();
}

// A file was loaded into the editor: preview it once the interpreter is idle.
// the feature switch lives with the other G Viewer settings (GViewer.txt)
bool CKMotionCNCDlg::PreviewEnabled()
{
	return TheFrame->GViewDlg.m_PreviewOnLoad != 0;
}

void CKMotionCNCDlg::RequestPreview(int thread)
{
	if (!PreviewEnabled()) return;
	m_PreviewRequestThread = thread;
}

// Timer: start a requested preview when nothing is running.
void CKMotionCNCDlg::ServicePreview()
{
	// finish a completed preview once the interpreter thread has fully exited
	if (m_PreviewFinishPending && Interpreter->m_InterpretThreadID == (DWORD)-1)
	{
		m_PreviewFinishPending = false;
		FinishPreview(m_PreviewFinishOk);
	}

	if (m_PreviewRequestThread < 0 || m_PreviewRunning || ThreadIsExecuting) return;

	int thread = m_PreviewRequestThread;
	m_PreviewRequestThread = -1;

	if (thread != m_Thread) return;		// user switched to another file tab meanwhile
	if (FileNames[thread].IsEmpty() || FileNames[thread] == TheFrame->MainPathRoot + GCODE_SUB_DIR) return;

	StartPreview(thread);
}

// Save (save=true) or restore the stopped/resume state the interpreter keeps,
// which a completed simulation would otherwise clear.
void CKMotionCNCDlg::SaveResumeState(bool save)
{
	CCoordMotion *CM = Interpreter->CoordMotion;
	double *pos[24] = {
		&CM->m_Stoppedx, &CM->m_Stoppedy, &CM->m_Stoppedz, &CM->m_Stoppeda, &CM->m_Stoppedb, &CM->m_Stoppedc, &CM->m_Stoppedu, &CM->m_Stoppedv,
		&CM->m_StoppedMidx, &CM->m_StoppedMidy, &CM->m_StoppedMidz, &CM->m_StoppedMida, &CM->m_StoppedMidb, &CM->m_StoppedMidc, &CM->m_StoppedMidu, &CM->m_StoppedMidv,
		&CM->m_StoppedMachinex, &CM->m_StoppedMachiney, &CM->m_StoppedMachinez, &CM->m_StoppedMachinea, &CM->m_StoppedMachineb, &CM->m_StoppedMachinec, &CM->m_StoppedMachineu, &CM->m_StoppedMachinev };
	int *st[5] = { &CM->m_Stopping, &CM->m_PreviouslyStopped, &CM->m_PreviouslyStoppedType, &CM->m_PreviouslyStoppedID, &CM->m_PreviouslyStoppedSeqNo };

	if (save)
	{
		for (int i = 0; i < 24; i++) m_PreviewSavedStoppedPos[i] = *pos[i];
		m_PreviewSavedStopping = *st[0];
		for (int i = 0; i < 4; i++) m_PreviewSavedStopped[i] = *st[i + 1];
		m_PreviewSavedThreadStopped = m_ThreadThatWasOriginallyStopped;
	}
	else
	{
		for (int i = 0; i < 24; i++) *pos[i] = m_PreviewSavedStoppedPos[i];
		*st[0] = m_PreviewSavedStopping;
		for (int i = 0; i < 4; i++) *st[i + 1] = m_PreviewSavedStopped[i];
		m_ThreadThatWasOriginallyStopped = m_PreviewSavedThreadStopped;
	}
}

// Quietly simulate the whole file to plot the preview.  Everything the run
// could disturb is snapshotted here and restored in FinishPreview.
void CKMotionCNCDlg::StartPreview(int thread)
{
	CFileStatus fs;
	if (!CFile::GetStatus(FileNames[thread], fs)) return;
	m_PreviewFile = FileNames[thread];
	m_PreviewFileSize = fs.m_size;
	m_PreviewFileTime = fs.m_mtime;

	// interpreter state: a simulated G92/G10 changes offsets in p_setup->parameters
	// (which the periodic emc.var save would persist), plus modal state, tool, ...
	if (!m_PreviewSetupSave) m_PreviewSetupSave = new setup;
	*m_PreviewSetupSave = *Interpreter->p_setup;

	m_PreviewSavedCurrentLine = CurrentLine[thread];
	m_PreviewSavedHadError = ThreadHadError[thread];
	m_PreviewSavedThreadLaunched = m_ThreadThatWasLaunched;
	SaveResumeState(true);

	m_ProgressActive = false;
	m_PreviewValid = false;
	m_ChainValid = false;
	m_PreviewFileLine.RemoveAll();
	m_PreviewTime.RemoveAll();
	m_PreviewPrevValid = false;
	m_PreviewThread = thread;
	m_PreviewRunning = true;
	m_ThreadThatWasLaunched = thread;	// so LaunchExecution clears the path first

	m_PreviewLaunching = true;
	int err = LaunchExecution(FileNames[thread], 0, -1);
	m_PreviewLaunching = false;

	if (err) FinishPreview(false);
}

// Preview simulation done (or failed to start): restore everything.
void CKMotionCNCDlg::FinishPreview(bool ok)
{
	int thread = m_PreviewThread;

	if (m_PreviewSetupSave) *Interpreter->p_setup = *m_PreviewSetupSave;
	Interpreter->CoordMotion->m_Simulate = (m_Simulate != 0);

	if (thread >= 0 && thread < N_USER_GCODE_FILES)
	{
		CurrentLine[thread] = m_PreviewSavedCurrentLine;
		ThreadHadError[thread] = m_PreviewSavedHadError;
	}
	m_ThreadThatWasLaunched = m_PreviewSavedThreadLaunched;
	SaveResumeState(false);

	CPath3d *path = ActualGViewParent->m_Path;
	m_PreviewValid = ok && path->NbVertex() > 0 && m_PreviewFileLine.GetSize() == path->NbVertex()
					 && m_PreviewTime.GetSize() == path->NbVertex();
	if (m_PreviewValid) ActualGViewParent->FitPath();	// zoom to the program

	// how runs from a mid-file line can be mapped onto the preview (ArmProgress)
	m_PreviewSeqMonotonic = m_PreviewLineMonotonic = m_PreviewValid;
	for (int i = 1; m_PreviewValid && i < path->NbVertex(); i++)
	{
		if (path->GetVertex(i)->Get_sequence_number() < path->GetVertex(i - 1)->Get_sequence_number()) m_PreviewSeqMonotonic = false;
		if (m_PreviewFileLine[i] < m_PreviewFileLine[i - 1]) m_PreviewLineMonotonic = false;
	}
	m_PreviewRunning = false;
}

// Is the kept preview still an exact picture of this file on disk?
bool CKMotionCNCDlg::PreviewMatchesFile(const CString &file)
{
	if (!m_PreviewValid || file.CompareNoCase(m_PreviewFile) != 0) return false;
	CFileStatus fs;
	if (!CFile::GetStatus(file, fs)) return false;
	return fs.m_size == m_PreviewFileSize && fs.m_mtime == m_PreviewFileTime;
}

// Real run starting at file line 'begin': if the preview matches the file,
// keep it (reset to uncut, or cut up to the start line for a mid-file start)
// and arm progress colouring.  Otherwise leave m_ProgressActive false (stock).
void CKMotionCNCDlg::ArmProgress(int begin, const CString &file)
{
	m_ProgressActive = false;
	if (!PreviewEnabled() || !PreviewMatchesFile(file)) return;

	CPath3d *path = ActualGViewParent->m_Path;
	int n = path->NbVertex();
	if (n == 0 || m_PreviewFileLine.GetSize() != n) return;

	// The interpreter numbers blocks as it reads them from the file's start; a
	// mid-file start skips lines without numbering them.  m_ProgressSeqOffset
	// converts this run's numbering to the preview's.
	int k = 0;
	if (begin <= 0)
	{
		m_ProgressSeqOffset = 0;
	}
	else if (m_ChainValid && begin == m_ChainLine && m_PreviewSeqMonotonic)
	{
		// continues where the last run over the preview stopped
		m_ProgressSeqOffset = m_ChainSeq - m_PreviewSeq0;
		while (k < n && path->GetVertex(k)->Get_sequence_number() <= m_ChainSeq) k++;
	}
	else if (m_PreviewLineMonotonic && m_PreviewSeqMonotonic)
	{
		// straight-through program: the start line is drawn in one place only
		while (k < n && m_PreviewFileLine[k] < begin) k++;
		if (k < n)
			m_ProgressSeqOffset = path->GetVertex(k)->Get_sequence_number()
								  - (m_PreviewSeq0 + 1 + m_PreviewFileLine[k] - begin);
	}
	else
		return;		// a line inside a loop or subroutine: which pass is unknown, stock behaviour

	m_ChainValid = false;	// re-established when this run stops part way
	if (k > 0) ColorPreviewRange(0, k - 1, true);
	ColorPreviewRange(k, n - 1, false);
	m_ProgressIndex = k;
	m_ProgressToolMoved = false;
	m_ProgressToolStartValid = false;
	m_ProgressAdvanceWall = ElapsedTimer.Elapsed_Seconds();
	m_ProgressActive = true;
}

// The run over the preview completed: the first vertex after the blocks it
// executed (-1 = unknown).  M2/M30 (line reset to 0) and end of file ran to the
// end; otherwise it stopped after block m_CompleteSeq (its own numbering).
int CKMotionCNCDlg::PreviewIndexAfterRun()
{
	CPath3d *path = ActualGViewParent->m_Path;
	int n = path->NbVertex();

	if (m_exitcode == RS274NGC_ENDFILE || m_exitcode == RS274NGC_EXIT || (m_exitcode == 0 && m_CompleteLine <= 0))
		return n;
	if (m_exitcode != 0 || !m_PreviewSeqMonotonic) return -1;

	int last = m_CompleteSeq + m_ProgressSeqOffset;
	int i = m_ProgressIndex;
	while (i < n && path->GetVertex(i)->Get_sequence_number() <= last) i++;
	return i;
}

// Timer (after m_RealTimeSetup is refreshed): colour everything before the
// block the machine is executing as cut.
void CKMotionCNCDlg::UpdateProgress()
{
	if (!m_ProgressActive || m_PlaybackArmed || !ThreadIsExecuting) return;

	CCoordMotion *CM = Interpreter->CoordMotion;
	CPath3d *path = ActualGViewParent->m_Path;
	int n = path->NbVertex();
	int i = m_ProgressIndex;

	// 1) KMotionCNC's record of which block is executing: everything before it is cut
	if (CM->m_realtime_Sequence_number_valid && m_RealTimeSetup)
	{
		int target = m_RealTimeSetup->sequence_number + m_ProgressSeqOffset;
		while (i < n && path->GetVertex(i)->Get_sequence_number() < target) i++;
	}

	// 2) where the tool actually is: within a long move, and near the end of a
	// program where the block record stops updating until motion finishes
	int j = FindToolOnPath(i);
	if (j > i) i = j;

	if (i > m_ProgressIndex)
	{
		ColorPreviewRange(m_ProgressIndex, i - 1, true);
		m_ProgressIndex = i;
		m_ProgressAdvanceWall = ElapsedTimer.Elapsed_Seconds();
	}
}

// Distance from point p to segment ab, squared.
static double DistToSegment2(double px, double py, double pz, CVertex3dFast *a, CVertex3dFast *b)
{
	double ax = a->x(), ay = a->y(), az = a->z();
	double dx = b->x() - ax, dy = b->y() - ay, dz = b->z() - az;
	double L2 = dx*dx + dy*dy + dz*dz;
	double t = L2 > 0.0 ? ((px - ax)*dx + (py - ay)*dy + (pz - az)*dz) / L2 : 0.0;
	if (t < 0.0) t = 0.0; else if (t > 1.0) t = 1.0;
	double ex = ax + t*dx - px, ey = ay + t*dy - py, ez = az + t*dz - pz;
	return ex*ex + ey*ey + ez*ez;
}

// Find the path segment the tool is on, searching forward from vertex 'first'.
// Returns j such that the tool is on segment (j-1 -> j), i.e. vertices before j
// are cut; or -1.  The search only reaches as far along the path as the tool
// could physically have gone since progress last advanced (using the preview's
// time estimate, which is optimistic), so a later pass that happens to cross
// the tool's position can't be mistaken for the current one.
#define TOOL_ON_PATH_TOL 0.01		// inches
int CKMotionCNCDlg::FindToolOnPath(int first)
{
	if (!m_ToolViewValid) return -1;

	CPath3d *path = ActualGViewParent->m_Path;
	int n = path->NbVertex();
	if (n < 2 || m_PreviewTime.GetSize() != n || first >= n) return -1;

	double tx = m_ToolViewX, ty = m_ToolViewY, tz = m_ToolViewZ;

	// don't track until the tool has moved from where the run found it
	if (!m_ProgressToolMoved)
	{
		if (!m_ProgressToolStartValid)
		{
			m_ProgressToolStartX = tx; m_ProgressToolStartY = ty; m_ProgressToolStartZ = tz;
			m_ProgressToolStartValid = true;
			return -1;
		}
		double dx = tx - m_ProgressToolStartX, dy = ty - m_ProgressToolStartY, dz = tz - m_ProgressToolStartZ;
		if (dx*dx + dy*dy + dz*dz < TOOL_ON_PATH_TOL*TOOL_ON_PATH_TOL) return -1;
		m_ProgressToolMoved = true;
	}

	double ovr = 1.0;
	if (m_FeedRateValue > ovr) ovr = m_FeedRateValue;
	if (m_FeedRateRapidValue > ovr) ovr = m_FeedRateRapidValue;
	double reach = m_PreviewTime[m_ProgressIndex < n ? m_ProgressIndex : n - 1]
				 + (ElapsedTimer.Elapsed_Seconds() - m_ProgressAdvanceWall) * ovr * 1.5 + 0.5;

	double tol2 = TOOL_ON_PATH_TOL * TOOL_ON_PATH_TOL;
	for (int j = first > 1 ? first : 1; j < n && m_PreviewTime[j - 1] <= reach; j++)
	{
		CVertex3dFast *b = path->GetVertex(j);
		if (DistToSegment2(tx, ty, tz, path->GetVertex(j - 1), b) <= tol2)
		{
			// at the segment's end point: that segment is done too
			double ex = b->x() - tx, ey = b->y() - ty, ez = b->z() - tz;
			if (ex*ex + ey*ey + ez*ez <= tol2) return j + 1;
			return j;
		}
	}

	return -1;
}

void CKMotionCNCDlg::StopPlayback()
{
	m_PlaybackArmed = false;
	m_PlaybackRunning = false;
	m_PlaybackPaused = false;
}

// Timer: a Simulate run over the preview is played back at the programmed
// speed (times the feed-rate override), colouring the path as if it were cut.
// Feed Hold pauses it; Halt, E-stop, a new run or reloading stop it.
void CKMotionCNCDlg::UpdatePlayback()
{
	if (!m_PlaybackRunning) return;

	double now = ElapsedTimer.Elapsed_Seconds();
	double dt = now - m_PlaybackWall;
	m_PlaybackWall = now;
	if (m_PlaybackPaused) return;

	CPath3d *path = ActualGViewParent->m_Path;
	int n = path->NbVertex();
	if (!m_PreviewValid || m_PreviewTime.GetSize() != n)
	{
		StopPlayback();
		return;
	}

	if (dt > 0.0 && m_FeedRateValue > 0.0) m_PlaybackTime += dt * m_FeedRateValue;

	int end = m_PlaybackEnd < n ? m_PlaybackEnd : n;
	int i = m_ProgressIndex;
	while (i < end && m_PreviewTime[i] <= m_PlaybackTime) i++;

	if (i > m_ProgressIndex)
	{
		ColorPreviewRange(m_ProgressIndex, i - 1, true);
		m_ProgressIndex = i;
	}
	if (i >= end) StopPlayback();
}


int DoProcessScript(const wchar_t *FileName)
{
	return TheFrame->GCodeDlg.Screen.ProcessScript(FileName);
}
	
void CKMotionCNCDlg::OnHalt() 
{
	StopPlayback();
	m_PerformPostHaltCommand=true;
	Interpreter->Halt();
}



HBRUSH CKMotionCNCDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDlgX::OnCtlColor(pDC, pWnd, nCtlColor);

	// Determine if the background is an image

	bool DoBackgroundImage = (Screen.BGFile_loaded != "");
	
	int ID = pWnd->GetDlgCtrlID();

	// ComboBox uses these for dropdown
	if (ID == 1000 || ID == 1001) return hbr;

	// Edit controls may have Screen Script colors (read only ones use CTLCOLOR_STATIC)
	if (nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_STATIC)
	{
		CEditScreen *E = Screen.FindEditScreen(ID);
		if (E)
		{
			HBRUSH EditBrush = E->CtlColor(pDC);
			if (EditBrush) return EditBrush;
		}
	}

	CImageButton *I;
	I = Screen.FindImageButton(ID);

	for (int i=0; i<N_USER_GCODE_FILES; i++)
	{
		if (pWnd->m_hWnd==GetDlgItem(NumberToThreadID(i))->m_hWnd)
		{
			DisplayedThreadStat[i] = GCodeThreadActive[i];
			
			if (DisplayedThreadStat[i])
			{
				pDC->SetBkColor(0xff00);
				return *GreenBrush;
			}
			else
			{
				if (I && (I->GetStyle() & BS_TYPEMASK) != BS_OWNERDRAW && pDC && pDC->m_hDC)
					pDC->SetTextColor(I->m_clrNormalText);
				
				pDC->SetBkMode(TRANSPARENT);
				if (DoBackgroundImage)
					return CreateDIBrush(pWnd);
				else
					return m_DlgBackgroundBrush;
			}
		}
	}

	// Return the brush.

	if (I)
	{
		if (I->Style == Label || I->Style == DROLabel || I->Style == DualLabel || I->Style == CheckBox || I->Style == Radio)
		{
			if ((I->GetStyle() & BS_TYPEMASK) != BS_OWNERDRAW && pDC && pDC->m_hDC)
				pDC->SetTextColor(I->m_clrNormalText);
			

			if (pDC && pDC->m_hDC) 
				pDC->SetBkColor(m_DlgBackgroundColor);

			pDC->SetBkMode(TRANSPARENT);
			if (DoBackgroundImage)
				return CreateDIBrush(pWnd);
			else
				return m_DlgBackgroundBrush;
		}
	}

	if (nCtlColor == CTLCOLOR_EDIT)
		return hbr;

	//Kludge to get sliders to update when background changes
	if (ID == IDC_FeedSlider)
	{
		m_FeedSlider.m_bkcolor = m_DlgBackgroundColor;
		m_FeedSlider.m_bkbitmap = Screen.BGFile_loaded;
	}
	if (ID == IDC_SpindleSlider)
	{
		m_SpindleSlider.m_bkcolor = m_DlgBackgroundColor;
		m_SpindleSlider.m_bkbitmap = Screen.BGFile_loaded;
	}
	
	if (nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_BTN || nCtlColor == CTLCOLOR_LISTBOX ||
		(DoBackgroundImage && nCtlColor == CTLCOLOR_STATIC))
	{
		pDC->SetBkMode(TRANSPARENT);

		if (DoBackgroundImage)
			return CreateDIBrush(pWnd);
		else
			return m_DlgBackgroundBrush;
	}
	else
	{
		if (pDC && pDC->m_hDC) 
			pDC->SetBkColor(m_DlgBackgroundColor);

		return m_DlgBackgroundBrush;
	}
}


HBRUSH CKMotionCNCDlg::CreateDIBrush(CWnd* pWnd)
{
	int x, y, cx, cy;
	RECT Rect, RectDlg, Rect3;

	if (hBrush)
	{
		DeleteObject(hBrush);
		hBrush = NULL;
	}

	if 	(hDIB)
	{
		GlobalFree(hDIB);
		DeleteObject(hDIB);
		hDIB = NULL;
	}

	pWnd->GetWindowRect(&Rect);
	GetWindowRect(&RectDlg);
	GetClientRect(&Rect3);
	int border = (RectDlg.right - RectDlg.left - Rect3.right) / 2;
	int topmargin = RectDlg.bottom - RectDlg.top - Rect3.bottom - border;

	x = Rect.left - RectDlg.left - border, 
	y = Rect.top - RectDlg.top - topmargin;
	cx = Rect.right - Rect.left;
	cy = Rect.bottom - Rect.top;

	int m_iBackBmpWidth = m_sizeBkgrBitmap.cx;
	int m_iBackBmpHeight = m_sizeBkgrBitmap.cy;

	if (m_iBackBmpWidth == 0 || m_iBackBmpHeight == 0) return NULL;

	BITMAP bmpval;
	GetObject(m_hBkgrBitmap, sizeof(BITMAP), &bmpval);

	if ((x < 0) || (y < 0) ||
		(0 == cx) || (0 == cy))
		return NULL;

	hDIB = GlobalAlloc(GHND, sizeof(BITMAPINFOHEADER) + cx * cy * 4);

	if (NULL == hDIB)
		return NULL;

	LPVOID lpvBits = GlobalLock(hDIB);

	if (NULL == lpvBits)
	{
		GlobalFree(hDIB);
		return NULL;
	}

	if (CurrentBackgroundFile != Screen.BGFile_loaded)  // did the background image change?
	{
		delete m_pvBackBits;
		m_pvBackBits = new BYTE[bmpval.bmWidthBytes * bmpval.bmHeight];
		if (!m_pvBackBits)
			return NULL;

		GetBitmapBits(m_hBkgrBitmap, bmpval.bmWidthBytes * bmpval.bmHeight, m_pvBackBits);
	}

	BITMAPINFOHEADER *bih = (BITMAPINFOHEADER*)lpvBits;

	bih->biBitCount = 32;
	bih->biCompression = BI_RGB;
	bih->biWidth = cx;
	bih->biHeight = -cy;  // make origin top left
	bih->biPlanes = 1;
	bih->biSize = sizeof(BITMAPINFOHEADER);
	bih->biSizeImage = cx*cy * 4;

	PDWORD pdwData = (PDWORD)(bih + 1);

	for (int yi = 0; yi<cy; yi++)
	{
		
		// copy cx pixels with the starting position wrapping around the source
		// image in a tiled manner.  Limit the length copied to sections
		// that do not exceed past the right edge of the image

		int xstart = x;	// Starting point in array
		int ystart = yi + y;
		int pixels_copied = 0;

		while (pixels_copied < cx)
		{
			xstart = xstart % m_iBackBmpWidth; // tile/wrap stating point
			ystart = ystart % m_iBackBmpHeight;

			int amount_to_copy = cx - pixels_copied;

			// copy only to end of image
			if (xstart + amount_to_copy > m_iBackBmpWidth) amount_to_copy = m_iBackBmpWidth - xstart;

			PDWORD pdwDst = pdwData + yi *cx + pixels_copied;
			PDWORD pdwSrc = ((PDWORD)m_pvBackBits) + ystart * bmpval.bmWidthBytes / 4 + xstart;
			memcpy(pdwDst, pdwSrc, amount_to_copy * 4);

			pixels_copied += amount_to_copy;
			xstart += amount_to_copy;
		}
	}

	GlobalUnlock(hDIB);

	LOGBRUSH lb;

	lb.lbStyle = BS_DIBPATTERN;
	lb.lbColor = DIB_RGB_COLORS;
	lb.lbHatch = (HANDLE64)hDIB;

	hBrush = CreateBrushIndirect(&lb);

	if (NULL == hBrush)
		GlobalFree(hDIB);

	return hBrush;
}


void CKMotionCNCDlg::OnTimer(UINT_PTR nIDEvent) 
{
	static int Entry=0;
	static int prev_result=KMOTION_LOCKED;
	static int skip=0;
	int result=KMOTION_IN_USE;

	InitDialogComplete = TRUE;  // Time only runs after window has been created and displayed

	if (Entry>0) return;  // do not allow re-entries
	Entry++;

	// Listing USB devices can take a long time so if we weren't previously
	// able to communicate with the board then on check for a connection
	// once in a while
	if (ReadStatus && (prev_result==KMOTION_LOCKED || ++skip>=20))
	{
		skip=0;
		result=TheFrame->KMotionDLL->WaitToken(false,100.0,"KMCNCReadStatus");

		if (result == KMOTION_LOCKED)
		{
			m_ConnectedForStatus=true;

			// Note only service the console 
			// after we have the token so we
			// are sure of no getting blocked

			TheFrame->KMotionDLL->ServiceConsole();
		
			// upload bulk status

			if (GetStatus())
			{
				Interpreter->Abort();

				// error reading status
				TheFrame->KMotionDLL->Failed();
				m_ConnectedForStatus=false;
			}
			
			TheFrame->KMotionDLL->ReleaseToken();

			UpdateScreen(m_ConnectedForStatus);

			if (m_ConnectedForStatus)
				ServiceKFLOPCommands();
		}
		else
		{
			m_ConnectedForStatus=false;
			UpdateScreen(false);
		}

		CString NewTitle;

		CString FileAbreviated = AbreviateFile(FileNames[m_Thread], 100);

		if (m_Thread != -1)
		{
			if (result == KMOTION_NOT_CONNECTED || (result == KMOTION_IN_USE && prev_result == KMOTION_NOT_CONNECTED))
				NewTitle = 	"KMotionCNC - Disconnected - " + FileAbreviated;
			else
				NewTitle = 	"KMotionCNC - Connected - " + FileAbreviated;
		}

		if (result != KMOTION_IN_USE)
			prev_result=result;  // remember previous state that was successfully determined

		if (ThreadIsExecuting && JobStartTimeValid)
		{
			if (m_Simulate && m_DoTime && JobDoTimeValid)
				LastTitleElapsed.Format(L"Job Time: %ls", convertSeconds(JobDoTimeSecs));
			else
				LastTitleElapsed.Format(L"Run Time: %ls", convertSeconds(ElapsedTimer.Elapsed_Seconds() - JobStartTimeSecs));
		}
		else if (JobStartTimeValid && JobEndTimeValid)
		{
			if (m_DoTime && JobDoTimeValid)
				LastTitleElapsed.Format(L"Job Time: %ls", convertSeconds(JobDoTimeSecs));
			else
				LastTitleElapsed.Format(L"Run Time: %ls", convertSeconds(JobEndTimeSecs-JobStartTimeSecs));
		}

		if (LastTitleElapsed != "")
		{
			NewTitle += " - " + LastTitleElapsed;
		}

		if (LastTitleText != NewTitle)
		{
			LastTitleText = NewTitle;
			SetWindowText(LastTitleText);
		}
	}
	else // skipping because we aren't connected
	{
		if (m_Simulate) // Update Screen frequently if in Simulate Mode
			UpdateScreen(false);
	}

	DoJoyStick();

	ServicePreview();	// G-code viewer: start a requested full-path preview when idle
	UpdatePlayback();	// G-code viewer: animate a Simulate run's cut along the preview


	int ThreadStat;
	
	// check if thread status is different from displayed

	for (int i=0; i<N_USER_GCODE_FILES; i++)
	{
		ThreadStat = GCodeThreadActive[i];
		if (DisplayedThreadStat[i] != ThreadStat)
			GetDlgItem(NumberToThreadID(i))->InvalidateRect(NULL);
	}

	
	GCodeMutex->Lock();
	
	if (GCodeThreadActive[m_Thread])
		CurrentLine[m_Thread] = m_RealTimeSetup->current_line;
	
	int Line = CurrentLine[m_Thread];

	if (Line != DisplayedCurrentLine ||
		ThreadHadError[m_Thread] != DisplayedThreadHadError)
	{
	//	m_Editor.MarkerDeleteAll(0);

		m_Editor.MarkerDelete(DisplayedCurrentLine,0);

		if (ThreadHadError[m_Thread])
		{
			m_Editor.MarkerSetFore(0,0x0000ff);
			m_Editor.MarkerSetBack(0,0x0000ff);
		}
		else
		{
			m_Editor.MarkerSetFore(0,FLAG_COLOR); // bluish green
			m_Editor.MarkerSetBack(0,FLAG_COLOR);
		}
		
		m_Editor.MarkerDefine(0,SC_MARK_ARROW);
		m_Editor.MarkerAdd(Line, 0);

		int LinesInView = m_Editor.LinesOnScreen();
		m_Editor.GotoLine(Line);
		m_Editor.SetVisiblePolicy(CARET_STRICT+CARET_EVEN,0);
		m_Editor.EnsureVisibleEnforcePolicy(Line);

		m_Editor.GotoLine(Line);

		DisplayedCurrentLine=Line;
		DisplayedThreadHadError=ThreadHadError[m_Thread];
	}
	else
	{
		// verify that the marker is really there
		//
		// if it isn't the user probably added/deleted some lines

		if (m_Editor.MarkerGet(DisplayedCurrentLine) != 1)
		{
			// see if it moved

			int line = m_Editor.MarkerNext(0,1);

			if (line == -1)
			{
				// not found, put it at the end

				DisplayedCurrentLine = CurrentLine[m_Thread] = m_Editor.GetLineCount()-1;

				m_Editor.MarkerDeleteAll(0);

				ThreadHadError[m_Thread]=false;  // lines were deleted remove error marker

				m_Editor.MarkerSetFore(0,FLAG_COLOR);
				m_Editor.MarkerSetBack(0,FLAG_COLOR);
				m_Editor.MarkerDefine(0,SC_MARK_ARROW);
				m_Editor.MarkerAdd(CurrentLine[m_Thread], 0);
			}
			else
			{
				// it was found, update Current line
				DisplayedCurrentLine = CurrentLine[m_Thread] = line;
			}
		}
	}
	GCodeMutex->Unlock();

	if (FirstStartup)
	{
		FirstStartup=false;
		if (Interpreter->InvokeAction(ACTION_PROG_START,FALSE))  // Special Command
		{
			MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate(L"Unable to perform Startup Action"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
		}
	}

	// periodically save parameters (not while a preview simulation is running:
	// it may have changed parameters with G10/G92 that are about to be restored)
	if (m_SaveFixtureOnOK && !m_PreviewRunning)
	{
		if (!m_OffsetTimerStarted || m_OffsetSaveTimer.Elapsed_Seconds() > OFFSET_SAVE_TIME)
		{
			if (Interpreter->rs274ngc_save_parameters_changed())
			{
				if (Interpreter->rs274ngc_save_parameters())
				{
					MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate(L"Error Saving Interpreter variables"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
				}
			}
			m_OffsetSaveTimer.Start();
			m_OffsetTimerStarted = true;
		}
	}

	// periodically save Screen Edit Controls
	if (CEditScreen::PersistDirty)
	{
		if (!m_EditScreenTimerStarted || m_EditScreenSaveTimer.Elapsed_Seconds() > OFFSET_SAVE_TIME)
		{
			if (CEditScreen::SavePersists())
			{
				MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate(L"Error Saving Edit Control Values"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
			}
			m_EditScreenSaveTimer.Start();
			m_EditScreenTimerStarted = true;
			CEditScreen::PersistDirty = false;
		}
	}

	CDlgX::OnTimer(nIDEvent);
	Entry--;
}

void CKMotionCNCDlg::ServiceKFLOPCommands()
{
	static bool VeryFirstTime=true;
	static bool ErrorDisplayed=false;
	static bool MsgDisplayed=false;
	static bool GcodeInProgress=false;
	double x,y,z,a,b,c,u,v,*dp;
	float F,NewValue;
	CStringA s;
	CString w;
	int i,result,index;
	MOTION_PARAMS *MP;
	CCoordMotion *CM = Interpreter->CoordMotion;


	// check for a leftover command already present when
	// the application begns.  If so, rather than just 
	// executing it flag it as a special error -2
	if (VeryFirstTime && MainStatus.PC_comm[0])
	{
		SetKFLOPCommandResult(-2);
	}
	VeryFirstTime=false;

	if (MainStatus.PC_comm[0]!=PC_COMM_MDI) GcodeInProgress=false;

	if (MainStatus.PC_comm[0] > 0)  // don't do anything if error or zero
	{
	switch (MainStatus.PC_comm[0])
	{
	case PC_COMM_ESTOP:
		// Ack the command, then perform the Emergency Stop from the normal
		// message loop (same as clicking the Screen button) rather than
		// synchronously inside command servicing.  OnEmergencyStop is a heavy
		// sequence (kills user threads, Halt/Abort, disables all axes, runs
		// the Stop Action) and executing it here while a Job is running can
		// deadlock with the executing GCode worker thread.
		SetKFLOPCommandResult(0);
		PostMessage(WM_COMMAND, IDC_EmergencyStop, 0);
		break;

	case PC_COMM_HALT:
		OnHalt();
		SetKFLOPCommandResult(0);
		break;

	case PC_COMM_HALT_NEXT_LINE:
		Interpreter->HaltNextLine();
		SetKFLOPCommandResult(0);
		break;

	case PC_COMM_ENABLE_JOG_KEYS:
		EnableJogKeys=true;
		ForceDisableJogKeys = false;
		SetKFLOPCommandResult(0);
		break;

	case PC_COMM_DISABLE_JOG_KEYS:
		EnableJogKeys = false;
		ForceDisableJogKeys = false;
		SetKFLOPCommandResult(0);
		break;

	case PC_COMM_FORCE_DISABLE_JOG_KEYS:
		ForceDisableJogKeys = true;
		SetKFLOPCommandResult(0);
		break;

	case PC_COMM_ENABLE_CONTROLS:
		if (GetStringFromGather(MainStatus.PC_comm[1], &s, 200)) break;

		for (int i = 0; i < 200; i++) // process each ID
		{
			int ID = ((int*)(s.GetBuffer()))[i];
			if (ID <= 0) break;
			GetDlgItem(ID)->EnableWindow(true);
		}
		SetKFLOPCommandResult(0);
		break;

	case PC_COMM_DISABLE_CONTROLS:
		if (GetStringFromGather(MainStatus.PC_comm[1], &s, 200)) break;

		for (int i = 0; i < 200; i++) // process each ID
		{
			int ID = ((int*)(s.GetBuffer()))[i];
			if (ID <= 0) break;
			GetDlgItem(ID)->EnableWindow(false);
		}
		SetKFLOPCommandResult(0);
		break;

	case PC_COMM_EXECUTE:
		if (!ThreadIsExecuting)
		{
			OnExecute();
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SINGLE_STEP:
		if (!ThreadIsExecuting)
		{
			OnSingleStep();
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_FRO:
		NewValue = *(float *)&MainStatus.PC_comm[1];
		if (NewValue > 0.00099 && NewValue < 1000.0)   // ignore if crazy number
		{
			m_FeedRateValue = NewValue;
			m_FeedRateValue = ((int)(m_FeedRateValue*1000.0 + 0.5)) / 1000.0;
			Interpreter->CoordMotion->SetFeedRateOverride(m_FeedRateValue);
			if (m_Rapid == 1)
			{
				w.Format(L"%.3f", m_FeedRateValue);

				// Remove trailing zeros after decimal
				while (w.Find(L'.') >= 0 && w.GetLength() > 0 && w.Right(1) == L"0")
					w = w.Left(w.GetLength() - 1);

				SetDlgItemText(IDC_FeedRateEdit, w);
				m_FeedSlider.SetPos(m_FeedRateValue);
			}
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_RRO:
		NewValue = *(float *)&MainStatus.PC_comm[1];
		if (NewValue > 0.00099 && NewValue < 1000.0)   // ignore if crazy number
		{
			m_FeedRateRapidValue = NewValue;
			m_FeedRateRapidValue = ((int)(m_FeedRateRapidValue*1000.0 + 0.5)) / 1000.0;
			Interpreter->CoordMotion->SetFeedRateRapidOverride(m_FeedRateRapidValue);
			if (m_Rapid == 0)
			{
				w.Format(L"%.3f", m_FeedRateRapidValue);
				// Remove trailing zeros after decimal
				while (w.Find(L'.') >= 0 && w.GetLength() > 0 && w.Right(1) == L"0")
					w = w.Left(w.GetLength() - 1);

				SetDlgItemText(IDC_FeedRateEdit, w);
				m_FeedSlider.SetPos(m_FeedRateRapidValue);
			}
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_SSO:
		NewValue = *(float *)&MainStatus.PC_comm[1];
		if (NewValue > 0.00099 && NewValue < 1000.0)   // ignore if crazy number
		{
			m_SpindleRateValue = NewValue;
			w.Format(L"%.3f",m_SpindleRateValue);
			// Remove trailing zeros after decimal
			while (w.Find(L'.') >= 0 && w.GetLength() > 0 && w.Right(1) == L"0")
				w = w.Left(w.GetLength() - 1);

			SetDlgItemText(IDC_SpindleRateEdit,w);
			Interpreter->CoordMotion->SetSpindleRateOverride(m_SpindleRateValue);
			m_SpindleSlider.SetPos(m_SpindleRateValue);
			Interpreter->InvokeAction(10,FALSE);  // resend new Speed
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_FRO_INC:
		NewValue = *(float *)&MainStatus.PC_comm[1] * m_FeedRateValue;
		if (NewValue > 0.00099 && NewValue < 1000.0)   // ignore if crazy number
		{
			m_FeedRateValue = NewValue;
			m_FeedRateValue = ((int)(m_FeedRateValue*1000.0 + 0.5)) / 1000.0;
			Interpreter->CoordMotion->SetFeedRateOverride(m_FeedRateValue);
			if (m_Rapid == 1)
			{
				w.Format(L"%.3f", m_FeedRateValue);
				// Remove trailing zeros after decimal
				while (w.Find(L'.') >= 0 && w.GetLength() > 0 && w.Right(1) == L"0")
					w = w.Left(w.GetLength() - 1);
				SetDlgItemText(IDC_FeedRateEdit, w);
				m_FeedSlider.SetPos(m_FeedRateValue);
			}
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_RRO_INC:
		NewValue = *(float *)&MainStatus.PC_comm[1] * m_FeedRateRapidValue;
		if (NewValue > 0.00099 && NewValue < 1000.0)   // ignore if crazy number
		{
			m_FeedRateRapidValue = NewValue;
			m_FeedRateRapidValue = ((int)(m_FeedRateRapidValue*1000.0 + 0.5)) / 1000.0;
			Interpreter->CoordMotion->SetFeedRateRapidOverride(m_FeedRateRapidValue);
			if (m_Rapid == 0)
			{
				w.Format(L"%.3f", m_FeedRateRapidValue);
				// Remove trailing zeros after decimal
				while (w.Find(L'.') >= 0 && w.GetLength() > 0 && w.Right(1) == L"0")
					w = w.Left(w.GetLength() - 1);
				SetDlgItemText(IDC_FeedRateEdit, w);
				m_FeedSlider.SetPos(m_FeedRateRapidValue);
			}
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_SSO_INC:
		NewValue = *(float *)&MainStatus.PC_comm[1] * m_SpindleRateValue;
		if (NewValue > 0.00099 && NewValue < 1000.0)   // ignore if crazy number
		{
			m_SpindleRateValue = NewValue;
			w.Format(L"%.3f",m_SpindleRateValue);
			// Remove trailing zeros after decimal
			while (w.Find(L'.') >= 0 && w.GetLength() > 0 && w.Right(1) == L"0")
				w = w.Left(w.GetLength() - 1);
			SetDlgItemText(IDC_SpindleRateEdit,w);
			Interpreter->CoordMotion->SetSpindleRateOverride(m_SpindleRateValue);
			m_SpindleSlider.SetPos(m_SpindleRateValue);
			Interpreter->InvokeAction(10,FALSE);  // resend new Speed
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_X:
		NewValue = *(float *)&MainStatus.PC_comm[1];
		if (!ThreadIsExecuting && !ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v))
		{
			Interpreter->p_setup->axis_offset_x += x - NewValue;
			RoundReasonable(Interpreter->p_setup->axis_offset_x);
			Interpreter->p_setup->parameters[5211]=Interpreter->p_setup->axis_offset_x;
			Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_Y:
		NewValue = *(float *)&MainStatus.PC_comm[1];
		if (!ThreadIsExecuting && !ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v))
		{
			Interpreter->p_setup->axis_offset_y += y - NewValue;
			RoundReasonable(Interpreter->p_setup->axis_offset_y);
			Interpreter->p_setup->parameters[5212]=Interpreter->p_setup->axis_offset_y;
			Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_Z:
		NewValue = *(float *)&MainStatus.PC_comm[1];
		if (!ThreadIsExecuting && !ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v))
		{
			Interpreter->p_setup->axis_offset_z += z - NewValue;
			RoundReasonable(Interpreter->p_setup->axis_offset_z);
			Interpreter->p_setup->parameters[5213]=Interpreter->p_setup->axis_offset_z;
			Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_A:
		NewValue = *(float *)&MainStatus.PC_comm[1];
		if (!ThreadIsExecuting && !ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v))
		{
			Interpreter->p_setup->AA_axis_offset += a - NewValue;
			RoundReasonable(Interpreter->p_setup->AA_axis_offset);
			Interpreter->p_setup->parameters[5214]=Interpreter->p_setup->AA_axis_offset;
			Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_B:
		NewValue = *(float *)&MainStatus.PC_comm[1];
		if (!ThreadIsExecuting && !ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v))
		{
			Interpreter->p_setup->BB_axis_offset += b - NewValue;
			RoundReasonable(Interpreter->p_setup->BB_axis_offset);
			Interpreter->p_setup->parameters[5215]=Interpreter->p_setup->BB_axis_offset;
			Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_C:
		NewValue = *(float*)&MainStatus.PC_comm[1];
		if (!ThreadIsExecuting && !ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v))
		{
			Interpreter->p_setup->CC_axis_offset += c - NewValue;
			RoundReasonable(Interpreter->p_setup->CC_axis_offset);
			Interpreter->p_setup->parameters[5216] = Interpreter->p_setup->CC_axis_offset;
			Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_U:
		NewValue = *(float*)&MainStatus.PC_comm[1];
		if (!ThreadIsExecuting && !ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v))
		{
			Interpreter->p_setup->UU_axis_offset += u - NewValue;
			RoundReasonable(Interpreter->p_setup->UU_axis_offset);
			Interpreter->p_setup->parameters[5217] = Interpreter->p_setup->UU_axis_offset;
			Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_V:
		NewValue = *(float*)&MainStatus.PC_comm[1];
		if (!ThreadIsExecuting && !ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v))
		{
			Interpreter->p_setup->VV_axis_offset += v - NewValue;
			RoundReasonable(Interpreter->p_setup->VV_axis_offset);
			Interpreter->p_setup->parameters[5218] = Interpreter->p_setup->VV_axis_offset;
			Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_USER_BUTTON:
		if (!ThreadIsExecuting)
		{
			Interpreter->InvokeAction(11+MainStatus.PC_comm[1],FALSE);
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_MCODE:
		if (!ThreadIsExecuting)
		{
			Interpreter->InvokeAction(MainStatus.PC_comm[1],FALSE);
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_MSG:
		if (!MsgDisplayed)
		{
			if (GetStringFromGather(MainStatus.PC_comm[1],&s,50)) break;
			CString sw = Screen.ConvertANSIToWide(s);
			MsgDisplayed=true;
			MainStatus.PC_comm[0]=0;  // clear the command now that it has been executed
			result = AfxMessageBox(sw,MainStatus.PC_comm[2]);
			MsgDisplayed=false;
			s.Format("SetPersistDec%d %d",PC_COMM_PERSIST+3,result);
			if (TheFrame->KMotionDLL->WriteLine(s)) break;
			SetKFLOPCommandResult(0);
		}
		break;


	case PC_COMM_INPUT:
		if (!MsgDisplayed)
		{	
			static CSetValue SetDlg(IDD_SetValue);
			float fvalue;
			DisableKeyJog();
			if (GetStringFromGather(MainStatus.PC_comm[1],&s,50)) break;
			SetDlg.m_Label = s;
			MsgDisplayed=true;
			if (SetDlg.DoModal() == IDOK)
				result = 0;
			else
				result = 1;
			MsgDisplayed=false;
			fvalue = SetDlg.m_Value;
			s.Format("SetPersistHex%d %x",PC_COMM_PERSIST+2,*(int *)&fvalue);
			if (TheFrame->KMotionDLL->WriteLine(s)) break;
			s.Format("SetPersistDec%d %d",PC_COMM_PERSIST+3,result);
			if (TheFrame->KMotionDLL->WriteLine(s)) break;
			SetKFLOPCommandResult(0);
		}
		break;

	case PC_COMM_GET_VARS:
		for (i=0; i<MainStatus.PC_comm[2]; i++)  // loop through how many to transfer
		{
			double d = Interpreter->p_setup->parameters[MainStatus.PC_comm[1]+i];
			if (SendOneDouble(MainStatus.PC_comm[3]+i, d)) break;
		}
		if (i==MainStatus.PC_comm[2])
			SetKFLOPCommandResult(0);
		else
			SetKFLOPCommandResult(-1);

		break;

	case PC_COMM_SET_VARS:
		for (i=0; i<MainStatus.PC_comm[2]; i++)  // loop through how many to transfer
		{
			double *d = &Interpreter->p_setup->parameters[MainStatus.PC_comm[1]+i];

			if (GetVar((MainStatus.PC_comm[3]+i)*2,    (int*)d   )) break;
			if (GetVar((MainStatus.PC_comm[3]+i)*2+1, ((int*)d)+1)) break;
		}
		SetKFLOPCommandResult(0);
		break;

	case PC_COMM_MDI:
		if (!GcodeInProgress)
		{
			if (!ThreadIsExecuting)
			{
				if (GetStringFromGather(MainStatus.PC_comm[1],&s,50)) break;
				if (DoGCodeLine((CString)s))
					SetKFLOPCommandResult(-1);  // failed tell KFLOP
				else
					GcodeInProgress=true;  // GCode is successfully Launched!
			}
			else
			{
				SetKFLOPCommandResult(-1);  // not possible to launch GCode
			}
		}
		else
		{
			if (!ThreadIsExecuting)
			{
				// tell KFLOP the final exit code
				s.Format("SetPersistDec%d %d",PC_COMM_PERSIST+2,m_exitcode);
				if (TheFrame->KMotionDLL->WriteLine(s)) break;
				SetKFLOPCommandResult(0);  // tell KFLOP Gcode is done
				GcodeInProgress=false;
			}
		}
		break;

	case PC_COMM_GETAXISRES:
		MP=&Interpreter->CoordMotion->Kinematics->m_MotionParams;
		F=MP->CountsPerInchX;
		s.Format("SetPersistHex%d %x",(MainStatus.PC_comm[1]+0),*(int*)&F);
		if (TheFrame->KMotionDLL->WriteLine(s)) break;
		F=MP->CountsPerInchY;
		s.Format("SetPersistHex%d %x",(MainStatus.PC_comm[1]+1),*(int*)&F);
		if (TheFrame->KMotionDLL->WriteLine(s)) break;
		F=MP->CountsPerInchZ;
		s.Format("SetPersistHex%d %x",(MainStatus.PC_comm[1]+2),*(int*)&F);
		if (TheFrame->KMotionDLL->WriteLine(s)) break;
		F=MP->CountsPerInchA;
		s.Format("SetPersistHex%d %x",(MainStatus.PC_comm[1]+3),*(int*)&F);
		if (TheFrame->KMotionDLL->WriteLine(s)) break;
		F=MP->CountsPerInchB;
		s.Format("SetPersistHex%d %x",(MainStatus.PC_comm[1]+4),*(int*)&F);
		if (TheFrame->KMotionDLL->WriteLine(s)) break;
		F=MP->CountsPerInchC;
		s.Format("SetPersistHex%d %x",(MainStatus.PC_comm[1]+5),*(int*)&F);
		if (TheFrame->KMotionDLL->WriteLine(s)) break;
		SetKFLOPCommandResult(0);
		break;

	case PC_COMM_RESTART:
		OnRestart();
		SetKFLOPCommandResult(0);
		break;

	case PC_COMM_SET_TOOLTABLE_LENGTH:
		{
			CEditToolFile EditToolFile;
			// first read the Tool Table File
			if (EditToolFile.LoadFile(m_ToolFile))
			{
				SetKFLOPCommandResult(-1);
				break;
			}

			index=MainStatus.PC_comm[1];

			if (index>=0 && index <100)
			{
				dp = &Interpreter->p_setup->tool_table[index].length;
				if (GetVar((MainStatus.PC_comm[2])*2,    (int*)dp   )) break;
				if (GetVar((MainStatus.PC_comm[2])*2+1, ((int*)dp)+1)) break;

				EditToolFile.ChangeLength(index,*dp);

				EditToolFile.SaveFile(m_ToolFile);
				TheFrame->GCodeDlg.Interpreter->m_ReadToolFile=true;

				// check if this tool is selected and in use.  If so,
				// update the tool length in use
				setup_pointer p=TheFrame->GCodeDlg.Interpreter->p_setup;
				if (p->active_g_codes[9]==G_43 && index == p->length_offset_index)  //active and
				{
					p->current_z = (p->current_z + p->tool_length_offset - *dp);
					p->tool_length_offset = *dp;
				}

				SetKFLOPCommandResult(0);
			}
			else
			{
				SetKFLOPCommandResult(-2);
			}
		}
		break;

	case PC_COMM_GET_TOOLTABLE_LENGTH:
		{
			CEditToolFile EditToolFile;
			// first read the Tool Table File
			if (EditToolFile.LoadFile(m_ToolFile))
			{
				SetKFLOPCommandResult(-1);
				break;
			}
			index=MainStatus.PC_comm[1];

			if (index>=0 && index <100)
			{
				double d = Interpreter->p_setup->tool_table[index].length;
				if (SendOneDouble(MainStatus.PC_comm[2], d)) break;
				SetKFLOPCommandResult(0);
			}
			else
			{
				SetKFLOPCommandResult(-2);
			}
		}
		break;

	case PC_COMM_SET_TOOLTABLE_DIAMETER:
		{
			CEditToolFile EditToolFile;
			// first read the Tool Table File
			if (EditToolFile.LoadFile(m_ToolFile))
			{
				SetKFLOPCommandResult(-1);
				break;
			}

			index=MainStatus.PC_comm[1];

			if (index>=0 && index <100)
			{
				dp = &Interpreter->p_setup->tool_table[index].diameter;
				if (GetVar((MainStatus.PC_comm[2])*2,    (int*)dp   )) break;
				if (GetVar((MainStatus.PC_comm[2])*2+1, ((int*)dp)+1)) break;

				EditToolFile.ChangeDiameter(index,*dp);

				EditToolFile.SaveFile(m_ToolFile);
				TheFrame->GCodeDlg.Interpreter->m_ReadToolFile=true;

				SetKFLOPCommandResult(0);
			}
			else
			{
				SetKFLOPCommandResult(-2);
			}
		}
		break;

	case PC_COMM_GET_TOOLTABLE_DIAMETER:
		{
			CEditToolFile EditToolFile;
			// first read the Tool Table File
			if (EditToolFile.LoadFile(m_ToolFile))
			{
				SetKFLOPCommandResult(-1);
				break;
			}
			index=MainStatus.PC_comm[1];

			if (index>=0 && index <100)
			{
				double d = Interpreter->p_setup->tool_table[index].diameter;
				if (SendOneDouble(MainStatus.PC_comm[2], d)) break;
				SetKFLOPCommandResult(0);
			}
			else
			{
				SetKFLOPCommandResult(-2);
			}
		}
		break;

	case PC_COMM_SET_TOOLTABLE_OFFSETX:
		{
			CEditToolFile EditToolFile;
			// first read the Tool Table File
			if (EditToolFile.LoadFile(m_ToolFile))
			{
				SetKFLOPCommandResult(-1);
				break;
			}

			index=MainStatus.PC_comm[1];

			if (index>=0 && index <100)
			{
				dp = &Interpreter->p_setup->tool_table[index].xoffset;
				if (GetVar((MainStatus.PC_comm[2])*2,    (int*)dp   )) break;
				if (GetVar((MainStatus.PC_comm[2])*2+1, ((int*)dp)+1)) break;

				EditToolFile.ChangeOffsetX(index,*dp);

				EditToolFile.SaveFile(m_ToolFile);
				TheFrame->GCodeDlg.Interpreter->m_ReadToolFile=true;

				// check if this tool is selected and in use.  If so,
				// update the tool length in use
				setup_pointer p=TheFrame->GCodeDlg.Interpreter->p_setup;
				if (p->active_g_codes[9]==G_43 && index == p->length_offset_index)  //active and
				{
					p->current_x = (p->current_x + p->tool_xoffset - *dp);
					p->tool_xoffset = *dp;
				}

				SetKFLOPCommandResult(0);
			}
			else
			{
				SetKFLOPCommandResult(-2);
			}
		}
		break;

	case PC_COMM_GET_TOOLTABLE_OFFSETX:
		{
			CEditToolFile EditToolFile;
			// first read the Tool Table File
			if (EditToolFile.LoadFile(m_ToolFile))
			{
				SetKFLOPCommandResult(-1);
				break;
			}
			index=MainStatus.PC_comm[1];

			if (index>=0 && index <100)
			{
				double d = Interpreter->p_setup->tool_table[index].xoffset;
				if (SendOneDouble(MainStatus.PC_comm[2], d)) break;
				SetKFLOPCommandResult(0);
			}
			else
			{
				SetKFLOPCommandResult(-2);
			}
		}
		break;

	case PC_COMM_SET_TOOLTABLE_OFFSETY:
		{
			CEditToolFile EditToolFile;
			// first read the Tool Table File
			if (EditToolFile.LoadFile(m_ToolFile))
			{
				SetKFLOPCommandResult(-1);
				break;
			}

			index=MainStatus.PC_comm[1];

			if (index>=0 && index <100)
			{
				dp = &Interpreter->p_setup->tool_table[index].yoffset;
				if (GetVar((MainStatus.PC_comm[2])*2,    (int*)dp   )) break;
				if (GetVar((MainStatus.PC_comm[2])*2+1, ((int*)dp)+1)) break;

				EditToolFile.ChangeOffsetY(index,*dp);

				EditToolFile.SaveFile(m_ToolFile);
				TheFrame->GCodeDlg.Interpreter->m_ReadToolFile=true;

				// check if this tool is selected and in use.  If so,
				// update the tool length in use
				setup_pointer p=TheFrame->GCodeDlg.Interpreter->p_setup;
				if (p->active_g_codes[9]==G_43 && index == p->length_offset_index)  //active and
				{
					p->current_y = (p->current_y + p->tool_yoffset - *dp);
					p->tool_yoffset = *dp;
				}

				SetKFLOPCommandResult(0);
			}
			else
			{
				SetKFLOPCommandResult(-2);
			}
		}
		break;

	case PC_COMM_GET_TOOLTABLE_OFFSETY:
		{
			CEditToolFile EditToolFile;
			// first read the Tool Table File
			if (EditToolFile.LoadFile(m_ToolFile))
			{
				SetKFLOPCommandResult(-1);
				break;
			}
			index=MainStatus.PC_comm[1];

			if (index>=0 && index <100)
			{
				double d = Interpreter->p_setup->tool_table[index].yoffset;
				if (SendOneDouble(MainStatus.PC_comm[2], d)) break;
				SetKFLOPCommandResult(0);
			}
			else
			{
				SetKFLOPCommandResult(-2);
			}
		}
		break;



		// GetToolTableComment Persist+1 = Tool Table Index
		//					   Persist+2 = gather buffer offset (32-bit words) to where to place Comment string
		//					   Result indicating complete.  0=Not Complete, 1=Complete, -1=Err reading Tool File, -2=Invalid Tool Index
	case PC_COMM_GET_TOOLTABLE_COMMENT:
	{
		index = MainStatus.PC_comm[1];

		if (index >= 0 && index < 100)
		{
			index = MainStatus.PC_comm[1];

			if (index >= 0 && index < 100)
			{
				int gather_offset;

				if (TheFrame->GCodeDlg.Interpreter->m_ReadToolFile)
					TheFrame->GCodeDlg.Interpreter->ReadToolFile();

				s = Interpreter->p_setup->tool_table[index].Comment;
				gather_offset = MainStatus.PC_comm[2];
				if (SetStringToGather(gather_offset, s)) return;
				SetKFLOPCommandResult(0);
			}
			else
			{
				SetKFLOPCommandResult(-2);
			}
		}
	}
	break;

		// SetToolTableComment Persist+1 = Tool Table Index
		//					   Persist+2 = gather buffer offset (32-bit words) to where to get Comment string
		//					   Result indicating complete.  0=Not Complete, 1=Complete, -1=Invalid Tool Index
	case PC_COMM_SET_TOOLTABLE_COMMENT:
	{
		CEditToolFile EditToolFile;
		// first read the Tool Table File
		if (EditToolFile.LoadFile(m_ToolFile))
		{
			SetKFLOPCommandResult(-1);
			break;
		}

		index = MainStatus.PC_comm[1];

		if (index >= 0 && index < 100)
		{
			int gather_offset;
			TheFrame->GCodeDlg.Interpreter->m_ReadToolFile = true;
			gather_offset = MainStatus.PC_comm[2];
			if (GetStringFromGather(gather_offset, &s, 50)) return;
			EditToolFile.ChangeComment(index, (CString)s);
			EditToolFile.SaveFile(m_ToolFile);
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;
	}

	case PC_COMM_GET_TOOLTABLE_INDEX:
	{
		int ToolIndex;
		int ToolID = MainStatus.PC_comm[1];

		int result = ConvertToolToIndex(ToolID, &ToolIndex);

		if (!result)
		{
			s.Format("SetPersistHex%d %x", MainStatus.PC_comm[2], ToolIndex);
			if (TheFrame->KMotionDLL->WriteLine(s)) break;
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
	}
	break;


	case PC_COMM_GET_MISC_SETTINGS:  // Units, T, H, D indexes 
		s.Format("SetPersistHex%d %x",MainStatus.PC_comm[1],Interpreter->p_setup->length_units);
		if (TheFrame->KMotionDLL->WriteLine(s)) break;
		s.Format("SetPersistHex%d %x",MainStatus.PC_comm[1]+1,Interpreter->p_setup->selected_tool_slot);
		if (TheFrame->KMotionDLL->WriteLine(s)) break;
		s.Format("SetPersistHex%d %x",MainStatus.PC_comm[1]+2,Interpreter->p_setup->length_offset_index); // H number
		if (TheFrame->KMotionDLL->WriteLine(s)) break;
		s.Format("SetPersistHex%d %x",MainStatus.PC_comm[1]+3,Interpreter->p_setup->tool_table_index); // D number
		if (TheFrame->KMotionDLL->WriteLine(s)) break;
		SetKFLOPCommandResult(0);
		break;

	case PC_COMM_GET_TOOL_SLOT_ID:  // Tool Slot and ID
		s.Format("SetPersistHex%d %x", MainStatus.PC_comm[1], Interpreter->p_setup->tool_table[Interpreter->p_setup->selected_tool_slot].slot);
		if (TheFrame->KMotionDLL->WriteLine(s)) break;
		s.Format("SetPersistHex%d %x", MainStatus.PC_comm[1] + 1, Interpreter->p_setup->tool_table[Interpreter->p_setup->selected_tool_slot].id);
		if (TheFrame->KMotionDLL->WriteLine(s)) break;
		SetKFLOPCommandResult(0);
		break;

	case PC_COMM_GET_MACHINE_COORDS:
		if (SendCoordinates(MainStatus.PC_comm[1], true)) break;
		SetKFLOPCommandResult(0);
		break;

	case PC_COMM_GET_DROS:
		if (SendCoordinates(MainStatus.PC_comm[1], false)) break;
		SetKFLOPCommandResult(0);
		break;

	case PC_COMM_UPDATE_FIXTURE:
		{
			setup_pointer p=TheFrame->GCodeDlg.Interpreter->p_setup;
			int Fix = p->origin_index-1;
			double *Vars = p->parameters;	

			// for inches round to 6 digits for mm round to 4 digits
			for (int axis = 0; axis < NFIXAXES; axis++)
				RoundReasonable(Vars[5221 + Fix * 20 + axis]);

			p->origin_index=-1; // set invalid so it updates
			Interpreter->ChangeFixtureNumber(m_LastFixtureDisplayed);

			SetKFLOPCommandResult(0);
		}
		break;

	case PC_COMM_RELOAD_GEOCORRECTION:
		{
			if (Interpreter->CoordMotion->Kinematics->ReadGeoTable(m_GeoFile))  // reload Geo Correction Table
				SetKFLOPCommandResult(-1);  // return negative error code to KFLOP if failed
			else
				SetKFLOPCommandResult(0);   // Return success code
		}
		break;


	case PC_COMM_GET_JOG_OVERRIDE_X:
	case PC_COMM_GET_JOG_OVERRIDE_Y:
	case PC_COMM_GET_JOG_OVERRIDE_Z:
	case PC_COMM_GET_JOG_OVERRIDE_A:
	case PC_COMM_GET_JOG_OVERRIDE_B:
	case PC_COMM_GET_JOG_OVERRIDE_C:
	case PC_COMM_GET_JOG_OVERRIDE_U:
	case PC_COMM_GET_JOG_OVERRIDE_V:
		{
			F = m_JogSpeedOverride[MainStatus.PC_comm[0] - PC_COMM_GET_JOG_OVERRIDE_X];
			s.Format("SetPersistHex%d %x",PC_COMM_PERSIST+1,*(int*)&F);
			if (TheFrame->KMotionDLL->WriteLine(s)) break;
			SetKFLOPCommandResult(0);
		}
		break;

	case PC_COMM_SET_JOG_OVERRIDE_X:
	case PC_COMM_SET_JOG_OVERRIDE_Y:
	case PC_COMM_SET_JOG_OVERRIDE_Z:
	case PC_COMM_SET_JOG_OVERRIDE_A:
	case PC_COMM_SET_JOG_OVERRIDE_B:
	case PC_COMM_SET_JOG_OVERRIDE_C:
	case PC_COMM_SET_JOG_OVERRIDE_U:
	case PC_COMM_SET_JOG_OVERRIDE_V:
		{
			m_JogSpeedOverride[MainStatus.PC_comm[0] - PC_COMM_SET_JOG_OVERRIDE_X] = *(float*)&MainStatus.PC_comm[1];
			SetKFLOPCommandResult(0);
		}
		break;

	case PC_COMM_RECREATE_DIALOG_FACE:
		{
			DestroyWindow();
			Create(IDD_KMOTIONCNC_0_ORIGINAL+m_DialogFaceInUse); // put up the real main window
			SetKFLOPCommandResult(0);
		}
		break;

	// GetEditCell Persist+1 = gather buffer offset (32-bit words) to where to place Edit string
	//             Persist+2 = Result indicating complete.  0=Not Complete, 1=Complete, -1=Error
	case PC_GET_EDIT_CELL:
		if (Screen.GetEditScreenVar(MainStatus.PC_comm[1], &s))
		{
			SetKFLOPCommandResult(-1);
			break;
		}
		else
		{
			int gather_offset;
			if (GetVar((MainStatus.PC_comm[1]), &gather_offset)) return;
			if (SetStringToGather(gather_offset, s)) return;
			SetKFLOPCommandResult(0);
		}
		break;

		//For example "WinMsg:Keyboard;;KeyPress;120" to press F9
		//For example "WinMsg:DlgName; IDC_SpindleOnCW; BM_CLICK;" to click the Spindle on cw button
		//For example "Action : 5; 3; 0; 0; 0; 0; SpindleUsingJogs\CSS\OnCWJog.c" to execute/wait the seec prog in Thread 3 Var 0

	case PC_COMM_SCREEN_SCRIPT:
		{
			bool NewControl;
			if (GetStringFromGather(MainStatus.PC_comm[1], &s, 50)) break;
			CString sw = s;
			if (Screen.DlgControls.GetCount() > 0 && Screen.Execute(sw, Screen.DlgControls.GetHead(), &NewControl))
				SetKFLOPCommandResult(-1);  // failed tell KFLOP
			else
				SetKFLOPCommandResult(0); // success
		}
		break;

		// Get Current GCode Line Persist+1 = gather buffer offset (32-bit words) to where to place GCode string
		//					   Result indicating complete.  0=Not Complete, 1=Complete, -1=error
	case PC_COMM_GET_GCODE_LINE:
	{
		int gather_offset = MainStatus.PC_comm[1];
		s = Screen.ConvertWideToANSI(m_RealTimeSetup->linetext);
		if (SetStringToGather(gather_offset, s)) return;
		SetKFLOPCommandResult(0);
	}
	break;

	// Get Current GCode Line Persist+1 = gather buffer offset (32-bit words) to where to place GCode string
	//					   Result indicating complete.  0=Not Complete, 1=Complete, -1=error
	case PC_COMM_GET_DATE_TIME:
	{
		int gather_offset = MainStatus.PC_comm[1];
		CTime t = CTime::GetCurrentTime();
		s = Screen.ConvertWideToANSI(t.Format("%a, %b %d, %Y, %I:%M:%S  "));
		if (SetStringToGather(gather_offset, s)) return;
		SetKFLOPCommandResult(0);
	}
	break;

	// Get Trajectory Planner Axis Parameter Persist+1 = Parameter Type number (integer) Velocity, Acceleration, CountsPerInch, JogSpeed
	//										 Persist+2 = Axis Number (integer) XYZABCUV
	//										 Persist+3 = where to set value to KFLOP persist (double offset)
	//     (Parameter -> KFLOP persist) note: vars are transferred as doubles 2 persists each
	//					   Result indicating complete.  0=Not Complete, 1=Complete, -1=error
	case PC_COMM_GET_TP_PARAM:
	{
		int Type = MainStatus.PC_comm[1];
		int Axis = MainStatus.PC_comm[2];
		int Offset = MainStatus.PC_comm[3];

		if (Type >= 0 && Type < NUM_PARAM_TYPES && Axis >= 0 && Axis < NUM_AXES && Offset >= 0 && Offset < 100)
		{
			double d;
			if (Type == PT_JOG_VEL)
				d = m_JogSpeed[Axis];
			else
				d = this->*TP_ParamMember[Type][Axis];
			if (SendOneDouble(Offset, d)) break;
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-2);
		}
	}
	break;

	// Set Trajectory Planner Axis Parameter Persist+1 = Parameter Type number (integer) Velocity, Acceleration, CountsPerInch, JogSpeed
	//										 Persist+2 = Axis Number (integer) XYZABCUV
	//										 Persist+3 = where to get value from KFLOP persist (double offset)
	//     (Parameter -> KFLOP persist) note: vars are transferred as doubles 2 persists each
	//					   Result indicating complete.  0=Not Complete, 1=Complete, -1=error
	case PC_COMM_SET_TP_PARAM:
	{
		int Type = MainStatus.PC_comm[1];
		int Axis = MainStatus.PC_comm[2];
		int Offset = MainStatus.PC_comm[3];

		if (Type >= 0 && Type < NUM_PARAM_TYPES && Axis >= 0 && Axis < NUM_AXES && Offset >= 0 && Offset < 100)
		{
			double *dp;
			if (Type == PT_JOG_VEL)
				dp = &m_JogSpeed[Axis];
			else
				dp = &(this->*TP_ParamMember[Type][Axis]);
			if (GetVar(Offset * 2, (int*)dp)) break;
			if (GetVar(Offset * 2 + 1, ((int*)dp) + 1)) break;
			SetMotionParams();
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-2);
		}
	}
	break;

	// Do G43 Hxx Set Tool Length Comp On for Tool xx Persist+1 = H number (integer) from G43Hxx command
	case PC_COMM_G43:
	{
		int Tool = MainStatus.PC_comm[1];

		if (Interpreter->convert_tool_length_offset(G_43, Tool) == 0)
		{
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
	}
	break;

	// Do G43.4 Hxx Set Tool Length Comp On (with TCP for Tool xx Persist+1 = H number (integer) from G43Hxx command
	case PC_COMM_G43_4:
	{
		int Tool = MainStatus.PC_comm[1];

		if (Interpreter->convert_tool_length_offset(G_43_4, Tool) == 0)
		{
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
	}
	break;

	// Do G49 Set Tool Length Comp Off
	case PC_COMM_G49:
	{
		if (Interpreter->convert_tool_length_offset(G_49, -1) == 0)
		{
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
	}
	break;


	default:
		if (!ErrorDisplayed)
		{
			ErrorDisplayed=true;
			SetKFLOPCommandResult(-1);  // failed tell KFLOP
			MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate("Invalid External Command Received"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
			ErrorDisplayed=false;
		}
		break;
		}
	}

	MainStatus.PC_comm[0]=0;  // clear the command now that it has been executed
}

// convert GCode Tool number to tool table index based on 
// numbers 99 or less as slot numbers and higher numbers as IDs

int CKMotionCNCDlg::ConvertToolToIndex(int number,int *index)
{
	setup_pointer p=TheFrame->GCodeDlg.Interpreter->p_setup;
	int i;

	if (number > 99)
	{
		for (i=0; i<p->tool_max; i++)
		{
			if (p->tool_table[i].id == number) break; 
		}
		if (i >= p->tool_max)
		{
			return 1;
		}
	}
	else
	{
		for (i=0; i<p->tool_max; i++)
		{
			if (p->tool_table[i].slot == number) break; 
		}
		if (i >= p->tool_max)
		{
			return 1;
		}
	}
	*index=i;
    return 0;
}




int CKMotionCNCDlg::SendCoordinates(int n, bool MachineCoords)
{
	double ActsDest[MAX_ACTUATORS],x,y,z,a,b,c,u,v;
	CCoordMotion *CM = Interpreter->CoordMotion;

	for (int i=0; i<MAX_ACTUATORS; i++) ActsDest[i]=0.0;

	if (CM->x_axis >= 0) ActsDest[0] = MainStatus.Dest[CM->x_axis];
	if (CM->y_axis >= 0) ActsDest[1] = MainStatus.Dest[CM->y_axis];
	if (CM->z_axis >= 0) ActsDest[2] = MainStatus.Dest[CM->z_axis];
	if (CM->a_axis >= 0) ActsDest[3] = MainStatus.Dest[CM->a_axis];
	if (CM->b_axis >= 0) ActsDest[4] = MainStatus.Dest[CM->b_axis];
	if (CM->c_axis >= 0) ActsDest[5] = MainStatus.Dest[CM->c_axis];
	if (CM->u_axis >= 0) ActsDest[6] = MainStatus.Dest[CM->u_axis];
	if (CM->v_axis >= 0) ActsDest[7] = MainStatus.Dest[CM->v_axis];

	CM->Kinematics->TransformActuatorstoCAD(ActsDest,&x,&y,&z,&a,&b,&c,&u,&v);

	if (MachineCoords)
		Interpreter->ConvertAbsoluteToMachine(x,y,z,a,b,c,u,v,&x,&y,&z,&a,&b,&c,&u,&v);
	else
		Interpreter->ConvertAbsoluteToInterpreterCoord(x,y,z,a,b,c,u,v,&x,&y,&z,&a,&b,&c,&u,&v);

	if (SendOneDouble(n + 0, x)) return 1;
	if (SendOneDouble(n + 1, y)) return 1;
	if (SendOneDouble(n + 2, z)) return 1;
	if (SendOneDouble(n + 3, a)) return 1;
	if (SendOneDouble(n + 4, b)) return 1;
	if (SendOneDouble(n + 5, c)) return 1;
	if (SendOneDouble(n + 6, u)) return 1;
	if (SendOneDouble(n + 7, v)) return 1;
	return 0;
}


// Send one double to KFLOP as two 32-bit binary images
int CKMotionCNCDlg::SendOneDouble(int i, double d)
{
	CStringA s,r;

	s.Format("SetPersistHex%d %x",i*2,((int*)&d)[0]);
	if (TheFrame->KMotionDLL->WriteLine(s)) return 1;
	s.Format("SetPersistHex%d %x",i*2+1,((int*)&d)[1]);
	if (TheFrame->KMotionDLL->WriteLine(s)) return 1;
	return 0;
}

int CKMotionCNCDlg::GetVar(int Var, int *value)
{
	CStringA s,r;

	if (Var >= 0 && Var < 200)
	{
		s.Format("GetPersistHex %d",Var);
		if(TheFrame->KMotionDLL->WriteLineReadLine(s, r.GetBufferSetLength(200))) return 1;
		r.ReleaseBuffer();
		if (sscanf(r,"%x",value) != 1)
		{
			// response not valid hex - comm stream out of sync.  Treat as fatal
			// like any other comm error (disconnect/reconnect) rather than
			// silently keeping a stale or garbled value
			TheFrame->KMotionDLL->Failed();
			return 1;
		}
	}
	return 0;
}

int CKMotionCNCDlg::SetVar(int Var, int value)
{
	CStringA s, r;

	if (Var >= 0 && Var < 200)
	{
		s.Format("SetPersistDec %d %d", Var, value);
		if (TheFrame->KMotionDLL->WriteLine(s)) return 1;
	}
	return 0;
}



int CKMotionCNCDlg::GetStringFromGather(int WordOffset, CStringA *msg, int nWords)
{
	CStringA s;

	int L = 256;
	if (GetBoardType() == BOARD_TYPE_KFLOP) L = 8;

	TheFrame->KMotionDLL->WaitToken("KMCNCGetStrGath");
	s.Format("GetGatherHex %d %d",WordOffset,nWords);  
	TheFrame->KMotionDLL->WriteLine(s);

	for (int i=0;i<nWords;i++)  // convert hex to 32 bit words
	{
		if ((i%L)==0)  // every 8 hex get a new line
		{
			s.Empty();
			if (TheFrame->KMotionDLL->ReadLineTimeOut( s.GetBuffer(MAX_LINE+1),5000))
			{
				TheFrame->KMotionDLL->ReleaseToken();
				return 1;
			}
			s.ReleaseBuffer();
		}

		int result = sscanf(s.GetBuffer(0),"%8X ",(int*)(msg->GetBufferSetLength(nWords*4+8)+i*4));

		if (result!=1)
		{
			TheFrame->KMotionDLL->ReleaseToken();
			return 1;
		}

		s.Delete(0,9);  // delete hex value
	}
	TheFrame->KMotionDLL->ReleaseToken();

	msg->GetBufferSetLength(nWords*4+1)[nWords*4]=0;  // make sure it is terminated
	msg->ReleaseBuffer();

	return 0;
}



int CKMotionCNCDlg::SetStringToGather(int WordOffset, CStringA msg)
{
	CStringA s, s2;

	int L = 256;
	if (GetBoardType() == BOARD_TYPE_KFLOP) L = 8;

	int nWords = (msg.GetLength() + 1 + 3) / 4;

	TheFrame->KMotionDLL->WaitToken("KMCNCSetStrGath");
	s.Format("SetGatherHex %d %d", WordOffset, nWords);
	TheFrame->KMotionDLL->WriteLine(s);

	for (int i = 0; i < nWords; i++)  // convert hex to 32 bit words
	{
		int k = msg[i*4];
		if (msg.GetLength() >= i*4 + 1)k = k + (msg[i*4 + 1] << 8);
		if (msg.GetLength() >= i*4 + 2)k = k + (msg[i*4 + 2] << 16);
		if (msg.GetLength() >= i*4 + 3)k = k + (msg[i*4 + 3] << 24);

		// send the data  (8 hex words per line)

		s2.Format("%X", k);

		if ((i % L) == 0)
			s = s2;
		else
			s = s + " " + s2;

		if (((i % L) == L-1) || i == nWords - 1)  // every 8/256 or on the last send it
		{
			if (TheFrame->KMotionDLL->WriteLine(s))
			{
				TheFrame->KMotionDLL->ReleaseToken();
				return 1;
			}
		}
	}
	TheFrame->KMotionDLL->ReleaseToken();
	return 0;
}



int CKMotionCNCDlg::SetKFLOPCommandResult(int r)
{
	CStringA s;

	MainStatus.PC_comm[0]=0;  // clear the command now that it has been executed
	s.Format("SetPersistDec%d %d",PC_COMM_PERSIST,r);
	if (TheFrame->KMotionDLL->WriteLine(s)) return 1;
	return 0;
}

// converts 0 based index to thread ID

int CKMotionCNCDlg::NumberToThreadID(int i) 
{
	switch (i)
	{
	case 0 : return IDC_Thread1;
	case 1 : return IDC_Thread2;
	case 2 : return IDC_Thread3;
	case 3 : return IDC_Thread4;
	case 4 : return IDC_Thread5;
	case 5 : return IDC_Thread6;
	case 6 : return IDC_Thread7;
	}

	return 0;
}


void CKMotionCNCDlg::OnToolSetup() 
{
	bool lathe_changed, slow_jog_changed, screen_script_changed;

	CString PasswordFile = TheFrame->MainPathRoot + TOOL_SETUP_PASSWORD_FILE;

	FILE* f;
	_tfopen_s(&f, PasswordFile, _T("rt,ccs=UTF-8"));

	if (f)
	{
		CString Password;
		fgetws(Password.GetBufferSetLength(256), 255, f);
		Password.ReleaseBuffer();
		Password.Remove('\n');
		Password.Remove('\r');
		fclose(f);

		CSetValue SetDlg(IDD_EnterPassword);
		SetDlg.StringMode = true;

		if (SetDlg.DoModal() != IDOK) return;

		if (SetDlg.m_ValueString != Password)
		{
			MessageBox(/*TRAN*/TheFrame->KMotionDLL->Translate("Invalid Tool Setup Password"),
				/*TRAN*/TheFrame->KMotionDLL->Translate("Password"),
				MB_OK | MB_ICONERROR);
			return;
		}
	}


	CAllToolSetupSheet Dlg(L"Tool Setup Screen",this);

	DisableKeyJog();
	Dlg.LastPageViewed = m_LastToolSetupPage;
	Dlg.m_ToolSetupTPPage.ConfigUnitsMM = m_LastConfigUnitsMM;
	Dlg.m_ToolSetupFilesPage.m_ToolFile = m_ToolFile;
	Dlg.m_ToolSetupFilesPage.m_SetupFile = m_SetupFile;
	Dlg.m_ToolSetupFilesPage.m_GeoFile = m_GeoFile;
	Dlg.m_ToolSetupFilesPage.m_VarsFile = m_VarsFile;
	Dlg.m_ToolSetupFilesPage.m_ScreenScriptFile = m_ScreenScriptFile;
	Dlg.m_ToolSetupTPPage.m_BreakAngle = m_BreakAngle;
	Dlg.m_ToolSetupTPPage.m_CollinearTol = m_CollinearTol;
	Dlg.m_ToolSetupTPPage.m_CornerTol = m_CornerTol;
	Dlg.m_ToolSetupTPPage.m_FacetAngle = m_FacetAngle;
	Dlg.m_ToolSetupTPPage.m_ReverseRZ = m_ReverseRZ;
	Dlg.m_ToolSetupTPPage.m_EnableGamePad = m_EnableGamePad;
	Dlg.m_ToolSetupTPPage.m_ZeroUsingFixtures = m_ZeroUsingFixtures;
	Dlg.m_ToolSetupTPPage.m_ToolLengthImmediately = m_ToolLengthImmediately;
	Dlg.m_ToolSetupTPPage.m_ToolTableDoM6 = m_ToolTableDoM6;
	Dlg.m_ToolSetupTPPage.m_ConfirmExit = m_ConfirmExit;
	Dlg.m_ToolSetupTPPage.m_AllowConcaveCorners = m_AllowConcaveCorners;
	Dlg.m_ToolSetupTPPage.m_ArcsToSegs = m_ArcsToSegs;
	Dlg.m_ToolSetupTPPage.m_DisplayEncoder = m_DisplayEncoder;
	Dlg.m_ToolSetupTPPage.m_DegreesA = m_DegreesA;
	Dlg.m_ToolSetupTPPage.m_DegreesB = m_DegreesB;
	Dlg.m_ToolSetupTPPage.m_DegreesC = m_DegreesC;
	Dlg.m_ToolSetupTPPage.m_Lathe = m_Lathe;
	Dlg.m_ToolSetupTPPage.m_DoRapidsAsFeeds = m_DoRapidsAsFeeds;
	Dlg.m_ToolSetupTPPage.m_DiameterMode = m_DiameterMode;
	Dlg.m_ToolSetupTPPage.m_XPosFront = m_XPosFront;
	Dlg.m_ToolSetupTPPage.m_JogSlowPercent = m_JogSlowPercent;
	Dlg.m_ToolSetupTPPage.m_HardwareFRORange = m_HardwareFRORange;
	Dlg.m_ToolSetupTPPage.m_MaxRapidFRO = m_MaxRapidFRO;
	Dlg.m_ToolSetupTPPage.m_ArcRadiusTol = m_ArcRadiusTol;
	Dlg.m_ToolSetupTPPage.m_ArcRSmallTol = m_ArcRSmallTol;
	Dlg.m_ToolSetupTPPage.m_JogSpeedX = m_JogSpeed[0];
	Dlg.m_ToolSetupTPPage.m_JogSpeedY = m_JogSpeed[1];
	Dlg.m_ToolSetupTPPage.m_JogSpeedZ = m_JogSpeed[2];
	Dlg.m_ToolSetupTPPage.m_JogSpeedA = m_JogSpeed[3];
	Dlg.m_ToolSetupTPPage.m_JogSpeedB = m_JogSpeed[4];
	Dlg.m_ToolSetupTPPage.m_JogSpeedC = m_JogSpeed[5];
	Dlg.m_ToolSetupTPPage.m_JogSpeedU = m_JogSpeed[6];
	Dlg.m_ToolSetupTPPage.m_JogSpeedV = m_JogSpeed[7];
	Dlg.m_ToolSetupTPPage.m_Step0 = m_Step0;
	Dlg.m_ToolSetupTPPage.m_Step1 = m_Step1;
	Dlg.m_ToolSetupTPPage.m_Step2 = m_Step2;
	Dlg.m_ToolSetupTPPage.m_Step3 = m_Step3;
	Dlg.m_ToolSetupTPPage.m_Step4 = m_Step4;
	Dlg.m_ToolSetupTPPage.m_Step5 = m_Step5;


	Dlg.m_ToolSetupTPPage.m_SpindleType = m_SpindleType;
	Dlg.m_ToolSetupTPPage.m_SpindleAxis = m_SpindleAxis;
	Dlg.m_ToolSetupTPPage.m_SpindleUpdateTime = m_SpindleUpdateTime;
	Dlg.m_ToolSetupTPPage.m_SpindleTau = m_SpindleTau;
	Dlg.m_ToolSetupTPPage.m_SpindleCntsPerRev = m_SpindleCntsPerRev;

	Dlg.m_ToolSetupTPPage.m_TPLookahead = m_TPLookahead;
	Dlg.m_ToolSetupTPPage.m_MaxAccelX = m_MaxAccelX;
	Dlg.m_ToolSetupTPPage.m_MaxAccelY = m_MaxAccelY;
	Dlg.m_ToolSetupTPPage.m_MaxAccelZ = m_MaxAccelZ;
	Dlg.m_ToolSetupTPPage.m_MaxAccelU = m_MaxAccelU;
	Dlg.m_ToolSetupTPPage.m_MaxAccelV = m_MaxAccelV;
	Dlg.m_ToolSetupTPPage.m_MaxAccelA = m_MaxAccelA;
	Dlg.m_ToolSetupTPPage.m_MaxAccelB = m_MaxAccelB;
	Dlg.m_ToolSetupTPPage.m_MaxAccelC = m_MaxAccelC;

	Dlg.m_ToolSetupTPPage.m_RadiusA = m_RadiusA;
	Dlg.m_ToolSetupTPPage.m_RadiusB = m_RadiusB;
	Dlg.m_ToolSetupTPPage.m_RadiusC = m_RadiusC;
	
	Dlg.m_ToolSetupTPPage.m_MaxVelC = m_MaxVelC;
	Dlg.m_ToolSetupTPPage.m_MaxVelB = m_MaxVelB;
	Dlg.m_ToolSetupTPPage.m_MaxVelA = m_MaxVelA;
	Dlg.m_ToolSetupTPPage.m_MaxVelX = m_MaxVelX;
	Dlg.m_ToolSetupTPPage.m_MaxVelY = m_MaxVelY;
	Dlg.m_ToolSetupTPPage.m_MaxVelZ = m_MaxVelZ;
	Dlg.m_ToolSetupTPPage.m_MaxVelU = m_MaxVelU;
	Dlg.m_ToolSetupTPPage.m_MaxVelV = m_MaxVelV;

	Dlg.m_ToolSetupTPPage.m_CountsPerInchC = m_CountsPerInchC;
	Dlg.m_ToolSetupTPPage.m_CountsPerInchB = m_CountsPerInchB;
	Dlg.m_ToolSetupTPPage.m_CountsPerInchA = m_CountsPerInchA;
	Dlg.m_ToolSetupTPPage.m_CountsPerInchX = m_CountsPerInchX;
	Dlg.m_ToolSetupTPPage.m_CountsPerInchY = m_CountsPerInchY;
	Dlg.m_ToolSetupTPPage.m_CountsPerInchZ = m_CountsPerInchZ;
	Dlg.m_ToolSetupTPPage.m_CountsPerInchU = m_CountsPerInchU;
	Dlg.m_ToolSetupTPPage.m_CountsPerInchV = m_CountsPerInchV;

	// Actuator Space Limits + 3rd Order TP options (on the TP page)
	Dlg.m_ToolSetupTPPage.m_ActuatorLimits = m_ActuatorLimits;
	for (int iact = 0; iact < MAX_TP_ACTUATORS; iact++)
	{
		Dlg.m_ToolSetupTPPage.m_ActScale[iact]    = m_ActScale[iact];
		Dlg.m_ToolSetupTPPage.m_MaxActVel[iact]   = m_MaxActVel[iact];
		Dlg.m_ToolSetupTPPage.m_MaxActAccel[iact] = m_MaxActAccel[iact];
		Dlg.m_ToolSetupTPPage.m_MaxActJerk[iact]  = m_MaxActJerk[iact];
		Dlg.m_ToolSetupTPPage.m_ActDegrees[iact]  = m_ActDegrees[iact];
	}
	Dlg.m_ToolSetupTPPage.m_ThirdOrderTP = m_ThirdOrderTP;
	Dlg.m_ToolSetupTPPage.m_TPLogSegs = m_TPLogSegs;
	Dlg.m_ToolSetupTPPage.m_TPCubicKnots = m_TPCubicKnots;
	Dlg.m_ToolSetupTPPage.m_MaxJerkX = m_MaxJerkX;
	Dlg.m_ToolSetupTPPage.m_MaxJerkY = m_MaxJerkY;
	Dlg.m_ToolSetupTPPage.m_MaxJerkZ = m_MaxJerkZ;
	Dlg.m_ToolSetupTPPage.m_MaxJerkA = m_MaxJerkA;
	Dlg.m_ToolSetupTPPage.m_MaxJerkB = m_MaxJerkB;
	Dlg.m_ToolSetupTPPage.m_MaxJerkC = m_MaxJerkC;
	Dlg.m_ToolSetupTPPage.m_MaxJerkU = m_MaxJerkU;
	Dlg.m_ToolSetupTPPage.m_MaxJerkV = m_MaxJerkV;

	Dlg.m_ToolSetupButtonsPage.m_Button0 = m_Button0;
	Dlg.m_ToolSetupButtonsPage.m_Button1 = m_Button1;
	Dlg.m_ToolSetupButtonsPage.m_Button2 = m_Button2;
	Dlg.m_ToolSetupButtonsPage.m_Button3 = m_Button3;
	Dlg.m_ToolSetupButtonsPage.m_Button4 = m_Button4;
	Dlg.m_ToolSetupButtonsPage.m_Button5 = m_Button5;
	Dlg.m_ToolSetupButtonsPage.m_Button6 = m_Button6;
	Dlg.m_ToolSetupButtonsPage.m_Button7 = m_Button7;
	Dlg.m_ToolSetupButtonsPage.m_Button8 = m_Button8;
	Dlg.m_ToolSetupButtonsPage.m_Button9 = m_Button9;
	Dlg.m_ToolSetupFilesPage.m_DialogFace = m_DialogFace;

	// M1 page handles M0-11 and Specials
	MCODE_ACTION *src=Interpreter->McodeActions;
	MCODE_ACTION *dst=Dlg.m_ToolSetupM1Page.McodeActions;
	memcpy(&dst[0],&src[0],MAX_MCODE_ACTIONS_M1*sizeof(MCODE_ACTION));
	memcpy(&dst[MAX_MCODE_ACTIONS_M1],&src[MCODE_ACTIONS_SPECIAL_OFFSET],MAX_MCODE_ACTIONS_SPECIAL*sizeof(MCODE_ACTION));
	dst=Dlg.m_ToolSetupButtonsPage.McodeActions;
	memcpy(&dst[MAX_MCODE_ACTIONS_M1],&src[MAX_MCODE_ACTIONS_M1],MAX_MCODE_ACTIONS_BUTTONS*sizeof(MCODE_ACTION));
	dst=Dlg.m_ToolSetupM100Page.McodeActions;
	memcpy(&dst[0],&src[MAX_MCODE_ACTIONS_M1+MAX_MCODE_ACTIONS_BUTTONS],MAX_MCODE_ACTIONS_M100*sizeof(MCODE_ACTION));
	
	memcpy(Dlg.m_ToolSetupButtonsPage.UserButtonKeys,m_UserButtonKeys,NUSERBUTTONS*sizeof(int));

	if (Dlg.DoModal() == IDOK)
	{
		m_ToolFile  = Dlg.m_ToolSetupFilesPage.m_ToolFile;
		m_SetupFile = Dlg.m_ToolSetupFilesPage.m_SetupFile;
		m_GeoFile = Dlg.m_ToolSetupFilesPage.m_GeoFile;
		m_VarsFile = Dlg.m_ToolSetupFilesPage.m_VarsFile;
		screen_script_changed = (m_ScreenScriptFile != Dlg.m_ToolSetupFilesPage.m_ScreenScriptFile);
		m_ScreenScriptFile = Dlg.m_ToolSetupFilesPage.m_ScreenScriptFile;
		m_BreakAngle = Dlg.m_ToolSetupTPPage.m_BreakAngle;
		m_CollinearTol = Dlg.m_ToolSetupTPPage.m_CollinearTol;
		m_CornerTol = Dlg.m_ToolSetupTPPage.m_CornerTol;
		m_FacetAngle = Dlg.m_ToolSetupTPPage.m_FacetAngle;
		m_ReverseRZ = Dlg.m_ToolSetupTPPage.m_ReverseRZ;
		m_EnableGamePad = Dlg.m_ToolSetupTPPage.m_EnableGamePad;
		m_ZeroUsingFixtures = Dlg.m_ToolSetupTPPage.m_ZeroUsingFixtures;
		m_ToolLengthImmediately = Dlg.m_ToolSetupTPPage.m_ToolLengthImmediately;
		m_ToolTableDoM6 = Dlg.m_ToolSetupTPPage.m_ToolTableDoM6;
		m_ConfirmExit = Dlg.m_ToolSetupTPPage.m_ConfirmExit;
		m_AllowConcaveCorners = Dlg.m_ToolSetupTPPage.m_AllowConcaveCorners;
		m_ArcsToSegs = Dlg.m_ToolSetupTPPage.m_ArcsToSegs;
		m_DisplayEncoder = Dlg.m_ToolSetupTPPage.m_DisplayEncoder;
		m_DegreesA = Dlg.m_ToolSetupTPPage.m_DegreesA;
		m_DegreesB = Dlg.m_ToolSetupTPPage.m_DegreesB;
		m_DegreesC = Dlg.m_ToolSetupTPPage.m_DegreesC;
		lathe_changed = (m_Lathe != Dlg.m_ToolSetupTPPage.m_Lathe) || (m_XPosFront != Dlg.m_ToolSetupTPPage.m_XPosFront);
		m_Lathe = Dlg.m_ToolSetupTPPage.m_Lathe;
		m_DoRapidsAsFeeds = Dlg.m_ToolSetupTPPage.m_DoRapidsAsFeeds;
		m_DiameterMode = Dlg.m_ToolSetupTPPage.m_DiameterMode;
		m_XPosFront = Dlg.m_ToolSetupTPPage.m_XPosFront;
		slow_jog_changed = (m_JogSlowPercent != Dlg.m_ToolSetupTPPage.m_JogSlowPercent);
		m_JogSlowPercent = Dlg.m_ToolSetupTPPage.m_JogSlowPercent;
		m_HardwareFRORange = Dlg.m_ToolSetupTPPage.m_HardwareFRORange;
		m_MaxRapidFRO = Dlg.m_ToolSetupTPPage.m_MaxRapidFRO;
		m_ArcRadiusTol = Dlg.m_ToolSetupTPPage.m_ArcRadiusTol;
		m_ArcRSmallTol = Dlg.m_ToolSetupTPPage.m_ArcRSmallTol;

		m_JogSpeed[0] = Dlg.m_ToolSetupTPPage.m_JogSpeedX;
		m_JogSpeed[1] = Dlg.m_ToolSetupTPPage.m_JogSpeedY;
		m_JogSpeed[2] = Dlg.m_ToolSetupTPPage.m_JogSpeedZ;
		m_JogSpeed[3] = Dlg.m_ToolSetupTPPage.m_JogSpeedA;
		m_JogSpeed[4] = Dlg.m_ToolSetupTPPage.m_JogSpeedB;
		m_JogSpeed[5] = Dlg.m_ToolSetupTPPage.m_JogSpeedC;
		m_JogSpeed[6] = Dlg.m_ToolSetupTPPage.m_JogSpeedU;
		m_JogSpeed[7] = Dlg.m_ToolSetupTPPage.m_JogSpeedV;
		
		m_Step0 = Dlg.m_ToolSetupTPPage.m_Step0;
		m_Step1 = Dlg.m_ToolSetupTPPage.m_Step1;
		m_Step2 = Dlg.m_ToolSetupTPPage.m_Step2;
		m_Step3 = Dlg.m_ToolSetupTPPage.m_Step3;
		m_Step4 = Dlg.m_ToolSetupTPPage.m_Step4;
		m_Step5 = Dlg.m_ToolSetupTPPage.m_Step5;
	
		m_SpindleType = Dlg.m_ToolSetupTPPage.m_SpindleType;
		m_SpindleAxis = Dlg.m_ToolSetupTPPage.m_SpindleAxis;
		m_SpindleUpdateTime = Dlg.m_ToolSetupTPPage.m_SpindleUpdateTime;
		m_SpindleTau = Dlg.m_ToolSetupTPPage.m_SpindleTau;
		m_SpindleCntsPerRev = Dlg.m_ToolSetupTPPage.m_SpindleCntsPerRev;

		m_TPLookahead = Dlg.m_ToolSetupTPPage.m_TPLookahead;
		m_MaxAccelX = Dlg.m_ToolSetupTPPage.m_MaxAccelX;
		m_MaxAccelY = Dlg.m_ToolSetupTPPage.m_MaxAccelY;
		m_MaxAccelZ = Dlg.m_ToolSetupTPPage.m_MaxAccelZ;
		m_MaxAccelU = Dlg.m_ToolSetupTPPage.m_MaxAccelU;
		m_MaxAccelV = Dlg.m_ToolSetupTPPage.m_MaxAccelV;
		m_MaxAccelA = Dlg.m_ToolSetupTPPage.m_MaxAccelA;
		m_MaxAccelB = Dlg.m_ToolSetupTPPage.m_MaxAccelB;
		m_MaxAccelC = Dlg.m_ToolSetupTPPage.m_MaxAccelC;

		m_RadiusA = Dlg.m_ToolSetupTPPage.m_RadiusA;
		m_RadiusB = Dlg.m_ToolSetupTPPage.m_RadiusB;
		m_RadiusC = Dlg.m_ToolSetupTPPage.m_RadiusC;
		
		m_MaxVelC = Dlg.m_ToolSetupTPPage.m_MaxVelC;
		m_MaxVelB = Dlg.m_ToolSetupTPPage.m_MaxVelB;
		m_MaxVelA = Dlg.m_ToolSetupTPPage.m_MaxVelA;
		m_MaxVelX = Dlg.m_ToolSetupTPPage.m_MaxVelX;
		m_MaxVelY = Dlg.m_ToolSetupTPPage.m_MaxVelY;
		m_MaxVelZ = Dlg.m_ToolSetupTPPage.m_MaxVelZ;
		m_MaxVelU = Dlg.m_ToolSetupTPPage.m_MaxVelU;
		m_MaxVelV = Dlg.m_ToolSetupTPPage.m_MaxVelV;

		m_CountsPerInchC = Dlg.m_ToolSetupTPPage.m_CountsPerInchC;
		m_CountsPerInchB = Dlg.m_ToolSetupTPPage.m_CountsPerInchB;
		m_CountsPerInchA = Dlg.m_ToolSetupTPPage.m_CountsPerInchA;
		m_CountsPerInchX = Dlg.m_ToolSetupTPPage.m_CountsPerInchX;
		m_CountsPerInchY = Dlg.m_ToolSetupTPPage.m_CountsPerInchY;
		m_CountsPerInchZ = Dlg.m_ToolSetupTPPage.m_CountsPerInchZ;
		m_CountsPerInchU = Dlg.m_ToolSetupTPPage.m_CountsPerInchU;
		m_CountsPerInchV = Dlg.m_ToolSetupTPPage.m_CountsPerInchV;

		m_ActuatorLimits = Dlg.m_ToolSetupTPPage.m_ActuatorLimits;
		for (int iact = 0; iact < MAX_TP_ACTUATORS; iact++)
		{
			m_ActScale[iact]    = Dlg.m_ToolSetupTPPage.m_ActScale[iact];
			m_MaxActVel[iact]   = Dlg.m_ToolSetupTPPage.m_MaxActVel[iact];
			m_MaxActAccel[iact] = Dlg.m_ToolSetupTPPage.m_MaxActAccel[iact];
			m_MaxActJerk[iact]  = Dlg.m_ToolSetupTPPage.m_MaxActJerk[iact];
			m_ActDegrees[iact]  = Dlg.m_ToolSetupTPPage.m_ActDegrees[iact];
		}
		m_ThirdOrderTP = Dlg.m_ToolSetupTPPage.m_ThirdOrderTP;
		m_TPLogSegs = Dlg.m_ToolSetupTPPage.m_TPLogSegs;
		m_TPCubicKnots = Dlg.m_ToolSetupTPPage.m_TPCubicKnots;
		m_MaxJerkX = Dlg.m_ToolSetupTPPage.m_MaxJerkX;
		m_MaxJerkY = Dlg.m_ToolSetupTPPage.m_MaxJerkY;
		m_MaxJerkZ = Dlg.m_ToolSetupTPPage.m_MaxJerkZ;
		m_MaxJerkA = Dlg.m_ToolSetupTPPage.m_MaxJerkA;
		m_MaxJerkB = Dlg.m_ToolSetupTPPage.m_MaxJerkB;
		m_MaxJerkC = Dlg.m_ToolSetupTPPage.m_MaxJerkC;
		m_MaxJerkU = Dlg.m_ToolSetupTPPage.m_MaxJerkU;
		m_MaxJerkV = Dlg.m_ToolSetupTPPage.m_MaxJerkV;

		m_Button0 = Dlg.m_ToolSetupButtonsPage.m_Button0;
		m_Button1 = Dlg.m_ToolSetupButtonsPage.m_Button1;
		m_Button2 = Dlg.m_ToolSetupButtonsPage.m_Button2;
		m_Button3 = Dlg.m_ToolSetupButtonsPage.m_Button3;
		m_Button4 = Dlg.m_ToolSetupButtonsPage.m_Button4;
		m_Button5 = Dlg.m_ToolSetupButtonsPage.m_Button5;
		m_Button6 = Dlg.m_ToolSetupButtonsPage.m_Button6;
		m_Button7 = Dlg.m_ToolSetupButtonsPage.m_Button7;
		m_Button8 = Dlg.m_ToolSetupButtonsPage.m_Button8;
		m_Button9 = Dlg.m_ToolSetupButtonsPage.m_Button9;

		// M1 page handles M0-11 and Specials
		src=Dlg.m_ToolSetupM1Page.McodeActions;
		dst=Interpreter->McodeActions;
		memcpy(&dst[0],&src[0],MAX_MCODE_ACTIONS_M1*sizeof(MCODE_ACTION));
		memcpy(&dst[MCODE_ACTIONS_SPECIAL_OFFSET],&src[MAX_MCODE_ACTIONS_M1],MAX_MCODE_ACTIONS_SPECIAL*sizeof(MCODE_ACTION));
		src=Dlg.m_ToolSetupButtonsPage.McodeActions;
		memcpy(&dst[MAX_MCODE_ACTIONS_M1],&src[MAX_MCODE_ACTIONS_M1],MAX_MCODE_ACTIONS_BUTTONS*sizeof(MCODE_ACTION));
		src=Dlg.m_ToolSetupM100Page.McodeActions;
		memcpy(&dst[MAX_MCODE_ACTIONS_M1+MAX_MCODE_ACTIONS_BUTTONS],&src[0],MAX_MCODE_ACTIONS_M100*sizeof(MCODE_ACTION));

		memcpy(m_UserButtonKeys,Dlg.m_ToolSetupButtonsPage.UserButtonKeys,NUSERBUTTONS*sizeof(int));
	
		SetMotionParams();

		Interpreter->CoordMotion->Kinematics->ReadGeoTable(m_GeoFile);
		Interpreter->p_setup->DiameterMode=m_DiameterMode;
		Interpreter->CoordMotion->SetHardwareFRORange(m_HardwareFRORange);

		SetUserButtons();
		SetStepSizes();

		m_ConfigSpindleDirty=true;  // remember to resend parameters
		ActualGViewParent->m_SceneIsDirty=true;

		if (m_DialogFaceInUse != Dlg.m_ToolSetupFilesPage.m_DialogFace || lathe_changed || slow_jog_changed || screen_script_changed  ||
			(Dlg.m_ToolSetupFilesPage.m_DialogFace == CUSTOM_DLG_FACE && Screen.LastLoadedScreen != Dlg.m_ToolSetupFilesPage.m_ScreenScriptFile))
		{
			m_DialogFaceInUse = m_DialogFace = Dlg.m_ToolSetupFilesPage.m_DialogFace;

			if (m_DialogFace == CUSTOM_DLG_FACE && m_ScreenScriptFile == "")
				Screen.DeleteDlgControls();  // start anew

			DestroyWindow();
			Create(IDD_KMOTIONCNC_0_ORIGINAL+m_DialogFace);  // put up the real main window
			m_LastFixtureDisplayed=m_LastToolDisplayed=-1;
		}

		SaveConfig();
	}
	m_LastToolSetupPage = Dlg.LastPageViewed;
	m_LastConfigUnitsMM = Dlg.m_ToolSetupTPPage.ConfigUnitsMM;
}


void CKMotionCNCDlg::OnUpdateRestart(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(CurrentLine[m_Thread]!=0 && !ThreadIsExecuting && !ForceDisableJogKeys);
}

void CKMotionCNCDlg::OnUpdateSingleStep(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!ThreadIsExecuting && !ForceDisableJogKeys);
}

void CKMotionCNCDlg::OnUpdateOpenFile(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!ThreadIsExecuting && !ForceDisableJogKeys);
}

void CKMotionCNCDlg::OnUpdateNew(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!ThreadIsExecuting && !ForceDisableJogKeys);
}

int CKMotionCNCDlg::SetExecutionPoint(int line)
{
	CurrentLine[m_Thread] = line;
	Interpreter->CoordMotion->m_PreviouslyStopped = STOPPED_NONE;
	Interpreter->p_setup->ConcaveDefered = FALSE;
	if (SaveFile(m_Thread,false)) return 1;  // don't force the save
	Interpreter->DoReverseSearch(FileNames[m_Thread],line);
	m_ThreadThatWasOriginallyStopped = m_Thread; // remember what Thread/file we are set to restore 
	return 0;
}

void CKMotionCNCDlg::OnGView() 
{
	CreateDlgOrBringToTop(IDD_GView,&TheFrame->GViewDlg);
}

void CKMotionCNCDlg::OnSimulate()
{
	UpdateData();
	Interpreter->CoordMotion->m_Simulate = (m_Simulate != 0);
}

void CKMotionCNCDlg::OnDoTime()
{
	UpdateData();
	Interpreter->CoordMotion->m_Simulate = (m_DoTime != 0);
}

void CKMotionCNCDlg::OnBlockDelete()
{
	UpdateData();	
}

void CKMotionCNCDlg::OnShowMach() 
{
	UpdateData();	
}

void CKMotionCNCDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CString s;
	CCoordMotion *CM=Interpreter->CoordMotion;
	
	CDlgX::OnVScroll(nSBCode, nPos, pScrollBar);

	int Dlg=pScrollBar->GetDlgCtrlID();

	if (Dlg==IDC_FeedSlider)
	{
		if (m_Rapid==0)  // 0=rapid
		{
			m_FeedRateRapidValue = m_FeedSlider.GetPos();

			if (m_FeedRateRapidValue > m_MaxRapidFRO)
				m_FeedRateRapidValue = m_MaxRapidFRO;

			// round to 2 digits so fits in display
			m_FeedRateRapidValue = ((int)(m_FeedRateRapidValue*100.0 + 0.5))/100.0;

			s.Format(L"%.2f",m_FeedRateRapidValue);

			SetDlgItemText(IDC_FeedRateEdit,s);
			m_FeedSlider.SetPos(m_FeedRateRapidValue);
			CM->SetFeedRateRapidOverride(m_FeedRateRapidValue);
		}
		else
		{
			m_FeedRateValue = m_FeedSlider.GetPos();

			// round to 2 digits so fits in display
			m_FeedRateValue = ((int)(m_FeedRateValue*100.0 + 0.5))/100.0;

			s.Format(L"%.2f",m_FeedRateValue);

			SetDlgItemText(IDC_FeedRateEdit,s);
			
			CM->SetFeedRateOverride(m_FeedRateValue);
		}
	}
	else if (Dlg==IDC_SpindleSlider)
	{

		// if previous Invoke is in progress and not a final message ignore
		if (nSBCode != SB_THUMBPOSITION && Interpreter->m_InvokeThreadID != -1)  return;

		m_SpindleRateValue = m_SpindleSlider.GetPos();

		// round to 2 digits so fits in display
		m_SpindleRateValue = ((int)(m_SpindleRateValue*100.0 + 0.5))/100.0;

		s.Format(L"%.2f",m_SpindleRateValue);

		SetDlgItemText(IDC_SpindleRateEdit,s);
		
		CM->SetSpindleRateOverride(m_SpindleRateValue);
		Interpreter->InvokeAction(10,FALSE);  // resend new Speed
	}
}


void CKMotionCNCDlg::OnFeedRateApply() 
{
	if (!UpdateData()) return;

	if (m_Rapid==0)
	{
		if (m_FeedRateRapidValue > m_MaxRapidFRO)
			m_FeedRateRapidValue = m_MaxRapidFRO;

		if (!UpdateData(FALSE)) return;

		m_FeedSlider.SetPos(m_FeedRateRapidValue);
		Interpreter->CoordMotion->SetFeedRateRapidOverride(m_FeedRateRapidValue);
	}
	else
	{
		m_FeedSlider.SetPos(m_FeedRateValue);
		Interpreter->CoordMotion->SetFeedRateOverride(m_FeedRateValue);
	}
}

void CKMotionCNCDlg::OnSpindleRateApply() 
{
	if (!UpdateData()) return;

	m_SpindleSlider.SetPos(m_SpindleRateValue);

	Interpreter->CoordMotion->SetSpindleRateOverride(m_SpindleRateValue);
	Interpreter->InvokeAction(10,FALSE);  // resend new Speed
}


CKMotionCNCDlg::CKMotionCNCDlg(CWnd* pParent /*=NULL*/)
	: CDlgX(pParent)
{
	ShuttingDownApplication=FALSE;

	hDIB = NULL;
	hBrush = NULL;
	m_pvBackBits = NULL;


	ActualGViewParent = &GViewControlParent;

	m_ColorTraverse.Set(255,0,0);
	m_ColorFeed.Set(0,255,0);
	m_ColorJump.Set(0,0,0);

	// G-code viewer preview + cut progress.  Colours must be entries of the path
	// palette (CPath3d::colorlookup) or they draw white.
	m_ColorPreviewFeed.Set(150,150,150);	// medium gray (UNCUT_FEED_COLOR_INDEX)
	m_ColorPreviewTraverse.Set(200,80,80);	// muted red   (UNCUT_RAPID_COLOR_INDEX)
	m_ColorCutFeed.Set(255,255,255);		// white       (WHITE_COLOR_INDEX)
	m_ColorCutTraverse.Set(255,175,175);	// light red   (CUT_RAPID_COLOR_INDEX)
	m_PreviewRequestThread = -1;
	m_PreviewRunning = false;
	m_PreviewLaunching = false;
	m_PreviewFinishPending = false;
	m_PreviewFinishOk = false;
	m_PreviewThread = -1;
	m_PreviewValid = false;
	m_PreviewFileSize = 0;
	m_PreviewSetupSave = NULL;
	m_ProgressActive = false;
	m_ProgressIndex = 0;
	m_ProgressSeqOffset = 0;
	m_PreviewSeq0 = 0;
	m_PreviewSeqMonotonic = m_PreviewLineMonotonic = false;
	m_ChainValid = false;
	m_ChainLine = m_ChainSeq = 0;
	m_CompleteLine = m_CompleteSeq = 0;
	m_CompleteStopped = false;
	m_PreviewPrevValid = false;
	m_PreviewPrevX = m_PreviewPrevY = m_PreviewPrevZ = 0.0;
	m_PlaybackArmed = m_PlaybackRunning = m_PlaybackPaused = false;
	m_PlaybackEnd = 0;
	m_ToolViewValid = m_ProgressToolMoved = m_ProgressToolStartValid = false;
	m_ToolViewX = m_ToolViewY = m_ToolViewZ = 0.0;
	m_ProgressToolStartX = m_ProgressToolStartY = m_ProgressToolStartZ = 0.0;
	m_ProgressAdvanceWall = 0.0;
	m_PlaybackTime = m_PlaybackWall = 0.0;

	m_GCodeTools = NULL;
	GreenBrush = NULL;

	FirstInitDlg=FirstStartup=true;
	m_PerformPostHaltCommand=false;

	GCodeMutex = new CMutex(FALSE,L"GCodeInterpreter",NULL);
	
	int i;
	for (i=0; i<N_USER_GCODE_FILES; i++)
	{
		ThreadHadError[i]=false;
		GCodeThreadActive[i]=false;
		CurrentLine[i]=0;
	}

	ThreadIsExecuting=false;

	m_ToolFile = _T("");
	m_SetupFile = _T("");
	m_GeoFile = _T("");
	m_VarsFile = _T("");
	m_ScreenScriptFile = _T("");
	m_BreakAngle = 30.0;
	m_CollinearTol = 0.0002;
	m_CornerTol = 0.0002;
	m_FacetAngle = 0.5;
	m_ReverseRZ = false;
	m_EnableGamePad = true;
	m_ZeroUsingFixtures = false;
	m_ConfirmExit = true;
	m_AllowConcaveCorners = false;
	m_ToolLengthImmediately = false;
	m_ToolTableDoM6 = false;
	m_ArcsToSegs = true;
	m_DisplayEncoder = false;
	m_DegreesA = false;
	m_DegreesB = false;
	m_DegreesC = false;
	m_Lathe = false;
	m_DoRapidsAsFeeds = false;
	m_DiameterMode = false;
	m_XPosFront = false;
	m_TPLookahead = 3.0;
	m_RadiusC = 1;
	m_RadiusB = 1;
	m_RadiusA = 1;
	m_MaxAccelC = 0.01;
	m_MaxAccelB = 0.01;
	m_MaxAccelA = 0.01;
	m_MaxAccelX = 0.01;
	m_MaxAccelY = 0.01;
	m_MaxAccelZ = 0.01;
	m_MaxAccelU = 0.01;
	m_MaxAccelV = 0.01;
	m_MaxVelC = 0.1;
	m_MaxVelB = 0.1;
	m_MaxVelA = 0.1;
	m_MaxVelX = 0.1;
	m_MaxVelY = 0.1;
	m_MaxVelZ = 0.1;
	m_MaxVelU = 0.1;
	m_MaxVelV = 0.1;
	m_CountsPerInchC = 100.0;
	m_CountsPerInchB = 100.0;
	m_CountsPerInchA = 100.0;
	m_CountsPerInchX = 100.0;
	m_CountsPerInchY = 100.0;
	m_CountsPerInchZ = 100.0;
	m_CountsPerInchU = 100.0;
	m_CountsPerInchV = 100.0;
	m_ActuatorLimits = FALSE;
	for (i = 0; i < MAX_TP_ACTUATORS; i++)
	{
		m_ActScale[i] = 100.0;
		m_MaxActVel[i] = 1.0;
		m_MaxActAccel[i] = 10.0;
		m_MaxActJerk[i] = 100.0;
		m_ActDegrees[i] = FALSE;
	}
	m_ThirdOrderTP = FALSE;
	m_TPLogSegs = FALSE;
	m_TPCubicKnots = FALSE;
	m_MaxJerkX = m_MaxJerkY = m_MaxJerkZ = m_MaxJerkA = 1.0;
	m_MaxJerkB = m_MaxJerkC = m_MaxJerkU = m_MaxJerkV = 1.0;
	m_Step0 = 0.0001;
	m_Step1 = 0.001;
	m_Step2 = 0.01;
	m_Step3 = 0.1;
	m_Step4 = 1;
	m_Step5 = 10;
	m_DialogFace = m_DialogFaceInUse = 0;
	m_ConfigCheckWord = 0;
	m_ConfigCheckWordVersion=false;
	m_SpindleType = 0;
	m_SpindleAxis = 4;
	m_SpindleUpdateTime=0.1;
	m_SpindleTau = 0.1;
	m_SpindleCntsPerRev = 1000;
	m_SafeZ = 0.0;
	m_SafeRelAbs = 0;
	m_JoyExtvx = m_JoyExtvy = m_JoyExtvz = m_JoyExtva = m_JoyExtvb = m_JoyExtvc = m_JoyExtvu = m_JoyExtvv = 0.0;
	m_SaveFixtureOnOK = TRUE;
	m_EditScreenTimerStarted = m_OffsetTimerStarted = false;

	HWndClient = NULL;

	//{{AFX_DATA_INIT(CKMotionCNCDlg)
	m_Thread = -1;
	m_Rapid = 1;
	m_Simulate = m_DoTime = FALSE;
	m_ShowLineNumbers = FALSE;
	m_LastToolSetupPage = 0;
	m_LastConfigUnitsMM = FALSE;
	m_ShowMach = FALSE;
	m_FeedRateValue = 1.0;
	m_FeedRateRapidValue = 1.0;
	m_SpindleRateValue = 1.0;
	m_CommandString = _T("");
	m_StepSize = 0;
	//}}AFX_DATA_INIT
	

	for (i=0; i<MAX_ACTUATORS; i++)
	{
		CS_axis[i]=-1;
		m_JogSpeed[i]=1;
		m_JogSpeedOverride[i]=1;
	}

	m_JogSlowPercent=25.0;
	m_HardwareFRORange=0.0;
	m_MaxRapidFRO=1.0;
	m_ArcRadiusTol=TOLERANCE_INCH_DEFAULT;
	m_ArcRSmallTol=TOLERANCE_RSMALL_INCH_DEFAULT;

	if (LoadLibrary(L"SciLexer.DLL")==NULL)
	{
		MessageBox(/*TRAN*/TheFrame->KMotionDLL->Translate("The Scintilla DLL could not be loaded."),
		/*TRAN*/TheFrame->KMotionDLL->Translate("Error loading Scintilla"),
		MB_OK | MB_ICONERROR);
	}

	m_hAccelTable=LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));

	board=0;
	m_BoardType = BOARD_TYPE_UNKNOWN;
	ReadStatus=true;
	CurAbsX=CurAbsY=CurAbsZ=CurAbsA=CurAbsB=CurAbsC=0;
	m_Joyvx = m_Joyvy = m_Joyvz = m_Joyva = m_Joyvb = m_Joyvc = 0.0;
	m_JoyMovedx = false;
	m_JoyMovedy = false;
	m_JoyMovedz = false;
	m_JoyMoveda = false;
	m_JoyMovedb = false;
	m_JoyMovedc = false;
	m_JoyMovedu = false;
	m_JoyMovedv = false;
	m_LastFixtureDisplayed=m_LastToolDisplayed=-1;

	m_RestoreStoppedState=false;
	m_SaveStoppedState=0;
	m_BulkStatusCount=0;
	m_ThreadThatWasLaunched=-1;  // special code for no file
	m_ThreadThatWasOriginallyStopped=-2;  // set invalid

	JobStartTimeValid=JobEndTimeValid=JobDoTimeValid=false;

	m_DoingSimulationRun=false;
	m_ConnectedForStatus=false;
	m_PrevABCPlotValid = false;

	for (int i=0; i<NUSERBUTTONS; i++)
		m_UserButtonKeys[i]=-1;

	m_DisplayGViewer=FALSE;

	EnableJogKeys = ForceDisableJogKeys = false;
}

CKMotionCNCDlg::~CKMotionCNCDlg()
{
	if (m_GCodeTools)
		delete m_GCodeTools;

	if (GreenBrush) delete GreenBrush;

	if (m_PreviewSetupSave) delete m_PreviewSetupSave;

	delete m_pvBackBits;

	delete GCodeMutex;
}



void CKMotionCNCDlg::OnDropdownCommand() 
{
	UpdateData(TRUE);
	// Delete every item from the combo box.
	m_Command.ResetAll();
	UpdateData(FALSE);
	
	// add them in backwards skipping blank lines
	
	for (int i=0; i<NCOMMAND_HISTORY; i++)
		if (!CommandHistory[i].IsEmpty())
			m_Command.InsertItem(CommandHistory[i]);

}



void CKMotionCNCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_TOOLSETUP)
	{
		OnToolSetup();
	}
	else if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDlgX::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKMotionCNCDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDlgX::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CKMotionCNCDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}




void CKMotionCNCDlg::OnESC() 
{
	if (AfxMessageBox(L"ABORT?",MB_ICONQUESTION|MB_YESNO)==IDYES)
		OnEmergencyStop();
}

void CKMotionCNCDlg::OnF2() 
{
	m_KeyJogMode.Toggled = !m_KeyJogMode.Toggled;
	m_KeyJogMode.Invalidate();

	CheckDlgButton(IDC_KeyJogMode,(IsDlgButtonChecked(IDC_KeyJogMode)==0));
}

BOOL CKMotionCNCDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (m_hAccelTable) 
	{
		if (::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg)) 
		{
			return(TRUE);
		}
	}

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)  // check for enter key
	{
		if ((GetDlgItem(IDC_Send)->GetStyle() & BS_DEFPUSHBUTTON) == 1) // check if Send Button will get action
		{
			CWnd *Focus = GetFocus();  // check if MDI has focus
			if (Focus)
			{
				CWnd *Parent = Focus->GetParent(); // if MDI CEdit then Parent will be ComboBox
				// check if MDI has Focus or GCode Editor has Focus
				if ((Parent && Parent->GetDlgCtrlID() == IDC_Command) || Focus->GetDlgCtrlID() == IDC_Editor)
					return  CDlgX::PreTranslateMessage(pMsg); // only in this case dispatch ENTER message
			}
			return TRUE;  // don't handle RETURN for any other case and say that it was handled
		}
	}

	if (pMsg->message == WM_MOUSEMOVE)
	{
		CComboBoxScreen *C = Screen.FindComboBoxScreenFromHandle(pMsg->hwnd);
		if (C)
		{
			CString LocalToolTipText;

			if (C->GetID() == IDC_fixture)
			{
				CString s;
				double *d = &m_RealTimeSetup->parameters[5200 + (m_RealTimeSetup->origin_index * 20)];
				LocalToolTipText.Format(L"Offsets X:%.4f Y:%.4f Z:%.4f", d[1], d[2], d[3]);
				if (d[4] != 0) { s.Format(L" A:%.4f", d[4]); LocalToolTipText += s; }
				if (d[5] != 0) { s.Format(L" B:%.4f", d[5]); LocalToolTipText += s; }
				if (d[6] != 0) { s.Format(L" C:%.4f", d[6]); LocalToolTipText += s; }
			}
			else if (C->GetID() == IDC_tool)
			{
				CANON_TOOL_TABLE *T = &m_RealTimeSetup->tool_table[m_RealTimeSetup->selected_tool_slot];

				CString s;

				if (T->Comment.IsEmpty()) { ToolTipText = "Tool"; }
				else { LocalToolTipText = T->Comment; }

				if (T->slot > 0) { s.Format(L" Slot:%d", T->slot); LocalToolTipText += s; }
				if (T->id > 0) { s.Format(L" ID:%d", T->id); LocalToolTipText += s; }
				if (T->length > 0) { s.Format(L" Length:%.4f", T->length); LocalToolTipText += s; }
				if (T->diameter > 0) { s.Format(L" Diam:%.4f", T->diameter); LocalToolTipText += s; }
				if (T->xoffset != 0) { s.Format(L" X offset:%.4f", T->xoffset); LocalToolTipText += s; }
				if (T->yoffset != 0) { s.Format(L" Y offset:%.4f", T->yoffset); LocalToolTipText += s; }
			}
			else
			{
				LocalToolTipText = C->ToolTipText;
			}
		
			C->FixMyComboboxExTip(LocalToolTipText);
		}
	}

	return CDlgX::PreTranslateMessage(pMsg);  // process others normally
}


int CKMotionCNCDlg::DoJoyStick()
{
	JOYINFOEX ji;
	CCoordMotion *CM=Interpreter->CoordMotion;
	unsigned long RtoUse, ZtoUse;
	static bool FirstEmergencyStop=true;
	static bool FirstSingleStep=true;
	static bool FirstHalt = true;
	static bool FirstRun = true;
	static bool FirstFeedRateHigh=true;
	static bool FirstFeedRateLow=true; 
	static CString OrigFeedRate;

	ji.dwSize=sizeof(JOYINFOEX);
	ji.dwFlags=JOY_RETURNALL;

	if (m_EnableGamePad && joyGetPosEx(JOYSTICKID1,&ji) == JOYERR_NOERROR)
	{

		// POV full down causes emergency stop
		if (FirstEmergencyStop && ji.dwPOV != 0xffff)
		{
			FirstEmergencyStop=false;
			OnEmergencyStop();
		}

		if (ji.dwButtons & 0x10) // halt mode ??
		{
			FirstHalt = false;
			OnHalt();
		}

		if ((ji.dwButtons & 0x04) && FirstRun) // Run mode ??
		{
			FirstRun = false;
			OnGO();
		}



		if (ji.dwButtons & 0x10)
			m_JogSpeedFactor=m_JogSlowPercent* 0.01;
		else
			m_JogSpeedFactor=1.00f;


		if (FirstSingleStep && ji.dwButtons & 0x1)
		{
			FirstSingleStep=false;
			OnSingleStep();
		}

		if (!FirstFeedRateHigh && ((ji.dwButtons & 0x8)==0))
		{
			SetDlgItemText(IDC_FeedRateEdit,OrigFeedRate);
			PostMessage(WM_COMMAND,IDC_FeedRateApply);
			FirstFeedRateHigh=true;
		}
		else if (FirstFeedRateHigh && (ji.dwButtons & 0x8))
		{
			FirstFeedRateHigh=false;
			// remember what it was
			GetDlgItemText(IDC_FeedRateEdit,OrigFeedRate);
			SetDlgItemText(IDC_FeedRateEdit,L"2.0");
			PostMessage(WM_COMMAND,IDC_FeedRateApply);
		}

		if (!FirstFeedRateLow && ((ji.dwButtons & 0x2)==0))
		{
			SetDlgItemText(IDC_FeedRateEdit,OrigFeedRate);
			PostMessage(WM_COMMAND,IDC_FeedRateApply);
			FirstFeedRateLow=true;
		}
		else if (FirstFeedRateLow && (ji.dwButtons & 0x2))
		{
			FirstFeedRateLow=false;
			// remember what it was
			GetDlgItemText(IDC_FeedRateEdit,OrigFeedRate);
			SetDlgItemText(IDC_FeedRateEdit,L"0.5");
			PostMessage(WM_COMMAND,IDC_FeedRateApply);
		}

		if (ji.dwPOV == 0xffff && ji.dwButtons == 0)  // all buttons are completely released
		{
			FirstEmergencyStop=true;
			FirstSingleStep=true;
			FirstHalt = true;
			FirstRun = true;
		}

		if (m_ReverseRZ)
		{
			ZtoUse = ji.dwRpos;
			RtoUse = ji.dwZpos;
		}
		else
		{
			RtoUse = ji.dwRpos;
			ZtoUse = ji.dwZpos;
		}

		// rearrange joystick for lathe modes
		if (m_Lathe)
		{
			m_Joyvy = DoJoyAxis(1,0xffff-RtoUse   );  // Y maps to Z Joy
			m_Joyvz = DoJoyAxis(2,       ji.dwXpos);  // Z maps to X Joy

			if (m_XPosFront)
			{
				m_Joyvx = DoJoyAxis(0,ji.dwYpos);   // X Maps to Y Joy Opposite
			}
			else
			{
				m_Joyvx = DoJoyAxis(0,0xffff-ji.dwYpos);  // X Maps to Y Joy Opposite
			}
		}
		else
		{
			// normal case
			m_Joyvx = DoJoyAxis(0,       ji.dwXpos);
			m_Joyvy = DoJoyAxis(1,0xffff-ji.dwYpos);
			m_Joyvz = DoJoyAxis(2,0xffff-RtoUse   );
		}
		
		m_Joyva = DoJoyAxis(3,       ZtoUse   );

		m_Joyvb = m_Joyvc = 0.0;
	}
	else
	{
		// joystick not connected
		//
		// in case of loss of connection stop any jogs

		m_Joyvx = m_Joyvy = m_Joyvz = m_Joyva = m_Joyvb = m_Joyvc = m_Joyvu = m_Joyvv = 0.0;
	}

	// handle external Jog commands

	if (m_Joyvx == 0.0) m_Joyvx = m_JoyExtvx;
	if (m_Joyvy == 0.0) m_Joyvy = m_JoyExtvy;
	if (m_Joyvz == 0.0) m_Joyvz = m_JoyExtvz;
	if (m_Joyva == 0.0) m_Joyva = m_JoyExtva;
	if (m_Joyvb == 0.0) m_Joyvb = m_JoyExtvb;
	if (m_Joyvc == 0.0) m_Joyvc = m_JoyExtvc;
	if (m_Joyvu == 0.0) m_Joyvu = m_JoyExtvu;
	if (m_Joyvv == 0.0) m_Joyvv = m_JoyExtvv;


	if (m_Simulate)
	{
		CM->current_x += m_Joyvx * STATUS_TIME; //Sim for 0.1 sec
		CM->current_y += m_Joyvy * STATUS_TIME; 
		CM->current_z += m_Joyvz * STATUS_TIME; 
		CM->current_a += m_Joyva * STATUS_TIME;
		CM->current_b += m_Joyvb * STATUS_TIME;
		CM->current_c += m_Joyvc * STATUS_TIME;
		CM->current_u += m_Joyvu * STATUS_TIME;
		CM->current_v += m_Joyvv * STATUS_TIME;
	}
	else
	{
		ProcessChangeInJogVelocity();
	}

	return 0;
}

// ----- Coordinated (kinematics-exact) jog streaming ----------------------
//
// Open-loop actuator velocity jogs cannot follow curved actuator paths:
// the direction tangent goes stale at the Windows update rate (at
// A=100 deg/s a 100ms-old tangent misses a 300mm tool point by many mm)
// and independent per-axis accel ramps skew the velocity ratios during
// every change.  Under nonlinear kinematics, jogs are instead STREAMED
// as short coordinated moves: each timer tick dead-reckons the target
// along the commanded CAD ray and appends that stretch through
// StraightFeedAccelRapid (which subdivides through the kinematics
// exactly like G-code feeds).  The controller then executes the exact
// kinematic path at the servo rate:
//  - per-axis velocity/accel limits are honored by the planner
//  - axes not being jogged are held EXACTLY (no feedback needed, no
//    final correction move)
//  - if input pauses, the firmware's buffer-starvation feed-forward
//    ramps TimeBase to zero ON PATH near the data end and ramps back up
//    automatically when appending resumes
//  - releasing the input flushes the tail, which plans a decel to rest
//    exactly at the last streamed point
// The jog uses the LEGACY planner (ThirdOrderTP temporarily off) with a
// short TPLookahead so the stream launches quickly and OutputSegment's
// throttle bounds any overfeed; pacing against wall time keeps roughly
// COORD_JOG_HORIZON seconds buffered so the motion tracks the sticks.

#define COORD_JOG_MINLEN    2e-4  // accumulate very slow jogs until the
                                  // stretch is resolvable (CAD units)

static UINT CoordJogWorkerEntry(LPVOID p)
{
	((CKMotionCNCDlg*)p)->CoordJogWorker();
	return 0;
}

// Probe whether the kinematics is locally NONLINEAR along the commanded
// jog direction: transform the endpoints and midpoint of a FIXED
// CAD-space span - if the midpoint image deviates from the chord by more
// than a fraction of a count the actuator path is curved and velocity
// jogs cannot follow it.  Purely linear machines return false and keep
// the classic jog behavior identically.
// The span must NOT scale with the commanded speed (it originally probed
// v * 4*STATUS_TIME): chord sagitta grows with span squared, so the same
// pose then classified nonlinear at 100% jog speed but linear at Slow
// 25% - the two speeds took entirely different jog mechanisms.
#define COORD_JOG_PROBE_SPAN 0.4   // CAD units, ~ what a 1 unit/s jog
                                   // covers in the classic jog's pacing
bool CKMotionCNCDlg::JogKinematicsNonlinear(double *v)
{
	int i;
	CCoordMotion *CM=Interpreter->CoordMotion;
	double d[8], A0[MAX_ACTUATORS], A1[MAX_ACTUATORS], Am[MAX_ACTUATORS];

	double F=0;
	for (i=0;i<8;i++) F += v[i]*v[i];
	F = sqrt(F);
	if (F < 1e-12) return false;

	for (i=0;i<8;i++) d[i]=v[i]*(COORD_JOG_PROBE_SPAN/F);

	double x=CurAbsX, y=CurAbsY, z=CurAbsZ, a=CurAbsA;
	double b=CurAbsB, c=CurAbsC, u=CurAbsU, vv=CurAbsV;

	if (CM->Kinematics->TransformCADtoActuators(x, y, z, a, b, c, u, vv, A0)) return false;
	if (CM->Kinematics->TransformCADtoActuators(x+d[0], y+d[1], z+d[2], a+d[3],
			b+d[4], c+d[5], u+d[6], vv+d[7], A1)) return false;
	if (CM->Kinematics->TransformCADtoActuators(x+d[0]/2, y+d[1]/2, z+d[2]/2, a+d[3]/2,
			b+d[4]/2, c+d[5]/2, u+d[6]/2, vv+d[7]/2, Am)) return false;

	double err=0, span=0;
	for (i=0;i<MAX_ACTUATORS;i++)
	{
		double e=fabs(Am[i]-0.5*(A0[i]+A1[i]));
		double s=fabs(A1[i]-A0[i]);
		if (e>err) err=e;
		if (s>span) span=s;
	}
	return err > 0.25 && err > 1e-6*span;   // curved by > 1/4 count
}

// UI side of a streaming jog: publish the commanded CAD velocities and
// make sure the worker thread is running.  NEVER blocks - all controller
// communication happens on the worker.
int CKMotionCNCDlg::DoCoordinatedJog(double *v)
{
	int i;

	if (!m_CoordJogCSInit)
	{
		InitializeCriticalSection(&m_CoordJogCS);
		m_CoordJogCSInit = true;
	}

	EnterCriticalSection(&m_CoordJogCS);
	for (i=0;i<8;i++) m_CoordJogV[i]=v[i];
	LeaveCriticalSection(&m_CoordJogCS);

	bool anyv=false;
	for (i=0;i<8;i++) if (v[i]!=0.0) anyv=true;

	static bool s_prevAny = false;   // for release-EDGE detection below

	if (!m_CoordJogActive)
	{
		if (!anyv) { s_prevAny = false; return 0; }

		m_CoordJogActive = true;    // worker clears on exit
		AfxBeginThread(::CoordJogWorkerEntry, this);
	}
	else if (!anyv && s_prevAny)
	{
		// RELEASE EDGE: stop NOW from this (GUI) thread rather than
		// waiting for the worker to notice.  Near a kinematic fold one
		// worker operation (replan over the inflated buffered set + a
		// download chunk) takes 0.2-0.35s, so worker-detected release
		// always lags the button.
		// The flag FIRST, and UNGATED: the worker may be inside a
		// controller-paced DLWAIT - including one entered by
		// OutputSegment's own auto-launch BEFORE the worker ever set
		// m_CoordJogLaunched (near a fold one emission can finalize
		// past the auto-launch threshold in a single call).  Gating
		// the flag on m_CoordJogLaunched left that wait unsupervised:
		// a release during it sent nothing and the board ran the
		// emission excess at full speed until the wait drained.
		Interpreter->CoordMotion->m_JogReleaseRequested = true;

		if (m_CoordJogLaunched)
		{
			// The STOP COMMAND stays gated on the coordinated buffer
			// actually EXECUTING (m_CoordJogLaunched, maintained by the
			// worker): a stop sent to an idle controller parks an
			// independent-stop state that nothing clears - observed as
			// feedhold pinned ON after velocity flickers re-fired the
			// release edge post-teardown.
			// SetFROwRateTemp ramps TimeBase to zero at the PLANNER-limit
			// rate (the content was planned within those limits, so it
			// can stop within them) - the firmware's channel-parameter
			// stop cushion made near-fold stops take ~1s.  It also raises
			// no stop state, so the feedhold indicator never flashes.
			CStringA scmd;
			scmd.Format("SetFROwRateTemp 0 %.3f", m_CoordJogStopTime);
			TheFrame->KMotionDLL->WriteLine(scmd);
		}
	}
	s_prevAny = anyv;
	return 0;
}

// Worker thread: stream the jog as coordinated motion until the input is
// released, then drain and clean up.  Runs the CONFIGURED planner (legacy
// or 3rd Order - StraightFeedAccelRapid routes by MP->ThirdOrderTP) with
// a shortened TPLookahead so the stream launches promptly and stays only
// a fraction of a second ahead of execution.
//
// The wrapper below re-runs the jog pass if the operator pressed a jog
// again while the previous pass was still tearing down: such a press
// only updates m_CoordJogV (the GUI sees m_CoordJogActive and spawns no
// new worker), and was previously swallowed - the jog appeared dead for
// ~0.5s after every stop.
void CKMotionCNCDlg::CoordJogWorker()
{
	for (;;)
	{
		CoordJogWorkerOnce();

		bool again = false;
		EnterCriticalSection(&m_CoordJogCS);
		for (int i=0;i<8;i++) if (m_CoordJogV[i]!=0.0) again=true;
		LeaveCriticalSection(&m_CoordJogCS);
		if (!again || Interpreter->CoordMotion->GetAbort()) break;
		Sleep(50);   // paced: a comms-dead pass cannot hot-loop
	}
	m_CoordJogActive = false;
}

void CKMotionCNCDlg::CoordJogWorkerOnce()
{
	int i, tries;
	CCoordMotion *CM=Interpreter->CoordMotion;
	MOTION_PARAMS *MP=CM->GetMotionParams();
	CStringA response;
	double v[8], target[8];
	double SaveLookahead = MP->TPLookahead;
	bool ok = true;

	// settle guard: take the position reference only at rest (bounded wait)
	for (tries = 0; tries < 300; tries++)
	{
		if (TheFrame->KMotionDLL->WriteLineReadLine("CheckDoneXYZABC",response.GetBufferSetLength(MAX_LINE)))
			{ return; }
		response.ReleaseBuffer();
		if (response != "0")
		{
			if (TheFrame->KMotionDLL->WriteLineReadLine("CheckDoneBuf",response.GetBufferSetLength(MAX_LINE)))
				{ return; }
			response.ReleaseBuffer();
			if (response != "0") break;
		}
		Sleep(10);
	}
	if (tries >= 300) { return; }

	// start the streaming run from the actual machine position
	if (CM->ReadCurAbsPosition(&CM->current_x,&CM->current_y,&CM->current_z,&CM->current_a,
			&CM->current_b,&CM->current_c,&CM->current_u,&CM->current_v,true))
		{ return; }

	target[0]=CM->current_x; target[1]=CM->current_y;
	target[2]=CM->current_z; target[3]=CM->current_a;
	target[4]=CM->current_b; target[5]=CM->current_c;
	target[6]=CM->current_u; target[7]=CM->current_v;

	// Jog horizon: fixed.  The host now declares the buffer's ACTUAL
	// worst-case stop time to the controller (SetStarveTime, emitted
	// automatically by OutputSegment), so the firmware's starvation
	// margin matches the jog's real speed instead of the
	// worst-case-from-max-velocity default - the horizon works for slow
	// AND fast machines.  Sized to ride through a step in the planner's
	// emission margin: the end-fade term is TIME through a fixed arc
	// length, so when nonlinear kinematics slow the actuator path mid-jog
	// the margin can jump by most of a second in one replan and that
	// update emits NOTHING (observed: 1.60 -> 2.35s on a 3Link X jog).
	// With only 0.25s downloaded the controller ran dry before the next
	// update (underflow canary at 0.24s ahead).  Release latency is
	// unaffected - release stops via the FRO ramp, not by draining the
	// buffer; the cost is a slightly longer launch fill.  The DLWAIT cap
	// (lookahead) must stay above the horizon or bursts can never top
	// the cushion up.
	double lookahead = 0.85;
	double horizon = 0.75;

	// The 3rd Order streaming planner withholds its emission margin
	// (decel shadow + jerk filter history + end-fade TIME) from
	// finalization: nothing downloads until MORE than that much content
	// is buffered.  The stream allowance must exceed it or the jog can
	// never emit a single sample - the planner swallows everything
	// streamed, the launch no-ops forever, and the jog does nothing (or
	// crawls at a few percent).  The margin is speed dependent (the end
	// fade is a fixed ARC length, so its TIME grows as the actuator
	// path slows - dramatically near a kinematic fold), so the bound is
	// re-read from the planner's ACTUAL margin every pass below.
	double stream_bound = CM->TP3ActualEmissionMargin() + horizon + 0.5;
	if (stream_bound < 1.5) stream_bound = 1.5;

	MP->TPLookahead = lookahead;
	CM->m_LookaheadOverride = true;   // our own short Lookahead - don't warn
	                                  // about the user's configured value

	CM->ClearAbort();
	CM->ClearHalt();
	CM->m_JogReleaseRequested = false;   // armed by the GUI at release
	CM->SetTPParams();
	m_CoordJogStopTime = CM->TP3PlannedStopTime();   // release ramp time

	double streamed = 0.0;
	DWORD T0 = GetTickCount();
	bool launched = false;

	for (;;)
	{
		EnterCriticalSection(&m_CoordJogCS);
		for (i=0;i<8;i++) v[i]=m_CoordJogV[i];
		LeaveCriticalSection(&m_CoordJogCS);

		bool anyv=false;
		for (i=0;i<8;i++) if (v[i]!=0.0) anyv=true;
		if (!anyv) break;   // released: stop below
		if (CM->GetAbort()) { ok=false; break; }

		// executed time from the controller (negative = not launched or
		// externally stopped)
		double exec = 0.0;
		{
			if (TheFrame->KMotionDLL->WriteLineReadLine("ExecTime",response.GetBufferSetLength(MAX_LINE)))
				{ ok=false; break; }
			response.ReleaseBuffer();
			double e;
			if (sscanf(response, "%lf", &e) == 1)
			{
				if (e >= 0.0) exec = e;
				else if (launched)
				{
					// Negative after launch: either the buffer RAN DRY
					// (executed everything downloaded before the next
					// download burst arrived - the controller finishes
					// and reports not-executing) or an EXTERNAL stop
					// (feedhold, halt).  GetStopState discriminates:
					// 0 = nothing stopped us - re-arm and relaunch once
					// more content is down; nonzero = real stop.
					CStringA ss;
					if (TheFrame->KMotionDLL->WriteLineReadLine("GetStopState",ss.GetBufferSetLength(MAX_LINE)))
						{ ok=false; break; }
					ss.ReleaseBuffer();
					if (ss != "0")
						break;       // real external stop: tear down
					CM->RearmCoordLaunch();  // ran dry: re-arm ExecBuf
					launched = false;
				}
			}
		}

		// Pace on DOWNLOADED time vs EXECUTED time.  The controller can
		// only consume segments the planner has marked Done and the host
		// has downloaded - at fast jog speeds the Done-marking lags the
		// stream end by the decel shadow, so pacing on commanded time vs
		// wall clock underfed the controller (observed as a mid-jog
		// buffer underflow).  Append stretches until the downloaded
		// prefix truly covers the horizon.
		// track the planner's actual margin (grows as the buffered path
		// slows; the initial estimate is only a floor)
		double sb = CM->TP3ActualEmissionMargin() + horizon + 0.5;
		if (sb > stream_bound) stream_bound = sb;

		double elapsed = (DWORD)(GetTickCount()-T0)*1e-3;
		int guard = 0;
		DWORD burst0 = GetTickCount();
		while ((CM->m_TotalDownloadedTime - exec) < horizon
				&& streamed - elapsed < stream_bound  // runaway bound (> planner emission margin)
				&& guard++ < 12)
		{
			// Respond to RELEASE inside the burst: near a kinematic fold
			// the buffered waypoint set is large and each stretch's
			// replan is expensive - a full 12-stretch burst was observed
			// taking ~1 second, during which the button release went
			// unseen and the machine kept following the streamed path.
			// Also cap the burst's wall time for the same reason.
			EnterCriticalSection(&m_CoordJogCS);
			for (i=0;i<8;i++) v[i]=m_CoordJogV[i];
			LeaveCriticalSection(&m_CoordJogCS);
			anyv=false;
			for (i=0;i<8;i++) if (v[i]!=0.0) anyv=true;
			if (!anyv) break;
			if (GetTickCount() - burst0 > 100) break;   // re-check release at least 10x/sec

			double dt = 0.10;
			BOOL pure_angle;
			double F = CM->FeedRateDistance(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],&pure_angle);
			if (F <= 1e-9 || F*dt < COORD_JOG_MINLEN) break;

			double t[8];
			for (i=0;i<8;i++) t[i]=target[i]+v[i]*dt;

			if (CM->StraightFeedAccelRapid(F, 1e99, false, true,
					t[0],t[1],t[2],t[3],t[4],t[5],t[6],t[7], 0, 0))
			{
				// A nonzero return here is USUALLY the DLWAIT release
				// bail surfacing through the in-flight call (the GUI
				// froze the controller and set m_JogReleaseRequested) -
				// that is a clean RELEASE, not a planner failure, and
				// must not SetAbort.
				if (CM->m_JogReleaseRequested)
					break;
				ok=false; break;
			}

			for (i=0;i<8;i++) target[i]=t[i];
			streamed += dt;

			// start execution as soon as anything is downloaded - the
			// auto-launch waits for TPLookahead of PLANNED time, which a
			// slow jog never reaches.  Only mark launched once something
			// HAS downloaded: LaunchIfNotStarted no-ops on an empty
			// buffer (3rd Order planner still inside its emission
			// margin), and treating that as launched made the negative
			// ExecTime that follows look like an external stop - the jog
			// tore down ~40ms after the press without ever moving.
			// Launch only once a real cushion is downloaded: launching on
			// the first knot (20ms) let the controller execute it to
			// completion before the next download burst (~30ms cadence)
			// arrived - it then reported not-executing and the jog tore
			// down after 0.5mm.  0.15s rides through several bursts (and
			// content time accumulates fast - the stream runs well ahead
			// of real time - so the added press-to-motion latency is ms).
			if (CM->m_TotalDownloadedTime > 0.15)
			{
				// Never (re)launch after the release edge: the GUI's
				// SetFROwRateTemp raised no stop state, so a late
				// ExecBuf would SNAP TimeBase straight back to full
				// rate (firmware restores TIMEBASE*LastFRO whenever
				// StoppingState==0) and resume the abandoned buffer.
				if (CM->m_JogReleaseRequested)
					break;
				if (CM->LaunchIfNotStarted()) { ok=false; break; }
				if (!launched)
				{
					launched = true;
					m_CoordJogLaunched = true;   // enables the GUI release stop
				}
			}
		}
		if (!ok) break;
		Sleep(20);
	}

	{
		// RELEASE: feedhold instead of running out the streamed buffer.
		// StopImmediate0's TimeBase ramp is computed by the firmware from
		// the CURRENT axis velocities, so it stops ON PATH in the true
		// decel distance; draining the buffer instead would continue to
		// the end of everything streamed ahead (a horizon's worth of
		// overshoot).  The remaining streamed path is then abandoned.
		bool comms = true;

		// VERIFIED stop.  First attempt: SetFROwRateTemp 0 - ramps
		// TimeBase to rest at the PLANNER-limit rate (fast: the content
		// was planned within those limits) with no stop state raised.
		// Wait for ExecTime to freeze (ramp done) or go negative (buffer
		// finished), detach with StopImmediate2, and CONFIRM the
		// controller reports not-executing - a relaunch ExecBuf could
		// still be in flight from the last streaming burst.  Retries
		// fall back to StopImmediate0 (belt and braces).
		for (int attempt = 0; comms && attempt < 5; attempt++)
		{
			if (attempt == 0)
			{
				CStringA scmd;
				scmd.Format("SetFROwRateTemp 0 %.3f", m_CoordJogStopTime);
				if (TheFrame->KMotionDLL->WriteLine(scmd)) { comms=false; ok=false; break; }
			}
			else
			{
				if (TheFrame->KMotionDLL->WriteLine("StopImmediate0")) { comms=false; ok=false; break; }

				for (tries = 0; tries < 500; tries++)
				{
					if (TheFrame->KMotionDLL->WriteLineReadLine("GetStopState",response.GetBufferSetLength(MAX_LINE)))
						{ comms=false; ok=false; break; }
					response.ReleaseBuffer();
					// 3/4 = stopped (coord/indep); 0 = the stop found nothing
					if (response=="3" || response=="4" || response=="0") break;
					Sleep(10);
				}
				if (!comms) break;
			}

			// wait for the ramp: ExecTime frozen (two equal reads) or
			// negative (buffer ran out first)
			double lastE = -1e9;
			int same = 0;
			for (tries = 0; tries < 100; tries++)
			{
				if (TheFrame->KMotionDLL->WriteLineReadLine("ExecTime",response.GetBufferSetLength(MAX_LINE)))
					{ comms=false; ok=false; break; }
				response.ReleaseBuffer();
				double e1;
				if (sscanf(response, "%lf", &e1) != 1) break;
				if (e1 < 0.0) break;
				if (fabs(e1 - lastE) < 1e-6) { if (++same >= 2) break; }
				else { same = 0; lastE = e1; }
				Sleep(30);
			}
			if (!comms) break;

			// StopImmediate2 detaches the buffer and clears any stop
			// state (an uncleared StopImmediate0 would pin StoppingState
			// and freeze the NEXT ExecBuf at TimeBase 0)
			if (TheFrame->KMotionDLL->WriteLine("StopImmediate2")) { ok=false; break; }

			// let any in-flight ExecBuf land, then confirm not executing
			Sleep(50);
			if (TheFrame->KMotionDLL->WriteLineReadLine("ExecTime",response.GetBufferSetLength(MAX_LINE)))
				{ comms=false; ok=false; break; }
			response.ReleaseBuffer();
			double e2;
			if (sscanf(response, "%lf", &e2) == 1 && e2 < 0.0)
				break;   // confirmed stopped
			// still executing (e.g. an in-flight relaunch ExecBuf beat the
			// stop): loop to the next attempt's StopImmediate0 fallback
		}

		// SETTLE: backstop for a stop state left standing after the
		// attempt loop - e.g. the StopImmediate0 fallback parked
		// StoppingState and its paired StopImmediate2 failed transiently.
		// An uncleared state would pin the feedhold indicator ON with the
		// abandoned buffer still attached (toggling feedhold would then
		// RESUME orphaned content in little lurches), and every following
		// jog pass would die in its at-rest settle guard.  Clear it:
		// StopImmediate2 also detaches the buffer, so nothing is left to
		// resume.
		for (int settle = 0; comms && settle < 6; settle++)
		{
			Sleep(75);
			if (TheFrame->KMotionDLL->WriteLineReadLine("GetStopState",response.GetBufferSetLength(MAX_LINE)))
				{ comms=false; ok=false; break; }
			response.ReleaseBuffer();
			if (response == "0")
				break;
			if (TheFrame->KMotionDLL->WriteLine("StopImmediate2")) { ok=false; break; }
		}

		// reset the host planner and download bookkeeping.  NOT via
		// SetAbort+ClearAbort: raising the global abort flag from this
		// thread invited a concurrent ClearAbort (e.g. a GUI action)
		// into the same TP3 teardown - observed as a double-delete of
		// the streaming planner
		CM->AbandonCoordBuffer();
	}
	if (!ok) CM->SetAbort();   // ClearAbort at next start resets planner state

	MP->TPLookahead = SaveLookahead;
	CM->m_LookaheadOverride = false;  // restored: the user's Lookahead is
	                                  // in effect again and may warn
	CM->SetTPParams();

	// machine is at rest exactly on the commanded path; restart the
	// classic jog bookkeeping from here
	if (ok)
	{
		double ActsDest[MAX_ACTUATORS], ActsVel[MAX_ACTUATORS];
		if (GetCurrentDestsVels(ActsDest, ActsVel, &CurAbsX, &CurAbsY, &CurAbsZ,
				&CurAbsA, &CurAbsB, &CurAbsC, &CurAbsU, &CurAbsV)==0)
		{
			m_Joyx0 = CurAbsX; m_Joyy0 = CurAbsY; m_Joyz0 = CurAbsZ; m_Joya0 = CurAbsA;
			m_Joyb0 = CurAbsB; m_Joyc0 = CurAbsC; m_Joyu0 = CurAbsU; m_Joyv0 = CurAbsV;
		}
	}
	m_JoyMovedx = m_JoyMovedy = m_JoyMovedz = m_JoyMoveda =
	m_JoyMovedb = m_JoyMovedc = m_JoyMovedu = m_JoyMovedv = false;

	CM->m_JogReleaseRequested = false;
	m_CoordJogLaunched = false;   // GUI release stop disabled until next launch
	// m_CoordJogActive is cleared by the CoordJogWorker wrapper - it may
	// re-run this pass for a press that arrived during the teardown.
}

int CKMotionCNCDlg::ProcessChangeInJogVelocity()
{
	int i;
	static bool DoAFinalMove = false;
	static bool WaitingForFinalMove = false;
	CCoordMotion *CM=Interpreter->CoordMotion;
	double ActsDest[MAX_ACTUATORS];
	double ActsVel[MAX_ACTUATORS];
	double v[MAX_ACTUATORS];

	if (DoAFinalMove)
	{
		// check if we came to a stop yet (either from Jog to stop, or from offset)

		CStringA response;
		if (TheFrame->KMotionDLL->WriteLineReadLine("CheckDoneXYZABC",response.GetBufferSetLength(MAX_LINE)))
		{
			DoAFinalMove=false; // clear this on fatal error
			return 1;
		}
		response.ReleaseBuffer();

		if (response=="0") return 0;  // exit if still stopping 

		// if something is disabled, then do a further check to make sure
		// any of the enabled axes are still stopping
		if (response=="-1")
		{	
			int Done;
			if (CM->GetAxisDone(CM->x_axis,&Done)) {CM->SetAbort(); return 1;} else {if (Done==0) return 0;}
			if (CM->GetAxisDone(CM->y_axis,&Done)) {CM->SetAbort(); return 1;} else {if (Done==0) return 0;}
			if (CM->GetAxisDone(CM->z_axis,&Done)) {CM->SetAbort(); return 1;} else {if (Done==0) return 0;}
			if (CM->GetAxisDone(CM->a_axis,&Done)) {CM->SetAbort(); return 1;} else {if (Done==0) return 0;}
			if (CM->GetAxisDone(CM->b_axis,&Done)) {CM->SetAbort(); return 1;} else {if (Done==0) return 0;}
			if (CM->GetAxisDone(CM->c_axis,&Done)) {CM->SetAbort(); return 1;} else {if (Done==0) return 0;}
			if (CM->GetAxisDone(CM->u_axis,&Done)) {CM->SetAbort(); return 1;} else {if (Done==0) return 0;}
			if (CM->GetAxisDone(CM->v_axis,&Done)) {CM->SetAbort(); return 1;} else {if (Done==0) return 0;}
		}


		if (!WaitingForFinalMove)
		{
			// do a final move to either where the axis was originally
			// or to where it currently is depending on whether that
			// axis was ever moved or not

			if (GetCurrentDestsVels(ActsDest, ActsVel, &CurAbsX, &CurAbsY, &CurAbsZ, &CurAbsA, &CurAbsB, &CurAbsC, &CurAbsU, &CurAbsV)) return 1;


			double x = m_JoyMovedx ? CurAbsX : m_Joyx0;
			double y = m_JoyMovedy ? CurAbsY : m_Joyy0;
			double z = m_JoyMovedz ? CurAbsZ : m_Joyz0;
			double a = m_JoyMoveda ? CurAbsA : m_Joya0;
			double b = m_JoyMovedb ? CurAbsB : m_Joyb0;
			double c = m_JoyMovedc ? CurAbsC : m_Joyc0;
			double u = m_JoyMovedu ? CurAbsU : m_Joyu0;
			double v = m_JoyMovedv ? CurAbsV : m_Joyv0;

			double Acts[MAX_ACTUATORS];

			CM->Kinematics->TransformCADtoActuators(x, y, z, a, b, c, u, v, Acts);

			for (int i=0; i< MAX_ACTUATORS; i++)
				if (DoActPosition(i,Acts[i])) return 1;

			m_JoyMovedx = false;
			m_JoyMovedy = false;
			m_JoyMovedz = false;
			m_JoyMoveda = false;
			m_JoyMovedb = false;
			m_JoyMovedc = false;
			m_JoyMovedu = false;
			m_JoyMovedv = false;

			WaitingForFinalMove = true;
			return 0;
		}

		// we are stopped, remember the starting position

		m_Joyx0 = CurAbsX;
		m_Joyy0 = CurAbsY;
		m_Joyz0 = CurAbsZ;
		m_Joya0 = CurAbsA;
		m_Joyb0 = CurAbsB;
		m_Joyc0 = CurAbsC;
		m_Joyu0 = CurAbsU;
		m_Joyv0 = CurAbsV;

		DoAFinalMove = WaitingForFinalMove = false;
	}


	v[0] = m_Joyvx;
	v[1] = m_Joyvy;
	v[2] = m_Joyvz;
	v[3] = m_Joyva;
	v[4] = m_Joyvb;
	v[5] = m_Joyvc;
	v[6] = m_Joyvu;
	v[7] = m_Joyvv;

	m_Right2.PutRawV(v);
	m_Right.PutRawV(v);
	m_Left2.PutRawV(v);
	m_Left.PutRawV(v);

	m_Up2.PutRawV(v);
	m_Up.PutRawV(v);
	m_Down2.PutRawV(v);
	m_Down.PutRawV(v);

	m_Zplus2.PutRawV(v);
	m_Zplus.PutRawV(v);
	m_Zminus2.PutRawV(v);
	m_Zminus.PutRawV(v);

	m_Aplus2.PutRawV(v);
	m_Aplus.PutRawV(v);
	m_Aminus2.PutRawV(v);
	m_Aminus.PutRawV(v);

	m_Bplus2.PutRawV(v);
	m_Bplus.PutRawV(v);
	m_Bminus2.PutRawV(v);
	m_Bminus.PutRawV(v);

	m_Cplus2.PutRawV(v);
	m_Cplus.PutRawV(v);
	m_Cminus2.PutRawV(v);
	m_Cminus.PutRawV(v);

	m_Uplus2.PutRawV(v);
	m_Uplus.PutRawV(v);
	m_Uminus2.PutRawV(v);
	m_Uminus.PutRawV(v);

	m_Vplus2.PutRawV(v);
	m_Vplus.PutRawV(v);
	m_Vminus2.PutRawV(v);
	m_Vminus.PutRawV(v);


	// Nonlinear-kinematics jogging: velocity jogs cannot follow curved
	// actuator paths, so stream the jog as coordinated motion instead
	// (see DoCoordinatedJog above).
	// A machine with ANY derived kinematics class (or an active geo-
	// correction table) ALWAYS jogs coordinated: the local chord probe
	// alone misrouted locally-flat poses to the classic per-channel jog
	// (observed on 3Link mid-workspace - the press silently took the
	// classic path, which follows actuator-space rays instead of the
	// CAD ray and stops at the channel Accel/Jerk decel, ~1s at soft
	// demo settings, instead of the planner-limit stop).  The probe
	// still governs trivial linear kinematics, so classic machines
	// keep classic jog behavior identically.
	{
		CKinematics *Kin = Interpreter->CoordMotion->Kinematics;
		bool KinNontrivial = (typeid(*Kin) != typeid(CKinematics)) || Kin->GeoTableValid;

		if (m_CoordJogActive ||
			((v[0]!=0 || v[1]!=0 || v[2]!=0 || v[3]!=0 ||
			  v[4]!=0 || v[5]!=0 || v[6]!=0 || v[7]!=0)
				&& !ThreadIsExecuting
				&& (KinNontrivial || JogKinematicsNonlinear(v))))
			return DoCoordinatedJog(v);
	}


	// Check if we are to stop

	if (v[0]==0 && v[1]==0 && v[2]==0 && v[3]==0 && v[4] == 0 && v[5] == 0 && v[6] == 0 && v[7] == 0)
	{
		// check if we had been moving

		if (m_JoyMovedx || m_JoyMovedy || m_JoyMovedz || m_JoyMoveda || m_JoyMovedb || m_JoyMovedc || m_JoyMovedu || m_JoyMovedv)
		{
			for (i = 0; i < MAX_ACTUATORS; i++) ActsVel[i] = 0.0;
			DoAllActVelocity(ActsVel);  // Stop all actuators

			// Unless we are in FeedHold (likely due to a limit condition)do a final move
			CStringA response;
			if (TheFrame->KMotionDLL->WriteLineReadLine("GetStopState",response.GetBufferSetLength(MAX_LINE))) return 1;
			response.ReleaseBuffer();
			if (response == '0')
			{
				DoAFinalMove = true;
			}
			else
			{
				m_JoyMovedx = m_JoyMovedy = m_JoyMovedz = m_JoyMoveda = m_JoyMovedb = m_JoyMovedc = m_JoyMovedu = m_JoyMovedv = false;
			}
		}
		else
		{
			// whenever we are stopped, remember the starting position

			m_Joyx0 = CurAbsX;
			m_Joyy0 = CurAbsY;
			m_Joyz0 = CurAbsZ;
			m_Joya0 = CurAbsA;
			m_Joyb0 = CurAbsB;
			m_Joyc0 = CurAbsC;
			m_Joyu0 = CurAbsU;
			m_Joyv0 = CurAbsV;
			m_JoyMovedx = false;
			m_JoyMovedy = false;
			m_JoyMovedz = false;
			m_JoyMoveda = false;
			m_JoyMovedb = false;
			m_JoyMovedc = false;
			m_JoyMovedu = false;
			m_JoyMovedv = false;
		}
	}
	else
	{
		// no we are moving, keep track of which axis ever moved 

		if (v[0] != 0.0) m_JoyMovedx = true;
		if (v[1] != 0.0) m_JoyMovedy = true;
		if (v[2] != 0.0) m_JoyMovedz = true;
		if (v[3] != 0.0) m_JoyMoveda = true;
		if (v[4] != 0.0) m_JoyMovedb = true;
		if (v[5] != 0.0) m_JoyMovedc = true;
		if (v[6] != 0.0) m_JoyMovedu = true;
		if (v[7] != 0.0) m_JoyMovedv = true;

		// if an axis was never moved since the last time we were still
		// then drive it back toward it's original coordinate at a rate
		// such that it should be there by the next timer

		if (!m_JoyMovedx)
			v[0] = (m_Joyx0 - CurAbsX) / STATUS_TIME;

		if (!m_JoyMovedy)
			v[1] = (m_Joyy0 - CurAbsY) / STATUS_TIME;

		if (!m_JoyMovedz)
			v[2] = (m_Joyz0 - CurAbsZ) / STATUS_TIME;

		if (!m_JoyMoveda)
			v[3] = (m_Joya0 - CurAbsA) / STATUS_TIME;

		if (!m_JoyMovedb)
			v[4] = (m_Joyb0 - CurAbsB) / STATUS_TIME;

		if (!m_JoyMovedc)
			v[5] = (m_Joyc0 - CurAbsC) / STATUS_TIME;

		if (!m_JoyMovedu)
			v[6] = (m_Joyu0 - CurAbsU) / STATUS_TIME;

		if (!m_JoyMovedv)
			v[7] = (m_Joyv0 - CurAbsV) / STATUS_TIME;


		double Acts0[MAX_ACTUATORS];
		double Acts1[MAX_ACTUATORS];

		double x0 = CurAbsX;
		double y0 = CurAbsY;
		double z0 = CurAbsZ;
		double a0 = CurAbsA;
		double b0 = CurAbsB;
		double c0 = CurAbsC;
		double u0 = CurAbsU;
		double v0 = CurAbsV;

		double x1 = x0 + v[0] * STATUS_TIME;
		double y1 = y0 + v[1] * STATUS_TIME;
		double z1 = z0 + v[2] * STATUS_TIME;
		double a1 = a0 + v[3] * STATUS_TIME;
		double b1 = b0 + v[4] * STATUS_TIME;
		double c1 = c0 + v[5] * STATUS_TIME;
		double u1 = u0 + v[6] * STATUS_TIME;
		double v1 = v0 + v[7] * STATUS_TIME;

		CM->Kinematics->TransformCADtoActuators(x0, y0, z0, a0, b0, c0, u0, v0, Acts0);
		CM->Kinematics->TransformCADtoActuators(x1, y1, z1, a1, b1, c1, u1, v1, Acts1);

		for (i = 0; i < MAX_ACTUATORS; i++) ActsVel[i] = (Acts1[i] - Acts0[i]) / STATUS_TIME;
		if (DoAllActVelocity(ActsVel)) return 1;  // Move all actuators
	}
	return 0;
}

int CKMotionCNCDlg::DoAllActVelocity(double *V)
{
	CStringA cmds = "";
	for (int i = 0; i < MAX_ACTUATORS; i++)
	{
		if (DoActVelocity(i, V[i], cmds))
		{
			// Error flag all as stopped
			m_JoyMovedx = m_JoyMovedy = m_JoyMovedz = m_JoyMoveda = m_JoyMovedb = m_JoyMovedc = m_JoyMovedu = m_JoyMovedv = false;
			
			m_Right2.m_RawVel=
			m_Right.m_RawVel=
			m_Left2.m_RawVel=
			m_Left.m_RawVel=
			
			m_Up2.m_RawVel=
			m_Up.m_RawVel=
			m_Down2.m_RawVel=
			m_Down.m_RawVel=
			
			m_Zplus2.m_RawVel=
			m_Zplus.m_RawVel=
			m_Zminus2.m_RawVel=
			m_Zminus.m_RawVel=
			
			m_Aplus2.m_RawVel=
			m_Aplus.m_RawVel=
			m_Aminus2.m_RawVel=
			m_Aminus.m_RawVel=
			
			m_Bplus2.m_RawVel=
			m_Bplus.m_RawVel=
			m_Bminus2.m_RawVel=
			m_Bminus.m_RawVel=
			
			m_Cplus2.m_RawVel=
			m_Cplus.m_RawVel=
			m_Cminus2.m_RawVel=
			m_Cminus.m_RawVel=
			
			m_Uplus2.m_RawVel=
			m_Uplus.m_RawVel=
			m_Uminus2.m_RawVel=
			m_Uminus.m_RawVel=
			
			m_Vplus2.m_RawVel=
			m_Vplus.m_RawVel=
			m_Vminus2.m_RawVel=
			m_Vminus.m_RawVel=0.0;
			return 1;
		}
	}

	if (cmds.GetLength() > 0)
	{
		if (cmds[cmds.GetLength() - 1] == ';') cmds = cmds.Left(cmds.GetLength() - 1);  // strip off last ;
		if (TheFrame->KMotionDLL->WriteLine(cmds)) return 1;
	}

	return 0;
}



// unpack hex values from packed response of all destinations and velocities
// 1 double of destination (2 32-bit words)
// 1 float of velocity

int CKMotionCNCDlg::UnpackSingleAxisDestVel(int axis, CString s, double *d, double *v)
{
	if (axis >= 0)
	{
		float x;
		if (swscanf(s.Mid((axis * 3 + 0) * 9, 9), L"%x", ((int *)d) + 0) != 1) return 1;
		if (swscanf(s.Mid((axis * 3 + 1) * 9, 9), L"%x", ((int *)d) + 1) != 1) return 1;
		if (swscanf(s.Mid((axis * 3 + 2) * 9, 9), L"%x", ((int *)&x)) != 1) return 1;
		*v = x;
	}
	else
	{
		*d = *v = 0.0;
	}
	return 0;
}

// Get Current Actuators Raw Destinations and Velocities quickly 
// and also Absolute CAD Units

int CKMotionCNCDlg::GetCurrentDestsVels(double *ActsDest, double *ActsVel, double *CurAbsX, double *CurAbsY, double *CurAbsZ,
 double *CurAbsA, double *CurAbsB, double *CurAbsC, double *CurAbsU, double *CurAbsV)
{
	CCoordMotion *CM = Interpreter->CoordMotion;
	CStringA response;

	if (TheFrame->KMotionDLL->WriteLineReadLine("GetAllDestVelHex", response.GetBuffer(1000))) return 1;
	response.ReleaseBuffer();

	if (UnpackSingleAxisDestVel(CM->x_axis, (CString)response, &ActsDest[0], &ActsVel[0])) return 1;
	if (UnpackSingleAxisDestVel(CM->y_axis, (CString)response, &ActsDest[1], &ActsVel[1])) return 1;
	if (UnpackSingleAxisDestVel(CM->z_axis, (CString)response, &ActsDest[2], &ActsVel[2])) return 1;
	if (UnpackSingleAxisDestVel(CM->a_axis, (CString)response, &ActsDest[3], &ActsVel[3])) return 1;
	if (UnpackSingleAxisDestVel(CM->b_axis, (CString)response, &ActsDest[4], &ActsVel[4])) return 1;
	if (UnpackSingleAxisDestVel(CM->c_axis, (CString)response, &ActsDest[5], &ActsVel[5])) return 1;
	if (UnpackSingleAxisDestVel(CM->u_axis, (CString)response, &ActsDest[6], &ActsVel[6])) return 1;
	if (UnpackSingleAxisDestVel(CM->v_axis, (CString)response, &ActsDest[7], &ActsVel[7])) return 1;

	CM->Kinematics->TransformActuatorstoCAD(ActsDest, CurAbsX, CurAbsY, CurAbsZ, CurAbsA, CurAbsB, CurAbsC, CurAbsU, CurAbsV);
	return 0;
}


// command an actuator to a specified velocity

int CKMotionCNCDlg::DoActVelocity(int i, double v, CStringA &c)
{
	static double LastSpeed[MAX_ACTUATORS]={1e99,1e99,1e99,1e99,1e99,1e99,1e99,1e99};
	CStringA s;

	if (fabs(v) < 1e-35) v = 0.0; // check if velocity is so small as a float it might round to zero 

	if (CS_axis[i]>=0)
	{
		// command a distance (jog) of 2.0 seconds at that speed
		// so in the case we loose communication it will still stop

		if (fabs(v) > 0) 
			s.Format("MoveRelAtVel%d=%.8g %.8g",CS_axis[i],v*2.0,fabs(v));
		else if (LastSpeed[i] != 0)
			s.Format("Jog%d=%f",CS_axis[i],v);

		if (s != "")
		{
			if (c != "") c += ';';  // add separator if not first
			c += s;  // pack all commands into one string to send quickly
		}
	}
	LastSpeed[i]=v;
		
	return 0;
}


// command an actuator to stop

int CKMotionCNCDlg::StopAxis(int i)
{
	CStringA s;
	
	if (CS_axis[i]>=0)
	{
		s.Format("Jog%d=0",CS_axis[i]);

		if (TheFrame->KMotionDLL->WriteLine(s)) return 1;
	}
	return 0;
}

// command an actuator to a specified position

int CKMotionCNCDlg::DoActPosition(int i, double p)
{
	CStringA s;
	
	if (CS_axis[i]>=0)
	{
		// command a move to an abs position

		s.Format("Move%d=%f",CS_axis[i],p);
		if (TheFrame->KMotionDLL->WriteLine(s)) return 1;
	}
	
	return 0;
}


// command an actuator to a specified position exponentially

int CKMotionCNCDlg::DoActPositionExp(int i, double p, double tau)
{
	CStringA s;
	
	if (CS_axis[i]>=0)
	{
		// command a move to an abs position

		s.Format("MoveExp%d=%f %f",CS_axis[i],p,tau);
		if (TheFrame->KMotionDLL->WriteLine(s)) return 1;
	}
	
	return 0;
}


double CKMotionCNCDlg::DoJoyAxis(int axis, int joystick)
{
	double v;

	v = joystick-0x7fff;
	if (ThreadIsExecuting) v=0;
	if (fabs(v) < 1600) v=0;

	v *= m_JogSpeed[axis] * m_JogSpeedOverride[axis] * m_JogSpeedFactor / 32768.0;

	return v;
}




int CKMotionCNCDlg::GetStatus()
{
	int i,result,n;
	CStringA s;
	int *p=(int *)&MainStatus;
	MOTION_PARAMS *MP = &Interpreter->CoordMotion->Kinematics->m_MotionParams;

	int HostStatus=0;

	if (ThreadIsExecuting) HostStatus += HOST_JOB_ACTIVE_BIT;
	if (Interpreter->p_setup->block_delete) HostStatus += HOST_BLOCK_DELETE_BIT;
	if (MP->TCP_Active) HostStatus += HOST_RTCP_ACTIVE_BIT;

	// KMotion is available read the status, include Job Status, and screen changes count
	s.Format("GetStatus %x %x",HostStatus, Screen.EditScreenChangesCount);  
	if (TheFrame->KMotionDLL->WriteLine(s)) return 1;

	n=sizeof(MainStatus)/sizeof(int);

	s.Empty();

	for (i=0; i<n; i++)
	{
		if (s.IsEmpty())
		{
			if (TheFrame->KMotionDLL->ReadLineTimeOut(s.GetBuffer(257),5000))
			{
				return 1;
			}

			s.ReleaseBuffer();

			// change the CRLF at the to a space

			s.Delete(s.GetLength()-2,2);

			s += ' ';
		}

		// get a hex 32 bit int which may really be anything
		
		result = sscanf(s.GetBuffer(0), "%8X", p++);

		if (result!=1)
		{
			return 1;
		}

		if (s.GetLength() >=9)
		{
			s.Delete(0,9);
		}
		else
		{
			return 1;
		}

		// check if first word contains version

		if (i==0)
		{
			if ((p[-1]>>16)==306)  // check for previous version
			{
			}

			if ((p[-1]>>16)==0)
			{
				// probably old version of DSP Code

				// set version to something

				MainStatus.VersionAndSize=0;
				MainStatus.ThreadActive=0;

				p[0] = p[-1];
				p++;
				n=n-2;
			}
			else
			{
				// update number of words to read
				if (n!=(MainStatus.VersionAndSize & 0xffff))
				{
					int result = MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate("Error: Status Record Size mismatch\r\rDisable further status updates?"),
						L"KMotion", MB_ICONSTOP | MB_YESNO);

					if (result == IDYES)
						ReadStatus=false;
				}
			}
		}
	}

	m_BulkStatusCount++;

	return 0;
}

int CKMotionCNCDlg::UpdateScreen(bool KMotionPresent)
{
	int result;
	CStringA s, response;

	if (KMotionPresent)
	{
		// first find out which axis is which

		if (TheFrame->KMotionDLL->WriteLineReadLine("DefineCSEx",response.GetBufferSetLength(MAX_LINE))) return 1;
		response.ReleaseBuffer();
		result=sscanf(response, "%d%d%d%d%d%d%d%d",&CS_axis[0],&CS_axis[1],&CS_axis[2],&CS_axis[3], &CS_axis[4], &CS_axis[5], &CS_axis[6], &CS_axis[7]);
		if (result != 8)  return 1;
	}
	else
	{
		m_ConfigSpindleDirty=true;  // remember we were disconnected
	}
	
	m_RealTimeSetup = Interpreter->GetRealTimeState();

	UpdateProgress();	// G-code viewer: colour the preview up to the executing block

	SetBigValues(&m_PosX, &m_PosY, &m_PosZ, &m_PosA, &m_PosB, &m_PosC, &m_PosU, &m_PosV, KMotionPresent);

	if (m_RealTimeSetup->distance_mode==MODE_ABSOLUTE)
		CheckRadioButton(IDC_Rel,IDC_Abs,IDC_Abs);
	else
		CheckRadioButton(IDC_Rel,IDC_Abs,IDC_Rel);

	if (m_RealTimeSetup->length_units==CANON_UNITS_INCHES)
		CheckRadioButton(IDC_mm,IDC_inch,IDC_inch);
	else if (m_RealTimeSetup->length_units==CANON_UNITS_MM)
		CheckRadioButton(IDC_mm,IDC_inch,IDC_mm);

	if (m_LastToolDisplayed != m_RealTimeSetup->selected_tool_slot)
	{
		m_LastToolDisplayed=m_RealTimeSetup->selected_tool_slot;
		m_tool.SetCurSel(m_LastToolDisplayed);
	}

	if (m_LastFixtureDisplayed != m_RealTimeSetup->origin_index)
	{
		m_LastFixtureDisplayed=m_RealTimeSetup->origin_index;
		m_FixtureOffset.SetCurSel(m_LastFixtureDisplayed-1);
	}

	m_Forward.Style = Momentary;
	m_Reverse.Style = Momentary;
	m_FeedHold.Style = DualButton;

	if (!KMotionPresent) MainStatus.StopImmediateState = 0;  // No FeedHold if not present

	// Handle changes in Feedhold state for normal and Tapping modes (which only stop/start spindle)

	if (Interpreter->CoordMotion->m_TapCycleInProgress && KMotionPresent)
	{
		CStringA r, s;
		MCODE_ACTION *p = &Interpreter->McodeActions[119 - 100 + MCODE_ACTIONS_M100_OFFSET];
		s.Format("GetPersistHex %d", (int)p->dParams[1]);
		if (TheFrame->KMotionDLL->WriteLineReadLine(s, r.GetBufferSetLength(100))) return 1;
		r.ReleaseBuffer();

		if (r == "-2")
		{
			m_FeedHold.DualButtonState = 1;
			m_FeedHold.Invalidate(0);
			m_Forward.ShowWindow(SW_HIDE);
			m_Reverse.ShowWindow(SW_HIDE);
		}
	}
	else if (MainStatus.StopImmediateState > 0 && m_FeedHold.DualButtonState != 1)
	{
		m_FeedHold.DualButtonState = 1;
		m_FeedHold.Invalidate(0);
		m_Forward.ShowWindow(SW_SHOW);
		m_Reverse.ShowWindow(SW_SHOW);
		Interpreter->InvokeAction(ACTION_FEEDHOLD, FALSE);  // Do Special Action
	}
	else if (MainStatus.StopImmediateState == 0 && m_FeedHold.DualButtonState != 0)
	{
		m_FeedHold.DualButtonState = 0;
		m_FeedHold.Invalidate(0);
		m_Forward.ShowWindow(SW_HIDE);
		m_Reverse.ShowWindow(SW_HIDE);
	}

	m_GO.Style = DualButton;

	// a Simulate playback (G-code viewer) shows Halt too, so it can be stopped
	bool ShowHalt = ThreadIsExecuting || m_PlaybackRunning;

	if (ShowHalt && m_GO.DualButtonState != 1)
	{
		m_GO.DualButtonState = 1;
		m_GO.Invalidate(0);
	}
	else if (!ShowHalt && m_GO.DualButtonState != 0)
	{
		m_GO.DualButtonState = 0;
		m_GO.Invalidate(0);
	}

	// Service Screens based on Screen Scripts

	if (m_ConnectedForStatus)
		Screen.ServiceImageButtons();

	return 0;
}


void CKMotionCNCDlg::SetBigValueColor(CDisplay *Disp,int axis, bool KMotionPresent, bool DisplayedEnc, int ChanOverride)
{
	// normally the enable state of the Coordinate System's axis for this DRO
	// determines the color.  A special encoded-Var DRO (see GetEncodedDROValue)
	// instead passes the Controller Axis Channel it displays.
	int Chan = (ChanOverride >= 0) ? ChanOverride : CS_axis[axis];

	if (m_Simulate)
		Disp->SetTextColor(0xFFFFFF);
	else if (KMotionPresent && Chan >= 0 && (MainStatus.Enables & (1<<Chan)))
	{
		if (m_ShowMach)
			Disp->SetTextColor(0x0080FF);
		else
			if (DisplayedEnc)
				Disp->SetTextColor(0xFFC000);
			else
				Disp->SetTextColor(0x00FF00);
	}
	else
		Disp->SetTextColor(0x00FFFF);
}


void CKMotionCNCDlg::SetBigValues(CDisplay *Disp0, CDisplay *Disp1, CDisplay *Disp2, CDisplay *Disp3, CDisplay *Disp4, CDisplay *Disp5, CDisplay *Disp6, CDisplay *Disp7, bool KMotionPresent)
{
	double x,y,z,a,b,c,u,v;
	CString s;
	CCoordMotion *CM=Interpreter->CoordMotion;
	bool DisplayedEnc0, DisplayedEnc1, DisplayedEnc2, DisplayedEnc3, DisplayedEnc4, DisplayedEnc5, DisplayedEnc6, DisplayedEnc7;


	if (KMotionPresent && !m_Simulate)
	{
		// find out which axis is which

		if (CM->GetAxisDefinitions(&CM->x_axis, &CM->y_axis, &CM->z_axis, &CM->a_axis, &CM->b_axis, &CM->c_axis, &CM->u_axis, &CM->v_axis)) { CM->SetAbort(); return; }

		// read and set all axis (if undefined return interpreter)

		double Acts[MAX_ACTUATORS],ActsDest[MAX_ACTUATORS];

		for (int i=0; i<MAX_ACTUATORS; i++) Acts[i]=0.0;

		// Get either the Actuator Position or Dest depending on whether there is
		// a feedback device and the User is requesting to display feedback and
		// also the current Destination regardless.
		if (GetAxisDRO(CM->x_axis, &Acts[0], &ActsDest[0], &DisplayedEnc0))return;
		if (GetAxisDRO(CM->y_axis, &Acts[1], &ActsDest[1], &DisplayedEnc1))return;
		if (GetAxisDRO(CM->z_axis, &Acts[2], &ActsDest[2], &DisplayedEnc2))return;
		if (GetAxisDRO(CM->a_axis, &Acts[3], &ActsDest[3], &DisplayedEnc3))return;
		if (GetAxisDRO(CM->b_axis, &Acts[4], &ActsDest[4], &DisplayedEnc4))return;
		if (GetAxisDRO(CM->c_axis, &Acts[5], &ActsDest[5], &DisplayedEnc5))return;
		if (GetAxisDRO(CM->u_axis, &Acts[6], &ActsDest[6], &DisplayedEnc6))return;
		if (GetAxisDRO(CM->v_axis, &Acts[7], &ActsDest[7], &DisplayedEnc7))return;

		CM->Kinematics->TransformActuatorstoCAD(Acts,&x,&y,&z,&a, &b, &c, &u, &v);
		CM->Kinematics->TransformActuatorstoCAD(ActsDest,&CurAbsX,&CurAbsY,&CurAbsZ,&CurAbsA, &CurAbsB, &CurAbsC, &CurAbsU, &CurAbsV);
	}
	else
	{
		x = Interpreter->CoordMotion->current_x;
		y = Interpreter->CoordMotion->current_y;
		z = Interpreter->CoordMotion->current_z;
		a = Interpreter->CoordMotion->current_a;
		b = Interpreter->CoordMotion->current_b;
		c = Interpreter->CoordMotion->current_c;
		u = Interpreter->CoordMotion->current_u;
		v = Interpreter->CoordMotion->current_v;
		DisplayedEnc0 = DisplayedEnc1 = DisplayedEnc2 = DisplayedEnc3 = DisplayedEnc4 = DisplayedEnc5 = DisplayedEnc6 = DisplayedEnc7 = false;
	}

	// Optional special DRO behavior:  a Screen Control Var of the form -1xxyy
	// makes that DRO display a Controller Axis Channel even though the Axis is
	// not part of the GCode Coordinate System (see GetEncodedDROValue).  The
	// final display value is computed here and substituted after the normal
	// coordinate conversion below.  The encoder flag is overridden first so
	// the DRO colors behave the same as normal DROs.
	static const int DROIDs[8] = { IDC_PosX, IDC_PosY, IDC_PosZ, IDC_PosA, IDC_PosB, IDC_PosC, IDC_PosU, IDC_PosV };
	bool *DisplayedEncPtr[8] = { &DisplayedEnc0, &DisplayedEnc1, &DisplayedEnc2, &DisplayedEnc3, &DisplayedEnc4, &DisplayedEnc5, &DisplayedEnc6, &DisplayedEnc7 };
	double EncodedValue[8];
	int EncodedChan[8];
	bool EncodedDRO[8];
	for (int i = 0; i < 8; i++)
	{
		EncodedChan[i] = -1;
		EncodedDRO[i] = GetEncodedDROValue(DROIDs[i], KMotionPresent, &EncodedValue[i], DisplayedEncPtr[i], &EncodedChan[i]);
	}

	SetBigValueColor(Disp0, 0, KMotionPresent, DisplayedEnc0, EncodedChan[0]);
	SetBigValueColor(Disp1, 1, KMotionPresent, DisplayedEnc1, EncodedChan[1]);
	SetBigValueColor(Disp2, 2, KMotionPresent, DisplayedEnc2, EncodedChan[2]);
	SetBigValueColor(Disp3, 3, KMotionPresent, DisplayedEnc3, EncodedChan[3]);
	SetBigValueColor(Disp4, 4, KMotionPresent, DisplayedEnc4, EncodedChan[4]);
	SetBigValueColor(Disp5, 5, KMotionPresent, DisplayedEnc5, EncodedChan[5]);
	SetBigValueColor(Disp6, 6, KMotionPresent, DisplayedEnc6, EncodedChan[6]);
	SetBigValueColor(Disp7, 7, KMotionPresent, DisplayedEnc7, EncodedChan[7]);

	// display current spindle rate /feed rate

	if (m_BulkStatusCount>1 && m_RealTimeSetup->length_units == prev_length_units)
	{
		double dt = (MainStatus.TimeStamp - PrevMainStatusTimeStamp);

		if (dt>0.0)
		{
			CString s,sF;
			double SpindleSpeedToShow,SpeedToShow;
			double DesiredFeedRate = m_RealTimeSetup->feed_rate;

			// Do Spindle Control Updates
			double DesiredSpindleRate = m_RealTimeSetup->speed;

			if (KMotionPresent && m_ConfigSpindleDirty)
			{
				if (Interpreter->CoordMotion->ConfigSpindle(m_SpindleType, m_SpindleAxis,m_SpindleUpdateTime, m_SpindleTau, m_SpindleCntsPerRev)) return;
				m_ConfigSpindleDirty=false;
			}

			float speed_rps=0;
			if (KMotionPresent && Interpreter->CoordMotion->GetSpindleRPS(speed_rps)) return;

			SpindleSpeedToShow=fabs(speed_rps*60.0);  

			// if in G32 convert RPM to feet/min or Meters/min depending on units
			if (m_RealTimeSetup->spindle_mode == CANON_SPINDLE_CSS)
			{
				double xp,yp,zp,ap,bp,cp,up,vp;
				Interpreter->ConvertAbsoluteToInterpreterCoord(x, y, z, a, b, c, u, v, &xp, &yp, &zp, &ap, &bp, &cp, &up, &vp, m_RealTimeSetup);

				if (m_RealTimeSetup->DiameterMode) xp *= 0.5; // if result is a diameter, convert to radius to calc surface speed.

				if (m_RealTimeSetup->length_units==CANON_UNITS_MM)
					SpindleSpeedToShow *= TWO_PI * fabs(xp) * 0.001; // convert x (radius) to meters and multiply RPM by 2 PI R
				else
					SpindleSpeedToShow *= TWO_PI * fabs(xp) / 12.0; // convert x (radius) to feet and multiply RPM by 2 PI R

				if (!CSS_BitmapValid || !CSS_BitmapDisplayed)
				{
					m_SR.DualButtonState=CSS_BitmapValid=CSS_BitmapDisplayed=true;
					m_SR.Invalidate();
				}
			}
			else
			{
				if (!CSS_BitmapValid || CSS_BitmapDisplayed)
				{
					CSS_BitmapValid=true;
					m_SR.DualButtonState=CSS_BitmapDisplayed=false;
					m_SR.Invalidate();
				}
			}

			s.Format(L"%7.1f",SpindleSpeedToShow);
			sF.Format(L"%7.1f",DesiredSpindleRate);

			if (m_prevSpindleRateLabel != s)
			{
				m_prevSpindleRateLabel=s;
				SetDlgItemText(IDC_SpindleRateLabel,s);
			}

			if (m_prevSpindleRateLabelF != sF)
			{
				m_prevSpindleRateLabelF=sF;
				SetDlgItemText(IDC_SpindleRateCmd,sF);
			}

			if(DesiredSpindleRate!=0)
				m_SpindleSlider.PlotInstant(SpindleSpeedToShow/DesiredSpindleRate);


			// display current feed rate


			double dx = (CM->x_axis >= 0) ? x - PrevDROx : 0.0;
			double dy = (CM->y_axis >= 0) ? y - PrevDROy : 0.0;
			double dz = (CM->z_axis >= 0) ? z - PrevDROz : 0.0;
			double da = (CM->a_axis >= 0) ? a - PrevDROa : 0.0;
			double db = (CM->b_axis >= 0) ? b - PrevDROb : 0.0;
			double dc = (CM->c_axis >= 0) ? c - PrevDROc : 0.0;
			double du = (CM->u_axis >= 0) ? u - PrevDROu : 0.0;
			double dv = (CM->v_axis >= 0) ? v - PrevDROv : 0.0;

			BOOL pure_angle;
			double Dist=CM->FeedRateDistance(dx, dy, dz, da, db, dc, du, dv, &pure_angle);

			if (SpindleSpeedToShow <= 0.0) // make sure things are reasonable
				SpindleSpeedToShow=DesiredSpindleRate;
			if (SpindleSpeedToShow <= 0.0)
				SpindleSpeedToShow=1.0;

			if (prev_length_units == CANON_UNITS_MM)
			{
				if (m_RealTimeSetup->motion_mode == G_32 || m_RealTimeSetup->feed_mode == UNITS_PER_REV)
				{
					SpeedToShow=Dist/dt*60.0*25.4/SpindleSpeedToShow;
					s.Format(L"%7.3f",SpeedToShow);
					sF.Format(L"%7.3f",DesiredFeedRate);
				}
				else
				{
					if (pure_angle)
						SpeedToShow=Dist/dt*60.0;  //units of degrees don't convert to mm
					else
						SpeedToShow=Dist/dt*60.0*25.4;

					s.Format(L"%7.0f",SpeedToShow);
					sF.Format(L"%7.0f",DesiredFeedRate);
				}
			}
			else
			{
				if (m_RealTimeSetup->motion_mode == G_32 || m_RealTimeSetup->feed_mode == UNITS_PER_REV)
				{
					SpeedToShow=Dist/dt*60.0/SpindleSpeedToShow;
					s.Format(L"%7.4f",SpeedToShow);
					sF.Format(L"%7.4f",DesiredFeedRate);
				}
				else
				{
					SpeedToShow=Dist/dt*60.0;
					s.Format(L"%7.1f",SpeedToShow);
					sF.Format(L"%7.1f",DesiredFeedRate);
				}
			}

			if (m_prevFeedRateLabel != s)
			{
				m_prevFeedRateLabel=s;
				SetDlgItemText(IDC_FeedRateLabel,s);
			}

			if (m_prevFeedRateLabelF != sF)
			{
				m_prevFeedRateLabelF=sF;
				SetDlgItemText(IDC_FeedRateCmd,sF);
			}

			if(DesiredFeedRate!=0)
				m_FeedSlider.PlotInstant(SpeedToShow/DesiredFeedRate);


			if (m_RealTimeSetup->motion_mode == G_32 || m_RealTimeSetup->feed_mode == UNITS_PER_REV)
			{
				if (!G32_BitmapValid || !G32_BitmapDisplayed)
				{
					m_FR.DualButtonState=G32_BitmapValid=G32_BitmapDisplayed=true;
					m_FR.Invalidate();
				}
			}
			else
			{
				if (!G32_BitmapValid || G32_BitmapDisplayed)
				{
					G32_BitmapValid=true;
					m_FR.DualButtonState=G32_BitmapDisplayed=false;
					m_FR.Invalidate();
				}
			}
		}
	}

	PrevMainStatusTimeStamp=MainStatus.TimeStamp;
	prev_length_units = m_RealTimeSetup->length_units; 
	PrevDROx = x;
	PrevDROy = y;
	PrevDROz = z;
	PrevDROa = a;
	PrevDROb = b;
	PrevDROc = c;
	PrevDROu = u;
	PrevDROv = v;



	// convert absolute machine's position in inches to 
	// the Interpreter's position that may include offsets
	// and metric units

	if (m_ShowMach)
		Interpreter->ConvertAbsoluteToMachine(x,y,z,a,b,c,u,v,&x,&y,&z,&a,&b,&c,&u,&v);
	else
		Interpreter->ConvertAbsoluteToInterpreterCoord(x,y,z,a,b,c,u,v,&x,&y,&z,&a,&b,&c,&u,&v,m_RealTimeSetup);

	// substitute any special encoded-Var DRO values (already fully converted)
	if (EncodedDRO[0]) x = EncodedValue[0];
	if (EncodedDRO[1]) y = EncodedValue[1];
	if (EncodedDRO[2]) z = EncodedValue[2];
	if (EncodedDRO[3]) a = EncodedValue[3];
	if (EncodedDRO[4]) b = EncodedValue[4];
	if (EncodedDRO[5]) c = EncodedValue[5];
	if (EncodedDRO[6]) u = EncodedValue[6];
	if (EncodedDRO[7]) v = EncodedValue[7];

	CString Format;
	if (m_RealTimeSetup->length_units == CANON_UNITS_MM) Format = ":%10.3f ";
	else Format = ":%10.4f ";

	DoDROText(Disp0, Format, 'X', x);
	DoDROText(Disp1, Format, 'Y', y);
	DoDROText(Disp2, Format, 'Z', z);
	DoDROText(Disp3, Format, 'A', a);
	DoDROText(Disp4, Format, 'B', b);
	DoDROText(Disp5, Format, 'C', c);
	if (m_DialogFaceInUse == CUSTOM_DLG_FACE)
	{
		DoDROText(Disp6, Format, 'U', u);
		DoDROText(Disp7, Format, 'V', v);
	}
}

// Optional special DRO behavior:  Setting a Screen Control's Var to a value
// of the form -1xxyy (-10000 thru -10715) makes that DRO display a Controller
// Axis Channel even though the Axis is not part of the GCode Coordinate
// System.  Example: a grinder where GCode moves XY while the Operator moves Z
// with an external MPG.  xx = GCode axis 0-7 (X,Y,Z,A,B,C,U,V) whose
// Counts/inch resolution, GCode Offsets, and inch/mm units are applied.
// yy = Controller Axis Channel (0-15) to read.  A simple linear conversion is
// used (no Kinematics or Geo Correction).  Commanded Destination vs measured
// (encoder) Position is selected the same as normal DROs (Display Encoders
// option).  Example: Var = -10206 displays Channel 6 as a Z axis.
//
// Returns true (with *value set to the final display value and *DisplayedEnc
// updated) if the Screen Control with this ID has a Var of this form.

bool CKMotionCNCDlg::GetEncodedDROValue(int ID, bool KMotionPresent, double *value, bool *DisplayedEnc, int *Chan)
{
	// only applies to custom Screens - also avoids acting on stale Screen
	// controls after switching back to a standard Dialog Face
	if (m_DialogFaceInUse != CUSTOM_DLG_FACE) return false;

	DLG_CONTROL *Dlg = Screen.FindDlgControl(ID);
	if (!Dlg || Dlg->Var > -10000 || Dlg->Var < -19999) return false;

	int n = -Dlg->Var - 10000;
	int gcode_axis = n / 100;	// xx - GCode axis for resolution/offsets/units
	int chan = n % 100;			// yy - Controller Axis Channel to display
	if (gcode_axis >= 8 || chan >= N_CHANNELS_KOGNA) return false;

	double cnts = 0.0, dest;
	bool enc = false;
	if (KMotionPresent && !m_Simulate)
		if (GetAxisDRO(chan, &cnts, &dest, &enc)) return false;

	MOTION_PARAMS *MP = &Interpreter->CoordMotion->Kinematics->m_MotionParams;
	double cpi;
	switch (gcode_axis)
	{
	case 0: cpi = MP->CountsPerInchX; break;
	case 1: cpi = MP->CountsPerInchY; break;
	case 2: cpi = MP->CountsPerInchZ; break;
	case 3: cpi = MP->CountsPerInchA; break;
	case 4: cpi = MP->CountsPerInchB; break;
	case 5: cpi = MP->CountsPerInchC; break;
	case 6: cpi = MP->CountsPerInchU; break;
	default: cpi = MP->CountsPerInchV; break;
	}

	// place the absolute position (inches) in the GCode axis slot and convert
	// identically to the normal DROs (units, Diameter mode, and Offsets)
	double A[8] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	double G[8];
	if (cpi != 0.0) A[gcode_axis] = cnts / cpi;

	if (m_ShowMach)
		Interpreter->ConvertAbsoluteToMachine(A[0], A[1], A[2], A[3], A[4], A[5], A[6], A[7],
			&G[0], &G[1], &G[2], &G[3], &G[4], &G[5], &G[6], &G[7]);
	else
		Interpreter->ConvertAbsoluteToInterpreterCoord(A[0], A[1], A[2], A[3], A[4], A[5], A[6], A[7],
			&G[0], &G[1], &G[2], &G[3], &G[4], &G[5], &G[6], &G[7], m_RealTimeSetup);

	*value = G[gcode_axis];
	*DisplayedEnc = enc;
	*Chan = chan;
	return true;
}


// handle DRO custom format from Screen editor

void  CKMotionCNCDlg::DoDROText(CDisplay *Disp, CString DefaultFormat, char X, double x)
{
	CString s;
	CString FormatToUse;
	CString F = Disp->GetFormat();

	if (F == "")  // any format specified?
	{
		FormatToUse.Format(L" %c%s", X, DefaultFormat);
	}
	else  // format specified
	{
		int i = Disp->GetFormat().Find(';');
		if (i > 1) // two formats specified for inch & mm?
		{
			if (m_RealTimeSetup->length_units == CANON_UNITS_MM)
				FormatToUse = F.Right(F.GetLength() - i - 1);
			else
				FormatToUse = F.Left(i);
		}
		else // use specified format for both
		{
			FormatToUse = F;
		}
	}
	
	s.Format(FormatToUse, x);
	KillMinusZero(s);
	Disp->SetText(s);
}


int CKMotionCNCDlg::GetAxisDRO(int axis, double *Act, double *Dest, bool *DisplayedEnc)
{
	CCoordMotion *CM=Interpreter->CoordMotion;

	*DisplayedEnc=false;

	if (axis >=0)
	{
		*Dest = MainStatus.Dest[axis];

		if (AxisInputModeNone(axis) || !m_DisplayEncoder)
		{
			*Act = *Dest;  // return both the same
		}
		else
		{
			// Also read the position for display
			*Act = MainStatus.Position[axis];
			*DisplayedEnc=true;
		}
	}
	else
	{
		*Act = *Dest = 0.0;  // return zero for unused actuator
	}
	return 0;
}


bool CKMotionCNCDlg::AxisInputModeNone(int axis)
{
	if (axis < 4)
		return ((MainStatus.InputModes >> (axis*4)) & 0xf) == NO_INPUT_MODE;
	else if (axis < 8)
		return ((MainStatus.InputModes2 >> ((axis-4)*4)) & 0xf) == NO_INPUT_MODE;
	else if (axis < 12)
		return ((MainStatus.InputModes3 >> ((axis-8)*4)) & 0xf) == NO_INPUT_MODE;
	else
		return ((MainStatus.InputModes4 >> ((axis-12)*4)) & 0xf) == NO_INPUT_MODE;
}

// format shows very small negative numbers as -0
// which looks bad, so just change to zero

void CKMotionCNCDlg::KillMinusZero(CString &s)
{
	int i=3;
	int n=s.GetLength();

	while (i<n && s.GetAt(i)==' ') i++;  // skip over leading zeros
	
	if (i >=n || s.GetAt(i) != '-') return;  // if no minus sign, no problen

	int k=i++; // remember where minus sign is

	while (i<n)
	{
		if (s.GetAt(i)!='.' && s.GetAt(i)!='0' && s.GetAt(i)!=' ') return;
		i++;  
	}
	s.SetAt(k,' ');
}




void CKMotionCNCDlg::OnEmergencyStop() 
{
	CStringA s;
	int i;

	StopPlayback();

	for (i=1;i<N_USER_THREADS;i++)   // kill all user programs except Thread #1
	{
		s.Format("Kill %d", i+1);

		if (TheFrame->KMotionDLL->WriteLine(s)) return;
	}

	Interpreter->Halt();
	
	Interpreter->Abort();

	for (i=0;i<GetNChans();i++)             // disable all servo channels
	{
		s.Format("DISABLEAXIS%d",i);
		if (TheFrame->KMotionDLL->WriteLine(s)) return;
	}

	Interpreter->InvokeAction(ACTION_STOP,FALSE);  // Do Special Action
}



void CKMotionCNCDlg::OnZeroAll() 
{
	if (AfxMessageBox(L"Zero All Axes?",MB_YESNO|MB_ICONQUESTION) == IDYES)
	{
		OnZeroX();
		OnZeroY();
		OnZeroZ();
		OnZeroA();
		OnZeroB();
		OnZeroC();
		OnZeroU();
		OnZeroV();
	}
}

void CKMotionCNCDlg::OnZeroX() 
{
	double x, y, z, a, b, c, u, v;
	setup_pointer ps = Interpreter->p_setup;

	if (ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v)) return;
	if (m_ZeroUsingFixtures)
	{
		int Fix = ps->origin_index-1;
		ps->origin_offset_x += x;
		RoundReasonable(ps->origin_offset_x);
		ps->parameters[5221+Fix*20+0]=ps->origin_offset_x;
	}
	else
	{
		ps->axis_offset_x += x;
		RoundReasonable(ps->axis_offset_x);
		ps->parameters[5211]=ps->axis_offset_x;
		Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
	}
}

void CKMotionCNCDlg::OnZeroY() 
{
	double x, y, z, a, b, c, u, v;
	setup_pointer ps = Interpreter->p_setup;

	if (ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v)) return;
	if (m_ZeroUsingFixtures)
	{
		int Fix = ps->origin_index-1;
		ps->origin_offset_y += y;
		RoundReasonable(ps->origin_offset_y);
		ps->parameters[5221+Fix*20+1]=ps->origin_offset_y;
	}
	else
	{
		ps->axis_offset_y += y;
		RoundReasonable(ps->axis_offset_y);
		ps->parameters[5212]=ps->axis_offset_y;
		Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
	}
}

void CKMotionCNCDlg::OnZeroZ() 
{
	double x, y, z, a, b, c, u, v;
	setup_pointer ps = Interpreter->p_setup;

	if (ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v)) return;
	if (m_ZeroUsingFixtures)
	{
		int Fix = ps->origin_index-1;
		ps->origin_offset_z += z;
		RoundReasonable(ps->origin_offset_z);
		ps->parameters[5221+Fix*20+2]=ps->origin_offset_z;
	}
	else
	{
		ps->axis_offset_z += z;
		RoundReasonable(ps->axis_offset_z);
		ps->parameters[5213]=ps->axis_offset_z;
		Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
	}
}

void CKMotionCNCDlg::OnZeroA() 
{
	double x, y, z, a, b, c, u, v;
	setup_pointer ps = Interpreter->p_setup;

	if (ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v)) return;
	if (m_ZeroUsingFixtures)
	{
		int Fix = ps->origin_index-1;
		ps->AA_origin_offset += a;
		RoundReasonable(ps->AA_origin_offset);
		ps->parameters[5221+Fix*20+3]=ps->AA_origin_offset;
	}
	else
	{
		ps->AA_axis_offset += a;
		RoundReasonable(ps->AA_axis_offset);
		ps->parameters[5214]=ps->AA_axis_offset;
		Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
	}
}

void CKMotionCNCDlg::OnZeroB() 
{
	double x, y, z, a, b, c, u, v;
	setup_pointer ps = Interpreter->p_setup;

	if (ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v)) return;
	if (m_ZeroUsingFixtures)
	{
		int Fix = ps->origin_index-1;
		ps->BB_origin_offset += b;
		RoundReasonable(ps->BB_origin_offset);
		ps->parameters[5221+Fix*20+4]=ps->BB_origin_offset;
	}
	else
	{
		ps->BB_axis_offset += b;
		RoundReasonable(ps->BB_axis_offset);
		ps->parameters[5215]=ps->BB_axis_offset;
		Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
	}
}

void CKMotionCNCDlg::OnZeroC()
{
	double x, y, z, a, b, c, u, v;
	setup_pointer ps = Interpreter->p_setup;

	if (ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v)) return;
	if (m_ZeroUsingFixtures)
	{
		int Fix = ps->origin_index - 1;
		ps->CC_origin_offset += c;
		RoundReasonable(ps->CC_origin_offset);
		ps->parameters[5221 + Fix * 20 + 5] = ps->CC_origin_offset;
	}
	else
	{
		ps->CC_axis_offset += c;
		RoundReasonable(ps->CC_axis_offset);
		ps->parameters[5216] = ps->CC_axis_offset;
		Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
	}
}

void CKMotionCNCDlg::OnZeroU()
{
	double x, y, z, a, b, c, u, v;
	setup_pointer ps = Interpreter->p_setup;

	if (ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v)) return;
	if (m_ZeroUsingFixtures)
	{
		int Fix = ps->origin_index - 1;
		ps->UU_origin_offset += u;
		RoundReasonable(ps->UU_origin_offset);
		ps->parameters[5221 + Fix * 20 + 6] = ps->UU_origin_offset;
	}
	else
	{
		ps->UU_axis_offset += u;
		RoundReasonable(ps->UU_axis_offset);
		ps->parameters[5217] = ps->UU_axis_offset;
		Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
	}
}

void CKMotionCNCDlg::OnZeroV()
{
	double x, y, z, a, b, c, u, v;
	setup_pointer ps = Interpreter->p_setup;

	if (ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v)) return;
	if (m_ZeroUsingFixtures)
	{
		int Fix = ps->origin_index - 1;
		ps->VV_origin_offset += v;
		RoundReasonable(ps->VV_origin_offset);
		ps->parameters[5221 + Fix * 20 + 7] = ps->VV_origin_offset;
	}
	else
	{
		ps->VV_axis_offset += v;
		RoundReasonable(ps->VV_axis_offset);
		ps->parameters[5218] = ps->VV_axis_offset;
		Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
	}
}

void CKMotionCNCDlg::MakeSureFileIsntReadOnly(CString FN)
{
	FN=TheFrame->MainPathRoot+FN;
	if (_waccess(FN,00)==0)						// check if file exists
		if (_waccess(FN,06)!=0)					// check if doesn't have r/w
			_wchmod(FN,_S_IREAD | _S_IWRITE);	// make it r/w
}


void CKMotionCNCDlg::OnDropdownfixture() 
{
	// TODO: Add your control notification handler code here
	
}

void CKMotionCNCDlg::OnCloseupfixture() 
{
	int fixture = m_FixtureOffset.GetCurSel()+1;
	m_LastFixtureDisplayed = fixture;
	Interpreter->ChangeFixtureNumber(fixture);
}

void CKMotionCNCDlg::OnCloseuptool() 
{
	if (m_tool.GetCount() > 0)  // empty?
	{
	int tool = m_tool.GetCurSel();
	CString s;

	m_LastToolDisplayed = tool;
	m_tool.GetLBText(tool,s);

	if (s.GetAt(0) == 'I')
		s.Delete(0,3);
	else
		s.Delete(0,5);

	swscanf(s,L"%d",&tool);

	if (m_ToolLengthImmediately)
		s.Format(L"T%dM6H%dG43.4",tool,tool);
	else
		s.Format(L"T%dM6",tool);
	DoGCodeLine(s);
}
}

void CKMotionCNCDlg::OnClose() 
{
	CHiResTimer Timer;
	Timer.Start();

	if (m_ConfirmExit && AfxMessageBox(L"Exit KMotionCNC ??", MB_YESNO | MB_ICONINFORMATION) != IDYES) return;

	if (Interpreter->InvokeAction(ACTION_PROG_EXIT, FALSE))  // Special Command
	{
		MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate("Unable to perform Program Exit Action"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
	}

	// wait for any Actions to complete
	MSG msg;
	while (Interpreter->m_InvokeThreadID != -1 && GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


	ShuttingDownApplication=TRUE;

	// Flag other process thread to terminate

	while (ThreadIsExecuting && Timer.Elapsed_Seconds() < 3.0)
	{
		Interpreter->Abort();
	}


	if (PersistRestored ||  // check if valid parameters were loaded on startup
			MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate("Configuration File was not properly loaded on startup.\r\rWould you like to save the current configuration?"),
				L"KMotion", MB_YESNO | MB_ICONINFORMATION)
					  == IDYES)
	{
		CString File = TheFrame->MainPathRoot + PERSISTANT_FILE;
		
		FILE *f;
		_tfopen_s(&f, File, _T("wt,ccs=UTF-8"));

		if (f)
		{
			fwprintf(f,L"Version 6\n");

			// be nice and remember what directory we were last time

			fwprintf(f,L"%ls\n",CurrentDirectory.GetBuffer());
			fwprintf(f,L"%d\n",m_ShowLineNumbers);
			fwprintf(f,L"%d %d\n",m_LastToolSetupPage, m_LastConfigUnitsMM);

			// sequence through all the dialogs and give them
			// a chance to save their state

			SaveOnExit(f);
			TheFrame->GViewDlg.SaveOnExit(f);

			fclose(f);
		}
	}


	CDlgX::OnClose();

	AfxGetApp()->GetMainWnd()->PostMessage(WM_CLOSE);
}

void CKMotionCNCDlg::FillComboWithTools(CComboBoxScreen *Box)
{
	CEditToolFile EditToolFile;
	CString s;

	Box->ResetContent();

	if (EditToolFile.LoadFile(m_ToolFile))
	{
		Box->InsertItem("");
		return;
	}

	CString Comment,Image;
	int ID,Pocket;
	double Diameter,Length,Xoffset,Yoffset,FeedTime,FeedDist;

	for (int i=0; i<MAX_TOOLS-1; i++)
	{
		EditToolFile.GetTool(i,Pocket,ID,Length,Diameter,Xoffset,Yoffset,FeedTime,FeedDist,Comment,Image);
		if (ID>0)
		{
			s.Format(L"ID %d",ID);
			Box->InsertItem(s);
		}
		else if (Pocket)
		{
			s.Format(L"Slot %d",Pocket);
			Box->InsertItem(s);
		}
	}
}

void CKMotionCNCDlg::FillComboWithCountFixture(int i0, int i1, CComboBoxScreen *Box)
{
	CString s;

	Box->ResetAll();

	for (int i=i0; i<=i1; i++)
	{
		if (i<7)
			s.Format(L"%d - G%d",i,53+i);
		else
			s.Format(L"%d - G%.1f",i,59+0.1*(i-6));

		Box->InsertItem(s);
	}
}

void CKMotionCNCDlg::OnSend() 
{
	if (!UpdateData()) return;

	if (GetFocus()->GetDlgCtrlID() != IDC_Send &&
		GetFocus()->GetParent()->GetDlgCtrlID() != IDC_Command)
		return;

	// if the MDI control or the Send Button aren't visible then don't do anything
	if (!GetDlgItem(IDC_Command)->IsWindowVisible()) return;
	if (!GetDlgItem(IDC_Send)->IsWindowVisible()) return;

	// select the text in the MDI so Typing will erase it
	((CComboBoxScreen *)GetDlgItem(IDC_Command))->GetEditCtrl()->SetFocus();
	((CComboBoxScreen *)GetDlgItem(IDC_Command))->GetEditCtrl()->SetSel(0,-1);

	if (DoGCodeLine(m_CommandString)) return;

	// Save last few enterd commands

	// first check for a duplicate

	int i;
	for (i=0; i<NCOMMAND_HISTORY-1; i++)
		if (CommandHistory[i] == m_CommandString) break;

	// push the others down over where it is (or to the last)

	for (int k=i; k>0; k--)
		CommandHistory[k] = CommandHistory[k-1];

	CommandHistory[0] = m_CommandString;

}

int CKMotionCNCDlg::DoGCodeLine(CString G) 
{
	CString File = TheFrame->MainPathRoot + TEMP_GCODE_FILE;


	int i=0;

	while (ThreadIsExecuting)
	{
		Sleep(10);
		if (i++ > 1000)
		{
			MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate("Error - Interpreter Busy"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
			return 1;
		}
	}
	
	FILE *f;
	_tfopen_s(&f, File, _T("wt,ccs=UTF-8"));
		
	if (!f)
	{
		MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate("Unable to open Temporary File:\r\r") +  File, L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
		return 1;
	}

	G += '\n';
	fputws(G, f);

	fclose(f);

	m_ThreadThatWasLaunched=-1;  // special code for no file

	// don't disturb the state of GCode Stopping for misc commands
	m_SaveStoppedState = Interpreter->CoordMotion->m_PreviouslyStopped;
	m_RestoreStoppedState=true;
	Interpreter->CoordMotion->m_PreviouslyStopped=0;

	// if we aren't connected then just do a simulation
	if (!m_ConnectedForStatus)
	{
		m_Simulate=m_DoingSimulationRun=true;
	}
	
	if (LaunchExecution(File,0,0)) return 1;

	return 0;
}


void CKMotionCNCDlg::CreateDlgOrBringToTop(UINT ID, CDialogEx *Dlg)
{
	int result;

    // create the dialog, or if it already exists, bring to top

	if (Dlg->m_hWnd)
	{
		Dlg->BringWindowToTop();
		// restore the window if it is minimized
		WINDOWPLACEMENT wndplace;
		if (Dlg->GetWindowPlacement(&wndplace))
		{
			if (SW_SHOWMINIMIZED==wndplace.showCmd)
				Dlg->ShowWindow(SW_RESTORE);
		}
	}
	else
		result = Dlg->Create(ID,TheFrame);

}

void CKMotionCNCDlg::setMainPathAndRoot(LPWSTR arg0)
{
	TheFrame->MainPath = arg0;
	TheFrame->MainPath.Replace(L"\"", L"");  // remove quotes
	TheFrame->MainPath.TrimRight();
	TheFrame->MainPath.TrimLeft();

	int LastSlash = TheFrame->MainPath.ReverseFind('\\');
	TheFrame->MainPathDLL = TheFrame->MainPathDLL64 = TheFrame->MainPath.Left(LastSlash);

	// Check if we are running from a 64bit directory directory
	// if we are, then strip it off

	if (TheFrame->MainPathDLL.Right(2).CompareNoCase(L"64") == 0)
	{
		TheFrame->MainPathDLL = TheFrame->MainPathDLL.Left(TheFrame->MainPathDLL.GetLength() - 2);
	}

	TheFrame->MainPath = TheFrame->MainPathDLL;

	// Check if we are running from the debug directory
	// if we are, then strip it off

	if (TheFrame->MainPath.Right(6).CompareNoCase(L"\\debug") == 0)
	{
		TheFrame->MainPath = TheFrame->MainPath.Left(TheFrame->MainPath.GetLength() - 6);
	}

	// Check if we are running from the release directory
	// if we are, then strip it off

	if (TheFrame->MainPath.Right(8).CompareNoCase(L"\\release") == 0)
	{
		TheFrame->MainPath = TheFrame->MainPath.Left(TheFrame->MainPath.GetLength() - 8);
	}

	// Now set the root install directory

	if (TheFrame->MainPath.Right(8).CompareNoCase(L"\\KMotion") == 0)
	{
		TheFrame->MainPathRoot = TheFrame->MainPath.Left(TheFrame->MainPath.GetLength() - 8);
	}


}


int CKMotionCNCDlg::ExternalRestore(void)
{
	// Save for everybody what directory we are installed in
	CString param;
	int nParams,NewBoard;
	LPWSTR *CL= CommandLineToArgvW(GetCommandLineW(),&nParams);
	wchar_t full[_MAX_PATH];
	_wfullpath(full, CL[0], _MAX_PATH);  // remove relative paths that VS sometimes puts in command line 
	setMainPathAndRoot(full);
	// Set the config file and backup member vars to the defaults. We'll override that if we find a config file arg in the command line
	TheFrame->config_file = DEFAULT_CONFIG_FILE;
	TheFrame->config_file_backup = DEFAULT_CONFIG_FILE_BACKUP;

	// Parse the command line args. 
	// Command line args are of the form /<char> <value> pairs except for the special case of the USB location identifier for using multiple boards. This arg looks like 0x<value>
	// Embedded blanks are allowed in the file name if it is enclosed in quotes
	int next_param = 1;
	while (next_param < nParams)
	{
		param = CL[next_param];
		// do we have key/value pair or a USB id
				//switch on the key type
		if (param == "/no_ethernet" || param == "-no_ethernet") //config file name
		{
			TheFrame->KMotionDLL->NoEthernet = true;
			// increment the parameter count by one
			next_param += 1;
		}
		else if (param.Left(1) == "/" || param.Left(1) == "-")
			//it's a key/value pair
		{
			// make sure there is a param value to fetch
			if (next_param >= nParams - 1)
			{
				// parameter is missing
				AfxMessageBox(L"Command line parameter is missing. Rest of command line will be ignored", MB_OK);
				break;
			}
			else
			{
				if (param == "/c" || param == "-c") //config file name
				{
						TheFrame->config_file = CString("\\KMotion\\Data\\") + CL[next_param + 1];
					TheFrame->config_file_backup = TheFrame->config_file + ".bak";
				}
				else if (param == "/i" || param == "-i") //config file name
				{
					param = CL[next_param + 1];

					// Parse Address
					int IP[4];
					if (swscanf(param, L"%d.%d.%d.%d", &IP[0], &IP[1], &IP[2], &IP[3]) == 4)
					{
						NewBoard = (IP[0] << 24) | (IP[1] << 16) | (IP[2] << 8) | IP[3];
					}
					else
					{
						MessageBox(/*TRAN*/TheFrame->KMotionDLL->Translate("Invalid IP Address on Command Line \r\r") + param,
							L"KMotion", MB_ICONSTOP | MB_OK | MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL);
						NewBoard = 0;
					}
					board = NewBoard;

					delete TheFrame->KMotionDLL;

					TheFrame->KMotionDLL = new CKMotionDLL(board);
					Interpreter->CoordMotion->KMotionDLL = TheFrame->KMotionDLL;
				}
				// put additional tests for flags here as else if cases

				// we drop through here if the flag is not recognized
				else
				{
					AfxMessageBox(L"Unrecognized option on command line. Rest of command line will be ignored", MB_OK);
					break;
				}

				// increment the parameter count by two
				next_param += 2;

			}

		}
		else if (param.Left(2) == "0x" || param.Left(2) == "0X")
			// USB Board id
			{
				param.Delete(0, 2);
				swscanf(param, L"%x", &NewBoard);
				board = NewBoard;
				next_param += 1;

				delete TheFrame->GCodeDlg.Interpreter->CoordMotion->KMotionDLL;
				delete TheFrame->GCodeDlg.Interpreter->CoordMotion;
				delete TheFrame->GCodeDlg.Interpreter;

				TheFrame->KMotionDLL = new CKMotionDLL(board);
				CCoordMotion *CM = new CCoordMotion(TheFrame->KMotionDLL);
				TheFrame->GCodeDlg.Interpreter = new CGCodeInterpreter(CM);
			}
		else // default error case
		{
			AfxMessageBox(L"Unrecognized text on command line. Rest of command line will be ignored");
			break;
		}
	}
	
	GlobalFree(CL);

	PersistRestored=false;


	
	MakeSureFileIsntReadOnly(TheFrame->config_file);
	MakeSureFileIsntReadOnly(/*TRAN*/"\\KMotion\\Data\\GFilesCNC.txt");
	MakeSureFileIsntReadOnly("\\KMotion\\Data\\GViewer.txt");
	MakeSureFileIsntReadOnly("\\KMotion\\Data\\persistCNC.ini");
	

	// be nice and put the current directory back to where
	// it was last time, also all the window positions 

	CString File = TheFrame->MainPathRoot + PERSISTANT_FILE;

	FILE *f;
	_tfopen_s(&f, File, _T("rt,ccs=UTF-8"));

	
	if (f)
	{
		CString Version;
		fgetws(Version.GetBufferSetLength(100),100,f);
		Version.ReleaseBuffer();

		if (Version=="Version 3\n" || Version=="Version 4\n" || Version=="Version 5\n" || Version == "Version 6\n")  // don't attempt to read if incompatable
		{
			fgetws(CurrentDirectory.GetBufferSetLength(MAX_PATH),MAX_PATH,f);
			CurrentDirectory.ReleaseBuffer();
			SetCurrentDirectory(CurrentDirectory);

			if (Version == "Version 4\n" || Version == "Version 5\n" || Version == "Version 6\n")  // Version 4 adds Line number option
			{
				fwscanf(f,L"%d",&m_ShowLineNumbers);
			}

			if (Version=="Version 5\n" || Version == "Version 6\n")  // Version 5 adds Checkword in Config file
			{
				m_ConfigCheckWordVersion=true;
			}
				
			if (Version == "Version 6\n")  // Version 6 restores Tool Setup Page
			{
				int result = fwscanf(f, L"%d %d", &m_LastToolSetupPage, &m_LastConfigUnitsMM);
				if (result < 2) m_LastConfigUnitsMM = FALSE;
			}

			if (m_ShowLineNumbers!=0 && m_ShowLineNumbers!=1) m_ShowLineNumbers=0;

			RestoreOnStart(f);
			TheFrame->GViewDlg.RestoreOnStart(f);

		}

		fclose(f);
	}
	else
	{
		if (MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate("Previous Configuration File:\r\r") +  File +
			          /*TRAN*/TheFrame->KMotionDLL->Translate("\r\rcould not be read.  Continuing will cause a loss of all\rsettings.  Are you sure you would like to continue?"),
			L"KMotion", MB_YESNO | MB_ICONSTOP)
					  != IDYES)
			exit(0);
	}

	// now that all the parameters are loaded - Initialize the Interpreter
	SetMotionParams();
	Interpreter->InitializeInterp();
	return 0;
}



int CKMotionCNCDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDlgX::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}


void CKMotionCNCDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	if (!InitDialogComplete)
	{
		InitDialogComplete=TRUE;
		SetWindowPos(NULL,LastMoveX,LastMoveY,0,0,SWP_NOZORDER|SWP_NOSIZE);
	}
	CDlgX::OnShowWindow(bShow, nStatus);
}





void CKMotionCNCDlg::OnBut0() 
{
	Interpreter->InvokeAction(11,FALSE);
}

void CKMotionCNCDlg::OnBut1() 
{
	Interpreter->InvokeAction(12,FALSE);
}

void CKMotionCNCDlg::OnBut2() 
{
	Interpreter->InvokeAction(13,FALSE);
}

void CKMotionCNCDlg::OnBut3() 
{
	Interpreter->InvokeAction(14,FALSE);
}

void CKMotionCNCDlg::OnBut4() 
{
	Interpreter->InvokeAction(15,FALSE);
}

void CKMotionCNCDlg::OnBut5() 
{
	Interpreter->InvokeAction(16,FALSE);
}

void CKMotionCNCDlg::OnBut6() 
{
	Interpreter->InvokeAction(17,FALSE);
}

void CKMotionCNCDlg::OnBut7() 
{
	Interpreter->InvokeAction(18,FALSE);
}

void CKMotionCNCDlg::OnBut8() 
{
	Interpreter->InvokeAction(19,FALSE);
}

void CKMotionCNCDlg::OnBut9() 
{
	Interpreter->InvokeAction(20,FALSE);
}

void CKMotionCNCDlg::SetUserButtons()
{
	SetAUserButton(IDC_But0,m_Button0);
	SetAUserButton(IDC_But1,m_Button1);
	SetAUserButton(IDC_But2,m_Button2);
	SetAUserButton(IDC_But3,m_Button3);
	SetAUserButton(IDC_But4,m_Button4);
	SetAUserButton(IDC_But5,m_Button5);
	SetAUserButton(IDC_But6,m_Button6);
	SetAUserButton(IDC_But7,m_Button7);
	SetAUserButton(IDC_But8,m_Button8);
	SetAUserButton(IDC_But9,m_Button9);
}


void CKMotionCNCDlg::SetAUserButton(int ID, CString s)
{
	if (s.IsEmpty())
		GetDlgItem(ID)->ShowWindow(SW_HIDE);
	else
		GetDlgItem(ID)->ShowWindow(SW_SHOW);

	m_UserImageBut[ID - IDC_But0].m_HotKey = m_UserButtonKeys[ID - IDC_But0];

	SetDlgItemText(ID,s);
}

void CKMotionCNCDlg::OnMeasure() 
{
	Interpreter->CoordMotion->MeasurePointAppendToFile(TheFrame->MainPathRoot + "\\KMotion\\Data\\Measurements.txt");
}

void CKMotionCNCDlg::OnStep0() 
{
	UpdateData();
}

void CKMotionCNCDlg::OnStep1() 
{
	UpdateData();
}

void CKMotionCNCDlg::OnStep2() 
{
	UpdateData();
}

void CKMotionCNCDlg::OnStep3() 
{
	UpdateData();
}

void CKMotionCNCDlg::OnStep4() 
{
	UpdateData();
}

void CKMotionCNCDlg::OnStep5() 
{
	UpdateData();
}

int CKMotionCNCDlg::ReadInterpPos(double *x, double *y, double *z, double *a, double *b, double *c, double *u, double *v)
{
	bool KMotionPresent = TheFrame->KMotionDLL->WaitToken(false,100.0,"KMCNCReadInterp") == KMOTION_LOCKED;

	if (KMotionPresent)	TheFrame->KMotionDLL->ReleaseToken();

	if (KMotionPresent && !m_Simulate)
	{
		if (Interpreter->CoordMotion->ReadCurAbsPosition(x,y,z,a,b,c,u,v)) return 1;
	}
	else
	{
		*x = Interpreter->CoordMotion->current_x;
		*y = Interpreter->CoordMotion->current_y;
		*z = Interpreter->CoordMotion->current_z;
		*a = Interpreter->CoordMotion->current_a;
		*b = Interpreter->CoordMotion->current_b;
		*c = Interpreter->CoordMotion->current_c;
		*u = Interpreter->CoordMotion->current_u;
		*v = Interpreter->CoordMotion->current_v;
	}

	// convert absolute machine's position in inches to 
	// the Interpreter's position that may include offsets
	// and metric units

	Interpreter->ConvertAbsoluteToInterpreterCoord(*x, *y, *z, *a, *b, *c, *u, *v, x, y, z, a, b, c, u, v, m_RealTimeSetup);

	return 0;
}

void CKMotionCNCDlg::OnSetX() 
{
	static CSetValue SetDlg(IDD_SetValueHalf);
	double x, y, z, a, b, c, u, v;

	DisableKeyJog();
	if (ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v)) return;

	SetDlg.halfDRO = x / 2.0;

	if (SetDlg.DoModal() == IDOK)
	{
		setup_pointer ps = Interpreter->p_setup;

		if (m_ZeroUsingFixtures)
		{
			int Fix = ps->origin_index-1;
			ps->origin_offset_x += x - SetDlg.m_Value;
			RoundReasonable(ps->origin_offset_x);
			ps->parameters[5221+Fix*20+0]=ps->origin_offset_x;
		}
		else
		{
			ps->axis_offset_x += x - SetDlg.m_Value;
			RoundReasonable(ps->axis_offset_x);
			ps->parameters[5211]=ps->axis_offset_x;
			Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
		}
	}
}

void CKMotionCNCDlg::OnSetY() 
{
	static CSetValue SetDlg(IDD_SetValueHalf);
	double x, y, z, a, b, c, u, v;

	DisableKeyJog();
	if (ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v)) return;

	SetDlg.halfDRO = y / 2.0;

	if (SetDlg.DoModal() == IDOK)
	{
		setup_pointer ps = Interpreter->p_setup;

		if (m_ZeroUsingFixtures)
		{
			int Fix = ps->origin_index-1;
			ps->origin_offset_y += y - SetDlg.m_Value;
			RoundReasonable(ps->origin_offset_y);
			ps->parameters[5221+Fix*20+1]=ps->origin_offset_y;
		}
		else
		{
			ps->axis_offset_y += y - SetDlg.m_Value;
			RoundReasonable(ps->axis_offset_y);
			ps->parameters[5212]=ps->axis_offset_y;
			Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
		}
	}
}


void CKMotionCNCDlg::OnSetZ() 
{
	static CSetValue SetDlg(IDD_SetValueHalf);
	double x, y, z, a, b, c, u, v;

	DisableKeyJog();
	if (ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v)) return;

	SetDlg.halfDRO = z / 2.0;

	if (SetDlg.DoModal() == IDOK)
	{
		setup_pointer ps = Interpreter->p_setup;

		if (m_ZeroUsingFixtures)
		{
			int Fix = ps->origin_index-1;
			ps->origin_offset_z += z - SetDlg.m_Value;
			RoundReasonable(ps->origin_offset_z);
			ps->parameters[5221+Fix*20+2]=ps->origin_offset_z;
		}
		else
		{
			ps->axis_offset_z += z - SetDlg.m_Value;
			RoundReasonable(ps->axis_offset_z);
			ps->parameters[5213]=ps->axis_offset_z;
			Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
		}
	}
}

void CKMotionCNCDlg::OnSetA() 
{
	static CSetValue SetDlg(IDD_SetValueHalf);
	double x, y, z, a, b, c, u, v;

	DisableKeyJog();
	if (ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v)) return;

	SetDlg.halfDRO = a / 2.0;

	if (SetDlg.DoModal() == IDOK)
	{
		setup_pointer ps = Interpreter->p_setup;

		if (m_ZeroUsingFixtures)
		{
			int Fix = ps->origin_index-1;
			ps->AA_origin_offset += a - SetDlg.m_Value;
			RoundReasonable(ps->AA_origin_offset);
			ps->parameters[5221+Fix*20+3]=ps->AA_origin_offset;
		}
		else
		{
			ps->AA_axis_offset += a - SetDlg.m_Value;
			RoundReasonable(ps->AA_axis_offset);
			ps->parameters[5214]=ps->AA_axis_offset;
			Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
		}
	}
}

void CKMotionCNCDlg::OnSetB() 
{
	static CSetValue SetDlg(IDD_SetValueHalf);
	double x, y, z, a, b, c, u, v;

	DisableKeyJog();
	if (ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v)) return;

	SetDlg.halfDRO = b / 2.0;

	if (SetDlg.DoModal() == IDOK)
	{
		setup_pointer ps = Interpreter->p_setup;

		if (m_ZeroUsingFixtures)
		{
			int Fix = ps->origin_index-1;
			ps->BB_origin_offset += b - SetDlg.m_Value;
			RoundReasonable(ps->BB_origin_offset);
			ps->parameters[5221+Fix*20+4]=ps->BB_origin_offset;
		}
		else
		{
			ps->BB_axis_offset += b - SetDlg.m_Value;
			RoundReasonable(ps->BB_axis_offset);
			ps->parameters[5215]=ps->BB_axis_offset;
			Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
		}
	}
}

void CKMotionCNCDlg::OnSetC()
{
	static CSetValue SetDlg(IDD_SetValueHalf);
	double x, y, z, a, b, c, u, v;

	DisableKeyJog();
	if (ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v)) return;

	SetDlg.halfDRO = c / 2.0;

	if (SetDlg.DoModal() == IDOK)
	{
		setup_pointer ps = Interpreter->p_setup;

		if (m_ZeroUsingFixtures)
		{
			int Fix = ps->origin_index - 1;
			ps->CC_origin_offset += c - SetDlg.m_Value;
			RoundReasonable(ps->CC_origin_offset);
			ps->parameters[5221 + Fix * 20 + 5] = ps->CC_origin_offset;
		}
		else
		{
			ps->CC_axis_offset += c - SetDlg.m_Value;
			RoundReasonable(ps->CC_axis_offset);
			ps->parameters[5216] = ps->CC_axis_offset;
			Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
		}
	}
}

void CKMotionCNCDlg::OnSetU()
{
	static CSetValue SetDlg(IDD_SetValueHalf);
	double x, y, z, a, b, c, u, v;

	DisableKeyJog();
	if (ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v)) return;

	SetDlg.halfDRO = u / 2.0;

	if (SetDlg.DoModal() == IDOK)
	{
		setup_pointer ps = Interpreter->p_setup;

		if (m_ZeroUsingFixtures)
		{
			int Fix = ps->origin_index - 1;
			ps->CC_origin_offset += u - SetDlg.m_Value;
			RoundReasonable(ps->UU_origin_offset);
			ps->parameters[5221 + Fix * 20 + 6] = ps->UU_origin_offset;
		}
		else
		{
			ps->UU_axis_offset += u - SetDlg.m_Value;
			RoundReasonable(ps->UU_axis_offset);
			ps->parameters[5217] = ps->UU_axis_offset;
			Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
		}
	}
}

void CKMotionCNCDlg::OnSetV()
{
	static CSetValue SetDlg(IDD_SetValueHalf);
	double x, y, z, a, b, c, u, v;

	DisableKeyJog();
	if (ReadInterpPos(&x, &y, &z, &a, &b, &c, &u, &v)) return;

	SetDlg.halfDRO = v / 2.0;

	if (SetDlg.DoModal() == IDOK)
	{
		setup_pointer ps = Interpreter->p_setup;

		if (m_ZeroUsingFixtures)
		{
			int Fix = ps->origin_index - 1;
			ps->VV_origin_offset += v - SetDlg.m_Value;
			RoundReasonable(ps->VV_origin_offset);
			ps->parameters[5221 + Fix * 20 + 7] = ps->VV_origin_offset;
		}
		else
		{
			ps->VV_axis_offset += v - SetDlg.m_Value;
			RoundReasonable(ps->VV_axis_offset);
			ps->parameters[5218] = ps->VV_axis_offset;
			Interpreter->StampAxisOffsetUnits();   // units known: the interpreter's
		}
	}
}

void CKMotionCNCDlg::RoundReasonable(double &v)
{
	if (Interpreter->p_setup->length_units == CANON_UNITS_INCHES)			// for inches round to 6 digits
	{
		if (v<0)
			v =  (ceil(v * 1e6 - 0.5)) / 1e6;
		else
			v =  (floor(v * 1e6 + 0.5)) / 1e6;
	}
	else									// for mm round to 4 digits
	{
		if (v<0)
			v =  (ceil(v * 1e4 - 0.5)) / 1e4;
		else
			v =  (floor(v * 1e4 + 0.5)) / 1e4;
	}
}

void CKMotionCNCDlg::SetStepSizes()
{
	SetStepText(5,m_Step5,IDC_Step5);
	SetStepText(4,m_Step4,IDC_Step4);
	SetStepText(3,m_Step3,IDC_Step3);
	SetStepText(2,m_Step2,IDC_Step2);
	SetStepText(1,m_Step1,IDC_Step1);
	SetStepText(0,m_Step0,IDC_Step0);
	UpdateData(false);
}

void CKMotionCNCDlg::SetStepText(int i, double v, int ID)
{
	if (v != 0)
	{
		CString s;
		s.Format(L"%f",v);
		while (s.GetAt(s.GetLength()-1) == '0') s.Delete(s.GetLength()-1);  // remove trailing zeros
		while (s.GetAt(s.GetLength()-1) == '.') s.Delete(s.GetLength()-1);  // remove trailing decimals
		SetDlgItemText(ID,s);
		GetDlgItem(ID)->SetWindowText(s);
		GetDlgItem(ID)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(ID)->ShowWindow(SW_HIDE);
		if (i == m_StepSize && m_StepSize != 0) m_StepSize--;
	}

}

void CKMotionCNCDlg::OnStopStep() 
{
	Interpreter->Halt();
}

void CKMotionCNCDlg::Onmm() 
{
	DoGCodeLine(L"G21");
}

void CKMotionCNCDlg::Oninch() 
{
	DoGCodeLine(L"G20");
}

void CKMotionCNCDlg::OnAbs() 
{
	DoGCodeLine(L"G90");
}

void CKMotionCNCDlg::OnRel() 
{
	DoGCodeLine(L"G91");
}


BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString s;

#ifdef _WIN64
	s = "KMotionCNC 64-bit " KMOTION_VER;
#else
	s = "KMotionCNC 32-Bit " KMOTION_VER;
#endif	
	SetDlgItemText(IDC_KMotionCNCVersion,s);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CKMotionCNCDlg::OnBnClickedFeedhold()
{
	CStringA r,s,response;
	CCoordMotion *CM = Interpreter->CoordMotion;

	// G-code viewer: during a Simulate playback nothing is moving, so Feed Hold
	// just pauses / resumes the animation
	if (m_PlaybackRunning && !ThreadIsExecuting)
	{
		m_PlaybackPaused = !m_PlaybackPaused;
		return;
	}

	if (CM->m_TapCycleInProgress)
	{
		MCODE_ACTION *p = &Interpreter->McodeActions[119 - 100 + MCODE_ACTIONS_M100_OFFSET];

		s.Format("GetPersistDec %d", (int)p->dParams[1]);
		if (TheFrame->KMotionDLL->WriteLineReadLine(s, r.GetBufferSetLength(100))) return;
		r.ReleaseBuffer();

		if (r == "-2") //already in feedhold?
		{
			s.Format("SetPersistDec%d 0", (int)p->dParams[1]); // get Var from M119 Action, set to 0 to release feedhold
			if (TheFrame->KMotionDLL->WriteLine(s)) return;
		}
		else
		{
			s.Format("SetPersistDec%d -2", (int)p->dParams[1]); // get Var from M119 Action, set to -2 to feedhold
			if (TheFrame->KMotionDLL->WriteLine(s)) return;
		}
	}
	else
	{
		if (TheFrame->KMotionDLL->WriteLineReadLine("GetStopState", response.GetBufferSetLength(MAX_LINE))) return;
		response.ReleaseBuffer();

		if (response == "0")
		{
			TheFrame->KMotionDLL->WriteLine("StopImmediate0");
		}
		else
		{
			Interpreter->InvokeAction(ACTION_RESUME, FALSE);  // Do Special Action
			TheFrame->KMotionDLL->WriteLine("StopImmediate1");
		}
	}
}

void CKMotionCNCDlg::OnBnClickedEdittoolfile()
{
	CEditToolFile EditToolFile;

	if (!ThreadIsExecuting)
	{
		DisableKeyJog();

		if (EditToolFile.LoadFile(m_ToolFile)) return;


		if (EditToolFile.DoModal() == IDOK)
		{
			EditToolFile.SaveFile(m_ToolFile);
			HandleToolTableClose();
		}
	}
}


void CKMotionCNCDlg::HandleToolTableClose()
{
	FillComboWithTools(&m_tool);
	Interpreter->ReadToolFile();
	ActualGViewParent->m_ToolFileDisplayed="";

	// check if a tool offset is currently selected
	if (Interpreter->p_setup->length_offset_index !=-1 && m_tool.GetCount() > 0)
	{
		// yes one is selected, re-select it in case something changed

		CString s;
		int tool = m_LastToolDisplayed;
		if (tool >=0)
		{
			m_tool.GetLBText(tool,s);

			if (s.GetAt(0) == 'I')
				s.Delete(0,3);
			else
				s.Delete(0,5);

			swscanf(s,L"%d",&tool);

			if (m_ToolTableDoM6)
			{
				if (m_ToolLengthImmediately)
					s.Format(L"T%dM6H%dG43.4",tool,tool);
				else
					s.Format(L"T%dM6",tool);

				DoGCodeLine(s);
			}

		}
	}
	m_LastToolDisplayed=-1;
	Interpreter->m_ReadToolFile=true;
}


void CKMotionCNCDlg::OnBnClickedEditfixtures()
{
	setup_pointer settings = TheFrame->GCodeDlg.Interpreter->p_setup;
	double *Vars = settings->parameters;	

	CEditFixtures EditFixtures(this,Interpreter);

	DisableKeyJog();

	for (int axis=0; axis<NFIXAXES; axis++)
		for (int Fix=0; Fix<NFIXTURES; Fix++)
			EditFixtures.m_Fixture[axis][Fix] = Vars[5221+Fix*20+axis];

	for (int axis=0; axis<NFIXAXES; axis++)
		EditFixtures.m_Global[axis] = Vars[5211+axis];

	EditFixtures.m_SaveFixtureOnOK = m_SaveFixtureOnOK;

	if (EditFixtures.DoModal() == IDOK)
	{
		m_SaveFixtureOnOK = EditFixtures.m_SaveFixtureOnOK;

		for (int axis=0; axis<NFIXAXES; axis++)
			for (int Fix=0; Fix<NFIXTURES; Fix++)
				Vars[5221+Fix*20+axis] = EditFixtures.m_Fixture[axis][Fix];

		for (int axis=0; axis<NFIXAXES; axis++)
			Vars[5211+axis] = EditFixtures.m_Global[axis];

		// the G92 row is displayed and edited in the interpreter's current
		// units and applied in them below - stamp the tuple accordingly so a
		// later G20/G21 or a startup in the other units converts it (a mm
		// offset typed here must not come back as inches)
		Interpreter->StampAxisOffsetUnits();

		// make sure Settings are in sync with any modified Vars
		Interpreter->p_setup->origin_index=-1; // set invalid so it updates
		Interpreter->ChangeFixtureNumber(m_LastFixtureDisplayed);

		settings->axis_offset_x = Vars[5211];
		settings->axis_offset_y = Vars[5212];
		settings->axis_offset_z = Vars[5213];
		settings->AA_axis_offset = Vars[5214];
		settings->BB_axis_offset = Vars[5215];
		settings->CC_axis_offset = Vars[5216];
		settings->UU_axis_offset = Vars[5217];
		settings->VV_axis_offset = Vars[5218];
	}
}

void CKMotionCNCDlg::OnBnClickedSetfixture()
{
	double *Vars = TheFrame->GCodeDlg.Interpreter->p_setup->parameters;	
	setup_pointer ps = Interpreter->p_setup;

	if (Interpreter->ReadAndSyncCurPositions(&ps->current_x,&ps->current_y,&ps->current_z,
				&ps->AA_current, &ps->BB_current, &ps->CC_current, &ps->UU_current, &ps->VV_current)) return;
	
	int Fix = ps->origin_index-1;

	Vars[5221 + Fix * 20 + 0] += ps->current_x;
	Vars[5221 + Fix * 20 + 1] += ps->current_y;
	Vars[5221 + Fix * 20 + 2] += ps->current_z;
	Vars[5221 + Fix * 20 + 3] += ps->AA_current;
	Vars[5221 + Fix * 20 + 4] += ps->BB_current;
	Vars[5221 + Fix * 20 + 5] += ps->CC_current;
	Vars[5221 + Fix * 20 + 6] += ps->UU_current;
	Vars[5221 + Fix * 20 + 7] += ps->VV_current;

	// for inches round to 6 digits for mm round to 4 digits
	for (int axis=0; axis<NFIXAXES; axis++)
		RoundReasonable(Vars[5221+Fix*20+axis]);

	Interpreter->p_setup->origin_index=-1; // set invalid so it updates
	Interpreter->ChangeFixtureNumber(m_LastFixtureDisplayed);
}


void CKMotionCNCDlg::OnEnSetfocusFeedrateedit()
{
	DisableKeyJog();
}

void CKMotionCNCDlg::OnEnSetfocusSpindlerateedit()
{
	DisableKeyJog();
}

void CKMotionCNCDlg::DisableKeyJog()
{
	if (m_KeyJogMode.Toggled)
	{
		m_KeyJogMode.Toggled = false;
		m_KeyJogMode.Invalidate();
	}
}

void CKMotionCNCDlg::OnBnSetfocusSetx()
{
	DisableKeyJog();
}

void CKMotionCNCDlg::OnBnSetfocusSety()
{
	DisableKeyJog();
}

void CKMotionCNCDlg::OnBnSetfocusSetz()
{
	DisableKeyJog();
}

void CKMotionCNCDlg::OnBnSetfocusSeta()
{
	DisableKeyJog();
}

void CKMotionCNCDlg::OnBnSetfocusSetb()
{
	DisableKeyJog();
}

void CKMotionCNCDlg::OnBnSetfocusSetc()
{
	DisableKeyJog();
}

void CKMotionCNCDlg::OnCbnSetfocusCommand()
{
	DisableKeyJog();
}


void CKMotionCNCDlg::OnSize(UINT nType, int cx, int cy) 
{
	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);

	if (cx > nScreenWidth)  // for some reason after Sleep dialog size is huge, restore it
	{
		cx = LastSizeX;
		cy = LastSizeY;
		SetWindowPos(NULL, LastMoveX, LastMoveY, LastSizeX, LastSizeY, SWP_NOZORDER);
	}
	CDlgX::OnSize(nType,cx,cy);
	if (m_DialogFaceInUse != CUSTOM_DLG_FACE && cx > 0) // don't update when minimizing
	{
		UPDATE_EASYSIZE;
	}
}
void CKMotionCNCDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDlgX::OnSizing(fwSide, pRect);
	EASYSIZE_MINSIZE(OrigWindowRect,fwSide,pRect);
}


void CKMotionCNCDlg::OnBnClickedRunsimulate()
{
	if (!UpdateData()) return;
	if (!m_Simulate)
	{
		m_Simulate=m_DoingSimulationRun=true;
	}
	else
	{
		m_DoingSimulationRun=false;
	}
	if (!UpdateData(FALSE)) return;

	OnExecute();
}

void CKMotionCNCDlg::OnBnClickedSpindleoncw()
{
	Interpreter->p_setup->spindle_turning = CANON_CLOCKWISE;
	Interpreter->InvokeAction(10,FALSE);  // Set the Speed Sxxxx
	Interpreter->InvokeAction(3,FALSE);   // do the defined action for M3 Code
}

void CKMotionCNCDlg::OnBnClickedSpindleonccw()
{
	Interpreter->p_setup->spindle_turning = CANON_COUNTERCLOCKWISE;
	Interpreter->InvokeAction(10,FALSE);  // Set the Speed Sxxxx
	Interpreter->InvokeAction(4,FALSE);   // do the defined action for M4 Code
}

void CKMotionCNCDlg::OnBnClickedSpindleoff()
{
	Interpreter->p_setup->spindle_turning = CANON_STOPPED;
	Interpreter->InvokeAction(5,FALSE);  // do the defined action for M Code
}


LRESULT CKMotionCNCDlg::OnNotifyFormat(WPARAM wParam, LPARAM lParam)
{
	return NFR_UNICODE;
}


BOOL CKMotionCNCDlg::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{ 
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	// to be thorough we will need to handle UNICODE versions of the message also !!
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;

	CString strTipText;
	UINT nID = pNMHDR->idFrom;

	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
		pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
	{
        // idFrom is actually the HWND of the tool
        nID = ::GetDlgCtrlID((HWND)(HANDLE64)(nID));   

		if(nID)        
		{
			if (Screen.CheckForScreenEditorToolTip(nID, pTTTW->lpszText)) return TRUE;
			static CString m_strToolTip;
			m_strToolTip = L"";
			pTTTW->lpszText = m_strToolTip.GetBuffer(1000);
			int result = GetDefaultToolTipTextFromID(nID, pTTTW->lpszText);
			m_strToolTip.ReleaseBuffer();
			if (result) return TRUE;
        }   
	}    

	return FALSE;
}

BOOL CKMotionCNCDlg::GetDefaultToolTipTextFromID(UINT nID, LPWSTR Tip)
{
	CString LocalToolTipText;

	if (m_Lathe)
	{
		if (nID == IDC_RightStep) {LocalToolTipText = "Step Right (+Z)"; wcscpy(Tip, LocalToolTipText); return TRUE; }
		if (nID == IDC_Right) { LocalToolTipText = "Right (+Z)";      wcscpy(Tip, LocalToolTipText); return TRUE; }
		if (nID == IDC_Right2) { LocalToolTipText = "Fast Right (+Z)"; wcscpy(Tip, LocalToolTipText); return TRUE; }
		if (nID == IDC_LeftStep) { LocalToolTipText = "Step Left (-Z)";  wcscpy(Tip, LocalToolTipText); return TRUE; }
		if (nID == IDC_Left) { LocalToolTipText = "Left (-Z)";       wcscpy(Tip, LocalToolTipText); return TRUE; }
		if (nID == IDC_Left2) { LocalToolTipText = "Fast Left (-Z)";  wcscpy(Tip, LocalToolTipText); return TRUE; 
	}

		if (m_XPosFront)
		{
			if (nID == IDC_ZplusStep) { LocalToolTipText = "Step Up (-X)";    wcscpy(Tip, LocalToolTipText); return TRUE; }
			if (nID == IDC_Zplus) { LocalToolTipText = "Up (-X)";         wcscpy(Tip, LocalToolTipText); return TRUE; }
			if (nID == IDC_Zplus2) { LocalToolTipText = "Fast Up (-X)";    wcscpy(Tip, LocalToolTipText); return TRUE; }
			if (nID == IDC_ZminusStep) { LocalToolTipText = "Step Down (+X)";  wcscpy(Tip, LocalToolTipText); return TRUE; }
			if (nID == IDC_Zminus) { LocalToolTipText = "Down (+X)";       wcscpy(Tip, LocalToolTipText); return TRUE; }
			if (nID == IDC_Zminus2) { LocalToolTipText = "Fast Down (+X)";  wcscpy(Tip, LocalToolTipText); return TRUE; }
		}
		else
		{
			if (nID == IDC_ZplusStep) { LocalToolTipText = "Step Up (+X)";    wcscpy(Tip, LocalToolTipText); return TRUE; }
			if (nID == IDC_Zplus) { LocalToolTipText = "Up (+X)";         wcscpy(Tip, LocalToolTipText); return TRUE; }
			if (nID == IDC_Zplus2) { LocalToolTipText = "Fast Up (+X)";    wcscpy(Tip, LocalToolTipText); return TRUE; }
			if (nID == IDC_ZminusStep) { LocalToolTipText = "Step Down (-X)";  wcscpy(Tip, LocalToolTipText); return TRUE; }
			if (nID == IDC_Zminus) { LocalToolTipText = "Down (-X)";       wcscpy(Tip, LocalToolTipText); return TRUE; }
			if (nID == IDC_Zminus2) { LocalToolTipText = "Fast Down (-X)";  wcscpy(Tip, LocalToolTipText); return TRUE; }
		}
	}

	SetLastError(0);
	if (LocalToolTipText.LoadString(nID) && !GetLastError())
	{
		wcscpy(Tip, LocalToolTipText);
		return TRUE;
	}
	return FALSE;
}

void CKMotionCNCDlg::LogJobEndTime(double seconds)
{
	CString File=TheFrame->MainPathRoot+LOG_RUNTIME_FILE, s;

	if (m_ThreadThatWasLaunched>=0 && m_ThreadThatWasLaunched<N_USER_GCODE_FILES)
	{
		FILE* f;

		_tfopen_s(&f, File, _T("at+,ccs=UTF-8"));

		if (!f)  // failed
		{
			MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate("Unable to open Runtime Log File\r\r") +  File, L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
			return;
		}


		CTime t = CTime::GetCurrentTime();
		CString time = t.Format( "%a, %b %d, %Y, %I:%M:%S  " );	

		if (m_Simulate && m_DoTime && JobDoTimeValid)
			s.Format(L"Expected Job Time: %ls ", convertSeconds(JobDoTimeSecs));
		else
			s.Format(L"Elapsed Job Time: %ls ", convertSeconds(seconds));

		s=time + s + FileNames[m_ThreadThatWasLaunched] + "\n";
		fputws(s, f);
		fclose(f);
	}
}


afx_msg BOOL CKMotionCNCDlg::OnNcActivate(BOOL bActive)
{
	if (!bActive)
	{
		POSITION position=CMotionButton::AxisGroup.GetHeadPosition();
		LPCMotionButton p;
		for (int i=0; i<CMotionButton::AxisGroup.GetCount(); i++)
		{
			p=CMotionButton::AxisGroup.GetNext(position);

			// moving?
			if (p->m_Moving || p->m_SimulateMotion || p->DrawPushed)
			{
				// stop it
				if (p->m_hWnd)
					p->PostMessage(WM_LBUTTONUP,0,0); 
			}
		}
	}
	return TRUE;
}

void CKMotionCNCDlg::OnReloadGeoCorrection()
{
	Interpreter->CoordMotion->Kinematics->ReadGeoTable(m_GeoFile);
}

void CKMotionCNCDlg::OnReloadGCodeFile()
{
	LoadFile(m_Thread,true);
	CurrentDirectory = ExtractDirectory(FileNames[m_Thread]);
}

void CKMotionCNCDlg::OnOpenGCodeFile()
{
	FileNames[m_Thread] = InterprocessString;
	LoadFile(m_Thread,true);
	CurrentDirectory = ExtractDirectory(FileNames[m_Thread]);
}

int CKMotionCNCDlg::OnScreenScript(UINT nID)
{
	DLG_CONTROL Dlg;
	bool NewControl;

	CString ws = InterprocessString;
	return Screen.Execute(ws, &Dlg, &NewControl);
}


int CKMotionCNCDlg::OnGetControlInfo(UINT nID)
{
	int ID = -1;
	CString strDataToSend;
	CImageButton *I;
	DLG_CONTROL *Dlg, Dlgx;

	if (m_DialogFaceInUse != CUSTOM_DLG_FACE)
	{
		MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate("Error Please set KMotionCNC Dialog Face to Custom before running Screen Editor"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
		return 1;
	}

	swscanf(InterprocessString, L"%d", &ID);

	Dlg = Screen.FindDlgControl(ID);

	if (Dlg == NULL)
	{
		Dlgx.Name = Screen.FindResourceName(ID);
		Dlgx.Type = "UNKNOWN";
		Dlgx.BitmapFile = "";
		Dlgx.Var = -1;
		Dlgx.Style = -1;
		Dlgx.Colors = "";
		Dlgx.FontName = "";
		Dlgx.FontSize = 10;
		Dlgx.HotKey = -1;
		Dlgx.Bold = Dlgx.Italic = false;
		Dlgx.Script = "";

		CString s;
		if (ID == IDC_Editor)  // For Scintilla Editor don't get the whole file
			s = "GCODE";
		else
		{
			::GetWindowTextW(GetDlgItem(ID)->m_hWnd, s.GetBufferSetLength(1000), 1000 - 1);
			s.ReleaseBuffer();
		}
		
		Dlgx.Text = s;

		RECT wresult;
		GetDlgItem(ID)->GetWindowRect(&wresult);

		POINT p;

		p.x = wresult.left;
		p.y = wresult.top;

		ScreenToClient(&p);
		Dlgx.x = p.x;
		Dlgx.y = p.y;

		p.x = wresult.right;
		p.y = wresult.bottom;
		ScreenToClient(&p);
		Dlgx.width = p.x - Dlgx.x;
		Dlgx.height = p.y - Dlgx.y;

		CFont *FontOrig;
		FontOrig = GetDlgItem(ID)->GetFont();

		if (FontOrig)
		{
			LOGFONT LF;
			int result = FontOrig->GetLogFont(&LF);

			if (result)
			{
				Dlgx.FontName = LF.lfFaceName;
				Dlgx.FontSize = -LF.lfHeight;
				Dlgx.Bold = LF.lfWeight == FW_BOLD;
				Dlgx.Italic = LF.lfItalic == TRUE;
			}
		}

		Dlgx.show = GetDlgItem(ID)->IsWindowVisible() != 0;

		Dlg = &Dlgx;
	}


	I = Screen.FindImageButton(ID);
	if (I)
	{
		Dlg->HotKey = I->m_HotKey;
		Dlg->Style = I->Style;

		CString up = StripPathMatch(I->m_up_file, SCREEN_BITMAPS_DIR);
		CString down = StripPathMatch(I->m_down_file, SCREEN_BITMAPS_DIR);

		Dlg->BitmapFile = up + ';' + down;
	}

	CDisplay *D = Screen.FindDisplay(ID);
	if (D)
	{
		Dlg->FontName = D->m_FaceName;
		Dlg->FontSize = D->m_fheight;
		Dlg->Bold = D->m_Bold;
		Dlg->Italic = D->m_Italic;
		CString s;
		Dlg->Colors.Format(L";%X;;",D->m_BackColor);
	}


	GetDefaultToolTipTextFromID(ID, Dlgx.ToolTipText.GetBufferSetLength(500));
	Dlgx.ToolTipText.ReleaseBuffer();

	strDataToSend = Screen.CreateScript(Dlg);

	COPYDATASTRUCT cpd;
	cpd.dwData = 0;
	cpd.cbData = (strDataToSend.GetLength() + 1) * 2;
	cpd.lpData = (PVOID)strDataToSend.GetBuffer(cpd.cbData);
	::SendMessage(HWndClient, WM_COPYDATA, (WPARAM)::GetDesktopWindow(), (LPARAM)&cpd);

	return 0;
}

int CKMotionCNCDlg::OnMainDlgInfo(UINT nID)
{
	CString strDataToSend, File, LoadedScreen = Screen.LastLoadedScreen;

	// the Screen Editor (like the Screen Script) expects RRGGBB, not a COLORREF (BBGGRR)
	strDataToSend.Format(L"%06x", Screen.SwapRGB(m_DlgBackgroundColor));
	File = Screen.BGFile_loaded;
	strDataToSend = strDataToSend + L"," + File + L"," + LoadedScreen;

	COPYDATASTRUCT cpd;
	cpd.dwData = 0;
	cpd.cbData = (strDataToSend.GetLength() + 1) * 2;
	cpd.lpData = (PVOID)strDataToSend.GetBuffer(cpd.cbData);
	::SendMessage(HWndClient, WM_COPYDATA, (WPARAM)::GetDesktopWindow(), (LPARAM)&cpd);

	return 0;
}


BOOL CKMotionCNCDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
{
	HWndClient = pWnd->m_hWnd;
	InterprocessString = (LPCWSTR)pCopyDataStruct->lpData;
	return TRUE;
}


int CKMotionCNCDlg::OnDoJog(UINT nID)
{
	CStringA s;

	swscanf(InterprocessString, L"%lf%lf%lf%lf%lf%lf", &m_JoyExtvx, &m_JoyExtvy, &m_JoyExtvz, &m_JoyExtva, &m_JoyExtvb, &m_JoyExtvc);

	if (m_JoyExtvx == 0.0 && m_JoyExtvy == 0.0 && m_JoyExtvz == 0.0 && m_JoyExtva == 0.0 && m_JoyExtvb == 0.0 && m_JoyExtvc == 0.0)
	{
		for (int i = 0; i < ACTUATORS_CONTROLLED; i++)
		{
			if (CS_axis[i] >= 0)
			{
				// Stop
				s.Format("Jog%d=0", CS_axis[i]);
				if (TheFrame->KMotionDLL->WriteLine(s)) return 1;
			}
		}
	}
	
	return 0;
}

int CKMotionCNCDlg::OnDoMove(UINT nID)
{
	double x, y, z, a, b, c;
	CCoordMotion *CM = Interpreter->CoordMotion;

	CStringA s;
	
	swscanf(InterprocessString, L"%lf%lf%lf%lf%lf%lf", &x, &y, &z, &a, &b, &c);

	if (CM->ReadCurAbsPosition(&CM->current_x, &CM->current_y, &CM->current_z, &CM->current_a, &CM->current_b, &CM->current_c)) return 1;

	CM->ClearAbort();
	CM->SetTPParams();
	if (CM->StraightTraverse(x, y, z, a, b, c, true)) return 1;
	if (CM->FlushSegments()) return 1;
	if (CM->WaitForSegmentsFinished()) return 1;

	return 0;
}

int CKMotionCNCDlg::OnDoMoveExp(UINT nID)
{
	double Acts[MAX_ACTUATORS], x, y, z, a, b, c, tau;
	CCoordMotion *CM = Interpreter->CoordMotion;

	swscanf(InterprocessString, L"%lf%lf%lf%lf%lf%lf%lf", &x, &y, &z, &a, &b, &c, &tau);

	CM->Kinematics->TransformCADtoActuators(x, y, z, a, b, c, Acts);

	for (int i = 0; i<ACTUATORS_CONTROLLED; i++)
		if (DoActPositionExp(i, Acts[i], tau)) return 1;

	return 0;
}

int CKMotionCNCDlg::OnGetPositionsNoGeo(UINT nID)
{
	return OnGetPositions2(nID, true);
}

int CKMotionCNCDlg::OnGetPositions(UINT nID)
{
	return OnGetPositions2(nID, false);
}

int CKMotionCNCDlg::OnGetPositions2(UINT nID, bool NoGeo)
{
	double ActsDest[MAX_ACTUATORS], x, y, z, a, b, c;
	CCoordMotion *CM = Interpreter->CoordMotion;

	for (int i = 0; i<MAX_ACTUATORS; i++) ActsDest[i] = 0.0;

	if (CM->x_axis >= 0)	ActsDest[0] = MainStatus.Dest[CM->x_axis];
	if (CM->y_axis >= 0)	ActsDest[1] = MainStatus.Dest[CM->y_axis];
	if (CM->z_axis >= 0)	ActsDest[2] = MainStatus.Dest[CM->z_axis];
	if (CM->a_axis >= 0)	ActsDest[3] = MainStatus.Dest[CM->a_axis];
	if (CM->b_axis >= 0)	ActsDest[4] = MainStatus.Dest[CM->b_axis];
	if (CM->c_axis >= 0)	ActsDest[5] = MainStatus.Dest[CM->c_axis];

	CM->Kinematics->TransformActuatorstoCAD(ActsDest, &x, &y, &z, &a, &b, &c, NoGeo);

	CString s;

	swprintf(s.GetBufferSetLength(150), L"%f %f %f %f %f %f", x, y, z, a, b, c);
	s.ReleaseBuffer();

	CString strDataToSend = s;

	COPYDATASTRUCT cpd;
	cpd.dwData = 0;
	cpd.cbData = (strDataToSend.GetLength()+1)*2;
	cpd.lpData = (PVOID)strDataToSend.GetBuffer(cpd.cbData);
	::SendMessage(HWndClient, WM_COPYDATA, (WPARAM)::GetDesktopWindow(), (LPARAM)&cpd);

	return 0;
}

void CKMotionCNCDlg::SetDefaultHotKeys()
{
	m_EmergencyStop.m_HotKey=VK_ESCAPE;
	m_GO.m_HotKey=VK_F5;
	m_FeedHold.m_HotKey=VK_F3;
	m_KeyJogMode.m_HotKey= VK_F2;
}


void CKMotionCNCDlg::OnXy()
{
	ActualGViewParent->OnXy();
}

void CKMotionCNCDlg::OnXz()
{
	ActualGViewParent->OnXz();
}

void CKMotionCNCDlg::OnYz()
{
	ActualGViewParent->OnYz();
}

void CKMotionCNCDlg::OnClearPaths()
{
	ActualGViewParent->OnClearPaths();
}

void CKMotionCNCDlg::OnShowAxis()
{
	ActualGViewParent->OnShowAxis();
}

void CKMotionCNCDlg::OnOrtho()
{
	ActualGViewParent->OnOrtho();
}

void CKMotionCNCDlg::OnShowTool()
{
	ActualGViewParent->OnShowTool();
}

void CKMotionCNCDlg::OnBox()
{
	ActualGViewParent->OnBox();
}

void CKMotionCNCDlg::OnRotXY()
{
	ActualGViewParent->OnRotXY();
}

void CKMotionCNCDlg::OnGViewerSetup()
{
	ActualGViewParent->OnGViewerSetup();
}

bool CKMotionCNCDlg::DetermineEnableDisableKeys()
{
	return (!ThreadIsExecuting || EnableJogKeys) && !ForceDisableJogKeys;
}

void CKMotionCNCDlg::WhenIdle()
{
	HandleEnable(m_Zplus2);
	HandleEnable(m_Zplus);
	HandleEnable(m_ZplusStep);
	HandleEnable(m_Zminus2);
	HandleEnable(m_Zminus);
	HandleEnable(m_ZminusStep);
	
	HandleEnable(m_Aplus2);
	HandleEnable(m_Aplus);
	HandleEnable(m_AplusStep);
	HandleEnable(m_Aminus2);
	HandleEnable(m_Aminus);
	HandleEnable(m_AminusStep);

	HandleEnable(m_Bplus2);
	HandleEnable(m_Bplus);
	HandleEnable(m_BplusStep);
	HandleEnable(m_Bminus2);
	HandleEnable(m_Bminus);
	HandleEnable(m_BminusStep);

	HandleEnable(m_Cplus2);
	HandleEnable(m_Cplus);
	HandleEnable(m_CplusStep);
	HandleEnable(m_Cminus2);
	HandleEnable(m_Cminus);
	HandleEnable(m_CminusStep);

	HandleEnable(m_Uplus2);
	HandleEnable(m_Uplus);
	HandleEnable(m_UplusStep);
	HandleEnable(m_Uminus2);
	HandleEnable(m_Uminus);
	HandleEnable(m_UminusStep);

	HandleEnable(m_Vplus2);
	HandleEnable(m_Vplus);
	HandleEnable(m_VplusStep);
	HandleEnable(m_Vminus2);
	HandleEnable(m_Vminus);
	HandleEnable(m_VminusStep);

	HandleEnable(m_Right2);
	HandleEnable(m_Right);
	HandleEnable(m_RightStep);
	HandleEnable(m_Left2);
	HandleEnable(m_Left);
	HandleEnable(m_LeftStep);

	HandleEnable(m_Down2);
	HandleEnable(m_Down);
	HandleEnable(m_DownStep);
	HandleEnable(m_Up2);
	HandleEnable(m_Up);
	HandleEnable(m_UpStep);

	HandleEnable(m_StopStep);

	if (m_GCNew.m_hWnd) m_GCNew.EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	if (m_GCOpenFile.m_hWnd) m_GCOpenFile.EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	if (m_GCRestart.m_hWnd) m_GCRestart.EnableWindow(CurrentLine[m_Thread] != 0 && !ThreadIsExecuting && !ForceDisableJogKeys);
	if (m_GCSingleStep.m_hWnd) m_GCSingleStep.EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);

	GetDlgItem(IDC_EditToolFile)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_tool)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_fixture)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_EditFixtures)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_SetFixture)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_ZeroX)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_ZeroY)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_ZeroZ)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_ZeroA)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_ZeroB)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_ZeroC)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	if (m_ZeroU.m_hWnd) GetDlgItem(IDC_ZeroU)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	if (m_ZeroV.m_hWnd) GetDlgItem(IDC_ZeroV)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_SetX)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_SetY)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_SetZ)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_SetA)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_SetB)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_SetC)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	if (m_SetU.m_hWnd) GetDlgItem(IDC_SetU)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	if (m_SetV.m_hWnd) GetDlgItem(IDC_SetV)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_ZeroAll)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_mm)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_inch)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_Rel)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_Abs)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_Simulate)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_RunSimulate)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);
	GetDlgItem(IDC_Send)->EnableWindow(!ThreadIsExecuting && !ForceDisableJogKeys);

	if (m_GVShowTool.m_hWnd) m_GVShowTool.SetToggled(TheFrame->GViewDlg.m_ShowTool != 0);
	if (m_GVShowAxis.m_hWnd) m_GVShowAxis.SetToggled(TheFrame->GViewDlg.m_ShowAxis != 0);
	if (m_GVOrtho.m_hWnd) m_GVOrtho.SetToggled(TheFrame->GViewDlg.m_Ortho != 0);
	if (m_GVBox.m_hWnd) m_GVBox.SetToggled(TheFrame->GViewDlg.m_ShowBox != 0);
	if (m_GVRotXY.m_hWnd) m_GVRotXY.SetToggled(ActualGViewParent->m_view.m_xyRotation != 0);
}

// Motion Buttons (Jog Buttons) now allow individually, forced disable/enable 
// via setting the Screen Script Var parameter to:
// 0=Force Disable, 1=Force Enable, -1=dynamic automatic behavior based on Job Running or not. 

void CKMotionCNCDlg::HandleEnable(CImageButton& M)
{
	if (M.m_hWnd) M.EnableWindow(M.Var == -1 ? DetermineEnableDisableKeys() : M.Var);
}
int CKMotionCNCDlg::GetBoardType()
{
	int Type;

	if (m_BoardType == BOARD_TYPE_UNKNOWN)
		if (TheFrame->KMotionDLL->CheckKMotionVersion(&Type, true) == 0)
			m_BoardType = Type;

	return m_BoardType;
}

int CKMotionCNCDlg::GetNChans()
{
	if (m_BoardType == BOARD_TYPE_KOGNA) return N_CHANNELS_KOGNA;
	else return N_CHANNELS_KFLOP;
}

CString CKMotionCNCDlg::convertSeconds(int seconds) 
{
	int hours = seconds / 3600;
	int minutes = (seconds % 3600) / 60;
	int secs = seconds % 60;

	CString result;
	if (hours > 0) {
		result.Format(_T("%d:%02d:%02dsec"), hours, minutes, secs);
	}
	else if (minutes > 0) {
		result.Format(_T("%d:%02dsec"), minutes, secs);
	}
	else {
		result.Format(_T("%dsec"), secs);
	}
	return result;
}


void CKMotionCNCDlg::OnNcMouseLeave()
{
	// This feature requires Windows 2000 or greater.
	// The symbols _WIN32_WINNT and WINVER must be >= 0x0500.
	// TODO: Add your message handler code here and/or call default

	TrackOn = false;

	CDlgX::OnNcMouseLeave();
}


void CKMotionCNCDlg::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (!TrackOn)
	{
		Track.cbSize = sizeof(Track);
		Track.dwFlags = TME_HOVER | TME_LEAVE | TME_NONCLIENT;
		Track.dwHoverTime = 1500;
		Track.hwndTrack = this->m_hWnd;

		bool result = TrackMouseEvent(&Track);
		TrackOn = true;
	}

	CDlgX::OnNcMouseMove(nHitTest, point);
}


void CKMotionCNCDlg::OnToolTipTextAboutToShow(NMHDR* pNotifyStruct, LRESULT* result)
{
	static CString Tip;

	if (pNotifyStruct->hwndFrom == hwndTT)
	{
		static int recurse = 0;
		recurse++; // count entries
		// Associate the tooltip with the "tool" window.
		Tip = FileNames[m_Thread] + "\r\n" + LastTitleElapsed;
		ti.lpszText = Tip.GetBuffer();
		::SendMessage(hwndTT, TTM_SETMAXTIPWIDTH, 0, 800);
		::SendMessage(hwndTT, TTM_UPDATETIPTEXT, 0, (LPARAM)(LPTOOLINFO)&ti);
		if (recurse == 1) // not already updated?
			::SendMessage(hwndTT, TTM_UPDATE, 0, 0);
		recurse--;
	}
}

CString CKMotionCNCDlg::AbreviateFile(CString s, int n)
{
	// name too long?  
	while (s.GetLength() >= n)
	{
		// remove middle folder
		int i = s.GetLength() / 2;

		// search backwards for a slash
		int k = i;
		while (k > 0 && (s[k] != '\\' || (s[k+1] == '.' && s[k+2] == '.')))
			k--;

		if (k < 0) break;  // not found?

		// search forwards for a slash
		int j = k+1;
		while (j < s.GetLength() && s[j] != '\\')
			j++;

		if (j == s.GetLength()) break;  // not found?

		// replace text \xxxxxxxx\ with \...

		s = s.Left(k) + "\\..." + s.Right(s.GetLength() - j);
	}
	return s;
}