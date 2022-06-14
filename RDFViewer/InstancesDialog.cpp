// InstancesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "RDFViewer.h"
#include "InstancesDialog.h"
#include "afxdialogex.h"
#include "RDFModel.h"
#include "RDFController.h"

#include <algorithm>

using namespace std;

// CInstancesDialog dialog

IMPLEMENT_DYNAMIC(CInstancesDialog, CDialogEx)

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CInstancesDialog::OnControllerChanged()
{
	ASSERT(GetController() != NULL);

	GetController()->RegisterView(this);
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CInstancesDialog::OnModelChanged()
{
	m_bInitInProgress = true;

	m_lcInstances.DeleteAllItems();
	m_mapInstance2Item.clear();	

	CRDFModel * pModel = GetController()->GetModel();
	ASSERT(pModel != NULL);

	const map<int64_t, CRDFInstance *> & mapRFDInstances = pModel->GetRDFInstances();

	vector<CRDFInstance *> vecModel;

	map<int64_t, CRDFInstance *>::const_iterator itRFDInstances = mapRFDInstances.begin();
	for (; itRFDInstances != mapRFDInstances.end(); itRFDInstances++)
	{
		CRDFInstance * pRDFInstance = itRFDInstances->second;

		if (pRDFInstance->isReferenced())
		{
			continue;
		}

		if (pRDFInstance->GetModel() == pModel->GetModel())
		{
			vecModel.push_back(pRDFInstance);
		}		
	} // for (; itRFDInstances != ...

	/*
	* Model
	*/
	sort(vecModel.begin(), vecModel.end(), SORT_RDFINSTANCES());	

	for (size_t iInstance = 0; iInstance < vecModel.size(); iInstance++)
	{
		int iItem = m_lcInstances.InsertItem((int)iInstance, vecModel[iInstance]->getUniqueName());

		m_lcInstances.SetItemData(iItem, (DWORD_PTR)vecModel[iInstance]);

		m_mapInstance2Item[vecModel[iInstance]] = iItem;
	}

	m_bInitInProgress = false;
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CInstancesDialog::OnInstanceSelected(CRDFView * pSender)
{
	if (pSender == this)
	{
		return;
	}	

	m_bInitInProgress = true;

	// Unselect
	POSITION pos = m_lcInstances.GetFirstSelectedItemPosition();
	while (pos)
	{
		int iItem = m_lcInstances.GetNextSelectedItem(pos);
		m_lcInstances.SetItemState(iItem, 0, LVIS_SELECTED);
	}

	ASSERT(GetController() != NULL);

	CRDFInstance * pSelectedInstance = GetController()->GetSelectedInstance();
	if (pSelectedInstance == NULL)
	{
		return;
	}

	map<CRDFInstance *, int>::iterator itInstance2Item = m_mapInstance2Item.find(pSelectedInstance);
	ASSERT(itInstance2Item != m_mapInstance2Item.end());
		
	m_lcInstances.EnsureVisible(itInstance2Item->second, TRUE);
	m_lcInstances.SetItemState(itInstance2Item->second, LVIS_SELECTED, LVIS_SELECTED);

	m_bInitInProgress = false;
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CInstancesDialog::OnInstanceDeleted(CRDFView * /*pSender*/, int64_t /*iInstance*/)
{
	OnModelChanged();
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CInstancesDialog::OnInstancesDeleted(CRDFView * /*pSender*/)
{
	OnModelChanged();
}

// ------------------------------------------------------------------------------------------------
void CInstancesDialog::OnSelectedInstanceChanged(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	*pResult = 0;

	if (m_bInitInProgress)
	{
		return;
	}

	vector<CRDFInstance *> vecSelectedInstances;

	POSITION pos = m_lcInstances.GetFirstSelectedItemPosition();
	while (pos)
	{
		int iItem = m_lcInstances.GetNextSelectedItem(pos);

		CRDFInstance * pRDFInstance = (CRDFInstance *)m_lcInstances.GetItemData(iItem);
		ASSERT(pRDFInstance != NULL);

		vecSelectedInstances.push_back(pRDFInstance);
	}

	ASSERT(GetController() != NULL);

	if (vecSelectedInstances.size() == 1)
	{
		GetController()->SelectInstance(this, vecSelectedInstances[0]);
	} // if (vecSelectedInstances.size() == 1)
	else
	{
		GetController()->SelectInstance(this, NULL);
	} // if (vecSelectedInstances.size() == 1)	
}

CInstancesDialog::CInstancesDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CInstancesDialog::IDD, pParent)
	, m_lcInstances()
	, m_bInitInProgress(false)
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
	if (!m_lcInstances.Create(dwViewStyle, rcClient, this, IDC_TREE_INSTANCE_VIEW))
	{
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

	m_lcInstances.SetWindowPos(NULL, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CInstancesDialog::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_lcInstances;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDialogEx::OnContextMenu(pWnd, point);
		return;
	}

	if (point == CPoint(-1, -1))
	{
		return;
	}

	vector<CRDFInstance *> vecSelectedInstances;

	POSITION pos = m_lcInstances.GetFirstSelectedItemPosition();
	while (pos)
	{
		int iItem = m_lcInstances.GetNextSelectedItem(pos);

		CRDFInstance * pRDFInstance = (CRDFInstance *)m_lcInstances.GetItemData(iItem);
		ASSERT(pRDFInstance != NULL);

		vecSelectedInstances.push_back(pRDFInstance);
	}

	ASSERT(GetController() != NULL);

	/*
	* Multi selection
	*/

	if (vecSelectedInstances.size() > 1)
	{
		bool bIsGeometryInstanceSelected = false;		
		for (size_t iInstance = 0; iInstance < vecSelectedInstances.size(); iInstance++)
		{
			if (vecSelectedInstances[iInstance]->hasGeometry())
			{
				bIsGeometryInstanceSelected = true;

				break;
			}
		}

		if (bIsGeometryInstanceSelected)
		{
			/*
			* At least one instance has a geometry
			*/

			CMenu menu;
			VERIFY(menu.LoadMenuW(IDR_POPUP_INSTANCES_MULTI_SEL));

			CMenu * pPopup = menu.GetSubMenu(0);

			UINT uiCommand = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD, point.x, point.y, pWndTree);
			if (uiCommand == 0)
			{
				return;
			}

			ASSERT(GetController() != NULL);

			switch (uiCommand)
			{
			case ID_INSTANCES_ENABLE:
			{
				for (size_t iInstance = 0; iInstance < vecSelectedInstances.size(); iInstance++)
				{
					vecSelectedInstances[iInstance]->setEnable(true);
				}

				GetController()->OnInstancesEnabledStateChanged();
			}
			break;

			case ID_INSTANCES_DISABLE:
			{
				for (size_t iInstance = 0; iInstance < vecSelectedInstances.size(); iInstance++)
				{
					vecSelectedInstances[iInstance]->setEnable(false);
				}

				GetController()->OnInstancesEnabledStateChanged();
			}
			break;

			case ID_INSTANCES_REMOVE:
			{
				vector<CRDFInstance *> vecInstances;
				for (size_t iInstance = 0; iInstance < vecSelectedInstances.size(); iInstance++)
				{
					if (vecSelectedInstances[iInstance]->isReferenced())
					{
						MessageBox(L"The instance is referenced and can't be removed.", L"Error", MB_ICONERROR | MB_OK);

						continue;
					}

					vecInstances.push_back(vecSelectedInstances[iInstance]);
				}

				GetController()->DeleteInstances(this, vecInstances);
			}
			break;

			default:
			{
				ASSERT(false);
			}
			break;
			} // switch (uiCommand)	
		} // if (bIsGeometryInstanceSelected)
		else
		{
			/*
			* Instances with a geometry
			*/

			CMenu menu;
			VERIFY(menu.LoadMenuW(IDR_POPUP_INSTANCES_NO_GEOMETRY));

			CMenu * pPopup = menu.GetSubMenu(0);

			UINT uiCommand = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD, point.x, point.y, pWndTree);
			if (uiCommand == 0)
			{
				return;
			}

			ASSERT(GetController() != NULL);

			switch (uiCommand)
			{
			case ID_INSTANCES_REMOVE:
			{
				vector<CRDFInstance *> vecInstances;
				for (size_t iInstance = 0; iInstance < vecSelectedInstances.size(); iInstance++)
				{
					if (vecSelectedInstances[iInstance]->isReferenced())
					{
						MessageBox(L"The instance is referenced and can't be removed.", L"Error", MB_ICONERROR | MB_OK);

						continue;
					}

					vecInstances.push_back(vecSelectedInstances[iInstance]);
				}

				GetController()->DeleteInstances(this, vecInstances);
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
	if (vecSelectedInstances.size() == 1)
	{
		/*
		* Instances with a geometry
		*/
		if (vecSelectedInstances[0]->hasGeometry())
		{
			CMenu menu;
			VERIFY(menu.LoadMenuW(IDR_POPUP_INSTANCES));

			CMenu * pPopup = menu.GetSubMenu(0);

			if (vecSelectedInstances[0]->getEnable())
			{
				pPopup->CheckMenuItem(ID_INSTANCES_ENABLE, MF_BYCOMMAND | MF_CHECKED);
			}

			UINT uiCommand = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD, point.x, point.y, pWndTree);
			if (uiCommand == 0)
			{
				return;
			}

			ASSERT(GetController() != NULL);

			CRDFModel * pModel = GetController()->GetModel();
			ASSERT(pModel != NULL);

			const map<int64_t, CRDFInstance *> & mapRFDInstances = pModel->GetRDFInstances();

			switch (uiCommand)
			{
			case ID_INSTANCES_ZOOM_TO:
			{
				GetController()->ZoomToInstance(vecSelectedInstances[0]->getInstance());
			}
			break;

			case ID_INSTANCES_DISABLE_ALL_BUT_THIS:
			{
				map<int64_t, CRDFInstance *>::const_iterator itRFDInstances = mapRFDInstances.begin();
				for (; itRFDInstances != mapRFDInstances.end(); itRFDInstances++)
				{
					if (vecSelectedInstances[0]->GetModel() != itRFDInstances->second->GetModel())
					{
						continue;
					}

					if (itRFDInstances->second == vecSelectedInstances[0])
					{
						itRFDInstances->second->setEnable(true);

						continue;
					}

					itRFDInstances->second->setEnable(false);
				}

				GetController()->OnInstancesEnabledStateChanged();
			}
			break;

			case ID_INSTANCES_ENABLE_ALL:
			{
				map<int64_t, CRDFInstance *>::const_iterator itRFDInstances = mapRFDInstances.begin();
				for (; itRFDInstances != mapRFDInstances.end(); itRFDInstances++)
				{
					if (vecSelectedInstances[0]->GetModel() != itRFDInstances->second->GetModel())
					{
						continue;
					}

					itRFDInstances->second->setEnable(true);
				}

				GetController()->OnInstancesEnabledStateChanged();
			}
			break;

			case ID_INSTANCES_ENABLE:
			{
				vecSelectedInstances[0]->setEnable(!vecSelectedInstances[0]->getEnable());

				GetController()->OnInstancesEnabledStateChanged();
			}
			break;

			case ID_INSTANCES_REMOVE:
			{
				if (vecSelectedInstances[0]->isReferenced())
				{
					MessageBox(L"The instance is referenced and can't be removed.", L"Error", MB_ICONERROR | MB_OK);

					return;
				}

				GetController()->DeleteInstance(this, vecSelectedInstances[0]);
			}
			break;

			default:
			{
				ASSERT(false);
			}
			break;
			} // switch (uiCommand)	

			return;
		} // if (vecSelectedInstances[0]->hasGeometry())

		/*
		* Instances without a geometry
		*/
		CMenu menu;
		VERIFY(menu.LoadMenuW(IDR_POPUP_INSTANCES_NO_GEOMETRY));

		CMenu * pPopup = menu.GetSubMenu(0);

		UINT uiCommand = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD, point.x, point.y, pWndTree);
		if (uiCommand == 0)
		{
			return;
		}

		ASSERT(GetController() != NULL);

		switch (uiCommand)
		{
		case ID_INSTANCES_REMOVE:
		{
			if (vecSelectedInstances[0]->isReferenced())
			{
				MessageBox(L"The instance is referenced and can't be removed.", L"Error", MB_ICONERROR | MB_OK);

				return;
			}

			GetController()->DeleteInstance(this, vecSelectedInstances[0]);
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