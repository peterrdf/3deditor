#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "TransitionalCurve.h"

/**
* This class represents the concept BiQuadraticParabola. (http://rdf.bg/BiQuadraticParabola)
*/
class BiQuadraticParabola : public TransitionalCurve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	BiQuadraticParabola()
	{
	}

	virtual ~BiQuadraticParabola()
	{
	}

	static const char* getClassName()
	{
		return "BiQuadraticParabola";
	}

protected:

	virtual void postCreate()
	{
		TransitionalCurve::postCreate();
	}
};