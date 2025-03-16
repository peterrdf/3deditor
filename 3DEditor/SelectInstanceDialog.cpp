// SelectInstanceDialog.cpp : implementation file
//

#include "stdafx.h"

#include "3DEditor.h"
#include "SelectInstanceDialog.h"
#include "afxdialogex.h"
#include "RDFModel.h"

#include "_ptr.h"

#define USED_SUFFIX L" [used]"

// ************************************************************************************************
IMPLEMENT_DYNAMIC(CSelectInstanceDialog, CDialogEx)

CSelectInstanceDialog::CSelectInstanceDialog(_rdf_controller* pController, _rdf_instance* pInstance,
	_rdf_property* pProperty, int64_t iCard, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SELECT_INSTANCE, pParent)
	, m_pController(pController)
	, m_pInstance(pInstance)
	, m_pProperty(pProperty)
	, m_iCard(iCard)
	, m_selectedOwlInstance(0)
	, m_strInstanceUniqueName(L"")
	, m_strInstanceOldUniqueName(EMPTY_INSTANCE)
{
	ASSERT(m_pController != nullptr);
	ASSERT(m_pInstance != nullptr);
	ASSERT(m_pProperty != nullptr);
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

	OwlInstance* pOwlInstances = nullptr;
	int64_t iCard = 0;
	GetObjectProperty(m_pInstance->getOwlInstance(), m_pProperty->getRdfProperty(), &pOwlInstances, &iCard);

	ASSERT(iCard > 0);
	ASSERT((m_iCard >= 0) && (m_iCard < iCard));
	UNUSED(iCard);
	ASSERT(pOwlInstances != nullptr);

	_ptr<_rdf_model> rdfModel(m_pController->getModel());

	vector<OwlInstance> vecCompatibleInstances;
	rdfModel->getCompatibleInstances(m_pInstance, m_pProperty, vecCompatibleInstances);

	int iInstance = m_cmbInstances.AddString(EMPTY_INSTANCE);
	m_cmbInstances.SetItemData(iInstance, 0);

	m_strInstanceOldUniqueName = EMPTY_INSTANCE;
	for (size_t iCompatibleInstance = 0; iCompatibleInstance < vecCompatibleInstances.size(); iCompatibleInstance++) {
		auto pCompatibleInstance = rdfModel->getInstance(vecCompatibleInstances[iCompatibleInstance]);
		ASSERT(pCompatibleInstance != nullptr);

		CString strInstanceUniqueName = pCompatibleInstance->getUniqueName();
		if ((m_pInstance->getOwlInstance() != vecCompatibleInstances[iCompatibleInstance]) &&
			IsUsedRecursively(m_pInstance->getOwlInstance(), pCompatibleInstance->getOwlInstance())) {
			strInstanceUniqueName += USED_SUFFIX;
		}

		iInstance = m_cmbInstances.AddString(strInstanceUniqueName);
		m_cmbInstances.SetItemData(iInstance, vecCompatibleInstances[iCompatibleInstance]);

		if (pOwlInstances[m_iCard] == vecCompatibleInstances[iCompatibleInstance]) {
			m_strInstanceOldUniqueName = strInstanceUniqueName;
		}
	}

	int iSelectedInstance = m_cmbInstances.FindStringExact(0, m_strInstanceOldUniqueName);
	m_cmbInstances.SetCurSel(iSelectedInstance);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CSelectInstanceDialog::OnOK()
{
	int iSelectedInstance = m_cmbInstances.GetCurSel();
	m_selectedOwlInstance = m_cmbInstances.GetItemData(iSelectedInstance);
	m_cmbInstances.GetLBText(iSelectedInstance, m_strInstanceUniqueName);

	CDialogEx::OnOK();
}
