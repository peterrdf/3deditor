#pragma once

#include "engdef.h"

#include <vector>
#include <string>
#include <set>
#include <map>
using namespace std;

// ************************************************************************************************
class _entity
{

private: // Members
	
	SdaiEntity m_sdaiEntity;
	wchar_t* m_szName;
	_entity* m_pParent;

	vector<wstring> m_vecAttributes;
	vector<_entity*> m_vecSubTypes;
	vector<SdaiInstance> m_vecInstances;

public: // Methods
	
	_entity(SdaiEntity sdaiEntity);
	virtual ~_entity();

	SdaiEntity getEntity() const { return m_sdaiEntity; }
	const wchar_t* getName() const { return m_szName; }
	static wchar_t* getName(SdaiEntity sdaiEntity);
	_entity* getParent() const { return m_pParent; }
	void setParent(_entity* pParent) { m_pParent = pParent; }
	bool hasParent() const { return m_pParent != nullptr; }
	const vector<wstring>& getAttributes() const { return m_vecAttributes; }
	bool isAttributeInherited(const wstring& strAttribute) const;
	void addSubType(_entity* pEntity) { m_vecSubTypes.push_back(pEntity); }
	const vector<_entity*>& getSubTypes() const { return m_vecSubTypes; }
	const vector<SdaiInstance>& getInstances() const { return m_vecInstances; }

	void postProcessing();
};

// ************************************************************************************************
struct _sort_entities
{
	bool operator()(const _entity* pLeft, const _entity* pRight) const
	{
		return wcscmp(pLeft->getName(), pRight->getName()) < 0;
	}
};

// ************************************************************************************************
class _entity_provider
{

private: // Members

	SdaiModel m_sdaiModel;
	map<SdaiEntity, _entity*> m_mapEntities;

public: // Methods

	_entity_provider(SdaiModel sdaiModel);
	virtual ~_entity_provider();

	const map<SdaiEntity, _entity*>& getEntities() const { return m_mapEntities; }

protected: // Methods

	void load();
};