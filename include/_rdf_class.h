#pragma once

#include "engine.h"

#include <string>
#include <vector>

// ************************************************************************************************
class _rdf_property_restriction;

// ************************************************************************************************
class _rdf_class
{

private: // Fields

	OwlClass m_owlClass;
	std::wstring m_strName;
	std::vector<OwlClass> m_vecParentClasses;
	std::vector<OwlClass> m_vecAncestorClasses;
	std::vector<_rdf_property_restriction*> m_vecPropertyRestrictions;

public: // Methods

	_rdf_class(OwlClass owlClass);
	virtual ~_rdf_class();

	void addPropertyRestriction(_rdf_property_restriction* pPropertyRestriction);

	static void getAncestors(OwlClass owlClass, std::vector<OwlClass>& vecAncestorClasses);
	static std::wstring getAncestors(OwlClass owlClass);

	OwlClass getOwlClass() const { return m_owlClass; }
	const wchar_t* getName() const { return m_strName.c_str(); }
	const std::vector<OwlClass>& getParentClasses() const { return m_vecParentClasses; }
	const std::vector<OwlClass>& getAncestorClasses() const { return m_vecAncestorClasses; }
	const std::vector<_rdf_property_restriction*>& getPropertyRestrictions() const { return m_vecPropertyRestrictions; }
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