#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Curve.h"

/**
* This class represents the concept Line3DSet. (http://rdf.bg/Line3DSet)
*/
class Line3DSet : public Curve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Line3DSet()
	{
	}

	virtual ~Line3DSet()
	{
	}

	static const char* getClassName()
	{
		return "Line3DSet";
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
	* This datatype property is named points. (Cardinality: [0, Inf])
	*/
	vector<double> getPoints()
	{
		auto property = Thing::getDataProperty<double>("points");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named points. (Cardinality: [0, Inf])
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
	* This datatype property is named points. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getPoints, put = setPoints)) vector<double> points;

protected:

	virtual void postCreate()
	{
		Curve::postCreate();
	}
};