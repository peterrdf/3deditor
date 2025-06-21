#pragma once

#include "_instance.h"
#include "_rdf_geometry.h"

// ************************************************************************************************
#define EMPTY_INSTANCE L"---<EMPTY>---"

// ************************************************************************************************
class _rdf_instance : public _instance
{
private: // Fields

    bool m_bNeedsRefresh; // Geometry is out of date

public:  // Methods

    _rdf_instance(int64_t iID, _geometry* pGeometry, _matrix4x3* pTransformationMatrix);
    virtual ~_rdf_instance();

    // instance
    virtual void setEnable(bool bEnable) override;

    void recalculate(bool bForce = false);

public: // Properties

    virtual const wchar_t* getName() const override { return getGeometry()->getName(); }
    virtual const wchar_t* getUniqueName() const override { return getGeometry()->getUniqueName(); }
    bool getDesignTreeConsistency() { return CheckInstanceConsistency(getOwlInstance(), FLAGBIT(0)) == 0; }
};