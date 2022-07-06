#include "stdafx.h"
#include "_dxp_parser.h"

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#include <iostream>
#include <fstream>

using namespace std;

// ------------------------------------------------------------------------------------------------
static inline string& ltrim(string& s) 
{
	s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));

	return s;
}

// ------------------------------------------------------------------------------------------------
static inline string& rtrim(string& s) 
{
	s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());

	return s;
}

// ------------------------------------------------------------------------------------------------
static inline string& trim(string& s) 
{	
	return ltrim(rtrim(s));
}

// ------------------------------------------------------------------------------------------------
namespace _dxf
{
	// --------------------------------------------------------------------------------------------
	/*static*/ const string _group_codes::start = "0";

	// --------------------------------------------------------------------------------------------
	/*static*/ const string _group_codes::name = "2";

	// --------------------------------------------------------------------------------------------
	/*static*/ const string _group_codes::eof = "EOF";

	// --------------------------------------------------------------------------------------------
	/*static*/ const string _group_codes::section = "SECTION";

	// --------------------------------------------------------------------------------------------
	/*static*/ const string _group_codes::end_section = "ENDSEC";

	// --------------------------------------------------------------------------------------------
	/*static*/ const string _group_codes::entities = "ENTITIES";

	// --------------------------------------------------------------------------------------------
	/*static*/ const string _group_codes::line = "LINE";

	// --------------------------------------------------------------------------------------------
	_group::_group(const string& strName)
		: m_strName(strName)
	{
		assert(!m_strName.empty());
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _group::~_group()
	{
	}

	// ----------------------------------------------------------------------------------------
	const string& _group::name() const
	{
		return m_strName;
	}

	// ----------------------------------------------------------------------------------------
	_section::_section(const string& strName)
		: _group(strName)
	{
	}

	// ----------------------------------------------------------------------------------------
	/*virtual*/ _section::~_section()
	{
	}

	// ----------------------------------------------------------------------------------------
	_entities_section::_entities_section()
		: _section("ENTITIES")
	{
	}

	// ----------------------------------------------------------------------------------------
	/*virtual*/ _entities_section::~_entities_section()
	{
	}

	// --------------------------------------------------------------------------------------------
	_parser::_parser(int64_t iModel)
		: m_iModel(iModel)
	{
		assert(m_iModel != 0);
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _parser::~_parser()
	{
	}

	// --------------------------------------------------------------------------------------------
	void _parser::load(const wchar_t* szFile)
	{
		if ((szFile == nullptr) || (wcslen(szFile) == 0))
		{
			assert(false);// #ex
		}
		
		ifstream dxfFile(szFile);
		if (!dxfFile)
		{
			return;// #ex
		}

		// The first Group
		string line;
		while (getline(dxfFile, line))
		{
			line = trim(line);			
			/*if (line == _group_codes::start)
			{
				break;
			}*/
		} // while (getline(dxfFile, line))

		if (line == _group_codes::start)
		{
			if (getline(dxfFile, line))
			{
				line = trim(line);
				if (line == _group_codes::entities)
				{

				}
			}
		}

		/*if (dxfFile.eof())
		{
			std::wcout << L"hhhhhhhhhhhhhhhhhh";
		}*/

		dxfFile.close();
	}
}