// Test3DEditor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdafx.h"

#include "CompareResults.h"

#include <iostream>
#include <atlstr.h>
#include <assert.h>

int main(int argc, char* argv[])
{
    CString stCommandLine = GetCommandLine();

	CString strTestResults1;
	CString strTestResults2;
	CString strTestReportDir;

	int iTokenPos = 0;
	CString strToken = stCommandLine.Tokenize(_T("#"), iTokenPos);

	int iArgument = -1;
	while (!strToken.IsEmpty())
	{
		iArgument++;

		if (iArgument == 1)
		{
			strTestResults1 = strToken;
		}
		else if (iArgument == 2)
		{
			strTestResults2 = strToken;
		}
		else if (iArgument == 3)
		{
			strTestReportDir = strToken;
		}
		else if (iArgument > 3)
		{
			ASSERT(FALSE); // unsupported!
		}

		strToken = stCommandLine.Tokenize(_T("#"), iTokenPos);
	} // while (!strToken.IsEmpty())

	CCompareResults compareResults(strTestResults1, strTestResults2, strTestReportDir);
	compareResults.Execute();

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
