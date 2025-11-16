#pragma once

#define LENGTH_TOLERANCE 1e-8

/// <summary>
/// 
/// </summary>
enum class GeomPosition
{
    Undefined = 0,
    AbovePlane,
    BelowPlane,
    Outside,
    // > Outside when inside or on edge
    Vertex,
    OnEdge,
    // < Inside when outside or on edge
    Inside
};

/// <summary>
/// 
/// </summary>
enum class CoordPlane
{
    YZ = 0,
    XZ = 1,
    XY = 2
};

extern void DumpFace(
    const STRUCT_FACE&      face,
    const VECTOR3*          shellPoints,
    int_t                   numShellPoints,
    const MATRIX*           localTransform
);

/// <summary>
/// 
/// </summary>
extern bool GetVertexPoint(
    VECTOR3&                outPoint,
    STRUCT_VERTEX*          vertex,
    const VECTOR3*          shellPoints,
    int_t                   numShellPoints,
    const MATRIX*           localTransform
    );

/// <summary>
/// 
/// </summary>   
extern void ProjectToCoordPlane(
    const VECTOR3&          xyz,
    CoordPlane              plane,
    VECTOR2&                uv
);

/// <summary>
/// 
/// </summary>
extern bool FindFacePlane(
    PLANE&                  plane,
    const STRUCT_FACE&      face,
    const VECTOR3*          shellPoints,
    int_t                   numShellPoints,
    const MATRIX*           localTransform
);

/// <summary>
/// 
/// </summary>
extern GeomPosition ClassifyPointToFaceFast(
    const VECTOR3&          pt,    
    const STRUCT_FACE&      face,
    const VECTOR3*          shellPoints,
    int_t                   numShellPoints,
    const MATRIX*           localTransform,
    PLANE*                  plane = nullptr,
    double                  eps = LENGTH_TOLERANCE
);
