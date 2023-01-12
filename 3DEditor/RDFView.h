#pragma once

#include "RDFInstance.h"
#include "RDFProperty.h"

// ------------------------------------------------------------------------------------------------
class CRDFController;

// ------------------------------------------------------------------------------------------------
// View - MVC
class CRDFView
{

private: // Members

	// --------------------------------------------------------------------------------------------
	// Controller - MVC
	CRDFController * m_pController;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CRDFView();

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CRDFView();

	// --------------------------------------------------------------------------------------------
	// Sets up Controller
	void SetController(CRDFController * pController);

	// --------------------------------------------------------------------------------------------
	// An RDF model has been loaded
	virtual void OnModelChanged();

	// --------------------------------------------------------------------------------------------
	// RDF model has been recalculated
	virtual void OnWorldDimensionsChanged();

	// --------------------------------------------------------------------------------------------
	// Base information support
	virtual void OnShowBaseInformation();

	// --------------------------------------------------------------------------------------------
	// Meta information support
	virtual void OnShowMetaInformation();

	// --------------------------------------------------------------------------------------------
	// An RDF Instance has been selected
	virtual void OnInstanceSelected(CRDFView * pSender);

	// --------------------------------------------------------------------------------------------
	// An RDF Property has been selected
	virtual void OnInstancePropertySelected();

	// --------------------------------------------------------------------------------------------
	// An RDF Property has been modified
	virtual void OnInstancePropertyEdited(CRDFInstance * pInstance, CRDFProperty * pProperty);

	// --------------------------------------------------------------------------------------------
	// An RDF Instance has been created
	virtual void OnNewInstanceCreated(CRDFView * pSender, CRDFInstance * pInstance);

	// --------------------------------------------------------------------------------------------
	// An RDF Instance has been deleted
	virtual void OnInstanceDeleted(CRDFView * pSender, int64_t iInstance);	

	// --------------------------------------------------------------------------------------------
	// RDF Instances have been deleted
	virtual void OnInstancesDeleted(CRDFView * pSender);

	// --------------------------------------------------------------------------------------------
	// Measurements have been added
	virtual void OnMeasurementsAdded(CRDFView * pSender, CRDFInstance * pInstance);

	// --------------------------------------------------------------------------------------------
	// Enable/Disable support
	virtual void OnInstancesEnabledStateChanged();

	// --------------------------------------------------------------------------------------------
	// UI properties support
	virtual void OnVisibleValuesCountLimitChanged();

protected: // Methods

	// --------------------------------------------------------------------------------------------
	// Getter
	CRDFController * GetController() const;

	// --------------------------------------------------------------------------------------------
	// The controller has been changed
	virtual void OnControllerChanged();
};

