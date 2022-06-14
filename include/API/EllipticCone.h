#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Solid.h"

/**
* This class represents the concept EllipticCone. (http://rdf.bg/EllipticCone)
*/
class EllipticCone : public Solid
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	EllipticCone()
	{
	}

	virtual ~EllipticCone()
	{
	}

	static const char* getClassName()
	{
		return "EllipticCone";
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
	* This datatype property is named radiusI. (Cardinality: [1, 1])
	*/
	double getRadiusI()
	{
		auto property = Thing::getDataProperty<double>("radiusI");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named radiusI. (Cardinality: [1, 1])
	*/
	void setRadiusI(double value)
	{
		Thing::setDataProperty("radiusI", &value, 1);
	}

	/**
	* This datatype property is named radiusII. (Cardinality: [1, 1])
	*/
	double getRadiusII()
	{
		auto property = Thing::getDataProperty<double>("radiusII");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named radiusII. (Cardinality: [1, 1])
	*/
	void setRadiusII(double value)
	{
		Thing::setDataProperty("radiusII", &value, 1);
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
	* This datatype property is named height. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getHeight, put = setHeight)) double height;

	/**
	* This datatype property is named radiusI. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getRadiusI, put = setRadiusI)) double radiusI;

	/**
	* This datatype property is named radiusII. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getRadiusII, put = setRadiusII)) double radiusII;

	/**
	* This datatype property is named segmentationParts. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getSegmentationParts, put = setSegmentationParts)) int64_t segmentationParts;

protected:

	virtual void postCreate()
	{
		Solid::postCreate();
	}
};