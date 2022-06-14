#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Curve.h"

/**
* This class represents the abstract concept ConicalCurve. (http://rdf.bg/ConicalCurve)
*/
class ConicalCurve : public Curve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	ConicalCurve()
	{
	}

	virtual ~ConicalCurve()
	{
	}

	static const char* getClassName()
	{
		return "ConicalCurve";
	}

	/**
	* This datatype property is named a. (Cardinality: [1, 1])
	*/
	double getA()
	{
		auto property = Thing::getDataProperty<double>("a");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named a. (Cardinality: [1, 1])
	*/
	void setA(double value)
	{
		Thing::setDataProperty("a", &value, 1);
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
	* This datatype property is named a. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getA, put = setA)) double a;

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