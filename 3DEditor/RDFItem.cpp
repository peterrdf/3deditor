#include "stdafx.h"
#include "RDFItem.h"

// ------------------------------------------------------------------------------------------------
CRDFItem::CRDFItem(CRDFInstance * pInstance)
	: m_vecItems()
	, m_pInstance(pInstance)
	, m_enRDFItemType(rdftUnknown)
{
	ASSERT(m_pInstance != NULL);
}

// ------------------------------------------------------------------------------------------------
CRDFItem::~CRDFItem()
{
}

// ------------------------------------------------------------------------------------------------
CRDFInstance * CRDFItem::getInstance() const
{
	return m_pInstance;
}

// ------------------------------------------------------------------------------------------------
enumRDFItemType CRDFItem::getType() const
{
	return m_enRDFItemType;
}

// ------------------------------------------------------------------------------------------------
vector<HTREEITEM> & CRDFItem::items()
{
	return m_vecItems;
}