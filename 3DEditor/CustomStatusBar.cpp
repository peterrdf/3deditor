#include "stdafx.h"
#include "CustomStatusBar.h"
#include "Resource.h"
#include "LogDialog.h"

#define WM_UPDATE_LOG_STATUS (WM_USER + 100)

BEGIN_MESSAGE_MAP(CCustomStatusBar, CMFCStatusBar)
    ON_WM_LBUTTONDOWN()
    ON_MESSAGE(WM_UPDATE_LOG_STATUS, OnUpdateLogStatus)   
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

CCustomStatusBar::CCustomStatusBar()
	: m_pLogHub(nullptr)
	, m_hIconInfo(nullptr)
	, m_hIconWarn(nullptr)
	, m_hIconErr(nullptr)
{	
}

CCustomStatusBar::~CCustomStatusBar()
{
}

void CCustomStatusBar::SetLogHub(_log_hub* pLogHub)
{
    ASSERT(pLogHub != nullptr);

    m_pLogHub = pLogHub;
    m_pLogHub->setLogView(this);
}

/*virtual*/ void CCustomStatusBar::onLogWrite(enumLogEvent enLogEvent, const std::string& strEvent) /*override*/
{
    if (GetCurrentThreadId() == AfxGetApp()->m_nThreadID) {
        UpdateLogStatus(enLogEvent, strEvent);
    }
    else {
        string* pMessage = new string(strEvent);
        PostMessage(WM_UPDATE_LOG_STATUS, (WPARAM)enLogEvent, (LPARAM)pMessage);
    }
}

LRESULT CCustomStatusBar::OnUpdateLogStatus(WPARAM wParam, LPARAM lParam)
{
    enumLogEvent enLogEvent = (enumLogEvent)wParam;
    string* pMessage = (string*)lParam;

    if (pMessage) {
        UpdateLogStatus(enLogEvent, *pMessage);
        delete pMessage;
    }

    return 0;
}

void CCustomStatusBar::OnLButtonDown(UINT nFlags, CPoint point)
{
    // Find which pane was clicked
    for (int i = 0; i < GetCount(); i++)
    {
        CRect paneRect;
        GetItemRect(i, &paneRect);
        
        if (paneRect.PtInRect(point))
        {
            HandlePaneClick(i, point);
            return;
        }
    }
    
    CMFCStatusBar::OnLButtonDown(nFlags, point);
}

void CCustomStatusBar::UpdateLogStatus(enumLogEvent enLogEvent, const std::string& strEvent)
{
    int nIndex = CommandToIndex(ID_INDICATOR_LOG);
    if (nIndex != -1) {
		HICON hIcon = nullptr;
        switch (enLogEvent)
        {
            case enumLogEvent::info:
                hIcon = m_hIconInfo;
                break;
            case enumLogEvent::warning:
                hIcon = m_hIconWarn;
                break;
            case enumLogEvent::error:
                hIcon = m_hIconErr;
                break;
		}

        if (hIcon != nullptr) {
            SetPaneIcon(nIndex, hIcon);
		}        

        CString strMessage;
        strMessage.Format(L"[WARN: %d] [ERR: %d] %S", m_pLogHub->getWarningsCount(), m_pLogHub->getErrorsCount(), strEvent.c_str());
        SetPaneText(nIndex, strMessage);
    }
}

void CCustomStatusBar::HandlePaneClick(int nPane, CPoint /*point*/)
{
    if (GetItemID(nPane) == ID_INDICATOR_LOG)
    {
        if (m_pLogHub) {
            CLogDialog logDlg(m_pLogHub, AfxGetMainWnd());
            logDlg.DoModal();
        }
	}
}

int CCustomStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_hIconInfo = (HICON)LoadImage(
        AfxGetResourceHandle(),
        MAKEINTRESOURCE(IDI_ICON_INFO),
        IMAGE_ICON,
        16, 16, LR_DEFAULTCOLOR
	);

    m_hIconWarn = (HICON)LoadImage(
        AfxGetResourceHandle(),
        MAKEINTRESOURCE(IDI_ICON_WARN),
        IMAGE_ICON,
		16, 16, LR_DEFAULTCOLOR
	);

    m_hIconErr = (HICON)LoadImage(
        AfxGetResourceHandle(),
        MAKEINTRESOURCE(IDI_ICON_ERR),
		IMAGE_ICON,
		16, 16, LR_DEFAULTCOLOR
	);

    return 0;
}

void CCustomStatusBar::OnDestroy()
{
    __super::OnDestroy();

    if (m_pLogHub != nullptr) {
		m_pLogHub->setLogView(nullptr);
    }

	::DestroyIcon(m_hIconInfo);
	m_hIconInfo = nullptr;
	::DestroyIcon(m_hIconWarn);
	m_hIconWarn = nullptr;
	::DestroyIcon(m_hIconErr);
	m_hIconErr = nullptr;
}
