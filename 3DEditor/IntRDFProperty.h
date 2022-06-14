#pragma once

#include "RDFProperty.h"

// ------------------------------------------------------------------------------------------------
class CIntRDFProperty : public CRDFProperty
{

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CIntRDFProperty(int64_t iInstance);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CIntRDFProperty();
};

