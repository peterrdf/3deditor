#include "stdafx.h"
#include "TestManager.h"
#include "Test.h"
#include "TestHtmlReport.h"

#include <fstream>
#include <iostream>
#include <codecvt>

using namespace std;

// ------------------------------------------------------------------------------------------------
CTestManager::CTestManager(CRDFController* pController, const wchar_t* szInputDataDir)
	: m_pController(pController)
	, m_pOpenGLView(nullptr)
	, m_strInputDataDir(szInputDataDir)
	, m_strTestsDir(_T(""))
	, m_strTestReportDir(_T(""))
{
	ASSERT(m_pController != nullptr);

	m_pOpenGLView = (COpenGLRDFView*)(m_pController->GetView<COpenGLRDFView>());
	ASSERT(m_pOpenGLView != nullptr);

	ASSERT(!m_strInputDataDir.IsEmpty());
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ CTestManager::~CTestManager()
{
}

// ------------------------------------------------------------------------------------------------
/*static*/ void CTestManager::FindFiles(const wchar_t* szFolder, const wchar_t* szWildcard, CStringList& lsFiles)
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

// ------------------------------------------------------------------------------------------------
void CTestManager::GenerateTests(const CString& strWildcards)
{
	int64_t iRevision = ::GetRevision(nullptr);

	wchar_t szRevision[1024];
	swprintf(szRevision, 1024, L"%lld", iRevision);

	CString strTestsDir = m_strInputDataDir;
	strTestsDir += L"\\Tests-";
	strTestsDir += szRevision;
	//strTestsDir += CTime::GetCurrentTime().Format(_T("%Y-%m-%d-%H-%M-%S"));

	if (!CreateDirectory((LPCTSTR)strTestsDir, nullptr) && (GetLastError() != ERROR_ALREADY_EXISTS))
	{
		AfxMessageBox(L"Error: can not create tests folder.");

		return;
	}

	CStringList lsInputFiles;

	if (!strWildcards.IsEmpty())
	{
		int iTokenPos = 0;
		CString strWildcard = strWildcards.Tokenize(_T("|"), iTokenPos);

		while (!strWildcard.IsEmpty())
		{
			FindFiles((LPCTSTR)m_strInputDataDir, strWildcard, lsInputFiles);

			strWildcard = strWildcards.Tokenize(_T("|"), iTokenPos);
		}
	}
	else
	{
		FindFiles((LPCTSTR)m_strInputDataDir, L"*.bin", lsInputFiles);
	}	

	if (lsInputFiles.IsEmpty())
	{
		AfxMessageBox(L"Error: there is no test data.");

		return;
	}

	m_pController->BeginTestMode();

	for (POSITION pos = lsInputFiles.GetHeadPosition(); pos != nullptr;)
	{
		CString strInputFileName = lsInputFiles.GetNext(pos);

		CString stTestDir = strTestsDir;
		stTestDir += "\\";
		stTestDir += strInputFileName;

		if (!CreateDirectory((LPCTSTR)stTestDir, nullptr) && (GetLastError() != ERROR_ALREADY_EXISTS))
		{
			AfxMessageBox(L"Error: can not create test folder.");

			continue;
		}

		CString strInputFilePath = m_strInputDataDir;
		strInputFilePath += L"\\";
		strInputFilePath += strInputFileName;

		m_pController->LoadModel((LPCTSTR)strInputFilePath);

		m_pOpenGLView->Reset();

		int iTestIndex = 0;
		CString strTestFilePath;
		CString strScreenshotFileName;
		CString strScreenshotFilePath;		

		// Test 1	
		{
			m_pOpenGLView->SetRotation(30.f, 30.f, TRUE);

			strTestFilePath.Format(L"%s\\test%d.3deditortest", (LPCTSTR)stTestDir, iTestIndex);
			strScreenshotFileName.Format(L"test%d.bmp", iTestIndex);
			strScreenshotFilePath.Format(L"%s\\%s", (LPCTSTR)stTestDir, (LPCTSTR)strScreenshotFileName);
			
			if (!m_pOpenGLView->SaveScreenshot((LPCTSTR)strScreenshotFilePath))
			{
				AfxMessageBox(L"Error: Can not save the screenshot.");

				m_pController->EndTestMode();

				return;
			}
			
			CTest test(m_pOpenGLView);
			test.Save(strTestFilePath, strScreenshotFileName);

			iTestIndex++;
		}		

		// Test 2
		{
			m_pOpenGLView->SetRotation(120.f, 120.f, TRUE);

			strTestFilePath.Format(L"%s\\test%d.3deditortest", (LPCTSTR)stTestDir, iTestIndex);
			strScreenshotFileName.Format(L"test%d.bmp", iTestIndex);
			strScreenshotFilePath.Format(L"%s\\%s", (LPCTSTR)stTestDir, (LPCTSTR)strScreenshotFileName);

			if (!m_pOpenGLView->SaveScreenshot((LPCTSTR)strScreenshotFilePath))
			{
				AfxMessageBox(L"Error: Can not save the screenshot.");

				m_pController->EndTestMode();

				return;
			}

			CTest test(m_pOpenGLView);
			test.Save(strTestFilePath, strScreenshotFileName);

			iTestIndex++;
		}
		
		// Test 3
		{
			m_pOpenGLView->SetRotation(210.f, 210.f, TRUE);

			strTestFilePath.Format(L"%s\\test%d.3deditortest", (LPCTSTR)stTestDir, iTestIndex);
			strScreenshotFileName.Format(L"test%d.bmp", iTestIndex);
			strScreenshotFilePath.Format(L"%s\\%s", (LPCTSTR)stTestDir, (LPCTSTR)strScreenshotFileName);

			if (!m_pOpenGLView->SaveScreenshot((LPCTSTR)strScreenshotFilePath))
			{
				AfxMessageBox(L"Error: Can not save the screenshot.");

				m_pController->EndTestMode();

				return;
			}

			CTest test(m_pOpenGLView);
			test.Save(strTestFilePath, strScreenshotFileName);

			iTestIndex++;
		}
	} // for (POSITION pos = ...	

	m_pController->EndTestMode();
}

// ------------------------------------------------------------------------------------------------
void CTestManager::ExecuteTests(const wchar_t* szTestsFolder)
{
	CString strTestsDir = szTestsFolder;

	m_strTestReportDir = strTestsDir;
	m_strTestReportDir += L"\\Test Report-";
	m_strTestReportDir += CTime::GetCurrentTime().Format(_T("%Y-%m-%d-%H-%M-%S"));

	if (!CreateDirectory((LPCTSTR)m_strTestReportDir, nullptr) && (GetLastError() != ERROR_ALREADY_EXISTS))
	{
		AfxMessageBox(L"Error: can not create test report folder.");

		return;
	}

	CStringList lsInputFiles;
	FindFiles((LPCTSTR)m_strInputDataDir, L"*.bin", lsInputFiles);

	if (lsInputFiles.IsEmpty())
	{
		AfxMessageBox(L"Error: there is no test data.");

		return;
	}

//	CTestHtmlReport testHtmlReport;
//	if (!testHtmlReport.Initialize((LPCTSTR)m_strTestReportDir))
//	{
//		AfxMessageBox(L"Error: can not create test report.");
//
//		return;
//	}

	m_pController->BeginTestMode();

	for (POSITION posInputFile = lsInputFiles.GetHeadPosition(); posInputFile != nullptr;)
	{
		CString strInputFileName = lsInputFiles.GetNext(posInputFile);

//		testHtmlReport.BeginTest((LPCTSTR)strInputFileName);

		CString stTestDir = strTestsDir;
		stTestDir += "\\";
		stTestDir += strInputFileName;

		CStringList lsTestFiles;
		FindFiles((LPCTSTR)stTestDir, L"*.3deditortest", lsTestFiles);

		if (lsTestFiles.IsEmpty())
		{
//			testHtmlReport.WriteError(L"Error: There are no tests.");

//			testHtmlReport.EndTest();

			continue;
		}

		CString strInputFilePath = m_strInputDataDir;
		strInputFilePath += L"\\";
		strInputFilePath += strInputFileName;

		m_pController->LoadModel((LPCTSTR)strInputFilePath);

		m_pOpenGLView->Reset();

		for (POSITION posTestFile = lsTestFiles.GetHeadPosition(); posTestFile != nullptr;)
		{
			CString strTestFileName = lsTestFiles.GetNext(posTestFile);			

			CString strTestFilePath = stTestDir;
			strTestFilePath += L"\\";
			strTestFilePath += strTestFileName;

			CTest test(m_pOpenGLView);
			if (!test.Load((LPCTSTR)strTestFilePath))
			{
//				testHtmlReport.WriteError(L"Error: Can not load test file.");

				continue;
			}

			CString strInfo;
			strInfo.Format(L"Test: %s, engine.dll, revision %s", (LPCTSTR)strTestFileName, test.GetRevision());
//			testHtmlReport.WriteInfo((LPCTSTR)strInfo);

			CString strScreenshotFilePath = stTestDir;
			strScreenshotFilePath += L"\\";
			strScreenshotFilePath += test.GetScreenshotFileName();		

//			Test((LPCTSTR)strInputFileName, (LPCTSTR)strTestFileName, (LPCTSTR)strScreenshotFilePath, &testHtmlReport);
		} // for (POSITION posTestFile = ...		

//		testHtmlReport.EndTest();
	} // for (POSITION posInputFile = ...

//	testHtmlReport.End();

	m_pController->EndTestMode();
}

// ------------------------------------------------------------------------------------------------
/*void CTestManager::Test(const wchar_t* szInputFileName, const wchar_t* szTestFileName, const wchar_t* szScreenShotFilePath, CTestHtmlReport* pReport)
{
	unsigned char* arPixels;
	unsigned int iWidth;
	unsigned int iHeight;
	m_pOpenGLView->TakeScreenshot(arPixels, iWidth, iHeight);

	ifstream streamScreenshot(szScreenShotFilePath, ios::binary);
	if (!streamScreenshot)
	{
		pReport->WriteError(L"Error: Can not open screenshot file.");

		return;
	}

	std::vector<unsigned char> vecExpectedPixels(std::istreambuf_iterator<char>(streamScreenshot), {});

	unsigned __int16 w = ((unsigned __int16*)vecExpectedPixels.data())[9];
	unsigned __int16 h = ((unsigned __int16*)vecExpectedPixels.data())[11];

	if ((w != iWidth) || (h != iHeight))
	{
		CString strError;
		strError.Format(_T("Error: Wrong dimensions: %d/%d vs %d/%d"), iWidth, iHeight, w, h);

		pReport->WriteError((LPCTSTR)strError);

		return;
	}

	const int TOLERANCE_PER_PIXEL = 51; // 20% => (255. / 100.) * 20. = 51

	long lDifferentPixels = 0;
	for (size_t b = 0; b < (size_t)w * (size_t)h * 3; b += 3)
	{
		int iDifferences = 0;

		// B
		iDifferences += abs(arPixels[b + 0] - vecExpectedPixels[b + 0 + 54/*header* /]);

		// G
		iDifferences += abs(arPixels[b + 1] - vecExpectedPixels[b + 1 + 54/*header* /]);

		// R
		iDifferences += abs(arPixels[b + 2] - vecExpectedPixels[b + 2 + 54/*header* /]);

		if (iDifferences > TOLERANCE_PER_PIXEL)
		{
			lDifferentPixels++;

			// Wrong - Red

			// B
			arPixels[b + 0] = 0;
			// G
			arPixels[b + 1] = 0;
			// R
			arPixels[b + 2] = 255;
		}
		else
		{
			// Correct - White

			// B
			arPixels[b + 0] = 255;
			// G
			arPixels[b + 1] = 255;
			// R
			arPixels[b + 2] = 255;			
		}
	}

	if (lDifferentPixels != 0)
	{
		CString strError;
		strError.Format(_T("Error: Differences (pixels): %d"), lDifferentPixels);

		pReport->WriteError((LPCTSTR)strError);

		CString strDifferencesFileName = szInputFileName;
		strDifferencesFileName += L"-";
		strDifferencesFileName += szTestFileName;
		strDifferencesFileName += L".bmp";

		CString strDifferencesFilePath = m_strTestReportDir;
		strDifferencesFilePath += L"\\";
		strDifferencesFilePath += strDifferencesFileName;
		

		if (::SaveScreenshot(arPixels, iWidth, iHeight, (LPCTSTR)strDifferencesFilePath))
		{
			pReport->WriteImage((LPCTSTR)strDifferencesFileName);
		}
		else
		{
			pReport->WriteError(L"Error: Can not save the screenshot.");
		}
	}
	else
	{
		pReport->WriteInfo(L"OK");
	}	

	free(arPixels);
}	//	*/