#pragma once

#include "citygml/citygml.h"
#include "citygml/citymodel.h"
#include "citygml/cityobject.h"
#include "citygml/geometry.h"
#include "citygml/implictgeometry.h"
#include "citygml/polygon.h"

#include <bitset>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <time.h>

using namespace std;

// ------------------------------------------------------------------------------------------------
class CCityGMLParser
{

private: // Members

	// --------------------------------------------------------------------------------------------
	int64_t m_iModel;

	// --------------------------------------------------------------------------------------------
	Model* m_pModel;

public: // Methods

	// --------------------------------------------------------------------------------------------
	CCityGMLParser(int64_t iModel);

	// --------------------------------------------------------------------------------------------
	virtual ~CCityGMLParser();

	// --------------------------------------------------------------------------------------------
	void Import(const wchar_t* szGMLFile);

private: // Methods

	// --------------------------------------------------------------------------------------------
	void ImportObject(const citygml::CityObject& object, vector<GeometricItem*>& vecObjects);

	// --------------------------------------------------------------------------------------------
	void ImportImplictGeometry(const citygml::ImplicitGeometry& implicitGeometry, vector<GeometricItem*>& vecTriangleSets);

	// --------------------------------------------------------------------------------------------
	void ImportGeometry(const citygml::Geometry& geometry, vector<GeometricItem*>& vecTriangleSets);
};

