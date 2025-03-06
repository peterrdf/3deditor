#pragma once

#include "_instance.h"
#include "_ap_geometry.h"

// ************************************************************************************************
class _ap_instance : public _instance
{

public:  // Methods

	_ap_instance(int64_t iID, _geometry* pGeometry, _matrix4x3* pTransformationMatrix)
		: _instance(iID, pGeometry, pTransformationMatrix)
	{
	}

	virtual ~_ap_instance()
	{
	}

	virtual void saveInstance(const wchar_t* szPath)
	{
		OwlInstance owlInstance = getOwlInstance();
		if (owlInstance == 0)
		{
			owlInstance = _ap_geometry::buildOwlInstance(getSdaiInstance());
			ASSERT(owlInstance != 0);
		}

		OwlInstance	owlMatrixInstance = CreateInstance(GetClassByName(getOwlModel(), "Matrix"));
		ASSERT(owlMatrixInstance != 0);

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
		ASSERT(owlTransformationInstance != 0);

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

	// _instance
	virtual wstring getName() const override
	{
		return getName(getSdaiInstance());
	}

	SdaiInstance getSdaiInstance() const { return getGeometryAs<_ap_geometry>()->getSdaiInstance(); }
	ExpressID getExpressID() const { return getGeometryAs<_ap_geometry>()->getExpressID(); }
	SdaiEntity getSdaiEntity() const { return getGeometryAs<_ap_geometry>()->getSdaiEntity(); }
	const wchar_t* getEntityName() const { return getGeometryAs<_ap_geometry>()->getEntityName(); }

	static wstring getName(SdaiInstance sdaiInstance)
	{
		wstring strUniqueName;

		int64_t iExpressID = internalGetP21Line(sdaiInstance);
		if (iExpressID != 0)
		{
			CString strID;
			strID.Format(_T("#%lld"), iExpressID);

			strUniqueName = strID;
			strUniqueName += L" ";
			strUniqueName += _ap_geometry::getEntityName(sdaiInstance);
		}

		wchar_t* szName = nullptr;
		sdaiGetAttrBN(sdaiInstance, "Name", sdaiUNICODE, &szName);

		if ((szName != nullptr) && (wcslen(szName) > 0))
		{
			strUniqueName += L" '";
			strUniqueName += szName;
			strUniqueName += L"'";
		}

		wchar_t* szDescription = nullptr;
		sdaiGetAttrBN(sdaiInstance, "Description", sdaiUNICODE, &szDescription);

		if ((szDescription != nullptr) && (wcslen(szDescription) > 0))
		{
			strUniqueName += L" (";
			strUniqueName += szDescription;
			strUniqueName += L")";
		}

		return strUniqueName;
	}	
};