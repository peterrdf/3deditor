#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "DirectLight.h"
#include "Vector.h"

/**
* This class represents the abstract concept DirectionalLight. (http://rdf.bg/DirectionalLight)
*/
class DirectionalLight : public DirectLight
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	DirectionalLight()
	{
	}

	virtual ~DirectionalLight()
	{
	}

	static const char* getClassName()
	{
		return "DirectionalLight";
	}

	/**
	* This object property represents the relation direction. (Cardinality: [0, 1])
	*/
	Vector* getDirection()
	{
		auto property = Thing::getThingProperty<Vector>("direction");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation direction. (Cardinality: [0, 1])
	*/
	void setDirection(Vector* pValue)
	{
		Thing::setThingProperty("direction", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation direction. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getDirection, put = setDirection)) Vector* direction;

protected:

	virtual void postCreate()
	{
		DirectLight::postCreate();
	}
};