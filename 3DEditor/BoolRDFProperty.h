#pragma once

#include "RDFProperty.h"

// ------------------------------------------------------------------------------------------------
class CBoolRDFProperty : public CRDFProperty
{

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CBoolRDFProperty(int64_t iInstance);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CBoolRDFProperty();
};

