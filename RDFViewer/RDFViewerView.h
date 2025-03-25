
// RDFViewerView.h : interface of the CRDFViewerView class
//

#pragma once

#include "RDFOpenGLView.h"
#include "InstancesDialog.h"

class CRDFViewerView 
	: public CView
	, public _rdf_view
{

private: // Fields

	CRDFOpenGLView* m_pOpenGLView;
	CInstancesDialog * m_pInstancesDialog;

public: // Methods

	// _view
	virtual void onModelLoaded() override;
	virtual void onModelUpdated() override;

	CRDFController* GetController();

protected: // create from serialization only
	CRDFViewerView();
	DECLARE_DYNCREATE(CRDFViewerView)

// Attributes
public:
	CRDFViewerDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CRDFViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnViewInstances();
	afx_msg void OnUpdateViewInstances(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // debug version in RDFViewerView.cpp
inline CRDFViewerDoc* CRDFViewerView::GetDocument() const
   { return reinterpret_cast<CRDFViewerDoc*>(m_pDocument); }
#endif

