#include "stdafx.h"

#include "RDFModel.h"
#include "Generic.h"
#include "ProgressIndicator.h"

#include "_dxf_parser.h"
#include "gisengine.h"

#include <bitset>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <time.h>
using namespace std;

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

// ------------------------------------------------------------------------------------------------
CRDFModel::CRDFModel()
	: m_strModel(L"")
	, m_iModel(0)
	, m_mapClasses()
	, m_mapProperties()
	, m_mapInstances()
	, m_mapInstanceMetaData()
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
	, m_mapTextures()
	, m_pOctree(nullptr)
{
}

// ------------------------------------------------------------------------------------------------
CRDFModel::~CRDFModel()
{
	Clean();
}

// ------------------------------------------------------------------------------------------------
int64_t CRDFModel::GetModel() const
{
	return m_iModel;
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::CreateDefaultModel()
{
	Clean();
	
	m_iModel = CreateModel();
	ASSERT(m_iModel != 0);	

	int64_t	pModel = m_iModel;

	SetFormatSettings(m_iModel);

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

	LoadRDFModel();
}

// ------------------------------------------------------------------------------------------------
const map<int64_t, CRDFClass *> & CRDFModel::GetClasses() const
{
	return m_mapClasses;
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::GetClassAncestors(int64_t iClassInstance, vector<int64_t> & vecAncestors) const
{
	ASSERT(iClassInstance != 0);

	map<int64_t, CRDFClass *>::const_iterator itClass = m_mapClasses.find(iClassInstance);
	ASSERT(itClass != m_mapClasses.end());

	CRDFClass * pClass = itClass->second;

	const vector<int64_t> & vecParentClasses = pClass->getParentClasses();
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
const map<int64_t, CRDFProperty *>& CRDFModel::GetProperties()
{
	return m_mapProperties;
}

// ------------------------------------------------------------------------------------------------
const map<int64_t, CRDFInstance *>& CRDFModel::GetInstances() const
{
	return m_mapInstances;
}
// ------------------------------------------------------------------------------------------------
CRDFInstance * CRDFModel::GetInstanceByID(int64_t iID)
{
	ASSERT(iID != 0);

	auto itInstance = m_mapInstances.begin();
	for (; itInstance != m_mapInstances.end(); itInstance++)
	{
		if (itInstance->second->GetID() == iID)
		{
			return itInstance->second;
		}
	}

	ASSERT(false);

	return nullptr;
}

CRDFInstance * CRDFModel::GetInstanceByIInstance(int64_t iInstance)
{
	ASSERT(iInstance != 0);

	map<int64_t, CRDFInstance *>::iterator itInstance = m_mapInstances.begin();
	for (; itInstance != m_mapInstances.end(); itInstance++)
	{
		if (itInstance->first == iInstance)
		{
			return itInstance->second;
		}
	}

	return nullptr;
}

// ------------------------------------------------------------------------------------------------
CRDFInstance * CRDFModel::CreateNewInstance(int64_t iClassInstance)
{
	ASSERT(iClassInstance != 0);

	int64_t iInstance = CreateInstance(iClassInstance);
	ASSERT(iInstance != 0);

	auto pInstance = new CRDFInstance(m_iID++, iInstance);
	pInstance->CalculateMinMax(m_fXmin, m_fXmax, m_fYmin, m_fYmax, m_fZmin, m_fZmax);

	m_mapInstances[iInstance] = pInstance;

	return pInstance;
}

// ------------------------------------------------------------------------------------------------
CRDFInstance* CRDFModel::AddNewInstance(int64_t pThing)
{
	auto pInstance = new CRDFInstance(m_iID++, pThing);
	pInstance->CalculateMinMax(m_fXmin, m_fXmax, m_fYmin, m_fYmax, m_fZmin, m_fZmax);

	m_mapInstances[pThing] = pInstance;

	return pInstance;
}

// ------------------------------------------------------------------------------------------------
bool CRDFModel::DeleteInstance(CRDFInstance * pInstance)
{
	ASSERT(pInstance != nullptr);

	bool bResult = RemoveInstance(pInstance->GetInstance()) == 0 ? true : false;

	map<int64_t, CRDFInstance *>::iterator itInstance = m_mapInstances.find(pInstance->GetInstance());
	ASSERT(itInstance != m_mapInstances.end());

	m_mapInstances.erase(itInstance);

	delete pInstance;

	return bResult;
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::AddMeasurements(CRDFInstance * /*pInstance*/)
{	
	ASSERT(FALSE); // TODO: PENDING REFACTORING!
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::ImportModel(const wchar_t* szPath)
{
	/*
	* Import
	*/
	ImportModelW(m_iModel, szPath);

	/*
	* Load
	*/
	LoadRDFInstances();
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::GetCompatibleInstances(CRDFInstance * pInstance, CObjectRDFProperty * pObjectRDFProperty, vector<int64_t> & vecCompatibleInstances) const
{
	ASSERT(pInstance != nullptr);
	ASSERT(pObjectRDFProperty != nullptr);

	int64_t iClassInstance = GetInstanceClass(pInstance->GetInstance());
	ASSERT(iClassInstance != 0);

	auto& vecRestrictions = pObjectRDFProperty->getRestrictions();
	ASSERT(!vecRestrictions.empty());

	auto& mapRFDInstances = GetInstances();

	auto itRFDInstances = mapRFDInstances.begin();
	for (; itRFDInstances != mapRFDInstances.end(); itRFDInstances++)
	{
		/*
		* Skip this instance
		*/
		if (itRFDInstances->second == pInstance)
		{
			continue;
		}

		/*
		* Skip the instances that belong to a different model
		*/
		if (itRFDInstances->second->GetModel() != pInstance->GetModel())
		{
			continue;
		}

		/*
		* Check this instance
		*/
		if (std::find(vecRestrictions.begin(), vecRestrictions.end(), itRFDInstances->second->GetClassInstance()) != vecRestrictions.end())
		{
			vecCompatibleInstances.push_back(itRFDInstances->second->GetInstance());

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
				vecCompatibleInstances.push_back(itRFDInstances->second->GetInstance());

				break;
			}
		} // for (size_t iAncestorClass = ...
	} // for (; itRFDInstances != ...
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::GetWorldDimensions(float& fXmin, float& fXmax, float& fYmin, float& fYmax, float& fZmin, float& fZmax) const
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
	ProgressStatus stat(L"Calculate scene sizes");

	/*
	* Min/Max
	*/
	m_fXmin = FLT_MAX;
	m_fXmax = -FLT_MAX;
	m_fYmin = FLT_MAX;
	m_fYmax = -FLT_MAX;
	m_fZmin = FLT_MAX;
	m_fZmax = -FLT_MAX;

	m_fBoundingSphereDiameter = 0.f;

	m_fXTranslation = 0.f;
	m_fYTranslation = 0.f;
	m_fZTranslation = 0.f;

	auto itInstance = m_mapInstances.begin();
	for (; itInstance != m_mapInstances.end(); itInstance++)
	{
		itInstance->second->ResetScaleAndCenter();
		itInstance->second->CalculateMinMax(
			m_fXmin, m_fXmax, 
			m_fYmin, m_fYmax, 
			m_fZmin, m_fZmax);
	}

	if ((m_fXmin == FLT_MAX) ||
		(m_fXmax == -FLT_MAX) ||
		(m_fYmin == FLT_MAX) ||
		(m_fYmax == -FLT_MAX) ||
		(m_fZmin == FLT_MAX) ||
		(m_fZmax == -FLT_MAX))
	{
		m_fXmin = -1.;
		m_fXmax = 1.;
		m_fYmin = -1.;
		m_fYmax = 1.;
		m_fZmin = -1.;
		m_fZmax = 1.;
	}

	/*
	* World
	*/
	m_fBoundingSphereDiameter = m_fXmax - m_fXmin;
	m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fYmax - m_fYmin);
	m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fZmax - m_fZmin);

	TRACE(L"\n*** Scale and Center I *** => Xmin/max, Ymin/max, Zmin/max: %.16f, %.16f, %.16f, %.16f, %.16f, %.16f",
		m_fXmin,
		m_fXmax,
		m_fYmin,
		m_fYmax,
		m_fZmin,
		m_fZmax);
	TRACE(L"\n*** Scale and Center, Bounding sphere I *** =>  %.16f",
		m_fBoundingSphereDiameter);

	bool bScale = true;
	if (((m_fXmax - m_fXmin) / m_fBoundingSphereDiameter) <= 0.0001)
	{
		bScale = false;
	}
	else if (((m_fYmax - m_fYmin) / m_fBoundingSphereDiameter) <= 0.0001)
	{
		bScale = false;
	}
	else if (((m_fZmax - m_fZmin) / m_fBoundingSphereDiameter) <= 0.0001)
	{
		bScale = false;
	}

	if (!bScale)
	{
		CString strWarning = L"'Scale' algorithm cannot be used.\n";
		strWarning += L"Please, use 'Zoom to/extent' to explore the model.";

		::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), strWarning, L"Warning", MB_ICONWARNING | MB_OK);
	}

	/*
	* Scale and Center
	*/
	itInstance = m_mapInstances.begin();
	for (; itInstance != m_mapInstances.end(); itInstance++)
	{
		itInstance->second->ScaleAndCenter(
			m_fXmin, m_fXmax, 
			m_fYmin, m_fYmax, 
			m_fZmin, m_fZmax, 
			m_fBoundingSphereDiameter,
			bScale);
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

	itInstance = m_mapInstances.begin();
	for (; itInstance != m_mapInstances.end(); itInstance++)
	{
		if (itInstance->second->getEnable())
		{
			itInstance->second->CalculateMinMax(
				m_fXmin, m_fXmax, 
				m_fYmin, m_fYmax, 
				m_fZmin, m_fZmax);
		}
	}

	if ((m_fXmin == FLT_MAX) ||
		(m_fXmax == -FLT_MAX) ||
		(m_fYmin == FLT_MAX) ||
		(m_fYmax == -FLT_MAX) ||
		(m_fZmin == FLT_MAX) ||
		(m_fZmax == -FLT_MAX))
	{
		m_fXmin = -1.;
		m_fXmax = 1.;
		m_fYmin = -1.;
		m_fYmax = 1.;
		m_fZmin = -1.;
		m_fZmax = 1.;
	}

	/*
	* World
	*/
	m_fBoundingSphereDiameter = m_fXmax - m_fXmin;
	m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fYmax - m_fYmin);
	m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fZmax - m_fZmin);

	TRACE(L"\n*** Scale and Center II *** => Xmin/max, Ymin/max, Zmin/max: %.16f, %.16f, %.16f, %.16f, %.16f, %.16f",
		m_fXmin,
		m_fXmax,
		m_fYmin,
		m_fYmax,
		m_fZmin,
		m_fZmax);
	TRACE(L"\n*** Scale and Center, Bounding sphere II *** =>  %.16f",
		m_fBoundingSphereDiameter);

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
	if (bScale)
	{
		m_fXTranslation /= (m_fBoundingSphereDiameter / 2.0f);
		m_fYTranslation /= (m_fBoundingSphereDiameter / 2.0f);
		m_fZTranslation /= (m_fBoundingSphereDiameter / 2.0f);
	}	
}

// ------------------------------------------------------------------------------------------------
float CRDFModel::GetBoundingSphereDiameter() const
{
	return m_fBoundingSphereDiameter;
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::ZoomToInstance(int64_t iInstance)
{
	m_fBoundingSphereDiameter = 0.f;

	m_fXTranslation = 0.f;
	m_fYTranslation = 0.f;
	m_fZTranslation = 0.f;

	ASSERT(iInstance != 0);
	ASSERT(m_mapInstances.find(iInstance) != m_mapInstances.end());

	m_fXmin = FLT_MAX;
	m_fXmax = -FLT_MAX;
	m_fYmin = FLT_MAX;
	m_fYmax = -FLT_MAX;
	m_fZmin = FLT_MAX;
	m_fZmax = -FLT_MAX;

	m_mapInstances[iInstance]->CalculateMinMax(m_fXmin, m_fXmax, m_fYmin, m_fYmax, m_fZmin, m_fZmax);

	if ((m_fXmin == FLT_MAX) ||
		(m_fXmax == -FLT_MAX) ||
		(m_fYmin == FLT_MAX) ||
		(m_fYmax == -FLT_MAX) ||
		(m_fZmin == FLT_MAX) ||
		(m_fZmax == -FLT_MAX))
	{
		m_fXmin = -1.;
		m_fXmax = 1.;
		m_fYmin = -1.;
		m_fYmax = 1.;
		m_fZmin = -1.;
		m_fZmax = 1.;
	}

	m_fBoundingSphereDiameter = m_fXmax - m_fXmin;
	m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fYmax - m_fYmin);
	m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fZmax - m_fZmin);

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
void CRDFModel::ZoomOut()
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

	auto itInstance = m_mapInstances.begin();
	for (; itInstance != m_mapInstances.end(); itInstance++)
	{
		if (!itInstance->second->getEnable())
		{
			continue;
		}

		itInstance->second->CalculateMinMax(m_fXmin, m_fXmax, m_fYmin, m_fYmax, m_fZmin, m_fZmax);
	}

	if ((m_fXmin == FLT_MAX) ||
		(m_fXmax == -FLT_MAX) ||
		(m_fYmin == FLT_MAX) ||
		(m_fYmax == -FLT_MAX) ||
		(m_fZmin == FLT_MAX) ||
		(m_fZmax == -FLT_MAX))
	{
		m_fXmin = -1.;
		m_fXmax = 1.;
		m_fYmin = -1.;
		m_fYmax = 1.;
		m_fZmin = -1.;
		m_fZmax = 1.;
	}

	/*
	* World
	*/
	m_fBoundingSphereDiameter = m_fXmax - m_fXmin;
	m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fYmax - m_fYmin);
	m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fZmax - m_fZmin);

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
void CRDFModel::OnInstancePropertyEdited(CRDFInstance * /*pInstance*/, CRDFProperty * /*pProperty*/)
{
	SetFormatSettings(m_iModel);

	map<int64_t, CRDFInstance *>::iterator itInstance = m_mapInstances.begin();
	for (; itInstance != m_mapInstances.end(); itInstance++)
	{
		if (itInstance->second->GetModel() != m_iModel)
		{
			continue;
		}

		itInstance->second->Recalculate();
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

	m_strModel = szPath;
	
	CString strExtension = PathFindExtension(szPath);
	strExtension.MakeUpper();

	if (strExtension == L".DXF")
	{
		LoadDXF(szPath);
	} else if ((strExtension == L".GML") ||
		(strExtension == L".CITYGML") ||
		(strExtension == L".XML") ||
		(strExtension == L".JSON"))
	{
		LoadGISModel(szPath);
	}
	else
	{
		m_iModel = OpenModelW(szPath);
		if (m_iModel != 0)
		{
			SetFormatSettings(m_iModel);
			LoadRDFModel();
		}
	}

	if (m_iModel == 0) 
	{
		CString strError;
		strError.Format(L"Failed to open '%s'.", szPath);

		::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), strError, L"Error", MB_ICONERROR | MB_OK);
	}	
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::LoadDXF(const wchar_t* szPath)
{
	m_iModel = CreateModel();
	ASSERT(m_iModel != 0);

	SetFormatSettings(m_iModel);	

	try
	{
		_dxf::_parser parser(m_iModel);
		parser.load(CW2A(szPath));
	}
	catch (const std::runtime_error& ex)
	{
		::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), CA2W(ex.what()), L"Error", MB_ICONERROR | MB_OK);

		return;
	}

	LoadRDFModel();
}

void CRDFModel::LoadGISModel(const wchar_t* szPath)
{
	Clean();

	m_iModel = CreateModel();
	ASSERT(m_iModel != 0);

	SetFormatSettings(m_iModel);	

	try
	{
		wchar_t szAppPath[_MAX_PATH];
		::GetModuleFileName(::GetModuleHandle(nullptr), szAppPath, sizeof(szAppPath));

		fs::path pthExe = szAppPath;
		auto pthRootFolder = pthExe.parent_path();
		string strRootFolder = pthRootFolder.string();
		strRootFolder += "\\";

		SetGISOptions(strRootFolder.c_str(), true);

		ImportGISModel(m_iModel, CW2A(szPath));
	}
	catch (const std::runtime_error& err)
	{
		::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), CA2W(err.what()), L"Error", MB_ICONERROR | MB_OK);

		return;
	}
	catch (...)
	{
		::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Unknown error.", L"Error", MB_ICONERROR | MB_OK);
	}	

	LoadRDFModel();
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::PostLoad()
{
}

CTexture* CRDFModel::GetTexture(const wstring& strTexture)
{
	if (!m_strModel.empty())
	{
		if (m_mapTextures.find(strTexture) != m_mapTextures.end())
		{
			return m_mapTextures.at(strTexture);
		}

		fs::path pthFile = m_strModel;
		fs::path pthTexture = pthFile.parent_path();
		pthTexture.append(strTexture);

		if (fs::exists(pthTexture))
		{
			auto pOGLTexture = new CTexture();
			pOGLTexture->LoadFile(pthTexture.wstring().c_str());

			m_mapTextures[strTexture] = pOGLTexture;

			return pOGLTexture;
		}
	} // if (!m_strModel.empty())

	return GetDefaultTexture();
}

// ------------------------------------------------------------------------------------------------
CTexture * CRDFModel::GetDefaultTexture()
{
	if (m_pDefaultTexture == nullptr)
	{
		wchar_t szAppPath[_MAX_PATH];
		::GetModuleFileName(::GetModuleHandle(nullptr), szAppPath, sizeof(szAppPath));

		CString strDefaultTexture = szAppPath;
		strDefaultTexture.MakeLower();

		int iLastSlash = strDefaultTexture.ReverseFind(L'\\');
		ASSERT(iLastSlash != -1);

		strDefaultTexture = strDefaultTexture.Left(iLastSlash + 1);
		strDefaultTexture += L"texture.jpg";

		LPCTSTR szDefaultTexture = (LPCTSTR)strDefaultTexture;

		m_pDefaultTexture = new CTexture();
		if (!m_pDefaultTexture->LoadFile(szDefaultTexture))
		{
            MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"The default texture is not found.", L"Error", MB_ICONERROR | MB_OK);
		}
	} // if (m_pDefaultTexture == nullptr)

	return m_pDefaultTexture;
}

const CString& CRDFModel::GetInstanceMetaData(CRDFInstance* pInstance)
{
	if (m_mapInstanceMetaData.find(pInstance) == m_mapInstanceMetaData.end())
	{
		wchar_t* szName = nullptr;
		GetNameOfInstanceW(pInstance->GetInstance(), &szName);

		CString strMetaData = szName != nullptr ? szName : L"NA";

		int64_t iPropertyInstance = GetInstancePropertyByIterator(pInstance->GetInstance(), 0);
		while (iPropertyInstance != 0)
		{
			auto itProperty = m_mapProperties.find(iPropertyInstance);
			ASSERT(itProperty != m_mapProperties.end());

			GetPropertyMetaData(pInstance, itProperty->second, strMetaData);

			iPropertyInstance = GetInstancePropertyByIterator(pInstance->GetInstance(), iPropertyInstance);
		} // while (iPropertyInstance != 0)

		if (strMetaData.GetLength() >= 256)
		{
			strMetaData = strMetaData.Left(250);
			strMetaData += L"...";
		}
		 
		m_mapInstanceMetaData[pInstance] = strMetaData;
	} // if (m_mapInstanceMetaData.find(pInstance) == ...

	return m_mapInstanceMetaData.at(pInstance);
}

void CRDFModel::GetPropertyMetaData(CRDFInstance* pInstance, CRDFProperty* pProperty, CString& strMetaData)
{
	strMetaData += L"\n";
	strMetaData += pProperty->GetName() != nullptr ? pProperty->GetName() : L"NA";
	strMetaData += L": ";

	/* value */
	wchar_t szBuffer[1000];
	switch (pProperty->getType())
	{
		case TYPE_OBJECTTYPE:
		{
			int64_t* piInstances = nullptr;
			int64_t iCard = 0;
			GetObjectProperty(pInstance->GetInstance(), pProperty->GetInstance(), &piInstances, &iCard);

			if ((iCard == 1) && (pProperty->GetName() == CString(L"$semantics")))
			{
				auto itInstance = m_mapInstances.find(piInstances[0]);
				ASSERT(itInstance != m_mapInstances.end());

				int64_t iPropertyInstance = GetInstancePropertyByIterator(piInstances[0], 0);
				while (iPropertyInstance != 0)
				{
					auto itProperty = m_mapProperties.find(iPropertyInstance);
					ASSERT(itProperty != m_mapProperties.end());

					GetPropertyMetaData(itInstance->second, itProperty->second, strMetaData);

					iPropertyInstance = GetInstancePropertyByIterator(piInstances[0], iPropertyInstance);
				} // while (iPropertyInstance != 0)
			}
			else
			{
				strMetaData += iCard > 0 ? L"[...]" : L"[]";
			}
		}
		break;

		case TYPE_BOOL_DATATYPE:
		{
			int64_t iCard = 0;
			bool* pbValue = nullptr;
			GetDatatypeProperty(pInstance->GetInstance(), pProperty->GetInstance(), (void**)&pbValue, &iCard);

			if (iCard == 1)
			{
				swprintf(szBuffer, 100, L"value = %s", pbValue[0] ? L"TRUE" : L"FALSE");
				strMetaData += szBuffer;
			}
			else
			{
				strMetaData += iCard > 0 ? L"[...]" : L"[]";
			}
		}
		break;

		case TYPE_CHAR_DATATYPE:
		{
			int64_t iCard = 0;
			char** szValue = nullptr;
			GetDatatypeProperty(pInstance->GetInstance(), pProperty->GetInstance(), (void**)&szValue, &iCard);

			if (iCard == 1)
			{
				strMetaData += CA2W(szValue[0]);
			}
			else
			{
				strMetaData += iCard > 0 ? L"[...]" : L"[]";
			}
		}
		break;

		case TYPE_DOUBLE_DATATYPE:
		{
			int64_t iCard = 0;
			double* pdValue = nullptr;
			GetDatatypeProperty(pInstance->GetInstance(), pProperty->GetInstance(), (void**)&pdValue, &iCard);

			if (iCard == 1)
			{
				swprintf(szBuffer, 100, L"%.6f", pdValue[0]);
				strMetaData += szBuffer;
			}
			else
			{
				strMetaData += iCard > 0 ? L"[...]" : L"[]";
			}
		}
		break;

		case TYPE_INT_DATATYPE:
		{
			int64_t iCard = 0;
			int64_t* piValue = nullptr;
			GetDatatypeProperty(pInstance->GetInstance(), pProperty->GetInstance(), (void**)&piValue, &iCard);

			if (iCard == 1)
			{
				swprintf(szBuffer, 100, L"%lld", piValue[0]);
				strMetaData += szBuffer;
			}
			else
			{
				strMetaData += iCard > 0 ? L"[...]" : L"[]";
			}
		}
		break;

		default:
		{
			ASSERT(FALSE); // unknown property

			strMetaData += L"NA";
		}
		break;
	} // switch (pProperty->getType())
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::SetFormatSettings(int64_t iModel)
{
	string strSettings = "111111000000001011000001110001";

	bitset<64> bitSettings(strSettings);
	int64_t iSettings = bitSettings.to_ulong();

	string strMask = "11111111111111111011011101110111";
	bitset <64> bitMask(strMask);
	int64_t iMask = bitMask.to_ulong();

	SetFormat(iModel, (int64_t)iSettings, (int64_t)iMask);
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::LoadRDFModel()
{
	ProgressStatus(L"Loading RDF model schema");

	int64_t	iClassInstance = GetClassesByIterator(m_iModel, 0);
	while (iClassInstance != 0)
	{
		m_mapClasses[iClassInstance] = new CRDFClass(iClassInstance);

		iClassInstance = GetClassesByIterator(m_iModel, iClassInstance);
	} // while (iClassInstance != 0)
	
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
				m_mapProperties[iPropertyInstance] = new CObjectRDFProperty(iPropertyInstance);
			}
			break;

			case TYPE_BOOL_DATATYPE:
			{
				m_mapProperties[iPropertyInstance] = new CBoolRDFProperty(iPropertyInstance);
			}
			break;

			case TYPE_CHAR_DATATYPE:
			{
				m_mapProperties[iPropertyInstance] = new CStringRDFProperty(iPropertyInstance);
			}
			break;

			case TYPE_INT_DATATYPE:
			{
				m_mapProperties[iPropertyInstance] = new CIntRDFProperty(iPropertyInstance);
			}
			break;

			case TYPE_DOUBLE_DATATYPE:
			{
				m_mapProperties[iPropertyInstance] = new CDoubleRDFProperty(iPropertyInstance);
			}
			break;

			case 0:
			{
				m_mapProperties[iPropertyInstance] = new CUndefinedRDFProperty(iPropertyInstance);
			}
			break;

			default:
				ASSERT(false);
				break;
		} // switch (iPropertyType)

		map<int64_t, CRDFClass *>::iterator itClasses = m_mapClasses.begin();
		for (; itClasses != m_mapClasses.end(); itClasses++)
		{
			int64_t	iMinCard = -1;
			int64_t iMaxCard = -1;
			GetClassPropertyCardinalityRestrictionNested(itClasses->first, iPropertyInstance, &iMinCard, &iMaxCard);

			if ((iMinCard == -1) && (iMaxCard == -1))
			{
				continue;
			}

			itClasses->second->AddPropertyRestriction(new CRDFPropertyRestriction(iPropertyInstance, iMinCard, iMaxCard));
		} // for (; itClasses != ...

		iPropertyInstance = GetPropertiesByIterator(m_iModel, iPropertyInstance);
	} // while (iPropertyInstance != 0)

	// Instances
	LoadRDFInstances();
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::EnableInstancesRecursively(CRDFInstance* iRDFInstance)
{
	if (iRDFInstance->getEnable() == false) {
		iRDFInstance->setEnable(true);

		if (iRDFInstance->GetConceptualFacesCount() == 0) {
			auto& mapRFDInstances = GetInstances();

			//
			//	Walk over all relations (object properties)
			//
			RdfProperty myProperty = GetInstancePropertyByIterator(iRDFInstance->GetInstance(), 0);
			while (myProperty) {
				if (GetPropertyType(myProperty) == OBJECTPROPERTY_TYPE) {
					int64_t card = 0;
					OwlInstance* values = nullptr;
					GetObjectProperty(iRDFInstance->GetInstance(), myProperty, &values, &card);
					for (int64_t i = 0; i < card; i++) {
						if (values[i]) {
							auto itRFDInstance = mapRFDInstances.find(values[i]);

							if (!itRFDInstance->second->getEnable())
								EnableInstancesRecursively(itRFDInstance->second);
						}
					}
				}
				myProperty = GetInstancePropertyByIterator(iRDFInstance->GetInstance(), myProperty);
			}
		}
	}
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::SetDefaultEnabledInstances()
{
	ProgressStatus prgs (L"Collect visible instances");

	auto& mapRFDInstances = GetInstances();

	auto itRFDInstances = mapRFDInstances.begin();
	for (; itRFDInstances != mapRFDInstances.end(); itRFDInstances++)
	{
		if (itRFDInstances->second->getEnable() &&
			!GetInstanceGeometryClass(itRFDInstances->second->GetInstance())) {
			itRFDInstances->second->setEnable(false);
			EnableInstancesRecursively(itRFDInstances->second);
		}
	}
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::LoadRDFInstances()
{
	/*
	* Default instances
	*/
	ProgressStatus prgs(L"Loading RDF instances");

	int64_t iInstance = GetInstancesByIterator(m_iModel, 0);

	int64_t cntTotal = 0;
	for (auto i = iInstance; i; i = GetInstancesByIterator(m_iModel, i)) {
		cntTotal++;
	}
	
	prgs.Start(cntTotal);

	while (iInstance != 0)
	{
		prgs.Step();

		CRDFInstance* pInstance = nullptr;

		auto itInstance = m_mapInstances.find(iInstance);
		if (itInstance == m_mapInstances.end())
		{
			pInstance = new CRDFInstance(m_iID++, iInstance);
			m_mapInstances[iInstance] = pInstance;
		}
		else
		{
			pInstance = itInstance->second;
		}

		pInstance->setEnable(!pInstance->IsReferenced());

		iInstance = GetInstancesByIterator(m_iModel, iInstance);
	} // while (iInstance != 0)

	prgs.Finish();

	SetDefaultEnabledInstances();

	/**
	* Scale and Center
	*/

	ScaleAndCenter();
}

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
	* RDF Classes
	*/
	auto itClasses = m_mapClasses.begin();
	for (; itClasses != m_mapClasses.end(); itClasses++)
	{
		delete itClasses->second;
	}
	m_mapClasses.clear();

	/*
	* RDF Properties
	*/
	auto itProperty = m_mapProperties.begin();
	for (; itProperty != m_mapProperties.end(); itProperty++)
	{
		delete itProperty->second;
	}
	m_mapProperties.clear();

	/*
	* RDF Instances
	*/
	auto itInstance = m_mapInstances.begin();
	for (; itInstance != m_mapInstances.end(); itInstance++)
	{
		delete itInstance->second;
	}
	m_mapInstances.clear();

	m_mapInstanceMetaData.clear();
	
	/*
	* Texture
	*/
	delete m_pDefaultTexture;
	m_pDefaultTexture = nullptr;

	for (auto itTexure : m_mapTextures)
	{
		delete itTexure.second;
	}
	m_mapTextures.clear();

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
	GetClassPropertyAggregatedCardinalityRestriction(iRDFClass, iRDFProperty, pMinCard, pMaxCard);
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

	_vector3d vecBoundingBoxMin = { dXmin, dYmin, dZmin };
	_vector3d vecBoundingBoxMax = { dXmax, dYmax, dZmax };

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

	_vector3d vecMin1 = { vecBoundingBoxMin.x, vecBoundingBoxMin.y, vecBoundingBoxMin.z };
	_vector3d vecMin2 = { vecBoundingBoxMax.x, vecBoundingBoxMin.y, vecBoundingBoxMin.z };
	_vector3d vecMin3 = { vecBoundingBoxMax.x, vecBoundingBoxMin.y, vecBoundingBoxMax.z };
	_vector3d vecMin4 = { vecBoundingBoxMin.x, vecBoundingBoxMin.y, vecBoundingBoxMax.z };

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

	_vector3d vecMax1 = { vecBoundingBoxMax.x, vecBoundingBoxMax.y, vecBoundingBoxMax.z };
	_vector3d vecMax2 = { vecBoundingBoxMin.x, vecBoundingBoxMax.y, vecBoundingBoxMax.z };
	_vector3d vecMax3 = { vecBoundingBoxMin.x, vecBoundingBoxMax.y, vecBoundingBoxMin.z };
	_vector3d vecMax4 = { vecBoundingBoxMax.x, vecBoundingBoxMax.y, vecBoundingBoxMin.z };

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