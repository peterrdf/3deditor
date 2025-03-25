#pragma once

#include "_ptr.h"
#include "_mvc.h"
#include "_entity.h"
#include "_attribute.h"
#include "_ap_instance.h"

#include <string>
using namespace std;

// ************************************************************************************************
class _ap_controller;

// ************************************************************************************************
// Application Protocol
enum class enumAP : int
{
    Unknown = -1,
    STEP = 0,
    IFC = 1,
    CIS2 = 2,
};

// ************************************************************************************************
class _ap_model : public _model
{

private: // Members

    // Model
    SdaiModel m_sdaiModel;
    enumAP m_enAP;

    // Helpers
    _entity_provider* m_pEntityProvider;
    _attribute_provider* m_pAttributeProvider;

    // Cache
    map<ExpressID, _geometry*> m_mapExpressID2Geometry;
    map<SdaiInstance, _geometry*> m_mapGeometries;

public: // Methods

    _ap_model(enumAP enAP);
    virtual ~_ap_model();

    // _model
    virtual OwlModel getOwlModel() const override;

    bool openModel(const wchar_t* szPath);
    void attachModel(const wchar_t* szPath, SdaiModel sdaiModel, _model* pWorld);

    _geometry* getGeometryByInstance(SdaiInstance sdaiInstance);
    _geometry* getGeometryByExpressID(ExpressID iExpressID) const;
    template<typename T>
    T* getGeometryByExpressIDAs(ExpressID iExpressID) const;
    void getGeometriesByType(const char* szType, vector<_ap_geometry*>& vecGeometries);

protected: // Methods

    virtual void attachModelCore() PURE;
    virtual void preLoadInstance(OwlInstance owlInstance);
    void addGeometry(_ap_geometry* pGeometry);

    virtual void clean(bool bCloseModel = true) override;

public: // Properties

    // Model
    SdaiModel getSdaiModel() const { return m_sdaiModel; }
    enumAP getAP() const { return m_enAP; }

    // Cache	
    const map<ExpressID, _geometry*>& getExpressID2Geometry() const { return m_mapExpressID2Geometry; }

    // Helpers
    _entity_provider* getEntityProvider();
    _attribute_provider* getAttributeProvider();
};

// ************************************************************************************************
class _ap_view : public _view
{

public: // Methods

    _ap_view();
    virtual ~_ap_view();

    // Events	
    virtual void onViewRelations(_view* /*pSender*/, SdaiInstance /*sdaiInstance*/) {}
    virtual void onViewRelations(_view* /*pSender*/, _entity* /*pEntity*/) {}
    virtual void onInstanceAttributeEdited(_view* /*pSender*/, SdaiInstance /*sdaiInstance*/, SdaiAttr /*sdaiAttribute*/) {}

public: // Properties

    _ap_controller* getAPController() const;
};

// ************************************************************************************************
class _ap_controller : public _controller
{

public: // Methods

    _ap_controller();
    virtual ~_ap_controller();

    // Events
    void onViewRelations(_view* pSender, SdaiInstance sdaiInstance);
    void onViewRelations(_view* pSender, _entity* pEntity);
    void onInstanceAttributeEdited(_view* pSender, SdaiInstance sdaiInstance, SdaiAttr sdaiAttr);
};