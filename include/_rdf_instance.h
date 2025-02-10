#pragma once

#include "_instance.h"
#include "_rdf_geometry.h"

// ************************************************************************************************
class _rdf_instance : public _instance
{

public:  // Methods

	_rdf_instance(int64_t iID, _geometry* pGeometry, _matrix4x3* pTransformationMatrix)
		: _instance(iID, pGeometry, pTransformationMatrix)
	{
	}

	virtual ~_rdf_instance()
	{
	}

	static void buildInstanceNames(OwlModel owlModel, OwlInstance owlInstance, wstring& strName, wstring& strUniqueName)
	{
		ASSERT(owlModel != 0);
		ASSERT(owlInstance != 0);

		OwlClass owlClass = GetInstanceClass(owlInstance);
		ASSERT(owlClass != 0);

		wchar_t* szClassName = nullptr;
		GetNameOfClassW(owlClass, &szClassName);

		wchar_t* szName = nullptr;
		GetNameOfInstanceW(owlInstance, &szName);

		if (szName == nullptr)
		{
			RdfProperty rdfTagProperty = GetPropertyByName(owlModel, "tag");
			if (rdfTagProperty != 0)
			{
				SetCharacterSerialization(owlModel, 0, 0, false);

				int64_t iCard = 0;
				wchar_t** szValue = nullptr;
				GetDatatypeProperty(owlInstance, rdfTagProperty, (void**)&szValue, &iCard);

				if (iCard == 1)
				{
					szName = szValue[0];
				}

				SetCharacterSerialization(owlModel, 0, 0, true);
			}
		} // if (szName == nullptr)

		wchar_t szUniqueName[200];

		if (szName != nullptr)
		{
			strName = szName;
			swprintf(szUniqueName, 200, L"%s (%s)", szName, szClassName);
		}
		else
		{
			strName = szClassName;
			swprintf(szUniqueName, 200, L"#%lld (%s)", owlInstance, szClassName);
		}

		strUniqueName = szUniqueName;
	}

public: // Properties

	OwlInstance getOwlInstance() const { return getGeometryAs<_rdf_geometry>()->getOwlInstance(); }

	wstring getName() const
	{
		CString strName;
		strName.Format(_T("#%lld"), getOwlInstance());

		strName += L" ";
		strName += getClassName();

		return (LPCWSTR)strName;
	}

	static wstring getName(OwlInstance owlInstance)
	{
		CString strName;
		strName.Format(_T("#%lld"), owlInstance);

		strName += L" ";
		strName += getClassName(owlInstance);

		return (LPCWSTR)strName;
	}

	OwlClass getClassInstance() const
	{
		return getClassInstance(getOwlInstance());
	}

	static OwlClass getClassInstance(OwlInstance owlInstance)
	{
		return GetInstanceClass(owlInstance);
	}

	const wchar_t* getClassName() const
	{
		return getClassName(getOwlInstance());
	}

	static const wchar_t* getClassName(OwlInstance owlInstance)
	{
		wchar_t* szClassName = nullptr;
		GetNameOfClassW(GetInstanceClass(owlInstance), &szClassName);

		return szClassName;
	}

	static double getDoubleProperty(OwlInstance owlInstance, char* szPropertyName)
	{
		double* pdValues = nullptr;
		int64_t	iCard = 0;
		GetDatatypeProperty(
			owlInstance,
			GetPropertyByName(GetModel(owlInstance), szPropertyName),
			(void**)&pdValues,
			&iCard);

		return (iCard == 1) ? pdValues[0] : 0.;
	}

	static int64_t getObjectProperty(OwlInstance owlInstance, char* szPropertyName)
	{
		OwlInstance* piValues = nullptr;
		int64_t	iCard = 0;
		GetObjectProperty(
			owlInstance,
			GetPropertyByName(GetModel(owlInstance), szPropertyName),
			&piValues,
			&iCard);

		return (iCard == 1) ? piValues[0] : 0;
	}
};