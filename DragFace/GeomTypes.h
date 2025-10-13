#pragma once

#if 0
mathematicsGeometryDoublePrecision.cpp / .h
mathematicsGeometryDoublePrecisionDerived.cpp
//
//	https://math.stackexchange.com/questions/2213165/find-shortest-distance-between-lines-in-3d
//
double	LineLineDistance(
	const VECTOR3 * lineI_pointI,
	const VECTOR3 * lineI_pointII,
	const VECTOR3 * lineII_pointI,
	const VECTOR3 * lineII_pointII
)
#endif

typedef point3d<double> Point3d;

typedef vector3d<double> Vector3d;

struct Segment3D {
	Point3d pt[2];

	Vector3d Direction() { auto vec = pt[1] - pt[0]; vec = normalize(vec); return vec; }
};


static inline Point3d MakePoint(double coord[3]) { return make_point(coord[0], coord[1], coord[2]); }

static inline Vector3d MakeVector(double coord[3]) { return make_vector(coord[0], coord[1], coord[2]); }
