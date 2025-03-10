
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
	CClassViewMenuButton(HMENU hMenu = nullptr) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
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
/*virtual*/ void CClassView::onModelLoaded()
{
	if (GetController()->IsTestMode())
	{
		return;
	}

	UpdateView();
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ CTreeCtrlEx* CClassView::GetTreeView() /*override*/
{
	return &m_treeCtrl;
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ vector<CString> CClassView::GetSearchFilters() /*override*/
{
	return vector<CString>
	{
		_T("(All)"),
		_T("Classes"),
		_T("Properties"),
		_T("Values"),
	};
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ BOOL CClassView::ContainsText(int iFilter, HTREEITEM hItem, const CString& strText) /*override*/
{
	if (hItem == NULL)
	{
		assert(false);

		return FALSE;
	}

	CString strItemText = GetTreeView()->GetItemText(hItem);
	strItemText.MakeLower();

	CString strTextLower = strText;
	strTextLower.MakeLower();

	// Classes
	if (iFilter == (int)enumSearchFilter::Classes)
	{
		int iImage, iSelectedImage = -1;
		GetTreeView()->GetItemImage(hItem, iImage, iSelectedImage);

		assert(iImage == iSelectedImage);

		if (iImage == IMAGE_CLASS)
		{
			return strItemText.Find(strTextLower, 0) != -1;
		}
		else
		{
			return FALSE;
		}
	}

	// Properties
	if (iFilter == (int)enumSearchFilter::Properties)
	{
		int iImage, iSelectedImage = -1;
		GetTreeView()->GetItemImage(hItem, iImage, iSelectedImage);

		assert(iImage == iSelectedImage);

		if (iImage == IMAGE_PROPERTY)
		{
			return strItemText.Find(strTextLower, 0) != -1;
		}
		else
		{
			return FALSE;
		}
	}

	// Values
	if (iFilter == (int)enumSearchFilter::Values)
	{
		int iImage, iSelectedImage = -1;
		GetTreeView()->GetItemImage(hItem, iImage, iSelectedImage);

		assert(iImage == iSelectedImage);

		if (iImage == IMAGE_VALUE)
		{
			return strItemText.Find(strTextLower, 0) != -1;
		}
		else
		{
			return FALSE;
		}
	}

	// All
	return strItemText.Find(strTextLower, 0) != -1;
}

CRDFModel* CClassView::GetModel() const
{
	auto pController = GetController();
	if (pController == nullptr)
	{
		assert(false);

		return nullptr;
	}

	auto pModel = pController->GetModel();
	assert(pModel != nullptr);

	return pModel;
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
	m_treeCtrl.DeleteAllItems();

	auto pModel = GetModel();

	auto& mapClasses = pModel->GetClasses();

	// RDF Classes => Name : Instance
	map<wstring, int64_t> mapName2Instance;	
	for (auto itClass = mapClasses.begin();
		itClass != mapClasses.end(); 
		itClass++)
	{
		mapName2Instance[itClass->second->GetName()] = itClass->second->GetInstance();
	}

	HTREEITEM hRoot = m_treeCtrl.InsertItem(_T("Classes"), IMAGE_MODEL, IMAGE_MODEL);
	for (auto itName2Instance = mapName2Instance.begin();
		itName2Instance != mapName2Instance.end(); 
		itName2Instance++)
	{
		auto itClass = mapClasses.find(itName2Instance->second);
		assert(itClass != mapClasses.end());

		auto pClass = itClass->second;

		HTREEITEM hClass = AddClass(hRoot, pClass->GetInstance(), true);
		AddProperties(hClass, pClass->GetInstance());
	} // for (; itName2Instance != ...	

	m_treeCtrl.Expand(hRoot, TVE_EXPAND);
}

// ------------------------------------------------------------------------------------------------
void CClassView::ClassesHierarchicalView()
{
	m_treeCtrl.DeleteAllItems();

	auto pModel = GetModel();

	auto& mapClasses = pModel->GetClasses();

	vector<int64_t> vecRootClasses;	
	for (auto itClass = mapClasses.begin(); 
		itClass != mapClasses.end();
		itClass++)
	{
		auto pClass = itClass->second;

		if (pClass->getParentClasses().size() == 0)
		{
			vecRootClasses.push_back(pClass->GetInstance());
		}
	}

	HTREEITEM hRoot = m_treeCtrl.InsertItem(_T("Classes"), IMAGE_MODEL, IMAGE_MODEL);
	for (size_t iClass = 0; iClass < vecRootClasses.size(); iClass++)
	{
		HTREEITEM hClass = AddClass(hRoot, vecRootClasses[iClass], false);

		AddProperties(hClass, vecRootClasses[iClass]);
		AddChildClasses(hClass, vecRootClasses[iClass]);
	} // for (size_t iClass = ...

	m_treeCtrl.Expand(hRoot, TVE_EXPAND);
}

// ------------------------------------------------------------------------------------------------
HTREEITEM CClassView::AddClass(HTREEITEM hParent, int64_t iClassInstance, bool bAddParentClasses)
{
	assert(iClassInstance != 0);
	
	auto pModel = GetModel();

	auto mapClasses = pModel->GetClasses();

	auto itClass = mapClasses.find(iClassInstance);
	assert(itClass != mapClasses.end());

	auto pClass = itClass->second;

	HTREEITEM hClass = m_treeCtrl.InsertItem(pClass->GetName(), IMAGE_CLASS, IMAGE_CLASS, hParent);

	if (bAddParentClasses)
	{
		const vector<int64_t> & vecParentClasses = pClass->getParentClasses();
		if (!vecParentClasses.empty())
		{
			HTREEITEM hParentClasses = m_treeCtrl.InsertItem(L"Parent Classes", IMAGE_COLLECTION, IMAGE_COLLECTION, hClass);

			for (size_t iParentClass = 0; iParentClass < vecParentClasses.size(); iParentClass++)
			{
				auto itParentRDFClass = mapClasses.find(vecParentClasses[iParentClass]);
				assert(itParentRDFClass != mapClasses.end());

				auto pParentRDFClass = itParentRDFClass->second;

				m_treeCtrl.InsertItem(pParentRDFClass->GetName(), IMAGE_CLASS, IMAGE_CLASS, hParentClasses);
			} // for (size_t iParentClass = ...
		} // if (!vecParentClasses.empty())
	} // if (bAddParentClasses)	

	return hClass;
}

// ------------------------------------------------------------------------------------------------
void CClassView::AddProperties(HTREEITEM hParent, int64_t iClassInstance)
{
	assert(iClassInstance != 0);

	wchar_t szBuffer[512];	
	
	auto pModel = GetModel();

	auto& mapClasses = pModel->GetClasses();
	auto& mapProperties = pModel->GetProperties();

	auto itClass = mapClasses.find(iClassInstance);
	assert(itClass != mapClasses.end());

	auto pClass = itClass->second;

	vector<int64_t> vecAncestors;
	pModel->GetClassAncestors(pClass->GetInstance(), vecAncestors);

	vecAncestors.push_back(pClass->GetInstance());

	for (size_t iAncestorClass = 0; iAncestorClass < vecAncestors.size(); iAncestorClass++)
	{
		auto itAncestorRDFClass = mapClasses.find(vecAncestors[iAncestorClass]);
		assert(itAncestorRDFClass != mapClasses.end());

		auto pAncestorRDFClass = itAncestorRDFClass->second;

		const vector<CRDFPropertyRestriction *> & vecPropertyRestrictions = pAncestorRDFClass->getPropertyRestrictions();
		for (size_t iPropertyRestriction = 0; iPropertyRestriction < vecPropertyRestrictions.size(); iPropertyRestriction++)
		{
			auto itProperty = mapProperties.find(vecPropertyRestrictions[iPropertyRestriction]->getPropertyInstance());
			assert(itProperty != mapProperties.end());

			auto pProperty = itProperty->second;

			wstring strNameAndType = pProperty->GetName();
			strNameAndType += L" : ";

			switch (pProperty->GetType())
			{
				case OBJECTPROPERTY_TYPE:
				{
					strNameAndType += L"owl:ObjectProperty";
				}
				break;

				case DATATYPEPROPERTY_TYPE_BOOLEAN:
				{
					strNameAndType += L"owl:DatatypeProperty";
				}
				break;

				case DATATYPEPROPERTY_TYPE_STRING:
				{
					strNameAndType += L"owl:DatatypeProperty";
				}
				break;

				case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
				{
					strNameAndType += L"owl:DatatypeProperty";
				}
				break;

				case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
				{
					strNameAndType += L"owl:DatatypeProperty";
				}
				break;

				case DATATYPEPROPERTY_TYPE_DOUBLE:
				{
					strNameAndType += L"owl:DatatypeProperty";
				}
				break;

				case DATATYPEPROPERTY_TYPE_INTEGER:
				{
					strNameAndType += L"owl:DatatypeProperty";
				}
				break;

				default:
				{
					strNameAndType += L"unknown";
				}
				break;
			} // switch (pProperty->getType())

			HTREEITEM hProperty = m_treeCtrl.InsertItem(strNameAndType.c_str(), IMAGE_PROPERTY, IMAGE_PROPERTY, hParent);

			switch (pProperty->GetType())
			{
				case OBJECTPROPERTY_TYPE:
				{
					HTREEITEM hRange = m_treeCtrl.InsertItem(L"rdfs:range", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);

					CObjectRDFProperty * pObjectRDFProperty = dynamic_cast<CObjectRDFProperty *>(pProperty);
					assert(pObjectRDFProperty != nullptr);

					auto& vecRestrictions = pObjectRDFProperty->GetRestrictions();
					for (size_t iRestriction = 0; iRestriction < vecRestrictions.size(); iRestriction++)
					{
						auto itRestrictionRDFClass = mapClasses.find(vecRestrictions[iRestriction]);
						assert(itRestrictionRDFClass != mapClasses.end());
						assert(itRestrictionRDFClass->second != nullptr);

						m_treeCtrl.InsertItem(itRestrictionRDFClass->second->GetName(), IMAGE_VALUE, IMAGE_VALUE, hRange);
					}
				}
				break;

				case DATATYPEPROPERTY_TYPE_BOOLEAN:
				{
					m_treeCtrl.InsertItem(L"rdfs:range : xsd:boolean", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
				}
				break;

				case DATATYPEPROPERTY_TYPE_STRING:
				{
					m_treeCtrl.InsertItem(L"rdfs:range : xsd:string", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
				}
				break;

				case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
				{
					m_treeCtrl.InsertItem(L"rdfs:range : xsd:string", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
				}
				break;

				case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
				{
					m_treeCtrl.InsertItem(L"rdfs:range : xsd:string", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
				}
				break;

				case DATATYPEPROPERTY_TYPE_DOUBLE:
				{
					m_treeCtrl.InsertItem(L"rdfs:range : xsd:double", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
				}
				break;

				case DATATYPEPROPERTY_TYPE_INTEGER:
				{
					m_treeCtrl.InsertItem(L"rdfs:range : xsd:integer", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
				}
				break;

				default:
				{
					m_treeCtrl.InsertItem(L"rdfs:range : unknown", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
				}
				break;
			} // switch (pProperty->getType())

			if (vecPropertyRestrictions[iPropertyRestriction]->getMaxCard() == -1)
			{
				swprintf(szBuffer, 100, L"owl:cardinality : [%lld - infinity>", vecPropertyRestrictions[iPropertyRestriction]->getMinCard());
			}
			else
			{
				swprintf(szBuffer, 100, L"owl:cardinality : [%lld - %lld]", vecPropertyRestrictions[iPropertyRestriction]->getMinCard(), vecPropertyRestrictions[iPropertyRestriction]->getMaxCard());
			}

			m_treeCtrl.InsertItem(szBuffer, IMAGE_VALUE, IMAGE_VALUE, hProperty);
		} // for (size_t iPropertyRestriction = ...
	} // for (size_t iAncestorClass = ...
}

// ------------------------------------------------------------------------------------------------
void CClassView::AddChildClasses(HTREEITEM hParent, int64_t iClassInstance)
{
	assert(iClassInstance != 0);
	
	auto pModel = GetModel();

	auto& mapClasses = pModel->GetClasses();

	auto itParentRDFClass = mapClasses.find(iClassInstance);
	assert(itParentRDFClass != mapClasses.end());

	auto itClass = mapClasses.begin();
	for (; itClass != mapClasses.end(); itClass++)
	{
		auto pClass = itClass->second;

		const vector<int64_t> & vecParentClasses = pClass->getParentClasses();
		if (vecParentClasses.empty())
		{
			continue;
		}

		if (vecParentClasses[0] == iClassInstance)
		{
			HTREEITEM hClass = AddClass(hParent, pClass->GetInstance(), false);
			AddProperties(hClass, pClass->GetInstance());
			AddChildClasses(hClass, pClass->GetInstance());
		}
	} // for (; itClass != ...
}

// ------------------------------------------------------------------------------------------------
void CClassView::PropertiesAlphabeticalView()
{
	m_treeCtrl.DeleteAllItems();

	auto pModel = GetModel();

	auto& mapProperties = pModel->GetProperties();

	// RDF Property => Name : Instance
	map<wstring, int64_t> mapName2Instance;

	HTREEITEM hRoot = m_treeCtrl.InsertItem(_T("Properties"), IMAGE_MODEL, IMAGE_MODEL);	
	for (auto itProperty = mapProperties.begin(); 
		itProperty != mapProperties.end(); 
		itProperty++)
	{
		auto pProperty = itProperty->second;

		mapName2Instance[pProperty->GetName()] = pProperty->GetInstance();
	} // for (; itProperty != ...
	
	for (auto itName2Instance = mapName2Instance.begin();
		itName2Instance != mapName2Instance.end(); 
		itName2Instance++)
	{
		auto itProperty = mapProperties.find(itName2Instance->second);
		assert(itProperty != mapProperties.end());

		auto pProperty = itProperty->second;

		wstring strNameAndType = pProperty->GetName();
		strNameAndType += L" : ";

		switch (pProperty->GetType())
		{
			case OBJECTPROPERTY_TYPE:
			{
				strNameAndType += L"owl:ObjectProperty";
			}
			break;

			case DATATYPEPROPERTY_TYPE_BOOLEAN:
			{
				strNameAndType += L"owl:DatatypeProperty";
			}
			break;

			case DATATYPEPROPERTY_TYPE_STRING:
			{
				strNameAndType += L"owl:DatatypeProperty";
			}
			break;

			case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
			{
				strNameAndType += L"owl:DatatypeProperty";
			}
			break;

			case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
			{
				strNameAndType += L"owl:DatatypeProperty";
			}
			break;

			case DATATYPEPROPERTY_TYPE_DOUBLE:
			{
				strNameAndType += L"owl:DatatypeProperty";
			}
			break;

			case DATATYPEPROPERTY_TYPE_INTEGER:
			{
				strNameAndType += L"owl:DatatypeProperty";
			}
			break;

			default:
			{
				strNameAndType += L"unknown";
			}
			break;
		} // switch (pProperty->getType())

		HTREEITEM hProperty = m_treeCtrl.InsertItem(strNameAndType.c_str(), IMAGE_PROPERTY, IMAGE_PROPERTY, hRoot);
		switch (pProperty->GetType())
		{
			case OBJECTPROPERTY_TYPE:
			{
				HTREEITEM hRange = m_treeCtrl.InsertItem(L"rdfs:range", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);

				auto pObjectRDFProperty = dynamic_cast<CObjectRDFProperty *>(pProperty);
				assert(pObjectRDFProperty != nullptr);

				auto& vecRestrictions = pObjectRDFProperty->GetRestrictions();
				assert(!vecRestrictions.empty());

				for (size_t iRestriction = 0; iRestriction < vecRestrictions.size(); iRestriction++)
				{
					char * szClassName = nullptr;
					GetNameOfClass(vecRestrictions[iRestriction], &szClassName);

					m_treeCtrl.InsertItem(CA2W(szClassName), IMAGE_VALUE, IMAGE_VALUE, hRange);
				}
			}
			break;

			case DATATYPEPROPERTY_TYPE_BOOLEAN:
			{
				m_treeCtrl.InsertItem(L"rdfs:range : xsd:boolean", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
			}
			break;

			case DATATYPEPROPERTY_TYPE_STRING:
			{
				m_treeCtrl.InsertItem(L"rdfs:range : xsd:string", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
			}
			break;

			case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
			{
				m_treeCtrl.InsertItem(L"rdfs:range : xsd:string", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
			}
			break;

			case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
			{
				m_treeCtrl.InsertItem(L"rdfs:range : xsd:string", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
			}
			break;

			case DATATYPEPROPERTY_TYPE_DOUBLE:
			{
				m_treeCtrl.InsertItem(L"rdfs:range : xsd:double", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
			}
			break;

			case DATATYPEPROPERTY_TYPE_INTEGER:
			{
				m_treeCtrl.InsertItem(L"rdfs:range : xsd:integer", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
			}
			break;

			default:
			{
				m_treeCtrl.InsertItem(L"rdfs:range : unknown", IMAGE_PROPERTY, IMAGE_PROPERTY, hProperty);
			}
			break;
		} // switch (pProperty->getType())
	} // for (; itName2Instance != ...

	m_treeCtrl.Expand(hRoot, TVE_EXPAND);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClassView::CClassView()
	: m_pSearchDialog(nullptr)
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

	assert(GetController() != nullptr);
	GetController()->registerView(this);

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create views:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_treeCtrl.Create(dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("Failed to create Class View\n");
		return -1;      // fail to create
	}

	// Load images:
	m_toolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SORT);
	m_toolBar.LoadToolBar(IDR_SORT, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle();

	m_toolBar.SetPaneStyle(m_toolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_toolBar.SetPaneStyle(m_toolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_toolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_toolBar.SetRouteCommandsViaFrame(FALSE);

	CMenu menuSort;
	menuSort.LoadMenu(IDR_POPUP_SORT);

	m_toolBar.ReplaceButton(ID_SORT_MENU, CClassViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));

	CClassViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CClassViewMenuButton, m_toolBar.GetButton(0));

	if (pButton != nullptr)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
		pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
		pButton->SetMessageWnd(this);
	}

	//  Search
	m_pSearchDialog = new CSearchTreeCtrlDialog(this);
	m_pSearchDialog->Create(IDD_DIALOG_SEARCH, this);

	return 0;
}

void CClassView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CClassView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_treeCtrl;
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
		if (hTreeItem != nullptr)
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
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_toolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_toolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_treeCtrl.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
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

	CClassViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CClassViewMenuButton, m_toolBar.GetButton(0));

	if (pButton != nullptr)
	{
		pButton->SetImage(GetCmdMgr()->GetCmdImage(id));
		m_toolBar.Invalidate();
		m_toolBar.UpdateWindow();
	}

	UpdateView();
}

void CClassView::OnUpdateSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

void CClassView::OnClassAddMemberFunction()
{
	::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"TODO.", L"Error", MB_ICONERROR | MB_OK);
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
	m_treeCtrl.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CClassView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_treeCtrl.SetFocus();
}

void CClassView::OnChangeVisualStyle()
{
	m_images.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASS_VIEW_24 : IDB_CLASS_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		assert(false);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_images.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_images.Add(&bmp, RGB(0, 0, 0));

	m_treeCtrl.SetImageList(&m_images, TVSIL_NORMAL);

	m_toolBar.CleanUpLockedImages();
	m_toolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_24 : IDR_SORT, 0, 0, TRUE /* Locked */);
}


void CClassView::OnDestroy()
{
	assert(GetController() != nullptr);
	GetController()->unRegisterView(this);

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
