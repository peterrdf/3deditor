#pragma once

#include "_mvc.h"
#include "_rdf_class.h"
#include "_rdf_property.h"
#include "_oglScene.h"

#include <map>
using namespace std;

// ************************************************************************************************
class _rdf_controller;
class _rdf_instance;
class _text_builder;

// ************************************************************************************************
class _rdf_model : public _model
{

private: // Fields

	OwlModel m_owlModel;
	map<OwlInstance, _rdf_instance*> m_mapInstances;
	map<OwlClass, _rdf_class*> m_mapClasses;
	map<RdfProperty, _rdf_property*> m_mapProperties;
	map<OwlInstance, bool> m_mapInstanceDefaultState;

public:  // Methods

	_rdf_model();
	virtual ~_rdf_model();

	// _model
	virtual void scale() override;
	virtual OwlModel getOwlModel() const override { return m_owlModel; }

	void attachModel(const wchar_t* szPath, OwlModel owlModel);
	void importModel(const wchar_t* szPath);

protected:

	// _model
	virtual void addInstance(_instance* pInstance) override;
	virtual _instance* loadInstance(int64_t /*iInstance*/) override { assert(false); return nullptr; }
	virtual void clean(bool bCloseModel = true) override;

	void loadClasses();
	void loadProperties();
	void loadInstances();

	virtual void preLoad();
	void load();
	virtual void postLoad() {}

	void getInstancesDefaultEnableState();
	void getInstanceDefaultEnableStateRecursive(OwlInstance owlInstance);
	void updateVertexBufferOffset();

public:  // Methods	

	_rdf_instance* getInstanceByOwlInstance(OwlInstance owlInstance);

	void getCompatibleInstances(_rdf_instance* pInstance, _rdf_property* pProperty, vector<OwlInstance>& vecCompatibleInstances) const;
	void getClassAncestors(OwlClass owlClass, vector<OwlClass>& vecAncestors) const;

	_rdf_instance* createInstance(OwlClass owlClass);
	bool deleteInstance(_rdf_instance* pInstance);
	void resetInstancesDefaultEnableState();

	void reloadGeometries();

	// Helpers
	static OwlInstance translateTransformation(
		OwlModel owlModel,
		OwlInstance owlInstance,
		double dX, double dY, double dZ);
	static OwlInstance translateTransformation(
		OwlModel owlModel,
		OwlInstance owlInstance,
		double dX, double dY, double dZ,
		double d11, double d22, double d33);
	static OwlInstance rotateTransformation(
		OwlModel owlModel,
		OwlInstance owlInstance,
		double alpha, double beta, double gamma);
	static OwlInstance scaleTransformation(
		OwlModel owlModel,
		OwlInstance OwlInstance, 
		double dFactor);

public: // Properties

	const map<OwlClass, _rdf_class*>& getClasses() const { return m_mapClasses; }
	const map<RdfProperty, _rdf_property*>& getProperties() const { return m_mapProperties; }
};

// ************************************************************************************************
class _rdf_view : public _view
{

public: // Methods

	_rdf_view();
	virtual ~_rdf_view();

	// Events
	virtual void onInstancePropertySelected(_view* /*pSender*/) {}
	virtual void onShowBaseInformation(_view* /*pSender*/, _rdf_instance* /*pInstance*/) {}
	virtual void onShowMetaInformation(_view* /*pSender*/, _rdf_instance* /*pInstance*/) {}
	virtual void onInstanceRenamed(_view* /*pSender*/, _rdf_instance* /*pInstance*/) {}
	virtual void onInstanceCreated(_view* /*pSender*/, _rdf_instance* /*pInstance*/) {}
	virtual void onInstanceDeleted(_view* /*pSender*/, _rdf_instance* /*pInstance*/) {}
	virtual void onInstancesDeleted(_view* /*pSender*/) {}
	virtual void onMeasurementsAdded(_view* /*pSender*/, _rdf_instance* /*pInstance*/) {}
	virtual void onInstancePropertyEdited(_view* /*pSender*/, _rdf_instance* /*pInstance*/, _rdf_property* /*pProperty*/) {}

public: // Properties

	_rdf_controller* getRDFController() const;
	_rdf_model* getRDFModel() const;
};

// ************************************************************************************************
class _rdf_controller : public _controller
{

private: // Fields

	// Selection
	_rdf_property* m_pSelectedProperty;

	// UI
	int m_iVisibleValuesCountLimit;

	// OpenGL View
	bool m_bScaleAndCenterAllVisibleGeometry;
	bool m_bModelCoordinateSystem;

public: // Methods

	_rdf_controller();
	virtual ~_rdf_controller();

	// _controller
	virtual void selectInstances(_view* pSender, const vector<_instance*>& vecInstance, bool bAdd = false) override;

protected: // Methods

	// _controller
	virtual void onModelUpdated() override;
	virtual void onInstancesEnabledStateChanged(_view* pSender) override;
	virtual void cleanSelection() override;

public: // Methods

	// Events
	void selectInstanceProperty(_view* pSender, _rdf_instance* pInstance, _rdf_property* pProperty);
	void showBaseInformation(_view* pSender, _rdf_instance* pInstance);
	void showMetaInformation(_view* pSender, _rdf_instance* pInstance);
	void renameInstance(_view* pSender, _rdf_instance* pInstance, const wchar_t* szName);
	_rdf_instance* createInstance(_view* pSender, OwlClass owlClass);
	bool deleteInstance(_view* pSender, _rdf_instance* pInstance);
	bool deleteInstanceTree(_view* pSender, _rdf_instance* pInstance);
	bool deleteInstanceTreeRecursive(_view* pSender, _rdf_instance* pInstance);
	bool deleteInstances(_view* pSender, const vector<_rdf_instance*>& vecInstances);
	void onMeasurementsAdded(_view* pSender, _rdf_instance* pInstance);
	void onInstancePropertyEdited(_view* pSender, _rdf_instance* pInstance, _rdf_property* pProperty);

public: // Properties

	_rdf_property* getSelectedInstanceProperty() const { return m_pSelectedProperty; }

	int getVisibleValuesCountLimit() const { return m_iVisibleValuesCountLimit; }
	void setVisibleValuesCountLimit(int iNewValue) { m_iVisibleValuesCountLimit = iNewValue; }
	bool getScaleAndCenterAllVisibleGeometry() const { return m_bScaleAndCenterAllVisibleGeometry; }
	void setScaleAndCenterAllVisibleGeometry(bool bNewValue) { m_bScaleAndCenterAllVisibleGeometry = bNewValue; }
	bool getModelCoordinateSystem() const { return m_bModelCoordinateSystem; }
	void setModelCoordinateSystem(bool bNewValue) { m_bModelCoordinateSystem = bNewValue; }
};

// ************************************************************************************************
class _coordinate_system_model 
	: public _rdf_model
	, public _decoration
{

private: // Fields

	_controller* m_pController;
	bool m_bUpdateVertexBuffers;
	_text_builder* m_pTextBuilder;

public: // Methods

	_coordinate_system_model(_controller* pController, bool bUpdateVertexBuffers = true);
	virtual ~_coordinate_system_model();

	// _model
	virtual void scale() override {};

	// _decoration
	virtual void onModelUpdated() override;

protected: // Methods

	// _rdf_model
	virtual void preLoad() override;

private: // Methods

	void create();
};

// ************************************************************************************************
class _navigator_model 
	: public _rdf_model
	, public _decoration
{

public: // Constants

	static const int NAVIGATION_VIEW_LENGTH;

private: // Fields

	_text_builder* m_pTextBuilder;

public: // Methods

	_navigator_model();
	virtual ~_navigator_model();

	// _model	
	virtual bool prepareScene(_oglScene* pScene) override;
	virtual void scale() override {};

protected: // Methods

	// _rdf_model
	virtual void preLoad() override;

private: // Methods

	void create();
	void createLabels(OwlModel owlModel);
};

// ************************************************************************************************
class _navigator_coordinate_system_model : public _coordinate_system_model
{

public: // Methods

	_navigator_coordinate_system_model(_controller* pController);
	virtual ~_navigator_coordinate_system_model();

	// _model	
	virtual bool prepareScene(_oglScene* pScene) override;
};