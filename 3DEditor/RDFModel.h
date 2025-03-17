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

protected: // Fields

	OwlModel m_iModel;

	// Model
	bool m_bExternalModel;
	map<OwlInstance, _rdf_instance*> m_mapInstances;

	// Support for textures
	_texture* m_pDefaultTexture;

	// Support for text
	CTextBuilder* m_pTextBuilder;

public: // Methods

	CRDFModel();
	virtual ~CRDFModel();	

	// _model
	virtual OwlModel getOwlModel() const override { return m_iModel; }
	virtual _instance* loadInstance(int64_t /*iInstance*/) override { ASSERT(FALSE); return nullptr; } //#todo

	// _rdf_model
	virtual _texture* getDefaultTexture() override;
	
	virtual void CreateDefaultModel();
	void Load(const wchar_t* szPath, bool bLoading);
	void Load(OwlInstance iInstance);
#ifdef _DXF_SUPPORT
	void LoadDXF(const wchar_t* szPath);
#endif
#ifdef _GIS_SUPPORT
	void LoadGISModel(const wchar_t* szPath);
#endif
	void ImportModel(const wchar_t* szPath);
	void Save(const wchar_t* szPath);
		
	_rdf_instance* GetInstanceByID(int64_t iID);
	_rdf_instance* GetInstanceByIInstance(int64_t iInstance);

	_rdf_instance* AddNewInstance(int64_t pThing);

	virtual void ScaleAndCenter(bool bLoadingModel = false);
	
	const map<OwlInstance, _rdf_instance*>& GetInstances() const { return m_mapInstances; }

protected: // Methods

	void SetFormatSettings(int64_t /*iModel*/) { assert(0); /*#todo*/ }

	void Clean();

	OwlInstance Translate(
		OwlInstance iInstance, 
		double dX, double dY, double dZ,
		double d11, double d22, double d33);
	OwlInstance Scale(OwlInstance iInstance, double dFactor);
	OwlInstance Rotate(
		OwlInstance iInstance, 
		double alpha, double beta, double gamma);
};

// ************************************************************************************************
class CSceneRDFModel : public CRDFModel
{

public: // Methods

	CSceneRDFModel();
	virtual ~CSceneRDFModel();

	void TranslateModel(float fX, float fY, float fZ);

	// CRDFModel
	virtual void CreateDefaultModel() override;
	virtual void ScaleAndCenter(bool /*bLoadingModel = false*/) override {};

protected: // Methods

	// CRDFModel
	virtual void PostLoadDRFModel() /*override*/;//#todo

	void CreateCoordinateSystem();
};

// ************************************************************************************************
class CNavigatorRDFModel : public CSceneRDFModel
{

public: // Methods

	CNavigatorRDFModel();
	virtual ~CNavigatorRDFModel();

	virtual void CreateDefaultModel() override;

protected: // Methods

private: // Methods

	void CreateNaigator();
	void CreateNaigatorLabels();
};

