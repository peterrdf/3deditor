#pragma once

#include <string>
using namespace std;

// ************************************************************************************************
class CRDFProperty
{

private: // Members

	RdfProperty m_iInstance;
	wchar_t* m_szName;

protected: // Members

	int64_t m_iType;

public: // Methods

	CRDFProperty(RdfProperty iPropertyInstance);
	virtual ~CRDFProperty();	

	wstring getTypeAsString() const;
	wstring getRange() const;
	wstring getCardinality(OwlInstance iInstance) const;
	void GetRestrictions(OwlInstance iInstance, int64_t& iMinCard, int64_t& iMaxCard) const;

	RdfProperty GetInstance() const { return m_iInstance; }
	const wchar_t* GetName() const { return m_szName; }
	int64_t getType() const { return m_iType; }
};

