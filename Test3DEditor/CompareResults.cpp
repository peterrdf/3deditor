#include "stdafx.h"
#include "CompareResults.h"
#include "_test.h"

#include <string>
#include <vector>

using namespace std;

CCompareResults::CCompareResults(const CString& strTestResults1, const CString& strTestResults2, const CString& strTestReportDir)
	: m_iComparisonIndex(0)
	, m_strTestResults1(strTestResults1)
	, m_strTestResults2(strTestResults2)
	, m_strTestReportDir(L"")
{
	m_strTestReportDir = strTestReportDir;
	m_strTestReportDir += L"\\Test Report-";
	m_strTestReportDir += CTime::GetCurrentTime().Format(_T("%Y-%m-%d-%H-%M-%S"));
}

void CCompareResults::Execute()
{
	m_iComparisonIndex = 0;

	CStringList lsTests;
	FindTests((LPCTSTR)m_strTestResults1, lsTests);

	if (lsTests.IsEmpty())
	{
		::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Error: there is no test data.", L"Error", MB_ICONERROR | MB_OK);

		return;
	}	

	if (!CreateDirectory((LPCTSTR)m_strTestReportDir, nullptr) && (GetLastError() != ERROR_ALREADY_EXISTS))
	{
		::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Error: can not create test report folder.", L"Error", MB_ICONERROR | MB_OK);

		return;
	}

	CString strMessage;
	CTestHtmlReport testHtmlReport;
	if (!testHtmlReport.Initialize((LPCTSTR)m_strTestReportDir))
	{
		AfxMessageBox(L"Error: can not create test report.");

		return;
	}

	for (POSITION posTest = lsTests.GetHeadPosition(); posTest != nullptr;)
	{
		CString strTest = lsTests.GetNext(posTest);

		CString stTestDir = m_strTestResults1;
		stTestDir += "\\";
		stTestDir += strTest;

		CStringList lsTestFiles;
		FindFiles((LPCTSTR)stTestDir, L"*.3deditortest", lsTestFiles);

		testHtmlReport.BeginTest((LPCTSTR)strTest);

		if (lsTestFiles.IsEmpty())
		{
			testHtmlReport.WriteError(L"Error: There are no tests.");

			testHtmlReport.EndTest();

			continue;
		}

		for (POSITION posTestFile = lsTestFiles.GetHeadPosition(); posTestFile != nullptr;)
		{
			CString strTestFileName = lsTestFiles.GetNext(posTestFile);

			CString strTest1FilePath = m_strTestResults1;
			strTest1FilePath += "\\";
			strTest1FilePath += strTest;
			strTest1FilePath += "\\";
			strTest1FilePath += strTestFileName;

			CString strTest2FilePath = m_strTestResults2;
			strTest2FilePath += "\\";
			strTest2FilePath += strTest;
			strTest2FilePath += "\\";
			strTest2FilePath += strTestFileName;

			strMessage.Format(L"Test: '%s' vs '%s'", (LPCTSTR)m_strTestResults1, (LPCTSTR)m_strTestResults2);
			testHtmlReport.WriteInfo((LPCTSTR)strMessage);

			TEST test1;
			if (!LoadTest(strTest1FilePath, test1))
			{
				ASSERT(FALSE);

				strMessage.Format(L"Error: can not load '%s'", (LPCTSTR)strTest1FilePath);
				testHtmlReport.WriteError((LPCTSTR)strMessage);

				continue;
			}			

			TEST test2;
			if (!LoadTest(strTest2FilePath, test2))
			{
				ASSERT(FALSE);

				strMessage.Format(L"Error: can not load '%s'", (LPCTSTR)strTest2FilePath);
				testHtmlReport.WriteError((LPCTSTR)strMessage);
				
				continue; 
			}

			if (!AreTestIdentical(test1, test2))
			{
				ASSERT(FALSE);

				testHtmlReport.WriteError(L"Error: test settings do not match.");
			}

			CString strScreenshotFilePath1 = m_strTestResults1;
			strScreenshotFilePath1 += "\\";
			strScreenshotFilePath1 += strTest;
			strScreenshotFilePath1 += "\\";
			strScreenshotFilePath1 += test1.SCREENSHOT;

			CString strScreenshotFilePath2 = m_strTestResults2;
			strScreenshotFilePath2 += "\\";
			strScreenshotFilePath2 += strTest;
			strScreenshotFilePath2 += "\\";
			strScreenshotFilePath2 += test2.SCREENSHOT;

			strMessage.Format(L"Revision: '%s' vs '%s'", (LPCTSTR)test1.REVISION, (LPCTSTR)test2.REVISION);
			testHtmlReport.WriteInfo((LPCTSTR)strMessage);

			strMessage.Format(L"Screenshot: '%s' vs '%s'", (LPCTSTR)strScreenshotFilePath1, (LPCTSTR)strScreenshotFilePath2);
			testHtmlReport.WriteInfo((LPCTSTR)strMessage);

			Compare((LPCTSTR)strTest, (LPCTSTR)strScreenshotFilePath1, (LPCTSTR)strScreenshotFilePath2, &testHtmlReport);

			testHtmlReport.EndTest();
		} // for (POSITION posTestFile = ...
	} // for (POSITION posTest = ...
}

/*static*/ void CCompareResults::FindFiles(const wchar_t* szFolder, const wchar_t* szWildcard, CStringList& lsFiles)
{
	ASSERT(szFolder != nullptr);
	ASSERT(szWildcard != nullptr);

	CString strFolder = szFolder;

	CString strWildcard(strFolder);
	strWildcard += _T("\\");
	strWildcard += szWildcard;

	CFileFind fileFind;
	BOOL bResults = fileFind.FindFile(strWildcard);
	while (bResults)
	{
		bResults = fileFind.FindNextFile();

		if (!fileFind.IsDots() && !fileFind.IsDirectory())
		{
			CString strInputFile = fileFind.GetFilePath();
			strInputFile.Replace(strFolder + L"\\", L"");

			lsFiles.AddTail(strInputFile);
		}
	}

	fileFind.Close();
}

/*static*/ void CCompareResults::FindTests(const wchar_t* szFolder, CStringList& lsTests)
{
	ASSERT(szFolder != nullptr);

	CString strFolder = szFolder;

	CString strWildcard(strFolder);
	strWildcard += _T("\\");
	strWildcard += _T("*.*");

	CFileFind fileFind;
	BOOL bResults = fileFind.FindFile(strWildcard);
	while (bResults)
	{
		bResults = fileFind.FindNextFile();

		if (!fileFind.IsDots() && fileFind.IsDirectory())
		{
			CString strTest = fileFind.GetFilePath();
			strTest.Replace(strFolder + L"\\", L"");

			lsTests.AddTail(strTest);
		}
	}

	fileFind.Close();
}

/*static*/ bool CCompareResults::LoadTest(const wchar_t* szTestFilePath, TEST& test)
{
	wifstream streamTest(szTestFilePath);
	if (!streamTest)
	{
		return false;
	}

	locale loc(locale::classic(), new codecvt_utf8<wchar_t>);
	streamTest.imbue(loc);

	wstring strValue;

	// Test version	
	if (!getline(streamTest, strValue))
	{
		return false;
	}

	if (strValue != L"1")
	{
		ASSERT(FALSE);

		return false;
	}

	test.VERSION = strValue.c_str();

	// engine.dll version
	if (!getline(streamTest, strValue))
	{
		return false;
	}

	test.REVISION = strValue.c_str();

	// Screenshot file	
	if (!getline(streamTest, strValue))
	{
		return false;
	}

	test.SCREENSHOT = strValue.c_str();

	// View settings - Translation	
	if (!getline(streamTest, strValue))
	{
		return false;
	}

	test.Tx = (float)_wtof(strValue.c_str());

	if (!getline(streamTest, strValue))
	{
		return false;
	}

	test.Ty = (float)_wtof(strValue.c_str());

	if (!getline(streamTest, strValue))
	{
		return false;
	}

	test.Tz = (float)_wtof(strValue.c_str());

	// View settings - Rotation
	if (!getline(streamTest, strValue))
	{
		return false;
	}

	test.Rx = (float)_wtof(strValue.c_str());

	if (!getline(streamTest, strValue))
	{
		return false;
	}

	test.Ry = (float)_wtof(strValue.c_str());

	return true;
}

// https://stackoverflow.com/questions/48653689/c-sharp-comparing-2-bitmaps-with-tolerance#
static double CompareColours(const unsigned char* pixel1, const unsigned char* pixel2)
{
	return
		(pow((double)pixel1[0] - (double)pixel2[0], 2.) + 
		pow((double)pixel1[1] - (double)pixel2[1], 2) + 
		pow((double)pixel1[2] - (double)pixel2[2], 2)) / 198608. /*pow(256, 2) * 3*/;
}

/*static*/ void CCompareResults::Compare(const wchar_t* szTest, const wchar_t* szScreenshotFilePath1, const wchar_t* szScreenshotFilePath2, CTestHtmlReport* pReport)
{
	CString strError;

	ifstream streamScreenshot1(szScreenshotFilePath1, ios::binary);
	if (!streamScreenshot1)
	{
		strError.Format(L"Error: Can not open screenshot file '%s'.", szScreenshotFilePath1);
		pReport->WriteError(strError);

		return;
	}

	ifstream streamScreenshot2(szScreenshotFilePath2, ios::binary);
	if (!streamScreenshot2)
	{
		strError.Format(L"Error: Can not open screenshot file '%s'.", szScreenshotFilePath2);
		pReport->WriteError(strError);

		return;
	}

	std::vector<unsigned char> vecTest1Pixels(std::istreambuf_iterator<char>(streamScreenshot1), {});
	unsigned __int16 w1 = ((unsigned __int16*)vecTest1Pixels.data())[9];
	unsigned __int16 h1 = ((unsigned __int16*)vecTest1Pixels.data())[11];

	std::vector<unsigned char> vecTest2Pixels(std::istreambuf_iterator<char>(streamScreenshot2), {});
	unsigned __int16 w2 = ((unsigned __int16*)vecTest2Pixels.data())[9];
	unsigned __int16 h2 = ((unsigned __int16*)vecTest2Pixels.data())[11];

	if ((w1 != w2) || (h1 != h2))
	{		
		strError.Format(_T("Error: Wrong dimensions: %d/%d vs %d/%d"), h1, w1, w2, h2);
		pReport->WriteError((LPCTSTR)strError);

		return;
	}

	//const int TOLERANCE_PER_PIXEL = 51; // 20% => (255. / 100.) * 20. = 51

	long lDifferentPixels = 0;
	for (size_t b = 0; b < (size_t)w1 * (size_t)h1 * 3; b += 3)
	{
		////int iDifferences = 0;

		////// B
		////iDifferences += abs(vecTest2Pixels[b + 0] - vecTest1Pixels[b + 0 + 54/*header*/]);

		////// G
		////iDifferences += abs(vecTest2Pixels[b + 1] - vecTest1Pixels[b + 1 + 54/*header*/]);

		////// R
		////iDifferences += abs(vecTest2Pixels[b + 2] - vecTest1Pixels[b + 2 + 54/*header*/]);

		//if (iDifferences > TOLERANCE_PER_PIXEL)

		// https://stackoverflow.com/questions/48653689/c-sharp-comparing-2-bitmaps-with-tolerance#
		double dDifference = CompareColours(&vecTest1Pixels[b], &vecTest2Pixels[b]);
		if (dDifference > 0.01) // ???
		{
			lDifferentPixels++;

			// Wrong - Red

			// B
			vecTest2Pixels[b + 0] = 0;
			// G
			vecTest2Pixels[b + 1] = 0;
			// R
			vecTest2Pixels[b + 2] = 255;
		}
		else
		{
			// Correct - White

			// B
			vecTest2Pixels[b + 0] = 255;
			// G
			vecTest2Pixels[b + 1] = 255;
			// R
			vecTest2Pixels[b + 2] = 255;
		}
	}

	if (lDifferentPixels > 0) // ???
	{
		pReport->WriteImage(szScreenshotFilePath1);
		pReport->WriteImage(szScreenshotFilePath2);

		CString strError;
		strError.Format(_T("Error: Differences (pixels): %d"), lDifferentPixels);

		pReport->WriteError((LPCTSTR)strError);

		CString strDifferencesFileName;
		strDifferencesFileName.Format(L"Comparison%d.bmp", m_iComparisonIndex++);

		CString strDifferencesFilePath = m_strTestReportDir;
		strDifferencesFilePath += L"\\";
		strDifferencesFilePath += strDifferencesFileName;

		if (::SaveScreenshot(vecTest2Pixels.data(), w1, h1, (LPCTSTR)strDifferencesFilePath))
		{
			pReport->WriteImage((LPCTSTR)strDifferencesFileName);
		}
		else
		{
			pReport->WriteError(L"Error: Can not save the screenshot.");
		}
	} // if (lDifferentPixels > 0)
	else
	{
		pReport->WriteInfo(L"OK");
	}
}