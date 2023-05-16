#pragma once

#include "Generic.h"
#include "RDFView.h"
#include "InstancesList.h"

#include <set>

using namespace std;

// CInstancesDialog dialog

class CInstancesDialog
	: public CDialogEx
	, public CRDFView
{

private: // Members

	// --------------------------------------------------------------------------------------------
	// Instances
	CInstancesList m_lcInstances;

	// --------------------------------------------------------------------------------------------
	// Don't send notifications while updating the view
	bool m_bUpdateInProgress;

	// --------------------------------------------------------------------------------------------
	// Find an item by CRDFInstance *
	map<CRDFInstance *, int> m_mapInstance2Item;

protected: // Methods

	// --------------------------------------------------------------------------------------------
	// Overridden
	virtual void OnControllerChanged();

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnModelChanged();

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnInstanceSelected(CRDFView * pSender);

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnInstanceDeleted(CRDFView * pSender, int64_t iInstance);

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnInstancesDeleted(CRDFView * pSender);

	DECLARE_DYNAMIC(CInstancesDialog)

public:
	CInstancesDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CInstancesDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_INSTANCES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSelectedInstanceChanged(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
};
