#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "FiniteSurface.h"
#include "Curve.h"

/**
* This class represents the concept SurfaceOfRevolution. (http://rdf.bg/SurfaceOfRevolution)
*/
class SurfaceOfRevolution : public FiniteSurface
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	SurfaceOfRevolution()
	{
	}

	virtual ~SurfaceOfRevolution()
	{
	}

	static const char* getClassName()
	{
		return "SurfaceOfRevolution";
	}

	/**
	* This object property represents the relation path. (Cardinality: [1, 1])
	*/
	Curve* getPath()
	{
		auto property = Thing::getThingProperty<Curve>("path");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation path. (Cardinality: [1, 1])
	*/
	void setPath(Curve* pValue)
	{
		Thing::setThingProperty("path", (const _Thing**)&pValue, 1);
	}

	/**
	* This datatype property is named segmentationParts. (Cardinality: [0, 1])
	*/
	int64_t getSegmentationParts()
	{
		auto property = Thing::getDataProperty<int64_t>("segmentationParts");
		if (property.second == 1) { return property.first[0]; }

		return (int64_t)0;
	}

	/**
	* This datatype property is named segmentationParts. (Cardinality: [0, 1])
	*/
	void setSegmentationParts(int64_t value)
	{
		Thing::setDataProperty("segmentationParts", &value, 1);
	}

	/**
	* This object property represents the relation path. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getPath, put = setPath)) Curve* path;

	/**
	* This datatype property is named segmentationParts. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getSegmentationParts, put = setSegmentationParts)) int64_t segmentationParts;

protected:

	virtual void postCreate()
	{
		FiniteSurface::postCreate();
	}
};