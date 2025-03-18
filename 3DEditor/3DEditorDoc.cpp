
// 3DEditorDoc.cpp : implementation of the CMy3DEditorDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "3DEditor.h"
#endif

#include "3DEditorDoc.h"
#include "Generic.h"
#ifdef _GIS_SUPPORT
#include "gisengine.h"
#endif

#include <propkey.h>

#include "_ptr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*virtual*/ void CMy3DEditorDoc::_test_LoadModel(LPCTSTR szFileName) /*override*/
{
	if (szFileName != nullptr) {
		OnOpenDocument(szFileName);
	} else {
		OnNewDocument();
	}
}

// CMy3DEditorDoc

IMPLEMENT_DYNCREATE(CMy3DEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CMy3DEditorDoc, CDocument)
	ON_COMMAND(ID_VIEW_SCALE_AND_CENTER_ALL_GEOMETRY, &CMy3DEditorDoc::OnViewScaleAndCenterAllGeometry)
	ON_COMMAND(ID_FILE_OPEN, &CMy3DEditorDoc::OnFileOpen)
	ON_COMMAND(ID_FILE_IMPORT, &CMy3DEditorDoc::OnFileImport)
	ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT, &CMy3DEditorDoc::OnUpdateFileImport)
	ON_COMMAND(ID_VIEW_ZOOM_OUT, &CMy3DEditorDoc::OnViewZoomOut)
	ON_COMMAND(ID_INSTANCES_ZOOM_TO, &CMy3DEditorDoc::OnInstancesZoomTo)
	ON_UPDATE_COMMAND_UI(ID_INSTANCES_ZOOM_TO, &CMy3DEditorDoc::OnUpdateInstancesZoomTo)
	ON_COMMAND(ID_INSTANCES_SAVE, &CMy3DEditorDoc::OnInstancesSave)
	ON_UPDATE_COMMAND_UI(ID_INSTANCES_SAVE, &CMy3DEditorDoc::OnUpdateInstancesSave)
	ON_COMMAND(ID_EXPORT_AS_CITYGML, &CMy3DEditorDoc::OnExportAsCitygml)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_AS_CITYGML, &CMy3DEditorDoc::OnUpdateExportAsCitygml)
	ON_COMMAND(ID_EXPORT_AS_INFRAGML, &CMy3DEditorDoc::OnExportAsInfragml)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_AS_INFRAGML, &CMy3DEditorDoc::OnUpdateExportAsInfragml)
	ON_COMMAND(ID_EXPORT_AS_LANDXML, &CMy3DEditorDoc::OnExportAsLandxml)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_AS_LANDXML, &CMy3DEditorDoc::OnUpdateExportAsLandxml)
END_MESSAGE_MAP()


// CMy3DEditorDoc construction/destruction

CMy3DEditorDoc::CMy3DEditorDoc()
{
}

CMy3DEditorDoc::~CMy3DEditorDoc()
{
}

BOOL CMy3DEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	auto pModel = new CRDFModel();
	pModel->CreateDefaultModel();

	setModel(pModel);

	return TRUE;
}

// CMy3DEditorDoc serialization

void CMy3DEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring()) {
		// TODO: add storing code here
	} else {
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

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
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
	if (value.IsEmpty()) {
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	} else {
		CMFCFilterChunkValueImpl* pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr) {
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

	auto pModel = new CRDFModel();
	pModel->Load(lpszPathName, false);

	setModel(pModel);

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
	SaveModelW(getModel()->getOwlModel(), lpszPathName);

	return TRUE;
}

void CMy3DEditorDoc::OnViewScaleAndCenterAllGeometry()
{
	_ptr<_rdf_model>(getModel())->reloadGeometries();
	getModel()->scale();
}

void CMy3DEditorDoc::OnFileOpen()
{
	CFileDialog dlgFile(TRUE, nullptr, _T(""), OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, SUPPORTED_FILES);
	if (dlgFile.DoModal() != IDOK) {
		return;
	}

	OnOpenDocument(dlgFile.GetPathName());
}

void CMy3DEditorDoc::OnFileImport()
{
	CFileDialog dlgFile(TRUE, nullptr, _T(""), OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, SUPPORTED_FILES);

	if (dlgFile.DoModal() != IDOK) {
		return;
	}

	_ptr<CRDFModel>(getModel())->Load(dlgFile.GetPathName().GetString(), true);

	onModelUpdated();

	// MRU
	AfxGetApp()->AddToRecentFileList(dlgFile.GetPathName().GetString());
}

void CMy3DEditorDoc::OnUpdateFileImport(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(getModel() != nullptr);
}

void CMy3DEditorDoc::OnViewZoomOut()
{
	zoomOut();
}

void CMy3DEditorDoc::OnInstancesZoomTo()
{
	auto pInstance = getSelectedInstance();
	assert((pInstance != nullptr) && pInstance->getEnable() && pInstance->hasGeometry());

	zoomToInstance(pInstance);
}

void CMy3DEditorDoc::OnUpdateInstancesZoomTo(CCmdUI* pCmdUI)
{
	auto pInstance = getSelectedInstance();
	pCmdUI->Enable((pInstance != nullptr) && pInstance->getEnable() && pInstance->hasGeometry());
}

void CMy3DEditorDoc::OnInstancesSave()
{
	assert(getSelectedInstance() != nullptr);
	saveInstance(getSelectedInstance());
}

void CMy3DEditorDoc::OnUpdateInstancesSave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(getSelectedInstance() != nullptr);
}

void CMy3DEditorDoc::OnExportAsCitygml()
{
#ifdef _GIS_SUPPORT
	wstring strFileName = getModel()->getPath();
	strFileName += L".bin.citygml";

	TCHAR szFilters[] = _T("CityGML Files (*.citygml)|*.citygml|All Files (*.*)|*.*||");
	CFileDialog dlgFile(FALSE, _T(""), strFileName.c_str(),
		OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters);
	if (dlgFile.DoModal() != IDOK) {
		return;
	}

	SaveAsCityGMLW(getModel()->getOwlModel(), dlgFile.GetPathName().GetString());
#endif
}

void CMy3DEditorDoc::OnUpdateExportAsCitygml(CCmdUI* pCmdUI)
{
#ifdef _GIS_SUPPORT
	pCmdUI->Enable((getModel() != nullptr) &&
		(getModel()->getOwlModel() != 0) &&
		IsCityGML(getModel()->getOwlModel()));
#else
	pCmdUI->Enable(FALSE);
#endif
}

void CMy3DEditorDoc::OnExportAsInfragml()
{
#ifdef _GIS_SUPPORT
	wstring strFileName = getModel()->getPath();
	strFileName += L".bin.xml";

	TCHAR szFilters[] = _T("InfraGML Files (*.xml)|*.xml|All Files (*.*)|*.*||");
	CFileDialog dlgFile(FALSE, _T(""), strFileName.c_str(),
		OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters);
	if (dlgFile.DoModal() != IDOK) {
		return;
	}

	SaveAsInfraGMLW(getModel()->getOwlModel(), dlgFile.GetPathName().GetString());
#endif
}

void CMy3DEditorDoc::OnUpdateExportAsInfragml(CCmdUI* pCmdUI)
{
#ifdef _GIS_SUPPORT
	pCmdUI->Enable((getModel() != nullptr) &&
		(getModel()->getOwlModel() != 0) &&
		IsInfraGML(getModel()->getOwlModel()));
#else
	pCmdUI->Enable(FALSE);
#endif
}

void CMy3DEditorDoc::OnExportAsLandxml()
{
#ifdef _GIS_SUPPORT
	wstring strFileName = getModel()->getPath();
	strFileName += L".bin.xml";

	TCHAR szFilters[] = _T("LandXML Files (*.xml)|*.xml|All Files (*.*)|*.*||");
	CFileDialog dlgFile(FALSE, _T(""), strFileName.c_str(),
		OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters);
	if (dlgFile.DoModal() != IDOK) {
		return;
	}

	SaveAsLandXMLW(getModel()->getOwlModel(), dlgFile.GetPathName().GetString());
#endif
}

void CMy3DEditorDoc::OnUpdateExportAsLandxml(CCmdUI* pCmdUI)
{
#ifdef _GIS_SUPPORT
	pCmdUI->Enable((getModel() != nullptr) &&
		(getModel()->getOwlModel() != 0) &&
		IsLandXML(getModel()->getOwlModel()));
#else
	pCmdUI->Enable(FALSE);
#endif
}
