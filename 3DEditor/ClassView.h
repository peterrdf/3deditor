
#pragma once

#include "TreeCtrlEx.h"
#include "RDFView.h"
#include "SearchTreeCtrlDialog.h"

class CClassToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CClassView
	: public CDockablePane
	, public CRDFView
	, public CSearchTreeCtrlDialogSite
{

private: // Classes

	enum class enumSearchFilter {
		All = 0,
		Classes = 1,
		Properties = 2,
		Values = 3,
	};

private: // Members

	CSearchTreeCtrlDialog* m_pSearchDialog;

public: // Methods

	// CRDFView
	virtual void OnModelChanged() override;

	// CSearchTreeCtrlDialogSite
	virtual CTreeCtrlEx* GetTreeView() override;
	virtual vector<CString> GetSearchFilters() override;
	virtual void LoadChildrenIfNeeded(HTREEITEM /*hItem*/) override {};
	virtual BOOL ContainsText(int iFilter, HTREEITEM hItem, const CString& strText) override;

private: // Methods

	CRDFModel* GetModel() const;

	// --------------------------------------------------------------------------------------------
	// Reloads the tree
	void UpdateView();

	// --------------------------------------------------------------------------------------------
	// Alphabetical View   
	void ClassesAlphabeticalView();

	// --------------------------------------------------------------------------------------------
	// Hierarchical View   
	void ClassesHierarchicalView();

	// --------------------------------------------------------------------------------------------
	// Helper
	HTREEITEM AddClass(HTREEITEM hParent, int64_t iClassInstance, bool bAddParentClasses);

	// --------------------------------------------------------------------------------------------
	// Helper
	void AddProperties(HTREEITEM hParent, int64_t iClassInstance);

	// --------------------------------------------------------------------------------------------
	// Helper
	void AddChildClasses(HTREEITEM hParent, int64_t iClassInstance);

	// --------------------------------------------------------------------------------------------
	// Alphabetical View   
	void PropertiesAlphabeticalView();

public:
	CClassView();
	virtual ~CClassView();

	void AdjustLayout();
	void OnChangeVisualStyle();

protected:
	CClassToolBar m_toolBar;
	CTreeCtrlEx m_treeCtrl;
	CImageList m_images;
	UINT m_nCurrSort;

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnClassAddMemberFunction();
	afx_msg void OnClassAddMemberVariable();
	afx_msg void OnClassDefinition();
	afx_msg void OnClassProperties();
	afx_msg void OnNewFolder();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnChangeActiveTab(WPARAM, LPARAM);
	afx_msg void OnSort(UINT id);
	afx_msg void OnUpdateSort(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

