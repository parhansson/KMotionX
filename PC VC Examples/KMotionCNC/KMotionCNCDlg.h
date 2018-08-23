// KMotionCNCDlg.h : header file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/

#pragma once


#include "dlgbars.h"
#include "LogSlider.h"
#include "RichEditCtrlEx.h"
#include "Display.h"
#include "ImageButton.h"
#include "MotionButton.h"
#include "ForRevButton.h"
#include "DlgX.h"
#include "..\GCodeInterpreter\GCodeInterpreter.h"
#include "MainFrm.h"
#include "easysize.h"
#include "AllToolSetupSheet.h"
#include "HiResTimer.h"
#include "Screen.h"
#include "GViewParent.h"

#define N_USER_GCODE_FILES 7
#define GCODE_SUB_DIR                   "\\GCode Programs\\"
#define C_PROGRAMS_DIR                  "\\C Programs\\"
#define SCREEN_BITMAPS_DIR              "\\PC VC Examples\\KMotionCNC\\res\\"
#define SCREEN_SCRIPTS_DIR              "\\PC VC Examples\\KMotionCNC\\Screens\\"
#define DATA_SUB_DIR					"\\KMotion\\Data\\"
#define TOOL_IMAGE_SUB_DIR              "\\KMotion\\Data\\ToolImages\\"
#define DEFAULT_CONFIG_FILE				"\\KMotion\\Data\\GCodeConfigCNC.txt"
#define DEFAULT_CONFIG_FILE_BACKUP		"\\KMotion\\Data\\GCodeConfigCNC.txt.bak"
#define PERSISTANT_FILE					"\\KMotion\\Data\\persistCNC.ini"
#define PERSISTANT_FILE_BACKUP			"\\KMotion\\Data\\persistCNC.ini.bak"
#define TEMP_GCODE_FILE					"\\KMotion\\Data\\Temp_Gcode_Temp_.ngc"
#define LOG_RUNTIME_FILE				"\\KMotion\\Data\\RunLog.txt"
#define EDIT_CONTROL_PERSIST_FILE		"\\KMotion\\Data\\EditControlPersist.txt"


#define NCOMMAND_HISTORY 10

#define ACTUATORS_CONTROLLED 6


#define NSTEP_SIZES 6

#define MAX_USER_BUTTONS 50
#define MAX_EDIT_CONTROLS 40

#define CUSTOM_DLG_FACE 7

/////////////////////////////////////////////////////////////////////////////
// CKMotionCNCDlg dialog

class CKMotionCNCDlg : public CDlgX
{
// Construction
public:
	DECLARE_EASYSIZE
	void SetStepText(int i,double v,int ID);
	double CurAbsX,CurAbsY,CurAbsZ,CurAbsA,CurAbsB,CurAbsC;
	float m_JogSpeedFactor;
	double m_Joyvx, m_Joyvy, m_Joyvz, m_Joyva, m_Joyvb, m_Joyvc;
	double m_JoyExtvx, m_JoyExtvy, m_JoyExtvz, m_JoyExtva, m_JoyExtvb, m_JoyExtvc;
	double m_Joyx0,m_Joyy0,m_Joyz0,m_Joya0,m_Joyb0,m_Joyc0;
	bool m_JoyMovedx,m_JoyMovedy,m_JoyMovedz,m_JoyMoveda,m_JoyMovedb,m_JoyMovedc;
	bool PersistRestored;
	bool FirstStartup;
	bool FirstInitDlg;
	bool m_PerformPostHaltCommand;
	CString CommandHistory[NCOMMAND_HISTORY];
	int board;
	bool ReadStatus;
	CKMotionCNCDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CKMotionCNCDlg();
	CGCodeInterpreter *Interpreter;
	setup_pointer m_RealTimeSetup;
	BOOL ShuttingDownApplication;
	MAIN_STATUS MainStatus;
	double PrevMainStatusTimeStamp;
	double PrevDROx,PrevDROy,PrevDROz,PrevDROa,PrevDROb,PrevDROc;
	int prev_length_units;
	int m_BulkStatusCount;
	int CS_axis[MAX_ACTUATORS];
	int SetExecutionPoint(int line);
	CString CurrentDirectory;
	int m_ThreadThatWasLaunched;
	int m_ThreadThatWasOriginallyStopped;
	volatile bool ThreadIsExecuting;
	bool EnableJogKeys;
	bool GCodeThreadActive[N_USER_GCODE_FILES];
	bool ThreadHadError[N_USER_GCODE_FILES];
	DWORD m_exitcode;
	CString m_SetupFile;
	CString m_GeoFile;
	CString m_VarsFile;
	CString m_ScreenScriptFile;
	CString m_ToolFile;
	double	m_BreakAngle;
	double	m_CollinearTol;
	double	m_CornerTol;
	double	m_FacetAngle;
	double	m_TPLookahead;
	double	m_RadiusC;
	double	m_RadiusB;
	double	m_RadiusA;
	double	m_MaxAccelC;
	double	m_MaxAccelB;
	double	m_MaxAccelA;
	double	m_MaxAccelX;
	double	m_MaxAccelY;
	double	m_MaxAccelZ;
	double	m_MaxAccelU;
	double	m_MaxAccelV;
	double	m_MaxVelC;
	double	m_MaxVelB;
	double	m_MaxVelA;
	double	m_MaxVelX;
	double	m_MaxVelY;
	double	m_MaxVelZ;
	double	m_MaxVelU;
	double	m_MaxVelV;
	double	m_CountsPerInchC;
	double	m_CountsPerInchB;
	double	m_CountsPerInchA;
	double	m_CountsPerInchX;
	double	m_CountsPerInchY;
	double	m_CountsPerInchZ;
	double	m_CountsPerInchU;
	double	m_CountsPerInchV;
	double  m_JogSpeed[MAX_ACTUATORS];
	double  m_JogSpeedOverride[MAX_ACTUATORS];
	double  m_JogSlowPercent;
	double  m_HardwareFRORange;
	double  m_MaxRapidFRO;
	double  m_ArcRadiusTol;
	double  m_ArcRSmallTol;
	double	m_Step0;
	double	m_Step1;
	double	m_Step2;
	double	m_Step3;
	double	m_Step4;
	double	m_Step5;
	int m_SpindleType;
	int m_SpindleAxis;
	double m_SpindleUpdateTime;
	double m_SpindleTau;
	double m_SpindleCntsPerRev;

	BOOL	m_ReverseRZ;
	BOOL	m_EnableGamePad;
	BOOL	m_ZeroUsingFixtures;
	BOOL	m_ToolLengthImmediately;
	BOOL	m_ToolTableDoM6;
	BOOL	m_ConfirmExit;
	BOOL	m_ArcsToSegs;
	BOOL	m_DisplayEncoder;
	BOOL	m_DegreesA;
	BOOL	m_DegreesB;
	BOOL	m_DegreesC;
	BOOL	m_Lathe;
	BOOL	m_DoRapidsAsFeeds;
	BOOL	m_DiameterMode;
	BOOL	m_XPosFront;
	int		m_DialogFace;
	int		m_DialogFaceInUse;
	int		m_ConfigCheckWord;
	bool	m_ConfigCheckWordVersion;
	double  m_SafeZ;
	int		m_SafeRelAbs;
	bool m_ConnectedForStatus;

	bool m_PrevABCPlotValid;
	double m_PrevPlotA, m_PrevPlotB, m_PrevPlotC, m_PrevPlotX, m_PrevPlotY, m_PrevPlotZ;
	
	#define OFFSET_SAVE_TIME 1.0  // seconds between checks for changes
	BOOL m_SaveFixtureOnOK;
	bool m_OffsetTimerStarted;
	CHiResTimer m_OffsetSaveTimer;

	bool m_EditScreenTimerStarted;
	CHiResTimer m_EditScreenSaveTimer;

	CGViewParent *ActualGViewParent;
	CGViewParent GViewControlParent;  // handles GView Parent actions when Control is on Main Dialog

	int m_UserButtonKeys[NUSERBUTTONS];

	CString	m_Button0;
	CString	m_Button2;
	CString	m_Button1;
	CString	m_Button3;
	CString	m_Button4;
	CString	m_Button5;
	CString	m_Button6;
	CString	m_Button7;
	CString	m_Button8;
	CString	m_Button9;

	CString	m_prevFeedRateLabel;
	CString	m_prevFeedRateLabelF;
	CString	m_prevSpindleRateLabel;
	CString	m_prevSpindleRateLabelF;

	bool m_ToggleKeyMode;


	CColor m_ColorTraverse;
	CColor m_ColorFeed;
	CColor m_ColorJump;

	CMutex *GCodeMutex;
	int DisplayedThreadStat[N_USER_GCODE_FILES];  // -1 = undefined, 1=active, 0 = inactive
	int NumberToThreadID(int i) ;

	int CurrentLine[N_USER_GCODE_FILES];  // which line to start executing
	int DisplayedCurrentLine;             // which line to start executing
	bool DisplayedThreadHadError;

	bool m_RestoreStoppedState;
	int m_SaveStoppedState;

	int Halt();
	int LaunchExecution(CString InFile,int begin, int end);
	int CheckForResumeCircumstances();
	void SaveOnExit(FILE * f);
	void RestoreOnStart(FILE * f);
	void SaveFileNames();
	int SaveLoadConfig(FILE *f, char *s, bool save);
	int SaveConfig();
	int LoadConfig();
	void LoadFile(int thread,bool ResetPosition);
	int DoSaveAs(int thread) ;
	int SaveFile(int thread, bool ForceSave);
	void RefreshTitle(); 
	CString FileNames[N_USER_GCODE_FILES];
	void CreateDlgOrBringToTop(UINT ID, CDialogEx *Dlg);
	int ProcessChangeInJogVelocity();
	int DoActPosition(int i, double p);
	int DoActPositionExp(int i, double p, double tau);
	bool AxisInputModeNone(int axis);
	int GetAxisDRO(int axis, double *Act, double *Dest, bool *DisplayedEnc);
	int SendOneDouble(int i, double d);
	int SendCoordinates(int i, bool MachineCoords);
	int ConvertToolToIndex(int number,int *index);
	int OnGetPositions2(UINT nID, bool NoGeo);
	BOOL GetDefaultToolTipTextFromID(UINT ID, LPWSTR Tip);

	COLORREF m_DlgBackgroundColor;
	CBrush m_DlgBackgroundBrush;


	CString m_ErrorOutput;
	CStringW ToolTipText;
    CDlgToolBar *m_GCodeTools;

	CHiResTimer ElapsedTimer;
	bool JobStartTimeValid;
	bool JobEndTimeValid;
	double JobStartTimeSecs,JobEndTimeSecs;

	bool m_DoingSimulationRun;

	BOOL m_DisplayGViewer;
	int m_LastToolDisplayed;
	int m_LastFixtureDisplayed;


// Dialog Data
	//{{AFX_DATA(CKMotionCNCDlg)
	enum { IDD = IDD_KMOTIONCNC_DIALOG };
	CComboBoxScreen	m_Command;
	CComboBoxScreen	m_tool;
	CComboBoxScreen m_FixtureOffset;
	CRichEditCtrlEx	m_Editor;
	int		m_Thread;
	int		m_Rapid;
	BOOL	m_Simulate;
	BOOL	m_ShowLineNumbers;
	BOOL	m_ShowMach;
	CEditScreen	m_FeedRateEdit;
	CEditScreen	m_SpindleRateEdit;
	CDisplay	m_PosC;
	CDisplay	m_PosB;
	CDisplay	m_PosA;
	CDisplay	m_PosZ;
	CDisplay	m_PosY;
	CDisplay	m_PosX;
	CLogSlider	m_FeedSlider;
	CLogSlider	m_SpindleSlider;
	CMotionButton	m_Zplus2;
	CMotionButton	m_Zplus;
	CMotionButton	m_ZplusStep;
	CMotionButton	m_Zminus2;
	CMotionButton	m_Zminus;
	CMotionButton	m_ZminusStep;
	CMotionButton	m_Aplus2;
	CMotionButton	m_Aplus;
	CMotionButton	m_AplusStep;
	CMotionButton	m_Aminus2;
	CMotionButton	m_Aminus;
	CMotionButton	m_AminusStep;

	CMotionButton	m_Bplus2;
	CMotionButton	m_Bplus;
	CMotionButton	m_BplusStep;
	CMotionButton	m_Bminus2;
	CMotionButton	m_Bminus;
	CMotionButton	m_BminusStep;

	CMotionButton	m_Cplus2;
	CMotionButton	m_Cplus;
	CMotionButton	m_CplusStep;
	CMotionButton	m_Cminus2;
	CMotionButton	m_Cminus;
	CMotionButton	m_CminusStep;


	CMotionButton	m_Right2;
	CMotionButton	m_Right;
	CMotionButton	m_RightStep;
	CMotionButton	m_Down2;
	CMotionButton	m_Down;
	CMotionButton	m_DownStep;
	CMotionButton	m_Up2;
	CMotionButton	m_Up;
	CMotionButton	m_UpStep;
	CMotionButton	m_Left2;
	CMotionButton	m_Left;
	CMotionButton	m_LeftStep;

	CImageButton	m_StopStep;
	CImageButton	m_KeyJogMode;

	CImageButton m_UserImageBut[MAX_USER_BUTTONS];
	CImageButton m_ZeroX;
	CImageButton m_ZeroY;
	CImageButton m_ZeroZ;
	CImageButton m_ZeroA;
	CImageButton m_ZeroB;
	CImageButton m_ZeroC;
	CImageButton m_SetX;
	CImageButton m_SetY;
	CImageButton m_SetZ;
	CImageButton m_SetA;
	CImageButton m_SetB;
	CImageButton m_SetC;
	CImageButton m_EditToolFile;
	CImageButton m_EditFixtures;
	CImageButton m_SetFixture;
	CImageButton m_SpindleOnCW;
	CImageButton m_SpindleOnCCW;
	CImageButton m_SpindleOff;
	CImageButton m_FeedRateApply;
	CImageButton m_SpindleRateApply;
	CImageButton m_RunSimulate;
	CImageButton m_KMotion_HELP;
	CImageButton m_Send;
	CImageButton m_ZeroAll;
	CImageButton m_Measure;

	CEditScreen m_UserEditCtrls[MAX_EDIT_CONTROLS];

	CImageButton m_StaticLabelX;
	CImageButton m_StaticLabelY;
	CImageButton m_StaticLabelZ;
	CImageButton m_StaticLabelA;
	CImageButton m_StaticLabelB;
	CImageButton m_StaticLabelC;
	CImageButton m_StaticTool;
	CImageButton m_StaticThread;
	CImageButton m_SimulateStatic;
	CImageButton m_BlockDeleteStatic;
	CImageButton m_BlockDeleteStatic2;
	CImageButton m_StaticUnits;  
	CImageButton m_StaticCoord;
	CImageButton m_StaticStepSize;
	CImageButton m_Static_Fixture;

	CImageButton m_FeedRateCmd;
	CImageButton m_FeedRateLabel;
	CImageButton m_SpindleRateCmd;
	CImageButton m_SpindleRateLabel;
	CImageButton m_STATIC10;
	CImageButton m_STATIC15;
	CImageButton m_STATIC20;
	CImageButton m_STATIC30;
	CImageButton m_STATIC40;
	CImageButton m_STATIC50;
	CImageButton m_STATIC75;
	CImageButton m_STATIC100;
	CImageButton m_STATIC150;
	CImageButton m_STATIC200;


	CImageButton m_RadioStep0;
	CImageButton m_RadioStep1;
	CImageButton m_RadioStep2;
	CImageButton m_RadioStep3;
	CImageButton m_RadioStep4;
	CImageButton m_RadioStep5;

	CImageButton m_RapidRadio;
	CImageButton m_Feed;
	CImageButton m_mm;
	CImageButton m_inch;
	CImageButton m_Rel;
	CImageButton m_Abs;
	CImageButton m_Thread1;
	CImageButton m_Thread2;
	CImageButton m_Thread3;
	CImageButton m_Thread4;
	CImageButton m_Thread5;
	CImageButton m_Thread6;
	CImageButton m_Thread7;

	CImageButton m_ShowMachButton;
	CImageButton m_SimulateButton;
	CImageButton m_BlockDeleteButton;

	CImageButton m_GVXY;
	CImageButton m_GVYZ;
	CImageButton m_GVXZ;
	CImageButton m_GVRotXY;
	CImageButton m_GVClearPaths;
	CImageButton m_GVShowAxis;
	CImageButton m_GVBox;
	CImageButton m_GVShowTool;
	CImageButton m_GVOrtho;
	CImageButton m_GVGViewerSetup;

	CImageButton m_GCNew;
	CImageButton m_GCOpenFile;
	CImageButton m_GCSaveFile;
	CImageButton m_GCSaveAs;
	CImageButton m_GCRestart;
	CImageButton m_GCSingleStep;
	CImageButton m_GCToolSetup;
	CImageButton m_GView;



	int dpiX;
	int dpiY;
	int dpi_standard = 96;
	double m_dpi_scale;

	double	m_FeedRateValue;
	double	m_FeedRateRapidValue;
	double	m_SpindleRateValue;
	CString	m_CommandString;
	int		m_StepSize;

	HBRUSH CreateDIBrush(CWnd* pWnd);

	//}}AFX_DATA

	CScreen Screen;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKMotionCNCDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void CKMotionCNCDlg::MakeUnicode(int ID, CImageButton &I);
	void SetBigValues(CDisplay *Disp0, CDisplay *Disp1, CDisplay *Disp2, CDisplay *Disp3, CDisplay *Disp4, CDisplay *Disp5, bool KMotionPresent);
	void SetBigValueColor(CDisplay *Disp0,int axis, bool KMotionPresent, bool DisplayedEnc);
	CString LastTitleText;
	HACCEL  m_hAccelTable;
	CImageButton m_EmergencyStop;
	CImageButton m_GO;
	CImageButton m_FeedHold;
	CImageButton m_FR;
	CImageButton m_SR;
	CForRevButton m_Forward;
	CForRevButton m_Reverse;
	HICON m_hIcon;
	int GetStatus();
	void KillMinusZero(CString &s);
	HGLOBAL hDIB;
	HBRUSH hBrush;
	// Generated message map functions
	//{{AFX_MSG(CKMotionCNCDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnF2();
	afx_msg void OnESC();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnFeedRateApply();
	afx_msg void OnSpindleRateApply();
	afx_msg void OnEmergencyStop();
	afx_msg void OnZeroAll();
	afx_msg void OnZeroX();
	afx_msg void OnZeroY();
	afx_msg void OnZeroZ();
	afx_msg void OnZeroA();
	afx_msg void OnZeroB();
	afx_msg void OnZeroC();
	afx_msg void OnIhelp();
	afx_msg void OnRapid();
	afx_msg void OnFeed();
	afx_msg void OnThread1();
	afx_msg void OnThread2();
	afx_msg void OnThread3();
	afx_msg void OnThread4();
	afx_msg void OnThread5();
	afx_msg void OnThread6();
	afx_msg void OnThread7();
	afx_msg void OnSaveFile();
	afx_msg void OnNew();
	afx_msg void OnOpenFile();
	afx_msg void OnExecute();
	afx_msg void OnGO();
	afx_msg void OnSaveAs();
	afx_msg void OnHalt();
	afx_msg void OnToolSetup();
	afx_msg void OnReloadGeoCorrection();
	afx_msg void OnReloadGCodeFile();
	afx_msg void OnOpenGCodeFile();
	afx_msg int OnScreenScript(UINT nID);
	afx_msg int OnGetControlInfo(UINT nID);
	afx_msg int OnMainDlgInfo(UINT nID);
	afx_msg int OnDoJog(UINT nID);
	afx_msg int OnDoMove(UINT nID);
	afx_msg int OnDoMoveExp(UINT nID);
	afx_msg int OnGetPositions(UINT nID);
	afx_msg int OnGetPositionsNoGeo(UINT nID);
	afx_msg void OnExecuteComplete();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnSingleStep();
	afx_msg void OnRestart();
	afx_msg void OnUpdateHalt(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExecute(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRestart(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSingleStep(CCmdUI* pCmdUI);
	afx_msg void OnGView();
	afx_msg void OnSimulate();
	afx_msg void OnBlockDelete();
	afx_msg void OnShowMach();
	afx_msg void OnDropdownfixture();
	afx_msg void OnCloseupfixture();
	afx_msg void OnCloseuptool();
	afx_msg void OnClose();
	afx_msg void OnSend();
	afx_msg void OnDropdownCommand();
	afx_msg void OnCloseupCommand();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBut0();
	afx_msg void OnBut1();
	afx_msg void OnBut2();
	afx_msg void OnBut3();
	afx_msg void OnBut4();
	afx_msg void OnBut5();
	afx_msg void OnBut6();
	afx_msg void OnBut7();
	afx_msg void OnBut8();
	afx_msg void OnBut9();
	afx_msg void OnMeasure();
	afx_msg void OnStep0();
	afx_msg void OnStep1();
	afx_msg void OnStep2();
	afx_msg void OnStep3();
	afx_msg void OnStep4();
	afx_msg void OnStep5();
	afx_msg void OnSetX();
	afx_msg void OnSetY();
	afx_msg void OnSetZ();
	afx_msg void OnSetA();
	afx_msg void OnSetB();
	afx_msg void OnSetC();
	afx_msg void OnStopStep();
	afx_msg void Onmm();
	afx_msg void Oninch();
	afx_msg void OnAbs();
	afx_msg void OnRel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnNotifyFormat(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	void SetStepSizes();
	int DoActVelocity(int i, double v);
	int StopAxis(int i);
	double CKMotionCNCDlg::DoJoyAxis(int axis, int joystick);
	void SetMotionParams();
	void SetAUserButton(int ID, CString s);
	void SetUserButtons();
	void SetDefaultHotKeys();
	void FillComboWithTools(CComboBox *Box);
	void FillComboWithCountFixture(int i0, int i1, CComboBox *Box);
	void MakeSureFileIsntReadOnly(CString FN);
	int DoJoyStick();
	int ReadInterpPos(double *x, double *y, double *z, double *a, double *b, double *c);
	int UpdateScreen(bool KMotionPresent);
	void ServiceKFLOPCommands();
	int SetKFLOPCommandResult(int r);
	int DoGCodeLine(CString G);
	CBrush *GreenBrush;
	CBrush *TealBrush;
	void ThreadChanged();
	CString SaveUserCommand;
	CString *SaveUserCommandVar;
	CComboBox *SaveUserCommandCombo;
	bool m_ConfigSpindleDirty;
	bool CSS_BitmapValid;
	bool CSS_BitmapDisplayed;
	bool G32_BitmapValid;
	bool G32_BitmapDisplayed;
	void LogJobEndTime(double seconds);
	void setMainPathAndRoot(LPWSTR arg0);
	void RoundReasonable(double &v);

	CStringW InterprocessString;
	
	HWND HWndClient;

public:
	int GetStringFromGather(int WordOffset, CString *msg, int nWords);
	int SetStringToGather(int WordOffset, CString msg);
	int GetVar(int Var, int *value);
	int SetVar(int Var, int value);

	void WhenIdle();  // called by Ap pclass when message queue Idle

	void HandleToolTableClose();

	afx_msg void OnBnClickedFeedhold();
	afx_msg void OnBnClickedEdittoolfile();
	afx_msg void OnBnClickedEditfixtures();
	afx_msg void OnEnSetfocusFeedrateedit();
	afx_msg void OnEnSetfocusSpindlerateedit();
	void DisableKeyJog();
	afx_msg void OnBnSetfocusSetx();
	afx_msg void OnBnSetfocusSety();
	afx_msg void OnBnSetfocusSetz();
	afx_msg void OnBnSetfocusSeta();
	afx_msg void OnBnSetfocusSetb();
	afx_msg void OnBnSetfocusSetc();
	afx_msg void OnCbnSetfocusCommand();
	int ExternalRestore(void);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnBnClickedSetfixture();
	afx_msg void OnBnClickedRunsimulate();
	afx_msg void OnBnClickedSpindleoncw();
	afx_msg void OnBnClickedSpindleonccw();
	afx_msg void OnBnClickedSpindleoff();
	afx_msg BOOL OnNcActivate(BOOL bActive);

	afx_msg void OnXy();
	afx_msg void OnXz();
	afx_msg void OnYz();
	afx_msg void OnClearPaths();
	afx_msg void OnShowAxis();
	afx_msg void OnOrtho();
	afx_msg void OnBox();
	afx_msg void OnRotXY();
	afx_msg void OnGViewerSetup();
	afx_msg void OnShowTool();
};



//{{AFX_INSERT_LOCATION}}

