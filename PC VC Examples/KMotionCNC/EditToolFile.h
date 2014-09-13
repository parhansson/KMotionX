#pragma once


// CEditToolFile dialog

class CEditToolFile : public CDialog
{
	DECLARE_DYNAMIC(CEditToolFile)

public:
	CEditToolFile(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditToolFile();

#define MAX_TOOLS 100

	// Dialog Data
	enum { IDD = IDD_EditToolFile };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void ChangeABlankZero(int ID);
	void ChangeBlanksToZeros(void);
	void ChangeZeroToBlank(int ID0, int ID1, int ID2, int ID3);
	void ChangeZerosToBlanks(void);
	void PutTool(int Tool, int Pocket, int ID, double Length, double Diameter, CString Comment);
	void GetTool(int Tool, int &Pocket, int &ID, double &Length, double &Diameter, CString &Comment);
	void PutToolID(int Tool, int Pocket, int ID, double Length, double Diameter, CString Comment);
	void GetToolID(int Tool, int &Pocket, int &ID, double &Length, double &Diameter, CString &Comment);
	void PutToolsToScreen(int page);
	void GetToolsFromScreen(int page);
	void SortToolTable();
	
	int m_Pocket[MAX_TOOLS];
	int m_ID[MAX_TOOLS];
	double m_Length[MAX_TOOLS];	
	double m_Diameter[MAX_TOOLS];
	CString m_Comment[MAX_TOOLS];

	DECLARE_MESSAGE_MAP()
public:
	void ChangeLength(int Tool, double Length);

	int m_Pocket0;
	int m_Pocket1;
	int m_Pocket2;
	int m_Pocket3;
	int m_Pocket4;
	int m_Pocket5;
	int m_Pocket6;
	int m_Pocket7;
	int m_Pocket8;
	int m_Pocket9;
	int m_Pocket10;
	int m_Pocket11;
	int m_Pocket12;
	int m_Pocket13;
	int m_Pocket14;
	int m_Pocket15;
	int m_Pocket16;
	int m_Pocket17;
	int m_Pocket18;
	int m_Pocket19;
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
};
