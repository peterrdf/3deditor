#pragma once 

#include "_ap_geometry.h"

// ************************************************************************************************
class _ap242_geometry : public _ap_geometry
{

    private: // Members

        SdaiInstance m_sdaiBuildContextInstance;

public: // Methods

    _ap242_geometry(OwlInstance owlInstance, SdaiInstance sdaiInstance, MultiThreadOwlModelWrapper multiThreadOwlModelWrapper);
    virtual ~_ap242_geometry();

protected: // Methods

    // _geometry
    virtual void preCalculate() override;
    virtual void postCalculate() override;

public: // Properties

	SdaiInstance getSdaiBuildContextInstance() const { return m_sdaiBuildContextInstance; }
	void setSdaiBuildContextInstance(SdaiInstance sdaiBuildContextInstance) { m_sdaiBuildContextInstance = sdaiBuildContextInstance; }
};