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
	/*static*/ const string _entity::subclass_code = "100";
	/*static*/ const string _entity::layer_code = "8";

	// --------------------------------------------------------------------------------------------
	_entity::_entity(const string& strName)
		: _group(strName)
		, m_strLayer("")
	{
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _entity::~_entity()
	{
	}

	// --------------------------------------------------------------------------------------------
	const string& _entity::layer() const
	{
		return m_strLayer;
	}

	// ----------------------------------------------------------------------------------------
	bool _entity::_load(_reader& reader)
	{
		if (reader.row() == layer_code)
		{
			reader.forth();
			m_strLayer = reader.row();

			reader.forth(); return true;
		}
		
		if (reader.row() == subclass_code)
		{
			reader.forth();
			// TODO

			reader.forth(); return true;
		}

		return false;
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

	// ----------------------------------------------------------------------------------------
	const vector<_entity*>& _entities_section::entities() const
	{
		return m_vecEntities;
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
					reader.forth(); break;
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
		, m_dX1(0.)
		, m_dY1(0.)
		, m_dZ1(0.)
		, m_dX2(0.)
		, m_dY2(0.)
		, m_dZ2(0.)
	{
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _line::~_line()
	{
	}

	// ----------------------------------------------------------------------------------------
	/*virtual*/ int64_t _line::createInstance(int64_t iModel)
	{
		int64_t iClass = GetClassByName(iModel, "Line3D");
		assert(iClass != 0);

		vector<double> vecVertices
		{
			m_dX1,
			m_dY1,
			m_dZ1,
			m_dX2,
			m_dY2,
			m_dZ2,
		};

		int64_t iInstance = CreateInstance(iClass, "Line");
		assert(iInstance != 0);

		SetDataTypeProperty(iInstance, GetPropertyByName(iModel, "points"), vecVertices.data(), vecVertices.size());

		return iInstance;
	}

	// --------------------------------------------------------------------------------------------
	double _line::X1() const
	{
		return m_dX1;
	}

	// --------------------------------------------------------------------------------------------
	double _line::Y1() const
	{
		return m_dY1;
	}

	// --------------------------------------------------------------------------------------------
	double _line::Z1() const 
	{
		return m_dZ1;
	}

	// --------------------------------------------------------------------------------------------
	double _line::X2() const
	{
		return m_dX2;
	}

	// --------------------------------------------------------------------------------------------
	double _line::Y2() const
	{
		return m_dY2;
	}

	// --------------------------------------------------------------------------------------------
	double _line::Z2() const
	{
		return m_dZ2;
	}

	// --------------------------------------------------------------------------------------------
	void _line::load(_reader& reader)
	{
		while (true)
		{
			if (_load(reader))
			{
				continue;
			}

			if (reader.row() == _group_codes::start)
			{
				break;
			}

			if (reader.row() == _group_codes::start_point_x)
			{
				reader.forth();
				m_dX1 = atof(reader.row().c_str());

				reader.forth(); continue;
			}

			if (reader.row() == _group_codes::start_point_y)
			{
				reader.forth();
				m_dY1 = atof(reader.row().c_str());

				reader.forth(); continue;
			}

			if (reader.row() == _group_codes::start_point_z)
			{
				reader.forth();
				m_dZ1 = atof(reader.row().c_str());

				reader.forth(); continue;
			}

			if (reader.row() == _group_codes::end_point_x)
			{
				reader.forth();
				m_dX2 = atof(reader.row().c_str());

				reader.forth(); continue;
			}
			
			if (reader.row() == _group_codes::end_point_y)
			{
				reader.forth();
				m_dY2 = atof(reader.row().c_str());

				reader.forth(); continue;
			}
			
			if (reader.row() == _group_codes::end_point_z)
			{
				reader.forth();
				m_dZ2 = atof(reader.row().c_str());

				reader.forth(); continue;
			}

			reader.forth();
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

		/**
		* Load
		*/
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
						} // if (reader.row() == _group_codes::entities)
						else
						{
							// TODO: HEADER, CLASSES, TABLES, BLOCKS, ENTITIES, OBJECTS, THUMBNAILIMAGE
							reader.forth();
						}
					} // if (reader.row() == _group_codes::name)
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
		createInstances();		
	}

	// --------------------------------------------------------------------------------------------
	void _parser::createInstances()
	{
		/**
		* Create ifcengine instances
		*/
		map<string, vector<int64_t>> mapLayer2Instances;
		for (size_t iSection = 0; iSection < m_vecSections.size(); iSection++)
		{
			if (m_vecSections[iSection]->name() == _group_codes::entities)
			{
				auto pEntitiesSection = dynamic_cast<_entities_section*>(m_vecSections[iSection]);
				assert(pEntitiesSection != nullptr);

				for (size_t iEntity = 0; iEntity < pEntitiesSection->entities().size(); iEntity++)
				{
					auto pEntity = pEntitiesSection->entities()[iEntity];
					assert(pEntity != nullptr);

					auto iInstance = pEntity->createInstance(m_iModel);
					assert(iInstance != 0);

					auto strLayer = pEntity->layer();
					
					auto itLayer2Instances = mapLayer2Instances.find(strLayer);
					if (itLayer2Instances != mapLayer2Instances.end())
					{
						mapLayer2Instances[strLayer].push_back(iInstance);
					}
					else
					{
						mapLayer2Instances[strLayer] = vector<int64_t> { iInstance };
					}
				} // for (size_t iEntity = ...
			} // if (m_vecSections[iSection]->name() == _group_codes::entities)
		} // for (size_t iSection = ...

		/**
		* Create ifcengine collections
		*/
		int64_t iCollectionClass = GetClassByName(m_iModel, "Collection");
		assert(iCollectionClass != 0);

		auto itLayer2Instances = mapLayer2Instances.begin();
		for (; itLayer2Instances != mapLayer2Instances.end(); itLayer2Instances++)
		{
			string strCollectionName = "Layer: ";
			strCollectionName += itLayer2Instances->first;

			int64_t iCollectionInstance = CreateInstance(iCollectionClass, strCollectionName.c_str());
			SetObjectProperty(iCollectionInstance, GetPropertyByName(m_iModel, "objects"), itLayer2Instances->second.data(), itLayer2Instances->second.size());
		}
	}
	// _parser
	// --------------------------------------------------------------------------------------------
}