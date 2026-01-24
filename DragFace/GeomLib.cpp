#include "pch.h"
#include "GeomLib.h"

//
//
template <typename Type> static Type* PTR(const Type& ref) { return const_cast<Type*>(&ref); }

//
//
static const MATRIX* GetCurrentTransform(
    const CONCEPTUAL_FACE&  cface,
    const MATRIX*           parentTransform,
    MATRIX&                 buffer
)
{
    if (auto locatTransform = rdfgeom_cface_GetLocalTransformation(PTR(cface))) {
        if (parentTransform) {
            MatrixMultiply(&buffer, parentTransform, locatTransform);
            return &buffer;
        }
        else {
            return locatTransform;
        }
    }
    else {
        return parentTransform;
    }

}

//
//
extern void DumpFace(
    const STRUCT_FACE&      face,
    const VECTOR3*          shellPoints,
    int_t                   numShellPoints,
    const MATRIX*           transform
)
{
    TRACE("Dump face 0x%p\n", &face);
    int i = 0;
    for (auto vertex = *rdfgeom_face_GetBoundary(PTR(face)); vertex; vertex = *rdfgeom_vertex_GetNext(vertex)) {
        VECTOR3 pt;
        GetVertexPoint(pt, vertex, shellPoints, numShellPoints, transform);
        TRACE("   [%d] = %g, %g, %g\n", i++, pt.x, pt.y, pt.z);
    }
    TRACE("-------\n");
}
//
//
extern bool GetVertexPoint(
    VECTOR3&                outPoint,
    const STRUCT_VERTEX*    vertex,
    const VECTOR3*          shellPoints,
    int_t                   numShellPoints,
    const MATRIX*           transform
)
{
    if (!vertex) {
        assert(false);
        return false;
    }

    int_t index = rdfgeom_vertex_GetPointIndex(const_cast<STRUCT_VERTEX*>(vertex));
    if (index < 0 || index >= numShellPoints) {
        assert(false);
        return false;
    }

    outPoint = shellPoints[index];

    if (transform) {
        Vec3Transform(&outPoint, transform);
    }

    return true;
}

//
//
extern bool FindFacePlane(
    PLANE&                  plane,
    const STRUCT_FACE&      face,
    const VECTOR3*          shellPoints,
    int_t                   numShellPoints,
    const MATRIX*           transform

)
{
    plane = { 0, 0, 0, 0 };

    auto vertex = *rdfgeom_face_GetBoundary(PTR(face));
    VECTOR3 pt0;
    if (!GetVertexPoint(pt0, vertex, shellPoints, numShellPoints, transform)){
        return false;
    }

    vertex = *rdfgeom_vertex_GetNext(vertex);
    VECTOR3 pt1;
    if (!GetVertexPoint(pt1, vertex, shellPoints, numShellPoints, transform)){
        return false;
    }

    auto e1 = pt1 - pt0;

    VECTOR3 normal = { 0, 0, 0 };
    for (vertex = *rdfgeom_vertex_GetNext(vertex); vertex; vertex = *rdfgeom_vertex_GetNext(vertex)) {
        VECTOR3 ptNext;
        if (!GetVertexPoint(ptNext, vertex, shellPoints, numShellPoints, transform)) {
            return false;
        }
        auto eNext = ptNext - pt0;

        VECTOR3 n;
        Vec3Cross(&n, &e1, &eNext);
        Vec3Add(&normal, &n);
    }

    double length = Vec3Length(&normal);
    if (length < LENGTH_TOLERANCE) {
        return false;
    }

    plane.a = normal.x / length;
    plane.b = normal.y / length;
    plane.c = normal.z / length;
    plane.d = -(plane.a * pt0.x + plane.b * pt0.y + plane.c * pt0.z);

    TRACE("Face plane: %g x + %g y + %g z + %g = 0\n", plane.a, plane.b, plane.c, plane.d);
    return true;
}

//
//
static void ProjectToCoordPlane(
    const VECTOR3&          xyz,
    CoordPlane              plane,
    VECTOR2&                uv
)
{
    switch (plane)
    {
        case CoordPlane::YZ:
            uv.u = xyz.y; 
            uv.v = xyz.z;
            break;
        case CoordPlane::XZ:
            uv.u = xyz.x;
            uv.v = xyz.z;
            break;
        default: // XY
            uv.u = xyz.x;
            uv.v = xyz.y;
            break;
    }
}

//
//
extern GeomPosition ClassifyPointToFaceFast(
    const VECTOR3&          pt,    
    const STRUCT_FACE&      face,
    const VECTOR3*          shellPoints,
    int_t                   numShellPoints,
    const MATRIX*           transform,
    PLANE*                  plane,
    double                  maxDistToPlane,   //= LENGTH_TOLERANCE, //distance to consider point in the face plane
    double*                 foundDistToPlane  //= NULL
)
{
    const double eps2D = 1e-9;

    //DumpFace(face, shellPoints, numShellPoints, transform);

    PLANE plane_;
    if (!plane) {
        plane = &plane_;
    }

    if (!FindFacePlane(*plane, face, shellPoints, numShellPoints, transform)) {
        return GeomPosition::Undefined;
    }

    //
    double dist = plane->a * pt.x + plane->b * pt.y + plane->c * pt.z + plane->d;

    if (foundDistToPlane) {
        *foundDistToPlane = dist;
    }

    if (dist > maxDistToPlane) {
        return GeomPosition::AbovePlane;
    }
    if (dist < -maxDistToPlane) {
        return GeomPosition::BelowPlane;
    }

    //Find the main projection (by the largest component of the normal)
    CoordPlane coord = CoordPlane::YZ;
    double nx = fabs (plane->a), ny = fabs (plane->b), nz = fabs (plane->c);
    if (ny > nx) coord = CoordPlane::XZ;
    if (nz > (coord == CoordPlane::YZ ? nx : ny)) coord = CoordPlane::XY;

    //Ray Casting
    VECTOR2 uv;
    ProjectToCoordPlane(pt, coord, uv);
    //TRACE("Projected point uv=(%g, %g) on %s plane\n", uv.u, uv.v, coord == CoordPlane::YZ ? "YZ" : (coord == CoordPlane::XZ ? "XZ" : "XY"));

    auto vertex = *rdfgeom_face_GetBoundary(PTR(face));
    VECTOR3 pt1;
    if (!GetVertexPoint(pt1, vertex, shellPoints, numShellPoints, transform)) {
        return GeomPosition::Undefined;
    }

    VECTOR2 uv1;
    ProjectToCoordPlane(pt1, coord, uv1);
    if (Vec2DistanceSqr(&uv, &uv1) < eps2D) {
        return GeomPosition::Vertex;
    }

    bool inside = false;
    for (vertex = *rdfgeom_vertex_GetNext(vertex); vertex; vertex = *rdfgeom_vertex_GetNext(vertex)) {

        VECTOR3 pt2;
        if (!GetVertexPoint(pt2, vertex, shellPoints, numShellPoints, transform)) {
            return GeomPosition::Undefined;
        }

        VECTOR2 uv2;
        ProjectToCoordPlane(pt2, coord, uv2);
        if (Vec2DistanceSqr(&uv, &uv2) < eps2D) {
            return GeomPosition::Vertex;
        }

        //TRACE("   uv1 =(%g, %g) uv2=(%g, %g)\n", uv1.u, uv1.v, uv2.u, uv2.v);

        // Ray intersect segment test
        if (fabs(uv1.v - uv2.v) < eps2D) {
            //horizontal segment
            if (fabs(uv.v - uv1.v) < eps2D) {
                assert(!"not tested");
                //point is on the same horizontal line
                if ((uv.u > min(uv1.u, uv2.u)) && (uv.u < max(uv1.u, uv2.u))) {
                    return GeomPosition::OnEdge;
                }
                else {
                    TRACE("same line but outside of edge - do not move uv1 and go to next vertex\n");
                }
            }
            else {
                //point is not on the line
                uv1 = uv2;
            }
        }
        else {
            //non-horizontal segment
            if ((uv1.v > uv.v) != (uv2.v > uv.v)) { 
                //horizontal line cross the segment
                auto u = (uv2.u - uv1.u) * (uv.v - uv1.v) / (uv2.v - uv1.v) + uv1.u;
                if (fabs(u-uv.u) < eps2D) {
                    return GeomPosition::OnEdge;
                }
                else if (u > uv.u) {
                    inside = !inside; //cross
                }
            }
            uv1 = uv2;
        }
    }

    return inside ? GeomPosition::Inside : GeomPosition::Outside;
}


#if 0
can produce 0
//
//
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
#endif

//
//
static void FindNormals(std::map<double, VECTOR3>& normals, const VECTOR3& pt, CONCEPTUAL_FACE& cface, const MATRIX* transform, double eps)
{
    MATRIX buffer;
    transform = GetCurrentTransform(cface, transform, buffer);

    if (auto inst = rdfgeom_cface_GetInstance(&cface)) {
        if (auto shell = rdfgeom_GetBRep(inst)) {
            if (const VECTOR3* points = rdfgeom_GetPoints(shell)) {
                if (int_t numPoints = rdfgeom_GetNumOfPoints(shell)) {

                    for (auto face = *rdfgeom_cface_GetFaces(PTR(cface)); face; face = *rdfgeom_face_GetNext(face)) {
                        PLANE plane;
                        double dist = 0;
                        auto pos = ClassifyPointToFaceFast(pt, *face, points, numPoints, transform, &plane, eps, &dist);
                        if (pos > GeomPosition::Outside) {
                            VECTOR3 normal = Vec3Make(plane.a, plane.b, plane.c);
                            normals[fabs(dist)] = normal;
                        }
                    }
                }
            }
        }
    }

    for (auto child = *rdfgeom_cface_GetChildren(PTR(cface)); child; child = *rdfgeom_cface_GetNext(child)) {
        FindNormals(normals, pt, *child, transform, eps);
    }
}

/// <summary>
/// 
/// </summary>
extern double GetMinIntersectionPosition(OwlInstance inst, const RAY3& ray, VECTOR3* ptMin)
{
    CalculateInstance(inst);

    std::vector<VECTOR3> intersections;
    IntersectLineInstance(intersections, ray, inst);

    double minDot = FLT_MAX;

    for (auto& pt : intersections) {
        auto vecToPoint = pt - ray.org;
        double dot = Vec3Dot(&ray.dir, &vecToPoint);

        if (dot < minDot) {
            minDot = dot;
            if (ptMin) {
                *ptMin = pt;
            }
        }
    }

    return minDot;
}

/// <summary>
/// 
/// </summary>
static void ProjectPointToInstance(VECTOR3& pt, const VECTOR3& normal, OwlInstance inst)
{
    RAY3 ray;
    ray.org = pt;
    ray.dir = normal;

    VECTOR3 snap;
    GetMinIntersectionPosition(inst, ray, &snap);

    ASSERT(Vec3Distance(&snap, &ray.org) < 1e-1);

    pt = snap;
}


//
//
extern bool FindNormal (
    VECTOR3&                outNormal,
    VECTOR3&                ptBase,                 //IN: the point close to surface where to find normal, OUT: projected to surface
    OwlInstance             inst,
    int                     iConceptualFace,  //= -1,  // -1 - search all faces
    double                  maxDistToSurface  //= LENGTH_TOLERANCE  //allowable distance from input ptBase to surface
)
{
    outNormal = Vec3Make(0, 0, 0);

    if (auto shell = rdfgeom_GetBRep(inst)) {

        std::map<double, VECTOR3> normals;

        auto cface = *rdfgeom_GetConceptualFaces(shell);
        if (iConceptualFace >= 0) {
            auto cnt = iConceptualFace;
            while (cnt && cface) {
                cface = *rdfgeom_cface_GetNext(cface);
                cnt--;
            }
        }

        for (; cface; cface = *rdfgeom_cface_GetNext(cface)) {
            FindNormals(normals, ptBase, *cface, NULL, maxDistToSurface);
            if (iConceptualFace >= 0)
                break;
        }

        if (!normals.empty()) {
            outNormal = normals.begin()->second;

            //correct starting point to be exactly on surface - as staring point comes with bad tolerance
            ProjectPointToInstance(ptBase, outNormal, inst);

            return true; //>>>> found normal
        }
    }
    
    return false;
}

static bool LineLineClosestPoints(
    SEGMENT3& closestPoints,
    const VECTOR3& A, //lineI A + t*u
    const VECTOR3& u,
    const VECTOR3& B, //lineII B + s*v
    const VECTOR3& v
)
{
    VECTOR3 w0 = A - B;

    double a = Vec3LengthSqr(&u);
    double b = Vec3Dot(&u, &v);
    double c = Vec3LengthSqr(&v);
    double d = Vec3Dot(&u, &w0);
    double e = Vec3Dot(&v, &w0);

    double D = a * c - b * b;

    double t, s;

    if (fabs(D) > ANGLE_TOLERANCE)
    {
        // non-parallel lines
        t = (b * e - c * d) / D;
        s = (a * e - b * d) / D;
    }
    else
    {
        // parallel lines
        if (a < ANGLE_TOLERANCE)
            return false;   // degenerate lines

        t = -d / a;
        s = 0.0;
    }

    closestPoints.pt[0] = A + u * t;
    closestPoints.pt[1] = B + v * s;

    return true;
}

//
//
extern bool LineLineClosestPoints(
    SEGMENT3&       closestPoints,
    const SEGMENT3& lineI,
    const SEGMENT3& lineII
)
{
    //lineI A + t*u
    const VECTOR3& A = lineI.pt[0];
    VECTOR3 u = lineI.pt[1] - lineI.pt[0];

    //lineII B + s*v
    const VECTOR3& B = lineII.pt[0];
    VECTOR3 v = lineII.pt[1] - lineII.pt[0];

    return LineLineClosestPoints(closestPoints, A, u, B, v);
}

//
//
extern bool LineLineClosestPoints(
    SEGMENT3& closestPoints,
    const SEGMENT3& lineI,
    const RAY3& lineII
)
{
    //lineI A + t*u
    const VECTOR3& A = lineI.pt[0];
    VECTOR3 u = lineI.pt[1] - lineI.pt[0];

    return LineLineClosestPoints(closestPoints, A, u, lineII.org, lineII.dir);
}


//
//
extern void IntersectLineInstance(
    std::vector<VECTOR3>&   outPoints,
    const RAY3&             line,
    OwlInstance             instance
)
{
    if (auto shell = rdfgeom_GetBRep(instance)) {
        for (auto cface = *rdfgeom_GetConceptualFaces(shell); cface; cface = *rdfgeom_cface_GetNext(cface)) {
            IntersectLineCFace(outPoints, line, *cface, NULL);
        }
    }
}

//
//
extern void IntersectLineCFace(
    std::vector<VECTOR3>&   outPoints,
    const RAY3&             line,
    const CONCEPTUAL_FACE&  cface,
    const MATRIX*           transform
)
{
    MATRIX buffer;
    transform = GetCurrentTransform(cface, transform, buffer);

    if (auto inst = rdfgeom_cface_GetInstance(PTR(cface))) {
        if (auto shell = rdfgeom_GetBRep(inst)) {
            if (auto shellPoints = rdfgeom_GetPoints(shell)) {
                if (auto numShellPoints = rdfgeom_GetNumOfPoints(shell)) {

                    for (auto face = *rdfgeom_cface_GetFaces(PTR(cface)); face; face = *rdfgeom_face_GetNext(face)) {
                        IntersectLineFace(outPoints, line, *face, shellPoints, numShellPoints, transform);
                    }
                }
            }
        }
    }

    for (auto child = *rdfgeom_cface_GetChildren(PTR(cface)); child; child = *rdfgeom_cface_GetNext(child)) {
        IntersectLineCFace(outPoints, line, *child, transform);
    }

}

//
//
extern void IntersectLineFace(
    std::vector<VECTOR3>&   outPoints,
    const RAY3&             line,
    const STRUCT_FACE&      face,
    const VECTOR3*          shellPoints,
    int_t                   numShellPoints,
    const MATRIX*           transform
)
{
    PLANE plane;
    if (FindFacePlane(plane, face, shellPoints, numShellPoints, transform)) {
        //Intersect line with plane
        double denom = plane.a * line.dir.x + plane.b * line.dir.y + plane.c * line.dir.z;
        if (fabs(denom) > ANGLE_TOLERANCE) {
            double t = -(plane.a * line.org.x + plane.b * line.org.y + plane.c * line.org.z + plane.d) / denom;
            VECTOR3 pt;
            pt = line.org + line.dir * t;
            //Classify point to face
            auto pos = ClassifyPointToFaceFast(pt, face, shellPoints, numShellPoints, transform);
            if (pos == GeomPosition::Inside || pos == GeomPosition::OnEdge || pos == GeomPosition::Vertex) {
                outPoints.push_back(pt);
            }
        }
    }
}

/// <summary>
/// Calculate distance from point to infinite line and return closest point on line
/// </summary>
extern double LinePointDistance(
    const SEGMENT3&  line,
    const VECTOR3&   point,
    VECTOR3*         linePoint
)
{
    VECTOR3 lineDir = line.pt[1] - line.pt[0];

    if (Vec3Normalize(lineDir) < LENGTH_TOLERANCE) {
        return FLT_MAX;
    }

    VECTOR3 ptDir = point - line.pt[0];
    double t = Vec3Dot(&ptDir, &lineDir);

    VECTOR3 buff;
    if (!linePoint) {
        linePoint = &buff;
    }   

    *linePoint = line.pt[0] + lineDir * t;

    return Vec3Distance(&point, linePoint);
}
