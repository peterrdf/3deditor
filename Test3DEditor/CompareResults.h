#pragma once

#include "TestHtmlReport.h"

#include <atlstr.h>

#include <fstream>
#include <iostream>
#include <codecvt>

using namespace std;

// ************************************************************************************************
struct TEST
{
	CString VERSION;
	CString REVISION;
	CString SCREENSHOT;
	float Tx;
	float Ty;
	float Tz;
	float Rx;
	float Ry;
};

// ************************************************************************************************
static bool AreTestIdentical(const TEST& test1, const TEST& test2)
{
	if (test1.VERSION != test2.VERSION)
	{
		return false;
	}

	if (test1.Tx != test2.Tx)
	{
		return false;
	}

	if (test1.Ty != test2.Ty)
	{
		return false;
	}

	if (test1.Tz != test2.Tz)
	{
		return false;
	}

	if (test1.Rx != test2.Rx)
	{
		return false;
	}

	if (test1.Ry != test2.Ry)
	{
		return false;
	}

	return true;
}

class CCompareResults
{

private: // Fields

	// --------------------------------------------------------------------------------------------
	int m_iComparisonIndex;

	// --------------------------------------------------------------------------------------------
	CString m_strTestResults1;

	// --------------------------------------------------------------------------------------------
	CString m_strTestResults2;

	// --------------------------------------------------------------------------------------------
	CString m_strTestReportDir;

public: // Methods

	// --------------------------------------------------------------------------------------------
	CCompareResults(const CString& strTestResults1, const CString& strTestResults2, const CString& strTestReportDir);

	// --------------------------------------------------------------------------------------------
	void Execute();

	// --------------------------------------------------------------------------------------------
	static void FindFiles(const wchar_t* szFolder, const wchar_t* szWildcard, CStringList& lsFiles);

	// --------------------------------------------------------------------------------------------
	static void FindTests(const wchar_t* szFolder, CStringList& lsTests);

	// --------------------------------------------------------------------------------------------
	static bool LoadTest(const wchar_t* szTestFilePath, TEST& test);

	// --------------------------------------------------------------------------------------------
    void Compare(const wchar_t* szTest, const wchar_t* szScreenshotFilePath1, const wchar_t* szScreenshotFilePath2, CTestHtmlReport* pReport);
};

