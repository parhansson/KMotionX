
// G910_SAMPLEDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcolorbutton.h"

// CG910_SAMPLEDlg dialog
class CG910_SAMPLEDlg : public CDialogEx
{
// Construction
public:
	CG910_SAMPLEDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_G910_SAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSetlighting();
	afx_msg void OnBnClickedButtonW();
	afx_msg void OnBnClickedButtonA();
	afx_msg void OnBnClickedButtonS();
	afx_msg void OnBnClickedButtonD();
	afx_msg void OnBnClickedButtonBitmap();
	afx_msg void OnBnClickedButtonBitmapImage();
	afx_msg void OnBnClickedButtonToggleFlash();
	afx_msg void OnBnClickedButtonTogglePulse();
	afx_msg void OnBnClickedButtonSaveLighting();
	afx_msg void OnBnClickedButtonRestoreLighting();
	afx_msg void OnBnClickedButtonClearLighting();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CScrollBar m_scrollBarRed;
	CScrollBar m_scrollBarGreen;
	CScrollBar m_scrollBarBlue;


private:
	int ChangeRange(int oldVal, int oldMin, int oldMax, int newMin, int newMax); 
	void CheckBounds();
public:
    afx_msg void OnBnClickedButtonSetKeyFromScan();
    afx_msg void OnBnClickedButtonSetKeyFromHid();
    afx_msg void OnBnClickedButtonSetKeyFromName();
    afx_msg void OnBnClickedButtonStopEffects();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    
private:
    int m_targetDevice;
    void GetEffectColorValues(int *red, int *green, int *blue, bool startPicker);
public:
    afx_msg void OnBnClickedCheck1();
    CButton m_perKeyCheckBox;
    CButton m_rgbCheckBox;
    CButton m_monochromeCheckBox;
    afx_msg void OnBnClickedGetVersion();
    afx_msg void OnBnClickedButtonInitialize();
    afx_msg void OnBnClickedButtonShutdown();
    afx_msg void OnBnClickedButtonFlashR();
    CMFCColorButton m_singleKeyStartColorPicker;
    CMFCColorButton m_singleKeyFinishColorPicker;
    afx_msg void OnBnClickedButtonPulse1();
    afx_msg void OnBnClickedButtonFkeys();
    CButton m_perKeyPulseLoop;
    afx_msg void OnBnClickedCheckExcludeNumpad();
    CButton m_disableNumpad;
    afx_msg void OnBnClickedButtonSetGkeys();
};
