#pragma once

#include "RDFPropertyRestriction.h"

#include <string>
#include <vector>

using namespace std;

// ------------------------------------------------------------------------------------------------
// Class
class CRDFClass
{

private: // Members

	// --------------------------------------------------------------------------------------------
	// Instance
	int64_t m_iInstance;

	// --------------------------------------------------------------------------------------------
	// Name
	wstring m_strName;

	// --------------------------------------------------------------------------------------------
	// Parents
	vector<int64_t> m_vecParentClasses;

	// --------------------------------------------------------------------------------------------
	// Parents
	vector<int64_t> m_vecAncestorClasses;

	// --------------------------------------------------------------------------------------------
	// Parents
	vector<CRDFPropertyRestriction *> m_vecPropertyRestrictions;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CRDFClass(int64_t iInstance);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CRDFClass();

	// --------------------------------------------------------------------------------------------
	// Getter
	int64_t getInstance() const;

	// --------------------------------------------------------------------------------------------
	// Getter
	const wchar_t * getName() const;

	// --------------------------------------------------------------------------------------------
	// Getter
	const vector<int64_t> & getParentClasses();

	// --------------------------------------------------------------------------------------------
	// Getter
	const vector<int64_t> & getAncestorClasses();

	// --------------------------------------------------------------------------------------------
	// Adds an RDF Property
	void AddPropertyRestriction(CRDFPropertyRestriction * pRDFPropertyRestriction);

	// --------------------------------------------------------------------------------------------
	// Getter
	const vector<CRDFPropertyRestriction *> & getPropertyRestrictions();

public: // Methods

	// --------------------------------------------------------------------------------------------
	// Helper
	static void GetAncestors(int64_t iClassInstance, vector<int64_t> & vecAncestorClasses);
};

