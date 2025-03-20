#pragma once

#include "Texture.h"
#include "TextBuilder.h"

#include "_rdf_mvc.h"
#include "_rdf_instance.h"
#include "_rdf_class.h"
#include "_rdf_property.h"

#include <map>
using namespace std;

// ************************************************************************************************
static TCHAR SUPPORTED_FILES[] = _T("BIN Files (*.bin)|*.bin|\
CityGML Files (*.gml;*citygml)|\
*.gml;*citygml|LandXML Files (*.xml)|*.xml;|\
CityJSON Files (*.city.json;*.json)|*.city.json;*.json|\
Autocad DXF Files (*.dxf)|*.dxf|\
All Files (*.*)|*.*||");

// ************************************************************************************************
class CRDFModel : public _rdf_model
{
	friend class CLoadTask;

private: // Fields

	_texture* m_pDefaultTexture;

public: // Methods

	CRDFModel();
	virtual ~CRDFModel();

public: // Methods

	// _model
	virtual _texture* getDefaultTexture() override;

protected: // Methods

	// _model
	virtual void clean(bool bCloseModel = true) override;

public: // Methods

	void Load(const wchar_t* szPath, bool bAdd);

#ifdef _DXF_SUPPORT
	void LoadDXF(const wchar_t* szPath);
#endif

#ifdef _GIS_SUPPORT
	void LoadGISModel(const wchar_t* szPath);
#endif	
};


// ************************************************************************************************
class CDefaultModel : public CRDFModel
{

public: // Methods

	CDefaultModel();
	virtual ~CDefaultModel();

private: // Methods

	void Create();
};

// ************************************************************************************************
class CCoordinateSystemModel : public CRDFModel
{

private: // Members

	_model* m_pWorld;
	CTextBuilder* m_pTextBuilder;

public: // Methods

	CCoordinateSystemModel(_model* pWorld);
	virtual ~CCoordinateSystemModel();

	// _model	
	virtual void scale() override {};

protected: // Methods

	// _rdf_model
	virtual void preLoad() override;

private: // Methods

	void Create();
};

// ************************************************************************************************
class CNavigatorModel : public CRDFModel
{

private: // Members

	CTextBuilder* m_pTextBuilder;

public: // Methods

	CNavigatorModel();
	virtual ~CNavigatorModel();

	// _model	
	virtual void scale() override {};

protected: // Methods

	// _rdf_model
	virtual void preLoad() override;

private: // Methods

	void Create();
	void CreateLabels(OwlModel owlModel);
};