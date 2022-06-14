#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Point.h"

/**
* This class represents the concept Point3D. (http://rdf.bg/Point3DSet)
*/
class Point3DSet : public Point
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Point3DSet()
	{
	}

	virtual ~Point3DSet()
	{
	}

	static const char* getClassName()
	{
		return "Point3DSet";
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
	* This datatype property is named coordinates. (Cardinality: [0, Inf])
	*/
	vector<double> getCoordinates()
	{
		auto property = Thing::getDataProperty<double>("coordinates");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named coordinates. (Cardinality: [0, Inf])
	*/
	void setCoordinates(const vector<double>& vecValues)
	{
		Thing::setDataProperty("coordinates", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named asOpenGL. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getAsOpenGL, put = setAsOpenGL)) bool asOpenGL;

	/**
	* This datatype property is named coordinates. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getCoordinates, put = setCoordinates)) vector<double> coordinates;

protected:

	virtual void postCreate()
	{
		Point::postCreate();
	}
};