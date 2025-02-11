#pragma once

#include "_instance.h"
#include "_rdf_geometry.h"

// ************************************************************************************************
class _rdf_instance : public _instance
{

public:  // Methods

	_rdf_instance(int64_t iID, _geometry* pGeometry, _matrix4x3* pTransformationMatrix)
		: _instance(iID, pGeometry, pTransformationMatrix)
	{}

	virtual ~_rdf_instance()
	{}
	
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

	const wchar_t* getClassName() const
	{
		wchar_t* szClassName = nullptr;
		GetNameOfClassW(GetInstanceClass(getOwlInstance()), &szClassName);

		return szClassName;
	}
};