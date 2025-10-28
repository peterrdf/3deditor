#include "stdafx.h"
#include "CustomStatusBar.h"
#include "Resource.h"

#define WM_UPDATE_LOG_STATUS (WM_USER + 100)

BEGIN_MESSAGE_MAP(CCustomStatusBar, CMFCStatusBar)
    ON_WM_LBUTTONDOWN()
    ON_MESSAGE(WM_UPDATE_LOG_STATUS, OnUpdateLogStatus)
END_MESSAGE_MAP()

CCustomStatusBar::CCustomStatusBar()
{
}

CCustomStatusBar::~CCustomStatusBar()
{
}

/*virtual*/ void CCustomStatusBar::onLogWrite(enumLogEvent enLogEvent, const std::string& strEvent) /*override*/
{
    if (GetCurrentThreadId() == AfxGetApp()->m_nThreadID) {
        CString strMessage;
        strMessage.Format(_T("Log [%d]: %S"), enLogEvent, strEvent.c_str());

        int nIndex = CommandToIndex(ID_INDICATOR_LOG);
        if (nIndex != -1) {
            SetPaneText(nIndex, strMessage);
        }
    }
    else {
        CString* pMessage = new CString();
        pMessage->Format(_T("Log [%d]: %S"), enLogEvent, strEvent.c_str());

        PostMessage(WM_UPDATE_LOG_STATUS, (WPARAM)enLogEvent, (LPARAM)pMessage);
    }    
}

LRESULT CCustomStatusBar::OnUpdateLogStatus(WPARAM wParam, LPARAM lParam)
{
    enumLogEvent enLogEvent = (enumLogEvent)wParam;
    CString* pMessage = (CString*)lParam;

    if (pMessage) {
        int nIndex = CommandToIndex(ID_INDICATOR_LOG);
        if (nIndex != -1) {
            SetPaneText(nIndex, *pMessage);
        }

        delete pMessage; // Clean up allocated memory
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

void CCustomStatusBar::HandlePaneClick(int nPane, CPoint point)
{
    UINT nID = GetItemID(nPane);
    
    switch (nID)
    {
        case ID_INDICATOR_LOG:
            AfxMessageBox(_T("Log pane clicked!"));
            break;
        // Handle other panes...
    }
}