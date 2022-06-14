#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "ConicalCurve.h"

/**
* This class represents the concept Ellipse. (http://rdf.bg/Ellipse)
*/
class Ellipse : public ConicalCurve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Ellipse()
	{
	}

	virtual ~Ellipse()
	{
	}

	static const char* getClassName()
	{
		return "Ellipse";
	}

	/**
	* This datatype property is named b. (Cardinality: [1, 1])
	*/
	double getB()
	{
		auto property = Thing::getDataProperty<double>("b");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named b. (Cardinality: [1, 1])
	*/
	void setB(double value)
	{
		Thing::setDataProperty("b", &value, 1);
	}

	/**
	* This datatype property is named radiusI. (Cardinality: [0, 1])
	*/
	double getRadiusI()
	{
		auto property = Thing::getDataProperty<double>("radiusI");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named radiusI. (Cardinality: [0, 1])
	*/
	void setRadiusI(double value)
	{
		Thing::setDataProperty("radiusI", &value, 1);
	}

	/**
	* This datatype property is named radiusII. (Cardinality: [0, 1])
	*/
	double getRadiusII()
	{
		auto property = Thing::getDataProperty<double>("radiusII");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named radiusII. (Cardinality: [0, 1])
	*/
	void setRadiusII(double value)
	{
		Thing::setDataProperty("radiusII", &value, 1);
	}

	/**
	* This datatype property is named b. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getB, put = setB)) double b;

	/**
	* This datatype property is named radiusI. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getRadiusI, put = setRadiusI)) double radiusI;

	/**
	* This datatype property is named radiusII. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getRadiusII, put = setRadiusII)) double radiusII;

protected:

	virtual void postCreate()
	{
		ConicalCurve::postCreate();
	}
};