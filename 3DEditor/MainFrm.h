
// MainFrm.h : interface of the CMainFrame class
//

#pragma once

#include "RDFController.h"
#include "DesignTreeView.h"
#include "ClassView.h"
#include "PropertiesWnd.h"

// ------------------------------------------------------------------------------------------------
class CMainFrame : public CFrameWndEx
{
	// --------------------------------------------------------------------------------------------
	friend class CTestLayout;
	
private: // Methods

	// --------------------------------------------------------------------------------------------
	CDocument* GetDocument() const;

	// --------------------------------------------------------------------------------------------
	CView* GetView() const;

	// --------------------------------------------------------------------------------------------
	// Controller - MVC
	CRDFController * GetController() const;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// Tests
	void GenerateTests(const CString& strInputDataDir, const CString& strWildcard);


protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = nullptr, CCreateContext* pContext = nullptr);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCMenuBar       m_menuBar;
	CMFCToolBar       m_toolBar;
	CMFCStatusBar     m_statusBar;
	CMFCToolBarImages m_userImages;
	CDesignTreeView   m_designTreeView;
	CClassView        m_classView;
	CPropertiesWnd    m_propertiesView;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnViewFileView();
	afx_msg void OnUpdateViewFileView(CCmdUI* pCmdUI);
	afx_msg void OnViewClassView();
	afx_msg void OnUpdateViewClassView(CCmdUI* pCmdUI);
	afx_msg void OnViewPropertiesWindow();
	afx_msg void OnUpdateViewPropertiesWindow(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:

};

// ------------------------------------------------------------------------------------------------
class CTestLayout
{

private: // Fields

	// --------------------------------------------------------------------------------------------
	CMainFrame* m_pMainFrame;

	// --------------------------------------------------------------------------------------------
	CView* m_pView;

	// --------------------------------------------------------------------------------------------
	BOOL m_bInstancesPaneVisisble;
	BOOL m_bClassPaneVisisble;
	BOOL m_bPropertiesPaneVisisble;

	// --------------------------------------------------------------------------------------------
	CRect m_rcView;

public: // Methods

	// --------------------------------------------------------------------------------------------
	CTestLayout(CMainFrame* pMainFrame, CView* pView)
		: m_pMainFrame(pMainFrame)
		, m_pView(pView)
	{
		assert(m_pMainFrame != nullptr);
		assert(m_pView != nullptr);
	}

	// --------------------------------------------------------------------------------------------
	void Set()
	{
		m_bInstancesPaneVisisble = m_pMainFrame->m_designTreeView.IsVisible();
		m_bClassPaneVisisble = m_pMainFrame->m_classView.IsVisible();
		m_bPropertiesPaneVisisble = m_pMainFrame->m_classView.IsVisible();

		m_pMainFrame->m_designTreeView.ShowPane(FALSE, FALSE, FALSE);
		m_pMainFrame->m_classView.ShowPane(FALSE, FALSE, FALSE);
		m_pMainFrame->m_propertiesView.ShowPane(FALSE, FALSE, FALSE);

		m_pView->GetWindowRect(&m_rcView);

		m_pView->SetWindowPos(nullptr, 0, 0, 1024, 1024, SWP_NOMOVE);
		m_pView->RedrawWindow();
	}

	// --------------------------------------------------------------------------------------------
	void Restore()
	{
		m_pView->SetWindowPos(nullptr, 0, 0, m_rcView.Width(), m_rcView.Height(), SWP_NOMOVE);
		m_pView->RedrawWindow();

		if (m_bInstancesPaneVisisble)
		{
			m_pMainFrame->m_designTreeView.ShowPane(TRUE, FALSE, TRUE);
		}

		if (m_bClassPaneVisisble)
		{
			m_pMainFrame->m_classView.ShowPane(TRUE, FALSE, TRUE);
		}

		if (m_bPropertiesPaneVisisble)
		{
			m_pMainFrame->m_propertiesView.ShowPane(TRUE, FALSE, TRUE);
		}
	}
};