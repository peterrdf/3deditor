#pragma once

typedef point3d<double> Point3d;

typedef vector3d<double> Vector3d;

struct Segment3D {
	Point3d pt[2];

	Vector3d Direction() { auto vec = pt[1] - pt[0]; vec = normalize(vec); return vec; }
};


static inline Point3d MakePoint(double coord[3]) { return make_point(coord[0], coord[1], coord[2]); }

static inline Vector3d MakeVector(double coord[3]) { return make_vector(coord[0], coord[1], coord[2]); }
