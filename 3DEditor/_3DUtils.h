#pragma once

// ************************************************************************************************
#define PI 3.14159265

// ************************************************************************************************
struct _vector3d
{
	double x;
	double y;
	double z;
};

// ************************************************************************************************
struct _matrix
{
	double _11, _12, _13;
	double _21, _22, _23;
	double _31, _32, _33;
	double _41, _42, _43;
};

// ************************************************************************************************
static void	_transform(const _vector3d* pV, const _matrix* pM, _vector3d* pOut)
{
	_vector3d vecTmp;
	vecTmp.x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31 + pM->_41;
	vecTmp.y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32 + pM->_42;
	vecTmp.z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33 + pM->_43;

	pOut->x = vecTmp.x;
	pOut->y = vecTmp.y;
	pOut->z = vecTmp.z;
}

// mathematicsGeometryDoublePrecisionDerived.h
// Example: 30 degrees in the XY plane
// => MatrixRotateByEulerAngles(matrix, 0, 0, 2 * Pi * 30. / 360.);
static void	_matrixRotateByEulerAngles(
	_matrix* matrix,
	double	alpha,
	double	beta,
	double	gamma
)
{
	//
	//	https://en.wikipedia.org/wiki/Rotation_matrix
	//
	double	cos_alpha = cos(alpha), sin_alpha = sin(alpha),
		cos_beta = cos(beta), sin_beta = sin(beta),
		cos_gamma = cos(gamma), sin_gamma = sin(gamma);

	matrix->_11 = 1. * cos_beta * cos_gamma;
	matrix->_12 = 1. * cos_beta * sin_gamma;
	matrix->_13 = -1. * sin_beta * 1.;

	matrix->_21 = sin_alpha * sin_beta * cos_gamma
		- cos_alpha * 1. * sin_gamma;
	matrix->_22 = sin_alpha * sin_beta * sin_gamma
		+ cos_alpha * 1. * cos_gamma;
	matrix->_23 = sin_alpha * cos_beta * 1.;

	matrix->_31 = cos_alpha * sin_beta * cos_gamma
		+ sin_alpha * 1. * sin_gamma;
	matrix->_32 = cos_alpha * sin_beta * sin_gamma
		- sin_alpha * 1. * cos_gamma;
	matrix->_33 = cos_alpha * cos_beta * 1.;

	matrix->_41 = 0.;
	matrix->_42 = 0.;
	matrix->_43 = 0.;
}

// ************************************************************************************************
// https://gist.github.com/zooty/f2222e0bc33e6845f068
class _vector3f {

private:
	double x;
	double y;
	double z;

public:
	_vector3f()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	_vector3f(double, double, double)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	double getX() const { return x; }
	double getY() const { return y; }
	double getZ() const { return z; }

	double length() {
		return sqrt((x * x) + (y * y) + (z * z));
	}

	double length2() {
		return pow(length(), 2.);
	}

	_vector3f normalized() {
		double mag = length();

		return _vector3f(x / mag, y / mag, z / mag);
	}

	_vector3f neg() {
		return _vector3f(-x, -y, -z);
	}

	double dot(_vector3f other) {
		return x * other.getX() + y * other.getY() + z * other.getZ();
	}

	_vector3f cross(_vector3f other) {
		double x_ = y * other.getZ() - z * other.getY();
		double y_ = z * other.getX() - x * other.getZ();
		double z_ = x * other.getY() - y * other.getX();

		return _vector3f(x_, y_, z_);
	}

	_vector3f operator + (const _vector3f& other) {
		return _vector3f(x + other.x, y + other.y, z + other.z);
	}

	_vector3f operator - (const _vector3f& other) {
		return _vector3f(x - other.x, y - other.y, z - other.z);
	}

	_vector3f operator * (const _vector3f& other) {
		return _vector3f(x * other.x, y * other.y, z * other.z);
	}

	_vector3f operator * (double number) {
		return _vector3f(x * number, y * number, z * number);
	}

	_vector3f operator / (const _vector3f& other) {
		return _vector3f(x / other.x, y / other.y, z / other.z);
	}

	_vector3f operator / (double number) {
		return _vector3f(x / number, y / number, z / number);
	}
};