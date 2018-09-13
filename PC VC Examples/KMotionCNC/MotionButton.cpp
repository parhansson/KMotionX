// MotionButton.cpp : implementation file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#include "stdafx.h"
#include "MotionButton.h"
#include "..\GCodeInterpreter\GCodeInterpreter.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMotionButton

CList <LPCMotionButton,LPCMotionButton> CMotionButton::AxisGroup;

static DWORD DoExecuteShell(LPDWORD lpdwParam);

CMotionButton::CMotionButton()
{
	m_Moving=false;
	m_RawVel=0.0;
	m_HasMouseCapture=false;
	Style = MotionButton;

	// make a global list of all Motion Buttons
	// so we can allow mutiple buttons down but not
	// more than one with the same axis
	CMotionButton::AxisGroup.AddTail(this); 
}

CMotionButton::~CMotionButton()
{
}


BEGIN_MESSAGE_MAP(CMotionButton, CImageButton)
	//{{AFX_MSG_MAP(CMotionButton)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_KILLFOCUS()
	ON_WM_CAPTURECHANGED()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()


	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMotionButton message handlers


void CMotionButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	CImageButton::OnMouseMove(nFlags, point);
}

void CMotionButton::Init(CKMotionCNCDlg *pDlg, int axis, int dir, double *mag, bool StepMode, int HotKey)
{
	Dlg = pDlg;
	m_axis = axis;
	m_dir=dir;
	m_mag=mag;
	m_StepMode=StepMode;
	m_HotKey = HotKey;
	Style = MotionButton;

}


void CMotionButton::OnTimer(UINT nIDEvent) 
{
	CString s;

	static bool prev=false;

	if (Dlg->m_Simulate)
	{
		if (m_SimulateMotion)
		{
			*m_pSimulatePos += m_SimulateDelta; 
		}

		prev = m_SimulateMotion;
	}
	else
	{
		m_SimulateMotion=false;
	}
	
	CImageButton::OnTimer(nIDEvent);
}

void CMotionButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CImageButton::DrawItem(lpDrawItemStruct);
}

int CMotionButton::HandleButtonDown(void)
{
	CString s;
	CCoordMotion *CM= Dlg->Interpreter->CoordMotion;
	double delta=0;

	POSITION position=CMotionButton::AxisGroup.GetHeadPosition();
	LPCMotionButton p;
	for (int i=0; i<CMotionButton::AxisGroup.GetCount(); i++)
	{
		p=CMotionButton::AxisGroup.GetNext(position);

		// not me and same axis and moving?
		if (p!=this && p->m_axis==m_axis && (p->m_Moving || p->m_SimulateMotion))
		{
			// stop it
			p->PostMessageA(WM_LBUTTONUP,0,0); 
		}
	}



	switch (m_axis)
	{
	case 0:
		m_pSimulatePos =&CM->current_x;
		break;
	case 1:
		m_pSimulatePos =&CM->current_y;
		break;
	case 2:
		m_pSimulatePos =&CM->current_z;
		break;
	case 3:
		m_pSimulatePos =&CM->current_a;
		break;
	case 4:
		m_pSimulatePos =&CM->current_b;
		break;
	case 5:
		m_pSimulatePos =&CM->current_c;
		break;
	}

	

	if (m_StepMode)
	{
		double Step=0;

		switch (Dlg->m_StepSize)
		{
		case 0: Step = Dlg->m_Step0; break;
		case 1: Step = Dlg->m_Step1; break;
		case 2: Step = Dlg->m_Step2; break;
		case 3: Step = Dlg->m_Step3; break;
		case 4: Step = Dlg->m_Step4; break;
		case 5: Step = Dlg->m_Step5; break;
		}

		bool Degrees = (m_axis == 3 && CM->GetMotionParams()->DegreesA) || (m_axis == 4 && CM->GetMotionParams()->DegreesB) || (m_axis == 5 && CM->GetMotionParams()->DegreesC);

		if (Dlg->Interpreter->p_setup->length_units==CANON_UNITS_INCHES || Degrees)
			delta = Step*m_dir; 
		else if (Dlg->Interpreter->p_setup->length_units==CANON_UNITS_MM)
			delta = Step*m_dir/25.4; 


		if (Dlg->m_Simulate)
		{
			*m_pSimulatePos += delta; 
		}
		else
		{
			// check if we came to a stop yet (either from Jog to stop, or from offset)

			CString response;
			if (TheFrame->KMotionDLL->WriteLineReadLine("CheckDoneXYZABC", response.GetBufferSetLength(MAX_LINE))) return 1;
			response.ReleaseBuffer();
			if (response == "0") return 0;  // exit if still stopping 

			if (TheFrame->KMotionDLL->WriteLineReadLine("CheckDoneBuf", response.GetBufferSetLength(MAX_LINE))) return 1;
			response.ReleaseBuffer();
			if (response == "0") return 0;  // exit if still stopping 

			if (Dlg->CS_axis[m_axis]>=0)
			{
				if (CM->ReadCurAbsPosition(&stepx,&stepy,&stepz,&stepa, &stepb, &stepc, &stepu, &stepv)) return 1;

				CM->current_x = stepx;
				CM->current_y = stepy;
				CM->current_z = stepz;
				CM->current_a = stepa;
				CM->current_b = stepb;
				CM->current_c = stepc;
				CM->current_u = stepu;
				CM->current_v = stepv;

				switch (m_axis)
				{
				case 0:
					stepx += delta;
					break;
				case 1:
					stepy += delta;
					break;
				case 2:
					stepz += delta;
					break;
				case 3:
					stepa += delta;
					break;
				case 4:
					stepb += delta;
					break;
				case 5:
					stepc += delta;
					break;
				}

				//Do coordinated motion with a worker thread
				LaunchExecution();
			}
		}
	}
	else
	{
		// jog in velocity mode

		if (Dlg->m_Simulate)
		{
			m_SimulateDelta = Dlg->m_JogSpeed[m_axis] * Dlg->m_JogSpeedOverride[m_axis] * m_dir * *m_mag * 0.01 * 0.1; /* simulate this speed for short time delta */
			*m_pSimulatePos += m_SimulateDelta; 
			m_SimulateMotion=true;
			SetTimer(1,100,NULL);
		}
		else
		{
			if (Dlg->CS_axis[m_axis]>=0)
			{
				m_RawVel = Dlg->m_JogSpeed[m_axis] * Dlg->m_JogSpeedOverride[m_axis] * m_dir * *m_mag * 0.01;
				if (Dlg->ProcessChangeInJogVelocity()) 
				{
					m_RawVel=0.0;
					return 0;
				}
				m_Moving=true;
			}
		}
	}
	DrawPushed=true;
	InvalidateRect(NULL);
	return 0;
}

int CMotionButton::LaunchExecution()
{
	DWORD ID;


	// Now start a worker thread that performs the G Code

	HANDLE Thread = CreateThread(
		NULL,                        /* no security attributes        */
		0,                           /* use default stack size        */
		(LPTHREAD_START_ROUTINE) ::DoExecuteShell, /* thread function       */
		this,							 /* argument to thread function   */
		0,                           /* use default creation flags    */
		&ID);

	return 0;
}

static DWORD DoExecuteShell(LPDWORD lpdwParam)
{
	CMotionButton *p = (CMotionButton*)lpdwParam;

	p->m_exitcode = p->DoExecute();

	return 0;
}

int CMotionButton::DoExecute()
{
	CCoordMotion *CM = Dlg->Interpreter->CoordMotion;
	CM->ClearAbort();
	CM->ClearHalt();
	CM->SetTPParams();
	if (CM->StraightTraverse(stepx, stepy, stepz, stepa, stepb, stepc, true)) return 1;
	if (CM->FlushSegments())  return 1;
	if (CM->WaitForSegmentsFinished()) return 1;
	return 0;
}

void CMotionButton::HandleButtonUp(void)
{
	m_SimulateMotion=false;

	m_RawVel=0.0;
	if (m_Moving)
	{
		m_Moving=false;
		Dlg->ProcessChangeInJogVelocity();
		KillTimer(1);
	}
	DrawPushed=false;
	InvalidateRect(NULL);
}

void CMotionButton::PutRawV(double *v)
{
	if (m_axis < 6)
		if (v[m_axis] == 0.0) v[m_axis] = m_RawVel;
}

void CMotionButton::OnKillFocus(CWnd* pNewWnd)
{
	// don't do much to allow multiple motion buttons
	HandleButtonUp();
	ForceDisableFocus=true;
	InvalidateRect(NULL);
}

void CMotionButton::OnCaptureChanged(CWnd* Wnd)
{
	HandleButtonUp();
	CImageButton::OnCaptureChanged(Wnd);
}

void CMotionButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	HandleButtonUp();

	CImageButton::OnLButtonUp(nFlags, point);
}

void CMotionButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	ForceDisableFocus=false;
	if (HandleButtonDown()) return;
	CButton::OnLButtonDown(nFlags, point);
}

