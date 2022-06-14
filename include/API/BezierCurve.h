#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "SplineCurve.h"

/**
* This class represents the concept BezierCurve. (http://rdf.bg/BezierCurve)
*/
class BezierCurve : public SplineCurve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	BezierCurve()
	{
	}

	virtual ~BezierCurve()
	{
	}

	static const char* getClassName()
	{
		return "BezierCurve";
	}

protected:

	virtual void postCreate()
	{
		SplineCurve::postCreate();
	}
};