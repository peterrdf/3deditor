
// RDFViewerDoc.cpp : implementation of the CRDFViewerDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "RDFViewer.h"
#endif

#include "RDFViewerDoc.h"
#include "Generic.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRDFViewerDoc

IMPLEMENT_DYNCREATE(CRDFViewerDoc, CDocument)

BEGIN_MESSAGE_MAP(CRDFViewerDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, &CRDFViewerDoc::OnFileOpen)
END_MESSAGE_MAP()


// CRDFViewerDoc construction/destruction

CRDFViewerDoc::CRDFViewerDoc()
	: m_pModel(nullptr)
{
}

CRDFViewerDoc::~CRDFViewerDoc()
{
	delete m_pModel;
	
}

BOOL CRDFViewerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	if (m_pModel != nullptr)
	{
		delete m_pModel;
		m_pModel = nullptr;
	}

	m_pModel = new CRDFModel();
	m_pModel->CreateDefaultModel();	

	SetModel(m_pModel);

	return TRUE;
}

// CRDFViewerDoc serialization

void CRDFViewerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CRDFViewerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CRDFViewerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CRDFViewerDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CRDFViewerDoc diagnostics

#ifdef _DEBUG
void CRDFViewerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRDFViewerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CRDFViewerDoc commands

BOOL CRDFViewerDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	if (m_pModel != nullptr)
	{
		delete m_pModel;
		m_pModel = nullptr;
	}

	m_pModel = new CRDFModel();
	m_pModel->Load(lpszPathName);	

	SetModel(m_pModel);

	m_pModel->PostLoad();

	// Title
	CString strTitle = AfxGetAppName();
	strTitle += L" - ";
	strTitle += lpszPathName;

	AfxGetMainWnd()->SetWindowTextW(strTitle);

	// MRU
	AfxGetApp()->AddToRecentFileList(lpszPathName);

	return TRUE;
}

BOOL CRDFViewerDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	m_pModel->Save(lpszPathName);

	return TRUE;
}

void CRDFViewerDoc::OnFileOpen()
{
	CFileDialog dlgFile(TRUE, nullptr, _T(""), OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, SUPPORTED_FILES);
	if (dlgFile.DoModal() != IDOK)
	{
		return;
	}

	OnOpenDocument(dlgFile.GetPathName());
}
