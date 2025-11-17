#include "pch.h"
#include "DragFace.h"
#include "GeomLib.h"

static SEGMENT3& TrimLineToBox(
    const SEGMENT3& line,
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

                auto diff = vert - line.pt[0];
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

    return segment;
}

static void TrimLineToSize(SEGMENT3& line, double size)
{
    VECTOR3 lineDir = line.pt[1] - line.pt[0];
    Vec3Normalize(lineDir);
    VECTOR3 pt0 = line.pt[0];
    VECTOR3 offset = lineDir * (size * 0.5);
    line.pt[0] = pt0 - offset;
    line.pt[1] = pt0 + offset;
}

static OwlInstance DrawPoint(OwlModel model, VECTOR3 const& pt, double size, const char* name)
{
    char full_name[256];
    sprintf_s(full_name, "%s (%g, %g, %g)", name, pt.x, pt.y, pt.z);

	auto sphere = GEOM::Sphere::Create(model, full_name);
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


static void ModifyInstance(OwlInstance instance, const SEGMENT3& directrix)
{
    RdfProperty prop = NULL;
    while (NULL!=(prop = GetInstancePropertyByIterator(instance, prop))) {
        
        auto propType = GetPropertyType(prop);
        if (propType == DATATYPEPROPERTY_TYPE_DOUBLE) {
        
            double* values = NULL;
            int_t card = 0;
            GetDatatypeProperty(instance, prop, (void**) &values, &card);
            if (card == 1 && fabs(values[0])>LENGTH_TOLERANCE) {

                double oldValue = values[0];
                double newValue = oldValue * 2;

                SetDatatypeProperty(instance, prop, &newValue, 1);
                CalculateInstance(instance);

            }
        }
    }
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

    std::vector<OwlInstance> debug;
    //debug.push_back(DrawPoint(model, startDragPoint, size, "start drag"));

    //SEGMENT3 line = TrimLineToBox(endDragLine, box);
    //debug.push_back(DrawPoint(model, line.pt[0], size, "end drag 1"));
    //debug.push_back(DrawPoint(model, line.pt[1], size, "end drag 2"));

    VECTOR3 normal;
    if (FindNormal(normal, startDragPoint, instance, iConceptualFace)) {
        SEGMENT3 normalLine;
        normalLine.pt[0] = startDragPoint;
        normalLine.pt[1] = startDragPoint + normal;

        SEGMENT3 closest;
        if (LineLineClosestPoints(closest, normalLine, endDragLine)) {
            //debug.push_back(DrawPoint(model, closest.pt[0], size, "normal closest"));
            //debug.push_back(DrawPoint(model, closest.pt[1], size, "end drag closest"));

            SEGMENT3 directrix;
            directrix.pt[0] = startDragPoint;
            directrix.pt[1] = closest.pt[1];
            debug.push_back(DrawPoint(model, directrix.pt[0], size, "start point"));
            debug.push_back(DrawPoint(model, directrix.pt[1], size, "final point"));

           //ModifyInstance(instance, directrix);
        }

        
    }
    
    auto collection = GEOM::Collection::Create(model);
    collection.set_objects(debug.data(), debug.size());
    return collection;
}