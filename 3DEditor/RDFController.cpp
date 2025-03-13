#include "stdafx.h"
#include "RDFController.h"
#include "RDFModel.h"
#include "RDFView.h"

// ------------------------------------------------------------------------------------------------
BOOL TEST_MODE = FALSE;

// ------------------------------------------------------------------------------------------------
CRDFController::CRDFController()
	: _rdf_controller()
	, m_pSceneModel(new CSceneRDFModel())
	, m_pNavigatorModel(new CNavigatorRDFModel())
	, m_bUpdatingModel(false)
	, m_pSelectedInstance(nullptr)
	, m_prSelectedInstanceProperty(pair<CRDFInstance *, CRDFProperty *>(nullptr, nullptr))
	//, m_iVisibleValuesCountLimit(10000)
	//, m_bScaleAndCenter(TRUE)
	//, m_bModelCoordinateSystem(TRUE)
{
	//#todo
	m_pSceneModel->CreateDefaultModel();
	m_pNavigatorModel->CreateDefaultModel(); 

	wchar_t szAppPath[_MAX_PATH];
	::GetModuleFileName(::GetModuleHandle(nullptr), szAppPath, sizeof(szAppPath));

	fs::path pthExe = szAppPath;
	auto pthRootFolder = pthExe.parent_path();

	wstring strSettingsFile = pthRootFolder.wstring();
	strSettingsFile += L"\\3DEditor.settings";

	getSettingsStorage()->loadSettings(strSettingsFile);
}

// ------------------------------------------------------------------------------------------------
CRDFController::~CRDFController()
{
	delete m_pSceneModel;
	delete m_pNavigatorModel;	
}

CRDFModel* CRDFController::GetModel()
{
	return dynamic_cast<CRDFModel*>(getModel());
}


// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFController::LoadModel(LPCTSTR /*szFileName*/)
{
	assert(false);
}

// ------------------------------------------------------------------------------------------------
void CRDFController::SelectInstance(CRDFView* pSender, CRDFInstance* pInstance)
{
	ASSERT(FALSE);// obsolete!!!!
}

// ------------------------------------------------------------------------------------------------
CRDFInstance * CRDFController::GetSelectedInstance() const
{
	return m_pSelectedInstance;
}

// ------------------------------------------------------------------------------------------------
void CRDFController::SelectInstanceProperty(CRDFInstance * pInstance, CRDFProperty * pProperty)
{
	ASSERT(FALSE);//#todo
	/*if (m_bUpdatingModel)
	{
		return;
	}

	m_prSelectedInstanceProperty = pair<CRDFInstance *, CRDFProperty *>(pInstance, pProperty);
	m_pSelectedInstance = pInstance;

	auto itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnInstancePropertySelected();
	}*/
}

// ------------------------------------------------------------------------------------------------
pair<CRDFInstance *, CRDFProperty *> CRDFController::GetSelectedInstanceProperty() const
{
	return m_prSelectedInstanceProperty;
}

// ------------------------------------------------------------------------------------------------
//void CRDFController::UpdateCoordinateSystem()
//{
//	ASSERT(FALSE);//#todo
//	//auto pModel = GetModel();
//	//assert(pModel != nullptr);
//
//	//if (pModel == nullptr)
//	//{
//	//	assert(false);
//
//	//	return;
//	//}
//	//	
//	//if (!m_bModelCoordinateSystem)
//	//{
//	//	/* Vertex Buffers Offset */
//	//	double dVertexBuffersOffsetX = 0.;
//	//	double dVertexBuffersOffsetY = 0.;
//	//	double dVertexBuffersOffsetZ = 0.;
//	//	pModel->GetVertexBuffersOffset(dVertexBuffersOffsetX, dVertexBuffersOffsetY, dVertexBuffersOffsetZ);
//
//	//	double dBoundingSphereDiameter = pModel->GetOriginalBoundingSphereDiameter();
//
//	//	dVertexBuffersOffsetX /= dBoundingSphereDiameter / 2.;
//	//	dVertexBuffersOffsetY /= dBoundingSphereDiameter / 2.;
//	//	dVertexBuffersOffsetZ /= dBoundingSphereDiameter / 2.;
//
//	//	m_pSceneModel->TranslateModel(
//	//		dVertexBuffersOffsetX,
//	//		dVertexBuffersOffsetY,
//	//		dVertexBuffersOffsetZ);
//	//}
//	//else
//	//{
//	//	m_pSceneModel->TranslateModel(0.f, 0.f, 0.f);
//	//}
//}

// ------------------------------------------------------------------------------------------------
//void CRDFController::OnInstancePropertyEdited(CRDFInstance* pInstance, CRDFProperty* pProperty)
//{
//	ASSERT(FALSE);//#todo
//	/*auto pModel = GetModel();
//	assert(pModel != nullptr);
//
//	if (pModel == nullptr)
//	{
//		assert(false);
//
//		return;
//	}
//
//	pModel->OnInstancePropertyEdited(pInstance, pProperty);
//
//	if (m_bScaleAndCenter)
//	{
//		pModel->ScaleAndCenter();
//	}
//
//	auto itView = m_setViews.begin();
//	for (; itView != m_setViews.end(); itView++)
//	{
//		(*itView)->OnInstancePropertyEdited(pInstance, pProperty);
//	}*/
//}

CRDFInstance* CRDFController::CreateNewInstance(CRDFView * pSender, int64_t iClassInstance)
{
	ASSERT(FALSE);//#todo
	/*auto pModel = GetModel();
	assert(pModel != nullptr);

	if (pModel == nullptr)
	{
		assert(false);

		return nullptr;
	}

	auto pNewRDFInstance = pModel->CreateNewInstance(iClassInstance);
	assert(pNewRDFInstance != nullptr);

	auto itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnNewInstanceCreated(pSender, pNewRDFInstance);
	}

	return pNewRDFInstance;*/

	return nullptr;
}

void CRDFController::RenameInstance(CRDFView* pSender, CRDFInstance* pInstance, LPCTSTR szName)
{
	ASSERT(FALSE);//#todo
	//auto pModel = GetModel();
	//assert(pModel != nullptr);

	//if (pModel == nullptr)
	//{
	//	assert(false);

	//	return;
	//}

	//// Rename
	//SetNameOfInstanceW(
	//	pInstance->getOwlInstance(),
	//	szName);

	//// Update cache
	//pInstance->UpdateName();
	//pModel->OnInstanceNameEdited(pInstance);

	//// Notify
	//auto itView = m_setViews.begin();
	//for (; itView != m_setViews.end(); itView++)
	//{
	//	(*itView)->OnInstanceNameEdited(pSender, pInstance);
	//}
}

// ------------------------------------------------------------------------------------------------
bool CRDFController::DeleteInstance(CRDFView * pSender, CRDFInstance * pInstance)
{
	ASSERT(FALSE);//#todo
	/*auto pModel = GetModel();
	assert(pModel != nullptr);

	if (pModel == nullptr)
	{
		assert(false);

		return nullptr;
	}

	if (m_pSelectedInstance == pInstance)
	{
		m_pSelectedInstance = nullptr;
		m_prSelectedInstanceProperty = pair<CRDFInstance *, CRDFProperty *>(nullptr, nullptr);
	}

	int64_t iInstance = pInstance->getOwlInstance();

	bool bResult = pModel->DeleteInstance(pInstance);
	assert(bResult);

	auto itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnInstanceDeleted(pSender, iInstance);
	}

	return bResult;*/
	return false;
}

// ------------------------------------------------------------------------------------------------
bool CRDFController::DeleteInstanceTree(CRDFView * pSender, CRDFInstance * pInstance)
{
	ASSERT(FALSE);//#todo
	/*if (m_pSelectedInstance == pInstance)
	{
		m_pSelectedInstance = nullptr;
		m_prSelectedInstanceProperty = pair<CRDFInstance *, CRDFProperty *>(nullptr, nullptr);
	}

	if (DeleteInstanceTreeRecursive(pSender, pInstance))
	{
		auto itView = m_setViews.begin();
		for (; itView != m_setViews.end(); itView++)
		{
			(*itView)->OnInstancesDeleted(pSender);
		}

		return true;
	}*/

	return false;
}

bool CRDFController::DeleteInstanceTreeRecursive(CRDFView* pSender, CRDFInstance* pInstance)
{
	auto pModel = GetModel();
	assert(pModel != nullptr);

	if (pModel == nullptr)
	{
		assert(false);

		return false;
	}

	int64_t iInstance = pInstance->getOwlInstance();

	//
	//	find all child instances
	//
	int64_t	childCnt = 0;

	{
		int64_t iProperty = GetInstancePropertyByIterator(iInstance, 0);
		while (iProperty) {
			if (GetPropertyType(iProperty) == 1) {
				int64_t* values = 0, card = 0;
				GetObjectProperty(iInstance, iProperty, &values, &card);
				childCnt += card;
			}
			iProperty = GetInstancePropertyByIterator(iInstance, iProperty);
		}
	}

	int64_t* childInstanceArray = new int64_t[childCnt];

	{
		int64_t	i = 0, iProperty = GetInstancePropertyByIterator(iInstance, 0);
		while (iProperty) {
			if (GetPropertyType(iProperty) == 1) {
				int64_t* values = 0, card = 0;
				GetObjectProperty(iInstance, iProperty, &values, &card);
				memcpy(&childInstanceArray[i], values, card * sizeof(int64_t));
				i += card;
			}
			iProperty = GetInstancePropertyByIterator(iInstance, iProperty);
		}
		assert(i == childCnt);
	}

	bool bResult = pModel->DeleteInstance(pInstance);
	assert(bResult);

	for (int64_t i = 0; i < childCnt; i++) {
		auto pChildInstance = pModel->GetInstanceByIInstance(childInstanceArray[i]);
		if (pChildInstance) {
			DeleteInstanceTreeRecursive(pSender, pChildInstance);
		}
	}

	return bResult;
}

// ------------------------------------------------------------------------------------------------
bool CRDFController::DeleteInstances(CRDFView * pSender, vector<CRDFInstance *> vecInstances)
{
	ASSERT(FALSE);//#todo
	/*auto pModel = GetModel();
	assert(pModel != nullptr);

	if (pModel == nullptr)
	{
		assert(false);

		return false;
	}

	bool bResult = true;
	for (size_t iInstance = 0; iInstance < vecInstances.size(); iInstance++)
	{
		if (m_pSelectedInstance == vecInstances[iInstance])
		{
			m_pSelectedInstance = nullptr;
			m_prSelectedInstanceProperty = pair<CRDFInstance *, CRDFProperty *>(nullptr, nullptr);
		}

		bResult &= pModel->DeleteInstance(vecInstances[iInstance]);
		assert(bResult);
	}

	auto itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnInstancesDeleted(pSender);
	}

	return bResult;*/
	return false;
}

// ------------------------------------------------------------------------------------------------
//void CRDFController::ImportModel(CRDFView* /*pSender*/, const wchar_t* szPath)
//{
//	ASSERT(FALSE);//#todo
//	/*auto pModel = GetModel();
//	assert(pModel != nullptr);
//
//	if (pModel == nullptr)
//	{
//		assert(false);
//
//		return;
//	}
//
//	pModel->ImportModel(szPath);
//
//	UpdateCoordinateSystem();
//
//	m_bUpdatingModel = true;
//
//	auto itView = m_setViews.begin();
//	for (; itView != m_setViews.end(); itView++)
//	{
//		(*itView)->OnModelChanged();
//	}
//
//	m_bUpdatingModel = false;*/
//}

// ------------------------------------------------------------------------------------------------
void CRDFController::OnInstancesEnabledStateChanged()
{
	ASSERT(FALSE);//#todo
	/*auto pModel = GetModel();
	assert(pModel != nullptr);

	if (pModel == nullptr)
	{
		assert(false);

		return;
	}

	if (m_bScaleAndCenter)
	{
		pModel->ScaleAndCenter();
	}

	auto itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnInstancesEnabledStateChanged();
	}*/
}

// ------------------------------------------------------------------------------------------------
void CRDFController::BeginTestMode()
{
	TEST_MODE = TRUE;
}

// ------------------------------------------------------------------------------------------------
void CRDFController::EndTestMode()
{
	// PATCH: see PATCH: AMD 6700 XT - Access violation
	//TEST_MODE = FALSE;
}

// ------------------------------------------------------------------------------------------------
BOOL CRDFController::IsTestMode()
{
	return TEST_MODE;
}