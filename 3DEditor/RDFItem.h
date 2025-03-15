#pragma once

#include "_rdf_mvc.h"
#include "_rdf_property.h"

enum class enumItemType : int
{
	Unknown = 0,
	Instance = 1,
	Property = 2,
};

// ************************************************************************************************
class CRDFItem
{

private: // Fields

	_rdf_instance* m_pInstance;
	vector<HTREEITEM> m_vecItems;	

protected: // Fields

	enumItemType m_enItemType;

public: // Methods

	CRDFItem(_rdf_instance* pInstance);
	virtual ~CRDFItem();

public: // Properties
	
	_rdf_instance* GetInstance() const { return m_pInstance; }
	enumItemType GetType() const { return m_enItemType; }
	vector<HTREEITEM>& Items() { return m_vecItems; }
};

// ************************************************************************************************
class CRDFInstanceItem
	: public CRDFItem
{

public: // Methods

	CRDFInstanceItem(_rdf_instance* pInstance);
	virtual ~CRDFInstanceItem();
};

// ************************************************************************************************
class CRDFPropertyItem :
	public CRDFItem
{

private: // Fields

	_rdf_property* m_pProperty;

public: // Methods

	CRDFPropertyItem(_rdf_instance* pInstance, _rdf_property* pProperty);
	virtual ~CRDFPropertyItem();

	_rdf_property* GetProperty() const { return m_pProperty; }
};