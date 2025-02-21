#pragma once

#include "_mvc.h"

// ************************************************************************************************
class _rdf_controller;

// ************************************************************************************************
class _rdf_model : public _model
{

public:  // Methods

	_rdf_model();
	virtual ~_rdf_model();
};

// ************************************************************************************************
class _rdf_view : public _view
{

public: // Methods

	_rdf_view();
	virtual ~_rdf_view();

	// Events

public: // Properties

	_rdf_controller* getRDFController() const;
};

// ************************************************************************************************
class _rdf_controller : public _controller
{

public: // Methods

	_rdf_controller();
	virtual ~_rdf_controller();

	// Events
};