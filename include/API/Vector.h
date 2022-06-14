#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Mathematics.h"

/**
* This class represents the concept Vector. (http://rdf.bg/Vector)
*/
class Vector : public Mathematics
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Vector()
	{
	}

	virtual ~Vector()
	{
	}

	static const char* getClassName()
	{
		return "Vector";
	}

protected:

	virtual void postCreate()
	{
		Mathematics::postCreate();
	}
};