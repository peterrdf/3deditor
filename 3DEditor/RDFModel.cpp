#include "stdafx.h"

#include "RDFModel.h"
#include "Generic.h"
#include "ProgressIndicator.h"
#include "ProgressDialog.h"
#include "gisengine.h"

#include "_dxf_parser.h"
#include "gisengine.h"
#include "ascii.h"

#include <bitset>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <time.h>
using namespace std;

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

// ************************************************************************************************
CProgressDialog* m_pProgressDialog = nullptr;

// ************************************************************************************************
void STDCALL LogCallbackImpl(int/*enumLogEvent*/ enLogEvent, const char* szEvent)
{
	ASSERT(m_pProgressDialog != nullptr);

	m_pProgressDialog->Log(enLogEvent, szEvent);
}

// ************************************************************************************************
class CLoadTask : public CTask
{

private: // Members

	CRDFModel* m_pModel;
	const wchar_t* m_szPath;
	bool m_bLoading;

public: // Methods

	CLoadTask(CRDFModel* pModel, const wchar_t* szPath, bool bLoading)
		: CTask()
		, m_pModel(pModel)
		, m_szPath(szPath)
		, m_bLoading(bLoading)
	{
		ASSERT(m_pModel != nullptr);
		ASSERT(szPath != nullptr);
	}

	virtual ~CLoadTask()
	{}

	virtual void Run() override
	{
		ASSERT(m_pProgressDialog != nullptr);

		CString strLog;
		if (m_bLoading)
		{
			strLog.Format(_T("*** Loading '%s' ***"), m_szPath);
		}
		else
		{
			strLog.Format(_T("*** Importing '%s' ***"), m_szPath);
		}		

		m_pProgressDialog->Log(0/*enumLogEvent::info*/, CW2A(strLog));

		CString strExtension = PathFindExtension(m_szPath);
		strExtension.MakeUpper();

		if (strExtension == L".DXF")
		{
			m_pModel->LoadDXF(m_szPath);
		}
		else if ((strExtension == L".GML") ||
			(strExtension == L".CITYGML") ||
			(strExtension == L".XML") ||
			(strExtension == L".JSON"))
		{
			m_pModel->LoadGISModel(m_szPath);
		}
		else
		{
			if (m_bLoading)
			{
				m_pModel->m_iModel = OpenModelW(m_szPath);
				if (m_pModel->m_iModel != 0)
				{
					m_pModel->SetFormatSettings(m_pModel->m_iModel);

					m_pModel->LoadRDFModel();
				}
			}
			else
			{
				ASSERT(m_pModel->m_iModel != 0);

				ImportModelW(m_pModel->m_iModel, m_szPath);

				m_pModel->LoadRDFModel();
			}
		}

		if (m_pModel->m_iModel == 0)
		{
			CString strError;
			strError.Format(L"Failed to open '%s'.", m_szPath);

			m_pProgressDialog->Log(2/*enumLogEvent::error*/, CW2A(strError));
			::MessageBox(m_pProgressDialog->GetSafeHwnd(), strError, L"Error", MB_ICONERROR | MB_OK);
		}
		else
		{
			m_pProgressDialog->Log(0/*enumLogEvent::info*/, "*** Done. ***");
		}
	}
};

// ************************************************************************************************
CRDFModel::CRDFModel()
	: m_strModel(L"")
	, m_iModel(0)
	, m_mapClasses()
	, m_mapProperties()
	, m_mapInstances()
	, m_mapInstanceDefaultState()
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
	, m_pTextBuilder(new CTextBuilder())
{
}

// ------------------------------------------------------------------------------------------------
CRDFModel::~CRDFModel()
{
	delete m_pTextBuilder;

	Clean();
}

// ------------------------------------------------------------------------------------------------
int64_t CRDFModel::GetModel() const
{
	return m_iModel;
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFModel::CreateDefaultModel()
{
	Clean();
	
	m_iModel = CreateModel();
	ASSERT(m_iModel != 0);	

	SetFormatSettings(m_iModel);

	// Cube 1
	{
		auto pAmbient = GEOM::ColorComponent::Create(m_iModel);
		pAmbient.set_R(0.);
		pAmbient.set_G(1.);
		pAmbient.set_B(0.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(m_iModel);
		pColor.set_ambient(pAmbient);

		auto pTexture = GEOM::Texture::Create(m_iModel);
		pTexture.set_scalingX(1.);
		pTexture.set_scalingY(1.);
		vector<GEOM::Texture> vecTexures = { pTexture };

		auto pMaterial = GEOM::Material::Create(m_iModel);
		pMaterial.set_color(pColor);
		pMaterial.set_textures(&vecTexures[0], 1);

		auto pCube = GEOM::Cube::Create(m_iModel, "Cube 1");
		pCube.set_material(pMaterial);
		pCube.set_length(7.);
	}

	// Cone 1
	{
		auto pAmbient = GEOM::ColorComponent::Create(m_iModel);
		pAmbient.set_R(0.);
		pAmbient.set_G(0.);
		pAmbient.set_B(1.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(m_iModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(m_iModel);
		pMaterial.set_color(pColor);

		auto pCone = GEOM::Cone::Create(m_iModel, "Cone 1");
		pCone.set_material(pMaterial);
		pCone.set_radius(4.);
		pCone.set_height(12.);
		pCone.set_segmentationParts(36);
	}	

	// Cylinder 1
	{
		auto pAmbient = GEOM::ColorComponent::Create(m_iModel);
		pAmbient.set_R(1.);
		pAmbient.set_G(0.);
		pAmbient.set_B(0.);
		pAmbient.set_W(.5);

		auto pColor = GEOM::Color::Create(m_iModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(m_iModel);
		pMaterial.set_color(pColor);

		auto pCylinder = GEOM::Cylinder::Create(m_iModel, "Cylinder 1");
		pCylinder.set_material(pMaterial);
		pCylinder.set_radius(6.);
		pCylinder.set_length(6.);
		pCylinder.set_segmentationParts(36);
	}	

	// ASCII Chars
	m_pTextBuilder->Initialize(m_iModel);

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

	auto pInstance = new CRDFInstance(m_iID++, iInstance, true);
	pInstance->CalculateMinMax(m_fXmin, m_fXmax, m_fYmin, m_fYmax, m_fZmin, m_fZmax);

	m_mapInstances[iInstance] = pInstance;

	return pInstance;
}

// ------------------------------------------------------------------------------------------------
CRDFInstance* CRDFModel::AddNewInstance(int64_t pThing)
{
	auto pInstance = new CRDFInstance(m_iID++, pThing, true);
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
	/* Import */
	Load(szPath, false);
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
/*virtual*/ void CRDFModel::ScaleAndCenter(bool bLoadingModel/* = false*/)
{
	ProgressStatus stat(L"Calculate scene sizes");

	/* World */
	m_fBoundingSphereDiameter = 0.f;

	/* Translations */
	m_fXTranslation = 0.f;
	m_fYTranslation = 0.f;
	m_fZTranslation = 0.f;

	/* Min/Max */
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

		if (!bLoadingModel)
		{
			itInstance->second->ResetScaleAndCenter();
		}
		
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
		::MessageBox(
			m_pProgressDialog != nullptr ? m_pProgressDialog->GetSafeHwnd() : ::AfxGetMainWnd()->GetSafeHwnd(), 
			_T("Internal error."), _T("Error"), MB_ICONERROR | MB_OK);

		return;
	}
	
	/* World */
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
	TRACE(L"\n*** Scale and Center, Bounding sphere I *** =>  %.16f", m_fBoundingSphereDiameter);

	/* Scale and Center */
	itInstance = m_mapInstances.begin();
	for (; itInstance != m_mapInstances.end(); itInstance++)
	{
		if (!itInstance->second->getEnable())
		{
			continue;
		}

		itInstance->second->ScaleAndCenter(
			m_fXmin, m_fXmax, 
			m_fYmin, m_fYmax, 
			m_fZmin, m_fZmax, 
			m_fBoundingSphereDiameter,
			true);
	}

	/* Min/Max */
	m_fXmin = FLT_MAX;
	m_fXmax = -FLT_MAX;
	m_fYmin = FLT_MAX;
	m_fYmax = -FLT_MAX;
	m_fZmin = FLT_MAX;
	m_fZmax = -FLT_MAX;

	itInstance = m_mapInstances.begin();
	for (; itInstance != m_mapInstances.end(); itInstance++)
	{
		if (!itInstance->second->getEnable())
		{
			continue;
		}

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
		::MessageBox(
			m_pProgressDialog != nullptr ? m_pProgressDialog->GetSafeHwnd() : ::AfxGetMainWnd()->GetSafeHwnd(), 
			_T("Internal error."), _T("Error"), MB_ICONERROR | MB_OK);

		return;
	}

	/* World */
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
	TRACE(L"\n*** Scale and Center, Bounding sphere II *** =>  %.16f", m_fBoundingSphereDiameter);

	/* Translations */
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
void CRDFModel::OnInstanceNameEdited(CRDFInstance* pInstance)
{
	auto itMetaData = m_mapInstanceMetaData.find(pInstance);
	if (itMetaData != m_mapInstanceMetaData.end())
	{
		m_mapInstanceMetaData.erase(pInstance);
	}
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
void CRDFModel::Load(const wchar_t * szPath, bool bLoading)
{
	if (bLoading)
	{
		Clean();

		m_strModel = szPath;
	}	

	CLoadTask loadTask(this, szPath, bLoading);
	CProgressDialog dlgProgress(::AfxGetMainWnd(), &loadTask);

	m_pProgressDialog = &dlgProgress;
	dlgProgress.DoModal();
	m_pProgressDialog = nullptr;
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::LoadDXF(const wchar_t* szPath)
{
	if (m_iModel == 0)
	{
		m_iModel = CreateModel();
		ASSERT(m_iModel != 0);

		SetFormatSettings(m_iModel);
	}		

	try
	{
		_dxf::_parser parser(m_iModel);
		parser.load(CW2A(szPath));
	}
	catch (const std::runtime_error& ex)
	{
		::MessageBox(
			m_pProgressDialog != nullptr ? m_pProgressDialog->GetSafeHwnd() : ::AfxGetMainWnd()->GetSafeHwnd(), 
			CA2W(ex.what()), L"Error", MB_ICONERROR | MB_OK);

		return;
	}

	LoadRDFModel();
}

void CRDFModel::LoadGISModel(const wchar_t* szPath)
{
	if (m_iModel == 0)
	{
		m_iModel = CreateModel();
		ASSERT(m_iModel != 0);

		SetFormatSettings(m_iModel);
	}

	try
	{
		wchar_t szAppPath[_MAX_PATH];
		::GetModuleFileName(::GetModuleHandle(nullptr), szAppPath, sizeof(szAppPath));

		fs::path pthExe = szAppPath;
		auto pthRootFolder = pthExe.parent_path();
		string strRootFolder = pthRootFolder.string();
		strRootFolder += "\\";

		SetGISOptions(strRootFolder.c_str(), true, LogCallbackImpl);

		ImportGISModel(m_iModel, CW2A(szPath));
	}
	catch (const std::runtime_error& err)
	{
		::MessageBox(
			m_pProgressDialog != nullptr ? m_pProgressDialog->GetSafeHwnd() : ::AfxGetMainWnd()->GetSafeHwnd(), 
			CA2W(err.what()), L"Error", MB_ICONERROR | MB_OK);

		return;
	}
	catch (...)
	{
		::MessageBox(
			m_pProgressDialog != nullptr ? m_pProgressDialog->GetSafeHwnd() : ::AfxGetMainWnd()->GetSafeHwnd(), 
			L"Unknown error.", L"Error", MB_ICONERROR | MB_OK);
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

void CRDFModel::GetPropertyMetaData(CRDFInstance* pInstance, CRDFProperty* pProperty, CString& strMetaData, const CString& strPrefix/* = L""*/)
{
	strMetaData += L"\n";
	strMetaData += strPrefix;
	strMetaData += pProperty->GetName() != nullptr ? pProperty->GetName() : L"NA";
	strMetaData += L": ";

	/* value */
	wchar_t szBuffer[1000];
	switch (pProperty->getType())
	{
		case OBJECTPROPERTY_TYPE:
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

					GetPropertyMetaData(itInstance->second, itProperty->second, strMetaData, L" - ");

					iPropertyInstance = GetInstancePropertyByIterator(piInstances[0], iPropertyInstance);
				} // while (iPropertyInstance != 0)
			}
			else
			{
				strMetaData += iCard > 0 ? L"[...]" : L"[]";
			}
		}
		break;

		case DATATYPEPROPERTY_TYPE_BOOLEAN:
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

		case DATATYPEPROPERTY_TYPE_STRING:
		{
			int64_t iCard = 0;
			wchar_t** szValue = nullptr;
			SetCharacterSerialization(pInstance->GetModel(), 0, 0, false);
			GetDatatypeProperty(pInstance->GetInstance(), pProperty->GetInstance(), (void**)&szValue, &iCard);
			SetCharacterSerialization(pInstance->GetModel(), 0, 0, true);

			if (iCard == 1)
			{
				strMetaData += szValue[0];
			}
			else
			{
				strMetaData += iCard > 0 ? L"[...]" : L"[]";
			}
		}
		break;

		case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
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

		case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
		{
			int64_t iCard = 0;
			wchar_t** szValue = nullptr;
			GetDatatypeProperty(pInstance->GetInstance(), pProperty->GetInstance(), (void**)&szValue, &iCard);

			if (iCard == 1)
			{
				strMetaData += szValue[0];
			}
			else
			{
				strMetaData += iCard > 0 ? L"[...]" : L"[]";
			}
		}
		break;

		case DATATYPEPROPERTY_TYPE_DOUBLE:
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

		case DATATYPEPROPERTY_TYPE_INTEGER:
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

	PreLoadDRFModel();

	// Load/Import Model
	int64_t	iClassInstance = GetClassesByIterator(m_iModel, 0);
	while (iClassInstance != 0)
	{
		auto itClass = m_mapClasses.find(iClassInstance);
		if (itClass == m_mapClasses.end())
		{
			m_mapClasses[iClassInstance] = new CRDFClass(iClassInstance);
		}

		iClassInstance = GetClassesByIterator(m_iModel, iClassInstance);
	} // while (iClassInstance != 0)

	// Load/Import Model
	int64_t iPropertyInstance = GetPropertiesByIterator(m_iModel, 0);
	while (iPropertyInstance != 0)
	{
		auto itProperty = m_mapProperties.find(iPropertyInstance);
		if (itProperty == m_mapProperties.end())
		{
			int64_t iPropertyType = GetPropertyType(iPropertyInstance);
			switch (iPropertyType)
			{
			case OBJECTPROPERTY_TYPE:
			{
				m_mapProperties[iPropertyInstance] = new CObjectRDFProperty(iPropertyInstance);
			}
			break;

			case DATATYPEPROPERTY_TYPE_BOOLEAN:
			{
				m_mapProperties[iPropertyInstance] = new CBoolRDFProperty(iPropertyInstance);
			}
			break;

			case DATATYPEPROPERTY_TYPE_STRING:
			{
				m_mapProperties[iPropertyInstance] = new CStringRDFProperty(iPropertyInstance);
			}
			break;

			case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
			{
				m_mapProperties[iPropertyInstance] = new CCharArrayRDFProperty(iPropertyInstance);
			}
			break;

			case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
			{
				m_mapProperties[iPropertyInstance] = new CWCharArrayRDFProperty(iPropertyInstance);
			}
			break;

			case DATATYPEPROPERTY_TYPE_INTEGER:
			{
				m_mapProperties[iPropertyInstance] = new CIntRDFProperty(iPropertyInstance);
			}
			break;

			case DATATYPEPROPERTY_TYPE_DOUBLE:
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

			auto itClasses = m_mapClasses.begin();
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
		} // if (itProperty == m_mapProperties.end())

		iPropertyInstance = GetPropertiesByIterator(m_iModel, iPropertyInstance);
	} // while (iPropertyInstance != 0)

	PostLoadDRFModel();

	// Instances
	LoadRDFInstances();
}

/*virtual*/ void CRDFModel::PostLoadDRFModel()
{
	SetInstancesDefaultState();

	UpdateVertexBufferOffset();
}

void CRDFModel::SetInstancesDefaultState()
{
	m_mapInstanceDefaultState.clear();

	// Enable only unreferenced instances
	OwlInstance iInstance = GetInstancesByIterator(m_iModel, 0);
	while (iInstance != 0)
	{
		m_mapInstanceDefaultState[iInstance] = GetInstanceInverseReferencesByIterator(iInstance, 0) == 0;

		iInstance = GetInstancesByIterator(m_iModel, iInstance);
	}

	// Enable children/descendants with geometry
	for (auto& itInstanceDefaultState : m_mapInstanceDefaultState)
	{
		if (!itInstanceDefaultState.second)
		{
			continue;
		}

		if (!GetInstanceGeometryClass(itInstanceDefaultState.first) ||
			!GetBoundingBox(itInstanceDefaultState.first, nullptr, nullptr))
		{
			char* szClassName = nullptr;
			GetNameOfClass(GetInstanceClass(itInstanceDefaultState.first), &szClassName);

			if (strcmp(szClassName, "Nill") != 0)
			{
				SetInstanceDefaultStateRecursive(itInstanceDefaultState.first);
			}
		}
	}
}

void CRDFModel::SetInstanceDefaultStateRecursive(OwlInstance iInstance)
{
	ASSERT(iInstance != 0);

	RdfProperty iProperty = GetInstancePropertyByIterator(iInstance, 0);
	while (iProperty != 0)
	{
		if (GetPropertyType(iProperty) == OBJECTPROPERTY_TYPE)
		{
			int64_t iValuesCount = 0;
			OwlInstance* piValues = nullptr;
			GetObjectProperty(iInstance, iProperty, &piValues, &iValuesCount);

			for (int64_t iValue = 0; iValue < iValuesCount; iValue++)
			{
				if ((piValues[iValue] != 0) &&
					!m_mapInstanceDefaultState.at(piValues[iValue]))
				{
					// Enable to avoid infinity recursion
					m_mapInstanceDefaultState.at(piValues[iValue]) = true;

					if (!GetInstanceGeometryClass(piValues[iValue]) ||
						!GetBoundingBox(piValues[iValue], nullptr, nullptr))
					{
						SetInstanceDefaultStateRecursive(piValues[iValue]);
					}
				}
			}
		}

		iProperty = GetInstancePropertyByIterator(iInstance, iProperty);
	}
}

void CRDFModel::UpdateVertexBufferOffset()
{
	/* Min/Max */
	double dXmin = DBL_MAX;
	double dXmax = -DBL_MAX;
	double dYmin = DBL_MAX;
	double dYmax = -DBL_MAX;
	double dZmin = DBL_MAX;
	double dZmax = -DBL_MAX;

	OwlInstance iInstance = GetInstancesByIterator(m_iModel, 0);
	while (iInstance != 0)
	{
		if (m_mapInstanceDefaultState.at(iInstance))
		{
			CRDFInstance::CalculateBBMinMax(
				iInstance,
				dXmin, dXmax,
				dYmin, dYmax,
				dZmin, dZmax);
		}

		iInstance = GetInstancesByIterator(m_iModel, iInstance);
	}

	if ((dXmin == DBL_MAX) ||
		(dXmax == -DBL_MAX) ||
		(dYmin == DBL_MAX) ||
		(dYmax == -DBL_MAX) ||
		(dZmin == DBL_MAX) ||
		(dZmax == -DBL_MAX))
	{
		::MessageBox(
			m_pProgressDialog != nullptr ? m_pProgressDialog->GetSafeHwnd() : ::AfxGetMainWnd()->GetSafeHwnd(),
			_T("Internal error."), _T("Error"), MB_ICONERROR | MB_OK);

		return;
	}

	TRACE(L"\n*** SetVertexBufferOffset *** => x/y/z: %.16f, %.16f, %.16f",
		-(dXmin + dXmax) / 2.,
		-(dYmin + dYmax) / 2.,
		-(dZmin + dZmax) / 2.);

	// http://rdf.bg/gkdoc/CP64/SetVertexBufferOffset.html
	SetVertexBufferOffset(
		m_iModel,
		-(dXmin + dXmax) / 2.,
		-(dYmin + dYmax) / 2.,
		-(dZmin + dZmax) / 2.);

	// http://rdf.bg/gkdoc/CP64/ClearedExternalBuffers.html
	ClearedExternalBuffers(m_iModel);
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

		auto itInstance = m_mapInstances.find(iInstance);
		if (itInstance == m_mapInstances.end())
		{
			// Load Model
			m_mapInstances[iInstance] = new CRDFInstance(m_iID++, iInstance, m_mapInstanceDefaultState.at(iInstance));
		}
		else
		{
			// Import Model
			itInstance->second->Recalculate();
		}

		iInstance = GetInstancesByIterator(m_iModel, iInstance);
	} // while (iInstance != 0)

	prgs.Finish();

	/**
	* Scale and Center
	*/

	ScaleAndCenter(true);
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
}

OwlInstance CRDFModel::Translate(
	OwlInstance iInstance, 
	double dX, double dY, double dZ,
	double d11, double d22, double d33)
{
	ASSERT(iInstance != 0);

	int64_t iMatrixInstance = CreateInstance(GetClassByName(m_iModel, "Matrix"));
	ASSERT(iMatrixInstance != 0);

	vector<double> vecTransformationMatrix =
	{
		d11, 0., 0.,
		0., d22, 0.,
		0., 0., d33,
		dX, dY, dZ,
	};

	SetDatatypeProperty(
		iMatrixInstance,
		GetPropertyByName(m_iModel, "coordinates"),
		vecTransformationMatrix.data(),
		vecTransformationMatrix.size());

	int64_t iTransformationInstance = CreateInstance(GetClassByName(m_iModel, "Transformation"));
	ASSERT(iTransformationInstance != 0);

	SetObjectProperty(iTransformationInstance, GetPropertyByName(m_iModel, "matrix"), &iMatrixInstance, 1);
	SetObjectProperty(iTransformationInstance, GetPropertyByName(m_iModel, "object"), &iInstance, 1);

	return iTransformationInstance;
}

OwlInstance CRDFModel::Scale(OwlInstance iInstance, double dFactor)
{
	ASSERT(iInstance != 0);

	int64_t iMatrixInstance = CreateInstance(GetClassByName(m_iModel, "Matrix"));
	ASSERT(iMatrixInstance != 0);

	vector<double> vecTransformationMatrix =
	{
		dFactor, 0., 0.,
		0., dFactor, 0.,
		0., 0., dFactor,
		0., 0., 0.,
	};

	SetDatatypeProperty(
		iMatrixInstance,
		GetPropertyByName(m_iModel, "coordinates"),
		vecTransformationMatrix.data(),
		vecTransformationMatrix.size());

	int64_t iTransformationInstance = CreateInstance(GetClassByName(m_iModel, "Transformation"));
	ASSERT(iTransformationInstance != 0);

	SetObjectProperty(iTransformationInstance, GetPropertyByName(m_iModel, "matrix"), &iMatrixInstance, 1);
	SetObjectProperty(iTransformationInstance, GetPropertyByName(m_iModel, "object"), &iInstance, 1);

	return iTransformationInstance;
}

OwlInstance CRDFModel::Rotate(
	OwlInstance iInstance,
	double alpha, double beta, double gamma)
{
	ASSERT(iInstance != 0);

	int64_t iMatrixInstance = CreateInstance(GetClassByName(m_iModel, "Matrix"));
	ASSERT(iMatrixInstance != 0);

	_matrix matrix;
	memset(&matrix, 0, sizeof(_matrix));
	_matrixRotateByEulerAngles(&matrix, alpha, beta, gamma);

	SetDatatypeProperty(iMatrixInstance, GetPropertyByName(m_iModel, "_11"), &matrix._11, 1);
	SetDatatypeProperty(iMatrixInstance, GetPropertyByName(m_iModel, "_12"), &matrix._12, 1);
	SetDatatypeProperty(iMatrixInstance, GetPropertyByName(m_iModel, "_13"), &matrix._13, 1);

	SetDatatypeProperty(iMatrixInstance, GetPropertyByName(m_iModel, "_21"), &matrix._21, 1);
	SetDatatypeProperty(iMatrixInstance, GetPropertyByName(m_iModel, "_22"), &matrix._22, 1);
	SetDatatypeProperty(iMatrixInstance, GetPropertyByName(m_iModel, "_23"), &matrix._23, 1);

	SetDatatypeProperty(iMatrixInstance, GetPropertyByName(m_iModel, "_31"), &matrix._31, 1);
	SetDatatypeProperty(iMatrixInstance, GetPropertyByName(m_iModel, "_32"), &matrix._32, 1);
	SetDatatypeProperty(iMatrixInstance, GetPropertyByName(m_iModel, "_33"), &matrix._33, 1);

	int64_t iTransformationInstance = CreateInstance(GetClassByName(m_iModel, "Transformation"));
	ASSERT(iTransformationInstance != 0);

	SetObjectProperty(iTransformationInstance, GetPropertyByName(m_iModel, "matrix"), &iMatrixInstance, 1);
	SetObjectProperty(iTransformationInstance, GetPropertyByName(m_iModel, "object"), &iInstance, 1);

	return iTransformationInstance;
}

// ------------------------------------------------------------------------------------------------
void CRDFModel::GetClassPropertyCardinalityRestrictionNested(int64_t iRDFClass, int64_t iRDFProperty, int64_t * pMinCard, int64_t * pMaxCard)
{
	GetClassPropertyAggregatedCardinalityRestriction(iRDFClass, iRDFProperty, pMinCard, pMaxCard);
}

// ************************************************************************************************
CSceneRDFModel::CSceneRDFModel()
	: CRDFModel()
{}

/*virtual*/ CSceneRDFModel::~CSceneRDFModel()
{}

/*virtual*/ void CSceneRDFModel::CreateDefaultModel() /*override*/
{
	Clean();

	m_iModel = CreateModel();
	ASSERT(m_iModel != 0);

	SetFormatSettings(m_iModel);

	// ASCII Chars
	m_pTextBuilder->Initialize(m_iModel);

	CreateCoordinateSystem();

	LoadRDFModel();
}

/*virtual*/ void CSceneRDFModel::PostLoadDRFModel() /*override*/
{
	SetInstancesDefaultState();
}

void CSceneRDFModel::CreateCoordinateSystem()
{
	const double AXIS_LENGTH = 2.5;

	// Coordinate System
	vector<OwlInstance> vecInstances;

	// Coordinate System/X (1 Line3D)
	OwlInstance iXAxisMaterial = 0;
	{
		auto pAmbient = GEOM::ColorComponent::Create(m_iModel);
		pAmbient.set_R(1.);
		pAmbient.set_G(0.);
		pAmbient.set_B(0.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(m_iModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(m_iModel);
		pMaterial.set_color(pColor);

		iXAxisMaterial = (int64_t)pMaterial;

		vector<double> vecPoints =
		{
			0., 0., 0.,
			AXIS_LENGTH / 2., 0., 0.,
		};

		auto pXAxis = GEOM::Line3D::Create(m_iModel);
		pXAxis.set_material(pMaterial);
		pXAxis.set_points(vecPoints.data(), vecPoints.size());

		vecInstances.push_back((int64_t)pXAxis);
	}

	// Coordinate System/Y (Line3D)
	OwlInstance iYAxisMaterial = 0;
	{
		auto pAmbient = GEOM::ColorComponent::Create(m_iModel);
		pAmbient.set_R(0.);
		pAmbient.set_G(1.);
		pAmbient.set_B(0.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(m_iModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(m_iModel);
		pMaterial.set_color(pColor);

		iYAxisMaterial = (int64_t)pMaterial;

		vector<double> vecPoints =
		{
			0., 0., 0.,
			0., AXIS_LENGTH / 2., 0.,
		};

		auto pYAxis = GEOM::Line3D::Create(m_iModel);
		pYAxis.set_material(pMaterial);
		pYAxis.set_points(vecPoints.data(), vecPoints.size());

		vecInstances.push_back((int64_t)pYAxis);
	}

	// Coordinate System/Z (Line3D)
	OwlInstance iZAxisMaterial = 0;
	{
		auto pAmbient = GEOM::ColorComponent::Create(m_iModel);
		pAmbient.set_R(0.);
		pAmbient.set_G(0.);
		pAmbient.set_B(1.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(m_iModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(m_iModel);
		pMaterial.set_color(pColor);

		iZAxisMaterial = (int64_t)pMaterial;

		vector<double> vecPoints =
		{
			0., 0., 0.,
			0., 0., AXIS_LENGTH / 2.,
		};

		auto pZAxis = GEOM::Line3D::Create(m_iModel);
		pZAxis.set_material(pMaterial);
		pZAxis.set_points(vecPoints.data(), vecPoints.size());

		vecInstances.push_back((int64_t)pZAxis);
	}

	// Arrows (1 Cone => 3 Transformations)
	{
		const double ARROW_OFFSET = 2.5;

		auto pArrow = GEOM::Cone::Create(m_iModel);
		pArrow.set_height(AXIS_LENGTH / 15.);
		pArrow.set_radius(.075);

		// +X
		{
			OwlInstance iPlusXInstance = Translate(
				Rotate((int64_t)pArrow, 0., 2 * PI * 90. / 360., 0.),
				ARROW_OFFSET / 2., 0., 0.,
				1., 1., 1.);
			SetNameOfInstance(iPlusXInstance, "#(+X)");

			SetObjectProperty(
				iPlusXInstance,
				GetPropertyByName(m_iModel, "material"),
				&iXAxisMaterial,
				1);

			vecInstances.push_back(iPlusXInstance);
		}

		// +Y
		{
			OwlInstance iPlusYInstance = Translate(
				Rotate((int64_t)pArrow, 2 * PI * 270. / 360., 0., 0.),
				0., ARROW_OFFSET / 2., 0.,
				1., 1., 1.);
			SetNameOfInstance(iPlusYInstance, "#(+Y)");

			SetObjectProperty(
				iPlusYInstance,
				GetPropertyByName(m_iModel, "material"),
				&iYAxisMaterial,
				1);

			vecInstances.push_back(iPlusYInstance);
		}

		// +Z
		{
			OwlInstance iPlusZInstance = Translate(
				(int64_t)pArrow,
				0., 0., ARROW_OFFSET / 2.,
				1., 1., 1.);
			SetNameOfInstance(iPlusZInstance, "#(+Z)");

			SetObjectProperty(
				iPlusZInstance,
				GetPropertyByName(m_iModel, "material"),
				&iZAxisMaterial,
				1);

			vecInstances.push_back(iPlusZInstance);
		}
	}

	/* Labels */
	double dXmin = DBL_MAX;
	double dXmax = -DBL_MAX;
	double dYmin = DBL_MAX;
	double dYmax = -DBL_MAX;
	double dZmin = DBL_MAX;
	double dZmax = -DBL_MAX;

	// X-axis
	OwlInstance iPlusXLabelInstance = m_pTextBuilder->BuildText("X-axis", true);
	ASSERT(iPlusXLabelInstance != 0);

	CRDFInstance::CalculateBBMinMax(
		iPlusXLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	// Y-axis
	OwlInstance iPlusYLabelInstance = m_pTextBuilder->BuildText("Y-axis", true);
	ASSERT(iPlusYLabelInstance != 0);

	CRDFInstance::CalculateBBMinMax(
		iPlusYLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	// Z-axis
	OwlInstance iPlusZLabelInstance = m_pTextBuilder->BuildText("Z-axis", true);
	ASSERT(iPlusZLabelInstance != 0);

	CRDFInstance::CalculateBBMinMax(
		iPlusZLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Scale Factor */
	double dMaxLength = dXmax - dXmin;
	dMaxLength = fmax(dMaxLength, dYmax - dYmin);
	dMaxLength = fmax(dMaxLength, dZmax - dZmin);

	double dScaleFactor = ((AXIS_LENGTH / 2.) * .75) / dMaxLength;

	/* Transform Labels */

	// X-axis
	OwlInstance iInstance = Translate(
		Rotate(Scale(iPlusXLabelInstance, dScaleFactor / 2.), 2 * PI * 90. / 360., 0., 2 * PI * 90. / 180.),
		AXIS_LENGTH / 1.4, 0., 0.,
		-1., 1., 1.);
	
	SetNameOfInstance(iInstance, "#X-axis");
	SetObjectProperty(
		iInstance,
		GetPropertyByName(m_iModel, "material"),
		&iXAxisMaterial,
		1);

	// Y-axis
	iInstance = Translate(
		Rotate(Scale(iPlusYLabelInstance, dScaleFactor / 2.), 2 * PI * 90. / 360., 0., 2 * PI * 90. / 360.),
		0., AXIS_LENGTH / 1.4, 0.,
		-1., 1., 1.);

	SetNameOfInstance(iInstance, "#Y-axis");
	SetObjectProperty(
		iInstance,
		GetPropertyByName(m_iModel, "material"),
		&iYAxisMaterial,
		1);

	// Z-axis
	iInstance = Translate(
		Rotate(Scale(iPlusZLabelInstance, dScaleFactor / 2.), 2 * PI * 270. / 360., 2 * PI * 90. / 360., 0.),
		0., 0., AXIS_LENGTH / 1.4,
		1., 1., -1.);

	SetNameOfInstance(iInstance, "#Z-axis");
	SetObjectProperty(
		iInstance,
		GetPropertyByName(m_iModel, "material"),
		&iZAxisMaterial,
		1);

	/* Collection */
	OwlInstance iCollectionInstance = CreateInstance(GetClassByName(m_iModel, "Collection"), "#Coordinate System#");
	ASSERT(iCollectionInstance != 0);

	SetObjectProperty(
		iCollectionInstance,
		GetPropertyByName(m_iModel, "objects"),
		vecInstances.data(),
		vecInstances.size());
}

// ************************************************************************************************
CNavigatorRDFModel::CNavigatorRDFModel()
	: CSceneRDFModel()
{}

/*virtual*/ CNavigatorRDFModel::~CNavigatorRDFModel()
{}

/*virtual*/ void CNavigatorRDFModel::CreateDefaultModel() /*override*/
{
	Clean();

	m_iModel = CreateModel();
	ASSERT(m_iModel != 0);

	SetFormatSettings(m_iModel);

	// ASCII Chars
	m_pTextBuilder->Initialize(m_iModel);

	CreateCoordinateSystem();

	CreateNaigator();

	CreateNaigatorLabels();

	LoadRDFModel();
}

void CNavigatorRDFModel::CreateNaigator()
{
	// Cube (BoundaryRepresentations)
	{
		auto pAmbient = GEOM::ColorComponent::Create(m_iModel);
		pAmbient.set_R(.1);
		pAmbient.set_G(.1);
		pAmbient.set_B(.1);
		pAmbient.set_W(.05);

		auto pColor = GEOM::Color::Create(m_iModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(m_iModel);
		pMaterial.set_color(pColor);

		vector<double> vecVertices =
		{
			-.75, -.75, 0, // 1 (Bottom/Left)
			.75, -.75, 0,  // 2 (Bottom/Right)
			.75, .75, 0,   // 3 (Top/Right)
			-.75, .75, 0,  // 4 (Top/Left)
		};
		vector<int64_t> vecIndices =
		{
			0, 1, 2, 3, -1,
		};

		auto pBoundaryRepresentation = GEOM::BoundaryRepresentation::Create(m_iModel);
		pBoundaryRepresentation.set_material(pMaterial);
		pBoundaryRepresentation.set_vertices(vecVertices.data(), vecVertices.size());
		pBoundaryRepresentation.set_indices(vecIndices.data(), vecIndices.size());

		// Front
		OwlInstance iInstance = Translate(
			Rotate((int64_t)pBoundaryRepresentation, 2 * PI * 90. / 360., 0., 0.),
			0., -.75, 0.,
			1., -1., 1.);
		SetNameOfInstance(iInstance, "#front");

		// Back
		iInstance = Translate(
			Rotate((int64_t)pBoundaryRepresentation, 2 * PI * 90. / 360., 0., 0.),
			0., .75, 0.,
			-1., 1., 1.);
		SetNameOfInstance(iInstance, "#back");

		// Top
		iInstance = Translate(
			(int64_t)pBoundaryRepresentation,
			0., 0., .75,
			1., 1., -1.);
		SetNameOfInstance(iInstance, "#top");

		// Bottom
		iInstance = Translate(
			(int64_t)pBoundaryRepresentation,
			0., 0., -.75,
			1, 1., 1.);
		SetNameOfInstance(iInstance, "#bottom");

		// Left
		iInstance = Translate(
			Rotate((int64_t)pBoundaryRepresentation, 2 * PI * 90. / 360., 0., 2 * PI * 90. / 360.),
			-.75, 0., 0.,
			1., -1., 1.);
		SetNameOfInstance(iInstance, "#left");

		// Right
		iInstance = Translate(
			Rotate((int64_t)pBoundaryRepresentation, 2 * PI * 90. / 360., 0., 2 * PI * 90. / 360.),
			.75, 0., 0.,
			-1., 1., 1.);
		SetNameOfInstance(iInstance, "#right");
	}

	// Sphere (Sphere)
	{
		auto pAmbient = GEOM::ColorComponent::Create(m_iModel);
		pAmbient.set_R(0.);
		pAmbient.set_G(0.);
		pAmbient.set_B(1.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(m_iModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(m_iModel);
		pMaterial.set_color(pColor);

		auto pSphere = GEOM::Sphere::Create(m_iModel);
		pSphere.set_material(pMaterial);
		pSphere.set_radius(.1);
		pSphere.set_segmentationParts(36);

		// Front/Top/Left
		OwlInstance iInstance = Translate(
			(int64_t)pSphere,
			-.75, -.75, .75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#front-top-left");

		// Front/Top/Right
		iInstance = Translate(
			(int64_t)pSphere,
			.75, -.75, .75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#front-top-right");

		// Front/Bottom/Left
		iInstance = Translate(
			(int64_t)pSphere,
			-.75, -.75, -.75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#front-bottom-left");

		// Front/Bottom/Right
		iInstance = Translate(
			(int64_t)pSphere,
			.75, -.75, -.75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#front-bottom-right");

		// Back/Top/Left
		iInstance = Translate(
			(int64_t)pSphere,
			.75, .75, .75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#back-top-left");

		// Back/Top/Right
		iInstance = Translate(
			(int64_t)pSphere,
			-.75, .75, .75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#back-top-right");

		// Back/Bottom/Left
		iInstance = Translate(
			(int64_t)pSphere,
			.75, .75, -.75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#back-bottom-left");

		// Back/Bottom/Right
		iInstance = Translate(
			(int64_t)pSphere,
			-.75, .75, -.75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#back-bottom-right");		
	}
}

void CNavigatorRDFModel::CreateNaigatorLabels()
{
	double dXmin = DBL_MAX;
	double dXmax = -DBL_MAX;
	double dYmin = DBL_MAX;
	double dYmax = -DBL_MAX;
	double dZmin = DBL_MAX;
	double dZmax = -DBL_MAX;

	/* Top */
	OwlInstance iTopLabelInstance = m_pTextBuilder->BuildText("top", true);
	ASSERT(iTopLabelInstance != 0);

	CRDFInstance::CalculateBBMinMax(
		iTopLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Bottom */
	OwlInstance iBottomLabelInstance = m_pTextBuilder->BuildText("bottom", true);
	ASSERT(iBottomLabelInstance != 0);

	CRDFInstance::CalculateBBMinMax(
		iBottomLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Front */
	OwlInstance iFrontLabelInstance = m_pTextBuilder->BuildText("front", true);
	ASSERT(iFrontLabelInstance != 0);

	CRDFInstance::CalculateBBMinMax(
		iFrontLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Back */
	OwlInstance iBackLabelInstance = m_pTextBuilder->BuildText("back", true);
	ASSERT(iBackLabelInstance != 0);

	CRDFInstance::CalculateBBMinMax(
		iBackLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Left */
	OwlInstance iLeftLabelInstance = m_pTextBuilder->BuildText("left", true);
	ASSERT(iLeftLabelInstance != 0);

	CRDFInstance::CalculateBBMinMax(
		iLeftLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Right */
	OwlInstance iRightLabelInstance = m_pTextBuilder->BuildText("right", true);
	ASSERT(iRightLabelInstance != 0);

	CRDFInstance::CalculateBBMinMax(
		iRightLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Scale Factor */
	double dMaxLength = dXmax - dXmin;
	dMaxLength = fmax(dMaxLength, dYmax - dYmin);
	dMaxLength = fmax(dMaxLength, dZmax - dZmin);

	double dScaleFactor = (1.5 * .9) / dMaxLength;

	// Front
	OwlInstance iInstance = Translate(
		Rotate(Scale(iFrontLabelInstance, dScaleFactor), 2 * PI * 90. / 360., 0., 0.),
		0., -.751, 0.,
		1., 1., 1.);
	SetNameOfInstance(iInstance, "#front-label");

	// Back
	iInstance = Translate(
		Rotate(Scale(iBackLabelInstance, dScaleFactor), 2 * PI * 90. / 360., 0., 0.),
		0., .751, 0.,
		-1., 1., 1.);
	SetNameOfInstance(iInstance, "#back-label");

	// Top
	iInstance = Translate(
		Scale(iTopLabelInstance, dScaleFactor),
		0., 0., .751,
		1., 1., 1.);
	SetNameOfInstance(iInstance, "#top-label");

	// Bottom
	iInstance = Translate(
		Scale(iBottomLabelInstance, dScaleFactor),
		0., 0., -.751,
		-1, 1., 1.);
	SetNameOfInstance(iInstance, "#bottom-label");

	// Left
	iInstance = Translate(
		Rotate(Scale(iLeftLabelInstance, dScaleFactor), 2 * PI * 90. / 360., 0., 2 * PI * 90. / 360.),
		-.751, 0., 0.,
		1., -1., 1.);
	SetNameOfInstance(iInstance, "#left-label");

	// Right
	iInstance = Translate(
		Rotate(Scale(iRightLabelInstance, dScaleFactor), 2 * PI * 90. / 360., 0., 2 * PI * 90. / 360.),
		.751, 0., 0.,
		1., 1., 1.);
	SetNameOfInstance(iInstance, "#right-label");
}