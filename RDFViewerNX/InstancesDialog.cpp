#include "InstancesDialog.h"
#include "RDFController.h"
#include "RDFModel.h"

#include <algorithm>

using namespace std;


//(*InternalHeaders(InstancesDialog)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(InstancesDialog)
const long InstancesDialog::ID_LISTBOX_INSTANCES = wxNewId();
//*)

BEGIN_EVENT_TABLE(InstancesDialog,wxDialog)
	//(*EventTable(InstancesDialog)
	//*)
END_EVENT_TABLE()

InstancesDialog::InstancesDialog(wxWindow* parent,wxWindowID id)
    : m_mapInstance2Item()
{
	//(*Initialize(InstancesDialog)
	Create(parent, id, _("Instances"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("id"));
	Move(wxPoint(10,10));
	m_lbInstances = new wxListBox(this, ID_LISTBOX_INSTANCES, wxPoint(208,200), wxDefaultSize, 0, 0, wxLB_MULTIPLE, wxDefaultValidator, _T("ID_LISTBOX_INSTANCES"));

	Connect(ID_LISTBOX_INSTANCES,wxEVT_COMMAND_LISTBOX_SELECTED,(wxObjectEventFunction)&InstancesDialog::OnInstancesSelect);
	//*)

	m_lbInstances->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(InstancesDialog::OnListboxRDown), NULL, this);
}

InstancesDialog::~InstancesDialog()
{
	//(*Destroy(InstancesDialog)
	//*)
}


void InstancesDialog::OnInstancesSelect(wxCommandEvent& event)
{
    wxArrayInt arSelections;
    if (m_lbInstances->GetSelections(arSelections) == 0)
    {
        GetController()->SelectInstance(this, NULL);

        return;
    }

    if (arSelections.GetCount() == 1)
    {
        /*
        * Single selection
        */

        CRDFInstance * pRDFInstance = (CRDFInstance *)m_lbInstances->GetClientData(arSelections[0]);
        assert(pRDFInstance != NULL);

        GetController()->SelectInstance(this, pRDFInstance);
    } // if (arSelections.GetCount() == 1)
    else
    {
        /*
        * Multi selection
        */
        GetController()->SelectInstance(this, NULL);
    } // else if (arSelections.GetCount() == 1)
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void InstancesDialog::OnModelChanged()
{
    m_lbInstances->Clear();
    m_mapInstance2Item.clear();

    CRDFModel * pModel = GetController()->GetModel();
	assert(pModel != NULL);

	const map<int64_t, CRDFInstance *> & mapRFDInstances = pModel->GetRDFInstances();

	vector<CRDFInstance *> vecModel;

	map<int64_t, CRDFInstance *>::const_iterator itRFDInstances = mapRFDInstances.begin();
	for (; itRFDInstances != mapRFDInstances.end(); itRFDInstances++)
	{
		CRDFInstance * pRDFInstance = itRFDInstances->second;

		if (pRDFInstance->isReferenced())
		{
			continue;
		}

		if (pRDFInstance->GetModel() == pModel->GetModel())
		{
			vecModel.push_back(pRDFInstance);
		}
	} // for (; itRFDInstances != ...

	/*
	* Instances
	*/
	sort(vecModel.begin(), vecModel.end(), SORT_RDFINSTANCES());

	for (size_t iInstance = 0; iInstance < vecModel.size(); iInstance++)
	{
		int iItem = m_lbInstances->Append(wxString(vecModel[iInstance]->getUniqueName()));

		m_lbInstances->SetClientData(iItem, vecModel[iInstance]);

		m_mapInstance2Item[vecModel[iInstance]] = iItem;
	}
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void InstancesDialog::OnInstanceSelected(CRDFView * pSender)
{
    if (pSender == this)
	{
		return;
	}

    m_lbInstances->DeselectAll();

	assert(GetController() != NULL);

	CRDFInstance * pSelectedInstance = GetController()->GetSelectedInstance();
	if (pSelectedInstance == NULL)
	{
		return;
	}

	map<CRDFInstance *, int>::iterator itInstance2Item = m_mapInstance2Item.find(pSelectedInstance);
	assert(itInstance2Item != m_mapInstance2Item.end());

	m_lbInstances->SetSelection(itInstance2Item->second);
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void InstancesDialog::OnInstanceDeleted(CRDFView * pSender, int64_t iInstance)
{
    OnModelChanged();
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void InstancesDialog::OnInstancesDeleted(CRDFView * pSender)
{
    OnModelChanged();
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void InstancesDialog::OnControllerChanged()
{
    assert(GetController() != NULL);

	GetController()->RegisterView(this);
}

#define ID_ZOOM_TO 2000
#define ID_ENABLE 2001
#define ID_DISABLE 2002
#define ID_DISABLE_ALL_BUT_THIS 2003
#define ID_ENABLE_ALL 2004
#define ID_REMOVE 2005

// ------------------------------------------------------------------------------------------------
void InstancesDialog::OnListboxRDown(wxMouseEvent& event)
{
    /*
    * Multi selection
    */
    wxArrayInt arSelections;
    if (m_lbInstances->GetSelections(arSelections) > 1)
    {
        bool bIsGeometryInstanceSelected = false;
        for (int iItem = 0; iItem < arSelections.GetCount(); iItem++)
        {
            CRDFInstance * pRDFInstance = (CRDFInstance *)m_lbInstances->GetClientData(arSelections[iItem]);
            assert(pRDFInstance != NULL);

            if (pRDFInstance->hasGeometry())
            {
                bIsGeometryInstanceSelected = true;

                break;
            }
        }

        wxMenu menu;
        if (bIsGeometryInstanceSelected)
        {
            menu.Append(ID_ENABLE, "Enable");
            menu.Append(ID_DISABLE, "Disable");
            menu.AppendSeparator();
        }

        menu.Append(ID_REMOVE, "Remove");

        menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(InstancesDialog::OnPopupClick), NULL, this);

        PopupMenu(&menu);

        return;
    } // if (m_lbInstances->GetSelections(arSelections) > 1)

    /*
    * Single selection
    */
    int iItem = m_lbInstances->HitTest(event.GetPosition());
    if (iItem != wxNOT_FOUND)
    {
        m_lbInstances->DeselectAll();

        m_lbInstances->SetSelection(iItem);

        CRDFInstance * pRDFInstance = (CRDFInstance *)m_lbInstances->GetClientData(iItem);
        assert(pRDFInstance != NULL);

        GetController()->SelectInstance(this, pRDFInstance);
    }

    m_lbInstances->GetSelections(arSelections);
    if (m_lbInstances->GetSelections(arSelections) == 0)
    {
        return;
    }

    CRDFInstance * pRDFInstance = (CRDFInstance *)m_lbInstances->GetClientData(arSelections[0]);
    assert(pRDFInstance != NULL);

    wxMenu menu;
    if (pRDFInstance->hasGeometry())
    {
        menu.Append(ID_ZOOM_TO, "Zoom to");
        menu.AppendSeparator();
        menu.AppendCheckItem(ID_ENABLE, "Enable")->Check(pRDFInstance->enable());
        menu.Append(ID_DISABLE_ALL_BUT_THIS, "Disable All But This");
        menu.Append(ID_ENABLE_ALL, "Enable All");
        menu.AppendSeparator();
    }

    menu.Append(ID_REMOVE, "Remove");

    menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(InstancesDialog::OnPopupClick), NULL, this);

    PopupMenu(&menu);
}

// ------------------------------------------------------------------------------------------------
void InstancesDialog::OnPopupClick(wxCommandEvent &event)
{
    wxArrayInt arSelections;
    m_lbInstances->GetSelections(arSelections);

    assert(GetController() != NULL);

    CRDFModel * pModel = GetController()->GetModel();
    assert(pModel != NULL);

	const map<int64_t, CRDFInstance *> & mapRFDInstances = pModel->GetRDFInstances();

 	switch(event.GetId())
 	{
    case ID_ZOOM_TO:
        {
            assert(arSelections.GetCount() == 1);

            CRDFInstance * pRDFInstance = (CRDFInstance *)m_lbInstances->GetClientData(arSelections[0]);
            assert(pRDFInstance != NULL);

            GetController()->ZoomToInstance(pRDFInstance->getInstance());
        }
        break;

    case ID_ENABLE:
        {
            if (arSelections.GetCount() == 1)
            {
                CRDFInstance * pRDFInstance = (CRDFInstance *)m_lbInstances->GetClientData(arSelections[0]);
                assert(pRDFInstance != NULL);

                pRDFInstance->enable() = !pRDFInstance->enable();
            }
            else
            {
                for (int iItem = 0; iItem < arSelections.GetCount(); iItem++)
                {
                    CRDFInstance * pRDFInstance = (CRDFInstance *)m_lbInstances->GetClientData(arSelections[iItem]);
                    assert(pRDFInstance != NULL);

                    pRDFInstance->enable() = true;
                }
            }

			GetController()->OnInstancesEnabledStateChanged();
        }
        break;

    case ID_DISABLE:
        {
            assert(arSelections.GetCount() > 1);

            for (int iItem = 0; iItem < arSelections.GetCount(); iItem++)
            {
                CRDFInstance * pRDFInstance = (CRDFInstance *)m_lbInstances->GetClientData(arSelections[iItem]);
                assert(pRDFInstance != NULL);

                pRDFInstance->enable() = false;
            }

            GetController()->OnInstancesEnabledStateChanged();
        }
        break;

    case ID_DISABLE_ALL_BUT_THIS:
        {
            assert(arSelections.GetCount() == 1);

            CRDFInstance * pRDFInstance = (CRDFInstance *)m_lbInstances->GetClientData(arSelections[0]);
            assert(pRDFInstance != NULL);

            map<int64_t, CRDFInstance *>::const_iterator itRFDInstances = mapRFDInstances.begin();
			for (; itRFDInstances != mapRFDInstances.end(); itRFDInstances++)
			{
				if ((pRDFInstance->GetModel() != itRFDInstances->second->GetModel()) || !itRFDInstances->second->hasGeometry())
				{
					continue;
				}

				if (itRFDInstances->second == pRDFInstance)
				{
					itRFDInstances->second->enable() = true;

					continue;
				}

				itRFDInstances->second->enable() = false;
			}

			GetController()->OnInstancesEnabledStateChanged();
        }
        break;

    case ID_ENABLE_ALL:
        {
            assert(arSelections.GetCount() == 1);

            CRDFInstance * pRDFInstance = (CRDFInstance *)m_lbInstances->GetClientData(arSelections[0]);
            assert(pRDFInstance != NULL);

            map<int64_t, CRDFInstance *>::const_iterator itRFDInstances = mapRFDInstances.begin();
			for (; itRFDInstances != mapRFDInstances.end(); itRFDInstances++)
			{
				if ((pRDFInstance->GetModel() != itRFDInstances->second->GetModel()) || !itRFDInstances->second->hasGeometry())
				{
					continue;
				}

				itRFDInstances->second->enable() = true;
			}

			GetController()->OnInstancesEnabledStateChanged();
        }
        break;

    case ID_REMOVE:
        {
            if (arSelections.GetCount() == 1)
            {
                CRDFInstance * pRDFInstance = (CRDFInstance *)m_lbInstances->GetClientData(arSelections[0]);
                assert(pRDFInstance != NULL);

                GetController()->DeleteInstance(this, pRDFInstance);
            }
            else
            {
                vector<CRDFInstance *> vecInstances;
                for (int iItem = 0; iItem < arSelections.GetCount(); iItem++)
                {
                    CRDFInstance * pRDFInstance = (CRDFInstance *)m_lbInstances->GetClientData(arSelections[iItem]);
                    assert(pRDFInstance != NULL);

                    vecInstances.push_back(pRDFInstance);
                }

                GetController()->DeleteInstances(this, vecInstances);
            }
        }
        break;

    default:
        {
            assert(false); // unknown
        }
        break;
 	} // switch(event.GetId())
}
