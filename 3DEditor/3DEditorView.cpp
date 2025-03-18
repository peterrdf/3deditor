
// 3DEditorView.cpp : implementation of the CMy3DEditorView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "3DEditor.h"
#endif

#include "3DEditorDoc.h"
#include "3DEditorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ************************************************************************************************
CRDFController* CMy3DEditorView::GetController()
{
	auto pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	return pDoc;
}

/*virtual*/ void CMy3DEditorView::onModelLoaded() /*override*/
{
	delete m_pOpenGLView;
	m_pOpenGLView = nullptr;

	auto pController = GetController();
	if (pController == nullptr) {
		ASSERT(FALSE);
		return;
	}

	m_pOpenGLView = new CRDFOpenGLView(this);
	m_pOpenGLView->setController(pController);
	m_pOpenGLView->_load();
}

/*virtual*/ void CMy3DEditorView::onModelUpdated() /*override*/
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->_load();
	}
}

/*virtual*/ void CMy3DEditorView::onInstancePropertySelected(_view* pSender) /*override*/
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->onInstancePropertySelected(pSender);
	}
}

/*virtual*/ void CMy3DEditorView::onInstanceCreated(_view* pSender, _rdf_instance* pInstance) /*override*/
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->onInstanceCreated(pSender, pInstance);
	}
}

/*virtual*/ void CMy3DEditorView::onInstanceDeleted(_view* pSender, _rdf_instance* pInstance) /*override*/
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->onInstanceDeleted(pSender, pInstance);
	}
}

/*virtual*/ void CMy3DEditorView::onInstancesDeleted(_view* pSender) /*override*/
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->onInstancesDeleted(pSender);
	}
}

/*virtual*/ void CMy3DEditorView::onMeasurementsAdded(_view* pSender, _rdf_instance* pInstance) /*override*/
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->onMeasurementsAdded(pSender, pInstance);
	}
}

/*virtual*/ void CMy3DEditorView::onInstancePropertyEdited(_view* pSender, _rdf_instance* pInstance, _rdf_property* pProperty) /*override*/
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->onInstancePropertyEdited(pSender, pInstance, pProperty);
	}
}

// ************************************************************************************************
IMPLEMENT_DYNCREATE(CMy3DEditorView, CView)

BEGIN_MESSAGE_MAP(CMy3DEditorView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMy3DEditorView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_DROPFILES()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYUP()
	ON_COMMAND(ID_VIEW_TOP, &CMy3DEditorView::OnViewTop)
	ON_COMMAND(ID_VIEW_LEFT, &CMy3DEditorView::OnViewLeft)
	ON_COMMAND(ID_VIEW_RIGHT, &CMy3DEditorView::OnViewRight)
	ON_COMMAND(ID_VIEW_BOTTOM, &CMy3DEditorView::OnViewBottom)
	ON_COMMAND(ID_VIEW_FRONT, &CMy3DEditorView::OnViewFront)
	ON_COMMAND(ID_VIEW_BACK, &CMy3DEditorView::OnViewBack)
	ON_COMMAND(ID_VIEW_ISOMETRIC, &CMy3DEditorView::OnViewIsometric)
	ON_COMMAND(ID_PROJECTION_PERSPECTIVE, &CMy3DEditorView::OnProjectionPerspective)
	ON_UPDATE_COMMAND_UI(ID_PROJECTION_PERSPECTIVE, &CMy3DEditorView::OnUpdateProjectionPerspective)
	ON_COMMAND(ID_PROJECTION_ORTHOGRAPHIC, &CMy3DEditorView::OnProjectionOrthographic)
	ON_UPDATE_COMMAND_UI(ID_PROJECTION_ORTHOGRAPHIC, &CMy3DEditorView::OnUpdateProjectionOrthographic)
	ON_COMMAND(ID_SHOW_FACES, &CMy3DEditorView::OnShowFaces)
	ON_UPDATE_COMMAND_UI(ID_SHOW_FACES, &CMy3DEditorView::OnUpdateShowFaces)
	ON_COMMAND(ID_SHOW_FACES_WIREFRAMES, &CMy3DEditorView::OnShowFacesWireframes)
	ON_UPDATE_COMMAND_UI(ID_SHOW_FACES_WIREFRAMES, &CMy3DEditorView::OnUpdateShowFacesWireframes)
	ON_COMMAND(ID_SHOW_CONC_FACES_WIREFRAMES, &CMy3DEditorView::OnShowConcFacesWireframes)
	ON_UPDATE_COMMAND_UI(ID_SHOW_CONC_FACES_WIREFRAMES, &CMy3DEditorView::OnUpdateShowConcFacesWireframes)
	ON_COMMAND(ID_SHOW_LINES, &CMy3DEditorView::OnShowLines)
	ON_UPDATE_COMMAND_UI(ID_SHOW_LINES, &CMy3DEditorView::OnUpdateShowLines)
	ON_COMMAND(ID_SHOW_POINTS, &CMy3DEditorView::OnShowPoints)
	ON_UPDATE_COMMAND_UI(ID_SHOW_POINTS, &CMy3DEditorView::OnUpdateShowPoints)
	ON_COMMAND(ID_NORMAL_VECTORS, &CMy3DEditorView::OnNormalVectors)
	ON_UPDATE_COMMAND_UI(ID_NORMAL_VECTORS, &CMy3DEditorView::OnUpdateNormalVectors)
	ON_COMMAND(ID_SHOW_TANGENT_VECTORS, &CMy3DEditorView::OnShowTangentVectors)
	ON_UPDATE_COMMAND_UI(ID_SHOW_TANGENT_VECTORS, &CMy3DEditorView::OnUpdateShowTangentVectors)
	ON_COMMAND(ID_SHOW_BI_NORMAL_VECTORS, &CMy3DEditorView::OnShowBiNormalVectors)
	ON_UPDATE_COMMAND_UI(ID_SHOW_BI_NORMAL_VECTORS, &CMy3DEditorView::OnUpdateShowBiNormalVectors)
	ON_COMMAND(ID_SHOW_BOUNDING_BOXES, &CMy3DEditorView::OnShowBoundingBoxes)
	ON_UPDATE_COMMAND_UI(ID_SHOW_BOUNDING_BOXES, &CMy3DEditorView::OnUpdateShowBoundingBoxes)
END_MESSAGE_MAP()

// CMy3DEditorView construction/destruction

CMy3DEditorView::CMy3DEditorView()
	: CView()
	, _rdf_view()
	, m_pOpenGLView(nullptr)
{
}

CMy3DEditorView::~CMy3DEditorView()
{
}

BOOL CMy3DEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~CS_PARENTDC;
	cs.style |= CS_OWNDC;
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	return CView::PreCreateWindow(cs);
}

// CMy3DEditorView drawing

void CMy3DEditorView::OnDraw(CDC* pDC)
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->_draw(pDC);
	}
}


// CMy3DEditorView printing


void CMy3DEditorView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMy3DEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMy3DEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMy3DEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CMy3DEditorView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
	CMenu menu;
	VERIFY(menu.LoadMenuW(IDR_MENU_OPENGL));

	CMenu* pPopup = menu.GetSubMenu(0);

#ifndef SHARED_HANDLERS
	CMFCPopupMenu* settingsMenu = theApp.GetContextMenuManager()->ShowPopupMenu(pPopup->GetSafeHmenu(), point.x, point.y, this, TRUE);
	settingsMenu->AlwaysShowEmptyToolsEntry(1);
#endif
}


// CMy3DEditorView diagnostics

#ifdef _DEBUG
void CMy3DEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CMy3DEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMy3DEditorDoc* CMy3DEditorView::GetDocument() const // non-debug version is inline
{
	assert(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy3DEditorDoc)));
	return (CMy3DEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CMy3DEditorView message handlers


int CMy3DEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	DragAcceptFiles(TRUE);

	auto pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!pDoc) {
		return -1;
	}

	pDoc->registerView(this);

	return 0;
}


void CMy3DEditorView::OnDestroy()
{
	auto pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!pDoc) {
		return;
	}

	pDoc->unRegisterView(this);

	delete m_pOpenGLView;
	m_pOpenGLView = nullptr;

	CView::OnDestroy();
}


BOOL CMy3DEditorView::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}


void CMy3DEditorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->_onMouseEvent(enumMouseEvent::LBtnDown, nFlags, point);
	}

	CView::OnLButtonDown(nFlags, point);
}


void CMy3DEditorView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->_onMouseEvent(enumMouseEvent::LBtnUp, nFlags, point);
	}

	CView::OnLButtonUp(nFlags, point);
}


void CMy3DEditorView::OnMButtonDown(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->_onMouseEvent(enumMouseEvent::MBtnDown, nFlags, point);
	}

	CView::OnMButtonDown(nFlags, point);
}


void CMy3DEditorView::OnMButtonUp(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->_onMouseEvent(enumMouseEvent::MBtnUp, nFlags, point);
	}

	CView::OnMButtonUp(nFlags, point);
}

void CMy3DEditorView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->_onMouseEvent(enumMouseEvent::RBtnDown, nFlags, point);
	}

	CView::OnRButtonDown(nFlags, point);
}

void CMy3DEditorView::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->_onMouseEvent(enumMouseEvent::RBtnUp, nFlags, point);
	}

	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMy3DEditorView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->_onMouseEvent(enumMouseEvent::Move, nFlags, point);
	}

	CView::OnMouseMove(nFlags, point);
}

// https://code.msdn.microsoft.com/windowsdesktop/Drag-and-Drop-in-MFC-f3008aad
void CMy3DEditorView::OnDropFiles(HDROP hDropInfo)
{
	// Get the number of files dropped 
	int iFilesDropped = DragQueryFile(hDropInfo, 0xFFFFFFFF, nullptr, 0);
	if (iFilesDropped != 1) {
		return;
	}

	// Get the buffer size of the file.
	DWORD dwBuffer = DragQueryFile(hDropInfo, 0, nullptr, 0);

	// Get path and name of the file 
	CString strFile;
	DragQueryFile(hDropInfo, 0, strFile.GetBuffer(dwBuffer + 1), dwBuffer + 1);

	// Open
	CMy3DEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->OnOpenDocument(strFile);

	strFile.ReleaseBuffer();

	// Free the memory block containing the dropped-file information 
	DragFinish(hDropInfo);
}

BOOL CMy3DEditorView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->_onMouseWheel(nFlags, zDelta, pt);
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CMy3DEditorView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->_onKeyUp(nChar, nRepCnt, nFlags);
	}

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CMy3DEditorView::OnViewTop()
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->_setView(enumView::Top);

		GetController()->onApplicationPropertyChanged(nullptr, enumApplicationProperty::View);
	}
}

void CMy3DEditorView::OnViewLeft()
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->_setView(enumView::Left);

		GetController()->onApplicationPropertyChanged(nullptr, enumApplicationProperty::View);
	}
}

void CMy3DEditorView::OnViewRight()
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->_setView(enumView::Right);

		GetController()->onApplicationPropertyChanged(nullptr, enumApplicationProperty::View);
	}
}

void CMy3DEditorView::OnViewBottom()
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->_setView(enumView::Bottom);

		GetController()->onApplicationPropertyChanged(nullptr, enumApplicationProperty::View);
	}
}

void CMy3DEditorView::OnViewFront()
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->_setView(enumView::Front);

		GetController()->onApplicationPropertyChanged(nullptr, enumApplicationProperty::View);
	}
}

void CMy3DEditorView::OnViewBack()
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->_setView(enumView::Back);

		GetController()->onApplicationPropertyChanged(nullptr, enumApplicationProperty::View);
	}
}

void CMy3DEditorView::OnViewIsometric()
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->_setView(enumView::Isometric);

		GetController()->onApplicationPropertyChanged(nullptr, enumApplicationProperty::View);
	}
}

void CMy3DEditorView::OnProjectionPerspective()
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->_setProjection(enumProjection::Perspective);

		GetController()->onApplicationPropertyChanged(nullptr, enumApplicationProperty::Projection);
	}
}

void CMy3DEditorView::OnUpdateProjectionPerspective(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pOpenGLView != nullptr);
	pCmdUI->SetCheck((m_pOpenGLView != nullptr) && (m_pOpenGLView->_getProjection() == enumProjection::Perspective));
}

void CMy3DEditorView::OnProjectionOrthographic()
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->_setProjection(enumProjection::Orthographic);

		GetController()->onApplicationPropertyChanged(nullptr, enumApplicationProperty::Projection);
	}
}

void CMy3DEditorView::OnUpdateProjectionOrthographic(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pOpenGLView != nullptr);
	pCmdUI->SetCheck((m_pOpenGLView != nullptr) && (m_pOpenGLView->_getProjection() == enumProjection::Orthographic));
}

void CMy3DEditorView::OnShowFaces()
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->setShowFaces(!m_pOpenGLView->getShowFaces());

		GetController()->onApplicationPropertyChanged(nullptr, enumApplicationProperty::ShowFaces);
	}
}

void CMy3DEditorView::OnUpdateShowFaces(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pOpenGLView != nullptr);
	pCmdUI->SetCheck((m_pOpenGLView != nullptr) && m_pOpenGLView->getShowFaces());
}

void CMy3DEditorView::OnShowFacesWireframes()
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->setShowFacesPolygons(!m_pOpenGLView->getShowFacesPolygons());

		GetController()->onApplicationPropertyChanged(nullptr, enumApplicationProperty::ShowFacesWireframes);
	}
}

void CMy3DEditorView::OnUpdateShowFacesWireframes(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pOpenGLView != nullptr);
	pCmdUI->SetCheck((m_pOpenGLView != nullptr) && m_pOpenGLView->getShowFacesPolygons());
}

void CMy3DEditorView::OnShowConcFacesWireframes()
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->setShowConceptualFacesPolygons(!m_pOpenGLView->getShowConceptualFacesPolygons());

		GetController()->onApplicationPropertyChanged(nullptr, enumApplicationProperty::ShowConceptualFacesWireframes);
	}
}

void CMy3DEditorView::OnUpdateShowConcFacesWireframes(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pOpenGLView != nullptr);
	pCmdUI->SetCheck((m_pOpenGLView != nullptr) && m_pOpenGLView->getShowConceptualFacesPolygons());
}

void CMy3DEditorView::OnShowLines()
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->setShowLines(!m_pOpenGLView->getShowLines());

		GetController()->onApplicationPropertyChanged(nullptr, enumApplicationProperty::ShowLines);
	}
}

void CMy3DEditorView::OnUpdateShowLines(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pOpenGLView != nullptr);
	pCmdUI->SetCheck((m_pOpenGLView != nullptr) && m_pOpenGLView->getShowLines());
}

void CMy3DEditorView::OnShowPoints()
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->setShowPoints(!m_pOpenGLView->getShowPoints());

		GetController()->onApplicationPropertyChanged(nullptr, enumApplicationProperty::ShowPoints);
	}
}

void CMy3DEditorView::OnUpdateShowPoints(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pOpenGLView != nullptr);
	pCmdUI->SetCheck((m_pOpenGLView != nullptr) && m_pOpenGLView->getShowPoints());
}

void CMy3DEditorView::OnNormalVectors()
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->setShowNormalVectors(!m_pOpenGLView->getShowNormalVectors());

		GetController()->onApplicationPropertyChanged(nullptr, enumApplicationProperty::ShowNormalVectors);
	}
}

void CMy3DEditorView::OnUpdateNormalVectors(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pOpenGLView != nullptr);
	pCmdUI->SetCheck((m_pOpenGLView != nullptr) && m_pOpenGLView->getShowNormalVectors());
}

void CMy3DEditorView::OnShowTangentVectors()
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->setShowTangentVectors(!m_pOpenGLView->getShowTangentVectors());

		GetController()->onApplicationPropertyChanged(nullptr, enumApplicationProperty::ShowTangenVectors);
	}
}

void CMy3DEditorView::OnUpdateShowTangentVectors(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pOpenGLView != nullptr);
	pCmdUI->SetCheck((m_pOpenGLView != nullptr) && m_pOpenGLView->getShowTangentVectors());
}

void CMy3DEditorView::OnShowBiNormalVectors()
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->setShowBiNormalVectors(!m_pOpenGLView->getShowBiNormalVectors());

		GetController()->onApplicationPropertyChanged(nullptr, enumApplicationProperty::ShowBiNormalVectors);
	}
}

void CMy3DEditorView::OnUpdateShowBiNormalVectors(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pOpenGLView != nullptr);
	pCmdUI->SetCheck((m_pOpenGLView != nullptr) && m_pOpenGLView->getShowBiNormalVectors());
}

void CMy3DEditorView::OnShowBoundingBoxes()
{
	if (m_pOpenGLView != nullptr) {
		m_pOpenGLView->setShowBoundingBoxes(!m_pOpenGLView->getShowBoundingBoxes());

		GetController()->onApplicationPropertyChanged(nullptr, enumApplicationProperty::ShowBoundingBoxes);
	}
}

void CMy3DEditorView::OnUpdateShowBoundingBoxes(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pOpenGLView != nullptr);
	pCmdUI->SetCheck((m_pOpenGLView != nullptr) && m_pOpenGLView->getShowBoundingBoxes());
}
