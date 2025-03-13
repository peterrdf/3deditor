#include "_host.h"
#include "_rdf_property.h"

// ************************************************************************************************
_rdf_property::_rdf_property(RdfProperty rdfProperty)
	: m_rdfProperty(rdfProperty)
{}

/*virtual*/ _rdf_property::~_rdf_property()
{}

/*static*/ wstring _rdf_property::getRange(RdfProperty rdfProperty, vector<OwlClass>& vecRestrictionClasses)
{
	wstring strRange = L"unknown";
	vecRestrictionClasses.clear();

	switch (getType(rdfProperty))
	{
		case OBJECTTYPEPROPERTY_TYPE:
		{
			strRange = L"xsd:object";

			int64_t iRestrictionClassInstance = GetRangeRestrictionsByIterator(rdfProperty, 0);
			while (iRestrictionClassInstance != 0)
			{
				vecRestrictionClasses.push_back(iRestrictionClassInstance);				

				iRestrictionClassInstance = GetRangeRestrictionsByIterator(rdfProperty, iRestrictionClassInstance);
			}
		}
		break;

		case DATATYPEPROPERTY_TYPE_BOOLEAN:
		{
			strRange = L"xsd:boolean";
		}
		break;

		case DATATYPEPROPERTY_TYPE_STRING:
		{
			strRange = L"xsd:string";
		}
		break;

		case DATATYPEPROPERTY_TYPE_INTEGER:
		{
			strRange = L"xsd:integer";
		}
		break;

		case DATATYPEPROPERTY_TYPE_DOUBLE:
		{
			strRange = L"xsd:double";
		}
		break;

		case DATATYPEPROPERTY_TYPE_BYTE:
		{
			strRange = L"xsd:integer";
		}
		break;

		default:
		{
			ASSERT(false);
		}
		break;
	} // switch (getType())

	return strRange;
}

/*static*/ wstring _rdf_property::getCardinality(OwlInstance owlInstance, RdfProperty rdfProperty)
{
	ASSERT(owlInstance != 0);
	ASSERT(rdfProperty != 0);

	int64_t iCard = 0;
	switch (getType(rdfProperty))
	{
		case OBJECTTYPEPROPERTY_TYPE:
		{
			int64_t* piInstances = nullptr;
			GetObjectProperty(owlInstance, rdfProperty, &piInstances, &iCard);
		}
		break;

		case DATATYPEPROPERTY_TYPE_BOOLEAN:
		{
			bool* pbValue = nullptr;
			GetDatatypeProperty(owlInstance, rdfProperty, (void**)&pbValue, &iCard);
		}
		break;

		case DATATYPEPROPERTY_TYPE_STRING:
		{
			char** szValue = nullptr;
			GetDatatypeProperty(owlInstance, rdfProperty, (void**)&szValue, &iCard);
		}
		break;

		case DATATYPEPROPERTY_TYPE_INTEGER:
		{
			int64_t* piValue = nullptr;
			GetDatatypeProperty(owlInstance, rdfProperty, (void**)&piValue, &iCard);
		}
		break;

		case DATATYPEPROPERTY_TYPE_DOUBLE:
		{
			double* pdValue = nullptr;
			GetDatatypeProperty(owlInstance, rdfProperty, (void**)&pdValue, &iCard);
		}
		break;

		case DATATYPEPROPERTY_TYPE_BYTE:
		{
			BYTE* piValue = nullptr;
			GetDatatypeProperty(owlInstance, rdfProperty, (void**)&piValue, &iCard);
		}
		break;	

		default:
		{
			ASSERT(false);
		}
		break;
	} // switch (getType(rdfProperty))

	OwlClass iInstanceClass = GetInstanceClass(owlInstance);
	ASSERT(iInstanceClass != 0);

	int64_t	iMinCard = 0;
	int64_t iMaxCard = 0;
	GetClassPropertyAggregatedCardinalityRestriction(iInstanceClass, rdfProperty, &iMinCard, &iMaxCard);

	wchar_t szBuffer[100];
	if ((iMinCard == -1) && (iMaxCard == -1))
	{
		swprintf(szBuffer, 100, L"%lld of [0 - infinity>", iCard);
	}
	else
	{
		if (iMaxCard == -1)
		{
			swprintf(szBuffer, 100, L"%lld of [%lld - infinity>", iCard, iMinCard);
		}
		else
		{
			swprintf(szBuffer, 100, L"%lld of [%lld - %lld]", iCard, iMinCard, iMaxCard);
		}
	}

	return szBuffer;
}

/*static*/ wstring _rdf_property::getTypeName(RdfPropertyType rdfPropertyType)
{
	wstring strTypeName = rdfPropertyType == OBJECTPROPERTY_TYPE ?
		L"owl:ObjectProperty" :
		L"owl:DatatypeProperty";

	return strTypeName;
}

wchar_t* _rdf_property::getName() const
{
	wchar_t* szName = nullptr;
	GetNameOfPropertyW(m_rdfProperty, &szName);

	return szName;
}

// ************************************************************************************************
_rdf_property_collection::_rdf_property_collection()
	: m_vecProperties()
{}

/*virtual*/ _rdf_property_collection::~_rdf_property_collection()
{
	for (auto pProperty : m_vecProperties)
	{
		delete pProperty;
	}
}

// ************************************************************************************************
_rdf_property_provider::_rdf_property_provider()
	: m_mapPropertyCollections()
{}

/*virtual*/ _rdf_property_provider::~_rdf_property_provider()
{
	for (auto itPropertyCollection : m_mapPropertyCollections)
	{
		delete itPropertyCollection.second;
	}
}

_rdf_property_collection* _rdf_property_provider::getPropertyCollection(OwlInstance owlInstance)
{
	if (owlInstance == 0)
	{
		ASSERT(FALSE);

		return nullptr;
	}

	auto itPropertyCollection = m_mapPropertyCollections.find(owlInstance);
	if (itPropertyCollection != m_mapPropertyCollections.end())
	{
		return itPropertyCollection->second;
	}

	auto pPropertyCollection = loadPropertyCollection(owlInstance);
	m_mapPropertyCollections[owlInstance] = pPropertyCollection;

	return pPropertyCollection;
}

_rdf_property_collection* _rdf_property_provider::loadPropertyCollection(OwlInstance owlInstance)
{
	if (owlInstance == 0)
	{
		ASSERT(FALSE);

		return nullptr;
	}

	auto propertyCollection = new _rdf_property_collection();

	RdfProperty rdfProperty = GetInstancePropertyByIterator(owlInstance, 0);
	while (rdfProperty != 0)
	{
		propertyCollection->properties().push_back(new _rdf_property(rdfProperty));

		rdfProperty = GetInstancePropertyByIterator(owlInstance, rdfProperty);
	}

	return propertyCollection;
}

// ************************************************************************************************
_rdf_property_t::_rdf_property_t(RdfProperty iInstance, int iType)
	: _rdf_property(iInstance)
	, m_iInstance(iInstance)
	, m_szName(nullptr)
	, m_iType(iType)
{
	assert(m_iInstance != 0);

	GetNameOfPropertyW(m_iInstance, &m_szName);
}

/*virtual*/ _rdf_property_t::~_rdf_property_t()
{
}

wstring _rdf_property_t::GetTypeAsString() const
{
	wstring strType = L"unknown";

	switch (GetType())
	{
	case OBJECTPROPERTY_TYPE:
	{
		strType = L"owl:ObjectProperty";
	}
	break;

	case DATATYPEPROPERTY_TYPE_BOOLEAN:
	{
		strType = L"owl:DatatypeProperty";
	}
	break;

	case DATATYPEPROPERTY_TYPE_STRING:
	{
		strType = L"owl:DatatypeProperty";
	}
	break;

	case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
	{
		strType = L"owl:DatatypeProperty";
	}
	break;

	case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
	{
		strType = L"owl:DatatypeProperty";
	}
	break;

	case DATATYPEPROPERTY_TYPE_DOUBLE:
	{
		strType = L"owl:DatatypeProperty";
	}
	break;

	case DATATYPEPROPERTY_TYPE_INTEGER:
	{
		strType = L"owl:DatatypeProperty";
	}
	break;

	default:
	{
		assert(false); // Not supported!
	}
	break;
	} // switch (getType())

	return strType;
}

wstring _rdf_property_t::GetRange() const
{
	wstring strRange = L"unknown";

	switch (GetType())
	{
	case OBJECTPROPERTY_TYPE:
	{
		strRange = L"xsd:object";
	}
	break;

	case DATATYPEPROPERTY_TYPE_BOOLEAN:
	{
		strRange = L"xsd:boolean";
	}
	break;

	case DATATYPEPROPERTY_TYPE_STRING:
	{
		strRange = L"xsd:string";
	}
	break;

	case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
	{
		strRange = L"xsd:string";
	}
	break;

	case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
	{
		strRange = L"xsd:string";
	}
	break;

	case DATATYPEPROPERTY_TYPE_DOUBLE:
	{
		strRange = L"xsd:double";
	}
	break;

	case DATATYPEPROPERTY_TYPE_INTEGER:
	{
		strRange = L"xsd:integer";
	}
	break;

	default:
	{
		assert(false); // Not supported!
	}
	break;
	} // switch (getType())

	return strRange;
}

wstring _rdf_property_t::GetCardinality(OwlInstance iInstance) const
{
	assert(iInstance != 0);

	int64_t iCard = 0;

	switch (GetType())
	{
	case OBJECTPROPERTY_TYPE:
	{
		int64_t* piInstances = nullptr;
		GetObjectProperty(iInstance, GetInstance(), &piInstances, &iCard);
	}
	break;

	case DATATYPEPROPERTY_TYPE_BOOLEAN:
	{
		bool* pbValue = nullptr;
		GetDatatypeProperty(iInstance, GetInstance(), (void**)&pbValue, &iCard);
	}
	break;

	case DATATYPEPROPERTY_TYPE_STRING:
	{
		wchar_t** szValue = nullptr;
		SetCharacterSerialization(GetModel(iInstance), 0, 0, false);
		GetDatatypeProperty(iInstance, GetInstance(), (void**)&szValue, &iCard);
		SetCharacterSerialization(GetModel(iInstance), 0, 0, true);
	}
	break;

	case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
	{
		char** szValue = nullptr;
		GetDatatypeProperty(iInstance, GetInstance(), (void**)&szValue, &iCard);
	}
	break;

	case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
	{
		wchar_t** szValue = nullptr;
		GetDatatypeProperty(iInstance, GetInstance(), (void**)&szValue, &iCard);
	}
	break;

	case DATATYPEPROPERTY_TYPE_DOUBLE:
	{
		double* pdValue = nullptr;
		GetDatatypeProperty(iInstance, GetInstance(), (void**)&pdValue, &iCard);
	}
	break;

	case DATATYPEPROPERTY_TYPE_INTEGER:
	{
		int64_t* piValue = nullptr;
		GetDatatypeProperty(iInstance, GetInstance(), (void**)&piValue, &iCard);
	}
	break;

	default:
	{
		assert(false);
	}
	break;
	} // switch (getType())

	int64_t	iMinCard = 0;
	int64_t iMaxCard = 0;
	GetRestrictions(iInstance, iMinCard, iMaxCard);

	wchar_t szBuffer[256];

	if ((iMinCard == -1) && (iMaxCard == -1))
	{
		swprintf(szBuffer, 256, L"%lld of [0 - infinity>", iCard);
	}
	else
	{
		if (iMaxCard == -1)
		{
			swprintf(szBuffer, 256, L"%lld of [%lld - infinity>", iCard, iMinCard);
		}
		else
		{
			swprintf(szBuffer, 256, L"%lld of [%lld - %lld]", iCard, iMinCard, iMaxCard);
		}
	}

	return szBuffer;
}

void _rdf_property_t::GetRestrictions(OwlInstance iInstance, int64_t& iMinCard, int64_t& iMaxCard) const
{
	assert(iInstance != 0);

	OwlClass iClassInstance = GetInstanceClass(iInstance);
	assert(iClassInstance != 0);

	iMinCard = -1;
	iMaxCard = -1;
	GetClassPropertyAggregatedCardinalityRestriction(iClassInstance, m_iInstance, &iMinCard, &iMaxCard);
}

// ************************************************************************************************
_double_rdf_property::_double_rdf_property(RdfProperty iInstance)
	: _rdf_property_t(iInstance, DATATYPEPROPERTY_TYPE_DOUBLE)
{
}

/*virtual*/ _double_rdf_property::~_double_rdf_property()
{
}

// ************************************************************************************************
_integer_rdf_property::_integer_rdf_property(RdfProperty iInstance)
	: _rdf_property_t(iInstance, DATATYPEPROPERTY_TYPE_INTEGER)
{
}

_integer_rdf_property::~_integer_rdf_property()
{
}

// ************************************************************************************************
_bool_rdf_property::_bool_rdf_property(RdfProperty iInstance)
	: _rdf_property_t(iInstance, DATATYPEPROPERTY_TYPE_BOOLEAN)
{
}

/*virtual*/ _bool_rdf_property::~_bool_rdf_property()
{
}

// ************************************************************************************************
_string_rdf_property::_string_rdf_property(RdfProperty iInstance)
	: _rdf_property_t(iInstance, DATATYPEPROPERTY_TYPE_STRING)
{
}

_string_rdf_property::~_string_rdf_property()
{
}

// ************************************************************************************************
CCharArrayRDFProperty::CCharArrayRDFProperty(RdfProperty iInstance)
	: _rdf_property_t(iInstance, DATATYPEPROPERTY_TYPE_CHAR_ARRAY)
{
}

// ************************************************************************************************
CCharArrayRDFProperty::~CCharArrayRDFProperty()
{
}

// ************************************************************************************************
CWCharArrayRDFProperty::CWCharArrayRDFProperty(RdfProperty iInstance)
	: _rdf_property_t(iInstance, DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY)
{
}

CWCharArrayRDFProperty::~CWCharArrayRDFProperty()
{
}

// ************************************************************************************************
CObjectRDFProperty::CObjectRDFProperty(RdfProperty iInstance)
	: _rdf_property_t(iInstance, OBJECTPROPERTY_TYPE)
	, m_vecRestrictions()
{
	int64_t	iRestrictionsClassInstance = GetRangeRestrictionsByIterator(GetInstance(), 0);
	while (iRestrictionsClassInstance != 0)
	{
		m_vecRestrictions.push_back(iRestrictionsClassInstance);

		iRestrictionsClassInstance = GetRangeRestrictionsByIterator(GetInstance(), iRestrictionsClassInstance);
	}
}

CObjectRDFProperty::~CObjectRDFProperty()
{
}

// ************************************************************************************************
CUndefinedRDFProperty::CUndefinedRDFProperty(RdfProperty iInstance)
	: _rdf_property_t(iInstance, -1)
{
}

CUndefinedRDFProperty::~CUndefinedRDFProperty()
{
}
