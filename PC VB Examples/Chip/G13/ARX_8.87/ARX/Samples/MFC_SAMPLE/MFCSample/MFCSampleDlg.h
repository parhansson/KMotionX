
// MFCSampleDlg.h : header file
//

#pragma once

#pragma comment(lib, "LogitechGArxControlLib.lib")

#include "LogitechGArxControlLib.h"
#include "afxeditbrowsectrl.h"
#include "afxwin.h"

typedef struct arxAppCallbackMessage
{
	unsigned __int32 eventType;
	unsigned __int32 eventValue;
	wchar_t eventArg[128]; //Max length for this callback param (see documentation for more details)
}arxAppCallbackMessage;

#define WM_ARXAPP_SDK_CALLBACK WM_APP + 1


// CMFCSampleDlg dialog
class CMFCSampleDlg : public CDialogEx
{
// Construction
public:
	CMFCSampleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MFCSAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg LRESULT OnGetDefID(WPARAM wp, LPARAM lp);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonInit();
	afx_msg LRESULT OnCallbackReport(WPARAM wParam, LPARAM lParam);
private:
	static void OutputDebugStringAndErrorCode(wchar_t *string, bool outputMessageBox);
	static void __cdecl onCallback(int eventType, int eventValue, wchar_t * eventArg, void *context);
	std::vector<BYTE> ReadAllBytes(char const* filename);
	CEdit m_initFriendlyName;
	CEdit m_initUniqueId;
	CStatic m_statusLabel;
	CMFCEditBrowseCtrl m_fileFromPathControl;
	CEdit m_fileNameFfp;
	CListBox m_listFilesSent;
	bool updateByIdOrClass; //true: by ID, false: byClass
public:

	afx_msg void OnBnClickedButtonFileFromPath();

private:
	CEdit m_fileNameFfs;
	CEdit m_fileContentFfs;
public:
	afx_msg void OnBnClickedButtonFileFromString();
	afx_msg void OnBnClickedButtonFileFromContent();
	afx_msg void OnBnClickedButtonFileFromBitmap();
private:
	CEdit m_fileNameFfc;
	CEdit m_fileNameFfb;
public:
	afx_msg void OnBnClickedButtonSetIndex();
private:
	CStatic m_updateIdOrVariableLabel;
public:
	afx_msg void OnBnClickedRadioByid();
	afx_msg void OnBnClickedRadiobyclass();
	afx_msg void OnBnClickedButtonUpdateContent();
private:
	CEdit m_propertyToUpdate;
	CEdit m_propertyNewValue;
	CEdit m_newContentToUpdate;
	CEdit m_updateTag;
public:
	afx_msg void OnBnClickedButtonUpdateProperty();
private:
	int m_UpdateByIdCheckBox;
public:
	CButton m_usingCustomIcon;
	afx_msg void OnBnClickedButtonShutdown();
};
