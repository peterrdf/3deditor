#pragma once

#include "_rdf_mvc.h"
#include "Texture.h"
#include "RDFClass.h"
#include "_rdf_property.h"
#include "RDFInstance.h"
#include "TextBuilder.h"

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

protected: // Members

	OwlModel m_iModel;

	// Model
	bool m_bExternalModel;
	map<OwlClass, CRDFClass*> m_mapClasses;
	map<RdfProperty, _rdf_property_t*> m_mapProperties;
	int64_t m_iID; // ID (1-based index)
	map<OwlInstance, CRDFInstance*> m_mapInstances;

	// Cache
	map<OwlInstance, bool> m_mapInstanceDefaultState; 
	map<CRDFInstance*, CString> m_mapInstanceMetaData;

	// http://rdf.bg/gkdoc/CP64/SetVertexBufferOffset.html
	double m_dVertexBuffersOffsetX;
	double m_dVertexBuffersOffsetY;
	double m_dVertexBuffersOffsetZ;
	double m_dOriginalBoundingSphereDiameter;

	// World's dimensions
	float m_fXmin;
	float m_fXmax;
	float m_fYmin;
	float m_fYmax;
	float m_fZmin;
	float m_fZmax;
	float m_fBoundingSphereDiameter;

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
	void ResetInstancesDefaultState();
		
	void GetClassAncestors(OwlClass iClassInstance, vector<OwlClass> & vecAncestors) const;
	CRDFInstance* GetInstanceByID(int64_t iID);
	CRDFInstance* GetInstanceByIInstance(int64_t iInstance);

	CRDFInstance* CreateNewInstance(int64_t iClassInstance);
	CRDFInstance* AddNewInstance(int64_t pThing);
	bool DeleteInstance(CRDFInstance* pInstance);

	void GetCompatibleInstances(CRDFInstance* pInstance, CObjectRDFProperty* pObjectRDFProperty, vector<int64_t>& vecCompatibleInstances) const;
	
	void GetVertexBuffersOffset(double& dVertexBuffersOffsetX, double& dVertexBuffersOffsetY, double& dVertexBuffersOffsetZ) const;
	double GetOriginalBoundingSphereDiameter() const;
	void GetWorldDimensions(float& fXmin, float& fXmax, float& fYmin, float& fYmax, float& fZmin, float& fZmax) const;
	float GetBoundingSphereDiameter() const;

	virtual void ScaleAndCenter(bool bLoadingModel = false);	

	void ZoomToInstance(int64_t iInstance);
	void ZoomOut();

	void OnInstanceNameEdited(CRDFInstance* pInstance);
	void OnInstancePropertyEdited(CRDFInstance* pInstance, _rdf_property_t* pProperty);

	const CString& GetInstanceMetaData(CRDFInstance* pInstance);
	void GetPropertyMetaData(CRDFInstance* pInstance, _rdf_property_t* pProperty, CString& strMetaData, const CString& strPrefix, bool& bMultiValue);

	const map<OwlClass, CRDFClass*>& GetClasses() const { return m_mapClasses; }
	const map<RdfProperty, _rdf_property_t*>& GetProperties() const { return m_mapProperties; }
	const map<OwlInstance, CRDFInstance*>& GetInstances() const { return m_mapInstances; }

protected: // Methods

	void SetFormatSettings(int64_t iModel);
	virtual void PreLoadDRFModel() {}
	void LoadRDFModel();
	virtual void PostLoadDRFModel();
	void GetInstancesDefaultState();
	void GetInstanceDefaultStateRecursive(OwlInstance iInstance);
	void UpdateVertexBufferOffset();
	void LoadRDFInstances();

	void Clean();

	OwlInstance Translate(
		OwlInstance iInstance, 
		double dX, double dY, double dZ,
		double d11, double d22, double d33);
	OwlInstance Scale(OwlInstance iInstance, double dFactor);
	OwlInstance Rotate(
		OwlInstance iInstance, 
		double alpha, double beta, double gamma);

private: // Methods	

	void GetClassPropertyCardinalityRestrictionNested(int64_t iRDFClass, int64_t iRDFProperty, int64_t * pMinCard, int64_t * pMaxCard);
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
	virtual void PostLoadDRFModel() override;

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

