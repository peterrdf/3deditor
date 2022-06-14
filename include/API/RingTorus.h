#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Torus.h"

/**
* This class represents the concept RingTorus. (http://rdf.bg/RingTorus)
*/
class RingTorus : public Torus
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	RingTorus()
	{
	}

	virtual ~RingTorus()
	{
	}

	static const char* getClassName()
	{
		return "RingTorus";
	}

protected:

	virtual void postCreate()
	{
		Torus::postCreate();
	}
};