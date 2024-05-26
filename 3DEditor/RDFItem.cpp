#include "stdafx.h"
#include "RDFItem.h"

// ------------------------------------------------------------------------------------------------
CRDFItem::CRDFItem(CRDFInstance * pInstance)
	: m_vecItems()
	, m_pInstance(pInstance)
	, m_enItemType(enumItemType::Unknown)
{
	assert(m_pInstance != nullptr);
}

// ------------------------------------------------------------------------------------------------
CRDFItem::~CRDFItem()
{
}

// ------------------------------------------------------------------------------------------------
CRDFInstance * CRDFItem::GetInstance() const
{
	return m_pInstance;
}

// ------------------------------------------------------------------------------------------------
enumItemType CRDFItem::getType() const
{
	return m_enItemType;
}

// ------------------------------------------------------------------------------------------------
vector<HTREEITEM> & CRDFItem::items()
{
	return m_vecItems;
}