#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Curve.h"

/**
* This class represents the concept Polygon2D. (http://rdf.bg/Polygon2D)
*/
class Polygon2D : public Curve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Polygon2D()
	{
	}

	virtual ~Polygon2D()
	{
	}

	static const char* getClassName()
	{
		return "Polygon2D";
	}

	/**
	* This object property represents the relation lineParts. (Cardinality: [1, Inf])
	*/
	vector<Curve*> getLineParts()
	{
		auto property = Thing::getThingProperty<Curve>("lineParts");
		return property;
	}

	/**
	* This object property represents the relation lineParts. (Cardinality: [1, Inf])
	*/
	void setLineParts(const vector<Curve*>& vecValues)
	{
		Thing::setThingProperty("lineParts", (const _Thing**)vecValues.data(), vecValues.size());
	}

	/**
	* This object property represents the relation lineParts. (Cardinality: [1, Inf])
	*/
	__declspec(property(get = getLineParts, put = setLineParts)) vector<Curve*> lineParts;

protected:

	virtual void postCreate()
	{
		Curve::postCreate();
	}
};