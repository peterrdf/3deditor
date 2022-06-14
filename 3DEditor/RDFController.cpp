#include "stdafx.h"
#include "RDFController.h"
#include "RDFModel.h"
#include "RDFView.h"

// ------------------------------------------------------------------------------------------------
BOOL TEST_MODE = FALSE;

// ------------------------------------------------------------------------------------------------
CRDFController::CRDFController()
	: m_pModel(NULL)
	, m_bUpdatingModel(false)
	, m_setViews()
	, m_pSelectedInstance(NULL)
	, m_prSelectedInstanceProperty(pair<CRDFInstance *, CRDFProperty *>(NULL, NULL))
	, m_iVisibleValuesCountLimit(100)
	, m_bScaleAndCenter(FALSE)
{
}

// ------------------------------------------------------------------------------------------------
CRDFController::~CRDFController()
{
}

// ------------------------------------------------------------------------------------------------
CRDFModel * CRDFController::GetModel() const
{
	return m_pModel;
}

// ------------------------------------------------------------------------------------------------
void CRDFController::SetModel(CRDFModel * pModel)
{
	ASSERT(pModel != NULL);

	m_pModel = pModel;

	m_pSelectedInstance = NULL;
	m_prSelectedInstanceProperty = pair<CRDFInstance *, CRDFProperty *>(NULL, NULL);

	m_bUpdatingModel = true;

	set<CRDFView *>::iterator itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnModelChanged();
	}

	m_bUpdatingModel = false;
}

// ------------------------------------------------------------------------------------------------
void CRDFController::RegisterView(CRDFView * pView)
{
	ASSERT(pView != NULL);
	ASSERT(m_setViews.find(pView) == m_setViews.end());

	m_setViews.insert(pView);
}

// ------------------------------------------------------------------------------------------------
void CRDFController::UnRegisterView(CRDFView * pView)
{
	ASSERT(pView != NULL);
	ASSERT(m_setViews.find(pView) != m_setViews.end());

	m_setViews.erase(pView);
}

// ------------------------------------------------------------------------------------------------
const set<CRDFView *> & CRDFController::GetViews()
{
	return m_setViews;
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFController::LoadModel(LPCTSTR /*szFileName*/)
{
	ASSERT(FALSE);
}

// ------------------------------------------------------------------------------------------------
void CRDFController::ZoomToInstance(int64_t iInstance)
{
	ASSERT(m_pModel != NULL);

	m_pModel->ZoomToInstance(iInstance);

	set<CRDFView *>::iterator itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnWorldDimensionsChanged();
	}
}

// ------------------------------------------------------------------------------------------------
void CRDFController::ScaleAndCenter()
{
	m_pModel->ScaleAndCenter();

	set<CRDFView*>::iterator itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnWorldDimensionsChanged();
	}
}

// ------------------------------------------------------------------------------------------------
void CRDFController::ShowMetaInformation(CRDFInstance * pInstance)
{
	m_pSelectedInstance = pInstance;
	m_prSelectedInstanceProperty = pair<CRDFInstance *, CRDFProperty *>(NULL, NULL);

	set<CRDFView *>::iterator itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnShowMetaInformation();
	}
}

// ------------------------------------------------------------------------------------------------
void CRDFController::SelectInstance(CRDFView * pSender, CRDFInstance * pInstance)
{
	if (m_bUpdatingModel)
	{
		return;
	}

	m_pSelectedInstance = pInstance;
	m_prSelectedInstanceProperty = pair<CRDFInstance *, CRDFProperty *>(NULL, NULL);

	set<CRDFView *>::iterator itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnInstanceSelected(pSender);
	}
}

// ------------------------------------------------------------------------------------------------
CRDFInstance * CRDFController::GetSelectedInstance() const
{
	return m_pSelectedInstance;
}

// ------------------------------------------------------------------------------------------------
void CRDFController::SelectInstanceProperty(CRDFInstance * pInstance, CRDFProperty * pProperty)
{
	if (m_bUpdatingModel)
	{
		return;
	}

	m_prSelectedInstanceProperty = pair<CRDFInstance *, CRDFProperty *>(pInstance, pProperty);
	m_pSelectedInstance = pInstance;

	set<CRDFView *>::iterator itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnInstancePropertySelected();
	}
}

// ------------------------------------------------------------------------------------------------
pair<CRDFInstance *, CRDFProperty *> CRDFController::GetSelectedInstanceProperty() const
{
	return m_prSelectedInstanceProperty;
}

// ------------------------------------------------------------------------------------------------
int CRDFController::GetVisibleValuesCountLimit() const
{
	return m_iVisibleValuesCountLimit;
}

// ------------------------------------------------------------------------------------------------
void CRDFController::SetVisibleValuesCountLimit(int iVisibleValuesCountLimit)
{
	m_iVisibleValuesCountLimit = iVisibleValuesCountLimit;

	set<CRDFView *>::iterator itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnVisibleValuesCountLimitChanged();
	}
}

// ------------------------------------------------------------------------------------------------
BOOL CRDFController::GetScaleAndCenter() const
{
	return m_bScaleAndCenter;
}

// ------------------------------------------------------------------------------------------------
void CRDFController::SetScaleAndCenter(BOOL bScaleAndCenter)
{
	m_bScaleAndCenter = bScaleAndCenter;
}

// ------------------------------------------------------------------------------------------------
void CRDFController::OnInstancePropertyEdited(CRDFInstance * pInstance, CRDFProperty * pProperty)
{
	m_pModel->OnInstancePropertyEdited(pInstance, pProperty);

	if (m_bScaleAndCenter)
	{
		m_pModel->ScaleAndCenter();
	}

	set<CRDFView *>::iterator itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnInstancePropertyEdited(pInstance, pProperty);
	}
}

// ------------------------------------------------------------------------------------------------
CRDFInstance * CRDFController::CreateNewInstance(CRDFView * pSender, int64_t iClassInstance)
{
	CRDFInstance * pNewRDFInstance = m_pModel->CreateNewInstance(iClassInstance);
	ASSERT(pNewRDFInstance != NULL);

	set<CRDFView *>::iterator itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnNewInstanceCreated(pSender, pNewRDFInstance);
	}

	return pNewRDFInstance;
}

// ------------------------------------------------------------------------------------------------
CRDFInstance* CRDFController::OnOctreeInstanceCreated(CRDFView* pSender, Thing* pThing)
{
	CRDFInstance* pNewRDFInstance = m_pModel->AddNewInstance(pThing);
	ASSERT(pNewRDFInstance != NULL);

	set<CRDFView*>::iterator itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnNewInstanceCreated(pSender, pNewRDFInstance);
	}

	return pNewRDFInstance;
}

// ------------------------------------------------------------------------------------------------
bool CRDFController::DeleteInstance(CRDFView * pSender, CRDFInstance * pInstance)
{
	if (m_pSelectedInstance == pInstance)
	{
		m_pSelectedInstance = NULL;
		m_prSelectedInstanceProperty = pair<CRDFInstance *, CRDFProperty *>(NULL, NULL);
	}

	int64_t iInstance = pInstance->getInstance();

	bool bResult = m_pModel->DeleteInstance(pInstance);
	ASSERT(bResult);

	set<CRDFView *>::iterator itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnInstanceDeleted(pSender, iInstance);
	}

	return bResult;
}

// ------------------------------------------------------------------------------------------------
bool CRDFController::DeleteInstanceTree(CRDFView * pSender, CRDFInstance * pInstance)
{
	if (m_pSelectedInstance == pInstance)
	{
		m_pSelectedInstance = NULL;
		m_prSelectedInstanceProperty = pair<CRDFInstance *, CRDFProperty *>(NULL, NULL);
	}

	int64_t iInstance = pInstance->getInstance();

	//
	//	find all child instances
	//
	int64_t	childCnt = 0;

	{
		int64_t iProperty = GetInstancePropertyByIterator(iInstance, 0);
		while (iProperty) {
			if (GetPropertyType(iProperty) == 1) {
				int64_t	* values = 0, card = 0;
				GetObjectProperty(iInstance, iProperty, &values, &card);
				childCnt += card;
			}
			iProperty = GetInstancePropertyByIterator(iInstance, iProperty);
		}
	}

	int64_t	* childInstanceArray = new int64_t[childCnt];

	{
		int64_t	i = 0, iProperty = GetInstancePropertyByIterator(iInstance, 0);
		while (iProperty) {
			if (GetPropertyType(iProperty) == 1) {
				int64_t	* values = 0, card = 0;
				GetObjectProperty(iInstance, iProperty, &values, &card);
				memcpy(&childInstanceArray[i], values, card * sizeof(int64_t));
				i += card;
			}
			iProperty = GetInstancePropertyByIterator(iInstance, iProperty);
		}
		ASSERT(i == childCnt);
	}

	bool bResult = m_pModel->DeleteInstance(pInstance);
	ASSERT(bResult);

	set<CRDFView *>::iterator itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnInstanceDeleted(pSender, iInstance);
	}

	for (int64_t i = 0; i < childCnt; i++) {
		CRDFInstance	* pChildInstance = m_pModel->GetRDFInstanceByIInstance(childInstanceArray[i]);
		if (pChildInstance) {
			DeleteInstanceTree(pSender, pChildInstance);
		}
	}

	return bResult;
}

// ------------------------------------------------------------------------------------------------
bool CRDFController::DeleteInstances(CRDFView * pSender, vector<CRDFInstance *> vecInstances)
{
	bool bResult = true;
	for (size_t iInstance = 0; iInstance < vecInstances.size(); iInstance++)
	{
		if (m_pSelectedInstance == vecInstances[iInstance])
		{
			m_pSelectedInstance = NULL;
			m_prSelectedInstanceProperty = pair<CRDFInstance *, CRDFProperty *>(NULL, NULL);
		}

		bResult &= m_pModel->DeleteInstance(vecInstances[iInstance]);
		ASSERT(bResult);
	}

	set<CRDFView *>::iterator itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnInstancesDeleted(pSender);
	}

	return bResult;
}

// ------------------------------------------------------------------------------------------------
void CRDFController::AddMeasurements(CRDFView * pSender, CRDFInstance * pInstance)
{
	m_pModel->AddMeasurements(pInstance);

	set<CRDFView *>::iterator itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnMeasurementsAdded(pSender, pInstance);
	}
}

// ------------------------------------------------------------------------------------------------
void CRDFController::ImportModel(CRDFView* /*pSender*/, const wchar_t* szPath)
{
	m_pModel->ImportModel(szPath);

	set<CRDFView*>::iterator itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnModelChanged();
	}
}

// ------------------------------------------------------------------------------------------------
void CRDFController::OnInstancesEnabledStateChanged()
{
	set<CRDFView *>::iterator itView = m_setViews.begin();
	for (; itView != m_setViews.end(); itView++)
	{
		(*itView)->OnInstancesEnabledStateChanged();
	}
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