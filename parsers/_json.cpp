#include "_host.h"

#include "_json.h"
#include "_string.h"
#if _LOAD_SCHEMAS
#include "_net.h"
#endif _LOAD_SCHEMAS

#include <chrono>

// ------------------------------------------------------------------------------------------------
namespace _json	
{
	// ********************************************************************************************
	const char START_OBJECT = '{';
	const char END_OBJECT = '}';
	const char START_ARRAY = '[';
	const char END_ARRAY = ']';	

	// ********************************************************************************************
	static const char SCHEMA_TAG[] = "#schema";
	static const char DOCUMENT_TAG[] = "#document";

	// ********************************************************************************************
	_value::_value(_property* pParent, _element_site* pSite, enumValueType enValueType)
		: m_pParent(pParent)
		, m_pSite(pSite)
		, m_enType(enValueType)
	{
		VERIFY_POINTER(m_pSite);
	}

	/*virtual*/ _value::~_value()
	{}

	/*static*/ _value* _value::loadValue(_stream_reader* pReader, _property* pParent, _element_site* pSite)
	{
		VERIFY_POINTER(pReader);
		VERIFY_POINTER(pParent);

		char ch = pReader->getChar();
		if (ch == EOF)
		{
			THROW_INTERNAL_ERROR();
		}

		while (ch != EOF)
		{
			if (isblank(ch) || (ch == LF) || (ch == CR))
			{
				ch = pReader->getNextChar(false);

				continue;
			}
			else if (ch == START_OBJECT)
			{	
				auto pObject = new _object(pParent, pSite);
				pObject->load(pReader);

				return pObject;
			}
			else if (ch == START_ARRAY)
			{
				auto pArray = new _array(pParent, pSite);
				pArray->load(pReader);

				return pArray;
			}
			else
			{
				auto pSimple = new _simple(pParent, pSite);
				pSimple->load(pReader);

				return pSimple;
			}
		} // while (ch != EOF)

		THROW_INTERNAL_ERROR();		
	}

	/*static*/ void _value::loadString(_stream_reader* pReader, string& strValue)
	{
		VERIFY_POINTER(pReader);
		VERIFY_STLOBJ_IS_EMPTY(strValue);

		char ch = pReader->getChar();
		if (ch != DOULE_QUOT_MARK)
		{
			THROW_INTERNAL_ERROR();
		}

		ch = pReader->getNextChar(true);
		while (ch != EOF)
		{
			if ((ch == DOULE_QUOT_MARK) &&
				(strValue.empty() || (strValue.back() != BACK_SLASH)))
			{
				ch = pReader->getNextChar(true);

				break;
			}

			strValue += ch;

			ch = pReader->getNextChar(true);
		} // while (ch != EOF)
	}

	// ********************************************************************************************
	_simple::_simple(_property* pParent, _element_site* pSite)
		: _value(pParent, pSite, enumValueType::simple)
		, m_strValue()
		, m_bIsString(false)
	{}

	/*virtual*/ void _simple::serialize(_srln::_archive* pArchive) /*override*/
	{
		VERIFY_POINTER(pArchive);

		vector<string> vecData
		{
			"_simple",
			m_strValue,
			m_bIsString ? "true" : "false",
		};

		pArchive->writeObject(this, vecData);
	}

	_simple::_simple(_property* pParent, _element_site* pSite, _srln::_archive* pArchive, const string& strTag)
		: _simple(pParent, pSite)
	{
		VERIFY_POINTER(pArchive);
		VERIFY_STLOBJ_IS_NOT_EMPTY(strTag);

		auto& vecData = pArchive->readData(strTag);
		if ((vecData.size() != 3) || (vecData[0] != "_simple"))
		{
			THROW_INTERNAL_ERROR();
		}

		m_strValue = vecData[1];
		m_bIsString = vecData[2] == "true";
	}

	/*virtual*/ _simple::~_simple()
	{}

	/*virtual*/ void _simple::load(_stream_reader* pReader) /*override*/
	{
		VERIFY_POINTER(pReader);

		char ch = pReader->getChar();
		if (ch == EOF)
		{
			THROW_INTERNAL_ERROR();
		}
		
		while (ch != EOF)
		{
			if (isblank(ch) || (ch == LF) || (ch == CR))
			{
				ch = pReader->getNextChar(true);

				continue;
			}
			else if ((ch == END_OBJECT) || (ch == END_ARRAY))
			{
				return;
			}
			else if (ch == COMMA)
			{
				ch = pReader->getNextChar(true);

				return;
			}
			else if (ch == DOULE_QUOT_MARK)
			{
				_value::loadString(pReader, m_strValue);

				m_bIsString = true;

				return;
			}
			else
			{
				m_strValue += ch;
			}

			ch = pReader->getNextChar(true);
		} // while (ch != EOF)

		THROW_INTERNAL_ERROR();
	}

	// ********************************************************************************************
	_object::_object(_property* pParent, _element_site* pSite)
		: _value(pParent, pSite, enumValueType::object)
		, m_mapProperties()
	{}

	/*virtual*/ void _object::serialize(_srln::_archive* pArchive) /*override*/
	{
		VERIFY_POINTER(pArchive);

		vector<string> vecData
		{
			"_object",
		};

		for (auto itProperty : m_mapProperties)
		{
			itProperty.second->serialize(pArchive);
			vecData.push_back(pArchive->getObjectTag(itProperty.second));
		}

		pArchive->writeObject(this, vecData);
	}

	_object::_object(_property* pParent, _element_site* pSite, _srln::_archive* pArchive, const string& strTag)
		: _object(pParent, pSite)
	{
		VERIFY_POINTER(pArchive);
		VERIFY_STLOBJ_IS_NOT_EMPTY(strTag);

		auto& vecData = pArchive->readData(strTag);
		if (vecData.empty() || (vecData[0] != "_object"))
		{
			THROW_INTERNAL_ERROR();
		}

		for (size_t iProperty = 1; iProperty < vecData.size(); iProperty++)
		{
			string strReference = vecData[iProperty];

			auto pProperty = new _property(this, pSite, pArchive, strReference);
			m_mapProperties[pProperty->getName()] = pProperty;
		}
	}

	/*virtual*/ _object::~_object()
	{
		for (auto itProperty : m_mapProperties)
		{
			delete itProperty.second;
		}
	}

	/*virtual*/ void _object::load(_stream_reader* pReader) /*override*/
	{
		VERIFY_POINTER(pReader);

		char ch = pReader->getChar();
		if (ch != START_OBJECT)
		{
			THROW_INTERNAL_ERROR();
		}

		ch = pReader->getNextChar(true);
		while (ch != EOF)
		{
			if (isblank(ch) || (ch == COMMA) || (ch == LF) || (ch == CR))
			{
				ch = pReader->getNextChar(true);

				continue;
			}
			else if (ch == DOULE_QUOT_MARK)
			{
				auto pProperty = new _property(this, getSite());
				pProperty->load(pReader);

				if (m_mapProperties.find(pProperty->getName()) == m_mapProperties.end())
				{					
					m_mapProperties[pProperty->getName()] = pProperty;
				}
				else
				{
					// TODO: report error
					// logErrf("Duplicated Property: '%s' - %s.", getName().c_str(), pProperty->getName().c_str());
					delete pProperty;
				}

				ch = pReader->getChar();

				continue;
			}
			else if (ch == END_OBJECT)
			{
				ch = pReader->getNextChar(false);

				return;
			}

			THROW_INTERNAL_ERROR();
		} // while (ch != EOF)

		THROW_INTERNAL_ERROR();
	}

	bool _object::hasProperty(const string& strProperty) const
	{
		return getProperty(strProperty) != nullptr;
	}

	const _property* _object::getProperty(const string& strProperty) const
	{
		auto itProperty = m_mapProperties.find(strProperty);
		if (itProperty != m_mapProperties.end())
		{
			return itProperty->second;
		}

		return nullptr;
	}

	const _value* _object::getPropertyValue(const string& strProperty) const
	{
		auto pProperty = getProperty(strProperty);
		if (pProperty != nullptr)
		{
			return pProperty->getValue();
		}

		return nullptr;
	}

	bool _object::hasProperty(const vector<string>& vecPath, const string& strProperty) const
	{
		return getProperty(vecPath, strProperty) != nullptr;
	}

	const _property* _object::getProperty(const vector<string>& vecObjectProperties, const string& strProperty) const
	{
		if (vecObjectProperties.empty())
		{
			THROW_ARGUMENT_ERROR();
		}

		const _object* pObject = this;
		for (auto strObjectProperty : vecObjectProperties)
		{
			auto pProperty = pObject->getProperty(strObjectProperty);
			if (pProperty == nullptr)
			{
				return nullptr;
			}

			pObject = pProperty->getValue()->as<_object>();
			if (pObject == nullptr)
			{
				return nullptr;
			}
		}

		return pObject->getProperty(strProperty);
	}

	const _value* _object::getPropertyValue(const vector<string>& vecObjectProperties, const string& strProperty) const
	{
		auto pProperty = getProperty(vecObjectProperties, strProperty);
		if (pProperty != nullptr)
		{
			return pProperty->getValue();
		}

		return nullptr;
	}

	// ********************************************************************************************
	_array::_array(_property* pParent, _element_site* pSite)
		: _value(pParent, pSite, enumValueType::array)
		, m_vecValues()
	{}

	/*virtual*/ void _array::serialize(_srln::_archive* pArchive) /*override*/
	{
		VERIFY_POINTER(pArchive);

		vector<string> vecData =
		{
			"_array",
		};

		for (auto pValue : m_vecValues)
		{
			pValue->serialize(pArchive);
			vecData.push_back(pArchive->getObjectTag(pValue));
		}

		pArchive->writeObject(this, vecData);
	}

	_array::_array(_property* pParent, _element_site* pSite, _srln::_archive* pArchive, const string& strTag)
		: _array(pParent, pSite)
	{
		VERIFY_POINTER(pArchive);
		VERIFY_STLOBJ_IS_NOT_EMPTY(strTag);

		auto& vecData = pArchive->readData(strTag);
		if (vecData.empty() || (vecData[0] != "_array"))
		{
			THROW_INTERNAL_ERROR();
		}

		for (size_t iValue = 1; iValue < vecData.size(); iValue++)
		{
			string strReference = vecData[iValue];

			auto& vecValueData = pArchive->readData(strReference);
			if (vecValueData.empty())
			{
				THROW_INTERNAL_ERROR();
			}

			string strType = vecValueData[0];
			if (strType == "_simple")
			{
				auto pValue = new _simple(pParent, pSite, pArchive, strReference);
				m_vecValues.push_back(pValue);
			}
			else if (strType == "_object")
			{
				auto pValue = new _object(pParent, pSite, pArchive, strReference);
				m_vecValues.push_back(pValue);
			}
			else if (strType == "_array")
			{
				auto pValue = new _array(pParent, pSite, pArchive, strReference);
				m_vecValues.push_back(pValue);
			}
			else
			{
				THROW_INTERNAL_ERROR();
			}
		}
	}

	/*virtual*/ _array::~_array()
	{
		for (auto pValue : m_vecValues)
		{
			delete pValue;
		}
	}

	/*virtual*/ void _array::load(_stream_reader* pReader) /*override*/
	{
		VERIFY_POINTER(pReader);

		char ch = pReader->getChar();
		if (ch != START_ARRAY)
		{
			THROW_INTERNAL_ERROR();
		}

		ch = pReader->getNextChar(true);
		while (ch != EOF)
		{
			if (isblank(ch) || (ch == COMMA) || (ch == LF) || (ch == CR))
			{
				ch = pReader->getNextChar(true);

				continue;
			}
			else if (ch == END_ARRAY)
			{
				ch = pReader->getNextChar(true);

				return;
			}
			else
			{
				auto pValue = _value::loadValue(pReader, getParent(), getSite());
				VERIFY_POINTER(pValue);
				m_vecValues.push_back(pValue);

				ch = pReader->getChar();

				continue;
			}
		} // while (ch != EOF)

		THROW_INTERNAL_ERROR();
	}		

	// ********************************************************************************************
	_property::_property(_object* pParent, _element_site* pSite)
		: m_pParent(pParent)
		, m_pSite(pSite)
		, m_strName("")
		, m_pValue(nullptr)
	{
		VERIFY_POINTER(m_pParent);
		VERIFY_POINTER(m_pSite);
	}

	/*virtual*/ void _property::serialize(_srln::_archive* pArchive)
	{
		VERIFY_POINTER(pArchive);

		m_pValue->serialize(pArchive);

		vector<string> vecData
		{
			"_property",
			m_strName,
			pArchive->getObjectTag(m_pValue),
		};

		pArchive->writeObject(this, vecData);
	}

	_property::_property(_object* pParent, _element_site* pSite, _srln::_archive* pArchive, const string& strTag)
		: _property(pParent, pSite)
	{
		VERIFY_POINTER(pArchive);
		VERIFY_STLOBJ_IS_NOT_EMPTY(strTag);

		auto& vecData = pArchive->readData(strTag);
		if ((vecData.size() != 3) || (vecData[0] != "_property"))
		{
			THROW_INTERNAL_ERROR();
		}

		m_strName = vecData[1];
		string strReference = vecData[2];

		auto& vecValueData = pArchive->readData(strReference);
		if (vecValueData.empty())
		{
			THROW_INTERNAL_ERROR();
		}

		string strType = vecValueData[0];
		if (strType == "_simple")
		{
			m_pValue = new _simple(this, pSite, pArchive, strReference);
		}
		else if (strType == "_object")
		{
			m_pValue = new _object(this, pSite, pArchive, strReference);
		}
		else if (strType == "_array")
		{
			m_pValue = new _array(this, m_pSite, pArchive, strReference);
		}
		else
		{
			THROW_INTERNAL_ERROR();
		}
	}

	/*virtual*/ _property::~_property()
	{
		delete m_pValue;
	}	

	/*virtual*/ void _property::load(_stream_reader* pReader)
	{
		VERIFY_POINTER(pReader);

		loadName(pReader);

		pReader->goToNextChar(COLON);
		pReader->getNextChar(true);

		m_pValue = _value::loadValue(pReader, this, m_pSite);
		VERIFY_POINTER(m_pValue);

		m_pSite->onPropertyLoaded(this);
	}

	void _property::loadName(_stream_reader* pReader)
	{
		VERIFY_POINTER(pReader);
		VERIFY_STLOBJ_IS_EMPTY(m_strName);

		char ch = pReader->getChar();
		if (ch != DOULE_QUOT_MARK)
		{
			THROW_INTERNAL_ERROR();
		}

		ch = pReader->getNextChar(true);
		while (ch != EOF)
		{
			if ((ch == DOULE_QUOT_MARK) && 
				(m_strName.empty() || (m_strName.back() != BACK_SLASH)))
			{
				ch = pReader->getNextChar(true);

				break;
			}

			m_strName += ch;

			ch = pReader->getNextChar(true);
		} // while (ch != EOF)
	}

	// ********************************************************************************************
	_document::_document(_document_site* pSite)
		: m_pSite(pSite)
		, m_pReader(nullptr)
		, m_pRoot(nullptr)
		, m_mapSchemas()
		, m_pInfoset(nullptr)
		, m_mapMetaData()		
	{}

	/*virtual*/ _document::~_document()
	{
		clean();
	}

	/*virtual*/ void _document::serialize(_srln::_archive* pArchive) /*override*/
	{
		VERIFY_POINTER(pArchive);
		VERIFY_POINTER(m_pRoot);

		m_pRoot->serialize(pArchive);

		vector<string> vecData
		{
			"_document",
			pArchive->getObjectTag(getRoot()),
		};

		pArchive->writeData(DOCUMENT_TAG, vecData);
	}

	/*virtual*/ void _document::deserialize(_srln::_archive* pArchive) /*override*/
	{
		VERIFY_POINTER(pArchive);

		auto& vecData = pArchive->readData(DOCUMENT_TAG);
		if ((vecData.size() != 2) || (vecData[0] != "_document"))
		{
			THROW_INTERNAL_ERROR();
		}

		m_pRoot = new _object(nullptr, this, pArchive, vecData[1]);
	}

	/*virtual*/ void _document::onValueLoaded(_value* pValue) /*override*/
	{
		if (m_pSite != nullptr)
		{
			m_pSite->onValueLoaded(pValue);
		}
	}

	/*virtual*/ void _document::onPropertyLoaded(_property* pProperty) /*override*/
	{
		if (m_pSite != nullptr)
		{
			m_pSite->onPropertyLoaded(pProperty);
		}
	}

	void _document::load(const char* szFile)
	{
		clean();

		if (szFile != nullptr)
		{
			// Load a file
			m_pReader = new _file_reader();
			if (!dynamic_cast<_file_reader*>(m_pReader)->open(szFile))
			{
				THROW_ERROR(_err::_file);
			}

			load();
		}
		else
		{
			// Create empty document
			m_pRoot = new _object(nullptr, this);
		}		
	}

	void _document::load(istream* pStream)
	{
		clean();

		m_pReader = new _stream_reader();
		m_pReader->init(pStream);

		load();
	}

#ifdef _LOAD_SCHEMAS
	/*virtual*/ void _document::loadSchemas()
	{
		VERIFY_POINTER(m_pSite);

		m_pSite->loadSchemas();

		loadInfoset();
	}

	_schema* _document::getSchema(const string& strNamespace, const string& strLocation, bool& bNew)
	{
		if (strNamespace.empty() || strLocation.empty())
		{
			THROW_ARGUMENT_ERROR();
		}

		bNew = false;

		string strNPNamespace = _net::_url::removeProtocol(strNamespace);
		string strNPLocation = _net::_url::removeProtocol(strLocation);

		auto itNamespace2Schemas = m_mapSchemas.find(strNPNamespace);
		if (itNamespace2Schemas != m_mapSchemas.end())
		{
			auto itLocation2Schema = itNamespace2Schemas->second.find(strNPLocation);
			if (itLocation2Schema != itNamespace2Schemas->second.end())
			{
				return itLocation2Schema->second;
			}
		}

		bNew = true;

		auto pSchema = new _schema(m_pSite, strNamespace, strLocation);
		if (itNamespace2Schemas != m_mapSchemas.end())
		{
			itNamespace2Schemas->second[strNPLocation] = pSchema;
		}
		else
		{
			m_mapSchemas[strNPNamespace] = map<string, _schema*>{ { strNPLocation, pSchema } };
		}

		return pSchema;
	}
#endif // _LOAD_SCHEMAS

	void _document::loadInfoset()
	{
		assert(m_pInfoset == nullptr);

		m_pInfoset = new _infoset(this);
		m_pInfoset->load();
	}

	/*virtual*/ void _document::clean()
	{
		delete m_pReader;
		m_pReader = nullptr;

		delete m_pRoot;
		m_pRoot = nullptr;

		for (auto itNamespace2Schemas : m_mapSchemas)
		{
			for (auto itLocation2Schema : itNamespace2Schemas.second)
			{
				delete itLocation2Schema.second;
			}
		}
		m_mapSchemas.clear();

		delete m_pInfoset;
		m_pInfoset = nullptr;

		m_mapMetaData.clear();
	}

	/*virtual*/ void _document::load()
	{
		VERIFY_POINTER(m_pReader);

		char ch = m_pReader->getChar();

		/* EOF */
		if (ch == EOF)
		{
			THROW_ERROR(_err::_file);
		}

		while (ch != EOF)
		{
			if (isblank(ch) || (ch == LF) || (ch == CR))
			{
				ch = m_pReader->getNextChar(false);

				continue;
			}

			if (ch == START_OBJECT)
			{
				// Root
				if (m_pRoot != nullptr)
				{
					THROW_ERROR(_err::_format);
				}

				m_pRoot = new _object(nullptr, this);
				m_pRoot->load(m_pReader);

				ch = m_pReader->getChar();

				continue;
			} // if (ch == START_OBJECT)

			THROW_INTERNAL_ERROR();
		} // while (ch != EOF)
	}

	// ********************************************************************************************
	_schema::_schema(_document_site* pSite, const string& strNamespace, const string& strLocation)
		: _document(pSite)
		, m_strNamespace(strNamespace)
		, m_strLocation(strLocation)
	{
		VERIFY_POINTER(pSite);

		if (m_strNamespace.empty() || m_strLocation.empty())
		{
			THROW_ARGUMENT_ERROR();
		}
	}

	/*virtual*/ _schema::~_schema()
	{}	

	/*virtual*/ void _schema::serialize(_srln::_archive* pArchive) /*override*/
	{
		VERIFY_POINTER(pArchive);

		_document::serialize(pArchive);

		vector<string> vecData
		{
			"_schema",
			m_strNamespace,
			m_strLocation,
		};

		pArchive->writeData(SCHEMA_TAG, vecData);
	}

	/*virtual*/ void _schema::deserialize(_srln::_archive* pArchive) /*override*/
	{
		VERIFY_POINTER(pArchive);

		_document::deserialize(pArchive);

		auto& vecData = pArchive->readData(SCHEMA_TAG);
		if ((vecData.size() != 3) || (vecData[0] != "_schema"))
		{
			THROW_INTERNAL_ERROR();
		}

		m_strNamespace = vecData[1];
		m_strLocation = vecData[2];
	}

#ifdef _LOAD_SCHEMAS
	string _schema::getNamespace(bool bRemoveProtocol) const
	{
		if (bRemoveProtocol)
		{
			_net::_url::removeProtocol(m_strNamespace);
		}

		return m_strNamespace;
	}

	string _schema::getLocation(bool bRemoveProtocol) const
	{
		if (bRemoveProtocol)
		{
			_net::_url::removeProtocol(m_strLocation);
		}

		return m_strLocation;
	}
#endif // _LOAD_SCHEMAS

	// ********************************************************************************************
	_infoset::_infoset(_document* pDocument)
		: m_pDocument(pDocument)
		, m_mapSchemaReferences()
		, m_mapObjectDefinitions()
	{
		VERIFY_POINTER(m_pDocument);
	}

	/*virtual*/ _infoset::~_infoset()
	{}

	void _infoset::load()
	{
		auto& mapSchemas = m_pDocument->getSchemas();
		if (mapSchemas.size() != 1)
		{
			THROW_INTERNAL_ERROR();
		}

		auto itNamespace2Schemas = mapSchemas.begin();
		for (auto itLocation2Schema : itNamespace2Schemas->second)
		{
			if (itLocation2Schema.second->getRoot() == nullptr)
			{
				continue;
			}

			/* References */
			string strFileName = _io::_file::getFileName(itLocation2Schema.first);
			if (m_mapSchemaReferences.find(strFileName) != m_mapSchemaReferences.end())
			{
				THROW_INTERNAL_ERROR();
			}

			m_mapSchemaReferences[strFileName] = itLocation2Schema.second;

			loadSchema(itLocation2Schema.second);
		}
	}

	string _infoset::getObjectParentType(const string& strType) const
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(strType);

		auto itObjectDefinitions = m_mapObjectDefinitions.find(strType);
		if (itObjectDefinitions == m_mapObjectDefinitions.end())
		{
			return "";
		}

		auto pAllOfProperty = itObjectDefinitions->second->getPropertyValueAs<_array>(ALL_OF_PROP);
		if (pAllOfProperty == nullptr)
		{
			return "";
		}

		for (auto pValue : pAllOfProperty->getValues())
		{
			if (pValue->getType() != enumValueType::object)
			{
				continue;
			}

			auto pObjectValue = pValue->as<_object>();

			auto pReferenceValue = pObjectValue->getPropertyValueAs<_simple>(REFERENCE_PROP);
			if (pReferenceValue == nullptr)
			{
				continue;
			}

			string strReference = pReferenceValue->getValue();

			size_t iIndex = strReference.find("#/");
			if (iIndex != string::npos)
			{
				strReference = strReference.substr(iIndex + 2);
			}

			return strReference;
		} // for (auto pValue ...

		return "";
	}

	void _infoset::getSimplePropertySupportedTypes(const _property* pJSONProperty, vector<string>& vecTypes)
	{
		VERIFY_POINTER(pJSONProperty);
		VERIFY_POINTER(pJSONProperty->getParent());		

		auto pJSDObjectDefinition = getObjectDefinition(pJSONProperty);
		VERIFY_POINTER(pJSDObjectDefinition);

		auto pJSDPropertyDefinition = getPropertyDefinition(pJSDObjectDefinition, pJSONProperty);
		if (pJSDPropertyDefinition != nullptr)
		{
			if (pJSDPropertyDefinition->hasProperty(TYPE_PROP))
			{
				getTypes(pJSDPropertyDefinition, vecTypes);

				if (vecTypes[0] == ARRAY_TYPE)
				{
					auto pJSDItemsDefinition = pJSDPropertyDefinition->getPropertyValueAs<_object>(ITEMS_PROP);
					getTypes(pJSDItemsDefinition, vecTypes);
				}
			}
			else if (pJSDObjectDefinition->hasProperty(ENUM_PROP))
			{
				vecTypes.push_back(STRING_TYPE);
			}
			else
			{
				THROW_INTERNAL_ERROR();
			}
		}
		else
		{
			// Unknown type
			vecTypes.push_back(STRING_TYPE);
		}		
	}

	void _infoset::loadSchema(_schema* pSchema)
	{
		VERIFY_POINTER(pSchema);

		auto pRoot = pSchema->getRoot();
		VERIFY_POINTER(pRoot);

		if (pRoot->hasProperty(TYPE_PROP))
		{
			auto pTypeValue = pRoot->getPropertyValueAs<_simple>(TYPE_PROP);
			if (pTypeValue->getValue() == OBJECT_TYPE)
			{
				/* Root objects, e.g. CityJSON, CityJSONFeature */
				auto pTypeEnum = pRoot->getPropertyValueAs<_array>(
					vector<string>{ PROPERTIES_PROP, TYPE_PROP }, ENUM_PROP);
				VERIFY_POINTER(pTypeEnum);

				for (auto pValue : pTypeEnum->getValues())
				{
					auto pType = pValue->as<_simple>();
					VERIFY_POINTER(pType);

					if (m_mapObjectDefinitions.find(pType->getValue()) != m_mapObjectDefinitions.end())
					{
						THROW_INTERNAL_ERROR();
					}

					m_mapObjectDefinitions[pType->getValue()] = pRoot;
				}
			} // if (pTypeValue->getValue() == OBJECT_TYPE)
			else if ((pTypeValue->getValue() == EXTENSION_TYPE) || 
				(pTypeValue->getValue() == EXTENSION_TYPE_II))
			{
				auto pExtraRootPropertiesValue = pRoot->getPropertyValueAs<_object>(EXTRA_ROOT_PROPERTIES_PROP);
				if (pExtraRootPropertiesValue != nullptr)
				{}

				auto pExtraAttributesValue = pRoot->getPropertyValueAs<_object>(EXTRA_ATTRIBUTES_PROP);
				if (pExtraAttributesValue != nullptr)
				{}

				auto pExtraCityObjectsValue = pRoot->getPropertyValueAs<_object>(EXTRA_CITY_OBJECTS_PROP);
				if (pExtraCityObjectsValue != nullptr)
				{
					/* Objects, e.g. Material, Texture, etc. */
					for (auto itProperty : pExtraCityObjectsValue->getProperties())
					{
						if (itProperty.second->getValue()->getType() != enumValueType::object)
						{
							continue;
						}

						auto pObject = itProperty.second->getValue()->as<_object>();
						VERIFY_POINTER(pObject);

						// Example I:
						// "+GenericCityObject": {
						//	"allOf": [
						//		{ "$ref": "cityobjects.schema.json#/_AbstractCityObject" }

						if (m_mapObjectDefinitions.find(itProperty.first) != m_mapObjectDefinitions.end())
						{
							THROW_INTERNAL_ERROR();
						}

						m_mapObjectDefinitions[itProperty.first] = pObject;
					} // for (auto itProperty ...
				} // if (pExtraCityObjectsValue != nullptr)

				auto pDefinitionsValue = pRoot->getPropertyValueAs<_object>(DEFINITIONS_PROP);
				if (pDefinitionsValue != nullptr)
				{
					/* Objects, e.g. Material, Texture, etc. */
					for (auto itProperty : pDefinitionsValue->getProperties())
					{
						if (itProperty.second->getValue()->getType() != enumValueType::object)
						{
							continue;
						}

						auto pObject = itProperty.second->getValue()->as<_object>();
						VERIFY_POINTER(pObject);

						// Example I:
						// "measure": {
						//	"type": "object",
						//	"properties": {
						//		"value": { "type": "number" },

						// Example II:
						// "train": {
						//	"type": "object",
						//		"properties" : {
						//		"trainType": { "type": "string" },

						if (m_mapObjectDefinitions.find(itProperty.first) != m_mapObjectDefinitions.end())
						{
							THROW_INTERNAL_ERROR();
						}

						m_mapObjectDefinitions[itProperty.first] = pObject;
					} // for (auto itProperty ...
				} // if (pDefinitionsValue != nullptr)
			} // if ((pTypeValue->getValue() == EXTENSION_TYPE) || ...
			else
			{
				THROW_INTERNAL_ERROR();
			}
		} // if pRoot->hasProperty(TYPE_PROP))
		else
		{
			/* Objects, e.g. Material, Texture, etc. */
			for (auto itProperty : pRoot->getProperties())
			{
				if (itProperty.second->getValue()->getType() != enumValueType::object)
				{
					continue;
				}

				auto pObject = itProperty.second->getValue()->as<_object>();
				VERIFY_POINTER(pObject);

				// Example I:
				// "ExtensionObject": {
				//	"type": "object",

				// Example II:
				// "_AbstractCityObject": {
				//	"properties": {
				//		"attributes": {
				//			"type": "object"

				if (m_mapObjectDefinitions.find(itProperty.first) != m_mapObjectDefinitions.end())
				{
					THROW_INTERNAL_ERROR();
				}

				m_mapObjectDefinitions[itProperty.first] = pObject;
			} // for (auto itProperty ...
		} // else if pRoot->hasProperty(TYPE_PROP))
	}

	const _object* _infoset::getObjectDefinition(const _property* pJSONProperty) const
	{
		VERIFY_POINTER(pJSONProperty);
		VERIFY_POINTER(pJSONProperty->getParent());

		vector<const _property*> vecJSONParentProperties;
		auto pJSDObjectDefinition = getObjectDefinition(pJSONProperty, vecJSONParentProperties);

		if (pJSDObjectDefinition != nullptr)
		{
			for (auto pJSONParentProperty : vecJSONParentProperties)
			{
				if (pJSDObjectDefinition->hasProperty(vector<string>{ PROPERTIES_PROP },
					pJSONParentProperty->getName()))
				{
					pJSDObjectDefinition = pJSDObjectDefinition->getProperty(vector<string>{ PROPERTIES_PROP },
						pJSONParentProperty->getName())->getValue()->as<_object>();
				}
				else if (pJSDObjectDefinition->hasProperty(ADDITIONAL_PROPERTIES_PROP) &&
					(pJSDObjectDefinition->getProperty(ADDITIONAL_PROPERTIES_PROP)->getValue()->getType() == enumValueType::object))
				{
					pJSDObjectDefinition = pJSDObjectDefinition->getProperty(ADDITIONAL_PROPERTIES_PROP)->getValue()->as<_object>();
				}				
			}
		}
		else
		{
			VERIFY_STLOBJ_IS_EMPTY(vecJSONParentProperties);
		}

		return pJSDObjectDefinition;
	}

	const _object* _infoset::getObjectDefinition(const _property* pJSONProperty, vector<const _property*>& vecJSONParentProperties) const
	{
		VERIFY_POINTER(pJSONProperty);
		VERIFY_POINTER(pJSONProperty->getParent());

		// parent object => 'type' property
		if (pJSONProperty->getParent()->hasProperty(TYPE_PROP))
		{
			string strJSONObjectType = pJSONProperty->getParent()->getPropertyValueAs<_simple>(TYPE_PROP)->getValue();

			auto itJSDObjectDefinition = m_mapObjectDefinitions.find(strJSONObjectType);
			if (itJSDObjectDefinition != m_mapObjectDefinitions.end())
			{
				return itJSDObjectDefinition->second;
			}
		}

		// parent object => parent property
		auto pJSONParentProperty = pJSONProperty->getParent()->getParent();
		VERIFY_POINTER(pJSONParentProperty);

		auto itJSDObjectDefinition = m_mapObjectDefinitions.find(pJSONParentProperty->getName());
		if (itJSDObjectDefinition != m_mapObjectDefinitions.end())
		{
			return itJSDObjectDefinition->second;
		}

		vecJSONParentProperties.insert(vecJSONParentProperties.begin(), pJSONParentProperty);

		return getObjectDefinition(pJSONParentProperty, vecJSONParentProperties);
	}

	const _object* _infoset::getPropertyDefinition(const _object* pJSDObjectDefinition, const _property* pJSONProperty) const
	{
		VERIFY_POINTER(pJSDObjectDefinition);
		VERIFY_POINTER(pJSONProperty);

		if (pJSDObjectDefinition->hasProperty(vector<string>{ PROPERTIES_PROP }, pJSONProperty->getName()))
		{
			auto pJSDProperty = pJSDObjectDefinition->getProperty(vector<string>{ PROPERTIES_PROP }, pJSONProperty->getName());

			return pJSDProperty->getValue()->as<_object>();
		}
		else if (pJSDObjectDefinition->hasProperty(ITEMS_PROP))
		{
			auto pJSDObject = pJSDObjectDefinition->getPropertyValueAs<_object>(ITEMS_PROP);
			if (pJSDObject->hasProperty(REFERENCE_PROP))
			{
				string strReference = pJSDObject->getPropertyValueAs<_simple>(REFERENCE_PROP)->getValue();
				VERIFY_STLOBJ_IS_NOT_EMPTY(strReference);

				vector<string> vecTokens;
				_string::split(strReference, CHR2STR(SLASH), vecTokens);

				if (vecTokens.size() != 2)
				{
					THROW_INTERNAL_ERROR();
				}

				auto itJSDObjectDefinition = m_mapObjectDefinitions.find(vecTokens[1]);
				if (itJSDObjectDefinition == m_mapObjectDefinitions.end())
				{
					THROW_INTERNAL_ERROR();
				}

				auto pJSDPropertyDefinition = getPropertyDefinition(itJSDObjectDefinition->second, pJSONProperty);
				if (pJSDPropertyDefinition != nullptr)
				{
					return pJSDPropertyDefinition;
				}
			}
			else
			{
				THROW_INTERNAL_ERROR();
			}
		}
		else if (pJSDObjectDefinition->hasProperty(ALL_OF_PROP))
		{
			auto pJSDAllOfArray = pJSDObjectDefinition->getPropertyValueAs<_array>(ALL_OF_PROP);
			VERIFY_POINTER(pJSDAllOfArray);

			for (auto pJSDValue : pJSDAllOfArray->getValues())
			{
				auto pJSDObject = pJSDValue->as<_object>();
				VERIFY_POINTER(pJSDObject);

				if (pJSDObject->hasProperty(REFERENCE_PROP))
				{
					string strReference = pJSDObject->getPropertyValueAs<_simple>(REFERENCE_PROP)->getValue();
					VERIFY_STLOBJ_IS_NOT_EMPTY(strReference);

					auto pJSDReferenceObjectDefinition = getPropertyDefinitionByReference(strReference);
					if (pJSDReferenceObjectDefinition == nullptr)
					{
						THROW_INTERNAL_ERROR();
					}

					auto pJSDPropertyDefinition = getPropertyDefinition(pJSDReferenceObjectDefinition, pJSONProperty);
					if (pJSDPropertyDefinition != nullptr)
					{
						return pJSDPropertyDefinition;
					}
				}
				else if (pJSDObject->hasProperty(vector<string>{ PROPERTIES_PROP }, pJSONProperty->getName()))
				{
					return pJSDObject->getProperty(vector<string>{ PROPERTIES_PROP }, pJSONProperty->getName())->getValue()->as<_object>();
				}
				else
				{
					THROW_INTERNAL_ERROR();
				}
			} // for (auto pJSDValue ...
		}
		else if (pJSDObjectDefinition->hasProperty(ONE_OF_PROP))
		{
			auto pJSDOneOfArray = pJSDObjectDefinition->getPropertyValueAs<_array>(ONE_OF_PROP);
			VERIFY_POINTER(pJSDOneOfArray);

			for (auto pJSDValue : pJSDOneOfArray->getValues())
			{
				auto pJSDObject = pJSDValue->as<_object>();
				VERIFY_POINTER(pJSDObject);

				if (pJSDObject->hasProperty(REFERENCE_PROP))
				{
					string strReference = pJSDObject->getPropertyValueAs<_simple>(REFERENCE_PROP)->getValue();
					VERIFY_STLOBJ_IS_NOT_EMPTY(strReference);

					auto pJSDReferenceObjectDefinition = getPropertyDefinitionByReference(strReference);
					if (pJSDReferenceObjectDefinition == nullptr)
					{
						THROW_INTERNAL_ERROR();
					}

					auto pJSDPropertyDefinition = getPropertyDefinition(pJSDReferenceObjectDefinition, pJSONProperty);
					if (pJSDPropertyDefinition != nullptr)
					{
						return pJSDPropertyDefinition;
					}
				}
				else if (pJSDObject->hasProperty(vector<string>{ PROPERTIES_PROP }, pJSONProperty->getName()))
				{
					return pJSDObject->getProperty(vector<string>{ PROPERTIES_PROP }, pJSONProperty->getName())->getValue()->as<_object>();
				}
				else
				{
					THROW_INTERNAL_ERROR();
				}
			} // for (auto pJSDValue ...
		}

		return nullptr;
	}

	const _object* _infoset::getPropertyDefinitionByReference(const string& strReference) const
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(strReference);

		vector<string> vecReferencePath;
		_string::split(strReference, CHR2STR(SLASH), vecReferencePath);

		if (vecReferencePath.size() != 2)
		{
			THROW_INTERNAL_ERROR();
		}

		auto itJSDObjectDefinition = m_mapObjectDefinitions.find(vecReferencePath[1]);
		if (itJSDObjectDefinition != m_mapObjectDefinitions.end())
		{
			return itJSDObjectDefinition->second;
		}

		return nullptr;
	}

	void _infoset::getTypes(const _object* pJSDPropertyDefinition, vector<string>& vecTypes) const
	{
		VERIFY_POINTER(pJSDPropertyDefinition);

		vecTypes.clear();

		auto pJSDTypeProperty = pJSDPropertyDefinition->getProperty(TYPE_PROP);
		if (pJSDTypeProperty->getValue()->getType() == enumValueType::simple)
		{
			vecTypes.push_back(pJSDPropertyDefinition->getPropertyValueAs<_simple>(TYPE_PROP)->getValue());
		}
		else if (pJSDTypeProperty->getValue()->getType() == enumValueType::array)
		{
			auto pJSDTypesArray = pJSDPropertyDefinition->getPropertyValueAs<_array>(TYPE_PROP);
			for (auto pJSDTypeValue : pJSDTypesArray->getValues())
			{
				vecTypes.push_back(pJSDTypeValue->as<_simple>()->getValue());
			}
		}
		else
		{
			THROW_INTERNAL_ERROR();
		}
	}
};