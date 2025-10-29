#pragma once
#include "afxdialogex.h"

#include "_log_hub.h"


// CLogDialog dialog

class CLogDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CLogDialog)

private: // Fields

	_log_hub* m_pLogHub;

public:

	CLogDialog(_log_hub* pLogHub, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CLogDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editMessages;
	virtual BOOL OnInitDialog();
};
