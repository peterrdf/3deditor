#pragma once

#include "_ap_geometry.h"

// ************************************************************************************************
class _ifc_geometry : public _ap_geometry
{
    friend class _ifc_model;

private: // Members

    vector<_ifc_geometry*> m_vecMappedGeometries;

    bool m_bIsMappedItem;
    bool m_bIsReferenced;

public: // Methods

    _ifc_geometry(OwlInstance owlInstance, SdaiInstance sdaiInstance, const vector<_ifc_geometry*>& vecMappedGeometries);
    virtual ~_ifc_geometry();

protected: // Methods

    // _geometry
    virtual void preCalculate() override;
    virtual void postCalculate() override;
    virtual bool hasGeometry() const override;
    virtual bool isPlaceholder() const override;
    virtual bool ignoreBB() const override;

public: // Methods

    void setDefaultShowState();

public: // Properties

    bool getIsMappedItem() const { return m_bIsMappedItem; }
    bool getIsReferenced() const { return m_bIsReferenced; }
};