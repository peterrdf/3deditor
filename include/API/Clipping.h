#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Solid.h"
#include "GeometricItem.h"
#include "Plane.h"

/**
* This class represents the concept Clipping. (http://rdf.bg/Clipping)
*/
class Clipping : public Solid
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Clipping()
	{
	}

	virtual ~Clipping()
	{
	}

	static const char* getClassName()
	{
		return "Clipping";
	}

	/**
	* This object property represents the relation object. (Cardinality: [0, 1])
	*/
	GeometricItem* getObject()
	{
		auto property = Thing::getThingProperty<GeometricItem>("object");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation object. (Cardinality: [0, 1])
	*/
	void setObject(GeometricItem* pValue)
	{
		Thing::setThingProperty("object", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation plane. (Cardinality: [1, 1])
	*/
	Plane* getPlane()
	{
		auto property = Thing::getThingProperty<Plane>("plane");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation plane. (Cardinality: [1, 1])
	*/
	void setPlane(Plane* pValue)
	{
		Thing::setThingProperty("plane", (const _Thing**)&pValue, 1);
	}

	/**
	* This datatype property is named type. (Cardinality: [1, 1])
	*/
	int64_t getType()
	{
		auto property = Thing::getDataProperty<int64_t>("type");
		if (property.second == 1) { return property.first[0]; }

		return (int64_t)0;
	}

	/**
	* This datatype property is named type. (Cardinality: [1, 1])
	*/
	void setType(int64_t value)
	{
		Thing::setDataProperty("type", &value, 1);
	}

	/**
	* This object property represents the relation object. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getObject, put = setObject)) GeometricItem* object;

	/**
	* This object property represents the relation plane. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getPlane, put = setPlane)) Plane* plane;

	/**
	* This datatype property is named type. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getType, put = setType)) int64_t type;

protected:

	virtual void postCreate()
	{
		Solid::postCreate();
	}
};