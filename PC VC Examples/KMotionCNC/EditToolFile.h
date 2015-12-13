#pragma once


// CEditToolFile dialog

class CEditToolFile : public CDialog
{
	DECLARE_DYNAMIC(CEditToolFile)

public:
	CEditToolFile(CWnd* pParent = NULL);   // standard constructor
	void GetTool(int Tool, int &Slot, int &ID, double &Length, double &Diameter, double &Xoffset, double &Yoffset, CString &Comment, CString &Image);
	virtual ~CEditToolFile();

#define MAX_TOOLS 100

	// Dialog Data
	enum { IDD = IDD_EditToolFile };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void PutTool(int Tool, int Slot, int ID, double Length, double Diameter, double Xoffset, double Yoffset, CString Comment, CString Image);
	void PutToolID(int Tool, int Slot, int ID, double Length, double Diameter, double Xoffset, double Yoffset, CString Comment, CString Image);
	void GetToolID(int Tool, int &Slot, int &ID, double &Length, double &Diameter, double &Xoffset, double &Yoffset, CString &Comment, CString &Image);
	void PutToolsToScreen(int page);
	void GetToolsFromScreen(int page);
	void SortToolTable();
	void DoImageDir(CString &Image);

	void DDX_Text_Blank(CDataExchange* pDX, int nIDC, int& value);
	void DDX_Text_Blank(CDataExchange* pDX, int nIDC, double& value);
	void DDV_MinMaxInt_Blank(CDataExchange* pDX, int value, int minVal, int maxVal);
	void DDV_MinMaxDouble_Blank(CDataExchange* pDX, double const& value, double minVal, double maxVal);
	
	int m_Slot[MAX_TOOLS];
	int m_ID[MAX_TOOLS];
	double m_Length[MAX_TOOLS];	
	double m_Diameter[MAX_TOOLS];
	double m_Xoffset[MAX_TOOLS];
	double m_Yoffset[MAX_TOOLS];
	CString m_Comment[MAX_TOOLS];
	CString m_Image[MAX_TOOLS];
	CString CurrentDirectory;

	int m_prevID;


	DECLARE_MESSAGE_MAP()
public:
	void ChangeLength(int Tool, double Length);
	void ChangeDiameter(int Tool, double Diameter);
	void ChangeOffsetX(int Tool, double OffsetX);
	void ChangeOffsetY(int Tool, double OffsetY);

	int m_Slot0;
	int m_Slot1;
	int m_Slot2;
	int m_Slot3;
	int m_Slot4;
	int m_Slot5;
	int m_Slot6;
	int m_Slot7;
	int m_Slot8;
	int m_Slot9;
	int m_Slot10;
	int m_Slot11;
	int m_Slot12;
	int m_Slot13;
	int m_Slot14;
	int m_Slot15;
	int m_Slot16;
	int m_Slot17;
	int m_Slot18;
	int m_Slot19;
	int m_ID0;
	int m_ID1;
	int m_ID2;
	int m_ID3;
	int m_ID4;
	int m_ID5;
	int m_ID6;
	int m_ID7;
	int m_ID8;
	int m_ID9;
	int m_ID10;
	int m_ID11;
	int m_ID12;
	int m_ID13;
	int m_ID14;
	int m_ID15;
	int m_ID16;
	int m_ID17;
	int m_ID18;
	int m_ID19;
	double m_Length0;
	double m_Length1;
	double m_Length2;
	double m_Length3;
	double m_Length4;
	double m_Length5;
	double m_Length6;
	double m_Length7;
	double m_Length8;
	double m_Length9;
	double m_Length10;
	double m_Length11;
	double m_Length12;
	double m_Length13;
	double m_Length14;
	double m_Length15;
	double m_Length16;
	double m_Length17;
	double m_Length18;
	double m_Length19;
	double m_Diameter0;
	double m_Diameter1;
	double m_Diameter2;
	double m_Diameter3;
	double m_Diameter4;
	double m_Diameter5;
	double m_Diameter6;
	double m_Diameter7;
	double m_Diameter8;
	double m_Diameter9;
	double m_Diameter10;
	double m_Diameter11;
	double m_Diameter12;
	double m_Diameter13;
	double m_Diameter14;
	double m_Diameter15;
	double m_Diameter16;
	double m_Diameter17;
	double m_Diameter18;
	double m_Diameter19;
	double m_Xoffset0;
	double m_Xoffset1;
	double m_Xoffset2;
	double m_Xoffset3;
	double m_Xoffset4;
	double m_Xoffset5;
	double m_Xoffset6;
	double m_Xoffset7;
	double m_Xoffset8;
	double m_Xoffset9;
	double m_Xoffset10;
	double m_Xoffset11;
	double m_Xoffset12;
	double m_Xoffset13;
	double m_Xoffset14;
	double m_Xoffset15;
	double m_Xoffset16;
	double m_Xoffset17;
	double m_Xoffset18;
	double m_Xoffset19;
	double m_Yoffset0;
	double m_Yoffset1;
	double m_Yoffset2;
	double m_Yoffset3;
	double m_Yoffset4;
	double m_Yoffset5;
	double m_Yoffset6;
	double m_Yoffset7;
	double m_Yoffset8;
	double m_Yoffset9;
	double m_Yoffset10;
	double m_Yoffset11;
	double m_Yoffset12;
	double m_Yoffset13;
	double m_Yoffset14;
	double m_Yoffset15;
	double m_Yoffset16;
	double m_Yoffset17;
	double m_Yoffset18;
	double m_Yoffset19;
	CString m_Comment0;
	CString m_Comment1;
	CString m_Comment2;
	CString m_Comment3;
	CString m_Comment4;
	CString m_Comment5;
	CString m_Comment6;
	CString m_Comment7;
	CString m_Comment8;
	CString m_Comment9;
	CString m_Comment10;
	CString m_Comment11;
	CString m_Comment12;
	CString m_Comment13;
	CString m_Comment14;
	CString m_Comment15;
	CString m_Comment16;
	CString m_Comment17;
	CString m_Comment18;
	CString m_Comment19;
	CString m_Image0;
	CString m_Image1;
	CString m_Image2;
	CString m_Image3;
	CString m_Image4;
	CString m_Image5;
	CString m_Image6;
	CString m_Image7;
	CString m_Image8;
	CString m_Image9;
	CString m_Image10;
	CString m_Image11;
	CString m_Image12;
	CString m_Image13;
	CString m_Image14;
	CString m_Image15;
	CString m_Image16;
	CString m_Image17;
	CString m_Image18;
	CString m_Image19;
	int LoadFile(CString File);
	int SaveFile(CString File);

	int m_nTools;
	afx_msg void OnBnClickedPage1();
	afx_msg void OnBnClickedPage2();
	afx_msg void OnBnClickedPage3();
	afx_msg void OnBnClickedPage4();
	afx_msg void OnBnClickedPage5();
	int m_CurPage;
	afx_msg void OnBnClickedSort();
	afx_msg void OnBnClickedImagedir0();
	afx_msg void OnBnClickedImagedir1();
	afx_msg void OnBnClickedImagedir2();
	afx_msg void OnBnClickedImagedir3();
	afx_msg void OnBnClickedImagedir4();
	afx_msg void OnBnClickedImagedir5();
	afx_msg void OnBnClickedImagedir6();
	afx_msg void OnBnClickedImagedir7();
	afx_msg void OnBnClickedImagedir8();
	afx_msg void OnBnClickedImagedir9();
	afx_msg void OnBnClickedImagedir10();
	afx_msg void OnBnClickedImagedir11();
	afx_msg void OnBnClickedImagedir12();
	afx_msg void OnBnClickedImagedir13();
	afx_msg void OnBnClickedImagedir14();
	afx_msg void OnBnClickedImagedir15();
	afx_msg void OnBnClickedImagedir16();
	afx_msg void OnBnClickedImagedir17();
	afx_msg void OnBnClickedImagedir18();
	afx_msg void OnBnClickedImagedir19();
};
