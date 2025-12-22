#include "_host.h"
#include "_ap242_instance.h"
#include "_ap242_geometry.h"
#include "_ap242_product_definition.h"
#include "_ptr.h"

// ************************************************************************************************
_ap242_instance::_ap242_instance(int64_t iID, _ap242_geometry* pGeometry, _matrix4x3* pTransformationMatrix)
    : _ap_instance(iID, pGeometry, pTransformationMatrix)
    , m_strName(L"")
{
    auto pProductDeifnition = getProductDefinition();

    wchar_t szBuffer[512];
    if (pProductDeifnition != nullptr) {
        swprintf(szBuffer, 512, L"#%lld %s %s", pProductDeifnition->getExpressID(), pProductDeifnition->getProductName(), L"(product)");
    }
	else {
        swprintf(szBuffer, 512, L"#%lld", pGeometry->getExpressID());
	}    

    m_strName = szBuffer;
}

/*virtual*/ _ap242_instance::~_ap242_instance()
{}

/*virtual*/ const wchar_t* _ap242_instance::getName() const /*override*/
{
    return m_strName.c_str();
}

_ap242_product_definition* _ap242_instance::getProductDefinition() const
{
    return getGeometryAs<_ap242_product_definition>();
}

// ************************************************************************************************
_ap242_product_shape_representation_item_instance::_ap242_product_shape_representation_item_instance(int64_t iID, _ap242_geometry* pGeometry, _matrix4x3* pTransformationMatrix)
    : _ap242_instance(iID, pGeometry, pTransformationMatrix)
{
}

/*virtual*/ _ap242_product_shape_representation_item_instance::~_ap242_product_shape_representation_item_instance()
{
}

/*virtual*/ _ap242_product_definition* _ap242_product_shape_representation_item_instance::getProductDefinition() const /*override*/
{
    return getGeometryAs<_ap242_product_shape_representation_item>()->getProductDefinition();
}