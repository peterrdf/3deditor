#pragma once

#include "_mvc.h"

#include <map>
using namespace std;

// ************************************************************************************************
class _rdf_controller;
class _rdf_instance;

// ************************************************************************************************
class _rdf_model : public _model
{

private: // Members

	map<OwlInstance, _rdf_instance*> m_mapInstances;

public:  // Methods

	_rdf_model();
	virtual ~_rdf_model();	

	_rdf_instance* getInstance(OwlInstance owlInstance);

protected:

	// _model
	virtual void addInstance(_instance* pInstance) override;
	virtual void clean(bool bCloseModel = true) override;
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

private: // Members

	// UI
	int m_iVisibleValuesCountLimit;

	// OpenGL View
	bool m_bScaleAndCenter;
	bool m_bModelCoordinateSystem;

public: // Methods

	_rdf_controller();
	virtual ~_rdf_controller();

	int getVisibleValuesCountLimit() const { return m_iVisibleValuesCountLimit; }
	void setVisibleValuesCountLimit(int iNewValue) { m_iVisibleValuesCountLimit = iNewValue; }
	bool getScaleAndCenter() const { return m_bScaleAndCenter; }
	void setScaleAndCenter(bool bNewValue) { m_bScaleAndCenter = bNewValue; }
	bool getModelCoordinateSystem() const { return m_bModelCoordinateSystem; }
	void setModelCoordinateSystem(bool bNewValue) { m_bModelCoordinateSystem = bNewValue; }

	// Events
};