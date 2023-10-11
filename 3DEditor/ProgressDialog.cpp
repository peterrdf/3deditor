// ProgressDialog.cpp : implementation file
//

#include "stdafx.h"
#include "3DEditor.h"
#include "ProgressDialog.h"
#include "afxdialogex.h"

#include <string>
using namespace std;

/*static*/ UINT CProgressDialog::ThreadProc(LPVOID pParam)
{
	auto pDialog = (CProgressDialog*)pParam;
	ASSERT(pDialog != nullptr);

	pDialog->m_pTask->Run();

	::MessageBox(pDialog->GetSafeHwnd(), L"Done.", L"Information", MB_ICONINFORMATION | MB_OK);

	::EnableWindow(pDialog->GetDlgItem(IDCANCEL)->GetSafeHwnd(), TRUE);

	return 0;
}

void CProgressDialog::Log(int/*enumLogEvent*/ enLogEvent, const char* szEvent)
{
	string strEntry =
		enLogEvent == 0/*enumLogEvent::info*/ ? "Information: " :
		enLogEvent == 1/*enumLogEvent::warning*/ ? "Warning: " :
		enLogEvent == 2/*enumLogEvent::error*/ ? "Error: " : "Unknown: ";
	strEntry += szEvent;
	strEntry += "\r\n";

	int iLength = m_edtProgress.GetWindowTextLength();
	m_edtProgress.SetSel(iLength, iLength);
	m_edtProgress.ReplaceSel(CA2W(strEntry.c_str()));
}

// CProgressDialog dialog

IMPLEMENT_DYNAMIC(CProgressDialog, CDialogEx)

CProgressDialog::CProgressDialog(CWnd* pParent, CTask* pTask)
	: CDialogEx(IDD_DIALOG_PROGRESS, pParent)
	, m_pThread(nullptr)
	, m_pTask(pTask)
{
	ASSERT(m_pTask != nullptr);
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
