#include "stdafx.h"

#include "RDFModel.h"
#include "Generic.h"

#include "E57Format.h"
using namespace e57;

#include "CityGMLParser.h"

#include "_dxp_parser.h"

#include <bitset>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <time.h>

using namespace std;

#ifdef _LINUX
#include <cfloat>
#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <cwchar>
#endif // _LINUX

// ------------------------------------------------------------------------------------------------
CRDFModel::CRDFModel()
	: m_iModel(0)
	, m_pCoordinateSystemModel(CreateModel())
	, m_mapRDFClasses()
	, m_mapRDFProperties()
	, m_mapRDFInstances()
	, m_iID(1)
	, m_fXmin(-1.f)
	, m_fXmax(1.f)
	, m_fYmin(-1.f)
	, m_fYmax(1.f)
	, m_fZmin(-1.f)
	, m_fZmax(1.f)
	, m_fBoundingSphereDiameter(1.f)
	, m_fXTranslation(0.f)
	, m_fYTranslation(0.f)
	, m_fZTranslation(0.f)
	, m_pDefaultTexture(nullptr)
	, m_pMeasurementsBuilder(nullptr)
	, m_pOctree(nullptr)
{
}

// ------------------------------------------------------------------------------------------------
CRDFModel::~CRDFModel()
{
	Clean();
	
	CloseModel(m_pCoordinateSystemModel);
//	delete m_pCoordinateSystemModel;
}

// ------------------------------------------------------------------------------------------------
int64_t CRDFModel::GetModel() const
{
	return m_iModel;
}

// ------------------------------------------------------------------------------------------------
int64_t CRDFModel::GetCoordinateSystemModel() const
{
	return m_pCoordinateSystemModel;
//	return m_pCoordinateSystemModel->getInstance();
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::CreateDefaultModel()
{
	Clean();
	
	m_iModel = OpenModelW(nullptr);
	ASSERT(m_iModel != 0);	

	int64_t	pModel = m_iModel;

	SetFormatSettings(m_iModel);

	LoadRDFModel();

	// Cube 1
	{
		auto pAmbient = GEOM::ColorComponent::Create(pModel);
		pAmbient.set_R(0.);
		pAmbient.set_G(1.);
		pAmbient.set_B(0.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(pModel);
		pColor.set_ambient(pAmbient);

		auto pTexture = GEOM::Texture::Create(pModel);
		pTexture.set_scalingX(1.);
		pTexture.set_scalingY(1.);
		vector<GEOM::Texture> vecTexures = { pTexture };

		auto pMaterial = GEOM::Material::Create(pModel);
		pMaterial.set_color(pColor);
		pMaterial.set_textures(&vecTexures[0], 1);

		auto pCube = GEOM::Cube::Create(pModel, "Cube 1");
		pCube.set_material(pMaterial);
		pCube.set_length(7.);
	}

	// Cone 1
	{
		auto pAmbient = GEOM::ColorComponent::Create(pModel);
		pAmbient.set_R(0.);
		pAmbient.set_G(0.);
		pAmbient.set_B(1.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(pModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(pModel);
		pMaterial.set_color(pColor);

		auto pCone = GEOM::Cone::Create(pModel, "Cone 1");
		pCone.set_material(pMaterial);
		pCone.set_radius(4.);
		pCone.set_height(12.);
		pCone.set_segmentationParts(36);
	}	

	// Cylinder 1
	{
		auto pAmbient = GEOM::ColorComponent::Create(pModel);
		pAmbient.set_R(1.);
		pAmbient.set_G(0.);
		pAmbient.set_B(0.);
		pAmbient.set_W(.5);

		auto pColor = GEOM::Color::Create(pModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(pModel);
		pMaterial.set_color(pColor);

		auto pCylinder = GEOM::Cylinder::Create(pModel, "Cylinder 1");
		pCylinder.set_material(pMaterial);
		pCylinder.set_radius(6.);
		pCylinder.set_length(6.);
		pCylinder.set_segmentationParts(36);
	}

	LoadRDFInstances();
}

// ------------------------------------------------------------------------------------------------
const map<int64_t, CRDFClass *> & CRDFModel::GetRDFClasses() const
{
	return m_mapRDFClasses;
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::GetClassAncestors(int64_t iClassInstance, vector<int64_t> & vecAncestors) const
{
	ASSERT(iClassInstance != 0);

	map<int64_t, CRDFClass *>::const_iterator itRDFClass = m_mapRDFClasses.find(iClassInstance);
	ASSERT(itRDFClass != m_mapRDFClasses.end());

	CRDFClass * pRDFClass = itRDFClass->second;

	const vector<int64_t> & vecParentClasses = pRDFClass->getParentClasses();
	if (vecParentClasses.empty())
	{
		return;
	}

	for (size_t iParentClass = 0; iParentClass < vecParentClasses.size(); iParentClass++)
	{
		vecAncestors.insert(vecAncestors.begin(), vecParentClasses[iParentClass]);

		GetClassAncestors(vecParentClasses[iParentClass], vecAncestors);
	}
}

// ------------------------------------------------------------------------------------------------
const map<int64_t, CRDFProperty *> & CRDFModel::GetRDFProperties()
{
	return m_mapRDFProperties;
}

// ------------------------------------------------------------------------------------------------
const map<int64_t, CRDFInstance *> & CRDFModel::GetRDFInstances() const
{
	return m_mapRDFInstances;
}
// ------------------------------------------------------------------------------------------------
CRDFInstance * CRDFModel::GetRDFInstanceByID(int64_t iID)
{
	ASSERT(iID != 0);

	map<int64_t, CRDFInstance *>::iterator itRDFInstances = m_mapRDFInstances.begin();
	for (; itRDFInstances != m_mapRDFInstances.end(); itRDFInstances++)
	{
		if (itRDFInstances->second->getID() == iID)
		{
			return itRDFInstances->second;
		}
	}

	ASSERT(false);

	return NULL;
}

CRDFInstance * CRDFModel::GetRDFInstanceByIInstance(int64_t iInstance)
{
	ASSERT(iInstance != 0);

	map<int64_t, CRDFInstance *>::iterator itRDFInstances = m_mapRDFInstances.begin();
	for (; itRDFInstances != m_mapRDFInstances.end(); itRDFInstances++)
	{
		if (itRDFInstances->first == iInstance)
		{
			return itRDFInstances->second;
		}
	}

	return NULL;
}

// ------------------------------------------------------------------------------------------------
CRDFInstance * CRDFModel::CreateNewInstance(int64_t iClassInstance)
{
	ASSERT(iClassInstance != 0);

	int64_t iInstance = CreateInstance(iClassInstance, NULL);
	ASSERT(iInstance != 0);

	CRDFInstance * pRDFInstance = new CRDFInstance(m_iID++, iInstance);

	pRDFInstance->CalculateMinMax(m_fXmin, m_fXmax, m_fYmin, m_fYmax, m_fZmin, m_fZmax);

	m_mapRDFInstances[iInstance] = pRDFInstance;

	return pRDFInstance;
}

// ------------------------------------------------------------------------------------------------
CRDFInstance* CRDFModel::AddNewInstance(int64_t pThing)
{
//	ASSERT(pThing != nullptr);

	CRDFInstance* pRDFInstance = new CRDFInstance(m_iID++, pThing);

	pRDFInstance->CalculateMinMax(m_fXmin, m_fXmax, m_fYmin, m_fYmax, m_fZmin, m_fZmax);

	m_mapRDFInstances[pThing] = pRDFInstance;

	return pRDFInstance;
}

// ------------------------------------------------------------------------------------------------
bool CRDFModel::DeleteInstance(CRDFInstance * pInstance)
{
	ASSERT(pInstance != NULL);

	bool bResult = RemoveInstance(pInstance->getInstance()) == 0 ? true : false;

	map<int64_t, CRDFInstance *>::iterator itRDFInstance = m_mapRDFInstances.find(pInstance->getInstance());
	ASSERT(itRDFInstance != m_mapRDFInstances.end());

	m_mapRDFInstances.erase(itRDFInstance);

	delete pInstance;

	return bResult;
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::AddMeasurements(CRDFInstance * pInstance)
{	
	CRDFMeasurementsBuilder * pMeasurementsBuilder = GetMeasurementsBuilder();
	ASSERT(pMeasurementsBuilder != NULL);

	/*
	* Create
	*/
	double	scaleOfInstance = pMeasurementsBuilder->DefineScaling(pInstance->getInstance());
	int64_t	rdfProperty = GetPropertiesByIterator(pInstance->GetModel(), 0);
	while (rdfProperty) {
		if (GetPropertyType(rdfProperty) == 5) {
			double	* values = nullptr;
			int64_t	card = 0;
			GetDatatypeProperty(pInstance->getInstance(), rdfProperty, (void**)&values, &card);
			if (card == 1) {
				CString	txt = L"";
				char	* propertyName = nullptr;
				GetNameOfProperty(rdfProperty, &propertyName);
				txt += propertyName;
				txt += L" = ";
				txt += std::to_wstring(values[0]).c_str();
				pMeasurementsBuilder->BuildMeasure(pInstance->getInstance(), rdfProperty, txt, scaleOfInstance);
			}
		}
		rdfProperty = GetPropertiesByIterator(pInstance->GetModel(), rdfProperty);
	}

	/*
	* Load the new instances
	*/
	int64_t iInstance = GetInstancesByIterator(m_iModel, 0);
	while (iInstance != 0)
	{
		map<int64_t, CRDFInstance*>::iterator itRDFInstances = m_mapRDFInstances.find(iInstance);
		if (itRDFInstances == m_mapRDFInstances.end())
		{
			CRDFInstance* pRDFInstance = new CRDFInstance(m_iID++, iInstance);
			if (pRDFInstance->isReferenced())
			{
				pRDFInstance->setEnable(false);
			}

			pRDFInstance->CalculateMinMax(m_fXmin, m_fXmax, m_fYmin, m_fYmax, m_fZmin, m_fZmax);

			m_mapRDFInstances[iInstance] = pRDFInstance;
		}		

		iInstance = GetInstancesByIterator(m_iModel, iInstance);
	} // while (iInstance != 0)
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::ImportModel(const wchar_t* szPath)
{
	/*
	* Import
	*/
	ImportModelW(m_iModel, szPath);

	/*
	* Load the new instances
	*/
	int64_t iInstance = GetInstancesByIterator(m_iModel, 0);
	while (iInstance != 0)
	{
		map<int64_t, CRDFInstance*>::iterator itRDFInstances = m_mapRDFInstances.find(iInstance);
		if (itRDFInstances == m_mapRDFInstances.end())
		{
			CRDFInstance* pRDFInstance = new CRDFInstance(m_iID++, iInstance);
			if (pRDFInstance->isReferenced())
			{
				pRDFInstance->setEnable(false);
			}

			pRDFInstance->CalculateMinMax(m_fXmin, m_fXmax, m_fYmin, m_fYmax, m_fZmin, m_fZmax);

			m_mapRDFInstances[iInstance] = pRDFInstance;
		}

		iInstance = GetInstancesByIterator(m_iModel, iInstance);
	} // while (iInstance != 0)
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::GetCompatibleInstances(CRDFInstance * pRDFInstance, CObjectRDFProperty * pObjectRDFProperty, vector<int64_t> & vecCompatibleInstances) const
{
	ASSERT(pRDFInstance != NULL);
	ASSERT(pObjectRDFProperty != NULL);

	int64_t iClassInstance = GetInstanceClass(pRDFInstance->getInstance());
	ASSERT(iClassInstance != 0);

	const vector<int64_t> & vecRestrictions = pObjectRDFProperty->getRestrictions();
	ASSERT(!vecRestrictions.empty());

	const map<int64_t, CRDFInstance *> & mapRFDInstances = GetRDFInstances();

	map<int64_t, CRDFInstance *>::const_iterator itRFDInstances = mapRFDInstances.begin();
	for (; itRFDInstances != mapRFDInstances.end(); itRFDInstances++)
	{
		/*
		* Skip this instance
		*/
		if (itRFDInstances->second == pRDFInstance)
		{
			continue;
		}

		/*
		* Skip the instances that belong to a different model
		*/
		if (itRFDInstances->second->GetModel() != pRDFInstance->GetModel())
		{
			continue;
		}

		/*
		* Check this instance
		*/
		if (std::find(vecRestrictions.begin(), vecRestrictions.end(), itRFDInstances->second->getClassInstance()) != vecRestrictions.end())
		{
			vecCompatibleInstances.push_back(itRFDInstances->second->getInstance());

			continue;
		}

		/*
		* Check the ancestor classes
		*/

		vector<int64_t> vecAncestorClasses;
		CRDFClass::GetAncestors(iClassInstance, vecAncestorClasses);

		if (vecAncestorClasses.empty())
		{
			continue;
		}

		for (size_t iAncestorClass = 0; iAncestorClass < vecAncestorClasses.size(); iAncestorClass++)
		{
			if (find(vecRestrictions.begin(), vecRestrictions.end(), vecAncestorClasses[iAncestorClass]) != vecRestrictions.end())
			{
				vecCompatibleInstances.push_back(itRFDInstances->second->getInstance());

				break;
			}
		} // for (size_t iAncestorClass = ...
	} // for (; itRFDInstances != ...
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::GetWorldDimensions(float & fXmin, float & fXmax, float & fYmin, float & fYmax, float & fZmin, float & fZmax) const
{
	fXmin = m_fXmin;
	fXmax = m_fXmax;
	fYmin = m_fYmin;
	fYmax = m_fYmax;
	fZmin = m_fZmin;
	fZmax = m_fZmax;
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::GetWorldTranslations(float& fXTranslation, float& fYTranslation, float& fZTranslation) const
{
	fXTranslation = m_fXTranslation;
	fYTranslation = m_fYTranslation;
	fZTranslation = m_fZTranslation;
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::ScaleAndCenter()
{
	m_fBoundingSphereDiameter = 0.f;

	m_fXTranslation = 0.f;
	m_fYTranslation = 0.f;
	m_fZTranslation = 0.f;

	m_fXmin = FLT_MAX;
	m_fXmax = -FLT_MAX;
	m_fYmin = FLT_MAX;
	m_fYmax = -FLT_MAX;
	m_fZmin = FLT_MAX;
	m_fZmax = -FLT_MAX;

	map<int64_t, CRDFInstance*>::iterator itRDFInstances = m_mapRDFInstances.begin();
	for (; itRDFInstances != m_mapRDFInstances.end(); itRDFInstances++)
	{
		itRDFInstances->second->CalculateMinMax(m_fXmin, m_fXmax, m_fYmin, m_fYmax, m_fZmin, m_fZmax);
	}

	m_fBoundingSphereDiameter = m_fXmax - m_fXmin;
	m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fYmax - m_fYmin);
	m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fZmax - m_fZmin);

#ifndef _LINUX
//	LOG_DEBUG("X/Y/Z min: " << m_fXmin << ", " << m_fYmin << ", " << m_fZmin);
//	LOG_DEBUG("X/Y/Z max: " << m_fXmax << ", " << m_fYmax << ", " << m_fZmax);
//	LOG_DEBUG("World's bounding sphere diameter: " << m_fBoundingSphereDiameter);
#endif // _LINUX

	itRDFInstances = m_mapRDFInstances.begin();
	for (; itRDFInstances != m_mapRDFInstances.end(); itRDFInstances++)
	{
		itRDFInstances->second->ScaleAndCenter(m_fXmin, m_fXmax, m_fYmin, m_fYmax, m_fZmin, m_fZmax, m_fBoundingSphereDiameter);
	}

	/*
	* Min/Max
	*/

	m_fXmin = FLT_MAX;
	m_fXmax = -FLT_MAX;
	m_fYmin = FLT_MAX;
	m_fYmax = -FLT_MAX;
	m_fZmin = FLT_MAX;
	m_fZmax = -FLT_MAX;

	itRDFInstances = m_mapRDFInstances.begin();
	for (; itRDFInstances != m_mapRDFInstances.end(); itRDFInstances++)
	{
		itRDFInstances->second->CalculateMinMax(m_fXmin, m_fXmax, m_fYmin, m_fYmax, m_fZmin, m_fZmax);
	}

	/*
	* World
	*/
	m_fBoundingSphereDiameter = m_fXmax - m_fXmin;
	m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fYmax - m_fYmin);
	m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fZmax - m_fZmin);

	/*
	* Translations
	*/

	// [0.0 -> X/Y/Zmin + X/Y/Zmax]
	m_fXTranslation -= m_fXmin;
	m_fYTranslation -= m_fYmin;
	m_fZTranslation -= m_fZmin;

	// center
	m_fXTranslation -= ((m_fXmax - m_fXmin) / 2.0f);
	m_fYTranslation -= ((m_fYmax - m_fYmin) / 2.0f);
	m_fZTranslation -= ((m_fZmax - m_fZmin) / 2.0f);

	// [-1.0 -> 1.0]
	m_fXTranslation /= (m_fBoundingSphereDiameter / 2.0f);
	m_fYTranslation /= (m_fBoundingSphereDiameter / 2.0f);
	m_fZTranslation /= (m_fBoundingSphereDiameter / 2.0f);
}

// ------------------------------------------------------------------------------------------------
float CRDFModel::GetBoundingSphereDiameter() const
{
	return m_fBoundingSphereDiameter;
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::ZoomToInstance(int64_t iInstance)
{
	ASSERT(iInstance != 0);
	ASSERT(m_mapRDFInstances.find(iInstance) != m_mapRDFInstances.end());

	m_fXmin = FLT_MAX;
	m_fXmax = -FLT_MAX;
	m_fYmin = FLT_MAX;
	m_fYmax = -FLT_MAX;
	m_fZmin = FLT_MAX;
	m_fZmax = -FLT_MAX;

	m_mapRDFInstances[iInstance]->CalculateMinMax(m_fXmin, m_fXmax, m_fYmin, m_fYmax, m_fZmin, m_fZmax);

	m_fBoundingSphereDiameter = m_fXmax - m_fXmin;
	m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fYmax - m_fYmin);
	m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fZmax - m_fZmin);

#ifndef _LINUX
//	LOG_DEBUG("X/Y/Z min: " << m_fXmin << ", " << m_fYmin << ", " << m_fZmin);
//	LOG_DEBUG("X/Y/Z max: " << m_fXmax << ", " << m_fYmax << ", " << m_fZmax);
//	LOG_DEBUG("World's bounding sphere diameter: " << m_fBoundingSphereDiameter);
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::OnInstancePropertyEdited(CRDFInstance * /*pInstance*/, CRDFProperty * /*pProperty*/)
{
	SetFormatSettings(m_iModel);

	map<int64_t, CRDFInstance *>::iterator itRDFInstances = m_mapRDFInstances.begin();
	for (; itRDFInstances != m_mapRDFInstances.end(); itRDFInstances++)
	{
		if (itRDFInstances->second->GetModel() != m_iModel)
		{
			continue;
		}

		itRDFInstances->second->Recalculate();
	}
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::Save(const wchar_t * szPath)
{
	SaveModelW(m_iModel, szPath);
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::Load(const wchar_t * szPath)
{
	Clean();

	///////////////////////////////////////////////////////////////////////////////////////////////
	// E57 TEST
	CString strExtension = PathFindExtension(szPath);
	strExtension.MakeUpper();

	if (strExtension == L".E57")
	{
		m_iModel = OpenModelW(nullptr);
		ASSERT(m_iModel != 0);

		SetFormatSettings(m_iModel);

		LoadRDFModel();

		try {
			/// Read file from disk
			ImageFile imf(ustring(CW2A(szPath)), "r");
			StructureNode root = imf.root();

			/// Make sure vector of scans is defined and of expected type.
			/// If "/data3D" wasn't defined, the call to root.get below would raise an exception.
			if (!root.isDefined("/data3D")) {
				::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"File doesn't contain 3D images.", L"Error", MB_ICONERROR | MB_OK);				

				return;
			}
			Node n = root.get("/data3D");
			if (n.type() != E57_VECTOR) {
				::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Invalid format.", L"Error", MB_ICONERROR | MB_OK);

				return;
			}

			//ofstream dump("dump.txt");
			//imf.dump(0, dump);

			/// The node is a vector so we can safely get a VectorNode handle to it.
			/// If n was not a VectorNode, this would raise an exception.
			VectorNode data3D(n);

			/// Print number of children of data3D.  This is the number of scans in file.
			int64_t scanCount = data3D.childCount();
			TRACE(L"\nNumber of scans in file: %d", scanCount);

			if (scanCount == 1)
			{
				/// For each scan, print out first N points in either Cartesian or Spherical coordinates.
				for (int scanIndex = 0; scanIndex < scanCount; scanIndex++) 
				{
					/// Get scan from "/data3D", assume its a Structure (else get exception)
					StructureNode scan(data3D.get(scanIndex));
					TRACE(L"\nGot: %s", CA2W(scan.pathName().c_str()).m_psz);

					if (scan.isDefined("pointGroupingSchemes"))
					{
						::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Point Grouping Schemes are not supported.", L"Error", MB_ICONERROR | MB_OK);

						//StructureNode pointGroupingSchemes(scan.get("pointGroupingSchemes"));
						//if (pointGroupingSchemes.isDefined("groupingByLine"))
						//{
						//	StructureNode groupingByLine(pointGroupingSchemes.get("groupingByLine"));

						//	StringNode	idElementName(groupingByLine.get("idElementName"));
						//	CompressedVectorNode groups(groupingByLine.get("groups"));
						//	StructureNode lineGroupRecord(groups.prototype()); //not used here

						//	int64_t protoCount = lineGroupRecord.childCount();
						//	int64_t protoIndex;
						//	vector<SourceDestBuffer> groupSDBuffers;
						//}
					} // if (scan.isDefined("pointGroupingSchemes"))
					else
					{
						/*
						* X/Y/Z min/max
						*/
						double xMinimum = -1.;
						double xMaximum = 1.;
						double yMinimum = -1.;
						double yMaximum = 1.;
						double zMinimum = -1.;
						double zMaximum = 1.;

						// Get Cartesian bounding box to scan.
						if (scan.isDefined("cartesianBounds"))
						{
							StructureNode bbox(scan.get("cartesianBounds"));
							if (bbox.get("xMinimum").type() == E57_SCALED_INTEGER) 
							{
								xMinimum = (double)ScaledIntegerNode(bbox.get("xMinimum")).scaledValue();
								xMaximum = (double)ScaledIntegerNode(bbox.get("xMaximum")).scaledValue();
								yMinimum = (double)ScaledIntegerNode(bbox.get("yMinimum")).scaledValue();
								yMaximum = (double)ScaledIntegerNode(bbox.get("yMaximum")).scaledValue();
								zMinimum = (double)ScaledIntegerNode(bbox.get("zMinimum")).scaledValue();
								zMaximum = (double)ScaledIntegerNode(bbox.get("zMaximum")).scaledValue();
							}
							else if (bbox.get("xMinimum").type() == E57_FLOAT) 
							{
								xMinimum = FloatNode(bbox.get("xMinimum")).value();
								xMaximum = FloatNode(bbox.get("xMaximum")).value();
								yMinimum = FloatNode(bbox.get("yMinimum")).value();
								yMaximum = FloatNode(bbox.get("yMaximum")).value();
								zMinimum = FloatNode(bbox.get("zMinimum")).value();
								zMaximum = FloatNode(bbox.get("zMaximum")).value();
							}
						}
						else
						{
							::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"The bounds are not detected.", L"Error", MB_ICONERROR | MB_OK);

							return;
						}

						/*
						* Cube octree
						*/
						double dMaxDimesion = xMaximum - xMinimum;
						dMaxDimesion = max(dMaxDimesion, yMaximum - yMinimum);
						dMaxDimesion = max(dMaxDimesion, zMaximum - zMinimum);

						if ((xMaximum - xMinimum) < dMaxDimesion)
						{
							xMaximum += dMaxDimesion - (xMaximum - xMinimum);
						}

						if ((yMaximum - yMinimum) < dMaxDimesion)
						{
							yMaximum += dMaxDimesion - (yMaximum - yMinimum);
						}

						if ((zMaximum - zMinimum) < dMaxDimesion)
						{
							zMaximum += dMaxDimesion - (zMaximum - zMinimum);
						}

						ASSERT(m_pOctree == nullptr);
						m_pOctree = new _octree(m_iModel, xMinimum, xMaximum, yMinimum, yMaximum, zMinimum, zMaximum);

						/// Get "points" field in scan.  Should be a CompressedVectorNode.
						CompressedVectorNode points(scan.get("points"));
						TRACE(L"\nGot: %s", CA2W(points.pathName().c_str()).m_psz);

						/// Need to figure out if has Cartesian or spherical coordinate system.
						/// Interrogate the CompressedVector's prototype of its records.
						StructureNode proto(points.prototype());

						if (proto.isDefined("cartesianX") && proto.isDefined("cartesianY") && proto.isDefined("cartesianZ"))
						{
							auto type = proto.get("cartesianX").type();
							bool bScaled = type == E57_SCALED_INTEGER;

							/// Make a list of buffers to receive the xyz values.
							const int N = 10240;
							vector<SourceDestBuffer> destBuffers;
							double x[N];     destBuffers.push_back(SourceDestBuffer(imf, "cartesianX", x, N, true, bScaled));
							double y[N];     destBuffers.push_back(SourceDestBuffer(imf, "cartesianY", y, N, true, bScaled));
							double z[N];     destBuffers.push_back(SourceDestBuffer(imf, "cartesianZ", z, N, true, bScaled));

							// TODO
							/*int16_t intensities[N];
							if (proto.isDefined("intensity"))
							{
								destBuffers.push_back(SourceDestBuffer(imf, "intensity", intensities, N, true));
							}*/

							double red[N];
							if (proto.isDefined("colorRed"))
							{
								destBuffers.push_back(SourceDestBuffer(imf, "colorRed", red, N, true));
							}

							double green[N];
							if (proto.isDefined("colorGreen"))
							{
								destBuffers.push_back(SourceDestBuffer(imf, "colorGreen", green, N, true));
							}

							double blue[N];
							if (proto.isDefined("colorBlue"))
							{
								destBuffers.push_back(SourceDestBuffer(imf, "colorBlue", blue, N, true));
							}

							/// Create a reader of the points CompressedVector, try to read first block of N points
							/// Each call to reader.read() will fill the xyz buffers until the points are exhausted.
							CompressedVectorReader reader = points.reader(destBuffers);
							unsigned gotCount = 0;
							while ((gotCount = reader.read()) > 0)
							{
								TRACE(L"*** Point read: %d ***", gotCount);

								//vector<double> vecCoordinates;
								vector<int16_t> vecIntensities;
								vector<double> vecR;
								vector<double> vecG;
								vector<double> vecB;
								for (unsigned i = 0; i < gotCount; i++)
								{
									if (!m_pOctree->insertPoint(x[i], y[i], z[i]))
									{
										continue;
									}

									/*vecCoordinates.push_back(x[i]);
									vecCoordinates.push_back(y[i]);
									vecCoordinates.push_back(z[i]);*/

									// ScaledInteger - see scale
									/*if (proto.isDefined("intensity"))
									{
										vecIntensities.push_back(intensities[i]);
									}*/

									if (proto.isDefined("colorRed"))
									{
										vecR.push_back(red[i]);
									}

									if (proto.isDefined("colorGreen"))
									{
										vecG.push_back(green[i]);
									}

									if (proto.isDefined("colorBlue"))
									{
										vecB.push_back(blue[i]);
									}									
								}

								//auto pPoint3DSet = m_pModel->create<Point3DSet>();
								//pPoint3DSet->coordinates = vecCoordinates;

								//auto pLine3DSet = m_pModel->create<Line3DSet>();
								//pLine3DSet->points = vecCoordinates;

								/*auto pMesh = m_pModel->create<Mesh>();
								pMesh->pointSet = pPoint3DSet;

								auto pAmbient = m_pModel->create<ColorComponent>();
								pAmbient->R = 0.;
								pAmbient->G = 0.;
								pAmbient->B = 1.;
								pAmbient->W = 1.;

								auto pColor = m_pModel->create<Color>();
								pColor->ambient = pAmbient;

								auto pMaterial = m_pModel->create<Material>();
								pMaterial->color = pColor;

								pMesh->material = pMaterial;*/
							} // while ((gotCount = reader.read()) > 0)

							reader.close();
						} // if (proto.isDefined("cartesianX") && ...
						else if (proto.isDefined("sphericalRange"))
						{
							::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Spherical coordinate system is not supported.", L"Error", MB_ICONERROR | MB_OK);
						}
					} // else if (scan.isDefined("pointGroupingSchemes"))
				} // for (int scanIndex = ...
			} // if (scanCount == 1)
			else
			{
				::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Multiple scans are not supported.", L"Error", MB_ICONERROR | MB_OK);
			}

			imf.close();

			/*
			* Statistics
			*/
			m_pOctree->dump();

			/*
			* Octree 1
			*/
			/*vector<GeometricItem*> vecOctantsGeometry;
			BuildOctants(m_pOctree, vecOctantsGeometry);

			auto pCollection = m_pModel->create<Collection>();
			pCollection->objects = vecOctantsGeometry;*/

			/*
			* Octree 2
			*/
#ifndef _DEBUG_OCTREE
			m_pOctree->buildMesh();
#endif // _DEBUG_OCTREE
		}
		catch (E57Exception& ex) {
			::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), CA2W(ex.what()), L"Error", MB_ICONERROR | MB_OK);
			
			return;
		}
		catch (std::exception& ex) {
			::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), CA2W(ex.what()), L"Error", MB_ICONERROR | MB_OK);

			return;
		}
		catch (...) {
			::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Unknown error.", L"Error", MB_ICONERROR | MB_OK);
			
			return;
		}

		LoadRDFInstances();

		return;
	} // if (strExtension == L"E57")
	// E57 TEST
	///////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////
	// CityGML TEST
	if ((strExtension == L".GML") || (strExtension == L".CITYGML"))
	{
		m_iModel = OpenModelW(nullptr);
		ASSERT(m_iModel != 0);

		SetFormatSettings(m_iModel);

		LoadRDFModel();

		try
		{
			CCityGMLParser cityGMLParser(m_iModel);
			cityGMLParser.Import(szPath);			
		}
		catch (const std::runtime_error& ex)
		{
			::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), CA2W(ex.what()), L"Error", MB_ICONERROR | MB_OK);

			return;
		}		

		LoadRDFInstances();

		return;
	} // if ((strExtension == L".GML") || ...
	// CityGML TEST
	///////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Autocad DXF TEST
	if (strExtension == L".DXF")
	{
		m_iModel = OpenModelW(nullptr);
		ASSERT(m_iModel != 0);

		SetFormatSettings(m_iModel);

		LoadRDFModel();

		try
		{
			_dxf::_parser parser(m_iModel);
			parser.load(szPath);
		}
		catch (const std::runtime_error& ex)
		{
			::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), CA2W(ex.what()), L"Error", MB_ICONERROR | MB_OK);

			return;
		}

		LoadRDFInstances();

		return;
	} // if (strExtension == L".DXF")
	///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _LINUX
//	LOG_DEBUG("OpenModelW() BEGIN");
#endif // _LINUX

#ifdef _LINUX
    wxString strPath(szPath);

	m_iModel = OpenModel(strPath.char_str());
#else    
	m_iModel = OpenModelW(szPath);
#endif // _LINUX

	ASSERT(m_iModel != 0);

#ifndef _LINUX
//	LOG_DEBUG("OpenModelW() END");
#endif // _LINUX

	SetFormatSettings(m_iModel);

	LoadRDFModel();

	LoadRDFInstances();
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::PostLoad()
{
#ifdef _DEBUG_OCTREE
	m_pOctree->buildMesh();
#endif // _DEBUG_OCTREE
}

// ------------------------------------------------------------------------------------------------
CTexture * CRDFModel::GetDefaultTexture()
{
	if (m_pDefaultTexture == NULL)
	{
#ifdef _LINUX
        wxStandardPaths standardPaths = wxStandardPaths::Get();
        wxString strAppPath = standardPaths.GetExecutablePath();

        size_t iPosition = strAppPath.rfind("/");
        strAppPath.replace(iPosition + 1, wcslen(L"RDGViewerNX"), L"");

        string strDefaultTexture = string(strAppPath.mbc_str());
        strDefaultTexture += "texture.bmp";

        const char * szDefaultTexture = strDefaultTexture.c_str();
#else
        wchar_t szAppPath[_MAX_PATH];
		::GetModuleFileName(::GetModuleHandle(NULL), szAppPath, sizeof(szAppPath));

		CString strDefaultTexture = szAppPath;
		strDefaultTexture.MakeLower();

		int iLastSlash = strDefaultTexture.ReverseFind(L'\\');
		ASSERT(iLastSlash != -1);

		strDefaultTexture = strDefaultTexture.Left(iLastSlash + 1);
		strDefaultTexture += L"texture.bmp";

		LPCTSTR szDefaultTexture = (LPCTSTR)strDefaultTexture;
#endif // _LINUX

		m_pDefaultTexture = new CTexture();
		if (!m_pDefaultTexture->LoadFile(szDefaultTexture))
		{
#ifdef _LINUX
            wxLogError(wxT("The default texture is not found."));
#else
            MessageBox(NULL, L"The default texture is not found.", L"Error", MB_ICONERROR | MB_OK);
#endif // _LINUX
		}
	} // if (m_pDefaultTexture == NULL)

	return m_pDefaultTexture;
}

// ------------------------------------------------------------------------------------------------
CRDFMeasurementsBuilder * CRDFModel::GetMeasurementsBuilder()
{
	if (m_pMeasurementsBuilder == NULL)
	{
#ifdef _LINUX
		wxStandardPaths standardPaths = wxStandardPaths::Get();
		wxString strAppPath = standardPaths.GetExecutablePath();

		size_t iPosition = strAppPath.rfind("/");
		strAppPath.replace(iPosition + 1, wcslen(L"RDGViewerNX"), L"");

		string strDefaultFont = string(strAppPath.mbc_str());
		strDefaultFont += "OpenSans-Regular.ttf";

		const char * szDefaultTexture = strDefaultFont.c_str();
#else
		wchar_t szAppPath[_MAX_PATH];
		::GetModuleFileName(::GetModuleHandle(NULL), szAppPath, sizeof(szAppPath));

		CString strDefaultFont = szAppPath;
		strDefaultFont.MakeLower();

		int iLastSlash = strDefaultFont.ReverseFind(L'\\');
		ASSERT(iLastSlash != -1);

		strDefaultFont = strDefaultFont.Left(iLastSlash + 1);
		strDefaultFont += L"OpenSans-Regular.ttf";

		LPCTSTR szDefaultFont = (LPCTSTR)strDefaultFont;
#endif // _LINUX

		m_pMeasurementsBuilder = new CRDFMeasurementsBuilder(szDefaultFont, m_iModel, EXTRSUSION_AREA_SOLID_SET);
	} // if (m_pMeasurementsBuilder == NULL)

	return m_pMeasurementsBuilder;
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::SetFormatSettings(int64_t iModel)
{
	// Model
//	int64_t pModel = iModel;

	string strSettings = "111111000000001011000001110001";

	bitset<64> bitSettings(strSettings);
	int64_t iSettings = bitSettings.to_ulong();

	string strMask = "11111111111111111011011101110111";
	bitset <64> bitMask(strMask);
	int64_t iMask = bitMask.to_ulong();

	SetFormat(iModel, (int64_t)iSettings, (int64_t)iMask);

/*	pModel->format->indexArrayContainsTriangles = true;
	pModel->format->indexArrayContainsConceptualFacePolygons = true;
	pModel->format->indexArrayContainsFacePolygons = true;
	pModel->format->indexArrayContainsLines = true;
	pModel->format->indexArrayContainsPoints = true;

	pModel->format->vertexArrayContainsUVTextureI = true;

	pModel->format->vertexArrayContainsNormals = true;
	pModel->format->vertexArrayContainsTangentTextureI = true;
	pModel->format->vertexArrayContainsBinormalTextureI = true;
	
	pModel->format->vertexArrayContainsAmbientColor = true;
	pModel->format->vertexArrayContainsDiffuseColor = true;
	pModel->format->vertexArrayContainsEmissiveColor = true;
	pModel->format->vertexArrayContainsSpecularColor = true;	//	*/

	// X, Y, Z, Nx, Ny, Nz, Tx, Ty, Ambient, Diffuse, Emissive, Specular, Tnx, Tny, Tnz, Bnx, Bny, Bnz
	// (Tx, Ty - bit 6; Normal vectors - bit 5, Diffuse, Emissive, Specular - bit 25, 26 & 27, Tangent vectors - bit 28, Binormal vectors - bit 29)
	/*string strSettings = "111111000000001011000001110001";

	bitset<64> bitSettings(strSettings);
	int64_t iSettings = bitSettings.to_ulong();

	string strMask = "11111111111111111011011101110111";
	bitset <64> bitMask(strMask);
	int64_t iMask = bitMask.to_ulong();

	int64_t iVertexLength = SetFormat(iModel, (int64_t)iSettings, (int64_t)iMask);

#ifndef _USE_BOOST
	TRACE("SetFormat(): Settings = %s, Mask = %s, Vertex length: %d", strSettings.c_str(), strMask.c_str(), iVertexLength);
#endif

#ifndef _LINUX
    LOG_DEBUG("SetFormat(): Settings = " << strSettings << ", Mask = " << strMask << ", Vertex length: " << iVertexLength);
#endif // _LINUX
	*/
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::CreateCoordinateSystem()
{
//	m_pCoordinateSystemModel->open(nullptr);
//	ASSERT(m_pCoordinateSystemModel->getInstance() != 0);

	int64_t	pModel = m_pCoordinateSystemModel;
	SetFormatSettings(m_pCoordinateSystemModel);

	auto instance_arrow_X = GEOM::Transformation::Create(pModel);
	auto instance_arrow_X_matrix = GEOM::Matrix::Create(pModel);
	auto instance_arrow_X_collection = GEOM::Collection::Create(pModel);
	auto instanceI = GEOM::Transformation::Create(pModel);
	auto instanceI_matrix = GEOM::Matrix::Create(pModel);
	auto instanceII = GEOM::Collection::Create(pModel);
	auto instanceIII = GEOM::ExtrudedPolygon::Create(pModel);
	auto instanceIV = GEOM::ExtrudedPolygon::Create(pModel);
	auto instanceV = GEOM::Transformation::Create(pModel);
	auto instanceV_matrix = GEOM::Matrix::Create(pModel);
	auto instanceXScaleCenter = GEOM::Transformation::Create(pModel);
	auto instanceXScaleCenter_matrix = GEOM::Matrix::Create(pModel);
	auto instanceYScaleCenter = GEOM::Transformation::Create(pModel);
	auto instanceYScaleCenter_matrix = GEOM::Matrix::Create(pModel);
	auto instanceZScaleCenter = GEOM::Transformation::Create(pModel);
	auto instanceZScaleCenter_matrix = GEOM::Matrix::Create(pModel);
	auto instanceVI = GEOM::Cone::Create(pModel);
	auto instanceVII = GEOM::Cylinder::Create(pModel);
	
	auto instance_arrow_Y = GEOM::Transformation::Create(pModel);
	auto instance_arrow_Y_matrix = GEOM::Matrix::Create(pModel);
	auto instance_arrow_Y_collection = GEOM::Collection::Create(pModel);
	auto instanceVIII = GEOM::Transformation::Create(pModel);
	auto instanceVIII_matrix = GEOM::Matrix::Create(pModel);
	auto instanceIX = GEOM::Collection::Create(pModel);
	auto instanceX = GEOM::ExtrudedPolygon::Create(pModel);
	
	auto instance_arrow_Z_collection = GEOM::Collection::Create(pModel);
	auto instanceXI = GEOM::Transformation::Create(pModel);
	auto instanceXI_matrix = GEOM::Matrix::Create(pModel);
	auto instanceXII = GEOM::ExtrudedPolygon::Create(pModel);
	
	auto instance_arrows = GEOM::Collection::Create(pModel);
	auto instance_lines = GEOM::Collection::Create(pModel);

	vector<GEOM::GeometricItem> instanceLines;

	double	xMin = m_fXmin - (m_fXmax - m_fXmin) * .1,
			xMax = m_fXmax + (m_fXmax - m_fXmin) * .1,
			yMin = m_fYmin - (m_fYmax - m_fYmin) * .1,
			yMax = m_fYmax + (m_fYmax - m_fYmin) * .1;

	for (int_t i = 0; i < 9; i++) {
		auto instanceLine = GEOM::Line3D::Create(pModel);
		vector<double> points = {
										xMin - (m_fXmax - m_fXmin) * .1,
										yMin + ((yMax - yMin) / 8.f) * i,
										m_fZmin + (m_fZmax - m_fZmin) / 2.,
										xMax + (m_fXmax - m_fXmin) * .1,
										yMin + ((yMax - yMin) / 8.) * i,
										m_fZmin + (m_fZmax - m_fZmin) / 2.
									};
		instanceLine.set_points(&points[0], points.size());
		 
		instanceLines.push_back(instanceLine);
	}

	for (int_t i = 0; i < 9; i++) {
		auto instanceLine = GEOM::Line3D::Create(pModel);
		vector<double> points = {
										xMin + ((xMax - xMin) / 8.) * i,
										yMin - (m_fYmax - m_fYmin) * .1,
										m_fZmin + (m_fZmax - m_fZmin) / 2.,
										xMin + ((xMax - xMin) / 8.) * i,
										yMax + (m_fYmax - m_fYmin) * .1,
										m_fZmin + (m_fZmax - m_fZmin) / 2.
									};
		instanceLine.set_points(&points[0], points.size());

		instanceLines.push_back(instanceLine);
	}

	//
	//	Line Collection
	//
	instance_lines.set_objects(&instanceLines[0], instanceLines.size());

	//
	//	Arrow Collection
	//
	{
		vector<GEOM::GeometricItem> objects = { instanceXScaleCenter, instanceYScaleCenter, instanceZScaleCenter };
		instance_arrows.set_objects(&objects[0], objects.size());
	}

	//
	//	X complete
	//
	instance_arrow_X.set_object(instance_arrow_X_collection);
	instance_arrow_X.set_matrix(instance_arrow_X_matrix);
	instance_arrow_X_matrix.set__11(0.);
	instance_arrow_X_matrix.set__12(1.);
	instance_arrow_X_matrix.set__22(0.);
	instance_arrow_X_matrix.set__23(1.);
	instance_arrow_X_matrix.set__31(1.);
	instance_arrow_X_matrix.set__33(0.);

	//
	//	Scale and center
	//
	instanceXScaleCenter.set_object(instance_arrow_X);
	instanceXScaleCenter.set_matrix(instanceXScaleCenter_matrix);
	instanceXScaleCenter_matrix.set__11(m_fBoundingSphereDiameter / 4.);
	instanceXScaleCenter_matrix.set__22(m_fBoundingSphereDiameter / 4.);
	instanceXScaleCenter_matrix.set__33(m_fBoundingSphereDiameter / 4.);
	instanceXScaleCenter_matrix.set__41(m_fXmin + (m_fXmax - m_fXmin) / 2.);
	instanceXScaleCenter_matrix.set__42(m_fYmin + (m_fYmax - m_fYmin) / 2.);
	instanceXScaleCenter_matrix.set__43(m_fZmin + (m_fZmax - m_fZmin) / 2.);

	//
	//	X Collection
	//
	{
		vector<GEOM::GeometricItem> objects = { instanceI, instanceV, instanceVII };
		instance_arrow_X_collection.set_objects(&objects[0], objects.size());
	}

	//
	//	Transformation letter
	//
	instanceI.set_object(instanceII);
	instanceI.set_matrix(instanceI_matrix);
	instanceI_matrix.set__11(0);
	instanceI_matrix.set__13(.6);
	instanceI_matrix.set__21(.6);
	instanceI_matrix.set__22(0);
	instanceI_matrix.set__32(1);
	instanceI_matrix.set__33(0);
	instanceI_matrix.set__41(-1.);
	instanceI_matrix.set__42(0);
	instanceI_matrix.set__43(4.3);

	//
	//	Inside Collection
	//
	{
		vector<GEOM::GeometricItem> objects = { instanceIII, instanceIV };
		instanceII.set_objects(&objects[0], objects.size());
	}

	//	//
	//	//	First Extruded Polygon
	//	//
	instanceIII.set_extrusionLength(0.02);
	{
		vector<double> points = { 0., 0., 0.15, 0., 1., 1.2, .85, 1.2 };
		instanceIII.set_points(&points[0], points.size());
	}

	//	//
	//	//	Second Extruded Polygon
	//	//
	instanceIV.set_extrusionLength(0.02);
	{
		vector<double> points = { 0., 1.2, .15, 1.2, 1., 0., .85, 0. };
		instanceIV.set_points(&points[0], points.size());
	}

	//
	//	Transformation arrow end
	//
	instanceV.set_object(instanceVI);
	instanceV.set_matrix(instanceV_matrix);
	instanceV_matrix.set__43(3.2);

	//
	//	Arrow end
	//
	instanceVI.set_radius(.3);
	instanceVI.set_height(.8);
	instanceVI.set_segmentationParts(36);

	//
	//	Arrow line
	//
	instanceVII.set_length(3.);
	instanceVII.set_radius(.06);
	instanceVII.set_segmentationParts(24);

	//
	//	Y complete
	//
	instance_arrow_Y.set_object(instance_arrow_Y_collection);
	instance_arrow_Y.set_matrix(instance_arrow_Y_matrix);
	instance_arrow_Y_matrix.set__11(-1);
	instance_arrow_Y_matrix.set__22(0);
	instance_arrow_Y_matrix.set__23(1);
	instance_arrow_Y_matrix.set__32(1);
	instance_arrow_Y_matrix.set__33(0);

	//
	//	Scale and center
	//
	instanceYScaleCenter.set_object(instance_arrow_Y);
	instanceYScaleCenter.set_matrix(instanceYScaleCenter_matrix);
	instanceYScaleCenter_matrix.set__11(m_fBoundingSphereDiameter / 4.);
	instanceYScaleCenter_matrix.set__22(m_fBoundingSphereDiameter / 4.);
	instanceYScaleCenter_matrix.set__33(m_fBoundingSphereDiameter / 4.);
	instanceYScaleCenter_matrix.set__41(m_fXmin + (m_fXmax - m_fXmin) / 2.);
	instanceYScaleCenter_matrix.set__42(m_fYmin + (m_fYmax - m_fYmin) / 2.);
	instanceYScaleCenter_matrix.set__43(m_fZmin + (m_fZmax - m_fZmin) / 2.);

	//
	//	Y Collection
	//
	{
		vector<GEOM::GeometricItem> objects = { instanceVIII, instanceV, instanceVII };
		instance_arrow_Y_collection.set_objects(&objects[0], objects.size());
	}

	//
	//	Transformation letter
	//
	
	instanceVIII.set_object(instanceIX);
	instanceVIII.set_matrix(instanceVIII_matrix);
	instanceVIII_matrix.set__11(0.);
	instanceVIII_matrix.set__13(.6);
	instanceVIII_matrix.set__21(.6);
	instanceVIII_matrix.set__22(0.);
	instanceVIII_matrix.set__32(1.);
	instanceVIII_matrix.set__33(0.);
	instanceVIII_matrix.set__41(-1.);
	instanceVIII_matrix.set__42(0.);
	instanceVIII_matrix.set__43(4.3);

	//
	//	Inside Collection
	//
	{
		vector<GEOM::GeometricItem> objects = { instanceX, instanceIII };
		instanceIX.set_objects(&objects[0], objects.size());
	}

	//
	//	First Extruded Polygon for Y
	//
	instanceX.set_extrusionLength(0.02);
	{
		vector<double> points = { 0., 1.2, 0.15, 1.2, .575, .6, .425, .6 };
		instanceX.set_points(&points[0], points.size());
	}

	//
	//	Z Collection
	//

	//
	//	Scale and center
	//
	instanceZScaleCenter.set_object(instance_arrow_Z_collection);
	instanceZScaleCenter.set_matrix(instanceZScaleCenter_matrix);
	instanceZScaleCenter_matrix.set__11(m_fBoundingSphereDiameter / 4.);
	instanceZScaleCenter_matrix.set__22(m_fBoundingSphereDiameter / 4.);
	instanceZScaleCenter_matrix.set__33(m_fBoundingSphereDiameter / 4.);
	instanceZScaleCenter_matrix.set__41(m_fXmin + (m_fXmax - m_fXmin) / 2.);
	instanceZScaleCenter_matrix.set__42(m_fYmin + (m_fYmax - m_fYmin) / 2.);
	instanceZScaleCenter_matrix.set__43(m_fZmin + (m_fZmax - m_fZmin) / 2.);

	{
		vector<GEOM::GeometricItem> objects = { instanceXI, instanceV, instanceVII };
		instance_arrow_Z_collection.set_objects(&objects[0], objects.size());
	}

	//
	//	Transformation letter
	//
	instanceXI.set_object(instanceXII);
	instanceXI.set_matrix(instanceXI_matrix);
	instanceXI_matrix.set__11(.6);
	instanceXI_matrix.set__22(0.);
	instanceXI_matrix.set__23(.6);
	instanceXI_matrix.set__32(1.);
	instanceXI_matrix.set__33(0.);
	instanceXI_matrix.set__41(.4);
	instanceXI_matrix.set__42(0.);
	instanceXI_matrix.set__43(4.3);

	//
	//	Z Extruded Polygon
	//
	instanceXII.set_extrusionLength(0.02);
	{
		vector<double> points = {
									0., 0., 1., 0.,
									1., .15, .18, .15,
									1., 1.05, 1., 1.2,
									0., 1.2, 0., 1.05,
									.82, 1.05, 0., .15
								};
		instanceXII.set_points(&points[0], points.size());
	}
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::LoadRDFModel()
{
    #ifndef _LINUX
//    LOG_DEBUG("CRDFModel::LoadRDFModel() BEGIN");
//	LOG_DEBUG("*** CLASSES ***");
    #endif // _LINUX

	int64_t	iClassInstance = GetClassesByIterator(m_iModel, 0);
	while (iClassInstance != 0)
	{
		m_mapRDFClasses[iClassInstance] = new CRDFClass(iClassInstance);

		iClassInstance = GetClassesByIterator(m_iModel, iClassInstance);
	} // while (iClassInstance != 0)

	#ifndef _LINUX
//	LOG_DEBUG("*** END CLASSES ***");
//	LOG_DEBUG("*** PROPERTIES ***");
	#endif // _LINUX

	int64_t iPropertyInstance = GetPropertiesByIterator(m_iModel, 0);
	while (iPropertyInstance != 0)
	{
		char	* propertyName = nullptr;
		GetNameOfProperty(iPropertyInstance, &propertyName);

		int64_t iPropertyType = GetPropertyType(iPropertyInstance);
		switch (iPropertyType)
		{
			case TYPE_OBJECTTYPE:
			{
				m_mapRDFProperties[iPropertyInstance] = new CObjectRDFProperty(iPropertyInstance);
			}
			break;

			case TYPE_BOOL_DATATYPE:
			{
				m_mapRDFProperties[iPropertyInstance] = new CBoolRDFProperty(iPropertyInstance);
			}
			break;

			case TYPE_CHAR_DATATYPE:
			{
				m_mapRDFProperties[iPropertyInstance] = new CStringRDFProperty(iPropertyInstance);
			}
			break;

			case TYPE_INT_DATATYPE:
			{
				m_mapRDFProperties[iPropertyInstance] = new CIntRDFProperty(iPropertyInstance);
			}
			break;

			case TYPE_DOUBLE_DATATYPE:
			{
				m_mapRDFProperties[iPropertyInstance] = new CDoubleRDFProperty(iPropertyInstance);
			}
			break;

			case 0:
			{
				m_mapRDFProperties[iPropertyInstance] = new CUndefinedRDFProperty(iPropertyInstance);
			}
			break;

			default:
				ASSERT(false);
				break;
		} // switch (iPropertyType)

		map<int64_t, CRDFClass *>::iterator itRDFClasses = m_mapRDFClasses.begin();
		for (; itRDFClasses != m_mapRDFClasses.end(); itRDFClasses++)
		{
			int64_t	iMinCard = -1;
			int64_t iMaxCard = -1;
			GetClassPropertyCardinalityRestrictionNested(itRDFClasses->first, iPropertyInstance, &iMinCard, &iMaxCard);

			if ((iMinCard == -1) && (iMaxCard == -1))
			{
				continue;
			}

			itRDFClasses->second->AddPropertyRestriction(new CRDFPropertyRestriction(iPropertyInstance, iMinCard, iMaxCard));
		} // for (; itRDFClasses != ...

		iPropertyInstance = GetPropertiesByIterator(m_iModel, iPropertyInstance);
	} // while (iPropertyInstance != 0)

	#ifndef _LINUX
//	LOG_DEBUG("*** END PROPERTIES ***");
//	LOG_DEBUG("CRDFModel::LoadRDFModel() END");
	#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::LoadRDFInstances()
{
    #ifndef _LINUX
 //   LOG_DEBUG("CRDFModel::LoadRDFInstances() BEGIN");
    #endif // _LINUX	

	m_fXmin = -1.f;
	m_fXmax = 1.f;
	m_fYmin = -1.f;
	m_fYmax = 1.f;
	m_fZmin = -1.f;
	m_fZmax = 1.f;

	/*
	* Coordinate System
	*/
	//CreateCoordinateSystem();

	m_fXmin = FLT_MAX;
	m_fXmax = -FLT_MAX;
	m_fYmin = FLT_MAX;
	m_fYmax = -FLT_MAX;
	m_fZmin = FLT_MAX;
	m_fZmax = -FLT_MAX;

	/*
	* Enumerate all instances and calculate X/Y/Z min/max
	*/
	{
		int64_t iInstance = GetInstancesByIterator(m_iModel, 0);
		while (iInstance != 0)
		{
			CRDFInstance* pRDFInstance = new CRDFInstance(m_iID++, iInstance);
			if (pRDFInstance->isReferenced())
			{
				pRDFInstance->setEnable(false);
			}

			pRDFInstance->CalculateMinMax(m_fXmin, m_fXmax, m_fYmin, m_fYmax, m_fZmin, m_fZmax);

			m_mapRDFInstances[iInstance] = pRDFInstance;

			iInstance = GetInstancesByIterator(m_iModel, iInstance);
		} // while (iInstance != 0)
	}		

	/*
	* Coordinate System - Instances
	*/
	{
		/*m_pCoordinateSystemModel->loadThings();

		const THINGS& mapThings = m_pCoordinateSystemModel->getThings();

		THINGS::const_iterator itThing = mapThings.begin();
		for (; itThing != mapThings.end(); itThing++)
		{
			CRDFInstance* pRDFInstance = new CRDFInstance(m_iID++, dynamic_cast<Thing*>(itThing->second));
			if (pRDFInstance->isReferenced())
			{
				pRDFInstance->setEnable(false);
			}

			pRDFInstance->CalculateMinMax(m_fXmin, m_fXmax, m_fYmin, m_fYmax, m_fZmin, m_fZmax);

			m_mapRDFInstances[itThing->first] = pRDFInstance;
		}*/
	}	

	/*
	* Coordinate System - Properties
	*/
	int64_t iPropertyInstance = GetPropertiesByIterator(m_pCoordinateSystemModel, 0);
	while (iPropertyInstance != 0)
	{
		int64_t iPropertyType = GetPropertyType(iPropertyInstance);
		switch (iPropertyType)
		{
		case TYPE_OBJECTTYPE:
		{
			m_mapRDFProperties[iPropertyInstance] = new CObjectRDFProperty(iPropertyInstance);
		}
		break;

		case TYPE_BOOL_DATATYPE:
		{
			m_mapRDFProperties[iPropertyInstance] = new CBoolRDFProperty(iPropertyInstance);
		}
		break;

		case TYPE_CHAR_DATATYPE:
		{
			m_mapRDFProperties[iPropertyInstance] = new CStringRDFProperty(iPropertyInstance);
		}
		break;

		case TYPE_INT_DATATYPE:
		{
			m_mapRDFProperties[iPropertyInstance] = new CIntRDFProperty(iPropertyInstance);
		}
		break;

		case TYPE_DOUBLE_DATATYPE:
		{
			m_mapRDFProperties[iPropertyInstance] = new CDoubleRDFProperty(iPropertyInstance);
		}
		break;

		case 0:
		{
			m_mapRDFProperties[iPropertyInstance] = new CUndefinedRDFProperty(iPropertyInstance);
		}
		break;

		default:
			ASSERT(false);
			break;
		} // switch (iPropertyType)

		iPropertyInstance = GetPropertiesByIterator(m_pCoordinateSystemModel, iPropertyInstance);
	} // while (iPropertyInstance != 0)

	/**
	* Scale and Center
	*/
	ScaleAndCenter();

#ifndef _LINUX
//	LOG_DEBUG("CRDFModel::LoadRDFInstances() END");
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
// UNUSED
/*
unsigned char * CRDFModel::LoadBMP(const char * imagepath, unsigned int& outWidth, unsigned int& outHeight, bool flipY)
{
	printf("Reading image %s\n", imagepath);
	outWidth = 0;
	outHeight = 0;
	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	// Actual RGB data
	unsigned char * data;

	// Open the file
	FILE * file = fopen(imagepath, "rb");
	if (!file)							    { ASSERT(false); printf("Image could not be opened\n"); return NULL; }

	// Read the header, i.e. the 54 first bytes

	// If less than 54 byes are read, problem
	if (fread(header, 1, 54, file) != 54){
		ASSERT(false);
		printf("Not a correct BMP file\n");
		return NULL;
	}
	// A BMP files always begins with "BM"
	if (header[0] != 'B' || header[1] != 'M'){
		ASSERT(false);
		printf("Not a correct BMP file\n");
		return NULL;
	}
	// Make sure this is a 24bpp file
	if (*(int*)&(header[0x1E]) != 0)         { ASSERT(false); printf("Not a correct BMP file\n");    return NULL; }
	if (*(int*)&(header[0x1C]) != 24)         { ASSERT(false); printf("Not a correct BMP file\n");    return NULL; }

	// Read the information about the image
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	outWidth = *(int*)&(header[0x12]);
	outHeight = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    imageSize = outWidth*outHeight * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	// Everything is in memory now, the file wan be closed
	fclose(file);

	if (flipY){
		// swap y-axis
		unsigned char * tmpBuffer = new unsigned char[outWidth * 3];
		int size = outWidth * 3;
		for (unsigned int i = 0; i<outHeight / 2; i++){
			// copy row i to tmp
			memcpy_s(tmpBuffer, size, data + outWidth * 3 * i, size);
			// copy row h-i-1 to i
			memcpy_s(data + outWidth * 3 * i, size, data + outWidth * 3 * (outHeight - i - 1), size);
			// copy tmp to row h-i-1
			memcpy_s(data + outWidth * 3 * (outHeight - i - 1), size, tmpBuffer, size);
		}
		delete[] tmpBuffer;
	}

	return data;
}
*/

// ------------------------------------------------------------------------------------------------
void CRDFModel::Clean()
{
	/*
	* Model
	*/
	if (m_iModel != 0)
	{
		CloseModel(m_iModel);
		m_iModel = 0;
	}

	/*
	* Model
	*/
//	m_pCoordinateSystemModel->close();
	m_pCoordinateSystemModel = 0;

	/*
	* RDF Classes
	*/
	map<int64_t, CRDFClass *>::iterator itRDFClasses = m_mapRDFClasses.begin();
	for (; itRDFClasses != m_mapRDFClasses.end(); itRDFClasses++)
	{
		delete itRDFClasses->second;
	}

	m_mapRDFClasses.clear();

	/*
	* RDF Properties
	*/
	map<int64_t, CRDFProperty *>::iterator itRDFProperties = m_mapRDFProperties.begin();
	for (; itRDFProperties != m_mapRDFProperties.end(); itRDFProperties++)
	{
		delete itRDFProperties->second;
	}

	m_mapRDFProperties.clear();

	/*
	* RDF Instances
	*/
	map<int64_t, CRDFInstance *>::iterator itRDFInstances = m_mapRDFInstances.begin();
	for (; itRDFInstances != m_mapRDFInstances.end(); itRDFInstances++)
	{
		delete itRDFInstances->second;
	}

	m_mapRDFInstances.clear();
	
	/*
	* Texture
	*/
	delete m_pDefaultTexture;
	m_pDefaultTexture = nullptr;

	/*
	* Measurements
	*/
	delete m_pMeasurementsBuilder;
	m_pMeasurementsBuilder = nullptr;

	m_iID = 1;

	/*
	* Octree
	*/
	delete m_pOctree;
	m_pOctree = nullptr;
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::GetClassPropertyCardinalityRestrictionNested(int64_t iRDFClass, int64_t iRDFProperty, int64_t * pMinCard, int64_t * pMaxCard)
{
	int64_t	minCard = 0, maxCard = -1;
	GetClassPropertyCardinalityRestriction(iRDFClass, iRDFProperty, &minCard, &maxCard);
	if ((*pMinCard) < minCard) {
		(*pMinCard) = minCard;
	}
	if (maxCard > 0 && (*pMaxCard) > maxCard) {
		(*pMaxCard) = maxCard;
	}

	int64_t	iRDFClassParent = GetParentsByIterator(iRDFClass, 0);
	while (iRDFClassParent) {
		GetClassPropertyCardinalityRestrictionNested(iRDFClassParent, iRDFProperty, pMinCard, pMaxCard);
		iRDFClassParent = GetParentsByIterator(iRDFClass, iRDFClassParent);
	}
}


// ------------------------------------------------------------------------------------------------
// Octants
void CRDFModel::BuildOctants(_octant* pOctant, vector<GEOM::GeometricItem>& vecOctantsGeometry)
{
	if (pOctant == nullptr)
	{
		return;
	}

	int64_t	pModel = m_iModel;

	double dXmin = 0.;
	double dXmax = 0.;
	double dYmin = 0.;
	double dYmax = 0.;
	double dZmin = 0.;
	double dZmax = 0.;

	if (pOctant->getType() == _octant_type::Node)
	{
		auto pOctreeNode = dynamic_cast<_octree_node*>(pOctant);
		ASSERT(pOctreeNode != nullptr);

		auto vecOctants = pOctreeNode->getOctants();
		for (size_t iOctant = 0; iOctant < vecOctants.size(); iOctant++)
		{
			BuildOctants(vecOctants[iOctant], vecOctantsGeometry);
		}	

		// Don't show nodes
		return;

		/*dXmin = pOctreeNode->getXmin();
		dXmax = pOctreeNode->getXmax();
		dYmin = pOctreeNode->getYmin();
		dYmax = pOctreeNode->getYmax();
		dZmin = pOctreeNode->getZmin();
		dZmax = pOctreeNode->getZmax();*/
	}			
	else
	{		
		// Show octants with a point
		auto pOctreePoint = dynamic_cast<_octree_point*>(pOctant);
		ASSERT(pOctreePoint != nullptr);

		dXmin = pOctreePoint->getXmin();
		dXmax = pOctreePoint->getXmax();
		dYmin = pOctreePoint->getYmin();
		dYmax = pOctreePoint->getYmax();
		dZmin = pOctreePoint->getZmin();
		dZmax = pOctreePoint->getZmax();
	}

	VECTOR3 vecBoundingBoxMin = { dXmin, dYmin, dZmin };
	VECTOR3 vecBoundingBoxMax = { dXmax, dYmax, dZmax };

	vector<GEOM::GeometricItem> vecObjects;

	// Bottom face
	/*
	Min1						Min2
	>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	|								|
	|								|
	|								|
	|								|
	|								|
	<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	Min4						Min3
	*/

	VECTOR3 vecMin1 = { vecBoundingBoxMin.x, vecBoundingBoxMin.y, vecBoundingBoxMin.z };
	VECTOR3 vecMin2 = { vecBoundingBoxMax.x, vecBoundingBoxMin.y, vecBoundingBoxMin.z };
	VECTOR3 vecMin3 = { vecBoundingBoxMax.x, vecBoundingBoxMin.y, vecBoundingBoxMax.z };
	VECTOR3 vecMin4 = { vecBoundingBoxMin.x, vecBoundingBoxMin.y, vecBoundingBoxMax.z };

	{
		vector<double> vecCoordinates;
		vecCoordinates.push_back(vecMin1.x);
		vecCoordinates.push_back(vecMin1.y);
		vecCoordinates.push_back(vecMin1.z);

		vecCoordinates.push_back(vecMin2.x);
		vecCoordinates.push_back(vecMin2.y);
		vecCoordinates.push_back(vecMin2.z);

		vecCoordinates.push_back(vecMin3.x);
		vecCoordinates.push_back(vecMin3.y);
		vecCoordinates.push_back(vecMin3.z);

		vecCoordinates.push_back(vecMin4.x);
		vecCoordinates.push_back(vecMin4.y);
		vecCoordinates.push_back(vecMin4.z);

		vecCoordinates.push_back(vecMin1.x);
		vecCoordinates.push_back(vecMin1.y);
		vecCoordinates.push_back(vecMin1.z);

		auto pPolyLine3D = GEOM::PolyLine3D::Create(pModel);
		pPolyLine3D.set_coordinates(&vecCoordinates[0], vecCoordinates.size());

		vecObjects.push_back(pPolyLine3D);
	}

	// Top face
	/*
	Max3						Max4
	>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	|								|
	|								|
	|								|
	|								|
	|								|
	<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	Max2						Max1
	*/	

	VECTOR3 vecMax1 = { vecBoundingBoxMax.x, vecBoundingBoxMax.y, vecBoundingBoxMax.z };
	VECTOR3 vecMax2 = { vecBoundingBoxMin.x, vecBoundingBoxMax.y, vecBoundingBoxMax.z };
	VECTOR3 vecMax3 = { vecBoundingBoxMin.x, vecBoundingBoxMax.y, vecBoundingBoxMin.z };
	VECTOR3 vecMax4 = { vecBoundingBoxMax.x, vecBoundingBoxMax.y, vecBoundingBoxMin.z };

	{
		vector<double> vecCoordinates;

		vecCoordinates.push_back(vecMax1.x);
		vecCoordinates.push_back(vecMax1.y);
		vecCoordinates.push_back(vecMax1.z);

		vecCoordinates.push_back(vecMax2.x);
		vecCoordinates.push_back(vecMax2.y);
		vecCoordinates.push_back(vecMax2.z);

		vecCoordinates.push_back(vecMax3.x);
		vecCoordinates.push_back(vecMax3.y);
		vecCoordinates.push_back(vecMax3.z);

		vecCoordinates.push_back(vecMax4.x);
		vecCoordinates.push_back(vecMax4.y);
		vecCoordinates.push_back(vecMax4.z);

		vecCoordinates.push_back(vecMax1.x);
		vecCoordinates.push_back(vecMax1.y);
		vecCoordinates.push_back(vecMax1.z);

		auto pPolyLine3D = GEOM::PolyLine3D::Create(pModel);
		pPolyLine3D.set_coordinates(&vecCoordinates[0], vecCoordinates.size());

		vecObjects.push_back(pPolyLine3D);
	}

	// Left face
	/*
	Max3						Max2	
	|								|
	|								|
	|								|
	|								|
	|								|	
	Min1						Min4
	*/

	{
		vector<double> vecCoordinates;

		vecCoordinates.push_back(vecMin1.x);
		vecCoordinates.push_back(vecMin1.y);
		vecCoordinates.push_back(vecMin1.z);

		vecCoordinates.push_back(vecMax3.x);
		vecCoordinates.push_back(vecMax3.y);
		vecCoordinates.push_back(vecMax3.z);

		auto pLine3D = GEOM::Line3D::Create(pModel);
		pLine3D.set_points(&vecCoordinates[0], vecCoordinates.size());

		vecObjects.push_back(pLine3D);
	}

	{
		vector<double> vecCoordinates;

		vecCoordinates.push_back(vecMin4.x);
		vecCoordinates.push_back(vecMin4.y);
		vecCoordinates.push_back(vecMin4.z);

		vecCoordinates.push_back(vecMax2.x);
		vecCoordinates.push_back(vecMax2.y);
		vecCoordinates.push_back(vecMax2.z);

		auto pLine3D = GEOM::Line3D::Create(pModel);
		pLine3D.set_points(&vecCoordinates[0], vecCoordinates.size());

		vecObjects.push_back(pLine3D);
	}

	// Right face
	/*
	Max1						Max4
	|								|
	|								|
	|								|
	|								|
	|								|
	Min3						Min2
	*/

	{
		vector<double> vecCoordinates;

		vecCoordinates.push_back(vecMin3.x);
		vecCoordinates.push_back(vecMin3.y);
		vecCoordinates.push_back(vecMin3.z);

		vecCoordinates.push_back(vecMax1.x);
		vecCoordinates.push_back(vecMax1.y);
		vecCoordinates.push_back(vecMax1.z);

		auto pLine3D = GEOM::Line3D::Create(pModel);
		pLine3D.set_points(&vecCoordinates[0], vecCoordinates.size());

		vecObjects.push_back(pLine3D);
	}

	{
		vector<double> vecCoordinates;

		vecCoordinates.push_back(vecMin2.x);
		vecCoordinates.push_back(vecMin2.y);
		vecCoordinates.push_back(vecMin2.z);

		vecCoordinates.push_back(vecMax4.x);
		vecCoordinates.push_back(vecMax4.y);
		vecCoordinates.push_back(vecMax4.z);

		auto pLine3D = GEOM::Line3D::Create(pModel);
		pLine3D.set_points(&vecCoordinates[0], vecCoordinates.size());

		vecObjects.push_back(pLine3D);
	}

	auto pCollection = GEOM::Collection::Create(pModel);
	pCollection.set_objects(&vecObjects[0], vecObjects.size());

	vecOctantsGeometry.push_back(pCollection);
}