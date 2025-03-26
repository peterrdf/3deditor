#pragma once

#include "_instance.h"
#include "_ap_geometry.h"

// ************************************************************************************************
class _ap_instance : public _instance
{

public:  // Methods

    _ap_instance(int64_t iID, _geometry* pGeometry, _matrix4x3* pTransformationMatrix)
        : _instance(iID, pGeometry, pTransformationMatrix)
    {}

    virtual ~_ap_instance()
    {}

    virtual void saveInstance(const wchar_t* szPath)
    {
        OwlInstance owlInstance = getOwlInstance();
        if (owlInstance == 0) {
            owlInstance = _ap_geometry::buildOwlInstance(getSdaiInstance());
            assert(owlInstance != 0);
        }

        OwlInstance	owlMatrixInstance = CreateInstance(GetClassByName(getOwlModel(), "Matrix"));
        assert(owlMatrixInstance != 0);

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
    }

public: // Properties

    SdaiInstance getSdaiInstance() const { return getGeometryAs<_ap_geometry>()->getSdaiInstance(); }
    ExpressID getExpressID() const { return getGeometryAs<_ap_geometry>()->getExpressID(); }
    SdaiEntity getSdaiEntity() const { return getGeometryAs<_ap_geometry>()->getSdaiEntity(); }
    const wchar_t* getEntityName() const { return getGeometryAs<_ap_geometry>()->getEntityName(); }
};