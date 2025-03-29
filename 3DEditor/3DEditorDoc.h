
// 3DEditorDoc.h : interface of the CMy3DEditorDoc class
//


#pragma once

#include "RDFModel.h"
#include "RDFController.h"

#include "_instance.h"

// ************************************************************************************************
class CMy3DEditorDoc
	: public CDocument
	, public CRDFController
{

public: // Methods

	// CRDFController
	virtual void _test_LoadModel(LPCTSTR szFileName) override;

protected: // create from serialization only
	CMy3DEditorDoc();
	DECLARE_DYNCREATE(CMy3DEditorDoc)

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
	virtual ~CMy3DEditorDoc();
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
	afx_msg void OnViewScaleAndCenterAllGeometry();
	afx_msg void OnFileOpen();
	afx_msg void OnFileImport();
	afx_msg void OnUpdateFileImport(CCmdUI* pCmdUI);
	afx_msg void OnViewZoomOut();
	afx_msg void OnInstancesZoomTo();
	afx_msg void OnUpdateInstancesZoomTo(CCmdUI* pCmdUI);
	afx_msg void OnInstancesSave();
	afx_msg void OnUpdateInstancesSave(CCmdUI* pCmdUI);
	afx_msg void OnExportAsCitygml();
	afx_msg void OnUpdateExportAsCitygml(CCmdUI* pCmdUI);
	afx_msg void OnExportAsInfragml();
	afx_msg void OnUpdateExportAsInfragml(CCmdUI* pCmdUI);
	afx_msg void OnExportAsLandxml();
	afx_msg void OnUpdateExportAsLandxml(CCmdUI* pCmdUI);
	afx_msg void OnExportAsGltf();
	afx_msg void OnUpdateExportAsGltf(CCmdUI* pCmdUI);
};
