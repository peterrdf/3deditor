#pragma once

#include "ifcengine.h"

#include <string>
#include <vector>
#include <map>
using namespace std;

// ************************************************************************************************
class _ap242_property
{

private: // Members	

	SdaiInstance m_sdaiInstance;
	wstring m_strEntityName;
	wstring m_strName;
	wstring m_strDescription;
	wstring m_strValue;
	wstring m_strValueType;

public: // Methods

	_ap242_property(SdaiInstance sdaiInstance);
	virtual ~_ap242_property();

protected: // Methods

	void load();

public: // Properties

	SdaiInstance getSdaiInstance() const { return m_sdaiInstance; }
	wstring getEntityName() const { return m_strEntityName; }
	wstring getName() const { return m_strName; }
	wstring getDescription() const { return m_strDescription; }
	wstring getValue() const { return m_strValue; }
	wstring getValueType() const { return m_strValueType; }
};

// ************************************************************************************************
struct _ap242_property_comparator
{
	bool operator()(const _ap242_property* lhs, const _ap242_property* rhs) const
	{
		int entityNameCmp = lhs->getEntityName().compare(rhs->getEntityName());
		if (entityNameCmp != 0) {
			return entityNameCmp < 0;
		}

		int nameCmp = lhs->getName().compare(rhs->getName());
		if (nameCmp != 0) {
			return nameCmp < 0;
		}

		int descriptionCmp = lhs->getDescription().compare(rhs->getDescription());
		if (descriptionCmp != 0) {
			return descriptionCmp < 0;
		}

		int valueCmp = lhs->getValue().compare(rhs->getValue());
		if (valueCmp != 0) {
			return valueCmp < 0;
		}

		return lhs->getValueType().compare(rhs->getValueType()) < 0;
	}
};

// ************************************************************************************************
class _ap242_property_collection
{

private: // Members

	vector<_ap242_property*> m_vecProperties;

public: // Methods

	_ap242_property_collection();
	virtual ~_ap242_property_collection();

public: // Properties

	vector<_ap242_property*>& properties() { return m_vecProperties; }
};

// ************************************************************************************************
class _ap242_property_provider
{

private: // Members

	SdaiModel m_sdaiModel;
	map<SdaiInstance, _ap242_property_collection*> m_mapPropertyCollections;

public: // Methods

	_ap242_property_provider(SdaiModel sdaiModel);
	virtual ~_ap242_property_provider();

public: // Properties

	_ap242_property_collection* getPropertyCollection(SdaiInstance sdaiInstance);

private: // Methods

	_ap242_property_collection* loadPropertyCollection(SdaiInstance sdaiInstance);

	void loadProperties(SdaiInstance sdaiInstance, _ap242_property_collection* pPropertyCollection);
};