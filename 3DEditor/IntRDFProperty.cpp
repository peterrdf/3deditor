#include "stdafx.h"
#include "IntRDFProperty.h"

// ------------------------------------------------------------------------------------------------
CIntRDFProperty::CIntRDFProperty(int64_t iInstance)
	: CRDFProperty(iInstance)
{
	m_iType = TYPE_INT_DATATYPE;
}

// ------------------------------------------------------------------------------------------------
CIntRDFProperty::~CIntRDFProperty()
{
}
