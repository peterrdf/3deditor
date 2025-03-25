#pragma once 

#include "_ap_geometry.h"

// ************************************************************************************************
class _ap242_geometry : public _ap_geometry
{

public: // Methods

    _ap242_geometry(OwlInstance owlInstance, SdaiInstance sdaiInstance);
    virtual ~_ap242_geometry();

protected: // Methods

    // _geometry
    virtual void preCalculate() override;
};