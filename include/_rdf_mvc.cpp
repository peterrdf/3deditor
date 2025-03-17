#include "_host.h"
#include "_rdf_mvc.h"
#include "_rdf_instance.h"
#include "_rdf_class.h"
#include "_ptr.h"

// ************************************************************************************************
_rdf_model::_rdf_model()
	: _model()
	, m_mapInstances()
	, m_mapInstanceDefaultState()
{
}

/*virtual*/ _rdf_model::~_rdf_model()
{
	clean();
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
	m_mapInstanceDefaultState.clear();
}

_rdf_instance* _rdf_model::getInstanceByOwlInstance(OwlInstance owlInstance)
{
	assert(owlInstance != 0);

	auto itInstance = m_mapInstances.find(owlInstance);
	if (itInstance != m_mapInstances.end()) {
		return itInstance->second;
	}

	return nullptr;
}

void _rdf_model::getCompatibleInstances(_rdf_instance* pInstance, _rdf_property* pProperty, vector<OwlInstance>& vecCompatibleInstances) const
{
	assert(pInstance != nullptr);
	assert(pProperty != nullptr);

	vecCompatibleInstances.clear();

	vector<OwlClass> vecRestrictionClasses;
	_rdf_property::getRangeRestrictions(pProperty->getRdfProperty(), vecRestrictionClasses);

	for (auto pInstance2 : getInstances()) {
		if (pInstance2 == pInstance) {
			continue;
		}

		if (std::find(vecRestrictionClasses.begin(), vecRestrictionClasses.end(), pInstance->getGeometry()->getOwlClass()) != vecRestrictionClasses.end()) {
			vecCompatibleInstances.push_back(pInstance2->getOwlInstance());
			continue;
		}

		vector<OwlClass> vecAncestorClasses;
		_rdf_class::getAncestors(pInstance->getGeometry()->getOwlClass(), vecAncestorClasses);

		for (size_t iAncestorClass = 0; iAncestorClass < vecAncestorClasses.size(); iAncestorClass++) {
			if (find(vecRestrictionClasses.begin(), vecRestrictionClasses.end(), vecAncestorClasses[iAncestorClass]) != vecRestrictionClasses.end()) {
				vecCompatibleInstances.push_back(pInstance2->getOwlInstance());
				break;
			}
		}
	}
}

_rdf_instance* _rdf_model::createInstance(OwlClass owlClass)
{
	assert(owlClass != 0);

	OwlInstance owlInstance = CreateInstance(owlClass);
	assert(owlInstance != 0);

	auto pGeometry = new _rdf_geometry(owlInstance);
	addGeometry(pGeometry);

	auto pInstance = new _rdf_instance(_model::getNextInstanceID(), pGeometry, nullptr);
	addInstance(pInstance);

	m_mapInstanceDefaultState[owlInstance] = true;

	return pInstance;
}

bool _rdf_model::deleteInstance(_rdf_instance* pInstance)
{
	assert(pInstance != nullptr);

	bool bResult = RemoveInstance(pInstance->getOwlInstance()) == 0 ? true : false;
	assert(bResult);

	// _rdf_model
	auto itInstance = m_mapInstances.find(pInstance->getOwlInstance());
	assert(itInstance != m_mapInstances.end());
	m_mapInstances.erase(itInstance);

	// _model
	_model::deleteGeometry(pInstance->getGeometry());

	return bResult;
}

void _rdf_model::recalculate()
{
	for (auto pInstance : getInstances()) {
		_ptr<_rdf_instance>(pInstance)->recalculate();
	}
}

// ************************************************************************************************
_rdf_view::_rdf_view()
	: _view()
{
}

/*virtual*/ _rdf_view::~_rdf_view()
{
}

_rdf_controller* _rdf_view::getRDFController() const
{
	return _ptr<_rdf_controller>(m_pController);
}

_rdf_model* _rdf_view::getRDFModel() const
{
	if (getController()->getModel() != nullptr) {
		return _ptr<_rdf_model>(getController()->getModel());
	}

	return nullptr;
}

// ************************************************************************************************
_rdf_controller::_rdf_controller()
	: _controller()
	, m_pSelectedProperty(nullptr)
	, m_iVisibleValuesCountLimit(10000)
	, m_bScaleAndCenter(true)
	, m_bModelCoordinateSystem(true)
{
}

/*virtual*/ _rdf_controller::~_rdf_controller()
{
}

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

void _rdf_controller::renameInstance(_view* pSender, _rdf_instance* pInstance, const wchar_t* szName)
{
	assert(pInstance != nullptr);
	assert(szName != nullptr);

	if (getModel() == nullptr) {
		assert(false);
		return;
	}

	SetNameOfInstanceW(
		pInstance->getOwlInstance(),
		szName);

	pInstance->getGeometryAs<_rdf_geometry>()->loadName();

	auto itView = getViews().begin();
	for (; itView != getViews().end(); itView++) {
		_ptr<_rdf_view> rdfView(*itView, false);
		if (rdfView) {
			rdfView->onInstanceRenamed(pSender, pInstance);
		}
	}
}

_rdf_instance* _rdf_controller::createInstance(_view* pSender, OwlClass owlClass)
{
	assert(owlClass != 0);

	if (getModel() == nullptr) {
		assert(false);
		return nullptr;
	}

	auto pInstance = _ptr<_rdf_model>(getModel())->createInstance(owlClass);
	assert(pInstance != nullptr);

	auto itView = getViews().begin();
	for (; itView != getViews().end(); itView++) {
		_ptr<_rdf_view> rdfView(*itView, false);
		if (rdfView) {
			rdfView->onInstanceCreated(pSender, pInstance);
		}
	}

	return pInstance;
}

bool _rdf_controller::deleteInstance(_view* pSender, _rdf_instance* pInstance)
{
	assert(pInstance != nullptr);

	if (getModel() == nullptr) {
		assert(false);
		return false;
	}

	if (getSelectedInstance() == pInstance) {
		cleanSelection();
	}

	bool bResult = _ptr<_rdf_model>(getModel())->deleteInstance(pInstance);
	assert(bResult);

	auto itView = getViews().begin();
	for (; itView != getViews().end(); itView++) {
		_ptr<_rdf_view> rdfView(*itView, false);
		if (rdfView) {
			rdfView->onInstanceDeleted(pSender, pInstance);
		}
	}

	return bResult;
}

bool _rdf_controller::deleteInstanceTree(_view* pSender, _rdf_instance* pInstance)
{
	assert(pInstance != nullptr);

	if (getModel() == nullptr) {
		assert(false);
		return false;
	}

	if (getSelectedInstance() == pInstance) {
		cleanSelection();
	}

	if (deleteInstanceTreeRecursive(pSender, pInstance)) {
		auto itView = getViews().begin();
		for (; itView != getViews().end(); itView++) {
			_ptr<_rdf_view> rdfView(*itView, false);
			if (rdfView) {
				rdfView->onInstancesDeleted(pSender);
			}
		}

		return true;
	}

	return false;
}

bool _rdf_controller::deleteInstanceTreeRecursive(_view* pSender, _rdf_instance* pInstance)
{
	if (getModel() == nullptr) {
		assert(false);
		return false;
	}

	OwlInstance owlInstance = pInstance->getOwlInstance();

	int64_t	iChildrenCount = 0;
	RdfProperty rdfProperty = GetInstancePropertyByIterator(owlInstance, 0);
	while (rdfProperty != 0) {
		if (GetPropertyType(rdfProperty) == OBJECTPROPERTY_TYPE) {
			OwlInstance* values = nullptr, card = 0;
			GetObjectProperty(owlInstance, rdfProperty, &values, &card);
			iChildrenCount += card;
		}
		rdfProperty = GetInstancePropertyByIterator(owlInstance, rdfProperty);
	}

	int64_t	i = 0;
	OwlInstance* arChildren = new OwlInstance[iChildrenCount];
	rdfProperty = GetInstancePropertyByIterator(owlInstance, 0);
	while (rdfProperty != 0) {
		if (GetPropertyType(rdfProperty) == OBJECTPROPERTY_TYPE) {
			OwlInstance* values = nullptr, card = 0;
			GetObjectProperty(owlInstance, rdfProperty, &values, &card);
			memcpy(&arChildren[i], values, card * sizeof(int64_t));
			i += card;
		}
		rdfProperty = GetInstancePropertyByIterator(owlInstance, rdfProperty);
	}
	assert(i == iChildrenCount);

	_ptr<_rdf_model> rdfModel(getModel());
	bool bResult = rdfModel->deleteInstance(pInstance);
	assert(bResult);

	for (i = 0; i < iChildrenCount; i++) {
		auto pChildInstance = rdfModel->getInstanceByOwlInstance(arChildren[i]);
		if (pChildInstance) {
			bResult &= deleteInstanceTreeRecursive(pSender, pChildInstance);
			assert(bResult);
		}
	}

	return bResult;
}

bool _rdf_controller::deleteInstances(_view* pSender, const vector<_rdf_instance*>& vecInstances)
{
	if (getModel() == nullptr) {
		assert(false);
		return false;
	}

	bool bResult = true;
	for (auto pInstance : vecInstances) {
		if (getSelectedInstance() == pInstance) {
			cleanSelection();
		}

		bResult &= _ptr<_rdf_model>(getModel())->deleteInstance(pInstance);
		assert(bResult);
	}

	auto itView = getViews().begin();
	for (; itView != getViews().end(); itView++) {
		_ptr<_rdf_view> rdfView(*itView, false);
		if (rdfView) {
			rdfView->onInstancesDeleted(pSender);
		}
	}

	return bResult;
}

void _rdf_controller::onMeasurementsAdded(_view* pSender, _rdf_instance* pInstance)
{
	auto itView = getViews().begin();
	for (; itView != getViews().end(); itView++) {
		_ptr<_rdf_view> rdfView(*itView, false);
		if (rdfView) {
			rdfView->onMeasurementsAdded(pSender, pInstance);
		}
	}
}

void _rdf_controller::onInstancePropertyEdited(_view* pSender, _rdf_instance* pInstance, _rdf_property* pProperty)
{
	if (getModel() == nullptr) {
		assert(false);
		return;
	}

	_ptr<_rdf_model>(getModel())->recalculate();
	if (m_bScaleAndCenter) {
		getModel()->scale();
	}

	auto itView = getViews().begin();
	for (; itView != getViews().end(); itView++) {
		_ptr<_rdf_view> rdfView(*itView, false);
		if (rdfView) {
			rdfView->onInstancePropertyEdited(pSender, pInstance, pProperty);
		}
	}
}