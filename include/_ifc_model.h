#pragma once

#include "_ap_mvc.h"
#include "_ifc_unit.h"
#include "_ifc_property.h"

#include <string>
#include <map>
#include <set>
using namespace std;

// ************************************************************************************************
static bool	Equals(const char* txtI, const char* txtII)
{
    if (txtI && txtII) {
        size_t	i = 0;
        while (txtI[i] && txtII[i])
            if (txtI[i] != txtII[i])
                return	false;
            else
                i++;
        if (txtII[i] == 0)
            return	true;
    }
    return  false;
}

struct STRUCT_MATERIAL
{
    uint32_t ambient;
    uint32_t diffuse;
    uint32_t emissive;
    uint32_t specular;
};

struct STRUCT_INTERNAL
{
    SdaiInstance ifcRepresentationInstance;
    OwlInstance owlInstanceMatrix;
    //STRUCT_MATERIAL* material;
};

struct STRUCT_MAPPED_ITEM
{
    SdaiInstance ifcRepresentationInstance;
    _matrix4x3 matrix;
    //STRUCT_MATERIAL* material;
};

struct STRUCT_IFC_PRODUCT
{
    SdaiInstance ifcProductInstance;
    std::vector<STRUCT_MAPPED_ITEM*> mappedItems;
};

// ************************************************************************************************
class _ifc_geometry;
class _ifc_instance;

// ************************************************************************************************
class _ifc_model : public _ap_model
{

private: // Members

    // Load
    bool m_bUseWorldCoordinates;
    bool m_bLoadInstancesOnDemand;

    // Entities
    SdaiEntity m_sdaiSpaceEntity;
    SdaiEntity m_sdaiOpeningElementEntity;
    SdaiEntity m_sdaiDistributionElementEntity;
    SdaiEntity m_sdaiElectricalElementEntity;
    SdaiEntity m_sdaiElementAssemblyEntity;
    SdaiEntity m_sdaiElementComponentEntity;
    SdaiEntity m_sdaiEquipmentElementEntity;
    SdaiEntity m_sdaiFeatureElementEntity;
    SdaiEntity m_sdaiFeatureElementSubtractionEntity;
    SdaiEntity m_sdaiFurnishingElementEntity;
    SdaiEntity m_sdaiReinforcingElementEntity;
    SdaiEntity m_sdaiTransportElementEntity;
    SdaiEntity m_sdaiVirtualElementEntity;

    _ifc_unit_provider* m_pUnitProvider;
    _ifc_property_provider* m_pPropertyProvider;

    vector<pair<_instance*, STRUCT_MAPPED_ITEM*>> m_vecMappedItemPendingUpdate;

public: // Methods

    _ifc_model(bool bUseWorldCoordinates = false, bool bLoadInstancesOnDemand = false);
    virtual ~_ifc_model();

protected: // Methods

    // _model
    virtual _instance* loadInstance(int64_t iInstance) override;
    virtual void zoomToInstances(const set<_instance*>& setInstances) override;
    virtual void resetInstancesEnabledState() override;
    virtual void clean(bool bCloseModel = true) override;

    // _ap_model
    virtual void attachModelCore() override;

protected: // Methods

    virtual _ifc_geometry* createGeometry(OwlInstance owlInstance, SdaiInstance sdaiInstance);
    virtual _ifc_instance* createInstance(int64_t iID, _ifc_geometry* pGeometry, _matrix4x3* pTransformationMatrix);

private: // Methods

    void getObjectsReferencedState();
    void getObjectsReferencedStateGroups();
    void getObjectsReferencedStateIsDecomposedBy(SdaiInstance sdaiInstance);
    void getObjectsReferencedStateIsNestedBy(SdaiInstance sdaiInstance);
    void getObjectsReferencedStateContainsElements(SdaiInstance sdaiInstance);
    void getObjectsReferencedStateHasAssignments(SdaiInstance sdaiInstance);
    void getObjectsReferencedStateBoundedBy(SdaiInstance sdaiInstance);
    void getObjectsReferencedStateHasOpenings(SdaiInstance sdaiInstance);
    void getObjectsReferencedStateRecursively(SdaiInstance sdaiInstance);

    void retrieveGeometryRecursively(SdaiEntity sdaiParentEntity, SdaiInteger iCircleSegments);
    void retrieveGeometry(const char* szEntityName, SdaiInteger iCircleSegements);
    _geometry* loadGeometry(const char* szEntityName, SdaiInstance sdaiInstance, bool bMappedItem, SdaiInteger iCircleSegments);

    STRUCT_IFC_PRODUCT* recognizeMappedItems(SdaiInstance ifcProductInstance);
    void parseMappedItem(SdaiInstance ifcMappedItemInstance, std::vector<STRUCT_INTERNAL*>* pVectorMappedItemData);

public: // Properties

    _ifc_unit_provider* getUnitProvider();
    _ifc_property_provider* getPropertyProvider();
};


