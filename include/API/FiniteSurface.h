#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Surface.h"

/**
* This class represents the concept FiniteSurface. (http://rdf.bg/FiniteSurface)
*/
class FiniteSurface : public Surface
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	FiniteSurface()
	{
	}

	virtual ~FiniteSurface()
	{
	}

	static const char* getClassName()
	{
		return "FiniteSurface";
	}

protected:

	virtual void postCreate()
	{
		Surface::postCreate();
	}
};