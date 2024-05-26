// NewInstanceDialog.cpp : implementation file
//

#include "stdafx.h"
#include "3DEditor.h"
#include "NewInstanceDialog.h"
#include "afxdialogex.h"
#include "RDFModel.h"

// CNewInstanceDialog dialog

IMPLEMENT_DYNAMIC(CNewInstanceDialog, CDialogEx)

CNewInstanceDialog::CNewInstanceDialog(CRDFController * pController, CWnd* pParent /*=nullptr*/)
	: CDialogEx(CNewInstanceDialog::IDD, pParent)
	, m_pController(pController)
	, m_pNewInstanceRDFClass(nullptr)
{
	assert(m_pController != nullptr);
}

CNewInstanceDialog::~CNewInstanceDialog()
{
}

void CNewInstanceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CLASSES, m_cmbClasses);
}


BEGIN_MESSAGE_MAP(CNewInstanceDialog, CDialogEx)
END_MESSAGE_MAP()


// CNewInstanceDialog message handlers


BOOL CNewInstanceDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRDFModel * pModel = m_pController->GetModel();
	assert(pModel != nullptr);

	auto& mapClasses = pModel->GetClasses();

	auto itClass = mapClasses.begin();
	for (; itClass != mapClasses.end(); itClass++)
	{
		int iItem = m_cmbClasses.AddString(itClass->second->GetName());
		m_cmbClasses.SetItemDataPtr(iItem, itClass->second);
	} // for (; itClass != ...

	m_cmbClasses.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CNewInstanceDialog::OnOK()
{
	assert(m_cmbClasses.GetCurSel() != CB_ERR);

	m_pNewInstanceRDFClass = (CRDFClass *)m_cmbClasses.GetItemDataPtr(m_cmbClasses.GetCurSel());

	CDialogEx::OnOK();
}
