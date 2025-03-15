#pragma once

#include <string>
#include <vector>
using namespace std;

// ************************************************************************************************
class CRDFPropertyRestriction;

// ************************************************************************************************
class _rdf_class
{

private: // Fields

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

// ************************************************************************************************
class CRDFPropertyRestriction
{

private: // Fields

	int64_t m_iPropertyInstance;
	int64_t m_iMinCard;
	int64_t m_iMaxCard;

public: // Methods

	CRDFPropertyRestriction(int64_t iPropertyInstance, int64_t iMinCard, int64_t iMaxCard);
	virtual ~CRDFPropertyRestriction();

public: // Properties

	int64_t getPropertyInstance() const;
	int64_t getMinCard() const;
	int64_t getMaxCard() const;
};