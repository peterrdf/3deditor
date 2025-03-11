#include "stdafx.h"
#include "RDFInstance.h"
#include "Generic.h"

#include <memory>

// ************************************************************************************************
CRDFInstance::CRDFInstance(int64_t iID, OwlInstance iInstance)
	: _rdf_instance(iID, nullptr, nullptr)
{
}

/*virtual*/ CRDFInstance::~CRDFInstance()
{
}

