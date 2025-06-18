#pragma once

#include "_reader.h"
#include "_log.h"
#include "_io.h"
#include "_serialization.h"

#include <set>
#include <map>
#include <vector>
using namespace std;

// ************************************************************************************************
namespace _json
{
	// ********************************************************************************************
	const char TYPE_PROP[] = "type";	
	const char ENUM_PROP[] = "enum";
	const char ALL_OF_PROP[] = "allOf";
	const char ONE_OF_PROP[] = "oneOf";
	const char PROPERTIES_PROP[] = "properties";
	const char ADDITIONAL_PROPERTIES_PROP[] = "additionalProperties";
	const char REQUIRED_PROP[] = "required";
	const char REFERENCE_PROP[] = "$ref";
	const char ITEMS_PROP[] = "items";

	const char OBJECT_TYPE[] = "object";
	const char STRING_TYPE[] = "string";
	const char INT_TYPE[] = "integer";
	const char NUMBER_TYPE[] = "number";
	const char BOOL_TYPE[] = "boolean";
	const char UNDEFINED_TYPE[] = "undefined";
	const char ARRAY_TYPE[] = "array";	

	const char EXTENSION_TYPE[] = "CityJSONExtension";
	const char EXTENSION_TYPE_II[] = "CityJSON_Extension";
	const char EXTRA_ROOT_PROPERTIES_PROP[] = "extraRootProperties";
	const char EXTRA_ATTRIBUTES_PROP[] = "extraAttributes";
	const char EXTRA_CITY_OBJECTS_PROP[] = "extraCityObjects";
	const char DEFINITIONS_PROP[] = "definitions";

	// ********************************************************************************************
	class _value;
	class _simple;
	class _object;
	class _array;
	class _property;
	class _document;
	class _schema;
	class _infoset;

	// ********************************************************************************************
	class _element_site
	{
		friend class _value;
		friend class _property;

	protected: // Methods

		virtual void onValueLoaded(_value* pValue) = 0;
		virtual void onPropertyLoaded(_property* pProperty) = 0;
	};

	// ********************************************************************************************
	enum class enumValueType
	{
		simple = 0, // simple type, e.g. string, integer, double, etc.		
		object, // complex type
		array, // list of simple || object
	};

	class _value
	{

	private: // Members

		_property* m_pParent;
		_element_site* m_pSite;
		enumValueType m_enType;		

	public: // Methods

		_value(_property* pParent, _element_site* pSite, enumValueType enValueType);

		// serialization
		virtual void serialize(_srln::_archive* pArchive) = 0;

		virtual ~_value();		
		
		virtual void load(_stream_reader* pReader) = 0;

		static _value* loadValue(_stream_reader* pReader, _property* pParent, _element_site* pSite);
		static void loadString(_stream_reader* pReader, string& strValue);

		_property* getParent() const { return m_pParent; }
		_element_site* getSite() const { return m_pSite; }
		enumValueType getType() const { return m_enType; }

		template <typename V>
		const V* as() const
		{
			const V* pValue = dynamic_cast<const V*>(this);
			VERIFY_POINTER(pValue);

			return pValue;
		}
	};

	// ********************************************************************************************
	class _simple : public _value
	{

	private: // Members

		string m_strValue;
		bool m_bIsString; // "[VALUE]"

	public: // Methods

		_simple(_property* pParent, _element_site* pSite);

		// serialization
		virtual void serialize(_srln::_archive* pArchive) override;
		_simple(_property* pParent, _element_site* pSite, _srln::_archive* pArchive, const string& strTag);

		virtual ~_simple();

		// _value
		virtual void load(_stream_reader* pReader) override;		

		const string& getValue() const { return m_strValue; }
		bool getIsString() const { return m_bIsString; }
	};

	// ********************************************************************************************
	class _object : public _value
	{

	private: // Members

		map<string, _property*> m_mapProperties;

	public: // Methods

		_object(_property* pParent, _element_site* pSite);

		// serialization
		virtual void serialize(_srln::_archive* pArchive) override;
		_object(_property* pParent, _element_site* pSite, _srln::_archive* pArchive, const string& strTag);

		virtual ~_object();

		// _value
		virtual void load(_stream_reader* pReader) override;		

		const map<string, _property*>& getProperties() const { return m_mapProperties; }

		bool hasProperty(const string& strProperty) const;		
		const _property* getProperty(const string& strProperty) const;
		const _value* getPropertyValue(const string& strProperty) const;
		template<typename V>
		const V* getPropertyValueAs(const string& strProperty) const
		{
			return dynamic_cast<const V*>(getPropertyValue(strProperty));
		}
		bool hasProperty(const vector<string>& vecObjectProperties, const string& strProperty) const;
		const _property* getProperty(const vector<string>& vecObjectProperties, const string& strProperty) const;
		const _value* getPropertyValue(const vector<string>& vecPath, const string& strProperty) const;
		template<typename V>
		const V* getPropertyValueAs(const vector<string>& vecPath, const string& strProperty) const
		{
			return dynamic_cast<const V*>(getPropertyValue(vecPath, strProperty));
		}
	};

	// ********************************************************************************************
	class _array : public _value
	{

	private: // Members

		vector<_value*> m_vecValues;

	public: // Methods

		_array(_property* pParent, _element_site* pSite);

		// serialization
		virtual void serialize(_srln::_archive* pArchive) override;
		_array(_property* pParent, _element_site* pSite, _srln::_archive* pArchive, const string& strTag);

		virtual ~_array();

		// _value
		virtual void load(_stream_reader* pReader) override;

		const vector<_value*>& getValues() const { return m_vecValues; }
	};	

	// ********************************************************************************************
	class _property
	{

	private: // Members

		_object* m_pParent;
		_element_site* m_pSite;
		string m_strName;
		_value* m_pValue;

	public: // Methods

		_property(_object* pParent, _element_site* pSite);

		// serialization
		virtual void serialize(_srln::_archive* pArchive);
		_property(_object* pParent, _element_site* pSite, _srln::_archive* pArchive, const string& strTag);

		virtual ~_property();

		virtual void load(_stream_reader* pReader);

		const _object* getParent() const { return m_pParent; }
		_element_site* getSite() const { return m_pSite; }
		const string& getName() const { return m_strName; }
		const _value* getValue() const { return m_pValue; }

	private: // Methods

		void loadName(_stream_reader* pReader);
	};

	// ********************************************************************************************
	class _document_site
		: public _log_client
	{
		friend class _document;

	protected: // Methods

		virtual void onValueLoaded(_value* pValue) = 0;
		virtual void onPropertyLoaded(_property* pProperty) = 0;
		virtual void loadSchemas() = 0;
		virtual _schema* loadSchema(const string& strNamespace, const string& strLocation) = 0;
	};

	// ********************************************************************************************
	class _document 
		: public _srln::_serializable
		, public _element_site
	{

	private: // Members

		_document_site* m_pSite;
		_stream_reader* m_pReader;
		_object* m_pRoot;

		map<string, map<string, _schema*>> m_mapSchemas; // namespace : (location : _schema*)
		_infoset* m_pInfoset;

		map<string, string> m_mapMetaData; // user-defined attributes

	public: // Methods

		_document(_document_site* pSite);
		virtual ~_document();

	public: // Methods

		// _serializable
		virtual void serialize(_srln::_archive* pArchive) override;
		virtual void deserialize(_srln::_archive* pArchive) override;

	protected: // Methods

		// _element_site
		virtual void onValueLoaded(_value* pValue) override;
		virtual void onPropertyLoaded(_property* pProperty) override;

	public: // Methods
		
		void load(const char* szFile);
		void load(istream* pStream);

		virtual void loadSchemas();		
		_schema* getSchema(const string& strNamespace, const string& strLocation, bool& bNew);

		void loadInfoset();

		virtual void clean();

		_document_site* getSite() const { return m_pSite; }
		_object* getRoot() const { return m_pRoot; }
		const map<string, map<string, _schema*>>& getSchemas() const { return m_mapSchemas; }
		map<string, string>& metaData() { return m_mapMetaData; }
		_infoset* getInfoset() const { return m_pInfoset; }

	protected: // Methods			
		
		virtual void load();
	};	

	// ********************************************************************************************
	class _schema : public _document		
	{

	private: // Members

		string m_strNamespace;
		string m_strLocation;

	public: // Methods

		_schema(_document_site* pSite, const string& strNamespace, const string& strLocation);
		virtual ~_schema();
	
		// _serializable
		virtual void serialize(_srln::_archive* pArchive) override;
		virtual void deserialize(_srln::_archive* pArchive) override;

		string getNamespace(bool bRemoveProtocol) const;
		string getLocation(bool bRemoveProtocol) const;
	};

	// ********************************************************************************************
	// JSON Schema Definition (JSD)
	class _infoset
	{

	private: // Members

		_document* m_pDocument;

		// file name : _schema*
		// Example: 
		// { "$ref": "geomprimitives.schema.json#/MultiPoint" }
		map<string, _schema*> m_mapSchemaReferences;

		// name : type definition (_object*)
		// Example I:
		// cityjson.schema.json/cityjsonfeature.schema.json
		// "type": "object",
		// "properties": {
		//	"type": {
		//		"enum": [
		//			"CityJSON" ...
		// Example II:
		// appearance.schema.json
		// "Material": {
		//	"type": "object",
		//    "properties": {
		//		"name": {
		//			"type": "string" ...
		map<string, const _object*> m_mapObjectDefinitions;

	public: // Methods

		_infoset(_document* pDocument);
		virtual ~_infoset();

		void load();

		const map<string, _schema*>& getSchemaReferences() const { return m_mapSchemaReferences; }
		const map<string, const _object*>& getObjectDefinitions() const { return m_mapObjectDefinitions; }

		string getObjectParentType(const string& strType) const;
		void getSimplePropertySupportedTypes(const _property* pJSONProperty, vector<string>& vecTypes);

	private: // Methods

		void loadSchema(_schema* pSchema);

		const _object* getObjectDefinition(const _property* pJSONProperty) const;
		const _object* getObjectDefinition(const _property* pJSONProperty, vector<const _property*>& vecJSONParentProperties) const;
		const _object* getPropertyDefinition(const _object* pJSDObjectDefinition, const _property* pJSONProperty) const;
		const _object* getPropertyDefinitionByReference(const string& strReference) const;
		void getTypes(const _object* pJSDPropertyDefinition, vector<string>& vecTypes) const;		
	};
};