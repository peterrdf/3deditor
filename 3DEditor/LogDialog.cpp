// LogDialog.cpp : implementation file
//

#include "stdafx.h"
#include "3DEditor.h"
#include "afxdialogex.h"
#include "LogDialog.h"


// CLogDialog dialog

IMPLEMENT_DYNAMIC(CLogDialog, CDialogEx)

CLogDialog::CLogDialog(_log_hub* pLogHub, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LOG, pParent)
	, m_pLogHub(pLogHub)
{
	ASSERT(m_pLogHub != nullptr);
}

CLogDialog::~CLogDialog()
{
}

void CLogDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_MESSAGES, m_editMessages);
}


BEGIN_MESSAGE_MAP(CLogDialog, CDialogEx)
END_MESSAGE_MAP()


// CLogDialog message handlers

BOOL CLogDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	for (auto& message : m_pLogHub->getMessages()) {
		string strEntry =
			message.first == enumLogEvent::info ? "[INF] " :
			message.first == enumLogEvent::warning ? "[WARN] " :
			message.first == enumLogEvent::error ? "[ERR] " : "[UNK] ";
		strEntry += message.second;
		strEntry += "\r\n";

		int iLength = m_editMessages.GetWindowTextLength();
		if (iLength > 10240) {
			m_editMessages.SetSel(0, iLength);
			m_editMessages.ReplaceSel(L"...\r\n");

			iLength = m_editMessages.GetWindowTextLength();
		}

		m_editMessages.SetSel(iLength, iLength);
		m_editMessages.ReplaceSel(CA2W(strEntry.c_str()));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
