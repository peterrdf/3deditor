#include "_host.h"
#include "_ap_mvc.h"

#include <string>
using namespace std;

// ************************************************************************************************
_ap_model::_ap_model(enumAP enAP)
    : _model()
    , m_sdaiModel(0)
    , m_enAP(enAP)
    , m_pEntityProvider(nullptr)
    , m_pAttributeProvider(nullptr)
    , m_mapExpressID2Geometry()
    , m_mapGeometries()
{}

/*virtual*/ _ap_model::~_ap_model()
{
    clean();
}

/*virtual*/ OwlModel _ap_model::getOwlModel() const /*override*/
{
    assert(m_sdaiModel != 0);

    OwlModel owlModel = 0;
    owlGetModel(m_sdaiModel, &owlModel);
    assert(owlModel != 0);

    return owlModel;
}

bool _ap_model::openModel(const wchar_t* szPath)
{
    assert((szPath != nullptr) && (wcslen(szPath) > 0));

    SdaiModel sdaiModel = sdaiOpenModelBNUnicode(0, szPath, L"");
    if (sdaiModel == 0) {
        return false;
    }

    attachModel(szPath, sdaiModel, nullptr);

    return true;
}

void _ap_model::attachModel(const wchar_t* szPath, SdaiModel sdaiModel, _model* pWorld)
{
    assert((szPath != nullptr) && (wcslen(szPath) > 0));
    assert(sdaiModel != 0);

    clean();

    m_strPath = szPath;
    m_sdaiModel = sdaiModel;
    m_pWorld = pWorld;

    attachModelCore();
}

_geometry* _ap_model::getGeometryByInstance(SdaiInstance sdaiInstance)
{
    auto itGeometry = m_mapGeometries.find(sdaiInstance);
    if (itGeometry != m_mapGeometries.end()) {
        return itGeometry->second;
    }

    return nullptr;
}

_geometry* _ap_model::getGeometryByExpressID(ExpressID iExpressID) const
{
    assert(iExpressID != 0);

    auto itExpressID2Geometry = m_mapExpressID2Geometry.find(iExpressID);
    if (itExpressID2Geometry != m_mapExpressID2Geometry.end()) {
        return itExpressID2Geometry->second;
    }

    return nullptr;
}

template<typename T>
T* _ap_model::getGeometryByExpressIDAs(ExpressID iExpressID) const
{
    assert(iExpressID != 0);

    auto itExpressID2Geometry = m_mapExpressID2Geometry.find(iExpressID);
    if (itExpressID2Geometry != m_mapExpressID2Geometry.end()) {
        return dynamic_cast<T*>(itExpressID2Geometry->second);
    }

    return nullptr;
}

void _ap_model::getGeometriesByType(const char* szType, vector<_ap_geometry*>& vecGeometries)
{
    assert((szType != nullptr) && (strlen(szType) > 0));

    vecGeometries.clear();

    SdaiAggr sdaiGroupAggr = xxxxGetEntityAndSubTypesExtentBN(getSdaiModel(), szType);
    ASSERT(sdaiGroupAggr != nullptr);

    SdaiInteger iInstancesCount = sdaiGetMemberCount(sdaiGroupAggr);
    for (SdaiInteger i = 0; i < iInstancesCount; i++) {
        SdaiInstance sdaiInstance = 0;
        sdaiGetAggrByIndex(sdaiGroupAggr, i, sdaiINSTANCE, &sdaiInstance);
        assert(sdaiInstance != 0);

        auto pGeometry = getGeometryByInstance(sdaiInstance);
        assert(pGeometry != nullptr);

        vecGeometries.push_back(_ptr<_ap_geometry>(pGeometry));
    }
}

/*virtual*/ void _ap_model::preLoadInstance(OwlInstance owlInstance)
{
    setVertexBufferOffset(owlInstance);
}

void _ap_model::addGeometry(_ap_geometry* pGeometry)
{
    _model::addGeometry(pGeometry);

    assert(m_mapGeometries.find(pGeometry->getSdaiInstance()) == m_mapGeometries.end());
    m_mapGeometries[pGeometry->getSdaiInstance()] = pGeometry;

    assert(m_mapExpressID2Geometry.find(pGeometry->getExpressID()) == m_mapExpressID2Geometry.end());
    m_mapExpressID2Geometry[pGeometry->getExpressID()] = pGeometry;
}

/*virtual*/ void _ap_model::clean(bool bCloseModel/* = true*/) /*override*/
{
    _model::clean(bCloseModel);

    if (bCloseModel) {
        if (m_sdaiModel != 0) {
            sdaiCloseModel(m_sdaiModel);
            m_sdaiModel = 0;
        }

        delete m_pEntityProvider;
        m_pEntityProvider = nullptr;

        delete m_pAttributeProvider;
        m_pAttributeProvider = nullptr;
    }

    m_mapGeometries.clear();
    m_mapExpressID2Geometry.clear();
}

_entity_provider* _ap_model::getEntityProvider()
{
    if ((m_pEntityProvider == nullptr) && (m_sdaiModel != 0)) {
        m_pEntityProvider = new _entity_provider(m_sdaiModel);
    }

    return m_pEntityProvider;
}

_attribute_provider* _ap_model::getAttributeProvider()
{
    if (m_pAttributeProvider == nullptr) {
        m_pAttributeProvider = new _attribute_provider();
    }

    return m_pAttributeProvider;
}

// ************************************************************************************************
_ap_view::_ap_view()
    : _view()
{}

/*virtual*/ _ap_view::~_ap_view()
{}

_ap_controller* _ap_view::getAPController() const
{
    return _ptr<_ap_controller>(m_pController);
}

// ************************************************************************************************
_ap_controller::_ap_controller()
    : _controller()
{}

/*virtual*/ _ap_controller::~_ap_controller()
{}

void _ap_controller::onViewRelations(_view* pSender, SdaiInstance sdaiInstance)
{
    auto itView = getViews().begin();
    for (; itView != getViews().end(); itView++) {
        _ptr<_ap_view> apView(*itView, false);
        if (apView) {
            apView->onViewRelations(pSender, sdaiInstance);
        }
    }
}

void _ap_controller::onViewRelations(_view* pSender, _entity* pEntity)
{
    m_pTargetInstance = nullptr;

    auto itView = getViews().begin();
    for (; itView != getViews().end(); itView++) {
        _ptr<_ap_view> apView(*itView, false);
        if (apView) {
            apView->onViewRelations(pSender, pEntity);
        }
    }
}

void _ap_controller::onInstanceAttributeEdited(_view* pSender, SdaiInstance sdaiInstance, SdaiAttr sdaiAttr)
{
    auto itView = getViews().begin();
    for (; itView != getViews().end(); itView++) {
        _ptr<_ap_view> apView(*itView, false);
        if (apView) {
            apView->onInstanceAttributeEdited(pSender, sdaiInstance, sdaiAttr);
        }
    }
}