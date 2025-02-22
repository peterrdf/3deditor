#include "_host.h"
#include "_rdf_mvc.h"
#include "_ptr.h"

// ************************************************************************************************
_rdf_model::_rdf_model()
	: _model()	
{
}

/*virtual*/ _rdf_model::~_rdf_model()
{	
}

// ************************************************************************************************
_rdf_view::_rdf_view()
	: _view()
{
}

/*virtual*/ _rdf_view::~_rdf_view()
{
}

_rdf_controller* _rdf_view::getRDFController() const
{
	return _ptr<_rdf_controller>(m_pController);
}

// ************************************************************************************************
_rdf_controller::_rdf_controller()
	: _controller()
{
}

/*virtual*/ _rdf_controller::~_rdf_controller()
{
}
