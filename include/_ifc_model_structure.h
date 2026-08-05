#pragma once

#include "_ifc_instance.h"
#include "_ifc_model.h"

#include <string>
#include <vector>
#include <map>

// ************************************************************************************************
#define MODEL_NODE	L"model"
#define DECOMPOSITION_NODE	L"decomposition"
#define CONTAINS_NODE		L"contains"

// ************************************************************************************************
class _ifc_node {

private: // Members

	_ifc_instance* m_pInstance;
	_ifc_node* m_pParent;
	std::vector<_ifc_node*> m_vecChildren;

public: // Methods

	_ifc_node(_ifc_instance* pIfcInstance, _ifc_node* pParentNode);
	virtual ~_ifc_node();

	bool hasChild(SdaiInstance sdaiInstance);

public: // Properties

	_ifc_instance* getInstance() const { return m_pInstance; }
	SdaiInstance getSdaiInstance() const { return m_pInstance != nullptr ? m_pInstance->getSdaiInstance() : 0; }
	_ifc_node* getParent() const { return m_pParent; }
	virtual const wchar_t* getGlobalId() const;
	std::vector<_ifc_node*>& children() { return m_vecChildren; }
};

// ************************************************************************************************
class _ifc_model_node : public _ifc_node {

private: // Members

	_ifc_model* m_pModel;

public: // Methods

	_ifc_model_node(_ifc_model* pModel);
	virtual ~_ifc_model_node();

public: // Properties

	virtual const wchar_t* getGlobalId() const override { return MODEL_NODE; }
};

// ************************************************************************************************
class _ifc_decomposition_node : public _ifc_node {

public: // Methods

	_ifc_decomposition_node(_ifc_node* pParentNode);
	virtual ~_ifc_decomposition_node();

public: // Properties

	virtual const wchar_t* getGlobalId() const override { return DECOMPOSITION_NODE; }
};

// ************************************************************************************************
class _ifc_contains_node : public _ifc_node {

public: // Methods

	_ifc_contains_node(_ifc_node* pParentNode);
	virtual ~_ifc_contains_node();

public: // Properties

	virtual const wchar_t* getGlobalId() const override { return CONTAINS_NODE; }
};

// ************************************************************************************************
class _ifc_entity_node : public _ifc_node {

private: // Members

	wstring m_strEntityName;

public: // Methods

	_ifc_entity_node(const wstring& strEntityName, _ifc_node* pParentNode);
	virtual ~_ifc_entity_node();

public: // Properties

	virtual const wchar_t* getGlobalId() const override { return m_strEntityName.c_str(); }
};

// ************************************************************************************************
class _ifc_model_structure {

private: // Members

	_ifc_model* m_pModel;
	_ifc_node* m_pModelNode;
	_ifc_node* m_pProjectNode;
	_ifc_node* m_pGroupsNode;
	_ifc_node* m_pUnreferencedNode;
	std::map<SdaiInstance, _ifc_node*> m_mapInstance2Node;

public: // Methods

	_ifc_model_structure(_ifc_model* pModel);
	virtual ~_ifc_model_structure();

	void build();

#ifdef _DEBUG
	void print(int iLevel = 0, _ifc_node* pNode = nullptr);
#endif

	void getInstancePath(SdaiInstance sdaiInstance, std::vector<_ifc_node*>& vecPath);
	void getInstanceChildren(SdaiInstance sdaiInstance, std::vector<SdaiInstance>& vecChildren, bool bRecursive);
	bool hasChild(_ifc_node* pNode, SdaiInstance sdaiInstance);

protected: // Methods

	void loadProjectNode(SdaiInstance sdaiProjectInstance);
	void loadGroupsNode();
	void loadUnreferencedNode();
	void loadIsDecomposedBy(_ifc_node* pParentNode, SdaiInstance sdaiInstance);
	void loadIsNestedBy(_ifc_node* pParentNode, SdaiInstance sdaiInstance);
	void loadContainsElements(_ifc_node* pParentNode, SdaiInstance sdaiInstance);
	void loadBoundedBy(_ifc_node* pParentNode, SdaiInstance sdaiInstance);
	void loadHasOpenings(_ifc_node* pParentNode, SdaiInstance sdaiInstance);
	void loadInstance(_ifc_node* pParentNode, SdaiInstance sdaiInstance);

	void clean();

public: // Properties

	_ifc_model* getModel() const { return m_pModel; }
	_ifc_node* getModelNode() const { return m_pModelNode; }
	_ifc_node* getProjectNode() const { return m_pProjectNode; }
	_ifc_node* getGroupsNode() const { return m_pGroupsNode; }
	_ifc_node* getUnreferencedNode() const { return m_pUnreferencedNode; }
	const std::map<SdaiInstance, _ifc_node*>& getInstance2Node() const { return m_mapInstance2Node; }
};