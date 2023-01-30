
#pragma once

#include "ViewTree.h"
#include "RDFView.h"
#include "RDFInstance.h"
#include "RDFInstanceItem.h"
#include "RDFPropertyItem.h"
#include "SearchInstancesDialog.h"

#include <map>

using namespace std;

class CFileViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CFileView
	: public CDockablePane
	, public CRDFView
	, CItemStateProvider
{

private: // Members

	// --------------------------------------------------------------------------------------------
	// INSTANCE : CRDFInstanceItem *
	map<int64_t, CRDFInstanceItem *> m_mapInstance2Item;

	// --------------------------------------------------------------------------------------------
	// INSTANCE : (PROPERTY INSTANCE : CRDFPropertyItem *)
	map<int64_t, map<int64_t, CRDFPropertyItem *> > m_mapInstance2Properties;

	// --------------------------------------------------------------------------------------------
	// Selected HTREEITEM
	HTREEITEM m_hSelectedItem;

	// --------------------------------------------------------------------------------------------
	// Don't send notifications while updating the view
	bool m_bInitInProgress;

	// --------------------------------------------------------------------------------------------
	// Search
	CSearchInstancesDialog* m_pSearchDialog;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnModelChanged();

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnInstanceSelected(CRDFView * pSender);

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnInstancePropertyEdited(CRDFInstance * pInstance, CRDFProperty * pProperty);

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnNewInstanceCreated(CRDFView * pSender, CRDFInstance * pInstance);

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnInstanceDeleted(CRDFView * pSender, int64_t iInstance);

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnMeasurementsAdded(CRDFView * pSender, CRDFInstance * pInstance);

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnVisibleValuesCountLimitChanged();

	// --------------------------------------------------------------------------------------------
	// CItemStateProvider
	virtual bool IsSelected(HTREEITEM hItem);

private: // Methods

	// --------------------------------------------------------------------------------------------
	// Full path of an item
	void GetItemPath(HTREEITEM hItem, vector<pair<CRDFInstance *, CRDFProperty *> > & vecPath);

	// --------------------------------------------------------------------------------------------
	// All descendants
	void GetDescendants(HTREEITEM hItem, vector<HTREEITEM> & vecDescendants);

	// --------------------------------------------------------------------------------------------
	// Update the data structures
	void RemoveInstanceItemData(CRDFInstance * pInstance, HTREEITEM hInstance);

	// --------------------------------------------------------------------------------------------
	// Update the data structures
	void RemovePropertyItemData(CRDFInstance * pInstance, CRDFProperty * pProperty, HTREEITEM hProperty);

	// --------------------------------------------------------------------------------------------
	// Update the data structures
	void RemoveItemData(HTREEITEM hItem);

	// --------------------------------------------------------------------------------------------
	// Updates the tree
	void UpdateView();

	// --------------------------------------------------------------------------------------------
	// Alphabetical View
	void InstancesAlphabeticalView();

	// --------------------------------------------------------------------------------------------
	// Group by Class View
	void InstancesGroupByClassView();

	// --------------------------------------------------------------------------------------------
	// Group by Class View
	void InstancesUnreferencedItemsView();

	// --------------------------------------------------------------------------------------------
	// RDF Instance
	void AddInstance(HTREEITEM hParent, CRDFInstance * pInstance);

	// --------------------------------------------------------------------------------------------
	// RDF Properties
	void AddProperties(HTREEITEM hParent, CRDFInstance * pInstance);

	// --------------------------------------------------------------------------------------------
	// Refreshing of Unreferenced items view
	void UpdateRootItemsUnreferencedItemsView(int64_t iModel, HTREEITEM hModel);

// Construction
public:
	CFileView();

	void AdjustLayout();
	void OnChangeVisualStyle();

// Attributes
protected:

	CViewTree m_wndFileView;
	CImageList m_FileViewImages;
	CFileViewToolBar m_wndToolBar;
	UINT m_nCurrSort;

protected:

// Implementation
public:
	virtual ~CFileView();

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

