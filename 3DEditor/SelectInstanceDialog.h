#pragma once

#include "RDFController.h"
#include "ObjectRDFProperty.h"

// CSelectInstanceDialog dialog

class CSelectInstanceDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectInstanceDialog)

private: // Members

	// --------------------------------------------------------------------------------------------
	CRDFController* m_pController;

	// --------------------------------------------------------------------------------------------
	CRDFInstance* m_pInstance;

	// --------------------------------------------------------------------------------------------
	CObjectRDFProperty* m_pObjectRDFProperty;

	// --------------------------------------------------------------------------------------------
	int64_t m_iCard;

public: // Members

	// --------------------------------------------------------------------------------------------
	int64_t m_iInstance;

	// --------------------------------------------------------------------------------------------
	CString m_strInstanceUniqueName;

	// --------------------------------------------------------------------------------------------
	CString m_strOldInstanceUniqueName;

public:
	CSelectInstanceDialog(CRDFController* pController, CRDFInstance* pInstance, 
		CObjectRDFProperty* pObjectRDFProperty, int64_t iCard, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSelectInstanceDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SELECT_INSTANCE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox m_cmbInstances;
	virtual void OnOK();
};
