#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "FiniteSurface.h"
#include "Vector.h"
#include "Curve.h"

/**
* This class represents the concept SurfaceOfRevolution. (http://rdf.bg/SurfaceOfLinearExtrusion)
*/
class SurfaceOfLinearExtrusion : public FiniteSurface
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	SurfaceOfLinearExtrusion()
	{
	}

	virtual ~SurfaceOfLinearExtrusion()
	{
	}

	static const char* getClassName()
	{
		return "SurfaceOfLinearExtrusion";
	}

	/**
	* This object property represents the relation extrusionArea. (Cardinality: [1, 1])
	*/
	Vector* getExtrusion()
	{
		auto property = Thing::getThingProperty<Vector>("extrusion");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation extrusionArea. (Cardinality: [1, 1])
	*/
	void setExtrusion(Vector* pValue)
	{
		Thing::setThingProperty("extrusion", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation polygons. (Cardinality: [1, 1])
	*/
	Curve* getPolygon()
	{
		auto property = Thing::getThingProperty<Curve>("polygon");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation polygons. (Cardinality: [1, 1])
	*/
	void setPolygon(Curve* pValue)
	{
		Thing::setThingProperty("polygon", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation extrusionArea. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getExtrusion, put = setExtrusion)) Vector* extrusion;

	/**
	* This object property represents the relation polygons. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getPolygon, put = setPolygon)) Curve* polygon;

protected:

	virtual void postCreate()
	{
		FiniteSurface::postCreate();
	}
};