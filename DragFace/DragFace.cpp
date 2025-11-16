#include "pch.h"
#include "DragFace.h"
#include "FaceGeom.h"

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

static VECTOR3 AverageVector(std::list<VECTOR3>& lst)
{
    VECTOR3 average = Vec3Make(0, 0, 0);
    int cnt = 0;
    for (auto& pt : lst) {
        average = average + pt;
        cnt++;  
    }

    average = average * (1.0 / cnt);

    return average;
}

static void FindNormals(std::list<VECTOR3>& normals, const VECTOR3& pt, CONCEPTUAL_FACE* cface, VECTOR3* points, int_t numPoints, MATRIX* transform)
{    
    MATRIX currentTransform;
    if (auto locatTransform = rdfgeom_cface_GetLocalTransformation(cface)) {
        assert(!"not tested");
        if (transform) {
            MatrixMultiply(&currentTransform, transform, locatTransform);
            transform = &currentTransform;
        } else {
            transform = locatTransform;
        }
    }

    for (auto face = *rdfgeom_cface_GetFaces(cface); face; face = *rdfgeom_face_GetNext(face)) {
        PLANE plane;
        auto pos = ClassifyPointToFaceFast(pt, *face, points, numPoints, transform, &plane, 1e-1);
        if (pos > GeomPosition::Outside) {
            VECTOR3 normal = Vec3Make(plane.a, plane.b, plane.c);
            normals.push_back(normal);
        }
    }

    for (auto child = *rdfgeom_cface_GetChildren(cface); child; child = *rdfgeom_cface_GetNext(cface)) {
        FindNormals(normals, pt, child, points, numPoints, transform);
    }
}


static bool FindNormal (OwlInstance inst, int iConceptualFace, VECTOR3 const& pt, VECTOR3& outNormal)
{
    outNormal = Vec3Make(0, 0, 0);

    if (auto shell = rdfgeom_GetBRep(inst)) {
        if (auto points = rdfgeom_GetPoints(shell)) {
            auto numPoints = rdfgeom_GetNumOfPoints(shell);

            auto cface = *rdfgeom_GetConceptualFaces(shell);
            while (iConceptualFace && cface) {
                cface = *rdfgeom_cface_GetNext(cface);
                iConceptualFace--;
            }

            std::list<VECTOR3> normals;
            FindNormals(normals, pt, cface, points, numPoints, NULL);
            //for (cface = *rdfgeom_GetConceptualFaces(shell); cface; cface = *rdfgeom_cface_GetNext(cface)) {
            //    FindNormals(normals, pt, cface, points, numPoints, NULL);
            //}

            if (!normals.empty()) {
                outNormal = AverageVector(normals);
                Vec3Invert(&outNormal);
                return true; //>>>> found normal
            }
        }
    }
    assert(!"normal not found");
    return false;
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

    std::vector<OwlInstance> debug;
    debug.push_back(DrawPoint(model, startDragPoint, size, "start drag"));
    debug.push_back(DrawPoint(model, line.pt[0], size, "end drag"));
    debug.push_back(DrawPoint(model, line.pt[1], size, "end drag"));


    VECTOR3 normal;
    if (FindNormal(instance, iConceptualFace, startDragPoint, normal)) {
        auto target = startDragPoint + normal * (5*size);
        debug.push_back(DrawPoint(model, target, size, "normal"));
    }

    auto collection = GEOM::Collection::Create(model);
    collection.set_objects(debug.data(), debug.size());
    return collection;
}