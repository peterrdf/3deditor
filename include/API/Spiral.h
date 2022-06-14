#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Curve.h"

/**
* This class represents the concept Spiral. (http://rdf.bg/Spiral)
*/
class Spiral : public Curve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Spiral()
	{
	}

	virtual ~Spiral()
	{
	}

	static const char* getClassName()
	{
		return "Spiral";
	}

	/**
	* This datatype property is named height. (Cardinality: [1, 1])
	*/
	double getHeight()
	{
		auto property = Thing::getDataProperty<double>("height");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named height. (Cardinality: [1, 1])
	*/
	void setHeight(double value)
	{
		Thing::setDataProperty("height", &value, 1);
	}

	/**
	* This datatype property is named offsetZ. (Cardinality: [1, 1])
	*/
	double getOffsetZ()
	{
		auto property = Thing::getDataProperty<double>("offsetZ");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named offsetZ. (Cardinality: [1, 1])
	*/
	void setOffsetZ(double value)
	{
		Thing::setDataProperty("offsetZ", &value, 1);
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
	* This datatype property is named size. (Cardinality: [0, 1])
	*/
	double getSize()
	{
		auto property = Thing::getDataProperty<double>("size");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named size. (Cardinality: [0, 1])
	*/
	void setSize(double value)
	{
		Thing::setDataProperty("size", &value, 1);
	}

	/**
	* This datatype property is named start. (Cardinality: [0, 1])
	*/
	double getStart()
	{
		auto property = Thing::getDataProperty<double>("start");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named start. (Cardinality: [0, 1])
	*/
	void setStart(double value)
	{
		Thing::setDataProperty("start", &value, 1);
	}

	/**
	* This datatype property is named height. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getHeight, put = setHeight)) double height;

	/**
	* This datatype property is named offsetZ. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getOffsetZ, put = setOffsetZ)) double offsetZ;

	/**
	* This datatype property is named radius. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getRadius, put = setRadius)) double radius;

	/**
	* This datatype property is named segmentationParts. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getSegmentationParts, put = setSegmentationParts)) int64_t segmentationParts;

	/**
	* This datatype property is named size. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getSize, put = setSize)) double size;

	/**
	* This datatype property is named start. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getStart, put = setStart)) double start;

protected:

	virtual void postCreate()
	{
		Curve::postCreate();
	}
};