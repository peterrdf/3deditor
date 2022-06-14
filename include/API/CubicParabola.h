#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "TransitionalCurve.h"

/**
* This class represents the concept CubicParabola. (http://rdf.bg/CubicParabola)
*/
class CubicParabola : public TransitionalCurve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	CubicParabola()
	{
	}

	virtual ~CubicParabola()
	{
	}

	static const char* getClassName()
	{
		return "CubicParabola";
	}

protected:

	virtual void postCreate()
	{
		TransitionalCurve::postCreate();
	}
};