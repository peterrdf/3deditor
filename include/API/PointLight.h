#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "DirectLight.h"
#include "Point3D.h"

/**
* This class represents the abstract concept PointLight. (http://rdf.bg/PointLight)
*/
class PointLight : public DirectLight
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	PointLight()
	{
	}

	virtual ~PointLight()
	{
	}

	static const char* getClassName()
	{
		return "PointLight";
	}

	/**
	* This object property represents the relation position. (Cardinality: [0, 1])
	*/
	Point3D* getPosition()
	{
		auto property = Thing::getThingProperty<Point3D>("position");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation position. (Cardinality: [0, 1])
	*/
	void setPosition(Point3D* pValue)
	{
		Thing::setThingProperty("position", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation position. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getPosition, put = setPosition)) Point3D* position;

protected:

	virtual void postCreate()
	{
		DirectLight::postCreate();
	}
};