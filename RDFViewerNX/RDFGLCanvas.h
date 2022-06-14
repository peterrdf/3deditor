#ifndef _RDF_GL_CANVAS_H_
#define _RDF_GL_CANVAS_H_

#include <wx/wx.h>
#include <wx/glcanvas.h>

#include "OpenGLRDFView.h"
#include "RDFController.h"

class RDFGLCanvas
 : public wxGLCanvas
 , public CRDFController
{
public: // Methods

    RDFGLCanvas(wxWindow *parent, int * pAttributes);
    virtual ~RDFGLCanvas();

    void LoadModel(const wchar_t * szRDFFile);

private: // Methods

    void CheckGLError();
    void OnCreate(wxWindowCreateEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnMouseButtonDown(wxMouseEvent& event);
    void OnMouseButtonUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);

private: // Members

    // --------------------------------------------------------------------------------------------
	// Model (MVC)
	CRDFModel * m_pModel;

    // --------------------------------------------------------------------------------------------
	// View (MVC)
    COpenGLRDFView * m_pOpenGLRDFView;

    wxDECLARE_EVENT_TABLE();
};

#endif // _RDF_GL_CANVAS_H
