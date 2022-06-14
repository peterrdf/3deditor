#ifndef INSTANCESDIALOG_H
#define INSTANCESDIALOG_H

//(*Headers(InstancesDialog)
#include <wx/dialog.h>
#include <wx/listbox.h>
//*)

#include "Generic.h"
#include "RDFView.h"

#include <map>

using namespace std;

class InstancesDialog: public wxDialog, public CRDFView
{
	public:

		InstancesDialog(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~InstancesDialog();

		//(*Declarations(InstancesDialog)
		wxListBox* m_lbInstances;
		//*)

	protected:

		//(*Identifiers(InstancesDialog)
		static const long ID_LISTBOX_INSTANCES;
		//*)

	private:

		//(*Handlers(InstancesDialog)
		void OnInstancesSelect(wxCommandEvent& event);
		//*)


    private: // Members

        // ----------------------------------------------------------------------------------------
        // Find an HTREEITEM by CRDFInstance *
        map<CRDFInstance *, int> m_mapInstance2Item;

    public:// Methods

		// ----------------------------------------------------------------------------------------
        // CRDFView
        virtual void OnModelChanged();

        // ----------------------------------------------------------------------------------------
        // CRDFView
        virtual void OnInstanceSelected(CRDFView * pSender);

        // ----------------------------------------------------------------------------------------
        // CRDFView
        virtual void OnInstanceDeleted(CRDFView * pSender, int64_t iInstance);

        // ----------------------------------------------------------------------------------------
        // CRDFView
        virtual void OnInstancesDeleted(CRDFView * pSender);

    protected: // Methods

        // ----------------------------------------------------------------------------------------
        // CRDFView
        virtual void OnControllerChanged();

        // ----------------------------------------------------------------------------------------
        // Event handler
        void OnListboxRDown(wxMouseEvent& event);

        // ----------------------------------------------------------------------------------------
        // Event handler
        void OnPopupClick(wxCommandEvent &event);

		DECLARE_EVENT_TABLE()
};

#endif
