#pragma once

#include "_rdf_mvc.h"

#include <set>
#include <vector>
using namespace std;

// ***********************************************************************************************
class CRDFController : public _rdf_controller
{

public: // Methods

	CRDFController();
	virtual ~CRDFController();

// MOVE !!!	#todo
	//void ImportModel(_rdf_view* pSender, const wchar_t* szPath);

	// Tests
	virtual void LoadModel(LPCTSTR szFileName);
	void BeginTestMode();
	void EndTestMode();
	BOOL IsTestMode();
};




