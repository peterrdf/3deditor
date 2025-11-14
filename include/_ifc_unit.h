#ifndef IFCUNIT_H
#define IFCUNIT_H

#ifdef _WINDOWS
#include "engine.h"
#include "ifcengine.h"
#else
#include "../../include/engine.h"
#include "../../include/ifcengine.h"
#endif

#include <string>
#include <tuple>
#include <map>
using namespace std;

// ************************************************************************************************
#define	UNKNOWN	0
#define	ABSORBEDDOSEUNIT	101
#define	AREAUNIT	102
#define	DOSEEQUIVALENTUNIT	103
#define	ELECTRICCAPACITANCEUNIT	104
#define	ELECTRICCHARGEUNIT	105
#define	ELECTRICCONDUCTANCEUNIT	106
#define	ELECTRICCURRENTUNIT	107
#define	ELECTRICRESISTANCEUNIT	108
#define	ELECTRICVOLTAGEUNIT	109
#define	ENERGYUNIT	110
#define	FORCEUNIT	111
#define	FREQUENCYUNIT	112
#define	ILLUMINANCEUNIT	113
#define	INDUCTANCEUNIT	114
#define	LENGTHUNIT	115
#define	LUMINOUSFLUXUNIT	116
#define	LUMINOUSINTENSITYUNIT	117
#define	MAGNETICFLUXDENSITYUNIT	118
#define	MAGNETICFLUXUNIT	119
#define	MASSUNIT	120
#define	PLANEANGLEUNIT	121
#define	POWERUNIT	122
#define	PRESSUREUNIT	123
#define	RADIOACTIVITYUNIT	124
#define	SOLIDANGLEUNIT	125
#define	THERMODYNAMICTEMPERATUREUNIT	126
#define	TIMEUNIT	127
#define	VOLUMEUNIT	128
#define	USERDEFINED	129

// ************************************************************************************************
class _ifc_unit
{

private: // Members	

	SdaiInstance m_sdaiInstance;
    int m_iType;
    wstring m_strType;
    wstring m_strPrefix;
    wstring m_strPrefixSymbol;
    wstring m_strName;
    wstring m_strNameSymbol;

public: // Methods

    _ifc_unit(SdaiInstance sdaiInstance, const wchar_t* szType, const wchar_t* szPrefix, const wchar_t* szName);
    virtual ~_ifc_unit();

public: // Properties

	SdaiInstance getSdaiInstance() const { return m_sdaiInstance; }
    const wstring& getType() const { return m_strType; }
    const wstring& getPrefix() const { return m_strPrefix; }
    const wstring& getPrefixSymbol() const { return m_strPrefixSymbol; }
    const wstring& getName() const { return m_strName; }
    const wstring& getNameSymbol()  const { return m_strNameSymbol; }
    wstring getUnit() const; // [PREFIX][SPACE][NAME], e.g. 'Square Metre'
    wstring getUnitSymbol() const; // [PREFIX SYMBOL][SYMBOL], e.g. 'm2'

protected: // Methods

    void convertType(const wchar_t* szUnitType);
    void convertPrefix(const wchar_t* szPrefix);
    void convertName(const wchar_t* szName);
};

// ************************************************************************************************
class _ifc_unit_provider
{

private: // Members

    SdaiModel m_sdaiModel;
    map<wstring, _ifc_unit*> m_mapUnits;

public: // Methods

    _ifc_unit_provider(SdaiModel iModel);
    virtual ~_ifc_unit_provider();

    const _ifc_unit* getUnit(const wchar_t* szUnit) const;

    tuple<wstring, wstring, wstring, wstring> getQuantity(SdaiInstance sdaiQuantityInstance, const char* szValueName, const wchar_t* szUnitName) const;
    tuple<wstring, wstring, wstring, wstring> getQuantityLength(SdaiInstance sdaiQuantityInstance) const;
    tuple<wstring, wstring, wstring, wstring> getQuantityArea(SdaiInstance sdaiQuantityInstance) const;
    tuple<wstring, wstring, wstring, wstring> getQuantityVolume(SdaiInstance sdaiQuantityInstance) const;
    tuple<wstring, wstring, wstring, wstring> getQuantityCount(SdaiInstance sdaiQuantityInstance) const;
    tuple<wstring, wstring, wstring, wstring> getQuantityWeight(SdaiInstance sdaiQuantityInstance) const;
    tuple<wstring, wstring, wstring, wstring> getQuantityTime(SdaiInstance sdaiQuantityInstance) const;

protected: // Methods

    void load();
    void loadUnits(SdaiInstance sdaiProjectInstance);

public: // Properties

    const map<wstring, _ifc_unit*>& getUnits() const { return m_mapUnits; }
};

#endif // IFCUNIT_H
