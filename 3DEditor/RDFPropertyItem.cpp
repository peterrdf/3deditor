#include "stdafx.h"
#include "RDFPropertyItem.h"

// ------------------------------------------------------------------------------------------------
CRDFPropertyItem::CRDFPropertyItem(CRDFInstance * pInstance, CRDFProperty * pProperty)
	: CRDFItem(pInstance)
	, m_pProperty(pProperty)
{
	ASSERT(m_pProperty != NULL);

	m_enItemType = enumItemType::Property;
}

// ------------------------------------------------------------------------------------------------
CRDFPropertyItem::~CRDFPropertyItem()
{
}

// ------------------------------------------------------------------------------------------------
CRDFProperty * CRDFPropertyItem::getProperty() const
{
	return m_pProperty;
}

