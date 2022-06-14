#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Light.h"

/**
* This class represents the abstract concept AmbientLight. (http://rdf.bg/AmbientLight)
*/
class AmbientLight : public Light
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	AmbientLight()
	{
	}

	virtual ~AmbientLight()
	{
	}

	static const char* getClassName()
	{
		return "AmbientLight";
	}

protected:

	virtual void postCreate()
	{
		Light::postCreate();
	}
};