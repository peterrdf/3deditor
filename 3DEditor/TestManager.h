#pragma once

#include "RDFOpenGLView.h"
#include "RDFController.h"
#include "TestHtmlReport.h"

// ------------------------------------------------------------------------------------------------
class CTestManager
{

private: // Members

	// --------------------------------------------------------------------------------------------
	CRDFController* m_pController;

	// --------------------------------------------------------------------------------------------
	CRDFOpenGLView* m_pOpenGLView;

	// --------------------------------------------------------------------------------------------
	CString m_strInputDataDir;

	// --------------------------------------------------------------------------------------------
	CString m_strTestsDir;

	// --------------------------------------------------------------------------------------------
	CString m_strTestReportDir;

public: // Methods

	// --------------------------------------------------------------------------------------------
	CTestManager(CRDFController* pController, const wchar_t* szInputDataDir);

	// --------------------------------------------------------------------------------------------
	virtual ~CTestManager();

	// --------------------------------------------------------------------------------------------
	static void FindFiles(const wchar_t* szFolder, const wchar_t* szWildcard, CStringList& lsFiles);

	// --------------------------------------------------------------------------------------------
	void GenerateTests(const CString& strWildcards);
};

