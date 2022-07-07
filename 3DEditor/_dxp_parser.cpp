#include "stdafx.h"
#include "_dxp_parser.h"

static inline string& ltrim(string& s) 
{
	s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));

	return s;
}

static inline string& rtrim(string& s) 
{
	s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());

	return s;
}

static inline string& trim(string& s) 
{	
	return ltrim(rtrim(s));
}

namespace _dxf
{
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// _dxf_reader
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	_dxf_reader::_dxf_reader(ifstream& dxfFile)
		: m_dxfFile(dxfFile)
		, m_vecBuffer()
	{
	}

	/*virtual*/ _dxf_reader::~_dxf_reader()
	{
	}

	vector<string>& _dxf_reader::buffer()
	{
		return m_vecBuffer;
	}

	bool _dxf_reader::read()
	{
		string line;
		if (m_dxfFile.eof() || !getline(m_dxfFile, line))
		{
			m_vecBuffer.clear();

			return false;
		}

		line = trim(line);
		if (line == _group_codes::eof)
		{
			m_vecBuffer.clear();

			return false;
		}

		m_vecBuffer.push_back(line);

		return true;
	}
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// _group_codes
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>	
	/*static*/ const string _group_codes::start = "0";
	/*static*/ const string _group_codes::name = "2";
	/*static*/ const string _group_codes::eof = "EOF";
	/*static*/ const string _group_codes::section = "SECTION";
	/*static*/ const string _group_codes::end_section = "ENDSEC";
	/*static*/ const string _group_codes::entities = "ENTITIES";
	/*static*/ const string _group_codes::line = "LINE";
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// _group
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>	
	_group::_group(const string& strName)
		: m_strName(strName)
	{
		assert(!m_strName.empty());
	}
	
	/*virtual*/ _group::~_group()
	{
	}

	const string& _group::name() const
	{
		return m_strName;
	}
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// _section
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	_section::_section(const string& strName)
		: _group(strName)
	{
	}
		
	/*virtual*/ _section::~_section()
	{
	}
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// _entities_section
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	_entities_section::_entities_section()
		: _section("ENTITIES")
	{
	}

	/*virtual*/ _entities_section::~_entities_section()
	{
	}

	void _entities_section::load(_dxf_reader& dxfReader)
	{
		while (true)
		{
			if (!dxfReader.read())
			{
				break; // EOF
			}

			if (dxfReader.buffer().back() == _group_codes::line)
			{
				dxfReader.buffer().clear();

				/*auto pEntitiesSection = new _entities_section();
				m_vecSections.push_back(pEntitiesSection);

				pEntitiesSection->load(dxfReader);*/
			}
			else
			{
				//ENTITIES
				continue; // todo
			}
		} // while (true)
	}
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// _parser
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	_parser::_parser(int64_t iModel)
		: m_iModel(iModel)
		, m_vecSections()
	{
		assert(m_iModel != 0);
	}

	/*virtual*/ _parser::~_parser()
	{
		for (size_t i = 0; i < m_vecSections.size(); i++)
		{
			delete m_vecSections[i];
		}
		m_vecSections.clear();
	}

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

		_dxf_reader dxfReader(dxfFile);
		while (true)
		{
			if (!dxfReader.read())
			{
				break; // EOF
			}

			if (dxfReader.buffer().back() == _group_codes::start)
			{
				if (!dxfReader.read())
				{
					break; // EOF
				}

				if (dxfReader.buffer().back() == _group_codes::section)
				{
					if (!dxfReader.read())
					{
						break; // EOF
					}

					if (dxfReader.buffer().back() != _group_codes::name)
					{
						continue; // Wrong format
					}

					if (dxfReader.buffer().back() == _group_codes::entities)
					{
						auto pEntitiesSection = new _entities_section();
						m_vecSections.push_back(pEntitiesSection);

						pEntitiesSection->load(dxfReader);
					}
					else
					{
						//HEADER, CLASSES, TABLES, BLOCKS, ENTITIES, OBJECTS, THUMBNAILIMAGE
						continue; // todo
					}					
				} // if (dxfReader.buffer().back() == _group_codes::section)
			} // if (dxfReader.buffer().back() == _group_codes::start)
		} // while (true)

		//// The first Group
		//string line;
		//while (getline(dxfFile, line))
		//{
		//	line = trim(line);			
		//	/*if (line == _group_codes::start)
		//	{
		//		break;
		//	}*/
		//} // while (getline(dxfFile, line))

		//if (line == _group_codes::start)
		//{
		//	if (getline(dxfFile, line))
		//	{
		//		line = trim(line);
		//		if (line == _group_codes::entities)
		//		{

		//		}
		//	}
		//}

		/*if (dxfFile.eof())
		{
			std::wcout << L"hhhhhhhhhhhhhhhhhh";
		}*/

		dxfFile.close();
	}
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
}