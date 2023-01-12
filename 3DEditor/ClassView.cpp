
#include "stdafx.h"
#include "MainFrm.h"
#include "ClassView.h"
#include "Resource.h"
#include "3DEditor.h"
#include "RDFClass.h"
#include "RDFModel.h"
#include "ClassViewConsts.h"

class CClassViewMenuButton : public CMFCToolBarMenuButton
{

	DECLARE_SERIAL(CClassViewMenuButton)

public:
	CClassViewMenuButton(HMENU hMenu = NULL) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		// Patch: wrong background when the app starts
		pImages->SetTransparentColor(::GetSysColor(COLOR_BTNFACE));

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CClassViewMenuButton, CMFCToolBarMenuButton, 1)

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CClassView::OnModelChanged()
{
	if (GetController()->IsTestMode())
	{
		return;
	}

	UpdateView();
}

// ------------------------------------------------------------------------------------------------
void CClassView::UpdateView()
{
	m_pSearchDialog->Reset();

	switch (m_nCurrSort)
	{
	case ID_SORTING_SORTALPHABETIC:
	{
		ClassesAlphabeticalView();
	}
	break;

	case ID_SORTING_SORTBYTYPE:
	{
		ClassesHierarchicalView();
	}
	break;

	case ID_SORTING_SORTBYACCESS:
	{
		PropertiesAlphabeticalView();
	}
	break;
	} // switch (m_nCurrSort)
}

// ------------------------------------------------------------------------------------------------
void CClassView::ClassesAlphabeticalView()
{
	m_wndClassView.DeleteAllItems();

	ASSERT(GetController() != NULL);

	CRDFModel * pModel = GetController()->GetModel();
	ASSERT(pModel != NULL);

	const map<int64_t, CRDFClass *> & mapRDFClasses = pModel->GetRDFClasses();

	// RDF Classes => Name : Instance
	map<wstring, int64_t> mapName2Instance;

	map<int64_t, CRDFClass *>::const_iterator itRDFClass = mapRDFClasses.begin();
	for (; itRDFClass != mapRDFClasses.end(); itRDFClass++)
	{
		mapName2Instance[itRDFClass->second->getName()] = itRDFClass->second->getInstance();
	}

	HTREEITEM hRoot = m_wndClassView.InsertItem(_T("Classes"), IMAGE_MODEL, IMAGE_MODEL);
	m_wndClassView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	map<wstring, int64_t>::iterator itName2Instance = mapName2Instance.begin();
	for (; itName2Instance != mapName2Instance.end(); itName2Instance++)
	{
		itRDFClass = mapRDFClasses.find(itName2Instance->second);
		ASSERT(itRDFClass != mapRDFClasses.end());

		CRDFClass * pRDFClass = itRDFClass->second;

		/*
		* Classes
		*/
		HTREEITEM hClass = AddClass(hRoot, pRDFClass->getInstance(), true);

		/*
		* Properties
		*/
		AddProperties(hClass, pRDFClass->getInstance());
	} // for (; itName2Instance != ...	

	m_wndClassView.Expand(hRoot, TVE_EXPAND);
}

// ------------------------------------------------------------------------------------------------
void CClassView::ClassesHierarchicalView()
{
	m_wndClassView.DeleteAllItems();

	ASSERT(GetController() != NULL);

	CRDFModel * pModel = GetController()->GetModel();
	ASSERT(pModel != NULL);

	const map<int64_t, CRDFClass *> & mapRDFClasses = pModel->GetRDFClasses();

	vector<int64_t> vecRootClasses;

	map<int64_t, CRDFClass *>::const_iterator itRDFClass = mapRDFClasses.begin();
	for (; itRDFClass != mapRDFClasses.end(); itRDFClass++)
	{
		CRDFClass * pRDFClass = itRDFClass->second;

		if (pRDFClass->getParentClasses().size() == 0)
		{
			vecRootClasses.push_back(pRDFClass->getInstance());
		}
	}

	HTREEITEM hRoot = m_wndClassView.InsertItem(_T("Classes"), IMAGE_MODEL, IMAGE_MODEL);
	m_wndClassView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	for (size_t iClass = 0; iClass < vecRootClasses.size(); iClass++)
	{
		HTREEITEM hClass = AddClass(hRoot, vecRootClasses[iClass], false);

		AddProperties(hClass, vecRootClasses[iClass]);

		AddChildClasses(hClass, vecRootClasses[iClass]);
	} // for (size_t iClass = ...

	m_wndClassView.Expand(hRoot, TVE_EXPAND);
}

// ------------------------------------------------------------------------------------------------
HTREEITEM CClassView::AddClass(HTREEITEM hParent, int64_t iClassInstance, bool bAddParentClasses)
{
	ASSERT(iClassInstance != 0);
	ASSERT(GetController() != NULL);

	CRDFModel * pModel = GetController()->GetModel();
	ASSERT(pModel != NULL);

	const map<int64_t, CRDFClass *> & mapRDFClasses = pModel->GetRDFClasses();

	map<int64_t, CRDFClass *>::const_iterator itRDFClass = mapRDFClasses.find(iClassInstance);
	ASSERT(itRDFClass != mapRDFClasses.end());

	CRDFClass * pRDFClass = itRDFClass->second;

	HTREEITEM hClass = m_wndClassView.InsertItem(pRDFClass->getName(), IMAGE_CLASS, IMAGE_CLASS, hParent);

	if (bAddParentClasses)
	{
		const vector<int64_t> & vecParentClasses = pRDFClass->getParentClasses();
		if (!vecParentClasses.empty())
		{
			HTREEITEM hParentClasses = m_wndClassView.InsertItem(L"Parent Classes", IMAGE_COLLECTION, IMAGE_COLLECTION, hClass);

			for (size_t iParentClass = 0; iParentClass < vecParentClasses.size(); iParentClass++)
			{
				map<int64_t, CRDFClass *>::const_iterator itParentRDFClass = mapRDFClasses.find(vecParentClasses[iParentClass]);
				ASSERT(itParentRDFClass != mapRDFClasses.end());

				CRDFClass * pParentRDFClass = itParentRDFClass->second;

				m_wndClassView.InsertItem(pParentRDFClass->getName(), IMAGE_CLASS, IMAGE_CLASS, hParentClasses);
			} // for (size_t iParentClass = ...
		} // if (!vecParentClasses.empty())
	} // if (bAddParentClasses)	

	return hClass;
}

// ------------------------------------------------------------------------------------------------
void CClassView::AddProperties(HTREEITEM hParent, int64_t iClassInstance)
{
	wchar_t szBuffer[100];

	ASSERT(iClassInstance != 0);
	ASSERT(GetController() != NULL);

	CRDFModel * pModel = GetController()->GetModel();
	ASSERT(pModel != NULL);

	const map<int64_t, CRDFClass *> & mapRDFClasses = pModel->GetRDFClasses();
	const map<int64_t, CRDFProperty *> & mapRDFProperties = pModel->GetRDFProperties();

	map<int64_t, CRDFClass *>::const_iterator itRDFClass = mapRDFClasses.find(iClassInstance);
	ASSERT(itRDFClass != mapRDFClasses.end());

	CRDFClass * pRDFClass = itRDFClass->second;

	vector<int64_t> vecAncestors;
	pModel->GetClassAncestors(pRDFClass->getInstance(), vecAncestors);

	vecAncestors.push_back(pRDFClass->getInstance());

	for (size_t iAncestorClass = 0; iAncestorClass < vecAncestors.size(); iAncestorClass++)
	{
		map<int64_t, CRDFClass *>::const_iterator itAncestorRDFClass = mapRDFClasses.find(vecAncestors[iAncestorClass]);
		ASSERT(itAncestorRDFClass != mapRDFClasses.end());

		CRDFClass * pAncestorRDFClass = itAncestorRDFClass->second;

		const vector<CRDFPropertyRestriction *> & vecPropertyRestrictions = pAncestorRDFClass->getPropertyRestrictions();
		for (size_t iPropertyRestriction = 0; iPropertyRestriction < vecPropertyRestrictions.size(); iPropertyRestriction++)
		{
			map<int64_t, CRDFProperty *>::const_iterator itRDFProperty = mapRDFProperties.find(vecPropertyRestrictions[iPropertyRestriction]->getPropertyInstance());
			ASSERT(itRDFProperty != mapRDFProperties.end());

			CRDFProperty * pRDFProperty = itRDFProperty->second;

			wstring strNameAndType = pRDFProperty->getName();
			strNameAndType += L" : ";

			switch (pRDFProperty->getType())
			{
			case TYPE_OBJECTTYPE:
			{
				strNameAndType += L"owl:ObjectProperty";
			}
			break;

			case TYPE_BOOL_DATATYPE:
			{
				strNameAndType += L"owl:DatatypeProperty";
			}
			break;

			case TYPE_CHAR_DATATYPE:
			{
				strNameAndType += L"owl:DatatypeProperty";
			}
			break;

			case TYPE_DOUBLE_DATATYPE:
			{
				strNameAndType += L"owl:DatatypeProperty";
			}
			break;

			case TYPE_INT_DATATYPE:
			{
				strNameAndType += L"owl:DatatypeProperty";
			}
			break;

			default:
			{
				strNameAndType += L"unknown";
			}
			break;
			} // switch (pRDFProperty->getType())

			HTREEITEM hProperty = m_wndClassView.InsertItem(strNameAndType.c_str(), IMAGE_PROPERTY, IMAGE_PROPERTY, hParent);

			switch (pRDFProperty->getType())
			{
			case TYPE_OBJECTTYPE:
			{
				HTREEITEM hRange = m_wndClassView.InsertItem(L"rdfs:range", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);

				CObjectRDFProperty * pObjectRDFProperty = dynamic_cast<CObjectRDFProperty *>(pRDFProperty);
				ASSERT(pObjectRDFProperty != NULL);

				auto& vecRestrictions = pObjectRDFProperty->getRestrictions();
				for (size_t iRestriction = 0; iRestriction < vecRestrictions.size(); iRestriction++)
				{
					auto itRestrictionRDFClass = mapRDFClasses.find(vecRestrictions[iRestriction]);
					ASSERT(itRestrictionRDFClass != mapRDFClasses.end());
					ASSERT(itRestrictionRDFClass->second != NULL);

					m_wndClassView.InsertItem(itRestrictionRDFClass->second->getName(), IMAGE_VALUE, IMAGE_VALUE, hRange);
				}
			}
			break;

			case TYPE_BOOL_DATATYPE:
			{
				m_wndClassView.InsertItem(L"rdfs:range : xsd:boolean", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
			}
			break;

			case TYPE_CHAR_DATATYPE:
			{
				m_wndClassView.InsertItem(L"rdfs:range : xsd:string", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
			}
			break;

			case TYPE_DOUBLE_DATATYPE:
			{
				m_wndClassView.InsertItem(L"rdfs:range : xsd:double", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
			}
			break;

			case TYPE_INT_DATATYPE:
			{
				m_wndClassView.InsertItem(L"rdfs:range : xsd:integer", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
			}
			break;

			default:
			{
				m_wndClassView.InsertItem(L"rdfs:range : unknown", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
			}
			break;
			} // switch (pRDFProperty->getType())

			if (vecPropertyRestrictions[iPropertyRestriction]->getMaxCard() == -1)
			{
				swprintf(szBuffer, 100, L"owl:cardinality : [%lld - infinity>", vecPropertyRestrictions[iPropertyRestriction]->getMinCard());
			}
			else
			{
				swprintf(szBuffer, 100, L"owl:cardinality : [%lld - %lld]", vecPropertyRestrictions[iPropertyRestriction]->getMinCard(), vecPropertyRestrictions[iPropertyRestriction]->getMaxCard());
			}

			m_wndClassView.InsertItem(szBuffer, IMAGE_VALUE, IMAGE_VALUE, hProperty);
		} // for (size_t iPropertyRestriction = ...
	} // for (size_t iAncestorClass = ...
}

// ------------------------------------------------------------------------------------------------
void CClassView::AddChildClasses(HTREEITEM hParent, int64_t iClassInstance)
{
	ASSERT(iClassInstance != 0);
	ASSERT(GetController() != NULL);

	CRDFModel * pModel = GetController()->GetModel();
	ASSERT(pModel != NULL);

	const map<int64_t, CRDFClass *> & mapRDFClasses = pModel->GetRDFClasses();

	map<int64_t, CRDFClass *>::const_iterator itParentRDFClass = mapRDFClasses.find(iClassInstance);
	ASSERT(itParentRDFClass != mapRDFClasses.end());

	map<int64_t, CRDFClass *>::const_iterator itRDFClass = mapRDFClasses.begin();
	for (; itRDFClass != mapRDFClasses.end(); itRDFClass++)
	{
		CRDFClass * pRDFClass = itRDFClass->second;

		const vector<int64_t> & vecParentClasses = pRDFClass->getParentClasses();
		if (vecParentClasses.empty())
		{
			continue;
		}

		if (vecParentClasses[0] == iClassInstance)
		{
			HTREEITEM hClass = AddClass(hParent, pRDFClass->getInstance(), false);

			AddProperties(hClass, pRDFClass->getInstance());

			AddChildClasses(hClass, pRDFClass->getInstance());
		}
	} // for (; itRDFClass != ...
}

// ------------------------------------------------------------------------------------------------
void CClassView::PropertiesAlphabeticalView()
{
	m_wndClassView.DeleteAllItems();

	ASSERT(GetController() != NULL);

	CRDFModel * pModel = GetController()->GetModel();
	ASSERT(pModel != NULL);

	const map<int64_t, CRDFProperty *> & mapRDFProperties = pModel->GetRDFProperties();

	// RDF Property => Name : Instance
	map<wstring, int64_t> mapName2Instance;

	HTREEITEM hRoot = m_wndClassView.InsertItem(_T("Properties"), IMAGE_MODEL, IMAGE_MODEL);
	m_wndClassView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);	

	map<int64_t, CRDFProperty *>::const_iterator itRDFProperty = mapRDFProperties.begin();
	for (; itRDFProperty != mapRDFProperties.end(); itRDFProperty++)
	{
		CRDFProperty * pRDFProperty = itRDFProperty->second;

		mapName2Instance[pRDFProperty->getName()] = pRDFProperty->getInstance();
	} // for (; itRDFProperty != ...	

	map<wstring, int64_t>::iterator itName2Instance = mapName2Instance.begin();
	for (; itName2Instance != mapName2Instance.end(); itName2Instance++)
	{
		itRDFProperty = mapRDFProperties.find(itName2Instance->second);
		ASSERT(itRDFProperty != mapRDFProperties.end());

		CRDFProperty * pRDFProperty = itRDFProperty->second;

		wstring strNameAndType = pRDFProperty->getName();
		strNameAndType += L" : ";

		switch (pRDFProperty->getType())
		{
		case TYPE_OBJECTTYPE:
		{
			strNameAndType += L"owl:ObjectProperty";
		}
		break;

		case TYPE_BOOL_DATATYPE:
		{
			strNameAndType += L"owl:DatatypeProperty";
		}
		break;

		case TYPE_CHAR_DATATYPE:
		{
			strNameAndType += L"owl:DatatypeProperty";
		}
		break;

		case TYPE_DOUBLE_DATATYPE:
		{
			strNameAndType += L"owl:DatatypeProperty";
		}
		break;

		case TYPE_INT_DATATYPE:
		{
			strNameAndType += L"owl:DatatypeProperty";
		}
		break;

		default:
		{
			strNameAndType += L"unknown";
		}
		break;
		} // switch (pRDFProperty->getType())

		HTREEITEM hProperty = m_wndClassView.InsertItem(strNameAndType.c_str(), IMAGE_PROPERTY, IMAGE_PROPERTY, hRoot);

		switch (pRDFProperty->getType())
		{
		case TYPE_OBJECTTYPE:
		{
			HTREEITEM hRange = m_wndClassView.InsertItem(L"rdfs:range", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);

			CObjectRDFProperty * pObjectRDFProperty = dynamic_cast<CObjectRDFProperty *>(pRDFProperty);
			ASSERT(pObjectRDFProperty != NULL);

			const vector<int64_t> & vecRestrictions = pObjectRDFProperty->getRestrictions();
			ASSERT(!vecRestrictions.empty());

			for (size_t iRestriction = 0; iRestriction < vecRestrictions.size(); iRestriction++)
			{
				char * szClassName = NULL;
				GetNameOfClass(vecRestrictions[iRestriction], &szClassName);

				m_wndClassView.InsertItem(CA2W(szClassName), IMAGE_VALUE, IMAGE_VALUE, hRange);
			}
		}
		break;

		case TYPE_BOOL_DATATYPE:
		{
			m_wndClassView.InsertItem(L"rdfs:range : xsd:boolean", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
		}
		break;

		case TYPE_CHAR_DATATYPE:
		{
			m_wndClassView.InsertItem(L"rdfs:range : xsd:string", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
		}
		break;

		case TYPE_DOUBLE_DATATYPE:
		{
			m_wndClassView.InsertItem(L"rdfs:range : xsd:double", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
		}
		break;

		case TYPE_INT_DATATYPE:
		{
			m_wndClassView.InsertItem(L"rdfs:range : xsd:integer", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
		}
		break;

		default:
		{
			m_wndClassView.InsertItem(L"rdfs:range : unknown", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
		}
		break;
		} // switch (pRDFProperty->getType())
	} // for (; itName2Instance != ...

	m_wndClassView.Expand(hRoot, TVE_EXPAND);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClassView::CClassView()
	: m_pSearchDialog(NULL)
{
	m_nCurrSort = ID_SORTING_SORTALPHABETIC;
}

CClassView::~CClassView()
{
}

BEGIN_MESSAGE_MAP(CClassView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CLASS_ADD_MEMBER_FUNCTION, OnClassAddMemberFunction)
	ON_COMMAND(ID_CLASS_ADD_MEMBER_VARIABLE, OnClassAddMemberVariable)
	ON_COMMAND(ID_CLASS_DEFINITION, OnClassDefinition)
	ON_COMMAND(ID_CLASS_PROPERTIES, OnClassProperties)
	ON_COMMAND(ID_NEW_FOLDER, OnNewFolder)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnSort)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnUpdateSort)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassView message handlers

int CClassView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	ASSERT(GetController() != NULL);
	GetController()->RegisterView(this);

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create views:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndClassView.Create(dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("Failed to create Class View\n");
		return -1;      // fail to create
	}

	// Load images:
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SORT);
	m_wndToolBar.LoadToolBar(IDR_SORT, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	CMenu menuSort;
	menuSort.LoadMenu(IDR_POPUP_SORT);

	m_wndToolBar.ReplaceButton(ID_SORT_MENU, CClassViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));

	CClassViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CClassViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
		pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
		pButton->SetMessageWnd(this);
	}

	//  Search
	m_pSearchDialog = new CSearchClassesDialog(&m_wndClassView);
	m_pSearchDialog->Create(IDD_DIALOG_SEARCH_CLASSES, this);

	return 0;
}

void CClassView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CClassView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndClassView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// Select clicked item:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	CMenu menu;
	menu.LoadMenu(IDR_POPUP_SORT);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}
}

void CClassView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndClassView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CClassView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CClassView::OnSort(UINT id)
{
	if (m_nCurrSort == id)
	{
		return;
	}

	m_nCurrSort = id;

	CClassViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CClassViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->SetImage(GetCmdMgr()->GetCmdImage(id));
		m_wndToolBar.Invalidate();
		m_wndToolBar.UpdateWindow();
	}

	UpdateView();
}

void CClassView::OnUpdateSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

void CClassView::OnClassAddMemberFunction()
{
	AfxMessageBox(_T("Add member function..."));
}

void CClassView::OnClassAddMemberVariable()
{
	// TODO: Add your command handler code here
}

void CClassView::OnClassDefinition()
{
	// TODO: Add your command handler code here
}

void CClassView::OnClassProperties()
{
	// TODO: Add your command handler code here
}

void CClassView::OnNewFolder()
{
	if (!m_pSearchDialog->IsWindowVisible())
	{
		m_pSearchDialog->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pSearchDialog->ShowWindow(SW_HIDE);
	}
}

void CClassView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndClassView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CClassView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndClassView.SetFocus();
}

void CClassView::OnChangeVisualStyle()
{
	m_ClassViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASS_VIEW_24 : IDB_CLASS_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ClassViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ClassViewImages.Add(&bmp, RGB(255, 0, 0));

	m_wndClassView.SetImageList(&m_ClassViewImages, TVSIL_NORMAL);

	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_24 : IDR_SORT, 0, 0, TRUE /* Locked */);
}


void CClassView::OnDestroy()
{
	ASSERT(GetController() != NULL);
	GetController()->UnRegisterView(this);

	__super::OnDestroy();

	delete m_pSearchDialog;
}


void CClassView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	if (!bShow)
	{
		m_pSearchDialog->ShowWindow(SW_HIDE);
	}
}
