#pragma once

#include "_ifc_instance.h"
#include "_ifc_model.h"

#include <string>
#include <vector>
using namespace std;

// ************************************************************************************************
#define DECOMPOSITION_NODE	L"decomposition"
#define CONTAINS_NODE		L"contains"

// ************************************************************************************************
class _ifc_node
{

private: // Members

	SdaiInstance m_sdaiInstance;
	_ifc_node* m_pParent;
	vector<_ifc_node*> m_vecChildren;

public: // Methods

	_ifc_node(SdaiInstance sdaiInstance, _ifc_node* pParentNode);
	virtual ~_ifc_node();

public: // Properties

	SdaiInstance getSdaiInstance() const { return m_sdaiInstance; }
	_ifc_node* getParent() const { return m_pParent; }
	virtual wchar_t* getGlobalId() const;
	vector<_ifc_node*>& children() { return m_vecChildren; }
};

// ************************************************************************************************
class _ifc_decomposition_node : public _ifc_node
{
public: // Methods

	_ifc_decomposition_node(_ifc_node* pParentNode);
	virtual ~_ifc_decomposition_node();

public: // Properties

	virtual wchar_t* getGlobalId() const override { return DECOMPOSITION_NODE; }
};

// ************************************************************************************************
class _ifc_contains_node : public _ifc_node
{

public: // Methods

	_ifc_contains_node(_ifc_node* pParentNode);
	virtual ~_ifc_contains_node();

public: // Properties

	virtual wchar_t* getGlobalId() const override { return CONTAINS_NODE; }
};

// ************************************************************************************************
class _ifc_model_structure
{

private: // Members

	_ifc_model* m_pModel;
	_ifc_node* m_pProjectNode;

public: // Methods

	_ifc_model_structure(_ifc_model* pModel);
	virtual ~_ifc_model_structure();

	void build();

#ifdef _DEBUG
	void print(int iLevel = 0, _ifc_node* pNode = nullptr);
#endif

protected: // Methods
	
	void loadProjectNode(SdaiInstance sdaiProjectInstance);
	void loadIsDecomposedBy(_ifc_node* pParentNode, SdaiInstance sdaiInstance);
	void loadIsNestedBy(_ifc_node* pParentNode, SdaiInstance sdaiInstance);
	void loadContainsElements(_ifc_node* pParentNode, SdaiInstance sdaiInstance);
	void loadInstance(_ifc_node* pParentNode, SdaiInstance sdaiInstance);
	void clean();

public: // Properties

	_ifc_model* getModel() const { return m_pModel; }
	_ifc_node* getProjectNode() const { return m_pProjectNode; }
};

