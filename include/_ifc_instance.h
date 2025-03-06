#pragma once

#include "_ap_instance.h"

// ************************************************************************************************
class _ifc_geometry;

// ************************************************************************************************
class _ifc_instance : public _ap_instance
{
	friend class _ifc_model;

private: // Members

	// not null when this instance is IfcMappedItem
	_ifc_instance* m_pOwner;

public:  // Methods

	_ifc_instance(int64_t iID, _ifc_geometry* pGeometry, _matrix4x3* pTransformationMatrix);
	virtual ~_ifc_instance();

	// _instance
	virtual _instance* getOwner() const override;

	void setDefaultEnableState();
};