// SetValue.cpp : implementation file
//

#include "stdafx.h"
#include "kmotioncnc.h"
#include "SetValue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetValue dialog


CSetValue::CSetValue(CWnd* pParent /*=NULL*/)
	: CDialog(CSetValue::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetValue)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSetValue::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetValue)
	DDX_Control(pDX, IDC_ValueCombo, m_ValueCombo);
	DDX_CBString(pDX, IDC_ValueCombo, m_ValueString);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetValue, CDialog)
	//{{AFX_MSG_MAP(CSetValue)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetValue message handlers

void CSetValue::OnClose() 
{
	UpdateData();	
	CDialog::OnClose();
}

void CSetValue::OnOK() 
{
	UpdateData();

	double v;

	int result = sscanf(m_ValueString,"%lf",&v);

	if (result==1)
	{
		m_Value = v;

		POSITION pos = Recent.GetHeadPosition();
		POSITION prev=pos;
		for (int i=0; i < Recent.GetCount(); i++)
		{
			if (m_ValueString == Recent.GetNext(pos))
			{
				Recent.RemoveAt(prev);
			}
			prev = pos;
		}

		if (Recent.GetCount() >10)
			Recent.RemoveTail();

		Recent.AddHead(m_ValueString);
	}
	else
	{
		AfxMessageBox("Invalid Entry");
	}

	
	CDialog::OnOK();
}

BOOL CSetValue::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ValueCombo.ResetContent();

	if (Recent.GetCount() > 0)
		m_ValueString = Recent.GetHead();
	else
		m_ValueString = "0.0";

	POSITION pos = Recent.GetHeadPosition();
	for (int i=0; i < Recent.GetCount(); i++)
		m_ValueCombo.AddString(Recent.GetNext(pos));

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
