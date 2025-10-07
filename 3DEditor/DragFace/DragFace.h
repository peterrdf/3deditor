#pragma once

#include "_rdf_mvc.h"

extern void DragFace(
	OwlInstance					instance,
	int							iConceptualFace,
	double						startDragPoint[3],
	double						targetRayOrg[3],
	double						tagretRayDir[3],
	std::vector<OwlInstance>&	newInstances
);