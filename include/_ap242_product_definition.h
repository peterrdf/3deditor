#pragma once

#include "_ap242_geometry.h"

// ************************************************************************************************
class _ap242_product_definition : public _ap242_geometry
{
	friend class _ap242_model;

private: // Members

	wchar_t* m_szId;
	wchar_t* m_szName;
	wchar_t* m_szDescription;
	wchar_t* m_szProductId;
	wchar_t* m_szProductName;

	SdaiInteger m_iRelatingProducts; // if == 0 then it has geometry, otherwise it is a placeholder
	SdaiInteger m_iRelatedProducts;  // if == 0 then it is a root element

public: // Methods

	_ap242_product_definition(OwlInstance owlInstance, SdaiInstance sdaiInstance);
	virtual ~_ap242_product_definition();

public: // Properties

	const wchar_t* getId() const { return m_szId; }
	const wchar_t* getName() const { return m_szName; }
	const wchar_t* getDescription() const { return m_szDescription; }
	const wchar_t* getProductId() const { return m_szProductId; }
	const wchar_t* getProductName() const { return m_szProductName; }

	SdaiInteger getRelatingProducts() const { return m_iRelatingProducts; }
	SdaiInteger getRelatedProducts() const { return m_iRelatedProducts; }
};

// ************************************************************************************************
class _ap242_assembly
{

private: // Members

	SdaiInstance m_sdaiInstance;
	wchar_t* m_szId;
	wchar_t* m_szName;
	wchar_t* m_szDescription;
	_ap242_product_definition* m_pRelatingProductDefinition;
	_ap242_product_definition* m_pRelatedProductDefinition;

public: // Methods

	_ap242_assembly(SdaiInstance iInstance, _ap242_product_definition* pRelatingProductDefinition, _ap242_product_definition* pRelatedProductDefinition);
	virtual ~_ap242_assembly();

public: // Properties

	SdaiInstance getSdaiInstance() const { return m_sdaiInstance; }
	ExpressID getExpressID() const { return internalGetP21Line(m_sdaiInstance); }
	const wchar_t* getId() const { return m_szId; }
	const wchar_t* getName() const { return m_szName; }
	const wchar_t* getDescription() const { return m_szDescription; }
	_ap242_product_definition* getRelatingProductDefinition() const { return m_pRelatingProductDefinition; }
	_ap242_product_definition* getRelatedProductDefinition() const { return m_pRelatedProductDefinition; }
};