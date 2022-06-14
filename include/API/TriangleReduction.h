#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Solid.h"
#include "GeometricItem.h"

/**
* This class represents the concept TriangleReduction. (http://rdf.bg/TriangleReduction)
*/
class TriangleReduction : public Solid
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	TriangleReduction()
	{
	}

	virtual ~TriangleReduction()
	{
	}

	static const char* getClassName()
	{
		return "TriangleReduction";
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
	* This object property represents the relation object. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getObject, put = setObject)) GeometricItem* object;

protected:

	virtual void postCreate()
	{
		Solid::postCreate();
	}
};