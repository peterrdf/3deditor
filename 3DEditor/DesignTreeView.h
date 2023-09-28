
#pragma once

#include "TreeCtrlEx.h"
#include "RDFView.h"
#include "RDFInstance.h"
#include "RDFInstanceItem.h"
#include "RDFPropertyItem.h"
#include "SearchTreeCtrlDialog.h"

#include <map>
using namespace std;

class CDesignTreeViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CDesignTreeView
	: public CDockablePane
	, public CRDFView
	, public CItemStateProvider
	, public CSearchTreeCtrlDialogSite
{

	enum class enumSearchFilter : int {
		All = 0,
		Instances,
		Properties,
		Values,
	};

private: // Members

	// Cache
	map<int64_t, CRDFInstanceItem*> m_mapInstance2Item; // INSTANCE : CRDFInstanceItem*	
	map<int64_t, map<int64_t, CRDFPropertyItem*>> m_mapInstance2Properties; // INSTANCE : (PROPERTY INSTANCE : CRDFPropertyItem*)

	// Selection
	CRDFInstance* m_pSelectedInstance;
	HTREEITEM m_hSelectedInstance;

	// UI
	bool m_bUpdateInProgress; // Don't send notifications while updating the view

	// Search
	CSearchTreeCtrlDialog* m_pSearchDialog;

public: // Methods
	
	// CRDFView
	virtual void OnModelChanged() override;
	virtual void OnInstanceSelected(CRDFView* pSender) override;
	virtual void OnInstancePropertyEdited(CRDFInstance* pInstance, CRDFProperty* pProperty) override;
	virtual void OnNewInstanceCreated(CRDFView* pSender, CRDFInstance* pInstance) override;
	virtual void OnInstanceDeleted(CRDFView* pSender, int64_t iInstance) override;
	virtual void OnInstancesDeleted(CRDFView* pSender);
	virtual void OnMeasurementsAdded(CRDFView* pSender, CRDFInstance* pInstance) override;
	virtual void OnApplicationPropertyChanged(CRDFView* pSender, enumApplicationProperty enApplicationProperty) override;
	
	// CItemStateProvider
	virtual bool IsSelected(HTREEITEM hItem) override;

	// CSearchTreeCtrlDialogSite
	virtual CTreeCtrlEx* GetTreeView() override;
	virtual vector<CString> GetSearchFilters() override;
	virtual void LoadChildrenIfNeeded(HTREEITEM hItem) override;
	virtual BOOL ContainsText(int iFilter, HTREEITEM hItem, const CString& strText) override;

private: // Methods

	CRDFModel* GetModel() const;

	void SelectInstance(CRDFInstance* pInstance, BOOL bSelectTreeItem);
	
	void GetItemPath(HTREEITEM hItem, vector<pair<CRDFInstance*, CRDFProperty*>>& vecPath);
	void GetDescendants(HTREEITEM hItem, vector<HTREEITEM> & vecDescendants);
	void RemoveInstanceItemData(CRDFInstance* pInstance, HTREEITEM hInstance);
	void RemovePropertyItemData(CRDFInstance* pInstance, CRDFProperty * pProperty, HTREEITEM hProperty);
	void RemoveItemData(HTREEITEM hItem);
	void UpdateView();

	void InstancesAlphabeticalView();
	void InstancesGroupByClassView();
	void InstancesUnreferencedItemsView();
	
	void AddInstance(HTREEITEM hParent, CRDFInstance* pInstance);
	void AddProperties(HTREEITEM hParent, CRDFInstance* pInstance);

	void UpdateRootItemsUnreferencedItemsView(int64_t iModel, HTREEITEM hModel);

	void Clean();

	void GetAncestors(OwlInstance iInstance, vector<int64_t>& vecAncestors);

// Construction
public:
	CDesignTreeView();

	void AdjustLayout();
	void OnChangeVisualStyle();

// Attributes
protected:

	CTreeCtrlEx m_treeCtrl;
	CImageList m_images;
	CDesignTreeViewToolBar m_toolBar;
	UINT m_nCurrSort;

protected:

// Implementation
public:
	virtual ~CDesignTreeView();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProperties();
	afx_msg void OnFileOpen();
	afx_msg void OnFileOpenWith();
	afx_msg void OnDummyCompile();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSort(UINT id);
	afx_msg void OnUpdateSort(CCmdUI* pCmdUI);
	afx_msg void OnSelectedItemChanged(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnItemExpanding(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnNewInstance();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

