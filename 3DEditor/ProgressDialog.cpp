// ProgressDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ProgressDialog.h"
#include "afxdialogex.h"

#include <string>
using namespace std;

#ifdef _PROGRESS_UI_SUPPORT
/*virtual*/ void CProgressDialog::Log(int/*enumLogEvent*/ enLogEvent, const char* szEvent) /*override*/
{
	string strEntry = CreateLogEntry(enLogEvent, szEvent);
	strEntry += "\r\n";

	int iLength = m_edtProgress.GetWindowTextLength();
	if (iLength > 2048)
	{
		m_edtProgress.SetSel(0, iLength);
		m_edtProgress.ReplaceSel(L"...\r\n");

		iLength = m_edtProgress.GetWindowTextLength();
	}

	m_edtProgress.SetSel(iLength, iLength);
	m_edtProgress.ReplaceSel(CA2W(strEntry.c_str()));
}

/*static*/ UINT CProgressDialog::ThreadProc(LPVOID pParam)
{
	auto pDialog = (CProgressDialog*)pParam;
	assert(pDialog != nullptr);

	pDialog->m_pTask->Run();

	::EnableWindow(pDialog->GetDlgItem(IDCANCEL)->GetSafeHwnd(), TRUE);

	return 0;
}

// CProgressDialog dialog

IMPLEMENT_DYNAMIC(CProgressDialog, CDialogEx)

CProgressDialog::CProgressDialog(CWnd* pParent, CTask* pTask)
	: CDialogEx(IDD_DIALOG_PROGRESS, pParent)
	, m_pThread(nullptr)
	, m_pTask(pTask)
{
	assert(m_pTask != nullptr);
}

CProgressDialog::~CProgressDialog()
{
	if (m_pThread != nullptr)
	{
		delete m_pThread;
	}
}

void CProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PROGRESS, m_edtProgress);
}

BEGIN_MESSAGE_MAP(CProgressDialog, CDialogEx)
END_MESSAGE_MAP()

// CProgressDialog message handlers

BOOL CProgressDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_pThread = ::AfxBeginThread(ThreadProc, this);
	m_pThread->m_bAutoDelete = FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
#endif // _PROGRESS_UI_SUPPORT
