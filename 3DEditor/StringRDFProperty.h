#pragma once

#include "RDFProperty.h"

// ------------------------------------------------------------------------------------------------
class CStringRDFProperty : public CRDFProperty
{

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CStringRDFProperty(int64_t iInstance);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CStringRDFProperty();
};

