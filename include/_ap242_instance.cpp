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

/*virtual*/ void _ap242_instance::saveInstance(const wchar_t* szPath) /*override*/
{
    OwlInstance owlInstance = getOwlInstance();
    if (owlInstance == 0) {
		SdaiInstance sdaiBuildContextInstance = getGeometryAs<_ap242_geometry>()->getSdaiBuildContextInstance();
        if (sdaiBuildContextInstance != 0) {
            owlInstance = _ap_geometry::buildOwlInstanceInContext(getSdaiInstance(), sdaiBuildContextInstance);
        }
        else {
            owlInstance = _ap_geometry::buildOwlInstance(getSdaiInstance());
        }
        assert(owlInstance != 0);
    }

    OwlInstance	owlMatrixInstance = CreateInstance(GetClassByName(getOwlModel(), "Matrix"));
    assert(owlMatrixInstance != 0);

    if (getTransformationMatrix()) {
        vector<double> vecMatrix
        {
            getTransformationMatrix()->_11,
            getTransformationMatrix()->_12,
            getTransformationMatrix()->_13,
            getTransformationMatrix()->_21,
            getTransformationMatrix()->_22,
            getTransformationMatrix()->_23,
            getTransformationMatrix()->_31,
            getTransformationMatrix()->_32,
            getTransformationMatrix()->_33,
            getTransformationMatrix()->_41,
            getTransformationMatrix()->_42,
            getTransformationMatrix()->_43,
        };

        SetDatatypeProperty(
            owlMatrixInstance,
            GetPropertyByName(getOwlModel(), "coordinates"),
            vecMatrix.data(),
            vecMatrix.size());

        OwlInstance owlTransformationInstance = CreateInstance(GetClassByName(getOwlModel(), "Transformation"));
        assert(owlTransformationInstance != 0);

        SetObjectProperty(
            owlTransformationInstance,
            GetPropertyByName(getOwlModel(), "object"),
            owlInstance);

        SetObjectProperty(
            owlTransformationInstance,
            GetPropertyByName(getOwlModel(), "matrix"),
            owlMatrixInstance);

        SaveInstanceTreeW(owlTransformationInstance, szPath);

        RemoveInstance(owlTransformationInstance);
        RemoveInstance(owlMatrixInstance);
    }
    else {
        SaveInstanceTreeW(owlInstance, szPath);
    }
}

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