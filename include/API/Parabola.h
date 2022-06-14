#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "ConicalCurve.h"

/**
* This class represents the concept Parabola. (http://rdf.bg/Parabola)
*/
class Parabola : public ConicalCurve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Parabola()
	{
	}

	virtual ~Parabola()
	{
	}

	static const char* getClassName()
	{
		return "Parabola";
	}

protected:

	virtual void postCreate()
	{
		ConicalCurve::postCreate();
	}
};