#include "_host.h"
#include "_ifc_model_structure.h"
#include "_ifc_geometry.h"

// ************************************************************************************************
_ifc_node::_ifc_node(SdaiInstance sdaiInstance, _ifc_node* pParentNode)
	: m_sdaiInstance(sdaiInstance)
	, m_pParent(pParentNode)
	, m_vecChildren()
{
}

/*virtual*/ _ifc_node::~_ifc_node()
{
	for (auto pChild : m_vecChildren) {
		delete pChild;
	}
}

wchar_t* _ifc_node::getGlobalId() const
{
	wchar_t* szGlobalId = nullptr;
	sdaiGetAttrBN(m_sdaiInstance, "GlobalId", sdaiUNICODE, &szGlobalId);
	assert(szGlobalId != nullptr);

	return szGlobalId;
}

// ************************************************************************************************
_ifc_decomposition_node::_ifc_decomposition_node(_ifc_node* pParentNode)
	: _ifc_node(0, pParentNode)
{
}

/*virtual*/ _ifc_decomposition_node::~_ifc_decomposition_node()
{
}

// ************************************************************************************************
_ifc_contains_node::_ifc_contains_node(_ifc_node* pParentNode)
	: _ifc_node(0, pParentNode)
{
}

/*virtual*/ _ifc_contains_node::~_ifc_contains_node()
{
}

// ************************************************************************************************
_ifc_model_structure::_ifc_model_structure(_ifc_model* pModel)
	: m_pModel(pModel)
	, m_pProjectNode(nullptr)
{
	assert(m_pModel != nullptr);
}

/*virtual*/ _ifc_model_structure::~_ifc_model_structure()
{
	clean();
}

#ifdef _DEBUG
void _ifc_model_structure::print(int iLevel/* = 0*/, _ifc_node* pNode/* = nullptr*/)
{
	if (pNode == nullptr) {
		pNode = m_pProjectNode;
	}
	if (pNode == nullptr) {
		return;
	}
	for (int i = 0; i < iLevel; ++i) {
		TRACE("  ");
	}
	wchar_t* szGlobalId = pNode->getGlobalId();
	if (szGlobalId != nullptr) {
		TRACE(L"%s\n", szGlobalId);
	}
	else {
		TRACE("(null)\n");
	}
	for (auto pChild : pNode->children()) {
		print(iLevel + 1, pChild);
	}
}
#endif

void _ifc_model_structure::build()
{
	// Clean
	clean();

	//
	// Root
	// 

	SdaiAggr sdaiProjectAggr = sdaiGetEntityExtentBN(m_pModel->getSdaiModel(), "IFCPROJECT");
	SdaiInteger iProjectInstancesCount = sdaiGetMemberCount(sdaiProjectAggr);
	if (iProjectInstancesCount > 0) {
		SdaiInstance sdaiProjectInstance = 0;
		engiGetAggrElement(sdaiProjectAggr, 0, sdaiINSTANCE, &sdaiProjectInstance);
		assert(sdaiProjectInstance != 0);

		loadProjectNode(sdaiProjectInstance);
		//LoadGroups(m_pModel, m_pModel->GetGroupsItems()); //#todo
		//LoadUnreferencedItems(m_pModel, m_pModel->GetUnreferencedItems()); //#todo
	} // if (iProjectInstancesCount > 0)
}

void _ifc_model_structure::loadProjectNode(SdaiInstance sdaiProjectInstance)
{
	assert(sdaiProjectInstance != 0);
	assert(m_pProjectNode == nullptr);
	m_pProjectNode = new _ifc_node(sdaiProjectInstance, nullptr);

	auto pGeometry = m_pModel->getGeometryByInstance(sdaiProjectInstance);
	if (pGeometry != nullptr) {
		_ptr<_ifc_geometry> ifcGeometry(pGeometry);
		assert(!ifcGeometry->getIsMappedItem());
		assert(pGeometry->getInstances().size() == 1);

		// decomposition/contains
		loadIsDecomposedBy(m_pProjectNode, sdaiProjectInstance);
		loadIsNestedBy(m_pProjectNode, sdaiProjectInstance);
		loadContainsElements(m_pProjectNode, sdaiProjectInstance);
	}
}

void _ifc_model_structure::loadIsDecomposedBy(_ifc_node* pParentNode, SdaiInstance sdaiInstance)
{
	assert(pParentNode != nullptr);
	assert(sdaiInstance != 0);

	SdaiAggr sdaiIsDecomposedByAggr = nullptr;
	sdaiGetAttrBN(sdaiInstance, "IsDecomposedBy", sdaiAGGR, &sdaiIsDecomposedByAggr);

	if (sdaiIsDecomposedByAggr == nullptr) {
		return;
	}

	SdaiEntity sdaiRelAggregatesEntity = sdaiGetEntity(m_pModel->getSdaiModel(), "IFCRELAGGREGATES");

	SdaiInteger iIsDecomposedByInstancesCount = sdaiGetMemberCount(sdaiIsDecomposedByAggr);
	for (SdaiInteger i = 0; i < iIsDecomposedByInstancesCount; ++i) {
		SdaiInstance sdaiIsDecomposedByInstance = 0;
		engiGetAggrElement(sdaiIsDecomposedByAggr, i, sdaiINSTANCE, &sdaiIsDecomposedByInstance);

		if (sdaiGetInstanceType(sdaiIsDecomposedByInstance) != sdaiRelAggregatesEntity) {
			continue;
		}

		auto pDecompositioNode = new _ifc_decomposition_node(pParentNode);
		pParentNode->children().push_back(pDecompositioNode);

		SdaiAggr sdaiRelatedObjectsAggr = 0;
		sdaiGetAttrBN(sdaiIsDecomposedByInstance, "RelatedObjects", sdaiAGGR, &sdaiRelatedObjectsAggr);

		SdaiInteger iRelatedObjectsInstancesCount = sdaiGetMemberCount(sdaiRelatedObjectsAggr);
		for (SdaiInteger j = 0; j < iRelatedObjectsInstancesCount; ++j) {
			SdaiInstance sdaiRelatedObjectsInstance = 0;
			engiGetAggrElement(sdaiRelatedObjectsAggr, j, sdaiINSTANCE, &sdaiRelatedObjectsInstance);

			loadInstance(pDecompositioNode, sdaiRelatedObjectsInstance);
		}
	} // for (SdaiInteger i = ...	
}

void _ifc_model_structure::loadIsNestedBy(_ifc_node* pParentNode, SdaiInstance sdaiInstance)
{
	assert(pParentNode != nullptr);
	assert(sdaiInstance != 0);

	SdaiAggr sdaiIsNestedByAggr = nullptr;
	sdaiGetAttrBN(sdaiInstance, "IsNestedBy", sdaiAGGR, &sdaiIsNestedByAggr);
	if (sdaiIsNestedByAggr == nullptr) {
		return;
	}

	SdaiEntity sdaiRelNestsEntity = sdaiGetEntity(m_pModel->getSdaiModel(), "IFCRELNESTS");

	SdaiInteger iIsNestedByInstancesCount = sdaiGetMemberCount(sdaiIsNestedByAggr);
	for (SdaiInteger i = 0; i < iIsNestedByInstancesCount; ++i) {
		SdaiInstance sdaiIsNestedByInstance = 0;
		engiGetAggrElement(sdaiIsNestedByAggr, i, sdaiINSTANCE, &sdaiIsNestedByInstance);

		if (sdaiGetInstanceType(sdaiIsNestedByInstance) != sdaiRelNestsEntity) {
			continue;
		}

		auto pDecompositioNode = new _ifc_decomposition_node(pParentNode);
		pParentNode->children().push_back(pDecompositioNode);

		SdaiAggr sdaiRelatedObjectsAggr = 0;
		sdaiGetAttrBN(sdaiIsNestedByInstance, "RelatedObjects", sdaiAGGR, &sdaiRelatedObjectsAggr);

		SdaiInteger iRelatedObjectsInstancesCount = sdaiGetMemberCount(sdaiRelatedObjectsAggr);
		for (SdaiInteger j = 0; j < iRelatedObjectsInstancesCount; ++j) {
			SdaiInstance sdaiRelatedObjectsInstance = 0;
			engiGetAggrElement(sdaiRelatedObjectsAggr, j, sdaiINSTANCE, &sdaiRelatedObjectsInstance);

			loadInstance(pDecompositioNode, sdaiRelatedObjectsInstance);
		}
	} // for (SdaiInteger i = ...	
}

void _ifc_model_structure::loadContainsElements(_ifc_node* pParentNode, SdaiInstance sdaiInstance)
{
	assert(pParentNode != nullptr);
	assert(sdaiInstance != 0);

	SdaiAggr sdaiContainsElementsAggr = nullptr;
	sdaiGetAttrBN(sdaiInstance, "ContainsElements", sdaiAGGR, &sdaiContainsElementsAggr);
	if (sdaiContainsElementsAggr == nullptr) {
		return;
	}

	SdaiEntity sdaiRelContainedInSpatialStructureEntity = sdaiGetEntity(m_pModel->getSdaiModel(), "IFCRELCONTAINEDINSPATIALSTRUCTURE");
	SdaiInteger iContainsElementsInstancesCount = sdaiGetMemberCount(sdaiContainsElementsAggr);
	for (SdaiInteger i = 0; i < iContainsElementsInstancesCount; ++i) {
		SdaiInstance sdaiContainsElementsInstance = 0;
		engiGetAggrElement(sdaiContainsElementsAggr, i, sdaiINSTANCE, &sdaiContainsElementsInstance);

		if (sdaiGetInstanceType(sdaiContainsElementsInstance) != sdaiRelContainedInSpatialStructureEntity) {
			continue;
		}

		auto pContainsNode = new _ifc_contains_node(pParentNode);
		pParentNode->children().push_back(pContainsNode);

		SdaiAggr sdaiRelatedElementsInstances = 0;
		sdaiGetAttrBN(sdaiContainsElementsInstance, "RelatedElements", sdaiAGGR, &sdaiRelatedElementsInstances);

		SdaiInteger iIFCRelatedElementsInstancesCount = sdaiGetMemberCount(sdaiRelatedElementsInstances);
		for (SdaiInteger j = 0; j < iIFCRelatedElementsInstancesCount; ++j) {
			SdaiInstance sdaiRelatedElementsInstance = 0;
			engiGetAggrElement(sdaiRelatedElementsInstances, j, sdaiINSTANCE, &sdaiRelatedElementsInstance);

			loadInstance(pContainsNode, sdaiRelatedElementsInstance);
		}
	} // for (SdaiInteger i = ...
}

void _ifc_model_structure::loadInstance(_ifc_node* pParentNode, SdaiInstance sdaiInstance)
{
	assert(pParentNode != nullptr);
	assert(sdaiInstance != 0);

	auto pGeometry = m_pModel->getGeometryByInstance(sdaiInstance);
	if (pGeometry != nullptr) {
		_ptr<_ifc_geometry> ifcGeometry(pGeometry);
		assert(!ifcGeometry->getIsMappedItem());
		assert(pGeometry->getInstances().size() == 1);

		_ifc_node* pInstanceNode = new _ifc_node(sdaiInstance, pParentNode);
		pParentNode->children().push_back(pInstanceNode);

		// decomposition/contains
		loadIsDecomposedBy(pInstanceNode, sdaiInstance);
		loadIsNestedBy(pInstanceNode, sdaiInstance);
		loadContainsElements(pInstanceNode, sdaiInstance);
	}
}

void _ifc_model_structure::clean()
{
	if (m_pProjectNode != nullptr) {
		delete m_pProjectNode;
		m_pProjectNode = nullptr;
	}
}