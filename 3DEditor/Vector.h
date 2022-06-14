#pragma once

// https://gist.github.com/zooty/f2222e0bc33e6845f068

class Vector3f {
private:
	double x;
	double y;
	double z;

public:
	Vector3f();
	Vector3f(double, double, double);

	double getX() const { return x; }
	double getY() const { return y; }
	double getZ() const { return z; }

	double length() {
		return sqrt((x * x) + (y * y) + (z * z));
	}

	double length2() {
		return pow(length(), 2.);
	}

	Vector3f normalized() {
		double mag = length();

		return Vector3f(x / mag, y / mag, z / mag);
	}

	Vector3f neg() {
		return Vector3f(-x, -y, -z);
	}

	double dot(Vector3f other) {
		return x * other.getX() + y * other.getY() + z * other.getZ();
	}

	Vector3f cross(Vector3f other) {
		double x_ = y * other.getZ() - z * other.getY();
		double y_ = z * other.getX() - x * other.getZ();
		double z_ = x * other.getY() - y * other.getX();

		return Vector3f(x_, y_, z_);
	}

	Vector3f operator + (const Vector3f &other) {
		return Vector3f(x + other.x, y + other.y, z + other.z);
	}

	Vector3f operator - (const Vector3f &other) {
		return Vector3f(x - other.x, y - other.y, z - other.z);
	}

	Vector3f operator * (const Vector3f &other) {
		return Vector3f(x * other.x, y * other.y, z * other.z);
	}

	Vector3f operator * (double number) {
		return Vector3f(x * number, y * number, z * number);
	}

	Vector3f operator / (const Vector3f &other) {
		return Vector3f(x / other.x, y / other.y, z / other.z);
	}

	Vector3f operator / (double number) {
		return Vector3f(x / number, y / number, z / number);
	}
};

Vector3f::Vector3f() {
	x = 0;
	y = 0;
	z = 0;
}

Vector3f::Vector3f(double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;
}