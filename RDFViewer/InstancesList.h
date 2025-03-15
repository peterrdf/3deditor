
#pragma once

// ************************************************************************************************
class _rdf_controller;

// ************************************************************************************************
class CInstancesList : public CListCtrl
{

private: // Members

	_rdf_controller* m_pController;

public: // Methods

	void SetController(_rdf_controller* pController);

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
