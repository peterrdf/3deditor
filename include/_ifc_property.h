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

	wstring m_strName;
	wstring m_strValue;

public: // Methods

	_ifc_property(const wstring& strName, const wstring& strValue);
	virtual ~_ifc_property();

	static bool hasProperties(SdaiModel sdaiModel, SdaiInstance sdaiInstance);
	static wstring getPropertySingleValue(SdaiInstance sdaiPropertySingleValueInstance);

public: // Properties

	wstring getName() const { return m_strName; }
	wstring getValue() const { return m_strValue; }
};

// ************************************************************************************************
class _ifc_property_set
{

private: // Members	

	wstring m_strName;
	vector<_ifc_property*> m_vecProperties;

public: // Methods

	_ifc_property_set(const wstring& strName);
	virtual ~_ifc_property_set();

public: // Properties

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