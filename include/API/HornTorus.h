#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Torus.h"

/**
* This class represents the concept HornTorus. (http://rdf.bg/HornTorus)
*/
class HornTorus : public Torus
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	HornTorus()
	{
	}

	virtual ~HornTorus()
	{
	}

	static const char* getClassName()
	{
		return "HornTorus";
	}

protected:

	virtual void postCreate()
	{
		Torus::postCreate();
	}
};