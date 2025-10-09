#include "_host.h"
#include "_ap242_property.h"
#include "_ap_geometry.h"

#include "_string.h"

#include <cwctype>
#include <algorithm>
using namespace std;

// ************************************************************************************************
_ap242_property::_ap242_property(SdaiInstance sdaiInstance)
	: m_sdaiInstance(sdaiInstance)
	, m_strEntityName(_ap_geometry::getEntityName(sdaiInstance))
	, m_strName(L"$")
	, m_strDescription(L"$")
	, m_strValue(L"$")
	, m_strValueType(L"$")
{
	assert(m_sdaiInstance != 0);

	load();
}

/*virtual*/ _ap242_property::~_ap242_property()
{}

void _ap242_property::load()
{
	assert(m_sdaiInstance != 0);

	SdaiModel sdaiModel = sdaiGetInstanceModel(m_sdaiInstance);
	assert(sdaiModel != 0);

	wchar_t* szName = nullptr;
	sdaiGetAttrBN(m_sdaiInstance, "name", sdaiUNICODE, &szName);
	m_strName = szName != nullptr ? szName : L"$";

	wchar_t* szDescription = nullptr;
	sdaiGetAttrBN(m_sdaiInstance, "description", sdaiUNICODE, &szDescription);
	m_strDescription = szDescription != nullptr ? szDescription : L"$";

	SdaiAggr sdaiPropertyDefinitionRepresentationAggr = sdaiGetEntityExtentBN(sdaiModel, "PROPERTY_DEFINITION_REPRESENTATION");
	SdaiInteger	iPropertyDefinitionRepresentationsCount = sdaiGetMemberCount(sdaiPropertyDefinitionRepresentationAggr);
	for (SdaiInteger j = 0; j < iPropertyDefinitionRepresentationsCount; j++) {
		SdaiInstance sdaiPropertyDefinitionRepresentationInstance = 0;
		sdaiGetAggrByIndex(sdaiPropertyDefinitionRepresentationAggr, j, sdaiINSTANCE, &sdaiPropertyDefinitionRepresentationInstance);

		SdaiInstance sdaiPDRDefinitionInstance = 0;
		sdaiGetAttrBN(sdaiPropertyDefinitionRepresentationInstance, "definition", sdaiINSTANCE, &sdaiPDRDefinitionInstance);
		if (sdaiPDRDefinitionInstance == m_sdaiInstance) {
			SdaiInstance sdaiUsedRepresentationInstance = 0;
			sdaiGetAttrBN(sdaiPropertyDefinitionRepresentationInstance, "used_representation", sdaiINSTANCE, &sdaiUsedRepresentationInstance);

			SdaiAggr sdaiItemsAggr = nullptr;
			sdaiGetAttrBN(sdaiUsedRepresentationInstance, "items", sdaiAGGR, &sdaiItemsAggr);
			SdaiInteger	iItemsCount = sdaiGetMemberCount(sdaiItemsAggr);
			for (SdaiInteger k = 0; k < iItemsCount; k++) {
				SdaiInstance sdaiItemInstance = 0;
				sdaiGetAggrByIndex(sdaiItemsAggr, k, sdaiINSTANCE, &sdaiItemInstance);

				if (sdaiGetInstanceType(sdaiItemInstance) == sdaiGetEntity(sdaiModel, "DESCRIPTIVE_REPRESENTATION_ITEM")) {
					wchar_t* szDescription = nullptr;
					sdaiGetAttrBN(sdaiItemInstance, "description", sdaiUNICODE, &szDescription);

					m_strValue = szDescription != nullptr ? szDescription : L"$";
					m_strValueType = L"string";
				} // DESCRIPTIVE_REPRESENTATION_ITEM
				else if (sdaiGetInstanceType(sdaiItemInstance) == sdaiGetEntity(sdaiModel, "VALUE_REPRESENTATION_ITEM")) {
					SdaiADB sdaiValueComponentADB = nullptr;
					sdaiGetAttrBN(sdaiItemInstance, "value_component", sdaiADB, &sdaiValueComponentADB);

					const char* szTypePath = sdaiGetADBTypePath(sdaiValueComponentADB, 0);
					switch (sdaiGetADBType(sdaiValueComponentADB)) {
						case sdaiINTEGER:
							{
								SdaiInteger iValue = 0;
								sdaiGetADBValue(sdaiValueComponentADB, sdaiINTEGER, (void*)&iValue);

								m_strValue = (const wchar_t*)CA2W(_string::format("%lld", iValue).c_str());
								m_strValueType = L"number";
							}
							break;

						case sdaiREAL:
							{
								double dValue = 0;
								sdaiGetADBValue(sdaiValueComponentADB, sdaiREAL, (void*)&dValue);

								m_strValue = (const wchar_t*)CA2W(_string::format("%f", dValue).c_str());
								m_strValueType = L"number";
							}
							break;

						case sdaiSTRING:
							{
								wchar_t* szValue = nullptr;
								sdaiGetADBValue(sdaiValueComponentADB, sdaiUNICODE, (void*)&szValue);

								m_strValue = szValue != nullptr ? szValue : L"$";
								m_strValueType = L"string";
							}
							break;

						default:
							assert(false);
							break;
					} // switch (sdaiGetADBType(valueComponentADB))
				} // VALUE_REPRESENTATION_ITEM
			} // for (SdaiInteger k = ...
		} // if (sdaiPDRDefinitionInstance == ...
	} // for (SdaiInteger j = ...
}

// ************************************************************************************************
_ap242_property_collection::_ap242_property_collection()
	: m_vecProperties()
{}

/*virtual*/ _ap242_property_collection::~_ap242_property_collection()
{
	for (auto pProperty : m_vecProperties) {
		delete pProperty;
	}
}

// ************************************************************************************************
_ap242_property_provider::_ap242_property_provider(SdaiModel sdaiModel)
	: m_sdaiModel(sdaiModel)
	, m_mapPropertyCollections()
{
	assert(m_sdaiModel != 0);
}

/*virtual*/ _ap242_property_provider::~_ap242_property_provider()
{
	for (auto itPropertyCollection : m_mapPropertyCollections) {
		delete itPropertyCollection.second;
	}
}

_ap242_property_collection* _ap242_property_provider::getPropertyCollection(SdaiInstance sdaiInstance)
{
	if (sdaiInstance == 0) {
		assert(false);
		return nullptr;
	}

	auto itPropertyCollection = m_mapPropertyCollections.find(sdaiInstance);
	if (itPropertyCollection != m_mapPropertyCollections.end()) {
		return itPropertyCollection->second;
	}

	auto pPropertyCollection = loadPropertyCollection(sdaiInstance);
	m_mapPropertyCollections[sdaiInstance] = pPropertyCollection;

	return pPropertyCollection;
}

_ap242_property_collection* _ap242_property_provider::loadPropertyCollection(SdaiInstance sdaiInstance)
{
	auto pPropertyCollection = new _ap242_property_collection();
	loadProperties(sdaiInstance, pPropertyCollection);

	return pPropertyCollection;
}

void _ap242_property_provider::loadProperties(SdaiInstance sdaiInstance, _ap242_property_collection* pPropertyCollection)
{
	assert(sdaiInstance != 0);

	SdaiAggr sdaiPropertyDefinitionAggr = sdaiGetEntityExtentBN(m_sdaiModel, "PROPERTY_DEFINITION");
	SdaiInteger iPropertyDefinitionsCount = sdaiGetMemberCount(sdaiPropertyDefinitionAggr);
	for (SdaiInteger i = 0; i < iPropertyDefinitionsCount; i++) {
		SdaiInstance sdaiPropertyDefinitionInstance = 0;
		sdaiGetAggrByIndex(sdaiPropertyDefinitionAggr, i, sdaiINSTANCE, &sdaiPropertyDefinitionInstance);

		SdaiInstance sdaiDefinitionInstance = 0;
		sdaiGetAttrBN(sdaiPropertyDefinitionInstance, "definition", sdaiINSTANCE, &sdaiDefinitionInstance);
		if (sdaiDefinitionInstance == sdaiInstance) {
			pPropertyCollection->properties().push_back(new _ap242_property(sdaiPropertyDefinitionInstance));			
		}
	}
}
