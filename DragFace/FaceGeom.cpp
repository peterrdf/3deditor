#include "pch.h"
#include "FaceGeom.h"

extern void DumpFace(
    const STRUCT_FACE&      face,
    const VECTOR3*          shellPoints,
    int_t                   numShellPoints,
    const MATRIX*           transform
)
{
    TRACE("Dump face 0x%p\n", &face);
    int i = 0;
    for (auto vertex = *rdfgeom_face_GetBoundary(const_cast<STRUCT_FACE*>(&face)); vertex; vertex = *rdfgeom_vertex_GetNext(vertex)) {
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
    STRUCT_VERTEX*          vertex,
    const VECTOR3*          shellPoints,
    int_t                   numShellPoints,
    const MATRIX*           transform
)
{
    if (!vertex) {
        assert(false);
        return false;
    }

    int_t index = rdfgeom_vertex_GetPointIndex(vertex);
    if (index < 0 || index >= numShellPoints) {
        assert(false);
        return false;
    }

    outPoint = shellPoints[index];

    if (transform) {
        assert(!"not tested");
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

    STRUCT_FACE* pface = const_cast<STRUCT_FACE*>(&face);

    auto vertex = *rdfgeom_face_GetBoundary(pface);
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
extern void ProjectToCoordPlane(
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
    double                  eps
)
{
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
    if (dist > eps) {
        return GeomPosition::AbovePlane;
    }
    if (dist < -eps) {
        assert(!"not tested. Expect start point is inside solid?");
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

    auto vertex = *rdfgeom_face_GetBoundary(const_cast<STRUCT_FACE*>(&face));
    VECTOR3 pt1;
    if (!GetVertexPoint(pt1, vertex, shellPoints, numShellPoints, transform)) {
        return GeomPosition::Undefined;
    }

    VECTOR2 uv1;
    ProjectToCoordPlane(pt1, coord, uv1);
    if (Vec2DistanceSqr(&uv, &uv1) < eps) {
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
        if (Vec2DistanceSqr(&uv, &uv2) < eps) {
            return GeomPosition::Vertex;
        }

        //TRACE("   uv1 =(%g, %g) uv2=(%g, %g)\n", uv1.u, uv1.v, uv2.u, uv2.v);

        // Ray intersect segment test
        if (fabs(uv1.v - uv2.v) < eps) {
            //horizontal segment
            if (fabs(uv.v - uv1.v) < eps) {
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
                if (fabs(u-uv.u) < eps) {
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