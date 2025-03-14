#include "_host.h"
#include "_rdf_mvc.h"
#include "_rdf_instance.h"
#include "_ptr.h"

// ************************************************************************************************
_rdf_model::_rdf_model()
    : _model()
{}

/*virtual*/ _rdf_model::~_rdf_model()
{
    clean();
}

_rdf_instance* _rdf_model::getInstance(OwlInstance owlInstance)
{
    assert(owlInstance != 0);

    auto itInstance = m_mapInstances.find(owlInstance);
    if (itInstance != m_mapInstances.end()) {
        return itInstance->second;
    }

    return nullptr;
}

/*virtual*/ void _rdf_model::addInstance(_instance* pInstance) /*override*/
{
    _model::addInstance(pInstance);

    assert(m_mapInstances.find(pInstance->getOwlInstance()) == m_mapInstances.end());
    m_mapInstances[pInstance->getOwlInstance()] = _ptr<_rdf_instance>(pInstance);
}

/*virtual*/ void _rdf_model::clean(bool bCloseModel/* = true*/) /*override*/
{
    _model::clean(bCloseModel);

    m_mapInstances.clear();
}

// ************************************************************************************************
_rdf_view::_rdf_view()
    : _view()
{}

/*virtual*/ _rdf_view::~_rdf_view()
{}

_rdf_controller* _rdf_view::getRDFController() const
{
    return _ptr<_rdf_controller>(m_pController);
}

// ************************************************************************************************
_rdf_controller::_rdf_controller()
    : _controller()
    , m_pSelectedProperty(nullptr)
    , m_iVisibleValuesCountLimit(10000)
    , m_bScaleAndCenter(true)
    , m_bModelCoordinateSystem(true)
{}

/*virtual*/ _rdf_controller::~_rdf_controller()
{}

/*virtual*/ void _rdf_controller::selectInstances(_view* pSender, const vector<_instance*>& vecInstance, bool bAdd/* = false*/) /*override*/
{
    _controller::selectInstances(pSender, vecInstance, bAdd);

    m_pSelectedProperty = nullptr;
}

/*virtual*/ void _rdf_controller::cleanSelection() /*override*/
{
    _controller::cleanSelection();

    m_pSelectedProperty = nullptr;
}

void _rdf_controller::selectInstanceProperty(_view* pSender, _rdf_instance* pInstance, _rdf_property* pProperty)
{
    assert(pInstance != nullptr);
    assert(pProperty != nullptr);

    if (m_bUpdatingModel) {
        return;
    }

    m_vecSelectedInstances = vector<_instance*>{ pInstance };
    m_pSelectedProperty = pProperty;

    auto itView = getViews().begin();
    for (; itView != getViews().end(); itView++) {
        _ptr<_rdf_view> rdfView(*itView, false);
        if (rdfView) {
            rdfView->onInstancePropertySelected(pSender);
        }
    }
}

void _rdf_controller::showBaseInformation(_view* pSender, _rdf_instance* pInstance)
{
    auto itView = getViews().begin();
    for (; itView != getViews().end(); itView++) {
        _ptr<_rdf_view> rdfView(*itView, false);
        if (rdfView) {
            rdfView->onShowBaseInformation(pSender, pInstance);
        }
    }
}

void _rdf_controller::showMetaInformation(_view* pSender, _rdf_instance* pInstance)
{
    auto itView = getViews().begin();
    for (; itView != getViews().end(); itView++) {
        _ptr<_rdf_view> rdfView(*itView, false);
        if (rdfView) {
            rdfView->onShowMetaInformation(pSender, pInstance);
        }
    }
}