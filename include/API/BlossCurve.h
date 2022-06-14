#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "TransitionalCurve.h"

/**
* This class represents the concept BlossCurve. (http://rdf.bg/BlossCurve)
*/
class BlossCurve : public TransitionalCurve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	BlossCurve()
	{
	}

	virtual ~BlossCurve()
	{
	}

	static const char* getClassName()
	{
		return "BlossCurve";
	}

protected:

	virtual void postCreate()
	{
		TransitionalCurve::postCreate();
	}
};