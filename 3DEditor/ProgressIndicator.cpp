
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "ProgressIndicator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ------------------------------------------------------------------------------------------------
ProgressStatus::ProgressStatus(LPCTSTR phase)
{
	m_phase = phase;
	m_range = 1;
	m_step = -1;
	m_10procents = -1;

	CString text;
	text.Format(L"%s...", m_phase);
	SetStatusText(text);
}

// ------------------------------------------------------------------------------------------------
ProgressStatus::~ProgressStatus()
{
	SetStatusText();
}

// ------------------------------------------------------------------------------------------------
void ProgressStatus::Start(int64_t range) 
{
	m_range = range > 0 ? range : 1;
	Step();
}

// ------------------------------------------------------------------------------------------------
void ProgressStatus::Step()
{
	m_step++;
	int p = (int)(10 * m_step / m_range);
	if (p > m_10procents) {
		m_10procents = p;
		CString text;
		text.Format(L"%s %d%%...", m_phase, m_10procents * 10);
		SetStatusText(text);
	}
}

// ------------------------------------------------------------------------------------------------
void ProgressStatus::Finish()
{
	CString text;
	text.Format(L"%s finished", m_phase);
	SetStatusText(text);
}

// ------------------------------------------------------------------------------------------------
void ProgressStatus::SetStatusText(LPCTSTR text)
{
	auto pMainWnd = AfxGetMainWnd();
	if (pMainWnd) {
		auto pStatusBar = pMainWnd->GetDlgItem(AFX_IDW_STATUS_BAR);
		if (pStatusBar) {
			if (text)
				pStatusBar->SetWindowText(text);
			else
				pStatusBar->SetWindowText(L"Ready");

			pStatusBar->UpdateWindow();
		}
	}
}
