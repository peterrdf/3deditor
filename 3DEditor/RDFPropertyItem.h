#pragma once

#include "RDFItem.h"
#include "RDFProperty.h"
#include "RDFPropertyRestriction.h"

// ------------------------------------------------------------------------------------------------
class CRDFPropertyItem :
	public CRDFItem
{

private: // Members

	// --------------------------------------------------------------------------------------------
	// RDF Property
	CRDFProperty * m_pProperty;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CRDFPropertyItem(CRDFInstance * pInstance, CRDFProperty * pProperty);

	// --------------------------------------------------------------------------------------------
	// ctor
	virtual ~CRDFPropertyItem();

	// --------------------------------------------------------------------------------------------
	// Getter
	CRDFProperty * getProperty() const;
};

