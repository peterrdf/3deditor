#pragma once

#include "_geometry.h"

// ************************************************************************************************
class _ap_geometry : public _geometry
{

private: // Members

	SdaiInstance m_sdaiInstance;

public: // Methods

	_ap_geometry(OwlInstance owlInstance, SdaiInstance sdaiInstance)
		: _geometry(owlInstance)
		, m_sdaiInstance(sdaiInstance)
	{
	}

	virtual ~_ap_geometry()
	{
	}

	// _geometry
	virtual OwlModel getOwlModel() override
	{
		return getOwlModel(getSdaiModel());
	}

	virtual void cleanCachedGeometry() override
	{
		_geometry::cleanCachedGeometry();

		cleanMemory(getSdaiModel(), 0);
	}

	static OwlModel getOwlModel(SdaiModel sdaiModel)
	{
		assert(sdaiModel != 0);

		OwlModel owlModel = 0;
		owlGetModel(sdaiModel, &owlModel);
		assert(owlModel != 0);

		return owlModel;
	}

	static OwlInstance buildOwlInstance(SdaiInstance sdaiInstance)
	{
		assert(sdaiInstance != 0);

		SdaiModel sdaiModel = sdaiGetInstanceModel(sdaiInstance);
		assert(sdaiModel != 0);

		OwlInstance owlInstance = 0;
		owlBuildInstance(sdaiModel, sdaiInstance, &owlInstance);

		return owlInstance;
	}

	void setAPFormatSettings()
	{
		uint64_t mask = 0;
		mask += FORMAT_SIZE_VERTEX_DOUBLE;
		mask += FORMAT_SIZE_INDEX_INT64;
		mask += FORMAT_VERTEX_NORMAL;
		mask += FORMAT_VERTEX_TEXTURE_UV;
		mask += FORMAT_EXPORT_TRIANGLES;
		mask += FORMAT_EXPORT_LINES;
		mask += FORMAT_EXPORT_POINTS;
		mask += FORMAT_EXPORT_CONCEPTUAL_FACE_POLYGONS;
		mask += FORMAT_EXPORT_POLYGONS_AS_TUPLES;

		uint64_t setting = 0;
		setting += FORMAT_VERTEX_NORMAL;
		setting += FORMAT_EXPORT_TRIANGLES;
		setting += FORMAT_EXPORT_LINES;
		setting += FORMAT_EXPORT_POINTS;
		setting += FORMAT_EXPORT_CONCEPTUAL_FACE_POLYGONS;
		setting += FORMAT_EXPORT_POLYGONS_AS_TUPLES;

		SetFormat(getOwlModel(), setting, mask);
		SetBehavior(getOwlModel(), 2048 + 4096, 2048 + 4096);
	}

public: // Properties

	SdaiInstance getSdaiInstance() const { return m_sdaiInstance; }
	ExpressID getExpressID() const { return internalGetP21Line(m_sdaiInstance); }
	SdaiModel getSdaiModel() const { return sdaiGetInstanceModel(m_sdaiInstance); }
	SdaiEntity getSdaiEntity() const { return getSdaiEntity(m_sdaiInstance); }
	const wchar_t* getEntityName() const { return getEntityName(m_sdaiInstance); }

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
			strUniqueName += getEntityName(sdaiInstance);
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

	static SdaiEntity getSdaiEntity(SdaiInstance sdaiInstance)
	{
		return sdaiGetInstanceType(sdaiInstance);
	}

	static const wchar_t* getEntityName(SdaiInstance sdaiInstance)
	{
		wchar_t* szEntityName = nullptr;
		engiGetEntityName(getSdaiEntity(sdaiInstance), sdaiUNICODE, (const char**)&szEntityName);

		return szEntityName;
	}
};