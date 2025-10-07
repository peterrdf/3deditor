#include "stdafx.h"
#include "_rdf_mvc.h"

#include <vector>
using namespace std;

static vector<OwlInstance> DrawPoint(OwlModel model, double pt[3], double size)
{
	auto sphere = GEOM::Sphere::Create(model);
	sphere.set_radius(size);
	sphere.set_segmentationParts(36);

	auto T = GEOM::Matrix::Create(model);
	T.set__41(pt[0]);
	T.set__42(pt[1]);
	T.set__43(pt[2]);

	auto trans = GEOM::Transformation::Create(model);
	trans.set_object(sphere);
	trans.set_matrix(T);

	return vector<OwlInstance>{ (OwlInstance)sphere, (OwlInstance)T, (OwlInstance)trans };
}

extern void DragFace(
	OwlInstance					instance,
	int							iConceptualFace,
	double						startDragPoint[3],
	double						targetRayOrg[3],
	double						tagretRayDir[3],
	std::vector<OwlInstance>& newInstances
)
{
	auto model = GetModel(instance);

	double box[6] = { 0,0,0,0,0,0 };
	GetBoundingBox(instance, box, box + 3);

	double size = 0;
	for (int i = 0; i < 3; i++) {
		size = max(size, (box[i + 3] - box[i]) / 15);
	}

	newInstances = DrawPoint(model, startDragPoint, size);
}