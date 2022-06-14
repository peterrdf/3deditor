/***************************************************************
 * Name:      RDFViewerNXMain.h
 * Purpose:   Defines Application Frame
 * Author:    RDF LTD ()
 * Created:   2017-08-15
 * Copyright: RDF LTD (www.rdf.com)
 * License:
 **************************************************************/

#ifndef RDFVIEWERNXMAIN_H
#define RDFVIEWERNXMAIN_H

//(*Headers(RDFViewerNXFrame)
#include <wx/menu.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
//*)

#include "RDFGLCanvas.h"
#include "InstancesDialog.h"

class RDFViewerNXFrame: public wxFrame
{
    public:

        RDFViewerNXFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~RDFViewerNXFrame();

    private:

        //(*Handlers(RDFViewerNXFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnMenuItemOpenSelected(wxCommandEvent& event);
        void OnViewInstances(wxCommandEvent& event);
        //*)

        //(*Identifiers(RDFViewerNXFrame)
        static const long idMenuOpen;
        static const long idMenuQuit;
        static const long ID_MENUITEM_INSTANCES;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(RDFViewerNXFrame)
        wxStatusBar* StatusBar1;
        wxMenu* Menu3;
        wxMenuItem* m_miInstances;
        wxMenuItem* MenuItemOpen;
        //*)

    private: // Members

        RDFGLCanvas * m_glCanvas;
        InstancesDialog * m_pInstancesDialog;

        DECLARE_EVENT_TABLE()
};

#endif // RDFVIEWERNXMAIN_H
