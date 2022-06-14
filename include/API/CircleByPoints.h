#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Circle.h"
#include "Point3D.h"

/**
* This class represents the concept CircleByPoints. (http://rdf.bg/CircleByPoints)
*/
class CircleByPoints : public Circle
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	CircleByPoints()
	{
	}

	virtual ~CircleByPoints()
	{
	}

	static const char* getClassName()
	{
		return "CircleByPoints";
	}

	/**
	* This object property represents the relation pointReferences. (Cardinality: [3, 3])
	*/
	vector<Point3D*> getPointReferences()
	{
		auto property = Thing::getThingProperty<Point3D>("pointReferences");
		return property;
	}

	/**
	* This object property represents the relation pointReferences. (Cardinality: [3, 3])
	*/
	void setPointReferences(const vector<Point3D*>& vecValues)
	{
		Thing::setThingProperty("pointReferences", (const _Thing**)vecValues.data(), vecValues.size());
	}

	/**
	* This object property represents the relation pointReferences. (Cardinality: [3, 3])
	*/
	__declspec(property(get = getPointReferences, put = setPointReferences)) vector<Point3D*> pointReferences;

protected:

	virtual void postCreate()
	{
		Circle::postCreate();
	}
};