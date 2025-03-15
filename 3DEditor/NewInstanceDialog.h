#pragma once

#include "_rdf_mvc.h"
#include "RDFClass.h"
#include "afxwin.h"

// CNewInstanceDialog dialog

class CNewInstanceDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CNewInstanceDialog)

private: // Members

	_rdf_controller * m_pController;

public: // Members

	// --------------------------------------------------------------------------------------------
	// RDF Class
	_rdf_class * m_pNewInstanceRDFClass;

public:
	CNewInstanceDialog(_rdf_controller* pController, CWnd* pParent = nullptr);   // standard constructor
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
