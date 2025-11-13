#include "pch.h"
#include "DragFace.h"

static void TrimLineToBox(
    SEGMENT3&       line,
    double          box[6])
{
    VECTOR3 lineDir = line.pt[1] - line.pt[0];
    Vec3Normalize(lineDir);

    // Segment range
    double tmm[2] = { FLT_MAX,  -(FLT_MAX-1) };

    // project 8 cube vertices
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {

                auto vert = Vec3Make(
                    (i ? box[0] : box[3]),
                    (j ? box[1] : box[4]),
                    (k ? box[2] : box[5])
                );

                auto diff = line.pt[0] - vert;
                double t = Vec3Dot(diff, lineDir);
                if (t < tmm[0]) tmm[0] = t;
                if (t > tmm[1]) tmm[1] = t;
            }
        }
    }

    // 
    SEGMENT3 segment;
    segment.pt[0] = line.pt[0] + lineDir * tmm[0];
    segment.pt[1] = line.pt[1] + lineDir * tmm[1];

    //check
    auto dir = segment.pt[1]-segment.pt[0];
    Vec3Normalize(dir);
    auto collinear = Vec3Dot(&dir, &lineDir);
    assert(fabs(fabs(collinear) - 1) < 1e-7);

    line = segment;
}

static OwlInstance DrawPoint(OwlModel model, VECTOR3 const& pt, double size)
{
    char name[256];
    sprintf_s(name, "(%g, %g, %g)", pt.x, pt.y, pt.z);

	auto sphere = GEOM::Sphere::Create(model, name);
	sphere.set_radius(size);
	sphere.set_segmentationParts(36);

	auto T = GEOM::Matrix::Create(model);
	T.set__41(pt.x);
	T.set__42(pt.y);
	T.set__43(pt.z);

	auto trans = GEOM::Transformation::Create(model, name);
	trans.set_object(sphere);
	trans.set_matrix(T);

	return trans;
}

static OwlInstance DrawInput(OwlModel model, VECTOR3 const& pt, SEGMENT3& line, double size)
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
	VECTOR3 const&				startDragPoint,
	SEGMENT3 const&				endDragLine
)
{
    double box[6] = { 0,0,0,0,0,0 };
    GetBoundingBox(instance, box, box + 3);

    double size = 0;
    for (int i = 0; i < 3; i++) {
        size = max(size, (box[i + 3] - box[i]) / 30);
    }

    auto model = GetModel(instance);

    SEGMENT3 line = endDragLine;

    TrimLineToBox(line, box);

	return DrawInput(model, startDragPoint, line, size);
}