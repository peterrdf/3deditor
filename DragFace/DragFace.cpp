#include "pch.h"
#include "DragFace.h"
#include "GeomTypes.h"

static void TrimLineToBox(
    Segment3D&      line,
    double          box[6])
{
    auto lineDir = line.Direction();

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

                Vector3d diff = line.pt[0] - vert;
                double t = dot_product(diff, lineDir);
                if (t < tmm[0]) tmm[0] = t;
                if (t > tmm[1]) tmm[1] = t;
            }
        }
    }

    // 
    Segment3D segment;
    segment.pt[0] = line.pt[0] + lineDir * tmm[0];
    segment.pt[1] = line.pt[1] + lineDir * tmm[1];

    //check
    auto dir = segment.Direction();
    auto collinear = dot_product(dir, lineDir);
    assert(fabs(fabs(collinear) - 1) < 1e-7);

    line = segment;
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

static OwlInstance DrawInput(OwlModel model, Point3d const& pt, Segment3D& line, double size)
{
    std::vector<OwlInstance> rinst;
    
    rinst.push_back(DrawPoint(model, pt, size));
    rinst.push_back(DrawPoint(model, line.pt[0], size));
    rinst.push_back(DrawPoint(model, line.pt[1], size));

    auto collection = GEOM::Collection::Create(model);
    collection.set_objects(rinst.data(), rinst.size());

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
        size = max(size, (box[i + 3] - box[i]) / 30);
    }

    auto model = GetModel(instance);

    Point3d ptStart;
    ptStart = MakePoint(startDragPoint);

    Segment3D line;
    line.pt[0] = MakePoint(endDragLine);
    line.pt[1] = MakePoint(endDragLine + 3);

    TrimLineToBox(line, box);

	return DrawInput(model, ptStart, line, size);
}