#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Surface.h"

/**
* This class represents the concept InvertedSurface. (http://rdf.bg/InvertedSurface)
*/
class InvertedSurface : public Surface
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	InvertedSurface()
	{
	}

	virtual ~InvertedSurface()
	{
	}

	static const char* getClassName()
	{
		return "InvertedSurface";
	}

	/**
	* This object property represents the relation surface. (Cardinality: [1, 1])
	*/
	Surface* getSurface()
	{
		auto property = Thing::getThingProperty<Surface>("surface");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation surface. (Cardinality: [1, 1])
	*/
	void setSurface(Surface* pValue)
	{
		Thing::setThingProperty("surface", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation surface. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getSurface, put = setSurface)) Surface* surface;

protected:

	virtual void postCreate()
	{
		Surface::postCreate();
	}
};