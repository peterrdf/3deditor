#include "stdafx.h"
#include "RDFItem.h"

// ************************************************************************************************
CRDFItem::CRDFItem(_rdf_instance* pInstance)
	: m_pInstance(pInstance) 
	, m_enItemType(enumItemType::Unknown)
	, m_vecItems()
{
	ASSERT(m_pInstance != nullptr);
}

CRDFItem::~CRDFItem()
{
}

// ************************************************************************************************
CRDFInstanceItem::CRDFInstanceItem(_rdf_instance* pInstance)
	: CRDFItem(pInstance)
{
	m_enItemType = enumItemType::Instance;
}

CRDFInstanceItem::~CRDFInstanceItem()
{
}


// ************************************************************************************************
CRDFPropertyItem::CRDFPropertyItem(_rdf_instance* pInstance, _rdf_property_t* pProperty)
	: CRDFItem(pInstance)
	, m_pProperty(pProperty)
{
	assert(m_pProperty != nullptr);

	m_enItemType = enumItemType::Property;
}

CRDFPropertyItem::~CRDFPropertyItem()
{
}