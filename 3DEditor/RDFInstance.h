#pragma once

#include "_oglUtils.h"
#include "_instance.h"
#include "_rdf_instance.h"

// ************************************************************************************************
class CRDFInstance : public _rdf_instance
{

public: // Methods

	CRDFInstance(int64_t iID, OwlInstance iInstance);
	virtual ~CRDFInstance();
};
