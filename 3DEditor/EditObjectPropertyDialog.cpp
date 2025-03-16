// EditObjectPropertyDialog.cpp : implementation file
//

#include "stdafx.h"
#include "3DEditor.h"
#include "EditObjectPropertyDialog.h"
#include "afxdialogex.h"
#include "RDFModel.h"

#include "_ptr.h"

// ************************************************************************************************
void CEditObjectPropertyDialog::ValidateUI()
{
	UpdateData(TRUE);

	switch (m_iMode) {
		case 0: // Existing instance
		{
			GetDlgItem(ID_APPLY_CHANGES)->EnableWindow(m_cmbExistingInstance.GetCurSel() != CB_ERR ? TRUE : FALSE);
		}
		break;

		case 1: // New instance
		{
			GetDlgItem(ID_APPLY_CHANGES)->EnableWindow(m_cmbNewInstance.GetCurSel() != CB_ERR ? TRUE : FALSE);
		}
		break;

		default:
		{
			ASSERT(false);
		}
		break;
	} // switch (m_iMode)
}

// CEditObjectPropertyDialog dialog

IMPLEMENT_DYNAMIC(CEditObjectPropertyDialog, CDialogEx)

CEditObjectPropertyDialog::CEditObjectPropertyDialog(_rdf_controller* pController, _rdf_instance* pInstance, _rdf_property* pProperty, CWnd* pParent /*=nullptr*/)
	: CDialogEx(CEditObjectPropertyDialog::IDD, pParent)
	, m_pController(pController)
	, m_pInstance(pInstance)
	, m_pProperty(pProperty)
	, m_iMode(0)
	, m_pSelectedInstance(nullptr)
	, m_selectedOwlClass(0)
{
	ASSERT(m_pController != nullptr);
	ASSERT(m_pInstance != nullptr);
	ASSERT(m_pProperty != nullptr);
}

CEditObjectPropertyDialog::~CEditObjectPropertyDialog()
{
}

void CEditObjectPropertyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_EXISTING_INSTANCE, m_iMode);
	DDX_Control(pDX, IDC_COMBO_EXISTING_INSTANCE, m_cmbExistingInstance);
	DDX_Control(pDX, IDC_COMBO_NEW_INSTANCE, m_cmbNewInstance);
}

BEGIN_MESSAGE_MAP(CEditObjectPropertyDialog, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO_EXISTING_INSTANCE, &CEditObjectPropertyDialog::OnBnClickedRadioExistingInstance)
	ON_BN_CLICKED(IDC_RADIO_NEW_INSTANCE, &CEditObjectPropertyDialog::OnBnClickedRadioNewInstance)
	ON_CBN_SELCHANGE(IDC_COMBO_EXISTING_INSTANCE, &CEditObjectPropertyDialog::OnCbnSelchangeComboExistingInstance)
	ON_CBN_SELCHANGE(IDC_COMBO_NEW_INSTANCE, &CEditObjectPropertyDialog::OnCbnSelchangeComboNewInstance)
	ON_BN_CLICKED(ID_APPLY_CHANGES, &CEditObjectPropertyDialog::OnBnClickedApplyChanges)
END_MESSAGE_MAP()

BOOL CEditObjectPropertyDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	int64_t* piInstances = nullptr;
	int64_t iCard = 0;
	GetObjectProperty(m_pInstance->getOwlInstance(), m_pProperty->getRdfProperty(), &piInstances, &iCard);

	/*
	* Restrictions
	*/

	vector<OwlClass> vecRestrictionClasses;
	m_pProperty->getRangeRestrictions(vecRestrictionClasses);
	ASSERT(!vecRestrictionClasses.empty());

	/*
	* Populate Existing instance combo
	*/
	for (auto pInstance : m_pController->getModel()->getInstances()) {
		/*
		* Skip this instance
		*/
		if (pInstance == m_pInstance) {
			continue;
		}

		/*
		* Check this instance
		*/
		if (find(vecRestrictionClasses.begin(), vecRestrictionClasses.end(), pInstance->getGeometry()->getOwlClass()) != vecRestrictionClasses.end()) {
			int iItem = m_cmbExistingInstance.AddString(pInstance->getUniqueName());
			m_cmbExistingInstance.SetItemDataPtr(iItem, pInstance);

			continue;
		}

		/*
		* Check the ancestor classes
		*/

		vector<OwlClass> vecAncestorClasses;
		_rdf_class::getAncestors(pInstance->getGeometry()->getOwlClass(), vecAncestorClasses);

		if (vecAncestorClasses.empty()) {
			continue;
		}

		for (size_t iAncestorClass = 0; iAncestorClass < vecAncestorClasses.size(); iAncestorClass++) {
			if (find(vecRestrictionClasses.begin(), vecRestrictionClasses.end(), vecAncestorClasses[iAncestorClass]) != vecRestrictionClasses.end()) {
				int iItem = m_cmbExistingInstance.AddString(pInstance->getUniqueName());
				m_cmbExistingInstance.SetItemDataPtr(iItem, pInstance);

				break;
			}
		} // for (size_t iAncestorClass = ...
	} // for (; itRFDInstances != ...

	/*
	* Populate New instance combo
	*/

	/*
	* Restrictions
	*/
	set<OwlClass> setClasses;
	for (size_t iRestriction = 0; iRestriction < vecRestrictionClasses.size(); iRestriction++) {
		setClasses.insert(vecRestrictionClasses[iRestriction]);
	}

	/*
	* Ancestors
	*/
	OwlClass owlClass = GetClassesByIterator(m_pInstance->getOwlModel(), 0);
	while (owlClass != 0) {
		vector<OwlClass> vecAncestorClasses;
		_rdf_class::getAncestors(owlClass, vecAncestorClasses);

		if (!vecAncestorClasses.empty()) {
			for (size_t iAncestorClass = 0; iAncestorClass < vecAncestorClasses.size(); iAncestorClass++) {
				if (find(vecRestrictionClasses.begin(), vecRestrictionClasses.end(), vecAncestorClasses[iAncestorClass]) != vecRestrictionClasses.end()) {
					setClasses.insert(owlClass);

					break;
				}
			}
		}

		owlClass = GetClassesByIterator(m_pInstance->getOwlModel(), owlClass);
	}

	for (set<int64_t>::iterator itClass = setClasses.begin(); itClass != setClasses.end(); itClass++) {
		char* szClassName = nullptr;
		GetNameOfClass(*itClass, &szClassName);

		m_cmbNewInstance.AddString(CA2W(szClassName));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditObjectPropertyDialog::OnBnClickedRadioExistingInstance()
{
	ValidateUI();
}

void CEditObjectPropertyDialog::OnBnClickedRadioNewInstance()
{
	ValidateUI();
}

void CEditObjectPropertyDialog::OnCbnSelchangeComboExistingInstance()
{
	ValidateUI();
}

void CEditObjectPropertyDialog::OnCbnSelchangeComboNewInstance()
{
	ValidateUI();
}

void CEditObjectPropertyDialog::OnBnClickedApplyChanges()
{
	UpdateData(TRUE);

	switch (m_iMode) {
		case 0: // Existing instance
		{
			ASSERT(m_cmbExistingInstance.GetCurSel() != CB_ERR);

			m_pSelectedInstance = (_instance*)m_cmbExistingInstance.GetItemDataPtr(m_cmbExistingInstance.GetCurSel());
		}
		break;

		case 1: // New instance
		{
			ASSERT(m_cmbNewInstance.GetCurSel() != CB_ERR);

			CString strClassName;
			m_cmbNewInstance.GetLBText(m_cmbNewInstance.GetCurSel(), strClassName);

			m_selectedOwlClass = GetClassByName(m_pInstance->getOwlModel(), CW2A((LPCTSTR)strClassName));
			ASSERT(m_selectedOwlClass != 0);
		}
		break;

		default:
		{
			ASSERT(false);
		}
		break;
	} // switch (m_iMode)

	OnOK();
}
