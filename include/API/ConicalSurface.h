#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "InfiniteSurface.h"

/**
* This class represents the concept ConicalSurface. (http://rdf.bg/ConicalSurface)
*/
class ConicalSurface : public InfiniteSurface
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	ConicalSurface()
	{
	}

	virtual ~ConicalSurface()
	{
	}

	static const char* getClassName()
	{
		return "ConicalSurface";
	}

	/**
	* This datatype property is named height. (Cardinality: [0, 1])
	*/
	double getHeight()
	{
		auto property = Thing::getDataProperty<double>("height");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named height. (Cardinality: [0, 1])
	*/
	void setHeight(double value)
	{
		Thing::setDataProperty("height", &value, 1);
	}

	/**
	* This datatype property is named radius. (Cardinality: [0, 1])
	*/
	double getRadius()
	{
		auto property = Thing::getDataProperty<double>("radius");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named radius. (Cardinality: [0, 1])
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
	* This datatype property is named semiVerticalAngle. (Cardinality: [1, 1])
	*/
	double getSemiVerticalAngle()
	{
		auto property = Thing::getDataProperty<double>("semiVerticalAngle");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named semiVerticalAngle. (Cardinality: [1, 1])
	*/
	void setSemiVerticalAngle(double value)
	{
		Thing::setDataProperty("semiVerticalAngle", &value, 1);
	}

	/**
	* This datatype property is named height. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getHeight, put = setHeight)) double height;

	/**
	* This datatype property is named radius. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getRadius, put = setRadius)) double radius;

	/**
	* This datatype property is named segmentationParts. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getSegmentationParts, put = setSegmentationParts)) int64_t segmentationParts;

	/**
	* This datatype property is named semiVerticalAngle. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getSemiVerticalAngle, put = setSemiVerticalAngle)) double semiVerticalAngle;

protected:

	virtual void postCreate()
	{
		InfiniteSurface::postCreate();
	}
};