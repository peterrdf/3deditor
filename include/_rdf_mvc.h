#pragma once

#include "_mvc.h"
#include "_texture.h"

// ************************************************************************************************
class _rdf_controller;

// ************************************************************************************************
class _rdf_model : public _model
{

private: // Members

	map<wstring, _texture*> m_mapTextures;

public:  // Methods

	_rdf_model();
	virtual ~_rdf_model();

	_texture* getTexture(const wstring& strTexture);
	virtual _texture* getDefaultTexture() { return nullptr; };
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