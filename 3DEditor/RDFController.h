#pragma once

#include "_rdf_mvc.h"

#include <set>
#include <vector>
using namespace std;

// ------------------------------------------------------------------------------------------------
class CRDFModel;
class CSceneRDFModel;
class CNavigatorRDFModel;
class CRDFView;
enum class enumApplicationProperty;
class CRDFInstance;
class _rdf_property;

// ***********************************************************************************************
class CRDFController : public _rdf_controller
{

public: // Methods

	CRDFController();
	virtual ~CRDFController();
	
// REMOVE!!! #todo	
	CRDFModel* GetModel();

// MOVE !!!	#todo
	//void ImportModel(CRDFView* pSender, const wchar_t* szPath);

	// Tests
	virtual void LoadModel(LPCTSTR szFileName);
	void BeginTestMode();
	void EndTestMode();
	BOOL IsTestMode();
};




