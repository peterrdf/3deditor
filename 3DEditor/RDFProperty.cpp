#include "stdafx.h"
#include "RDFProperty.h"

#ifdef _LINUX
#include<wx/string.h>
#endif // _LINUX

// ************************************************************************************************
CRDFProperty::CRDFProperty(RdfProperty iPropertyInstance)
	: m_iInstance(iPropertyInstance)
	, m_szName(nullptr)
	, m_iType(-1)
{
	ASSERT(m_iInstance != 0);

	char* szPropertyName = nullptr;
	GetNameOfPropertyW(m_iInstance, &m_szName);
}

/*virtual*/ CRDFProperty::~CRDFProperty()
{}

wstring CRDFProperty::getTypeAsString() const
{
	wstring strType = L"unknown";

	switch (getType())
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
			ASSERT(FALSE); // Not supported!
		}
		break;
	} // switch (getType())

	return strType;
}

wstring CRDFProperty::getRange() const
{
	wstring strRange = L"unknown";

	switch (getType())
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
			ASSERT(FALSE); // Not supported!
		}
		break;
	} // switch (getType())

	return strRange;
}

wstring CRDFProperty::getCardinality(OwlInstance iInstance) const
{
	ASSERT(iInstance != 0);

	int64_t iCard = 0;

	switch (getType())
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
			ASSERT(false);
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
	ASSERT(iInstance != 0);

	OwlClass iClassInstance = GetInstanceClass(iInstance);
	ASSERT(iClassInstance != 0);

	iMinCard = -1;
	iMaxCard = -1;
	GetClassPropertyAggregatedCardinalityRestriction(iClassInstance, m_iInstance, &iMinCard, &iMaxCard);
}

