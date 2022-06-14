#pragma once

#include "RDFItem.h"

// ------------------------------------------------------------------------------------------------
class CRDFInstanceItem
	: public CRDFItem
{

public:

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CRDFInstanceItem(CRDFInstance * pInstance);

public: // Methods

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CRDFInstanceItem();
};

