#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "TransitionalCurve.h"

/**
* This class represents the concept CosineCurve. (http://rdf.bg/CosineCurve)
*/
class CosineCurve : public TransitionalCurve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	CosineCurve()
	{
	}

	virtual ~CosineCurve()
	{
	}

	static const char* getClassName()
	{
		return "CosineCurve";
	}

protected:

	virtual void postCreate()
	{
		TransitionalCurve::postCreate();
	}
};