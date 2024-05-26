#include "stdafx.h"
#include "RDFPropertyItem.h"

CRDFPropertyItem::CRDFPropertyItem(CRDFInstance * pInstance, CRDFProperty * pProperty)
	: CRDFItem(pInstance)
	, m_pProperty(pProperty)
{
	assert(m_pProperty != nullptr);

	m_enItemType = enumItemType::Property;
}

CRDFPropertyItem::~CRDFPropertyItem()
{}

