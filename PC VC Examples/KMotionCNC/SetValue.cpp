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


CSetValue::CSetValue(int DialogID, CWnd* pParent)
	: CDialog(DialogID, pParent)
{
	StringMode = false;
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
	ON_BN_CLICKED(IDUSEDRO2, &CSetValue::OnUsedDRO2)
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

	if (!StringMode)
	{

		double v;

		int result = swscanf(m_ValueString, L"%lf", &v);

		if (result == 1)
		{
			m_Value = v;

			POSITION pos = Recent.GetHeadPosition();
			POSITION prev = pos;
			for (int i = 0; i < Recent.GetCount(); i++)
			{
				if (m_ValueString == Recent.GetNext(pos))
				{
					Recent.RemoveAt(prev);
				}
				prev = pos;
			}

			if (Recent.GetCount() > 10)
				Recent.RemoveTail();

			Recent.AddHead(m_ValueString);
		}
		else
		{
			MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate("Invalid Entry"), L"KMotion", MB_ICONSTOP | MB_OK | MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL);
			return;
		}

	}
	CDialog::OnOK();
}

BOOL CSetValue::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ValueCombo.ResetContent();

	if (!m_Label.IsEmpty())
	{
		int i, k = 0;   // check if recent values separated by ';' are to be set.
		// extract field
		while (k < m_Label.GetLength() - 1)  // at least one character left?
		{
			CString s;

			i = m_Label.Find(';', k);
			if (i < 0)  // no more separators ?
				i = m_Label.GetLength();
			else if (k == 0)  // first time and separator found
				Recent.RemoveAll();

			s = m_Label.Mid(k, i - k);

			if (k == 0) // first one?
				SetDlgItemText(IDC_Label, s);
			else
				Recent.AddTail(s);

			k = i + 1;  // move past ';'
		}
	}

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


void CSetValue::OnUsedDRO2()
{
	m_Value = halfDRO;
	CDialog::OnOK();
}
