// EditObjectPropertyDialog.cpp : implementation file
//

#include "stdafx.h"
#include "3DEditor.h"
#include "EditObjectPropertyDialog.h"
#include "afxdialogex.h"
#include "RDFModel.h"

// ------------------------------------------------------------------------------------------------
void CEditObjectPropertyDialog::ValidateUI()
{
	UpdateData(TRUE);

	switch (m_iMode)
	{
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
		assert(false);
	}
	break;
	} // switch (m_iMode)
}

// CEditObjectPropertyDialog dialog

IMPLEMENT_DYNAMIC(CEditObjectPropertyDialog, CDialogEx)

CEditObjectPropertyDialog::CEditObjectPropertyDialog(CRDFController * pController, CRDFInstance * pInstance, CRDFProperty * pProperty, CWnd* pParent /*=nullptr*/)
	: CDialogEx(CEditObjectPropertyDialog::IDD, pParent)
	, m_pController(pController)
	, m_pInstance(pInstance)
	, m_pProperty(pProperty)
	, m_pExisitngRDFInstance(nullptr)
	, m_iNewInstanceRDFClass(0)
	, m_iMode(0)
{
	assert(m_pController != nullptr);
	assert(m_pInstance != nullptr);
	assert(m_pProperty != nullptr);
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


// CEditObjectPropertyDialog message handlers


BOOL CEditObjectPropertyDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	int64_t * piInstances = nullptr;
	int64_t iCard = 0;
	GetObjectProperty(m_pInstance->_instance::getOwlInstance(), m_pProperty->GetInstance(), &piInstances, &iCard);

	CRDFModel * pModel = m_pController->GetModel();
	assert(pModel != nullptr);

	auto& mapInstances = pModel->GetInstances();

	/*
	* Restrictions
	*/
	CObjectRDFProperty * pObjectRDFProperty = dynamic_cast<CObjectRDFProperty *>(m_pProperty);
	assert(pObjectRDFProperty != nullptr);

	const vector<int64_t> & vecRestrictions = pObjectRDFProperty->GetRestrictions();
	assert(!vecRestrictions.empty());

	/*
	* Populate Existing instance combo
	*/
	map<int64_t, CRDFInstance *>::const_iterator itRFDInstances = mapInstances.begin();
	for (; itRFDInstances != mapInstances.end(); itRFDInstances++)
	{
		/*
		* Skip this instance
		*/
		if (itRFDInstances->second == m_pInstance)
		{
			continue;
		}

		/*
		* Skip the instances that belong to a different model
		*/
		if (itRFDInstances->second->_instance::getOwlModel() != m_pInstance->_instance::getOwlModel())
		{
			continue;
		}

		/*
		* Check this instance
		*/
		if (find(vecRestrictions.begin(), vecRestrictions.end(), itRFDInstances->second->getClassInstance()) != vecRestrictions.end())
		{
			int iItem = m_cmbExistingInstance.AddString(itRFDInstances->second->_instance::getUniqueName());
			m_cmbExistingInstance.SetItemDataPtr(iItem, itRFDInstances->second);

			continue;
		}

		/*
		* Check the ancestor classes
		*/

		vector<int64_t> vecAncestorClasses;
		CRDFClass::GetAncestors(itRFDInstances->second->getClassInstance(), vecAncestorClasses);

		if (vecAncestorClasses.empty())
		{
			continue;
		}

		for (size_t iAncestorClass = 0; iAncestorClass < vecAncestorClasses.size(); iAncestorClass++)
		{
			if (find(vecRestrictions.begin(), vecRestrictions.end(), vecAncestorClasses[iAncestorClass]) != vecRestrictions.end())
			{
				int iItem = m_cmbExistingInstance.AddString(itRFDInstances->second->_instance::getUniqueName());
				m_cmbExistingInstance.SetItemDataPtr(iItem, itRFDInstances->second);

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
	set<int64_t> setClasses;
	for (size_t iRestriction = 0; iRestriction < vecRestrictions.size(); iRestriction++)
	{
		setClasses.insert(vecRestrictions[iRestriction]);
	}

	/*
	* Ancestors
	*/
	int64_t	iClassInstance = GetClassesByIterator(m_pInstance->_instance::getOwlModel(), 0);
	while (iClassInstance != 0)
	{
		vector<int64_t> vecAncestorClasses;
		CRDFClass::GetAncestors(iClassInstance, vecAncestorClasses);

		if (!vecAncestorClasses.empty())
		{
			for (size_t iAncestorClass = 0; iAncestorClass < vecAncestorClasses.size(); iAncestorClass++)
			{
				if (find(vecRestrictions.begin(), vecRestrictions.end(), vecAncestorClasses[iAncestorClass]) != vecRestrictions.end())
				{
					setClasses.insert(iClassInstance);

					break;
				}
			} // for (size_t iAncestorClass = ...
		} // if (!vecAncestorClasses.empty())

		iClassInstance = GetClassesByIterator(m_pInstance->_instance::getOwlModel(), iClassInstance);
	} // while (iClassInstance != 0)

	for (set<int64_t>::iterator itClass = setClasses.begin(); itClass != setClasses.end(); itClass++)
	{
		char * szClassName = nullptr;
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

	switch (m_iMode)
	{
	case 0: // Existing instance
	{
		assert(m_cmbExistingInstance.GetCurSel() != CB_ERR);

		m_pExisitngRDFInstance = (CRDFInstance *)m_cmbExistingInstance.GetItemDataPtr(m_cmbExistingInstance.GetCurSel());
	}
	break;

	case 1: // New instance
	{
		assert(m_cmbNewInstance.GetCurSel() != CB_ERR);

		CString strClassName;
		m_cmbNewInstance.GetLBText(m_cmbNewInstance.GetCurSel(), strClassName);

		m_iNewInstanceRDFClass = GetClassByName(m_pInstance->_instance::getOwlModel(), CW2A((LPCTSTR)strClassName));
		assert(m_iNewInstanceRDFClass != 0);
	}
	break;

	default:
	{
		assert(false);
	}
	break;
	} // switch (m_iMode)

	OnOK();
}
