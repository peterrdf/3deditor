#pragma once

#include "ifcengine.h"

#include "_geometry.h"

#include <algorithm>

// ************************************************************************************************
class _ap_geometry : public _geometry
{

private: // Members

    SdaiInstance m_sdaiInstance;
    MultiThreadOwlModelWrapper m_multiThreadOwlModelWrapper;

public: // Methods

    _ap_geometry(OwlInstance owlInstance, SdaiInstance sdaiInstance, MultiThreadOwlModelWrapper multiThreadOwlModelWrapper)
        : _geometry(owlInstance)
        , m_sdaiInstance(sdaiInstance)
		, m_multiThreadOwlModelWrapper(multiThreadOwlModelWrapper)
    {
        if (multiThreadOwlModelWrapper == 0) {
            m_strName = m_strUniqueName = getDisplayString(sdaiInstance);
        }        
    }

    virtual ~_ap_geometry()
    {}

    // _geometry
    virtual OwlModel getOwlModel() const override
    {
        if (OwlInstance owlInstance = getOwlInstance())
            return GetModel(owlInstance);

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

        OwlInstance owlInstance = owlBuildInstanceMT(sdaiInstance);

        return owlInstance;
    }

    static OwlInstance buildOwlInstance(SdaiInstance sdaiInstance, MultiThreadOwlModelWrapper multiThreadOwlModelWrapper)
    {
        assert(sdaiInstance != 0);
		assert(multiThreadOwlModelWrapper != 0);

        OwlInstance owlInstance = owlBuildInstanceMT(sdaiInstance, multiThreadOwlModelWrapper);

        return owlInstance;
    }

    void loadDisplayString()
    {
        m_strName = m_strUniqueName = getDisplayString(getSdaiInstance());
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
    MultiThreadOwlModelWrapper getMultiThreadOwlModelWrapper() const { return m_multiThreadOwlModelWrapper; }
    ExpressID getExpressID() const { return internalGetP21Line(m_sdaiInstance); }
    SdaiModel getSdaiModel() const { return sdaiGetInstanceModel(m_sdaiInstance); }
    SdaiEntity getSdaiEntity() const { return getSdaiEntity(m_sdaiInstance); }
    const wchar_t* getEntityName() const { return getEntityName(m_sdaiInstance); }

    static wstring getDisplayString(SdaiInstance sdaiInstance, bool full = true)
    {
        int64_t iExpressID = internalGetP21Line(sdaiInstance);
        wchar_t szStepId[512];
        swprintf(szStepId, 512, L"#%lld", iExpressID);

        wstring strUniqueName (szStepId);
        strUniqueName += L"=";

        auto entity = sdaiGetInstanceType(sdaiInstance);
        if (engiIsComplexEntity(entity) && full) {
            // do nothing, follow full display string
        }
        else {
            strUniqueName += getEntityName(sdaiInstance);
        }

        if (full) {
            strUniqueName += InstanceContentToString(sdaiInstance);
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

    private:

        static std::wstring InstanceContentToString(SdaiInstance sdaiInstance)
        {
            engiEnableDerivedAttributes(sdaiGetInstanceModel(sdaiInstance), false); //want to show * for derived

            auto entity = sdaiGetInstanceType(sdaiInstance);
            if (engiIsComplexEntity(entity)) {

                std::wstring content (L"(");
                SdaiInteger index = 0;
                while (SdaiEntity component = engiGetEntityParentEx(entity, index++)) {
                    SdaiString componentName = nullptr;
                    engiGetEntityNameEx(component, sdaiSTRING, &componentName, false);
                    content += CA2W(componentName);
                    content += DirectAttributesToString(sdaiInstance, component, true);
                }
                content += L")";
                return content;
            }
            else {
                return DirectAttributesToString(sdaiInstance, entity, false);
            }
        }

        static std::wstring AdbToString(SdaiADB adb)
        {
            std::wstring content;

            auto typePath = sdaiGetADBTypePath(adb, 0);
            if (typePath && *typePath) {
                content += CA2W(typePath);
                content += L"(";
            }

            SdaiString strValue = NULL;
            SdaiInstance instVal = NULL;
            SdaiAggr aggrVal = NULL;

            sdaiGetADBValue(adb, sdaiEXPRESSSTRING, &strValue);
            if (strValue) {
                content += CA2W(strValue);
            }
            else if (sdaiGetADBValue(adb, sdaiINSTANCE, &instVal) && instVal) {
                auto stepId = internalGetP21Line(instVal);
                content += L"#";
                content += std::to_wstring(stepId);
            }
            else if (sdaiGetADBValue(adb, sdaiAGGR, &aggrVal) && aggrVal) {
                content += AggregationToString(aggrVal);
            }
            else {
                assert(!"TODO: handle attribute type");
            }

            if (typePath && *typePath) {
                content += L")";
            }
            return content;
        }

        static std::wstring DirectAttributesToString(SdaiInstance instance, SdaiEntity entity, bool partOfComplexEntity)
        {
            std::wstring content = L"(";

            SdaiInteger attributeIndex = 0;
            while (SdaiAttr attribute = engiGetEntityAttributeByIndex(entity, attributeIndex++, !partOfComplexEntity, false)) {

                if (attributeIndex > 1) {
                    content += L",";
                }

                SdaiADB adb = NULL;
                if (sdaiGetAttr(instance, attribute, sdaiADB, &adb)) {
                    content += AdbToString(adb);
                    sdaiDeleteADB(adb);
                }
                else {
                    SdaiString strValue = NULL;
                    auto res = sdaiGetAttr(instance, attribute, sdaiEXPRESSSTRING, &strValue);
                    assert(!res && strValue); // expected everything except $/* will return ADB
                    if (!res && strValue)
                        content += CA2W(strValue);
                }
            }

            content += L")";

            return content;
        }

        static std::wstring AggregationToString(SdaiAggr aggr)
        {
            std::wstring content = L"(";
            
            SdaiInteger N = min(sdaiGetMemberCount(aggr), (SdaiInteger)3);
            for (SdaiAggrIndex index = 0; index < N; index++) {
                
                if (index > 0) {
                    content += L",";
                }

                SdaiADB adb = NULL;
                if (sdaiGetAggrByIndex(aggr, index, sdaiADB, &adb)) {
                    content += AdbToString(adb);
                    sdaiDeleteADB(adb);
                }
                else {
                    SdaiString strValue = NULL;
                    auto res = sdaiGetAggrByIndex(aggr, index, sdaiEXPRESSSTRING, &strValue);
                    assert(!res && strValue); // expected everything except $/* will return ADB
                    if (!res && strValue)
                        content += CA2W(strValue);

                }
            }

            if (N < sdaiGetMemberCount(aggr)) {
                content += L"...";
            }

            content += L")";

            return content;
        }
};