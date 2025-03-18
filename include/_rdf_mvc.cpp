#include "_host.h"
#include "_rdf_mvc.h"
#include "_rdf_instance.h"
#include "_rdf_class.h"
#include "_ptr.h"

// ************************************************************************************************
_rdf_model::_rdf_model()
	: _model()
	, m_owlModel(0)
	, m_mapInstances()
	, m_mapClasses()
	, m_mapProperties()
	, m_mapInstanceDefaultState()
{
}

/*virtual*/ _rdf_model::~_rdf_model()
{
	clean();
}

/*virtual*/ void _rdf_model::scale() /*override*/
{
	// World
	m_dOriginalBoundingSphereDiameter = 2.;
	m_fBoundingSphereDiameter = 2.f;

	// Min/Max
	m_fXmin = FLT_MAX;
	m_fXmax = -FLT_MAX;
	m_fYmin = FLT_MAX;
	m_fYmax = -FLT_MAX;
	m_fZmin = FLT_MAX;
	m_fZmax = -FLT_MAX;

	for (auto pGeometry : getGeometries()) {
		if (!pGeometry->hasGeometry() ||
			pGeometry->ignoreBB()) {
			continue;
		}

		for (auto pInstance : pGeometry->getInstances()) {
			if (!pInstance->getEnable()) {
				continue;
			}

			pGeometry->calculateBB(
				pInstance,
				m_fXmin, m_fXmax,
				m_fYmin, m_fYmax,
				m_fZmin, m_fZmax);
		}
	} // for (auto pGeometry : ...

	if ((m_fXmin == FLT_MAX) ||
		(m_fXmax == -FLT_MAX) ||
		(m_fYmin == FLT_MAX) ||
		(m_fYmax == -FLT_MAX) ||
		(m_fZmin == FLT_MAX) ||
		(m_fZmax == -FLT_MAX)) {
		// TODO: new status bar for messages
		return;
	}

	// World
	m_fBoundingSphereDiameter = m_fXmax - m_fXmin;
	m_fBoundingSphereDiameter = fmax(m_fBoundingSphereDiameter, m_fYmax - m_fYmin);
	m_fBoundingSphereDiameter = fmax(m_fBoundingSphereDiameter, m_fZmax - m_fZmin);

	m_dOriginalBoundingSphereDiameter = m_fBoundingSphereDiameter;

	TRACE(L"\n*** Scale I *** => Xmin/max, Ymin/max, Zmin/max: %.16f, %.16f, %.16f, %.16f, %.16f, %.16f",
		m_fXmin,
		m_fXmax,
		m_fYmin,
		m_fYmax,
		m_fZmin,
		m_fZmax);
	TRACE(L"\n*** Scale I, Bounding sphere *** =>  %.16f", m_fBoundingSphereDiameter);

	// Scale
	for (auto pGeometry : getGeometries()) {
		if (!pGeometry->hasGeometry()) {
			continue;
		}

		pGeometry->scale((float)m_dOriginalBoundingSphereDiameter / 2.f);
	}

	// Min/Max
	m_fXmin = FLT_MAX;
	m_fXmax = -FLT_MAX;
	m_fYmin = FLT_MAX;
	m_fYmax = -FLT_MAX;
	m_fZmin = FLT_MAX;
	m_fZmax = -FLT_MAX;

	int64_t iEnabledInstances = 0;
	for (auto pGeometry : getGeometries()) {
		if (!pGeometry->hasGeometry()
			|| pGeometry->ignoreBB() ||
			!pGeometry->getShow()) {
			continue;
		}

		for (auto pInstance : pGeometry->getInstances()) {
			if (!pInstance->getEnable()) {
				continue;
			}

			iEnabledInstances++;

			pGeometry->calculateBB(
				pInstance,
				m_fXmin, m_fXmax,
				m_fYmin, m_fYmax,
				m_fZmin, m_fZmax);
		}
	}

	// Special case: all instances are disabled
	if (iEnabledInstances == 0) {
		for (auto pGeometry : getGeometries()) {
			if (!pGeometry->hasGeometry()
				|| pGeometry->ignoreBB() ||
				!pGeometry->getShow()) {
				continue;
			}

			for (auto pInstance : pGeometry->getInstances()) {
				pGeometry->calculateBB(
					pInstance,
					m_fXmin, m_fXmax,
					m_fYmin, m_fYmax,
					m_fZmin, m_fZmax);
			}
		}
	} // if (iEnabledInstances == 0)

	if ((m_fXmin == FLT_MAX) ||
		(m_fXmax == -FLT_MAX) ||
		(m_fYmin == FLT_MAX) ||
		(m_fYmax == -FLT_MAX) ||
		(m_fZmin == FLT_MAX) ||
		(m_fZmax == -FLT_MAX)) {
		// TODO: new status bar for messages
		return;
	}

	// World
	m_fBoundingSphereDiameter = m_fXmax - m_fXmin;
	m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fYmax - m_fYmin);
	m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fZmax - m_fZmin);

	TRACE(L"\n*** Scale II *** => Xmin/max, Ymin/max, Zmin/max: %.16f, %.16f, %.16f, %.16f, %.16f, %.16f",
		m_fXmin,
		m_fXmax,
		m_fYmin,
		m_fYmax,
		m_fZmin,
		m_fZmax);
	TRACE(L"\n*** Scale II, Bounding sphere *** =>  %.16f", m_fBoundingSphereDiameter);
}

void _rdf_model::attachModel(const wchar_t* szPath, OwlModel owlModel)
{
	assert((szPath != nullptr) && (wcslen(szPath) > 0));
	assert(owlModel != 0);

	clean();

	m_strPath = szPath;
	m_owlModel = owlModel;	

	load();
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

	if (m_owlModel != 0) {
		::CloseModel(m_owlModel);
		m_owlModel = 0;
	}	

	m_mapInstances.clear();

	auto itClasses = m_mapClasses.begin();
	for (; itClasses != m_mapClasses.end(); itClasses++) {
		delete itClasses->second;
	}
	m_mapClasses.clear();

	auto itProperty = m_mapProperties.begin();
	for (; itProperty != m_mapProperties.end(); itProperty++) {
		delete itProperty->second;
	}
	m_mapProperties.clear();

	m_mapInstanceDefaultState.clear();
}

void _rdf_model::loadClasses()
{
	OwlClass owlClass = GetClassesByIterator(getOwlModel(), 0);
	while (owlClass != 0) {
		auto itClass = m_mapClasses.find(owlClass);
		if (itClass == m_mapClasses.end()) {
			m_mapClasses[owlClass] = new _rdf_class(owlClass);
		}

		owlClass = GetClassesByIterator(getOwlModel(), owlClass);
	}
}

void _rdf_model::loadProperties()
{
	RdfProperty rdfProperty = GetPropertiesByIterator(getOwlModel(), 0);
	while (rdfProperty != 0) {
		auto itProperty = m_mapProperties.find(rdfProperty);
		if (itProperty == m_mapProperties.end()) {
			m_mapProperties[rdfProperty] = new _rdf_property(rdfProperty);

			auto itClasses = m_mapClasses.begin();
			for (; itClasses != m_mapClasses.end(); itClasses++) {
				int64_t	iMinCard = -1;
				int64_t iMaxCard = -1;
				GetClassPropertyAggregatedCardinalityRestriction(itClasses->first, rdfProperty, &iMinCard, &iMaxCard);

				if ((iMinCard == -1) && (iMaxCard == -1)) {
					continue;
				}

				itClasses->second->addPropertyRestriction(new _rdf_property_restriction(rdfProperty, iMinCard, iMaxCard));
			}
		} // if (itProperty == m_mapProperties.end())

		rdfProperty = GetPropertiesByIterator(getOwlModel(), rdfProperty);
	} // while (rdfProperty != 0)
}

void _rdf_model::loadInstances()
{
	OwlInstance owlInstance = GetInstancesByIterator(getOwlModel(), 0);
	while (owlInstance != 0) {
		auto itInstance = m_mapInstances.find(owlInstance);
		if (itInstance == m_mapInstances.end()) {
			// Load Model
			auto pGeometry = new _rdf_geometry(owlInstance);
			addGeometry(pGeometry);

			auto pInstance = new _rdf_instance(_model::getNextInstanceID(), pGeometry, nullptr);
			pInstance->setEnable(m_mapInstanceDefaultState.at(owlInstance));
			addInstance(pInstance);
		} else {
			// Import Model
			itInstance->second->recalculate();
		}

		owlInstance = GetInstancesByIterator(getOwlModel(), owlInstance);
	} // while (owlInstance != 0)
}

/*virtual*/ void  _rdf_model::preLoad()
{
	getInstancesDefaultEnableState();
	updateVertexBufferOffset();
}

void _rdf_model::load()
{
	preLoad();

	loadClasses();
	loadProperties();
	loadInstances();

	postLoad();	

	scale();
}

void _rdf_model::getInstancesDefaultEnableState()
{
	m_mapInstanceDefaultState.clear();

	// Enable only unreferenced instances
	OwlInstance owlInstance = GetInstancesByIterator(getOwlModel(), 0);
	while (owlInstance != 0) {
		m_mapInstanceDefaultState[owlInstance] = GetInstanceInverseReferencesByIterator(owlInstance, 0) == 0;

		owlInstance = GetInstancesByIterator(getOwlModel(), owlInstance);
	}

	// Enable children/descendants with geometry
	OwlClass NillOwlClass = GetClassByName(getOwlModel(), "Nill");
	for (auto& itInstanceDefaultState : m_mapInstanceDefaultState) {
		if (!itInstanceDefaultState.second) {
			continue;
		}

		if (!GetInstanceGeometryClass(itInstanceDefaultState.first) ||
			!GetBoundingBox(itInstanceDefaultState.first, nullptr, nullptr)) {
			OwlClass owlClass = GetInstanceClass(itInstanceDefaultState.first);
			assert(owlClass != 0);

			if ((owlClass != NillOwlClass) && !IsClassAncestor(owlClass, NillOwlClass)) {
				getInstanceDefaultEnableStateRecursive(itInstanceDefaultState.first);
			}
		}
	}
}

void _rdf_model::getInstanceDefaultEnableStateRecursive(OwlInstance owlInstance)
{
	assert(owlInstance != 0);

	RdfProperty rdfProperty = GetInstancePropertyByIterator(owlInstance, 0);
	while (rdfProperty != 0) {
		if (GetPropertyType(rdfProperty) == OBJECTPROPERTY_TYPE) {
			int64_t iValuesCount = 0;
			OwlInstance* pValues = nullptr;
			GetObjectProperty(owlInstance, rdfProperty, &pValues, &iValuesCount);

			for (int64_t iValue = 0; iValue < iValuesCount; iValue++) {
				if ((pValues[iValue] != 0) &&
					!m_mapInstanceDefaultState.at(pValues[iValue])) {
					// Enable to avoid infinity recursion
					m_mapInstanceDefaultState.at(pValues[iValue]) = true;

					if (!GetInstanceGeometryClass(pValues[iValue]) ||
						!GetBoundingBox(pValues[iValue], nullptr, nullptr)) {
						getInstanceDefaultEnableStateRecursive(pValues[iValue]);
					}
				}
			}
		}

		rdfProperty = GetInstancePropertyByIterator(owlInstance, rdfProperty);
	}
}

void _rdf_model::updateVertexBufferOffset()
{
	double dXmin = DBL_MAX;
	double dXmax = -DBL_MAX;
	double dYmin = DBL_MAX;
	double dYmax = -DBL_MAX;
	double dZmin = DBL_MAX;
	double dZmax = -DBL_MAX;

	OwlInstance owlInstance = GetInstancesByIterator(getOwlModel(), 0);
	while (owlInstance != 0) {
		if (m_mapInstanceDefaultState.at(owlInstance)) {
			_geometry::calculateBB(
				owlInstance,
				dXmin, dXmax,
				dYmin, dYmax,
				dZmin, dZmax);
		}

		owlInstance = GetInstancesByIterator(getOwlModel(), owlInstance);
	}

	if ((dXmin == DBL_MAX) ||
		(dXmax == -DBL_MAX) ||
		(dYmin == DBL_MAX) ||
		(dYmax == -DBL_MAX) ||
		(dZmin == DBL_MAX) ||
		(dZmax == -DBL_MAX)) {
		return;
	}

	double dVertexBuffersOffsetX = -(dXmin + dXmax) / 2.;
	double dVertexBuffersOffsetY = -(dYmin + dYmax) / 2.;
	double dVertexBuffersOffsetZ = -(dZmin + dZmax) / 2.;

	TRACE(L"\n*** SetVertexBufferOffset *** => x/y/z: %.16f, %.16f, %.16f",
		dVertexBuffersOffsetX,
		dVertexBuffersOffsetY,
		dVertexBuffersOffsetZ);

	// http://rdf.bg/gkdoc/CP64/SetVertexBufferOffset.html
	SetVertexBufferOffset(
		getOwlModel(),
		dVertexBuffersOffsetX,
		dVertexBuffersOffsetY,
		dVertexBuffersOffsetZ);

	// http://rdf.bg/gkdoc/CP64/ClearedExternalBuffers.html
	ClearedExternalBuffers(getOwlModel());
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

void _rdf_model::getClassAncestors(OwlClass owlClass, vector<OwlClass>& vecAncestors) const
{
	assert(owlClass != 0);

	const auto& itClass = m_mapClasses.find(owlClass);
	assert(itClass != m_mapClasses.end());

	const auto& vecParentClasses = itClass->second->getParentClasses();
	if (vecParentClasses.empty()) {
		return;
	}

	for (size_t iParentClass = 0; iParentClass < vecParentClasses.size(); iParentClass++) {
		vecAncestors.insert(vecAncestors.begin(), vecParentClasses[iParentClass]);
		getClassAncestors(vecParentClasses[iParentClass], vecAncestors);
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

void _rdf_model::resetInstancesDefaultEnableState()
{
	getInstancesDefaultEnableState();

	auto itInstance = m_mapInstances.begin();
	for (; itInstance != m_mapInstances.end(); itInstance++) {
		if (m_mapInstanceDefaultState.find(itInstance->second->getOwlInstance()) != m_mapInstanceDefaultState.end()) {
			itInstance->second->setEnable(m_mapInstanceDefaultState.at(itInstance->second->getOwlInstance()));
		}
	}
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

/*virtual*/ void _rdf_controller::onInstancesEnabledStateChanged(_view* pSender) /*override*/
{
	if (getModel() == nullptr) {
		assert(false);
		return;
	}

	_ptr<_rdf_model>(getModel())->recalculate();
	if (m_bScaleAndCenter) {
		getModel()->scale();
	}

	_controller::onInstancesEnabledStateChanged(pSender);
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