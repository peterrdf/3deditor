#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "InfiniteSurface.h"
#include "Plane.h"

/**
* This class represents the concept PlaneSurface. (http://rdf.bg/PlaneSurface)
*/
class PlaneSurface : public InfiniteSurface
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	PlaneSurface()
	{
	}

	virtual ~PlaneSurface()
	{
	}

	static const char* getClassName()
	{
		return "PlaneSurface";
	}

	/**
	* This object property represents the relation plane. (Cardinality: [0, 1])
	*/
	Plane* getPlane()
	{
		auto property = Thing::getThingProperty<Plane>("plane");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation plane. (Cardinality: [0, 1])
	*/
	void setPlane(Plane* pValue)
	{
		Thing::setThingProperty("plane", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation plane. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getPlane, put = setPlane)) Plane* plane;

protected:

	virtual void postCreate()
	{
		InfiniteSurface::postCreate();
	}
};