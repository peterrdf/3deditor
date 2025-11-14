#include "_host.h"
#include "_ifc_unit.h"

// ************************************************************************************************
_ifc_unit::_ifc_unit(SdaiInstance sdaiInstance, const wchar_t* szType, const wchar_t* szPrefix, const wchar_t* szName)
	: m_sdaiInstance(sdaiInstance)
    , m_iType(UNKNOWN)
    , m_strType(L"")
    , m_strPrefix(L"")
    , m_strPrefixSymbol(L"")
    , m_strName(L"")
    , m_strNameSymbol(L"")
{
    assert(m_sdaiInstance != 0);
    assert(szName != nullptr);
	assert(szType != nullptr);

    convertType(szType);
    convertPrefix(szPrefix);
    convertName(szName);
}

/*virtual*/ _ifc_unit::~_ifc_unit()
{}

wstring _ifc_unit::getUnit() const
{
    wstring strUnit = m_strPrefix;
    if (!strUnit.empty()) {
        strUnit += L" ";
    }

    strUnit += m_strName;

    return strUnit;
}

wstring _ifc_unit::getUnitSymbol() const
{
    wstring strUnitSymbol = m_strPrefixSymbol;
    strUnitSymbol += m_strNameSymbol;

    return strUnitSymbol;
}

void _ifc_unit::convertType(const wchar_t* szUnitType)
{
    assert(szUnitType != nullptr);

    if (wcscmp(szUnitType, L".ABSORBEDDOSEUNIT.") == 0) {
        m_iType = ABSORBEDDOSEUNIT;
        m_strType = L"ABSORBEDDOSEUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".AREAUNIT.") == 0) {
        m_iType = AREAUNIT;
        m_strType = L"AREAUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".DOSEEQUIVALENTUNIT.") == 0) {
        m_iType = DOSEEQUIVALENTUNIT;
        m_strType = L"DOSEEQUIVALENTUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".ELECTRICCAPACITANCEUNIT.") == 0) {
        m_iType = ELECTRICCAPACITANCEUNIT;
        m_strType = L"ELECTRICCAPACITANCEUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".ELECTRICCHARGEUNIT.") == 0) {
        m_iType = ELECTRICCHARGEUNIT;
        m_strType = L"ELECTRICCHARGEUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".ELECTRICCONDUCTANCEUNIT.") == 0) {
        m_iType = ELECTRICCONDUCTANCEUNIT;
        m_strType = L"ELECTRICCONDUCTANCEUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".ELECTRICCURRENTUNIT.") == 0) {
        m_iType = ELECTRICCURRENTUNIT;
        m_strType = L"ELECTRICCURRENTUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".ELECTRICRESISTANCEUNIT.") == 0) {
        m_iType = ELECTRICRESISTANCEUNIT;
        m_strType = L"ELECTRICRESISTANCEUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".ELECTRICVOLTAGEUNIT.") == 0) {
        m_iType = ELECTRICVOLTAGEUNIT;
        m_strType = L"ELECTRICVOLTAGEUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".ENERGYUNIT.") == 0) {
        m_iType = ENERGYUNIT;
        m_strType = L"ENERGYUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".FORCEUNIT.") == 0) {
        m_iType = FORCEUNIT;
        m_strType = L"FORCEUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".FREQUENCYUNIT.") == 0) {
        m_iType = FREQUENCYUNIT;
        m_strType = L"FREQUENCYUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".ILLUMINANCEUNIT.") == 0) {
        m_iType = ILLUMINANCEUNIT;
        m_strType = L"ILLUMINANCEUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".INDUCTANCEUNIT.") == 0) {
        m_iType = INDUCTANCEUNIT;
        m_strType = L"INDUCTANCEUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".LENGTHUNIT.") == 0) {
        m_iType = LENGTHUNIT;
        m_strType = L"LENGTHUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".LUMINOUSFLUXUNIT.") == 0) {
        m_iType = LUMINOUSFLUXUNIT;
        m_strType = L"LUMINOUSFLUXUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".LUMINOUSINTENSITYUNIT.") == 0) {
        m_iType = LUMINOUSINTENSITYUNIT;
        m_strType = L"LUMINOUSINTENSITYUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".MAGNETICFLUXDENSITYUNIT.") == 0) {
        m_iType = MAGNETICFLUXDENSITYUNIT;
        m_strType = L"MAGNETICFLUXDENSITYUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".MAGNETICFLUXUNIT.") == 0) {
        m_iType = MAGNETICFLUXUNIT;
        m_strType = L"MAGNETICFLUXUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".MASSUNIT.") == 0) {
        m_iType = MASSUNIT;
        m_strType = L"MASSUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".PLANEANGLEUNIT.") == 0) {
        m_iType = PLANEANGLEUNIT;
        m_strType = L"PLANEANGLEUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".POWERUNIT.") == 0) {
        m_iType = POWERUNIT;
        m_strType = L"POWERUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".PRESSUREUNIT.") == 0) {
        m_iType = PRESSUREUNIT;
        m_strType = L"PRESSUREUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".RADIOACTIVITYUNIT.") == 0) {
        m_iType = RADIOACTIVITYUNIT;
        m_strType = L"RADIOACTIVITYUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".SOLIDANGLEUNIT.") == 0) {
        m_iType = SOLIDANGLEUNIT;
        m_strType = L"SOLIDANGLEUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".THERMODYNAMICTEMPERATUREUNIT.") == 0) {
        m_iType = THERMODYNAMICTEMPERATUREUNIT;
        m_strType = L"THERMODYNAMICTEMPERATUREUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".TIMEUNIT.") == 0) {
        m_iType = TIMEUNIT;
        m_strType = L"TIMEUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".VOLUMEUNIT.") == 0) {
        m_iType = VOLUMEUNIT;
        m_strType = L"VOLUMEUNIT";
        return;
    }

    if (wcscmp(szUnitType, L".USERDEFINED.") == 0) {
        m_iType = USERDEFINED;
        m_strType = L"USERDEFINED";
        return;
    }

    m_iType = UNKNOWN;
    m_strType = L"UNKNOWN";
}

void _ifc_unit::convertPrefix(const wchar_t* szPrefix)
{
    if (szPrefix == nullptr) {
        return;
    }

    if (wcscmp(szPrefix, L".EXA.") == 0) {
        m_strPrefix = L"Exa";
        m_strPrefixSymbol = L"E";
        return;
    }

    if (wcscmp(szPrefix, L".PETA.") == 0) {
        m_strPrefix = L"Peta";
        m_strPrefixSymbol = L"P";
        return;
    }

    if (wcscmp(szPrefix, L".TERA.") == 0) {
        m_strPrefix = L"Tera";
        m_strPrefixSymbol = L"T";
        return;
    }

    if (wcscmp(szPrefix, L".GIGA.") == 0) {
        m_strPrefix = L"Giga";
        m_strPrefixSymbol = L"G";
        return;
    }

    if (wcscmp(szPrefix, L".MEGA.") == 0) {
        m_strPrefix = L"Mega";
        m_strPrefixSymbol = L"M";
        return;
    }

    if (wcscmp(szPrefix, L".KILO.") == 0) {
        m_strPrefix = L"Kilo";
        m_strPrefixSymbol = L"k";
        return;
    }

    if (wcscmp(szPrefix, L".HECTO.") == 0) {
        m_strPrefix = L"Hecto";
        m_strPrefixSymbol = L"h";
        return;
    }

    if (wcscmp(szPrefix, L".DECA.") == 0) {
        m_strPrefix = L"Deca";
        m_strPrefixSymbol = L"da";
        return;
    }

    if (wcscmp(szPrefix, L".DECI.") == 0) {
        m_strPrefix = L"Deci";
        m_strPrefixSymbol = L"d";
        return;
    }

    if (wcscmp(szPrefix, L".CENTI.") == 0) {
        m_strPrefix = L"Centi";
        m_strPrefixSymbol = L"c";
        return;
    }

    if (wcscmp(szPrefix, L".MILLI.") == 0) {
        m_strPrefix = L"Milli";
        m_strPrefixSymbol = L"m";
        return;
    }

    if (wcscmp(szPrefix, L".MICRO.") == 0) {
        m_strPrefix = L"Micro";
        m_strPrefixSymbol = L"μ";
        return;
    }

    if (wcscmp(szPrefix, L".NANO.") == 0) {
        m_strPrefix = L"Nano";
        m_strPrefixSymbol = L"n";
        return;
    }

    if (wcscmp(szPrefix, L".PICO.") == 0) {
        m_strPrefix = L"Pico";
        m_strPrefixSymbol = L"p";
        return;
    }

    if (wcscmp(szPrefix, L".FEMTO.") == 0) {
        m_strPrefix = L"Femto";
        m_strPrefixSymbol = L"f";
        return;
    }

    if (wcscmp(szPrefix, L".ATTO.") == 0) {
        m_strPrefix = L"Atto";
        m_strPrefixSymbol = L"a";
        return;
    }
}

void _ifc_unit::convertName(const wchar_t* szName)
{
    assert(szName != nullptr);

    if (wcscmp(szName, L".AMPERE.") == 0) {
        m_strName = L"Ampere";
        m_strNameSymbol = L"A";
        return;
    }

    if (wcscmp(szName, L".BECQUEREL.") == 0) {
        m_strName = L"Becquerel";
        m_strNameSymbol = L"Bq";
        return;
    }

    if (wcscmp(szName, L".CANDELA.") == 0) {
        m_strName = L"Candela";
        m_strNameSymbol = L"cd";
        return;
    }

    if (wcscmp(szName, L".COULOMB.") == 0) {
        m_strName = L"Coulomb";
        m_strNameSymbol = L"C";
        return;
    }

    if (wcscmp(szName, L".CUBIC_METRE.") == 0) {
        m_strName = L"Cubic Metre";
        m_strNameSymbol = L"m3";
        //m_strNameSymbol = L"m³";
        return;
    }

    if (wcscmp(szName, L".DEGREE_CELSIUS.") == 0) {
        m_strName = L"Degree Celcius";
        m_strNameSymbol = L"DEGREE_CELSIUS"; // XKT
        //m_strNameSymbol = L"°C";
        return;
    }

    if (wcscmp(szName, L".FARAD.") == 0) {
        m_strName = L"Farad";
        m_strNameSymbol = L"F";
        return;
    }

    if (wcscmp(szName, L".GRAM.") == 0) {
        m_strName = L"Gram";
        m_strNameSymbol = L"g";
        return;
    }

    if (wcscmp(szName, L".GRAY.") == 0) {
        m_strName = L"Gray";
        m_strNameSymbol = L"Gy";
        return;
    }

    if (wcscmp(szName, L".HENRY.") == 0) {
        m_strName = L"Henry";
        m_strNameSymbol = L"H";
        return;
    }

    if (wcscmp(szName, L".HERTZ.") == 0) {
        m_strName = L"Hertz";
        m_strNameSymbol = L"Hz";
        return;
    }

    if (wcscmp(szName, L".JOULE.") == 0) {
        m_strName = L"Joule";
        m_strNameSymbol = L"J";
        return;
    }

    if (wcscmp(szName, L".KELVIN.") == 0) {
        m_strName = L"Kelvin";
        m_strNameSymbol = L"K";
        return;
    }

    if (wcscmp(szName, L".LUMEN.") == 0) {
        m_strName = L"Lumen";
        m_strNameSymbol = L"lm";
        return;
    }

    if (wcscmp(szName, L".LUX.") == 0) {
        m_strName = L"Lux";
        m_strNameSymbol = L"lx";
        return;
    }

    if (wcscmp(szName, L".METRE.") == 0) {
        m_strName = L"Metre";
        m_strNameSymbol = L"m";
        return;
    }

    if (wcscmp(szName, L".MOLE.") == 0) {
        m_strName = L"Mole";
        m_strNameSymbol = L"mol";
        return;
    }

    if (wcscmp(szName, L".NEWTON.") == 0) {
        m_strName = L"Newton";
        m_strNameSymbol = L"N";
        return;
    }

    if (wcscmp(szName, L".OHM.") == 0) {
        m_strName = L"Ohm";
        m_strNameSymbol = L"Ω";
        return;
    }

    if (wcscmp(szName, L".PASCAL.") == 0) {
        m_strName = L"Pascal";
        m_strNameSymbol = L"Pa";
        return;
    }

    if (wcscmp(szName, L".RADIAN.") == 0) {
        m_strName = L"Radian";
        m_strNameSymbol = L"rad";
        return;
    }

    if (wcscmp(szName, L".SECOND.") == 0) {
        m_strName = L"Second";
        m_strNameSymbol = L"s";
        return;
    }

    if (wcscmp(szName, L".SIEMENS.") == 0) {
        m_strName = L"Siemens";
        m_strNameSymbol = L"S";
        return;
    }

    if (wcscmp(szName, L".SIEVERT.") == 0) {
        m_strName = L"Sievert";
        m_strNameSymbol = L"Sv";
        return;
    }

    if (wcscmp(szName, L".SQUARE_METRE.") == 0) {
        m_strName = L"Square Metre";
        m_strNameSymbol = L"m2"; // XKT
        //m_strNameSymbol = L"m²";
        return;
    }

    if (wcscmp(szName, L".STERADIAN.") == 0) {
        m_strName = L"Steradian";
        m_strNameSymbol = L"sr";
        return;
    }

    if (wcscmp(szName, L".TESLA.") == 0) {
        m_strName = L"Tesla";
        m_strNameSymbol = L"T";
        return;
    }

    if (wcscmp(szName, L".VOLT.") == 0) {
        m_strName = L"Volt";
        m_strNameSymbol = L"V";
        return;
    }

    if (wcscmp(szName, L".WATT.") == 0) {
        m_strName = L"Watt";
        m_strNameSymbol = L"W";
        return;
    }

    if (wcscmp(szName, L".WEBER.") == 0) {
        m_strName = L"Weber";
        m_strNameSymbol = L"Wb";
        return;
    }

    m_strName = szName;
}

// ************************************************************************************************
_ifc_unit_provider::_ifc_unit_provider(SdaiModel iModel)
    : m_sdaiModel(iModel)
    , m_mapUnits()
{
    assert(m_sdaiModel != 0);

    load();
}

/*virtual*/ _ifc_unit_provider::~_ifc_unit_provider()
{
    for (auto itUnit : m_mapUnits) {
        delete itUnit.second;
    }
}

const _ifc_unit* _ifc_unit_provider::getUnit(const wchar_t* szUnit) const
{
    auto itUnit = m_mapUnits.find(szUnit);
    if (itUnit != m_mapUnits.end()) {
        return itUnit->second;
    }

    return nullptr;
}

tuple<wstring, wstring, wstring, wstring> _ifc_unit_provider::getQuantity(SdaiInstance sdaiQuantityInstance, const char* szValueName, const wchar_t* szUnitName) const
{
    wchar_t* szQuantityName = nullptr;
    sdaiGetAttrBN(sdaiQuantityInstance, "Name", sdaiUNICODE, &szQuantityName);

    wchar_t* szQuantityDescription = nullptr;
    sdaiGetAttrBN(sdaiQuantityInstance, "Description", sdaiUNICODE, &szQuantityDescription);

    wchar_t* szValue = nullptr;
    sdaiGetAttrBN(sdaiQuantityInstance, szValueName, sdaiUNICODE, &szValue);

    wchar_t* szUnit = nullptr;
    sdaiGetAttrBN(sdaiQuantityInstance, "Unit", sdaiUNICODE, &szUnit);

    wstring strName = szQuantityName;

    wstring strDescription;
    if (szQuantityDescription != nullptr) {
        strDescription = szQuantityDescription;
    }

    wstring strValue = szValue != nullptr ? szValue : L"NA";

    wstring strUnit;
    if (szUnit != nullptr) {
        strUnit = szUnit;
    }
    else {
        auto itUnit = m_mapUnits.find(szUnitName);
        if (itUnit != m_mapUnits.end()) {
            strUnit = itUnit->second->getName();
        }
    }

    return tuple<wstring, wstring, wstring, wstring>(strName, strDescription, strValue, strUnit);
}

tuple<wstring, wstring, wstring, wstring> _ifc_unit_provider::getQuantityLength(SdaiInstance sdaiQuantityInstance) const
{
    return getQuantity(sdaiQuantityInstance, "LengthValue", L"LENGTHUNIT");
}

tuple<wstring, wstring, wstring, wstring> _ifc_unit_provider::getQuantityArea(SdaiInstance sdaiQuantityInstance) const
{
    return getQuantity(sdaiQuantityInstance, "AreaValue", L"AREAUNIT");
}

tuple<wstring, wstring, wstring, wstring> _ifc_unit_provider::getQuantityVolume(SdaiInstance sdaiQuantityInstance) const
{
    return getQuantity(sdaiQuantityInstance, "VolumeValue", L"VOLUMEUNIT");
}

tuple<wstring, wstring, wstring, wstring> _ifc_unit_provider::getQuantityCount(SdaiInstance sdaiQuantityInstance) const
{
    return getQuantity(sdaiQuantityInstance, "CountValue", L"");
}

tuple<wstring, wstring, wstring, wstring> _ifc_unit_provider::getQuantityWeight(SdaiInstance sdaiQuantityInstance) const
{
    return getQuantity(sdaiQuantityInstance, "WeigthValue", L"MASSUNIT");
}

tuple<wstring, wstring, wstring, wstring> _ifc_unit_provider::getQuantityTime(SdaiInstance sdaiQuantityInstance) const
{
    return getQuantity(sdaiQuantityInstance, "TimeValue", L"TIMEUNIT");
}

void _ifc_unit_provider::load()
{
    SdaiAggr sdaiProjectAggr = sdaiGetEntityExtentBN(m_sdaiModel, "IFCPROJECT");

    SdaiInteger iMembersCount = sdaiGetMemberCount(sdaiProjectAggr);
    if (iMembersCount > 0) {
        SdaiInstance sdaiProjectInstance = 0;
        engiGetAggrElement(sdaiProjectAggr, 0, sdaiINSTANCE, &sdaiProjectInstance);

        if (sdaiProjectInstance != 0) {
            loadUnits(sdaiProjectInstance);
        }
    }
}

void _ifc_unit_provider::loadUnits(SdaiInstance sdaiProjectInstance)
{
    assert(sdaiProjectInstance != 0);

    SdaiInstance sdaiUnitsInContextInstance = 0;
    sdaiGetAttrBN(sdaiProjectInstance, "UnitsInContext", sdaiINSTANCE, &sdaiUnitsInContextInstance);

    SdaiAggr sdaiUnitsAggr = nullptr;
    sdaiGetAttrBN(sdaiUnitsInContextInstance, "Units", sdaiAGGR, &sdaiUnitsAggr);

    SdaiInteger iMembersCount = sdaiGetMemberCount(sdaiUnitsAggr);
    for (SdaiInteger iMember = 0; iMember < iMembersCount; iMember++) {
        SdaiInstance sdaiAggrInstance = 0;
        engiGetAggrElement(sdaiUnitsAggr, iMember, sdaiINSTANCE, &sdaiAggrInstance);

        if (sdaiGetInstanceType(sdaiAggrInstance) == sdaiGetEntity(m_sdaiModel, "IFCCONVERSIONBASEDUNIT")) {
            SdaiInstance sdaiConversionFactorInstance = 0;
            sdaiGetAttrBN(sdaiAggrInstance, "ConversionFactor", sdaiINSTANCE, &sdaiConversionFactorInstance);

            if (sdaiConversionFactorInstance != 0) {
                SdaiInstance sdaiUnitComponentInstance = 0;
                sdaiGetAttrBN(sdaiConversionFactorInstance, "UnitComponent", sdaiINSTANCE, &sdaiUnitComponentInstance);

                if (sdaiGetInstanceType(sdaiUnitComponentInstance) == sdaiGetEntity(m_sdaiModel, "IFCSIUNIT")) {
                    wchar_t* szUnitType = nullptr;
                    sdaiGetAttrBN(sdaiUnitComponentInstance, "UnitType", sdaiUNICODE, &szUnitType);

                    wchar_t* szPrefix = nullptr;
                    sdaiGetAttrBN(sdaiUnitComponentInstance, "Prefix", sdaiUNICODE, &szPrefix);

                    wchar_t* szName = nullptr;
                    sdaiGetAttrBN(sdaiUnitComponentInstance, "Name", sdaiUNICODE, &szName);

                    _ifc_unit* pUnit = new _ifc_unit(sdaiUnitComponentInstance, szUnitType, szPrefix, szName);
                    m_mapUnits[pUnit->getType()] = pUnit;
                }
                else {
                    assert(FALSE);
                }
            } // if (sdaiConversionFactorInstance != 0) 
            else {
                assert(FALSE);
            }
        } // IFCCONVERSIONBASEDUNIT
        else {
            if (sdaiGetInstanceType(sdaiAggrInstance) == sdaiGetEntity(m_sdaiModel, "IFCSIUNIT")) {
                wchar_t* szUnitType = nullptr;
                sdaiGetAttrBN(sdaiAggrInstance, "UnitType", sdaiUNICODE, &szUnitType);

                wchar_t* szPrefix = nullptr;
                sdaiGetAttrBN(sdaiAggrInstance, "Prefix", sdaiUNICODE, &szPrefix);

                wchar_t* szName = nullptr;
                sdaiGetAttrBN(sdaiAggrInstance, "Name", sdaiUNICODE, &szName);

                auto pUnit = new _ifc_unit(sdaiAggrInstance, szUnitType, szPrefix, szName);
                m_mapUnits[pUnit->getType()] = pUnit;
            } // IFCSIUNIT
        } // else IFCCONVERSIONBASEDUNIT
    } // for (SdaiInteger iMember = ...
}

