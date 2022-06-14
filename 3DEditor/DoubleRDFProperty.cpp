#include "stdafx.h"
#include "DoubleRDFProperty.h"

// ------------------------------------------------------------------------------------------------
CDoubleRDFProperty::CDoubleRDFProperty(int64_t iInstance)
	: CRDFProperty(iInstance)
{
	m_iType = TYPE_DOUBLE_DATATYPE;
}

// ------------------------------------------------------------------------------------------------
CDoubleRDFProperty::~CDoubleRDFProperty()
{
}
