#pragma once

#include "_ap242_model.h"
#include "_ap242_instance.h"

#include <string>
#include <vector>
#include <map>

// ************************************************************************************************
typedef _vector_sequential_iterator<_instance> _instance_iterator;

// ************************************************************************************************
enum class _ap242_node_type : int {
	Model = 0,
	ProductDefinition,
	ProductShape,
	ProductShapeRepresentation,
	ProductShapeRepresentationItem,
	Assembly,
	ProductInstance,
	DraughtingModel,
	AnnotationPlane,
	DraughtingCallout,
};

// ************************************************************************************************
class _ap242_node {

private: // Members

	_ap242_node_type m_type;
	SdaiInstance m_sdaiInstance;
	_ap242_instance* m_pInstance;
	int64_t m_iId;
	std::string m_strId;
	_ap242_node* m_pParent;
	std::vector<_ap242_node*> m_vecChildren;

public: // Methods

	_ap242_node(_ap242_node_type type, SdaiInstance sdaiInstance, _ap242_instance* pInstance, const std::string& strId, _ap242_node* pParentNode);
	virtual ~_ap242_node();

public: // Properties

	_ap242_node_type getType() const { return m_type; }
	SdaiInstance getSdaiInstance() const { return m_sdaiInstance != 0 ? m_sdaiInstance : (m_pInstance != nullptr ? m_pInstance->getSdaiInstance() : 0); }
	_ap242_instance* getInstance() const { return m_pInstance; }
	int64_t& id() { return m_iId; }
	const std::string& getId() const { return m_strId; }
	_ap242_node* getParent() const { return m_pParent; }
	std::vector<_ap242_node*>& children() { return m_vecChildren; }
};

// ************************************************************************************************
class _ap242_model_node : public _ap242_node {

private: // Members

	_ap242_model* m_pModel;

public: // Methods

	_ap242_model_node(_ap242_model* pModel);
	virtual ~_ap242_model_node();
};

// ************************************************************************************************
class _ap242_model_structure {

private: // Members

	_ap242_model* m_pModel;

	// Cache
	_ap242_model_node* m_pModelNode;
	std::vector<_ap242_node*> m_vecRootProducts;
	std::map<_ap242_instance*, _ap242_node*> m_mapInstance2Node;
	std::map<_ap242_geometry*, _instance_iterator*> m_mapInstanceIterators;

public: // Methods

	_ap242_model_structure(_ap242_model* pModel);
	virtual ~_ap242_model_structure();

	void build();

#ifdef _DEBUG
	void print();
	void print(int iLevel, _ap242_node* pNode);
#endif

	void getInstancePath(_ap242_instance* pInstance, std::vector<_ap242_node*>& vecPath);
	void getNodeChildren(_ap242_node* pNode, vector<_ap242_node*>& vecChildren, bool bRecursive);
	bool hasChild(_ap242_node* pParentNode, int64_t iId);

protected: // Methods

	void loadProductNode(_ap242_node* pParentNode, _ap242_product_definition* pProduct);
	void clean();

public: // Properties

	_ap242_model* getModel() const { return m_pModel; }
	_ap242_model_node* getModelNode() const { return m_pModelNode; }
	const std::vector<_ap242_node*>& getRootProducts() { return m_vecRootProducts; }
	const std::map<_ap242_instance*, _ap242_node*>& getInstance2NodeMap() const { return m_mapInstance2Node; }
};

