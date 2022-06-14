#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "TransitionalCurve.h"

/**
* This class represents the concept ClothoidCurve. (http://rdf.bg/ClothoidCurve)
*/
class ClothoidCurve : public TransitionalCurve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	ClothoidCurve()
	{
	}

	virtual ~ClothoidCurve()
	{
	}

	static const char* getClassName()
	{
		return "ClothoidCurve";
	}

protected:

	virtual void postCreate()
	{
		TransitionalCurve::postCreate();
	}
};