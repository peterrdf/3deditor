#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "FiniteSurface.h"

/**
* This class represents the concept SphericalSurface. (http://rdf.bg/SphericalSurface)
*/
class SphericalSurface : public FiniteSurface
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	SphericalSurface()
	{
	}

	virtual ~SphericalSurface()
	{
	}

	static const char* getClassName()
	{
		return "SphericalSurface";
	}

	/**
	* This datatype property is named radius. (Cardinality: [1, 1])
	*/
	double getRadius()
	{
		auto property = Thing::getDataProperty<double>("radius");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named radius. (Cardinality: [1, 1])
	*/
	void setRadius(double value)
	{
		Thing::setDataProperty("radius", &value, 1);
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
	* This datatype property is named radius. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getRadius, put = setRadius)) double radius;

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