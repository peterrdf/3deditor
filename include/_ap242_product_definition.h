#pragma once

#include "_ap242_geometry.h"

// ************************************************************************************************
class _ap242_product_shape;
class _ap242_product_shape_representation;
class _ap242_product_shape_representation_item;

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
	
	_ap242_product_shape* m_pProductShape;

public: // Methods

	_ap242_product_definition(OwlInstance owlInstance, SdaiInstance sdaiInstance, MultiThreadOwlModelWrapper multiThreadOwlModelWrapper);
	virtual ~_ap242_product_definition();

public: // Properties

	const wchar_t* getId() const { return m_szId; }
	const wchar_t* getName() const { return m_szName; }
	const wchar_t* getDescription() const { return m_szDescription; }
	const wchar_t* getProductId() const { return m_szProductId; }
	const wchar_t* getProductName() const { return m_szProductName; }

	SdaiInteger getRelatingProducts() const { return m_iRelatingProducts; }
	SdaiInteger getRelatedProducts() const { return m_iRelatedProducts; }

	void setProductShape(_ap242_product_shape* pProductShape) { assert(pProductShape); m_pProductShape = pProductShape; }
	_ap242_product_shape* getProductShape() const { return m_pProductShape; }
};

// ************************************************************************************************
class _ap242_product_shape : public _ap242_geometry
{
	friend class _ap242_model;

private: // Members

	_ap242_product_definition* m_pProductDefinition;
	vector<_ap242_product_shape_representation*> m_vecProductShapeRepresentations;

public: // Methods

	_ap242_product_shape(_ap242_product_definition* pProductDefinition, SdaiInstance sdaiInstance, MultiThreadOwlModelWrapper multiThreadOwlModelWrapper);
	virtual ~_ap242_product_shape();

	void addProductShapeRepresentation(_ap242_product_shape_representation* pProductShapeRepresentation);

public: // Properties

	const vector<_ap242_product_shape_representation*>& getProductShapeRepresentations() const { return m_vecProductShapeRepresentations; }
	_ap242_product_definition* getProductDefinition() const { return m_pProductDefinition; }
};

// ************************************************************************************************
class _ap242_product_shape_representation : public _ap242_geometry
{
	friend class _ap242_model;

private: // Members

	_ap242_product_shape* m_pProductShape;
	vector<_ap242_product_shape_representation_item*> m_vecRepresentationItems;

public: // Methods

	_ap242_product_shape_representation(_ap242_product_shape* pProductShape, SdaiInstance sdaiInstance, MultiThreadOwlModelWrapper multiThreadOwlModelWrapper);
	virtual ~_ap242_product_shape_representation();

	void addRepresentationItem(_ap242_product_shape_representation_item* pRepresentationItem);

public: // Properties

	const vector<_ap242_product_shape_representation_item*>& getRepresentationItems() const { return m_vecRepresentationItems; }
	_ap242_product_shape* getProductShape() const { return m_pProductShape; }
	_ap242_product_definition* getProductDefinition() const { return m_pProductShape->getProductDefinition(); }
};

// ************************************************************************************************
class _ap242_product_shape_representation_item : public _ap242_geometry
{
	friend class _ap242_model;

private: // Members
	
	_ap242_product_shape_representation* m_pProductShapeRepresentation;

public: // Methods

	_ap242_product_shape_representation_item(_ap242_product_shape_representation* pProductShapeRepresentation, OwlInstance owlInstance, SdaiInstance sdaiInstance, MultiThreadOwlModelWrapper multiThreadOwlModelWrapper);
	virtual ~_ap242_product_shape_representation_item();

public: // Properties

	_ap242_product_shape_representation* getProductShapeRepresentation() const { return m_pProductShapeRepresentation; }
	_ap242_product_definition* getProductDefinition() const { return m_pProductShapeRepresentation->getProductDefinition(); }
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