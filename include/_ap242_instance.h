#pragma once

#include "_ap_instance.h"

// ************************************************************************************************
class _ap242_geometry;
class _ap242_product_definition;

// ************************************************************************************************
class _ap242_instance : public _ap_instance
{

private: // Members

    wstring m_strName;

public:  // Methods

    _ap242_instance(int64_t iID, _ap242_geometry* pGeometry, _matrix4x3* pTransformationMatrix);
    virtual ~_ap242_instance();

public:  // Properties

public: // Properties

    // _instance
    virtual const wchar_t* getName() const override;

    _ap242_product_definition* getProductDefinition() const;
};