#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "ConicalCurve.h"

/**
* This class represents the concept Hyperbola. (http://rdf.bg/Hyperbola)
*/
class Hyperbola : public ConicalCurve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Hyperbola()
	{
	}

	virtual ~Hyperbola()
	{
	}

	static const char* getClassName()
	{
		return "Hyperbola";
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
	* This datatype property is named b. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getB, put = setB)) double b;

protected:

	virtual void postCreate()
	{
		ConicalCurve::postCreate();
	}
};