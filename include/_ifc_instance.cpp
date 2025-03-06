#include "_host.h"
#include "_ifc_instance.h"
#include "_ifc_geometry.h"

#include <algorithm>

// ************************************************************************************************
_ifc_instance::_ifc_instance(int64_t iID, _ifc_geometry* pGeometry, _matrix4x3* pTransformationMatrix)
	: _ap_instance(iID, pGeometry, pTransformationMatrix)
	, m_pOwner(nullptr)
{
}

/*virtual*/ _ifc_instance::~_ifc_instance()
{
}

/*virtual*/ _instance* _ifc_instance::getOwner() const /*override*/
{
	return m_pOwner;
}

void _ifc_instance::setDefaultEnableState()
{
	wstring strEntity = getEntityName();
	std::transform(strEntity.begin(), strEntity.end(), strEntity.begin(), ::towupper);

	setEnable(
		(strEntity == L"IFCSPACE") ||
		(strEntity == L"IFCRELSPACEBOUNDARY") ||
		(strEntity == L"IFCOPENINGELEMENT") ||
		(strEntity == L"IFCALIGNMENTVERTICAL") ||
		(strEntity == L"IFCALIGNMENTHORIZONTAL") ||
		(strEntity == L"IFCALIGNMENTSEGMENT") ||
		(strEntity == L"IFCALIGNMENTCANT") ? false : true);
}
