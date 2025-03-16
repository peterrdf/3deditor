#pragma once

#include "afxwin.h"

#include "_rdf_mvc.h"
#include "_rdf_instance.h"
#include "_rdf_class.h"

// ************************************************************************************************
class CEditObjectPropertyDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CEditObjectPropertyDialog)

private: // Fields

	_rdf_controller* m_pController;
	_rdf_instance* m_pInstance;
	_rdf_property* m_pProperty;

	int m_iMode;
	_instance* m_pSelectedInstance;
	OwlClass m_selectedOwlClass;

public: // Properties

	int GetMode() const { return m_iMode; }
	_instance* GetInstance() const { return m_pSelectedInstance; }
	OwlClass GetOwlClass() const { return m_selectedOwlClass; }
	
private: // Methods

	void ValidateUI();

public:
	CEditObjectPropertyDialog(_rdf_controller* pController, _rdf_instance* pInstance, _rdf_property* pProperty, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CEditObjectPropertyDialog();

	// Dialog Data
	enum { IDD = IDD_DIALOG_EDIT_OBJECT_PROPRTY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL OnInitDialog();
	CComboBox m_cmbExistingInstance;
	CComboBox m_cmbNewInstance;
	afx_msg void OnBnClickedRadioExistingInstance();
	afx_msg void OnBnClickedRadioNewInstance();
	afx_msg void OnCbnSelchangeComboExistingInstance();
	afx_msg void OnCbnSelchangeComboNewInstance();
	afx_msg void OnBnClickedApplyChanges();
};
