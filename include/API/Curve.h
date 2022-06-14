#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "GeometricItem.h"

/**
* This class represents the abstract concept Curve. (http://rdf.bg/Curve)
*/
class Curve : public GeometricItem
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Curve()
	{
	}

	virtual ~Curve()
	{
	}

	static const char* getClassName()
	{
		return "Curve";
	}

protected:

	virtual void postCreate()
	{
		GeometricItem::postCreate();
	}
};