#include "_host.h"
#include "_ap242_product_definition.h"

// ************************************************************************************************
_ap242_product_definition::_ap242_product_definition(OwlInstance owlInstance, SdaiInstance sdaiInstance, MultiThreadOwlModelWrapper multiThreadOwlModelWrapper)
    : _ap242_geometry(owlInstance, sdaiInstance, multiThreadOwlModelWrapper)
    , m_szId(nullptr)
    , m_szName(nullptr)
    , m_szDescription(nullptr)
    , m_szProductId(nullptr)
    , m_szProductName(nullptr)
    , m_iRelatingProducts(0)
    , m_iRelatedProducts(0)
	, m_pProductShape(nullptr)
{
    sdaiGetAttrBN(sdaiInstance, "id", sdaiUNICODE, &m_szId);
    sdaiGetAttrBN(sdaiInstance, "name", sdaiUNICODE, &m_szName);
    sdaiGetAttrBN(sdaiInstance, "description", sdaiUNICODE, &m_szDescription);

    SdaiInstance sdaiFormationInstance = 0;
    sdaiGetAttrBN(sdaiInstance, "formation", sdaiINSTANCE, &sdaiFormationInstance);
    assert(sdaiFormationInstance != 0);

    SdaiInstance sdaiOfProductInstance = 0;
    sdaiGetAttrBN(sdaiFormationInstance, "of_product", sdaiINSTANCE, &sdaiOfProductInstance);

    sdaiGetAttrBN(sdaiOfProductInstance, "id", sdaiUNICODE, &m_szProductId);
    sdaiGetAttrBN(sdaiOfProductInstance, "name", sdaiUNICODE, &m_szProductName);
}

/*virtual*/ _ap242_product_definition::~_ap242_product_definition()
{
}

// ************************************************************************************************
_ap242_product_shape::_ap242_product_shape(_ap242_product_definition* pProductDefinition, SdaiInstance sdaiInstance, MultiThreadOwlModelWrapper multiThreadOwlModelWrapper)
    : _ap242_geometry(0, sdaiInstance, multiThreadOwlModelWrapper)
    , m_pProductDefinition(pProductDefinition)
	, m_vecProductShapeRepresentations()
{
    assert(m_pProductDefinition != nullptr);
}

/*virtual*/ _ap242_product_shape::~_ap242_product_shape()
{
}

void _ap242_product_shape::addProductShapeRepresentation(_ap242_product_shape_representation* pProductShapeRepresentation)
{
    assert(pProductShapeRepresentation != nullptr);
    m_vecProductShapeRepresentations.push_back(pProductShapeRepresentation);
}

// ************************************************************************************************
_ap242_product_shape_representation::_ap242_product_shape_representation(_ap242_product_shape* pProductShape, SdaiInstance sdaiInstance, MultiThreadOwlModelWrapper multiThreadOwlModelWrapper)
    : _ap242_geometry(0, sdaiInstance, multiThreadOwlModelWrapper)
    , m_pProductShape(pProductShape)
    , m_vecRepresentationItems()
{
    assert(m_pProductShape != nullptr);
}

/*virtual*/ _ap242_product_shape_representation::~_ap242_product_shape_representation()
{
}

void _ap242_product_shape_representation::addRepresentationItem(_ap242_product_shape_representation_item* pRepresentationItem)
{
    assert(pRepresentationItem != nullptr);
    m_vecRepresentationItems.push_back(pRepresentationItem);
}

// ************************************************************************************************
_ap242_product_shape_representation_item::_ap242_product_shape_representation_item(_ap242_product_shape_representation* pProductShapeRepresentation, OwlInstance owlInstance, SdaiInstance sdaiInstance, MultiThreadOwlModelWrapper multiThreadOwlModelWrapper)
    : _ap242_geometry(owlInstance, sdaiInstance, multiThreadOwlModelWrapper)
    , m_pProductShapeRepresentation(pProductShapeRepresentation)
{
    assert(m_pProductShapeRepresentation != nullptr);
}

/*virtual*/ _ap242_product_shape_representation_item::~_ap242_product_shape_representation_item()
{
}

// ************************************************************************************************
_ap242_assembly::_ap242_assembly(SdaiInstance sdaiInstance, _ap242_product_definition* pRelatingProductDefinition, _ap242_product_definition* pRelatedProductDefinition)
    : m_sdaiInstance(sdaiInstance)
    , m_szId(nullptr)
    , m_szName(nullptr)
    , m_szDescription(nullptr)
    , m_pRelatingProductDefinition(pRelatingProductDefinition)
    , m_pRelatedProductDefinition(pRelatedProductDefinition)
{
    assert(m_sdaiInstance != 0);

    sdaiGetAttrBN(m_sdaiInstance, "id", sdaiUNICODE, &m_szId);
    sdaiGetAttrBN(m_sdaiInstance, "name", sdaiUNICODE, &m_szName);
    sdaiGetAttrBN(m_sdaiInstance, "description", sdaiUNICODE, &m_szDescription);

    assert(m_pRelatingProductDefinition != nullptr);
    assert(m_pRelatedProductDefinition != nullptr);
}

/*virtual*/ _ap242_assembly::~_ap242_assembly()
{}