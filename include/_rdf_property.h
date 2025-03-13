#pragma once

#include <string>
#include <vector>
#include <map>
using namespace std;

// ************************************************************************************************
class _rdf_property 
{

private: // Members	

	RdfProperty m_rdfProperty;

public: // Methods

	_rdf_property(RdfProperty rdfProperty);
	virtual ~_rdf_property();

	wstring getRange(vector<int64_t>& vecRestrictionClasses) const { return getRange(m_rdfProperty, vecRestrictionClasses); }
	static wstring getRange(RdfProperty rdfProperty, vector<OwlClass>& vecRestrictionClasses);
	wstring getCardinality(OwlInstance owlInstance) const { return getCardinality(owlInstance, getRdfProperty()); }
	static wstring getCardinality(OwlInstance owlInstance, RdfProperty rdfProperty);

public: // Properties

	RdfProperty getRdfProperty() const { return m_rdfProperty; }
	RdfPropertyType getType() const { return getType(m_rdfProperty); }
	static RdfPropertyType getType(RdfProperty rdfProperty) { return GetPropertyType(rdfProperty); }
	wstring getTypeAsString() { return getTypeAsString(getType()); }
	static wstring getTypeAsString(RdfPropertyType rdfPropertyType);
	wchar_t* getName() const;
};

// ************************************************************************************************
class _rdf_property_collection
{

private: // Members

	vector<_rdf_property*> m_vecProperties;

public: // Methods

	_rdf_property_collection();
	virtual ~_rdf_property_collection();

public: // Properties

	vector<_rdf_property*>& properties() { return m_vecProperties; }
};

// ************************************************************************************************
class _rdf_property_provider
{

private: // Members

	map<OwlInstance, _rdf_property_collection*> m_mapPropertyCollections;

public: // Methods

	_rdf_property_provider();
	virtual ~_rdf_property_provider();

	_rdf_property_collection* getPropertyCollection(OwlInstance owlInstance);

private: // Methods

	_rdf_property_collection* loadPropertyCollection(OwlInstance owlInstance);
};

// ************************************************************************************************
class _rdf_property_t : public _rdf_property
{

private: // Members

	RdfProperty m_iInstance;
	wchar_t* m_szName;
	int m_iType;

public: // Methods

	_rdf_property_t(RdfProperty iInstance, int iType);
	virtual ~_rdf_property_t();

	wstring GetRange() const;
	wstring GetCardinality(OwlInstance iInstance) const;
	void GetRestrictions(OwlInstance iInstance, int64_t& iMinCard, int64_t& iMaxCard) const;

	RdfProperty GetInstance() const { return m_iInstance; }
	const wchar_t* GetName() const { return m_szName; }
	int GetType() const { return m_iType; }
};

// ************************************************************************************************
class _double_rdf_property : public _rdf_property_t
{

public: // Methods

	_double_rdf_property(RdfProperty iInstance);
	virtual ~_double_rdf_property();
};

// ************************************************************************************************
class _integer_rdf_property : public _rdf_property_t
{

public: // Methods

	_integer_rdf_property(RdfProperty iInstance);
	virtual ~_integer_rdf_property();
};

// ************************************************************************************************
class _bool_rdf_property : public _rdf_property_t
{

public: // Methods

	_bool_rdf_property(RdfProperty iInstance);
	virtual ~_bool_rdf_property();
};

// ************************************************************************************************
class _string_rdf_property : public _rdf_property_t
{

public: // Methods

	_string_rdf_property(RdfProperty iInstance);
	virtual ~_string_rdf_property();
};

// ************************************************************************************************
class CCharArrayRDFProperty : public _rdf_property_t
{

public: // Methods

	CCharArrayRDFProperty(RdfProperty iInstance);
	virtual ~CCharArrayRDFProperty();
};

// ************************************************************************************************
class CWCharArrayRDFProperty : public _rdf_property_t
{

public: // Methods

	CWCharArrayRDFProperty(RdfProperty iInstance);
	virtual ~CWCharArrayRDFProperty();
};

// ************************************************************************************************
class CObjectRDFProperty : public _rdf_property_t
{

private: // Members

	vector<int64_t> m_vecRestrictions;

public: // Methods

	CObjectRDFProperty(RdfProperty iInstance);
	virtual ~CObjectRDFProperty();

	const vector<int64_t>& GetRestrictions() const { return m_vecRestrictions; }
};

// ************************************************************************************************
class CUndefinedRDFProperty : public _rdf_property_t
{

public: // Methods

	CUndefinedRDFProperty(RdfProperty iInstance);
	virtual ~CUndefinedRDFProperty();
};
