#include "stdafx.h"
#include "RDFProperty.h"

// ************************************************************************************************
CRDFProperty::CRDFProperty(RdfProperty iInstance, int iType)
	: m_iInstance(iInstance)
	, m_szName(nullptr)
	, m_iType(iType)
{
	assert(m_iInstance != 0);

	GetNameOfPropertyW(m_iInstance, &m_szName);
}

/*virtual*/ CRDFProperty::~CRDFProperty()
{}

wstring CRDFProperty::GetTypeAsString() const
{
	wstring strType = L"unknown";

	switch (GetType())
	{
		case OBJECTPROPERTY_TYPE:
		{
			strType = L"owl:ObjectProperty";
		}
		break;

		case DATATYPEPROPERTY_TYPE_BOOLEAN:
		{
			strType = L"owl:DatatypeProperty";
		}
		break;

		case DATATYPEPROPERTY_TYPE_STRING:
		{
			strType = L"owl:DatatypeProperty";
		}
		break;

		case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
		{
			strType = L"owl:DatatypeProperty";
		}
		break;

		case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
		{
			strType = L"owl:DatatypeProperty";
		}
		break;

		case DATATYPEPROPERTY_TYPE_DOUBLE:
		{
			strType = L"owl:DatatypeProperty";
		}
		break;

		case DATATYPEPROPERTY_TYPE_INTEGER:
		{
			strType = L"owl:DatatypeProperty";
		}
		break;

		default:
		{
			assert(false); // Not supported!
		}
		break;
	} // switch (getType())

	return strType;
}

wstring CRDFProperty::GetRange() const
{
	wstring strRange = L"unknown";

	switch (GetType())
	{
		case OBJECTPROPERTY_TYPE:
		{
			strRange = L"xsd:object";
		}
		break;

		case DATATYPEPROPERTY_TYPE_BOOLEAN:
		{
			strRange = L"xsd:boolean";
		}
		break;

		case DATATYPEPROPERTY_TYPE_STRING:
		{
			strRange = L"xsd:string";
		}
		break;

		case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
		{
			strRange = L"xsd:string";
		}
		break;

		case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
		{
			strRange = L"xsd:string";
		}
		break;

		case DATATYPEPROPERTY_TYPE_DOUBLE:
		{
			strRange = L"xsd:double";
		}
		break;

		case DATATYPEPROPERTY_TYPE_INTEGER:
		{
			strRange = L"xsd:integer";
		}
		break;

		default:
		{
			assert(false); // Not supported!
		}
		break;
	} // switch (getType())

	return strRange;
}

wstring CRDFProperty::GetCardinality(OwlInstance iInstance) const
{
	assert(iInstance != 0);

	int64_t iCard = 0;

	switch (GetType())
	{
		case OBJECTPROPERTY_TYPE:
		{
			int64_t* piInstances = nullptr;
			GetObjectProperty(iInstance, GetInstance(), &piInstances, &iCard);
		}
		break;

		case DATATYPEPROPERTY_TYPE_BOOLEAN:
		{
			bool* pbValue = nullptr;
			GetDatatypeProperty(iInstance, GetInstance(), (void**)&pbValue, &iCard);
		}
		break;

		case DATATYPEPROPERTY_TYPE_STRING:
		{
			wchar_t** szValue = nullptr;
			SetCharacterSerialization(GetModel(iInstance), 0, 0, false);
			GetDatatypeProperty(iInstance, GetInstance(), (void**)&szValue, &iCard);
			SetCharacterSerialization(GetModel(iInstance), 0, 0, true);
		}
		break;

		case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
		{
			char** szValue = nullptr;
			GetDatatypeProperty(iInstance, GetInstance(), (void**)&szValue, &iCard);
		}
		break;

		case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
		{
			wchar_t** szValue = nullptr;
			GetDatatypeProperty(iInstance, GetInstance(), (void**)&szValue, &iCard);
		}
		break;

		case DATATYPEPROPERTY_TYPE_DOUBLE:
		{
			double* pdValue = nullptr;
			GetDatatypeProperty(iInstance, GetInstance(), (void **)&pdValue, &iCard);
		}
		break;

		case DATATYPEPROPERTY_TYPE_INTEGER:
		{
			int64_t* piValue = nullptr;
			GetDatatypeProperty(iInstance, GetInstance(), (void **)&piValue, &iCard);
		}
		break;

		default:
		{
			assert(false);
		}
		break;
	} // switch (getType())

	int64_t	iMinCard = 0;
	int64_t iMaxCard = 0;
	GetRestrictions(iInstance, iMinCard, iMaxCard);

	wchar_t szBuffer[256];

	if ((iMinCard == -1) && (iMaxCard == -1))
	{
		swprintf(szBuffer, 256, L"%lld of [0 - infinity>", iCard);
	}
	else
	{
		if (iMaxCard == -1)
		{
			swprintf(szBuffer, 256, L"%lld of [%lld - infinity>", iCard, iMinCard);
		}
		else
		{
			swprintf(szBuffer, 256, L"%lld of [%lld - %lld]", iCard, iMinCard, iMaxCard);
		}
	}

	return szBuffer;
}

void CRDFProperty::GetRestrictions(OwlInstance iInstance, int64_t& iMinCard, int64_t& iMaxCard) const
{
	assert(iInstance != 0);

	OwlClass iClassInstance = GetInstanceClass(iInstance);
	assert(iClassInstance != 0);

	iMinCard = -1;
	iMaxCard = -1;
	GetClassPropertyAggregatedCardinalityRestriction(iClassInstance, m_iInstance, &iMinCard, &iMaxCard);
}

// ************************************************************************************************
CDoubleRDFProperty::CDoubleRDFProperty(RdfProperty iInstance)
	: CRDFProperty(iInstance, DATATYPEPROPERTY_TYPE_DOUBLE)
{}

/*virtual*/ CDoubleRDFProperty::~CDoubleRDFProperty()
{}

// ************************************************************************************************
CIntRDFProperty::CIntRDFProperty(RdfProperty iInstance)
	: CRDFProperty(iInstance, DATATYPEPROPERTY_TYPE_INTEGER)
{}

CIntRDFProperty::~CIntRDFProperty()
{}

// ************************************************************************************************
CBoolRDFProperty::CBoolRDFProperty(RdfProperty iInstance)
	: CRDFProperty(iInstance, DATATYPEPROPERTY_TYPE_BOOLEAN)
{}

/*virtual*/ CBoolRDFProperty::~CBoolRDFProperty()
{}

// ************************************************************************************************
CStringRDFProperty::CStringRDFProperty(RdfProperty iInstance)
	: CRDFProperty(iInstance, DATATYPEPROPERTY_TYPE_STRING)
{}

CStringRDFProperty::~CStringRDFProperty()
{}

// ************************************************************************************************
CCharArrayRDFProperty::CCharArrayRDFProperty(RdfProperty iInstance)
	: CRDFProperty(iInstance, DATATYPEPROPERTY_TYPE_CHAR_ARRAY)
{}

// ************************************************************************************************
CCharArrayRDFProperty::~CCharArrayRDFProperty()
{}

// ************************************************************************************************
CWCharArrayRDFProperty::CWCharArrayRDFProperty(RdfProperty iInstance)
	: CRDFProperty(iInstance, DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY)
{}

CWCharArrayRDFProperty::~CWCharArrayRDFProperty()
{}

// ************************************************************************************************
CObjectRDFProperty::CObjectRDFProperty(RdfProperty iInstance)
	: CRDFProperty(iInstance, OBJECTPROPERTY_TYPE)
	, m_vecRestrictions()
{
	int64_t	iRestrictionsClassInstance = GetRangeRestrictionsByIterator(GetInstance(), 0);
	while (iRestrictionsClassInstance != 0)
	{
		m_vecRestrictions.push_back(iRestrictionsClassInstance);

		iRestrictionsClassInstance = GetRangeRestrictionsByIterator(GetInstance(), iRestrictionsClassInstance);
	}
}

CObjectRDFProperty::~CObjectRDFProperty()
{}

// ************************************************************************************************
CUndefinedRDFProperty::CUndefinedRDFProperty(RdfProperty iInstance)
	: CRDFProperty(iInstance, -1)
{}

CUndefinedRDFProperty::~CUndefinedRDFProperty()
{}
