#pragma once

#include "RDFProperty.h"
#include <vector>

using namespace std;

// ------------------------------------------------------------------------------------------------
class CObjectRDFProperty : public CRDFProperty
{

private: // Members

	// --------------------------------------------------------------------------------------------
	// Restrictions
	vector<int64_t> m_vecRestrictions;


public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CObjectRDFProperty(int64_t iInstance);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CObjectRDFProperty();

	// --------------------------------------------------------------------------------------------
	// Getter
	const vector<int64_t> & getRestrictions();
};

