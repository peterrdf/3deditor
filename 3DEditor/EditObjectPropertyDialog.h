#pragma once

#include "RDFController.h"
#include "afxwin.h"

#include "_rdf_instance.h"
#include "_rdf_class.h"

// ************************************************************************************************
class CEditObjectPropertyDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CEditObjectPropertyDialog)

private: // Fields

	// --------------------------------------------------------------------------------------------
	// Controller
	CRDFController * m_pController;

	// --------------------------------------------------------------------------------------------
	// RDF Instance
	_rdf_instance* m_pInstance;

	// --------------------------------------------------------------------------------------------
	// RDF Property
	_rdf_property * m_pProperty;

public: // Fields

	// --------------------------------------------------------------------------------------------
	// RDF Instance
	_rdf_instance* m_pExisitngRDFInstance;

	// --------------------------------------------------------------------------------------------
	// RDF Class
	int64_t m_iNewInstanceRDFClass;

private: // Methods

	// --------------------------------------------------------------------------------------------
	// Validation
	void ValidateUI();

public:
	CEditObjectPropertyDialog(CRDFController * pController, _rdf_instance* pInstance, _rdf_property * pProperty, CWnd* pParent = nullptr);   // standard constructor
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
