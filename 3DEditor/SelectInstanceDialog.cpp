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

CSelectInstanceDialog::CSelectInstanceDialog(CRDFController* pController, CRDFInstance* pRDFInstance, 
	CObjectRDFProperty* pObjectRDFProperty, int64_t iCard, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SELECT_INSTANCE, pParent)
	, m_pController(pController)
	, m_pRDFInstance(pRDFInstance)
	, m_pObjectRDFProperty(pObjectRDFProperty)
	, m_iCard(iCard)
	, m_iInstance(-1)
	, m_strInstanceUniqueName(L"")
	, m_strOldInstanceUniqueName(EMPTY_INSTANCE)
{
	ASSERT(m_pController != NULL);
	ASSERT(m_pRDFInstance != NULL);
	ASSERT(m_pObjectRDFProperty != NULL);
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

	int64_t* piInstances = NULL;
	int64_t iCard = 0;
	GetObjectProperty(m_pRDFInstance->getInstance(), m_pObjectRDFProperty->getInstance(), &piInstances, &iCard);

	ASSERT(iCard > 0);
	ASSERT((m_iCard >= 0) && (m_iCard < iCard));
	UNUSED(iCard);
	ASSERT(piInstances != NULL);

	CRDFModel* pModel = m_pController->GetModel();
	ASSERT(pModel != NULL);

	auto& mapRFDInstances = pModel->GetRDFInstances();

	vector<int64_t> vecCompatibleInstances;
	pModel->GetCompatibleInstances(m_pRDFInstance, m_pObjectRDFProperty, vecCompatibleInstances);
		
	int iInstance = m_cmbInstances.AddString(EMPTY_INSTANCE);
	m_cmbInstances.SetItemData(iInstance, 0);

	m_strOldInstanceUniqueName = EMPTY_INSTANCE;
	for (size_t iCompatibleInstance = 0; iCompatibleInstance < vecCompatibleInstances.size(); iCompatibleInstance++)
	{
		map<int64_t, CRDFInstance *>::const_iterator itInstanceValue = mapRFDInstances.find(vecCompatibleInstances[iCompatibleInstance]);
		ASSERT(itInstanceValue != mapRFDInstances.end());

		CString strInstanceUniqueName = itInstanceValue->second->getUniqueName();
		if ((m_pRDFInstance->getInstance() != vecCompatibleInstances[iCompatibleInstance]) &&
			IsUsedRecursively(m_pRDFInstance->getInstance(), itInstanceValue->second->getInstance()))
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
