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

CMotionButton::CMotionButton()
{
	m_Moving=false;
	m_RawVel=0.0;
}

CMotionButton::~CMotionButton()
{
}


BEGIN_MESSAGE_MAP(CMotionButton, CImageButton)
	//{{AFX_MSG_MAP(CMotionButton)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMotionButton message handlers


void CMotionButton::OnMouseMove(UINT nFlags, CPoint point) 
{
}

void CMotionButton::Init(CKMotionCNCDlg *pDlg, int axis, int dir, double mag, bool StepMode)
{
	Dlg = pDlg;
	m_axis = axis;
	m_dir=dir;
	m_mag=mag;
	m_StepMode=StepMode;
}


void CMotionButton::OnTimer(UINT nIDEvent) 
{
	CString s;

	if (Dlg->m_Simulate)
	{
		if (m_SimulateMotion)
		{
			*m_pSimulatePos += m_SimulateDelta; 
		}
	}
	else
	{
		m_SimulateMotion=false;
	}
	
	CImageButton::OnTimer(nIDEvent);
}

void CMotionButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->itemAction & ODA_DRAWENTIRE)
	{
		if (lpDrawItemStruct->itemState & ODS_SELECTED)
				HandleButtonDown();
		else
				HandleButtonUp();
	}
	CImageButton::DrawItem(lpDrawItemStruct);
}

void CMotionButton::HandleButtonDown(void)
{
	CString s;
	CCoordMotion *CM= Dlg->Interpreter->CoordMotion;
	double delta=0,x,y,z,a,b,c;


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

		if (Dlg->Interpreter->p_setup->length_units==CANON_UNITS_INCHES)
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
			if (TheFrame->KMotionDLL->WriteLineReadLine("CheckDoneXYZABC",response.GetBufferSetLength(MAX_LINE))) return;
			response.ReleaseBuffer();

			if (response=="0") return;  // exit if still stopping 
			
			if (Dlg->CS_axis[m_axis]>=0)
			{
				if (CM->ReadCurAbsPosition(&x,&y,&z,&a,&b,&c)) return;
			
				switch (m_axis)
				{
				case 0:
					x += delta;
					break;
				case 1:
					y += delta;
					break;
				case 2:
					z += delta;
					break;
				case 3:
					a += delta;
					break;
				case 4:
					b += delta;
					break;
				case 5:
					c += delta;
					break;
				}

				double Acts[MAX_ACTUATORS];

				CM->Kinematics->TransformCADtoActuators(x, y, z, a, b, c, Acts);

				for (int i=0; i<MAX_ACTUATORS; i++)
					if (Dlg->DoActPosition(i,Acts[i])) return;
			}
		}
	}
	else
	{
		// jog in velocity mode

		if (Dlg->m_Simulate)
		{
			m_SimulateDelta = Dlg->m_JogSpeed[m_axis] * m_dir * m_mag * 0.1; /* simulate this speed for short time delta */
			*m_pSimulatePos += m_SimulateDelta; 
			m_SimulateMotion=true;
			SetTimer(1,100,NULL);
		}
		else
		{
			if (Dlg->CS_axis[m_axis]>=0)
			{
				m_RawVel = Dlg->m_JogSpeed[m_axis] * m_dir * m_mag;
				m_Moving=true;
				Dlg->ProcessChangeInJogVelocity();
			}
		}
	}
}

void CMotionButton::HandleButtonUp(void)
{
	CString s;

	m_SimulateMotion=false;

	if (m_Moving)
	{
		m_RawVel=0.0;
		m_Moving=false;
		Dlg->ProcessChangeInJogVelocity();
		KillTimer(1);
	}
}

void CMotionButton::PutRawV(double *v)
{
	if (m_axis < 6)
		if (v[m_axis] == 0.0) v[m_axis] = m_RawVel;
}
