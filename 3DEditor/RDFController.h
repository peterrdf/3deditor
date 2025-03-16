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

	// Tests
	virtual void _test_LoadModel(LPCTSTR szFileName) PURE;
	void _test_BeginTestMode();
	void _test_EndTestMode();
	BOOL _test_IsTestMode();
};




