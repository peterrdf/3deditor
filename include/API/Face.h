#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "GeometricItem.h"

/**
* This class represents the abstract concept Face. (http://rdf.bg/Face)
*/
class Face : public GeometricItem
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Face()
	{
	}

	virtual ~Face()
	{
	}

	static const char* getClassName()
	{
		return "Face";
	}

protected:

	virtual void postCreate()
	{
		GeometricItem::postCreate();
	}
};