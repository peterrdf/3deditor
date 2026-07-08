#include "_host.h"
#include "_ifc_geometry.h"

#include <cwctype>
#include <algorithm>
using namespace std;

// ************************************************************************************************
_ifc_geometry::_ifc_geometry(OwlInstance owlInstance, SdaiInstance sdaiInstance, const vector<_ifc_geometry*>& vecMappedGeometries, MultiThreadOwlModelWrapper multiThreadOwlModelWrapper)
	: _ap_geometry(owlInstance, sdaiInstance, multiThreadOwlModelWrapper)
	, m_vecMappedGeometries(vecMappedGeometries)
	, m_bIsMappedItem(false)
	, m_bIsReferenced(false)
{
	if (m_vecMappedGeometries.empty()) {
		calculate();
	}
}

/*virtual*/ _ifc_geometry::~_ifc_geometry()
{}

/*virtual*/ void _ifc_geometry::preCalculate() /*override*/
{
	// Format
	setAPFormatSettings();

	// Extra settings
	setFilter(getSdaiModel(), FLAGBIT(1), FLAGBIT(1));
	if (getMultiThreadOwlModelWrapper() != 0) {
		setSegmentation(getMultiThreadOwlModelWrapper(), 16, 0.);
	}
	else {
		setSegmentation(getSdaiModel(), 16, 0.);
	}
}

/*virtual*/ void _ifc_geometry::postCalculate() /*override*/
{
	_geometry::cleanCachedGeometry();

	if (getMultiThreadOwlModelWrapper() != 0) {
		cleanMemory(getMultiThreadOwlModelWrapper(), 1);
	}
	else {
		cleanMemory(getSdaiModel(), 1);
	}
}

/*virtual*/ bool _ifc_geometry::hasGeometry() const /*override*/
{
	if (!m_vecMappedGeometries.empty()) {
		for (auto pMappedGeometry : m_vecMappedGeometries) {
			if (pMappedGeometry->hasGeometry()) {
				return true;
			}
		}

		return false;
	}

	return _geometry::hasGeometry();
}

/*virtual*/ bool _ifc_geometry::isPlaceholder() const /*override*/
{
	return !m_vecMappedGeometries.empty();
}

/*virtual*/ bool _ifc_geometry::ignoreBB() const /*override*/
{
	if (_geometry::ignoreBB()) {
		return true;
	}

	if (sdaiGetEntity(getSdaiModel(), "IfcAlignmentSegment") == sdaiGetInstanceType(getSdaiInstance())) {
		return true;
	}

	return false;
}

void _ifc_geometry::setDefaultShowState()
{
	wstring strEntity = getEntityName();
	std::transform(strEntity.begin(), strEntity.end(), strEntity.begin(), ::towupper);

	setShow(
		(strEntity == L"IFCRELSPACEBOUNDARY") ||
		(strEntity == L"IFCOPENINGELEMENT") ? false : true);
}