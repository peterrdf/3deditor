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

public: // Methods

	CRDFView();
	virtual ~CRDFView();

	// --------------------------------------------------------------------------------------------
	// An RDF Property has been modified
	virtual void OnInstancePropertyEdited(CRDFInstance* pInstance, _rdf_property* pProperty);
};

