#include "stdafx.h"
#include "ObjectRDFProperty.h"

// ------------------------------------------------------------------------------------------------
CObjectRDFProperty::CObjectRDFProperty(int64_t iInstance)
	: CRDFProperty(iInstance)
	, m_vecRestrictions()
{
	m_iType = TYPE_OBJECTTYPE;

#ifndef _LINUX
//	LOG_DEBUG("*** RESTRICTIONS ***");
#endif // _LINUX

	int64_t	iRestrictionsClassInstance = GetRangeRestrictionsByIterator(GetInstance(), 0);
	while (iRestrictionsClassInstance != 0)
	{
		char * szRestrictionsClassName = nullptr;
		GetNameOfClass(iRestrictionsClassInstance, &szRestrictionsClassName);

#ifndef _LINUX
//		LOG_DEBUG("*** CLASS " << szRestrictionsClassName);
#endif // _LINUX

		m_vecRestrictions.push_back(iRestrictionsClassInstance);

		iRestrictionsClassInstance = GetRangeRestrictionsByIterator(GetInstance(), iRestrictionsClassInstance);
	} // while (iRestrictionsClassInstance != 0)

#ifndef _LINUX
//	LOG_DEBUG("*** END RESTRICTIONS ***");
#endif // _LINUX
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
