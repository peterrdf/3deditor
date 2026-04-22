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

    void saveInstance(const wchar_t* szPath, OwlInstance owlMapConversionTransformationInstance)
    {
        if (owlMapConversionTransformationInstance == 0) {
			return saveInstance(szPath);
		}

        OwlInstance owlScaleTransformationInstance = 0;

        double dLengthConversionFactor = getProjectUnitConversionFactor(
            sdaiGetInstanceModel(getSdaiInstance()), "LENGTHUNIT", nullptr, nullptr, nullptr);

        if (dLengthConversionFactor != 1.) {
            OwlInstance owlScaleMatrixInstance = CreateInstance(GetClassByName(getGeometry()->getOwlModel(), "Matrix"));
            assert(owlScaleMatrixInstance != 0);

            vector<double> vecScaleTransformationMatrix =
            {
                dLengthConversionFactor, 0., 0.,
                0., dLengthConversionFactor, 0.,
                0., 0., dLengthConversionFactor,
                0., 0., 0.,
            };

            SetDatatypeProperty(
                owlScaleMatrixInstance,
                GetPropertyByName(getGeometry()->getOwlModel(), "coordinates"),
                vecScaleTransformationMatrix.data(),
                vecScaleTransformationMatrix.size());

            owlScaleTransformationInstance = CreateInstance(
                GetClassByName(getGeometry()->getOwlModel(),
                    "Transformation"));
            assert(owlScaleTransformationInstance != 0);

            SetObjectProperty(
                owlScaleTransformationInstance,
                GetPropertyByName(getGeometry()->getOwlModel(), "matrix"),
                &owlScaleMatrixInstance,
                1);
        }
        
        OwlInstance owlInstance = getOwlInstance();
        if (owlInstance == 0) {
            owlInstance = _ap_geometry::buildOwlInstance(getSdaiInstance());
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

            if (owlScaleTransformationInstance) {
                SetObjectProperty(
                    owlScaleTransformationInstance,
                    GetPropertyByName(getGeometry()->getOwlModel(), "object"),
                    &owlTransformationInstance,
                    1);

                SetObjectProperty(
                    owlMapConversionTransformationInstance,
                    GetPropertyByName(getOwlModel(), "object"),
                    owlScaleTransformationInstance);

                SaveInstanceTreeW(owlMapConversionTransformationInstance, szPath);

                RemoveInstance(owlScaleTransformationInstance);
            }
            else {
                SetObjectProperty(
                    owlMapConversionTransformationInstance,
                    GetPropertyByName(getOwlModel(), "object"),
                    &owlTransformationInstance,
                    1);

                SaveInstanceTreeW(owlMapConversionTransformationInstance, szPath);
            }           

            RemoveInstance(owlTransformationInstance);
            RemoveInstance(owlMatrixInstance);            
        }
        else {
            if (owlScaleTransformationInstance) {
                SetObjectProperty(
                    owlScaleTransformationInstance,
                    GetPropertyByName(getGeometry()->getOwlModel(), "object"),
                    &owlInstance,
                    1);

                SetObjectProperty(
                    owlMapConversionTransformationInstance,
                    GetPropertyByName(getOwlModel(), "object"),
                    owlScaleTransformationInstance);

                SaveInstanceTreeW(owlMapConversionTransformationInstance, szPath);

                RemoveInstance(owlScaleTransformationInstance);
            }
            else {
                SetObjectProperty(
                    owlMapConversionTransformationInstance,
                    GetPropertyByName(getOwlModel(), "object"),
                    &owlInstance,
                    1);

                SaveInstanceTreeW(owlMapConversionTransformationInstance, szPath);
			}
        }
    }

public: // Properties

    SdaiInstance getSdaiInstance() const { return getGeometryAs<_ap_geometry>()->getSdaiInstance(); }
    ExpressID getExpressID() const { return getGeometryAs<_ap_geometry>()->getExpressID(); }
    SdaiEntity getSdaiEntity() const { return getGeometryAs<_ap_geometry>()->getSdaiEntity(); }
    const wchar_t* getEntityName() const { return getGeometryAs<_ap_geometry>()->getEntityName(); }
};