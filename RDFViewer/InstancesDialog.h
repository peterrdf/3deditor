#pragma once

#include "Generic.h"
#include "InstancesList.h"

#include "_rdf_mvc.h"

#include <set>
using namespace std;

// CInstancesDialog dialog

class CInstancesDialog
	: public CDialogEx
	, public _rdf_view
{

private: // Members

	CInstancesList m_lcInstances;
	bool m_bUpdateInProgress;
	map<_rdf_instance*, int> m_mapInstance2Item;

protected: // Methods

	// _veiw
	virtual void onModelLoaded() override;
	virtual void onInstanceSelected(_view* pSender) override;
	virtual void onControllerChanged() override;

	// _rdf_view
	virtual void onInstanceDeleted(_view* pSender, _rdf_instance* pInstance) override;
	virtual void onInstancesDeleted(_view* pSender) override;

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
