#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Surface.h"

/**
* This class represents the concept InfiniteSurface. (http://rdf.bg/InfiniteSurface)
*/
class InfiniteSurface : public Surface
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	InfiniteSurface()
	{
	}

	virtual ~InfiniteSurface()
	{
	}

	static const char* getClassName()
	{
		return "InfiniteSurface";
	}

protected:

	virtual void postCreate()
	{
		Surface::postCreate();
	}
};