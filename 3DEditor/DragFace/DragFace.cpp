#include "stdafx.h"
#include "_rdf_mvc.h"

static void TestPoint(_rdf_controller* controller,
	OwlModel model, double pt[3], double size)
{
	auto sphere = GEOM::Sphere::Create(model);
	sphere.set_radius(size);

	auto T = GEOM::Matrix::Create(model);
	T.set__41(pt[0]);
	T.set__42(pt[1]);
	T.set__43(pt[2]);

	auto trans = GEOM::Transformation::Create(model);
	trans.set_object(sphere);
	trans.set_matrix(T);

	controller->addInstances(nullptr, // update all views
		vector<OwlInstance>{(OwlInstance)sphere, (OwlInstance)trans});
}

extern void DragFace(
	_rdf_controller* controller,
	OwlInstance		instance,
	int				iConceptualFace,
	double			startDragPoint[3],
	double          targetRayOrg[3],
	double          tagretRayDir[3]
)
{
	auto model = GetModel(instance);

	double box[6] = { 0,0,0,0,0,0 };
	GetBoundingBox(instance, box, box + 3);

	double size = 0;
	for (int i = 0; i < 3; i++) {
		size = max (size, (box[i + 3] - box[i]) / 5);
	}

	TestPoint(controller, model, startDragPoint, size);
}