#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Face.h"
#include "Point3DSet.h"

/**
* This class represents the concept Mesh. (http://rdf.bg/Mesh)
*/
class Mesh : public Face
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Mesh()
	{
	}

	virtual ~Mesh()
	{
	}

	static const char* getClassName()
	{
		return "Mesh";
	}

	/**
	* This object property represents the relation pointSet. (Cardinality: [1, 1])
	*/
	Point3DSet* getPointSet()
	{
		auto property = Thing::getThingProperty<Point3DSet>("pointSet");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation pointSet. (Cardinality: [1, 1])
	*/
	void setPointSet(Point3DSet* pValue)
	{
		Thing::setThingProperty("pointSet", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation pointSet. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getPointSet, put = setPointSet)) Point3DSet* pointSet;

protected:

	virtual void postCreate()
	{
		Face::postCreate();
	}
};