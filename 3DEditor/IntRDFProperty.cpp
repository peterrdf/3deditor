#include "stdafx.h"
#include "IntRDFProperty.h"

// ------------------------------------------------------------------------------------------------
CIntRDFProperty::CIntRDFProperty(int64_t iInstance)
	: CRDFProperty(iInstance)
{
	m_iType = DATATYPEPROPERTY_TYPE_INTEGER;
}

// ------------------------------------------------------------------------------------------------
CIntRDFProperty::~CIntRDFProperty()
{
}
