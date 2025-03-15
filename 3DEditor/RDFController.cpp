#include "stdafx.h"
#include "RDFController.h"
#include "RDFModel.h"
#include "RDFView.h"

// ------------------------------------------------------------------------------------------------
BOOL TEST_MODE = FALSE;

// ------------------------------------------------------------------------------------------------
CRDFController::CRDFController()
	: _rdf_controller()
	//, m_pSceneModel(new CSceneRDFModel())
	//, m_pNavigatorModel(new CNavigatorRDFModel())
	//, m_bUpdatingModel(false)
	//, m_iVisibleValuesCountLimit(10000)
	//, m_bScaleAndCenter(TRUE)
	//, m_bModelCoordinateSystem(TRUE)
{
	//#todo
	//m_pSceneModel->CreateDefaultModel();
	//m_pNavigatorModel->CreateDefaultModel(); 

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
	//delete m_pSceneModel;
	//delete m_pNavigatorModel;	
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
//void CRDFController::OnInstancePropertyEdited(CRDFInstance* pInstance, _rdf_property* pProperty)
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