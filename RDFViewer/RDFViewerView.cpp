
// RDFViewerView.cpp : implementation of the CRDFViewerView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "RDFViewer.h"
#endif

#include "RDFViewerDoc.h"
#include "RDFViewerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRDFViewerView

IMPLEMENT_DYNCREATE(CRDFViewerView, CView)

BEGIN_MESSAGE_MAP(CRDFViewerView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CRDFViewerView::OnFilePrintPreview)
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
	ON_COMMAND(ID_VIEW_INSTANCES, &CRDFViewerView::OnViewInstances)
	ON_UPDATE_COMMAND_UI(ID_VIEW_INSTANCES, &CRDFViewerView::OnUpdateViewInstances)
END_MESSAGE_MAP()

// CRDFViewerView construction/destruction

CRDFViewerView::CRDFViewerView()
	: m_pOpenGLView(NULL)
	, m_pInstancesDialog(NULL)
{
}

CRDFViewerView::~CRDFViewerView()
{	
}

BOOL CRDFViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~CS_PARENTDC;
	cs.style |= CS_OWNDC;
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	return CView::PreCreateWindow(cs);
}

// CRDFViewerView drawing

void CRDFViewerView::OnDraw(CDC* pDC)
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->Draw(pDC);
	}
}


// CRDFViewerView printing


void CRDFViewerView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CRDFViewerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CRDFViewerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CRDFViewerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CRDFViewerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CRDFViewerView diagnostics

#ifdef _DEBUG
void CRDFViewerView::AssertValid() const
{
	CView::AssertValid();
}

void CRDFViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRDFViewerDoc* CRDFViewerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRDFViewerDoc)));
	return (CRDFViewerDoc*)m_pDocument;
}
#endif //_DEBUG


// CRDFViewerView message handlers

int CRDFViewerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	DragAcceptFiles(TRUE);

	CRDFViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return -1;

	m_pOpenGLView = new COpenGLRDFView(this);
	m_pOpenGLView->SetController(pDoc);

	m_pInstancesDialog = new CInstancesDialog();
	m_pInstancesDialog->SetController(pDoc);
	m_pInstancesDialog->Create(CInstancesDialog::IDD, this);	
	m_pInstancesDialog->ShowWindow(SW_SHOW);

	return 0;
}

void CRDFViewerView::OnDestroy()
{
	delete m_pOpenGLView;
	m_pOpenGLView = NULL;

	delete m_pInstancesDialog;
	m_pInstancesDialog = NULL;

	CView::OnDestroy();
}

BOOL CRDFViewerView::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CRDFViewerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->OnMouseEvent(enumMouseEvent::LBtnDown, nFlags, point);
	}

	CView::OnLButtonDown(nFlags, point);
}


void CRDFViewerView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->OnMouseEvent(enumMouseEvent::LBtnUp, nFlags, point);
	}

	CView::OnLButtonUp(nFlags, point);
}


void CRDFViewerView::OnMButtonDown(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->OnMouseEvent(enumMouseEvent::MBtnDown, nFlags, point);
	}

	CView::OnMButtonDown(nFlags, point);
}


void CRDFViewerView::OnMButtonUp(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->OnMouseEvent(enumMouseEvent::MBtnUp, nFlags, point);
	}

	CView::OnMButtonUp(nFlags, point);
}


void CRDFViewerView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->OnMouseEvent(enumMouseEvent::RBtnDown, nFlags, point);
	}

	CView::OnRButtonDown(nFlags, point);
}

void CRDFViewerView::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->OnMouseEvent(enumMouseEvent::RBtnUp, nFlags, point);
	}

	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CRDFViewerView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->OnMouseEvent(enumMouseEvent::Move, nFlags, point);
	}

	CView::OnMouseMove(nFlags, point);
}

// https://code.msdn.microsoft.com/windowsdesktop/Drag-and-Drop-in-MFC-f3008aad
void CRDFViewerView::OnDropFiles(HDROP hDropInfo)
{
	// Get the number of files dropped 
	int iFilesDropped = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	if (iFilesDropped != 1)
	{
		return;
	}

	// Get the buffer size of the file.
	DWORD dwBuffer = DragQueryFile(hDropInfo, 0, NULL, 0);

	// Get path and name of the file 
	CString strFile;
	DragQueryFile(hDropInfo, 0, strFile.GetBuffer(dwBuffer + 1), dwBuffer + 1);

	// Open
	CRDFViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->OnOpenDocument(strFile);

	strFile.ReleaseBuffer();

	// Free the memory block containing the dropped-file information 
	DragFinish(hDropInfo);
}

void CRDFViewerView::OnViewInstances()
{
	m_pInstancesDialog->ShowWindow(m_pInstancesDialog->IsWindowVisible() ? SW_HIDE : SW_SHOW);
}

void CRDFViewerView::OnUpdateViewInstances(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pInstancesDialog->IsWindowVisible());
}
