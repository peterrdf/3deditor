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
