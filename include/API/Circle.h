#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "ConicalCurve.h"

/**
* This class represents the concept Circle. (http://rdf.bg/Circle)
*/
class Circle : public ConicalCurve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Circle()
	{
	}

	virtual ~Circle()
	{
	}

	static const char* getClassName()
	{
		return "Circle";
	}

protected:

	virtual void postCreate()
	{
		ConicalCurve::postCreate();
	}
};