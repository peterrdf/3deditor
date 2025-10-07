#pragma once

#include "_rdf_mvc.h"

extern void DragFace(
	_rdf_controller* controller,
	OwlInstance		instance,
	int				iConceptualFace,
	double			startDragPoint[3],
	double          targetRayOrg[3],
	double          tagretRayDir[3]
);