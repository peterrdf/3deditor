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
		ASSERT(false);
	}
	break;
	} // switch (m_iMode)
}

// CEditObjectPropertyDialog dialog

IMPLEMENT_DYNAMIC(CEditObjectPropertyDialog, CDialogEx)

CEditObjectPropertyDialog::CEditObjectPropertyDialog(CRDFController * pController, CRDFInstance * pRDFInstance, CRDFProperty * pRDFProperty, CWnd* pParent /*=NULL*/)
	: CDialogEx(CEditObjectPropertyDialog::IDD, pParent)
	, m_pController(pController)
	, m_pRDFInstance(pRDFInstance)
	, m_pRDFProperty(pRDFProperty)
	, m_pExisitngRDFInstance(NULL)
	, m_iNewInstanceRDFClass(NULL)
	, m_iMode(0)
{
	ASSERT(m_pController != NULL);
	ASSERT(m_pRDFInstance != NULL);
	ASSERT(m_pRDFProperty != NULL);
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

	int64_t * piInstances = NULL;
	int64_t iCard = 0;
	GetObjectProperty(m_pRDFInstance->getInstance(), m_pRDFProperty->getInstance(), &piInstances, &iCard);

	CRDFModel * pModel = m_pController->GetModel();
	ASSERT(pModel != NULL);

	auto& mapRFDInstances = pModel->GetRDFInstances();

	/*
	* Restrictions
	*/
	CObjectRDFProperty * pObjectRDFProperty = dynamic_cast<CObjectRDFProperty *>(m_pRDFProperty);
	ASSERT(pObjectRDFProperty != NULL);

	const vector<int64_t> & vecRestrictions = pObjectRDFProperty->getRestrictions();
	ASSERT(!vecRestrictions.empty());

	/*
	* Populate Existing instance combo
	*/
	map<int64_t, CRDFInstance *>::const_iterator itRFDInstances = mapRFDInstances.begin();
	for (; itRFDInstances != mapRFDInstances.end(); itRFDInstances++)
	{
		/*
		* Skip this instance
		*/
		if (itRFDInstances->second == m_pRDFInstance)
		{
			continue;
		}

		/*
		* Skip the instances that belong to a different model
		*/
		if (itRFDInstances->second->GetModel() != m_pRDFInstance->GetModel())
		{
			continue;
		}

		/*
		* Check this instance
		*/
		if (find(vecRestrictions.begin(), vecRestrictions.end(), itRFDInstances->second->getClassInstance()) != vecRestrictions.end())
		{
			int iItem = m_cmbExistingInstance.AddString(itRFDInstances->second->getUniqueName());
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
				int iItem = m_cmbExistingInstance.AddString(itRFDInstances->second->getUniqueName());
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
	int64_t	iClassInstance = GetClassesByIterator(m_pRDFInstance->GetModel(), 0);
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

		iClassInstance = GetClassesByIterator(m_pRDFInstance->GetModel(), iClassInstance);
	} // while (iClassInstance != 0)

	for (set<int64_t>::iterator itClass = setClasses.begin(); itClass != setClasses.end(); itClass++)
	{
		char * szClassName = NULL;
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
		ASSERT(m_cmbExistingInstance.GetCurSel() != CB_ERR);

		m_pExisitngRDFInstance = (CRDFInstance *)m_cmbExistingInstance.GetItemDataPtr(m_cmbExistingInstance.GetCurSel());
	}
	break;

	case 1: // New instance
	{
		ASSERT(m_cmbNewInstance.GetCurSel() != CB_ERR);

		CString strClassName;
		m_cmbNewInstance.GetLBText(m_cmbNewInstance.GetCurSel(), strClassName);

		m_iNewInstanceRDFClass = GetClassByName(m_pRDFInstance->GetModel(), CW2A((LPCTSTR)strClassName));
		ASSERT(m_iNewInstanceRDFClass != 0);
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
