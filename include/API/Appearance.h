#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Thing.h"

/**
* This class represents the abstract concept Appearance. (http://rdf.bg/Appearance)
*/
class Appearance : public Thing
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Appearance()
	{
	}

	virtual ~Appearance()
	{
	}

	static const char* getClassName()
	{
		return "Appearance";
	}

protected:

	virtual void postCreate()
	{
		Thing::postCreate();
	}
};