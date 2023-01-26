
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


// CMy3DEditorView

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
END_MESSAGE_MAP()

// CMy3DEditorView construction/destruction

CMy3DEditorView::CMy3DEditorView()
	: m_pOpenGLView(NULL)
{
	// TODO: add construction code here

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
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->Draw(pDC);
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

void CMy3DEditorView::OnContextMenu(CWnd* /* pWnd */, CPoint /*point*/)
{
#ifndef SHARED_HANDLERS
	// DSIABLED
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
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
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy3DEditorDoc)));
	return (CMy3DEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CMy3DEditorView message handlers


int CMy3DEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	DragAcceptFiles(TRUE);

	CMy3DEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return -1;

	m_pOpenGLView = new COpenGLRDFView(this);
	m_pOpenGLView->SetController(pDoc);

	return 0;
}


void CMy3DEditorView::OnDestroy()
{
	delete m_pOpenGLView;
	m_pOpenGLView = NULL;

	CView::OnDestroy();
}


BOOL CMy3DEditorView::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}


void CMy3DEditorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->OnMouseEvent(enumMouseEvent::meLBtnDown, nFlags, point);
	}

	CView::OnLButtonDown(nFlags, point);
}


void CMy3DEditorView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->OnMouseEvent(enumMouseEvent::meLBtnUp, nFlags, point);
	}

	CView::OnLButtonUp(nFlags, point);
}


void CMy3DEditorView::OnMButtonDown(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->OnMouseEvent(enumMouseEvent::meMBtnDown, nFlags, point);
	}

	CView::OnMButtonDown(nFlags, point);
}


void CMy3DEditorView::OnMButtonUp(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->OnMouseEvent(enumMouseEvent::meMBtnUp, nFlags, point);
	}

	CView::OnMButtonUp(nFlags, point);
}

void CMy3DEditorView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->OnMouseEvent(enumMouseEvent::meRBtnDown, nFlags, point);
	}

	CView::OnRButtonDown(nFlags, point);
}

void CMy3DEditorView::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->OnMouseEvent(enumMouseEvent::meRBtnUp, nFlags, point);
	}

	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMy3DEditorView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->OnMouseEvent(enumMouseEvent::meMove, nFlags, point);
	}

	CView::OnMouseMove(nFlags, point);
}

// https://code.msdn.microsoft.com/windowsdesktop/Drag-and-Drop-in-MFC-f3008aad
void CMy3DEditorView::OnDropFiles(HDROP hDropInfo)
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
	CMy3DEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->OnOpenDocument(strFile);

	strFile.ReleaseBuffer();

	// Free the memory block containing the dropped-file information 
	DragFinish(hDropInfo);
}

BOOL CMy3DEditorView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->OnMouseWheel(nFlags, zDelta, pt);
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CMy3DEditorView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->OnKeyUp(nChar, nRepCnt, nFlags);
	}

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CMy3DEditorView::OnViewTop()
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->SetView(enumView::Top);
	}
}

void CMy3DEditorView::OnViewLeft()
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->SetView(enumView::Left);
	}
}

void CMy3DEditorView::OnViewRight()
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->SetView(enumView::Right);
	}
}

void CMy3DEditorView::OnViewBottom()
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->SetView(enumView::Bottom);
	}
}

void CMy3DEditorView::OnViewFront()
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->SetView(enumView::Front);
	}
}

void CMy3DEditorView::OnViewBack()
{
	if (m_pOpenGLView != NULL)
	{
		m_pOpenGLView->SetView(enumView::Back);
	}
}
