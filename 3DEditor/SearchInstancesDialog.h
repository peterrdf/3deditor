#pragma once

#include "ViewTree.h"

// CSearchInstancesDialog dialog

class CSearchInstancesDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CSearchInstancesDialog)

private: // Declarations

	enum class enumSearchFilter : int {
		All = 0,
		Instances = 1
	};

private: // Members

	// --------------------------------------------------------------------------------------------
	// IFC Tree
	CViewTree * m_pIFCTreeCtrl;

	// --------------------------------------------------------------------------------------------
	// Filter
	enumSearchFilter m_enSearchFilter;

	// --------------------------------------------------------------------------------------------
	// Last found HTREEITEM
	HTREEITEM m_hSearchResult;

	// --------------------------------------------------------------------------------------------
	// No more results
	BOOL m_bEndOfSearch;

private: // Methods

	// --------------------------------------------------------------------------------------------
	// Search
	BOOL ContainsText(HTREEITEM hItem, const CString& strText);

	// --------------------------------------------------------------------------------------------
	// Select
	void SelectItem(HTREEITEM hItem);

	// --------------------------------------------------------------------------------------------
	// Select
	void UnselectItem(HTREEITEM hItem);

	// --------------------------------------------------------------------------------------------
	// Search the item's children
	HTREEITEM SearchChildren(HTREEITEM hParent);

	// --------------------------------------------------------------------------------------------
	// Search the item's siblings
	HTREEITEM SearchSiblings(HTREEITEM hItem);

	// --------------------------------------------------------------------------------------------
	// Search the item's parents
	HTREEITEM SearchParents(HTREEITEM hItem);

public: // Methods

	// --------------------------------------------------------------------------------------------
	// Resets internal state
	void Reset();

public:
	CSearchInstancesDialog(CViewTree* pIFCTreeCtrl);   // standard constructor
	virtual ~CSearchInstancesDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SEARCH_INSTANCES
	};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strSearchText;
	afx_msg void OnEnChangeEditSearchText();
	CButton m_btnSearch;
	afx_msg void OnBnClickedButtonSearch();
	CComboBox m_cmbSearchFilter;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboSearchFilter();
};
