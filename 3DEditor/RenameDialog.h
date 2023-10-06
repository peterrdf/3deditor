#pragma once

#include "RDFInstance.h"


// CRenameDialog dialog

class CRenameDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CRenameDialog)

public:
	CRenameDialog(const CString& strName, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CRenameDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RENAME };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
public:
	CString m_strName;
};
