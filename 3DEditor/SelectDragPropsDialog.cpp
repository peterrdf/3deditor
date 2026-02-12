// CSelectDragPropsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "3DEditor.h"
#include "afxdialogex.h"
#include "../include/displayName.h"
#include "SelectDragPropsDialog.h"


// CSelectDragPropsDialog dialog

IMPLEMENT_DYNAMIC(CSelectDragPropsDialog, CDialogEx)

CSelectDragPropsDialog::CSelectDragPropsDialog(DragFace& dragFace, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SELECT_DRAG_PROPS, pParent)
    , m_dragFace(dragFace)	
{

}

CSelectDragPropsDialog::~CSelectDragPropsDialog()
{
}

void CSelectDragPropsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROPS, m_wndPropsList);
}


BEGIN_MESSAGE_MAP(CSelectDragPropsDialog, CDialogEx)
END_MESSAGE_MAP()


// CSelectDragPropsDialog message handlers

BOOL CSelectDragPropsDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_wndPropsList.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
	m_wndPropsList.InsertColumn(0, L"Property");
	m_wndPropsList.InsertColumn(1, L"Effect");

	RdfProperty prop = 0;
    double effect = 0.0;

	while (NULL != (prop = m_dragFace.GetActivePropertyByIterator(prop, effect))) {
		auto name = DisplayName(prop);
		auto item = m_wndPropsList.InsertItem(0, name.c_str());
		m_wndPropsList.SetCheck(item);
		m_wndPropsList.SetItemText(item, 1, std::to_wstring(fabs(effect) * 100).c_str());
		m_wndPropsList.SetItemData(item, prop);
	}

	for (int i = 0; i < m_wndPropsList.GetHeaderCtrl()->GetItemCount(); ++i)
		m_wndPropsList.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectDragPropsDialog::OnOK()
{
	int nProps = m_wndPropsList.GetItemCount();

	bool cnt = 0;
	for (int item = 0; item < nProps; item++) {
		if (m_wndPropsList.GetCheck(item)) {
			cnt++;
		}
	}

	if (!cnt) {
		AfxMessageBox(L"At least one property must be checked", MB_ICONSTOP);
		return;
	}


	for (int item = 0; item < nProps; item++) {
		if (!m_wndPropsList.GetCheck(item)) {
			RdfProperty prop = m_wndPropsList.GetItemData(item);
			m_dragFace.RemoveActiveProperty(prop);
		}
	}

	CDialogEx::OnOK();
}
