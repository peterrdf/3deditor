#include "_host.h"
#include "_ap242_model_structure.h"

#include "_ap242_product_definition.h"
#include "_ap242_instance.h"
#include "_ap242_draughting_model.h"

#include "_string.h"

// ************************************************************************************************
_ap242_node::_ap242_node(_ap242_node_type type, SdaiInstance sdaiInstance, _ap242_instance* pInstance, const string& strId, _ap242_node* pParentNode)
	: m_type(type)
	, m_sdaiInstance(sdaiInstance)
	, m_pInstance(pInstance)
	, m_iId(-1)
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
_ap242_model_node::_ap242_model_node(_ap242_model* pModel)
	: _ap242_node(_ap242_node_type::Model, pModel->getSdaiModel(), nullptr, (LPCSTR)CW2A(pModel->getPath()), nullptr)
	, m_pModel(pModel)
{
	assert(m_pModel != nullptr);
}

/*virtual*/ _ap242_model_node::~_ap242_model_node()
{
}

// ************************************************************************************************
_ap242_model_structure::_ap242_model_structure(_ap242_model* pModel)
	: m_pModel(pModel)
	, m_pModelNode(nullptr)
	, m_vecRootProducts()
	, m_mapInstance2Node()
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

	wstring strId = L"\n";
	for (int i = 0; i < iLevel; ++i) {
		strId += L"  ";
	}

	strId += L"Node ID: ";
	strId += (const wchar_t*)CA2W(pNode->getId().c_str());
#ifdef _WINDOWS
	TRACE(L"%s", strId.c_str());
#endif

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
	// Model
	//
	assert(m_pModelNode == nullptr);
	m_pModelNode = new _ap242_model_node(m_pModel);

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
		auto pDraughtingModelNode = new _ap242_node(
			_ap242_node_type::DraughtingModel,
			pDraughtingModel->getSdaiInstance(),
			nullptr,
			_string::format("#%lld", pDraughtingModel->getExpressID()),
			nullptr);
		m_vecRootProducts.push_back(pDraughtingModelNode);
		m_pModelNode->children().push_back(pDraughtingModelNode);

		for (auto pAnnotationPlane : pDraughtingModel->getAnnotationPlanes()) {
			assert(pAnnotationPlane->getInstances().size() == 1);
			_ptr<_ap242_instance> apAnnotationPlaneInstance(pAnnotationPlane->getInstances().front());

			pDraughtingModelNode->children().push_back(new _ap242_node(
				_ap242_node_type::AnnotationPlane,
				apAnnotationPlaneInstance->getSdaiInstance(),
				apAnnotationPlaneInstance,
				_string::format("#%lld:0", pAnnotationPlane->getExpressID()),
				pDraughtingModelNode));
			pDraughtingModelNode->children().back()->id() = pAnnotationPlane->getInstances().front()->getID();

			assert(m_mapInstance2Node.find(apAnnotationPlaneInstance) == m_mapInstance2Node.end());
			m_mapInstance2Node[apAnnotationPlaneInstance] = pDraughtingModelNode->children().back();
		}

		for (auto pDraughtingCallout : pDraughtingModel->getDraughtingCallouts()) {
			assert(pDraughtingCallout->getInstances().size() == 1);
			_ptr<_ap242_instance> apDraughtingCalloutInstance(pDraughtingCallout->getInstances().front());

			pDraughtingModelNode->children().push_back(new _ap242_node(
				_ap242_node_type::DraughtingCallout,
				apDraughtingCalloutInstance->getSdaiInstance(),
				apDraughtingCalloutInstance,
				_string::format("#%lld:0", pDraughtingCallout->getExpressID()),
				pDraughtingModelNode));
			pDraughtingModelNode->children().back()->id() = pDraughtingCallout->getInstances().front()->getID();

			assert(m_mapInstance2Node.find(apDraughtingCalloutInstance) == m_mapInstance2Node.end());
			m_mapInstance2Node[apDraughtingCalloutInstance] = pDraughtingModelNode->children().back();
		}
	}
}

void _ap242_model_structure::getInstancePath(_ap242_instance* pInstance, std::vector<_ap242_node*>& vecPath)
{
	assert(pInstance != nullptr);

	vecPath.clear();
	auto it = m_mapInstance2Node.find(pInstance);
	if (it == m_mapInstance2Node.end()) {
		return;
	}
	_ap242_node* pNode = it->second;
	while (pNode != nullptr) {
		vecPath.push_back(pNode);
		pNode = pNode->getParent();
	}
	std::reverse(vecPath.begin(), vecPath.end());
}

void _ap242_model_structure::getNodeChildren(_ap242_node* pNode, vector<_ap242_node*>& vecChildren, bool bRecursive)
{
	assert(pNode != nullptr);
	if (!bRecursive) {
		vecChildren.clear();
	}
	for (auto pChildNode : pNode->children()) {
		vecChildren.push_back(pChildNode);
		if (bRecursive) {
			getNodeChildren(pChildNode, vecChildren, bRecursive);
		}
	}
}

bool _ap242_model_structure::hasChild(_ap242_node* pParentNode, int64_t iId)
{
	assert(pParentNode != nullptr);
	for (auto pChildNode : pParentNode->children()) {
		if (pChildNode->id() == iId) {
			return true;
		}
		if (hasChild(pChildNode, iId)) {
			return true;
		}
	}
	return false;
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
			_ap242_node_type::ProductInstance,
			apProductInstance->getSdaiInstance(),
			apProductInstance,
			_string::format("#%lld:%lld", apProductInstance->getExpressID(), pInstanceIterator->index()),
			pParentNode));
		vecChildren.back()->id() = pInstanceIterator->data()[pInstanceIterator->index()]->getID();
		if (pParentNode == nullptr) {
			m_pModelNode->children().push_back(vecChildren.back());
		}

		assert(m_mapInstance2Node.find(apProductInstance) == m_mapInstance2Node.end());
		m_mapInstance2Node[apProductInstance] = vecChildren.back();

		pParentNode = vecChildren.back();

		if (pProduct->getProductShape()) {
			auto pProductShape = pProduct->getProductShape();
			assert(pProductShape->getInstances().size() == 1);
			_ptr<_ap242_instance> apProductShapeInstance(pProductShape->getInstances().front());

			auto pProductShapeNode = new _ap242_node(
				_ap242_node_type::ProductShape,
				apProductShapeInstance->getSdaiInstance(),
				apProductShapeInstance,
				_string::format("#%lld", pProductShape->getExpressID()),
				pParentNode);
			pParentNode->children().push_back(pProductShapeNode);

			if (m_mapInstance2Node.find(apProductShapeInstance) == m_mapInstance2Node.end()) {
				m_mapInstance2Node[apProductShapeInstance] = pProductShapeNode;
			}

			for (auto pProductShapeRepresentation : pProductShape->getProductShapeRepresentations()) {
				assert(pProductShapeRepresentation->getInstances().size() == 1);
				_ptr<_ap242_instance> apProductShapeRepresentationInstance(pProductShapeRepresentation->getInstances().front());

				auto pProductShapeRepresentationNode = new _ap242_node(
					_ap242_node_type::ProductShapeRepresentation,
					apProductShapeRepresentationInstance->getSdaiInstance(),
					apProductShapeRepresentationInstance,
					_string::format("#%lld", pProductShapeRepresentation->getExpressID()),
					pProductShapeNode);
				pProductShapeNode->children().push_back(pProductShapeRepresentationNode);

				if (m_mapInstance2Node.find(apProductShapeRepresentationInstance) == m_mapInstance2Node.end()) {
					m_mapInstance2Node[apProductShapeRepresentationInstance] = pProductShapeRepresentationNode;
				}

				for (auto pRepresentationItem : pProductShapeRepresentation->getRepresentationItems()) {
					pInstanceIterator = nullptr;
					itInstanceIterator = m_mapInstanceIterators.find(pRepresentationItem);
					if (itInstanceIterator == m_mapInstanceIterators.end()) {
						pInstanceIterator = new _instance_iterator(pRepresentationItem->getInstances());
						m_mapInstanceIterators[pRepresentationItem] = pInstanceIterator;
					}
					else {
						pInstanceIterator = itInstanceIterator->second;
					}

					_ptr<_ap242_product_shape_representation_item_instance> apRepresentationItemInstance(pInstanceIterator->getNextItem());
					if (apRepresentationItemInstance) {
						pProductShapeRepresentationNode->children().push_back(new _ap242_node(
							_ap242_node_type::ProductShapeRepresentationItem,
							apRepresentationItemInstance->getSdaiInstance(),
							apRepresentationItemInstance,
							_string::format("#%lld:%lld", apRepresentationItemInstance->getExpressID(), pInstanceIterator->index()),
							pProductShapeRepresentationNode));
						pProductShapeRepresentationNode->children().back()->id() = pInstanceIterator->data()[pInstanceIterator->index()]->getID();

						assert(m_mapInstance2Node.find(apRepresentationItemInstance) == m_mapInstance2Node.end());
						m_mapInstance2Node[apRepresentationItemInstance] = pProductShapeRepresentationNode->children().back();
					}
					else {
						assert(false);
					}
				}
			}
		}
	}
	else {
		assert(false);
	}

	// Assemblies
	for (auto itExpressID2Assembly : m_pModel->getExpressID2Assembly()) {
		if (itExpressID2Assembly.second->getRelatingProductDefinition() == pProduct) {
			pParentNode->children().push_back(new _ap242_node(
				_ap242_node_type::Assembly,
				itExpressID2Assembly.second->getSdaiInstance(),
				nullptr,
				_string::format("#%lld", itExpressID2Assembly.second->getExpressID()),
				pParentNode));

			loadProductNode(pParentNode->children().back(), itExpressID2Assembly.second->getRelatedProductDefinition());
		}
	}
}

void _ap242_model_structure::clean()
{
	delete m_pModelNode;
	m_pModelNode = nullptr;

	m_mapInstance2Node.clear();

	for (auto itInstanceIterator : m_mapInstanceIterators) {
		delete itInstanceIterator.second;
	}
	m_mapInstanceIterators.clear();
}