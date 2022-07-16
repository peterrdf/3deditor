#include "stdafx.h"
#include "_dxp_parser.h"

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
static inline double vector3_normalize(double& dX, double& dY, double& dZ)
{
	double dSize = pow(dX, 2.) + pow(dY, 2.) + pow(dZ, 2.);
	if (dSize > 0.0000000000000001)
	{
		double dSqrtSize = sqrt(dSize);

		dX /= dSqrtSize;
		dY /= dSqrtSize;
		dZ /= dSqrtSize;

		return dSqrtSize;
	}
	else 
	{
		dX = 0.;
		dY = 0.;
		dZ = 0.;

		return	0.;
	}
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

	// --------------------------------------------------------------------------------------------
	/*static*/ const string _group_codes::entities = "ENTITIES";
	/*static*/ const string _group_codes::header = "HEADER";
	/*static*/ const string _group_codes::tables = "TABLES";
	/*static*/ const string _group_codes::blocks = "BLOCKS";
	/*static*/ const string _group_codes::line = "LINE";
	/*static*/ const string _group_codes::vertex = "VERTEX";
	/*static*/ const string _group_codes::polyline = "POLYLINE";
	/*static*/ const string _group_codes::seqend = "SEQEND";
	/*static*/ const string _group_codes::circle = "CIRCLE";

	// --------------------------------------------------------------------------------------------
	/*static*/ const string _group_codes::subclass = "100";
	/*static*/ const string _group_codes::layer = "8";
	/*static*/ const string _group_codes::x = "10";
	/*static*/ const string _group_codes::y = "20";
	/*static*/ const string _group_codes::z = "30";
	/*static*/ const string _group_codes::x2 = "11";
	/*static*/ const string _group_codes::y2 = "21";
	/*static*/ const string _group_codes::z2 = "31";
	/*static*/ const string _group_codes::radius = "40";
	/*static*/ const string _group_codes::extrusion_x = "210";
	/*static*/ const string _group_codes::extrusion_y = "220";
	/*static*/ const string _group_codes::extrusion_z = "230";
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
			m_vecRows.push_back(line);
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
	const string& _reader::forth()
	{
		m_iRow++;

		if (m_iRow >= m_vecRows.size())
		{
			throw _error(_error::eof);
		}

		return row();
	}

	// --------------------------------------------------------------------------------------------
	const string& _reader::back()
	{
		if (m_iRow == 0)
		{
			throw _error(_error::sof);
		}

		m_iRow--;

		return row();
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
		, m_mapCode2Value()
	{
		// Common Group Codes for Entities, page 61
		m_mapCode2Value =
		{
			{ _group_codes::subclass, "" },
			{ _group_codes::layer, "" },
		};
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _entity::~_entity()
	{
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ void _entity::load(_reader& reader)
	{
		while (true)
		{
			if (reader.row() == _group_codes::start)
			{
				break;
			}

			auto itCode2Value = m_mapCode2Value.find(reader.row());
			if (itCode2Value != m_mapCode2Value.end())
			{
				itCode2Value->second = reader.forth();
			}

			reader.forth();
		} // while (true)
	}

	// --------------------------------------------------------------------------------------------
	const string& _entity::value(const string& strCode)
	{
		return m_mapCode2Value[strCode];
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
	// _section
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _entities_section
	_entities_section::_entities_section()
		: _section(_group_codes::entities)
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
					reader.forth(); 
					
					break;
				}

				if (reader.row() == _group_codes::line)
				{
					reader.forth();

					auto pLine = new _line();
					m_vecEntities.push_back(pLine);

					pLine->load(reader);
				} // if (reader.row() == _group_codes::line)
				else if (reader.row() == _group_codes::polyline)
				{
					reader.forth();

					auto pPolyline = new _polyline();
					m_vecEntities.push_back(pPolyline);

					pPolyline->load(reader);
				}
				else if (reader.row() == _group_codes::circle)
				{
					reader.forth();

					auto pCircle = new _circle();
					m_vecEntities.push_back(pCircle);

					pCircle->load(reader);
				}
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
	// _entities_section
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _header_section
	_header_section::_header_section()
		: _section(_group_codes::header)
	{
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _header_section::~_header_section()
	{
	}

	// --------------------------------------------------------------------------------------------
	void _header_section::load(_reader& reader)
	{
		while (true)
		{
			if (reader.row() == _group_codes::start)
			{
				reader.forth();
				if (reader.row() == _group_codes::endsec)
				{
					reader.forth(); 
					
					break;
				}
			} // if (reader.row() == _group_codes::start)
			else
			{
				reader.forth();
			}
		} // while (true)
	}
	// _header_section
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _tables_section
	_tables_section::_tables_section()
		: _section(_group_codes::tables)
	{
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _tables_section::~_tables_section()
	{
	}

	// --------------------------------------------------------------------------------------------
	void _tables_section::load(_reader& reader)
	{
		while (true)
		{
			if (reader.row() == _group_codes::start)
			{
				reader.forth();
				if (reader.row() == _group_codes::endsec)
				{
					reader.forth();

					break;
				}
			} // if (reader.row() == _group_codes::start)
			else
			{
				reader.forth();
			}
		} // while (true)
	}
	// _tables_section
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _blocks_section
	_blocks_section::_blocks_section()
		: _section(_group_codes::blocks)
	{
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _blocks_section::~_blocks_section()
	{
	}

	// --------------------------------------------------------------------------------------------
	void _blocks_section::load(_reader& reader)
	{
		while (true)
		{
			if (reader.row() == _group_codes::start)
			{
				reader.forth();
				if (reader.row() == _group_codes::endsec)
				{
					reader.forth();

					break;
				}
			} // if (reader.row() == _group_codes::start)
			else
			{
				reader.forth();
			}
		} // while (true)
	}
	// _blocks_section
	// --------------------------------------------------------------------------------------------
	
	// --------------------------------------------------------------------------------------------
	// _line
	_line::_line()
		: _entity(_group_codes::line)
	{
		// LINE, page 101
		map<string, string> mapCode2Value =
		{	
			{_group_codes::x, "0"}, // Start point (in WCS); DXF: X value; APP: 3D point
			{_group_codes::y, "0"}, // DXF: Y and Z values of start point (in WCS)
			{_group_codes::z, "0"}, // DXF: Y and Z values of start point (in WCS)
			{_group_codes::x2, "0"}, // Endpoint (in WCS); DXF: X value; APP: 3D point
			{_group_codes::y2, "0"}, // DXF: Y and Z values of endpoint (in WCS)
			{_group_codes::z2, "0"}, // DXF: Y and Z values of endpoint (in WCS)
		};

		m_mapCode2Value.insert(mapCode2Value.begin(), mapCode2Value.end());
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
			atof(m_mapCode2Value[_group_codes::x].c_str()),
			atof(m_mapCode2Value[_group_codes::y].c_str()),
			atof(m_mapCode2Value[_group_codes::z].c_str()),
			atof(m_mapCode2Value[_group_codes::x2].c_str()),
			atof(m_mapCode2Value[_group_codes::y2].c_str()),
			atof(m_mapCode2Value[_group_codes::z2].c_str()),
		};

		int64_t iInstance = CreateInstance(iClass, "LINE");
		assert(iInstance != 0);

		SetDataTypeProperty(iInstance, GetPropertyByName(iModel, "points"), vecVertices.data(), vecVertices.size());

		return iInstance;
	}
	// _line
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _vertex
	/*static*/ const string _vertex::polyface_mesh_vertex1 = "71";
	/*static*/ const string _vertex::polyface_mesh_vertex2 = "72";
	/*static*/ const string _vertex::polyface_mesh_vertex3 = "73";
	/*static*/ const string _vertex::polyface_mesh_vertex4 = "74";

	// --------------------------------------------------------------------------------------------
	_vertex::_vertex()
		: _entity(_group_codes::vertex)
	{
		// VERTEX, page 149
		map<string, string> mapCode2Value =
		{
			{_group_codes::x, "0"}, // Location point (in OCS when 2D, and WCS when 3D); DXF: X value; APP: 3D point
			{_group_codes::y, "0"}, // DXF: Y and Z values of location point (in OCS when 2D, and WCS when 3D)
			{_group_codes::z, "0"}, // DXF: Y and Z values of location point (in OCS when 2D, and WCS when 3D)
			{polyface_mesh_vertex1, "0"}, // Polyface mesh vertex index (optional; present only if nonzero)
			{polyface_mesh_vertex2, "0"}, // Polyface mesh vertex index (optional; present only if nonzero)
			{polyface_mesh_vertex3, "0"}, // Polyface mesh vertex index (optional; present only if nonzero)
			{polyface_mesh_vertex4, "0"}, // Polyface mesh vertex index (optional; present only if nonzero)
		};

		m_mapCode2Value.insert(mapCode2Value.begin(), mapCode2Value.end());
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _vertex::~_vertex()
	{
	}

	// ----------------------------------------------------------------------------------------
	/*virtual*/ int64_t _vertex::createInstance(int64_t /*iModel*/)
	{
		assert(false); // Not implemented!

		return 0;
	}
	// _vertex
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _circle
	_circle::_circle()
		: _entity(_group_codes::circle)
	{
		// CIRCLE, page 77
		map<string, string> mapCode2Value =
		{
			{_group_codes::x, "0"}, // Center point (in OCS) DXF: X value; APP: 3D point
			{_group_codes::y, "0"}, // DXF: Y and Z values of center point (in OCS)
			{_group_codes::z, "0"}, // DXF: Y and Z values of center point (in OCS)
			{_group_codes::radius, "0"}, // Radius
			{_group_codes::extrusion_x, "0"}, // Extrusion direction (optional; default = 0, 0, 1) DXF: X value; APP: 3D vector
			{_group_codes::extrusion_y, "0"}, // DXF: Y and Z values of extrusion direction (optional)
			{_group_codes::extrusion_z, "1"}, // DXF: Y and Z values of extrusion direction (optional)
		};

		m_mapCode2Value.insert(mapCode2Value.begin(), mapCode2Value.end());
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _circle::~_circle()
	{
	}

	// ----------------------------------------------------------------------------------------
	/*virtual*/ int64_t _circle::createInstance(int64_t iModel)
	{
		int64_t iCircleClass = GetClassByName(iModel, "Circle");
		assert(iCircleClass != 0);		

		int64_t iExtrusionAreaSolidClass = GetClassByName(iModel, "ExtrusionAreaSolid");
		assert(iExtrusionAreaSolidClass != 0);

		int64_t iTransformationClass = GetClassByName(iModel, "Transformation");
		assert(iTransformationClass != 0);

		int64_t iMatrixClass = GetClassByName(iModel, "Matrix");
		assert(iMatrixClass != 0);

		// Circle
		int64_t iCircleInstance = CreateInstance(iCircleClass, "CIRCLE");
		assert(iCircleInstance != 0);		

		double dValue = atof(m_mapCode2Value[_group_codes::radius].c_str());
		SetDataTypeProperty(iCircleInstance, GetPropertyByName(iModel, "a"), &dValue, 1);		

		// ExtrusionAreaSolid
		int64_t iExtrusionAreaSolidInstance = CreateInstance(iExtrusionAreaSolidClass, "CIRCLE");
		assert(iExtrusionAreaSolidInstance != 0);

		SetObjectProperty(iExtrusionAreaSolidInstance, GetPropertyByName(iModel, "extrusionArea"), &iCircleInstance, 1);		

		double dExtrusionX = atof(m_mapCode2Value[_group_codes::extrusion_x].c_str());
		double dExtrusionY = atof(m_mapCode2Value[_group_codes::extrusion_y].c_str());
		double dExtrusionZ = atof(m_mapCode2Value[_group_codes::extrusion_z].c_str());

		dValue = vector3_normalize(dExtrusionX, dExtrusionY, dExtrusionZ);

		vector<double> vecValues = { dExtrusionX, dExtrusionY, dExtrusionZ };
		SetDataTypeProperty(iExtrusionAreaSolidInstance, GetPropertyByName(iModel, "extrusionDirection"), vecValues.data(), vecValues.size());		
		
		SetDataTypeProperty(iExtrusionAreaSolidInstance, GetPropertyByName(iModel, "extrusionLength"), &dValue, 1);

		// Matrix
		int64_t iMatrixInstance = CreateInstance(iMatrixClass, "CIRCLE");
		assert(iMatrixInstance != 0);

		dValue = atof(m_mapCode2Value[_group_codes::x].c_str());
		SetDataTypeProperty(iMatrixInstance, GetPropertyByName(iModel, "_41"), &dValue, 1);

		dValue = atof(m_mapCode2Value[_group_codes::y].c_str());
		SetDataTypeProperty(iMatrixInstance, GetPropertyByName(iModel, "_42"), &dValue, 1);

		dValue = atof(m_mapCode2Value[_group_codes::z].c_str());
		SetDataTypeProperty(iMatrixInstance, GetPropertyByName(iModel, "_43"), &dValue, 1);

		// Transformation
		int64_t iTransformationInstance = CreateInstance(iTransformationClass, "CIRCLE");
		assert(iTransformationInstance != 0);

		SetObjectProperty(iTransformationInstance, GetPropertyByName(iModel, "matrix"), &iMatrixInstance, 1);

		SetObjectProperty(iTransformationInstance, GetPropertyByName(iModel, "object"), &iExtrusionAreaSolidInstance, 1);

		return iTransformationInstance;
	}
	// _circle
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _seqend
	_seqend::_seqend()
		: _entity(_group_codes::seqend)
	{
		// SEQEND, page 128
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _seqend::~_seqend()
	{
	}

	// ----------------------------------------------------------------------------------------
	/*virtual*/ int64_t _seqend::createInstance(int64_t /*iModel*/)
	{
		assert(false); // Not implemented!

		return 0;
	}
	// _seqend
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _polyline
	/*static*/ const string _polyline::flag = "70";
	/*static*/ const string _polyline::m = "71";
	/*static*/ const string _polyline::n = "72";

	// --------------------------------------------------------------------------------------------
	_polyline::_polyline()
		: _entity(_group_codes::polyline)
		, m_vecVertices()
		, m_pSeqend(nullptr)
	{
		// POLYLINE, page 123
		map<string, string> mapCode2Value =
		{
			{_group_codes::x, "0"}, // DXF: always 0; APP: a “dummy” point; the X and Y values are always 0, and the Z value is the polyline's elevation(in OCS when 2D, WCS when 3D)
			{_group_codes::y, "0"}, // DXF: always 0; APP: a “dummy” point; the X and Y values are always 0, and the Z value is the polyline's elevation(in OCS when 2D, WCS when 3D)
			{_group_codes::z, "0"}, // DXF: polyline's elevation (in OCS when 2D; WCS when 3D)
			{flag, "1"}, // Polyline flag (bit-coded; default = 0):
			/*
			1 = This is a closed polyline (or a polygon mesh closed in the M direction)
			2 = Curve-fit vertices have been added
			4 = Spline-fit vertices have been added
			8 = This is a 3D polyline
			16 = This is a 3D polygon mesh
			32 = The polygon mesh is closed in the N direction
			64 = The polyline is a polyface mesh
			128 = The linetype pattern is generated continuously around the vertices of this polyline
			*/
			{m, "0"}, // Polygon mesh M vertex count (optional; default = 0)
			{n, "0"}, // Polygon mesh N vertex count (optional; default = 0)
		};

		m_mapCode2Value.insert(mapCode2Value.begin(), mapCode2Value.end());
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _polyline::~_polyline()
	{
		for (size_t i = 0; i < m_vecVertices.size(); i++)
		{
			delete m_vecVertices[i];
		}

		delete m_pSeqend;
	}

	// ----------------------------------------------------------------------------------------
	/*virtual*/ void _polyline::load(_reader& reader)
	{
		while (true)
		{
			_entity::load(reader);

			if (reader.row() == _group_codes::start)
			{
				reader.forth();
				if (reader.row() == _group_codes::seqend)
				{
					reader.forth();

					assert(m_pSeqend == nullptr);

					m_pSeqend = new _seqend();
					m_pSeqend->load(reader);
				}
				else if (reader.row() == _group_codes::vertex)
				{
					reader.forth();

					auto pVertex = new _vertex();
					m_vecVertices.push_back(pVertex);

					pVertex->load(reader);
				} // if (reader.row() == _group_codes::line)
				else
				{	
					reader.back();

					break;
				}
			} // if (reader.row() == _group_codes::start)
			else
			{
				reader.forth();
			}
		} // while (true)
	}

	// ----------------------------------------------------------------------------------------
	/*virtual*/ int64_t _polyline::createInstance(int64_t iModel)
	{
		int iFlag = atoi(m_mapCode2Value[flag].c_str());
		switch (iFlag)
		{
			case 1:
			{
				vector<double> vecVertices;
				for (auto itVertex : m_vecVertices)
				{
					vecVertices.push_back(atof(itVertex->value(_group_codes::x).c_str()));
					vecVertices.push_back(atof(itVertex->value(_group_codes::y).c_str()));
					vecVertices.push_back(atof(itVertex->value(_group_codes::z).c_str()));
				}

				int64_t iClass = GetClassByName(iModel, "PolyLine3D");
				assert(iClass != 0);

				int64_t iInstance = CreateInstance(iClass, "POLYLINE");
				assert(iInstance != 0);

				SetDataTypeProperty(iInstance, GetPropertyByName(iModel, "points"), vecVertices.data(), vecVertices.size());

				return iInstance;
			} // case 1:

			case 64:
			{	
				int64_t iTriangleSetClass = GetClassByName(iModel, "TriangleSet");
				assert(iTriangleSetClass != 0);

				vector<double> vecVertices;
				vector<int64_t> vecIndices;
				for (auto itVertex : m_vecVertices)
				{
					if (itVertex->value(_group_codes::subclass) == "AcDbFaceRecord")
					{
						vecIndices.push_back(abs(atoi(itVertex->value(_vertex::polyface_mesh_vertex1).c_str())) - 1);
						vecIndices.push_back(abs(atoi(itVertex->value(_vertex::polyface_mesh_vertex2).c_str())) - 1);
						vecIndices.push_back(abs(atoi(itVertex->value(_vertex::polyface_mesh_vertex3).c_str())) - 1);

						continue;
					}

					vecVertices.push_back(atof(itVertex->value(_group_codes::x).c_str()));
					vecVertices.push_back(atof(itVertex->value(_group_codes::y).c_str()));
					vecVertices.push_back(atof(itVertex->value(_group_codes::z).c_str()));
				}

				int64_t iTriangleSetInstance = CreateInstance(iTriangleSetClass, "Triangles");
				assert(iTriangleSetInstance != 0);

				SetDataTypeProperty(iTriangleSetInstance, GetPropertyByName(iModel, "vertices"), vecVertices.data(), vecVertices.size());
				SetDataTypeProperty(iTriangleSetInstance, GetPropertyByName(iModel, "indices"), vecIndices.data(), vecIndices.size());

				return iTriangleSetInstance;
			} // case 64:

			default:
			{
				assert(false);
			}
			break;
		} // switch (iFlag)

		return 0;
	}
	// _polyline
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
						else if (reader.row() == _group_codes::header)
						{
							reader.forth();

							auto pHeaderSection = new _header_section();
							m_vecSections.push_back(pHeaderSection);

							pHeaderSection->load(reader);
						}
						else if (reader.row() == _group_codes::tables)
						{
							reader.forth();

							auto pTablesSection = new _tables_section();
							m_vecSections.push_back(pTablesSection);

							pTablesSection->load(reader);
						}
						else if (reader.row() == _group_codes::blocks)
						{
							reader.forth();

							auto pBlocksSection = new _blocks_section();
							m_vecSections.push_back(pBlocksSection);

							pBlocksSection->load(reader);
						}
						else
						{
							// TODO: CLASSES, OBJECTS, THUMBNAILIMAGE
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
					if (iInstance != 0)
					{
						auto strLayer = pEntity->value(_group_codes::layer);

						auto itLayer2Instances = mapLayer2Instances.find(strLayer);
						if (itLayer2Instances != mapLayer2Instances.end())
						{
							mapLayer2Instances[strLayer].push_back(iInstance);
						}
						else
						{
							mapLayer2Instances[strLayer] = vector<int64_t>{ iInstance };
						}
					} // if (iInstance != 0)
				} // for (size_t iEntity = ...
			} // if (m_vecSections[iSection]->name() == _group_codes::entities)
		} // for (size_t iSection = ...

		/**
		* Create ifcengine collections
		*/
		int64_t iCollectionClass = GetClassByName(m_iModel, "Collection");
		assert(iCollectionClass != 0);
		
		for (auto itLayer2Instances : mapLayer2Instances)
		{
			string strCollectionName = "Layer: '";
			strCollectionName += itLayer2Instances.first;
			strCollectionName += "'";

			int64_t iCollectionInstance = CreateInstance(iCollectionClass, strCollectionName.c_str());
			SetObjectProperty(iCollectionInstance, GetPropertyByName(m_iModel, "objects"), itLayer2Instances.second.data(), itLayer2Instances.second.size());
		}
	}
	// _parser
	// --------------------------------------------------------------------------------------------
}