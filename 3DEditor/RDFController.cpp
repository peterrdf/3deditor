#include "stdafx.h"
#include "RDFController.h"
#include "RDFModel.h"

// ************************************************************************************************
BOOL TEST_MODE = FALSE;

// ************************************************************************************************
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

CRDFController::~CRDFController()
{
}

/*virtual*/ void CRDFController::loadDecorationModels() /*override*/
{
	addDecorationModel(new _world_coordinate_system_model(this));
	addDecorationModel(new _model_coordinate_system_model(this));
	addDecorationModel(new _navigator_model()); 
	addDecorationModel(new _navigator_coordinate_system_model());
}

void CRDFController::_test_BeginTestMode()
{
	TEST_MODE = TRUE;
}

void CRDFController::_test_EndTestMode()
{
	// PATCH: see PATCH: AMD 6700 XT - Access violation
	//TEST_MODE = FALSE;
}

BOOL CRDFController::_test_IsTestMode()
{
	return TEST_MODE;
}