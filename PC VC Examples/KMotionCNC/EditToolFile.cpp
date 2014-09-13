// EditToolFile.cpp : implementation file
//

#include "stdafx.h"
#include "EditToolFile.h"

#define MAX_TOOL_LINE 200

// CEditToolFile dialog

IMPLEMENT_DYNAMIC(CEditToolFile, CDialog)

CEditToolFile::CEditToolFile(CWnd* pParent /*=NULL*/)
	: CDialog(CEditToolFile::IDD, pParent)
	, m_Pocket0(0)
	, m_Pocket1(0)
	, m_Pocket2(0)
	, m_Pocket3(0)
	, m_Pocket4(0)
	, m_Pocket5(0)
	, m_Pocket6(0)
	, m_Pocket7(0)
	, m_Pocket8(0)
	, m_Pocket9(0)
	, m_Pocket10(0)
	, m_Pocket11(0)
	, m_Pocket12(0)
	, m_Pocket13(0)
	, m_Pocket14(0)
	, m_Pocket15(0)
	, m_Pocket16(0)
	, m_Pocket17(0)
	, m_Pocket18(0)
	, m_Pocket19(0)
	, m_ID0(0)
	, m_ID1(0)
	, m_ID2(0)
	, m_ID3(0)
	, m_ID4(0)
	, m_ID5(0)
	, m_ID6(0)
	, m_ID7(0)
	, m_ID8(0)
	, m_ID9(0)
	, m_ID10(0)
	, m_ID11(0)
	, m_ID12(0)
	, m_ID13(0)
	, m_ID14(0)
	, m_ID15(0)
	, m_ID16(0)
	, m_ID17(0)
	, m_ID18(0)
	, m_ID19(0)
	, m_Length0(0)
	, m_Length1(0)
	, m_Length2(0)
	, m_Length3(0)
	, m_Length4(0)
	, m_Length5(0)
	, m_Length6(0)
	, m_Length7(0)
	, m_Length8(0)
	, m_Length9(0)
	, m_Length10(0)
	, m_Length11(0)
	, m_Length12(0)
	, m_Length13(0)
	, m_Length14(0)
	, m_Length15(0)
	, m_Length16(0)
	, m_Length17(0)
	, m_Length18(0)
	, m_Length19(0)
	, m_Diameter0(0)
	, m_Diameter1(0)
	, m_Diameter2(0)
	, m_Diameter3(0)
	, m_Diameter4(0)
	, m_Diameter5(0)
	, m_Diameter6(0)
	, m_Diameter7(0)
	, m_Diameter8(0)
	, m_Diameter9(0)
	, m_Diameter10(0)
	, m_Diameter11(0)
	, m_Diameter12(0)
	, m_Diameter13(0)
	, m_Diameter14(0)
	, m_Diameter15(0)
	, m_Diameter16(0)
	, m_Diameter17(0)
	, m_Diameter18(0)
	, m_Diameter19(0)
	, m_Comment0(_T(""))
	, m_Comment1(_T(""))
	, m_Comment2(_T(""))
	, m_Comment3(_T(""))
	, m_Comment4(_T(""))
	, m_Comment5(_T(""))
	, m_Comment6(_T(""))
	, m_Comment7(_T(""))
	, m_Comment8(_T(""))
	, m_Comment9(_T(""))
	, m_Comment10(_T(""))
	, m_Comment11(_T(""))
	, m_Comment12(_T(""))
	, m_Comment13(_T(""))
	, m_Comment14(_T(""))
	, m_Comment15(_T(""))
	, m_Comment16(_T(""))
	, m_Comment17(_T(""))
	, m_Comment18(_T(""))
	, m_Comment19(_T(""))
	, m_CurPage(0)
	{
	m_nTools=0;
	m_CurPage=0;
}

CEditToolFile::~CEditToolFile()
{
}


void CEditToolFile::ChangeABlankZero(int ID)
{
	char Buf[MAX_TOOL_LINE+1];
	
	GetDlgItemText(ID,Buf,MAX_TOOL_LINE);

	if (Buf[0]==0)	SetDlgItemText(ID,"0");
}

void CEditToolFile::ChangeBlanksToZeros(void)
{
	ChangeABlankZero(IDC_Pocket0);
	ChangeABlankZero(IDC_Pocket1);
	ChangeABlankZero(IDC_Pocket2);
	ChangeABlankZero(IDC_Pocket3);
	ChangeABlankZero(IDC_Pocket4);
	ChangeABlankZero(IDC_Pocket5);
	ChangeABlankZero(IDC_Pocket6);
	ChangeABlankZero(IDC_Pocket7);
	ChangeABlankZero(IDC_Pocket8);
	ChangeABlankZero(IDC_Pocket9);
	ChangeABlankZero(IDC_Pocket10);
	ChangeABlankZero(IDC_Pocket11);
	ChangeABlankZero(IDC_Pocket12);
	ChangeABlankZero(IDC_Pocket13);
	ChangeABlankZero(IDC_Pocket14);
	ChangeABlankZero(IDC_Pocket15);
	ChangeABlankZero(IDC_Pocket16);
	ChangeABlankZero(IDC_Pocket17);
	ChangeABlankZero(IDC_Pocket18);
	ChangeABlankZero(IDC_Pocket19);
	ChangeABlankZero(IDC_ID0);
	ChangeABlankZero(IDC_ID1);
	ChangeABlankZero(IDC_ID2);
	ChangeABlankZero(IDC_ID3);
	ChangeABlankZero(IDC_ID4);
	ChangeABlankZero(IDC_ID5);
	ChangeABlankZero(IDC_ID6);
	ChangeABlankZero(IDC_ID7);
	ChangeABlankZero(IDC_ID8);
	ChangeABlankZero(IDC_ID9);
	ChangeABlankZero(IDC_ID10);
	ChangeABlankZero(IDC_ID11);
	ChangeABlankZero(IDC_ID12);
	ChangeABlankZero(IDC_ID13);
	ChangeABlankZero(IDC_ID14);
	ChangeABlankZero(IDC_ID15);
	ChangeABlankZero(IDC_ID16);
	ChangeABlankZero(IDC_ID17);
	ChangeABlankZero(IDC_ID18);
	ChangeABlankZero(IDC_ID19);
	ChangeABlankZero(IDC_Length0);
	ChangeABlankZero(IDC_Length1);
	ChangeABlankZero(IDC_Length2);
	ChangeABlankZero(IDC_Length3);
	ChangeABlankZero(IDC_Length4);
	ChangeABlankZero(IDC_Length5);
	ChangeABlankZero(IDC_Length6);
	ChangeABlankZero(IDC_Length7);
	ChangeABlankZero(IDC_Length8);
	ChangeABlankZero(IDC_Length9);
	ChangeABlankZero(IDC_Length10);
	ChangeABlankZero(IDC_Length11);
	ChangeABlankZero(IDC_Length12);
	ChangeABlankZero(IDC_Length13);
	ChangeABlankZero(IDC_Length14);
	ChangeABlankZero(IDC_Length15);
	ChangeABlankZero(IDC_Length16);
	ChangeABlankZero(IDC_Length17);
	ChangeABlankZero(IDC_Length18);
	ChangeABlankZero(IDC_Length19);
	ChangeABlankZero(IDC_Diameter0);
	ChangeABlankZero(IDC_Diameter1);
	ChangeABlankZero(IDC_Diameter2);
	ChangeABlankZero(IDC_Diameter3);
	ChangeABlankZero(IDC_Diameter4);
	ChangeABlankZero(IDC_Diameter5);
	ChangeABlankZero(IDC_Diameter6);
	ChangeABlankZero(IDC_Diameter7);
	ChangeABlankZero(IDC_Diameter8);
	ChangeABlankZero(IDC_Diameter9);
	ChangeABlankZero(IDC_Diameter10);
	ChangeABlankZero(IDC_Diameter11);
	ChangeABlankZero(IDC_Diameter12);
	ChangeABlankZero(IDC_Diameter13);
	ChangeABlankZero(IDC_Diameter14);
	ChangeABlankZero(IDC_Diameter15);
	ChangeABlankZero(IDC_Diameter16);
	ChangeABlankZero(IDC_Diameter17);
	ChangeABlankZero(IDC_Diameter18);
	ChangeABlankZero(IDC_Diameter19);
}

void CEditToolFile::ChangeZeroToBlank(int ID0, int ID1, int ID2, int ID3)
{
	char Buf[MAX_TOOL_LINE+1];
	
	GetDlgItemText(ID0,Buf,MAX_TOOL_LINE);
	if (Buf[0]!='0' || Buf[1]!=0) return;
	GetDlgItemText(ID1,Buf,MAX_TOOL_LINE);
	if (Buf[0]!='0' || Buf[1]!=0) return;
	GetDlgItemText(ID2,Buf,MAX_TOOL_LINE);
	if (Buf[0]!='0' || Buf[1]!=0) return;
	GetDlgItemText(ID3,Buf,MAX_TOOL_LINE);
	if (Buf[0]!='0' || Buf[1]!=0) return;
	
	SetDlgItemText(ID0,"");
	SetDlgItemText(ID1,"");
	SetDlgItemText(ID2,"");
	SetDlgItemText(ID3,"");
}

void CEditToolFile::ChangeZerosToBlanks(void)
{
	ChangeZeroToBlank(IDC_Pocket0,IDC_ID0,IDC_Length0,IDC_Diameter0);
	ChangeZeroToBlank(IDC_Pocket1,IDC_ID1,IDC_Length1,IDC_Diameter1);
	ChangeZeroToBlank(IDC_Pocket2,IDC_ID2,IDC_Length2,IDC_Diameter2);
	ChangeZeroToBlank(IDC_Pocket3,IDC_ID3,IDC_Length3,IDC_Diameter3);
	ChangeZeroToBlank(IDC_Pocket4,IDC_ID4,IDC_Length4,IDC_Diameter4);
	ChangeZeroToBlank(IDC_Pocket5,IDC_ID5,IDC_Length5,IDC_Diameter5);
	ChangeZeroToBlank(IDC_Pocket6,IDC_ID6,IDC_Length6,IDC_Diameter6);
	ChangeZeroToBlank(IDC_Pocket7,IDC_ID7,IDC_Length7,IDC_Diameter7);
	ChangeZeroToBlank(IDC_Pocket8,IDC_ID8,IDC_Length8,IDC_Diameter8);
	ChangeZeroToBlank(IDC_Pocket9,IDC_ID9,IDC_Length9,IDC_Diameter9);
	ChangeZeroToBlank(IDC_Pocket10,IDC_ID10,IDC_Length10,IDC_Diameter10);
	ChangeZeroToBlank(IDC_Pocket11,IDC_ID11,IDC_Length11,IDC_Diameter11);
	ChangeZeroToBlank(IDC_Pocket12,IDC_ID12,IDC_Length12,IDC_Diameter12);
	ChangeZeroToBlank(IDC_Pocket13,IDC_ID13,IDC_Length13,IDC_Diameter13);
	ChangeZeroToBlank(IDC_Pocket14,IDC_ID14,IDC_Length14,IDC_Diameter14);
	ChangeZeroToBlank(IDC_Pocket15,IDC_ID15,IDC_Length15,IDC_Diameter15);
	ChangeZeroToBlank(IDC_Pocket16,IDC_ID16,IDC_Length16,IDC_Diameter16);
	ChangeZeroToBlank(IDC_Pocket17,IDC_ID17,IDC_Length17,IDC_Diameter17);
	ChangeZeroToBlank(IDC_Pocket18,IDC_ID18,IDC_Length18,IDC_Diameter18);
	ChangeZeroToBlank(IDC_Pocket19,IDC_ID19,IDC_Length19,IDC_Diameter19);
}


void CEditToolFile::DoDataExchange(CDataExchange* pDX)
{
	int OriginalPage=m_CurPage;

	if (!pDX->m_bSaveAndValidate)
	{
		PutToolsToScreen(OriginalPage);
	}
	ChangeBlanksToZeros();
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_Page1, m_CurPage);
	DDX_Text(pDX, IDC_Pocket0, m_Pocket0);
	DDV_MinMaxInt(pDX, m_Pocket0, 0, 999);
	DDX_Text(pDX, IDC_Pocket1, m_Pocket1);
	DDV_MinMaxInt(pDX, m_Pocket1, 0, 999);
	DDX_Text(pDX, IDC_Pocket2, m_Pocket2);
	DDV_MinMaxInt(pDX, m_Pocket2, 0, 999);
	DDX_Text(pDX, IDC_Pocket3, m_Pocket3);
	DDV_MinMaxInt(pDX, m_Pocket3, 0, 999);
	DDX_Text(pDX, IDC_Pocket4, m_Pocket4);
	DDV_MinMaxInt(pDX, m_Pocket4, 0, 999);
	DDX_Text(pDX, IDC_Pocket5, m_Pocket5);
	DDV_MinMaxInt(pDX, m_Pocket5, 0, 999);
	DDX_Text(pDX, IDC_Pocket6, m_Pocket6);
	DDV_MinMaxInt(pDX, m_Pocket6, 0, 999);
	DDX_Text(pDX, IDC_Pocket7, m_Pocket7);
	DDV_MinMaxInt(pDX, m_Pocket7, 0, 999);
	DDX_Text(pDX, IDC_Pocket8, m_Pocket8);
	DDV_MinMaxInt(pDX, m_Pocket8, 0, 999);
	DDX_Text(pDX, IDC_Pocket9, m_Pocket9);
	DDV_MinMaxInt(pDX, m_Pocket9, 0, 999);
	DDX_Text(pDX, IDC_Pocket10, m_Pocket10);
	DDV_MinMaxInt(pDX, m_Pocket10, 0, 999);
	DDX_Text(pDX, IDC_Pocket11, m_Pocket11);
	DDV_MinMaxInt(pDX, m_Pocket11, 0, 999);
	DDX_Text(pDX, IDC_Pocket12, m_Pocket12);
	DDV_MinMaxInt(pDX, m_Pocket12, 0, 999);
	DDX_Text(pDX, IDC_Pocket13, m_Pocket13);
	DDV_MinMaxInt(pDX, m_Pocket13, 0, 999);
	DDX_Text(pDX, IDC_Pocket14, m_Pocket14);
	DDV_MinMaxInt(pDX, m_Pocket14, 0, 999);
	DDX_Text(pDX, IDC_Pocket15, m_Pocket15);
	DDV_MinMaxInt(pDX, m_Pocket15, 0, 999);
	DDX_Text(pDX, IDC_Pocket16, m_Pocket16);
	DDV_MinMaxInt(pDX, m_Pocket16, 0, 999);
	DDX_Text(pDX, IDC_Pocket17, m_Pocket17);
	DDV_MinMaxInt(pDX, m_Pocket17, 0, 999);
	DDX_Text(pDX, IDC_Pocket18, m_Pocket18);
	DDV_MinMaxInt(pDX, m_Pocket18, 0, 999);
	DDX_Text(pDX, IDC_Pocket19, m_Pocket19);
	DDV_MinMaxInt(pDX, m_Pocket19, 0, 999);

	DDX_Text(pDX, IDC_ID0, m_ID0);
	DDV_MinMaxInt(pDX, m_ID0, 0, 999);
	DDX_Text(pDX, IDC_ID1, m_ID1);
	DDV_MinMaxInt(pDX, m_ID1, 0, 999);
	DDX_Text(pDX, IDC_ID2, m_ID2);
	DDV_MinMaxInt(pDX, m_ID2, 0, 999);
	DDX_Text(pDX, IDC_ID3, m_ID3);
	DDV_MinMaxInt(pDX, m_ID3, 0, 999);
	DDX_Text(pDX, IDC_ID4, m_ID4);
	DDV_MinMaxInt(pDX, m_ID4, 0, 999);
	DDX_Text(pDX, IDC_ID5, m_ID5);
	DDV_MinMaxInt(pDX, m_ID5, 0, 999);
	DDX_Text(pDX, IDC_ID6, m_ID6);
	DDV_MinMaxInt(pDX, m_ID6, 0, 999);
	DDX_Text(pDX, IDC_ID7, m_ID7);
	DDV_MinMaxInt(pDX, m_ID7, 0, 999);
	DDX_Text(pDX, IDC_ID8, m_ID8);
	DDV_MinMaxInt(pDX, m_ID8, 0, 999);
	DDX_Text(pDX, IDC_ID9, m_ID9);
	DDV_MinMaxInt(pDX, m_ID9, 0, 999);
	DDX_Text(pDX, IDC_ID10, m_ID10);
	DDV_MinMaxInt(pDX, m_ID10, 0, 999);
	DDX_Text(pDX, IDC_ID11, m_ID11);
	DDV_MinMaxInt(pDX, m_ID11, 0, 999);
	DDX_Text(pDX, IDC_ID12, m_ID12);
	DDV_MinMaxInt(pDX, m_ID12, 0, 999);
	DDX_Text(pDX, IDC_ID13, m_ID13);
	DDV_MinMaxInt(pDX, m_ID13, 0, 999);
	DDX_Text(pDX, IDC_ID14, m_ID14);
	DDV_MinMaxInt(pDX, m_ID14, 0, 999);
	DDX_Text(pDX, IDC_ID15, m_ID15);
	DDV_MinMaxInt(pDX, m_ID15, 0, 999);
	DDX_Text(pDX, IDC_ID16, m_ID16);
	DDV_MinMaxInt(pDX, m_ID16, 0, 999);
	DDX_Text(pDX, IDC_ID17, m_ID17);
	DDV_MinMaxInt(pDX, m_ID17, 0, 999);
	DDX_Text(pDX, IDC_ID18, m_ID18);
	DDV_MinMaxInt(pDX, m_ID18, 0, 999);
	DDX_Text(pDX, IDC_ID19, m_ID19);
	DDV_MinMaxInt(pDX, m_ID19, 0, 999);



	DDX_Text(pDX, IDC_Length0, m_Length0);
	DDV_MinMaxDouble(pDX, m_Length0, -999,999);
	DDX_Text(pDX, IDC_Length1, m_Length1);
	DDV_MinMaxDouble(pDX, m_Length1, -999,999);
	DDX_Text(pDX, IDC_Length2, m_Length2);
	DDV_MinMaxDouble(pDX, m_Length2, -999,999);
	DDX_Text(pDX, IDC_Length3, m_Length3);
	DDV_MinMaxDouble(pDX, m_Length3, -999,999);
	DDX_Text(pDX, IDC_Length4, m_Length4);
	DDV_MinMaxDouble(pDX, m_Length4, -999,999);
	DDX_Text(pDX, IDC_Length5, m_Length5);
	DDV_MinMaxDouble(pDX, m_Length5, -999,999);
	DDX_Text(pDX, IDC_Length6, m_Length6);
	DDV_MinMaxDouble(pDX, m_Length6, -999,999);
	DDX_Text(pDX, IDC_Length7, m_Length7);
	DDV_MinMaxDouble(pDX, m_Length7, -999,999);
	DDX_Text(pDX, IDC_Length8, m_Length8);
	DDV_MinMaxDouble(pDX, m_Length8, -999,999);
	DDX_Text(pDX, IDC_Length9, m_Length9);
	DDV_MinMaxDouble(pDX, m_Length9, -999,999);
	DDX_Text(pDX, IDC_Length10, m_Length10);
	DDV_MinMaxDouble(pDX, m_Length10, -999,999);
	DDX_Text(pDX, IDC_Length11, m_Length11);
	DDV_MinMaxDouble(pDX, m_Length11, -999,999);
	DDX_Text(pDX, IDC_Length12, m_Length12);
	DDV_MinMaxDouble(pDX, m_Length12, -999,999);
	DDX_Text(pDX, IDC_Length13, m_Length13);
	DDV_MinMaxDouble(pDX, m_Length13, -999,999);
	DDX_Text(pDX, IDC_Length14, m_Length14);
	DDV_MinMaxDouble(pDX, m_Length14, -999,999);
	DDX_Text(pDX, IDC_Length15, m_Length15);
	DDV_MinMaxDouble(pDX, m_Length15, -999,999);
	DDX_Text(pDX, IDC_Length16, m_Length16);
	DDV_MinMaxDouble(pDX, m_Length16, -999,999);
	DDX_Text(pDX, IDC_Length17, m_Length17);
	DDV_MinMaxDouble(pDX, m_Length17, -999,999);
	DDX_Text(pDX, IDC_Length18, m_Length18);
	DDV_MinMaxDouble(pDX, m_Length18, -999,999);
	DDX_Text(pDX, IDC_Length19, m_Length19);
	DDV_MinMaxDouble(pDX, m_Length19, -999,999);

	DDX_Text(pDX, IDC_Diameter0, m_Diameter0);
	DDV_MinMaxDouble(pDX, m_Diameter0, 0, 999);
	DDX_Text(pDX, IDC_Diameter1, m_Diameter1);
	DDV_MinMaxDouble(pDX, m_Diameter1, 0, 999);
	DDX_Text(pDX, IDC_Diameter2, m_Diameter2);
	DDV_MinMaxDouble(pDX, m_Diameter2, 0, 999);
	DDX_Text(pDX, IDC_Diameter3, m_Diameter3);
	DDV_MinMaxDouble(pDX, m_Diameter3, 0, 999);
	DDX_Text(pDX, IDC_Diameter4, m_Diameter4);
	DDV_MinMaxDouble(pDX, m_Diameter4, 0, 999);
	DDX_Text(pDX, IDC_Diameter5, m_Diameter5);
	DDV_MinMaxDouble(pDX, m_Diameter5, 0, 999);
	DDX_Text(pDX, IDC_Diameter6, m_Diameter6);
	DDV_MinMaxDouble(pDX, m_Diameter6, 0, 999);
	DDX_Text(pDX, IDC_Diameter7, m_Diameter7);
	DDV_MinMaxDouble(pDX, m_Diameter7, 0, 999);
	DDX_Text(pDX, IDC_Diameter8, m_Diameter8);
	DDV_MinMaxDouble(pDX, m_Diameter8, 0, 999);
	DDX_Text(pDX, IDC_Diameter9, m_Diameter9);
	DDV_MinMaxDouble(pDX, m_Diameter9, 0, 999);
	DDX_Text(pDX, IDC_Diameter10, m_Diameter10);
	DDV_MinMaxDouble(pDX, m_Diameter10, 0, 999);
	DDX_Text(pDX, IDC_Diameter11, m_Diameter11);
	DDV_MinMaxDouble(pDX, m_Diameter11, 0, 999);
	DDX_Text(pDX, IDC_Diameter12, m_Diameter12);
	DDV_MinMaxDouble(pDX, m_Diameter12, 0, 999);
	DDX_Text(pDX, IDC_Diameter13, m_Diameter13);
	DDV_MinMaxDouble(pDX, m_Diameter13, 0, 999);
	DDX_Text(pDX, IDC_Diameter14, m_Diameter14);
	DDV_MinMaxDouble(pDX, m_Diameter14, 0, 999);
	DDX_Text(pDX, IDC_Diameter15, m_Diameter15);
	DDV_MinMaxDouble(pDX, m_Diameter15, 0, 999);
	DDX_Text(pDX, IDC_Diameter16, m_Diameter16);
	DDV_MinMaxDouble(pDX, m_Diameter16, 0, 999);
	DDX_Text(pDX, IDC_Diameter17, m_Diameter17);
	DDV_MinMaxDouble(pDX, m_Diameter17, 0, 999);
	DDX_Text(pDX, IDC_Diameter18, m_Diameter18);
	DDV_MinMaxDouble(pDX, m_Diameter18, 0, 999);
	DDX_Text(pDX, IDC_Diameter19, m_Diameter19);
	DDV_MinMaxDouble(pDX, m_Diameter19, 0, 999);


	DDX_Text(pDX, IDC_Comment0, m_Comment0);
	DDV_MaxChars(pDX, m_Comment0, 100);
	DDX_Text(pDX, IDC_Comment1, m_Comment1);
	DDV_MaxChars(pDX, m_Comment1, 100);
	DDX_Text(pDX, IDC_Comment2, m_Comment2);
	DDV_MaxChars(pDX, m_Comment2, 100);
	DDX_Text(pDX, IDC_Comment3, m_Comment3);
	DDV_MaxChars(pDX, m_Comment3, 100);
	DDX_Text(pDX, IDC_Comment4, m_Comment4);
	DDV_MaxChars(pDX, m_Comment4, 100);
	DDX_Text(pDX, IDC_Comment5, m_Comment5);
	DDV_MaxChars(pDX, m_Comment5, 100);
	DDX_Text(pDX, IDC_Comment6, m_Comment6);
	DDV_MaxChars(pDX, m_Comment6, 100);
	DDX_Text(pDX, IDC_Comment7, m_Comment7);
	DDV_MaxChars(pDX, m_Comment7, 100);
	DDX_Text(pDX, IDC_Comment8, m_Comment8);
	DDV_MaxChars(pDX, m_Comment8, 100);
	DDX_Text(pDX, IDC_Comment9, m_Comment9);
	DDV_MaxChars(pDX, m_Comment9, 100);
	DDX_Text(pDX, IDC_Comment10, m_Comment10);
	DDV_MaxChars(pDX, m_Comment10, 100);
	DDX_Text(pDX, IDC_Comment11, m_Comment11);
	DDV_MaxChars(pDX, m_Comment11, 100);
	DDX_Text(pDX, IDC_Comment12, m_Comment12);
	DDV_MaxChars(pDX, m_Comment12, 100);
	DDX_Text(pDX, IDC_Comment13, m_Comment13);
	DDV_MaxChars(pDX, m_Comment13, 100);
	DDX_Text(pDX, IDC_Comment14, m_Comment14);
	DDV_MaxChars(pDX, m_Comment14, 100);
	DDX_Text(pDX, IDC_Comment15, m_Comment15);
	DDV_MaxChars(pDX, m_Comment15, 100);
	DDX_Text(pDX, IDC_Comment16, m_Comment16);
	DDV_MaxChars(pDX, m_Comment16, 100);
	DDX_Text(pDX, IDC_Comment17, m_Comment17);
	DDV_MaxChars(pDX, m_Comment17, 100);
	DDX_Text(pDX, IDC_Comment18, m_Comment18);
	DDV_MaxChars(pDX, m_Comment18, 100);
	DDX_Text(pDX, IDC_Comment19, m_Comment19);
	DDV_MaxChars(pDX, m_Comment19, 100);

	ChangeZerosToBlanks();

	if (pDX->m_bSaveAndValidate)
	{
		GetToolsFromScreen(OriginalPage);
	}
}


BEGIN_MESSAGE_MAP(CEditToolFile, CDialog)
	ON_BN_CLICKED(IDC_Page1, &CEditToolFile::OnBnClickedPage1)
	ON_BN_CLICKED(IDC_Page2, &CEditToolFile::OnBnClickedPage2)
	ON_BN_CLICKED(IDC_Page3, &CEditToolFile::OnBnClickedPage3)
	ON_BN_CLICKED(IDC_Page4, &CEditToolFile::OnBnClickedPage4)
	ON_BN_CLICKED(IDC_Page5, &CEditToolFile::OnBnClickedPage5)
	ON_BN_CLICKED(ID_SORT, &CEditToolFile::OnBnClickedSort)
END_MESSAGE_MAP()


// CEditToolFile message handlers


int CEditToolFile::LoadFile(CString File)
{
	CString s,Comment;
	int ID, Pocket;
	double Diameter,Length;

	FILE *f = fopen(File,"rt");

	if (!f)
	{
		AfxMessageBox("Unable to open Tool Table file:\r\r" + File);
		return 1;
	}

	if (!fgets(s.GetBufferSetLength(MAX_TOOL_LINE+1),MAX_TOOL_LINE,f))
	{
		AfxMessageBox("Invalid Tool Table file (line 1):\r\r" + File);
		fclose(f);
		return 1;
	}

	s.MakeUpper();

	if (!s.Find("ID") ||!s.Find("DIAM") || !s.Find("LENGTH"))
	{
		AfxMessageBox("Invalid Tool Table file (line 1):\r\r" + File);
		fclose(f);
		return 1;
	}

	int n,Tool=0,line=1;

	while (!feof(f))
	{
		fgets(s.GetBufferSetLength(MAX_TOOL_LINE+1),MAX_TOOL_LINE,f);
		s.ReleaseBuffer();

		if (!feof(f))
		{
			s.Remove('\n');
			s.Remove('\r');

			// remove whitespace
			while (!s.IsEmpty() && 
				(s.GetAt(0) == ' ' || s.GetAt(0) == '\t'))
				s.Delete(0,1);

			line++;

			if (!s.IsEmpty())
			{
				if (Tool >= MAX_TOOLS)
				{
					AfxMessageBox("Invalid Tool Table file:\r\r" + File + "\r\rToo many Tools Max Allowed is 99");
					fclose(f);
					return 1;
				}

				int result = sscanf(s,"%d%d%lf%lf%n",&Pocket,&ID,&Length,&Diameter,&n);

				if (result !=4)
				{
					CString err;
					err.Format("Line %d doesn't have 4 valid numbers",line);
					AfxMessageBox("Invalid Tool Table file:\r\r" + File + "\r\r" + err);
					fclose(f);
					return 1;
				}

				s.Delete(0,n);

				Comment = s;

				// remove whitespace
				while (!Comment.IsEmpty() && 
					(Comment.GetAt(0) == ' ' || Comment.GetAt(0) == '\t'))
					Comment.Delete(0,1);

				PutTool(Tool,Pocket,ID,Length,Diameter,Comment); 
				Tool++;
			}
		}
	}
	fclose(f);

	// Blank out others
	for (int i=Tool; i<MAX_TOOLS; i++)
	{
		PutTool(i,0,0,0,0,"");
	}

	m_nTools=Tool;  // remember how many tools defined
	SortToolTable();
	return 0;
}

void CEditToolFile::PutToolsToScreen(int page)
{
	CString Comment;
	int ID,Pocket;
	double Diameter,Length;

	for (int i=0; i<20; i++)
	{
		GetTool(i+page*20,Pocket,ID,Length,Diameter,Comment);
		PutToolID(i,Pocket,ID,Length,Diameter,Comment);
	}
}

void CEditToolFile::GetToolsFromScreen(int page)
{
	CString Comment;
	int ID,Pocket;
	double Diameter,Length;

	for (int i=0; i<20; i++)
	{
		GetToolID(i,Pocket,ID,Length,Diameter,Comment);
		PutTool(i+page*20,Pocket,ID,Length,Diameter,Comment);
	}
}

void CEditToolFile::PutTool(int Tool, int Pocket, int ID, double Length, double Diameter, CString Comment)
{
	if (Tool>=0 && Tool<MAX_TOOLS)
	{
		m_Pocket[Tool]=Pocket; m_ID[Tool]=ID; m_Length[Tool]=Length; m_Diameter[Tool]=Diameter; m_Comment[Tool]=Comment; 
	}
}

void CEditToolFile::GetTool(int Tool, int &Pocket, int &ID, double &Length, double &Diameter, CString &Comment)
{
	if (Tool>=0 && Tool<MAX_TOOLS)
	{
		Pocket=m_Pocket[Tool]; ID=m_ID[Tool]; Length=m_Length[Tool]; Diameter=m_Diameter[Tool]; Comment=m_Comment[Tool]; 
	}
	else
	{
		Pocket=0; ID=0; Length=0; Diameter=0; Comment=""; 
	}
}

void CEditToolFile::PutToolID(int Tool, int Pocket, int ID, double Length, double Diameter, CString Comment)
{
	switch (Tool)
	{
		case 0: m_Pocket0=Pocket; m_ID0=ID; m_Length0=Length; m_Diameter0=Diameter; m_Comment0=Comment; break; 
		case 1: m_Pocket1=Pocket; m_ID1=ID; m_Length1=Length; m_Diameter1=Diameter; m_Comment1=Comment; break; 
		case 2: m_Pocket2=Pocket; m_ID2=ID; m_Length2=Length; m_Diameter2=Diameter; m_Comment2=Comment; break; 
		case 3: m_Pocket3=Pocket; m_ID3=ID; m_Length3=Length; m_Diameter3=Diameter; m_Comment3=Comment; break; 
		case 4: m_Pocket4=Pocket; m_ID4=ID; m_Length4=Length; m_Diameter4=Diameter; m_Comment4=Comment; break; 
		case 5: m_Pocket5=Pocket; m_ID5=ID; m_Length5=Length; m_Diameter5=Diameter; m_Comment5=Comment; break; 
		case 6: m_Pocket6=Pocket; m_ID6=ID; m_Length6=Length; m_Diameter6=Diameter; m_Comment6=Comment; break; 
		case 7: m_Pocket7=Pocket; m_ID7=ID; m_Length7=Length; m_Diameter7=Diameter; m_Comment7=Comment; break; 
		case 8: m_Pocket8=Pocket; m_ID8=ID; m_Length8=Length; m_Diameter8=Diameter; m_Comment8=Comment; break; 
		case 9: m_Pocket9=Pocket; m_ID9=ID; m_Length9=Length; m_Diameter9=Diameter; m_Comment9=Comment; break; 
		case 10: m_Pocket10=Pocket; m_ID10=ID; m_Length10=Length; m_Diameter10=Diameter; m_Comment10=Comment; break; 
		case 11: m_Pocket11=Pocket; m_ID11=ID; m_Length11=Length; m_Diameter11=Diameter; m_Comment11=Comment; break; 
		case 12: m_Pocket12=Pocket; m_ID12=ID; m_Length12=Length; m_Diameter12=Diameter; m_Comment12=Comment; break; 
		case 13: m_Pocket13=Pocket; m_ID13=ID; m_Length13=Length; m_Diameter13=Diameter; m_Comment13=Comment; break; 
		case 14: m_Pocket14=Pocket; m_ID14=ID; m_Length14=Length; m_Diameter14=Diameter; m_Comment14=Comment; break; 
		case 15: m_Pocket15=Pocket; m_ID15=ID; m_Length15=Length; m_Diameter15=Diameter; m_Comment15=Comment; break; 
		case 16: m_Pocket16=Pocket; m_ID16=ID; m_Length16=Length; m_Diameter16=Diameter; m_Comment16=Comment; break; 
		case 17: m_Pocket17=Pocket; m_ID17=ID; m_Length17=Length; m_Diameter17=Diameter; m_Comment17=Comment; break; 
		case 18: m_Pocket18=Pocket; m_ID18=ID; m_Length18=Length; m_Diameter18=Diameter; m_Comment18=Comment; break; 
		case 19: m_Pocket19=Pocket; m_ID19=ID; m_Length19=Length; m_Diameter19=Diameter; m_Comment19=Comment; break; 
	}
}

void CEditToolFile::GetToolID(int Tool, int &Pocket, int &ID, double &Length, double &Diameter, CString &Comment)
{
	switch (Tool)
	{
		case 0: Pocket=m_Pocket0; ID=m_ID0; Length=m_Length0; Diameter=m_Diameter0; Comment=m_Comment0; break; 
		case 1: Pocket=m_Pocket1; ID=m_ID1; Length=m_Length1; Diameter=m_Diameter1; Comment=m_Comment1; break; 
		case 2: Pocket=m_Pocket2; ID=m_ID2; Length=m_Length2; Diameter=m_Diameter2; Comment=m_Comment2; break; 
		case 3: Pocket=m_Pocket3; ID=m_ID3; Length=m_Length3; Diameter=m_Diameter3; Comment=m_Comment3; break; 
		case 4: Pocket=m_Pocket4; ID=m_ID4; Length=m_Length4; Diameter=m_Diameter4; Comment=m_Comment4; break; 
		case 5: Pocket=m_Pocket5; ID=m_ID5; Length=m_Length5; Diameter=m_Diameter5; Comment=m_Comment5; break; 
		case 6: Pocket=m_Pocket6; ID=m_ID6; Length=m_Length6; Diameter=m_Diameter6; Comment=m_Comment6; break; 
		case 7: Pocket=m_Pocket7; ID=m_ID7; Length=m_Length7; Diameter=m_Diameter7; Comment=m_Comment7; break; 
		case 8: Pocket=m_Pocket8; ID=m_ID8; Length=m_Length8; Diameter=m_Diameter8; Comment=m_Comment8; break; 
		case 9: Pocket=m_Pocket9; ID=m_ID9; Length=m_Length9; Diameter=m_Diameter9; Comment=m_Comment9; break; 
		case 10: Pocket=m_Pocket10; ID=m_ID10; Length=m_Length10; Diameter=m_Diameter10; Comment=m_Comment10; break; 
		case 11: Pocket=m_Pocket11; ID=m_ID11; Length=m_Length11; Diameter=m_Diameter11; Comment=m_Comment11; break; 
		case 12: Pocket=m_Pocket12; ID=m_ID12; Length=m_Length12; Diameter=m_Diameter12; Comment=m_Comment12; break; 
		case 13: Pocket=m_Pocket13; ID=m_ID13; Length=m_Length13; Diameter=m_Diameter13; Comment=m_Comment13; break; 
		case 14: Pocket=m_Pocket14; ID=m_ID14; Length=m_Length14; Diameter=m_Diameter14; Comment=m_Comment14; break; 
		case 15: Pocket=m_Pocket15; ID=m_ID15; Length=m_Length15; Diameter=m_Diameter15; Comment=m_Comment15; break; 
		case 16: Pocket=m_Pocket16; ID=m_ID16; Length=m_Length16; Diameter=m_Diameter16; Comment=m_Comment16; break; 
		case 17: Pocket=m_Pocket17; ID=m_ID17; Length=m_Length17; Diameter=m_Diameter17; Comment=m_Comment17; break; 
		case 18: Pocket=m_Pocket18; ID=m_ID18; Length=m_Length18; Diameter=m_Diameter18; Comment=m_Comment18; break; 
		case 19: Pocket=m_Pocket19; ID=m_ID19; Length=m_Length19; Diameter=m_Diameter19; Comment=m_Comment19; break; 
	}
}


void CEditToolFile::SortToolTable()
{
	CString Comment,Comment2;
	int ID,ID2,Pocket,Pocket2;
	double Diameter,Diameter2,Length,Length2;
	bool something_swapped;
	// bubble sort the tool table using Pocket number as key
	do
	{
		something_swapped=false;
		for (int i=0; i<MAX_TOOLS-1; i++)
		{
			GetTool(i,Pocket,ID,Length,Diameter,Comment);
			GetTool(i+1,Pocket2,ID2,Length2,Diameter2,Comment2);
			// sort by Pocket number but put nulls at the end
			if (Pocket2!=0 && (Pocket2 < Pocket || Pocket==0))
			{
				//swap
				PutTool(i,Pocket2,ID2,Length2,Diameter2,Comment2);
				PutTool(i+1,Pocket,ID,Length,Diameter,Comment);
				something_swapped=true;
			}
		}
	}
	while (something_swapped);
	for (m_nTools=0; m_nTools<MAX_TOOLS; m_nTools++)
		if (!Pocket && !ID) break;
}

int CEditToolFile::SaveFile(CString File)
{
	FILE *f = fopen(File,"wt");

	SortToolTable();
	if (!f)
	{
		AfxMessageBox("Unable to write Tool Table file:\r\r" + File);
		return 1;
	}

	fprintf(f,"POCKET  ID        LENGTH         DIAMETER  COMMENT\n");
	fprintf(f,"\n");

	CString Comment;
	int ID, Pocket;
	double Diameter,Length;

	for (int i=0; i<MAX_TOOLS; i++)
	{
		GetTool(i,Pocket,ID,Length,Diameter,Comment);
		if (Pocket || ID) fprintf(f,"%3d %6d %15.6f %15.6f %s\n",Pocket,ID,Length,Diameter,Comment);
	}
	fclose(f);
	return 0;
}


void CEditToolFile::ChangeLength(int Tool, double NewLength)
{
	CString Comment;
	int ID, Pocket;
	double Diameter,Length;

	SortToolTable();
	for (int i=0; i<m_nTools; i++)
	{
		GetTool(i,Pocket,ID,Length,Diameter,Comment);
		if (Tool==Pocket)
		{
			PutTool(i,Pocket,ID,NewLength,Diameter,Comment);
			return;
		}
	}
	if (Tool >= MAX_TOOLS)
	{
		AfxMessageBox("Tool Length Change to non-existing Tool with Max Allowed Tools already 100");
		return;
	}
	PutTool(m_nTools,Tool,Tool,NewLength,0.0,"");
	SortToolTable();
}

void CEditToolFile::OnBnClickedPage1()
{
	if (!UpdateData(TRUE)) return;
	UpdateData(FALSE);
}

void CEditToolFile::OnBnClickedPage2()
{
	if (!UpdateData(TRUE)) return;
	UpdateData(FALSE);
}

void CEditToolFile::OnBnClickedPage3()
{
	if (!UpdateData(TRUE)) return;
	UpdateData(FALSE);
}

void CEditToolFile::OnBnClickedPage4()
{
	if (!UpdateData(TRUE)) return;
	UpdateData(FALSE);
}

void CEditToolFile::OnBnClickedPage5()
{
	if (!UpdateData(TRUE)) return;
	UpdateData(FALSE);
}

void CEditToolFile::OnBnClickedSort()
{
	if (!UpdateData(TRUE)) return;
	SortToolTable();
	UpdateData(FALSE);
}
