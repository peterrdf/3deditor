#include "_host.h"
#include "_rdf_instance.h"

// ************************************************************************************************
_rdf_instance::_rdf_instance(int64_t iID, _geometry* pGeometry, _matrix4x3* pTransformationMatrix)
	: _instance(iID, pGeometry, pTransformationMatrix)
	, m_bNeedsRefresh(true)
{
}

/*virtual*/ _rdf_instance::~_rdf_instance()
{
}

/*virtual*/ void _rdf_instance::setEnable(bool bEnable) /*override*/
{
	_instance::setEnable(bEnable);

	if (getEnable() && m_bNeedsRefresh)
	{
		getGeometryAs<_rdf_geometry>()->recalculate();

		m_bNeedsRefresh = false;
	}
}

void _rdf_instance::recalculate(bool bForce/* = false*/)
{
	// Recalculate
	if (bForce)
	{
		getGeometryAs<_rdf_geometry>()->recalculate();
		m_bNeedsRefresh = false;

		return;
	}

	// Recalculate if Enabled
	if (!getEnable())
	{
		// Recalculate on demand
		m_bNeedsRefresh = true;

		return;
	}

	getGeometryAs<_rdf_geometry>()->recalculate();
}