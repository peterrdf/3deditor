#pragma once

#include "_mvc.h"
#include "Texture.h"
#include "RDFClass.h"
#include "RDFProperty.h"
#include "RDFInstance.h"
#include "TextBuilder.h"

#include <map>
using namespace std;

// ************************************************************************************************
static TCHAR SUPPORTED_FILES[] = _T("RDF Files (*.rdf;*.bin)|*.rdf;*.bin|\
CityGML Files (*.gml;*citygml)|\
*.gml;*citygml|LandXML Files (*.xml)|*.xml;|\
CityJSON Files (*.city.json)|*.city.json|\
Autocad DXF Files (*.dxf)|*.dxf|\
All Files (*.*)|*.*||");

// ************************************************************************************************
class CRDFModel : public _model
{
	friend class CLoadTask;

protected: // Members

	// Model	
	map<OwlClass, CRDFClass*> m_mapClasses;
	map<RdfProperty, CRDFProperty*> m_mapProperties;
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
	CTexture* m_pDefaultTexture;
	map<wstring, CTexture*> m_mapTextures;

	// Support for text
	CTextBuilder* m_pTextBuilder;

public: // Methods

	CRDFModel();
	virtual ~CRDFModel();	
	
	virtual void CreateDefaultModel();
	void Load(const wchar_t* szPath, bool bLoading);
	void LoadDXF(const wchar_t* szPath);
	void LoadGISModel(const wchar_t* szPath);
	void ImportModel(const wchar_t* szPath);
	void Save(const wchar_t* szPath);
	void ResetInstancesDefaultState();
		
	void GetClassAncestors(OwlClass iClassInstance, vector<OwlClass> & vecAncestors) const;
	CRDFInstance* GetInstanceByID(int64_t iID);
	CRDFInstance* GetInstanceByIInstance(int64_t iInstance);

	CRDFInstance* CreateNewInstance(int64_t iClassInstance);
	CRDFInstance* AddNewInstance(int64_t pThing);
	bool DeleteInstance(CRDFInstance* pInstance);

	void AddMeasurements(CRDFInstance* pInstance);	

	void GetCompatibleInstances(CRDFInstance* pInstance, CObjectRDFProperty* pObjectRDFProperty, vector<int64_t>& vecCompatibleInstances) const;
	
	void GetVertexBuffersOffset(double& dVertexBuffersOffsetX, double& dVertexBuffersOffsetY, double& dVertexBuffersOffsetZ) const;
	double GetOriginalBoundingSphereDiameter() const;
	void GetWorldDimensions(float& fXmin, float& fXmax, float& fYmin, float& fYmax, float& fZmin, float& fZmax) const;
	float GetBoundingSphereDiameter() const;

	virtual void ScaleAndCenter(bool bLoadingModel = false);	

	void ZoomToInstance(int64_t iInstance);
	void ZoomOut();

	void OnInstanceNameEdited(CRDFInstance* pInstance);
	void OnInstancePropertyEdited(CRDFInstance* pInstance, CRDFProperty* pProperty);

	CTexture* GetTexture(const wstring& strTexture);
	CTexture* GetDefaultTexture();

	const CString& GetInstanceMetaData(CRDFInstance* pInstance);
	void GetPropertyMetaData(CRDFInstance* pInstance, CRDFProperty* pProperty, CString& strMetaData, const CString& strPrefix = L"");

	const map<OwlClass, CRDFClass*>& GetClasses() const { return m_mapClasses; }
	const map<RdfProperty, CRDFProperty*>& GetProperties() const { return m_mapProperties; }
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

