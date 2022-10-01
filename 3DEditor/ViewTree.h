
#pragma once

// ------------------------------------------------------------------------------------------------
class CItemStateProvider
{

public: // Methods

	// --------------------------------------------------------------------------------------------
	CItemStateProvider()
	{
	}

	// --------------------------------------------------------------------------------------------
	virtual bool IsSelected(HTREEITEM hItem) = 0;
};

/////////////////////////////////////////////////////////////////////////////
// CViewTree window

class CViewTree : public CTreeCtrl
{

private: // Members

	// --------------------------------------------------------------------------------------------
	CItemStateProvider* m_pItemStateProvider;

public: // Methods

	// --------------------------------------------------------------------------------------------
	void SetItemStateProvider(CItemStateProvider* pItemStateProvider);

// Construction
public:
	CViewTree();

// Overrides
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// Implementation
public:
	virtual ~CViewTree();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
};
