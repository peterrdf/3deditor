
#pragma once

// ------------------------------------------------------------------------------------------------
class CRDFController;

// ------------------------------------------------------------------------------------------------
class CInstancesList : public CListCtrl
{

private: // Members

	// --------------------------------------------------------------------------------------------
	CRDFController* m_pController;

public: // Methods

	// --------------------------------------------------------------------------------------------
	void SetController(CRDFController* pController);

// Construction
public:
	CInstancesList();

// Overrides
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// Implementation
public:
	virtual ~CInstancesList();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
};
