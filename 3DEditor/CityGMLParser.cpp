#include "stdafx.h"
#include "CityGMLParser.h"

// ------------------------------------------------------------------------------------------------
CCityGMLParser::CCityGMLParser(int64_t iModel)
{
	m_iModel = iModel;
	m_pModel = m_iModel;
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ CCityGMLParser::~CCityGMLParser()
{
//	delete m_pModel;
}

// ------------------------------------------------------------------------------------------------
void CCityGMLParser::Import(const wchar_t* szGMLFile)
{
	std::ifstream file;
	file.open(szGMLFile, std::ifstream::in);

	citygml::ParserParams params;
	shared_ptr<const citygml::CityModel> city = citygml::load(file, params);
	
	auto pModelCollection = GEOM::Collection::CreateW(m_pModel, szGMLFile);

	vector<GEOM::GeometricItem> vecRootObjects;
	for (unsigned int iRootObject = 0; iRootObject < city->getNumRootCityObjects(); iRootObject++)
	{
		const citygml::CityObject& rootObject = city->getRootCityObject(iRootObject);
		ImportObject(rootObject, vecRootObjects);
	}

	pModelCollection.set_objects(vecRootObjects.data(), vecRootObjects.size());
}


// ------------------------------------------------------------------------------------------------
void CCityGMLParser::ImportObject(const citygml::CityObject& object, vector<GEOM::GeometricItem>& vecObjects)
{
	string strType = object.getTypeAsString();
	auto pObjectCollection = GEOM::Collection::Create(m_pModel, strType.c_str());
	vecObjects.push_back(pObjectCollection);

	vector<GEOM::GeometricItem> vecTriangleSets;
	for (unsigned int iGeometry = 0; iGeometry < object.getImplicitGeometryCount(); iGeometry++)
	{
		const citygml::ImplicitGeometry& implicitGeometry = object.getImplicitGeometry(iGeometry);
		ImportImplictGeometry(implicitGeometry, vecTriangleSets);
	}

	for (unsigned int iGeometry = 0; iGeometry < object.getGeometriesCount(); iGeometry++)
	{
		const citygml::Geometry& geometry = object.getGeometry(iGeometry);
		ImportGeometry(geometry, vecTriangleSets);
	}
	
	vector<GEOM::GeometricItem> vecChildObjects;
	for (unsigned int iChildObject = 0; iChildObject < object.getChildCityObjectsCount(); iChildObject++)
	{
		const citygml::CityObject& childObject = object.getChildCityObject(iChildObject);
		ImportObject(childObject, vecChildObjects);
	}

	auto pObjectGeometryCollection = GEOM::Collection::Create(m_pModel, "Geometry");
	pObjectGeometryCollection.set_objects(vecTriangleSets.data(), vecTriangleSets.size());

	auto pObjectChildsCollection = GEOM::Collection::Create(m_pModel, "Objects");
	pObjectChildsCollection.set_objects(vecChildObjects.data(), vecChildObjects.size());

	GEOM::GeometricItem coll[2] = { pObjectGeometryCollection, pObjectChildsCollection };
	pObjectCollection.set_objects(coll, 2);
}

// ------------------------------------------------------------------------------------------------
void CCityGMLParser::ImportImplictGeometry(const citygml::ImplicitGeometry& implicitGeometry, vector<GEOM::GeometricItem>& vecTriangleSets)
{
	for (unsigned int iGeometry = 0; iGeometry < implicitGeometry.getGeometriesCount(); iGeometry++)
	{
		const citygml::Geometry& geometry = implicitGeometry.getGeometry(iGeometry);
		ImportGeometry(geometry, vecTriangleSets);
	}
}

// ------------------------------------------------------------------------------------------------
void CCityGMLParser::ImportGeometry(const citygml::Geometry& geometry, vector<GEOM::GeometricItem>& vecTriangleSets)
{
	/*auto pAmbient = m_pModel->create<ColorComponent>();
	pAmbient->R = 0.;
	pAmbient->G = 0.;
	pAmbient->B = 1.;
	pAmbient->W = 1.;

	auto pColor = m_pModel->create<Color>();
	pColor->ambient = pAmbient;

	auto pMaterial = m_pModel->create<Material>();
	pMaterial->color = pColor;*/

	for (unsigned int iPolygon = 0; iPolygon < geometry.getPolygonsCount(); iPolygon++)
	{
		shared_ptr<const citygml::Polygon> polygon = geometry.getPolygon(iPolygon);
		const std::vector<TVec3d>& vecPolygonVertices = polygon->getVertices();
		const std::vector<unsigned int>& vecPolygonIndices = polygon->getIndices();

		vector<double> vecVertices;
		for (size_t iVertex = 0; iVertex < vecPolygonVertices.size(); iVertex++)
		{
			vecVertices.push_back(vecPolygonVertices[iVertex].x);
			vecVertices.push_back(vecPolygonVertices[iVertex].y);
			vecVertices.push_back(vecPolygonVertices[iVertex].z);
		}

		vector<int64_t> vecIndices;
		for (size_t iIndex = 0; iIndex < vecPolygonIndices.size(); iIndex++)
		{
			vecIndices.push_back(vecPolygonIndices[iIndex]);
		}

		auto pTriangleSet = GEOM::TriangleSet::Create(m_pModel, "Triangles");

		pTriangleSet.set_vertices(vecVertices.data(), vecVertices.size());
		pTriangleSet.set_indices(vecIndices.data(), vecIndices.size());

		shared_ptr<const citygml::Material> materail = polygon->getMaterialFor("", true);

		//pTriangleSet->material = pMaterial;

		vecTriangleSets.push_back(pTriangleSet);
	} // for (unsigned int iPolygon = ...

	// TODO
	/*for (unsigned int iLineString = 0; iLineString < geometry.getLineStringCount(); iLineString++)
	{
		shared_ptr<const citygml::LineString> lineString = geometry.getLineString(iLineString);
	}*/

	for (unsigned int iGeometry = 0; iGeometry < geometry.getGeometriesCount(); iGeometry++)
	{
		const citygml::Geometry& childGeometry = geometry.getGeometry(iGeometry);
		ImportGeometry(childGeometry, vecTriangleSets);
	}
}