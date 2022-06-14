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
	// Coordinate system and XY grid
	Model* m_pCoordinateSystemModel;

	// --------------------------------------------------------------------------------------------
	// Instance : Class
	map<int64_t, CRDFClass *> m_mapRDFClasses;

	// --------------------------------------------------------------------------------------------
	// Instance : Property
	map<int64_t, CRDFProperty *> m_mapRDFProperties;

	// --------------------------------------------------------------------------------------------
	// Instance : Object
	map<int64_t, CRDFInstance *> m_mapRDFInstances;

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
	CTexture * m_pDefaultTexture;

	// --------------------------------------------------------------------------------------------
	// Support for measurements
	CRDFMeasurementsBuilder * m_pMeasurementsBuilder;

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
	// Getter
	int64_t GetCoordinateSystemModel() const;

	// --------------------------------------------------------------------------------------------
	// Default model
	void CreateDefaultModel();

	// --------------------------------------------------------------------------------------------
	// Getter
	const map<int64_t, CRDFClass *> & GetRDFClasses() const;

	// --------------------------------------------------------------------------------------------
	// Getter
	void GetClassAncestors(int64_t iClassInstance, vector<int64_t> & vecAncestors) const;

	// --------------------------------------------------------------------------------------------
	// Getter
	const map<int64_t, CRDFProperty *> & GetRDFProperties();

	// --------------------------------------------------------------------------------------------
	// Getter
	const map<int64_t, CRDFInstance *> & GetRDFInstances() const;

	// --------------------------------------------------------------------------------------------
	// Getter
	CRDFInstance * GetRDFInstanceByID(int64_t iID);

	// --------------------------------------------------------------------------------------------
	// Getter
	CRDFInstance * GetRDFInstanceByIInstance(int64_t iInstance);

	// --------------------------------------------------------------------------------------------
	// Factory
	CRDFInstance * CreateNewInstance(int64_t iClassInstance);

	// ------------------------------------------------------------------------------------------------
	CRDFInstance* AddNewInstance(Thing* pThing);

	// --------------------------------------------------------------------------------------------
	// Removes an instance
	bool DeleteInstance(CRDFInstance * pInstance);

	// --------------------------------------------------------------------------------------------
	// Adds measurements
	void AddMeasurements(CRDFInstance * pInstance);

	// --------------------------------------------------------------------------------------------
	// Import
	void ImportModel(const wchar_t* szPath);

	// --------------------------------------------------------------------------------------------
	// Support for editing of object properties
	void GetCompatibleInstances(CRDFInstance * pRDFInstance, CObjectRDFProperty * pObjectRDFProperty, vector<int64_t> & vecCompatibleInstances) const;

	// --------------------------------------------------------------------------------------------
	// Getter
	void GetWorldDimensions(float & fXmin, float & fXmax, float & fYmin, float & fYmax, float & fZmin, float & fZmax) const;

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
	// Edit properties support
	void OnInstancePropertyEdited(CRDFInstance * pInstance, CRDFProperty * pProperty);

	// --------------------------------------------------------------------------------------------
	// Stores model
	void Save(const wchar_t * szPath);

	// --------------------------------------------------------------------------------------------
	// Loads a model
	void Load(const wchar_t * szPath);

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
	// Coordinate system & grid
	void CreateCoordinateSystem();

	// --------------------------------------------------------------------------------------------
	// Loads RDF hierarchy
	void LoadRDFModel();

	// --------------------------------------------------------------------------------------------
	// Loads RDF instances
	void LoadRDFInstances();

	// --------------------------------------------------------------------------------------------
	// Support for textures; https://github.com/mortennobel/OpenGL_3_2_Utils/blob/master/src/TextureLoader.cpp
	// UNUSED
	//unsigned char * LoadBMP(const char * imagepath, unsigned int& outWidth, unsigned int& outHeight, bool flipY);	

	// --------------------------------------------------------------------------------------------
	// Clean up
	void Clean();

	// --------------------------------------------------------------------------------------------
	void GetClassPropertyCardinalityRestrictionNested(int64_t iRDFClass, int64_t iRDFProperty, int64_t * pMinCard, int64_t * pMaxCard);
	
	// --------------------------------------------------------------------------------------------
	// Octants
	void BuildOctants(_octant* pOctant, vector<GeometricItem*>& vecOctantsGeometry);
};

