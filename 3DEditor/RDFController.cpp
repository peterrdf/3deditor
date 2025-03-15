#include "stdafx.h"
#include "RDFController.h"
#include "RDFModel.h"

// ------------------------------------------------------------------------------------------------
BOOL TEST_MODE = FALSE;

// ------------------------------------------------------------------------------------------------
CRDFController::CRDFController()
	: _rdf_controller()
{
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
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFController::LoadModel(LPCTSTR /*szFileName*/)
{
	assert(false);
}

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
//void CRDFController::ImportModel(_rdf_view* /*pSender*/, const wchar_t* szPath)
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