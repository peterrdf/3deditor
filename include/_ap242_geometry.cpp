#include "_host.h"
#include "_ap242_geometry.h"

// ************************************************************************************************
_ap242_geometry::_ap242_geometry(OwlInstance owlInstance, SdaiInstance sdaiInstance)
    : _ap_geometry(owlInstance, sdaiInstance)
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
    setSegmentation(getSdaiModel(), 16, 0.);
}

/*virtual*/ void _ap242_geometry::postCalculate() /*override*/
{
    cleanCachedGeometry();
}