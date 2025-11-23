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


#include    "engine.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Geometry representation of instances
//
// This API intended to access B-Rep geometry of GeometricItem instances as textured polygons, wires and points.
// It can be used to get information from existing instances after CalculateInstance call
// or to describe and construct new concepts by specifying non-constructive (B-Rep) geometry 
// 

#ifdef __cplusplus
	extern "C" {
#endif

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

    struct SEGMENT3
    {
        VECTOR3    pt[2];
    };

    struct RAY3
    {
        VECTOR3    org;
        VECTOR3    dir;//normalized direction
    };

    //
    // 2D point, usually used for texture coordinates 
    //
    struct VECTOR2
    {
        double	u, v;
    };

    //
    struct PLANE
    {
        double	a, b, c, d;
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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

#ifdef __cplusplus
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Base geometry functions
    //

static	inline		double	Sqr(
										double		value
									)
{
	return value * value;
}

//
//	Vector2
//


void	Vec2Transform(
				VECTOR2						* pInOut,
				const MATRIX				* pM
			);

void	Vec2Transform(
				VECTOR2						* pOut,
				const VECTOR2				* pV,
				const MATRIX				* pM
			);


static	inline		void	Vec2Add(
										VECTOR2						* pOut,
										const VECTOR2				* pV1,
										const VECTOR2				* pV2
									)
{
	pOut->u = pV1->u + pV2->u;
	pOut->v = pV1->v + pV2->v;
}

static	inline		double	Vec2DistanceSqr(
										const VECTOR2				* pV1,
										const VECTOR2				* pV2
									)
{
	return Sqr(pV1->u - pV2->u) + Sqr(pV1->v - pV2->v);
}

static	inline		double	Vec2Distance(
										const VECTOR2				* pV1,
										const VECTOR2				* pV2
									)
{
	return sqrt(
					Vec2DistanceSqr(
							pV1,
							pV2
						)
				);
}

static	inline		double	Vec2Dot(
										const VECTOR2				* pV
									)
{
	return pV->u * pV->u + pV->v * pV->v;
}

static	inline		double	Vec2Dot(
										const VECTOR2				* pV1,
										const VECTOR2				* pV2
									)
{
	return pV1->u * pV2->u + pV1->v * pV2->v;
}

static	inline		double	Vec2Normalize(
										VECTOR2						* pInOut
									)
{
	double	size = Sqr(pInOut->u) + Sqr(pInOut->v);
	
	if (size > 0.0000000000000001) {
		double	sqrtSize = sqrt(size);

		pInOut->u /= sqrtSize;
		pInOut->v /= sqrtSize;

		return sqrtSize;
	}
	else {
		pInOut->u = 0.;
		pInOut->v = 0.;
		return 0.;
	}
}

//
//	Vector3
//

inline double* Vec3Coordinates(VECTOR3& vec) { return &(vec.x); }
inline const double* Vec3Coordinates(const VECTOR3& vec) { return &(vec.x); }

static inline void    Vec3Init(
							VECTOR3& vec,
							double	 x = 0,
							double   y = 0,
						    double   z = 0
						)
{
	vec.x = x;
	vec.y = y;
	vec.z = z;
}

static inline void    Vec3Init(
							VECTOR3&        vec,
							const double	xyz[3]
						)
{
	double	* coord = Vec3Coordinates(vec);
	for (int i = 0; i < 3; i++)
		coord[i] = xyz[i];
}

static inline VECTOR3 Vec3Make(
                            double	 x = 0,
                            double   y = 0,
                            double   z = 0
                        )
{
    VECTOR3 vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    return vec;
}

static inline VECTOR3 Vec3Make(
                            double    xyz[3]
                        )
{
    VECTOR3 vec;
    Vec3Init(vec, xyz);
    return vec;
}

static	inline		void	Vec3Identity(
										VECTOR3						* pV
									)
{
	pV->x = 0.;
	pV->y = 0.;
	pV->z = 0.;
}

static	inline		double	Vec3Normalize(
										VECTOR3						* pInOut
									)
{
	double	size = Sqr(pInOut->x) + Sqr(pInOut->y) + Sqr(pInOut->z);

	if (size > 0.0000000000000001) {
		double	sqrtSize = sqrt(size);

		pInOut->x /= sqrtSize;
		pInOut->y /= sqrtSize;
		pInOut->z /= sqrtSize;

		return sqrtSize;
	}
	else {
		pInOut->x = 0.;
		pInOut->y = 0.;
		pInOut->z = 0.;

		return 0.;
	}
}

static inline double Vec3Normalize (VECTOR3& vInOut)
{
    return Vec3Normalize(&vInOut);
}

static	inline		void	Vec3Invert(
										VECTOR3						* pV
									)
{
	pV->x = - pV->x;
	pV->y = - pV->y;
	pV->z = - pV->z;
}

//void	Vec3Transform(
//				VECTOR3						* pInOut,
//				const MATRIX				* pM
//			);


static inline void	Vec3Transform(
				VECTOR3			* pInOut,
				const MATRIX	* pM
			)
{
	double	x = pInOut->x * pM->_11 + pInOut->y * pM->_21 + pInOut->z * pM->_31 + pM->_41,
			y = pInOut->x * pM->_12 + pInOut->y * pM->_22 + pInOut->z * pM->_32 + pM->_42;
	pInOut->z = pInOut->x * pM->_13 + pInOut->y * pM->_23 + pInOut->z * pM->_33 + pM->_43;

	pInOut->x = x;
	pInOut->y = y;
}

static inline void	Vec3Transform(
				VECTOR3			* pOut,
				const VECTOR3	* pV,
				const MATRIX	* pM
			)
{
	VECTOR3	pTmp;

	pTmp.x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31 + pM->_41;
	pTmp.y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32 + pM->_42;
	pTmp.z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33 + pM->_43;

	pOut->x = pTmp.x;
	pOut->y = pTmp.y;
	pOut->z = pTmp.z;
}


void	Vec3TransformNoTranslation(
				VECTOR3						* pInOut,
				const MATRIX				* pM
			);

void	Vec3TransformNoTranslation(
				VECTOR3						* pOut,
				const VECTOR3				* pV,
				const MATRIX				* pM
			);

void	Vec3TransformNoTranslation(
				VECTOR3						* pOut,
				const VECTOR3				* pV,
				const MATRIX				* pM,
				bool						invertNormal
			);

static	inline		void	Vec3Add(
										VECTOR3						* pOut,
										const VECTOR3				* pV
									)
{
	pOut->x += pV->x;
	pOut->y += pV->y;
	pOut->z += pV->z;
}

static	inline		void	Vec3Add(
										VECTOR3						* pOut,
										const VECTOR3				* pV1,
										const VECTOR3				* pV2
									)
{
	pOut->x = pV1->x + pV2->x;
	pOut->y = pV1->y + pV2->y;
	pOut->z = pV1->z + pV2->z;
}

static inline VECTOR3 operator+(const VECTOR3& v1, const VECTOR3& v2)
{
    VECTOR3 result = v1;
    Vec3Add(&result, &v2);
    return result;
}

static	inline		void	Vec3Subtract(
										VECTOR3						* pInOut,
										const VECTOR3				* pV
									)
{
	pInOut->x -= pV->x;
	pInOut->y -= pV->y;
	pInOut->z -= pV->z;
}

static	inline		void	Vec3Subtract(
										VECTOR3						* pOut,
										const VECTOR3				* pV1,
										const VECTOR3				* pV2
									)
{
	assert(pOut != pV1);

	pOut->x = pV1->x - pV2->x;
	pOut->y = pV1->y - pV2->y;
	pOut->z = pV1->z - pV2->z;
}

static inline VECTOR3 operator-(const VECTOR3& v1, const VECTOR3& v2)
{
    VECTOR3 result = v1;
    Vec3Subtract(&result, &v2);
    return result;
}


static inline VECTOR3 operator*(const VECTOR3& v, double scalar)
{
    VECTOR3 result;
    result.x = v.x * scalar;
    result.y = v.y * scalar;
    result.z = v.z * scalar;
    return result;
}

static	inline		double	Vec3Dot(
										const VECTOR3				* pV1,
										const VECTOR3				* pV2
									)
{
	assert(pV1 != pV2);

	double	dotProduct = pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z;

	return dotProduct;
}

static inline   double Vec3Dot(const VECTOR3& v1, const VECTOR3& v2)
{
    return Vec3Dot(&v1, &v2);
}

static	inline		void	Vec3Cross(
										VECTOR3						* pOut,
										const VECTOR3				* pV1,
										const VECTOR3				* pV2
									)
{
	VECTOR3 v;

	v.x = pV1->y * pV2->z - pV1->z * pV2->y;
	v.y = pV1->z * pV2->x - pV1->x * pV2->z;
	v.z = pV1->x * pV2->y - pV1->y * pV2->x;

	pOut->x = v.x;
	pOut->y = v.y;
	pOut->z = v.z;
}

static	inline		double	Vec3DistanceSqr(
										const VECTOR3				* pV1,
										const VECTOR3				* pV2
									)
{
	return Sqr(pV1->x - pV2->x) + Sqr(pV1->y - pV2->y) + Sqr(pV1->z - pV2->z);
}

static  inline double Vec3dDistanceSqr(const VECTOR3& pt1, const VECTOR3& pt2) { return Vec3DistanceSqr(&pt1, &pt2); }

static	inline		double	Vec3Distance(
										const VECTOR3				* pV1,
										const VECTOR3				* pV2
									)
{
	return sqrt(
					Vec3DistanceSqr(
							pV1,
							pV2
						)
				);
}

static	inline		double	Vec3LengthSqr(
    const VECTOR3* pV
)
{
    return Sqr(pV->x) + Sqr(pV->y) + Sqr(pV->z);
}

static	inline		double	Vec3Length(
										const VECTOR3				* pV
									)
{
	return sqrt(Vec3LengthSqr(pV));
}

static inline bool Vec3IsUnit(const VECTOR3& v, double eps = 1e-7)
{
    return fabs(Vec3LengthSqr(&v)-1) < Sqr(eps);
}

static inline SEGMENT3 Seg3Make(const double coords[6])
{
    SEGMENT3 seg;
    Vec3Init(seg.pt[0], coords);
    Vec3Init(seg.pt[1], coords+3);
    return seg;
}

#ifdef __cplusplus
    extern "C" {
#endif

void	MatrixMultiply(
				MATRIX						* pOut,
				const MATRIX				* pM1,
				const MATRIX				* pM2
			);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Shell access functions
    //

    //
    // Get B-Rep geometry of instance
    // The shell is accessible after call to CalculateInstance or within RDFGEOM_CALLBACK*.
    // It returns NULL when shell is inaccessible.
    //
    extern SHELL* rdfgeom_GetBRep(OwlInstance instance);

    //
    // Sets calculated bounding box cached on instance
    // Transform T may be NULL or given.
    // If transform is given, the implemented should fill the transform, either startVector and endVector should receive World Coordinate box.
    // Pass all NULLs if box is not known yet.
    //
    extern void rdfgeom_SetBoundingBox(OwlInstance instance, VECTOR3* low, VECTOR3* high, MATRIX* T);

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

#ifdef __cplusplus
    }
#endif

    //
    // Check iterator points to the end of conceptual face list
    //
    static inline bool rdfgeom_cface_EndOfList(CONCEPTUAL_FACE** cfaceP) { return !(cfaceP && *cfaceP); }

#ifdef __cplusplus
    extern "C" {
#endif

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
    extern MATRIX* rdfgeom_cface_GetLocalTransformation(CONCEPTUAL_FACE* cface);

    //
    // Get iterator for nested conceptual faces
    // It can return NULL only if input argument is invalid.
    // To get pointer to conceptual face just dereference the iterator.
    // Dereferenced iterator can be a pointer to existing CONCEPTUAL_FACE or NULL for end of conceptual faces list.
    // Child conceptual face can be a conceptual face of another instance.
    //
    extern CONCEPTUAL_FACE** rdfgeom_cface_GetChildren(CONCEPTUAL_FACE* cface);

    //
    // Add children to conceptual faces.
    // Children are usually conceptual faces of another instance 
    //
    extern void rdfgeom_cface_SetChildren(CONCEPTUAL_FACE* cface, CONCEPTUAL_FACE* children);

    //
    // Get instance of the conceptual face
    //
    extern OwlInstance     rdfgeom_cface_GetInstance(CONCEPTUAL_FACE* cface);

#ifdef __cplusplus
    }
#endif

    //
    // Check iterator points to the end of face list
    //
    static inline bool rdfgeom_face_EndOfList(STRUCT_FACE** faceP) { return !(faceP && *faceP); }

#ifdef __cplusplus
    extern "C" {
#endif

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

#ifdef __cplusplus
    }
#endif

    //
    // Check iterator points to the end of vertex list
    //
    static inline bool rdfgeom_vertex_EndOfList(STRUCT_VERTEX** vertexP) { return !(vertexP && *vertexP); }

#ifdef __cplusplus
    extern "C" {
#endif

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
    // Implementer should call rdfgeom_SetBoundingBox.
    // rdfgeom_GetBRep can be used in the call but will return NULL until CalculateInstance call.
    // clientData is any value passed to rdfgeom_SetClassGeometry
    //
    typedef bool (*RDFGEOM_CALLBACK_GET_BBOX)(OwlInstance inst, void* clientData);

    //
    // The callback to create B-Rep of a concept
    // Use rdfgeom_GetBRep to get SHELL to populate. It will return not NULL but empty representation (empty conceptual face list) 
    // Implemented should use rdfgeom_AllocatePoints and rdfgeom_*_Create to initialize geometry
    // All nested instances already have defined representation to the moment of the call, and their shells can be used.
    // clientData is any value passed to rdfgeom_SetClassGeometry
    // Also implementer should call rdfgeom_SetBoundingBox
    //
    typedef void (*RDFGEOM_CALLBACK_INIT_SHELL)(OwlInstance inst, void* clientData);
    
    //
    // Set callbacks to define B-Rep geometry of custom concept
    // To define new GeometricItem the call should implement callbacks and call the function after CreateClass
    // clientData is any pointer and will be passed to callbacks
    //
    extern bool rdfgeom_SetClassGeometry(OwlClass cls, RDFGEOM_CALLBACK_INIT_SHELL fnInitRepr, RDFGEOM_CALLBACK_GET_BBOX fnGetBBox, void* clientData);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 
    // Extension support
    // 

    //
    // The logging callback can be provided by host application which loads engine extension    
    //
    enum class RDFGEOM_LOG_LEVEL : unsigned char
    {
        INFO = 0,
        WARN,
        ERR
    };

    //
    // Extension can use and host application can provide this callback to log messages to host application 
    // 
    typedef void (*RDFGEOM_CALLBACK_LOG)(RDFGEOM_LOG_LEVEL level, const char* msg, void* hostData);


    //
    // Extension should implement and host application should call this function to initialize the extension 
    //
    #define RDFGEOM_LOAD_EXTENSION_FUNC         rdfgeom_LoadExtension
    #define RDFGEOM_LOAD_EXTENSION_FUNC_NAME    "rdfgeom_LoadExtension"
    typedef void (*RDFGEOM_LOAD_EXTENSION_FUNC_TYPE)(OwlModel model, RDFGEOM_CALLBACK_LOG logger, void* hostData);

    #define RDFGEOM_UNLOAD_EXTENSION_FUNC         rdfgeom_UnLoadExtension
    #define RDFGEOM_UNLOAD_EXTENSION_FUNC_NAME    "rdfgeom_UnLoadExtension"
    typedef void (*RDFGEOM_UNLOAD_EXTENSION_FUNC_TYPE)(void* hostData);


#ifdef __cplusplus
    }
#endif

#endif
