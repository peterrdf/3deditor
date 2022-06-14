#pragma once

#include "RDFController.h"
#include "RDFClass.h"
#include "afxwin.h"

// CEditObjectPropertyDialog dialog

class CEditObjectPropertyDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CEditObjectPropertyDialog)

private: // Members

	// --------------------------------------------------------------------------------------------
	// Controller
	CRDFController * m_pController;

	// --------------------------------------------------------------------------------------------
	// RDF Instance
	CRDFInstance * m_pRDFInstance;

	// --------------------------------------------------------------------------------------------
	// RDF Property
	CRDFProperty * m_pRDFProperty;

public: // Members

	// --------------------------------------------------------------------------------------------
	// RDF Instance
	CRDFInstance * m_pExisitngRDFInstance;

	// --------------------------------------------------------------------------------------------
	// RDF Class
	int64_t m_iNewInstanceRDFClass;

private: // Methods

	// --------------------------------------------------------------------------------------------
	// Validation
	void ValidateUI();

public:
	CEditObjectPropertyDialog(CRDFController * pController, CRDFInstance * pRDFInstance, CRDFProperty * pRDFProperty, CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditObjectPropertyDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_EDIT_OBJECT_PROPRTY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_iMode;
	virtual BOOL OnInitDialog();
	CComboBox m_cmbExistingInstance;
	CComboBox m_cmbNewInstance;
	afx_msg void OnBnClickedRadioExistingInstance();
	afx_msg void OnBnClickedRadioNewInstance();
	afx_msg void OnCbnSelchangeComboExistingInstance();
	afx_msg void OnCbnSelchangeComboNewInstance();
	afx_msg void OnBnClickedApplyChanges();
};
