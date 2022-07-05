#include "stdafx.h"
#include "_dxp_parser.h"

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#include <iostream>
#include <fstream>

using namespace std;

// ----------------------------------------------------------------------------------------------------
static inline string& ltrim(string& s) 
{
	s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));

	return s;
}

// ----------------------------------------------------------------------------------------------------
static inline string& rtrim(string& s) {
	s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());

	return s;
}

// ----------------------------------------------------------------------------------------------------
static inline string& trim(string& s) 
{	
	return ltrim(rtrim(s));
}

// ----------------------------------------------------------------------------------------------------
namespace _dxf
{
	// --------------------------------------------------------------------------------------------
	/*static*/ const string _group_codes::start = "0";

	// ------------------------------------------------------------------------------------------------
	_parser::_parser(int64_t iModel)
		: m_iModel(iModel)
	{
		assert(m_iModel != 0);
	}

	// ------------------------------------------------------------------------------------------------
	/*virtual*/ _parser::~_parser()
	{
	}

	// ------------------------------------------------------------------------------------------------
	void _parser::load(const wchar_t* szFile)
	{
		ifstream dxfFile(szFile);
		if (!dxfFile)
		{
			return;
		}

		string line;
		while (getline(dxfFile, line))
		{
			line = trim(line);

			if (line == _group_codes::start)
			{
				// New group
				if (!getline(dxfFile, line))
				{
					break;
				}

				line = trim(line);
				if (line == "SECTION")
				{

				}
			}
			else if (line == "0")
			{

			}
			else if (line == "0")
			{

			}
		} // while (getline(dxfFile, line))

		dxfFile.close();
	}
}