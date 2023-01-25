
// RDFViewerDoc.h : interface of the CRDFViewerDoc class
//


#pragma once

#include "RDFModel.h"
#include "RDFController.h"

// ------------------------------------------------------------------------------------------------
class CRDFViewerDoc
	: public CDocument
	, public CRDFController
{

private: // Members

	// --------------------------------------------------------------------------------------------
	// Model (MVC)
	CRDFModel * m_pModel;

protected: // create from serialization only
	CRDFViewerDoc();
	DECLARE_DYNCREATE(CRDFViewerDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CRDFViewerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	afx_msg void OnFileOpen();
};
