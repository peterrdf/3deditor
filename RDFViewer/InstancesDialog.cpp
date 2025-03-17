// InstancesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "RDFViewer.h"
#include "InstancesDialog.h"
#include "afxdialogex.h"
#include "RDFModel.h"
#include "RDFController.h"

#include "_ptr.h"

#include <algorithm>
using namespace std;

// CInstancesDialog dialog

IMPLEMENT_DYNAMIC(CInstancesDialog, CDialogEx)

/*virtual*/ void CInstancesDialog::onModelLoaded() /*override*/
{
	m_bUpdateInProgress = true;

	m_lcInstances.DeleteAllItems();
	m_mapInstance2Item.clear();

	auto pModel = getRDFController()->getModel();

	vector<_rdf_instance*> vecModel;
	for (auto pInstance : pModel->getInstances()) {
		if (pInstance->getGeometry()->isReferenced()) {
			continue;
		}

		vecModel.push_back(_ptr<_rdf_instance>(pInstance));
	}

	/*
	* Model
	*/
	sort(vecModel.begin(), vecModel.end(), _instancesComparator());

	for (size_t iInstance = 0; iInstance < vecModel.size(); iInstance++) {
		int iItem = m_lcInstances.InsertItem((int)iInstance, vecModel[iInstance]->getUniqueName());

		m_lcInstances.SetItemData(iItem, (DWORD_PTR)vecModel[iInstance]);

		m_mapInstance2Item[vecModel[iInstance]] = iItem;
	}

	m_bUpdateInProgress = false;
}

/*virtual*/ void CInstancesDialog::onInstanceSelected(_view* pSender) /*override*/
{
	if (pSender == this) {
		return;
	}

	m_bUpdateInProgress = true;

	// Unselect
	POSITION pos = m_lcInstances.GetFirstSelectedItemPosition();
	while (pos) {
		int iItem = m_lcInstances.GetNextSelectedItem(pos);
		m_lcInstances.SetItemState(iItem, 0, LVIS_SELECTED);
	}

	auto pSelectedInstance = getRDFController()->getSelectedInstance();
	if (pSelectedInstance == nullptr) {
		return;
	}

	_ptr<_rdf_instance> rdfInstance(pSelectedInstance);

	auto itInstance2Item = m_mapInstance2Item.find(rdfInstance);
	if (itInstance2Item != m_mapInstance2Item.end()) {
		m_lcInstances.EnsureVisible(itInstance2Item->second, TRUE);
		m_lcInstances.SetItemState(itInstance2Item->second, LVIS_SELECTED, LVIS_SELECTED);
	}

	m_bUpdateInProgress = false;
}

/*virtual*/ void CInstancesDialog::onControllerChanged() /*override*/
{
	getController()->registerView(this);

	m_lcInstances.SetController(getRDFController());
}

/*virtual*/ void CInstancesDialog::onInstanceDeleted(_view* /*pSender*/, _rdf_instance* /*pInstance*/) /*override*/
{
	onModelLoaded();
}

/*virtual*/ void CInstancesDialog::onInstancesDeleted(_view* /*pSender*/) /*override*/
{
	onModelLoaded();
}

// ------------------------------------------------------------------------------------------------
void CInstancesDialog::OnSelectedInstanceChanged(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	*pResult = 0;

	if (m_bUpdateInProgress) {
		return;
	}

	vector<_rdf_instance*> vecSelectedInstances;

	POSITION pos = m_lcInstances.GetFirstSelectedItemPosition();
	while (pos) {
		int iItem = m_lcInstances.GetNextSelectedItem(pos);

		_rdf_instance* pInstance = (_rdf_instance*)m_lcInstances.GetItemData(iItem);
		ASSERT(pInstance != nullptr);

		vecSelectedInstances.push_back(pInstance);
	}

	getRDFController()->selectInstance(this, vecSelectedInstances.size() == 1 ? vecSelectedInstances[0] : nullptr);
}

CInstancesDialog::CInstancesDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CInstancesDialog::IDD, pParent)
	, m_lcInstances()
	, m_bUpdateInProgress(false)
	, m_mapInstance2Item()
{

}

CInstancesDialog::~CInstancesDialog()
{
}

void CInstancesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInstancesDialog, CDialogEx)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TREE_INSTANCE_VIEW, OnSelectedInstanceChanged)
END_MESSAGE_MAP()


// CInstancesDialog message handlers


int CInstancesDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rcClient;
	GetClientRect(rcClient);

	rcClient.DeflateRect(10, 10);

	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_SHOWSELALWAYS;
	if (!m_lcInstances.Create(dwViewStyle, rcClient, this, IDC_TREE_INSTANCE_VIEW)) {
		TRACE0("Failed to create Instances list\n");

		return -1; // fail to create
	}

	m_lcInstances.InsertColumn(0, L"Instances", LVCFMT_LEFT, rcClient.Width());

	// disable the sorting
	m_lcInstances.ModifyStyle(LVS_SORTASCENDING | LVS_SORTDESCENDING, 0);

	return 0;
}


void CInstancesDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect(rcClient);

	rcClient.DeflateRect(10, 10);

	m_lcInstances.SetWindowPos(nullptr, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CInstancesDialog::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	if (point == CPoint(-1, -1)) {
		return;
	}

	vector<_rdf_instance*> vecSelectedInstances;

	POSITION pos = m_lcInstances.GetFirstSelectedItemPosition();
	while (pos) {
		int iItem = m_lcInstances.GetNextSelectedItem(pos);

		_rdf_instance* pInstance = (_rdf_instance*)m_lcInstances.GetItemData(iItem);
		ASSERT(pInstance != nullptr);

		vecSelectedInstances.push_back(pInstance);
	}

	/*
	* Multi selection
	*/

	if (vecSelectedInstances.size() > 1) {
		bool bIsGeometryInstanceSelected = false;
		for (size_t iInstance = 0; iInstance < vecSelectedInstances.size(); iInstance++) {
			if (vecSelectedInstances[iInstance]->hasGeometry()) {
				bIsGeometryInstanceSelected = true;

				break;
			}
		}

		if (bIsGeometryInstanceSelected) {
			/*
			* At least one instance has a geometry
			*/

			CMenu menu;
			VERIFY(menu.LoadMenuW(IDR_POPUP_INSTANCES_MULTI_SEL));

			CMenu* pPopup = menu.GetSubMenu(0);

			UINT uiCommand = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD, point.x, point.y, &m_lcInstances);
			if (uiCommand == 0) {
				return;
			}

			switch (uiCommand) {
				case ID_INSTANCES_ENABLE:
				{
					for (size_t iInstance = 0; iInstance < vecSelectedInstances.size(); iInstance++) {
						vecSelectedInstances[iInstance]->setEnable(true);
					}

					getController()->onInstancesEnabledStateChanged(this);
				}
				break;

				case ID_INSTANCES_DISABLE:
				{
					for (size_t iInstance = 0; iInstance < vecSelectedInstances.size(); iInstance++) {
						vecSelectedInstances[iInstance]->setEnable(false);
					}

					getController()->onInstancesEnabledStateChanged(this);
				}
				break;

				case ID_INSTANCES_REMOVE:
				{
					vector<_rdf_instance*> vecInstances;
					for (size_t iInstance = 0; iInstance < vecSelectedInstances.size(); iInstance++) {
						if (vecSelectedInstances[iInstance]->getGeometry()->isReferenced()) {
							MessageBox(L"The instance is referenced and can't be removed.", L"Error", MB_ICONERROR | MB_OK);

							continue;
						}

						vecInstances.push_back(vecSelectedInstances[iInstance]);
					}

					getRDFController()->deleteInstances(this, vecInstances);
				}
				break;

				default:
				{
					ASSERT(false);
				}
				break;
			} // switch (uiCommand)	
		} // if (bIsGeometryInstanceSelected)
		else {
			/*
			* Instances with a geometry
			*/

			CMenu menu;
			VERIFY(menu.LoadMenuW(IDR_POPUP_INSTANCES_NO_GEOMETRY));

			CMenu* pPopup = menu.GetSubMenu(0);

			UINT uiCommand = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD, point.x, point.y, &m_lcInstances);
			if (uiCommand == 0) {
				return;
			}

			switch (uiCommand) {
				case ID_INSTANCES_REMOVE:
				{
					vector<_rdf_instance*> vecInstances;
					for (size_t iInstance = 0; iInstance < vecSelectedInstances.size(); iInstance++) {
						if (vecSelectedInstances[iInstance]->getGeometry()->isReferenced()) {
							MessageBox(L"The instance is referenced and can't be removed.", L"Error", MB_ICONERROR | MB_OK);

							continue;
						}

						vecInstances.push_back(vecSelectedInstances[iInstance]);
					}

					getRDFController()->deleteInstances(this, vecInstances);
				}
				break;

				default:
				{
					ASSERT(false);
				}
				break;
			} // switch (uiCommand)
		} // else if (bIsGeometryInstanceSelected)

		return;
	} // if (m_setSelectedItems.size() > 1)

	/*
	* Single selection
	*/
	if (vecSelectedInstances.size() == 1) {
		/*
		* Instances with a geometry
		*/
		if (vecSelectedInstances[0]->hasGeometry()) {
			CMenu menu;
			VERIFY(menu.LoadMenuW(IDR_POPUP_INSTANCES));

			CMenu* pPopup = menu.GetSubMenu(0);

			if (vecSelectedInstances[0]->getEnable()) {
				pPopup->CheckMenuItem(ID_INSTANCES_ENABLE, MF_BYCOMMAND | MF_CHECKED);
			}

			UINT uiCommand = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD, point.x, point.y, &m_lcInstances);
			if (uiCommand == 0) {
				return;
			}

			switch (uiCommand) {
				case ID_INSTANCES_ZOOM_TO:
				{
					getController()->zoomToInstance(vecSelectedInstances[0]);
				}
				break;

				case ID_VIEW_ZOOM_OUT:
				{
					getController()->zoomOut();
				}
				break;

				case ID_INSTANCES_DISABLE_ALL_BUT_THIS:
				{
					for (auto pInstance : getRDFModel()->getInstances()) {
						pInstance->setEnable(pInstance == vecSelectedInstances[0]);
					}

					getController()->onInstancesEnabledStateChanged(this);
				}
				break;

				case ID_INSTANCES_ENABLE_ALL:
				{
					for (auto pInstance : getRDFModel()->getInstances()) {
						pInstance->setEnable(true);
					}

					getController()->onInstancesEnabledStateChanged(this);
				}
				break;

				case ID_INSTANCES_ENABLE:
				{
					vecSelectedInstances[0]->setEnable(!vecSelectedInstances[0]->getEnable());

					getController()->onInstancesEnabledStateChanged(this);
				}
				break;

				case ID_INSTANCES_REMOVE:
				{
					if (vecSelectedInstances[0]->getGeometry()->isReferenced()) {
						MessageBox(L"The instance is referenced and can't be removed.", L"Error", MB_ICONERROR | MB_OK);

						return;
					}

					getRDFController()->deleteInstance(this, vecSelectedInstances[0]);
				}
				break;

				default:
				{
					ASSERT(false);
				}
				break;
			} // switch (uiCommand)	

			return;
		} // if (vecSelectedInstances[0]->HasGeometry())

		/*
		* Instances without a geometry
		*/
		CMenu menu;
		VERIFY(menu.LoadMenuW(IDR_POPUP_INSTANCES_NO_GEOMETRY));

		CMenu* pPopup = menu.GetSubMenu(0);

		UINT uiCommand = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD, point.x, point.y, &m_lcInstances);
		if (uiCommand == 0) {
			return;
		}

		switch (uiCommand) {
			case ID_INSTANCES_REMOVE:
			{
				if (vecSelectedInstances[0]->getGeometry()->isReferenced()) {
					MessageBox(L"The instance is referenced and can't be removed.", L"Error", MB_ICONERROR | MB_OK);

					return;
				}

				getRDFController()->deleteInstance(this, vecSelectedInstances[0]);
			}
			break;

			default:
			{
				ASSERT(false);
			}
			break;
		} // switch (uiCommand)
		return;
	} // if (vecSelectedInstances.size() == 1)	
}