#include "_host.h"
#include "_ifc_property.h"
#include "_ifc_unit.h"

// ************************************************************************************************
_ifc_property::_ifc_property(const wstring& strName, const wstring& strValue)
	: m_strName(strName)
	, m_strValue(strValue)
{}

/*virtual*/ _ifc_property::~_ifc_property()
{}

/*static*/ bool _ifc_property::hasProperties(SdaiModel sdaiModel, SdaiInstance sdaiInstance)
{
	assert(sdaiModel != 0);
	assert(sdaiInstance != 0);

	SdaiAggr sdaiIsDefinedByAggr = 0;
	sdaiGetAttrBN(sdaiInstance, "IsDefinedBy", sdaiAGGR, &sdaiIsDefinedByAggr);

	if (sdaiIsDefinedByAggr == nullptr)
	{
		return false;
	}

	SdaiInteger iMembersCount = sdaiGetMemberCount(sdaiIsDefinedByAggr);
	for (SdaiInteger iMember = 0; iMember < iMembersCount; iMember++)
	{
		SdaiInstance sdaiAggrInstance = 0;
		engiGetAggrElement(sdaiIsDefinedByAggr, iMember, sdaiINSTANCE, &sdaiAggrInstance);

		if ((sdaiGetInstanceType(sdaiAggrInstance) == sdaiGetEntity(sdaiModel, "IFCRELDEFINESBYPROPERTIES")) ||
			(sdaiGetInstanceType(sdaiAggrInstance) == sdaiGetEntity(sdaiModel, "IFCRELDEFINESBYTYPE")))
		{
			return true;
		}
	} // for (SdaiInteger iMember = ...

	return	false;
}

/*static*/ wstring _ifc_property::getPropertySingleValue(SdaiInstance sdaiPropertySingleValueInstance)
{
	assert(sdaiPropertySingleValueInstance != 0);

	wchar_t* szNominalValueADB = nullptr;
	sdaiGetAttrBN(sdaiPropertySingleValueInstance, "NominalValue", sdaiUNICODE, &szNominalValueADB);

	if (szNominalValueADB == nullptr)
	{
		return L"";
	}

	wchar_t* szUnitADB = nullptr;
	sdaiGetAttrBN(sdaiPropertySingleValueInstance, "Unit", sdaiUNICODE, &szUnitADB);

	wchar_t* szTypePath = (wchar_t*)sdaiGetADBTypePath(szNominalValueADB, 0);
	if (szTypePath == nullptr)
	{
		return szNominalValueADB;
	}

	return L"";
}

// ************************************************************************************************
_ifc_property_set::_ifc_property_set(const wstring& strName)
	: m_strName(strName)
	, m_vecProperties()
{
	assert(!m_strName.empty());
}

/*virtual*/ _ifc_property_set::~_ifc_property_set()
{
	for (auto pProperty : m_vecProperties)
	{
		delete pProperty;
	}
}

_ifc_property_set_collection::_ifc_property_set_collection()
	: m_vecPropertySets()
{
}

/*virtual*/ _ifc_property_set_collection::~_ifc_property_set_collection()
{
	for (auto pPropertySet : m_vecPropertySets)
	{
		delete pPropertySet;
	}
}

// ************************************************************************************************
_ifc_property_provider::_ifc_property_provider(SdaiModel sdaiModel, _ifc_unit_provider* pUnitProvider)
	: m_sdaiModel(sdaiModel)
	, m_pUnitProvider(pUnitProvider)
	, m_mapPropertyCollections()
{	
	assert(m_sdaiModel != 0);
	assert(m_pUnitProvider != nullptr);
}

/*virtual*/ _ifc_property_provider::~_ifc_property_provider()
{
	for (auto itPropertyCollection : m_mapPropertyCollections)
	{
		delete itPropertyCollection.second;
	}
}

_ifc_property_set_collection* _ifc_property_provider::getPropertySetCollection(SdaiInstance sdaiInstance)
{
	if (sdaiInstance == 0)
	{
		assert(FALSE);

		return nullptr;
	}

	auto itPropertyCollection = m_mapPropertyCollections.find(sdaiInstance);
	if (itPropertyCollection != m_mapPropertyCollections.end())
	{
		return itPropertyCollection->second;
	}

	auto pPropertyCollection = loadPropertyCollection(sdaiInstance);
	m_mapPropertyCollections[sdaiInstance] = pPropertyCollection;

	return pPropertyCollection;
}

_ifc_property_set_collection* _ifc_property_provider::loadPropertyCollection(SdaiInstance sdaiInstance)
{
	auto pPropertySetCollection = new _ifc_property_set_collection();
	loadProperties(sdaiInstance, pPropertySetCollection);

	return pPropertySetCollection;
}

void _ifc_property_provider::loadProperties(SdaiInstance sdaiInstance, _ifc_property_set_collection* pPropertySetCollection)
{
	SdaiAggr sdaiIsDefinedByAggr = nullptr;
	sdaiGetAttrBN(sdaiInstance, "IsDefinedBy", sdaiAGGR, &sdaiIsDefinedByAggr);

	if (sdaiIsDefinedByAggr == nullptr)
	{
		return;
	}

	SdaiEntity sdaiRelDefinesByTypeEntity = sdaiGetEntity(m_sdaiModel, "IFCRELDEFINESBYTYPE");
	SdaiEntity sdaiRelDefinesByPropertiesEntity = sdaiGetEntity(m_sdaiModel, "IFCRELDEFINESBYPROPERTIES");

	SdaiInteger iMembersCount = sdaiGetMemberCount(sdaiIsDefinedByAggr);
	for (SdaiInteger iMember = 0; iMember < iMembersCount; iMember++)
	{
		SdaiInstance sdaiAggrInstance = 0;
		engiGetAggrElement(sdaiIsDefinedByAggr, iMember, sdaiINSTANCE, &sdaiAggrInstance);

		if (sdaiGetInstanceType(sdaiAggrInstance) == sdaiRelDefinesByPropertiesEntity)
		{
			loadRelDefinesByProperties(sdaiAggrInstance, pPropertySetCollection);
		}
		else
		{
			if (sdaiGetInstanceType(sdaiAggrInstance) == sdaiRelDefinesByTypeEntity)
			{
				loadRelDefinesByType(sdaiAggrInstance, pPropertySetCollection);
			}
		}
	} // for (SdaiInteger iMember = ...
}

void _ifc_property_provider::loadRelDefinesByProperties(SdaiInstance sdaiIsDefinedByPropertiesInstance, _ifc_property_set_collection* pPropertySetCollection)
{
	assert(sdaiIsDefinedByPropertiesInstance != 0);

	SdaiEntity sdaiElementQuantityEntity = sdaiGetEntity(m_sdaiModel, "IFCELEMENTQUANTITY");
	SdaiEntity sdaiPropertySetEntity = sdaiGetEntity(m_sdaiModel, "IFCPROPERTYSET");

	SdaiInstance sdaiRelatingPropertyDefinitionInstance = 0;
	sdaiGetAttrBN(sdaiIsDefinedByPropertiesInstance, "RelatingPropertyDefinition", sdaiINSTANCE, &sdaiRelatingPropertyDefinitionInstance);

	if (sdaiGetInstanceType(sdaiRelatingPropertyDefinitionInstance) == sdaiElementQuantityEntity)
	{
		loadQuantites(sdaiRelatingPropertyDefinitionInstance, pPropertySetCollection);
	}
	else
	{
		if (sdaiGetInstanceType(sdaiRelatingPropertyDefinitionInstance) == sdaiPropertySetEntity)
		{
			loadPropertySet(sdaiRelatingPropertyDefinitionInstance, pPropertySetCollection);
		}
	}
}

void _ifc_property_provider::loadPropertySet(SdaiInstance sdaiPropertySetInstance, _ifc_property_set_collection* pPropertySetCollection)
{
	assert(sdaiPropertySetInstance != 0);

	/*
	* Property set
	*/
	SdaiAggr sdaiHasPropertiesAggr = nullptr;
	sdaiGetAttrBN(sdaiPropertySetInstance, "HasProperties", sdaiAGGR, &sdaiHasPropertiesAggr);

	if (sdaiHasPropertiesAggr == nullptr)
	{
		return;
	}

	wstring strItem = getName(sdaiPropertySetInstance);

	auto pPropertySet = new _ifc_property_set(strItem);

	SdaiEntity iIFCPropertySingleValueEntity = sdaiGetEntity(m_sdaiModel, "IFCPROPERTYSINGLEVALUE");

	SdaiInteger iMembersCount = sdaiGetMemberCount(sdaiHasPropertiesAggr);
	for (SdaiInteger iMember = 0; iMember < iMembersCount; iMember++)
	{
		SdaiInstance sdaiAggrInstance = 0;
		engiGetAggrElement(sdaiHasPropertiesAggr, iMember, sdaiINSTANCE, &sdaiAggrInstance);

		strItem = getName(sdaiAggrInstance);

		wstring strValue;
		if (sdaiGetInstanceType(sdaiAggrInstance) == iIFCPropertySingleValueEntity)
		{
			strValue = _ifc_property::getPropertySingleValue(sdaiAggrInstance);
			if (strValue.empty())
			{
				strValue = L"<empty>";
			}
		}

		auto pProperty = new _ifc_property(strItem, strValue);
		pPropertySet->properties().push_back(pProperty);
	} // for (SdaiInteger iMember = ...

	pPropertySetCollection->propertySets().push_back(pPropertySet);
}

void _ifc_property_provider::loadRelDefinesByType(SdaiInstance sdaiRelDefinesByTypeInstance, _ifc_property_set_collection* pPropertySetCollection)
{
	assert(sdaiRelDefinesByTypeInstance != 0);

	SdaiInstance sdaiRelatingTypeInstance = 0;
	sdaiGetAttrBN(sdaiRelDefinesByTypeInstance, "RelatingType", sdaiINSTANCE, &sdaiRelatingTypeInstance);

	if (sdaiRelatingTypeInstance == 0)
	{
		return;
	}

	SdaiEntity sdaiElementQuantityEntity = sdaiGetEntity(m_sdaiModel, "IFCELEMENTQUANTITY");
	SdaiEntity sdaiPropertySetEntity = sdaiGetEntity(m_sdaiModel, "IFCPROPERTYSET");

	SdaiAggr sdaiHasPropertySetsAggr = nullptr;
	sdaiGetAttrBN(sdaiRelatingTypeInstance, "HasPropertySets", sdaiAGGR, &sdaiHasPropertySetsAggr);

	SdaiInteger iMembersCount = sdaiGetMemberCount(sdaiHasPropertySetsAggr);
	for (SdaiInteger iMember = 0; iMember < iMembersCount; iMember++)
	{
		SdaiInstance sdaiAggrInstance = 0;
		engiGetAggrElement(sdaiHasPropertySetsAggr, iMember, sdaiINSTANCE, &sdaiAggrInstance);
		if (sdaiGetInstanceType(sdaiAggrInstance) == sdaiElementQuantityEntity)
		{
			loadQuantites(sdaiAggrInstance, pPropertySetCollection);
		}
		else if (sdaiGetInstanceType(sdaiAggrInstance) == sdaiPropertySetEntity)
		{
			loadPropertySet(sdaiAggrInstance, pPropertySetCollection);
		}
	} // for (SdaiInteger iMember = ...
}

void _ifc_property_provider::loadQuantites(SdaiInstance sdaiElementQuantityInstance, _ifc_property_set_collection* pPropertySetCollection)
{
	assert(sdaiElementQuantityInstance != 0);

	wstring strItem = getName(sdaiElementQuantityInstance);

	/*
	* Property set
	*/
	auto pPropertySet = new _ifc_property_set(strItem);

	SdaiAggr sdaiQuantitiesAggr = nullptr;
	sdaiGetAttrBN(sdaiElementQuantityInstance, "Quantities", sdaiAGGR, &sdaiQuantitiesAggr);

	SdaiInteger iMembersCount = sdaiGetMemberCount(sdaiQuantitiesAggr);
	for (SdaiInteger iMember = 0; iMember < iMembersCount; iMember++)
	{
		SdaiInstance sdaiAggrInstance = 0;
		engiGetAggrElement(sdaiQuantitiesAggr, iMember, sdaiINSTANCE, &sdaiAggrInstance);

		if (sdaiGetInstanceType(sdaiAggrInstance) == sdaiGetEntity(m_sdaiModel, "IFCQUANTITYLENGTH"))
		{
			loadIFCQuantityLength(sdaiAggrInstance, pPropertySet);
		}
		else if (sdaiGetInstanceType(sdaiAggrInstance) == sdaiGetEntity(m_sdaiModel, "IFCQUANTITYAREA"))
		{
			loadIFCQuantityArea(sdaiAggrInstance, pPropertySet);
		}
		else if (sdaiGetInstanceType(sdaiAggrInstance) == sdaiGetEntity(m_sdaiModel, "IFCQUANTITYVOLUME"))
		{
			loadIFCQuantityVolume(sdaiAggrInstance, pPropertySet);
		}
		else if (sdaiGetInstanceType(sdaiAggrInstance) == sdaiGetEntity(m_sdaiModel, "IFCQUANTITYCOUNT"))
		{
			loadIFCQuantityCount(sdaiAggrInstance, pPropertySet);
		} 
		else if (sdaiGetInstanceType(sdaiAggrInstance) == sdaiGetEntity(m_sdaiModel, "IFCQUANTITYWEIGHT"))
		{
			loadIFCQuantityWeight(sdaiAggrInstance, pPropertySet);
		}
		else if (sdaiGetInstanceType(sdaiAggrInstance) == sdaiGetEntity(m_sdaiModel, "IFCQUANTITYTIME"))
		{
			loadIFCQuantityTime(sdaiAggrInstance, pPropertySet);
		}
		else
		{
			assert(FALSE); // TODO
		}
	} // for (SdaiInteger iMember = ...

	pPropertySetCollection->propertySets().push_back(pPropertySet);
}

void _ifc_property_provider::loadIFCQuantityLength(SdaiInstance sdaiQuantityInstance, _ifc_property_set* pPropertySet)
{
	auto prProperty = m_pUnitProvider->getQuantityLength(sdaiQuantityInstance);

	pPropertySet->properties().push_back(new _ifc_property(prProperty.first, prProperty.second));
}

void _ifc_property_provider::loadIFCQuantityArea(SdaiInstance sdaiQuantityInstance, _ifc_property_set* pPropertySet)
{
	auto prProperty = m_pUnitProvider->getQuantityArea(sdaiQuantityInstance);

	pPropertySet->properties().push_back(new _ifc_property(prProperty.first, prProperty.second));
}

void _ifc_property_provider::loadIFCQuantityVolume(SdaiInstance sdaiQuantityInstance, _ifc_property_set* pPropertySet)
{
	auto prProperty = m_pUnitProvider->getQuantityVolume(sdaiQuantityInstance);

	pPropertySet->properties().push_back(new _ifc_property(prProperty.first, prProperty.second));
}

void _ifc_property_provider::loadIFCQuantityCount(SdaiInstance sdaiQuantityInstance, _ifc_property_set* pPropertySet)
{
	auto prProperty = m_pUnitProvider->getQuantityCount(sdaiQuantityInstance);

	pPropertySet->properties().push_back(new _ifc_property(prProperty.first, prProperty.second));
}

void _ifc_property_provider::loadIFCQuantityWeight(SdaiInstance sdaiQuantityInstance, _ifc_property_set* pPropertySet)
{
	auto prProperty = m_pUnitProvider->getQuantityWeight(sdaiQuantityInstance);

	pPropertySet->properties().push_back(new _ifc_property(prProperty.first, prProperty.second));
}

void _ifc_property_provider::loadIFCQuantityTime(SdaiInstance sdaiQuantityInstance, _ifc_property_set* pPropertySet)
{
	auto prProperty = m_pUnitProvider->getQuantityTime(sdaiQuantityInstance);

	pPropertySet->properties().push_back(new _ifc_property(prProperty.first, prProperty.second));
}

wstring _ifc_property_provider::getName(SdaiInstance sdaiInstance) const
{
	assert(sdaiInstance != 0);

	wchar_t* szName = nullptr;
	sdaiGetAttrBN(sdaiInstance, "Name", sdaiUNICODE, &szName);

	wchar_t* szDescription = nullptr;
	sdaiGetAttrBN(sdaiInstance, "Description", sdaiUNICODE, &szDescription);

	wstring strName;
	if ((szName != nullptr) && (wcslen(szName) > 0))
	{
		strName = szName;
	}
	else
	{
		strName = L"<empty>";
	}

	if ((szDescription != nullptr) && (wcslen(szDescription) > 0))
	{
		strName += L" (";
		strName += szDescription;
		strName += L")";
	}

	return strName;
}