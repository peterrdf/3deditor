#pragma once

#include "_ap242_model.h"

#include <string>
#include <vector>
using namespace std;

// ************************************************************************************************
typedef _vector_sequential_iterator<_instance> _instance_iterator;

// ************************************************************************************************
class _ap242_node
{

private: // Members

	SdaiInstance m_sdaiInstance;
	string m_strId;
	_ap242_node* m_pParent;
	vector<_ap242_node*> m_vecChildren;

public: // Methods

	_ap242_node(SdaiInstance sdaiInstance, const string& strId, _ap242_node* pParentNode);
	virtual ~_ap242_node();

public: // Properties

	SdaiInstance getSdaiInstance() const { return m_sdaiInstance; }
	const string& getId() const { return m_strId; }
	_ap242_node* getParent() const { return m_pParent; }
	vector<_ap242_node*>& children() { return m_vecChildren; }
};

// ************************************************************************************************
class _ap242_model_structure
{

private: // Members

	_ap242_model* m_pModel;
	vector<_ap242_node*> m_vecRootProducts;

	// Cache	
	map<_ap242_product_definition*, _instance_iterator*> m_mapInstanceIterators;

public: // Methods

	_ap242_model_structure(_ap242_model* pModel);
	virtual ~_ap242_model_structure();

	void build();

#ifdef _DEBUG
	void print();
	void print(int iLevel, _ap242_node* pNode);
#endif

protected: // Methods

	void loadProductNode(_ap242_node* pParentNode, _ap242_product_definition* pProduct);
	void clean();

public: // Properties

	_ap242_model* getModel() const { return m_pModel; }
	const vector<_ap242_node*>& getRootProducts() { return m_vecRootProducts; }
};

