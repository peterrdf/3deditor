#pragma once

#include "RDFProperty.h"

// ------------------------------------------------------------------------------------------------
class CUndefinedRDFProperty : public CRDFProperty
{

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CUndefinedRDFProperty(int64_t iInstance);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CUndefinedRDFProperty();
};

