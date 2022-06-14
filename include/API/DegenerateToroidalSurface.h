#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "InfiniteSurface.h"

/**
* This class represents the concept Cylinder. (http://rdf.bg/DegenerateToroidalSurface)
*/
class DegenerateToroidalSurface : public InfiniteSurface
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	DegenerateToroidalSurface()
	{
	}

	virtual ~DegenerateToroidalSurface()
	{
	}

	static const char* getClassName()
	{
		return "DegenerateToroidalSurface";
	}

	/**
	* This datatype property is named majorRadius. (Cardinality: [1, 1])
	*/
	double getMajorRadius()
	{
		auto property = Thing::getDataProperty<double>("majorRadius");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named majorRadius. (Cardinality: [1, 1])
	*/
	void setMajorRadius(double value)
	{
		Thing::setDataProperty("majorRadius", &value, 1);
	}

	/**
	* This datatype property is named minorRadius. (Cardinality: [1, 1])
	*/
	double getMinorRadius()
	{
		auto property = Thing::getDataProperty<double>("minorRadius");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named minorRadius. (Cardinality: [1, 1])
	*/
	void setMinorRadius(double value)
	{
		Thing::setDataProperty("minorRadius", &value, 1);
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
	* This datatype property is named majorRadius. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getMajorRadius, put = setMajorRadius)) double majorRadius;

	/**
	* This datatype property is named minorRadius. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getMinorRadius, put = setMinorRadius)) double minorRadius;

	/**
	* This datatype property is named segmentationParts. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getSegmentationParts, put = setSegmentationParts)) int64_t segmentationParts;

protected:

	virtual void postCreate()
	{
		InfiniteSurface::postCreate();
	}
};