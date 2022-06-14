#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "GeometricItem.h"

/**
* This class represents the abstract concept Solid. (http://rdf.bg/Solid)
*/
class Solid : public GeometricItem
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Solid()
	{
	}

	virtual ~Solid()
	{
	}

	static const char* getClassName()
	{
		return "Solid";
	}

protected:

	virtual void postCreate()
	{
		GeometricItem::postCreate();
	}
};