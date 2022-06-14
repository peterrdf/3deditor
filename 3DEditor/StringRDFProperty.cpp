#include "stdafx.h"
#include "StringRDFProperty.h"

// ------------------------------------------------------------------------------------------------
CStringRDFProperty::CStringRDFProperty(int64_t iInstance)
	: CRDFProperty(iInstance)
{
	m_iType = TYPE_CHAR_DATATYPE;
}

// ------------------------------------------------------------------------------------------------
CStringRDFProperty::~CStringRDFProperty()
{
}
