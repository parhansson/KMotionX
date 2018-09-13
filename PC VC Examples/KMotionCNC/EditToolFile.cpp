// EditToolFile.cpp : implementation file
//

#include "stdafx.h"
#include "EditToolFile.h"
#include "OpenDlg.h"
#include "Frame.h"
#include "CPreviewFileDialog.h"
#include "DoPreview.h"


#define MAX_TOOL_LINE 200

// CEditToolFile dialog

IMPLEMENT_DYNAMIC(CEditToolFile, CDialog)

CEditToolFile::CEditToolFile(CWnd* pParent /*=NULL*/)
	: CDialog(CEditToolFile::IDD, pParent)
	, m_Slot0(0)
	, m_Slot1(0)
	, m_Slot2(0)
	, m_Slot3(0)
	, m_Slot4(0)
	, m_Slot5(0)
	, m_Slot6(0)
	, m_Slot7(0)
	, m_Slot8(0)
	, m_Slot9(0)
	, m_Slot10(0)
	, m_Slot11(0)
	, m_Slot12(0)
	, m_Slot13(0)
	, m_Slot14(0)
	, m_Slot15(0)
	, m_Slot16(0)
	, m_Slot17(0)
	, m_Slot18(0)
	, m_Slot19(0)
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
	
	, m_Xoffset0(0)
	, m_Xoffset1(0)
	, m_Xoffset2(0)
	, m_Xoffset3(0)
	, m_Xoffset4(0)
	, m_Xoffset5(0)
	, m_Xoffset6(0)
	, m_Xoffset7(0)
	, m_Xoffset8(0)
	, m_Xoffset9(0)
	, m_Xoffset10(0)
	, m_Xoffset11(0)
	, m_Xoffset12(0)
	, m_Xoffset13(0)
	, m_Xoffset14(0)
	, m_Xoffset15(0)
	, m_Xoffset16(0)
	, m_Xoffset17(0)
	, m_Xoffset18(0)
	, m_Xoffset19(0)
	
	
	, m_Yoffset0(0)
	, m_Yoffset1(0)
	, m_Yoffset2(0)
	, m_Yoffset3(0)
	, m_Yoffset4(0)
	, m_Yoffset5(0)
	, m_Yoffset6(0)
	, m_Yoffset7(0)
	, m_Yoffset8(0)
	, m_Yoffset9(0)
	, m_Yoffset10(0)
	, m_Yoffset11(0)
	, m_Yoffset12(0)
	, m_Yoffset13(0)
	, m_Yoffset14(0)
	, m_Yoffset15(0)
	, m_Yoffset16(0)
	, m_Yoffset17(0)
	, m_Yoffset18(0)
	, m_Yoffset19(0)
	
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

	, m_Image0(_T(""))
	, m_Image1(_T(""))
	, m_Image2(_T(""))
	, m_Image3(_T(""))
	, m_Image4(_T(""))
	, m_Image5(_T(""))
	, m_Image6(_T(""))
	, m_Image7(_T(""))
	, m_Image8(_T(""))
	, m_Image9(_T(""))
	, m_Image10(_T(""))
	, m_Image11(_T(""))
	, m_Image12(_T(""))
	, m_Image13(_T(""))
	, m_Image14(_T(""))
	, m_Image15(_T(""))
	, m_Image16(_T(""))
	, m_Image17(_T(""))
	, m_Image18(_T(""))
	, m_Image19(_T(""))



	, m_CurPage(0)
	{
	m_nTools=0;
	m_CurPage=0;
}

CEditToolFile::~CEditToolFile()
{
}






void CEditToolFile::DoDataExchange(CDataExchange* pDX)
{
	int OriginalPage=m_CurPage;

	if (!pDX->m_bSaveAndValidate)
	{
		PutToolsToScreen(OriginalPage);
	}
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_Page1, m_CurPage);
	DDX_Text_Blank(pDX, IDC_Slot0, m_Slot0);
	DDV_MinMaxInt_Blank(pDX, m_Slot0, 0, 999);
	DDX_Text_Blank(pDX, IDC_Slot1, m_Slot1);
	DDV_MinMaxInt_Blank(pDX, m_Slot1, 0, 999);
	DDX_Text_Blank(pDX, IDC_Slot2, m_Slot2);
	DDV_MinMaxInt_Blank(pDX, m_Slot2, 0, 999);
	DDX_Text_Blank(pDX, IDC_Slot3, m_Slot3);
	DDV_MinMaxInt_Blank(pDX, m_Slot3, 0, 999);
	DDX_Text_Blank(pDX, IDC_Slot4, m_Slot4);
	DDV_MinMaxInt_Blank(pDX, m_Slot4, 0, 999);
	DDX_Text_Blank(pDX, IDC_Slot5, m_Slot5);
	DDV_MinMaxInt_Blank(pDX, m_Slot5, 0, 999);
	DDX_Text_Blank(pDX, IDC_Slot6, m_Slot6);
	DDV_MinMaxInt_Blank(pDX, m_Slot6, 0, 999);
	DDX_Text_Blank(pDX, IDC_Slot7, m_Slot7);
	DDV_MinMaxInt_Blank(pDX, m_Slot7, 0, 999);
	DDX_Text_Blank(pDX, IDC_Slot8, m_Slot8);
	DDV_MinMaxInt_Blank(pDX, m_Slot8, 0, 999);
	DDX_Text_Blank(pDX, IDC_Slot9, m_Slot9);
	DDV_MinMaxInt_Blank(pDX, m_Slot9, 0, 999);
	DDX_Text_Blank(pDX, IDC_Slot10, m_Slot10);
	DDV_MinMaxInt_Blank(pDX, m_Slot10, 0, 999);
	DDX_Text_Blank(pDX, IDC_Slot11, m_Slot11);
	DDV_MinMaxInt_Blank(pDX, m_Slot11, 0, 999);
	DDX_Text_Blank(pDX, IDC_Slot12, m_Slot12);
	DDV_MinMaxInt_Blank(pDX, m_Slot12, 0, 999);
	DDX_Text_Blank(pDX, IDC_Slot13, m_Slot13);
	DDV_MinMaxInt_Blank(pDX, m_Slot13, 0, 999);
	DDX_Text_Blank(pDX, IDC_Slot14, m_Slot14);
	DDV_MinMaxInt_Blank(pDX, m_Slot14, 0, 999);
	DDX_Text_Blank(pDX, IDC_Slot15, m_Slot15);
	DDV_MinMaxInt_Blank(pDX, m_Slot15, 0, 999);
	DDX_Text_Blank(pDX, IDC_Slot16, m_Slot16);
	DDV_MinMaxInt_Blank(pDX, m_Slot16, 0, 999);
	DDX_Text_Blank(pDX, IDC_Slot17, m_Slot17);
	DDV_MinMaxInt_Blank(pDX, m_Slot17, 0, 999);
	DDX_Text_Blank(pDX, IDC_Slot18, m_Slot18);
	DDV_MinMaxInt_Blank(pDX, m_Slot18, 0, 999);
	DDX_Text_Blank(pDX, IDC_Slot19, m_Slot19);
	DDV_MinMaxInt_Blank(pDX, m_Slot19, 0, 999);

	DDX_Text_Blank(pDX, IDC_ID0, m_ID0);
	DDV_MinMaxInt_Blank(pDX, m_ID0, 0, 999999);
	DDX_Text_Blank(pDX, IDC_ID1, m_ID1);
	DDV_MinMaxInt_Blank(pDX, m_ID1, 0, 999999);
	DDX_Text_Blank(pDX, IDC_ID2, m_ID2);
	DDV_MinMaxInt_Blank(pDX, m_ID2, 0, 999999);
	DDX_Text_Blank(pDX, IDC_ID3, m_ID3);
	DDV_MinMaxInt_Blank(pDX, m_ID3, 0, 999999);
	DDX_Text_Blank(pDX, IDC_ID4, m_ID4);
	DDV_MinMaxInt_Blank(pDX, m_ID4, 0, 999999);
	DDX_Text_Blank(pDX, IDC_ID5, m_ID5);
	DDV_MinMaxInt_Blank(pDX, m_ID5, 0, 999999);
	DDX_Text_Blank(pDX, IDC_ID6, m_ID6);
	DDV_MinMaxInt_Blank(pDX, m_ID6, 0, 999999);
	DDX_Text_Blank(pDX, IDC_ID7, m_ID7);
	DDV_MinMaxInt_Blank(pDX, m_ID7, 0, 999999);
	DDX_Text_Blank(pDX, IDC_ID8, m_ID8);
	DDV_MinMaxInt_Blank(pDX, m_ID8, 0, 999999);
	DDX_Text_Blank(pDX, IDC_ID9, m_ID9);
	DDV_MinMaxInt_Blank(pDX, m_ID9, 0, 999999);
	DDX_Text_Blank(pDX, IDC_ID10, m_ID10);
	DDV_MinMaxInt_Blank(pDX, m_ID10, 0, 999999);
	DDX_Text_Blank(pDX, IDC_ID11, m_ID11);
	DDV_MinMaxInt_Blank(pDX, m_ID11, 0, 999999);
	DDX_Text_Blank(pDX, IDC_ID12, m_ID12);
	DDV_MinMaxInt_Blank(pDX, m_ID12, 0, 999999);
	DDX_Text_Blank(pDX, IDC_ID13, m_ID13);
	DDV_MinMaxInt_Blank(pDX, m_ID13, 0, 999999);
	DDX_Text_Blank(pDX, IDC_ID14, m_ID14);
	DDV_MinMaxInt_Blank(pDX, m_ID14, 0, 999999);
	DDX_Text_Blank(pDX, IDC_ID15, m_ID15);
	DDV_MinMaxInt_Blank(pDX, m_ID15, 0, 999999);
	DDX_Text_Blank(pDX, IDC_ID16, m_ID16);
	DDV_MinMaxInt_Blank(pDX, m_ID16, 0, 999999);
	DDX_Text_Blank(pDX, IDC_ID17, m_ID17);
	DDV_MinMaxInt_Blank(pDX, m_ID17, 0, 999999);
	DDX_Text_Blank(pDX, IDC_ID18, m_ID18);
	DDV_MinMaxInt_Blank(pDX, m_ID18, 0, 999999);
	DDX_Text_Blank(pDX, IDC_ID19, m_ID19);
	DDV_MinMaxInt_Blank(pDX, m_ID19, 0, 999999);

	DDX_Text_Blank(pDX, IDC_Length0, m_Length0);
	DDV_MinMaxDouble_Blank(pDX, m_Length0, -999,999);
	DDX_Text_Blank(pDX, IDC_Length1, m_Length1);
	DDV_MinMaxDouble_Blank(pDX, m_Length1, -999,999);
	DDX_Text_Blank(pDX, IDC_Length2, m_Length2);
	DDV_MinMaxDouble_Blank(pDX, m_Length2, -999,999);
	DDX_Text_Blank(pDX, IDC_Length3, m_Length3);
	DDV_MinMaxDouble_Blank(pDX, m_Length3, -999,999);
	DDX_Text_Blank(pDX, IDC_Length4, m_Length4);
	DDV_MinMaxDouble_Blank(pDX, m_Length4, -999,999);
	DDX_Text_Blank(pDX, IDC_Length5, m_Length5);
	DDV_MinMaxDouble_Blank(pDX, m_Length5, -999,999);
	DDX_Text_Blank(pDX, IDC_Length6, m_Length6);
	DDV_MinMaxDouble_Blank(pDX, m_Length6, -999,999);
	DDX_Text_Blank(pDX, IDC_Length7, m_Length7);
	DDV_MinMaxDouble_Blank(pDX, m_Length7, -999,999);
	DDX_Text_Blank(pDX, IDC_Length8, m_Length8);
	DDV_MinMaxDouble_Blank(pDX, m_Length8, -999,999);
	DDX_Text_Blank(pDX, IDC_Length9, m_Length9);
	DDV_MinMaxDouble_Blank(pDX, m_Length9, -999,999);
	DDX_Text_Blank(pDX, IDC_Length10, m_Length10);
	DDV_MinMaxDouble_Blank(pDX, m_Length10, -999,999);
	DDX_Text_Blank(pDX, IDC_Length11, m_Length11);
	DDV_MinMaxDouble_Blank(pDX, m_Length11, -999,999);
	DDX_Text_Blank(pDX, IDC_Length12, m_Length12);
	DDV_MinMaxDouble_Blank(pDX, m_Length12, -999,999);
	DDX_Text_Blank(pDX, IDC_Length13, m_Length13);
	DDV_MinMaxDouble_Blank(pDX, m_Length13, -999,999);
	DDX_Text_Blank(pDX, IDC_Length14, m_Length14);
	DDV_MinMaxDouble_Blank(pDX, m_Length14, -999,999);
	DDX_Text_Blank(pDX, IDC_Length15, m_Length15);
	DDV_MinMaxDouble_Blank(pDX, m_Length15, -999,999);
	DDX_Text_Blank(pDX, IDC_Length16, m_Length16);
	DDV_MinMaxDouble_Blank(pDX, m_Length16, -999,999);
	DDX_Text_Blank(pDX, IDC_Length17, m_Length17);
	DDV_MinMaxDouble_Blank(pDX, m_Length17, -999,999);
	DDX_Text_Blank(pDX, IDC_Length18, m_Length18);
	DDV_MinMaxDouble_Blank(pDX, m_Length18, -999,999);
	DDX_Text_Blank(pDX, IDC_Length19, m_Length19);
	DDV_MinMaxDouble_Blank(pDX, m_Length19, -999,999);

	DDX_Text_Blank(pDX, IDC_Xoffset0, m_Xoffset0);
	DDV_MinMaxDouble_Blank(pDX, m_Xoffset0, -999,999);
	DDX_Text_Blank(pDX, IDC_Xoffset1, m_Xoffset1);
	DDV_MinMaxDouble_Blank(pDX, m_Xoffset1, -999,999);
	DDX_Text_Blank(pDX, IDC_Xoffset2, m_Xoffset2);
	DDV_MinMaxDouble_Blank(pDX, m_Xoffset2, -999,999);
	DDX_Text_Blank(pDX, IDC_Xoffset3, m_Xoffset3);
	DDV_MinMaxDouble_Blank(pDX, m_Xoffset3, -999,999);
	DDX_Text_Blank(pDX, IDC_Xoffset4, m_Xoffset4);
	DDV_MinMaxDouble_Blank(pDX, m_Xoffset4, -999,999);
	DDX_Text_Blank(pDX, IDC_Xoffset5, m_Xoffset5);
	DDV_MinMaxDouble_Blank(pDX, m_Xoffset5, -999,999);
	DDX_Text_Blank(pDX, IDC_Xoffset6, m_Xoffset6);
	DDV_MinMaxDouble_Blank(pDX, m_Xoffset6, -999,999);
	DDX_Text_Blank(pDX, IDC_Xoffset7, m_Xoffset7);
	DDV_MinMaxDouble_Blank(pDX, m_Xoffset7, -999,999);
	DDX_Text_Blank(pDX, IDC_Xoffset8, m_Xoffset8);
	DDV_MinMaxDouble_Blank(pDX, m_Xoffset8, -999,999);
	DDX_Text_Blank(pDX, IDC_Xoffset9, m_Xoffset9);
	DDV_MinMaxDouble_Blank(pDX, m_Xoffset9, -999,999);
	DDX_Text_Blank(pDX, IDC_Xoffset10, m_Xoffset10);
	DDV_MinMaxDouble_Blank(pDX, m_Xoffset10, -999,999);
	DDX_Text_Blank(pDX, IDC_Xoffset11, m_Xoffset11);
	DDV_MinMaxDouble_Blank(pDX, m_Xoffset11, -999,999);
	DDX_Text_Blank(pDX, IDC_Xoffset12, m_Xoffset12);
	DDV_MinMaxDouble_Blank(pDX, m_Xoffset12, -999,999);
	DDX_Text_Blank(pDX, IDC_Xoffset13, m_Xoffset13);
	DDV_MinMaxDouble_Blank(pDX, m_Xoffset13, -999,999);
	DDX_Text_Blank(pDX, IDC_Xoffset14, m_Xoffset14);
	DDV_MinMaxDouble_Blank(pDX, m_Xoffset14, -999,999);
	DDX_Text_Blank(pDX, IDC_Xoffset15, m_Xoffset15);
	DDV_MinMaxDouble_Blank(pDX, m_Xoffset15, -999,999);
	DDX_Text_Blank(pDX, IDC_Xoffset16, m_Xoffset16);
	DDV_MinMaxDouble_Blank(pDX, m_Xoffset16, -999,999);
	DDX_Text_Blank(pDX, IDC_Xoffset17, m_Xoffset17);
	DDV_MinMaxDouble_Blank(pDX, m_Xoffset17, -999,999);
	DDX_Text_Blank(pDX, IDC_Xoffset18, m_Xoffset18);
	DDV_MinMaxDouble_Blank(pDX, m_Xoffset18, -999,999);
	DDX_Text_Blank(pDX, IDC_Xoffset19, m_Xoffset19);
	DDV_MinMaxDouble_Blank(pDX, m_Xoffset19, -999,999);

	DDX_Text_Blank(pDX, IDC_Yoffset0, m_Yoffset0);
	DDV_MinMaxDouble_Blank(pDX, m_Yoffset0, -999,999);
	DDX_Text_Blank(pDX, IDC_Yoffset1, m_Yoffset1);
	DDV_MinMaxDouble_Blank(pDX, m_Yoffset1, -999,999);
	DDX_Text_Blank(pDX, IDC_Yoffset2, m_Yoffset2);
	DDV_MinMaxDouble_Blank(pDX, m_Yoffset2, -999,999);
	DDX_Text_Blank(pDX, IDC_Yoffset3, m_Yoffset3);
	DDV_MinMaxDouble_Blank(pDX, m_Yoffset3, -999,999);
	DDX_Text_Blank(pDX, IDC_Yoffset4, m_Yoffset4);
	DDV_MinMaxDouble_Blank(pDX, m_Yoffset4, -999,999);
	DDX_Text_Blank(pDX, IDC_Yoffset5, m_Yoffset5);
	DDV_MinMaxDouble_Blank(pDX, m_Yoffset5, -999,999);
	DDX_Text_Blank(pDX, IDC_Yoffset6, m_Yoffset6);
	DDV_MinMaxDouble_Blank(pDX, m_Yoffset6, -999,999);
	DDX_Text_Blank(pDX, IDC_Yoffset7, m_Yoffset7);
	DDV_MinMaxDouble_Blank(pDX, m_Yoffset7, -999,999);
	DDX_Text_Blank(pDX, IDC_Yoffset8, m_Yoffset8);
	DDV_MinMaxDouble_Blank(pDX, m_Yoffset8, -999,999);
	DDX_Text_Blank(pDX, IDC_Yoffset9, m_Yoffset9);
	DDV_MinMaxDouble_Blank(pDX, m_Yoffset9, -999,999);
	DDX_Text_Blank(pDX, IDC_Yoffset10, m_Yoffset10);
	DDV_MinMaxDouble_Blank(pDX, m_Yoffset10, -999,999);
	DDX_Text_Blank(pDX, IDC_Yoffset11, m_Yoffset11);
	DDV_MinMaxDouble_Blank(pDX, m_Yoffset11, -999,999);
	DDX_Text_Blank(pDX, IDC_Yoffset12, m_Yoffset12);
	DDV_MinMaxDouble_Blank(pDX, m_Yoffset12, -999,999);
	DDX_Text_Blank(pDX, IDC_Yoffset13, m_Yoffset13);
	DDV_MinMaxDouble_Blank(pDX, m_Yoffset13, -999,999);
	DDX_Text_Blank(pDX, IDC_Yoffset14, m_Yoffset14);
	DDV_MinMaxDouble_Blank(pDX, m_Yoffset14, -999,999);
	DDX_Text_Blank(pDX, IDC_Yoffset15, m_Yoffset15);
	DDV_MinMaxDouble_Blank(pDX, m_Yoffset15, -999,999);
	DDX_Text_Blank(pDX, IDC_Yoffset16, m_Yoffset16);
	DDV_MinMaxDouble_Blank(pDX, m_Yoffset16, -999,999);
	DDX_Text_Blank(pDX, IDC_Yoffset17, m_Yoffset17);
	DDV_MinMaxDouble_Blank(pDX, m_Yoffset17, -999,999);
	DDX_Text_Blank(pDX, IDC_Yoffset18, m_Yoffset18);
	DDV_MinMaxDouble_Blank(pDX, m_Yoffset18, -999,999);
	DDX_Text_Blank(pDX, IDC_Yoffset19, m_Yoffset19);
	DDV_MinMaxDouble_Blank(pDX, m_Yoffset19, -999,999);


	DDX_Text_Blank(pDX, IDC_Diameter0, m_Diameter0);
	DDV_MinMaxDouble_Blank(pDX, m_Diameter0, -999, 999);
	DDX_Text_Blank(pDX, IDC_Diameter1, m_Diameter1);
	DDV_MinMaxDouble_Blank(pDX, m_Diameter1, -999, 999);
	DDX_Text_Blank(pDX, IDC_Diameter2, m_Diameter2);
	DDV_MinMaxDouble_Blank(pDX, m_Diameter2, -999, 999);
	DDX_Text_Blank(pDX, IDC_Diameter3, m_Diameter3);
	DDV_MinMaxDouble_Blank(pDX, m_Diameter3, -999, 999);
	DDX_Text_Blank(pDX, IDC_Diameter4, m_Diameter4);
	DDV_MinMaxDouble_Blank(pDX, m_Diameter4, -999, 999);
	DDX_Text_Blank(pDX, IDC_Diameter5, m_Diameter5);
	DDV_MinMaxDouble_Blank(pDX, m_Diameter5, -999, 999);
	DDX_Text_Blank(pDX, IDC_Diameter6, m_Diameter6);
	DDV_MinMaxDouble_Blank(pDX, m_Diameter6, -999, 999);
	DDX_Text_Blank(pDX, IDC_Diameter7, m_Diameter7);
	DDV_MinMaxDouble_Blank(pDX, m_Diameter7, -999, 999);
	DDX_Text_Blank(pDX, IDC_Diameter8, m_Diameter8);
	DDV_MinMaxDouble_Blank(pDX, m_Diameter8, -999, 999);
	DDX_Text_Blank(pDX, IDC_Diameter9, m_Diameter9);
	DDV_MinMaxDouble_Blank(pDX, m_Diameter9, -999, 999);
	DDX_Text_Blank(pDX, IDC_Diameter10, m_Diameter10);
	DDV_MinMaxDouble_Blank(pDX, m_Diameter10, -999, 999);
	DDX_Text_Blank(pDX, IDC_Diameter11, m_Diameter11);
	DDV_MinMaxDouble_Blank(pDX, m_Diameter11, -999, 999);
	DDX_Text_Blank(pDX, IDC_Diameter12, m_Diameter12);
	DDV_MinMaxDouble_Blank(pDX, m_Diameter12, -999, 999);
	DDX_Text_Blank(pDX, IDC_Diameter13, m_Diameter13);
	DDV_MinMaxDouble_Blank(pDX, m_Diameter13, -999, 999);
	DDX_Text_Blank(pDX, IDC_Diameter14, m_Diameter14);
	DDV_MinMaxDouble_Blank(pDX, m_Diameter14, -999, 999);
	DDX_Text_Blank(pDX, IDC_Diameter15, m_Diameter15);
	DDV_MinMaxDouble_Blank(pDX, m_Diameter15, -999, 999);
	DDX_Text_Blank(pDX, IDC_Diameter16, m_Diameter16);
	DDV_MinMaxDouble_Blank(pDX, m_Diameter16, -999, 999);
	DDX_Text_Blank(pDX, IDC_Diameter17, m_Diameter17);
	DDV_MinMaxDouble_Blank(pDX, m_Diameter17, -999, 999);
	DDX_Text_Blank(pDX, IDC_Diameter18, m_Diameter18);
	DDV_MinMaxDouble_Blank(pDX, m_Diameter18, -999, 999);
	DDX_Text_Blank(pDX, IDC_Diameter19, m_Diameter19);
	DDV_MinMaxDouble_Blank(pDX, m_Diameter19, -999, 999);


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


	DDX_Text(pDX, IDC_Image0, m_Image0);
	DDV_MaxChars(pDX, m_Image0, 100);
	DDX_Text(pDX, IDC_Image1, m_Image1);
	DDV_MaxChars(pDX, m_Image1, 100);
	DDX_Text(pDX, IDC_Image2, m_Image2);
	DDV_MaxChars(pDX, m_Image2, 100);
	DDX_Text(pDX, IDC_Image3, m_Image3);
	DDV_MaxChars(pDX, m_Image3, 100);
	DDX_Text(pDX, IDC_Image4, m_Image4);
	DDV_MaxChars(pDX, m_Image4, 100);
	DDX_Text(pDX, IDC_Image5, m_Image5);
	DDV_MaxChars(pDX, m_Image5, 100);
	DDX_Text(pDX, IDC_Image6, m_Image6);
	DDV_MaxChars(pDX, m_Image6, 100);
	DDX_Text(pDX, IDC_Image7, m_Image7);
	DDV_MaxChars(pDX, m_Image7, 100);
	DDX_Text(pDX, IDC_Image8, m_Image8);
	DDV_MaxChars(pDX, m_Image8, 100);
	DDX_Text(pDX, IDC_Image9, m_Image9);
	DDV_MaxChars(pDX, m_Image9, 100);
	DDX_Text(pDX, IDC_Image10, m_Image10);
	DDV_MaxChars(pDX, m_Image10, 100);
	DDX_Text(pDX, IDC_Image11, m_Image11);
	DDV_MaxChars(pDX, m_Image11, 100);
	DDX_Text(pDX, IDC_Image12, m_Image12);
	DDV_MaxChars(pDX, m_Image12, 100);
	DDX_Text(pDX, IDC_Image13, m_Image13);
	DDV_MaxChars(pDX, m_Image13, 100);
	DDX_Text(pDX, IDC_Image14, m_Image14);
	DDV_MaxChars(pDX, m_Image14, 100);
	DDX_Text(pDX, IDC_Image15, m_Image15);
	DDV_MaxChars(pDX, m_Image15, 100);
	DDX_Text(pDX, IDC_Image16, m_Image16);
	DDV_MaxChars(pDX, m_Image16, 100);
	DDX_Text(pDX, IDC_Image17, m_Image17);
	DDV_MaxChars(pDX, m_Image17, 100);
	DDX_Text(pDX, IDC_Image18, m_Image18);
	DDV_MaxChars(pDX, m_Image18, 100);
	DDX_Text(pDX, IDC_Image19, m_Image19);
	DDV_MaxChars(pDX, m_Image19, 100);

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
	ON_BN_CLICKED(IDC_SORT, &CEditToolFile::OnBnClickedSort)
	ON_BN_CLICKED(IDC_ImageDir0, &CEditToolFile::OnBnClickedImagedir0)
	ON_BN_CLICKED(IDC_ImageDir1, &CEditToolFile::OnBnClickedImagedir1)
	ON_BN_CLICKED(IDC_ImageDir2, &CEditToolFile::OnBnClickedImagedir2)
	ON_BN_CLICKED(IDC_ImageDir3, &CEditToolFile::OnBnClickedImagedir3)
	ON_BN_CLICKED(IDC_ImageDir4, &CEditToolFile::OnBnClickedImagedir4)
	ON_BN_CLICKED(IDC_ImageDir5, &CEditToolFile::OnBnClickedImagedir5)
	ON_BN_CLICKED(IDC_ImageDir6, &CEditToolFile::OnBnClickedImagedir6)
	ON_BN_CLICKED(IDC_ImageDir7, &CEditToolFile::OnBnClickedImagedir7)
	ON_BN_CLICKED(IDC_ImageDir8, &CEditToolFile::OnBnClickedImagedir8)
	ON_BN_CLICKED(IDC_ImageDir9, &CEditToolFile::OnBnClickedImagedir9)
	ON_BN_CLICKED(IDC_ImageDir10, &CEditToolFile::OnBnClickedImagedir10)
	ON_BN_CLICKED(IDC_ImageDir11, &CEditToolFile::OnBnClickedImagedir11)
	ON_BN_CLICKED(IDC_ImageDir12, &CEditToolFile::OnBnClickedImagedir12)
	ON_BN_CLICKED(IDC_ImageDir13, &CEditToolFile::OnBnClickedImagedir13)
	ON_BN_CLICKED(IDC_ImageDir14, &CEditToolFile::OnBnClickedImagedir14)
	ON_BN_CLICKED(IDC_ImageDir15, &CEditToolFile::OnBnClickedImagedir15)
	ON_BN_CLICKED(IDC_ImageDir16, &CEditToolFile::OnBnClickedImagedir16)
	ON_BN_CLICKED(IDC_ImageDir17, &CEditToolFile::OnBnClickedImagedir17)
	ON_BN_CLICKED(IDC_ImageDir18, &CEditToolFile::OnBnClickedImagedir18)
	ON_BN_CLICKED(IDC_ImageDir19, &CEditToolFile::OnBnClickedImagedir19)
END_MESSAGE_MAP()


void CEditToolFile::DDX_Text_Blank(CDataExchange* pDX, int nIDC, int& value)
{
	char Buf[MAX_TOOL_LINE+1];
	m_prevID=nIDC;
	GetDlgItemText(nIDC,Buf,MAX_TOOL_LINE);
	if (Buf[0]==0)SetDlgItemText(nIDC,"0");
	DDX_Text(pDX, nIDC, value);
}

void CEditToolFile::DDX_Text_Blank(CDataExchange* pDX, int nIDC, double& value)
{
	char Buf[MAX_TOOL_LINE+1];
	m_prevID=nIDC;
	GetDlgItemText(nIDC,Buf,MAX_TOOL_LINE);
	if (Buf[0]==0)SetDlgItemText(nIDC,"0");
	DDX_Text(pDX, nIDC, value);
}

void CEditToolFile::DDV_MinMaxInt_Blank(CDataExchange* pDX, int value, int minVal, int maxVal)
{
	char Buf[MAX_TOOL_LINE+1];
	DDV_MinMaxInt(pDX, value, minVal, maxVal);
	GetDlgItemText(m_prevID,Buf,MAX_TOOL_LINE);
	if (Buf[0]=='0' && Buf[1]==0) SetDlgItemText(m_prevID,"");
}

void CEditToolFile::DDV_MinMaxDouble_Blank(CDataExchange* pDX, double const& value, double minVal, double maxVal)
{
	char Buf[MAX_TOOL_LINE+1];
	DDV_MinMaxDouble(pDX, value, minVal, maxVal);
	GetDlgItemText(m_prevID,Buf,MAX_TOOL_LINE);
	if (Buf[0]=='0' && Buf[1]==0) SetDlgItemText(m_prevID,"");
}

// CEditToolFile message handlers


int CEditToolFile::LoadFile(CString File)
{
	int result,i,Revision=0;
	CString Comment,Image,s;
	int ID,Slot;
	double Diameter,Length,Xoffset,Yoffset;
	CStdioFile f;

	// check if there is no path specified, then add in default

	if (File.Find(':') == -1 && File.Find("\\\\") == -1)
	{
		File = TheFrame->MainPathRoot + DATA_SUB_DIR + File;
	}


	if(!f.Open(File, CFile::modeRead|CFile::typeText))
	{
		AfxMessageBox("Unable to open Tool Table file:\r\r" + File);
		return 1;
	}

	if(!f.ReadString(s))
	{
		AfxMessageBox("Invalid Tool Table file (line 1):\r\r" + File);
		f.Close();
		return 1;
	}

	s.MakeUpper();

	if (s.Find("COMMENT")==-1 || s.Find("DIAM")==-1 || s.Find("LEN")==-1)
	{
		AfxMessageBox("Invalid Tool Table file (line 1):\r\r" + File);
		f.Close();
		return 1;
	}

	if (s.Find("XOFFSET")>0 && s.Find("YOFFSET")>0 && s.Find("IMAGE")>0) Revision = 1;


	int n,Tool=0,line=1;

	while (f.ReadString(s))
	{
		// remove whitespace
		s.Trim();
		line++;

		if (!s.IsEmpty())
		{
			if (Tool >= MAX_TOOLS)
			{
				AfxMessageBox("Invalid Tool Table file:\r\r" + File + "\r\rToo many Tools Max Allowed is 99");
				f.Close();
				return 1;
			}

			if (Revision==0)
			{
				result = sscanf(s,"%d%d%lf%lf%n",&Slot,&ID,&Length,&Diameter,&n);
				Xoffset=Yoffset=0;

				if (result !=4)
				{
					CString err;
					err.Format("Line %d doesn't have 4 valid numbers",line);
					AfxMessageBox("Invalid Tool Table file:\r\r" + File + "\r\r" + err);
					f.Close();
					return 1;
				}
			}
			else
			{
				result = sscanf(s,"%d%d%lf%lf%lf%lf%n",&Slot,&ID,&Length,&Diameter,&Xoffset,&Yoffset,&n);
				if (result !=6)
				{
					CString err;
					err.Format("Line %d doesn't have 6 valid numbers",line);
					AfxMessageBox("Invalid Tool Table file:\r\r" + File + "\r\r" + err);
					f.Close();
					return 1;
				}
			}

			s.Delete(0,n);

			Comment = s;
			//remove beginning and ending whitespace
			Comment.Trim();
			if (Revision==0)
			{
				Image = "";
				PutTool(Tool,Slot,ID,Length,Diameter,Xoffset,Yoffset,Comment,Image); 
				Tool++;
			}
			else
			{
				BOOL bImageSuccess = TRUE;
				BOOL bCommentSuccess = TRUE;
				//first isolate our image name
				if(Comment.Right(1) == '"')
				{
					Comment.Delete(Comment.GetLength()-1,1);
					i=Comment.ReverseFind('"');
					if (i!=-1)
					{
						Image = Comment.Mid(i+1);
						Comment = Comment.Left(i-1);
						Comment.Trim();
					}
					else
					{
						bImageSuccess = FALSE;
					}
				}

				if(bImageSuccess)
				{
					//Isolate our comment 
					if(Comment.GetAt(0) == '"' && Comment.Right(1) == '"')
					{
						Comment.Delete(0,1);
						Comment.Delete(Comment.GetLength()-1,1);
					}
					else
					{
						bCommentSuccess = FALSE;
					}
				}
				if(!bImageSuccess)
				{
					CString err;
					err.Format("Line %d doesn't have matching quotation marks for Image",line);
					AfxMessageBox("Invalid Tool Table file:\r\r" + File + "\r\r" + err);
					f.Close();
					return 1;
				}

				if (!bCommentSuccess)
				{
					CString err;
					err.Format("Line %d doesn't have quotation marks for comment",line);
					AfxMessageBox("Invalid Tool Table file:\r\r" + File + "\r\r" + err);
					f.Close();
					return 1;
				}

				PutTool(Tool,Slot,ID,Length,Diameter,Xoffset,Yoffset,Comment,Image); 
				Tool++;
			}
		}
	}
	f.Close();

	// Blank out others
	for (int i=Tool; i<MAX_TOOLS; i++)
	{
		PutTool(i,0,0,0,0,0,0,"","");
	}

	m_nTools=Tool;  // remember how many tools defined
	return 0;
}

void CEditToolFile::PutToolsToScreen(int page)
{
	CString Comment,Image;
	int ID,Slot;
	double Diameter,Length,Xoffset,Yoffset;

	for (int i=0; i<20; i++)
	{
		GetTool(i+page*20,Slot,ID,Length,Diameter,Xoffset,Yoffset,Comment,Image);
		PutToolID(i,Slot,ID,Length,Diameter,Xoffset,Yoffset,Comment,Image);
	}
}

void CEditToolFile::GetToolsFromScreen(int page)
{
	CString Comment,Image;
	int ID,Slot;
	double Diameter,Length,Xoffset,Yoffset;

	for (int i=0; i<20; i++)
	{
		GetToolID(i,Slot,ID,Length,Diameter,Xoffset,Yoffset,Comment,Image);
		PutTool(i+page*20,Slot,ID,Length,Diameter,Xoffset,Yoffset,Comment,Image);
	}
}

void CEditToolFile::PutTool(int Tool, int Slot, int ID, double Length, double Diameter, double Xoffset, double Yoffset, CString Comment, CString Image)
{
	if (Tool>=0 && Tool<MAX_TOOLS)
	{
		m_Slot[Tool]=Slot; m_ID[Tool]=ID; m_Length[Tool]=Length; m_Diameter[Tool]=Diameter; m_Xoffset[Tool]=Xoffset; m_Yoffset[Tool]=Yoffset; m_Comment[Tool]=Comment; m_Image[Tool]=Image; 
	}
}

void CEditToolFile::GetTool(int Tool, int &Slot, int &ID, double &Length, double &Diameter, double &Xoffset, double &Yoffset, CString &Comment, CString &Image)
{
	if (Tool>=0 && Tool<MAX_TOOLS)
	{
		Slot=m_Slot[Tool]; ID=m_ID[Tool]; Length=m_Length[Tool]; Diameter=m_Diameter[Tool]; Xoffset=m_Xoffset[Tool]; Yoffset=m_Yoffset[Tool]; Comment=m_Comment[Tool]; Image=m_Image[Tool]; 
	}
	else
	{
		Slot=0; ID=0; Length=0; Diameter=0; Xoffset=0; Yoffset=0; Comment=""; Image=""; 
	}
}

void CEditToolFile::PutToolID(int Tool, int Slot, int ID, double Length, double Diameter, double Xoffset, double Yoffset, CString Comment, CString Image)
{
	switch (Tool)
	{
		case 0: m_Slot0=Slot; m_ID0=ID; m_Length0=Length; m_Diameter0=Diameter; m_Xoffset0=Xoffset;m_Yoffset0=Yoffset;m_Comment0=Comment;m_Image0=Image; break; 
		case 1: m_Slot1=Slot; m_ID1=ID; m_Length1=Length; m_Diameter1=Diameter; m_Xoffset1=Xoffset;m_Yoffset1=Yoffset;m_Comment1=Comment;m_Image1=Image; break; 
		case 2: m_Slot2=Slot; m_ID2=ID; m_Length2=Length; m_Diameter2=Diameter; m_Xoffset2=Xoffset;m_Yoffset2=Yoffset;m_Comment2=Comment;m_Image2=Image; break; 
		case 3: m_Slot3=Slot; m_ID3=ID; m_Length3=Length; m_Diameter3=Diameter; m_Xoffset3=Xoffset;m_Yoffset3=Yoffset;m_Comment3=Comment;m_Image3=Image; break; 
		case 4: m_Slot4=Slot; m_ID4=ID; m_Length4=Length; m_Diameter4=Diameter; m_Xoffset4=Xoffset;m_Yoffset4=Yoffset;m_Comment4=Comment;m_Image4=Image; break; 
		case 5: m_Slot5=Slot; m_ID5=ID; m_Length5=Length; m_Diameter5=Diameter; m_Xoffset5=Xoffset;m_Yoffset5=Yoffset;m_Comment5=Comment;m_Image5=Image; break; 
		case 6: m_Slot6=Slot; m_ID6=ID; m_Length6=Length; m_Diameter6=Diameter; m_Xoffset6=Xoffset;m_Yoffset6=Yoffset;m_Comment6=Comment;m_Image6=Image; break; 
		case 7: m_Slot7=Slot; m_ID7=ID; m_Length7=Length; m_Diameter7=Diameter; m_Xoffset7=Xoffset;m_Yoffset7=Yoffset;m_Comment7=Comment;m_Image7=Image; break; 
		case 8: m_Slot8=Slot; m_ID8=ID; m_Length8=Length; m_Diameter8=Diameter; m_Xoffset8=Xoffset;m_Yoffset8=Yoffset;m_Comment8=Comment;m_Image8=Image; break; 
		case 9: m_Slot9=Slot; m_ID9=ID; m_Length9=Length; m_Diameter9=Diameter; m_Xoffset9=Xoffset;m_Yoffset9=Yoffset;m_Comment9=Comment;m_Image9=Image; break; 
		case 10: m_Slot10=Slot; m_ID10=ID; m_Length10=Length; m_Diameter10=Diameter; m_Xoffset10=Xoffset;m_Yoffset10=Yoffset;m_Comment10=Comment;m_Image10=Image; break; 
		case 11: m_Slot11=Slot; m_ID11=ID; m_Length11=Length; m_Diameter11=Diameter; m_Xoffset11=Xoffset;m_Yoffset11=Yoffset;m_Comment11=Comment;m_Image11=Image; break; 
		case 12: m_Slot12=Slot; m_ID12=ID; m_Length12=Length; m_Diameter12=Diameter; m_Xoffset12=Xoffset;m_Yoffset12=Yoffset;m_Comment12=Comment;m_Image12=Image; break; 
		case 13: m_Slot13=Slot; m_ID13=ID; m_Length13=Length; m_Diameter13=Diameter; m_Xoffset13=Xoffset;m_Yoffset13=Yoffset;m_Comment13=Comment;m_Image13=Image; break; 
		case 14: m_Slot14=Slot; m_ID14=ID; m_Length14=Length; m_Diameter14=Diameter; m_Xoffset14=Xoffset;m_Yoffset14=Yoffset;m_Comment14=Comment;m_Image14=Image; break; 
		case 15: m_Slot15=Slot; m_ID15=ID; m_Length15=Length; m_Diameter15=Diameter; m_Xoffset15=Xoffset;m_Yoffset15=Yoffset;m_Comment15=Comment;m_Image15=Image; break; 
		case 16: m_Slot16=Slot; m_ID16=ID; m_Length16=Length; m_Diameter16=Diameter; m_Xoffset16=Xoffset;m_Yoffset16=Yoffset;m_Comment16=Comment;m_Image16=Image; break; 
		case 17: m_Slot17=Slot; m_ID17=ID; m_Length17=Length; m_Diameter17=Diameter; m_Xoffset17=Xoffset;m_Yoffset17=Yoffset;m_Comment17=Comment;m_Image17=Image; break; 
		case 18: m_Slot18=Slot; m_ID18=ID; m_Length18=Length; m_Diameter18=Diameter; m_Xoffset18=Xoffset;m_Yoffset18=Yoffset;m_Comment18=Comment;m_Image18=Image; break; 
		case 19: m_Slot19=Slot; m_ID19=ID; m_Length19=Length; m_Diameter19=Diameter; m_Xoffset19=Xoffset;m_Yoffset19=Yoffset;m_Comment19=Comment;m_Image19=Image; break; 
	}
}

void CEditToolFile::GetToolID(int Tool, int &Slot, int &ID, double &Length, double &Diameter, double &Xoffset, double &Yoffset, CString &Comment, CString &Image)
{
	switch (Tool)
	{
		case 0: Slot=m_Slot0; ID=m_ID0; Length=m_Length0; Diameter=m_Diameter0; Xoffset=m_Xoffset0; Yoffset=m_Yoffset0; Comment=m_Comment0; Image=m_Image0; break; 
		case 1: Slot=m_Slot1; ID=m_ID1; Length=m_Length1; Diameter=m_Diameter1; Xoffset=m_Xoffset1; Yoffset=m_Yoffset1; Comment=m_Comment1; Image=m_Image1; break; 
		case 2: Slot=m_Slot2; ID=m_ID2; Length=m_Length2; Diameter=m_Diameter2; Xoffset=m_Xoffset2; Yoffset=m_Yoffset2; Comment=m_Comment2; Image=m_Image2; break; 
		case 3: Slot=m_Slot3; ID=m_ID3; Length=m_Length3; Diameter=m_Diameter3; Xoffset=m_Xoffset3; Yoffset=m_Yoffset3; Comment=m_Comment3; Image=m_Image3; break; 
		case 4: Slot=m_Slot4; ID=m_ID4; Length=m_Length4; Diameter=m_Diameter4; Xoffset=m_Xoffset4; Yoffset=m_Yoffset4; Comment=m_Comment4; Image=m_Image4; break; 
		case 5: Slot=m_Slot5; ID=m_ID5; Length=m_Length5; Diameter=m_Diameter5; Xoffset=m_Xoffset5; Yoffset=m_Yoffset5; Comment=m_Comment5; Image=m_Image5; break; 
		case 6: Slot=m_Slot6; ID=m_ID6; Length=m_Length6; Diameter=m_Diameter6; Xoffset=m_Xoffset6; Yoffset=m_Yoffset6; Comment=m_Comment6; Image=m_Image6; break; 
		case 7: Slot=m_Slot7; ID=m_ID7; Length=m_Length7; Diameter=m_Diameter7; Xoffset=m_Xoffset7; Yoffset=m_Yoffset7; Comment=m_Comment7; Image=m_Image7; break; 
		case 8: Slot=m_Slot8; ID=m_ID8; Length=m_Length8; Diameter=m_Diameter8; Xoffset=m_Xoffset8; Yoffset=m_Yoffset8; Comment=m_Comment8; Image=m_Image8; break; 
		case 9: Slot=m_Slot9; ID=m_ID9; Length=m_Length9; Diameter=m_Diameter9; Xoffset=m_Xoffset9; Yoffset=m_Yoffset9; Comment=m_Comment9; Image=m_Image9; break; 
		case 10: Slot=m_Slot10; ID=m_ID10; Length=m_Length10; Diameter=m_Diameter10; Xoffset=m_Xoffset10; Yoffset=m_Yoffset10; Comment=m_Comment10; Image=m_Image10; break; 
		case 11: Slot=m_Slot11; ID=m_ID11; Length=m_Length11; Diameter=m_Diameter11; Xoffset=m_Xoffset11; Yoffset=m_Yoffset11; Comment=m_Comment11; Image=m_Image11; break; 
		case 12: Slot=m_Slot12; ID=m_ID12; Length=m_Length12; Diameter=m_Diameter12; Xoffset=m_Xoffset12; Yoffset=m_Yoffset12; Comment=m_Comment12; Image=m_Image12; break; 
		case 13: Slot=m_Slot13; ID=m_ID13; Length=m_Length13; Diameter=m_Diameter13; Xoffset=m_Xoffset13; Yoffset=m_Yoffset13; Comment=m_Comment13; Image=m_Image13; break; 
		case 14: Slot=m_Slot14; ID=m_ID14; Length=m_Length14; Diameter=m_Diameter14; Xoffset=m_Xoffset14; Yoffset=m_Yoffset14; Comment=m_Comment14; Image=m_Image14; break; 
		case 15: Slot=m_Slot15; ID=m_ID15; Length=m_Length15; Diameter=m_Diameter15; Xoffset=m_Xoffset15; Yoffset=m_Yoffset15; Comment=m_Comment15; Image=m_Image15; break; 
		case 16: Slot=m_Slot16; ID=m_ID16; Length=m_Length16; Diameter=m_Diameter16; Xoffset=m_Xoffset16; Yoffset=m_Yoffset16; Comment=m_Comment16; Image=m_Image16; break; 
		case 17: Slot=m_Slot17; ID=m_ID17; Length=m_Length17; Diameter=m_Diameter17; Xoffset=m_Xoffset17; Yoffset=m_Yoffset17; Comment=m_Comment17; Image=m_Image17; break; 
		case 18: Slot=m_Slot18; ID=m_ID18; Length=m_Length18; Diameter=m_Diameter18; Xoffset=m_Xoffset18; Yoffset=m_Yoffset18; Comment=m_Comment18; Image=m_Image18; break; 
		case 19: Slot=m_Slot19; ID=m_ID19; Length=m_Length19; Diameter=m_Diameter19; Xoffset=m_Xoffset19; Yoffset=m_Yoffset19; Comment=m_Comment19; Image=m_Image19; break; 
	}
}


void CEditToolFile::SortToolTable()
{
	CString Comment,Comment2,Image,Image2;
	int ID,ID2,Slot,Slot2;
	double Diameter,Diameter2,Length,Length2,Xoffset,Xoffset2,Yoffset,Yoffset2;
	bool something_swapped;
	// bubble sort the tool table using Slot number as key
	do
	{
		something_swapped=false;
		for (int i=0; i<MAX_TOOLS-1; i++)
		{
			GetTool(i,Slot,ID,Length,Diameter,Xoffset,Yoffset,Comment,Image);
			GetTool(i+1,Slot2,ID2,Length2,Diameter2,Xoffset2,Yoffset2,Comment2,Image2);
			// sort first by Slot number then by ID also blanks to end
			if ((ID==0 && ID2!=0) ||
				(ID==0 && Slot==0 && (ID2!=0 || Slot2!=0)) || 
				(ID2!=0 && ID !=0 && ID2 < ID) || 
				(ID2==ID && Slot2!=0 && Slot!=0 && Slot2<Slot))
			{
				//swap
				PutTool(i,Slot2,ID2,Length2,Diameter2,Xoffset2,Yoffset2,Comment2,Image2);
				PutTool(i+1,Slot,ID,Length,Diameter,Xoffset,Yoffset,Comment,Image);
				something_swapped=true;
			}
		}
	}
	while (something_swapped);

	for (m_nTools=0; m_nTools<MAX_TOOLS; m_nTools++)
	{
		GetTool(m_nTools,Slot,ID,Length,Diameter,Xoffset,Yoffset,Comment,Image);
		if (!Slot && !ID) break;
	}
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

	fprintf(f,"SLOT    ID        LENGTH         DIAMETER        XOFFSET        YOFFSET   COMMENT     IMAGE\n");
	fprintf(f,"\n");

	CString Comment,Image;
	int ID, Slot;
	double Diameter,Length,Xoffset,Yoffset;

	for (int i=0; i<MAX_TOOLS; i++)
	{
		GetTool(i,Slot,ID,Length,Diameter,Xoffset,Yoffset,Comment,Image);
		if (Slot || ID) fprintf(f,"%3d %6d %15.6f %15.6f %15.6f %15.6f \"%s\" \"%s\"\n",Slot,ID,Length,Diameter,Xoffset,Yoffset,Comment.GetBuffer(),Image.GetBuffer());
	}
	fclose(f);
	return 0;
}


void CEditToolFile::ChangeLength(int Tool, double NewLength)
{
	CString Comment,Image;
	int ID, Slot;
	double Diameter,Length,Xoffset,Yoffset;

	GetTool(Tool,Slot,ID,Length,Diameter,Xoffset,Yoffset,Comment,Image);
	PutTool(Tool,Slot,ID,NewLength,Diameter,Xoffset,Yoffset,Comment,Image);
}

void CEditToolFile::ChangeDiameter(int Tool, double NewDiameter)
{
	CString Comment,Image;
	int ID, Slot;
	double Diameter,Length,Xoffset,Yoffset;

	GetTool(Tool,Slot,ID,Length,Diameter,Xoffset,Yoffset,Comment,Image);
	PutTool(Tool,Slot,ID,Length,NewDiameter,Xoffset,Yoffset,Comment,Image);
}

void CEditToolFile::ChangeOffsetX(int Tool, double NewXoffset)
{
	CString Comment,Image;
	int ID, Slot;
	double Diameter,Length,Xoffset,Yoffset;

	GetTool(Tool,Slot,ID,Length,Diameter,Xoffset,Yoffset,Comment,Image);
	PutTool(Tool,Slot,ID,Length,Diameter,NewXoffset,Yoffset,Comment,Image);
}

void CEditToolFile::ChangeOffsetY(int Tool, double NewYoffset)
{
	CString Comment,Image;
	int ID, Slot;
	double Diameter,Length,Xoffset,Yoffset;

	GetTool(Tool,Slot,ID,Length,Diameter,Xoffset,Yoffset,Comment,Image);
	PutTool(Tool,Slot,ID,Length,Diameter,Xoffset,NewYoffset,Comment,Image);
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

void CEditToolFile::OnBnClickedImagedir0()
{
	DoImageDir(m_Image[0+m_CurPage*20]);
}

void CEditToolFile::OnBnClickedImagedir1()
{
	DoImageDir(m_Image[1+m_CurPage*20]);
}

void CEditToolFile::OnBnClickedImagedir2()
{
	DoImageDir(m_Image[2+m_CurPage*20]);
}

void CEditToolFile::OnBnClickedImagedir3()
{
	DoImageDir(m_Image[3+m_CurPage*20]);
}

void CEditToolFile::OnBnClickedImagedir4()
{
	DoImageDir(m_Image[4+m_CurPage*20]);
}

void CEditToolFile::OnBnClickedImagedir5()
{
	DoImageDir(m_Image[5+m_CurPage*20]);
}

void CEditToolFile::OnBnClickedImagedir6()
{
	DoImageDir(m_Image[6+m_CurPage*20]);
}

void CEditToolFile::OnBnClickedImagedir7()
{
	DoImageDir(m_Image[7+m_CurPage*20]);
}

void CEditToolFile::OnBnClickedImagedir8()
{
	DoImageDir(m_Image[8+m_CurPage*20]);
}

void CEditToolFile::OnBnClickedImagedir9()
{
	DoImageDir(m_Image[9+m_CurPage*20]);
}

void CEditToolFile::OnBnClickedImagedir10()
{
	DoImageDir(m_Image[10+m_CurPage*20]);
}

void CEditToolFile::OnBnClickedImagedir11()
{
	DoImageDir(m_Image[11+m_CurPage*20]);
}

void CEditToolFile::OnBnClickedImagedir12()
{
	DoImageDir(m_Image[12+m_CurPage*20]);
}

void CEditToolFile::OnBnClickedImagedir13()
{
	DoImageDir(m_Image[13+m_CurPage*20]);
}

void CEditToolFile::OnBnClickedImagedir14()
{
	DoImageDir(m_Image[14+m_CurPage*20]);
}

void CEditToolFile::OnBnClickedImagedir15()
{
	DoImageDir(m_Image[15+m_CurPage*20]);
}

void CEditToolFile::OnBnClickedImagedir16()
{
	DoImageDir(m_Image[16+m_CurPage*20]);
}

void CEditToolFile::OnBnClickedImagedir17()
{
	DoImageDir(m_Image[17+m_CurPage*20]);
}

void CEditToolFile::OnBnClickedImagedir18()
{
	DoImageDir(m_Image[18+m_CurPage*20]);
}

void CEditToolFile::OnBnClickedImagedir19()
{
	DoImageDir(m_Image[19+m_CurPage*20]);
}




void CEditToolFile::DoImageDir(CString &Image)
{
	CString s, FullPath,ImagePath=TheFrame->MainPathRoot + TOOL_IMAGE_SUB_DIR;
	
	if (!UpdateData(TRUE)) return;

	CDoPreview preview;
	
	if (Image=="") 
		FullPath="";
	else 
	{
		if (Image.Find(':') == -1 && Image.Find("\\\\") == -1)
			FullPath=ImagePath+Image;
		else
			FullPath=Image;
	}

	CPreviewFileDialog FileDlg(&preview,TRUE,".wrl",FullPath,OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST,
			"VRML Image Files (*.wrl)|*.wrl|All Files (*.*)|*.*||");

	FileDlg.m_ofn.lpstrInitialDir = ImagePath;
	
	if(FileDlg.DoModal() == IDOK)
	{
		Image = FileDlg.GetPathName();
		if (Image.Find(ImagePath) == 0)
		{
			Image.Delete(0, ImagePath.GetLength());
		}
	}

	UpdateData(FALSE);
}
