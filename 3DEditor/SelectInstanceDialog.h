#pragma once

#include "_rdf_mvc.h"
#include "_rdf_property.h"

// ************************************************************************************************
class CSelectInstanceDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectInstanceDialog)

private: // Fields

	_rdf_controller* m_pController;
	_rdf_instance* m_pInstance;
	_rdf_property* m_pProperty;
	int64_t m_iCard;

	OwlInstance m_selectedOwlInstance;
	CString m_strInstanceUniqueName;
	CString m_strInstanceOldUniqueName;

public: // Properties

	OwlInstance GetSelectedOwlInstance() const { return m_selectedOwlInstance; }
	const CString& GetSelectedInstanceUniqueName() const { return m_strInstanceUniqueName; }
	const CString& GetSelectedInstanceOldUniqueName() const { return m_strInstanceOldUniqueName; }

public:
	CSelectInstanceDialog(_rdf_controller* pController, _rdf_instance* pInstance,
		_rdf_property* pObjectRDFProperty, int64_t iCard, CWnd* pParent = nullptr);   // standard constructor
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
