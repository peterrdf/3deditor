#pragma once

#include "RDFPropertyRestriction.h"

#include <string>
#include <vector>
using namespace std;

// ************************************************************************************************
class _rdf_class
{

private: // Members

	OwlClass m_owlClass;
	wchar_t* m_szName;
	vector<OwlClass> m_vecParentClasses;
	vector<OwlClass> m_vecAncestorClasses;
	vector<CRDFPropertyRestriction*> m_vecPropertyRestrictions;

public: // Methods

	_rdf_class(OwlClass iInstance);
	virtual ~_rdf_class();

	void AddPropertyRestriction(CRDFPropertyRestriction* pPropertyRestriction);

	static void GetAncestors(OwlClass owlClass, vector<OwlClass>& vecAncestorClasses);
	static wstring GetAncestors(OwlClass owlClass);

	OwlClass GetInstance() const { return m_owlClass; }
	const wchar_t* GetName() const { return m_szName; }
	const vector<OwlClass>& getParentClasses() const { return m_vecParentClasses; }
	const vector<OwlClass> & getAncestorClasses() const { return m_vecAncestorClasses; }
	const vector<CRDFPropertyRestriction*>& getPropertyRestrictions() const { return m_vecPropertyRestrictions; }
};

