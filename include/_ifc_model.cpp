#include "_host.h"
#include "_ifc_model.h"
#include "_ifc_geometry.h"
#include "_ifc_instance.h"
#include "_ptr.h"

#ifdef __EMSCRIPTEN__
#include "../../gisengine/Parsers/_string.h"
#endif

#include <cfloat>

// ************************************************************************************************
#define DEFAULT_CIRCLE_SEGMENTS 36

// ************************************************************************************************
_ifc_model::_ifc_model(bool bUseWorldCoordinates /*= false*/, bool bLoadInstancesOnDemand /*= false*/)
	: _ap_model(enumAP::IFC)
	, m_bUseWorldCoordinates(bUseWorldCoordinates)
	, m_bLoadInstancesOnDemand(bLoadInstancesOnDemand)
	, m_sdaiSpaceEntity(0)
	, m_sdaiOpeningElementEntity(0)
	, m_sdaiDistributionElementEntity(0)
	, m_sdaiElectricalElementEntity(0)
	, m_sdaiElementAssemblyEntity(0)
	, m_sdaiElementComponentEntity(0)
	, m_sdaiEquipmentElementEntity(0)
	, m_sdaiFeatureElementEntity(0)
	, m_sdaiFeatureElementSubtractionEntity(0)
	, m_sdaiFurnishingElementEntity(0)
	, m_sdaiReinforcingElementEntity(0)
	, m_sdaiTransportElementEntity(0)
	, m_sdaiVirtualElementEntity(0)
	, m_pUnitProvider(nullptr)
	, m_pPropertyProvider(nullptr)
	, m_vecMappedItemPendingUpdate()
{
}

/*virtual*/ _ifc_model::~_ifc_model()
{
	clean();
}

/*virtual*/ _instance* _ifc_model::loadInstance(int64_t iInstance) /*override*/
{
	assert(iInstance != 0);
	SdaiInstance sdaiInstance = (SdaiInstance)iInstance;

	clean(false);

	m_bUpdteVertexBuffers = true;

	OwlInstance owlInstance = _ap_geometry::buildOwlInstance(sdaiInstance);
	if (owlInstance != 0) {
		preLoadInstance(owlInstance);
	}

	auto pGeometry = createGeometry(owlInstance, sdaiInstance);
	addGeometry(pGeometry);

	auto pInstance = createInstance(_model::getNextInstanceID(), pGeometry, nullptr);
	addInstance(pInstance);

	scale();

	return pInstance;
}

/*virtual*/ void _ifc_model::zoomToInstances(const set<_instance*>& setInstances) /*override*/
{
	assert(!setInstances.empty());

	// World
	m_fBoundingSphereDiameter = 2.f;

	// Min/Max
	m_fXmin = FLT_MAX;
	m_fXmax = -FLT_MAX;
	m_fYmin = FLT_MAX;
	m_fYmax = -FLT_MAX;
	m_fZmin = FLT_MAX;
	m_fZmax = -FLT_MAX;

	for (auto pInstance : setInstances) {
		if (pInstance->getGeometry()->isPlaceholder()) {
			_ptr<_ifc_geometry> ifcGeometry(pInstance->getGeometry());
			assert(!ifcGeometry->m_vecMappedGeometries.empty());

			for (auto pMappedItemGeometry : ifcGeometry->m_vecMappedGeometries) {
				for (auto pMappedItemInstance : pMappedItemGeometry->getInstances()) {
					if (pMappedItemInstance->getOwner() == pInstance) {
						pMappedItemGeometry->calculateBB(
							pMappedItemInstance,
							m_fXmin, m_fXmax,
							m_fYmin, m_fYmax,
							m_fZmin, m_fZmax);
					}
				}
			}
		} // if (pInstance->getGeometry()->isPlaceholder())
		else {
			pInstance->getGeometry()->calculateBB(
				pInstance,
				m_fXmin, m_fXmax,
				m_fYmin, m_fYmax,
				m_fZmin, m_fZmax);
		}
	} // for (auto pInstance : ...

	if ((m_fXmin == FLT_MAX) ||
		(m_fXmax == -FLT_MAX) ||
		(m_fYmin == FLT_MAX) ||
		(m_fYmax == -FLT_MAX) ||
		(m_fZmin == FLT_MAX) ||
		(m_fZmax == -FLT_MAX)) {
		m_fXmin = -1.f;
		m_fXmax = 1.f;
		m_fYmin = -1.f;
		m_fYmax = 1.f;
		m_fZmin = -1.f;
		m_fZmax = 1.f;
	}

	m_fBoundingSphereDiameter = m_fXmax - m_fXmin;
	m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fYmax - m_fYmin);
	m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fZmax - m_fZmin);
}

/*virtual*/ void _ifc_model::resetInstancesEnabledState() /*override*/
{
	for (auto pInstance : getInstances()) {
		_ptr<_ifc_instance> ifcInstance(pInstance);
		if (ifcInstance->getOwner() == nullptr) {
			ifcInstance->setDefaultEnableState();
		}
		else {
			// Mapped Item
			pInstance->setEnable(true);
		}
	}
}

/*virtual*/ void _ifc_model::clean(bool bCloseModel/* = true*/) /*override*/
{
	_ap_model::clean(bCloseModel);

	if (bCloseModel) {
		delete m_pUnitProvider;
		m_pUnitProvider = nullptr;

		delete m_pPropertyProvider;
		m_pPropertyProvider = nullptr;
	}

	m_vecMappedItemPendingUpdate.clear();
}

/*virtual*/ void _ifc_model::attachModelCore() /*override*/
{
	if (m_bUseWorldCoordinates) {
		SetBehavior(getOwlModel(), FLAGBIT(10), FLAGBIT(10));
	}

	setBRepProperties(getSdaiModel(), 7, 0.9, 0., 20000);

	// Entities
	SdaiEntity sdaiObjectEntity = sdaiGetEntity(getSdaiModel(), "IFCOBJECT");
	m_sdaiSpaceEntity = sdaiGetEntity(getSdaiModel(), "IFCSPACE");
	m_sdaiOpeningElementEntity = sdaiGetEntity(getSdaiModel(), "IFCOPENINGELEMENT");
	m_sdaiDistributionElementEntity = sdaiGetEntity(getSdaiModel(), "IFCDISTRIBUTIONELEMENT");
	m_sdaiElectricalElementEntity = sdaiGetEntity(getSdaiModel(), "IFCELECTRICALELEMENT");
	m_sdaiElementAssemblyEntity = sdaiGetEntity(getSdaiModel(), "IFCELEMENTASSEMBLY");
	m_sdaiElementComponentEntity = sdaiGetEntity(getSdaiModel(), "IFCELEMENTCOMPONENT");
	m_sdaiEquipmentElementEntity = sdaiGetEntity(getSdaiModel(), "IFCEQUIPMENTELEMENT");
	m_sdaiFeatureElementEntity = sdaiGetEntity(getSdaiModel(), "IFCFEATUREELEMENT");
	m_sdaiFeatureElementSubtractionEntity = sdaiGetEntity(getSdaiModel(), "IFCFEATUREELEMENTSUBTRACTION");
	m_sdaiFurnishingElementEntity = sdaiGetEntity(getSdaiModel(), "IFCFURNISHINGELEMENT");
	m_sdaiReinforcingElementEntity = sdaiGetEntity(getSdaiModel(), "IFCREINFORCINGELEMENT");
	m_sdaiTransportElementEntity = sdaiGetEntity(getSdaiModel(), "IFCTRANSPORTELEMENT");
	m_sdaiVirtualElementEntity = sdaiGetEntity(getSdaiModel(), "IFCVIRTUALELEMENT");

	// Objects & Unreferenced
	if (!m_bLoadInstancesOnDemand) {
		retrieveGeometryRecursively(sdaiObjectEntity, DEFAULT_CIRCLE_SEGMENTS);
		retrieveGeometry("IFCPROJECT", DEFAULT_CIRCLE_SEGMENTS);
		retrieveGeometry("IFCRELSPACEBOUNDARY", DEFAULT_CIRCLE_SEGMENTS);

		getObjectsReferencedState();

		// Post-processing
		if (!m_vecMappedItemPendingUpdate.empty()) {
			double arOffset[3] = { 0., 0., 0. };
			GetVertexBufferOffset(getOwlModel(), arOffset);

			double dScaleFactor = getOriginalBoundingSphereDiameter() / 2.;
			for (auto& pMappedItemPendingUpdate : m_vecMappedItemPendingUpdate) {
				auto pMappedItem = pMappedItemPendingUpdate.second;

				pMappedItem->matrix._41 += arOffset[0];
				pMappedItem->matrix._42 += arOffset[1];
				pMappedItem->matrix._43 += arOffset[2];

				pMappedItem->matrix._41 /= dScaleFactor;
				pMappedItem->matrix._42 /= dScaleFactor;
				pMappedItem->matrix._43 /= dScaleFactor;

				pMappedItemPendingUpdate.first->setTransformationMatrix(&pMappedItem->matrix);

				delete pMappedItem;
			}
		}

		scale();

#ifdef _DEBUG
		int64_t iGeometriesCount = 0;
		int64_t iMappedItemsCount = 0;
		int64_t iMappedInstancesCount = 0;

		for (auto pGeometry : getGeometries()) {
			if (!pGeometry->hasGeometry() || pGeometry->isPlaceholder()) {
				continue;
			}

			_ptr<_ifc_geometry> ifcGeometry(pGeometry);
			if (ifcGeometry->getIsMappedItem()) {
				iMappedItemsCount++;
				iMappedInstancesCount += (int64_t)pGeometry->getInstances().size();
			}
			else {
				iGeometriesCount++;
			}
		}

		TRACE(L"\n*** _ifc_model *** Geometries: %lld", iGeometriesCount);
		TRACE(L"\n*** _ifc_model *** Mapped Items: %lld", iMappedItemsCount);
		TRACE(L"\n*** _ifc_model *** Mapped Instances: %lld", iMappedInstancesCount);
#endif // _DEBUG		 
	}
}

/*virtual*/ _ifc_geometry* _ifc_model::createGeometry(OwlInstance owlInstance, SdaiInstance sdaiInstance)
{
	return new _ifc_geometry(owlInstance, sdaiInstance, vector<_ifc_geometry*>());
}

/*virtual*/ _ifc_instance* _ifc_model::createInstance(int64_t iID, _ifc_geometry* pGeometry, _matrix4x3* pTransformationMatrix)
{
	return new _ifc_instance(iID, pGeometry, pTransformationMatrix);
}

void _ifc_model::getObjectsReferencedState()
{
	SdaiAggr sdaiAggr = sdaiGetEntityExtentBN(getSdaiModel(), "IFCPROJECT");
	SdaiInteger iMembersCount = sdaiGetMemberCount(sdaiAggr);
	if (iMembersCount > 0) {
		SdaiInstance sdaiProjectInstance = 0;
		engiGetAggrElement(sdaiAggr, 0, sdaiINSTANCE, &sdaiProjectInstance);

		getObjectsReferencedStateIsDecomposedBy(sdaiProjectInstance);
		getObjectsReferencedStateIsNestedBy(sdaiProjectInstance);
		getObjectsReferencedStateContainsElements(sdaiProjectInstance);
		getObjectsReferencedStateHasAssignments(sdaiProjectInstance);
		getObjectsReferencedStateBoundedBy(sdaiProjectInstance);
		getObjectsReferencedStateHasOpenings(sdaiProjectInstance);

		// Disable Unreferenced instances
		for (auto pGeometry : getGeometries()) {
			if (!_ptr<_ifc_geometry>(pGeometry)->getIsReferenced()) {
				pGeometry->enableInstances(false);
			}
		}
	} // if (iMembersCount > 0)

	getObjectsReferencedStateGroups();
}

void _ifc_model::getObjectsReferencedStateGroups()
{
	vector<_ap_geometry*> vecGeometries;
	getGeometriesByType("IFCGROUP", vecGeometries);

	for (auto pGeometry : vecGeometries) {
		SdaiInstance sdaiIsGroupedByInstance = 0;
		sdaiGetAttrBN(pGeometry->getSdaiInstance(), "IsGroupedBy", sdaiINSTANCE, &sdaiIsGroupedByInstance);
		if (sdaiIsGroupedByInstance != 0) {
			SdaiAggr sdaiRelatedObjectsAggr = nullptr;
			sdaiGetAttrBN(sdaiIsGroupedByInstance, "RelatedObjects", sdaiAGGR, &sdaiRelatedObjectsAggr);

			SdaiInteger iRelatedObjectsCount = sdaiGetMemberCount(sdaiRelatedObjectsAggr);
			for (SdaiInteger i = 0; i < iRelatedObjectsCount; i++) {
				SdaiInstance sdaiRelatedObject = 0;
				sdaiGetAggrByIndex(sdaiRelatedObjectsAggr, i, sdaiINSTANCE, &sdaiRelatedObject);

				auto pRelatedObjectGeometry = getGeometryByInstance(sdaiRelatedObject);
				if (pRelatedObjectGeometry != nullptr) {
					_ptr<_ifc_geometry>(pRelatedObjectGeometry)->m_bIsReferenced = true;
				}
			} // for (SdaiInteger i = ...
		} // if (sdaiIsGroupedByInstance != 0)
	} // for (auto pGeometry : ...
}

void _ifc_model::getObjectsReferencedStateIsDecomposedBy(SdaiInstance sdaiInstance)
{
	SdaiAggr sdaiIsDecomposedByAggr = nullptr;
	sdaiGetAttrBN(sdaiInstance, "IsDecomposedBy", sdaiAGGR, &sdaiIsDecomposedByAggr);
	if (sdaiIsDecomposedByAggr == nullptr) {
		return;
	}

	SdaiEntity sdaiRelAggregatesEntity = sdaiGetEntity(getSdaiModel(), "IFCRELAGGREGATES");

	SdaiInteger iIsDecomposedByMembersCount = sdaiGetMemberCount(sdaiIsDecomposedByAggr);
	for (SdaiInteger i = 0; i < iIsDecomposedByMembersCount; ++i) {
		SdaiInstance sdaiIsDecomposedByInstance = 0;
		engiGetAggrElement(sdaiIsDecomposedByAggr, i, sdaiINSTANCE, &sdaiIsDecomposedByInstance);
		if (sdaiGetInstanceType(sdaiIsDecomposedByInstance) != sdaiRelAggregatesEntity) {
			continue;
		}

		SdaiAggr sdaiRelatedObjectsAggr = 0;
		sdaiGetAttrBN(sdaiIsDecomposedByInstance, "RelatedObjects", sdaiAGGR, &sdaiRelatedObjectsAggr);

		SdaiInteger iRelatedObjectsMembersCount = sdaiGetMemberCount(sdaiRelatedObjectsAggr);
		for (SdaiInteger j = 0; j < iRelatedObjectsMembersCount; ++j) {
			SdaiInstance sdaiRelatedObjectsInstance = 0;
			engiGetAggrElement(sdaiRelatedObjectsAggr, j, sdaiINSTANCE, &sdaiRelatedObjectsInstance);

			getObjectsReferencedStateRecursively(sdaiRelatedObjectsInstance);
		}
	} // for (SdaiInteger i = ...
}

void _ifc_model::getObjectsReferencedStateIsNestedBy(SdaiInstance sdaiInstance)
{
	assert(sdaiInstance != 0);

	SdaiAggr sdaiIsNestedByAggr = nullptr;
	sdaiGetAttrBN(sdaiInstance, "IsNestedBy", sdaiAGGR, &sdaiIsNestedByAggr);
	if (sdaiIsNestedByAggr == nullptr) {
		return;
	}

	SdaiEntity sdaiRelNestsEntity = sdaiGetEntity(getSdaiModel(), "IFCRELNESTS");

	SdaiInteger iIsNestedByMembersCount = sdaiGetMemberCount(sdaiIsNestedByAggr);
	for (SdaiInteger i = 0; i < iIsNestedByMembersCount; ++i) {
		SdaiInstance sdaiIsNestedByInstance = 0;
		engiGetAggrElement(sdaiIsNestedByAggr, i, sdaiINSTANCE, &sdaiIsNestedByInstance);
		if (sdaiGetInstanceType(sdaiIsNestedByInstance) != sdaiRelNestsEntity) {
			continue;
		}

		SdaiAggr sdaiRelatedObjectsAggr = 0;
		sdaiGetAttrBN(sdaiIsNestedByInstance, "RelatedObjects", sdaiAGGR, &sdaiRelatedObjectsAggr);

		SdaiInteger iRelatedObjectsMembersCount = sdaiGetMemberCount(sdaiRelatedObjectsAggr);
		for (SdaiInteger j = 0; j < iRelatedObjectsMembersCount; ++j) {
			SdaiInstance sdaiRelatedObjectsInstance = 0;
			engiGetAggrElement(sdaiRelatedObjectsAggr, j, sdaiINSTANCE, &sdaiRelatedObjectsInstance);

			getObjectsReferencedStateRecursively(sdaiRelatedObjectsInstance);
		} // for (SdaiInteger j = ...
	} // for (SdaiInteger i = ...
}

void _ifc_model::getObjectsReferencedStateContainsElements(SdaiInstance sdaiInstance)
{
	assert(sdaiInstance != 0);

	SdaiAggr sdaiContainsElementsAggr = nullptr;
	sdaiGetAttrBN(sdaiInstance, "ContainsElements", sdaiAGGR, &sdaiContainsElementsAggr);
	if (sdaiContainsElementsAggr == nullptr) {
		return;
	}

	SdaiEntity iIFCRelContainedInSpatialStructureEntity = sdaiGetEntity(getSdaiModel(), "IFCRELCONTAINEDINSPATIALSTRUCTURE");

	SdaiInteger iContainsElementsMembersCount = sdaiGetMemberCount(sdaiContainsElementsAggr);
	for (SdaiInteger i = 0; i < iContainsElementsMembersCount; ++i) {
		SdaiInstance sdaiContainsElementsInstance = 0;
		engiGetAggrElement(sdaiContainsElementsAggr, i, sdaiINSTANCE, &sdaiContainsElementsInstance);

		if (sdaiGetInstanceType(sdaiContainsElementsInstance) != iIFCRelContainedInSpatialStructureEntity) {
			continue;
		}

		SdaiAggr sdaiRelatedElementsAggr = 0;
		sdaiGetAttrBN(sdaiContainsElementsInstance, "RelatedElements", sdaiAGGR, &sdaiRelatedElementsAggr);

		SdaiInteger iRelatedElementsMembersCount = sdaiGetMemberCount(sdaiRelatedElementsAggr);
		for (SdaiInteger j = 0; j < iRelatedElementsMembersCount; ++j) {
			SdaiInstance sdaiRelatedElementsInstance = 0;
			engiGetAggrElement(sdaiRelatedElementsAggr, j, sdaiINSTANCE, &sdaiRelatedElementsInstance);

			getObjectsReferencedStateRecursively(sdaiRelatedElementsInstance);
		} // for (SdaiInteger j = ...
	} // for (SdaiInteger i = ...
}

void _ifc_model::getObjectsReferencedStateHasAssignments(SdaiInstance sdaiInstance)
{
	assert(sdaiInstance != 0);

	SdaiAggr sdaiContainsElementsAggr = nullptr;
	sdaiGetAttrBN(sdaiInstance, "HasAssignments", sdaiAGGR, &sdaiContainsElementsAggr);
	if (sdaiContainsElementsAggr == nullptr) {
		return;
	}

	SdaiInteger iContainsElementsMembersCount = sdaiGetMemberCount(sdaiContainsElementsAggr);
	for (SdaiInteger i = 0; i < iContainsElementsMembersCount; ++i) {
		SdaiInstance sdaiContainsElementsInstance = 0;
		engiGetAggrElement(sdaiContainsElementsAggr, i, sdaiINSTANCE, &sdaiContainsElementsInstance);

		SdaiInstance sdaiRelatingProductInstance = 0;
		sdaiGetAttrBN(sdaiContainsElementsInstance, "RelatingProduct", sdaiINSTANCE, &sdaiRelatingProductInstance);

		if (sdaiIsKindOfBN(sdaiRelatingProductInstance, "IFCPRODUCT")) {
			getObjectsReferencedStateRecursively(sdaiRelatingProductInstance);
		}
	} // for (SdaiInteger i = ...
}

void _ifc_model::getObjectsReferencedStateBoundedBy(SdaiInstance sdaiInstance)
{
	assert(sdaiInstance != 0);

	SdaiAggr sdaiBoundedByAggr = nullptr;
	sdaiGetAttrBN(sdaiInstance, "BoundedBy", sdaiAGGR, &sdaiBoundedByAggr);

	if (sdaiBoundedByAggr == nullptr) {
		return;
	}

	SdaiInteger iBoundedByInstancesCount = sdaiGetMemberCount(sdaiBoundedByAggr);
	for (SdaiInteger i = 0; i < iBoundedByInstancesCount; ++i) {
		SdaiInstance sdaiBoundedByInstance = 0;
		engiGetAggrElement(sdaiBoundedByAggr, i, sdaiINSTANCE, &sdaiBoundedByInstance);

		if (sdaiIsKindOfBN(sdaiBoundedByInstance, "IFCRELSPACEBOUNDARY")) {
			getObjectsReferencedStateRecursively(sdaiBoundedByInstance);
		}
	}
}

void _ifc_model::getObjectsReferencedStateHasOpenings(SdaiInstance sdaiInstance)
{
	assert(sdaiInstance != 0);

	SdaiAggr sdaiHasOpeningsAggr = nullptr;
	sdaiGetAttrBN(sdaiInstance, "HasOpenings", sdaiAGGR, &sdaiHasOpeningsAggr);

	if (sdaiHasOpeningsAggr == nullptr) {
		return;
	}

	SdaiInteger iHasOpeningsInstancesCount = sdaiGetMemberCount(sdaiHasOpeningsAggr);
	for (SdaiInteger i = 0; i < iHasOpeningsInstancesCount; ++i) {
		SdaiInstance sdaiHasOpeningsInstance = 0;
		engiGetAggrElement(sdaiHasOpeningsAggr, i, sdaiINSTANCE, &sdaiHasOpeningsInstance);

		SdaiInstance sdaiRelatedOpeningElementInstance = 0;
		sdaiGetAttrBN(sdaiHasOpeningsInstance, "RelatedOpeningElement", sdaiINSTANCE, &sdaiRelatedOpeningElementInstance);

		getObjectsReferencedStateRecursively(sdaiRelatedOpeningElementInstance);
	}
}

void _ifc_model::getObjectsReferencedStateRecursively(SdaiInstance sdaiInstance)
{
	assert(sdaiInstance != 0);

	auto pGeometry = getGeometryByInstance(sdaiInstance);
	if (pGeometry != nullptr) {
		_ptr<_ifc_geometry>(pGeometry)->m_bIsReferenced = true;

		getObjectsReferencedStateIsDecomposedBy(sdaiInstance);
		getObjectsReferencedStateIsNestedBy(sdaiInstance);
		getObjectsReferencedStateContainsElements(sdaiInstance);
		getObjectsReferencedStateHasAssignments(sdaiInstance);
		getObjectsReferencedStateBoundedBy(sdaiInstance);
		getObjectsReferencedStateHasOpenings(sdaiInstance);
	}
	else {
		assert(!sdaiIsKindOfBN(sdaiInstance, "IFCPRODUCT"));
	}
}

void _ifc_model::retrieveGeometry(const char* szEntityName, SdaiInteger iCircleSegements)
{
	SdaiAggr sdaiAggr = sdaiGetEntityExtentBN(getSdaiModel(), (char*)szEntityName);
	SdaiInteger iMembersCount = sdaiGetMemberCount(sdaiAggr);
	if (iMembersCount == 0) {
		return;
	}

	for (SdaiInteger i = 0; i < iMembersCount; ++i) {
		SdaiInstance sdaiInstance = 0;
		engiGetAggrElement(sdaiAggr, i, sdaiINSTANCE, &sdaiInstance);

		loadGeometry(szEntityName, sdaiInstance, false, iCircleSegements);
	}
}

void _ifc_model::retrieveGeometryRecursively(SdaiEntity sdaiParentEntity, SdaiInteger iCircleSegments)
{
	if ((sdaiParentEntity == m_sdaiDistributionElementEntity) ||
		(sdaiParentEntity == m_sdaiElectricalElementEntity) ||
		(sdaiParentEntity == m_sdaiElementAssemblyEntity) ||
		(sdaiParentEntity == m_sdaiElementComponentEntity) ||
		(sdaiParentEntity == m_sdaiEquipmentElementEntity) ||
		(sdaiParentEntity == m_sdaiFeatureElementEntity) ||
		(sdaiParentEntity == m_sdaiFurnishingElementEntity) ||
		(sdaiParentEntity == m_sdaiTransportElementEntity) ||
		(sdaiParentEntity == m_sdaiVirtualElementEntity)) {
		iCircleSegments = 12;
	}
	else if (sdaiParentEntity == m_sdaiReinforcingElementEntity) {
		iCircleSegments = 6;
	}

	SdaiAggr sdaiAggr = sdaiGetEntityExtent(getSdaiModel(), sdaiParentEntity);
	SdaiInteger iMembersCount = sdaiGetMemberCount(sdaiAggr);
	if (iMembersCount != 0) {
		char* szParenEntityName = nullptr;
		engiGetEntityName(sdaiParentEntity, sdaiSTRING, (const char**)&szParenEntityName);

		retrieveGeometry(szParenEntityName, iCircleSegments);
	} // if (iIntancesCount != 0)

	iMembersCount = engiGetEntityCount(getSdaiModel());
	for (SdaiInteger i = 0; i < iMembersCount; i++) {
		SdaiEntity sdaiEntity = engiGetEntityElement(getSdaiModel(), i);
		if (engiGetEntityParent(sdaiEntity) == sdaiParentEntity) {
			retrieveGeometryRecursively(sdaiEntity, iCircleSegments);
		}
	}
}

_geometry* _ifc_model::loadGeometry(const char* szEntityName, SdaiInstance sdaiInstance, bool bMappedItem, SdaiInteger iCircleSegments)
{
	auto pGeometry = dynamic_cast<_ifc_geometry*>(getGeometryByInstance(sdaiInstance));
	if (pGeometry != nullptr) {
		return pGeometry;
	}

	wstring strEntity = (const wchar_t*)CA2W(szEntityName);
	std::transform(strEntity.begin(), strEntity.end(), strEntity.begin(), ::towupper);

	auto pProduct = recognizeMappedItems(sdaiInstance);
	if (pProduct != nullptr) {
		double arOffset[3] = { 0., 0., 0. };
		GetVertexBufferOffset(getOwlModel(), arOffset);
		SetVertexBufferOffset(getOwlModel(), 0., 0., 0.);

		int64_t iParenInstanceID = _model::getNextInstanceID();

		vector<_ifc_geometry*> vecMappedGeometries;
		vector<_ifc_instance*> vecMappedInstances;

		for (auto pMappedItem : pProduct->mappedItems) {
			auto pMappedGeometry = dynamic_cast<_ifc_geometry*>(getGeometryByInstance(pMappedItem->ifcRepresentationInstance));
			if (pMappedGeometry == nullptr) {
				pMappedGeometry = dynamic_cast<_ifc_geometry*>(loadGeometry(szEntityName, pMappedItem->ifcRepresentationInstance, true, iCircleSegments));
			}

			vecMappedGeometries.push_back(pMappedGeometry);

			auto pMappedInstance = createInstance(_model::getNextInstanceID(), pMappedGeometry, nullptr);
			pMappedInstance->setEnable(true);
			addInstance(pMappedInstance);

			vecMappedInstances.push_back(pMappedInstance);

			// Pending update
			m_vecMappedItemPendingUpdate.push_back({ pMappedInstance, pMappedItem });
		} // for (auto pMappedItem : ...

		delete pProduct;
		pProduct = nullptr;

		// Owner
		pGeometry = new _ifc_geometry(0, sdaiInstance, vecMappedGeometries);
		pGeometry->setDefaultShowState();
		addGeometry(pGeometry);

		// Owner
		auto pInstance = createInstance(iParenInstanceID, pGeometry, nullptr);
		pInstance->setDefaultEnableState();
		addInstance(pInstance);

		for (auto pMappedInstance : vecMappedInstances) {
			pMappedInstance->m_pOwner = pInstance;
		}

		SetVertexBufferOffset(getOwlModel(), arOffset);

		return pGeometry;
	} // if (pProduct != nullptr)

	OwlInstance owlInstance = _ap_geometry::buildOwlInstance(sdaiInstance);
	if (!bMappedItem && owlInstance != 0) {
		preLoadInstance(owlInstance);
	}

	// Set up circleSegments()
	if (iCircleSegments != DEFAULT_CIRCLE_SEGMENTS) {
		circleSegments(iCircleSegments, 5);
	}

	pGeometry = createGeometry(owlInstance, sdaiInstance);
	addGeometry(pGeometry);

	if (!bMappedItem) {
		pGeometry->setDefaultShowState();

		auto pInstance = createInstance(_model::getNextInstanceID(), pGeometry, nullptr);
		pInstance->setDefaultEnableState();
		addInstance(pInstance);
	}
	else {
		pGeometry->m_bIsMappedItem = true;
		pGeometry->m_bIsReferenced = true;
	}

	// Restore circleSegments()
	if (iCircleSegments != DEFAULT_CIRCLE_SEGMENTS) {
		circleSegments(DEFAULT_CIRCLE_SEGMENTS, 5);
	}

	return pGeometry;
}

void _ifc_model::parseMappedItem(SdaiInstance ifcMappedItemInstance, std::vector<STRUCT_INTERNAL*>* pVectorMappedItemData)
{
	assert(sdaiGetInstanceType(ifcMappedItemInstance) == sdaiGetEntity(getSdaiModel(), "IFCMAPPEDITEM"));

	/*SdaiInstance	ifcStyledItemInstance = 0;
	sdaiGetAttrBN(ifcMappedItemInstance, "StyledByItem", sdaiINSTANCE, &ifcStyledItemInstance);

	STRUCT_MATERIAL* material = nullptr;
	if (ifcStyledItemInstance) {
		assert(sdaiGetInstanceType(ifcStyledItemInstance) == sdaiGetEntity(getSdaiModel(), "IFCSTYLEDITEM"));
		OwlInstance		owlInstanceMaterial = 0;
		owlBuildInstance(getSdaiModel(), ifcStyledItemInstance, &owlInstanceMaterial);
		if (owlInstanceMaterial) {
			assert(GetInstanceClass(owlInstanceMaterial) == GetClassByName(getOwlModel(), "Material"));
			material = new STRUCT_MATERIAL;
			material->ambient = GetMaterialColorAmbient(owlInstanceMaterial);
			material->diffuse = GetMaterialColorDiffuse(owlInstanceMaterial);
			material->emissive = GetMaterialColorEmissive(owlInstanceMaterial);
			material->specular = GetMaterialColorSpecular(owlInstanceMaterial);
		}
	}*/

	SdaiInstance	ifcRepresentationMapInstance = 0;
	sdaiGetAttrBN(ifcMappedItemInstance, "MappingSource", sdaiINSTANCE, &ifcRepresentationMapInstance);

	SdaiInstance	ifcCartesianTransformationOperatorInstance = 0;
	sdaiGetAttrBN(ifcMappedItemInstance, "MappingTarget", sdaiINSTANCE, &ifcCartesianTransformationOperatorInstance);

	OwlInstance		owlInstanceCartesianTransformationOperatorMatrix = 0;
	owlBuildInstance(getSdaiModel(), ifcCartesianTransformationOperatorInstance, &owlInstanceCartesianTransformationOperatorMatrix);

	if (GetInstanceClass(owlInstanceCartesianTransformationOperatorMatrix) == GetClassByName(getOwlModel(), "Transformation"))
		owlInstanceCartesianTransformationOperatorMatrix = GetObjectProperty(owlInstanceCartesianTransformationOperatorMatrix, GetPropertyByName(getOwlModel(), "matrix"));
	assert(GetInstanceClass(owlInstanceCartesianTransformationOperatorMatrix) == GetClassByName(getOwlModel(), "Matrix"));

	SdaiInstance	ifcAxis2PlacementInstance = 0;
	sdaiGetAttrBN(ifcRepresentationMapInstance, "MappingOrigin", sdaiINSTANCE, &ifcAxis2PlacementInstance);

	OwlInstance		owlInstanceAxis2PlacementMatrix = 0;
	owlBuildInstance(getSdaiModel(), ifcAxis2PlacementInstance, &owlInstanceAxis2PlacementMatrix);
	assert((owlInstanceAxis2PlacementMatrix && GetInstanceClass(owlInstanceAxis2PlacementMatrix) == GetClassByName(getOwlModel(), "Matrix")) ||
		!owlInstanceAxis2PlacementMatrix);

	SdaiInstance	ifcRepresentationInstance = 0;
	sdaiGetAttrBN(ifcRepresentationMapInstance, "MappedRepresentation", sdaiINSTANCE, &ifcRepresentationInstance);

	OwlInstance		owlInstanceMatrix;
	if (owlInstanceCartesianTransformationOperatorMatrix && owlInstanceAxis2PlacementMatrix) {
		OwlInstance	owlInstanceMatrixMultiplication = CreateInstance(GetClassByName(getOwlModel(), "MatrixMultiplication"));
		SetObjectProperty(owlInstanceMatrixMultiplication, GetPropertyByName(getOwlModel(), "firstMatrix"), owlInstanceCartesianTransformationOperatorMatrix);
		SetObjectProperty(owlInstanceMatrixMultiplication, GetPropertyByName(getOwlModel(), "secondMatrix"), owlInstanceAxis2PlacementMatrix);
		owlInstanceMatrix = owlInstanceMatrixMultiplication;
	}
	else {
		owlInstanceMatrix = owlInstanceCartesianTransformationOperatorMatrix ? owlInstanceCartesianTransformationOperatorMatrix : owlInstanceAxis2PlacementMatrix;
	}

	SdaiAggr	itemsAGGR = nullptr;
	sdaiGetAttrBN(ifcRepresentationInstance, "Items", sdaiAGGR, &itemsAGGR);
	SdaiInteger	noItemsAGGR = sdaiGetMemberCount(itemsAGGR);
	if (noItemsAGGR) {
		for (SdaiInteger index = 0; index < noItemsAGGR; index++) {
			SdaiInstance	ifcRepresentationItemInstance = 0;
			sdaiGetAggrByIndex(itemsAGGR, index, sdaiINSTANCE, &ifcRepresentationItemInstance);

			STRUCT_INTERNAL* mappedItemData = new STRUCT_INTERNAL;
			mappedItemData->owlInstanceMatrix = owlInstanceMatrix;
			mappedItemData->ifcRepresentationInstance = ifcRepresentationItemInstance;
			//mappedItemData->material = material;
			(*pVectorMappedItemData).push_back(mappedItemData);
		}
	}
	else {
		STRUCT_INTERNAL* mappedItemData = new STRUCT_INTERNAL;
		mappedItemData->owlInstanceMatrix = owlInstanceMatrix;
		mappedItemData->ifcRepresentationInstance = ifcRepresentationInstance;
		//mappedItemData->material = material;
		(*pVectorMappedItemData).push_back(mappedItemData);
	}
}

STRUCT_IFC_PRODUCT* _ifc_model::recognizeMappedItems(SdaiInstance ifcProductInstance)
{
	assert(ifcProductInstance);

	std::vector<STRUCT_INTERNAL*> vectorMappedItemData;

	{
		//
		//	If this object has opening elements mapped items should be ignored
		//
		SdaiAggr	hasOpeningsAGGR = nullptr;
		sdaiGetAttrBN(ifcProductInstance, "HasOpenings", sdaiAGGR, &hasOpeningsAGGR);
		if (hasOpeningsAGGR && sdaiGetMemberCount(hasOpeningsAGGR))
			return	nullptr;
	}

	SdaiEntity ifcMappedItemEntity = sdaiGetEntity(getSdaiModel(), "IFCMAPPEDITEM");

	SdaiInstance ifcProductRepresentationInstance = 0;
	sdaiGetAttrBN(ifcProductInstance, "Representation", sdaiINSTANCE, &ifcProductRepresentationInstance);

	if (ifcProductRepresentationInstance) {
		SdaiAggr	ifcRepresentationInstanceAGGR = nullptr;
		sdaiGetAttrBN(ifcProductRepresentationInstance, "Representations", sdaiAGGR, &ifcRepresentationInstanceAGGR);

		SdaiInteger	noIfcRepresentationInstanceAGGR = sdaiGetMemberCount(ifcRepresentationInstanceAGGR);
		for (SdaiInteger index = 0; index < noIfcRepresentationInstanceAGGR; index++) {
			SdaiInstance	ifcRepresentationInstance = 0;
			sdaiGetAggrByIndex(ifcRepresentationInstanceAGGR, index, sdaiINSTANCE, &ifcRepresentationInstance);

			char* representationIdentifier = nullptr;
			sdaiGetAttrBN(ifcRepresentationInstance, "RepresentationIdentifier", sdaiSTRING, &representationIdentifier);
			if (Equals(representationIdentifier, "Body")) {
				SdaiAggr	ifcRepresentationItemInstanceAGGR = nullptr;
				sdaiGetAttrBN(ifcRepresentationInstance, "Items", sdaiAGGR, &ifcRepresentationItemInstanceAGGR);

				SdaiInteger	noIfcRepresentationItemInstanceAGGR = sdaiGetMemberCount(ifcRepresentationItemInstanceAGGR);
				for (SdaiInteger i = 0; i < noIfcRepresentationItemInstanceAGGR; i++) {
					SdaiInstance	ifcRepresentationItemInstance = 0;
					sdaiGetAggrByIndex(ifcRepresentationItemInstanceAGGR, i, sdaiINSTANCE, &ifcRepresentationItemInstance);

					if (sdaiGetInstanceType(ifcRepresentationItemInstance) == ifcMappedItemEntity) {
						parseMappedItem(ifcRepresentationItemInstance, &vectorMappedItemData);
					}
					else
						return	nullptr;
				}
			}
		}
	}

	if (vectorMappedItemData.size()) {
		STRUCT_IFC_PRODUCT* myIfcProduct = new STRUCT_IFC_PRODUCT();
		myIfcProduct->ifcProductInstance = ifcProductInstance;

		SdaiInstance	ifcObjectPlacementInstance = 0;
		sdaiGetAttrBN(ifcProductInstance, "ObjectPlacement", sdaiINSTANCE, &ifcObjectPlacementInstance);

		OwlInstance		owlInstanceObjectPlacementMatrix = 0;
		owlBuildInstance(getSdaiModel(), ifcObjectPlacementInstance, &owlInstanceObjectPlacementMatrix);

		assert(GetInstanceClass(owlInstanceObjectPlacementMatrix) == GetClassByName(getOwlModel(), "Matrix") ||
			GetInstanceClass(owlInstanceObjectPlacementMatrix) == GetClassByName(getOwlModel(), "InverseMatrix") ||
			GetInstanceClass(owlInstanceObjectPlacementMatrix) == GetClassByName(getOwlModel(), "MatrixMultiplication"));

		for (auto mappedItemData = vectorMappedItemData.begin(); mappedItemData != vectorMappedItemData.end(); ++mappedItemData) {
			OwlInstance	owlInstanceMatrixMultiplication = CreateInstance(GetClassByName(getOwlModel(), "MatrixMultiplication"));
			SetObjectProperty(owlInstanceMatrixMultiplication, GetPropertyByName(getOwlModel(), "firstMatrix"), (*mappedItemData)->owlInstanceMatrix);
			SetObjectProperty(owlInstanceMatrixMultiplication, GetPropertyByName(getOwlModel(), "secondMatrix"), owlInstanceObjectPlacementMatrix);

			InferenceInstance(owlInstanceMatrixMultiplication);

			double* values = nullptr;
			int64_t	card = 0;
			GetDatatypeProperty(owlInstanceMatrixMultiplication, GetPropertyByName(getOwlModel(), "coordinates"), (void**)&values, &card);
			if (card == sizeof(_matrix4x3) / sizeof(double)) {
				STRUCT_MAPPED_ITEM* myMappedItem = new STRUCT_MAPPED_ITEM;
				myMappedItem->ifcRepresentationInstance = (*mappedItemData)->ifcRepresentationInstance;
				memcpy(&myMappedItem->matrix, values, sizeof(_matrix4x3));
				myIfcProduct->mappedItems.push_back(myMappedItem);
			}
			else {
				assert(false);
				delete	myIfcProduct;
				return	nullptr;
			}

			delete* mappedItemData;
		}

		return	myIfcProduct;
	}

	return	nullptr;
}


_ifc_unit_provider* _ifc_model::getUnitProvider()
{
	if (m_pUnitProvider == nullptr) {
		m_pUnitProvider = new _ifc_unit_provider(getSdaiModel());
	}

	return m_pUnitProvider;
}

_ifc_property_provider* _ifc_model::getPropertyProvider()
{
	if (m_pPropertyProvider == nullptr) {
		m_pPropertyProvider = new _ifc_property_provider(getSdaiModel(), getUnitProvider());
	}

	return m_pPropertyProvider;
}

