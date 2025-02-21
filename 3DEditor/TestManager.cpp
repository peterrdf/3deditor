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
	assert(m_pController != nullptr);

	m_pOpenGLView = (CRDFOpenGLView*)(m_pController->getViewAs<CRDFOpenGLView>());
	assert(m_pOpenGLView != nullptr);

	assert(!m_strInputDataDir.IsEmpty());
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ CTestManager::~CTestManager()
{
}

// ------------------------------------------------------------------------------------------------
/*static*/ void CTestManager::FindFiles(const wchar_t* szFolder, const wchar_t* szWildcard, CStringList& lsFiles)
{
	assert(szFolder != nullptr);
	assert(szWildcard != nullptr);

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
	int64_t iRevision = ::GetRevision();

	wchar_t szRevision[1024];
	swprintf(szRevision, 1024, L"%lld", iRevision);

	CString strTestsDir = m_strInputDataDir;
	strTestsDir += L"\\Tests-engine-rev.";
	strTestsDir += szRevision;
	strTestsDir += CTime::GetCurrentTime().Format(_T("-%Y-%m-%d-%H-%M-%S"));

	if (!CreateDirectory((LPCTSTR)strTestsDir, nullptr) && (GetLastError() != ERROR_ALREADY_EXISTS))
	{
		::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Error: can not create tests folder.", L"Error", MB_ICONERROR | MB_OK);

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
		::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Error: there is no test data.", L"Error", MB_ICONERROR | MB_OK);

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
			::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Error: can not create test folder.", L"Error", MB_ICONERROR | MB_OK);

			continue;
		}

		CString strInputFilePath = m_strInputDataDir;
		strInputFilePath += L"\\";
		strInputFilePath += strInputFileName;

		m_pController->LoadModel((LPCTSTR)strInputFilePath);

		m_pOpenGLView->_reset();

		int iTestIndex = 0;
		CString strTestFilePath;
		CString strScreenshotFileName;
		CString strScreenshotFilePath;

		// .bin file
		{
			CString strBinFilePath;
			strBinFilePath.Format(L"%s\\data.bin", (LPCTSTR)stTestDir);
			if (!CopyFile(strInputFilePath, strBinFilePath, TRUE))
			{
				::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Error: Can not save the .bin file.", L"Error", MB_ICONERROR | MB_OK);

				m_pController->EndTestMode();

				return;
			}
		}

		// Test 1	
		{
			m_pOpenGLView->SetRotation(30.f, 30.f, TRUE);

			strTestFilePath.Format(L"%s\\test%d.3deditortest", (LPCTSTR)stTestDir, iTestIndex);
			strScreenshotFileName.Format(L"test%d.bmp", iTestIndex);
			strScreenshotFilePath.Format(L"%s\\%s", (LPCTSTR)stTestDir, (LPCTSTR)strScreenshotFileName);
			
			if (!m_pOpenGLView->SaveScreenshot((LPCTSTR)strScreenshotFilePath))
			{
				::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Error: Can not save the screenshot.", L"Error", MB_ICONERROR | MB_OK);

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
				::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Error: Can not save the screenshot.", L"Error", MB_ICONERROR | MB_OK);

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
				::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Error: Can not save the screenshot.", L"Error", MB_ICONERROR | MB_OK);

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
