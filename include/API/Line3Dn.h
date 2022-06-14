#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Curve.h"

/**
* This class represents the concept Line3Dn. (http://rdf.bg/Line3Dn)
*/
class Line3Dn : public Curve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Line3Dn()
	{
	}

	virtual ~Line3Dn()
	{
	}

	static const char* getClassName()
	{
		return "Line3Dn";
	}

	/**
	* This datatype property is named asOpenGL. (Cardinality: [0, 1])
	*/
	bool getAsOpenGL()
	{
		auto property = Thing::getDataProperty<bool>("asOpenGL");
		if (property.second == 1) { return property.first[0]; }

		return (bool)0;
	}

	/**
	* This datatype property is named asOpenGL. (Cardinality: [0, 1])
	*/
	void setAsOpenGL(bool value)
	{
		Thing::setDataProperty("asOpenGL", &value, 1);
	}

	/**
	* This datatype property is named endDirection. (Cardinality: [0, 3])
	*/
	vector<double> getEndDirection()
	{
		auto property = Thing::getDataProperty<double>("endDirection");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named endDirection. (Cardinality: [0, 3])
	*/
	void setEndDirection(const vector<double>& vecValues)
	{
		Thing::setDataProperty("endDirection", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named points. (Cardinality: [0, 6])
	*/
	vector<double> getPoints()
	{
		auto property = Thing::getDataProperty<double>("points");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named points. (Cardinality: [0, 6])
	*/
	void setPoints(const vector<double>& vecValues)
	{
		Thing::setDataProperty("points", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named startDirection. (Cardinality: [0, 3])
	*/
	vector<double> getStartDirection()
	{
		auto property = Thing::getDataProperty<double>("startDirection");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named startDirection. (Cardinality: [0, 3])
	*/
	void setStartDirection(const vector<double>& vecValues)
	{
		Thing::setDataProperty("startDirection", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named x0. (Cardinality: [0, 1])
	*/
	double getX0()
	{
		auto property = Thing::getDataProperty<double>("x0");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named x0. (Cardinality: [0, 1])
	*/
	void setX0(double value)
	{
		Thing::setDataProperty("x0", &value, 1);
	}

	/**
	* This datatype property is named x1. (Cardinality: [0, 1])
	*/
	double getX1()
	{
		auto property = Thing::getDataProperty<double>("x1");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named x1. (Cardinality: [0, 1])
	*/
	void setX1(double value)
	{
		Thing::setDataProperty("x1", &value, 1);
	}

	/**
	* This datatype property is named y0. (Cardinality: [0, 1])
	*/
	double getY0()
	{
		auto property = Thing::getDataProperty<double>("y0");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named y0. (Cardinality: [0, 1])
	*/
	void setY0(double value)
	{
		Thing::setDataProperty("y0", &value, 1);
	}

	/**
	* This datatype property is named y1. (Cardinality: [0, 1])
	*/
	double getY1()
	{
		auto property = Thing::getDataProperty<double>("y1");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named y1. (Cardinality: [0, 1])
	*/
	void setY1(double value)
	{
		Thing::setDataProperty("y1", &value, 1);
	}

	/**
	* This datatype property is named z0. (Cardinality: [0, 1])
	*/
	double getZ0()
	{
		auto property = Thing::getDataProperty<double>("z0");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named z0. (Cardinality: [0, 1])
	*/
	void setZ0(double value)
	{
		Thing::setDataProperty("z0", &value, 1);
	}

	/**
	* This datatype property is named z1. (Cardinality: [0, 1])
	*/
	double getZ1()
	{
		auto property = Thing::getDataProperty<double>("z1");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named z1. (Cardinality: [0, 1])
	*/
	void setZ1(double value)
	{
		Thing::setDataProperty("z1", &value, 1);
	}

	/**
	* This datatype property is named asOpenGL. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getAsOpenGL, put = setAsOpenGL)) bool asOpenGL;

	/**
	* This datatype property is named endDirection. (Cardinality: [0, 3])
	*/
	__declspec(property(get = getEndDirection, put = setEndDirection)) vector<double> endDirection;

	/**
	* This datatype property is named points. (Cardinality: [0, 6])
	*/
	__declspec(property(get = getPoints, put = setPoints)) vector<double> points;

	/**
	* This datatype property is named startDirection. (Cardinality: [0, 3])
	*/
	__declspec(property(get = getStartDirection, put = setStartDirection)) vector<double> startDirection;

	/**
	* This datatype property is named x0. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getX0, put = setX0)) double x0;

	/**
	* This datatype property is named x1. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getX1, put = setX1)) double x1;

	/**
	* This datatype property is named y0. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getY0, put = setY0)) double y0;

	/**
	* This datatype property is named y1. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getY1, put = setY1)) double y1;

	/**
	* This datatype property is named z0. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getZ0, put = setZ0)) double z0;

	/**
	* This datatype property is named z1. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getZ1, put = setZ1)) double z1;

protected:

	virtual void postCreate()
	{
		Curve::postCreate();
	}
};