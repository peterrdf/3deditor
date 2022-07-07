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
	// --------------------------------------------------------------------------------------------
	// _exception
	// --------------------------------------------------------------------------------------------
	_exception::_exception(const string& error)
		: runtime_error(error)
	{
	}
	// _exception
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _group_codes
	// --------------------------------------------------------------------------------------------
	/*static*/ const string _group_codes::start = "0";
	/*static*/ const string _group_codes::name = "2";
	/*static*/ const string _group_codes::eof = "EOF";
	/*static*/ const string _group_codes::section = "SECTION";
	/*static*/ const string _group_codes::endsec = "ENDSEC";
	/*static*/ const string _group_codes::entities = "ENTITIES";
	/*static*/ const string _group_codes::line = "LINE";
	// _group_codes
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _reader
	// --------------------------------------------------------------------------------------------
	_reader::_reader(ifstream& dxfFile)
		: m_dxfFile(dxfFile)
		, m_vecRows()
		, m_iRow(0)
	{
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _reader::~_reader()
	{
	}

	// --------------------------------------------------------------------------------------------
	const string& _reader::row()
	{
		return m_vecRows[m_iRow];
	}

	// --------------------------------------------------------------------------------------------
	bool _reader::read()
	{
		string line;
		if (m_dxfFile.eof() || !getline(m_dxfFile, line))
		{
			return false;
		}

		line = trim(line);
		m_vecRows.push_back(line);

		m_iRow = m_vecRows.size() - 1;

		return true;
	}

	// --------------------------------------------------------------------------------------------
	void _reader::back()
	{
		if (m_iRow == 0)
		{
			throw _exception("Internal error");
		}

		m_iRow--;

		if (m_iRow >= m_vecRows.size())
		{
			throw _exception("Internal error");
		}
	}
	// _reader
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _group
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

	// --------------------------------------------------------------------------------------------
	const string& _group::name() const
	{
		return m_strName;
	}
	// _group
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _section
	// --------------------------------------------------------------------------------------------
	_section::_section(const string& strName)
		: _group(strName)
	{
	}
		
	// --------------------------------------------------------------------------------------------
	/*virtual*/ _section::~_section()
	{
	}
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _entities_section
	_entities_section::_entities_section()
		: _section("ENTITIES")
	{
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _entities_section::~_entities_section()
	{
	}

	// --------------------------------------------------------------------------------------------
	void _entities_section::load(_reader& reader)
	{
		while (true)
		{
			if (!reader.read())
			{
				break; // EOF
			}

			if (reader.row() == _group_codes::line)
			{
				/*auto pEntitiesSection = new _entities_section();
				m_vecSections.push_back(pEntitiesSection);

				pEntitiesSection->load(reader);*/
			}
			else
			{
				//ENTITIES except line
				continue; // todo
			}
		} // while (true)
	}
	// _section
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _entity
	// --------------------------------------------------------------------------------------------
	_entity::_entity(const string& strName)
		: _group(strName)
	{
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _entity::~_entity()
	{	
	}
	// _entity
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _line
	// --------------------------------------------------------------------------------------------
	

	// --------------------------------------------------------------------------------------------
	_line::_line()
		: _entity(_group_codes::line)
	{
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _line::~_line()
	{
	}
	// _line
	// --------------------------------------------------------------------------------------------
	
	// --------------------------------------------------------------------------------------------
	// _parser
	// --------------------------------------------------------------------------------------------
	_parser::_parser(int64_t iModel)
		: m_iModel(iModel)
		, m_vecSections()
	{
		assert(m_iModel != 0);
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _parser::~_parser()
	{
		for (size_t i = 0; i < m_vecSections.size(); i++)
		{
			delete m_vecSections[i];
		}
		m_vecSections.clear();
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

		_reader reader(dxfFile);
		while (true)
		{
			if (!reader.read())
			{
				break; // EOF
			}

			if (reader.row() == _group_codes::start)
			{
				if (!reader.read())
				{
					break; // EOF
				}

				if (reader.row() == _group_codes::eof)
				{
					break; // EOF
				}

				if (reader.row() == _group_codes::section)
				{
					if (!reader.read())
					{
						break; // EOF
					}

					if (reader.row() != _group_codes::name)
					{
						continue; // Wrong format
					}

					if (!reader.read())
					{
						break; // EOF
					}

					if (reader.row() == _group_codes::entities)
					{
						auto pEntitiesSection = new _entities_section();
						m_vecSections.push_back(pEntitiesSection);

						pEntitiesSection->load(reader);
					}
					else
					{
						//HEADER, CLASSES, TABLES, BLOCKS, ENTITIES, OBJECTS, THUMBNAILIMAGE
						continue; // todo
					}					
				} // if (reader.buffer().back() == _group_codes::section)
			} // if (reader.row() == _group_codes::start)
		} // while (true)

		dxfFile.close();
	}
	// _parser
	// --------------------------------------------------------------------------------------------
}