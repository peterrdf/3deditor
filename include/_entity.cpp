#include "_host.h"
#include "_entity.h"

#include <algorithm>

// ************************************************************************************************
_entity::_entity(SdaiEntity sdaiEntity)
    : m_sdaiEntity(sdaiEntity)
    , m_szName(nullptr)
    , m_pParent(nullptr)
    , m_vecAttributes()
    , m_vecSubTypes()
    , m_vecInstances()
{
    assert(m_sdaiEntity != 0);

    SdaiModel sdaiModel = engiGetEntityModel(sdaiEntity);
    assert(sdaiModel != 0);

    m_szName = _entity::getName(m_sdaiEntity);

    // Attributes
    SdaiInteger iAttributesCount = engiGetEntityNoArguments(sdaiEntity);
    for (SdaiInteger iIndex = 0; iIndex < iAttributesCount; iIndex++) {
        wchar_t* szArgumentName = 0;
        engiGetEntityArgumentName(m_sdaiEntity, iIndex, sdaiUNICODE, (const char**)&szArgumentName);

        m_vecAttributes.push_back(szArgumentName);
    }

    // Instances
    SdaiAggr sdaiAggr = sdaiGetEntityExtent(sdaiModel, m_sdaiEntity);
    SdaiInteger iInstancesCount = sdaiGetMemberCount(sdaiAggr);

    SdaiInteger iIndex = 0;
    while (iIndex < iInstancesCount) {
        SdaiInstance iInstance = 0;
        sdaiGetAggrByIndex(sdaiAggr, iIndex++, sdaiINSTANCE, &iInstance);

        m_vecInstances.push_back(iInstance);
    }
}

_entity::~_entity()
{}

/*static*/ wchar_t* _entity::getName(SdaiEntity sdaiEntity)
{
    wchar_t* szName = nullptr;
    engiGetEntityName(sdaiEntity, sdaiUNICODE, (const char**)&szName);

    return szName;
}

bool _entity::isAttributeInherited(const wstring& strAttribute) const
{
    if (m_pParent != nullptr) {
        const vector<wstring>& vecParentAttributes = m_pParent->getAttributes();

        return find(vecParentAttributes.begin(), vecParentAttributes.end(), strAttribute) != vecParentAttributes.end();
    }

    return false;
}

void _entity::postProcessing()
{
    if (!m_vecSubTypes.empty()) {
        std::sort(m_vecSubTypes.begin(), m_vecSubTypes.end(), _sort_entities());
    }
}

// ************************************************************************************************
_entity_provider::_entity_provider(SdaiModel sdaiModel)
    : m_sdaiModel(sdaiModel)
    , m_mapEntities()
{
    assert(m_sdaiModel != 0);

    load();
}

/*virtual*/ _entity_provider::~_entity_provider()
{
    for (auto itEntity : m_mapEntities) {
        delete itEntity.second;
    }
}

static void GetEntityParentsToDisplay(SdaiEntity entity, vector<SdaiEntity>& parents)
{
    // Get all parents
    SdaiInteger iParent = 0;
    while (SdaiEntity parent = engiGetEntityParentEx(entity, iParent++)) {
        parents.push_back(parent);
    }

    //for complex entity filter only top/leaf parents to display
    if (engiIsComplexEntity(entity)) {
        for (size_t i = 0; i < parents.size();) {

            bool isParent = false;
            for (size_t j = 0; j < parents.size() && !isParent; j++) {
                if (i != j && engiIsParentOf(parents[i], parents[j])) {
                    isParent = true;
                }
            }

            if (isParent) {
                parents.erase(parents.begin() + i);
            }
            else {
                i++;
            }
        }
    }
        
}

void _entity_provider::load()
{
    SdaiInteger iEntitiesCount = engiGetEntityCount(m_sdaiModel);

    // Retrieve the Entities
    SdaiInteger iIndex = 0;
    while (iIndex < iEntitiesCount) {
        SdaiEntity sdaiEntity = engiGetEntityElement(m_sdaiModel, iIndex);

        auto pEntity = new _entity(sdaiEntity);

        assert(m_mapEntities.find(sdaiEntity) == m_mapEntities.end());
        m_mapEntities[sdaiEntity] = pEntity;

        iIndex++;
    }

    // Connect the Entities
    for (auto itEntity : m_mapEntities) {
        vector<SdaiEntity> parents;
        GetEntityParentsToDisplay(itEntity.first, parents);

        for (auto iParentEntity : parents) {

            auto itParentEntity = m_mapEntities.find(iParentEntity);
            assert(itParentEntity != m_mapEntities.end());

            itEntity.second->setParent(itParentEntity->second);

            itParentEntity->second->addSubType(itEntity.second);
        }
    }

    // Post-processing
    for (auto itEntity : m_mapEntities) {
        itEntity.second->postProcessing();
    } // for (; itEntities != ...	
}
