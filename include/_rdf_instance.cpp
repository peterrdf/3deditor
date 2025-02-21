#include "_host.h"
#include "_rdf_instance.h"

// ************************************************************************************************
_rdf_instance::_rdf_instance(int64_t iID, _geometry* pGeometry, _matrix4x3* pTransformationMatrix)
	: _instance(iID, pGeometry, pTransformationMatrix)	
{
}

/*virtual*/ _rdf_instance::~_rdf_instance()
{
}

/*virtual*/ void _rdf_instance::setEnable(bool bEnable) /*override*/
{
	_instance::setEnable(bEnable);

	if (getEnable() && getGeometryAs<_rdf_geometry>()->needsRefresh())
	{
		getGeometryAs<_rdf_geometry>()->recalculate();
	}
}

void _rdf_instance::recalculate()
{
	if (!getEnable())
	{
		// Reloading on demand
		getGeometryAs<_rdf_geometry>()->needsRefresh() = true;

		return;
	}

	getGeometryAs<_rdf_geometry>()->recalculate();
}