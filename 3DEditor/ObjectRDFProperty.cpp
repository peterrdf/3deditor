#include "stdafx.h"
#include "ObjectRDFProperty.h"

// ------------------------------------------------------------------------------------------------
CObjectRDFProperty::CObjectRDFProperty(int64_t iInstance)
	: CRDFProperty(iInstance)
	, m_vecRestrictions()
{
	m_iType = TYPE_OBJECTTYPE;

	int64_t	iRestrictionsClassInstance = GetRangeRestrictionsByIterator(GetInstance(), 0);
	while (iRestrictionsClassInstance != 0)
	{
		char * szRestrictionsClassName = nullptr;
		GetNameOfClass(iRestrictionsClassInstance, &szRestrictionsClassName);

		m_vecRestrictions.push_back(iRestrictionsClassInstance);

		iRestrictionsClassInstance = GetRangeRestrictionsByIterator(GetInstance(), iRestrictionsClassInstance);
	} // while (iRestrictionsClassInstance != 0)
}

// ------------------------------------------------------------------------------------------------
CObjectRDFProperty::~CObjectRDFProperty()
{
}

// ------------------------------------------------------------------------------------------------
// Getter
const vector<int64_t> & CObjectRDFProperty::getRestrictions()
{
	return m_vecRestrictions;
}
