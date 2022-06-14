#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Point.h"

/**
* This class represents the concept Point3D. (http://rdf.bg/Point3D)
*/
class Point3D : public Point
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Point3D()
	{
	}

	virtual ~Point3D()
	{
	}

	static const char* getClassName()
	{
		return "Point3D";
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
	* This datatype property is named coordinates. (Cardinality: [0, 3])
	*/
	vector<double> getCoordinates()
	{
		auto property = Thing::getDataProperty<double>("coordinates");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named coordinates. (Cardinality: [0, 3])
	*/
	void setCoordinates(const vector<double>& vecValues)
	{
		Thing::setDataProperty("coordinates", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named x. (Cardinality: [0, 1])
	*/
	double getX()
	{
		auto property = Thing::getDataProperty<double>("x");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named x. (Cardinality: [0, 1])
	*/
	void setX(double value)
	{
		Thing::setDataProperty("x", &value, 1);
	}

	/**
	* This datatype property is named y. (Cardinality: [0, 1])
	*/
	double getY()
	{
		auto property = Thing::getDataProperty<double>("y");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named y. (Cardinality: [0, 1])
	*/
	void setY(double value)
	{
		Thing::setDataProperty("y", &value, 1);
	}

	/**
	* This datatype property is named z. (Cardinality: [0, 1])
	*/
	double getZ()
	{
		auto property = Thing::getDataProperty<double>("z");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named z. (Cardinality: [0, 1])
	*/
	void setZ(double value)
	{
		Thing::setDataProperty("z", &value, 1);
	}

	/**
	* This datatype property is named asOpenGL. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getAsOpenGL, put = setAsOpenGL)) bool asOpenGL;

	/**
	* This datatype property is named coordinates. (Cardinality: [0, 3])
	*/
	__declspec(property(get = getCoordinates, put = setCoordinates)) vector<double> coordinates;

	/**
	* This datatype property is named x. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getX, put = setX)) double x;

	/**
	* This datatype property is named y. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getY, put = setY)) double y;

	/**
	* This datatype property is named z. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getZ, put = setZ)) double z;

protected:

	virtual void postCreate()
	{
		Point::postCreate();
	}
};