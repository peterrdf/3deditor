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

// ------------------------------------------------------------------------------------------------
class CCharArrayRDFProperty : public CRDFProperty
{

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CCharArrayRDFProperty(int64_t iInstance);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CCharArrayRDFProperty();
};

// ------------------------------------------------------------------------------------------------
class CWCharArrayRDFProperty : public CRDFProperty
{

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CWCharArrayRDFProperty(int64_t iInstance);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CWCharArrayRDFProperty();
};

