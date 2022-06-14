#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Curve.h"

/**
* This class represents the concept InvertedCurve. (http://rdf.bg/InvertedCurve)
*/
class InvertedCurve : public Curve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	InvertedCurve()
	{
	}

	virtual ~InvertedCurve()
	{
	}

	static const char* getClassName()
	{
		return "InvertedCurve";
	}

	/**
	* This object property represents the relation curve. (Cardinality: [1, 1])
	*/
	Curve* getCurve()
	{
		auto property = Thing::getThingProperty<Curve>("curve");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation curve. (Cardinality: [1, 1])
	*/
	void setCurve(Curve* pValue)
	{
		Thing::setThingProperty("curve", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation curve. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getCurve, put = setCurve)) Curve* curve;

protected:

	virtual void postCreate()
	{
		Curve::postCreate();
	}
};