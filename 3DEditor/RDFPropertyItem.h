#pragma once

#include "RDFItem.h"
#include "RDFProperty.h"
#include "RDFPropertyRestriction.h"

class CRDFPropertyItem :
	public CRDFItem
{

private: // Members

	CRDFProperty* m_pProperty;

public: // Methods

	CRDFPropertyItem(CRDFInstance* pInstance, CRDFProperty* pProperty);
	virtual ~CRDFPropertyItem();

	CRDFProperty* GetProperty() const { return m_pProperty; }
};

