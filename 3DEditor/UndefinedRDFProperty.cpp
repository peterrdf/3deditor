#include "stdafx.h"
#include "UndefinedRDFProperty.h"

// ------------------------------------------------------------------------------------------------
CUndefinedRDFProperty::CUndefinedRDFProperty(int64_t iInstance)
	: CRDFProperty(iInstance)
{
	m_iType = 0;
}

// ------------------------------------------------------------------------------------------------
CUndefinedRDFProperty::~CUndefinedRDFProperty()
{
}
