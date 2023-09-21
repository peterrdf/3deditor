
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
	virtual bool IsSelected(HTREEITEM hItem) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx window

class CTreeCtrlEx : public CTreeCtrl
{

private: // Members
	
	CItemStateProvider* m_pItemStateProvider;

public: // Methods

	void SetItemStateProvider(CItemStateProvider* pItemStateProvider);

// Construction
public:
	CTreeCtrlEx();

// Overrides
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// Implementation
public:
	virtual ~CTreeCtrlEx();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};
