#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Environment.h"
#include "Color.h"

/**
* This class represents the concept Light. (http://rdf.bg/Light)
*/
class Light : public Environment
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Light()
	{
	}

	virtual ~Light()
	{
	}

	static const char* getClassName()
	{
		return "Light";
	}

	/**
	* This object property represents the relation color. (Cardinality: [0, 1])
	*/
	Color* getColor()
	{
		auto property = Thing::getThingProperty<Color>("color");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation color. (Cardinality: [0, 1])
	*/
	void setColor(Color* pValue)
	{
		Thing::setThingProperty("color", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation color. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getColor, put = setColor)) Color* color;

protected:

	virtual void postCreate()
	{
		Environment::postCreate();
	}
};