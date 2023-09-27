#include "stdafx.h"
#include "BoolRDFProperty.h"

// ------------------------------------------------------------------------------------------------
CBoolRDFProperty::CBoolRDFProperty(int64_t iInstance)
	: CRDFProperty(iInstance)
{
	m_iType = DATATYPEPROPERTY_TYPE_BOOLEAN;
}

// ------------------------------------------------------------------------------------------------
CBoolRDFProperty::~CBoolRDFProperty()
{
}
