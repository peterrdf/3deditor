#pragma once

#include "RDFController.h"
#include "RDFClass.h"
#include "afxwin.h"

// CNewInstanceDialog dialog

class CNewInstanceDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CNewInstanceDialog)

private: // Members

	// --------------------------------------------------------------------------------------------
	// Controller
	CRDFController * m_pController;

public: // Members

	// --------------------------------------------------------------------------------------------
	// RDF Class
	CRDFClass * m_pNewInstanceRDFClass;

public:
	CNewInstanceDialog(CRDFController * pController, CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewInstanceDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_NEW_INSTANCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox m_cmbClasses;
	virtual void OnOK();
};
