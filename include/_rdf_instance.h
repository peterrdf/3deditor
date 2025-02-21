#pragma once

#include "_instance.h"
#include "_rdf_geometry.h"

// ************************************************************************************************
class _rdf_instance : public _instance
{

public:  // Methods

	_rdf_instance(int64_t iID, _geometry* pGeometry, _matrix4x3* pTransformationMatrix);
	virtual ~_rdf_instance();
	
public: // Properties

	virtual wstring getName() const override { return getGeometry()->getName(); }
	wstring getUniqueName() const { return getGeometry()->getUniqueName(); }
};