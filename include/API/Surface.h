#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Face.h"

/**
* This class represents the concept Surface. (http://rdf.bg/Surface)
*/
class Surface : public Face
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Surface()
	{
	}

	virtual ~Surface()
	{
	}

	static const char* getClassName()
	{
		return "Surface";
	}

protected:

	virtual void postCreate()
	{
		Face::postCreate();
	}
};