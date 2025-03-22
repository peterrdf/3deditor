#include "_host.h"
#include "_rdf_mvc.h"
#include "_rdf_instance.h"
#include "_rdf_class.h"
#include "_text_builder.h"
#include "geom.h"
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

void _rdf_model::importModel(const wchar_t* szPath)
{
	if (m_owlModel == 0) {
		assert(false);
		return;
	}

	ImportModelW(m_owlModel, szPath);

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

void _rdf_model::reloadGeometries() {
	for (auto pGeometry : getGeometries()) {
		_ptr<_rdf_geometry>(pGeometry)->reload();
	}
}

/*static*/ OwlInstance _rdf_model::translateTransformation(
	OwlModel owlModel,
	OwlInstance owlInstance,
	double dX, double dY, double dZ)
{
	return translateTransformation(
		owlModel,
		owlInstance,
		dX, dY, dZ,
		1., 1., 1.);
}

/*static*/ OwlInstance _rdf_model::translateTransformation(
	OwlModel owlModel,
	OwlInstance owlInstance,
	double dX, double dY, double dZ,
	double d11, double d22, double d33)
{
	assert(owlInstance != 0);

	OwlInstance owlMatrixInstance = CreateInstance(GetClassByName(owlModel, "Matrix"));
	assert(owlMatrixInstance != 0);

	vector<double> vecTransformationMatrix =
	{
		d11, 0., 0.,
		0., d22, 0.,
		0., 0., d33,
		dX, dY, dZ,
	};

	SetDatatypeProperty(
		owlMatrixInstance,
		GetPropertyByName(owlModel, "coordinates"),
		vecTransformationMatrix.data(),
		vecTransformationMatrix.size());

	OwlInstance owlTransformationInstance = CreateInstance(GetClassByName(owlModel, "Transformation"));
	assert(owlTransformationInstance != 0);

	SetObjectProperty(owlTransformationInstance, GetPropertyByName(owlModel, "matrix"), &owlMatrixInstance, 1);
	SetObjectProperty(owlTransformationInstance, GetPropertyByName(owlModel, "object"), &owlInstance, 1);

	return owlTransformationInstance;
}

/*static*/ OwlInstance _rdf_model::rotateTransformation(
	OwlModel owlModel,
	OwlInstance owlInstance,
	double alpha, double beta, double gamma)
{
	assert(owlInstance != 0);

	OwlInstance owlMatrixInstance = CreateInstance(GetClassByName(owlModel, "Matrix"));
	assert(owlMatrixInstance != 0);

	_matrix matrix;
	memset(&matrix, 0, sizeof(_matrix));
	_matrixRotateByEulerAngles(&matrix, alpha, beta, gamma);

	SetDatatypeProperty(owlMatrixInstance, GetPropertyByName(owlModel, "_11"), &matrix._11, 1);
	SetDatatypeProperty(owlMatrixInstance, GetPropertyByName(owlModel, "_12"), &matrix._12, 1);
	SetDatatypeProperty(owlMatrixInstance, GetPropertyByName(owlModel, "_13"), &matrix._13, 1);

	SetDatatypeProperty(owlMatrixInstance, GetPropertyByName(owlModel, "_21"), &matrix._21, 1);
	SetDatatypeProperty(owlMatrixInstance, GetPropertyByName(owlModel, "_22"), &matrix._22, 1);
	SetDatatypeProperty(owlMatrixInstance, GetPropertyByName(owlModel, "_23"), &matrix._23, 1);

	SetDatatypeProperty(owlMatrixInstance, GetPropertyByName(owlModel, "_31"), &matrix._31, 1);
	SetDatatypeProperty(owlMatrixInstance, GetPropertyByName(owlModel, "_32"), &matrix._32, 1);
	SetDatatypeProperty(owlMatrixInstance, GetPropertyByName(owlModel, "_33"), &matrix._33, 1);

	OwlInstance owlTransformationInstance = CreateInstance(GetClassByName(owlModel, "Transformation"));
	assert(owlTransformationInstance != 0);

	SetObjectProperty(owlTransformationInstance, GetPropertyByName(owlModel, "matrix"), &owlMatrixInstance, 1);
	SetObjectProperty(owlTransformationInstance, GetPropertyByName(owlModel, "object"), &owlInstance, 1);

	return owlTransformationInstance;
}

/*static*/ OwlInstance _rdf_model::scaleTransformation(
	OwlModel owlModel, 
	OwlInstance owlInstance,
	double dFactor)
{
	assert(owlInstance != 0);

	OwlInstance owlMatrixInstance = CreateInstance(GetClassByName(owlModel, "Matrix"));
	assert(owlMatrixInstance != 0);

	vector<double> vecTransformationMatrix =
	{
		dFactor, 0., 0.,
		0., dFactor, 0.,
		0., 0., dFactor,
		0., 0., 0.,
	};

	SetDatatypeProperty(
		owlMatrixInstance,
		GetPropertyByName(owlModel, "coordinates"),
		vecTransformationMatrix.data(),
		vecTransformationMatrix.size());

	OwlInstance owlTransformationInstance = CreateInstance(GetClassByName(owlModel, "Transformation"));
	assert(owlTransformationInstance != 0);

	SetObjectProperty(owlTransformationInstance, GetPropertyByName(owlModel, "matrix"), &owlMatrixInstance, 1);
	SetObjectProperty(owlTransformationInstance, GetPropertyByName(owlModel, "object"), &owlInstance, 1);

	return owlTransformationInstance;
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
	, m_bScaleAndCenterAllVisibleGeometry(true)
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

/*virtual*/ void _rdf_controller::onModelUpdated() /*override*/
{
	for (auto pDecoration : getDecorationModels()) {
		_ptr<_decoration>(pDecoration)->onModelUpdated();
	}

	_controller::onModelUpdated();
}

/*virtual*/ void _rdf_controller::onInstancesEnabledStateChanged(_view* pSender) /*override*/
{
	if (getModel() == nullptr) {
		assert(false);
		return;
	}
	
	if (m_bScaleAndCenterAllVisibleGeometry) {
		_ptr<_rdf_model>(getModel())->reloadGeometries();
		getModel()->scale();
	}

	for (auto pDecoration : getDecorationModels()) {
		_ptr<_decoration>(pDecoration)->onModelUpdated();
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
	assert(pInstance != nullptr);
	assert(pProperty != nullptr);

	if (getModel() == nullptr) {
		assert(false);
		return;
	}

	pInstance->recalculate();

	if (m_bScaleAndCenterAllVisibleGeometry && pInstance->getEnable() ) {
		_ptr<_rdf_model>(getModel())->reloadGeometries();
		getModel()->scale();
	}

	for (auto pDecoration : getDecorationModels()) {
		_ptr<_decoration>(pDecoration)->onModelUpdated();
	}

	auto itView = getViews().begin();
	for (; itView != getViews().end(); itView++) {
		_ptr<_rdf_view> rdfView(*itView, false);
		if (rdfView) {
			rdfView->onInstancePropertyEdited(pSender, pInstance, pProperty);
		}
	}
}

// ************************************************************************************************
_coordinate_system_model_base::_coordinate_system_model_base()
	: _rdf_model()
	, m_pTextBuilder(new _text_builder())
{}

/*virtual*/ _coordinate_system_model_base::~_coordinate_system_model_base()
{
	delete m_pTextBuilder;
}

void _coordinate_system_model_base::create()
{
	const double AXIS_LENGTH = 4.;
	const double ARROW_OFFSET = AXIS_LENGTH;

	OwlModel owlModel = CreateModel();
	assert(owlModel != 0);

	m_pTextBuilder->initialize(owlModel);

	// Coordinate System
	vector<OwlInstance> vecInstances;

	// Coordinate System/X (1 Line3D)
	OwlInstance owlXAxisMaterialInstance = 0;
	{
		auto pAmbient = GEOM::ColorComponent::Create(owlModel);
		pAmbient.set_R(1.);
		pAmbient.set_G(0.);
		pAmbient.set_B(0.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(owlModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(owlModel);
		pMaterial.set_color(pColor);

		owlXAxisMaterialInstance = (int64_t)pMaterial;

		vector<double> vecPoints =
		{
			0., 0., 0.,
			AXIS_LENGTH / 2., 0., 0.,
		};

		auto pXAxis = GEOM::Line3D::Create(owlModel);
		pXAxis.set_material(pMaterial);
		pXAxis.set_points(vecPoints.data(), vecPoints.size());

		vecInstances.push_back((int64_t)pXAxis);
	}

	// Coordinate System/Y (Line3D)
	OwlInstance owlYAxisMaterialInstance = 0;
	{
		auto pAmbient = GEOM::ColorComponent::Create(owlModel);
		pAmbient.set_R(0.);
		pAmbient.set_G(1.);
		pAmbient.set_B(0.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(owlModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(owlModel);
		pMaterial.set_color(pColor);

		owlYAxisMaterialInstance = (int64_t)pMaterial;

		vector<double> vecPoints =
		{
			0., 0., 0.,
			0., AXIS_LENGTH / 2., 0.,
		};

		auto pYAxis = GEOM::Line3D::Create(owlModel);
		pYAxis.set_material(pMaterial);
		pYAxis.set_points(vecPoints.data(), vecPoints.size());

		vecInstances.push_back((int64_t)pYAxis);
	}

	// Coordinate System/Z (Line3D)
	OwlInstance owlZAxisMaterialInstance = 0;
	{
		auto pAmbient = GEOM::ColorComponent::Create(owlModel);
		pAmbient.set_R(0.);
		pAmbient.set_G(0.);
		pAmbient.set_B(1.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(owlModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(owlModel);
		pMaterial.set_color(pColor);

		owlZAxisMaterialInstance = (int64_t)pMaterial;

		vector<double> vecPoints =
		{
			0., 0., 0.,
			0., 0., AXIS_LENGTH / 2.,
		};

		auto pZAxis = GEOM::Line3D::Create(owlModel);
		pZAxis.set_material(pMaterial);
		pZAxis.set_points(vecPoints.data(), vecPoints.size());

		vecInstances.push_back((int64_t)pZAxis);
	}

	// Arrows (1 Cone => 3 Transformations)
	{
		auto pArrow = GEOM::Cone::Create(owlModel);
		pArrow.set_height(AXIS_LENGTH / 15.);
		pArrow.set_radius(.075);

		// +X
		{
			OwlInstance owlPlusXInstance = translateTransformation(
				owlModel,
				rotateTransformation(owlModel, (int64_t)pArrow, 0., 2 * PI * 90. / 360., 0.),
				ARROW_OFFSET / 2., 0., 0.,
				1., 1., 1.);
			SetNameOfInstance(owlPlusXInstance, "#(+X)");

			SetObjectProperty(
				owlPlusXInstance,
				GetPropertyByName(owlModel, "material"),
				&owlXAxisMaterialInstance,
				1);

			vecInstances.push_back(owlPlusXInstance);
		}

		// +Y
		{
			OwlInstance owlPlusYInstance = translateTransformation(
				owlModel,
				rotateTransformation(owlModel, (int64_t)pArrow, 2 * PI * 270. / 360., 0., 0.),
				0., ARROW_OFFSET / 2., 0.,
				1., 1., 1.);
			SetNameOfInstance(owlPlusYInstance, "#(+Y)");

			SetObjectProperty(
				owlPlusYInstance,
				GetPropertyByName(owlModel, "material"),
				&owlYAxisMaterialInstance,
				1);

			vecInstances.push_back(owlPlusYInstance);
		}

		// +Z
		{
			OwlInstance owlPlusZInstance = translateTransformation(
				owlModel,
				(int64_t)pArrow,
				0., 0., ARROW_OFFSET / 2.,
				1., 1., 1.);
			SetNameOfInstance(owlPlusZInstance, "#(+Z)");

			SetObjectProperty(
				owlPlusZInstance,
				GetPropertyByName(owlModel, "material"),
				&owlZAxisMaterialInstance,
				1);

			vecInstances.push_back(owlPlusZInstance);
		}
	}

	/* Labels */
	double dXmin = DBL_MAX;
	double dXmax = -DBL_MAX;
	double dYmin = DBL_MAX;
	double dYmax = -DBL_MAX;
	double dZmin = DBL_MAX;
	double dZmax = -DBL_MAX;

	// X-axis
	OwlInstance owlPlusXLabelInstance = m_pTextBuilder->buildText("X-axis", true);
	assert(owlPlusXLabelInstance != 0);

	_geometry::calculateBB(
		owlPlusXLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	// Y-axis
	OwlInstance owlPlusYLabelInstance = m_pTextBuilder->buildText("Y-axis", true);
	assert(owlPlusYLabelInstance != 0);

	_geometry::calculateBB(
		owlPlusYLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	// Z-axis
	OwlInstance owlPlusZLabelInstance = m_pTextBuilder->buildText("Z-axis", true);
	assert(owlPlusZLabelInstance != 0);

	_geometry::calculateBB(
		owlPlusZLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Scale Factor */
	double dMaxLength = dXmax - dXmin;
	dMaxLength = fmax(dMaxLength, dYmax - dYmin);
	dMaxLength = fmax(dMaxLength, dZmax - dZmin);

	double dScaleFactor = ((AXIS_LENGTH / 2.) * .75) / dMaxLength;

	/* Transform Labels */

	// X-axis
	OwlInstance owlInstance = translateTransformation(
		owlModel,
		rotateTransformation(owlModel, scaleTransformation(owlModel, owlPlusXLabelInstance, dScaleFactor / 2.), 2 * PI * 90. / 360., 0., 2 * PI * 90. / 180.),
		AXIS_LENGTH / 1.4, 0., 0.,
		-1., 1., 1.);

	SetNameOfInstance(owlInstance, "#X-axis");
	SetObjectProperty(
		owlInstance,
		GetPropertyByName(owlModel, "material"),
		&owlXAxisMaterialInstance,
		1);

	// Y-axis
	owlInstance = translateTransformation(
		owlModel,
		rotateTransformation(owlModel, scaleTransformation(owlModel, owlPlusYLabelInstance, dScaleFactor / 2.), 2 * PI * 90. / 360., 0., 2 * PI * 90. / 360.),
		0., AXIS_LENGTH / 1.4, 0.,
		-1., 1., 1.);

	SetNameOfInstance(owlInstance, "#Y-axis");
	SetObjectProperty(
		owlInstance,
		GetPropertyByName(owlModel, "material"),
		&owlYAxisMaterialInstance,
		1);

	// Z-axis
	owlInstance = translateTransformation(
		owlModel,
		rotateTransformation(owlModel, scaleTransformation(owlModel, owlPlusZLabelInstance, dScaleFactor / 2.), 2 * PI * 270. / 360., 2 * PI * 90. / 360., 0.),
		0., 0., AXIS_LENGTH / 1.4,
		1., 1., -1.);

	SetNameOfInstance(owlInstance, "#Z-axis");
	SetObjectProperty(
		owlInstance,
		GetPropertyByName(owlModel, "material"),
		&owlZAxisMaterialInstance,
		1);

	/* Collection */
	OwlInstance owlCollectionInstance = CreateInstance(GetClassByName(owlModel, "Collection"), "#Coordinate System#");
	assert(owlCollectionInstance != 0);

	SetObjectProperty(
		owlCollectionInstance,
		GetPropertyByName(owlModel, "objects"),
		vecInstances.data(),
		vecInstances.size());

	attachModel(L"_COORDINATE_SYSTEM_", owlModel);
}

// ************************************************************************************************
_coordinate_system_model::_coordinate_system_model(_controller* pController)
	: _coordinate_system_model_base()
	, _decoration()
	, m_pController(pController)
{
	assert(m_pController != nullptr);

	create();
}

/*virtual*/ _coordinate_system_model::~_coordinate_system_model()
{
}

/*virtual*/ bool _coordinate_system_model::prepareScene(_oglScene* pScene) /*override*/
{
	assert(pScene != nullptr);

	int iWidth = 0;
	int iHeight = 0;
	pScene->_getDimensions(iWidth, iHeight);

	float fWorldXmin = FLT_MAX;
	float fWorldXmax = -FLT_MAX;
	float fWorldYmin = FLT_MAX;
	float fWorldYmax = -FLT_MAX;
	float fWorldZmin = FLT_MAX;
	float fWorldZmax = -FLT_MAX;
	m_pController->getWorldDimensions(fWorldXmin, fWorldXmax, fWorldYmin, fWorldYmax, fWorldZmin, fWorldZmax);

	pScene->_prepare(
		true,
		0, 0,
		iWidth, iHeight,
		fWorldXmin, fWorldXmax,
		fWorldYmin, fWorldYmax,
		fWorldZmin, fWorldZmax,
		false,
		false);

	return true;
}

/*virtual*/ void _coordinate_system_model::onModelUpdated() /*override*/
{
	create();
}

/*virtual*/ void _coordinate_system_model::preLoad() /*override*/
{
	getInstancesDefaultEnableState();

	float fWorldXmin = FLT_MAX;
	float fWorldXmax = -FLT_MAX;
	float fWorldYmin = FLT_MAX;
	float fWorldYmax = -FLT_MAX;
	float fWorldZmin = FLT_MAX;
	float fWorldZmax = -FLT_MAX;
	m_pController->getWorldDimensions(fWorldXmin, fWorldXmax, fWorldYmin, fWorldYmax, fWorldZmin, fWorldZmax);

	TRACE(L"\n*** SetVertexBufferOffset *** => x/y/z: %.16f, %.16f, %.16f",
		-fWorldXmin,
		-fWorldYmin,
		-fWorldZmin);

	// http://rdf.bg/gkdoc/CP64/SetVertexBufferOffset.html
	SetVertexBufferOffset(
		getOwlModel(),
		-fWorldXmin,
		-fWorldYmin,
		-fWorldZmin);

	// http://rdf.bg/gkdoc/CP64/ClearedExternalBuffers.html
	ClearedExternalBuffers(getOwlModel());
}

// ************************************************************************************************
_world_coordinate_system_model::_world_coordinate_system_model(_controller* pController)
	: _coordinate_system_model_base()
	, _decoration()
	, m_pController(pController)
{
	assert(m_pController != nullptr);

	create();
}

/*virtual*/ _world_coordinate_system_model::~_world_coordinate_system_model()
{}

/*virtual*/ void _world_coordinate_system_model::onModelUpdated() /*override*/
{
	create();
}

/*virtual*/ void _world_coordinate_system_model::preLoad() /*override*/
{
	getInstancesDefaultEnableState();

	float fWorldXmin = FLT_MAX;
	float fWorldXmax = -FLT_MAX;
	float fWorldYmin = FLT_MAX;
	float fWorldYmax = -FLT_MAX;
	float fWorldZmin = FLT_MAX;
	float fWorldZmax = -FLT_MAX;
	m_pController->getWorldDimensions(fWorldXmin, fWorldXmax, fWorldYmin, fWorldYmax, fWorldZmin, fWorldZmax);

	TRACE(L"\n*** SetVertexBufferOffset *** => x/y/z: %.16f, %.16f, %.16f",
		fWorldXmin,
		fWorldYmin,
		fWorldZmin);

	// http://rdf.bg/gkdoc/CP64/SetVertexBufferOffset.html
	SetVertexBufferOffset(
		getOwlModel(),
		fWorldXmin,
		fWorldYmin,
		fWorldZmin);

	// http://rdf.bg/gkdoc/CP64/ClearedExternalBuffers.html
	ClearedExternalBuffers(getOwlModel());
}

// ************************************************************************************************
/*static*/ const int _navigator_model::NAVIGATION_VIEW_LENGTH = 250;

_navigator_model::_navigator_model()
	: _rdf_model()
	, _decoration()
	, m_pTextBuilder(new _text_builder())
{
	create();
}

/*virtual*/ _navigator_model::~_navigator_model()
{
	delete m_pTextBuilder;
}

/*virtual*/ bool _navigator_model::prepareScene(_oglScene* pScene) /*override*/
{
	assert(pScene != nullptr);

	int iWidth = 0;
	int iHeight = 0;
	pScene->_getDimensions(iWidth, iHeight);

	float fXmin = FLT_MAX;
	float fXmax = -FLT_MAX;
	float fYmin = FLT_MAX;
	float fYmax = -FLT_MAX;
	float fZmin = FLT_MAX;
	float fZmax = -FLT_MAX;
	getDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	pScene->_prepare(
		true,
		iWidth - NAVIGATION_VIEW_LENGTH, 0,
		NAVIGATION_VIEW_LENGTH, NAVIGATION_VIEW_LENGTH,
		fXmin, fXmax,
		fYmin, fYmax,
		fZmin, fZmax,
		false,
		false);

	return true;
}

/*virtual*/ void _navigator_model::preLoad() /*override*/
{
	getInstancesDefaultEnableState();
}

void _navigator_model::create()
{
	OwlModel owlModel = CreateModel();
	assert(owlModel != 0);

	m_pTextBuilder->initialize(owlModel);

	// Cube (BoundaryRepresentations)
	{
		auto pAmbient = GEOM::ColorComponent::Create(owlModel);
		pAmbient.set_R(.1);
		pAmbient.set_G(.1);
		pAmbient.set_B(.1);
		pAmbient.set_W(.05);

		auto pColor = GEOM::Color::Create(owlModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(owlModel);
		pMaterial.set_color(pColor);

		vector<double> vecVertices =
		{
			-.75, -.75, 0, // 1 (Bottom/Left)
			.75, -.75, 0,  // 2 (Bottom/Right)
			.75, .75, 0,   // 3 (Top/Right)
			-.75, .75, 0,  // 4 (Top/Left)
		};
		vector<int64_t> vecIndices =
		{
			0, 1, 2, 3, -1,
		};

		auto pBoundaryRepresentation = GEOM::BoundaryRepresentation::Create(owlModel);
		pBoundaryRepresentation.set_material(pMaterial);
		pBoundaryRepresentation.set_vertices(vecVertices.data(), vecVertices.size());
		pBoundaryRepresentation.set_indices(vecIndices.data(), vecIndices.size());

		// Front
		OwlInstance owlInstance = translateTransformation(
			owlModel,
			rotateTransformation(owlModel, (int64_t)pBoundaryRepresentation, 2 * PI * 90. / 360., 0., 0.),
			0., -.75, 0.,
			1., -1., 1.);
		SetNameOfInstance(owlInstance, "#front");

		// Back
		owlInstance = translateTransformation(
			owlModel,
			rotateTransformation(owlModel, (int64_t)pBoundaryRepresentation, 2 * PI * 90. / 360., 0., 0.),
			0., .75, 0.,
			-1., 1., 1.);
		SetNameOfInstance(owlInstance, "#back");

		// Top
		owlInstance = translateTransformation(
			owlModel,
			(int64_t)pBoundaryRepresentation,
			0., 0., .75,
			1., 1., -1.);
		SetNameOfInstance(owlInstance, "#top");

		// Bottom
		owlInstance = translateTransformation(
			owlModel,
			(int64_t)pBoundaryRepresentation,
			0., 0., -.75,
			1, 1., 1.);
		SetNameOfInstance(owlInstance, "#bottom");

		// Left
		owlInstance = translateTransformation(
			owlModel,
			rotateTransformation(owlModel, (int64_t)pBoundaryRepresentation, 2 * PI * 90. / 360., 0., 2 * PI * 90. / 360.),
			-.75, 0., 0.,
			1., -1., 1.);
		SetNameOfInstance(owlInstance, "#left");

		// Right
		owlInstance = translateTransformation(
			owlModel,
			rotateTransformation(owlModel, (int64_t)pBoundaryRepresentation, 2 * PI * 90. / 360., 0., 2 * PI * 90. / 360.),
			.75, 0., 0.,
			-1., 1., 1.);
		SetNameOfInstance(owlInstance, "#right");
	}

	// Sphere (Sphere)
	{
		auto pAmbient = GEOM::ColorComponent::Create(owlModel);
		pAmbient.set_R(0.);
		pAmbient.set_G(0.);
		pAmbient.set_B(1.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(owlModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(owlModel);
		pMaterial.set_color(pColor);

		auto pSphere = GEOM::Sphere::Create(owlModel);
		pSphere.set_material(pMaterial);
		pSphere.set_radius(.1);
		pSphere.set_segmentationParts(36);

		// Front/Top/Left
		OwlInstance owlInstance = translateTransformation(
			owlModel,
			(int64_t)pSphere,
			-.75, -.75, .75,
			1., 1., 1.);
		SetNameOfInstance(owlInstance, "#front-top-left");

		// Front/Top/Right
		owlInstance = translateTransformation(
			owlModel,
			(int64_t)pSphere,
			.75, -.75, .75,
			1., 1., 1.);
		SetNameOfInstance(owlInstance, "#front-top-right");

		// Front/Bottom/Left
		owlInstance = translateTransformation(
			owlModel,
			(int64_t)pSphere,
			-.75, -.75, -.75,
			1., 1., 1.);
		SetNameOfInstance(owlInstance, "#front-bottom-left");

		// Front/Bottom/Right
		owlInstance = translateTransformation(
			owlModel,
			(int64_t)pSphere,
			.75, -.75, -.75,
			1., 1., 1.);
		SetNameOfInstance(owlInstance, "#front-bottom-right");

		// Back/Top/Left
		owlInstance = translateTransformation(
			owlModel,
			(int64_t)pSphere,
			.75, .75, .75,
			1., 1., 1.);
		SetNameOfInstance(owlInstance, "#back-top-left");

		// Back/Top/Right
		owlInstance = translateTransformation(
			owlModel,
			(int64_t)pSphere,
			-.75, .75, .75,
			1., 1., 1.);
		SetNameOfInstance(owlInstance, "#back-top-right");

		// Back/Bottom/Left
		owlInstance = translateTransformation(
			owlModel,
			(int64_t)pSphere,
			.75, .75, -.75,
			1., 1., 1.);
		SetNameOfInstance(owlInstance, "#back-bottom-left");

		// Back/Bottom/Right
		owlInstance = translateTransformation(
			owlModel,
			(int64_t)pSphere,
			-.75, .75, -.75,
			1., 1., 1.);
		SetNameOfInstance(owlInstance, "#back-bottom-right");
	}

	createLabels(owlModel);

	attachModel(L"_NAVIGATOR_", owlModel);
}

void _navigator_model::createLabels(OwlModel owlModel)
{
	double dXmin = DBL_MAX;
	double dXmax = -DBL_MAX;
	double dYmin = DBL_MAX;
	double dYmax = -DBL_MAX;
	double dZmin = DBL_MAX;
	double dZmax = -DBL_MAX;

	/* Top */
	OwlInstance owlTopLabelInstance = m_pTextBuilder->buildText("top", true);
	assert(owlTopLabelInstance != 0);

	_geometry::calculateBB(
		owlTopLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Bottom */
	OwlInstance owlBottomLabelInstance = m_pTextBuilder->buildText("bottom", true);
	assert(owlBottomLabelInstance != 0);

	_geometry::calculateBB(
		owlBottomLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Front */
	OwlInstance owlFrontLabelInstance = m_pTextBuilder->buildText("front", true);
	assert(owlFrontLabelInstance != 0);

	_geometry::calculateBB(
		owlFrontLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Back */
	OwlInstance owlBackLabelInstance = m_pTextBuilder->buildText("back", true);
	assert(owlBackLabelInstance != 0);

	_geometry::calculateBB(
		owlBackLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Left */
	OwlInstance owlLeftLabelInstance = m_pTextBuilder->buildText("left", true);
	assert(owlLeftLabelInstance != 0);

	_geometry::calculateBB(
		owlLeftLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Right */
	OwlInstance owlRightLabelInstance = m_pTextBuilder->buildText("right", true);
	assert(owlRightLabelInstance != 0);

	_geometry::calculateBB(
		owlRightLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Scale Factor */
	double dMaxLength = dXmax - dXmin;
	dMaxLength = fmax(dMaxLength, dYmax - dYmin);
	dMaxLength = fmax(dMaxLength, dZmax - dZmin);

	double dScaleFactor = (1.5 * .9) / dMaxLength;

	// Front
	OwlInstance owlInstance = translateTransformation(
		owlModel,
		rotateTransformation(owlModel, scaleTransformation(owlModel, owlFrontLabelInstance, dScaleFactor), 2 * PI * 90. / 360., 0., 0.),
		0., -.751, 0.,
		1., 1., 1.);
	SetNameOfInstance(owlInstance, "#front-label");

	// Back
	owlInstance = translateTransformation(
		owlModel,
		rotateTransformation(owlModel, scaleTransformation(owlModel, owlBackLabelInstance, dScaleFactor), 2 * PI * 90. / 360., 0., 0.),
		0., .751, 0.,
		-1., 1., 1.);
	SetNameOfInstance(owlInstance, "#back-label");

	// Top
	owlInstance = translateTransformation(
		owlModel,
		scaleTransformation(owlModel, owlTopLabelInstance, dScaleFactor),
		0., 0., .751,
		1., 1., 1.);
	SetNameOfInstance(owlInstance, "#top-label");

	// Bottom
	owlInstance = translateTransformation(
		owlModel,
		scaleTransformation(owlModel, owlBottomLabelInstance, dScaleFactor),
		0., 0., -.751,
		-1, 1., 1.);
	SetNameOfInstance(owlInstance, "#bottom-label");

	// Left
	owlInstance = translateTransformation(
		owlModel,
		rotateTransformation(owlModel, scaleTransformation(owlModel, owlLeftLabelInstance, dScaleFactor), 2 * PI * 90. / 360., 0., 2 * PI * 90. / 360.),
		-.751, 0., 0.,
		1., -1., 1.);
	SetNameOfInstance(owlInstance, "#left-label");

	// Right
	owlInstance = translateTransformation(
		owlModel,
		rotateTransformation(owlModel, scaleTransformation(owlModel, owlRightLabelInstance, dScaleFactor), 2 * PI * 90. / 360., 0., 2 * PI * 90. / 360.),
		.751, 0., 0.,
		1., 1., 1.);
	SetNameOfInstance(owlInstance, "#right-label");
}

_navigator_coordinate_system_model::_navigator_coordinate_system_model()
	: _coordinate_system_model_base()
	, _decoration()
{
	create();
}

/*virtual*/ _navigator_coordinate_system_model::~_navigator_coordinate_system_model()
{
}

/*virtual*/ bool _navigator_coordinate_system_model::prepareScene(_oglScene* pScene) /*override*/
{
	const int NAVIGATION_VIEW_LENGTH = _navigator_model::NAVIGATION_VIEW_LENGTH;

	int iWidth = 0;
	int iHeight = 0;
	pScene->_getDimensions(iWidth, iHeight);

	float fXmin = FLT_MAX;
	float fXmax = -FLT_MAX;
	float fYmin = FLT_MAX;
	float fYmax = -FLT_MAX;
	float fZmin = FLT_MAX;
	float fZmax = -FLT_MAX;
	getDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	pScene->_prepare(
		true,
		iWidth - NAVIGATION_VIEW_LENGTH, 0,
		NAVIGATION_VIEW_LENGTH, NAVIGATION_VIEW_LENGTH,
		fXmin, fXmax,
		fYmin, fYmax,
		fZmin, fZmax,
		false,
		false);

	return true;
}

/*virtual*/ void _navigator_coordinate_system_model::preLoad() /*override*/
{
	getInstancesDefaultEnableState();
}