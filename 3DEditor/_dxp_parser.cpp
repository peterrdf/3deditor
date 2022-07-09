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
	// _error
	// --------------------------------------------------------------------------------------------
	/*static*/ const string _error::file_not_found = "File not found";
	/*static*/ const string _error::invalid_argument = "Invalid argument";
	/*static*/ const string _error::sof = "Start of file";
	/*static*/ const string _error::eof = "End of file";
	/*static*/ const string _error::invalid_format = "Invalid format";

	// --------------------------------------------------------------------------------------------
	_error::_error(const string& error)
		: runtime_error(error)
	{
	}
	// _error
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
	/*static*/ const string _group_codes::start_point_x = "10";
	/*static*/ const string _group_codes::start_point_y = "20";
	/*static*/ const string _group_codes::start_point_z = "30";
	/*static*/ const string _group_codes::end_point_x = "11";
	/*static*/ const string _group_codes::end_point_y = "21";
	/*static*/ const string _group_codes::end_point_z = "31";
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
	void _reader::load()
	{
		m_iRow = 0;
		m_vecRows.clear();

		string line;
		while (getline(m_dxfFile, line))
		{
			line = trim(line);
			if (!line.empty())
			{
				m_vecRows.push_back(line);
			}
		}

		if (m_vecRows.empty())
		{
			throw _error(_error::invalid_format);
		}
	}

	// --------------------------------------------------------------------------------------------
	const vector<string>& _reader::rows() const
	{
		return m_vecRows;
	}

	// ----------------------------------------------------------------------------------------
	size_t _reader::rowIndex() const
	{
		return m_iRow;
	}

	// --------------------------------------------------------------------------------------------
	const string& _reader::row()
	{
		return m_vecRows[m_iRow];
	}

	// --------------------------------------------------------------------------------------------
	void _reader::forth()
	{
		m_iRow++;

		if (m_iRow >= m_vecRows.size())
		{
			throw _error(_error::eof);
		}
	}

	// --------------------------------------------------------------------------------------------
	void _reader::back()
	{
		if (m_iRow == 0)
		{
			throw _error(_error::sof);
		}

		m_iRow--;
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
		, m_vecEntities()
	{
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _entities_section::~_entities_section()
	{
		for (size_t i = 0; i < m_vecEntities.size(); i++)
		{
			delete m_vecEntities[i];
		}
	}

	// --------------------------------------------------------------------------------------------
	void _entities_section::load(_reader& reader)
	{
		while (true)
		{
			if (reader.row() == _group_codes::start)
			{
				reader.forth();
				if (reader.row() == _group_codes::endsec)
				{
					reader.back();

					break;
				}

				if (reader.row() == _group_codes::line)
				{
					reader.forth();

					auto pLine = new _line();
					m_vecEntities.push_back(pLine);

					pLine->load(reader);
				} // if (reader.row() == _group_codes::line)
				else
				{
					// TODO: ALL ENTITIES
					reader.forth();
				}
			} // if (reader.row() == _group_codes::start)
			else
			{
				reader.forth();
			}
		} // while (true)
	}
	// _section
	// --------------------------------------------------------------------------------------------
	
	// --------------------------------------------------------------------------------------------
	// _line
	_line::_line()
		: _entity(_group_codes::line)
	{
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _line::~_line()
	{
	}

	// --------------------------------------------------------------------------------------------
	void _line::load(_reader& reader)
	{
		while (true)
		{
			if (reader.row() == _group_codes::start)
			{
				reader.forth();
				if (reader.row() != _group_codes::start_point_x)
				{
					reader.back();

					break;
				}
			}

			if (reader.row() == _group_codes::start_point_x)
			{
				reader.forth();
				//save
				reader.forth();
			}
			else if (reader.row() == _group_codes::start_point_y)
			{
				reader.forth();
				//save
				reader.forth();
			}
			else if (reader.row() == _group_codes::start_point_z)
			{
				reader.forth();
				//save
				reader.forth();
			}
			else if (reader.row() == _group_codes::end_point_x)
			{
				reader.forth();
				//save
				reader.forth();
			}
			else if (reader.row() == _group_codes::end_point_y)
			{
				reader.forth();
				//save
				reader.forth();
			}
			else if (reader.row() == _group_codes::end_point_z)
			{
				reader.forth();
				//save
				reader.forth();
			}
			else
			{
				reader.forth();
			}
		} // while (true)
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
	}

	// --------------------------------------------------------------------------------------------
	void _parser::load(const wchar_t* szFile)
	{
		if ((szFile == nullptr) || (wcslen(szFile) == 0))
		{
			throw _error(_error::invalid_argument);
		}
		
		ifstream dxfFile(szFile);
		if (!dxfFile)
		{
			throw _error(_error::file_not_found);
		}

		_reader reader(dxfFile);
		reader.load();		

		while (true)
		{	
			if (reader.row() == _group_codes::eof)
			{
				break;
			}

			if (reader.row() == _group_codes::start)
			{
				reader.forth();
				if (reader.row() == _group_codes::section)
				{
					reader.forth();
					if (reader.row() == _group_codes::name)
					{
						reader.forth();
						if (reader.row() == _group_codes::entities)
						{
							reader.forth();

							auto pEntitiesSection = new _entities_section();
							m_vecSections.push_back(pEntitiesSection);

							pEntitiesSection->load(reader);
						}
						else
						{
							// TODO: HEADER, CLASSES, TABLES, BLOCKS, ENTITIES, OBJECTS, THUMBNAILIMAGE
							reader.forth();
						}
					}
					else
					{
						throw _error(_error::invalid_format);
					}								
				} // if (reader.row() == _group_codes::section)
				else if (reader.row() == _group_codes::endsec)
				{
					reader.forth();
				}
			} // if (reader.row() == _group_codes::start)
			else
			{
				reader.forth();
			}
		} // while (true)

		/**
		* ifcengine
		*/
		//todo create instances
	}
	// _parser
	// --------------------------------------------------------------------------------------------
}