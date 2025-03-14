#pragma once

#include "_rdf_mvc.h"
#include "RDFInstance.h"
#include "_rdf_property.h"

// ------------------------------------------------------------------------------------------------
class CRDFController;


// ------------------------------------------------------------------------------------------------
// View - MVC
class CRDFView : public _rdf_view
{

private: // Members

	// --------------------------------------------------------------------------------------------
	// Controller - MVC
	CRDFController* m_pController;

public: // Methods

	CRDFView();
	virtual ~CRDFView();

	// --------------------------------------------------------------------------------------------
	// An RDF Property has been modified
	virtual void OnInstancePropertyEdited(CRDFInstance* pInstance, _rdf_property* pProperty);

	// --------------------------------------------------------------------------------------------
	// An RDF Instance has been deleted
	virtual void OnInstanceDeleted(CRDFView* pSender, int64_t iInstance);	

	// --------------------------------------------------------------------------------------------
	// RDF Instances have been deleted
	virtual void OnInstancesDeleted(CRDFView* pSender);

	// --------------------------------------------------------------------------------------------
	// Measurements have been added
	virtual void OnMeasurementsAdded(CRDFView* pSender, CRDFInstance* pInstance);

	// --------------------------------------------------------------------------------------------
	// Enable/Disable support
	virtual void OnInstancesEnabledStateChanged();

protected: // Methods

	// --------------------------------------------------------------------------------------------
	// Getter
	CRDFController* GetController() const;

	// --------------------------------------------------------------------------------------------
	// The controller has been changed
	virtual void OnControllerChanged();
};

