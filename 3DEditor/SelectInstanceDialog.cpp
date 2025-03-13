// SelectInstanceDialog.cpp : implementation file
//

#include "stdafx.h"

#include "3DEditor.h"
#include "SelectInstanceDialog.h"
#include "afxdialogex.h"
#include "RDFModel.h"

#define USED_SUFFIX L" [used]"


// CSelectInstanceDialog dialog

IMPLEMENT_DYNAMIC(CSelectInstanceDialog, CDialogEx)

CSelectInstanceDialog::CSelectInstanceDialog(CRDFController* pController, CRDFInstance* pInstance, 
	_rdf_property* pObjectRDFProperty, int64_t iCard, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SELECT_INSTANCE, pParent)
	, m_pController(pController)
	, m_pInstance(pInstance)
	, m_pObjectRDFProperty(pObjectRDFProperty)
	, m_iCard(iCard)
	, m_iInstance(-1)
	, m_strInstanceUniqueName(L"")
	, m_strOldInstanceUniqueName(EMPTY_INSTANCE)
{
	assert(m_pController != nullptr);
	assert(m_pInstance != nullptr);
	assert(m_pObjectRDFProperty != nullptr);
}

CSelectInstanceDialog::~CSelectInstanceDialog()
{
}

void CSelectInstanceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_INSTANCES, m_cmbInstances);
}


BEGIN_MESSAGE_MAP(CSelectInstanceDialog, CDialogEx)
END_MESSAGE_MAP()


// CSelectInstanceDialog message handlers

bool IsUsedRecursively(int64_t RDFInstanceI, int64_t RDFInstanceII)
{
	int64_t myInstance = GetInstanceInverseReferencesByIterator(RDFInstanceII, 0);
	while (myInstance) {
		if ((myInstance == RDFInstanceI) ||
			IsUsedRecursively(RDFInstanceI, myInstance)) {
			return true;
		}
		myInstance = GetInstanceInverseReferencesByIterator(RDFInstanceII, myInstance);
	}

	return false;
}

// ------------------------------------------------------------------------------------------------
BOOL CSelectInstanceDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	int64_t* piInstances = nullptr;
	int64_t iCard = 0;
	GetObjectProperty(m_pInstance->getOwlInstance(), m_pObjectRDFProperty->getRdfProperty(), &piInstances, &iCard);

	assert(iCard > 0);
	assert((m_iCard >= 0) && (m_iCard < iCard));
	UNUSED(iCard);
	assert(piInstances != nullptr);

	CRDFModel* pModel = m_pController->GetModel();
	assert(pModel != nullptr);

	auto& mapInstances = pModel->GetInstances();

	vector<int64_t> vecCompatibleInstances;
	pModel->GetCompatibleInstances(m_pInstance, m_pObjectRDFProperty, vecCompatibleInstances);
		
	int iInstance = m_cmbInstances.AddString(EMPTY_INSTANCE);
	m_cmbInstances.SetItemData(iInstance, 0);

	m_strOldInstanceUniqueName = EMPTY_INSTANCE;
	for (size_t iCompatibleInstance = 0; iCompatibleInstance < vecCompatibleInstances.size(); iCompatibleInstance++)
	{
		map<int64_t, CRDFInstance *>::const_iterator itInstanceValue = mapInstances.find(vecCompatibleInstances[iCompatibleInstance]);
		assert(itInstanceValue != mapInstances.end());

		CString strInstanceUniqueName = itInstanceValue->second->getUniqueName();
		if ((m_pInstance->getOwlInstance() != vecCompatibleInstances[iCompatibleInstance]) &&
			IsUsedRecursively(m_pInstance->getOwlInstance(), itInstanceValue->second->getOwlInstance()))
		{
			strInstanceUniqueName += USED_SUFFIX;
		}

		iInstance = m_cmbInstances.AddString(strInstanceUniqueName);
		m_cmbInstances.SetItemData(iInstance, vecCompatibleInstances[iCompatibleInstance]);

		if (piInstances[m_iCard] == vecCompatibleInstances[iCompatibleInstance])
		{
			m_strOldInstanceUniqueName = strInstanceUniqueName;
		}
	} // for (size_t iCompatibleInstance = ...	

	int iSelectedInstance = m_cmbInstances.FindString(0, m_strOldInstanceUniqueName);
	m_cmbInstances.SetCurSel(iSelectedInstance);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CSelectInstanceDialog::OnOK()
{
	int iSelectedInstance = m_cmbInstances.GetCurSel();
	m_iInstance = m_cmbInstances.GetItemData(iSelectedInstance);
	m_cmbInstances.GetLBText(iSelectedInstance, m_strInstanceUniqueName);

	CDialogEx::OnOK();
}
