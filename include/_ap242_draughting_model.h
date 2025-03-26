#pragma once

#include "_ap242_geometry.h"

#include <string>
#include <vector>
using namespace std;

// ************************************************************************************************
class _ap242_annotation_plane;
class _ap242_draughting_callout;

// ************************************************************************************************
class _ap242_draughting_model
{
    friend class _ap242_model;

private: // Members

    SdaiInstance m_sdaiInstance;
    wstring m_strName;

    // Reperesentation
    vector<_ap242_annotation_plane*> m_vecAnnotationPlanes;
    vector<_ap242_draughting_callout*> m_vecDraughtingCallouts;

public: // Methods

    _ap242_draughting_model(SdaiInstance sdaiInstance);
    virtual ~_ap242_draughting_model();

    void enableInstances(bool bEnable);

public: // Properties

    SdaiInstance getSdaiInstance() const { return m_sdaiInstance; }
    ExpressID getExpressID() const { return internalGetP21Line(m_sdaiInstance); }
    const wchar_t* getName() { return m_strName.c_str(); }

    const vector<_ap242_annotation_plane*>& getAnnotationPlanes() const { return m_vecAnnotationPlanes; }
    const vector<_ap242_draughting_callout*> getDraughtingCallouts() const { return m_vecDraughtingCallouts; }
};

// ************************************************************************************************
class _ap242_annotation_plane : public _ap242_geometry
{

private: // Members

    wstring m_strName;

public: // Methods

    _ap242_annotation_plane(OwlInstance owlInstance, SdaiInstance sdaiInstance);
    virtual ~_ap242_annotation_plane();

public: // Properties

    const wchar_t* getName() { return m_strName.c_str(); }
};

// ************************************************************************************************
class _ap242_draughting_callout : public _ap242_geometry
{

private: // Members

    wstring m_strName;

public: // Methods

    _ap242_draughting_callout(OwlInstance owlInstance, SdaiInstance sdaiInstance);
    virtual ~_ap242_draughting_callout();

public: // Properties

    const wchar_t* getName() { return m_strName.c_str(); }
};