#pragma once

#include <string>
#include <vector>
using namespace std;

// ************************************************************************************************
class CRDFProperty
{

private: // Members

	RdfProperty m_iInstance;
	wchar_t* m_szName;

protected: // Members

	int m_iType;

public: // Methods

	CRDFProperty(RdfProperty iInstance);
	virtual ~CRDFProperty();	

	wstring GetTypeAsString() const;
	wstring GetRange() const;
	wstring GetCardinality(OwlInstance iInstance) const;
	void GetRestrictions(OwlInstance iInstance, int64_t& iMinCard, int64_t& iMaxCard) const;

	RdfProperty GetInstance() const { return m_iInstance; }
	const wchar_t* GetName() const { return m_szName; }
	int GetType() const { return m_iType; }
};

// ************************************************************************************************
class CDoubleRDFProperty : public CRDFProperty
{

public: // Methods

	CDoubleRDFProperty(RdfProperty iInstance);
	virtual ~CDoubleRDFProperty();
};

// ************************************************************************************************
class CIntRDFProperty : public CRDFProperty
{

public: // Methods

	CIntRDFProperty(RdfProperty iInstance);
	virtual ~CIntRDFProperty();
};

// ************************************************************************************************
class CBoolRDFProperty : public CRDFProperty
{

public: // Methods

	CBoolRDFProperty(RdfProperty iInstance);
	virtual ~CBoolRDFProperty();
};

// ************************************************************************************************
class CStringRDFProperty : public CRDFProperty
{

public: // Methods

	CStringRDFProperty(RdfProperty iInstance);
	virtual ~CStringRDFProperty();
};

// ************************************************************************************************
class CCharArrayRDFProperty : public CRDFProperty
{

public: // Methods

	CCharArrayRDFProperty(RdfProperty iInstance);
	virtual ~CCharArrayRDFProperty();
};

// ************************************************************************************************
class CWCharArrayRDFProperty : public CRDFProperty
{

public: // Methods

	CWCharArrayRDFProperty(RdfProperty iInstance);
	virtual ~CWCharArrayRDFProperty();
};

// ************************************************************************************************
class CObjectRDFProperty : public CRDFProperty
{

private: // Members

	vector<int64_t> m_vecRestrictions;

public: // Methods

	CObjectRDFProperty(RdfProperty iInstance);
	virtual ~CObjectRDFProperty();

	const vector<int64_t>& GetRestrictions() const { return m_vecRestrictions; }
};

// ************************************************************************************************
class CUndefinedRDFProperty : public CRDFProperty
{

public: // Methods

	CUndefinedRDFProperty(RdfProperty iInstance);
	virtual ~CUndefinedRDFProperty();
};

