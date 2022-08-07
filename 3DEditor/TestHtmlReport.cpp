#include "stdafx.h"
#include "TestHtmlReport.h"

#include <xstring>

// ------------------------------------------------------------------------------------------------
CTestHtmlReport::CTestHtmlReport()
	: m_htmlReport()
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ CTestHtmlReport::~CTestHtmlReport()
{
}

// ------------------------------------------------------------------------------------------------
bool CTestHtmlReport::Initialize(const wchar_t* szDir)
{
	wstring strReportFilePath = szDir;
	strReportFilePath += L"\\index.html";

	m_htmlReport.open(strReportFilePath);
	if (!m_htmlReport)
	{
		ASSERT(FALSE); 

		return false;
	}

	return WriteHeader();
}

// ------------------------------------------------------------------------------------------------
bool CTestHtmlReport::BeginTest(const wchar_t* szInputFile)
{
	if (!m_htmlReport)
	{
		ASSERT(FALSE);

		return false;
	}

	m_htmlReport << wstring(L"<H3>");
	m_htmlReport << wstring(L"Input file: ");
	m_htmlReport << wstring(szInputFile);
	m_htmlReport << wstring(L"</H3>") << endl;

	return true;
}

// ------------------------------------------------------------------------------------------------
bool CTestHtmlReport::WriteInfo(const wchar_t* szMessage)
{
	if (!m_htmlReport)
	{
		ASSERT(FALSE);

		return false;
	}

	m_htmlReport << wstring(L"<div>");
	m_htmlReport << wstring(szMessage);
	m_htmlReport << wstring(L"</div>") << endl;

	return true;
}

// ------------------------------------------------------------------------------------------------
bool CTestHtmlReport::WriteImage(const wchar_t* szFilePath)
{
	if (!m_htmlReport)
	{
		ASSERT(FALSE);

		return false;
	}

	m_htmlReport << wstring(L"<img src=\"");
	m_htmlReport << wstring(szFilePath);
	m_htmlReport << wstring(L"\"></img>") << endl;

	return true;
}

// ------------------------------------------------------------------------------------------------
bool CTestHtmlReport::WriteError(const wchar_t* szMessage)
{
	if (!m_htmlReport)
	{
		ASSERT(FALSE);

		return false;
	}

	m_htmlReport << wstring(L"<div class=\"error\">");
	m_htmlReport << wstring(szMessage);
	m_htmlReport << wstring(L"</div>") << endl;

	return true;
}

// ------------------------------------------------------------------------------------------------
bool CTestHtmlReport::EndTest()
{
	if (!m_htmlReport)
	{
		ASSERT(FALSE);

		return false;
	}

	m_htmlReport << wstring(L"<hr />") << endl;

	return true;
}

// ------------------------------------------------------------------------------------------------
bool CTestHtmlReport::End()
{
	if (!m_htmlReport)
	{
		ASSERT(FALSE);

		return false;
	}

	return WriteFooter();
}

// ------------------------------------------------------------------------------------------------
bool CTestHtmlReport::WriteHeader()
{
	if (!m_htmlReport)
	{
		ASSERT(FALSE);

		return false;
	}

	m_htmlReport << wstring(L"<html>\n");

	m_htmlReport << wstring(L"<head>") << endl;

	m_htmlReport << wstring(L"<title>RDF LTD</title>") << endl;

	m_htmlReport << wstring(L"<style>") << endl;

	m_htmlReport << wstring(L".error { color: red; }") << endl;

	m_htmlReport << wstring(L"</style>") << endl;

	m_htmlReport << wstring(L"</head>") << endl;	

	m_htmlReport << wstring(L"<body>") << endl;

	m_htmlReport << wstring(L"<H1>Test Report</H1>") << endl;

	/*m_htmlReport << wstring(L"<H2>");

	m_htmlReport << wstring(L"engine.dll, revision ");

	char* szRevision = NULL;
	GetRevision(&szRevision);

	m_htmlReport << wstring(CA2W(szRevision));

	m_htmlReport << wstring(L"</H2>") << endl;*/

	m_htmlReport << wstring(L"<hr />") << endl;

	return true;
}

// ------------------------------------------------------------------------------------------------
bool CTestHtmlReport::WriteFooter()
{
	if (!m_htmlReport)
	{
		ASSERT(FALSE);

		return false;
	}

	m_htmlReport << wstring(L"</body>") << endl;

	m_htmlReport << wstring(L"</html>") << endl;

	return true;
}