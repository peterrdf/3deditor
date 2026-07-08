#pragma once

#include "_ap_mvc.h"
#include "_ap242_property.h"

#include <mutex>
#include <thread>

using namespace std;

// ************************************************************************************************
class _ap242_product_definition;
class _ap242_product_shape;
class _ap242_product_shape_representation;
class _ap242_assembly;
class _ap242_draughting_model;
class _ap242_annotation_plane;
class _ap242_draughting_callout;
class _ap242_model_structure;

// ************************************************************************************************
class _ap242_model : public _ap_model
{

private: // Classes

    struct AP242_REPRESENTATION_ITEM
    {
		_ap242_product_shape_representation* pProductShapeRepresentation = nullptr;
		SdaiInstance sdaiRepresentationInstance = 0;
        SdaiInstance sdaiRepresentationItemInstance = 0;
    };

    struct AP242_ANNOTATION_PLANE
    {
        _ap242_draughting_model* pDraughtingModel = nullptr;
        SdaiInstance sdaiInstance = 0;
    };

    struct AP242_DRAUGHTING_CALLOUT
    {
        _ap242_draughting_model* pDraughtingModel = nullptr;
        SdaiInstance sdaiInstance = 0;
    };

private: // Members

    bool m_bLoadProductRepresentationItems;
    bool m_bLoadInstancesOnDemand;

	_ap242_model_structure* m_pModelStructure;
    _ap242_property_provider* m_pPropertyProvider;

	map <SdaiInstance, AP242_REPRESENTATION_ITEM> m_mapRepresentationItemsPendingLoad; // SDAI Instance : Representation Item
	map<SdaiInstance, AP242_ANNOTATION_PLANE> m_mapAnnotationPlanesPendingLoad; // SDAI Instance : Annotation Plane
	map<SdaiInstance, AP242_DRAUGHTING_CALLOUT> m_mapDraughtingCalloutsPendingLoad; // SDAI Instance : Draughting Callout
    mutex m_mtxGeometriesPendingLoad;
    mutex m_mtxUpdateModel;

    map<ExpressID, _ap242_assembly*> m_mapExpressID2Assembly; // Express ID : Assembly
    vector<_ap242_draughting_model*> m_vecDraughtingModels;

public: // Methods

    _ap242_model(_log* pLog, bool bLoadProductRepresentationItem, bool bLoadInstancesOnDemand);
    virtual ~_ap242_model();

    _ap242_assembly* getAssemblyByInstance(SdaiInstance sdaiInstance) const;

protected: // Methods

    // _model
    virtual _instance* loadInstance(int64_t iInstance) override;
    virtual void clean(bool bCloseModel = true) override;

    // _ap_model
    virtual void attachModelCore() override;

private: // Methods

    void loadProductDefinitions();
    void loadProductDefinitionShapes(_ap242_product_definition* pProductDefinition);
    void loadProductDefinitionShape(_ap242_product_definition* pProductDefinition, SdaiInstance sdaiProductDefinitionShapeInstance);
    void loadShapeRepresentationItems(_ap242_product_shape_representation* pProductShapeRepresentation, SdaiInstance sdaiRepresentationInstance);
    void loadRepresentationItems(_ap242_product_shape_representation* pProductShapeRepresentation, SdaiInstance sdaiRepresentationInstance);
    _ap242_product_definition* loadProductDefinition(SdaiInstance sdaiProductDefinitionInstance);
    _ap242_product_definition* getProductDefinition(SdaiInstance sdaiProductDefinitionInstance, bool bRelatingProduct, bool bRelatedProduct);
    void loadAssemblies();
    void loadGeometry();
    void walkAssemblyTreeRecursively(_ap242_product_definition* pProductDefinition, _ap242_assembly* pAssembly, _matrix4x3* pParentMatrix);

    void loadDraughtingModels();
    _ap242_annotation_plane* loadAnnotationPlane(_ap242_draughting_model* pDraughtingModel, SdaiInstance sdaiInstance);
    _ap242_draughting_callout* loadDraughtingCallout(_ap242_draughting_model* pDraughtingModel, SdaiInstance sdaiInstance);

    void save(const wchar_t* szPath);

public: // Properties

    const map<ExpressID, _ap242_assembly*>& getExpressID2Assembly() const { return m_mapExpressID2Assembly; }
    const vector<_ap242_draughting_model*>& getDraughtingModels() const { return m_vecDraughtingModels; }
    _ap242_model_structure* getModelStructure();
    _ap242_property_provider* getPropertyProvider();
};