#include "RDFGLCanvas.h"
#include "RDFModel.h"
#include <GL/gl.h>
#include <GL/glext.h>

wxBEGIN_EVENT_TABLE(RDFGLCanvas, wxGLCanvas)
    EVT_PAINT(RDFGLCanvas::OnPaint)
    EVT_KEY_DOWN(RDFGLCanvas::OnKeyDown)
    EVT_LEFT_DOWN(RDFGLCanvas::OnMouseButtonDown)
    EVT_LEFT_UP(RDFGLCanvas::OnMouseButtonUp)
    EVT_MIDDLE_DOWN(RDFGLCanvas::OnMouseButtonDown)
    EVT_MIDDLE_UP(RDFGLCanvas::OnMouseButtonUp)
    EVT_RIGHT_DOWN(RDFGLCanvas::OnMouseButtonDown)
    EVT_RIGHT_UP(RDFGLCanvas::OnMouseButtonUp)
    EVT_MOTION(RDFGLCanvas::OnMouseMove)
    EVT_WINDOW_CREATE(RDFGLCanvas::OnCreate)
wxEND_EVENT_TABLE()


RDFGLCanvas::RDFGLCanvas(wxWindow *parent, int * pAttributes)
    : wxGLCanvas(parent, wxID_ANY, pAttributes, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
{
    m_pModel = new CRDFModel();
	m_pModel->CreateDefaultModel();

	m_pOpenGLRDFView = new COpenGLRDFView(this);
    m_pOpenGLRDFView->ShowCoordinateSystem(FALSE);
	m_pOpenGLRDFView->ShowReferencedInstances(FALSE);
    m_pOpenGLRDFView->SetController(this);
}

RDFGLCanvas::~RDFGLCanvas()
{
    delete m_pOpenGLRDFView;
    delete m_pModel;
}


void RDFGLCanvas::LoadModel(const wchar_t * szRDFFile)
{
    if (m_pModel != NULL)
	{
		delete m_pModel;
		m_pModel = NULL;
	}

	m_pModel = new CRDFModel();
	m_pModel->Load(szRDFFile);

	SetModel(m_pModel);

	Refresh(false);
}

void RDFGLCanvas::CheckGLError()
{
    GLenum errLast = GL_NO_ERROR;

    for ( ;; )
    {
        GLenum err = glGetError();
        if ( err == GL_NO_ERROR )
            return;

        // normally the error is reset by the call to glGetError() but if
        // glGetError() itself returns an error, we risk looping forever here
        // so check that we get a different error than the last time
        if ( err == errLast )
        {
            wxLogError(wxT("OpenGL error state couldn't be reset."));
            return;
        }

        errLast = err;

        wxLogError(wxT("OpenGL error %d"), err);
    }
}

void RDFGLCanvas::OnCreate(wxWindowCreateEvent& event)
{
	SetModel(m_pModel);
}

void RDFGLCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    // This is required even though dc is not used otherwise.
    wxPaintDC dc(this);

    if (m_pOpenGLRDFView != NULL)
    {
        m_pOpenGLRDFView->Draw(&dc);
    }
}

void RDFGLCanvas::OnKeyDown(wxKeyEvent& event)
{
}

void RDFGLCanvas::OnMouseButtonDown(wxMouseEvent& event)
{
    event.Skip();

    if (m_pOpenGLRDFView != NULL)
	{
	    UINT nFlags = 0;
	    if (event.LeftIsDown())
        {
            nFlags |= MK_LBUTTON;

            m_pOpenGLRDFView->OnMouseEvent(meLBtnDown, nFlags, event.GetPosition());

            return;
        }

        if (event.MiddleIsDown())
        {
            nFlags |= MK_MBUTTON;

            m_pOpenGLRDFView->OnMouseEvent(meMBtnDown, nFlags, event.GetPosition());

            return;
        }

        if (event.RightIsDown())
        {
            nFlags |= MK_RBUTTON;

            m_pOpenGLRDFView->OnMouseEvent(meRBtnDown, nFlags, event.GetPosition());

            return;
        }
	}
}

void RDFGLCanvas::OnMouseButtonUp(wxMouseEvent& event)
{
    event.Skip();

    if (m_pOpenGLRDFView != NULL)
	{
	    UINT nFlags = 0;
	    if (event.LeftUp())
        {
            nFlags |= MK_LBUTTON;

            m_pOpenGLRDFView->OnMouseEvent(meLBtnUp, nFlags, event.GetPosition());

            return;
        }

        if (event.MiddleUp())
        {
            nFlags |= MK_MBUTTON;

            m_pOpenGLRDFView->OnMouseEvent(meMBtnUp, nFlags, event.GetPosition());

            return;
        }

        if (event.RightUp())
        {
            nFlags |= MK_RBUTTON;

            m_pOpenGLRDFView->OnMouseEvent(meRBtnUp, nFlags, event.GetPosition());

            return;
        }
	}
}

void RDFGLCanvas::OnMouseMove(wxMouseEvent& event)
{
    event.Skip();

    if (m_pOpenGLRDFView != NULL)
	{
	    UINT nFlags = 0;
	    if (event.LeftIsDown())
        {
            nFlags |= MK_LBUTTON;
        }

        if (event.MiddleIsDown())
        {
            nFlags |= MK_MBUTTON;
        }

        if (event.RightIsDown())
        {
            nFlags |= MK_RBUTTON;
        }

		m_pOpenGLRDFView->OnMouseEvent(meMove, nFlags, event.GetPosition());
	}
}
