#include "stdafx.h"
#include "RDFInstanceItem.h"

// ------------------------------------------------------------------------------------------------
CRDFInstanceItem::CRDFInstanceItem(CRDFInstance * pInstance)
	: CRDFItem(pInstance)
{
	m_enRDFItemType = rdftInstance;
}

// ------------------------------------------------------------------------------------------------
CRDFInstanceItem::~CRDFInstanceItem()
{
}
