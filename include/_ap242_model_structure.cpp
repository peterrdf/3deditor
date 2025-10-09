#include "_host.h"
#include "_ap242_model_structure.h"

#include "_ap242_product_definition.h"
#include "_ap242_instance.h"
#include "_ap242_draughting_model.h"

#include "_string.h"

// ************************************************************************************************
_ap242_node::_ap242_node(SdaiInstance sdaiInstance, const string& strId, _ap242_node* pParentNode)
	: m_sdaiInstance(sdaiInstance)
	, m_strId(strId)
	, m_pParent(pParentNode)
	, m_vecChildren()
{
	assert(m_sdaiInstance != 0);
	assert(!m_strId.empty());
}

/*virtual*/ _ap242_node::~_ap242_node()
{
	for (auto pChild : m_vecChildren) {
		delete pChild;
	}
}

// ************************************************************************************************
_ap242_model_structure::_ap242_model_structure(_ap242_model* pModel)
	: m_pModel(pModel)
	, m_vecRootProducts()
	, m_mapInstanceIterators()

{
	assert(m_pModel != nullptr);
}

/*virtual*/ _ap242_model_structure::~_ap242_model_structure()
{
	clean();
}

#ifdef _DEBUG
void _ap242_model_structure::print()
{
	for (auto pRootProduct : m_vecRootProducts) {
		print(0, pRootProduct);
	}
}

void _ap242_model_structure::print(int iLevel, _ap242_node* pNode)
{
	assert(pNode != nullptr);

	for (int i = 0; i < iLevel; ++i) {
		TRACE("  ");
	}

	TRACE("%s\n", pNode->getId().c_str());

	for (auto pChild : pNode->children()) {
		print(iLevel + 1, pChild);
	}
}
#endif

void _ap242_model_structure::build()
{
	// Clean
	clean();

	//
	// Roots
	// 

	for (auto pGeometry : m_pModel->getGeometries()) {
		auto pProduct = dynamic_cast<_ap242_product_definition*>(pGeometry);
		if ((pProduct != nullptr) && (pProduct->getRelatedProducts() == 0)) {			
			loadProductNode(nullptr, pProduct);
		}
	}
	
	// Draughitng models
	for (auto pDraughtingModel : m_pModel->getDraughtingModels()) {
		for (auto pAnnotationPlane : pDraughtingModel->getAnnotationPlanes()) {
			m_vecRootProducts.push_back(new _ap242_node(
				pAnnotationPlane->getSdaiInstance(),
				_string::format("#%lld", pAnnotationPlane->getExpressID()),
				nullptr));			
		}

		for (auto pDraughtingCallout : pDraughtingModel->getDraughtingCallouts()) {
			m_vecRootProducts.push_back(new _ap242_node(
				pDraughtingCallout->getSdaiInstance(),
				_string::format("#%lld", pDraughtingCallout->getExpressID()),
				nullptr));
		}
	}
}

void _ap242_model_structure::loadProductNode(_ap242_node* pParentNode, _ap242_product_definition* pProduct)
{
	assert(pProduct != nullptr);

	// Iterator
	_instance_iterator* pInstanceIterator = nullptr;
	auto itInstanceIterator = m_mapInstanceIterators.find(pProduct);
	if (itInstanceIterator == m_mapInstanceIterators.end()) {
		pInstanceIterator = new _instance_iterator(pProduct->getInstances());
		m_mapInstanceIterators[pProduct] = pInstanceIterator;
	}
	else {
		pInstanceIterator = itInstanceIterator->second;
	}

	// Next Instance
	_ptr<_ap242_instance> apProductInstance(pInstanceIterator->getNextItem());
	if (apProductInstance) {
		auto& vecChildren = pParentNode != nullptr ? pParentNode->children() : m_vecRootProducts;
		vecChildren.push_back(new _ap242_node(
			apProductInstance->getSdaiInstance(),
			_string::format("#%lld:%lld", apProductInstance->getExpressID(), pInstanceIterator->index()),
			pParentNode));

		pParentNode = vecChildren.back();
	}
	else {
		assert(false);
	}

	// Assemblies
	for (auto itExpressID2Assembly : m_pModel->getExpressID2Assembly()) {
		if (itExpressID2Assembly.second->getRelatingProductDefinition() == pProduct) {
			pParentNode->children().push_back(new _ap242_node(
				itExpressID2Assembly.second->getSdaiInstance(), 
				_string::format("#%lld", itExpressID2Assembly.second->getExpressID()), 
				pParentNode));
			loadProductNode(pParentNode->children().back(), itExpressID2Assembly.second->getRelatedProductDefinition());
		}
	}
}

void _ap242_model_structure::clean()
{
	for (auto pRoot : m_vecRootProducts) {
		delete pRoot;
	}
	m_vecRootProducts.clear();

	for (auto itInstanceIterator : m_mapInstanceIterators) {
		delete itInstanceIterator.second;
	}
	m_mapInstanceIterators.clear();
}