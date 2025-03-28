#pragma once

#ifdef _WINDOWS
#include "stepengine.h"
#else
#include "../../include/stepengine.h"
#endif

#include <map>
#include <vector>
using namespace std;

// ************************************************************************************************
class _attribute
{

private: // Members

    SdaiAttr m_sdaiAttr;

public: // Methods

    _attribute(SdaiAttr sdaiAttr);
    virtual ~_attribute();

    SdaiAttr getSdaiAttr() const { return m_sdaiAttr; }
    SdaiPrimitiveType getSdaiPrimitiveType() const;
};

// ************************************************************************************************
class _attribute_provider
{

private: // Members

    map<SdaiInstance, vector<_attribute*>> m_mapInstanceAttributes;

public: // Methods

    _attribute_provider();
    virtual ~_attribute_provider();

    const vector<_attribute*>& getInstanceAttributes(SdaiInstance sdaiInstance);

private: // Methods

    void loadInstanceAttributes(SdaiEntity sdaiEntity, SdaiInstance sdaiInstance, vector<_attribute*>& vecAttributes);
};

