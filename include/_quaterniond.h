#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

// ************************************************************************************************
// https://en.m.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
class _quaterniond
{

private: // Members

	double m_dW;
	double m_dX;
	double m_dY;
	double m_dZ;

public: // Methods

	_quaterniond()
		: _quaterniond(1., 0., 0., 0.)
	{}

	_quaterniond(double dW, double dX, double dY, double dZ)
		: m_dW(dW)
		, m_dX(dX)
		, m_dY(dY)
		, m_dZ(dZ)
	{}

	_quaterniond(const _quaterniond & quaternion)
	{
		m_dW = quaternion.w();
		m_dX = quaternion.x();
		m_dY = quaternion.y();
		m_dZ = quaternion.z();
	}

	virtual ~_quaterniond()
	{}

	double w() const { return m_dW; }
	double x() const { return m_dX; }
	double y() const { return m_dY; }
	double z() const { return m_dZ; }

	// Euler Angles to Quaternion Conversion
	// yaw (Z), pitch (Y), roll (X) - radians
	static _quaterniond toQuaternion(double yaw, double pitch, double roll)
	{
		// Abbreviations for the various angular functions
		double cy = cos(yaw * 0.5);
		double sy = sin(yaw * 0.5);
		double cp = cos(pitch * 0.5);
		double sp = sin(pitch * 0.5);
		double cr = cos(roll * 0.5);
		double sr = sin(roll * 0.5);

		_quaterniond quaternion(
			cy * cp * cr + sy * sp * sr,
			cy * cp * sr - sy * sp * cr,
			sy * cp * sr + cy * sp * cr,
			sy * cp * cr - cy * sp * sr);

		return quaternion;
	}

	// Quaternion to Euler Angles Conversion
	// yaw (Z), pitch (Y), roll (X) - radians
	void toEulerAngle(double& roll, double& pitch, double& yaw)
	{
		// roll (x-axis rotation)
		double sinr_cosp = +2.0 * (w() * x() + y() * z());
		double cosr_cosp = +1.0 - 2.0 * (x() * x() + y() * y());
		roll = atan2(sinr_cosp, cosr_cosp);

		// pitch (y-axis rotation)
		double sinp = +2.0 * (w() * y() - z() * x());
		if (fabs(sinp) >= 1)
			pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
		else
			pitch = asin(sinp);

		// yaw (z-axis rotation)
		double siny_cosp = +2.0 * (w() * z() + x() * y());
		double cosy_cosp = +1.0 - 2.0 * (y() * y() + z() * z());
		yaw = atan2(siny_cosp, cosy_cosp);
	}

	_quaterniond& operator = (const _quaterniond & quaternion)
	{
		m_dW = quaternion.w();
		m_dX = quaternion.x();
		m_dY = quaternion.y();
		m_dZ = quaternion.z();

		return *this;
	}

	bool isEqualTo(const _quaterniond & q1) const
	{
		if ((q1.w() != m_dW) ||
			(q1.x() != m_dX) ||
			(q1.y() != m_dY) ||
			(q1.z() != m_dZ))
		{
			return false;
		}

		return true;
	}

	void cross(const _quaterniond & q2)
	{	
		m_dW = w() * q2.w() - x() * q2.x() - y() * q2.y() - z() * q2.z();
		m_dX = w() * q2.x() + x() * q2.w() + y() * q2.z() - z() * q2.y();
		m_dY = w() * q2.y() + y() * q2.w() + z() * q2.x() - x() * q2.z();
		m_dZ = w() * q2.z() + z() * q2.w() + x() * q2.y() - y() * q2.x();
	}

	static _quaterniond cross(const _quaterniond & q1, const _quaterniond & q2)
	{
		_quaterniond quaternion(
			q1.w() * q2.w() - q1.x() * q2.x() - q1.y() * q2.y() - q1.z() * q2.z(),
			q1.w() * q2.x() + q1.x() * q2.w() + q1.y() * q2.z() - q1.z() * q2.y(),
			q1.w() * q2.y() + q1.y() * q2.w() + q1.z() * q2.x() - q1.x() * q2.z(),
			q1.w() * q2.z() + q1.z() * q2.w() + q1.x() * q2.y() - q1.y() * q2.x());

		return quaternion;
	}

	_quaterniond conjugate() const
	{
		_quaterniond quaternion(
			w(),
			-x(),
			-y(),
			-z());

		return quaternion;
	}

	double dot() const
	{
		double dDot = sqrt(pow(w(), 2.) + pow(x(), 2.) + pow(y(), 2.) + pow(z(), 2.));

		return dDot;
	}

	_quaterniond inverse() const
	{
		double dDot = dot();

		_quaterniond quaternion = conjugate();
		quaternion.m_dW /= dDot;
		quaternion.m_dX /= dDot;
		quaternion.m_dY /= dDot;
		quaternion.m_dZ /= dDot;

		return quaternion;
	}

	double * toMatrix() const
	{
		double qw = w();
		double qx = x();
		double qy = y();
		double qz = z();

		const double n = 1.0f / sqrt(qx * qx + qy * qy + qz * qz + qw * qw);
		qx *= n;
		qy *= n;
		qz *= n;
		qw *= n;

		double * pMatrix = new double[16]
		{
			1. - 2. * qy * qy - 2. * qz * qz, 2. * qx * qy - 2. * qz * qw, 2. * qx * qz + 2. * qy * qw, 0.,
			2. * qx * qy + 2. * qz * qw, 1. - 2. * qx * qx - 2. * qz * qz, 2. * qy * qz - 2. * qx * qw, 0.,
			2. * qx * qz - 2. * qy * qw, 2. * qy * qz + 2. * qx * qw, 1. - 2. * qx * qx - 2. * qy * qy, 0.,
			0., 0., 0., 1.
		};

		return pMatrix;
	}
};
