#pragma once

#include "_ifc_unit.h"

#include <string>
#include <vector>
#include <map>
using namespace std;

// ************************************************************************************************
class _ifc_property
{

private: // Members	

	SdaiInstance m_sdaiInstance;
	wstring m_strEntityName;
	wstring m_strName;
	wstring m_strValue;
	wstring m_strIfcValueType;
	wstring m_strValueType;

public: // Methods

	_ifc_property(SdaiInstance sdaiInstance, const wstring& strName, const wstring& strValue);
	virtual ~_ifc_property();

	static bool hasProperties(SdaiModel sdaiModel, SdaiInstance sdaiInstance);
	static wstring getPropertySingleValue(SdaiInstance sdaiPropertySingleValueInstance);
	static pair<wstring, wstring> getValueTypes(SdaiInstance sdaiInstance);

public: // Properties

	SdaiInstance getSdaiInstance() const { return m_sdaiInstance; }
	wstring getEntityName() const { return m_strEntityName; }
	wstring getName() const { return m_strName; }
	wstring getValue() const { return m_strValue; }
	wstring getIfcValueType() const { return m_strIfcValueType; }
	wstring getValueType() const { return m_strValueType; }
};

// ************************************************************************************************
struct _ifc_property_comparator
{
	bool operator()(const _ifc_property* lhs, const _ifc_property* rhs) const
	{
		int entityNameCmp = lhs->getEntityName().compare(rhs->getEntityName());
		if (entityNameCmp != 0) {
			return entityNameCmp < 0;
		}

		int nameCmp = lhs->getName().compare(rhs->getName());
		if (nameCmp != 0) {
			return nameCmp < 0;
		}

		int valueCmp = lhs->getValue().compare(rhs->getValue());
		if (valueCmp != 0) {
			return valueCmp < 0;
		}

		int ifcValueTypeCmp = lhs->getIfcValueType().compare(rhs->getIfcValueType());
		if (ifcValueTypeCmp != 0) {
			return ifcValueTypeCmp < 0;
		}

		return lhs->getValueType().compare(rhs->getValueType()) < 0;
	}
};

// ************************************************************************************************
class _ifc_property_set
{

private: // Members	

	SdaiInstance m_sdaiInstance;
	wstring m_strName;
	vector<_ifc_property*> m_vecProperties;

public: // Methods

	_ifc_property_set(SdaiInstance sdaiInstance, const wstring& strName);
	virtual ~_ifc_property_set();

public: // Properties

	SdaiInstance getSdaiInstance() const { return m_sdaiInstance; }
	wstring getName() const { return m_strName; }
	vector<_ifc_property*>& properties() { return m_vecProperties; }
};

// ************************************************************************************************
class _ifc_property_set_collection
{

private: // Members

	vector<_ifc_property_set*> m_vecPropertySets;

public: // Methods

	_ifc_property_set_collection();
	virtual ~_ifc_property_set_collection();

public: // Properties

	vector<_ifc_property_set*>& propertySets() { return m_vecPropertySets; }
};

// ************************************************************************************************
class _ifc_property_provider
{

private: // Members

	SdaiModel m_sdaiModel;
	_ifc_unit_provider* m_pUnitProvider;
	map<SdaiInstance, _ifc_property_set_collection*> m_mapPropertyCollections;

public: // Methods

	_ifc_property_provider(SdaiModel sdaiModel, _ifc_unit_provider* pUnitProvider);
	virtual ~_ifc_property_provider();

public: // Properties

	_ifc_property_set_collection* getPropertySetCollection(SdaiInstance sdaiInstance);

private: // Methods

	_ifc_property_set_collection* loadPropertyCollection(SdaiInstance sdaiInstance);

	void loadProperties(SdaiInstance sdaiInstance, _ifc_property_set_collection* pPropertySetCollection);
	void loadRelDefinesByProperties(SdaiInstance sdaiIsDefinedByPropertiesInstance, _ifc_property_set_collection* pPropertySetCollection);
	void loadPropertySet(SdaiInstance sdaiPropertySetInstance, _ifc_property_set_collection* pPropertySetCollection);
	void loadRelDefinesByType(SdaiInstance sdaiRelDefinesByTypeInstance, _ifc_property_set_collection* pPropertySetCollection);
	void loadQuantites(SdaiInstance sdaiElementQuantityInstance, _ifc_property_set_collection* pPropertySetCollection);
	void loadIFCQuantityLength(SdaiInstance sdaiQuantityInstance, _ifc_property_set* pPropertySet);
	void loadIFCQuantityArea(SdaiInstance sdaiQuantityInstance, _ifc_property_set* pPropertySet);
	void loadIFCQuantityVolume(SdaiInstance sdaiQuantityInstance, _ifc_property_set* pPropertySet);
	void loadIFCQuantityCount(SdaiInstance sdaiQuantityInstance, _ifc_property_set* pPropertySet);
	void loadIFCQuantityWeight(SdaiInstance sdaiQuantityInstance, _ifc_property_set* pPropertySet);
	void loadIFCQuantityTime(SdaiInstance sdaiQuantityInstance, _ifc_property_set* pPropertySet);

	wstring getName(SdaiInstance sdaiInstance) const;
};