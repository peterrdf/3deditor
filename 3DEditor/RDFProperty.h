#pragma once

#include <string>

using namespace std;

// ------------------------------------------------------------------------------------------------
#define			TYPE_OBJECTTYPE						1
#define			TYPE_BOOL_DATATYPE					2
#define			TYPE_CHAR_DATATYPE					3
#define			TYPE_DOUBLE_DATATYPE				5
#define			TYPE_INT_DATATYPE					4

// ------------------------------------------------------------------------------------------------
class CRDFProperty
{

private: // Members

	// --------------------------------------------------------------------------------------------
	// Instance
	int64_t m_iInstance;

	// --------------------------------------------------------------------------------------------
	// Name
	wstring m_strName;

protected: // Members

	// --------------------------------------------------------------------------------------------
	// Type
	int64_t m_iType;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CRDFProperty(int64_t iInstance);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CRDFProperty();

	// --------------------------------------------------------------------------------------------
	// Getter
	int64_t GetInstance() const;

	// --------------------------------------------------------------------------------------------
	// Getter
	const wchar_t * GetName() const;

	// --------------------------------------------------------------------------------------------
	// Getter
	int64_t getType() const;

	// --------------------------------------------------------------------------------------------
	// Getter
	wstring getTypeAsString() const;

	// --------------------------------------------------------------------------------------------
	// Getter; rdfs:range
	wstring getRange() const;

	// --------------------------------------------------------------------------------------------
	// Getter; owl:cardinality
	wstring getCardinality(int64_t iInstance) const;

	// --------------------------------------------------------------------------------------------
	// Restrictions
	void GetRestrictions(int64_t iInstance, int64_t & iMinCard, int64_t & iMaxCard) const;

	void GetClassPropertyCardinalityRestrictionNested_(int64_t iRDFClass, int64_t iRDFProperty, int64_t* pMinCard, int64_t* pMaxCard) const;
};

