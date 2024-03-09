
// MFCSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCSample.h"
#include "MFCSampleDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define BUFFER_MAX 20000

// CMFCSampleDlg dialog



CMFCSampleDlg::CMFCSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCSampleDlg::IDD, pParent)
	, m_UpdateByIdCheckBox(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FRIENDLY_NAME, m_initFriendlyName);
	DDX_Control(pDX, IDC_EDIT_UNIQUE_ID, m_initUniqueId);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_statusLabel);
	DDX_Control(pDX, IDC_MFCEDITBROWSE1, m_fileFromPathControl);
	DDX_Control(pDX, IDC_EDIT_FILENAME_FFP, m_fileNameFfp);
	DDX_Control(pDX, IDC_LIST_SENT_FILES, m_listFilesSent);
	DDX_Control(pDX, IDC_EDIT_FILENAME_FFS, m_fileNameFfs);
	DDX_Control(pDX, IDC_EDIT_STRING_FFS, m_fileContentFfs);
	DDX_Control(pDX, IDC_EDIT_FN_FFC, m_fileNameFfc);
	DDX_Control(pDX, IDC_EDIT_FN_FFB, m_fileNameFfb);
	DDX_Control(pDX, IDC_STATIC_ID_OR_CLASS, m_updateIdOrVariableLabel);
	DDX_Control(pDX, IDC_EDIT_PROPERTY, m_propertyToUpdate);
	DDX_Control(pDX, IDC_EDIT_PROP_NEW_VAL, m_propertyNewValue);
	DDX_Control(pDX, IDC_EDIT_NEWCONTENT_UC, m_newContentToUpdate);
	DDX_Control(pDX, IDC_EDIT_TAG_UC, m_updateTag);
	DDX_Control(pDX, IDC_CHECK_CUSTOMICON, m_usingCustomIcon);
}

BEGIN_MESSAGE_MAP(CMFCSampleDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(DM_GETDEFID, OnGetDefID)
	ON_BN_CLICKED(IDC_BUTTON_INIT, &CMFCSampleDlg::OnBnClickedButtonInit)
	ON_MESSAGE(WM_ARXAPP_SDK_CALLBACK, OnCallbackReport)
	ON_BN_CLICKED(IDC_BUTTON_FILE_FROM_PATH, &CMFCSampleDlg::OnBnClickedButtonFileFromPath)
	ON_BN_CLICKED(IDC_BUTTON_FILE_FROM_STRING, &CMFCSampleDlg::OnBnClickedButtonFileFromString)
	ON_BN_CLICKED(IDC_BUTTON_FILE_FROM_CONTENT, &CMFCSampleDlg::OnBnClickedButtonFileFromContent)
	ON_BN_CLICKED(IDC_BUTTON_FILE_FROM_BITMAP, &CMFCSampleDlg::OnBnClickedButtonFileFromBitmap)
	ON_BN_CLICKED(IDC_BUTTON_SET_INDEX, &CMFCSampleDlg::OnBnClickedButtonSetIndex)
	ON_BN_CLICKED(IDC_RADIO_BYID, &CMFCSampleDlg::OnBnClickedRadioByid)
	ON_BN_CLICKED(IDC_RADIOBYCLASS, &CMFCSampleDlg::OnBnClickedRadiobyclass)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_CONTENT, &CMFCSampleDlg::OnBnClickedButtonUpdateContent)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_PROPERTY, &CMFCSampleDlg::OnBnClickedButtonUpdateProperty)
	ON_BN_CLICKED(IDC_BUTTON_SHUTDOWN, &CMFCSampleDlg::OnBnClickedButtonShutdown)
END_MESSAGE_MAP()


// CMFCSampleDlg message handlers

LRESULT CMFCSampleDlg::OnGetDefID(WPARAM wp, LPARAM lp) 
{
  return MAKELONG(0,DC_HASDEFID); 
}

BOOL CMFCSampleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	m_fileFromPathControl.EnableFileBrowseButton();
	m_initFriendlyName.SetWindowText(L"Sample Applet");
	m_initUniqueId.SetWindowText(L"arxsdk.sample.mfcsample");
	m_fileNameFfc.SetWindowText(L"g700.jpg");
	m_fileNameFfs.SetWindowText(L"alert.js");
	m_fileContentFfs.SetWindowText(L"alert('Welcome to the mfc test applet!');");
	m_fileNameFfb.SetWindowText(L"randomImage.png");
	m_fileNameFfp.SetWindowText(L"index.html");
	m_fileFromPathControl.SetWindowText(L"Resources/index.html");
	m_fileContentFfs.SetLimitText(BUFFER_MAX);
	m_propertyNewValue.SetWindowText(L"50%");
	m_propertyToUpdate.SetWindowText(L"style.width");
	m_updateTag.SetWindowText(L"progressbarProgress");
	updateByIdOrClass = true;
	CheckRadioButton(IDC_RADIO_BYID, IDC_RADIOBYCLASS, IDC_RADIO_BYID);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCSampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCSampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CMFCSampleDlg::OnCallbackReport(WPARAM wParam, LPARAM lParam)
{
	arxAppCallbackMessage *callbackMessageStruct = reinterpret_cast<arxAppCallbackMessage*> (wParam);
	OutputDebugString(L"Received callback on main thread for ");
	switch(callbackMessageStruct->eventType)
	{

	case  LOGI_ARX_EVENT_FOCUS_INACTIVE:
		{
			OutputDebugString(L"applet focus inactive \n");
			m_statusLabel.SetWindowText(L"Inactive");
		}
		break;

	case  LOGI_ARX_EVENT_FOCUS_ACTIVE:
		{
			if(callbackMessageStruct->eventValue == LOGI_ARX_ORIENTATION_PORTRAIT)
			{
				OutputDebugString(L"applet focus active in portrait mode\n");
				m_statusLabel.SetWindowText(L"Active/Portrait");
			}
			if(callbackMessageStruct->eventValue == LOGI_ARX_ORIENTATION_LANDSCAPE)
			{
				OutputDebugString(L"applet focus active in landscape mode\n");
				m_statusLabel.SetWindowText(L"Active/Landscape");

			}
			break;
		}

	case  LOGI_ARX_EVENT_MOBILEDEVICE_ARRIVAL:
		{
			OutputDebugString(L"mobile device arrival\n");
			m_statusLabel.SetWindowText(L"Device Present");
			break;
		}

	case  LOGI_ARX_EVENT_MOBILEDEVICE_REMOVAL:
		{
			OutputDebugString(L"mobile device removal\n");
			m_statusLabel.SetWindowText(L"No Device present");
			break;
		}
	case  LOGI_ARX_EVENT_TAP_ON_TAG:
		{
			OutputDebugString(L"click on tag with id : ");
			if(callbackMessageStruct->eventArg)
			{
				wchar_t str[MAX_PATH];
				wsprintf(str, L"User Tapped on tag with id :%ls",callbackMessageStruct->eventArg);
				::MessageBox(m_hWnd, str, L"User action", MB_OK);
				OutputDebugString(callbackMessageStruct->eventArg);
				OutputDebugString(L"\n");

			}
			else
			{
				OutputDebugString(L"NULL\n");
			}
			break;
		}

	default:
		printf("unknown message %d:%d", callbackMessageStruct->eventType, callbackMessageStruct->eventValue);
		break;
	}
	delete(callbackMessageStruct);
	return 0;
}
void CMFCSampleDlg::OnClose()
{

	LogiArxShutdown();
	CDialogEx::OnClose();
}

void CMFCSampleDlg::OutputDebugStringAndErrorCode(wchar_t *string, bool outputMessageBox)
{
	int lastError = LogiArxGetLastError();
	wchar_t debStr[128];
	wchar_t errorStr[128];
	switch(lastError)
	{
	case 0: 
		{
			wsprintf(errorStr,L"SUCCESS");
		}
		break;
	case 1:
		{
			wsprintf(errorStr,L"WRONG PARAMETER FORMAT");
		}
		break;
	case 2:
		{
			wsprintf(errorStr,L"NULL PARAMETER NOT SUPPORTED");
		}
		break;
	case 3:
		{
			wsprintf(errorStr,L"WRONG FILEPATH");
		}
		break;
	case 4:
		{
			wsprintf(errorStr,L"SDK NOT INITIALIZED");
		}
		break;
	case 5:
		{
			wsprintf(errorStr,L"SDK ALREADY INITIALIZED");
		}
		break;
	case 6:
		{
			wsprintf(errorStr,L"CONNECTION WITH LGS BROKEN");
		}
		break;
	case 7:
		{
			wsprintf(errorStr,L"COULDN'T CREATE WORKER THREAD");
		}
		break;
	case 8:
		{
			wsprintf(errorStr,L"MEMORY COPY FAIL");
		}
		break;
	default:
		{
			wsprintf(errorStr,L"UNKOWN ERROR");
		}
		break;
	}
	wsprintf(debStr, L"%ls with error : %d ( %ls )\n",string, lastError, errorStr);
	OutputDebugString(debStr);
	if(outputMessageBox)
		::MessageBox(NULL, debStr ,L"Error", MB_OK);
}

void CMFCSampleDlg::onCallback(int eventType, int eventValue, wchar_t * eventArg, void *context)
{
	HWND main_hwnd = (HWND)context;
	//Using PostMessage to notify main thread of callback event
	arxAppCallbackMessage *parameter = new arxAppCallbackMessage;
	parameter->eventType = eventType;
	parameter->eventValue = eventValue;
	wcsncpy_s(parameter->eventArg, eventArg, _TRUNCATE);
	::PostMessage(main_hwnd, WM_ARXAPP_SDK_CALLBACK, reinterpret_cast<WPARAM> (parameter) , static_cast<LPARAM>(0));
}
void CMFCSampleDlg::OnBnClickedButtonInit()
{
	logiArxCbContext callbackStruct;
	callbackStruct.arxContext = m_hWnd;
	callbackStruct.arxCallBack = (logiArxCb)onCallback;
	wchar_t initFN[MAX_PATH];
	wchar_t initUI[MAX_PATH];
    m_initFriendlyName.GetWindowText(initFN, MAX_PATH);
	m_initUniqueId.GetWindowText(initUI, MAX_PATH);
	if (!m_usingCustomIcon.GetCheck())
	{
		if (!LogiArxInit(initUI, initFN, &callbackStruct))
		{
			OutputDebugStringAndErrorCode(L"Could not initialize ARX Control SDK", true);
		}
		else
		{
			m_statusLabel.SetWindowText(L"Connected");
		}
	}
	else
	{
		//using a custom icon. Sending a random bitmap as icon.
		unsigned char iconTest[LOGI_CUSTOMICON_BYTEARRAY_SIZE];
		for (int i = 0; i < LOGI_CUSTOMICON_BYTEARRAY_SIZE; i++)
		{
			iconTest[i] = rand() % 255;
		}
		if (!LogiArxInitWithIcon(initUI, initFN, &callbackStruct, iconTest))
		{
			OutputDebugStringAndErrorCode(L"Could not initialize ARX Control SDK", true);
		}
		else
		{
			m_statusLabel.SetWindowText(L"Connected");
		}
	}
	
}

void CMFCSampleDlg::OnBnClickedButtonFileFromPath()
{
	wchar_t ffpFP[MAX_PATH];
	wchar_t ffpFN[MAX_PATH];
    m_fileFromPathControl.GetWindowText(ffpFP, MAX_PATH);
	m_fileNameFfp.GetWindowText(ffpFN, MAX_PATH);
	if(!LogiArxAddFileAs(ffpFP, ffpFN, L"text/html"))
	{
		OutputDebugStringAndErrorCode(L"Could not send file", true);
	}
	else
	{
		if(m_listFilesSent.FindString(0, ffpFN) == LB_ERR)
			m_listFilesSent.AddString(ffpFN);
	}
}


void CMFCSampleDlg::OnBnClickedButtonFileFromString()
{
	wchar_t ffsFC[BUFFER_MAX];
	wchar_t ffsFN[MAX_PATH];
	m_fileContentFfs.GetWindowText(ffsFC, MAX_PATH);
	m_fileNameFfs.GetWindowText(ffsFN, MAX_PATH);
	if(!LogiArxAddUTF8StringAs(ffsFC, ffsFN, L"text/javascript"))
	{
		OutputDebugStringAndErrorCode(L"Could not send file", true);
	}
	else
	{
		if(m_listFilesSent.FindString(0, ffsFN)  == LB_ERR)
			m_listFilesSent.AddString(ffsFN);
	}
}

std::vector<BYTE> CMFCSampleDlg::ReadAllBytes(char const* filename)
{

	std::ifstream ifs(filename, std::ios::binary|std::ios::ate);
	if(!ifs.good())
	{
		std::vector<BYTE> emptyVector;
		return emptyVector;
	}
	std::ifstream::pos_type pos = ifs.tellg();

	std::vector<BYTE>  result(pos);
	
	ifs.seekg(0, std::ios::beg);
	ifs.read((char *)&result[0], pos);

	return result;
}


void CMFCSampleDlg::OnBnClickedButtonFileFromContent()
{
	//Use this function if you have an asset in memory. In this example we just load in memory an existing file
	wchar_t ffsFC[MAX_PATH];
	m_fileNameFfc.GetWindowText(ffsFC, MAX_PATH);
	
	std::vector<BYTE> contentVector = ReadAllBytes("Resources//g700.jpg");
	if(contentVector.size() > 0)
	{
		BYTE *content = &contentVector[0];
		if(!LogiArxAddContentAs(content, contentVector.size(),  ffsFC, L"image/jpeg"))
		{
			OutputDebugStringAndErrorCode(L"Could not send file", true);
		}
		else
		{
			if(m_listFilesSent.FindString(0, ffsFC) == LB_ERR)
				m_listFilesSent.AddString(ffsFC);
		}
	}

	
	

}

void CMFCSampleDlg::OnBnClickedButtonFileFromBitmap()
{
	wchar_t ffbFC[MAX_PATH];
	m_fileNameFfb.GetWindowText(ffbFC, MAX_PATH);
	//creating a random image 300x300
	BYTE randomImage[300*300*4];
	for(int i=0;i<300*300*4;i++)
	{
		randomImage[i] = rand() % 255;
	}
	if(!LogiArxAddImageFromBitmap(randomImage, 100, 100, ffbFC))
	{
		OutputDebugStringAndErrorCode(L"Could not send file", true);
	}
	else
	{
		if(m_listFilesSent.FindString(0, ffbFC) == LB_ERR)
			m_listFilesSent.AddString(ffbFC);
	}
}


void CMFCSampleDlg::OnBnClickedButtonSetIndex()
{
	int itemSelected = m_listFilesSent.GetCurSel();
	wchar_t setIndexFile[MAX_PATH];
	m_listFilesSent.GetText(itemSelected, setIndexFile);
	if(!LogiArxSetIndex(setIndexFile))
	{
		OutputDebugStringAndErrorCode(L"Could not set index", true);
	}
}


void CMFCSampleDlg::OnBnClickedRadioByid()
{
	m_updateIdOrVariableLabel.SetWindowText(L"Tag Id");
	updateByIdOrClass = true;
}


void CMFCSampleDlg::OnBnClickedRadiobyclass()
{
	m_updateIdOrVariableLabel.SetWindowText(L"Tag Class");
	updateByIdOrClass = false;
}


void CMFCSampleDlg::OnBnClickedButtonUpdateContent()
{
	wchar_t utcTAG[MAX_PATH];
	wchar_t utcNC[BUFFER_MAX];
	m_newContentToUpdate.GetWindowText(utcNC, MAX_PATH);
	m_updateTag.GetWindowText(utcTAG, MAX_PATH);
	if(updateByIdOrClass)
	{
		if(!LogiArxSetTagContentById(utcTAG, utcNC))
		{
			OutputDebugStringAndErrorCode(L"Could not update content", true);
		}
	}
	else
	{
		if(!LogiArxSetTagsContentByClass(utcTAG, utcNC))
		{
			OutputDebugStringAndErrorCode(L"Could not update content", true);
		}
	}
	
}


void CMFCSampleDlg::OnBnClickedButtonUpdateProperty()
{
	wchar_t utcTAG[MAX_PATH];
	wchar_t utcPU[MAX_PATH];
	wchar_t utcNP[MAX_PATH];
	m_updateTag.GetWindowText(utcTAG, MAX_PATH);
	m_propertyToUpdate.GetWindowText(utcPU, MAX_PATH);
	m_propertyNewValue.GetWindowText(utcNP, MAX_PATH);
	if(updateByIdOrClass)
	{
		if(!LogiArxSetTagPropertyById(utcTAG, utcPU, utcNP))
		{
			OutputDebugStringAndErrorCode(L"Could not update property", true);
		}
	}
	else
	{
		if(!LogiArxSetTagsPropertyByClass(utcTAG, utcPU, utcNP))
		{
			OutputDebugStringAndErrorCode(L"Could not update property", true);
		}
	}
}


void CMFCSampleDlg::OnBnClickedButtonShutdown()
{
	LogiArxShutdown();
}
