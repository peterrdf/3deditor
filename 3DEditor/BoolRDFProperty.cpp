#include "stdafx.h"
#include "BoolRDFProperty.h"

// ------------------------------------------------------------------------------------------------
CBoolRDFProperty::CBoolRDFProperty(int64_t iInstance)
	: CRDFProperty(iInstance)
{
	m_iType = TYPE_BOOL_DATATYPE;
}

// ------------------------------------------------------------------------------------------------
CBoolRDFProperty::~CBoolRDFProperty()
{
}
