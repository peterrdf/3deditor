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

	CInstancesList m_lcInstances;
	bool m_bUpdateInProgress;
	map<_rdf_instance*, int> m_mapInstance2Item;

protected: // Methods

	// _veiw
	virtual void onInstanceSelected(_view* pSender) override;

	// _rdf_view
	virtual void onInstanceDeleted(_view* pSender, _rdf_instance* pInstance) override;
	virtual void onInstancesDeleted(_view* pSender) override;

	// --------------------------------------------------------------------------------------------
	// Overridden
	virtual void OnControllerChanged();//#todo

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnModelChanged();//#todo

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
