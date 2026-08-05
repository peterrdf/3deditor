#include "_host.h"
#include "_ap242_geometry.h"

// ************************************************************************************************
_ap242_geometry::_ap242_geometry(OwlInstance owlInstance, SdaiInstance sdaiInstance, MultiThreadOwlModelWrapper multiThreadOwlModelWrapper)
    : _ap_geometry(owlInstance, sdaiInstance, multiThreadOwlModelWrapper)
	, m_sdaiBuildContextInstance(0)
{
    calculate();
}

/*virtual*/ _ap242_geometry::~_ap242_geometry()
{}

/*virtual*/ void _ap242_geometry::preCalculate() /*override*/
{
    // Format
    setAPFormatSettings();

    // Extra settings
    if (getMultiThreadOwlModelWrapper() != 0) {
        setSegmentation(getMultiThreadOwlModelWrapper(), 16, 0.);
    }
    else {
        setSegmentation(getSdaiModel(), 16, 0.);
    }
}

/*virtual*/ void _ap242_geometry::postCalculate() /*override*/
{
    _geometry::cleanCachedGeometry();

    if (getMultiThreadOwlModelWrapper() != 0) {
        cleanMemory(getMultiThreadOwlModelWrapper(), 1);
    }
    else {
        cleanMemory(getSdaiModel(), 1);
    }
}