#include "stdafx.h"
#include "DoubleRDFProperty.h"

// ------------------------------------------------------------------------------------------------
CDoubleRDFProperty::CDoubleRDFProperty(int64_t iInstance)
	: CRDFProperty(iInstance)
{
	m_iType = DATATYPEPROPERTY_TYPE_DOUBLE;
}

// ------------------------------------------------------------------------------------------------
CDoubleRDFProperty::~CDoubleRDFProperty()
{
}
