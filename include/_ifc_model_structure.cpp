#include "_host.h"
#include "_ifc_model_structure.h"
#include "_ifc_geometry.h"

// ************************************************************************************************
_ifc_node::_ifc_node(_ifc_instance* pInstance, _ifc_node* pParentNode)
	: m_pInstance(pInstance)
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

bool _ifc_node::hasChild(SdaiInstance sdaiInstance)
{
	for (auto pChildNode : children()) {
		if (pChildNode->getSdaiInstance() == sdaiInstance) {
			return true;
		}
		if (pChildNode->hasChild(sdaiInstance)) {
			return true;
		}
	}
	return false;
}

const wchar_t* _ifc_node::getGlobalId() const
{
	wchar_t* szGlobalId = nullptr;
	sdaiGetAttrBN(getSdaiInstance(), "GlobalId", sdaiUNICODE, &szGlobalId);
	assert(szGlobalId != nullptr);

	return szGlobalId;
}

// ************************************************************************************************
_ifc_model_node::_ifc_model_node(_ifc_model* pModel)
	: _ifc_node(nullptr, nullptr)
	, m_pModel(pModel)
{
}

/*virtual*/ _ifc_model_node::~_ifc_model_node()
{
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
_ifc_entity_node::_ifc_entity_node(const wstring& strEntityName, _ifc_node* pParentNode)
	: _ifc_node(0, pParentNode)
	, m_strEntityName(strEntityName)
{
}

/*virtual*/ _ifc_entity_node::~_ifc_entity_node()
{
}

// ************************************************************************************************
_ifc_model_structure::_ifc_model_structure(_ifc_model* pModel)
	: m_pModel(pModel)
	, m_pModelNode(nullptr)
	, m_pProjectNode(nullptr)
	, m_pGroupsNode(nullptr)
	, m_pUnreferencedNode(nullptr)
	, m_mapInstance2Node()
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

	wstring strGlobalId = L"\n";
	for (int i = 0; i < iLevel; ++i) {
		strGlobalId += L"  ";
	}

	const wchar_t* szGlobalId = pNode->getGlobalId();
	strGlobalId += L"Node GlobalId: ";
	strGlobalId += szGlobalId != nullptr ? szGlobalId : L"(null)";
#ifdef _WINDOWS
	TRACE(L"%s", strGlobalId.c_str());
#endif

	for (auto pChild : pNode->children()) {
		print(iLevel + 1, pChild);
	}
}
#endif

void _ifc_model_structure::getInstancePath(SdaiInstance sdaiInstance, vector<_ifc_node*>& vecPath)
{
	vecPath.clear();
	auto it = m_mapInstance2Node.find(sdaiInstance);
	if (it == m_mapInstance2Node.end()) {
		return;
	}
	_ifc_node* pNode = it->second;
	while (pNode != nullptr) {
		vecPath.push_back(pNode);
		pNode = pNode->getParent();
	}
	std::reverse(vecPath.begin(), vecPath.end());
}

void _ifc_model_structure::getInstanceChildren(SdaiInstance sdaiInstance, vector<SdaiInstance>& vecChildren, bool bRecursive)
{
	assert(sdaiInstance != 0);

	auto it = m_mapInstance2Node.find(sdaiInstance);
	if (it == m_mapInstance2Node.end()) {
		return;
	}

	if (!bRecursive) {
		vecChildren.clear();
	}

	_ifc_node* pNode = it->second;
	for (auto pChildNode : pNode->children()) {
		SdaiInstance sdaiChildInstance = pChildNode->getSdaiInstance();
		if (sdaiChildInstance != 0) {
			vecChildren.push_back(sdaiChildInstance);
			if (bRecursive) {
				getInstanceChildren(sdaiChildInstance, vecChildren, bRecursive);
			}
		}
		else {
			// Decomposition/Contains nodes
			for (auto pGrandChildNode : pChildNode->children()) {
				SdaiInstance sdaiGrandChildInstance = pGrandChildNode->getSdaiInstance();
				if (sdaiGrandChildInstance != 0) {
					vecChildren.push_back(sdaiGrandChildInstance);
					if (bRecursive) {
						getInstanceChildren(sdaiGrandChildInstance, vecChildren, bRecursive);
					}
				}
			}
		}
	}
}

bool _ifc_model_structure::hasChild(_ifc_node* pNode, SdaiInstance sdaiInstance)
{
	assert(pNode != nullptr);

	return pNode->hasChild(sdaiInstance);
}

void _ifc_model_structure::build()
{
	// Clean
	clean();

	//
	// Model
	// 

	assert(m_pModelNode == nullptr);
	m_pModelNode = new _ifc_model_node(m_pModel);

	SdaiAggr sdaiProjectAggr = sdaiGetEntityExtentBN(m_pModel->getSdaiModel(), "IFCPROJECT");
	SdaiInteger iProjectInstancesCount = sdaiGetMemberCount(sdaiProjectAggr);
	if (iProjectInstancesCount > 0) {
		SdaiInstance sdaiProjectInstance = 0;
		sdaiGetAggrByIndex(sdaiProjectAggr, 0, sdaiINSTANCE, &sdaiProjectInstance);
		assert(sdaiProjectInstance != 0);

		loadProjectNode(sdaiProjectInstance);
		loadGroupsNode();
		loadUnreferencedNode();
	} // if (iProjectInstancesCount > 0)
}

void _ifc_model_structure::loadProjectNode(SdaiInstance sdaiProjectInstance)
{
	assert(sdaiProjectInstance != 0);
	assert(m_pProjectNode == nullptr);

	auto pGeometry = m_pModel->getGeometryByInstance(sdaiProjectInstance);
	if (pGeometry != nullptr) {
		_ptr<_ifc_geometry> ifcGeometry(pGeometry);
		assert(!ifcGeometry->getIsMappedItem());
		assert(pGeometry->getInstances().size() == 1);

		m_pProjectNode = new _ifc_node(_ptr<_ifc_instance>(ifcGeometry->getInstances()[0]), m_pModelNode);
		m_pModelNode->children().push_back(m_pProjectNode);

		assert(m_mapInstance2Node.find(sdaiProjectInstance) == m_mapInstance2Node.end());
		m_mapInstance2Node[sdaiProjectInstance] = m_pProjectNode;

		// decomposition/contains
		loadIsDecomposedBy(m_pProjectNode, sdaiProjectInstance);
		loadIsNestedBy(m_pProjectNode, sdaiProjectInstance);
		loadContainsElements(m_pProjectNode, sdaiProjectInstance);
		loadBoundedBy(m_pProjectNode, sdaiProjectInstance);
		loadHasOpenings(m_pProjectNode, sdaiProjectInstance);
	}
}

void _ifc_model_structure::loadGroupsNode()
{
	assert(m_pGroupsNode == nullptr);

	vector<_ap_geometry*> vecGeometries;
	m_pModel->getGeometriesByType("IFCGROUP", vecGeometries);

	if (vecGeometries.empty()) {
		return;
	}

	m_pGroupsNode = new _ifc_node(nullptr, m_pModelNode);
	m_pModelNode->children().push_back(m_pGroupsNode);

	for (auto pGeometry : vecGeometries) {
		_ptr<_ifc_geometry> ifcGeometry(pGeometry);
		assert(!ifcGeometry->getIsMappedItem());
		assert(pGeometry->getInstances().size() == 1);
		_ptr<_ifc_instance> ifcInstance(pGeometry->getInstances()[0]);

		auto pGroupNode = new _ifc_node(ifcInstance, m_pGroupsNode);
		m_pGroupsNode->children().push_back(pGroupNode);

		assert(m_mapInstance2Node.find(ifcInstance->getSdaiInstance()) == m_mapInstance2Node.end());
		m_mapInstance2Node[ifcInstance->getSdaiInstance()] = pGroupNode;

		SdaiInstance sdaiIsGroupedByInstance = 0;
		sdaiGetAttrBN(ifcInstance->getSdaiInstance(), "IsGroupedBy", sdaiINSTANCE, &sdaiIsGroupedByInstance);
		if (sdaiIsGroupedByInstance != 0) {
			SdaiAggr sdaiRelatedObjectsAggr = nullptr;
			sdaiGetAttrBN(sdaiIsGroupedByInstance, "RelatedObjects", sdaiAGGR, &sdaiRelatedObjectsAggr);

			SdaiInteger iRelatedObjectsCount = sdaiGetMemberCount(sdaiRelatedObjectsAggr);
			for (SdaiInteger i = 0; i < iRelatedObjectsCount; i++) {
				SdaiInstance sdaiRelatedObject = 0;
				sdaiGetAggrByIndex(sdaiRelatedObjectsAggr, i, sdaiINSTANCE, &sdaiRelatedObject);

				auto pChildGeometry = m_pModel->getGeometryByInstance(sdaiRelatedObject);
				_ptr<_ifc_geometry> ifcChildGeometry(pChildGeometry);
				assert(!ifcChildGeometry->getIsMappedItem());
				assert(ifcChildGeometry->getInstances().size() == 1);
				_ptr<_ifc_instance> ifcChildInstance(ifcChildGeometry->getInstances()[0]);

				auto pChildNode = new _ifc_node(ifcChildInstance, pGroupNode);
				pGroupNode->children().push_back(pChildNode);

				if (m_mapInstance2Node.find(ifcChildInstance->getSdaiInstance()) == m_mapInstance2Node.end()) {
					m_mapInstance2Node[ifcChildInstance->getSdaiInstance()] = pChildNode;
				}

				// decomposition/contains
				loadIsDecomposedBy(pChildNode, sdaiRelatedObject);
				loadIsNestedBy(pChildNode, sdaiRelatedObject);
				loadContainsElements(pChildNode, sdaiRelatedObject);
				loadBoundedBy(pChildNode, sdaiRelatedObject);
				loadHasOpenings(pChildNode, sdaiRelatedObject);
			} // for (SdaiInteger i = ...
		} // if (sdaiIsGroupedByInstance != 0)
	} // for (auto pGeometry : ...
}

void _ifc_model_structure::loadUnreferencedNode()
{
	assert(m_pUnreferencedNode == nullptr);

	map<wstring, vector<_ifc_instance*>> mapUnreferencedItems;
	for (auto pGeometry : m_pModel->getGeometries()) {
		if (!pGeometry->hasGeometry()) {
			continue;
		}

		_ptr<_ifc_geometry> ifcGeometry(pGeometry);
		if (!ifcGeometry->getIsReferenced()) {
			assert(pGeometry->getInstances().size() == 1);
			_ptr<_ifc_instance> ifcInstance(pGeometry->getInstances()[0]);

			const wchar_t* szEntity = ifcGeometry->getEntityName();

			auto itUnreferencedItems = mapUnreferencedItems.find(szEntity);
			if (itUnreferencedItems == mapUnreferencedItems.end()) {
				vector<_ifc_instance*> veCIFCInstances;
				veCIFCInstances.push_back(ifcInstance.p());

				mapUnreferencedItems[szEntity] = veCIFCInstances;
			}
			else {
				itUnreferencedItems->second.push_back(ifcInstance.p());
			}
		}
	} // for (auto pGeometry : ...

	if (mapUnreferencedItems.empty()) {
		return;
	}

	m_pUnreferencedNode = new _ifc_node(nullptr, m_pModelNode);
	m_pModelNode->children().push_back(m_pUnreferencedNode);

	auto itUnreferencedItems = mapUnreferencedItems.begin();
	for (; itUnreferencedItems != mapUnreferencedItems.end(); itUnreferencedItems++) {
		vector<_ifc_instance*>& vecInstances = itUnreferencedItems->second;
		auto pEntityNode = new _ifc_entity_node(itUnreferencedItems->first, m_pUnreferencedNode);
		m_pUnreferencedNode->children().push_back(pEntityNode);

		for (auto pIfcInstance : vecInstances) {
			auto pInstanceNode = new _ifc_node(pIfcInstance, pEntityNode);
			pEntityNode->children().push_back(pInstanceNode);

			assert(m_mapInstance2Node.find(pIfcInstance->getSdaiInstance()) == m_mapInstance2Node.end());
			m_mapInstance2Node[pIfcInstance->getSdaiInstance()] = pInstanceNode;
		}
	} // for (; itUnreferencedItems != ...
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
		sdaiGetAggrByIndex(sdaiIsDecomposedByAggr, i, sdaiINSTANCE, &sdaiIsDecomposedByInstance);

		if (sdaiGetInstanceType(sdaiIsDecomposedByInstance) != sdaiRelAggregatesEntity) {
			continue;
		}

		SdaiAggr sdaiRelatedObjectsAggr = 0;
		sdaiGetAttrBN(sdaiIsDecomposedByInstance, "RelatedObjects", sdaiAGGR, &sdaiRelatedObjectsAggr);

		SdaiInteger iRelatedObjectsInstancesCount = sdaiGetMemberCount(sdaiRelatedObjectsAggr);
		if (iRelatedObjectsInstancesCount == 0) {
			continue;
		}

		auto pDecompositioNode = new _ifc_decomposition_node(pParentNode);
		pParentNode->children().push_back(pDecompositioNode);

		for (SdaiInteger j = 0; j < iRelatedObjectsInstancesCount; ++j) {
			SdaiInstance sdaiRelatedObjectsInstance = 0;
			sdaiGetAggrByIndex(sdaiRelatedObjectsAggr, j, sdaiINSTANCE, &sdaiRelatedObjectsInstance);

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
		sdaiGetAggrByIndex(sdaiIsNestedByAggr, i, sdaiINSTANCE, &sdaiIsNestedByInstance);

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
			sdaiGetAggrByIndex(sdaiRelatedObjectsAggr, j, sdaiINSTANCE, &sdaiRelatedObjectsInstance);

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
		sdaiGetAggrByIndex(sdaiContainsElementsAggr, i, sdaiINSTANCE, &sdaiContainsElementsInstance);

		if (sdaiGetInstanceType(sdaiContainsElementsInstance) != sdaiRelContainedInSpatialStructureEntity) {
			continue;
		}

		SdaiAggr sdaiRelatedElementsInstances = 0;
		sdaiGetAttrBN(sdaiContainsElementsInstance, "RelatedElements", sdaiAGGR, &sdaiRelatedElementsInstances);

		SdaiInteger iIFCRelatedElementsInstancesCount = sdaiGetMemberCount(sdaiRelatedElementsInstances);
		if (iIFCRelatedElementsInstancesCount == 0) {
			continue;
		}

		auto pContainsNode = new _ifc_contains_node(pParentNode);
		pParentNode->children().push_back(pContainsNode);

		for (SdaiInteger j = 0; j < iIFCRelatedElementsInstancesCount; ++j) {
			SdaiInstance sdaiRelatedElementsInstance = 0;
			sdaiGetAggrByIndex(sdaiRelatedElementsInstances, j, sdaiINSTANCE, &sdaiRelatedElementsInstance);

			loadInstance(pContainsNode, sdaiRelatedElementsInstance);
		}
	} // for (SdaiInteger i = ...
}

void _ifc_model_structure::loadBoundedBy(_ifc_node* pParentNode, SdaiInstance sdaiInstance)
{
	assert(pParentNode != nullptr);
	assert(sdaiInstance != 0);

	SdaiAggr sdaiBoundedByAggr = nullptr;
	sdaiGetAttrBN(sdaiInstance, "BoundedBy", sdaiAGGR, &sdaiBoundedByAggr);
	if (sdaiBoundedByAggr == nullptr) {
		return;
	}

	SdaiEntity sdaiRelSpaceBoundaryEntity = sdaiGetEntity(m_pModel->getSdaiModel(), "IFCRELSPACEBOUNDARY");
	SdaiInteger iBoundedByInstancesCount = sdaiGetMemberCount(sdaiBoundedByAggr);
	for (SdaiInteger i = 0; i < iBoundedByInstancesCount; ++i) {
		SdaiInstance sdaiBoundedByInstance = 0;
		sdaiGetAggrByIndex(sdaiBoundedByAggr, i, sdaiINSTANCE, &sdaiBoundedByInstance);

		if (sdaiGetInstanceType(sdaiBoundedByInstance) != sdaiRelSpaceBoundaryEntity) {
			continue;
		}

		loadInstance(pParentNode, sdaiBoundedByInstance);
	}
}

void _ifc_model_structure::loadHasOpenings(_ifc_node* pParentNode, SdaiInstance sdaiInstance)
{
	assert(pParentNode != nullptr);
	assert(sdaiInstance != 0);

	SdaiAggr sdaiHasOpeningsAggr = nullptr;
	sdaiGetAttrBN(sdaiInstance, "HasOpenings", sdaiAGGR, &sdaiHasOpeningsAggr);
	if (sdaiHasOpeningsAggr == nullptr) {
		return;
	}

	SdaiInteger iHasOpeningsInstancesCount = sdaiGetMemberCount(sdaiHasOpeningsAggr);
	for (SdaiInteger i = 0; i < iHasOpeningsInstancesCount; ++i) {
		SdaiInstance sdaiHasOpeningsInstance = 0;
		sdaiGetAggrByIndex(sdaiHasOpeningsAggr, i, sdaiINSTANCE, &sdaiHasOpeningsInstance);
		assert(sdaiHasOpeningsInstance != 0);

		SdaiInstance sdaiRelatedOpeningElementInstance = 0;
		sdaiGetAttrBN(sdaiHasOpeningsInstance, "RelatedOpeningElement", sdaiINSTANCE, &sdaiRelatedOpeningElementInstance);

		loadInstance(pParentNode, sdaiRelatedOpeningElementInstance);
	}
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

		_ifc_node* pInstanceNode = new _ifc_node(_ptr<_ifc_instance>(ifcGeometry->getInstances()[0]), pParentNode);
		if (m_mapInstance2Node.find(sdaiInstance) == m_mapInstance2Node.end()) {
			m_mapInstance2Node[sdaiInstance] = pInstanceNode;
		}
		pParentNode->children().push_back(pInstanceNode);

		// decomposition/contains
		loadIsDecomposedBy(pInstanceNode, sdaiInstance);
		loadIsNestedBy(pInstanceNode, sdaiInstance);
		loadContainsElements(pInstanceNode, sdaiInstance);
		loadBoundedBy(pInstanceNode, sdaiInstance);
		loadHasOpenings(pInstanceNode, sdaiInstance);
	}
}

void _ifc_model_structure::clean()
{
	if (m_pModelNode != nullptr) {
		delete m_pModelNode;
		m_pModelNode = nullptr;
	}

	m_mapInstance2Node.clear();
}