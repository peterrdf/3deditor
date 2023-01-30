#include "stdafx.h"
#include "RDFInstanceItem.h"

// ------------------------------------------------------------------------------------------------
CRDFInstanceItem::CRDFInstanceItem(CRDFInstance* pInstance)
	: CRDFItem(pInstance)
{
	m_enItemType = enumItemType::Instance;
}

// ------------------------------------------------------------------------------------------------
CRDFInstanceItem::~CRDFInstanceItem()
{
}
