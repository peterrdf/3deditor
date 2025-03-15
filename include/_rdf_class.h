#pragma once

#include <string>
#include <vector>
using namespace std;

// ************************************************************************************************
class _rdf_property_restriction;

// ************************************************************************************************
class _rdf_class
{

private: // Fields

	OwlClass m_owlClass;
	wchar_t* m_szName;
	vector<OwlClass> m_vecParentClasses;
	vector<OwlClass> m_vecAncestorClasses;
	vector<_rdf_property_restriction*> m_vecPropertyRestrictions;

public: // Methods

	_rdf_class(OwlClass owlClass);
	virtual ~_rdf_class();

	void addPropertyRestriction(_rdf_property_restriction* pPropertyRestriction);

	static void GetAncestors(OwlClass owlClass, vector<OwlClass>& vecAncestorClasses);
	static wstring GetAncestors(OwlClass owlClass);

	OwlClass getOwlClass() const { return m_owlClass; }
	const wchar_t* getName() const { return m_szName; }
	const vector<OwlClass>& getParentClasses() const { return m_vecParentClasses; }
	const vector<OwlClass>& getAncestorClasses() const { return m_vecAncestorClasses; }
	const vector<_rdf_property_restriction*>& getPropertyRestrictions() const { return m_vecPropertyRestrictions; }
};

// ************************************************************************************************
class _rdf_property_restriction
{

private: // Fields

	RdfProperty m_rdfProperty;
	int64_t m_iMinCard;
	int64_t m_iMaxCard;

public: // Methods

	_rdf_property_restriction(RdfProperty rdfProperty, int64_t iMinCard, int64_t iMaxCard);
	virtual ~_rdf_property_restriction();

public: // Properties

	RdfProperty getRdfProperty() const { return m_rdfProperty; }
	int64_t getMinCard() const { return m_iMinCard; }
	int64_t getMaxCard() const { return m_iMaxCard; }
};