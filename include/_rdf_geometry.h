#include "_geometry.h"

// ************************************************************************************************
class _rdf_geometry : public _geometry
{

public: // Methods

	_rdf_geometry(OwlInstance owlInstance)
		: _geometry(owlInstance)
	{
	}

	virtual ~_rdf_geometry()
	{
	}
};