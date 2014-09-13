// CKMotionCNCDlg.cpp : implementation file
/*********************************************************************/
/*         Copyright (c) 2003-2014  DynoMotion Incorporated          */
/*********************************************************************/


#include "stdafx.h"
#include "MainFrm.h"
#include "EditToolFile.h"
#include "EditFixtures.h"
#include "AllToolSetupSheet.h"
#include "SetValue.h"
#include "KMotionCNCDlg.h"
#include "ResumeDlg.h"
#include "OpenDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FLAG_COLOR 0x808000

#define GCODE_FILES		"\\Data\\GFilesCNC.txt"

#define SLIDER_RANGE 100
#define OVERRIDE_MIN 0.10
#define OVERRIDE_MAX 2.0

#define STATUS_TIME 0.1 //sec

#define sqr(x) ((x)*(x))


//static callbacks

void StraightTraverseCallback(double x, double y, double z, double a, double b, double c, int sequence_number);

void ArcFeedCallback(bool ZeroLenAsFullCircles, double DesiredFeedRate_in_per_sec, 
			    CANON_PLANE plane,
				double first_end, double second_end, 
		        double first_axis, double second_axis, int rotation,
				double axis_end_point,double a, double b, double c,
				double first_start, double second_start, double axis_start_point, int sequence_number, int ID);

void StraightFeedCallback(double DesiredFeedRate_in_per_sec,
							   double x, double y, double z, double a, double b, double c, int sequence_number, int ID);




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
	fprintf(f,"%d\n",m_Thread);
	fprintf(f,"%d\n",m_Simulate);
	fprintf(f,"%d\n",m_ShowMach);
	fprintf(f,"%d\n",m_StepSize);
	CDlgX::SaveOnExit(f);

	SaveFileNames();
	SaveConfig();
}

void CKMotionCNCDlg::RestoreOnStart(FILE * f)
{
	CString s;

	fscanf(f,"%d",&m_Thread);
	fscanf(f,"%d",&m_Simulate);
	fscanf(f,"%d",&m_ShowMach);
	fscanf(f,"%d",&m_StepSize);

	// also read in file names

	for (int i=0; i<N_USER_GCODE_FILES; i++)      // set to default names
	{
		s.Format("\\GCode Programs\\user%d.ngc",i+1);
		FileNames[i] = TheFrame->MainPath + s;
	}

	CString File = TheFrame->MainPath + GCODE_FILES;
	
	FILE *g = fopen(File.GetBuffer(0),"rt");
	
	if (!g)
	{
		CString s;
		s.Format("Error opening file %s\n"
				"Previous G Code Filenames", File.GetBuffer(MAX_PATH));

		::MessageBox(NULL,s, "KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
	}
	else
	{
		for (int i=0; i<N_USER_GCODE_FILES && !feof(g); i++)
		{
			fgets(FileNames[i].GetBuffer(MAX_PATH),MAX_PATH,g);
			FileNames[i].ReleaseBuffer();
			FileNames[i].Remove('\n');

			// check if there is no path specified, then add in default

			if (FileNames[i].Find('\\') == -1)
			{
				FileNames[i] = TheFrame->MainPathRoot + "\\GCode Programs\\" + FileNames[i];
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

	


int CKMotionCNCDlg::SaveLoadConfig(FILE *f, char *s, bool save)
{
	CString DefaultPath = TheFrame->MainPathRoot + "\\GCode Programs\\";

	CSTRING_PATH(m_SetupFile,DefaultPath);
	CSTRING_PATH(m_ToolFile,DefaultPath);
	CSTRING_PATH(m_GeoFile,DefaultPath);
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
	DOUBLE(m_JogSlowPercent);
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
	DOUBLE(m_MaxVelC);
	DOUBLE(m_MaxVelB);
	DOUBLE(m_MaxVelA);
	DOUBLE(m_MaxVelX);
	DOUBLE(m_MaxVelY);
	DOUBLE(m_MaxVelZ);
	DOUBLE(m_CountsPerInchC);
	DOUBLE(m_CountsPerInchB);
	DOUBLE(m_CountsPerInchA);
	DOUBLE(m_CountsPerInchX);
	DOUBLE(m_CountsPerInchY);
	DOUBLE(m_CountsPerInchZ);
	
	DOUBLE(m_Step0);
	DOUBLE(m_Step1);
	DOUBLE(m_Step2);
	DOUBLE(m_Step3);
	DOUBLE(m_Step4);
	DOUBLE(m_Step5);

	INT(m_ReverseRZ);
	INT(m_EnableGamePad);
	INT(m_ZeroUsingFixtures);
	INT(m_ArcsToSegs);
	INT(m_DisplayEncoder);
	INT(m_Lathe);

	INT(m_SpindleType);
	INT(m_SpindleAxis);
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
	
	CSTRING_PATH(m_VarsFile,DefaultPath);

	return 0;
}


int CKMotionCNCDlg::SaveConfig()
{
	char s[81];
	CString Name = TheFrame->MainPath + "\\data\\GCodeConfigCNC.txt";

	FILE *f=fopen(Name.GetBuffer(0),"wb");
	
	if (!f)
	{
		CString cs;
		cs.Format("Error Opening Configuration File %s", Name.GetBuffer(0));
		TheFrame->MessageBox(cs,"Error",MB_ICONSTOP|MB_OK);
		return 1;
	}

	SaveLoadConfig(f, s, true);

	fclose(f);

	return 0;
}



int CKMotionCNCDlg::LoadConfig()
{
	char s[301];
	CString Name = TheFrame->MainPath + "\\data\\GCodeConfigCNC.txt";

	FILE *f=fopen(Name.GetBuffer(0),"rb");
	
	if (!f)
	{
		CString cs;
		cs.Format("Error Opening Configuration File %s", Name.GetBuffer(0));
		TheFrame->MessageBox(cs,"Error",MB_ICONSTOP|MB_OK);
		return 1;
	}

	while (!feof(f))
	{
		fgets(s,300,f);
		if (!feof(f))
		{
			SaveLoadConfig(f, s, false);
		}
	}

	fclose(f);

	return 0;
}




void CKMotionCNCDlg::SaveFileNames()
{
	CString DefaultPath = TheFrame->MainPathRoot + "\\GCode Programs\\";
	CString File = TheFrame->MainPath + GCODE_FILES;

	FILE *g = fopen(File.GetBuffer(0),"wt");
	
	if (!g)
	{
		CString s;
		s.Format("Error opening file %s\n"
				"To store Previous GCode Filenames", File.GetBuffer(MAX_PATH));
		::MessageBox(NULL,s, "KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
	}
	else
	{
		for (int i=0; i<N_USER_GCODE_FILES && !feof(g); i++)
		{
			// if the path is the default, and there is no other path specified, strip it off
			fprintf(g,"%s\n",StripPathMatch(FileNames[i],DefaultPath));
		}

		fclose(g);
	}
}


void CKMotionCNCDlg::LoadFile(int thread,bool ResetPosition)
{
	CString s;
	int fsize;
	CString DefaultPath = TheFrame->MainPathRoot + "\\GCode Programs\\";


	if (FileNames[thread].IsEmpty() || FileNames[thread] == DefaultPath)
	{
		// if filename for the thread is undefined
		// just set the editor empty

		s="";
	}
	else
	{
		FILE *f=fopen(FileNames[thread],"rb");

		if (!f)
		{
			s.Format("Error opening GCode file %s\n"
					"From Previous G Code Filenames", FileNames[thread]);
			::MessageBox(NULL,s, "KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);

			s="";
			FileNames[thread]="";
		}
		else
		{
			fsize = fseek(f,0,SEEK_END);
			fsize = ftell(f);

			fseek(f,0,SEEK_SET);

			s="";
			
			if (fsize>0 && fread(s.GetBuffer(fsize),fsize,1,f) != 1)
			{
				AfxMessageBox("unable to open file");
			}
			else
			{
				fclose(f);
				s.ReleaseBuffer(fsize);
			}
		}
	}

    m_Editor.SetText(s);
	m_Editor.EmptyUndoBuffer();
	m_Editor.SetSavePoint();
	DisplayedCurrentLine=-1;
	
	if (ResetPosition)
		OnRestart(); // reset to the beginning
	
	RefreshTitle();
	
	UpdateData(FALSE);
}


int CKMotionCNCDlg::SaveFile(int thread, bool ForceSave)
{
	CString s;

	// if the user isn't explicitly saving and the 
	// user hasn't changed anything then don't save

	if (!ForceSave && !m_Editor.GetModify()) return 0;

	// save the edit window

	int length = m_Editor.GetTextLength();


	s = m_Editor.GetText(length+1);

	// if the file to save to is undefined and the editor is empty, forget it

	if (s.IsEmpty() && FileNames[thread].IsEmpty()) return 0;

	if (FileNames[thread].IsEmpty())
	{
		// if there is something to save, but the filename is undefined
		// ask user where to save it 

		return DoSaveAs(thread);
	}

	FILE *f=fopen(FileNames[thread],"wb");

	if (!f)
	{
		// if we couldn't open the file and the editor is
		// empty, then just forget it

		if (s.IsEmpty()) return 0;

		CString err;
		err.Format("Error saving GCode file %s\n"
				"From Previous GCode Filenames\r\r"
				"Use SaveAs to save to different directory or filename", FileNames[thread]);
		::MessageBox(NULL,err, "KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);

		return 1;
	}
	else
	{
		if (s.GetLength()>0 && fwrite(s.GetBuffer(0),s.GetLength(),1,f) != 1)
		{
			AfxMessageBox("unable to write to file");
			return 1;
		}
		else
		{
			fclose(f);
		}
	}

	m_Editor.SetSavePoint();  // clear the Modified Flag
	RefreshTitle();

	return 0;
}

void CKMotionCNCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDlgX::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKMotionCNCDlg)
	DDX_Control(pDX, IDC_Command, m_Command);
	DDX_Control(pDX, IDC_tool, m_tool);
	DDX_Control(pDX, IDC_fixture, m_FixtureOffset);
	DDX_Control(pDX, IDC_Editor, m_Editor);
	DDX_Radio(pDX, IDC_Thread1, m_Thread);
	DDX_Check(pDX, IDC_Simulate, m_Simulate);
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
	DDX_Text(pDX, IDC_FeedRateEdit, m_FeedRateValue);
	DDX_Text(pDX, IDC_SpindleRateEdit, m_SpindleRateValue);
	DDV_MinMaxDouble(pDX, m_FeedRateValue, 0.1, 2.);
	DDX_Text(pDX, IDC_SpindleRateEdit, m_SpindleRateValue);
	DDV_MinMaxDouble(pDX, m_SpindleRateValue, 0.1, 2.);
	DDX_CBString(pDX, IDC_Command, m_CommandString);
	DDV_MaxChars(pDX, m_CommandString, 256);
	DDX_Control(pDX, IDC_EmergencyStop, m_EmergencyStop);
	DDX_Control(pDX, IDC_GO, m_GO);
	DDX_Control(pDX, IDC_KeyJogMode, m_KeyJogMode);
	DDX_Control(pDX, IDC_FeedHold, m_FeedHold);
	DDX_Radio(pDX, IDC_Step0, m_StepSize);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CKMotionCNCDlg, CDlgX)
	//{{AFX_MSG_MAP(CKMotionCNCDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_COMMAND(ID_ESC, OnESC)
	ON_COMMAND(ID_F2, OnF2)	
	ON_COMMAND(ID_F5, OnGO)
	ON_COMMAND(ID_F3, OnBnClickedFeedhold)
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
	ON_BN_CLICKED(IDC_KMotion_HELP, OnIhelp)
	ON_BN_CLICKED(IDC_Thread1, OnThread1)
	ON_BN_CLICKED(IDC_Thread2, OnThread2)
	ON_BN_CLICKED(IDC_Thread3, OnThread3)
	ON_BN_CLICKED(IDC_Thread4, OnThread4)
	ON_BN_CLICKED(IDC_Thread5, OnThread5)
	ON_BN_CLICKED(IDC_Thread6, OnThread6)
	ON_BN_CLICKED(IDC_Thread7, OnThread7)
	ON_COMMAND(ID_SaveFile, OnSaveFile)
	ON_COMMAND(ID_New, OnNew)
	ON_COMMAND(ID_OpenFile, OnOpenFile)
	ON_COMMAND(ID_Execute, OnExecute)
	ON_COMMAND(ID_SaveAs, OnSaveAs)
	ON_COMMAND(ID_GCodeHalt, OnHalt)
	ON_COMMAND(ID_ToolSetup, OnToolSetup)
	ON_BN_CLICKED(IDC_ExecuteComplete, OnExecuteComplete)
	ON_COMMAND(ID_SingleStep, OnSingleStep)
	ON_COMMAND(ID_Restart, OnRestart)
	ON_UPDATE_COMMAND_UI(ID_Restart, OnUpdateRestart)
	ON_UPDATE_COMMAND_UI(ID_SingleStep, OnUpdateSingleStep)
	ON_COMMAND(ID_GView, OnGView)
	ON_BN_CLICKED(IDC_Simulate, OnSimulate)
	ON_BN_CLICKED(IDC_BlockDelete, OnBlockDelete)
	ON_BN_CLICKED(IDC_ShowMach, OnShowMach)
	ON_CBN_DROPDOWN(IDC_fixture, OnDropdownfixture)
	ON_CBN_CLOSEUP(IDC_fixture, OnCloseupfixture)
	ON_CBN_CLOSEUP(IDC_tool, OnCloseuptool)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_Send, OnSend)
	ON_CBN_DROPDOWN(IDC_Command, OnDropdownCommand)
	ON_CBN_CLOSEUP(IDC_Command, OnCloseupCommand)
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
	END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CKMotionCNCDlg)

	if (m_DialogFace == 8)
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

    EASYSIZE(IDC_Send,          ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
    EASYSIZE(IDC_KMotion_HELP,  ES_KEEPSIZE,IDC_Simulate,ES_BORDER,IDC_Send,ES_VCENTER)
    EASYSIZE(IDC_Simulate,      ES_KEEPSIZE,IDC_Thread7,ES_BORDER,IDC_KMotion_HELP,ES_VCENTER)
    EASYSIZE(IDC_SimulateStatic,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,IDC_Simulate,0)
    EASYSIZE(IDC_RunSimulate,   ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,IDC_Simulate,0)
    EASYSIZE(IDC_BlockDelete,   ES_KEEPSIZE,IDC_SimulateStatic,ES_BORDER,IDC_Simulate,0)
    EASYSIZE(IDC_BlockDeleteStatic,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,IDC_BlockDelete,0)

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
	CString s;
	
	s.Format("\\GCode Programs\\user%d.ngc",m_Thread+1);
	CString File = TheFrame->MainPathRoot + s;


	int answer = AfxMessageBox("Overwrite File:"+File+" ?",MB_YESNO);

	if (answer != IDYES) return;

    m_Editor.SetText("");

	FileNames[m_Thread] = File;

	RefreshTitle();
	
	UpdateData(FALSE);

	SaveFile(m_Thread, true); // force the save
}

void CKMotionCNCDlg::OnOpenFile() 
{
	CString s;
	
	DisableKeyJog();
	CPersistOpenDlg FileDlg (TRUE, ".ngc", NULL, 
		OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ENABLESIZING, 
		"GCode Files (*.ngc *.nc *.ncc *.cnc *.tap *.txt)|*.ngc;*.nc;*.ncc;*.cnc;*.tap;*.txt|All Files (*.*)|*.*||");

	FileDlg.m_ofn.lpstrInitialDir = CurrentDirectory;

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
	CPersistOpenDlg FileDlg (FALSE, ".ngc", FileNames[thread], 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING, 
		"GCode Files (*.ngc *.nc *.ncc *.cnc *.tap *.txt)|*.ngc;*.nc;*.ncc;*.cnc;*.tap;*.txt|All Files (*.*)|*.*||");

	// check if the last filename exists, if not
	// don't try to select it otherwise the file dialog fails

	FILE *f=NULL;
	
	if (!FileNames[thread].IsEmpty())
		f=fopen(FileNames[thread],"r");

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
	if (m_DoingSimulationRun)
	{
		m_Simulate=false;
		Interpreter->CoordMotion->m_Simulate=false;
		UpdateData(FALSE);
		m_DoingSimulationRun=false;
	}

	if (m_exitcode)
	{
		GetDlgItem(IDC_Editor)->SetFocus();
	}
}



void CompleteCallback(int status, int lineno, int sequence_number, const char *err)
{
	CKMotionCNCDlg *p=&TheFrame->GCodeDlg;
	CCoordMotion *CM = p->Interpreter->CoordMotion;

	p->m_ErrorOutput=err;
	p->m_exitcode=status;

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

			TheFrame->GViewDlg.m_view.OpenGLMutex->Lock();

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

			if (CM->m_PreviouslyStoppedID==1 &&
				CM->m_Stopping == STOPPED_COORD)
				TheFrame->GViewDlg.m_Path->RemovePathEnd(sequence_number,-1,x,y,z);
			else
				TheFrame->GViewDlg.m_Path->RemovePathEnd(sequence_number,1,x,y,z);

			TheFrame->GViewDlg.m_Path->SetModified();
			TheFrame->GViewDlg.m_view.OpenGLMutex->Unlock();
		}

		if (p->m_ThreadThatWasLaunched>=0)
			p->ThreadHadError[p->m_ThreadThatWasLaunched]=true;

		if (!p->ShuttingDownApplication && 
			CM->m_Stopping == STOPPED_NONE &&
			status != 1005)
			AfxMessageBox("G Code Error\r\r" + p->m_ErrorOutput,MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
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
	p->JobEndTimeSecs=p->ElapsedTimer.Elapsed_Seconds();
	p->JobEndTimeValid=true;

	if (p->m_RestoreStoppedState)
	{
		CM->m_PreviouslyStopped = p->m_SaveStoppedState;
		p->m_RestoreStoppedState=false;
	}
	// post message so GUI thread updates screens

	if (p->m_hWnd)
		p->PostMessage(WM_COMMAND,IDC_ExecuteComplete,0);
}

void StatusCallback(int line_no, const char *msg)
{
	CKMotionCNCDlg *p=&TheFrame->GCodeDlg;

	p->GCodeMutex->Lock();

	if (p->m_ThreadThatWasLaunched>=0)
		p->CurrentLine[p->m_ThreadThatWasLaunched]=line_no;

	p->GCodeMutex->Unlock();
}

void CKMotionCNCDlg::OnGO()
{
	if (ThreadIsExecuting)
		OnHalt();
	else
		OnExecute();
}

void CKMotionCNCDlg::OnExecute() 
{
	if (!ThreadIsExecuting)
	{
		if (SaveFile(m_Thread,false)) return;  // don't force the save
		m_ThreadThatWasLaunched=m_Thread;
		m_RestoreStoppedState=false;

		JobStartTimeSecs=ElapsedTimer.Elapsed_Seconds();
		JobStartTimeValid=true;
		JobEndTimeValid=false;

		if (LaunchExecution(FileNames[m_Thread],CurrentLine[m_Thread],-1)) return;
		GCodeThreadActive[m_Thread]=1;
	}
}

void CKMotionCNCDlg::OnSingleStep() 
{
	if (!ThreadIsExecuting)
	{
		if (SaveFile(m_Thread,false)) return;  // don't force the save
		m_ThreadThatWasLaunched=m_Thread;
		m_RestoreStoppedState=false;

		JobStartTimeSecs=ElapsedTimer.Elapsed_Seconds();
		JobStartTimeValid=true;
		JobEndTimeValid=false;

		if (LaunchExecution(FileNames[m_Thread],CurrentLine[m_Thread],CurrentLine[m_Thread])) return;
		GCodeThreadActive[m_Thread]=1;
	}
}

void CKMotionCNCDlg::OnRestart() 
{
	ThreadHadError[m_Thread]=false;
	CurrentLine[m_Thread]=0;
	Interpreter->CoordMotion->m_PreviouslyStopped = STOPPED_NONE;
}


int CKMotionCNCDlg::LaunchExecution(CString InFile,int begin, int end)
{
	int BoardType=BOARD_TYPE_UNKNOWN;

	if (InFile.IsEmpty())
	{
		AfxMessageBox("Invalid Filename Specified");
		return 1;
	}

	Interpreter->CoordMotion->SetStraightTraverseCallback(StraightTraverseCallback);
	Interpreter->CoordMotion->SetStraightFeedCallback(StraightFeedCallback);
	Interpreter->CoordMotion->SetArcFeedCallback(ArcFeedCallback);

	Interpreter->CoordMotion->m_Simulate = (m_Simulate==0) ? false : true;
	Interpreter->CoordMotion->m_DisableSoftLimits = false;

	SetMotionParams();


	if (m_Simulate==0)
		if (Interpreter->CoordMotion->KMotionDLL->CheckKMotionVersion( &BoardType)) return 1;

	Interpreter->CoordMotion->KMotionDLL->BoardID=board;
	Interpreter->CoordMotion->m_board_type=BoardType;
	Interpreter->CoordMotion->ClearAbort();
	Interpreter->CoordMotion->m_AxisDisabled=false;
	Interpreter->CoordMotion->ClearHalt();

	if (!m_Simulate && CheckForResumeCircumstances()) return 1; // check if we should move back to where we stopped

	ThreadIsExecuting=true;

	if (begin==0 && m_ThreadThatWasLaunched!=-1)
	{
		TheFrame->GViewDlg.ClearPaths();
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

	if (!CM->m_PreviouslyStopped)
	{
		return Interpreter->ReadAndSyncCurPositions(&ps->current_x,&ps->current_y,&ps->current_z,&ps->AA_current,&ps->BB_current,&ps->CC_current);
	}

	// read the current positions

	if (CM->ReadCurAbsPosition(&Machinex,&Machiney,&Machinez,&Machinea,&Machineb,&Machinec,true)) return 1;

	if (!CM->m_PreviouslyStopped) return 0;

	if (((CM->x_axis < 0) || (Machinex == CM->m_StoppedMachinex)) &&
		((CM->y_axis < 0) || (Machiney == CM->m_StoppedMachiney)) &&
		((CM->z_axis < 0) || (Machinez == CM->m_StoppedMachinez)) &&
		((CM->a_axis < 0) || (Machinea == CM->m_StoppedMachinea)) &&
		((CM->b_axis < 0) || (Machineb == CM->m_StoppedMachineb)) &&
		((CM->c_axis < 0) || (Machinec == CM->m_StoppedMachinec))) return 0;


	CResumeDlg ResumeDlg;

	ResumeDlg.m_SafeZ = Interpreter->InchesToUserUnits(m_SafeZ);
	ResumeDlg.m_SafeRelAbs = m_SafeRelAbs;
	ResumeDlg.m_MoveToSafeZ = TRUE;
	ResumeDlg.m_TraverseXY = TRUE;
	ResumeDlg.m_Metric = Interpreter->p_setup->length_units == CANON_UNITS_MM;


	ResumeDlg.m_TraverseSafeX = CM->m_Stoppedx;
	ResumeDlg.m_TraverseSafeY = CM->m_Stoppedy;

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
	ResumeDlg.m_FeedSafeZ = CM->m_Stoppedz;
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
		Interpreter->m_ResumeTraverseSafeX=ResumeDlg.m_TraverseSafeX;
		Interpreter->m_ResumeTraverseSafeY=ResumeDlg.m_TraverseSafeY;
		Interpreter->m_ResumeSafeStartSpindle=ResumeDlg.m_SafeStartSpindle;
		Interpreter->m_ResumeSafeSpindleCWCCW=ResumeDlg.m_SafeSpindleCWCCW;
		Interpreter->m_ResumeDoSafeFeedZ=ResumeDlg.m_DoSafeFeedZ;
		Interpreter->m_ResumeFeedSafeZ=ResumeDlg.m_FeedSafeZ;
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
	p->MaxAccelA = m_MaxAccelA;
	p->MaxAccelB = m_MaxAccelB;
	p->MaxAccelC = m_MaxAccelC;
	p->MaxVelX = m_MaxVelX;
	p->MaxVelY = m_MaxVelY;
	p->MaxVelZ = m_MaxVelZ;
	p->MaxVelA = m_MaxVelA;
	p->MaxVelB = m_MaxVelB;
	p->MaxVelC = m_MaxVelC;
	p->CountsPerInchX = m_CountsPerInchX;
	p->CountsPerInchY = m_CountsPerInchY;
	p->CountsPerInchZ = m_CountsPerInchZ;
	p->CountsPerInchA = m_CountsPerInchA;
	p->CountsPerInchB = m_CountsPerInchB;
	p->CountsPerInchC = m_CountsPerInchC;
	strcpy(Interpreter->ToolFile,m_ToolFile);
	strcpy(Interpreter->SetupFile,m_SetupFile);
	strcpy(Interpreter->GeoFile,m_GeoFile);
	strcpy(Interpreter->VarsFile,m_VarsFile);
	p->DegreesA = m_DegreesA!=0;
	p->DegreesB = m_DegreesB!=0;
	p->DegreesC = m_DegreesC!=0;
	p->ArcsToSegs = m_ArcsToSegs!=0;

	Interpreter->CoordMotion->SetTPParams();
}


int DoCheckShiftAndKey(int Key,WPARAM wParam, LPARAM lParam, bool &Down, bool &Down2, 
						CMotionButton &Button, CMotionButton &Button2, CMotionButton &ButtonStep)
{
	if (wParam == VK_SHIFT)
	{
		if (lParam & 0x80000000)
		{
			// shift went up
			if (Down2)
			{
				Down2=false;
				PostMessage(Button.m_hWnd,WM_LBUTTONDOWN,MK_LBUTTON,0); 
				PostMessage(Button2.m_hWnd,WM_LBUTTONUP,0,0); 
				Down=true;
			}
		}
		else
		{
			// shift went down
			if (Down)
			{
				Down=false;
				PostMessage(Button2.m_hWnd,WM_LBUTTONDOWN,MK_LBUTTON,0); 
				PostMessage(Button.m_hWnd,WM_LBUTTONUP,0,0); 
				Down2=true;
			}
		}
	}

	if (GetKeyState(VK_CONTROL)&0x80000000)
	{
		// control key kills any motion
		if (Down)
		{
			PostMessage(Button.m_hWnd,WM_LBUTTONUP,0,0); 
			Down=false;
		}
		if (Down2)
		{
			PostMessage(Button2.m_hWnd,WM_LBUTTONUP,0,0); 
			Down2=false;
		}
	}

	if (Key == wParam)
	{
		if (GetKeyState(VK_CONTROL)&0x80000000)
		{
			if ((lParam & 0x80000000)==0)
			{
				// arrow key was pressed while control was down
				PostMessage(ButtonStep.m_hWnd,WM_LBUTTONDOWN,MK_LBUTTON,0); 
			}
			else
			{
				// arrow key was released while control was down
				PostMessage(ButtonStep.m_hWnd,WM_LBUTTONUP,0,0); 
			}
		}
		else if (GetKeyState(VK_SHIFT)&0x80000000)
		{
			if (lParam & 0x80000000)
			{
				if (Down2)
				{
					PostMessage(Button2.m_hWnd,WM_LBUTTONUP,0,0); 
					Down2=false;
				}
			}
			else
			{
				if (!Down2)
				{
					PostMessage(Button2.m_hWnd,WM_LBUTTONDOWN,MK_LBUTTON,0); 
					Down2=true;
				}
			}
		}
		else
		{
			if (lParam & 0x80000000)
			{
				if (Down)
				{
					PostMessage(Button.m_hWnd,WM_LBUTTONUP,0,0); 
					Down=false;
				}
			}
			else
			{
				if (!Down)
				{
					PostMessage(Button.m_hWnd,WM_LBUTTONDOWN,MK_LBUTTON,0); 
					Down=true;
				}
			}
		}
		return 1;
	}
	return 0;
}

HHOOK       g_hhk   =   NULL;


LRESULT CALLBACK KbdProc    (   int     nCode,  // hook code
                                WPARAM  wParam, // virtual-key code
                                LPARAM  lParam  // keystroke-message information

                            )
{

	static bool LeftDown=false;
	static bool Left2Down=false;
	static bool RightDown=false;
	static bool Right2Down=false;
	static bool UpDown=false;
	static bool Up2Down=false;
	static bool DownDown=false;
	static bool Down2Down=false;
	static bool ZplusDown=false;
	static bool Zplus2Down=false;
	static bool ZminusDown=false;
	static bool Zminus2Down=false;
	static bool AplusDown=false;
	static bool Aplus2Down=false;
	static bool AminusDown=false;
	static bool Aminus2Down=false;

	if (wParam == TheFrame->GCodeDlg.m_UserButtonKeys[0])
	{
		TheFrame->GCodeDlg.SendMessageA(WM_COMMAND,IDC_But0);
	}
	else if (wParam == TheFrame->GCodeDlg.m_UserButtonKeys[1])
	{
		TheFrame->GCodeDlg.SendMessageA(WM_COMMAND,IDC_But1);
	}
	else if (wParam == TheFrame->GCodeDlg.m_UserButtonKeys[2])
	{
		TheFrame->GCodeDlg.SendMessageA(WM_COMMAND,IDC_But2);
	}
	else if (wParam == TheFrame->GCodeDlg.m_UserButtonKeys[3])
	{
		TheFrame->GCodeDlg.SendMessageA(WM_COMMAND,IDC_But3);
	}
	else if (wParam == TheFrame->GCodeDlg.m_UserButtonKeys[4])
	{
		TheFrame->GCodeDlg.SendMessageA(WM_COMMAND,IDC_But4);
	}
	else if (wParam == TheFrame->GCodeDlg.m_UserButtonKeys[5])
	{
		TheFrame->GCodeDlg.SendMessageA(WM_COMMAND,IDC_But5);
	}
	else if (wParam == TheFrame->GCodeDlg.m_UserButtonKeys[6])
	{
		TheFrame->GCodeDlg.SendMessageA(WM_COMMAND,IDC_But6);
	}
	else if (wParam == TheFrame->GCodeDlg.m_UserButtonKeys[7])
	{
		TheFrame->GCodeDlg.SendMessageA(WM_COMMAND,IDC_But7);
	}
	else if (wParam == TheFrame->GCodeDlg.m_UserButtonKeys[8])
	{
		TheFrame->GCodeDlg.SendMessageA(WM_COMMAND,IDC_But8);
	}
	else if (wParam == TheFrame->GCodeDlg.m_UserButtonKeys[9])
	{
		TheFrame->GCodeDlg.SendMessageA(WM_COMMAND,IDC_But9);
	}
	else if (TheFrame->GCodeDlg.m_KeyJogMode.Toggled && 
		!TheFrame->GCodeDlg.ThreadIsExecuting)	
	{
		if (DoCheckShiftAndKey(VK_LEFT,wParam,lParam,LeftDown,Left2Down,
			TheFrame->GCodeDlg.m_Left,TheFrame->GCodeDlg.m_Left2,TheFrame->GCodeDlg.m_LeftStep)) return 1;

		if (DoCheckShiftAndKey(VK_RIGHT,wParam,lParam,RightDown,Right2Down,
			TheFrame->GCodeDlg.m_Right,TheFrame->GCodeDlg.m_Right2,TheFrame->GCodeDlg.m_RightStep)) return 1;

		if (DoCheckShiftAndKey(VK_UP,wParam,lParam,UpDown,Up2Down,
			TheFrame->GCodeDlg.m_Up,TheFrame->GCodeDlg.m_Up2,TheFrame->GCodeDlg.m_UpStep)) return 1;

		if (DoCheckShiftAndKey(VK_DOWN,wParam,lParam,DownDown,Down2Down,
			TheFrame->GCodeDlg.m_Down,TheFrame->GCodeDlg.m_Down2,TheFrame->GCodeDlg.m_DownStep)) return 1;

		if (DoCheckShiftAndKey(VK_PRIOR,wParam,lParam,ZplusDown,Zplus2Down,
			TheFrame->GCodeDlg.m_Zplus,TheFrame->GCodeDlg.m_Zplus2,TheFrame->GCodeDlg.m_ZplusStep)) return 1;

		if (DoCheckShiftAndKey(VK_NEXT,wParam,lParam,ZminusDown,Zminus2Down,
			TheFrame->GCodeDlg.m_Zminus,TheFrame->GCodeDlg.m_Zminus2,TheFrame->GCodeDlg.m_ZminusStep)) return 1;

		if (DoCheckShiftAndKey(VK_ADD,wParam,lParam,AplusDown,Aplus2Down,
			TheFrame->GCodeDlg.m_Aplus,TheFrame->GCodeDlg.m_Aplus2,TheFrame->GCodeDlg.m_AplusStep)) return 1;

		if (DoCheckShiftAndKey(VK_SUBTRACT,wParam,lParam,AminusDown,Aminus2Down,
			TheFrame->GCodeDlg.m_Aminus,TheFrame->GCodeDlg.m_Aminus2,TheFrame->GCodeDlg.m_AminusStep)) return 1;
	}
    return  (CallNextHookEx(g_hhk,nCode,wParam,lParam));
}





BOOL CKMotionCNCDlg::OnInitDialog() 
{
	int result,i;

	CDialog::OnInitDialog(); // bypass CDlgX Init Dialog for Modal Dialogs like this

	m_Editor.SetupForGCode();

	Interpreter->CoordMotion->m_Simulate=(m_Simulate!=0);  // make sure the libraries match

	for (i=0; i<N_USER_GCODE_FILES; i++)
	{
		DisplayedThreadStat[i]=-1;     // set all undefined
		GCodeThreadActive[i]=0;
	}

	DisplayedCurrentLine=-1;

	m_ConfigSpindleDirty=true;

	if (!GreenBrush)
	{
		GreenBrush = new CBrush;
		GreenBrush->CreateSolidBrush(0xff00); 
	}
	
	if (m_Thread <0 || m_Thread >= N_USER_GCODE_FILES) m_Thread = 0;
	LoadFile(m_Thread,false);

	LastTitleText="";
	m_prevFeedRateLabel="";
	m_prevFeedRateLabelF="";
	m_prevSpindleRateLabel="";
	m_prevSpindleRateLabelF="";

	RefreshTitle();

	if (CurrentDirectory.IsEmpty())
	{
		CurrentDirectory = TheFrame->MainPathRoot + GCODE_SUB_DIR;
	}



	m_EmergencyStop.LoadBitmaps(IDB_EmergencyStop,0,0);
	m_GO.LoadBitmaps(IDB_GO,0,0);
	m_KeyJogMode.LoadBitmaps(IDB_ToggleKeyModeUp,IDB_ToggleKeyModeDown,0);
	m_KeyJogMode.ToggleType=true;
	m_FeedHold.LoadBitmaps(IDB_FeedHold,0,0);
	m_LeftStep.LoadBitmaps(IDB_LeftStep,0,0);
	m_Left.LoadBitmaps(IDB_Left,0,0);
	m_Left2.LoadBitmaps(IDB_Left2,0,0);
	m_RightStep.LoadBitmaps(IDB_LeftStep,0,0);
	m_Right.LoadBitmaps(IDB_Right,0,0);
	m_Right2.LoadBitmaps(IDB_Right2,0,0);
	
	m_DownStep.LoadBitmaps(IDB_UpStep,0,0);
	m_Down.LoadBitmaps(IDB_Down,0,0);
	m_Down2.LoadBitmaps(IDB_Down2,0,0);
	m_UpStep.LoadBitmaps(IDB_UpStep,0,0);
	m_Up.LoadBitmaps(IDB_Up,0,0);
	m_Up2.LoadBitmaps(IDB_Up2,0,0);

	m_ZplusStep.LoadBitmaps(IDB_UpStep,0,0);
	m_Zplus.LoadBitmaps(IDB_Up,0,0);
	m_Zplus2.LoadBitmaps(IDB_Up2,0,0);
	m_ZminusStep.LoadBitmaps(IDB_UpStep,0,0);
	m_Zminus.LoadBitmaps(IDB_Down,0,0);
	m_Zminus2.LoadBitmaps(IDB_Down2,0,0);

	m_AminusStep.LoadBitmaps(IDB_LeftStep,0,0);
	m_Aminus.LoadBitmaps(IDB_Left,0,0);
	m_Aminus2.LoadBitmaps(IDB_Left2,0,0);
	m_AplusStep.LoadBitmaps(IDB_LeftStep,0,0);
	m_Aplus.LoadBitmaps(IDB_Right,0,0);
	m_Aplus2.LoadBitmaps(IDB_Right2,0,0);

	m_BminusStep.LoadBitmaps(IDB_LeftStep,0,0);
	m_Bminus.LoadBitmaps(IDB_Left,0,0);
	m_Bminus2.LoadBitmaps(IDB_Left2,0,0);
	m_BplusStep.LoadBitmaps(IDB_LeftStep,0,0);
	m_Bplus.LoadBitmaps(IDB_Right,0,0);
	m_Bplus2.LoadBitmaps(IDB_Right2,0,0);

	m_CminusStep.LoadBitmaps(IDB_LeftStep,0,0);
	m_Cminus.LoadBitmaps(IDB_Left,0,0);
	m_Cminus2.LoadBitmaps(IDB_Left2,0,0);
	m_CplusStep.LoadBitmaps(IDB_LeftStep,0,0);
	m_Cplus.LoadBitmaps(IDB_Right,0,0);
	m_Cplus2.LoadBitmaps(IDB_Right2,0,0);

	m_StopStep.LoadBitmaps(IDB_StopStep,0,0);

	if (m_Lathe)
	{
		m_RightStep.Init	(this, 2, 1,m_JogSlowPercent*0.01,true);
		m_Right.Init		(this, 2, 1,m_JogSlowPercent*0.01,false);
		m_Right2.Init		(this, 2, 1,1.0,false);
		m_LeftStep.Init		(this, 2,-1,m_JogSlowPercent*0.01,true);
		m_Left.Init			(this, 2,-1,m_JogSlowPercent*0.01,false);
		m_Left2.Init		(this, 2,-1,1.0,false);

		m_ZplusStep.Init	(this, 0, 1,m_JogSlowPercent*0.01,true);
		m_Zplus.Init		(this, 0, 1,m_JogSlowPercent*0.01,false);
		m_Zplus2.Init		(this, 0, 1,1.0,false);
		m_ZminusStep.Init	(this, 0,-1,m_JogSlowPercent*0.01,true);
		m_Zminus.Init		(this, 0,-1,m_JogSlowPercent*0.01,false);
		m_Zminus2.Init		(this, 0,-1,1.0,false);
	}
	else
	{
		m_RightStep.Init	(this, 0, 1,m_JogSlowPercent*0.01,true);
		m_Right.Init		(this, 0, 1,m_JogSlowPercent*0.01,false);
		m_Right2.Init		(this, 0, 1,1.0,false);
		m_LeftStep.Init		(this, 0,-1,m_JogSlowPercent*0.01,true);
		m_Left.Init			(this, 0,-1,m_JogSlowPercent*0.01,false);
		m_Left2.Init		(this, 0,-1,1.0,false);

		m_ZplusStep.Init	(this, 2, 1,m_JogSlowPercent*0.01,true);
		m_Zplus.Init		(this, 2, 1,m_JogSlowPercent*0.01,false);
		m_Zplus2.Init		(this, 2, 1,1.0,false);
		m_ZminusStep.Init	(this, 2,-1,m_JogSlowPercent*0.01,true);
		m_Zminus.Init		(this, 2,-1,m_JogSlowPercent*0.01,false);
		m_Zminus2.Init		(this, 2,-1,1.0,false);
	}

	m_UpStep.Init		(this, 1, 1,m_JogSlowPercent*0.01,true);
	m_Up.Init			(this, 1, 1,m_JogSlowPercent*0.01,false);
	m_Up2.Init			(this, 1, 1,1.0,false);
	m_DownStep.Init		(this, 1,-1,m_JogSlowPercent*0.01,true);
	m_Down.Init			(this, 1,-1,m_JogSlowPercent*0.01,false);
	m_Down2.Init		(this, 1,-1,1.0,false);


	m_AplusStep.Init	(this, 3, 1,m_JogSlowPercent*0.01,true);
	m_Aplus.Init		(this, 3, 1,m_JogSlowPercent*0.01,false);
	m_Aplus2.Init		(this, 3, 1,1.0,false);
	m_AminusStep.Init	(this, 3,-1,m_JogSlowPercent*0.01,true);
	m_Aminus.Init		(this, 3,-1,m_JogSlowPercent*0.01,false);
	m_Aminus2.Init		(this, 3,-1,1.0,false);

	m_BplusStep.Init	(this, 4, 1,m_JogSlowPercent*0.01,true);
	m_Bplus.Init		(this, 4, 1,m_JogSlowPercent*0.01,false);
	m_Bplus2.Init		(this, 4, 1,1.0,false);
	m_BminusStep.Init	(this, 4,-1,m_JogSlowPercent*0.01,true);
	m_Bminus.Init		(this, 4,-1,m_JogSlowPercent*0.01,false);
	m_Bminus2.Init		(this, 4,-1,1.0,false);

	m_CplusStep.Init	(this, 5, 1,m_JogSlowPercent*0.01,true);
	m_Cplus.Init		(this, 5, 1,m_JogSlowPercent*0.01,false);
	m_Cplus2.Init		(this, 5, 1,1.0,false);
	m_CminusStep.Init	(this, 5,-1,m_JogSlowPercent*0.01,true);
	m_Cminus.Init		(this, 5,-1,m_JogSlowPercent*0.01,false);
	m_Cminus2.Init		(this, 5,-1,1.0,false);



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
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	m_hIcon = AfxGetApp()->LoadIcon(IDI_KMotionCNC16);
	SetIcon(m_hIcon,FALSE);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_KMotionCNC32);
	SetIcon(m_hIcon,TRUE);
	

	
	m_GCodeTools = new CDlgToolBar;

	// Create toolbar at the top of the dialog window
	if (m_GCodeTools->Create(this))
	{
	    result = m_GCodeTools->LoadToolBar(IDR_GCodeBar);
	}      
	
	m_GCodeTools->SetBarStyle(m_GCodeTools->GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);


	// And position the control bars
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);


	m_FeedSlider.SetRange(OVERRIDE_MIN,OVERRIDE_MAX,100,-6,RGB(0,128,128));
	m_FeedSlider.SetTicFreq(5);
	m_FeedSlider.SetPos(1.0);

	m_SpindleSlider.SetRange(OVERRIDE_MIN,OVERRIDE_MAX,100,-34,RGB(192,0,0));
	m_SpindleSlider.SetTicFreq(5);
	m_SpindleSlider.SetPos(1.0);

	SetTimer(0,(int)(STATUS_TIME*1000),NULL);

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

	if (m_DialogFace == 8)
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
	
	m_Editor.SetupForGCode();

	FillComboWithCount(1,99,&m_tool);
	FillComboWithCountFixture(1,9,&m_FixtureOffset);

	SetUserButtons();

	SetMotionParams();

	Interpreter->CoordMotion->SetSpindleRateOverride(m_SpindleRateValue);
	Interpreter->CoordMotion->SetFeedRateOverride(m_FeedRateValue);

	Interpreter->ReadSetupFile();

	EnableToolTips();

	Interpreter->CoordMotion->Kinematics->ReadGeoTable(m_GeoFile);

	SetStepSizes();

	CSS_BitmapValid=false;
	CSS_BitmapDisplayed=false;

    g_hhk   =   SetWindowsHookEx    (   WH_KEYBOARD,
                                        ( HOOKPROC) KbdProc,
                                        NULL,
                                        GetCurrentThreadId ()
                                    );

	INIT_EASYSIZE;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
	
void StraightTraverseCallback(double x, double y, double z, double a, double b, double c, int sequence_number)
{
	MOTION_PARAMS *MP=&TheFrame->GCodeDlg.Interpreter->CoordMotion->Kinematics->m_MotionParams;

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

	TheFrame->GViewDlg.m_view.OpenGLMutex->Lock();
	TheFrame->GViewDlg.m_Path->AddVertex(new CVertex3d(x,y,z,TheFrame->GCodeDlg.m_ColorTraverse,sequence_number,0));
	TheFrame->GViewDlg.m_Path->SetModified();
	TheFrame->GViewDlg.m_view.OpenGLMutex->Unlock();
}
	
void StraightFeedCallback(double DesiredFeedRate_in_per_sec,
							   double x, double y, double z, double a, double b, double c, int sequence_number, int ID)

{
	MOTION_PARAMS *MP=&TheFrame->GCodeDlg.Interpreter->CoordMotion->Kinematics->m_MotionParams;

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

	TheFrame->GViewDlg.m_view.OpenGLMutex->Lock();
	TheFrame->GViewDlg.m_Path->AddVertex(new CVertex3d(x,y,z,TheFrame->GCodeDlg.m_ColorFeed,sequence_number,ID));
	TheFrame->GViewDlg.m_Path->SetModified();
	TheFrame->GViewDlg.m_view.OpenGLMutex->Unlock();
}

#define N_SEG 50.0

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
	MOTION_PARAMS *MP=&TheFrame->GCodeDlg.Interpreter->CoordMotion->Kinematics->m_MotionParams;


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
	// but actually for now just do 50 sided circles

	n_segs = (int)fabs(N_SEG * d_theta/TWO_PI) + 1;

	TheFrame->GViewDlg.m_view.OpenGLMutex->Lock();
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

		TheFrame->GViewDlg.m_Path->AddVertex(new CVertex3d(x,y,z,TheFrame->GCodeDlg.m_ColorFeed,sequence_number,ID));
	}
	TheFrame->GViewDlg.m_Path->SetModified();
	TheFrame->GViewDlg.m_view.OpenGLMutex->Unlock();
}
	
void CKMotionCNCDlg::OnHalt() 
{
	Interpreter->Halt();
}



HBRUSH CKMotionCNCDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDlgX::OnCtlColor(pDC, pWnd, nCtlColor);
	
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
				return hbr;
			}
		}
	}
	return hbr;
}


void CKMotionCNCDlg::OnTimer(UINT nIDEvent) 
{
	static int Entry=0;
	int result;

	if (Entry>0) return;  // do not allow re-entries

	Entry++;

	if (nIDEvent==2)
	{
		*SaveUserCommandVar=SaveUserCommand;
		SaveUserCommandCombo->SetWindowText(SaveUserCommand);
		SaveUserCommandCombo->SetEditSel(SaveUserCommand.GetLength(),SaveUserCommand.GetLength());
		KillTimer(2);
		CDlgX::OnTimer(nIDEvent);
	}
	else
	{
		if (ReadStatus)
		{
			result=TheFrame->KMotionDLL->WaitToken(false,100.0);

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
				}
				
				TheFrame->KMotionDLL->ReleaseToken();

				UpdateScreen(true);

				ServiceKFLOPCommands();
			}
			else
			{
				m_ConnectedForStatus=false;
				UpdateScreen(false);
			}

			CString NewTitle;

			if (m_Thread != -1)
			{
				if (result != KMOTION_NOT_CONNECTED)
					NewTitle = 	"KMotionCNC - Connected - " + FileNames[m_Thread];
				else
					NewTitle = 	"KMotionCNC - Disconnected - " + FileNames[m_Thread];
			}

			CString Elapsed;

			if (ThreadIsExecuting && JobStartTimeValid)
			{
				Elapsed.Format("  -  Run Time:%10.1f seconds",ElapsedTimer.Elapsed_Seconds()-JobStartTimeSecs);
			}
			else if (JobStartTimeValid && JobEndTimeValid)
			{
				Elapsed.Format("  -  Run Time:%10.1f seconds",JobEndTimeSecs-JobStartTimeSecs);
			}

			NewTitle += Elapsed;

			if (LastTitleText != NewTitle)
			{
				LastTitleText = NewTitle;
				SetWindowText(LastTitleText);
			}
		}

		DoJoyStick();


		int ThreadStat;
		
		// check if thread status is different from displayed

		for (int i=0; i<N_USER_GCODE_FILES; i++)
		{
			ThreadStat = GCodeThreadActive[i];
			if (DisplayedThreadStat[i] != ThreadStat)
				GetDlgItem(NumberToThreadID(i))->InvalidateRect(NULL);
		}

		
		GCodeMutex->Lock();

		if (CurrentLine[m_Thread]    != DisplayedCurrentLine ||
			ThreadHadError[m_Thread] != DisplayedThreadHadError)
		{
			m_Editor.MarkerDeleteAll(0);

			if (ThreadHadError[m_Thread])
			{
				m_Editor.MarkerSetFore(0,0x0000ff);
				m_Editor.MarkerSetBack(0,0x0000ff);
			}
			else
			{
				m_Editor.MarkerSetFore(0,FLAG_COLOR);
				m_Editor.MarkerSetBack(0,FLAG_COLOR);
			}
			
			m_Editor.MarkerDefine(0,SC_MARK_ARROW);
			m_Editor.MarkerAdd(CurrentLine[m_Thread], 0);

			int LinesInView = m_Editor.LinesOnScreen();
			m_Editor.GotoLine(CurrentLine[m_Thread]);
			m_Editor.SetVisiblePolicy(CARET_STRICT+CARET_EVEN,0);
			m_Editor.EnsureVisibleEnforcePolicy(CurrentLine[m_Thread]);

			m_Editor.GotoLine(CurrentLine[m_Thread]);

			DisplayedCurrentLine=CurrentLine[m_Thread];
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
	double x,y,z,a,b,c,*dp;
	float F,NewValue;
	CString s;
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
		OnEmergencyStop();
		SetKFLOPCommandResult(0);
		break;

	case PC_COMM_HALT:
		OnHalt();
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
		if (NewValue > 0.001 && NewValue < 1000.0)   // ignore if crazy number
		{
			m_FeedRateValue = NewValue;
			s.Format("%.2f",m_FeedRateValue);
			SetDlgItemText(IDC_FeedRateEdit,s);
			Interpreter->CoordMotion->SetFeedRateOverride(m_FeedRateValue);
			m_FeedSlider.SetPos(m_FeedRateValue);
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_SSO:
		NewValue = *(float *)&MainStatus.PC_comm[1];
		if (NewValue > 0.001 && NewValue < 1000.0)   // ignore if crazy number
		{
			m_SpindleRateValue = NewValue;
			s.Format("%.2f",m_SpindleRateValue);
			SetDlgItemText(IDC_SpindleRateEdit,s);
			Interpreter->CoordMotion->SetSpindleRateOverride(m_SpindleRateValue);
			m_SpindleSlider.SetPos(m_SpindleRateValue);
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_FRO_INC:
		NewValue = *(float *)&MainStatus.PC_comm[1] * m_FeedRateValue;
		if (NewValue > 0.001 && NewValue < 1000.0)   // ignore if crazy number
		{
			m_FeedRateValue = NewValue;
			s.Format("%.2f",m_FeedRateValue);
			SetDlgItemText(IDC_FeedRateEdit,s);
			Interpreter->CoordMotion->SetFeedRateOverride(m_FeedRateValue);
			m_FeedSlider.SetPos(m_FeedRateValue);
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_SSO_INC:
		NewValue = *(float *)&MainStatus.PC_comm[1] * m_SpindleRateValue;
		if (NewValue > 0.001 && NewValue < 1000.0)   // ignore if crazy number
		{
			m_SpindleRateValue = NewValue;
			s.Format("%.2f",m_SpindleRateValue);
			SetDlgItemText(IDC_SpindleRateEdit,s);
			Interpreter->CoordMotion->SetSpindleRateOverride(m_SpindleRateValue);
			m_SpindleSlider.SetPos(m_SpindleRateValue);
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_X:
		NewValue = *(float *)&MainStatus.PC_comm[1];
		if (!ThreadIsExecuting && !ReadInterpPos(&x,&y,&z,&a,&b,&c))
		{
			Interpreter->p_setup->axis_offset_x += x - NewValue;
			Interpreter->p_setup->parameters[5211]=Interpreter->p_setup->axis_offset_x;
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_Y:
		NewValue = *(float *)&MainStatus.PC_comm[1];
		if (!ThreadIsExecuting && !ReadInterpPos(&x,&y,&z,&a,&b,&c))
		{
			Interpreter->p_setup->axis_offset_y += y - NewValue;
			Interpreter->p_setup->parameters[5212]=Interpreter->p_setup->axis_offset_y;
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_Z:
		NewValue = *(float *)&MainStatus.PC_comm[1];
		if (!ThreadIsExecuting && !ReadInterpPos(&x,&y,&z,&a,&b,&c))
		{
			Interpreter->p_setup->axis_offset_z += z - NewValue;
			Interpreter->p_setup->parameters[5213]=Interpreter->p_setup->axis_offset_z;
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_A:
		NewValue = *(float *)&MainStatus.PC_comm[1];
		if (!ThreadIsExecuting && !ReadInterpPos(&x,&y,&z,&a,&b,&c))
		{
			Interpreter->p_setup->AA_axis_offset += a - NewValue;
			Interpreter->p_setup->parameters[5214]=Interpreter->p_setup->AA_axis_offset;
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_B:
		NewValue = *(float *)&MainStatus.PC_comm[1];
		if (!ThreadIsExecuting && !ReadInterpPos(&x,&y,&z,&a,&b,&c))
		{
			Interpreter->p_setup->BB_axis_offset += b - NewValue;
			Interpreter->p_setup->parameters[5215]=Interpreter->p_setup->BB_axis_offset;
			SetKFLOPCommandResult(0);
		}
		else
		{
			SetKFLOPCommandResult(-1);
		}
		break;

	case PC_COMM_SET_C:
		NewValue = *(float *)&MainStatus.PC_comm[1];
		if (!ThreadIsExecuting && !ReadInterpPos(&x,&y,&z,&a,&b,&c))
		{
			Interpreter->p_setup->CC_axis_offset += c - NewValue;
			Interpreter->p_setup->parameters[5216]=Interpreter->p_setup->CC_axis_offset;
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
			MsgDisplayed=true;
			MainStatus.PC_comm[0]=0;  // clear the command now that it has been executed
			result = AfxMessageBox(s,MainStatus.PC_comm[2]);
			MsgDisplayed=false;
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
			SetKFLOPCommandResult(1);

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
				if (DoGCodeLine(s))
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
				SetKFLOPCommandResult(1);
				break;
			}

			index=MainStatus.PC_comm[1];
			if (index>=0 && index <128)
			{
				dp = &Interpreter->p_setup->tool_table[index+1].length;
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
				SetKFLOPCommandResult(2);
			}
		}
		break;

	case PC_COMM_GET_TOOLTABLE_LENGTH:
		{
			CEditToolFile EditToolFile;
			// first read the Tool Table File
			if (EditToolFile.LoadFile(m_ToolFile))
			{
				SetKFLOPCommandResult(1);
				break;
			}
			index=MainStatus.PC_comm[1];
			if (index>=0 && index <20)
			{
				double d = Interpreter->p_setup->tool_table[index+1].length;
				if (SendOneDouble(MainStatus.PC_comm[2], d)) break;
				SetKFLOPCommandResult(0);
			}
			else
			{
				SetKFLOPCommandResult(2);
			}
		}
		break;


	case PC_COMM_GET_MISC_SETTINGS:  // Units, T, H, D indexes 
		{
			s.Format("SetPersistHex%d %x",MainStatus.PC_comm[1],Interpreter->p_setup->length_units);
			if (TheFrame->KMotionDLL->WriteLine(s)) break;
			s.Format("SetPersistHex%d %x",MainStatus.PC_comm[1]+1,Interpreter->p_setup->selected_tool_slot);
			if (TheFrame->KMotionDLL->WriteLine(s)) break;
			s.Format("SetPersistHex%d %x",MainStatus.PC_comm[1]+2,Interpreter->p_setup->length_offset_index); // H number
			if (TheFrame->KMotionDLL->WriteLine(s)) break;
			s.Format("SetPersistHex%d %x",MainStatus.PC_comm[1]+3,Interpreter->p_setup->tool_table_index); // D number
			if (TheFrame->KMotionDLL->WriteLine(s)) break;
			SetKFLOPCommandResult(0);
		}
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

			if (p->length_units==CANON_UNITS_INCHES)
			{
				// for inches round to 6 digits
				for (int axis=0; axis<NFIXAXES; axis++)
					Vars[5221+Fix*20+axis] = floor(Vars[5221+Fix*20+axis] * 1e6 + 0.5) / 1e6;
			}
			else if (p->length_units==CANON_UNITS_MM)
			{
				// for mm round to 4 digits
				for (int axis=0; axis<NFIXAXES; axis++)
					Vars[5221+Fix*20+axis] = floor(Vars[5221+Fix*20+axis] * 1e4 + 0.5) / 1e4;
			}

			p->origin_index=-1; // set invalid so it updates
			Interpreter->ChangeFixtureNumber(m_LastFixtureDisplayed);

			SetKFLOPCommandResult(0);
		}
		break;

	default:
		if (!ErrorDisplayed)
		{
			ErrorDisplayed=true;
			AfxMessageBox("Invalid External Command Received");
			ErrorDisplayed=false;
		}
		break;
		}
	}

	MainStatus.PC_comm[0]=0;  // clear the command now that it has been executed
}


int CKMotionCNCDlg::SendCoordinates(int n, bool MachineCoords)
{
	double ActsDest[MAX_ACTUATORS],x,y,z,a,b,c;
	CCoordMotion *CM = Interpreter->CoordMotion;

	for (int i=0; i<MAX_ACTUATORS; i++) ActsDest[i]=0.0;

	if (CM->x_axis >=0)	ActsDest[CM->x_axis] = MainStatus.Dest[CM->x_axis];
	if (CM->y_axis >=0)	ActsDest[CM->y_axis] = MainStatus.Dest[CM->y_axis];
	if (CM->z_axis >=0)	ActsDest[CM->z_axis] = MainStatus.Dest[CM->z_axis];
	if (CM->a_axis >=0)	ActsDest[CM->a_axis] = MainStatus.Dest[CM->a_axis];
	if (CM->b_axis >=0)	ActsDest[CM->b_axis] = MainStatus.Dest[CM->b_axis];
	if (CM->c_axis >=0)	ActsDest[CM->c_axis] = MainStatus.Dest[CM->c_axis];

	CM->Kinematics->TransformActuatorstoCAD(ActsDest,&x,&y,&z,&a,&b,&c);

	if (MachineCoords)
		Interpreter->ConvertAbsoluteToMachine(x,y,z,a,b,c,&x,&y,&z,&a,&b,&c);
	else
		Interpreter->ConvertAbsoluteToInterpreterCoord(x,y,z,a,b,c,&x,&y,&z,&a,&b,&c);

	if (SendOneDouble(n,  x)) return 1;
	if (SendOneDouble(n+1,y)) return 1;
	if (SendOneDouble(n+2,z)) return 1;
	if (SendOneDouble(n+3,a)) return 1;
	if (SendOneDouble(n+4,b)) return 1;
	if (SendOneDouble(n+5,c)) return 1;
	return 0;
}

int CKMotionCNCDlg::SendOneDouble(int i, double d)
{
	CString s,r;

	s.Format("SetPersistHex%d %x",i*2,((int*)&d)[0]);
	if (TheFrame->KMotionDLL->WriteLine(s)) return 1;
	s.Format("SetPersistHex%d %x",i*2+1,((int*)&d)[1]);
	if (TheFrame->KMotionDLL->WriteLine(s)) return 1;
	return 0;
}

int CKMotionCNCDlg::GetVar(int Var, int *value)
{
	CString s,r;

	if (Var >= 0 && Var < 200)
	{
		s.Format("GetPersistHex %d",Var);
		if(TheFrame->KMotionDLL->WriteLineReadLine(s, r.GetBufferSetLength(100))) return 1;
		r.ReleaseBuffer();
		sscanf(r,"%x",value);
	}
	return 0;
}



int CKMotionCNCDlg::GetStringFromGather(int WordOffset, CString *msg, int nWords)
{
	CString s;

	TheFrame->KMotionDLL->WaitToken();
	s.Format("GetGatherHex %d %d",WordOffset,nWords);  
	TheFrame->KMotionDLL->WriteLine(s);

	for (int i=0;i<nWords;i++)  // convert hex to 32 bit words
	{
		if ((i%8)==0)  // every 8 hex get a new line
		{
			s.Empty();
			if (TheFrame->KMotionDLL->ReadLineTimeOut( s.GetBuffer(MAX_LINE+1),5000)) return 1;  
			s.ReleaseBuffer();
		}

		int result = sscanf(s.GetBuffer(0),"%8X ",msg->GetBufferSetLength(nWords*4+8)+i*4);

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



int CKMotionCNCDlg::SetKFLOPCommandResult(int r)
{
	CString s;

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
	bool lathe_changed,slow_jog_changed;

	CAllToolSetupSheet Dlg("Tool Setup Screen",this);
	DisableKeyJog();

	Dlg.m_ToolSetupFilesPage.m_ToolFile = m_ToolFile;
	Dlg.m_ToolSetupFilesPage.m_SetupFile = m_SetupFile;
	Dlg.m_ToolSetupFilesPage.m_GeoFile = m_GeoFile;
	Dlg.m_ToolSetupFilesPage.m_VarsFile = m_VarsFile;
	Dlg.m_ToolSetupTPPage.m_BreakAngle = m_BreakAngle;
	Dlg.m_ToolSetupTPPage.m_CollinearTol = m_CollinearTol;
	Dlg.m_ToolSetupTPPage.m_CornerTol = m_CornerTol;
	Dlg.m_ToolSetupTPPage.m_FacetAngle = m_FacetAngle;
	Dlg.m_ToolSetupTPPage.m_ReverseRZ = m_ReverseRZ;
	Dlg.m_ToolSetupTPPage.m_EnableGamePad = m_EnableGamePad;
	Dlg.m_ToolSetupTPPage.m_ZeroUsingFixtures = m_ZeroUsingFixtures;
	Dlg.m_ToolSetupTPPage.m_ArcsToSegs = m_ArcsToSegs;
	Dlg.m_ToolSetupTPPage.m_DisplayEncoder = m_DisplayEncoder;
	Dlg.m_ToolSetupTPPage.m_DegreesA = m_DegreesA;
	Dlg.m_ToolSetupTPPage.m_DegreesB = m_DegreesB;
	Dlg.m_ToolSetupTPPage.m_DegreesC = m_DegreesC;
	Dlg.m_ToolSetupTPPage.m_Lathe = m_Lathe;
	Dlg.m_ToolSetupTPPage.m_JogSlowPercent = m_JogSlowPercent;
	Dlg.m_ToolSetupTPPage.m_JogSpeedX = m_JogSpeed[0];
	Dlg.m_ToolSetupTPPage.m_JogSpeedY = m_JogSpeed[1];
	Dlg.m_ToolSetupTPPage.m_JogSpeedZ = m_JogSpeed[2];
	Dlg.m_ToolSetupTPPage.m_JogSpeedA = m_JogSpeed[3];
	Dlg.m_ToolSetupTPPage.m_JogSpeedB = m_JogSpeed[4];
	Dlg.m_ToolSetupTPPage.m_JogSpeedC = m_JogSpeed[5];
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
	Dlg.m_ToolSetupTPPage.m_CountsPerInchC = m_CountsPerInchC;
	Dlg.m_ToolSetupTPPage.m_CountsPerInchB = m_CountsPerInchB;
	Dlg.m_ToolSetupTPPage.m_CountsPerInchA = m_CountsPerInchA;
	Dlg.m_ToolSetupTPPage.m_CountsPerInchX = m_CountsPerInchX;
	Dlg.m_ToolSetupTPPage.m_CountsPerInchY = m_CountsPerInchY;
	Dlg.m_ToolSetupTPPage.m_CountsPerInchZ = m_CountsPerInchZ;
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
	Dlg.m_ToolSetupFilesPage.m_DialogFace=m_DialogFace;
	memcpy(&Dlg.m_ToolSetupM1Page.McodeActions[0],&Interpreter->McodeActions[0],MAX_MCODE_ACTIONS_M1*sizeof(MCODE_ACTION));
	memcpy(&Dlg.m_ToolSetupButtonsPage.McodeActions[MAX_MCODE_ACTIONS_M1],&Interpreter->McodeActions[MAX_MCODE_ACTIONS_M1],MAX_MCODE_ACTIONS_BUTTONS*sizeof(MCODE_ACTION));
	memcpy(&Dlg.m_ToolSetupM100Page.McodeActions[0],&Interpreter->McodeActions[MAX_MCODE_ACTIONS_M1+MAX_MCODE_ACTIONS_BUTTONS],MAX_MCODE_ACTIONS_M100*sizeof(MCODE_ACTION));
	memcpy(Dlg.m_ToolSetupButtonsPage.UserButtonKeys,m_UserButtonKeys,NUSERBUTTONS*sizeof(int));


	if (Dlg.DoModal() == IDOK)
	{
		m_ToolFile  = Dlg.m_ToolSetupFilesPage.m_ToolFile;
		m_SetupFile = Dlg.m_ToolSetupFilesPage.m_SetupFile;
		m_GeoFile = Dlg.m_ToolSetupFilesPage.m_GeoFile;
		m_VarsFile = Dlg.m_ToolSetupFilesPage.m_VarsFile;
		m_BreakAngle = Dlg.m_ToolSetupTPPage.m_BreakAngle;
		m_CollinearTol = Dlg.m_ToolSetupTPPage.m_CollinearTol;
		m_CornerTol = Dlg.m_ToolSetupTPPage.m_CornerTol;
		m_FacetAngle = Dlg.m_ToolSetupTPPage.m_FacetAngle;
		m_ReverseRZ = Dlg.m_ToolSetupTPPage.m_ReverseRZ;
		m_EnableGamePad = Dlg.m_ToolSetupTPPage.m_EnableGamePad;
		m_ZeroUsingFixtures = Dlg.m_ToolSetupTPPage.m_ZeroUsingFixtures;
		m_ArcsToSegs = Dlg.m_ToolSetupTPPage.m_ArcsToSegs;
		m_DisplayEncoder = Dlg.m_ToolSetupTPPage.m_DisplayEncoder;
		m_DegreesA = Dlg.m_ToolSetupTPPage.m_DegreesA;
		m_DegreesB = Dlg.m_ToolSetupTPPage.m_DegreesB;
		m_DegreesC = Dlg.m_ToolSetupTPPage.m_DegreesC;
		lathe_changed = (m_Lathe != Dlg.m_ToolSetupTPPage.m_Lathe);
		m_Lathe = Dlg.m_ToolSetupTPPage.m_Lathe;
		slow_jog_changed = (m_JogSlowPercent != Dlg.m_ToolSetupTPPage.m_JogSlowPercent);
		m_JogSlowPercent = Dlg.m_ToolSetupTPPage.m_JogSlowPercent;
		m_JogSpeed[0] = Dlg.m_ToolSetupTPPage.m_JogSpeedX;
		m_JogSpeed[1] = Dlg.m_ToolSetupTPPage.m_JogSpeedY;
		m_JogSpeed[2] = Dlg.m_ToolSetupTPPage.m_JogSpeedZ;
		m_JogSpeed[3] = Dlg.m_ToolSetupTPPage.m_JogSpeedA;
		m_JogSpeed[4] = Dlg.m_ToolSetupTPPage.m_JogSpeedB;
		m_JogSpeed[5] = Dlg.m_ToolSetupTPPage.m_JogSpeedC;
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
		m_CountsPerInchC = Dlg.m_ToolSetupTPPage.m_CountsPerInchC;
		m_CountsPerInchB = Dlg.m_ToolSetupTPPage.m_CountsPerInchB;
		m_CountsPerInchA = Dlg.m_ToolSetupTPPage.m_CountsPerInchA;
		m_CountsPerInchX = Dlg.m_ToolSetupTPPage.m_CountsPerInchX;
		m_CountsPerInchY = Dlg.m_ToolSetupTPPage.m_CountsPerInchY;
		m_CountsPerInchZ = Dlg.m_ToolSetupTPPage.m_CountsPerInchZ;
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

		memcpy(&Interpreter->McodeActions[0],&Dlg.m_ToolSetupM1Page.McodeActions[0],MAX_MCODE_ACTIONS_M1*sizeof(MCODE_ACTION));
		memcpy(&Interpreter->McodeActions[MAX_MCODE_ACTIONS_M1],&Dlg.m_ToolSetupButtonsPage.McodeActions[MAX_MCODE_ACTIONS_M1],MAX_MCODE_ACTIONS_BUTTONS*sizeof(MCODE_ACTION));
		memcpy(&Interpreter->McodeActions[MAX_MCODE_ACTIONS_M1+MAX_MCODE_ACTIONS_BUTTONS],&Dlg.m_ToolSetupM100Page.McodeActions[0],MAX_MCODE_ACTIONS_M100*sizeof(MCODE_ACTION));
		memcpy(m_UserButtonKeys,Dlg.m_ToolSetupButtonsPage.UserButtonKeys,NUSERBUTTONS*sizeof(int));
	
		SetMotionParams();

		Interpreter->CoordMotion->Kinematics->ReadGeoTable(m_GeoFile);
		
		SetUserButtons();
		SetStepSizes();
		SaveConfig();

		m_ConfigSpindleDirty=true;  // remember to resend parameters

		if (m_DialogFace != Dlg.m_ToolSetupFilesPage.m_DialogFace || lathe_changed || slow_jog_changed)
		{
			m_DialogFace = Dlg.m_ToolSetupFilesPage.m_DialogFace;
			DestroyWindow();
			Create(IDD_KMOTIONCNC_0_ORIGINAL+m_DialogFace);  // put up the real main window
			m_LastFixtureDisplayed=m_LastToolDisplayed=-1;
		}
	}
}




void CKMotionCNCDlg::OnUpdateRestart(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(CurrentLine[m_Thread]!=0 && !ThreadIsExecuting);
	m_Zplus2.EnableWindow(!ThreadIsExecuting);
	m_Zplus.EnableWindow(!ThreadIsExecuting);
	m_ZplusStep.EnableWindow(!ThreadIsExecuting);
	m_Zminus2.EnableWindow(!ThreadIsExecuting);
	m_Zminus.EnableWindow(!ThreadIsExecuting);
	m_ZminusStep.EnableWindow(!ThreadIsExecuting);
	m_Aplus2.EnableWindow(!ThreadIsExecuting);
	m_Aplus.EnableWindow(!ThreadIsExecuting);
	m_AplusStep.EnableWindow(!ThreadIsExecuting);
	m_Aminus2.EnableWindow(!ThreadIsExecuting);
	m_Aminus.EnableWindow(!ThreadIsExecuting);
	m_AminusStep.EnableWindow(!ThreadIsExecuting);

	m_Bplus2.EnableWindow(!ThreadIsExecuting);
	m_Bplus.EnableWindow(!ThreadIsExecuting);
	m_BplusStep.EnableWindow(!ThreadIsExecuting);
	m_Bminus2.EnableWindow(!ThreadIsExecuting);
	m_Bminus.EnableWindow(!ThreadIsExecuting);
	m_BminusStep.EnableWindow(!ThreadIsExecuting);

	m_Cplus2.EnableWindow(!ThreadIsExecuting);
	m_Cplus.EnableWindow(!ThreadIsExecuting);
	m_CplusStep.EnableWindow(!ThreadIsExecuting);
	m_Cminus2.EnableWindow(!ThreadIsExecuting);
	m_Cminus.EnableWindow(!ThreadIsExecuting);
	m_CminusStep.EnableWindow(!ThreadIsExecuting);

	m_Right2.EnableWindow(!ThreadIsExecuting);
	m_Right.EnableWindow(!ThreadIsExecuting);
	m_RightStep.EnableWindow(!ThreadIsExecuting);
	m_Down2.EnableWindow(!ThreadIsExecuting);
	m_Down.EnableWindow(!ThreadIsExecuting);
	m_DownStep.EnableWindow(!ThreadIsExecuting);
	m_Up2.EnableWindow(!ThreadIsExecuting);
	m_Up.EnableWindow(!ThreadIsExecuting);
	m_UpStep.EnableWindow(!ThreadIsExecuting);
	m_Left2.EnableWindow(!ThreadIsExecuting);
	m_Left.EnableWindow(!ThreadIsExecuting);
	m_LeftStep.EnableWindow(!ThreadIsExecuting);
	m_StopStep.EnableWindow(!ThreadIsExecuting);
}

void CKMotionCNCDlg::OnUpdateSingleStep(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!ThreadIsExecuting);
}

int CKMotionCNCDlg::SetExecutionPoint(int line)
{
	CurrentLine[m_Thread] = line;
	Interpreter->CoordMotion->m_PreviouslyStopped = STOPPED_NONE;
	if (SaveFile(m_Thread,false)) return 1;  // don't force the save
	Interpreter->DoReverseSearch(FileNames[m_Thread],line);
	return 0;
}

void CKMotionCNCDlg::OnGView() 
{
	CreateDlgOrBringToTop(IDD_GView,&TheFrame->GViewDlg);
}

void CKMotionCNCDlg::OnSimulate() 
{
	UpdateData();	
	Interpreter->CoordMotion->m_Simulate=(m_Simulate!=0);
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
	
	CDlgX::OnVScroll(nSBCode, nPos, pScrollBar);

	int Dlg=pScrollBar->GetDlgCtrlID();

	if (Dlg==IDC_FeedSlider)
	{
		m_FeedRateValue = m_FeedSlider.GetPos();

		// round to 2 digits so fits in display
		m_FeedRateValue = ((int)(m_FeedRateValue*100.0 + 0.5))/100.0;

		s.Format("%.2f",m_FeedRateValue);

		SetDlgItemText(IDC_FeedRateEdit,s);
		
		Interpreter->CoordMotion->SetFeedRateOverride(m_FeedRateValue);
	}
	else if (Dlg==IDC_SpindleSlider)
	{
		m_SpindleRateValue = m_SpindleSlider.GetPos();

		// round to 2 digits so fits in display
		m_SpindleRateValue = ((int)(m_SpindleRateValue*100.0 + 0.5))/100.0;

		s.Format("%.2f",m_SpindleRateValue);

		SetDlgItemText(IDC_SpindleRateEdit,s);
		
		Interpreter->CoordMotion->SetSpindleRateOverride(m_SpindleRateValue);
		Interpreter->InvokeAction(10,FALSE);  // resend new Speed
	}
}


void CKMotionCNCDlg::OnFeedRateApply() 
{
	if (!UpdateData()) return;

	m_FeedSlider.SetPos(m_FeedRateValue);

	Interpreter->CoordMotion->SetFeedRateOverride(m_FeedRateValue);
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
	m_ColorTraverse.Set(255,0,0);
	m_ColorFeed.Set(0,255,0);

	GreenBrush=NULL;


	GCodeMutex = new CMutex(FALSE,"GCodeInterpreter",NULL);
	
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
	m_BreakAngle = 30.0;
	m_CollinearTol = 0.0002;
	m_CornerTol = 0.0002;
	m_FacetAngle = 0.5;
	m_ReverseRZ = false;
	m_EnableGamePad = true;
	m_ZeroUsingFixtures = false;
	m_ArcsToSegs = true;
	m_DisplayEncoder = false;
	m_DegreesA = false;
	m_DegreesB = false;
	m_DegreesC = false;
	m_Lathe = false;
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
	m_MaxVelC = 0.1;
	m_MaxVelB = 0.1;
	m_MaxVelA = 0.1;
	m_MaxVelX = 0.1;
	m_MaxVelY = 0.1;
	m_MaxVelZ = 0.1;
	m_CountsPerInchC = 100.0;
	m_CountsPerInchB = 100.0;
	m_CountsPerInchA = 100.0;
	m_CountsPerInchX = 100.0;
	m_CountsPerInchY = 100.0;
	m_CountsPerInchZ = 100.0;
	m_Step0 = 0.0001;
	m_Step1 = 0.001;
	m_Step2 = 0.01;
	m_Step3 = 0.1;
	m_Step4 = 1;
	m_Step5 = 10;
	m_DialogFace = 0;
	m_SpindleType = 0;
	m_SpindleAxis = 4;
	m_SpindleUpdateTime=0.1;
	m_SpindleTau = 0.1;
	m_SpindleCntsPerRev = 1000;
	m_SafeZ = 0.0;
	m_SafeRelAbs = 0;


	//{{AFX_DATA_INIT(CKMotionCNCDlg)
	m_Thread = -1;
	m_Simulate = FALSE;
	m_ShowLineNumbers = FALSE;
	m_ShowMach = FALSE;
	m_FeedRateValue = 1.0;
	m_SpindleRateValue = 1.0;
	m_CommandString = _T("");
	m_StepSize = 0;
	//}}AFX_DATA_INIT
	

	for (i=0; i<6; i++)
	{
		CS_axis[i]=-1;
		m_JogSpeed[i]=1;
	}

	m_JogSlowPercent=25.0;


	if (LoadLibrary("SciLexer.DLL")==NULL)
	{
		MessageBox("The Scintilla DLL could not be loaded.",
		"Error loading Scintilla",
		MB_OK | MB_ICONERROR);
	}

	m_hAccelTable=LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));

	board=0;
	ReadStatus=true;
	CurAbsX=CurAbsY=CurAbsZ=CurAbsA=CurAbsB=CurAbsC=0;
	m_Joyvx = m_Joyvy = m_Joyvz = m_Joyva = m_Joyvb = m_Joyvc = 0.0;
	m_JoyMovedx = false;
	m_JoyMovedy = false;
	m_JoyMovedz = false;
	m_JoyMoveda = false;
	m_JoyMovedb = false;
	m_JoyMovedc = false;
	m_LastFixtureDisplayed=m_LastToolDisplayed=-1;

	m_RestoreStoppedState=false;
	m_SaveStoppedState=0;
	m_BulkStatusCount=0;

	JobStartTimeValid=JobEndTimeValid=false;

	m_DoingSimulationRun=false;
	m_ConnectedForStatus=false;

	for (int i=0; i<NUSERBUTTONS; i++)
		m_UserButtonKeys[i]=-1;
}

CKMotionCNCDlg::~CKMotionCNCDlg()
{
	if (m_GCodeTools)
		delete m_GCodeTools;

	if (GreenBrush) delete GreenBrush;

	delete GCodeMutex;
}



void CKMotionCNCDlg::OnDropdownCommand() 
{
	UpdateData(TRUE);
	m_Command.ResetContent();
	UpdateData(FALSE);
	
	SaveUserCommand=m_CommandString;
	SaveUserCommandVar=&m_CommandString;
	SaveUserCommandCombo=&m_Command;

	// add them in backwards skipping blank lines
	
	for (int i=0; i<NCOMMAND_HISTORY; i++)
		if (!CommandHistory[i].IsEmpty())
			m_Command.AddString(CommandHistory[i]);
}

void CKMotionCNCDlg::OnCloseupCommand() 
{
	int i = m_Command.GetCurSel();
	
	if (i!=-1) m_Command.GetLBText(i, SaveUserCommand);
	
	SetTimer(2,1,NULL);  // put the edit control how we wish after going idle
}






void CKMotionCNCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
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
	if (AfxMessageBox("ABORT?",MB_ICONQUESTION|MB_YESNO)==IDYES)
		OnEmergencyStop();
}

void CKMotionCNCDlg::OnF2() 
{
	m_KeyJogMode.Toggled = !m_KeyJogMode.Toggled;
	m_KeyJogMode.Invalidate();

	CheckDlgButton(IDC_KeyJogMode,!IsDlgButtonChecked(IDC_KeyJogMode));
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
	return CDlgX::PreTranslateMessage(pMsg);
}


int CKMotionCNCDlg::DoJoyStick()
{
	JOYINFOEX ji;
	CCoordMotion *CM=Interpreter->CoordMotion;
	unsigned long RtoUse, ZtoUse;
	static bool FirstEmergencyStop=true;
	static bool FirstSingleStep=true;
	static bool FirstHalt=true; 
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

		if (ji.dwButtons &0x10) // slow mode ??
		{
			FirstHalt=false;
			OnHalt();
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
			SetDlgItemText(IDC_FeedRateEdit,"2.0");
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
			SetDlgItemText(IDC_FeedRateEdit,"0.5");
			PostMessage(WM_COMMAND,IDC_FeedRateApply);
		}

		if (ji.dwPOV == 0xffff && ji.dwButtons == 0)  // all buttons are completely released
		{
			FirstEmergencyStop=true;
			FirstSingleStep=true;
			FirstHalt=true;
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

		m_Joyvx = DoJoyAxis(0,       ji.dwXpos);
		m_Joyvy = DoJoyAxis(1,0xffff-ji.dwYpos);
		m_Joyvz = DoJoyAxis(2,0xffff-RtoUse   );
		m_Joyva = DoJoyAxis(3,       ZtoUse   );
	}
	else
	{
		// joystick not connected
		//
		// in case of loss of connection stop any jogs

		m_Joyvx = m_Joyvy = m_Joyvz = m_Joyva = 0.0;
	}


	if (m_Simulate)
	{
		CM->current_x += m_Joyvx * STATUS_TIME; //Sim for 0.1 sec
		CM->current_y += m_Joyvy * STATUS_TIME; 
		CM->current_z += m_Joyvz * STATUS_TIME; 
		CM->current_a += m_Joyva * STATUS_TIME;
		CM->current_b += m_Joyvb * STATUS_TIME;
		CM->current_c += m_Joyvc * STATUS_TIME;
	}
	else
	{
		ProcessChangeInJogVelocity();
	}

	return 0;
}

int CKMotionCNCDlg::ProcessChangeInJogVelocity()
{
	static bool DoAFinalMove = false;
	static bool WaitingForFinalMove = false;
	CCoordMotion *CM=Interpreter->CoordMotion;
	double v[6];

	if (DoAFinalMove)
	{
		// check if we came to a stop yet (either from Jog to stop, or from offset)

		CString response;
		if (TheFrame->KMotionDLL->WriteLineReadLine("CheckDoneXYZABC",response.GetBufferSetLength(MAX_LINE))) return 1;
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
		}


		if (!WaitingForFinalMove)
		{
			// do a final move to either where the axis was originally
			// or to where it currently is depending on whether that
			// axis was ever moved or not


			double ActsDest[MAX_ACTUATORS];

			for (int i=0; i<MAX_ACTUATORS; i++) ActsDest[i]=0.0;

			if (CM->x_axis >=0) if (CM->GetDestination(CM->x_axis,&ActsDest[0])) {CM->SetAbort(); return 1;}
			if (CM->y_axis >=0) if (CM->GetDestination(CM->y_axis,&ActsDest[1])) {CM->SetAbort(); return 1;}
			if (CM->z_axis >=0) if (CM->GetDestination(CM->z_axis,&ActsDest[2])) {CM->SetAbort(); return 1;}
			if (CM->a_axis >=0) if (CM->GetDestination(CM->a_axis,&ActsDest[3])) {CM->SetAbort(); return 1;}
			if (CM->b_axis >=0) if (CM->GetDestination(CM->b_axis,&ActsDest[4])) {CM->SetAbort(); return 1;}
			if (CM->c_axis >=0) if (CM->GetDestination(CM->c_axis,&ActsDest[5])) {CM->SetAbort(); return 1;}

			CM->Kinematics->TransformActuatorstoCAD(ActsDest,&CurAbsX,&CurAbsY,&CurAbsZ,&CurAbsA,&CurAbsB,&CurAbsC);

			double x = m_JoyMovedx ? CurAbsX : m_Joyx0;
			double y = m_JoyMovedy ? CurAbsY : m_Joyy0;
			double z = m_JoyMovedz ? CurAbsZ : m_Joyz0;
			double a = m_JoyMoveda ? CurAbsA : m_Joya0;
			double b = m_JoyMovedb ? CurAbsB : m_Joyb0;
			double c = m_JoyMovedc ? CurAbsC : m_Joyc0;
		
			double Acts[MAX_ACTUATORS];

			CM->Kinematics->TransformCADtoActuators(x, y, z, a, b, c, Acts);

			for (int i=0; i<MAX_ACTUATORS; i++)
				if (DoActPosition(i,Acts[i])) return 1;

			m_JoyMovedx = false;
			m_JoyMovedy = false;
			m_JoyMovedz = false;
			m_JoyMoveda = false;
			m_JoyMovedb = false;
			m_JoyMovedc = false;

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

		DoAFinalMove = WaitingForFinalMove = false;
	}


	v[0] = m_Joyvx;
	v[1] = m_Joyvy;
	v[2] = m_Joyvz;
	v[3] = m_Joyva;
	v[4] = m_Joyvb;
	v[5] = m_Joyvc;

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


	// Check if we are to stop
	
	if (v[0]==0 && v[1]==0 && v[2]==0 && v[3]==0 && v[4]==0 && v[5]==0)
	{
		// check if we had been moving

		if (m_JoyMovedx || m_JoyMovedy || m_JoyMovedz || m_JoyMoveda || m_JoyMovedb || m_JoyMovedc)
		{
			for (int i=0; i<MAX_ACTUATORS; i++)
				if (DoActVelocity(i,0.0)) return 1;

			// Unless we are in FeedHold (likely due to a limit condition)do a final move
			CString response;
			if (TheFrame->KMotionDLL->WriteLineReadLine("GetStopState",response.GetBufferSetLength(MAX_LINE))) return 1;
			response.ReleaseBuffer();
			if (response == '0')
			{
				DoAFinalMove = true;
			}
			else
			{
				m_JoyMovedx=m_JoyMovedy=m_JoyMovedz=m_JoyMoveda=m_JoyMovedb=m_JoyMovedc=false;
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
			m_JoyMovedx = false;
			m_JoyMovedy = false;
			m_JoyMovedz = false;
			m_JoyMoveda = false;
			m_JoyMovedb = false;
			m_JoyMovedc = false;
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

		// if an axis was never moved since the last time we were still
		// then drive it back toward it's original coordinate at a rate
		// such that it should be there by the next timer

		if (!m_JoyMovedx)
			v[0] = (m_Joyx0 - CurAbsX)/STATUS_TIME;

		if (!m_JoyMovedy)
			v[1] = (m_Joyy0 - CurAbsY)/STATUS_TIME;

		if (!m_JoyMovedz)
			v[2] = (m_Joyz0 - CurAbsZ)/STATUS_TIME;

		if (!m_JoyMoveda)
			v[3] = (m_Joya0 - CurAbsA)/STATUS_TIME;

		if (!m_JoyMovedb)
			v[4] = (m_Joyb0 - CurAbsB)/STATUS_TIME;

		if (!m_JoyMovedc)
			v[5] = (m_Joyc0 - CurAbsC)/STATUS_TIME;


		double Acts0[MAX_ACTUATORS];
		double Acts1[MAX_ACTUATORS];

		double x0 = CurAbsX;
		double y0 = CurAbsY;
		double z0 = CurAbsZ;
		double a0 = CurAbsA;
		double b0 = CurAbsB;
		double c0 = CurAbsC;

		double x1 = x0 + v[0] * STATUS_TIME;
		double y1 = y0 + v[1] * STATUS_TIME;
		double z1 = z0 + v[2] * STATUS_TIME;
		double a1 = a0 + v[3] * STATUS_TIME;
		double b1 = b0 + v[4] * STATUS_TIME;
		double c1 = c0 + v[5] * STATUS_TIME;

		CM->Kinematics->TransformCADtoActuators(x0, y0, z0, a0, b0, c0, Acts0);
		CM->Kinematics->TransformCADtoActuators(x1, y1, z1, a1, b1, c1, Acts1);

		for (int i=0; i<MAX_ACTUATORS; i++)
			if (DoActVelocity(i,(Acts1[i]-Acts0[i])/STATUS_TIME)) return 1;
	}
	return 0;
}


// command an actuator to a specified velocity

int CKMotionCNCDlg::DoActVelocity(int i, double v)
{
	static double LastSpeed[6]={1e99,1e99,1e99,1e99,1e99,1e99};
	CString s;
	
	if (CS_axis[i]>=0)
	{
		// command a distance (jog) of 2.0 seconds at that speed
		// so in the case we loose communication it will still stop

		if (fabs(v) > 0) 
			s.Format("MoveRelAtVel%d=%f %f",CS_axis[i],v*2.0,fabs(v));
		else if (LastSpeed[i] != 0)
			s.Format("Jog%d=%f",CS_axis[i],v);

		if (TheFrame->KMotionDLL->WriteLine(s)) return 1;
	}
	LastSpeed[i]=v;
	
	return 0;
}


// command an actuator to stop

int CKMotionCNCDlg::StopAxis(int i)
{
	CString s;
	
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
	CString s;
	
	if (CS_axis[i]>=0)
	{
		// command a move to an abs position

		s.Format("Move%d=%f",CS_axis[i],p);
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

	v *= m_JogSpeed[axis] * m_JogSpeedFactor / 32768.0;

	return v;
}




int CKMotionCNCDlg::GetStatus()
{
	int i,result,n;
	CString s;
	int *p=(int *)&MainStatus;

	int HostStatus=0;

	if (ThreadIsExecuting) HostStatus += HOST_JOB_ACTIVE_BIT;


	// KMotion is available read the status
	s.Format("GetStatus %x",HostStatus);  
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
		
		result = sscanf(s.GetBuffer(0),"%8X",p++);

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
					int result = AfxMessageBox("Error: Status Record Size mismatch\r\r"
						"Disable further status updates?",MB_ICONSTOP|MB_YESNO);

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
	CString s,response;

	if (KMotionPresent)
	{
		// first find out which axis is which

		if (TheFrame->KMotionDLL->WriteLineReadLine("DefineCS",response.GetBufferSetLength(MAX_LINE))) return 1;
		response.ReleaseBuffer();
		result=sscanf(response, "%d%d%d%d%d%d",&CS_axis[0],&CS_axis[1],&CS_axis[2],&CS_axis[3],&CS_axis[4],&CS_axis[5]);
		if (result != 6)  return 1;
	}
	else
	{
		m_ConfigSpindleDirty=true;  // remember we were disconnected
	}
	
	SetBigValues(&m_PosX, &m_PosY, &m_PosZ, &m_PosA, &m_PosB, &m_PosC, KMotionPresent);


	
	if (Interpreter->p_setup->distance_mode==MODE_ABSOLUTE)
		CheckRadioButton(IDC_Rel,IDC_Abs,IDC_Abs);
	else
		CheckRadioButton(IDC_Rel,IDC_Abs,IDC_Rel);

	if (Interpreter->p_setup->length_units==CANON_UNITS_INCHES)
		CheckRadioButton(IDC_mm,IDC_inch,IDC_inch);
	else if (Interpreter->p_setup->length_units==CANON_UNITS_MM)
		CheckRadioButton(IDC_mm,IDC_inch,IDC_mm);

	if (m_LastToolDisplayed != Interpreter->p_setup->selected_tool_slot)
	{
		m_LastToolDisplayed=Interpreter->p_setup->selected_tool_slot;
		m_tool.SetCurSel(m_LastToolDisplayed-1);
	}

	if (m_LastFixtureDisplayed != Interpreter->p_setup->origin_index)
	{
		m_LastFixtureDisplayed=Interpreter->p_setup->origin_index;
		m_FixtureOffset.SetCurSel(m_LastFixtureDisplayed-1);
	}

	UINT up,down,disabled;
	m_FeedHold.GetBitmaps(up,down,disabled);

	if (MainStatus.StopImmediateState>0 && up!=IDB_FeedHoldPause)
	{
		m_FeedHold.LoadBitmaps(IDB_FeedHoldPause,0,0);
		m_FeedHold.Invalidate(0);
	}
	else if (MainStatus.StopImmediateState==0 && up!=IDB_FeedHold)
	{
		m_FeedHold.LoadBitmaps(IDB_FeedHold,0,0);
		m_FeedHold.Invalidate(0);
	}

	m_GO.GetBitmaps(up,down,disabled);

	if (ThreadIsExecuting && up!=IDB_Halt)
	{
		m_GO.LoadBitmaps(IDB_Halt,0,0);
		m_GO.Invalidate(0);
	}
	else if (!ThreadIsExecuting && up!=IDB_GO)
	{
		m_GO.LoadBitmaps(IDB_GO,0,0);
		m_GO.Invalidate(0);
	}

	return 0;
}


void CKMotionCNCDlg::SetBigValueColor(CDisplay *Disp,int axis, bool KMotionPresent, bool DisplayedEnc)
{
	if (m_Simulate)
		Disp->SetTextColor(0xFFFFFF);
	else if (KMotionPresent && (MainStatus.Enables & (1<<CS_axis[axis])) && CS_axis[axis] >= 0)
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


void CKMotionCNCDlg::SetBigValues(CDisplay *Disp0, CDisplay *Disp1, CDisplay *Disp2, CDisplay *Disp3, CDisplay *Disp4, CDisplay *Disp5, bool KMotionPresent)
{
	double x,y,z,a,b,c;
	CString s;
	CCoordMotion *CM=Interpreter->CoordMotion;
	bool DisplayedEnc0,DisplayedEnc1,DisplayedEnc2,DisplayedEnc3,DisplayedEnc4,DisplayedEnc5;


	if (KMotionPresent && !m_Simulate)
	{
		// find out which axis is which

		if (CM->GetAxisDefinitions(&CM->x_axis,&CM->y_axis,&CM->z_axis,&CM->a_axis,&CM->b_axis,&CM->c_axis)) {CM->SetAbort(); return;}

		// read and set all axis (if undefined return interpreter)

		double Acts[MAX_ACTUATORS],ActsDest[MAX_ACTUATORS];

		for (int i=0; i<MAX_ACTUATORS; i++) Acts[i]=0.0;

		// Get either the Actuator Position or Dest depending on whether there is
		// a feedback device and the User is requesting to display feedback and
		// also the current Destination regardless.
		if (GetAxisDRO(CM->x_axis,&Acts[0],&ActsDest[0],&DisplayedEnc0))return;
		if (GetAxisDRO(CM->y_axis,&Acts[1],&ActsDest[1],&DisplayedEnc1))return;
		if (GetAxisDRO(CM->z_axis,&Acts[2],&ActsDest[2],&DisplayedEnc2))return;
		if (GetAxisDRO(CM->a_axis,&Acts[3],&ActsDest[3],&DisplayedEnc3))return;
		if (GetAxisDRO(CM->b_axis,&Acts[4],&ActsDest[4],&DisplayedEnc4))return;
		if (GetAxisDRO(CM->c_axis,&Acts[5],&ActsDest[5],&DisplayedEnc5))return;

		CM->Kinematics->TransformActuatorstoCAD(Acts,&x,&y,&z,&a,&b,&c);
		CM->Kinematics->TransformActuatorstoCAD(ActsDest,&CurAbsX,&CurAbsY,&CurAbsZ,&CurAbsA,&CurAbsB,&CurAbsC);
	}
	else
	{
		x = Interpreter->CoordMotion->current_x;
		y = Interpreter->CoordMotion->current_y;
		z = Interpreter->CoordMotion->current_z;
		a = Interpreter->CoordMotion->current_a;
		b = Interpreter->CoordMotion->current_b;
		c = Interpreter->CoordMotion->current_c;
		DisplayedEnc0=DisplayedEnc1=DisplayedEnc2=DisplayedEnc3=DisplayedEnc4=DisplayedEnc5=false;
	}

	SetBigValueColor(Disp0,0,KMotionPresent,DisplayedEnc0);
	SetBigValueColor(Disp1,1,KMotionPresent,DisplayedEnc1);
	SetBigValueColor(Disp2,2,KMotionPresent,DisplayedEnc2);
	SetBigValueColor(Disp3,3,KMotionPresent,DisplayedEnc3);
	SetBigValueColor(Disp4,4,KMotionPresent,DisplayedEnc4);
	SetBigValueColor(Disp5,5,KMotionPresent,DisplayedEnc5);

	// display current feed rate

	if (m_BulkStatusCount>1 && Interpreter->p_setup->length_units == prev_length_units)
	{
		double dt = (MainStatus.TimeStamp - PrevMainStatusTimeStamp);

		if (dt>0.0)
		{
			double dx = (CM->x_axis >=0) ? x-PrevDROx : 0.0;
			double dy = (CM->y_axis >=0) ? y-PrevDROy : 0.0;
			double dz = (CM->z_axis >=0) ? z-PrevDROz : 0.0;
			double da = (CM->a_axis >=0) ? a-PrevDROa : 0.0;
			double db = (CM->b_axis >=0) ? b-PrevDROb : 0.0;
			double dc = (CM->c_axis >=0) ? c-PrevDROc : 0.0;

			BOOL pure_angle;
			double Dist=CM->FeedRateDistance(dx, dy, dz, da, db, dc, &pure_angle);

			CString s,sF;
			double SpeedToShow;
			double DesiredFeedRate = Interpreter->p_setup->feed_rate;

			if (prev_length_units == CANON_UNITS_MM)
			{
				SpeedToShow=Dist/dt*60.0*25.4;
				s.Format("%7.0f",SpeedToShow);
				sF.Format("%7.0f",DesiredFeedRate);
			}
			else
			{
				SpeedToShow=Dist/dt*60.0;
				s.Format("%7.1f",SpeedToShow);
				sF.Format("%7.1f",DesiredFeedRate);
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

			// Do Spindle Control Updates
			double DesiredSpindleRate = Interpreter->p_setup->speed;

			if (m_ConfigSpindleDirty)
			{
				if (Interpreter->CoordMotion->ConfigSpindle(m_SpindleType, m_SpindleAxis,m_SpindleUpdateTime, m_SpindleTau, m_SpindleCntsPerRev)) return;
				m_ConfigSpindleDirty=false;
			}

			float speed_rps=0;
			if (Interpreter->CoordMotion->GetSpindleRPS(speed_rps)) return;

			SpeedToShow=speed_rps*60.0;  

			// if in CSS convert RPM to feet/min or Meters/min depending on units
			if (Interpreter->p_setup->spindle_mode == CANON_SPINDLE_CSS)
			{
				double xp,yp,zp,ap,bp,cp;
				Interpreter->ConvertAbsoluteToInterpreterCoord(x,y,z,a,b,c,&xp,&yp,&zp,&ap,&bp,&cp);

				if (Interpreter->p_setup->length_units==CANON_UNITS_MM)
					SpeedToShow *= TWO_PI * fabs(xp) * 0.001; // convert x (radius) to meters and multiply RPM by 2 PI R
				else
					SpeedToShow *= TWO_PI * fabs(xp) / 12.0; // convert x (radius) to feet and multiply RPM by 2 PI R

				if (!CSS_BitmapValid || !CSS_BitmapDisplayed)
				{
					CSS_BitmapValid=CSS_BitmapDisplayed=true;
					CStatic*  m_picture=(CStatic *)GetDlgItem(IDC_Cust2);    // pointer to a picture control
					HBITMAP hb = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SRCSS), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR|LR_SHARED);
					m_picture->SetBitmap(hb);
				}
			}
			else
			{
				if (!CSS_BitmapValid || CSS_BitmapDisplayed)
				{
					CSS_BitmapValid=true;
					CSS_BitmapDisplayed=false;
					CStatic*  m_picture=(CStatic *)GetDlgItem(IDC_Cust2);    // pointer to a picture control
					HBITMAP hb = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SR), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR|LR_SHARED);
					m_picture->SetBitmap(hb);
				}
			}

			s.Format("%7.1f",SpeedToShow);
			sF.Format("%7.1f",DesiredSpindleRate);

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
				m_SpindleSlider.PlotInstant(SpeedToShow/DesiredSpindleRate);

		}
	}

	PrevMainStatusTimeStamp=MainStatus.TimeStamp;
	prev_length_units = Interpreter->p_setup->length_units; 
	PrevDROx=x;
	PrevDROy=y;
	PrevDROz=z;
	PrevDROa=a;
	PrevDROb=b;
	PrevDROc=c;



	// convert absolute machine's position in inches to 
	// the Interpreter's position that may include offsets
	// and metric units

	if (m_ShowMach)
		Interpreter->ConvertAbsoluteToMachine(x,y,z,a,b,c,&x,&y,&z,&a,&b,&c);
	else
		Interpreter->ConvertAbsoluteToInterpreterCoord(x,y,z,a,b,c,&x,&y,&z,&a,&b,&c);

	s.Format(" X:%10.4f ",x);
	KillMinusZero(s);
	Disp0->SetText(s);

	s.Format(" Y:%10.4f ",y);
	KillMinusZero(s);
	Disp1->SetText(s);

	s.Format(" Z:%10.4f ",z);
	KillMinusZero(s);
	Disp2->SetText(s);

	s.Format(" A:%10.4f ",a);
	KillMinusZero(s);
	Disp3->SetText(s);

	s.Format(" B:%10.4f ",b);
	KillMinusZero(s);
	Disp4->SetText(s);

	s.Format(" C:%10.4f ",c);
	KillMinusZero(s);
	Disp5->SetText(s);


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
	else
		return ((MainStatus.InputModes2 >> ((axis-4)*4)) & 0xf) == NO_INPUT_MODE;
}

// format shows very small negative numbers as -0
// which looks bad, so just change to zero

void CKMotionCNCDlg::KillMinusZero(CString &s)
{
	int i=3;
	int n=s.GetLength();

	while (s.GetAt(i)==' ' && i<n) i++;  // skip over leading zeros
	
	if (s.GetAt(i) != '-') return;  // if no minus sign, no problen

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
	CString s;
	int i;

	for (i=1;i<N_USER_THREADS;i++)   // kill all user programs except Thread #1
	{
		s.Format("Kill %d", i+1);

		if (TheFrame->KMotionDLL->WriteLine(s)) return;
	}

	Interpreter->Halt();
	
	Interpreter->Abort();

	for (i=0;i<N_CHANNELS;i++)             // disable all servo channels
	{
		s.Format("DISABLEAXIS%d",i);
		if (TheFrame->KMotionDLL->WriteLine(s)) return;
	}
}



void CKMotionCNCDlg::OnZeroAll() 
{
	if (AfxMessageBox("Zero All Axes?",MB_YESNO|MB_ICONQUESTION) == IDYES)
	{
		OnZeroX();
		OnZeroY();
		OnZeroZ();
		OnZeroA();
		OnZeroB();
		OnZeroC();
	}
}

void CKMotionCNCDlg::OnZeroX() 
{
	double x,y,z,a,b,c;
	setup_pointer ps = Interpreter->p_setup;

	if (ReadInterpPos(&x,&y,&z,&a,&b,&c)) return;
	if (m_ZeroUsingFixtures)
	{
		int Fix = ps->origin_index-1;
		ps->origin_offset_x += x;
		ps->parameters[5221+Fix*20+0]=ps->origin_offset_x;
	}
	else
	{
		ps->axis_offset_x += x;
		ps->parameters[5211]=ps->axis_offset_x;
	}
}

void CKMotionCNCDlg::OnZeroY() 
{
	double x,y,z,a,b,c;
	setup_pointer ps = Interpreter->p_setup;

	if (ReadInterpPos(&x,&y,&z,&a,&b,&c)) return;
	if (m_ZeroUsingFixtures)
	{
		int Fix = ps->origin_index-1;
		ps->origin_offset_y += y;
		ps->parameters[5221+Fix*20+1]=ps->origin_offset_y;
	}
	else
	{
		Interpreter->p_setup->axis_offset_y += y;
		Interpreter->p_setup->parameters[5212]=Interpreter->p_setup->axis_offset_y;
	}
}

void CKMotionCNCDlg::OnZeroZ() 
{
	double x,y,z,a,b,c;
	setup_pointer ps = Interpreter->p_setup;

	if (ReadInterpPos(&x,&y,&z,&a,&b,&c)) return;
	if (m_ZeroUsingFixtures)
	{
		int Fix = ps->origin_index-1;
		ps->origin_offset_z += z;
		ps->parameters[5221+Fix*20+2]=ps->origin_offset_z;
	}
	else
	{
		Interpreter->p_setup->axis_offset_z += z;
		Interpreter->p_setup->parameters[5213]=Interpreter->p_setup->axis_offset_z;
	}
}

void CKMotionCNCDlg::OnZeroA() 
{
	double x,y,z,a,b,c;
	setup_pointer ps = Interpreter->p_setup;

	if (ReadInterpPos(&x,&y,&z,&a,&b,&c)) return;
	if (m_ZeroUsingFixtures)
	{
		int Fix = ps->origin_index-1;
		ps->AA_origin_offset += a;
		ps->parameters[5221+Fix*20+3]=ps->AA_origin_offset;
	}
	else
	{
		Interpreter->p_setup->AA_axis_offset += a;
		Interpreter->p_setup->parameters[5214]=Interpreter->p_setup->AA_axis_offset;
	}
}

void CKMotionCNCDlg::OnZeroB() 
{
	double x,y,z,a,b,c;
	setup_pointer ps = Interpreter->p_setup;

	if (ReadInterpPos(&x,&y,&z,&a,&b,&c)) return;
	if (m_ZeroUsingFixtures)
	{
		int Fix = ps->origin_index-1;
		ps->BB_origin_offset += b;
		ps->parameters[5221+Fix*20+4]=ps->BB_origin_offset;
	}
	else
	{
		Interpreter->p_setup->BB_axis_offset += b;
		Interpreter->p_setup->parameters[5215]=Interpreter->p_setup->BB_axis_offset;
	}
}

void CKMotionCNCDlg::OnZeroC() 
{
	double x,y,z,a,b,c;
	setup_pointer ps = Interpreter->p_setup;

	if (ReadInterpPos(&x,&y,&z,&a,&b,&c)) return;
	if (m_ZeroUsingFixtures)
	{
		int Fix = ps->origin_index-1;
		ps->CC_origin_offset += c;
		ps->parameters[5221+Fix*20+5]=ps->CC_origin_offset;
	}
	else
	{
		Interpreter->p_setup->CC_axis_offset += c;
		Interpreter->p_setup->parameters[5216]=Interpreter->p_setup->CC_axis_offset;
	}
}

void CKMotionCNCDlg::MakeSureFileIsntReadOnly(CString FN)
{
	FN=TheFrame->MainPathRoot+FN;
	if (_access(FN,00)==0)						// check if file exists
		if (!_access(FN,06)==0)					// check if doesn't have r/w
			_chmod(FN,_S_IREAD | _S_IWRITE);	// make it r/w
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
	int tool = m_tool.GetCurSel()+1;
	m_LastToolDisplayed = tool;

	CString s;
	s.Format("T%dM6",tool);
	DoGCodeLine(s);
}

void CKMotionCNCDlg::OnClose() 
{
	DWORD t0 = timeGetTime();

	ShuttingDownApplication=TRUE;

	// Flag other process thread to terminate

	while (ThreadIsExecuting && timeGetTime() < t0+3000)
	{
		Interpreter->Abort();
	}


	if (PersistRestored ||  // check if valid parameters were loaded on startup
			AfxMessageBox("Configuration File was not properly loaded on startup.\r\r"
			              "Would you like to save the current configuration?",MB_YESNO|MB_ICONINFORMATION)
					  == IDYES)

	{
		CString File = TheFrame->MainPathRoot + PERSISTANT_FILE;
		
		FILE *f=fopen(File.GetBuffer(0),"wt");
		
		if (f)
		{
			fprintf(f,"Version 4\n");

			// be nice and remember what directory we were last time

			fprintf(f,"%s\n",CurrentDirectory);
			fprintf(f,"%d\n",m_ShowLineNumbers);

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

void CKMotionCNCDlg::FillComboWithCount(int i0, int i1, CComboBox *Box)
{
	CString s;

	Box->ResetContent();

	for (int i=i0; i<=i1; i++)
	{
		s.Format("%d",i);
		Box->AddString(s);
	}
}

void CKMotionCNCDlg::FillComboWithCountFixture(int i0, int i1, CComboBox *Box)
{
	CString s;

	Box->ResetContent();

	for (int i=i0; i<=i1; i++)
	{
		if (i<7)
			s.Format("%d - G%d",i,53+i);
		else
			s.Format("%d - G%.1f",i,59+0.1*(i-6));

		Box->AddString(s);
	}
}

void CKMotionCNCDlg::OnSend() 
{
	if (!UpdateData()) return;

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
			AfxMessageBox("Error - Interpreter Busy");
			return 1;
		}
	}
	
	FILE *f=fopen(File,"wt");
	
	if (!f)
	{
		AfxMessageBox("Unable to open Temporary File:\r\r" + File);
		return 1;
	}

	fwrite(G + '\n',G.GetLength()+1,1,f);

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


void CKMotionCNCDlg::CreateDlgOrBringToTop(UINT ID, CDialog *Dlg)
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


int CKMotionCNCDlg::ExternalRestore(void)
{
	// Save for everybody what directory we are installed in
	CString Param1;
	int nParams,NewBoard;
	LPWSTR *CL= CommandLineToArgvW(GetCommandLineW(),&nParams);
	TheFrame->MainPath = CL[0];
	if (nParams>1)
	{
		Param1=CL[1];

		if (Param1.Left(2) == "0x" || Param1.Left(2) == "0X")
			Param1.Delete(0,2);
		
		sscanf(Param1,"%x",&NewBoard);

		board=NewBoard;
	}
	GlobalFree(CL);


	TheFrame->MainPath.Replace("\"","");  // remove quotes
	TheFrame->MainPath.TrimRight();
	TheFrame->MainPath.TrimLeft();

	int LastSlash=TheFrame->MainPath.ReverseFind('\\');
	TheFrame->MainPath=TheFrame->MainPath.Left(LastSlash);


	// Check if we are running from the debug directory
	// if we are, then strip it off

	if (TheFrame->MainPath.Right(6).CompareNoCase("\\debug") == 0)
	{
		TheFrame->MainPath = TheFrame->MainPath.Left(TheFrame->MainPath.GetLength()-6);
	}

	// Check if we are running from the release directory
	// if we are, then strip it off

	if (TheFrame->MainPath.Right(8).CompareNoCase("\\release") == 0)
	{
		TheFrame->MainPath = TheFrame->MainPath.Left(TheFrame->MainPath.GetLength()-8);
	}

	// Now set the root install directory

	if (TheFrame->MainPath.Right(8).CompareNoCase("\\KMotion") == 0)
	{
		TheFrame->MainPathRoot = TheFrame->MainPath.Left(TheFrame->MainPath.GetLength()-8);
	}

	MakeSureFileIsntReadOnly("\\KMotion\\Data\\GCodeConfigCNC.txt");
	MakeSureFileIsntReadOnly("\\KMotion\\Data\\GFilesCNC.txt");
	MakeSureFileIsntReadOnly("\\KMotion\\Data\\GViewer.txt");
	MakeSureFileIsntReadOnly("\\KMotion\\Data\\persistCNC.ini");
	

	// be nice and put the current directory back to where
	// it was last time, also all the window positions 

	CString File = TheFrame->MainPathRoot + PERSISTANT_FILE;
	
	FILE *f=fopen(File,"rt");
	
	if (f)
	{
		CString Version;
		fgets(Version.GetBufferSetLength(100),100,f);
		Version.ReleaseBuffer();

		if (Version=="Version 3\n" || Version=="Version 4\n")  // don't attempt to read if incompatable
		{
			fgets(CurrentDirectory.GetBufferSetLength(MAX_PATH),MAX_PATH,f);
			CurrentDirectory.ReleaseBuffer();
			SetCurrentDirectory(CurrentDirectory);

			if (Version=="Version 4\n")  // Version 4 adds Line number option
			{
				fscanf(f,"%d",&m_ShowLineNumbers);
				//restore the state
			}

			if (m_ShowLineNumbers!=0 && m_ShowLineNumbers!=1) m_ShowLineNumbers=0;

			RestoreOnStart(f);
			TheFrame->GViewDlg.RestoreOnStart(f);

		}

		fclose(f);
		PersistRestored=true;
	}
	else
	{
		if (AfxMessageBox("Previous Configuration File:\r\r" + File +
			          "\r\rcould not be read.  Continuing will cause a loss of all\r"
					  "settings.  Are you sure you would like to continue?",MB_YESNO|MB_ICONSTOP)
					  != IDYES)
		exit(0);
		PersistRestored=false;
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

int CKMotionCNCDlg::ReadInterpPos(double *x, double *y, double *z, double *a, double *b, double *c) 
{
	bool KMotionPresent = TheFrame->KMotionDLL->WaitToken(false,100.0) == KMOTION_LOCKED;

	if (KMotionPresent)	TheFrame->KMotionDLL->ReleaseToken();

	if (KMotionPresent && !m_Simulate)
	{
		if (Interpreter->CoordMotion->ReadCurAbsPosition(x,y,z,a,b,c)) return 1;
	}
	else
	{
		*x = Interpreter->CoordMotion->current_x;
		*y = Interpreter->CoordMotion->current_y;
		*z = Interpreter->CoordMotion->current_z;
		*a = Interpreter->CoordMotion->current_a;
		*b = Interpreter->CoordMotion->current_b;
		*c = Interpreter->CoordMotion->current_c;
	}

	// convert absolute machine's position in inches to 
	// the Interpreter's position that may include offsets
	// and metric units

	Interpreter->ConvertAbsoluteToInterpreterCoord(*x,*y,*z,*a,*b,*c,x,y,z,a,b,c);

	return 0;
}

void CKMotionCNCDlg::OnSetX() 
{
	static CSetValue SetDlg;

	DisableKeyJog();
	if (SetDlg.DoModal() == IDOK)
	{
		double x,y,z,a,b,c;
		setup_pointer ps = Interpreter->p_setup;

		if (ReadInterpPos(&x,&y,&z,&a,&b,&c)) return;
		if (m_ZeroUsingFixtures)
		{
			int Fix = ps->origin_index-1;
			ps->origin_offset_x += x - SetDlg.m_Value;
			ps->parameters[5221+Fix*20+0]=ps->origin_offset_x;
		}
		else
		{
			ps->axis_offset_x += x - SetDlg.m_Value;
			ps->parameters[5211]=ps->axis_offset_x;
		}
	}
}

void CKMotionCNCDlg::OnSetY() 
{
	static CSetValue SetDlg;

	DisableKeyJog();
	if (SetDlg.DoModal() == IDOK)
	{
		double x,y,z,a,b,c;
		setup_pointer ps = Interpreter->p_setup;

		if (ReadInterpPos(&x,&y,&z,&a,&b,&c)) return;
		if (m_ZeroUsingFixtures)
		{
			int Fix = ps->origin_index-1;
			ps->origin_offset_y += y - SetDlg.m_Value;
			ps->parameters[5221+Fix*20+1]=ps->origin_offset_y;
		}
		else
		{
			Interpreter->p_setup->axis_offset_y += y - SetDlg.m_Value;
			Interpreter->p_setup->parameters[5212]=Interpreter->p_setup->axis_offset_y;
		}
	}
}


void CKMotionCNCDlg::OnSetZ() 
{
	static CSetValue SetDlg;

	DisableKeyJog();
	if (SetDlg.DoModal() == IDOK)
	{
		double x,y,z,a,b,c;
		setup_pointer ps = Interpreter->p_setup;

		if (ReadInterpPos(&x,&y,&z,&a,&b,&c)) return;
		if (m_ZeroUsingFixtures)
		{
			int Fix = ps->origin_index-1;
			ps->origin_offset_z += z - SetDlg.m_Value;
			ps->parameters[5221+Fix*20+2]=ps->origin_offset_z;
		}
		else
		{
			Interpreter->p_setup->axis_offset_z += z - SetDlg.m_Value;
			Interpreter->p_setup->parameters[5213]=Interpreter->p_setup->axis_offset_z;
		}
	}
}

void CKMotionCNCDlg::OnSetA() 
{
	static CSetValue SetDlg;

	DisableKeyJog();
	if (SetDlg.DoModal() == IDOK)
	{
		double x,y,z,a,b,c;
		setup_pointer ps = Interpreter->p_setup;

		if (ReadInterpPos(&x,&y,&z,&a,&b,&c)) return;
		if (m_ZeroUsingFixtures)
		{
			int Fix = ps->origin_index-1;
			ps->AA_origin_offset += a - SetDlg.m_Value;
			ps->parameters[5221+Fix*20+3]=ps->AA_origin_offset;
		}
		else
		{
			Interpreter->p_setup->AA_axis_offset += a - SetDlg.m_Value;
			Interpreter->p_setup->parameters[5214]=Interpreter->p_setup->AA_axis_offset;
		}
	}
}

void CKMotionCNCDlg::OnSetB() 
{
	static CSetValue SetDlg;

	DisableKeyJog();
	if (SetDlg.DoModal() == IDOK)
	{
		double x,y,z,a,b,c;
		setup_pointer ps = Interpreter->p_setup;

		if (ReadInterpPos(&x,&y,&z,&a,&b,&c)) return;
		if (m_ZeroUsingFixtures)
		{
			int Fix = ps->origin_index-1;
			ps->BB_origin_offset += b - SetDlg.m_Value;
			ps->parameters[5221+Fix*20+4]=ps->BB_origin_offset;
		}
		else
		{
			Interpreter->p_setup->BB_axis_offset += b - SetDlg.m_Value;
			Interpreter->p_setup->parameters[5215]=Interpreter->p_setup->BB_axis_offset;
		}
	}
}

void CKMotionCNCDlg::OnSetC() 
{
	static CSetValue SetDlg;

	DisableKeyJog();
	if (SetDlg.DoModal() == IDOK)
	{
		double x,y,z,a,b,c;
		setup_pointer ps = Interpreter->p_setup;

		if (ReadInterpPos(&x,&y,&z,&a,&b,&c)) return;
		if (m_ZeroUsingFixtures)
		{
			int Fix = ps->origin_index-1;
			ps->CC_origin_offset += c - SetDlg.m_Value;
			ps->parameters[5221+Fix*20+5]=ps->CC_origin_offset;
		}
		else
		{
			Interpreter->p_setup->CC_axis_offset += c - SetDlg.m_Value;
			Interpreter->p_setup->parameters[5216]=Interpreter->p_setup->CC_axis_offset;
		}
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
		s.Format("%f",v);
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
	for (int i=0; i<MAX_ACTUATORS; i++)
		StopAxis(i);
}

void CKMotionCNCDlg::Onmm() 
{
	DoGCodeLine("G21");
}

void CKMotionCNCDlg::Oninch() 
{
	DoGCodeLine("G20");
}

void CKMotionCNCDlg::OnAbs() 
{
	DoGCodeLine("G90");
}

void CKMotionCNCDlg::OnRel() 
{
	DoGCodeLine("G91");
}


BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString s;

	s = "KMotionCNC " KMOTION_VER; 
	SetDlgItemText(IDC_KMotionCNCVersion,s);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CKMotionCNCDlg::OnBnClickedFeedhold()
{
	CString response;
	if (TheFrame->KMotionDLL->WriteLineReadLine("GetStopState",response.GetBufferSetLength(MAX_LINE))) return;
	response.ReleaseBuffer();

	if (response=="0")
	{
		TheFrame->KMotionDLL->WriteLine("StopImmediate0");
	}
	else
	{
		TheFrame->KMotionDLL->WriteLine("StopImmediate1");
	}
}

void CKMotionCNCDlg::OnBnClickedEdittoolfile()
{
	CEditToolFile EditToolFile;

	DisableKeyJog();

	if (EditToolFile.LoadFile(m_ToolFile)) return;

	if (EditToolFile.DoModal() == IDOK)
	{
		EditToolFile.SaveFile(m_ToolFile);
		TheFrame->GCodeDlg.Interpreter->m_ReadToolFile=true;
	}
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

	if (EditFixtures.DoModal() == IDOK)
	{
		for (int axis=0; axis<NFIXAXES; axis++)
			for (int Fix=0; Fix<NFIXTURES; Fix++)
				Vars[5221+Fix*20+axis] = EditFixtures.m_Fixture[axis][Fix];

		for (int axis=0; axis<NFIXAXES; axis++)
			Vars[5211+axis] = EditFixtures.m_Global[axis];

		// make sure Settings are in sync with any modified Vars
		Interpreter->p_setup->origin_index=-1; // set invalid so it updates
		Interpreter->ChangeFixtureNumber(m_LastFixtureDisplayed);

		settings->axis_offset_x = Vars[5211];
		settings->axis_offset_y = Vars[5212];
		settings->axis_offset_z = Vars[5213];
		settings->AA_axis_offset = Vars[5214];
		settings->BB_axis_offset = Vars[5215];
		settings->CC_axis_offset = Vars[5216];
	}
}

void CKMotionCNCDlg::OnBnClickedSetfixture()
{
	double *Vars = TheFrame->GCodeDlg.Interpreter->p_setup->parameters;	
	setup_pointer ps = Interpreter->p_setup;

	if (Interpreter->ReadAndSyncCurPositions(&ps->current_x,&ps->current_y,&ps->current_z,
											&ps->AA_current,&ps->BB_current,&ps->CC_current)) return;
	
	int Fix = ps->origin_index-1;

	Vars[5221+Fix*20+0] += ps->current_x;
	Vars[5221+Fix*20+1] += ps->current_y;
	Vars[5221+Fix*20+2] += ps->current_z;
	Vars[5221+Fix*20+3] += ps->AA_current;
	Vars[5221+Fix*20+4] += ps->BB_current;
	Vars[5221+Fix*20+5] += ps->CC_current;

	if (Interpreter->p_setup->length_units==CANON_UNITS_INCHES)
	{
		// for inches round to 6 digits
		for (int axis=0; axis<NFIXAXES; axis++)
			Vars[5221+Fix*20+axis] = floor(Vars[5221+Fix*20+axis] * 1e6 + 0.5) / 1e6;
	}
	else if (Interpreter->p_setup->length_units==CANON_UNITS_MM)
	{
		// for mm round to 4 digits
		for (int axis=0; axis<NFIXAXES; axis++)
			Vars[5221+Fix*20+axis] = floor(Vars[5221+Fix*20+axis] * 1e4 + 0.5) / 1e4;
	}


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
	CDlgX::OnSize(nType,cx,cy);
	if (cx>0)// don't update when minimizing
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
	Interpreter->InvokeAction(10,FALSE);  // Set the Speed Sxxxx
	Interpreter->InvokeAction(3,FALSE);   // do the defined action for M3 Code
}

void CKMotionCNCDlg::OnBnClickedSpindleonccw()
{
	Interpreter->InvokeAction(10,FALSE);  // Set the Speed Sxxxx
	Interpreter->InvokeAction(4,FALSE);   // do the defined action for M4 Code
}

void CKMotionCNCDlg::OnBnClickedSpindleoff()
{
	Interpreter->InvokeAction(5,FALSE);  // do the defined action for M Code
}
