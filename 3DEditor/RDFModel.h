#pragma once

#include "conceptMesh.h"
#include "Texture.h"
#include "RDFClass.h"
#include "ObjectRDFProperty.h"
#include "BoolRDFProperty.h"
#include "StringRDFProperty.h"
#include "IntRDFProperty.h"
#include "DoubleRDFProperty.h"
#include "UndefinedRDFProperty.h"
#include "RDFInstance.h"

#include <map>

using namespace std;

#define EMPTY_INSTANCE L"---<EMPTY>---"

// ------------------------------------------------------------------------------------------------
// Describes an RDF model
class CRDFModel
{

private: // Members

	wstring m_strModel;
	int64_t m_iModel;

	// Cache
	map<int64_t, CRDFClass *> m_mapClasses;
	map<int64_t, CRDFProperty *> m_mapProperties;
	map<int64_t, CRDFInstance *> m_mapInstances;
	map<CRDFInstance*, CString> m_mapInstanceMetaData;

	// ID (1-based index)
	int64_t m_iID;

	// World's dimensions
	float m_fXmin;
	float m_fXmax;
	float m_fYmin;
	float m_fYmax;
	float m_fZmin;
	float m_fZmax;

	// World's bounding sphere diameter
	float m_fBoundingSphereDiameter;

	// World's translations - center
	float m_fXTranslation;
	float m_fYTranslation;
	float m_fZTranslation;

	// Support for textures
	CTexture* m_pDefaultTexture;
	map<wstring, CTexture*> m_mapTextures;

	// Root
	_octree* m_pOctree;

public: // Methods

	CRDFModel();
	virtual ~CRDFModel();

	int64_t GetModel() const;
	void CreateDefaultModel();

	const map<int64_t, CRDFClass *>& GetClasses() const;
	void GetClassAncestors(int64_t iClassInstance, vector<int64_t> & vecAncestors) const;
	const map<int64_t, CRDFProperty *>& GetProperties();
	const map<int64_t, CRDFInstance *>& GetInstances() const;
	CRDFInstance* GetInstanceByID(int64_t iID);
	CRDFInstance* GetInstanceByIInstance(int64_t iInstance);
	CRDFInstance* CreateNewInstance(int64_t iClassInstance);

	CRDFInstance* AddNewInstance(int64_t pThing);
	bool DeleteInstance(CRDFInstance* pInstance);

	void AddMeasurements(CRDFInstance* pInstance);

	void ImportModel(const wchar_t* szPath);

	void GetCompatibleInstances(CRDFInstance* pInstance, CObjectRDFProperty* pObjectRDFProperty, vector<int64_t>& vecCompatibleInstances) const;

	void GetWorldDimensions(float& fXmin, float& fXmax, float& fYmin, float& fYmax, float& fZmin, float& fZmax) const;
	void GetWorldTranslations(float& fXTranslation, float& fYTranslation, float& fZTranslation) const;

	void ScaleAndCenter(bool bLoadingModel = false);

	float GetBoundingSphereDiameter() const;

	void ZoomToInstance(int64_t iInstance);
	void ZoomOut();

	void OnInstancePropertyEdited(CRDFInstance* pInstance, CRDFProperty* pProperty);

	void Save(const wchar_t * szPath);

	void Load(const wchar_t* szPath);	
	void LoadDXF(const wchar_t* szPath);
	void LoadGISModel(const wchar_t* szPath);
	void PostLoad();

	CTexture* GetTexture(const wstring& strTexture);
	CTexture* GetDefaultTexture();

	const CString& GetInstanceMetaData(CRDFInstance* pInstance);
	void GetPropertyMetaData(CRDFInstance* pInstance, CRDFProperty* pProperty, CString& strMetaData, const CString& strPrefix=L"");

private: // Methods

	void SetFormatSettings(int64_t iModel);
	void LoadRDFModel();
	void EnableInstancesRecursively(CRDFInstance* iRDFInstance);
	void SetDefaultEnabledInstances();
	void LoadRDFInstances();

	void Clean();

	void GetClassPropertyCardinalityRestrictionNested(int64_t iRDFClass, int64_t iRDFProperty, int64_t * pMinCard, int64_t * pMaxCard);
	void BuildOctants(_octant* pOctant, vector<GEOM::GeometricItem>& vecOctantsGeometry);
};

