#include "stdafx.h"
#include "StringRDFProperty.h"

// ------------------------------------------------------------------------------------------------
CStringRDFProperty::CStringRDFProperty(int64_t iInstance)
	: CRDFProperty(iInstance)
{
	m_iType = DATATYPEPROPERTY_TYPE_STRING;
}

// ------------------------------------------------------------------------------------------------
CStringRDFProperty::~CStringRDFProperty()
{
}

// ------------------------------------------------------------------------------------------------
CCharArrayRDFProperty::CCharArrayRDFProperty(int64_t iInstance)
	: CRDFProperty(iInstance)
{
	m_iType = DATATYPEPROPERTY_TYPE_CHAR_ARRAY;
}

// ------------------------------------------------------------------------------------------------
CCharArrayRDFProperty::~CCharArrayRDFProperty()
{
}

// ------------------------------------------------------------------------------------------------
CWCharArrayRDFProperty::CWCharArrayRDFProperty(int64_t iInstance)
	: CRDFProperty(iInstance)
{
	m_iType = DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY;
}

// ------------------------------------------------------------------------------------------------
CWCharArrayRDFProperty::~CWCharArrayRDFProperty()
{
}
