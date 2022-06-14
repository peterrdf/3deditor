/***************************************************************
 * Name:      RDFViewerNXApp.cpp
 * Purpose:   Code for Application Class
 * Author:    RDF LTD ()
 * Created:   2017-08-15
 * Copyright: RDF LTD (www.rdf.com)
 * License:
 **************************************************************/

#include "RDFViewerNXApp.h"

//(*AppHeaders
#include "RDFViewerNXMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(RDFViewerNXApp);

bool RDFViewerNXApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	RDFViewerNXFrame* Frame = new RDFViewerNXFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
