#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Solid.h"

/**
* This class represents the concept Cube. (http://rdf.bg/Cube)
*/
class Cube : public Solid
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Cube()
	{
	}

	virtual ~Cube()
	{
	}

	static const char* getClassName()
	{
		return "Cube";
	}

	/**
	* This datatype property is named length. (Cardinality: [1, 1])
	*/
	double getLength()
	{
		auto property = Thing::getDataProperty<double>("length");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named length. (Cardinality: [1, 1])
	*/
	void setLength(double value)
	{
		Thing::setDataProperty("length", &value, 1);
	}

	/**
	* This datatype property is named length. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getLength, put = setLength)) double length;

protected:

	virtual void postCreate()
	{
		Solid::postCreate();
	}
};