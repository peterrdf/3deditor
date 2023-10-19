#include "stdafx.h"

#include "RDFModel.h"
#include "Generic.h"
#include "ProgressIndicator.h"
#include "ProgressDialog.h"
#include "gisengine.h"

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
void CRDFModel::ScaleAndCenter(bool bLoadingModel/* = false*/)
{
	ProgressStatus stat(L"Calculate scene sizes");

	/* World */
	m_fBoundingSphereDiameter = 0.f;

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
			L"Internal error.", L"Error", MB_ICONERROR | MB_OK);

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
			L"Internal error.", L"Error", MB_ICONERROR | MB_OK);

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

	// http://rdf.bg/gkdoc/CP64/SetVertexBufferOffset.html
	UpdateVertexBufferOffset();

	// Instances
	LoadRDFInstances();
}

void CRDFModel::UpdateVertexBufferOffset()
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
			SetInstanceDefaultStateRecursive(itInstanceDefaultState.first);
		}
	}

	/* Min/Max */
	double dXmin = DBL_MAX;
	double dXmax = -DBL_MAX;
	double dYmin = DBL_MAX;
	double dYmax = -DBL_MAX;
	double dZmin = DBL_MAX;
	double dZmax = -DBL_MAX;

	iInstance = GetInstancesByIterator(m_iModel, 0);
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
			L"Internal error.", L"Error", MB_ICONERROR | MB_OK);

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

	int64_t iTransformationInstance = CreateInstance(GetClassByName(m_iModel, "Transformation"), "Translate");
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

	int64_t iTransformationInstance = CreateInstance(GetClassByName(m_iModel, "Transformation"), "Translate");
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

// ------------------------------------------------------------------------------------------------
void CRDFModel::BuildOctants(_octant* pOctant, vector<GEOM::GeometricItem>& vecOctantsGeometry)
{
	if (pOctant == nullptr)
	{
		return;
	}

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

		auto pPolyLine3D = GEOM::PolyLine3D::Create(m_iModel);
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

		auto pPolyLine3D = GEOM::PolyLine3D::Create(m_iModel);
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

		auto pLine3D = GEOM::Line3D::Create(m_iModel);
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

		auto pLine3D = GEOM::Line3D::Create(m_iModel);
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

		auto pLine3D = GEOM::Line3D::Create(m_iModel);
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

		auto pLine3D = GEOM::Line3D::Create(m_iModel);
		pLine3D.set_points(&vecCoordinates[0], vecCoordinates.size());

		vecObjects.push_back(pLine3D);
	}

	auto pCollection = GEOM::Collection::Create(m_iModel);
	pCollection.set_objects(&vecObjects[0], vecObjects.size());

	vecOctantsGeometry.push_back(pCollection);
}

// ************************************************************************************************
// Labels
static const char* TOP_LABEL = "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";
static const char* BOTTOM_LABEL = "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";
static const char* FRONT_LABEL = "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";
static const char* BACK_LABEL = "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";
static const char* LEFT_LABEL = "QwgMQmV6aWVyQ3VydmULQ29sbGVjdGlvbgpGYWNlMkRTZXQHTGluZTNEB01hdHJpeAhQb2ludDNEClBvbHlnb24zRA9UcmFuc2Zvcm1hdGlvblAKBF80MQUEXzQyBQxjb29yZGluYXRlcwUHcG9pbnRzBQ5jb250cm9sUG9pbnRzAQpsaW5lUGFydHMBB21hdHJpeAEHb2JqZWN0AQhvYmplY3RzAQlwb2x5Z29ucwFJ/7UHBAECBAcGBQUFAAUFBQAFBQUABQUFAAMDAwMDAwMDAwMDBQUFAAUFBQAFBQUABQUFAAMCBAcGAwMDAwMDBQUFAAUFBQAFBQUAAwUFBQAFBQUABQUFAAUFBQADAwMDAwIEBwYEBwYFBQUABQUFAAUFBQAFBQUAAwUFBQAFBQUABQUFAAUFBQADBQUFAAUFBQAFBQUABQUFAAUFBQAFBQUABQUFAAUFBQAFBQUAAwIEBwYDAwMDBwIGAf/+AgwDAPA/AAAAAAAAAAAAAPA/AAAAAAAAAAAAAPA/XCbAABjAAACA//4IBK1fMwP/CQX/AANANSABAz/1gP8HBgYE//4FFDIxLSklISAfHh0cGxoZGBcWEg4K//4DAwMgNUCA9T8AAAD//gMDA8A0QADwPwAAAP/+AwMDhDRAAPA/AAAA//4EAwkIB//+AwMDhDRAAPA/AAAA//4DAwM0NEAA8D8AAAD//gMDAwg0QMDxPwAAAP/+BAMNDAv//gMDAwg0QMDxPwAAAP/+AwMD3DNAwPM/AAAA//4DAwPIM0AA9z8AAAD//gQDERAP//4DAwPIM0AA9z8AAAD//gMDA7gzQED6PwAAAP/+AwMDuDNAwAJAAAAA//4EAxUUE//+AwYDuDNAACBAAAAAuDNAwAJAAAAA//4DBgMgNUAAIEAAAAC4M0AAIEAAAAD//gMGAyA1QAAiQAAAACA1QAAgQAAAAP/+AwYDuDNAACJAAAAAIDVAACJAAAAA//4DBgO4M0DQJ0AAAAC4M0AAIkAAAAD//gMGA1AyQCAmQAAAALgzQNAnQAAAAP/+AwYDUDJAACJAAAAAUDJAICZAAAAA//4DBgNIMUAAIkAAAABQMkAAIkAAAAD//gMGA0gxQAAgQAAAAEgxQAAiQAAAAP/+AwYDUDJAACBAAAAASDFAACBAAAAA//4DBgNQMkBABkAAAABQMkAAIEAAAAD//gMDA1AyQEAGQAAAAP/+AwMDUDJAQPY/AAAA//4DAwN8MkCA7j8AAAD//gQDJCMi//4DAwN8MkCA7j8AAAD//gMDA6gyQADhPwAAAP/+AwMDFDNAANE/AAAA//4EAygnJv/+AwMDFDNAANE/AAAA//4DAwOAM0AAAAAAAAD//gMDA0Q0QAAAAAAAAP/+BAMsKyr//gMDA0Q0QAAAAAAAAP/+AwMDvDRAAAAAAAAA//4DAwNUNUAAuD8AAAD//gQDMC8u//4DBgMgNUCA9T8AAABUNUAAuD8AAAD/CTX/AANALMgBAP8HNgY0//4FE15dXFtaWVVRTUlIREA8Ozo5ODf//gMGA5gvQAAAAAAAAMgsQAAAAAAAAP/+AwYDmC9AACBAAAAAmC9AAAAAAAAA//4DBgNoMUAAIEAAAACYL0AAIEAAAAD//gMGA2gxQAAiQAAAAGgxQAAgQAAAAP/+AwYDmC9AACJAAAAAaDFAACJAAAAA//4DBgOYL0CAI0AAAACYL0AAIkAAAAD//gMDA5gvQIAjQAAAAP/+AwMDmC9A8CRAAAAA//4DAwMQMEB4JUAAAAD//gQDPz49//4DAwMQMEB4JUAAAAD//gMDA1QwQAAmQAAAAP/+AwMD+DBAACZAAAAA//4EA0NCQf/+AwMD+DBAACZAAAAA//4DAwNcMUAAJkAAAAD//gMDA8wxQNglQAAAAP/+BANHRkX//gMGAwAyQMgnQAAAAMwxQNglQAAAAP/+AwMDADJAyCdAAAAA//4DAwNIMUAAKEAAAAD//gMDA7AwQAAoQAAAAP/+BANMS0r//gMDA7AwQAAoQAAAAP/+AwMDiC9AAChAAAAA//4DAwOALkBgJ0AAAAD//gQDUE9O//4DAwOALkBgJ0AAAAD//gMDA4AtQMgmQAAAAP/+AwMDGC1A0CVAAAAA//4EA1RTUv/+AwMDGC1A0CVAAAAA//4DAwPILEAQJUAAAAD//gMDA8gsQJgjQAAAAP/+BANYV1b//gMGA8gsQAAiQAAAAMgsQJgjQAAAAP/+AwYDUCpAACJAAAAAyCxAACJAAAAA//4DBgNQKkAAIEAAAABQKkAAIkAAAAD//gMGA8gsQAAgQAAAAFAqQAAgQAAAAP/+AwYDyCxAAAAAAAAAyCxAACBAAAAA//4JAmRh/wADQBiAAQJAFP8HYgZg//4FBXZ1cW1p/wADQCV4AQJACP8HZQZj//4FD6yrp6Ofm5eTj4uHhoJ+ev/+AwMDgBhAABRAAAAA//4DAwPAGECAGUAAAAD//gMDA2AbQMAcQAAAAP/+BANoZ2b//gMDA2AbQMAcQAAAAP/+AwMDEB5AACBAAAAA//4DAwP4IEAAIEAAAAD//gQDbGtq//4DAwP4IEAAIEAAAAD//gMDAxgjQAAgQAAAAP/+AwMDcCRA8BtAAAAA//4EA3Bvbv/+AwMDcCRA8BtAAAAA//4DAwNQJUBAGUAAAAD//gMDA4AlQAAUQAAAAP/+BAN0c3L//gMGA4AYQAAUQAAAAIAlQAAUQAAAAP/+AwMDeCVAAAhAAAAA//4DAwPYJEBA/z8AAAD//gMDA8AjQID3PwAAAP/+BAN5eHf//gMDA8AjQID3PwAAAP/+AwMDqCJAAPA/AAAA//4DAwMYIUAA8D8AAAD//gQDfXx7//4DAwMYIUAA8D8AAAD//gMDAxAeQADwPwAAAP/+AwMDQBtAQPw/AAAA//4EA4GAf//+AwMDQBtAQPw/AAAA//4DAwOAGEBgBEAAAAD//gMDAzAYQAAQQAAAAP/+BAOFhIP//gMGA3goQAAQQAAAADAYQAAQQAAAAP/+AwMDeChAABBAAAAA//4DAwN4KEAQEUAAAAD//gMDA3goQKARQAAAAP/+BAOKiYj//gMDA3goQKARQAAAAP/+AwMDeChAYBpAAAAA//4DAwNYJkAwH0AAAAD//gQDjo2M//4DAwNYJkAwH0AAAAD//gMDA0AkQAAiQAAAAP/+AwMD8CBAACJAAAAA//4EA5KRkP/+AwMD8CBAACJAAAAA//4DAwIbQCJAAAD//gMDA7AWQDAfQAAAAP/+BAOWlZT//gMDA7AWQDAfQAAAAP/+AwMDYBJAYBpAAAAA//4DAwNgEkCwEUAAAAD//gQDmpmY//4DAwNgEkCwEUAAAAD//gMDA2ASQKACQAAAAP/+AwMDoBZAgPI/AAAA//4EA56dnP/+AwMDoBZAgPI/AAAA//4DAwPgGkAAAAAAAAD//gMDAxghQAAAAAAAAP/+BAOioaD//gMDAxghQAAAAAAAAP/+AwMD+CNAAAAAAAAA//4DAwPQJUAA6T8AAAD//gQDpqWk//4DAwPQJUAA6T8AAAD//gMDA7AnQED5PwAAAP/+AwMDYChAAAhAAAAA//4EA6qpqP/+AwYDeCVAAAhAAAAAYChAAAhAAAAA/wmv/wADP/CAAQD/B7AGrv/+BQS0s7Kx//4DBgOAA0AAAAAAAACA8D8AAAAAAAD//gMGA4ADQAAoQAAAAIADQAAAAAAAAP/+AwYDgPA/AChAAAAAgANAAChAAAAA//4DBgOA8D8AAAAAAACA8D8AKEAAAAD/bgpUcmFuc2xhdGUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABa";
static const char* RIGHT_LABEL = "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";

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

	// Cube 
	{
		auto pAmbient = GEOM::ColorComponent::Create(m_iModel);
		pAmbient.set_R(0.);
		pAmbient.set_G(1.);
		pAmbient.set_B(0.);
		pAmbient.set_W(.35);

		auto pColor = GEOM::Color::Create(m_iModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(m_iModel);
		pMaterial.set_color(pColor);

		auto pCube = GEOM::Cube::Create(m_iModel);
		pCube.set_material(pMaterial);
		pCube.set_length(1.5);

		Translate(
			(int64_t)pCube,
			-.75, -.75, -.75,
			1., 1., 1.);
	}

	LoadLabels();

	LoadRDFModel();
}

void CSceneRDFModel::LoadLabels()
{
	double dXmin = DBL_MAX;
	double dXmax = -DBL_MAX;
	double dYmin = DBL_MAX;
	double dYmax = -DBL_MAX;
	double dZmin = DBL_MAX;
	double dZmax = -DBL_MAX;

	/* Top */
	OwlInstance iTopLabelInstance = ImportModelA(m_iModel, (const unsigned char*)TOP_LABEL);
	ASSERT(iTopLabelInstance != 0);
	
	CRDFInstance::CalculateBBMinMax(
		iTopLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Bottom */
	OwlInstance iBottomLabelInstance = ImportModelA(m_iModel, (const unsigned char*)BOTTOM_LABEL);
	ASSERT(iBottomLabelInstance != 0);
	
	CRDFInstance::CalculateBBMinMax(
		iBottomLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Front */
	OwlInstance iFrontLabelInstance = ImportModelA(m_iModel, (const unsigned char*)FRONT_LABEL);
	ASSERT(iFrontLabelInstance != 0);

	CRDFInstance::CalculateBBMinMax(
		iFrontLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Back */
	OwlInstance iBackLabelInstance = ImportModelA(m_iModel, (const unsigned char*)BACK_LABEL);
	ASSERT(iBackLabelInstance != 0);

	CRDFInstance::CalculateBBMinMax(
		iBackLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Left */
	OwlInstance iLeftLabelInstance = ImportModelA(m_iModel, (const unsigned char*)LEFT_LABEL);
	ASSERT(iLeftLabelInstance != 0);

	CRDFInstance::CalculateBBMinMax(
		iLeftLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Right */
	OwlInstance iRightLabelInstance = ImportModelA(m_iModel, (const unsigned char*)RIGHT_LABEL);
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

	// Top
	Translate(
		Scale(iTopLabelInstance, dScaleFactor),
		0., 0., .751,
		1., 1., 1.);

	// Bottom
	Translate(
		Scale(iBottomLabelInstance, dScaleFactor),
		0., 0., -.751,
		-1, 1., 1.);

	// Front
	Translate(
		Scale(iFrontLabelInstance, dScaleFactor),
		0., -.751, 0., 
		1., 1., 1.);

	// Back
	Translate(
		Scale(iBackLabelInstance, dScaleFactor),
		0., .751, 0.,
		1., 1., 1.);

	// Left
	Translate(
		Scale(iLeftLabelInstance, dScaleFactor),
		-.751, 0., 0., 
		1., 1., 1.);

	// Right
	Translate(
		Scale(iRightLabelInstance, dScaleFactor),
		.751, 0., 0.,
		1., 1., 1.);
}