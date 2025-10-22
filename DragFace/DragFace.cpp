#include "pch.h"
#include "DragFace.h"
#include "GeomTypes.h"

static Segment3D ProjectCubeOntoLine(
    double          box[6],
    const Point3d&  linePt,
    const Vector3d& lineDir)
{
    // Segment range
    double tmm[2] = { std::numeric_limits<double>::max(),  std::numeric_limits<double>::lowest() };

    // project 8 cube vertices
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {

                auto vert = make_point(
                    (i ? box[0] : box[3]),
                    (j ? box[1] : box[4]),
                    (k ? box[2] : box[5])
                );

                Vector3d diff = linePt - vert;
                double t = dot_product(diff, lineDir);
                if (t < tmm[0]) tmm[0] = t;
                if (t > tmm[1]) tmm[1] = t;
            }
        }
    }

    // 
    Segment3D segment;
    segment.pt[0] = linePt + lineDir * tmm[0];
    segment.pt[1] = linePt + lineDir * tmm[1];

    //check
    auto dir = segment.Direction();
    auto collinear = dot_product(dir, lineDir);
    assert(fabs(fabs(collinear) - 1) < 1e-7);

    return segment;
}

static OwlInstance DrawPoint(OwlModel model, Point3d const& pt, double size)
{
    char name[256];
    sprintf_s(name, "(%g, %g, %g)", pt.x, pt.y, pt.z);

	auto sphere = GEOM::Sphere::Create(model, name);
	sphere.set_radius(size);
	sphere.set_segmentationParts(36);

	auto T = GEOM::Matrix::Create(model);
	T.set__41(pt[0]);
	T.set__42(pt[1]);
	T.set__43(pt[2]);

	auto trans = GEOM::Transformation::Create(model, name);
	trans.set_object(sphere);
	trans.set_matrix(T);

	return trans;
}

static OwlInstance DrawPoints(OwlModel model, Point3d const* rpt, int npt, double size)
{
    std::vector<OwlInstance> rinst;
    
    for (int i = 0; i < npt; i++) {
        rinst.push_back(DrawPoint(model, rpt[i], size));
    }

    auto collection = GEOM::Collection::Create(model);
    collection.set_objects(rinst.data(), npt);

    return collection;
}

extern OwlInstance DragFace(
    OwlInstance					instance,
    int							iConceptualFace,
    double						startDragPoint[3],
    double						endDragLine[6]
)
{
    double box[6] = { 0,0,0,0,0,0 };
    GetBoundingBox(instance, box, box + 3);

    double size = 0;
    for (int i = 0; i < 3; i++) {
        size = max(size, (box[i + 3] - box[i]) / 15);
    }

    auto model = GetModel(instance);

    Point3d pt[3];
    pt[0] = MakePoint(startDragPoint);
    pt[1] = MakePoint(endDragLine);
    pt[2] = MakePoint(endDragLine + 3);

    auto vec = pt[2] - pt[1];
    //normalize(vec);     
    pt[2] = pt[1] - vec;

    //auto endRayDir = endPt2 - endPt1;

    //auto segment = ProjectCubeOntoLine(box, endPt1, endRayDir);

	return DrawPoints(model, pt+1, 2, size);
}