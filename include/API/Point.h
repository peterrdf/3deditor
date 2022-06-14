#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "GeometricItem.h"

/**
* This class represents the abstract concept Point. (http://rdf.bg/Point)
*/
class Point : public GeometricItem
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Point()
	{
	}

	virtual ~Point()
	{
	}

	static const char* getClassName()
	{
		return "Point";
	}

protected:

	virtual void postCreate()
	{
		GeometricItem::postCreate();
	}
};