// CRenameDialog.cpp : implementation file
//

#include "stdafx.h"
#include "3DEditor.h"
#include "RenameDialog.h"
#include "afxdialogex.h"


// CRenameDialog dialog

IMPLEMENT_DYNAMIC(CRenameDialog, CDialogEx)

CRenameDialog::CRenameDialog(const CString& strName, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_RENAME, pParent)
	, m_strName(strName)
{}

CRenameDialog::~CRenameDialog()
{}

void CRenameDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
}


BEGIN_MESSAGE_MAP(CRenameDialog, CDialogEx)
END_MESSAGE_MAP()


// CRenameDialog message handlers


void CRenameDialog::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialogEx::OnOK();
}
