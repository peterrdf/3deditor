#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "TransitionalCurve.h"

/**
* This class represents the concept SineCurve. (http://rdf.bg/SineCurve)
*/
class SineCurve : public TransitionalCurve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	SineCurve()
	{
	}

	virtual ~SineCurve()
	{
	}

	static const char* getClassName()
	{
		return "SineCurve";
	}

protected:

	virtual void postCreate()
	{
		TransitionalCurve::postCreate();
	}
};