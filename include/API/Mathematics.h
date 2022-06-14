#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Thing.h"

/**
* This class represents the abstract concept Mathematics. (http://rdf.bg/Mathematics)
*/
class Mathematics : public Thing
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Mathematics()
	{
	}

	virtual ~Mathematics()
	{
	}

	static const char* getClassName()
	{
		return "Mathematics";
	}

protected:

	virtual void postCreate()
	{
		Thing::postCreate();
	}
};