#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Solid.h"

/**
* This class represents the concept Prism. (http://rdf.bg/Prism)
*/
class Prism : public Solid
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Prism()
	{
	}

	virtual ~Prism()
	{
	}

	static const char* getClassName()
	{
		return "Prism";
	}

	/**
	* This datatype property is named height. (Cardinality: [1, 1])
	*/
	double getHeight()
	{
		auto property = Thing::getDataProperty<double>("height");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named height. (Cardinality: [1, 1])
	*/
	void setHeight(double value)
	{
		Thing::setDataProperty("height", &value, 1);
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
	* This datatype property is named height. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getHeight, put = setHeight)) double height;

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