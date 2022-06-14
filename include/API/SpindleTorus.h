#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Torus.h"

/**
* This class represents the concept SpindleTorus. (http://rdf.bg/SpindleTorus)
*/
class SpindleTorus : public Torus
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	SpindleTorus()
	{
	}

	virtual ~SpindleTorus()
	{
	}

	static const char* getClassName()
	{
		return "SpindleTorus";
	}

protected:

	virtual void postCreate()
	{
		Torus::postCreate();
	}
};