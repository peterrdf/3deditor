#pragma once

#include "conceptMesh.h"
#include "RDFMeasurementsBuilder.h"
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

	// --------------------------------------------------------------------------------------------
	// Model	
	int64_t m_iModel;

	// --------------------------------------------------------------------------------------------
	// Instance : Class
	map<int64_t, CRDFClass *> mapClasses;

	// --------------------------------------------------------------------------------------------
	// Instance : Property
	map<int64_t, CRDFProperty *> mapProperties;

	// --------------------------------------------------------------------------------------------
	// Instance : Object
	map<int64_t, CRDFInstance *> m_mapInstances;

	// --------------------------------------------------------------------------------------------
	// ID (1-based index)
	int64_t m_iID;

	// --------------------------------------------------------------------------------------------
	// World's dimensions
	float m_fXmin;
	float m_fXmax;
	float m_fYmin;
	float m_fYmax;
	float m_fZmin;
	float m_fZmax;

	// --------------------------------------------------------------------------------------------
	// World's bounding sphere diameter
	float m_fBoundingSphereDiameter;

	// --------------------------------------------------------------------------------------------
	// World's translations - center
	float m_fXTranslation;
	float m_fYTranslation;
	float m_fZTranslation;

	// --------------------------------------------------------------------------------------------
	// Support for textures
	CTexture* m_pDefaultTexture;

	// --------------------------------------------------------------------------------------------
	// Support for measurements
	CRDFMeasurementsBuilder* m_pMeasurementsBuilder;

	// --------------------------------------------------------------------------------------------
	// Root
	_octree* m_pOctree;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CRDFModel();

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CRDFModel();

	// --------------------------------------------------------------------------------------------
	// Getter
	int64_t GetModel() const;

	// --------------------------------------------------------------------------------------------
	// Default model
	void CreateDefaultModel();

	// --------------------------------------------------------------------------------------------
	// Getter
	const map<int64_t, CRDFClass *>& GetClasses() const;

	// --------------------------------------------------------------------------------------------
	// Getter
	void GetClassAncestors(int64_t iClassInstance, vector<int64_t> & vecAncestors) const;

	// --------------------------------------------------------------------------------------------
	// Getter
	const map<int64_t, CRDFProperty *>& GetProperties();

	// --------------------------------------------------------------------------------------------
	// Getter
	const map<int64_t, CRDFInstance *>& GetInstances() const;

	// --------------------------------------------------------------------------------------------
	// Getter
	CRDFInstance * GetInstanceByID(int64_t iID);

	// --------------------------------------------------------------------------------------------
	// Getter
	CRDFInstance * GetInstanceByIInstance(int64_t iInstance);

	// --------------------------------------------------------------------------------------------
	// Factory
	CRDFInstance * CreateNewInstance(int64_t iClassInstance);

	// ------------------------------------------------------------------------------------------------
	CRDFInstance* AddNewInstance(int64_t pThing);

	// --------------------------------------------------------------------------------------------
	// Removes an instance
	bool DeleteInstance(CRDFInstance* pInstance);

	// --------------------------------------------------------------------------------------------
	// Adds measurements
	void AddMeasurements(CRDFInstance* pInstance);

	// --------------------------------------------------------------------------------------------
	// Import
	void ImportModel(const wchar_t* szPath);

	// --------------------------------------------------------------------------------------------
	// Support for editing of object properties
	void GetCompatibleInstances(CRDFInstance* pInstance, CObjectRDFProperty* pObjectRDFProperty, vector<int64_t>& vecCompatibleInstances) const;

	// --------------------------------------------------------------------------------------------
	// Getter
	void GetWorldDimensions(float& fXmin, float& fXmax, float& fYmin, float& fYmax, float& fZmin, float& fZmax) const;

	// --------------------------------------------------------------------------------------------
	// Getter
	void GetWorldTranslations(float& fXTranslation, float& fYTranslation, float& fZTranslation) const;

	// --------------------------------------------------------------------------------------------
	// [-1, 1]
	void ScaleAndCenter();

	// --------------------------------------------------------------------------------------------
	// Getter
	float GetBoundingSphereDiameter() const;

	// --------------------------------------------------------------------------------------------
	// Zoom to an instance
	void ZoomToInstance(int64_t iInstance);

	// --------------------------------------------------------------------------------------------
	// Zoom to all enabled instances
	void ZoomOut();

	// --------------------------------------------------------------------------------------------
	// Edit properties support
	void OnInstancePropertyEdited(CRDFInstance * pInstance, CRDFProperty * pProperty);

	// --------------------------------------------------------------------------------------------
	// Stores model
	void Save(const wchar_t * szPath);

	// --------------------------------------------------------------------------------------------
	// Loads a model
	void Load(const wchar_t* szPath);
	void LoadE57(const wchar_t* szPath);
	void LoadCityGML(const wchar_t* szPath);
	void LoadDXF(const wchar_t* szPath);
	void LoadCityJSON(const wchar_t* szPath);

	// --------------------------------------------------------------------------------------------
	// Loads a model
	void PostLoad();

	// --------------------------------------------------------------------------------------------
	// Support for textures
	CTexture * GetDefaultTexture();

	// --------------------------------------------------------------------------------------------
	// Support for measurements
	CRDFMeasurementsBuilder * GetMeasurementsBuilder();

private: // Methods

	// --------------------------------------------------------------------------------------------
	void SetFormatSettings(int64_t iModel);

	// --------------------------------------------------------------------------------------------
	// Loads RDF hierarchy
	void LoadRDFModel();

	// --------------------------------------------------------------------------------------------
	// Default state
	void EnableInstancesRecursively(CRDFInstance* iRDFInstance);

	// --------------------------------------------------------------------------------------------
	// Default state
	void SetDefaultEnabledInstances();

	// --------------------------------------------------------------------------------------------
	// Loads RDF instances
	void LoadRDFInstances();

	// --------------------------------------------------------------------------------------------
	// Clean up
	void Clean();

	// --------------------------------------------------------------------------------------------
	void GetClassPropertyCardinalityRestrictionNested(int64_t iRDFClass, int64_t iRDFProperty, int64_t * pMinCard, int64_t * pMaxCard);
	
	// --------------------------------------------------------------------------------------------
	// Octants
	void BuildOctants(_octant* pOctant, vector<GEOM::GeometricItem>& vecOctantsGeometry);
};

