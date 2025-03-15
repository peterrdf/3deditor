
#pragma once

#include "TreeCtrlEx.h"
#include "RDFModel.h"
#include "RDFItem.h"
#include "SearchTreeCtrlDialog.h"

#include "_rdf_instance.h"
#include "_rdf_mvc.h"

#include <map>
using namespace std;

// ************************************************************************************************
class CDesignTreeViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

// ************************************************************************************************
class CDesignTreeView
	: public CDockablePane
	, public _rdf_view
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
	_rdf_instance* m_pSelectedInstance;
	HTREEITEM m_hSelectedInstance;

	// UI
	bool m_bUpdateInProgress; // Don't send notifications while updating the view

	// Search
	CSearchTreeCtrlDialog* m_pSearchDialog;

public: // Methods

	// _view
	virtual void onModelLoaded() override;
	virtual void onInstanceSelected(_view* pSender) override;
	virtual void onApplicationPropertyChanged(_view* pSender, enumApplicationProperty enApplicationProperty) override;
	
	// _rdf_view
	virtual void onInstanceCreated(_view* pSender, _rdf_instance* pInstance) override;	
	virtual void onInstanceDeleted(_view* pSender, _rdf_instance* pInstance) override;
	virtual void onInstancesDeleted(_view* pSender) override;
	virtual void onMeasurementsAdded(_view* pSender, _rdf_instance* pInstance) override;
	virtual void onInstancePropertyEdited(_view* pSender, _rdf_instance* pInstance, _rdf_property* pProperty) override;

	// CItemStateProvider
	virtual bool IsSelected(HTREEITEM hItem) override;

	// CSearchTreeCtrlDialogSite
	virtual CTreeCtrlEx* GetTreeView() override;
	virtual vector<CString> GetSearchFilters() override;
	virtual void LoadChildrenIfNeeded(HTREEITEM hItem) override;
	virtual BOOL ContainsText(int iFilter, HTREEITEM hItem, const CString& strText) override;

private: // Methods

	CRDFModel* GetModel() const;

	void SelectInstance(_rdf_instance* pInstance, BOOL bSelectTreeItem);
	
	void GetAscendants(HTREEITEM hItem, vector<HTREEITEM>& vecAscendants);
	void GetDescendants(HTREEITEM hItem, vector<HTREEITEM>& vecDescendants);
	void RemoveInstanceItemData(_rdf_instance* pInstance, HTREEITEM hInstance);
	void RemovePropertyItemData(_rdf_instance* pInstance, _rdf_property * pProperty, HTREEITEM hProperty);
	void RemoveItemData(HTREEITEM hItem);
	void UpdateView();

	void InstancesAlphabeticalView();
	void InstancesGroupByClassView();
	void InstancesUnreferencedItemsView();
	
	void AddInstance(HTREEITEM hParent, _rdf_instance* pInstance);
	void AddProperties(HTREEITEM hParent, _rdf_instance* pInstance);

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

