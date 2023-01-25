
// 3DEditor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "3DEditor.h"
#include "MainFrm.h"

#include "3DEditorDoc.h"
#include "3DEditorView.h"

#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ------------------------------------------------------------------------------------------------
extern BOOL TEST_MODE;


// CMy3DEditorApp

BEGIN_MESSAGE_MAP(CMy3DEditorApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CMy3DEditorApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CMy3DEditorApp construction

CMy3DEditorApp::CMy3DEditorApp()
{
	m_bHiColorIcons = TRUE;

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("3DEditor.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	std::wcout.imbue(std::locale::global(std::locale("")));
}

// The one and only CMy3DEditorApp object

CMy3DEditorApp theApp;


// CMy3DEditorApp initialization

BOOL CMy3DEditorApp::InitInstance()
{
//	LOG_INIT("3DEditor_%N.log");

//	LOG_INFO("3DEditor is starting...");

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("3DEditor, RDF LTD"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMy3DEditorDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CMy3DEditorView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	/*
	* Generate tests mode
	*/
	CString strInputDataDir;
	CString strWildcard;
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
	{
		CString stCommandLine = GetCommandLine();

		int iTokenPos = 0;
		CString strToken = stCommandLine.Tokenize(_T("#"), iTokenPos);

		int iArgument = -1;
		while (!strToken.IsEmpty())
		{
			iArgument++;

			if ((iArgument == 1) && (strToken != _T("GENERATE_TESTS")))			
			{
				break;
			}

			if (iArgument == 2)
			{
				strInputDataDir = strToken;
			}
			else if (iArgument == 3)
			{
				strWildcard = strToken;
			}
			else if (iArgument > 3)
			{
				ASSERT(FALSE); // unsupported!
			}

			strToken = stCommandLine.Tokenize(_T("#"), iTokenPos);
		} // while (!strToken.IsEmpty())
	} // if (cmdInfo.m_nShellCommand == ...

	if (!strInputDataDir.IsEmpty() && !strWildcard.IsEmpty())
	{
		TEST_MODE = TRUE;

		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;
	}	

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();

	if (TEST_MODE)
	{
		((CMainFrame*)m_pMainWnd)->GenerateTests(strInputDataDir, strWildcard);

		// PATCH: see PATCH: AMD 6700 XT - Access violation
		//TEST_MODE = FALSE;

		return FALSE;
	}	

	return TRUE;
}

int CMy3DEditorApp::ExitInstance()
{
//	LOG_INFO("3DEditor is terminating...");

	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CMy3DEditorApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CMy3DEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CMy3DEditorApp customization load/save methods

void CMy3DEditorApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);

	GetContextMenuManager()->AddMenu(L"Instances", IDR_POPUP_INSTANCES);	
}

void CMy3DEditorApp::LoadCustomState()
{
}

void CMy3DEditorApp::SaveCustomState()
{
}

// CMy3DEditorApp message handlers





void CMy3DEditorApp::AddToRecentFileList(LPCTSTR lpszPathName)
{
	// TODO: Add your specialized code here and/or call the base class

	CWinAppEx::AddToRecentFileList(lpszPathName);
}
