#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Curve.h"

/**
* This class represents the abstract concept Arc3D. (http://rdf.bg/Arc3D)
*/
class Arc3D : public Curve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Arc3D()
	{
	}

	virtual ~Arc3D()
	{
	}

	static const char* getClassName()
	{
		return "Arc3D";
	}

	/**
	* This datatype property is named hasNormals. (Cardinality: [0, 1])
	*/
	bool getHasNormals()
	{
		auto property = Thing::getDataProperty<bool>("hasNormals");
		if (property.second == 1) { return property.first[0]; }

		return (bool)0;
	}

	/**
	* This datatype property is named hasNormals. (Cardinality: [0, 1])
	*/
	void setHasNormals(bool value)
	{
		Thing::setDataProperty("hasNormals", &value, 1);
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
	* This datatype property is named hasNormals. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getHasNormals, put = setHasNormals)) bool hasNormals;

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