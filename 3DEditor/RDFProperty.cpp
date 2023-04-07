#include "stdafx.h"
#include "RDFProperty.h"

#ifdef _LINUX
#include<wx/string.h>
#endif // _LINUX

// ------------------------------------------------------------------------------------------------
CRDFProperty::CRDFProperty(int64_t iInstance)
	: m_iInstance(iInstance)
	, m_strName(L"")
	, m_iType(-1)
{
	ASSERT(m_iInstance != 0);

	/*
	* Name
	*/
	char * szPropertyName = nullptr;
	GetNameOfProperty(m_iInstance, &szPropertyName);

#ifndef _LINUX
//    LOG_DEBUG("*** PROPERTY " << szPropertyName);

	m_strName = CA2W(szPropertyName);

	/*
	* Parents
	*/
	int64_t iParentClassInstance = GetClassParentsByIterator(GetInstanceClass(m_iInstance), 0);
	while (iParentClassInstance != 0)
	{
		char * szParentClassName = nullptr;
		GetNameOfClass(iParentClassInstance, &szParentClassName);

//		LOG_DEBUG("*** PARENT CLASS " << szParentClassName);

		iParentClassInstance = GetClassParentsByIterator(GetInstanceClass(m_iInstance), iParentClassInstance);
	}
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
CRDFProperty::~CRDFProperty()
{
}

// ------------------------------------------------------------------------------------------------
// Getter
int64_t CRDFProperty::GetInstance() const
{
	return m_iInstance;
}

// ------------------------------------------------------------------------------------------------
// Getter
const wchar_t * CRDFProperty::GetName() const
{
	return m_strName.c_str();
}

// ------------------------------------------------------------------------------------------------
int64_t CRDFProperty::getType() const
{
	return m_iType;
}

// ------------------------------------------------------------------------------------------------
wstring CRDFProperty::getTypeAsString() const
{
	wstring strType = L"unknown";

	switch (getType())
	{
	case TYPE_OBJECTTYPE:
	{
		strType = L"owl:ObjectProperty";
	}
	break;

	case TYPE_BOOL_DATATYPE:
	{
		strType = L"owl:DatatypeProperty";
	}
	break;

	case TYPE_CHAR_DATATYPE:
	{
		strType = L"owl:DatatypeProperty";
	}
	break;

	case TYPE_DOUBLE_DATATYPE:
	{
		strType = L"owl:DatatypeProperty";
	}
	break;

	case TYPE_INT_DATATYPE:
	{
		strType = L"owl:DatatypeProperty";
	}
	break;

	default:
	{
		ASSERT(false);
	}
	break;
	} // switch (getType())

	return strType;
}

// ------------------------------------------------------------------------------------------------
wstring CRDFProperty::getRange() const
{
	wstring strRange = L"unknown";

	switch (getType())
	{
	case TYPE_OBJECTTYPE:
	{
		strRange = L"xsd:object";
	}
	break;

	case TYPE_BOOL_DATATYPE:
	{
		strRange = L"xsd:boolean";
	}
	break;

	case TYPE_CHAR_DATATYPE:
	{
		strRange = L"xsd:string";
	}
	break;

	case TYPE_DOUBLE_DATATYPE:
	{
		strRange = L"xsd:double";
	}
	break;

	case TYPE_INT_DATATYPE:
	{
		strRange = L"xsd:integer";
	}
	break;

	default:
	{
		ASSERT(false);
	}
	break;
	} // switch (getType())

	return strRange;
}

// ------------------------------------------------------------------------------------------------
wstring CRDFProperty::getCardinality(int64_t iInstance) const
{
	ASSERT(iInstance != 0);

	wchar_t szBuffer[100];

	int64_t iCard = 0;

	switch (getType())
	{
	case TYPE_OBJECTTYPE:
	{
		int64_t * piInstances = nullptr;
		GetObjectProperty(iInstance, GetInstance(), &piInstances, &iCard);
	}
	break;

	case TYPE_BOOL_DATATYPE:
	{
		bool* pbValue = nullptr;
		GetDatatypeProperty(iInstance, GetInstance(), (void**)&pbValue, &iCard);
	}
	break;

	case TYPE_CHAR_DATATYPE:
	{
		char ** szValue = nullptr;
		GetDatatypeProperty(iInstance, GetInstance(), (void **)&szValue, &iCard);
	}
	break;

	case TYPE_DOUBLE_DATATYPE:
	{
		double * pdValue = nullptr;
		GetDatatypeProperty(iInstance, GetInstance(), (void **)&pdValue, &iCard);
	}
	break;

	case TYPE_INT_DATATYPE:
	{
		int64_t * piValue = nullptr;
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

	if ((iMinCard == -1) && (iMaxCard == -1))
	{
		swprintf(szBuffer, 100, L"%lld of [0 - infinity>", iCard);
	}
	else
	{
		if (iMaxCard == -1)
		{
			swprintf(szBuffer, 100, L"%lld of [%lld - infinity>", iCard, iMinCard);
		}
		else
		{
			swprintf(szBuffer, 100, L"%lld of [%lld - %lld]", iCard, iMinCard, iMaxCard);
		}
	}

	return szBuffer;
}

void CRDFProperty::GetClassPropertyCardinalityRestrictionNested_(int64_t iRDFClass, int64_t iRDFProperty, int64_t * pMinCard, int64_t * pMaxCard) const
{
	GetClassPropertyAggregatedCardinalityRestriction(iRDFClass, iRDFProperty, pMinCard, pMaxCard);
#if 0
	int64_t	minCard = 0, maxCard = -1;
	GetClassPropertyCardinalityRestriction(iRDFClass, iRDFProperty, &minCard, &maxCard);
	if ((*pMinCard) < minCard) {
		(*pMinCard) = minCard;
	}
	if (maxCard > 0 && ((*pMaxCard) > maxCard || (*pMaxCard) < 0)) {
		(*pMaxCard) = maxCard;
	}

	int64_t	iRDFClassParent = GetClassParentsByIterator(iRDFClass, 0);
	while (iRDFClassParent) {
		GetClassPropertyCardinalityRestrictionNested_(iRDFClassParent, iRDFProperty, pMinCard, pMaxCard);
		iRDFClassParent = GetClassParentsByIterator(iRDFClass, iRDFClassParent);
	}
#endif
}

// ------------------------------------------------------------------------------------------------
void CRDFProperty::GetRestrictions(int64_t iInstance, int64_t& iMinCard, int64_t& iMaxCard) const
{
	ASSERT(iInstance != 0);

	int64_t iClassInstance = GetInstanceClass(iInstance);
	ASSERT(iClassInstance != 0);

	iMinCard = -1;
	iMaxCard = -1;
	GetClassPropertyCardinalityRestrictionNested_(iClassInstance, (int64_t) GetInstance(), &iMinCard, &iMaxCard);
}

