#include "stdafx.h"
#include "Test.h"

#include <fstream>
#include <iostream>
#include <codecvt>

using namespace std;

// ------------------------------------------------------------------------------------------------
CTest::CTest(CRDFOpenGLView* pOpenGLView)
	: m_pOpenGLView(pOpenGLView)
	, m_strRevision(L"")
	, m_strScreenshotFileName(L"")
{
	assert(m_pOpenGLView != nullptr);

	char* szTimeStamp = nullptr;
	int64_t iRevision = ::GetRevision(&szTimeStamp);

	wchar_t szBuffer[1024];
	swprintf(szBuffer, 1024, L"%lld; %s", iRevision, CA2W(szTimeStamp).m_psz);	

	m_strRevision = szBuffer;
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ CTest::~CTest()
{
}

// ------------------------------------------------------------------------------------------------
bool CTest::Save(const wchar_t* szTestFilePath, const wchar_t* szScreenshotFileName)
{
	wofstream streamTest(szTestFilePath);
	if (!streamTest)
	{
		return false;
	}

	locale loc(locale::classic(), new codecvt_utf8<wchar_t>);
	streamTest.imbue(loc);

	// Test version
	streamTest << VERSION << endl;

	// engine.dll version
	streamTest << m_strRevision << endl;

	// Screenshot file
	streamTest << wstring(szScreenshotFileName) << endl;

	// View settings - Translation
	float fXTranslation = 0.f;
	float fYTranslation = 0.f;
	float fZTranslation = 0.f;
	m_pOpenGLView->GetTranslation(fXTranslation, fYTranslation, fZTranslation);

	streamTest << fXTranslation << endl;
	streamTest << fYTranslation << endl;
	streamTest << fZTranslation << endl;

	// View settings - Rotation
	float fXRotation = 0.f;
	float fYRotation = 0.f;
	m_pOpenGLView->GetRotation(fXRotation, fYRotation);

	streamTest << fXRotation << endl;
	streamTest << fYRotation << endl;

	streamTest.close();

	return true;
}

// ------------------------------------------------------------------------------------------------
bool CTest::Load(const wchar_t* szTestFilePath)
{
	wifstream streamTest(szTestFilePath);
	if (!streamTest)
	{
		return false;
	}

	locale loc(locale::classic(), new codecvt_utf8<wchar_t>);
	streamTest.imbue(loc);

	// Test version
	wstring strVersion;
	if (!getline(streamTest, strVersion))
	{
		return false;
	}

	if (strVersion != L"1")
	{
		assert(false);

		return false;
	}

	// engine.dll version
	if (!getline(streamTest, m_strRevision))
	{
		return false;
	}

	// Screenshot file
	if (!getline(streamTest, m_strScreenshotFileName))
	{
		return false;
	}

	// View settings - Translation
	wstring strValue;
	if (!getline(streamTest, strValue))
	{
		return false;
	}

	float fXTranslation = (float)_wtof(strValue.c_str());

	if (!getline(streamTest, strValue))
	{
		return false;
	}

	float fYTranslation = (float)_wtof(strValue.c_str());

	if (!getline(streamTest, strValue))
	{
		return false;
	}

	float fZTranslation = (float)_wtof(strValue.c_str());

	// View settings - Rotation
	if (!getline(streamTest, strValue))
	{
		return false;
	}

	float fXRotation = (float)_wtof(strValue.c_str());

	if (!getline(streamTest, strValue))
	{
		return false;
	}

	float fYRotation = (float)_wtof(strValue.c_str());

	// Apply
	m_pOpenGLView->SetTranslation(fXTranslation, fYTranslation, fZTranslation, FALSE);
	m_pOpenGLView->SetRotation(fXRotation, fYRotation, TRUE);

	return true;
}

// ------------------------------------------------------------------------------------------------
const wchar_t* CTest::GetRevision() const
{
	return m_strRevision.c_str();
}

// ------------------------------------------------------------------------------------------------
const wchar_t* CTest::GetScreenshotFileName() const
{
	return m_strScreenshotFileName.c_str();
}