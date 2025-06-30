
#include "pch.h"

#include "conceptFacetSurface.h"
#include "conceptPointSet.h"

extern void ExtensionConceptsLoad(OwlModel model)
{
    PointSet::CreateClass(model);
    FacetSurface::CreateClass(model);
}
