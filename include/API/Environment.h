#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Thing.h"

/**
* This class represents the concept Environment. (http://rdf.bg/Environment)
*/
class Environment : public Thing
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Environment()
	{
	}

	virtual ~Environment()
	{
	}

	static const char* getClassName()
	{
		return "Environment";
	}

protected:

	virtual void postCreate()
	{
		Thing::postCreate();
	}
};