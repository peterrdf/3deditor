#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "DirectLight.h"

/**
* This class represents the abstract concept SpotLight. (http://rdf.bg/SpotLight)
*/
class SpotLight : public DirectLight
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	SpotLight()
	{
	}

	virtual ~SpotLight()
	{
	}

	static const char* getClassName()
	{
		return "SpotLight";
	}

protected:

	virtual void postCreate()
	{
		DirectLight::postCreate();
	}
};