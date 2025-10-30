#pragma once

#include <afxstatusbar.h>

#include "_log_hub.h"

// ************************************************************************************************
class CCustomStatusBar 
    : public CMFCStatusBar
    , public _log_view
{

private: // Fields

	_log_hub* m_pLogHub;

public:

    CCustomStatusBar();
    virtual ~CCustomStatusBar();

	// _log_view
    virtual void onLogWrite(enumLogEvent enLogEvent, const std::string& strEvent) override;

    void SetLogHub(_log_hub* pLogHub);

protected:
    afx_msg LRESULT OnUpdateLogStatus(WPARAM wParam, LPARAM lParam);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    
    DECLARE_MESSAGE_MAP()
    
private:
	void UpdateLogStatus(enumLogEvent enLogEvent, const std::string& strEvent);
    void HandlePaneClick(int nPane, CPoint point);
};