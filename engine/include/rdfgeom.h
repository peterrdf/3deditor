//
//  Author:  Peter Bonsma
//  $Date: 2016-01-04 06:20:14 +0100 (Mon, 04 Jan 2016) $
//  $Revision: 31 $
//  Project: Geometry Kernel
//
//  In case of use of the DLL:
//  be aware of license fee for use of this DLL when used commercially
//  more info for commercial use:  peter.bonsma@rdf.bg
//

#ifndef __RDF_LTD__RDFGEOM_H
#define __RDF_LTD__RDFGEOM_H

#include "engine.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Geometry representation of instances
//
// This API intended to access B-Rep geometry of GeometricItem instances as textured polygons, wires and points.
// It can be used to get information from existing instances after CalulateInstance call
// or to describe and construct new concepts by specifying non-constructive (B-Rep) geometry 
// 

extern "C" {

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Data types
    //
    
    //
    // Point or direction in 3D space
    //
    struct VECTOR3
    {
        double	x, y, z;
    };

    //
    // 2D point, usually used for texture coordinates 
    //
    struct VECTOR2
    {
        double	u, v;
    };

    //
    // 3D transformation with affine matrix and translation vector 
    //
    struct MATRIX 
    {
	double	_11, _12, _13,
			_21, _22, _23,
			_31, _32, _33,
			_41, _42, _43;
    };

    //
    // B-Rep geometry of an instance
    // It has list of points and list of CONCEPTUAL_FACE parts.
    //
    struct SHELL;

    //
    // Part of B-rep geometry 
    // It can be a bounded surfaces, wires or points, or has nested (child) conceptual faces 
    // Bounded surface is represented as list of polygonal faces (STRUCT_FACE) 
    // which can be facet approximation when exact shape is curved.
    //
    struct CONCEPTUAL_FACE;
    
    //
    // Planar polygon
    // Face has outer boundary loop of vertices,
    // and list of openings represented as faces list.
    //
    struct STRUCT_FACE;
    
    //
    // Point in 3D space
    // It can be used for point in loop or separate (standalone) point in the shell
    //
    struct STRUCT_VERTEX;


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Access functions
    //

    //
    // Get B-Rep geometry of instance
    // The shell is accessible after call to CalculateInstance or within RDFGEOM_CALLBACK*.
    // It returns NULL when shell is inaccessible.
    //
    extern SHELL* rdfgeom_GetBRep(OwlInstance instance);

    //
    // Get number of points of the SHELL
    // It can return 0. 
    //
    extern int_t rdfgeom_GetNumOfPoints(SHELL* geometry);

    //
    // Get array of points of the shell
    // It can return NULL if shell has no points.
    // When it returns array, the size of the array is returned by rdfgeom_GetNumOfPoints
    // A caller can modify elements in the array.
    // Points can be transformed when used in conceptual faces.
    // 
    extern VECTOR3* rdfgeom_GetPoints(SHELL* geometry);

    //
    // Get array of normal directions at each point of the shell
    // It can return NULL when normals are not present.
    // When it returns array, the size of the array is returned by rdfgeom_GetNumOfPoints
    // A caller can modify elements in the array.
    // 
    extern VECTOR3* rdfgeom_GetNormals(SHELL* geometry);

    //
    // Get array of texture coordinates at each point of the shell
    // It can return NULL when texture is not present.
    // When it returns array, the size of the array is returned by rdfgeom_GetNumOfPoints
    // A caller can modify elements in the array.
    // 
    extern VECTOR2* rdfgeom_GetTextureCoordinates(SHELL* geometry);

    //
    // Allocate new points of the shell
    // The function is intended to be used in RDFGEOM_CALLBACK_INIT_SHELL.
    // It always allocates point coordinate array.
    // Normals and texture coordinates arrays are allocated with withNormals and withTextures flags are true.
    // When flag is false, related array will be set to NULL
    // Points in allocated arrays are undefined after the call.
    // Caller should call rdfgeom_GetPoints, rdfgeom_GetNormals, rdfgeom_GetTextureCoordinates and set values of coordinates. 
    //
    extern bool rdfgeom_AllocatePoints(OwlInstance inst, SHELL* geometry, int_t noOfPoints, bool withNormals, bool withTexture);

    //
    // Get iterator for conceptual faces
    // It can return NULL only if input argument is invalid.
    // To get pointer to conceptual face just dereference the iterator.
    // Dereferenced iterator can be a pointer to existing CONCEPTUAL_FACE or NULL for end of conceptual faces list.
    //
    extern CONCEPTUAL_FACE** rdfgeom_GetConceptualFaces(SHELL* geometry);

    //
    // Check iterator points to the end of conceptual face list
    //
    static bool rdfgeom_cface_EndOfList(CONCEPTUAL_FACE** cfaceP) { return !(cfaceP && *cfaceP); }

    //
    // Go iterator to next conceptual face
    // It returns NULL in input argument is NULL or wrong type.
    // For the last conceptual face it returns iterator, that can be used to add at the end of conceptual faces list
    //
    extern CONCEPTUAL_FACE** rdfgeom_cface_GetNext(CONCEPTUAL_FACE* cface);
    
    //
    // Creates new conceptual face at the position specified by the iterator cfaceP
    // If the iterator was not end of list, existing conceptual face will become next after the new
    // *cfaceP will point to newly created conceptual face
    // 
    extern void rdfgeom_cface_Create(OwlInstance inst, CONCEPTUAL_FACE** cfaceP);

    //
    // Get iterator for polygonal facets of the conceptual face
    // It can return NULL only if input argument is invalid.
    // To get pointer to face just dereference the iterator.
    // Dereferenced iterator can be a pointer to existing STRUCT_FACE or NULL for end of faces list.
    //
    extern STRUCT_FACE** rdfgeom_cface_GetFaces(CONCEPTUAL_FACE* cface);

    //
    // Get iterator for separate (standalone) points of the conceptual face
    // These are points not included in faces or wires to draw separately.
    // The list is empty for a manifold shell.
    // The function can return NULL only if input argument is invalid.
    // To get pointer to vertex just dereference the iterator.
    // Dereferenced iterator can be a pointer to existing STRUCT_VERTEX or NULL for end of vertices list.
    //
    extern STRUCT_VERTEX** rdfgeom_cface_GetVertices(CONCEPTUAL_FACE* cface);

    //
    // Get transformation for points used for the conceptual face
    // It returns NULL when no transformation is set (identity transformation).
    // Transformation for child (nested) conceptual faces are superposed.
    //
    extern MATRIX* rdfgeom_cface_GetLocalTranformation(CONCEPTUAL_FACE* cface);

    //
    // Get iterator for nested conceptual faces
    // It can return NULL only if input argument is invalid.
    // To get pointer to conceptual face just dereference the iterator.
    // Dereferenced iterator can be a pointer to existing CONCEPTUAL_FACE or NULL for end of conceptual faces list.
    // Child conceptual face can be a conceptual face of another instance.
    //
    extern CONCEPTUAL_FACE** rdfgeom_cface_GetChildren(CONCEPTUAL_FACE* cface);

    //
    // Get instance of the conceptual face
    //
    extern OwlInstance     rdfgeom_cface_GetInstance(CONCEPTUAL_FACE* cface);

    //
    // Check iterator points to the end of face list
    //
    static bool rdfgeom_face_EndOfList(STRUCT_FACE** faceP) { return !(faceP && *faceP); }

    //
    // Go iterator to next face
    // It returns NULL in input argument is NULL or wrong type.
    // For the last face it returns iterator, that can be used to add at the end of faces list
    //
    extern STRUCT_FACE** rdfgeom_face_GetNext(STRUCT_FACE* face);

    //
    // Creates new face at the position specified by the iterator faceP
    // If the iterator was not end of list, existing face will become next after the new
    // *faceP will point to newly created face
    // 
    extern void rdfgeom_face_Create(OwlInstance inst, STRUCT_FACE** faceP);

    //
    // Get boundary of the polygonal face
    // Boundary is a list of vertices and the function returns iterator to the list.
    // It can return NULL only if input argument is invalid.
    // To get pointer to vertex just dereference the iterator.
    // Dereferenced iterator can be a pointer to existing STRUCT_VERTEX or NULL for end of conceptual faces list.
    // 
    extern STRUCT_VERTEX** rdfgeom_face_GetBoundary(STRUCT_FACE* face);

    //
    // Get iterator for openings in the face
    // Each opening is represented as STRUCT_FACE.
    // It can return NULL only if input argument is invalid.
    // To get pointer to face just dereference the iterator.
    // Dereferenced iterator can be a pointer to existing STRUCT_FACE or NULL for end of faces list.
    //
    extern STRUCT_FACE** rdfgeom_face_GetOpenings(STRUCT_FACE* face);

    //
    // Check iterator points to the end of vertex list
    //
    static bool rdfgeom_vertex_EndOfList(STRUCT_VERTEX** vertexP) { return !(vertexP && *vertexP); }

    //
    // Go iterator to next vertex
    // It returns NULL in input argument is NULL or wrong type.
    // For the last vertex it returns iterator, that can be used to add at the end of vertex list
    //
    extern STRUCT_VERTEX** rdfgeom_vertex_GetNext(STRUCT_VERTEX* vertex);

    //
    // Creates new vertex at the position specified by the iterator vertexP
    // If the iterator was not end of list, existing vertex will become next after the new
    // *vertexP will point to newly created vertex
    // Geometrical location of the new vertex is a rdfgeom_GetPoints(shell)[pointIndex]
    // with local transformations specified for conceptual face and parents conceptual faces.
    // For the last point in the list caller should call the function with argument last = true.
    // For the boundary loops the last point must have the same pointIndex as first.
    // 
    extern void rdfgeom_vertex_Create(OwlInstance inst, STRUCT_VERTEX** vertexP, int_t pointIndex, bool last);

    //
    // Get index of the vertex
    // Geometrical location of the vertex is a rdfgeom_GetPoints(shell)[pointIndex]
    // with local transformations specified for conceptual face and parents conceptual faces.
    //
    extern int_t rdfgeom_vertex_GetPointIndex(STRUCT_VERTEX* vertex);
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Define geometry for custom concepts
    // To create new subclass of GeometricItem a caller will call CreateClass and define properties and parent class.
    // Next, caller should implement callbacks below to get bounding box and initialize B-Rep
    // and call rdfgeom_SetClassGeometry
    //

    //
    // The callback to get bounding box of a concept
    // Implementer should fill startVector and endVector.
    // transform may be NULL or given.
    // If transform is given, the implemented should fill the transform, either startVector and endVector should receive coordinate box.
    // The callback should return false if box is not known yet, and true when it returns values.
    // rdfgeom_GetBRep can be used in the call but will return NULL until CalculateInstance call.
    // clientData is any value passed to rdfgeom_SetClassGeometry
    //
    typedef bool (*RDFGEOM_CALLBACK_GET_BBOX)(OwlInstance inst, VECTOR3* startVector, VECTOR3* endVector, MATRIX* transform, void* clientData);

    //
    // The callback to create B-Rep of a concept
    // User rdfgeom_GetBRep to get SHELL to populate. It will return not NULL but empty representation (empty conceptual face list) 
    // Implemented should use rdfgeom_AllocatePoints and rdfgeom_*_Create to initialize geometry
    // All nested instances already have defined representation to the moment of the call, and their shells can be used.
    // clientData is any value passed to rdfgeom_SetClassGeometry
    //
    typedef void (*RDFGEOM_CALLBACK_INIT_SHELL)(OwlInstance inst, void* clientData);
    
    //
    // Set callbacks to define B-Rep geometry of custom concept
    // To define new GeometricItem the call should implement callbacks and call the function after CreateClass
    // clientData is any pointer and will be passed to callbacks
    //
    extern bool rdfgeom_SetClassGeometry(OwlClass cls, RDFGEOM_CALLBACK_INIT_SHELL fnInitRepr, RDFGEOM_CALLBACK_GET_BBOX fnGetBBox, void* clientData);

}

#endif
