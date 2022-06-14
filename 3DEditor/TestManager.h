#pragma once

#include "OpenGLRDFView.h"
#include "RDFController.h"
#include "TestHtmlReport.h"

// ------------------------------------------------------------------------------------------------
class CTestManager
{

private: // Members

	// --------------------------------------------------------------------------------------------
	CRDFController* m_pController;

	// --------------------------------------------------------------------------------------------
	COpenGLRDFView* m_pOpenGLView;

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

	// --------------------------------------------------------------------------------------------
	void ExecuteTests(const wchar_t* szTestsFolder);	

private: // Methods

	// --------------------------------------------------------------------------------------------
	void Test(const wchar_t* szInputFileName, const wchar_t* szTestFileName, const wchar_t* szScreenShotFilePath, CTestHtmlReport* pReport);
};

