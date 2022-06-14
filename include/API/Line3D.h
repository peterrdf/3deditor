#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Curve.h"

/**
* This class represents the concept Line3D. (http://rdf.bg/Line3D)
*/
class Line3D : public Curve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Line3D()
	{
	}

	virtual ~Line3D()
	{
	}

	static const char* getClassName()
	{
		return "Line3D";
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
	* This datatype property is named points. (Cardinality: [6, 6])
	*/
	vector<double> getPoints()
	{
		auto property = Thing::getDataProperty<double>("points");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named points. (Cardinality: [6, 6])
	*/
	void setPoints(const vector<double>& vecValues)
	{
		Thing::setDataProperty("points", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named asOpenGL. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getAsOpenGL, put = setAsOpenGL)) bool asOpenGL;

	/**
	* This datatype property is named points. (Cardinality: [6, 6])
	*/
	__declspec(property(get = getPoints, put = setPoints)) vector<double> points;

protected:

	virtual void postCreate()
	{
		Curve::postCreate();
	}
};