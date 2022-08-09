#include "stdafx.h"
#include "_dxp_parser.h"
#include "conceptMesh.h"

// ------------------------------------------------------------------------------------------------
static char VALUE_DELIMITER = '\n';

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
static inline double _vector3_normalize(double& dX, double& dY, double& dZ)
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

// ------------------------------------------------------------------------------------------------
static inline void _tokenize(string const& str, const char delim, vector<std::string>& out)
{
	size_t start;
	size_t end = 0;

	while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
	{
		end = str.find(delim, start);
		out.push_back(str.substr(start, end - start));
	}
}

namespace _dxf
{
	// --------------------------------------------------------------------------------------------
	// _error	
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
	// _reader
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
	// _group_codes
	/*static*/ const string _group_codes::start = "0";
	/*static*/ const string _group_codes::name = "2";
	/*static*/ const string _group_codes::eof = "EOF";
	/*static*/ const string _group_codes::section = "SECTION";
	/*static*/ const string _group_codes::endsec = "ENDSEC";

	// --------------------------------------------------------------------------------------------
	/*static*/ const string _group_codes::entities = "ENTITIES";
	/*static*/ const string _group_codes::header = "HEADER";
	/*static*/ const string _group_codes::classes = "CLASSES";
	/*static*/ const string _group_codes::objects = "OBJECTS";
	/*static*/ const string _group_codes::tables = "TABLES";
	/*static*/ const string _group_codes::blocks = "BLOCKS";
	/*static*/ const string _group_codes::line = "LINE";	
	/*static*/ const string _group_codes::arc = "ARC";
	/*static*/ const string _group_codes::text = "TEXT";
	/*static*/ const string _group_codes::mtext = "MTEXT";
	/*static*/ const string _group_codes::viewport = "VIEWPORT";
	/*static*/ const string _group_codes::vertex = "VERTEX";
	/*static*/ const string _group_codes::polyline = "POLYLINE";
	/*static*/ const string _group_codes::lwpolyline = "LWPOLYLINE";
	/*static*/ const string _group_codes::seqend = "SEQEND";
	/*static*/ const string _group_codes::circle = "CIRCLE";	
	/*static*/ const string _group_codes::block = "BLOCK";
	/*static*/ const string _group_codes::endblock = "ENDBLK";
	/*static*/ const string _group_codes::insert = "INSERT";

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
	// _group
	_group::_group(const string& strType)
		: m_strType(strType)
	{
		assert(!m_strType.empty());
	}
	
	// --------------------------------------------------------------------------------------------
	/*virtual*/ _group::~_group()
	{
	}

	// --------------------------------------------------------------------------------------------
	const string& _group::type() const
	{
		return m_strType;
	}
	// _group
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _entity
	_entity::_entity(const string& strName)
		: _group(strName)
		, m_pParent(nullptr)
		, m_mapCode2Value()
		, m_setMultiValueCodes()
		, m_vecEntities()
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
		for (size_t i = 0; i < m_vecEntities.size(); i++)
		{
			delete m_vecEntities[i];
		}
	}

	// --------------------------------------------------------------------------------------------
	void _entity::setParent(_entity* pParent)
	{
		assert(pParent != nullptr);

		m_pParent = pParent;
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
				auto strValue = reader.forth();

				auto itMultiValueCode = m_setMultiValueCodes.find(itCode2Value->first);
				if (itMultiValueCode != m_setMultiValueCodes.end())
				{
					if (!itCode2Value->second.empty())
					{
						itCode2Value->second += VALUE_DELIMITER;
					}

					itCode2Value->second += strValue;
				}
				else
				{
					itCode2Value->second = strValue;
				}				
			}
			else
			{
				reader.forth();
			}

			reader.forth();
		} // while (true)
	}

	// --------------------------------------------------------------------------------------------
	bool _entity::hasValue(const string& strCode)
	{
		return m_mapCode2Value.find(strCode) != m_mapCode2Value.end();
	}

	// --------------------------------------------------------------------------------------------
	const string& _entity::getValue(const string& strCode)
	{
		return m_mapCode2Value[strCode];
	}

	// --------------------------------------------------------------------------------------------
	void _entity::setValue(const string& strCode, const string& strValue)
	{
		m_mapCode2Value[strCode] = strValue;
	}
	// _entity
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _extrusion
	_extrusion::_extrusion(_entity* pEntity)
		: m_pEntity(pEntity)
		, m_mapMapping()
		, m_dXFactor(1.)
		, m_dYFactor(1.)
		, m_dZFactor(1.)
	{
		assert(m_pEntity != nullptr);

		m_mapMapping =
		{
			{_group_codes::x, _group_codes::x},
			{_group_codes::y, _group_codes::y},
			{_group_codes::z, _group_codes::z},
			{_group_codes::x2, _group_codes::x2},
			{_group_codes::y2, _group_codes::y2},
			{_group_codes::z2, _group_codes::z2},
		};

		initialize();
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _extrusion::~_extrusion()
	{
	}

	// --------------------------------------------------------------------------------------------
	double _extrusion::getValue(const string& strCode)
	{
		return getValue(m_pEntity,strCode);
	}

	// ----------------------------------------------------------------------------------------
	double _extrusion::getValue(_entity* pEntity, const string& strCode)
	{
		string strMapping = m_mapMapping[strCode];
		string strValue = pEntity->getValue(strMapping);

		double dValue = atof(strValue.c_str());
		if ((strCode == _group_codes::x) || (strCode == _group_codes::x2))
		{
			dValue *= m_dXFactor;
		}
		else if ((strCode == _group_codes::y) || (strCode == _group_codes::y2))
		{
			dValue *= m_dYFactor;
		}
		else if ((strCode == _group_codes::z) || (strCode == _group_codes::z2))
		{
			dValue *= m_dZFactor;
		}
		else
		{
			assert(false); // Internal error!
		}

		return dValue;
	}

	// --------------------------------------------------------------------------------------------
	void _extrusion::initialize()
	{
		if (!m_pEntity->hasValue(_group_codes::extrusion_x) ||
			!m_pEntity->hasValue(_group_codes::extrusion_y) ||
			!m_pEntity->hasValue(_group_codes::extrusion_z))
		{
			return;
		}

		// Arbitrary Axis Algorithm, page 252
		double Nx = atof(m_pEntity->getValue(_group_codes::extrusion_x).c_str());
		double Ny = atof(m_pEntity->getValue(_group_codes::extrusion_y).c_str());
		double Nz = atof(m_pEntity->getValue(_group_codes::extrusion_z).c_str());
				
		_vector3f Wy(0., 1., 0.);
		_vector3f Wz(0., 0., 1.);

		_vector3f N(Nx, Ny, Nz);

		_vector3f Ax;
		if ((abs(Nx) < (1. / 64.)) && (abs(Ny) < (1. / 64.)))
		{
			Ax = Wy.cross(N);
		}
		else
		{
			Ax = Wz.cross(N);
		}

		_vector3f Ay = N.cross(Ax);

		if (Nz != 0.)
		{
			// X
			m_mapMapping[_group_codes::x] = _group_codes::x;
			m_dXFactor = Ax.getX();

			// Y
			m_mapMapping[_group_codes::y] = _group_codes::y;
			m_dYFactor = Ay.getY();

			// Z			
			m_mapMapping[_group_codes::z] = _group_codes::z;
			m_dZFactor = Nz;				
		} // if (Nz != 0.)
		else if (Nx != 0.)
		{
			// X
			m_mapMapping[_group_codes::x] = _group_codes::z;
			m_mapMapping[_group_codes::x2] = _group_codes::z2;
			m_dXFactor = Nx;

			// Y
			m_mapMapping[_group_codes::y] = _group_codes::x;
			m_mapMapping[_group_codes::y2] = _group_codes::x2;
			m_dYFactor = Ax.getY();

			// Z
			m_mapMapping[_group_codes::z] = _group_codes::y;
			m_mapMapping[_group_codes::z2] = _group_codes::y2;
			m_dZFactor = Ay.getZ();
		}
		else if (Ny != 0.)
		{
			assert(false); // TODO
		}

		assert(m_mapMapping[_group_codes::x] != m_mapMapping[_group_codes::y]);
		assert(m_mapMapping[_group_codes::x] != m_mapMapping[_group_codes::z]);
		assert(m_mapMapping[_group_codes::y] != m_mapMapping[_group_codes::z]);
	}
	// _extrusion
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
	/*virtual*/ int64_t _line::createInstance(_parser* pParser)
	{
		int64_t iClass = GetClassByName(pParser->getModel(), "Line3D");
		assert(iClass != 0);

		// Extrusion
		_extrusion extrusion(this);

		vector<double> vecPoints
		{
			extrusion.getValue(_group_codes::x),
			extrusion.getValue(_group_codes::y),
			extrusion.getValue(_group_codes::z),

			extrusion.getValue(_group_codes::x2),
			extrusion.getValue(_group_codes::y2),
			extrusion.getValue(_group_codes::z2),
		};

		int64_t iInstance = CreateInstance(iClass, type().c_str());
		assert(iInstance != 0);

		SetDataTypeProperty(iInstance, GetPropertyByName(pParser->getModel(), "points"), vecPoints.data(), vecPoints.size());
		
		if (m_pParent == nullptr)
		{
			pParser->onInstanceCreated(this, iInstance);
		}		

		return iInstance;
	}
	// _line
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _arc
	_arc::_arc()
		: _entity(_group_codes::arc)
	{
		// ARC, page 66
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
	/*virtual*/ _arc::~_arc()
	{
	}

	// ----------------------------------------------------------------------------------------
	/*virtual*/ int64_t _arc::createInstance(_parser* /*pParser*/)
	{
		// TODO

		return 0;
	}
	// _arc
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _text
	_text::_text()
		: _entity(_group_codes::text)
	{
		// TEXT, page 144
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _text::~_text()
	{
	}

	// ----------------------------------------------------------------------------------------
	/*virtual*/ int64_t _text::createInstance(_parser* /*pParser*/)
	{
		// TODO

		return 0;
	}
	// _text
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _mtext
	_mtext::_mtext()
		: _entity(_group_codes::mtext)
	{
		// MTEXT, page 117
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _mtext::~_mtext()
	{
	}

	// ----------------------------------------------------------------------------------------
	/*virtual*/ int64_t _mtext::createInstance(_parser* /*pParser*/)
	{
		// TODO

		return 0;
	}
	// _mtext
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _viewport
	_viewport::_viewport()
		: _entity(_group_codes::viewport)
	{
		// VIEWPORT, page 150
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _viewport::~_viewport()
	{
	}

	// ----------------------------------------------------------------------------------------
	/*virtual*/ int64_t _viewport::createInstance(_parser* /*pParser*/)
	{
		// TODO

		return 0;
	}
	// _viewport
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _lwpolyline
	_lwpolyline::_lwpolyline()
		: _entity(_group_codes::lwpolyline)
	{
		// LINE, page 101
		map<string, string> mapCode2Value =
		{
			{_group_codes::x, ""}, // Vertex coordinates (in OCS), multiple entries; one entry for each vertex; DXF: X value; APP: 2D point
			{_group_codes::y, ""}, // DXF: Y value of vertex coordinates (in OCS), multiple entries; one entry for each vertex
			{_group_codes::extrusion_x, "0"}, // Extrusion direction (optional; default = 0, 0, 1) DXF: X value; APP: 3D vector
			{_group_codes::extrusion_y, "0"}, // DXF: Y and Z values of extrusion direction (optional)
			{_group_codes::extrusion_z, "1"}, // DXF: Y and Z values of extrusion direction (optional)			
			{_polyline::flag, "0"}, // Polyline flag (bit-coded; default = 0): 1 = Closed; 128 = Plinegen
		};

		m_mapCode2Value.insert(mapCode2Value.begin(), mapCode2Value.end());

		set<string> setMultiValueCodes =
		{
			_group_codes::x,
			_group_codes::y,
		};

		m_setMultiValueCodes.insert(setMultiValueCodes.begin(), setMultiValueCodes.end());
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _lwpolyline::~_lwpolyline()
	{
	}

	// ----------------------------------------------------------------------------------------
	/*virtual*/ int64_t _lwpolyline::createInstance(_parser* pParser)
	{
		vector<string> vecXValues;
		_tokenize(m_mapCode2Value[_group_codes::x], VALUE_DELIMITER, vecXValues);

		vector<string> vecYValues;
		_tokenize(m_mapCode2Value[_group_codes::y], VALUE_DELIMITER, vecYValues);

		if (vecXValues.empty() || vecYValues.empty())
		{
			assert(false);

			return 0;
		}

		assert(vecXValues.size() == vecYValues.size());

		vector<double> vecPoints;
		for (size_t i = 0; i < vecXValues.size(); i++)
		{
			vecPoints.push_back(atof(vecXValues[i].c_str()));
			vecPoints.push_back(atof(vecYValues[i].c_str()));
			vecPoints.push_back(0.);
		}

		int iFlag = atoi(m_mapCode2Value[_polyline::flag].c_str());
		if (iFlag == 1)
		{
			vecPoints.push_back(atof(vecXValues[0].c_str()));
			vecPoints.push_back(atof(vecYValues[0].c_str()));
			vecPoints.push_back(0.);
		}
		else if (iFlag == 128)
		{
			assert(false); // TODO
		}		

		int64_t iClass = GetClassByName(pParser->getModel(), "PolyLine3D");
		assert(iClass != 0);

		int64_t iInstance = CreateInstance(iClass, type().c_str());
		assert(iInstance != 0);

		SetDataTypeProperty(iInstance, GetPropertyByName(pParser->getModel(), "points"), vecPoints.data(), vecPoints.size());

		if (m_pParent == nullptr)
		{
			pParser->onInstanceCreated(this, iInstance);
		}		

		return iInstance;
	}
	// _lwpolyline
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _vertex
	/*static*/ const string _vertex::flag = "70";
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
			{flag, "0"}, // Vertex flags:
			/*1 = Extra vertex created by curve - fitting
			2 = Curve - fit tangent defined for this vertex.A curve - fit tangent direction of 0 may be omitted
			from DXF output but is significant if this bit is set
			4 = Not used
			8 = Spline vertex created by spline - fitting
			16 = Spline frame control point
			32 = 3D polyline vertex
			64 = 3D polygon mesh
			128 = Polyface mesh vertex
			*/
			{polyface_mesh_vertex1, "0"}, // Polyface mesh vertex index (optional; present only if nonzero)
			{polyface_mesh_vertex2, "0"}, // Polyface mesh vertex index (optional; present only if nonzero)
			{polyface_mesh_vertex3, "0"}, // Polyface mesh vertex index (optional; present only if nonzero)
			{polyface_mesh_vertex4, ""}, // Polyface mesh vertex index (optional; present only if nonzero)
		};

		m_mapCode2Value.insert(mapCode2Value.begin(), mapCode2Value.end());
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _vertex::~_vertex()
	{
	}

	// ----------------------------------------------------------------------------------------
	/*virtual*/ int64_t _vertex::createInstance(_parser* /*pParser*/)
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
	/*virtual*/ int64_t _circle::createInstance(_parser* pParser)
	{
		int64_t iCircleClass = GetClassByName(pParser->getModel(), "Circle");
		assert(iCircleClass != 0);

		int64_t iTransformationClass = GetClassByName(pParser->getModel(), "Transformation");
		assert(iTransformationClass != 0);

		int64_t iMatrixClass = GetClassByName(pParser->getModel(), "Matrix");
		assert(iMatrixClass != 0);

		// Circle
		int64_t iCircleInstance = CreateInstance(iCircleClass, type().c_str());
		assert(iCircleInstance != 0);		

		double dValue = atof(m_mapCode2Value[_group_codes::radius].c_str());
		SetDataTypeProperty(iCircleInstance, GetPropertyByName(pParser->getModel(), "a"), &dValue, 1);

		// Extrusion
		_extrusion extrusion(this);		

		// Matrix
		int64_t iMatrixInstance = CreateInstance(iMatrixClass, type().c_str());
		assert(iMatrixInstance != 0);

		dValue = extrusion.getValue(_group_codes::x);
		SetDataTypeProperty(iMatrixInstance, GetPropertyByName(pParser->getModel(), "_41"), &dValue, 1);

		dValue = extrusion.getValue(_group_codes::y);
		SetDataTypeProperty(iMatrixInstance, GetPropertyByName(pParser->getModel(), "_42"), &dValue, 1);

		dValue = extrusion.getValue(_group_codes::z);
		SetDataTypeProperty(iMatrixInstance, GetPropertyByName(pParser->getModel(), "_43"), &dValue, 1);

		// Transformation
		int64_t iTransformationInstance = CreateInstance(iTransformationClass, type().c_str());
		assert(iTransformationInstance != 0);

		SetObjectProperty(iTransformationInstance, GetPropertyByName(pParser->getModel(), "matrix"), &iMatrixInstance, 1);
		SetObjectProperty(iTransformationInstance, GetPropertyByName(pParser->getModel(), "object"), &iCircleInstance, 1);

		if (m_pParent == nullptr)
		{
			pParser->onInstanceCreated(this, iTransformationInstance);
		}		

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
	/*virtual*/ int64_t _seqend::createInstance(_parser* /*pParser*/)
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
		delete m_pSeqend;
	}

	// ----------------------------------------------------------------------------------------
	/*virtual*/ void _polyline::load(_reader& reader)
	{
		_entity* pEntity = nullptr;

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

					break;
				}
				else if ((pEntity = _parser::loadEntity(reader)) != nullptr)
				{
					m_vecEntities.push_back(pEntity);
				}				
			} // if (reader.row() == _group_codes::start)
			else
			{
				reader.forth();
			}
		} // while (true)
	}

	// ----------------------------------------------------------------------------------------
	/*virtual*/ int64_t _polyline::createInstance(_parser* pParser)
	{
		// Vertices
		vector<_entity*> vecVertices;
		for (auto itEntity : m_vecEntities)
		{
			if (itEntity->type() == _group_codes::vertex)
			{
				vecVertices.push_back(itEntity);
			}
		}

		if (vecVertices.empty())
		{
			assert(false);

			return 0;
		}

		// Extrusion
		_extrusion extrusion(this);

		int iFlag = atoi(m_mapCode2Value[flag].c_str());
		switch (iFlag)
		{
			case 1:
			{
				vector<double> vecPolyLine3DPoints;
				for (auto itVertex : vecVertices)
				{
					vecPolyLine3DPoints.push_back(atof(itVertex->getValue(_group_codes::x).c_str()));
					vecPolyLine3DPoints.push_back(atof(itVertex->getValue(_group_codes::y).c_str()));
					vecPolyLine3DPoints.push_back(atof(itVertex->getValue(_group_codes::z).c_str()));
				}

				vecPolyLine3DPoints.push_back(atof(vecVertices[0]->getValue(_group_codes::x).c_str()));
				vecPolyLine3DPoints.push_back(atof(vecVertices[0]->getValue(_group_codes::y).c_str()));
				vecPolyLine3DPoints.push_back(atof(vecVertices[0]->getValue(_group_codes::z).c_str()));

				int64_t iPolyLine3DClass = GetClassByName(pParser->getModel(), "PolyLine3D");
				assert(iPolyLine3DClass != 0);

				int64_t iPolyLine3DInstance = CreateInstance(iPolyLine3DClass, type().c_str());
				assert(iPolyLine3DInstance != 0);

				SetDataTypeProperty(iPolyLine3DInstance, GetPropertyByName(pParser->getModel(), "points"), vecPolyLine3DPoints.data(), vecPolyLine3DPoints.size());

				if (m_pParent == nullptr)
				{
					pParser->onInstanceCreated(this, iPolyLine3DInstance);
				}				

				return iPolyLine3DInstance;
			} // case 1:

			case 64:
			{
				vector<double> vecTriangleSetVertices;
				vector<int64_t> vecIndices;
				for (auto itVertex : vecVertices)
				{
					if (itVertex->getValue(_vertex::flag) == "128")
					{
						vecIndices.push_back(abs(atoi(itVertex->getValue(_vertex::polyface_mesh_vertex1).c_str())) - 1/*to 0-based index*/);
						vecIndices.push_back(abs(atoi(itVertex->getValue(_vertex::polyface_mesh_vertex2).c_str())) - 1/*to 0-based index*/);
						vecIndices.push_back(abs(atoi(itVertex->getValue(_vertex::polyface_mesh_vertex3).c_str())) - 1/*to 0-based index*/);

						if (itVertex->getValue(_vertex::polyface_mesh_vertex4) != "")						
						{	
							vecIndices.push_back(abs(atoi(itVertex->getValue(_vertex::polyface_mesh_vertex3).c_str())) - 1/*to 0-based index*/);
							vecIndices.push_back(abs(atoi(itVertex->getValue(_vertex::polyface_mesh_vertex4).c_str())) - 1/*to 0-based index*/);
							vecIndices.push_back(abs(atoi(itVertex->getValue(_vertex::polyface_mesh_vertex1).c_str())) - 1/*to 0-based index*/);
						}						

						continue;
					}

					vecTriangleSetVertices.push_back(extrusion.getValue(itVertex, _group_codes::x));
					vecTriangleSetVertices.push_back(extrusion.getValue(itVertex, _group_codes::y));
					vecTriangleSetVertices.push_back(extrusion.getValue(itVertex, _group_codes::z));
				}				

				if (vecTriangleSetVertices.empty() || vecIndices.empty())
				{
					assert(false); // Internal error!

					return 0;
				}

				int64_t iTriangleSetClass = GetClassByName(pParser->getModel(), "TriangleSet");
				assert(iTriangleSetClass != 0);

				int64_t iTriangleSetInstance = CreateInstance(iTriangleSetClass, type().c_str());
				assert(iTriangleSetInstance != 0);

				SetDataTypeProperty(iTriangleSetInstance, GetPropertyByName(pParser->getModel(), "vertices"), vecTriangleSetVertices.data(), vecTriangleSetVertices.size());
				SetDataTypeProperty(iTriangleSetInstance, GetPropertyByName(pParser->getModel(), "indices"), vecIndices.data(), vecIndices.size());

				if (m_pParent == nullptr)
				{
					pParser->onInstanceCreated(this, iTriangleSetInstance);
				}				

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
	// _endblk
	_endblk::_endblk()
		: _entity(_group_codes::endblock)
	{
		// ENDBLK, page 59
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _endblk::~_endblk()
	{
	}

	// ----------------------------------------------------------------------------------------
	/*virtual*/ int64_t _endblk::createInstance(_parser* /*pParser*/)
	{
		assert(false); // Not implemented!

		return 0;
	}
	// _endblk
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _block
	_block::_block()
		: _entity(_group_codes::block)
		, m_iInstance(0)
		, m_pEndblk(nullptr)
	{
		// BLOCK, page 58
		map<string, string> mapCode2Value =
		{
			{_group_codes::name, ""}, // Block name
			{_group_codes::extrusion_x, "0"}, // Extrusion direction (optional; default = 0, 0, 1) DXF: X value; APP: 3D vector
			{_group_codes::extrusion_y, "0"}, // DXF: Y and Z values of extrusion direction (optional)
			{_group_codes::extrusion_z, "1"}, // DXF: Y and Z values of extrusion direction (optional)
		};

		m_mapCode2Value.insert(mapCode2Value.begin(), mapCode2Value.end());
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _block::~_block()
	{
		delete m_pEndblk;
	}

	// ----------------------------------------------------------------------------------------
	/*virtual*/ void _block::load(_reader& reader)
	{
		_entity* pEntity = nullptr;

		while (true)
		{
			_entity::load(reader);

			if (reader.row() == _group_codes::start)
			{
				reader.forth();
				if (reader.row() == _group_codes::endblock)
				{
					reader.forth();

					assert(m_pEndblk == nullptr);

					m_pEndblk = new _endblk();
					m_pEndblk->load(reader);

					break;
				}
				else if ((pEntity = _parser::loadEntity(reader)) != nullptr)
				{	
					m_vecEntities.push_back(pEntity);
				}
			} // if (reader.row() == _group_codes::start)
			else
			{
				reader.forth();
			}
		} // while (true)
	}

	// ----------------------------------------------------------------------------------------
	/*virtual*/ int64_t _block::createInstance(_parser* pParser)
	{
		/**
		* Reuse the Instance
		*/
		if (m_iInstance != 0)
		{
			return m_iInstance;
		}

		/**
		* Create ifcengine instances
		*/
		map<string, vector<int64_t>> mapLayer2Instances;
		for (auto itEntity : m_vecEntities)
		{
			itEntity->setParent(this);

			itEntity->setValue(_group_codes::extrusion_x, getValue(_group_codes::extrusion_x));
			itEntity->setValue(_group_codes::extrusion_y, getValue(_group_codes::extrusion_y));
			itEntity->setValue(_group_codes::extrusion_z, getValue(_group_codes::extrusion_z));

			auto iInstance = itEntity->createInstance(pParser);
			if (iInstance != 0)
			{
				auto strLayer = itEntity->getValue(_group_codes::layer);

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

		/**
		* Create ifcengine collections
		*/
		int64_t iCollectionClass = GetClassByName(pParser->getModel(), "Collection");
		assert(iCollectionClass != 0);

		vector<int64_t> vecCollections;
		for (auto itLayer2Instances : mapLayer2Instances)
		{
			string strCollectionName = "Layer: '";
			strCollectionName += itLayer2Instances.first;
			strCollectionName += "'";

			int64_t iCollectionInstance = CreateInstance(iCollectionClass, strCollectionName.c_str());
			SetObjectProperty(iCollectionInstance, GetPropertyByName(pParser->getModel(), "objects"), itLayer2Instances.second.data(), itLayer2Instances.second.size());

			vecCollections.push_back(iCollectionInstance);
		}

		string strCollectionName = "BLOCK: '";
		strCollectionName += m_mapCode2Value[_group_codes::name];
		strCollectionName += "'";

		m_iInstance = CreateInstance(iCollectionClass, strCollectionName.c_str());
		SetObjectProperty(m_iInstance, GetPropertyByName(pParser->getModel(), "objects"), vecCollections.data(), vecCollections.size());

		return m_iInstance;
	}
	// _block
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _insert
	_insert::_insert()
		: _entity(_group_codes::insert)
	{
		// INSERT, page 97
		map<string, string> mapCode2Value =
		{
			{_group_codes::name, ""}, // Block name
			{_group_codes::x, "0"}, // Insertion point (in OCS); DXF: X value; APP: 3D point
			{_group_codes::y, "0"}, // DXF: Y and Z values of insertion point (in OCS)
			{_group_codes::z, "0"}, // DXF: Y and Z values of insertion point (in OCS)
			{_group_codes::extrusion_x, "0"}, // Extrusion direction (optional; default = 0, 0, 1) DXF: X value; APP: 3D vector
			{_group_codes::extrusion_y, "0"}, // DXF: Y and Z values of extrusion direction (optional)
			{_group_codes::extrusion_z, "1"}, // DXF: Y and Z values of extrusion direction (optional)
		};

		m_mapCode2Value.insert(mapCode2Value.begin(), mapCode2Value.end());
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _insert::~_insert()
	{
	}

	// ----------------------------------------------------------------------------------------
	/*virtual*/ int64_t _insert::createInstance(_parser* pParser)
	{
		auto pBlock = pParser->findBlockByName(m_mapCode2Value[_group_codes::name]);
		if (pBlock == nullptr)
		{
			return 0; // TODO
		}

		pBlock->setValue(_group_codes::extrusion_x, getValue(_group_codes::extrusion_x));
		pBlock->setValue(_group_codes::extrusion_y, getValue(_group_codes::extrusion_y));
		pBlock->setValue(_group_codes::extrusion_z, getValue(_group_codes::extrusion_z));

		int64_t iBlockInstance = pBlock->createInstance(pParser);

		int64_t iTransformationClass = GetClassByName(pParser->getModel(), "Transformation");
		assert(iTransformationClass != 0);

		int64_t iMatrixClass = GetClassByName(pParser->getModel(), "Matrix");
		assert(iMatrixClass != 0);		

		_extrusion extrusion(this);

		// Matrix
		int64_t iMatrixInstance = CreateInstance(iMatrixClass, type().c_str());
		assert(iMatrixInstance != 0);

		double dValue = extrusion.getValue(_group_codes::x);
		SetDataTypeProperty(iMatrixInstance, GetPropertyByName(pParser->getModel(), "_41"), &dValue, 1);

		dValue = extrusion.getValue(_group_codes::y);
		SetDataTypeProperty(iMatrixInstance, GetPropertyByName(pParser->getModel(), "_42"), &dValue, 1);

		dValue = extrusion.getValue(_group_codes::z);
		SetDataTypeProperty(iMatrixInstance, GetPropertyByName(pParser->getModel(), "_43"), &dValue, 1);

		// Transformation
		int64_t iTransformationInstance = CreateInstance(iTransformationClass, type().c_str());
		assert(iTransformationInstance != 0);

		SetObjectProperty(iTransformationInstance, GetPropertyByName(pParser->getModel(), "matrix"), &iMatrixInstance, 1);
		SetObjectProperty(iTransformationInstance, GetPropertyByName(pParser->getModel(), "object"), &iBlockInstance, 1);

		if (m_pParent == nullptr)
		{
			pParser->onInstanceCreated(this, iTransformationInstance);
		}

		return iTransformationInstance;
	}
	// _block
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _section
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
		_entity* pEntity = nullptr;

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

				if ((pEntity = _parser::loadEntity(reader)) != nullptr)
				{
					m_vecEntities.push_back(pEntity);
				}
				else
				{					
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
	// _classes_section
	_classes_section::_classes_section()
		: _section(_group_codes::classes)
	{
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _classes_section::~_classes_section()
	{
	}

	// --------------------------------------------------------------------------------------------
	void _classes_section::load(_reader& reader)
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
	// _classes_section
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// _objects_section
	_objects_section::_objects_section()
		: _section(_group_codes::objects)
	{
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _objects_section::~_objects_section()
	{
	}

	// --------------------------------------------------------------------------------------------
	void _objects_section::load(_reader& reader)
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
	// _objects_section
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
		, m_vecBlocks()
	{
	}

	// --------------------------------------------------------------------------------------------
	/*virtual*/ _blocks_section::~_blocks_section()
	{
		for (size_t i = 0; i < m_vecBlocks.size(); i++)
		{
			delete m_vecBlocks[i];
		}
	}

	// --------------------------------------------------------------------------------------------
	const vector<_block*>& _blocks_section::blocks()
	{
		return m_vecBlocks;
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
				else if (reader.row() == _group_codes::block)
				{
					reader.forth();

					auto pBlock = new _block();
					m_vecBlocks.push_back(pBlock);

					pBlock->load(reader);
				}
				else
				{
					reader.forth();
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
	// _parser
	// --------------------------------------------------------------------------------------------
	_parser::_parser(int64_t iModel)
		: m_iModel(iModel)
		, m_vecSections()
		, m_mapLayer2Instances()
	{
		assert(m_iModel != 0);

		/*
		* Default color
		*/
		// ((R * 255 + G) * 255 + B) * 255 + A
		uint32_t iR = 122;
		uint32_t iG = 122;
		uint32_t iB = 122;
		uint32_t iDefaultColor = 256 * 256 * 256 * iR +
			256 * 256 * iG +
			256 * iB +
			255;

		SetDefaultColor(m_iModel, iDefaultColor, iDefaultColor, iDefaultColor, iDefaultColor);
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
	int64_t _parser::getModel() const
	{
		return m_iModel;
	}

	// --------------------------------------------------------------------------------------------
	void _parser::load(const char* szFile)
	{
		if ((szFile == nullptr) || (strlen(szFile) == 0))
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
						}
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
						else if (reader.row() == _group_codes::classes)
						{
							reader.forth();

							auto pClassesSection = new _classes_section();
							m_vecSections.push_back(pClassesSection);

							pClassesSection->load(reader);
						}
						else if (reader.row() == _group_codes::objects)
						{
							reader.forth();

							auto pObjectsSection = new _objects_section();
							m_vecSections.push_back(pObjectsSection);

							pObjectsSection->load(reader);
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
	void _parser::onInstanceCreated(_entity* pEntity, int64_t iInstance)
	{
		assert(pEntity != nullptr);

		auto strLayer = pEntity->getValue(_group_codes::layer);

		auto itLayer2Instances = m_mapLayer2Instances.find(strLayer);
		if (itLayer2Instances != m_mapLayer2Instances.end())
		{
			m_mapLayer2Instances[strLayer].push_back(iInstance);
		}
		else
		{
			m_mapLayer2Instances[strLayer] = vector<int64_t>{ iInstance };
		}
	}

	// --------------------------------------------------------------------------------------------
	/*static*/ _entity* _parser::loadEntity(_reader& reader)
	{
		if (reader.row() == _group_codes::line)
		{
			reader.forth();

			auto pLine = new _line();
			pLine->load(reader);

			return pLine;
		}
		else if (reader.row() == _group_codes::arc)
		{
			reader.forth();

			auto pArc = new _arc();
			pArc->load(reader);

			return pArc;
		}
		else if (reader.row() == _group_codes::text)
		{
			reader.forth();

			auto pText = new _text();
			pText->load(reader);

			return pText;
		}
		else if (reader.row() == _group_codes::mtext)
		{
			reader.forth();

			auto pMText = new _mtext();
			pMText->load(reader);

			return pMText;
		}
		else if (reader.row() == _group_codes::viewport)
		{
			reader.forth();

			auto pViewport = new _viewport();
			pViewport->load(reader);

			return pViewport;
		}
		else if (reader.row() == _group_codes::polyline)
		{
			reader.forth();

			auto pPolyline = new _polyline();
			pPolyline->load(reader);

			return pPolyline;
		}
		else if (reader.row() == _group_codes::vertex)
		{
			reader.forth();

			auto pVertex = new _vertex();
			pVertex->load(reader);

			return pVertex;
		}
		else if (reader.row() == _group_codes::lwpolyline)
		{
			reader.forth();

			auto pLWPolyline = new _lwpolyline();
			pLWPolyline->load(reader);

			return pLWPolyline;
		}
		else if (reader.row() == _group_codes::circle)
		{
			reader.forth();

			auto pCircle = new _circle();
			pCircle->load(reader);

			return pCircle;
		}
		else if (reader.row() == _group_codes::insert)
		{
			reader.forth();

			auto pInsert = new _insert();
			pInsert->load(reader);

			return pInsert;
		}

		// TODO: ALL ENTITIES

		return nullptr;
	}
	
	// --------------------------------------------------------------------------------------------
	_block* _parser::findBlockByName(const string& strBlockName)
	{
		for (auto itSection : m_vecSections)
		{
			if (itSection->type() == _group_codes::blocks)
			{
				auto pBlocksSection = dynamic_cast<_blocks_section*>(itSection);
				for (auto itBlock : pBlocksSection->blocks())
				{
					if (itBlock->getValue(_group_codes::name) == strBlockName)
					{
						return itBlock;
					}
				}
			}				
		}

		return nullptr;
	}

	// --------------------------------------------------------------------------------------------
	void _parser::createInstances()
	{
		/**
		* Create ifcengine instances
		*/
		for (size_t iSection = 0; iSection < m_vecSections.size(); iSection++)
		{
			if (m_vecSections[iSection]->type() == _group_codes::entities)
			{
				auto pEntitiesSection = dynamic_cast<_entities_section*>(m_vecSections[iSection]);
				assert(pEntitiesSection != nullptr);

				for (size_t iEntity = 0; iEntity < pEntitiesSection->entities().size(); iEntity++)
				{
					auto pEntity = pEntitiesSection->entities()[iEntity];
					assert(pEntity != nullptr);

					pEntity->createInstance(this);
				} // for (size_t iEntity = ...
			} // if (m_vecSections[iSection]->type() == _group_codes::entities)
		} // for (size_t iSection = ...

		/**
		* Create ifcengine collections
		*/
		int64_t iCollectionClass = GetClassByName(m_iModel, "Collection");
		assert(iCollectionClass != 0);
		
		for (auto itLayer2Instances : m_mapLayer2Instances)
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