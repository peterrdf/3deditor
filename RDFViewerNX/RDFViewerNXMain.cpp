/***************************************************************
 * Name:      RDFViewerNXMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    RDF LTD ()
 * Created:   2017-08-15
 * Copyright: RDF LTD (www.rdf.com)
 * License:
 **************************************************************/

#include "RDFViewerNXMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(RDFViewerNXFrame)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(RDFViewerNXFrame)
const long RDFViewerNXFrame::idMenuOpen = wxNewId();
const long RDFViewerNXFrame::idMenuQuit = wxNewId();
const long RDFViewerNXFrame::ID_MENUITEM_INSTANCES = wxNewId();
const long RDFViewerNXFrame::idMenuAbout = wxNewId();
const long RDFViewerNXFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(RDFViewerNXFrame,wxFrame)
    //(*EventTable(RDFViewerNXFrame)
    //*)
END_EVENT_TABLE()

RDFViewerNXFrame::RDFViewerNXFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(RDFViewerNXFrame)
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxMenu* Menu1;
    wxMenuBar* MenuBar1;
    wxMenu* Menu2;

    Create(parent, id, _("RDFViewer"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItemOpen = new wxMenuItem(Menu1, idMenuOpen, _("Open..."), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItemOpen);
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu3 = new wxMenu();
    m_miInstances = new wxMenuItem(Menu3, ID_MENUITEM_INSTANCES, _("Instances"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(m_miInstances);
    MenuBar1->Append(Menu3, _("View"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);

    Connect(idMenuOpen,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&RDFViewerNXFrame::OnMenuItemOpenSelected);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&RDFViewerNXFrame::OnQuit);
    Connect(ID_MENUITEM_INSTANCES,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&RDFViewerNXFrame::OnViewInstances);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&RDFViewerNXFrame::OnAbout);
    //*)

    int arAttributes[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};
    m_glCanvas = new RDFGLCanvas(this, arAttributes);

    wxTopLevelWindow::Maximize(true);

    m_pInstancesDialog = new InstancesDialog(this);
    m_pInstancesDialog->SetController(m_glCanvas);
    m_pInstancesDialog->Show();
}

RDFViewerNXFrame::~RDFViewerNXFrame()
{
    //(*Destroy(RDFViewerNXFrame)
    //*)
}

void RDFViewerNXFrame::OnQuit(wxCommandEvent& event)
{
    delete m_glCanvas;
    delete m_pInstancesDialog;

    Close();
}

void RDFViewerNXFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void RDFViewerNXFrame::OnMenuItemOpenSelected(wxCommandEvent& event)
{
     wxFileDialog * pOpenFileDialog = new wxFileDialog(
		this, _("Choose a file to open"), wxEmptyString, wxEmptyString,
		_("RDF files (*.rdf)|*.rdf"),
		wxFD_OPEN, wxDefaultPosition);

	if (pOpenFileDialog->ShowModal() == wxID_OK)
	{
		SetTitle(pOpenFileDialog->GetFilename());

		m_glCanvas->LoadModel(pOpenFileDialog->GetPath().c_str());
	}

	pOpenFileDialog->Destroy();
}

void RDFViewerNXFrame::OnViewInstances(wxCommandEvent& event)
{
    if (m_pInstancesDialog->IsShown())
    {
        m_pInstancesDialog->Hide();
    }
    else
    {
        m_pInstancesDialog->Show();
    }
}
