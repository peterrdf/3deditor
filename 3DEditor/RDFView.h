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

protected: // Methods

	// --------------------------------------------------------------------------------------------
	// Getter
	CRDFController* GetController() const;

	// --------------------------------------------------------------------------------------------
	// The controller has been changed
	virtual void OnControllerChanged();
};

