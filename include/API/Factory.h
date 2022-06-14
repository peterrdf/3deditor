#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

static _Thing* createAdvancedFace(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class AdvancedFace>(iInstance);
}

static _Thing* createAdvancedFace2D(_Model* pModel, int64_t iInstance)
{
	 return (AdvancedFace*)pModel->create<class AdvancedFace2D>(iInstance);
}

static _Thing* createAdvancedFace3D(_Model* pModel, int64_t iInstance)
{
	 return (AdvancedFace*)pModel->create<class AdvancedFace3D>(iInstance);
}

static _Thing* createAdvancedFaceMapped(_Model* pModel, int64_t iInstance)
{
	 return (AdvancedFace*)pModel->create<class AdvancedFaceMapped>(iInstance);
}

static _Thing* createAlignedSegment(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class AlignedSegment>(iInstance);
}

static _Thing* createAlignedSegments(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class AlignedSegments>(iInstance);
}

static _Thing* createAlignment(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Alignment>(iInstance);
}

static _Thing* createAmbientLight(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class AmbientLight>(iInstance);
}

static _Thing* createAppearance(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Appearance>(iInstance);
}

static _Thing* createArc3D(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Arc3D>(iInstance);
}

static _Thing* createBezierCurve(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class BezierCurve>(iInstance);
}

static _Thing* createBezierSurface(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class BezierSurface>(iInstance);
}

static _Thing* createBiQuadraticParabola(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class BiQuadraticParabola>(iInstance);
}

static _Thing* createBlend(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Blend>(iInstance);
}

static _Thing* createBlossCurve(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class BlossCurve>(iInstance);
}

static _Thing* createBooleanOperation(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class BooleanOperation>(iInstance);
}

static _Thing* createBooleanOperation2D(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class BooleanOperation2D>(iInstance);
}

static _Thing* createBoundaryRepresentation(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class BoundaryRepresentation>(iInstance);
}

static _Thing* createBox(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Box>(iInstance);
}

static _Thing* createBSplineCurve(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class BSplineCurve>(iInstance);
}

static _Thing* createBSplineSurface(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class BSplineSurface>(iInstance);
}

static _Thing* createCircle(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Circle>(iInstance);
}

static _Thing* createCircleByPoints(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class CircleByPoints>(iInstance);
}

static _Thing* createClippedPyramid(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class ClippedPyramid>(iInstance);
}

static _Thing* createClipping(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Clipping>(iInstance);
}

static _Thing* createClothoid(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Clothoid>(iInstance);
}

static _Thing* createClothoidCurve(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class ClothoidCurve>(iInstance);
}

static _Thing* createCollection(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Collection>(iInstance);
}

static _Thing* createColor(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Color>(iInstance);
}

static _Thing* createColorComponent(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class ColorComponent>(iInstance);
}

static _Thing* createCone(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Cone>(iInstance);
}

static _Thing* createConicalCurve(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class ConicalCurve>(iInstance);
}

static _Thing* createConicalSurface(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class ConicalSurface>(iInstance);
}

static _Thing* createCosineCurve(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class CosineCurve>(iInstance);
}

static _Thing* createCube(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Cube>(iInstance);
}

static _Thing* createCubicParabola(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class CubicParabola>(iInstance);
}

static _Thing* createCuboid(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Cuboid>(iInstance);
}

static _Thing* createCurve(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Curve>(iInstance);
}

static _Thing* createCurvesFromSurface(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class CurvesFromSurface>(iInstance);
}

static _Thing* createCylinder(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Cylinder>(iInstance);
}

static _Thing* createCylindricalSurface(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class CylindricalSurface>(iInstance);
}

static _Thing* createDegenerateToroidalSurface(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class DegenerateToroidalSurface>(iInstance);
}

static _Thing* createDirectionalLight(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class DirectionalLight>(iInstance);
}

static _Thing* createDirectLight(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class DirectLight>(iInstance);
}

static _Thing* createEllipse(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Ellipse>(iInstance);
}

static _Thing* createEllipticCone(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class EllipticCone>(iInstance);
}

static _Thing* createEnvironment(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Environment>(iInstance);
}

static _Thing* createExtrudedPolygon(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class ExtrudedPolygon>(iInstance);
}

static _Thing* createExtrudedPolygonTapered(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class ExtrudedPolygonTapered>(iInstance);
}

static _Thing* createExtrusionAreaSolid(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class ExtrusionAreaSolid>(iInstance);
}

static _Thing* createExtrusionAreaSolidSet(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class ExtrusionAreaSolidSet>(iInstance);
}

static _Thing* createFace(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Face>(iInstance);
}

static _Thing* createFace2D(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Face2D>(iInstance);
}

static _Thing* createFace2DSet(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Face2DSet>(iInstance);
}

static _Thing* createFiniteSurface(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class FiniteSurface>(iInstance);
}

static _Thing* createFrustumCone(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class FrustumCone>(iInstance);
}

static _Thing* createGeometricItem(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class GeometricItem>(iInstance);
}

static _Thing* createHornTorus(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class HornTorus>(iInstance);
}

static _Thing* createHyperbola(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Hyperbola>(iInstance);
}

static _Thing* createInfiniteSurface(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class InfiniteSurface>(iInstance);
}

static _Thing* createInverseMatrix(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class InverseMatrix>(iInstance);
}

static _Thing* createInvertedCurve(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class InvertedCurve>(iInstance);
}

static _Thing* createInvertedSurface(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class InvertedSurface>(iInstance);
}

static _Thing* createLight(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Light>(iInstance);
}

static _Thing* createLine3D(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Line3D>(iInstance);
}

static _Thing* createLine3Dn(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Line3Dn>(iInstance);
}

static _Thing* createLine3DSet(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Line3DSet>(iInstance);
}

static _Thing* createLineByFace(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class LineByFace>(iInstance);
}

static _Thing* createMaterial(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Material>(iInstance);
}

static _Thing* createMathematics(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Mathematics>(iInstance);
}

static _Thing* createMatrix(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Matrix>(iInstance);
}

static _Thing* createMatrixMultiplication(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class MatrixMultiplication>(iInstance);
}

static _Thing* createMesh(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Mesh>(iInstance);
}

static _Thing* createNURBSCurve(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class NURBSCurve>(iInstance);
}

static _Thing* createNURBSSurface(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class NURBSSurface>(iInstance);
}

static _Thing* createParabola(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Parabola>(iInstance);
}

static _Thing* createPlane(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Plane>(iInstance);
}

static _Thing* createPlaneSurface(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class PlaneSurface>(iInstance);
}

static _Thing* createPoint(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Point>(iInstance);
}

static _Thing* createPoint3D(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Point3D>(iInstance);
}

static _Thing* createPoint3DSet(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Point3DSet>(iInstance);
}

static _Thing* createPointLight(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class PointLight>(iInstance);
}

static _Thing* createPointLoop(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class PointLoop>(iInstance);
}

static _Thing* createPolygon2D(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Polygon2D>(iInstance);
}

static _Thing* createPolygon3D(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Polygon3D>(iInstance);
}

static _Thing* createPolyLine3D(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class PolyLine3D>(iInstance);
}

static _Thing* createPrism(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Prism>(iInstance);
}

static _Thing* createProjection(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Projection>(iInstance);
}

static _Thing* createPyramid(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Pyramid>(iInstance);
}

static _Thing* createRationalBezierCurve(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class RationalBezierCurve>(iInstance);
}

static _Thing* createRationalBezierSurface(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class RationalBezierSurface>(iInstance);
}

static _Thing* createRectangleCurve(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class RectangleCurve>(iInstance);
}

static _Thing* createRepetition(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Repetition>(iInstance);
}

static _Thing* createRingTorus(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class RingTorus>(iInstance);
}

static _Thing* createShadow(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Shadow>(iInstance);
}

static _Thing* createSineCurve(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class SineCurve>(iInstance);
}

static _Thing* createSkewedCone(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class SkewedCone>(iInstance);
}

static _Thing* createSkewedFrustumCone(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class SkewedFrustumCone>(iInstance);
}

static _Thing* createSolid(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Solid>(iInstance);
}

static _Thing* createSolidBySurface(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class SolidBySurface>(iInstance);
}

static _Thing* createSolidLine(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class SolidLine>(iInstance);
}

static _Thing* createSphere(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Sphere>(iInstance);
}

static _Thing* createSphericalSurface(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class SphericalSurface>(iInstance);
}

static _Thing* createSpindleTorus(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class SpindleTorus>(iInstance);
}

static _Thing* createSpiral(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Spiral>(iInstance);
}

static _Thing* createSplineCurve(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class SplineCurve>(iInstance);
}

static _Thing* createSplineSurface(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class SplineSurface>(iInstance);
}

static _Thing* createSpotLight(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class SpotLight>(iInstance);
}

static _Thing* createSurface(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Surface>(iInstance);
}

static _Thing* createSurfaceBySweptCurve(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class SurfaceBySweptCurve>(iInstance);
}

static _Thing* createSurfaceOfLinearExtrusion(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class SurfaceOfLinearExtrusion>(iInstance);
}

static _Thing* createSurfaceOfRevolution(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class SurfaceOfRevolution>(iInstance);
}

static _Thing* createSweptAreaSolid(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class SweptAreaSolid>(iInstance);
}

static _Thing* createSweptAreaSolidSet(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class SweptAreaSolidSet>(iInstance);
}

static _Thing* createSweptAreaSolidTapered(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class SweptAreaSolidTapered>(iInstance);
}

static _Thing* createSweptBlend(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class SweptBlend>(iInstance);
}

static _Thing* createTexture(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Texture>(iInstance);
}

static _Thing* createToroidalSurface(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class ToroidalSurface>(iInstance);
}

static _Thing* createTorus(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Torus>(iInstance);
}

static _Thing* createTransformation(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Transformation>(iInstance);
}

static _Thing* createTransitionalCurve(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class TransitionalCurve>(iInstance);
}

static _Thing* createTriangleCurve(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class TriangleCurve>(iInstance);
}

static _Thing* createTriangleReduction(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class TriangleReduction>(iInstance);
}

static _Thing* createTriangleSet(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class TriangleSet>(iInstance);
}

static _Thing* createVector(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Vector>(iInstance);
}

static _Thing* createVector3(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class Vector3>(iInstance);
}

static _Thing* createView(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class View>(iInstance);
}

static _Thing* createWorld(_Model* pModel, int64_t iInstance)
{
	 return pModel->create<class World>(iInstance);
}

typedef _Thing* (*createThing)(_Model*, int64_t);

#include <map>
#include <string>

static std::map<std::string, createThing> FACTORY =
{
	{"AdvancedFace", &createAdvancedFace},
	{"AdvancedFace2D", &createAdvancedFace2D},
	{"AdvancedFace3D", &createAdvancedFace3D},
	{"AdvancedFaceMapped", &createAdvancedFaceMapped},
	{"AlignedSegment", &createAlignedSegment},
	{"AlignedSegments", &createAlignedSegments},
	{"Alignment", &createAlignment},
	{"AmbientLight", &createAmbientLight},
	{"Appearance", &createAppearance},
	{"Arc3D", &createArc3D},
	{"BezierCurve", &createBezierCurve},
	{"BezierSurface", &createBezierSurface},
	{"BiQuadraticParabola", &createBiQuadraticParabola},
	{"Blend", &createBlend},
	{"BlossCurve", &createBlossCurve},
	{"BooleanOperation", &createBooleanOperation},
	{"BooleanOperation2D", &createBooleanOperation2D},
	{"BoundaryRepresentation", &createBoundaryRepresentation},
	{"Box", &createBox},
	{"BSplineCurve", &createBSplineCurve},
	{"BSplineSurface", &createBSplineSurface},
	{"Circle", &createCircle},
	{"CircleByPoints", &createCircleByPoints},
	{"ClippedPyramid", &createClippedPyramid},
	{"Clipping", &createClipping},
	{"Clothoid", &createClothoid},
	{"ClothoidCurve", &createClothoidCurve},
	{"Collection", &createCollection},
	{"Color", &createColor},
	{"ColorComponent", &createColorComponent},
	{"Cone", &createCone},
	{"ConicalCurve", &createConicalCurve},
	{"ConicalSurface", &createConicalSurface},
	{"CosineCurve", &createCosineCurve},
	{"Cube", &createCube},
	{"CubicParabola", &createCubicParabola},
	{"Cuboid", &createCuboid},
	{"Curve", &createCurve},
	{"CurvesFromSurface", &createCurvesFromSurface},
	{"Cylinder", &createCylinder},
	{"CylindricalSurface", &createCylindricalSurface},
	{"DegenerateToroidalSurface", &createDegenerateToroidalSurface},
	{"DirectionalLight", &createDirectionalLight},
	{"DirectLight", &createDirectLight},
	{"Ellipse", &createEllipse},
	{"EllipticCone", &createEllipticCone},
	{"Environment", &createEnvironment},
	{"ExtrudedPolygon", &createExtrudedPolygon},
	{"ExtrudedPolygonTapered", &createExtrudedPolygonTapered},
	{"ExtrusionAreaSolid", &createExtrusionAreaSolid},
	{"ExtrusionAreaSolidSet", &createExtrusionAreaSolidSet},
	{"Face", &createFace},
	{"Face2D", &createFace2D},
	{"Face2DSet", &createFace2DSet},
	{"FiniteSurface", &createFiniteSurface},
	{"FrustumCone", &createFrustumCone},
	{"GeometricItem", &createGeometricItem},
	{"HornTorus", &createHornTorus},
	{"Hyperbola", &createHyperbola},
	{"InfiniteSurface", &createInfiniteSurface},
	{"InverseMatrix", &createInverseMatrix},
	{"InvertedCurve", &createInvertedCurve},
	{"InvertedSurface", &createInvertedSurface},
	{"Light", &createLight},
	{"Line3D", &createLine3D},
	{"Line3Dn", &createLine3Dn},
	{"Line3DSet", &createLine3DSet},
	{"LineByFace", &createLineByFace},
	{"Material", &createMaterial},
	{"Mathematics", &createMathematics},
	{"Matrix", &createMatrix},
	{"MatrixMultiplication", &createMatrixMultiplication},
	{"Mesh", &createMesh},
	{"NURBSCurve", &createNURBSCurve},
	{"NURBSSurface", &createNURBSSurface},
	{"Parabola", &createParabola},
	{"Plane", &createPlane},
	{"PlaneSurface", &createPlaneSurface},
	{"Point", &createPoint},
	{"Point3D", &createPoint3D},
	{"Point3DSet", &createPoint3DSet},
	{"PointLight", &createPointLight},
	{"PointLoop", &createPointLoop},
	{"Polygon2D", &createPolygon2D},
	{"Polygon3D", &createPolygon3D},
	{"PolyLine3D", &createPolyLine3D},
	{"Prism", &createPrism},
	{"Projection", &createProjection},
	{"Pyramid", &createPyramid},
	{"RationalBezierCurve", &createRationalBezierCurve},
	{"RationalBezierSurface", &createRationalBezierSurface},
	{"RectangleCurve", &createRectangleCurve},
	{"Repetition", &createRepetition},
	{"RingTorus", &createRingTorus},
	{"Shadow", &createShadow},
	{"SineCurve", &createSineCurve},
	{"SkewedCone", &createSkewedCone},
	{"SkewedFrustumCone", &createSkewedFrustumCone},
	{"Solid", &createSolid},
	{"SolidBySurface", &createSolidBySurface},
	{"SolidLine", &createSolidLine},
	{"Sphere", &createSphere},
	{"SphericalSurface", &createSphericalSurface},
	{"SpindleTorus", &createSpindleTorus},
	{"Spiral", &createSpiral},
	{"SplineCurve", &createSplineCurve},
	{"SplineSurface", &createSplineSurface},
	{"SpotLight", &createSpotLight},
	{"Surface", &createSurface},
	{"SurfaceBySweptCurve", &createSurfaceBySweptCurve},
	{"SurfaceOfLinearExtrusion", &createSurfaceOfLinearExtrusion},
	{"SurfaceOfRevolution", &createSurfaceOfRevolution},
	{"SweptAreaSolid", &createSweptAreaSolid},
	{"SweptAreaSolidSet", &createSweptAreaSolidSet},
	{"SweptAreaSolidTapered", &createSweptAreaSolidTapered},
	{"SweptBlend", &createSweptBlend},
	{"Texture", &createTexture},
	{"ToroidalSurface", &createToroidalSurface},
	{"Torus", &createTorus},
	{"Transformation", &createTransformation},
	{"TransitionalCurve", &createTransitionalCurve},
	{"TriangleCurve", &createTriangleCurve},
	{"TriangleReduction", &createTriangleReduction},
	{"TriangleSet", &createTriangleSet},
	{"Vector", &createVector},
	{"Vector3", &createVector3},
	{"View", &createView},
	{"World", &createWorld}
};