
// 3DEditorDoc.cpp : implementation of the CMy3DEditorDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "3DEditor.h"
#endif

#include "3DEditorDoc.h"

#include <propkey.h>

#include "CUniqueVerticesCheck.h"
#include "UniqueVerticesCheckE0.h"

#include "Generic.h"

#ifdef _USE_BOOST
#include <boost/chrono.hpp>

using namespace boost::chrono;
#endif // _USE_BOOST

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CMy3DEditorDoc::LoadModel(LPCTSTR szFileName)
{
	if (szFileName != NULL)
	{
		OnOpenDocument(szFileName);
	}	
	else
	{
		OnNewDocument();
	}
}

// CMy3DEditorDoc

IMPLEMENT_DYNCREATE(CMy3DEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CMy3DEditorDoc, CDocument)
	ON_COMMAND(ID_VIEW_CHECKFORUNIQUEVERTICES, &CMy3DEditorDoc::OnViewCheckForUniqueVertices)
	ON_COMMAND(ID_VIEW_SCALE_AND_CENTER_ALL_GEOMETRY, &CMy3DEditorDoc::OnViewScaleAndCenterAllGeometry)
	ON_COMMAND(ID_FILE_OPEN, &CMy3DEditorDoc::OnFileOpen)
	ON_COMMAND(ID_FILE_IMPORT, &CMy3DEditorDoc::OnFileImport)
	ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT, &CMy3DEditorDoc::OnUpdateFileImport)
	ON_COMMAND(ID_VIEW_ZOOM_OUT, &CMy3DEditorDoc::OnViewZoomOut)
	ON_COMMAND(ID_INSTANCES_ZOOM_TO, &CMy3DEditorDoc::OnInstancesZoomTo)
	ON_UPDATE_COMMAND_UI(ID_INSTANCES_ZOOM_TO, &CMy3DEditorDoc::OnUpdateInstancesZoomTo)
	ON_COMMAND(ID_INSTANCES_SAVE, &CMy3DEditorDoc::OnInstancesSave)
	ON_UPDATE_COMMAND_UI(ID_INSTANCES_SAVE, &CMy3DEditorDoc::OnUpdateInstancesSave)
END_MESSAGE_MAP()


// CMy3DEditorDoc construction/destruction

CMy3DEditorDoc::CMy3DEditorDoc()
	: m_pModel(NULL)
{
}

CMy3DEditorDoc::~CMy3DEditorDoc()
{
	delete m_pModel;
}

BOOL CMy3DEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	if (m_pModel != NULL)
	{
		delete m_pModel;
		m_pModel = NULL;
	}

	m_pModel = new CRDFModel();
	m_pModel->CreateDefaultModel();

	SetModel(m_pModel);

	return TRUE;
}

// CMy3DEditorDoc serialization

void CMy3DEditorDoc::Serialize(CArchive& ar)
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
void CMy3DEditorDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
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
void CMy3DEditorDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CMy3DEditorDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMy3DEditorDoc diagnostics

#ifdef _DEBUG
void CMy3DEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMy3DEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMy3DEditorDoc commands


BOOL CMy3DEditorDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	if (m_pModel != NULL)
	{
		delete m_pModel;
		m_pModel = NULL;
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

BOOL CMy3DEditorDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	m_pModel->Save(lpszPathName);

	return TRUE;
}

void CMy3DEditorDoc::OnViewCheckForUniqueVertices()
{	
	TCHAR szFilters[] = _T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*||");

	CFileDialog dlgFile(FALSE, _T("txt"), _T(" Report - Check for unique vertices"),
		OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters);

	if (dlgFile.DoModal() != IDOK)
	{
		return;
	}

#ifdef _USE_BOOST
	high_resolution_clock::time_point tpStart = high_resolution_clock::now();
#endif

	// Binning algorithm
	//CUniqueVerticesCheck * pUniqueVerticesCheck = new CUniqueVerticesCheck();

	// Exact match - Epsilon = 0
	CUniqueVerticesCheckE0 * pUniqueVerticesCheckE0 = new CUniqueVerticesCheckE0();

	CString strReportFile = dlgFile.GetPathName();

	wofstream output((LPCTSTR)strReportFile);
	output << "****************************************************************************************************\n";
	output << "*** Unique vertices check ***\n";
	output << "****************************************************************************************************\n\n";

	auto& mapRDFInstances = m_pModel->GetRDFInstances();

	map<int64_t, CRDFInstance *>::const_iterator itRDFInstances = mapRDFInstances.begin();
	for (; itRDFInstances != mapRDFInstances.end(); itRDFInstances++)
	{
		CRDFInstance * pRDFInstance = itRDFInstances->second;
		if (!pRDFInstance->hasGeometry())
		{
			continue;
		}

		// Binning algorithm
		/*int32_t iDuplicatesCount1 =  pUniqueVerticesCheck->Check(pRDFInstance, output);*/

		// Exact match - Epsilon = 0
		/*int32_t iDuplicatesCount2 = */pUniqueVerticesCheckE0->Check(pRDFInstance, VERTEX_LENGTH, output);

		/*if (iDuplicatesCount2 > 0)
		{
			tuple<float *, int64_t, int32_t *> tpUniqueVertices = pUniqueVerticesCheck->RemoveDuplicates(pRDFInstance, VERTEX_LENGTH);
			pRDFInstance->UpdateVertices(get<0>(tpUniqueVertices), get<1>(tpUniqueVertices), get<2>(tpUniqueVertices));
		}*/

		/*if (iDuplicatesCount1 != iDuplicatesCount2)
		{
			output << "ERROR!\n";
		}*/
	} // for (; itRDFInstances != ...

#ifdef _USE_BOOST
	high_resolution_clock::time_point tpEnd = high_resolution_clock::now();
	duration<float> duration = tpEnd - tpStart;

	output << "\n****************************************************************************************************\n";	
	output << "Unique vertices check is done in " << duration << "." << "\n";
	output << "****************************************************************************************************\n";
#endif

	output.close();

	//delete pUniqueVerticesCheck;
	delete pUniqueVerticesCheckE0;
}

void CMy3DEditorDoc::OnViewScaleAndCenterAllGeometry()
{
	ScaleAndCenter();
}

void CMy3DEditorDoc::OnFileOpen()
{
	CFileDialog dlgFile(TRUE, nullptr, _T(""), OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, SUPPORTED_FILES);
	if (dlgFile.DoModal() != IDOK)
	{
		return;
	}

	OnOpenDocument(dlgFile.GetPathName());
}

void CMy3DEditorDoc::OnFileImport()
{
	CFileDialog dlgFile(TRUE, nullptr, _T(""), OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, SUPPORTED_FILES);

	if (dlgFile.DoModal() != IDOK)
	{
		return;
	}

	ImportModel(nullptr, dlgFile.GetPathName().GetString());
}

void CMy3DEditorDoc::OnUpdateFileImport(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

void CMy3DEditorDoc::OnViewZoomOut()
{
	ZoomOut();
}

void CMy3DEditorDoc::OnInstancesZoomTo()
{
	auto pInstance = GetSelectedInstance();
	ASSERT((pInstance != NULL) && pInstance->getEnable() && pInstance->hasGeometry());

	ZoomToInstance(pInstance->getInstance());
}

void CMy3DEditorDoc::OnUpdateInstancesZoomTo(CCmdUI* pCmdUI)
{
	auto pInstance = GetSelectedInstance();
	pCmdUI->Enable((pInstance != NULL) && pInstance->getEnable() && pInstance->hasGeometry());
}

void CMy3DEditorDoc::OnInstancesSave()
{
	ASSERT(GetSelectedInstance() != NULL);
	Save(GetSelectedInstance());
}

void CMy3DEditorDoc::OnUpdateInstancesSave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetSelectedInstance() != NULL);
}
