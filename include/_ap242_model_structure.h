#pragma once

#include "_ap242_model.h"
#include "_ap242_geometry.h"

#include <string>
#include <vector>
#include <map>

// ************************************************************************************************
typedef _vector_sequential_iterator<_instance> _instance_iterator;

// ************************************************************************************************
enum class _ap242_node_type : int
{
	ProductDefinition = 0,
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
class _ap242_node
{

private: // Members

	_ap242_node_type m_type;
	SdaiInstance m_sdaiInstance;
	int64_t m_iId;
	std::string m_strId;
	_ap242_node* m_pParent;
	std::vector<_ap242_node*> m_vecChildren;

public: // Methods

	_ap242_node(_ap242_node_type type, SdaiInstance sdaiInstance, const std::string& strId, _ap242_node* pParentNode);
	virtual ~_ap242_node();

public: // Properties

	_ap242_node_type getType() const { return m_type; }
	SdaiInstance getSdaiInstance() const { return m_sdaiInstance; }
	int64_t& id() { return m_iId; }
	const std::string& getId() const { return m_strId; }
	_ap242_node* getParent() const { return m_pParent; }
	std::vector<_ap242_node*>& children() { return m_vecChildren; }
};

// ************************************************************************************************
class _ap242_model_structure
{

private: // Members

	_ap242_model* m_pModel;
	std::vector<_ap242_node*> m_vecRootProducts;

	// Cache	
	std::map<_ap242_geometry*, _instance_iterator*> m_mapInstanceIterators;

public: // Methods

	_ap242_model_structure(_ap242_model* pModel);
	virtual ~_ap242_model_structure();

	void build();

#ifdef _DEBUG
	void print();
	void print(int iLevel, _ap242_node* pNode);
#endif

	void getNodeChildren(_ap242_node* pNode, vector<_ap242_node*>& vecChildren, bool bRecursive);
	bool hasChild(_ap242_node* pParentNode, int64_t iId);

protected: // Methods

	void loadProductNode(_ap242_node* pParentNode, _ap242_product_definition* pProduct);
	void clean();

public: // Properties

	_ap242_model* getModel() const { return m_pModel; }
	const std::vector<_ap242_node*>& getRootProducts() { return m_vecRootProducts; }
};

