#pragma once

#include "afxwin.h"

#include "_rdf_mvc.h"
#include "_rdf_class.h"

// ************************************************************************************************
class CNewInstanceDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CNewInstanceDialog)

private: // Fields

	_rdf_controller* m_pController;

public: // Fields

	_rdf_class* m_pNewInstanceRDFClass;

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
