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
	wstring getTypeName() { return getTypeName(getType()); }
	static wstring getTypeName(RdfPropertyType rdfPropertyType);
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